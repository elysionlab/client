
#include "ace/Proactor.h"
#include "ace/Malloc_T.h"
#include "ace/Message_Queue.h"
#include "mmsv_User.h"
#include "mmsv_Aio_Acceptor.h"


extern SERVER_RUNTIME_INFO RunServerInfo;

mmsv_User::mmsv_User(mmsv_Acceptor* acc/* = 0*/)
: m_pAcceptor(acc),    
  m_bDeferred_Close(0),
  m_bCloseSocket(0),
  m_bSession(0),
  m_bTimerCallMode(0),
  m_nPacketLast(0),
  m_nPacketCount(0),
  m_bFlag_Write(0),
  m_bFlag_Read(0),
  m_bFlag_Queue(0),
  m_pQueue_Writer(0),
  m_pQueue_Reader(0),  
  m_nFileNo(0),
  m_nFileSizeBegin(0),
  m_nFilePayMethod(0),
  m_bCleanProcess(0),
  m_pFtp(0),
  m_bDelayQuery(0),
  m_nMemberIdx(0),
  m_nLastVolumeUpload(0)
{
	ACE_OS::strcpy(m_pUserPoint, "mmsv");
	ACE_OS::memset(m_pUserid,	0, sizeof(m_pUserid));
	ACE_OS::memset(m_pUserInfo, 0, sizeof(m_pUserInfo));
	ACE_OS::memset(m_pUseridFile, 0, sizeof(m_pUseridFile));
}

mmsv_User::~mmsv_User(void)
{	
	m_pAcceptor->connect_manager(2, this);	

	cleanprocess();

	if(m_bTimerCallMode != 0)
		m_pAcceptor->m_pProactor->cancel_timer(*this);

	SAFE_DELETE(m_pFtp);
	SAFE_DELETE(m_pQueue_Writer);
	SAFE_DELETE(m_pQueue_Reader);
}


void mmsv_User::cleanprocess()
{

	ACE_GUARD(ACE_Thread_Mutex, c_flag_cleanpro, m_pLock_CleanPro_);

	if(m_bCleanProcess == 0)
	{
		// 한번만 실행하도록 설정
		m_bCleanProcess = 1;

		// 정액제회원에서 제거
		if(RunServerInfo.nServerRunMode == MMSV_SERVER_MODE_DOWN)
			m_pAcceptor->user_remove(this);	

		m_bCloseSocket = 1;

		// 등록된 비동기 작업 모두 취소
		m_pReader.cancel();
		m_pWriter.cancel();

		// 큐에 남아 있는 모든 전송 데이터 제거
		m_pQueue_Writer->flush();
		m_pQueue_Reader->flush();

		// 연결된 FTP 세션 종료
		m_pFtp->DisConnect();	
		
		// 소켓 핸들 종료
		if(handle() != ACE_INVALID_HANDLE)
		{
			ACE_OS::closesocket(handle());			
			handle(ACE_INVALID_HANDLE);
		}	
	}
}


__int64 mmsv_User::trans_result()
{
	__int64 nTransSize = 0;
	int nCheckCount = 0;
		
	nTransSize = m_pFtp->DownLoadSize();

	while(nTransSize == -2 || nTransSize == -3)
	{		
		if(nTransSize == -3)
			nCheckCount++;

		if(nCheckCount >= 3)
		{
			nTransSize = 0;
			m_pFtp->DisConnect();
			break;
		}

		nTransSize = m_pFtp->DownLoadSize();
		continue;
	}

	return nTransSize;
}

void mmsv_User::force_close(const mmsv_User* pUserPtr)
{
	char pSendPtrB[1024];	

	// 정상적인 연결일 때 전송
	if(((ACE_OS::gettimeofday() - m_tAccessTime).sec()) < CONNECT_LIMIT_TIME)
	{
		ZeroMemory(pSendPtrB, sizeof(pSendPtrB));
		sprintf(pSendPtrB, "%s::%d", pUserPtr->m_pAddr_Remote.get_host_addr(), (int)pUserPtr->m_pAddr_Remote.get_port_number());
		send_packet_queue(CHK_DUPLI_CLOSE, (char*)pSendPtrB, (int)strlen(pSendPtrB) + 1);
		m_pFtp->SendCmd("ABOR");
		m_pFtp->DisConnect();
	}
}


