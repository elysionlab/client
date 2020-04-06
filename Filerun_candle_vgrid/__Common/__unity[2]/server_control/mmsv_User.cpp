
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
		// �ѹ��� �����ϵ��� ����
		m_bCleanProcess = 1;

		// ������ȸ������ ����
		if(RunServerInfo.nServerRunMode == MMSV_SERVER_MODE_DOWN)
			m_pAcceptor->user_remove(this);	

		m_bCloseSocket = 1;

		// ��ϵ� �񵿱� �۾� ��� ���
		m_pReader.cancel();
		m_pWriter.cancel();

		// ť�� ���� �ִ� ��� ���� ������ ����
		m_pQueue_Writer->flush();
		m_pQueue_Reader->flush();

		// ����� FTP ���� ����
		m_pFtp->DisConnect();	
		
		// ���� �ڵ� ����
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

	// �������� ������ �� ����
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
	

	// ����ð�
	m_tConnectTime	= ACE_OS::gettimeofday();
	m_tAccessTime	= ACE_OS::gettimeofday();


	if( m_pReader.open(*this, new_handle, 0, m_pAcceptor->m_pProactor) != 0 ||
		m_pWriter.open(*this, new_handle, 0, m_pAcceptor->m_pProactor) != 0)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::open()��m_pReader.open() or m_pWriter.open() Faild\n"))); 

		delete this;
		return;
	}


	/* ��������� ��ŭ �б� ��û */	
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
��� �۾��� ����ɶ� ȣ��Ǵ� �Լ� 

1. bCloseMode : 1

	Ŭ���̾�Ʈ�� ������ ����Ǿ� handle_read_stream() ���� ȣ���
	�ش� ���ϰ������� ť�� ����Ǿ��ų�, Send ���� ����� �ʱ�ȭ �Ǿ��������� �ٷ� ������� �ʰ� 
	��� ó���� �Ϸ�ɶ����� ���

2. bCloseMode : 2

	��������(mmsv_Aio_Acceptor�� Ÿ�̸ӿ��� ȣ��) �ǰų�, recv ����, �߸��� �������ݷ� �������� ��ų�� ���
	���� �������� ��û�� ���� �������� ��û�� �����ð�(10��) ���� �ڵ� �������� ���Ҷ� �������Ḧ ���� Ÿ�̸� �ʱ�ȭ

3. bCloseMode : 3

	ť���� �۾��� ��ģ�� ť�� ����� ī���͸� ���ҽ�Ű�� �� ���̿� ���� ���� �Ǿ� �ִ��� üũ
	���ἳ���� ���¿��� ť �۾��� ��� �Ϸ��ص� send �۾��� ���� ������ ������� ����

4. bCloseMode : 4

	���� �۾��� ��ģ�� ���� ī���͸� ���ҽ�Ű�� �� ���̿� ���� ���� �Ǿ� �ִ��� üũ
	���ἳ���� ���¿��� ���� �۾��� ��� �Ϸ��ص� ť �۾��� ���� ������ ������� ����

*/
int	mmsv_User::closeclient(int bCloseMode)
{
	m_pLock_CloseCh_.acquire();

	if(strcmp(m_pUserPoint, "mmsv"))
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��closeclient()��UserPoint Error\n")));
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
			// ������ �ݰ� ���� �ð����� ������� ������ ���� ���� 
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
				// ť�� ������ �κ��� �ִ��� üũ�� ����
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
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��closeclient()��Mode(%d)\n"), bCloseMode)); 
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
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��putqueue(3)��m_pQueue->put() Error : Count(%d) : %s\n"), m_pQueue_Reader->message_count(), getuserinfo())); 
					}
				}
				else
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��putqueue(3)��dequeue_head() Error : No Empty Queue : Count(%d) : %s\n"), m_pQueue_Reader->message_count(), getuserinfo())); 
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
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::putqueue()��put() = -1 : FLAG_QUEUE_C_ : %s\n"), getuserinfo())); 
					m_bFlag_Queue = 0;
					pNewMessage->release();
				}
			}
			else
			{
				if(m_pQueue_Reader->enqueue_tail(pNewMessage) == -1)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::putqueue()��enqueue_tail() = -1 : %s\n"), getuserinfo())); 
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
		ACE_Asynch_Acceptor::open()�� pass_addresses �ɼ��� 0�� �ƴѰ��� ������ �Ÿ޼ҵ� ȣ���
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

			ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::handle_time_out()��Mode 1 : %s\n"), getuserinfo()));
			delete this;
		}		
		
		m_pAcceptor->m_pProactor->cancel_timer(*this);
		m_bTimerCallMode = 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::handle_time_out()��m_bTimerCallMode(%d) Catch(...)\n"), (int)m_bTimerCallMode)); 
	}
}