char* mmsv_User::getuserinfo()
{
	sprintf(m_pUserInfo, "{MEMBERIDX=%I64d}:{USERID=%s}:{IP=%s::%d}:{TIME=%d}",
			m_nMemberIdx, m_pUserid, m_pAddr_Remote.get_host_addr(), (int)m_pAddr_Remote.get_port_number(), connect_time());

	return m_pUserInfo;
}

void mmsv_User::open(ACE_HANDLE new_handle, ACE_Message_Block &message_block)
{	
	this->handle(new_handle);

	ACE_NEW_NORETURN(m_pQueue_Writer, QUEUE());
	m_pQueue_Writer->high_water_mark((size_t)RunServerInfo.nMaxSockBuffer * 100);

	ACE_NEW_NORETURN(m_pQueue_Reader, QUEUE());
	m_pQueue_Reader->high_water_mark((size_t)RunServerInfo.nMaxSockBuffer * 100);
	

	// 연결시간
	m_tConnectTime	= ACE_OS::gettimeofday();
	m_tAccessTime	= ACE_OS::gettimeofday();


	if( m_pReader.open(*this, new_handle, 0, m_pAcceptor->m_pProactor) != 0 ||
		m_pWriter.open(*this, new_handle, 0, m_pAcceptor->m_pProactor) != 0)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::open()━m_pReader.open() or m_pWriter.open() Faild\n"))); 

		delete this;
		return;
	}


	/* 헤더사이즈 만큼 읽기 요청 */	
	ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
	if(recv_packet(m_pMB_Read, MMSV_PACKET_HEAD_SIZE, 1) == -1)
		return;

	m_pFtp = new mmsv_FtpClient();
}


int mmsv_User::LogIn(char* pUserid, __int64 nUserIdx)
{
	m_bSession		= 1;	
	ACE_OS::strcpy(m_pUserid, pUserid);
	m_nMemberIdx	= nUserIdx;

	if(RunServerInfo.nServerRunMode == MMSV_SERVER_MODE_DOWN)
		m_pAcceptor->user_insert(this);
	return 1;
}

mmsv_User* mmsv_User::user_check(ACE_UINT64 nMemberIdx)
{
	return m_pAcceptor->user_check(nMemberIdx);
}


void mmsv_User::update_packet(ACE_UINT32 nCode)
{
	m_nPacketLast	= nCode;
	m_nPacketCount++;
}


/*
모든 작업이 종료될때 호출되는 함수 

1. bCloseMode : 1

	클라이언트와 소켓이 종료되어 handle_read_stream() 에서 호출됨
	해당 소켓관련으로 큐에 적재되었거나, Send 관련 명령이 초기화 되었을때에는 바로 종료되지 않고 
	모든 처리가 완료될때가지 대기

2. bCloseMode : 2

	세션종료(mmsv_Aio_Acceptor의 타이머에서 호출) 되거나, recv 실패, 잘못된 프로토콜로 강제종료 시킬때 사용
	소켓 연결종료 요청과 소켓 연결종료 요청후 일정시간(10초) 동안 자동 종료하지 못할때 강제종료를 위한 타이머 초기화

3. bCloseMode : 3

	큐에서 작업을 마친후 큐의 적재된 카운터를 감소시키고 그 사이에 종료 설정 되어 있는지 체크
	종료설정된 상태에서 큐 작업이 모두 완료해도 send 작업이 남아 있으면 종료되지 않음

4. bCloseMode : 4

	쓰기 작업을 마친후 쓰기 카운터를 감소시키고 그 사이에 종료 설정 되어 있는지 체크
	종료설정된 상태에서 쓰기 작업이 모두 완료해도 큐 작업이 남아 있으면 종료되지 않음

*/
int	mmsv_User::closeclient(int bCloseMode)
{
	m_pLock_CloseCh_.acquire();

	if(strcmp(m_pUserPoint, "mmsv"))
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋closeclient()━UserPoint Error\n")));
		m_pLock_CloseCh_.release();
		return 0;
	}

	int close_now = 0;

	try
	{			
		// Recv Result == 0
		if(bCloseMode == 1)
		{
			cleanprocess();

			m_bDeferred_Close = 1;

			if(flag_set(FLAG_EMPTY_, 0) == 1)
				close_now = 1;
		}

		// session close
		else if(bCloseMode == 2)
		{
			// 소켓을 닫고 일정 시간동안 종료되지 않으면 강제 종료 
			if(m_bTimerCallMode != 1)
			{
				m_pAcceptor->m_pProactor->cancel_timer(*this);
				m_bTimerCallMode = 0;

				ACE_Time_Value tCheck(CONNECT_LIMIT_TIME);
				if(m_pAcceptor->m_pProactor->schedule_timer(*this, 0, tCheck) != -1)
					m_bTimerCallMode = 1;
			}	

			cleanprocess();
			m_pLock_CloseCh_.release();
			return 0;
		}

		// queue work complete
		else if(bCloseMode == 3)
		{			
			if(putqueue(FLAG_QUEUE_) == 1)		
			{
				m_pLock_CloseCh_.release();
				return 0;
			}
			else
				close_now = m_bDeferred_Close;
			
		}

		// send complete
		else if(bCloseMode == 4)
		{
			if(flag_set(FLAG_WRITE_, 0) == 1)
			{
				close_now = m_bDeferred_Close;
			}
			else
			{
				// 큐에 전송할 부분이 있는지 체크후 전송
				m_pLock_CloseCh_.release();
				send_packet();
				return 0;
			}
		}


		if(close_now == 1)
		{		
			m_pAcceptor->m_pProactor->cancel_timer(*this);

			m_pLock_CloseCh_.release();
			delete this;
			return -1;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋closeclient()━Mode(%d)\n"), bCloseMode)); 
	}

	m_pLock_CloseCh_.release();
	return 0;
}


int  mmsv_User::putqueue(int nMode, ACE_Message_Block* pNewMessage/* = 0*/)
{
	ACE_GUARD_RETURN(ACE_Thread_Mutex, c_flag_pushqueue, m_pLock_PushQueue_, 1);

	switch(nMode)
	{
		case FLAG_QUEUE_:
		{
			if(m_pQueue_Reader->message_count() == 0)
			{
				m_bFlag_Queue = 0;
				return 0;				
			}
			else
			{
				ACE_Message_Block* pReadQueuePrt = 0;
				if(m_pQueue_Reader->message_count() != 0 && m_pQueue_Reader->dequeue_head(pReadQueuePrt) != -1 && pReadQueuePrt != 0)
				{
					if(m_pAcceptor->m_pQueue->put(pReadQueuePrt) != -1)
					{
						m_bFlag_Queue = 1;
						return 1;
					}
					else
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋putqueue(3)━m_pQueue->put() Error : Count(%d) : %s\n"), m_pQueue_Reader->message_count(), getuserinfo())); 
					}
				}
				else
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋putqueue(3)━dequeue_head() Error : No Empty Queue : Count(%d) : %s\n"), m_pQueue_Reader->message_count(), getuserinfo())); 
				}

				m_bFlag_Queue = 0;
				return 0;
			}
			break;
		}
		case FLAG_QUEUE_C_:
		{
			if(m_bFlag_Queue == 0)
			{
				if(m_pAcceptor->m_pQueue->put(pNewMessage) != -1)
					m_bFlag_Queue = 1;
				else
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::putqueue()━put() = -1 : FLAG_QUEUE_C_ : %s\n"), getuserinfo())); 
					m_bFlag_Queue = 0;
					pNewMessage->release();
				}
			}
			else
			{
				if(m_pQueue_Reader->enqueue_tail(pNewMessage) == -1)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::putqueue()━enqueue_tail() = -1 : %s\n"), getuserinfo())); 
					pNewMessage->release();
				}
			}
		}
	}

	return 1;
}

int mmsv_User::flag_set(int nMode, int nValue)
{
	ACE_GUARD_RETURN(ACE_Thread_Mutex, c_flag_set, m_pLock_Flag_Set_, 0);

	switch(nMode)
	{
		case FLAG_WRITE_:
		{
			m_bFlag_Write = nValue;
			break;
		}
		case FLAG_READ_:
		{
			m_bFlag_Read = nValue;
			break;
		}		
		case FLAG_WRITE_C_:
		{
			return m_bFlag_Write;
		}		
	}

	if(m_bFlag_Queue == 0 && m_bFlag_Write == 0 && m_bFlag_Read == 0 && m_pQueue_Writer->is_empty() && m_pQueue_Reader->is_empty())
		return 1;
	else
		return 0;
}

long mmsv_User::connect_time()
{
	return (ACE_OS::gettimeofday() - m_tConnectTime).sec();
}


void mmsv_User::addresses(const ACE_INET_Addr &remote_address, const ACE_INET_Addr &local_address)
{
	/*
		ACE_Asynch_Acceptor::open()의 pass_addresses 옵션을 0이 아닌값을 설정시 훅메소드 호출됨
	*/
	m_pAddr_Local	= local_address;
	m_pAddr_Remote	= remote_address;	
}