void mmsv_User::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
{
	/*
		result.success()			: �񵿱� ����� ��������
		result.bytes_to_read()		: �񵿱� ��ɿ��� ��û�� ����Ʈ ��
		result.bytes_transferred()	: �񵿱� ������� ���� ���۵� ����Ʈ ��
		message_block()				: ��ɿ��� ���� ACE_Message_Block�� ���� ��û
		m_pMB_Read->length()		: �Ҵ���� ������ ���۹��� �� ������ ������
	*/

	// �б��۾� �Ϸ�
	flag_set(FLAG_READ_, 0);

	try
	{

		/* �б⸦ �����߰ų� */
		if(!result.success() || result.bytes_transferred() == 0)
		{
			/* ��� �޽������� ���� */
			m_pMB_Read->release();
			closeclient(1);
			return;
		}	

		/* ��Ŷ�� ��� �Ǵ� �ٵ�, ���� �κ� ������ �Ϸ���� �ʾ����Ƿ� ���� ũ�⸸ŭ �ٽ� ��û */
		else if(result.bytes_transferred() < result.bytes_to_read())
		{
			recv_packet(&result.message_block(), result.bytes_to_read() - result.bytes_transferred(), 2);
			return;
		}

		/* ��������� ������ üũ�� ������ �κ��� ���� ��û */
		else if(&result.message_block() == m_pMB_Read && result.message_block().length() == MMSV_PACKET_HEAD_SIZE)
		{
			PHEAD pHead = (PHEAD)m_pMB_Read->rd_ptr();		

			/* ��Ŷ�� �ٵ� �κ��� ���� ��� */
			if(pHead->ps == m_pMB_Read->length())
			{
				push_queue();			

				/* ��������� ��ŭ �б� ��û */	
				ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
				if(recv_packet(m_pMB_Read, MMSV_PACKET_HEAD_SIZE, 3) == -1)
					return;
			}
			else
			{
				if(pHead->ps <= RunServerInfo.nMaxSockBuffer)
				{
					// ������ ����� ���� �����ͺ� ũ�⺸�� ������� ������ ����
					if(recv_packet(m_pMB_Read, (size_t)pHead->ps - m_pMB_Read->length(), 4) == -1)
						return;
				}
				else
				{
					// ������ ����� ���� �����ͺ� ũ�⺸�� Ŭ��� ������ ����
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

				/* �޽����� �ϳ� �߰��� ��� ��Ŷ ������ �����Ҷ� */
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

			/* ping ���������� �۾�ť�� ���� ���� */
			if(pHead->code == USER_PTR_UPDATE)
			{
				m_pMB_Read->release();
				m_pMB_Read = 0;				
			}
			else
			{
				push_queue();
			}

			// ������ ���� ���� �ð� ����
			connect_check(false);

			/* ��������� ��ŭ �б� ��û */	
			ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
			if(recv_packet(m_pMB_Read, MMSV_PACKET_HEAD_SIZE, 8) == -1)
				return;
		}	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��handle_read_stream()��m_bFlag_Read(%d)\n"), m_bFlag_Read)); 
	}
}


int mmsv_User::recv_packet(ACE_Message_Block* pMB_Block, size_t nSize, int nMode)
{
	if(strcmp(m_pUserPoint, "mmsv"))
		return 0;

	// ���� ó�����̸� ���̻� �������� ����
	if(m_bDeferred_Close == 1 || m_bCloseSocket == 1)
	{
		m_pMB_Read->release();			
		return 0;
	}

	try
	{
		if(m_pReader.read(*pMB_Block, nSize) == -1)
		{
			/* ��û�� �޽������� �������� �ʰ� �ֻ��� ���� �����ؼ� ��ü�� ���� */
			//ACE_DEBUG((LM_WARNING, ACE_TEXT("��recv_packet()��read() == -1 ��UserSign(%s) :: Call [%d]\n"), m_pUserSign, nMode)); 
			m_pMB_Read->release();			
			closeclient(1);
			return -1;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��recv_packet()��m_bFlag_Read(%d)\n"), m_bFlag_Read)); 
	}

	// �б��۾� ���
	flag_set(FLAG_READ_, 1);

	return 0;
}



int mmsv_User::push_queue()
{
	if(strcmp(m_pUserPoint, "mmsv"))
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::push_queue()��m_nUserRefuCall != 100\n"))); 
		return 0;
	}

	try
	{
		// ���� ó�����̸� ���̻� �������� ����
		if(m_bDeferred_Close == 1 || m_bCloseSocket == 1)
		{
			m_pMB_Read->release();
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::push_queue()��m_bDeferred_Close = 1 || m_bCloseSocket = 1\n"))); 
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
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_User::push_queue()��m_bFlag_Queue(%d)\n"), m_bFlag_Queue)); 
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
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��handle_write_stream()��!result.success() SIZE(%d)\n"), result.message_block().length())); 

			result.message_block().release();
			closeclient(4);
			return;
		}
		else
		{
			ACE_Message_Block* pWritePtr = &result.message_block();

			// ��� ���۵Ǿ�����
			if(result.message_block().length() == 0 || m_bCloseSocket == 1)
			{
				result.message_block().release();
				closeclient(4);
				return;
			}

			// ������ �κ��� ��������
			else
			{
				if(m_pWriter.write(result.message_block(), result.message_block().length()) == -1)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��handle_write_stream()��SIZE(%d)\n"), result.message_block().length())); 
					
					result.message_block().release();
					closeclient(4);
					return;
				}
				else
				{
					// �۽� ��ϻ��� ����
					return;
				}
			}		
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��handle_write_stream()��m_bFlag_Write(%d)\n"), m_bFlag_Write)); 
	}
	return;	
}

/***********************************************************************************************************
// ����Ʈ�� �̿��� ��Ŷ ����
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
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��send_packet_queue()��CODE(%d) SIZE(%d)\n"), nCode, nSize));
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
		// �������� �ƴϰ� ť�� ����� ��Ŷ�� �����ؾ���
		if(flag_set(FLAG_WRITE_C_, 0) == 1 || m_pQueue_Writer->message_count() == 0 || m_pQueue_Writer->dequeue_head(pWritePtr) == -1 || pWritePtr == 0)
		{
			return 0;
		}
	}

	// ���� ó�����̸� ���̻� �������� ����
	if(m_bDeferred_Close == 1 || m_bCloseSocket == 1)
	{
		pWritePtr->release();
		return 0;
	}
	

	try
	{
		// �۽� ��� �Ϸ�
		flag_set(FLAG_WRITE_, 1);

		if(m_pWriter.write(*pWritePtr, pWritePtr->length()) == -1)
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��send_packet(1)��write faild SIZE(%d)\n"), pWritePtr->length()));

			// �۽� �Ϸ�� ���
			pWritePtr->release();
			closeclient(4);			
			return -1;
		}	
	}

	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��send_packet(2)��catch(...) SIZE(%d)\n"), pWritePtr->length()));

		// �۽� �Ϸ�� ���
		pWritePtr->release();		
		closeclient(4);			
		return 0;
	}	
	return 0;
}