/* PING & PONG */
int mmsv_User::connect_check(bool bCheckMode)
{
	ACE_GUARD_RETURN(ACE_Thread_Mutex, c_flag_access, m_pLock_Access_, -1);

	ACE_Time_Value pCurTime = ACE_OS::gettimeofday();
	long nPassTime = pCurTime.sec() - m_tAccessTime.sec();
	long nConnectTime = pCurTime.sec() - m_tConnectTime.sec();

	if(bCheckMode)
	{
		if((nPassTime > CONNECT_LIMIT_TIME || (m_bSession == 0 && nConnectTime > MMSV_CHECK_USER_AUTH)) && !m_bDelayQuery)
		{
			try
			{
				closeclient(2);
				return 1;
			}
			catch(...)
			{
				return -2;
			}
		}
	}
	else
	{
		m_tAccessTime = ACE_OS::gettimeofday();
	}

	return 0;
}


void mmsv_User::handle_time_out(const ACE_Time_Value &tv, const void *arg)
{
	try
	{
		if(m_bTimerCallMode == 1)
		{
			m_pAcceptor->m_pProactor->cancel_timer(*this);
			m_bTimerCallMode = 0;

			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::handle_time_out()━Mode 1 : %s\n"), getuserinfo()));
			delete this;
		}		
		
		m_pAcceptor->m_pProactor->cancel_timer(*this);
		m_bTimerCallMode = 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::handle_time_out()━m_bTimerCallMode(%d) Catch(...)\n"), (int)m_bTimerCallMode)); 
	}
}




void mmsv_User::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
{
	/*
		result.success()			: 비동기 명령의 성공여부
		result.bytes_to_read()		: 비동기 명령에서 요청된 바이트 수
		result.bytes_transferred()	: 비동기 명려에서 실제 전송된 바이트 수
		message_block()				: 명령에서 사용된 ACE_Message_Block의 참조 요청
		m_pMB_Read->length()		: 할당받은 버퍼의 전송받을 총 데이터 사이즈
	*/

	// 읽기작업 완료
	flag_set(FLAG_READ_, 0);

	try
	{

		/* 읽기를 실패했거나 */
		if(!result.success() || result.bytes_transferred() == 0)
		{
			/* 모든 메시지블럭을 제거 */
			m_pMB_Read->release();
			closeclient(1);
			return;
		}	

		/* 패킷의 헤더 또는 바디, 꼬리 부분 수신이 완료되지 않았으므로 남은 크기만큼 다시 요청 */
		else if(result.bytes_transferred() < result.bytes_to_read())
		{
			recv_packet(&result.message_block(), result.bytes_to_read() - result.bytes_transferred(), 2);
			return;
		}

		/* 헤더파일이 도착을 체크후 나머지 부분을 전송 요청 */
		else if(&result.message_block() == m_pMB_Read && result.message_block().length() == MMSV_PACKET_HEAD_SIZE)
		{
			PHEAD pHead = (PHEAD)m_pMB_Read->rd_ptr();		

			/* 패킷의 바디 부분이 없을 경우 */
			if(pHead->ps == m_pMB_Read->length())
			{
				push_queue();			

				/* 헤더사이즈 만큼 읽기 요청 */	
				ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
				if(recv_packet(m_pMB_Read, MMSV_PACKET_HEAD_SIZE, 3) == -1)
					return;
			}
			else
			{
				if(pHead->ps <= RunServerInfo.nMaxSockBuffer)
				{
					// 데이터 사이즈가 남은 데이터블럭 크기보다 작을경우 사이즈 조절
					if(recv_packet(m_pMB_Read, (size_t)pHead->ps - m_pMB_Read->length(), 4) == -1)
						return;
				}
				else
				{
					// 데이터 사이즈가 남은 데이터블럭 크기보다 클경우 사이즈 조절
					if(recv_packet(m_pMB_Read, (size_t)RunServerInfo.nMaxSockBuffer, 5) == -1)
						return;
				}			
			}		
		}	
		
		else 
		{
			PHEAD pHead = (PHEAD)m_pMB_Read->rd_ptr();

			if(pHead->ps > m_pMB_Read->total_length())
			{
				ACE_Message_Block* pMB_Block;
				ACE_NEW_NORETURN(pMB_Block, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));

				m_pMB_Read->cont(pMB_Block);

				/* 메시지블럭 하나 추가로 모든 패킷 수신이 가능할때 */
				if(pHead->ps <= (unsigned int)m_pMB_Read->total_size())
				{
					if(recv_packet(pMB_Block, (size_t)pHead->ps - m_pMB_Read->total_length(), 6) == -1)
						return;
				}
				else
				{
					if(recv_packet(pMB_Block, (size_t)RunServerInfo.nMaxSockBuffer, 7) == -1)
						return;
				}

				return;
			}

			/* ping 프로토콜은 작업큐에 넣지 않음 */
			if(pHead->code == USER_PTR_UPDATE)
			{
				m_pMB_Read->release();
				m_pMB_Read = 0;				
			}
			else
			{
				push_queue();
			}

			// 마지막 전송 받은 시각 설정
			connect_check(false);

			/* 헤더사이즈 만큼 읽기 요청 */	
			ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
			if(recv_packet(m_pMB_Read, MMSV_PACKET_HEAD_SIZE, 8) == -1)
				return;
		}	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋handle_read_stream()━m_bFlag_Read(%d)\n"), m_bFlag_Read)); 
	}
}


int mmsv_User::recv_packet(ACE_Message_Block* pMB_Block, size_t nSize, int nMode)
{
	if(strcmp(m_pUserPoint, "mmsv"))
		return 0;

	// 종료 처리중이면 더이상 전송하지 않음
	if(m_bDeferred_Close == 1 || m_bCloseSocket == 1)
	{
		m_pMB_Read->release();			
		return 0;
	}

	try
	{
		if(m_pReader.read(*pMB_Block, nSize) == -1)
		{
			/* 요청한 메시지블럭을 해제하지 않고 최상위 블럭을 해제해서 전체에 적용 */
			//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋recv_packet()━read() == -1 ━UserSign(%s) :: Call [%d]\n"), m_pUserSign, nMode)); 
			m_pMB_Read->release();			
			closeclient(1);
			return -1;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋recv_packet()━m_bFlag_Read(%d)\n"), m_bFlag_Read)); 
	}

	// 읽기작업 등록
	flag_set(FLAG_READ_, 1);

	return 0;
}



int mmsv_User::push_queue()
{
	if(strcmp(m_pUserPoint, "mmsv"))
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::push_queue()━m_nUserRefuCall != 100\n"))); 
		return 0;
	}

	try
	{
		// 종료 처리중이면 더이상 전송하지 않음
		if(m_bDeferred_Close == 1 || m_bCloseSocket == 1)
		{
			m_pMB_Read->release();
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::push_queue()━m_bDeferred_Close = 1 || m_bCloseSocket = 1\n"))); 
			return 0;
		}
		
		ACE_Message_Block* pClientPoint;
		ACE_NEW_RETURN(pClientPoint, ACE_Message_Block(ACE_reinterpret_cast(char*, this)), -1);

		pClientPoint->cont(m_pMB_Read);
		m_pMB_Read = 0;

		putqueue(FLAG_QUEUE_C_, pClientPoint);					
		return 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::push_queue()━m_bFlag_Queue(%d)\n"), m_bFlag_Queue)); 
	}

	return 0;
}

void mmsv_User::handle_write_stream(const ACE_Asynch_Write_Stream::Result & result)
{
	if(strcmp(m_pUserPoint, "mmsv"))
		return;

	try
	{
		if(!result.success())
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋handle_write_stream()━!result.success() SIZE(%d)\n"), result.message_block().length())); 

			result.message_block().release();
			closeclient(4);
			return;
		}
		else
		{
			ACE_Message_Block* pWritePtr = &result.message_block();

			// 모두 전송되었을때
			if(result.message_block().length() == 0 || m_bCloseSocket == 1)
			{
				result.message_block().release();
				closeclient(4);
				return;
			}

			// 전송할 부분이 남았을때
			else
			{
				if(m_pWriter.write(result.message_block(), result.message_block().length()) == -1)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋handle_write_stream()━SIZE(%d)\n"), result.message_block().length())); 
					
					result.message_block().release();
					closeclient(4);
					return;
				}
				else
				{
					// 송신 등록상태 유지
					return;
				}
			}		
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋handle_write_stream()━m_bFlag_Write(%d)\n"), m_bFlag_Write)); 
	}
	return;	
}

/***********************************************************************************************************
// 리스트를 이용해 패킷 전송
***********************************************************************************************************/
int mmsv_User::send_packet_queue(int nCode, char* pData/* = 0*/, size_t nSize/* = 0*/)
{
	size_t dwCopySize		= 0;

	ACE_Message_Block* pWritePtr;

	if(m_bCloseSocket == 1)
		return 0;

	TAIL pTail;
	HEAD pHead;

	if(nSize == 1) nSize = 0;

	pHead.code	= nCode;
	pHead.ds	= nSize;
	pHead.ps	= MMSV_PACKET_HEAD_SIZE + nSize + MMSV_PACKET_FOOT_SIZE;
	ACE_OS::strcpy(pTail.key, szEndTitle);


	try
	{
		if(pHead.ps < RunServerInfo.nMaxSockBuffer)
		{
			ACE_NEW_RETURN(pWritePtr, ACE_Message_Block(pHead.ps, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator), -1);
			
			/* Header Copy */
			pWritePtr->copy((char*)&pHead, MMSV_PACKET_HEAD_SIZE);

			/* Data Copy */
			if(nSize > 0) 
				pWritePtr->copy((char*)pData,  nSize);

			/* Foot Copy */
			pWritePtr->copy((char*)&pTail, MMSV_PACKET_FOOT_SIZE);
			
			if(flag_set(FLAG_WRITE_C_, 0) == 0)
			{
				send_packet(pWritePtr);
			}
			else
			{
				m_pQueue_Writer->enqueue_tail(pWritePtr);
			}
			return 0;
		}
		else
		{

			ACE_NEW_RETURN(pWritePtr, ACE_Message_Block(RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator), -1);

			/* Header Copy */
			pWritePtr->copy((char*)&pHead, MMSV_PACKET_HEAD_SIZE);		


			/* Data Copy */
			while(1)
			{
				if(pWritePtr->space() <= 0)
				{
					m_pQueue_Writer->enqueue_tail(pWritePtr);

					pWritePtr = 0;
					ACE_NEW_RETURN(pWritePtr, ACE_Message_Block(RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator), -1);
				}

				if(pWritePtr->space() >= (pHead.ds - dwCopySize))
				{
					pWritePtr->copy((char*)pData + dwCopySize, pHead.ds - dwCopySize);	
					break;
				}
				else
				{
					size_t nSpace = pWritePtr->space();
					pWritePtr->copy((char*)pData + dwCopySize, pWritePtr->space());	
					dwCopySize += nSpace;
					continue;
				}				
			}


			/* Foot Copy */
			if(pWritePtr->space() >= MMSV_PACKET_FOOT_SIZE)
			{
				pWritePtr->copy((char*)&pTail, MMSV_PACKET_FOOT_SIZE);	
			}
			else
			{	
				m_pQueue_Writer->enqueue_tail(pWritePtr);
				
				pWritePtr = 0;
				ACE_NEW_RETURN(pWritePtr, ACE_Message_Block(MMSV_PACKET_FOOT_SIZE, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator), -1);

				pWritePtr->copy((char*)&pTail, MMSV_PACKET_FOOT_SIZE);	
			}
			
			m_pQueue_Writer->enqueue_tail(pWritePtr);

			if(flag_set(FLAG_WRITE_C_, 0) == 0)
				send_packet();
		}
	}

	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋send_packet_queue()━CODE(%d) SIZE(%d)\n"), nCode, nSize));
		return 0;
	}	
	return 0;
}

int mmsv_User::send_packet(ACE_Message_Block* pWritePtr/* = 0*/)
{
	if(strcmp(m_pUserPoint, "mmsv"))
		return 0;

	if(pWritePtr == 0)
	{
		// 전송중이 아니고 큐에 적재된 패킷이 존재해야함
		if(flag_set(FLAG_WRITE_C_, 0) == 1 || m_pQueue_Writer->message_count() == 0 || m_pQueue_Writer->dequeue_head(pWritePtr) == -1 || pWritePtr == 0)
		{
			return 0;
		}
	}

	// 종료 처리중이면 더이상 전송하지 않음
	if(m_bDeferred_Close == 1 || m_bCloseSocket == 1)
	{
		pWritePtr->release();
		return 0;
	}
	

	try
	{
		// 송신 등록 완료
		flag_set(FLAG_WRITE_, 1);

		if(m_pWriter.write(*pWritePtr, pWritePtr->length()) == -1)
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋send_packet(1)━write faild SIZE(%d)\n"), pWritePtr->length()));

			// 송신 완료로 등록
			pWritePtr->release();
			closeclient(4);			
			return -1;
		}	
	}

	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋send_packet(2)━catch(...) SIZE(%d)\n"), pWritePtr->length()));

		// 송신 완료로 등록
		pWritePtr->release();		
		closeclient(4);			
		return 0;
	}	
	return 0;
}
