#include "ace/Malloc_T.h"
#include "ace/Synch.h"
#include "ace/Proactor.h"
#include "mmsv_Aio_Daemon.h"
#include "mmsv_Aio_Acceptor.h"
#include "ace/os_include/arpa/os_inet.h"


extern SERVER_RUNTIME_INFO RunServerInfo;

mmsv_Acceptor::mmsv_Acceptor(mmsv_Aio_Daemon* pDaemon) 
:	m_nCloseFlag(0),
	m_nCloseComplete(0),
	m_bTimerCall(0),
	m_nHighConnects(0), 
	m_nNowConnects(0),
	m_pUserHashMap(0), 
	m_pDaemon(pDaemon)
{
}

//20080218 jyh
int mmsv_Acceptor::mmsv_init(ACE_Proactor*	pProactor, char* pIP, char* pName, char* pID, char* pPW)
//int mmsv_Acceptor::mmsv_init(ACE_Proactor*	pProactor, SERVER_RUNTIME_INFO* pServerInfo)
{	
	m_pProactor = pProactor;

	ACE_NEW_RETURN(m_pAllocator, ACE_Dynamic_Cached_Allocator<ACE_Thread_Mutex>
					(RunServerInfo.nMaxUserCount * RunServerInfo.nMaxUserAllocator, (size_t)RunServerInfo.nMaxSockBuffer + 1), -1);

	ACE_NEW_RETURN(m_pQueue, mmsv_Queue(this), -1);
	m_pQueue->mmsv_db_set(pIP, pName, pID, pPW);	

	ACE_NEW_RETURN(m_pUserHashMap, Hash_Map_Manager_Int(), -1);
	m_pUserHashMap->InitMap(RunServerInfo.nMaxUserCount * 1.5);

	// 유효성 체크 타이머 설정
	ACE_Time_Value tCheck(CONNECT_CHECK_TIME);		
    m_pProactor->schedule_timer(*this, 0, tCheck, tCheck);
	m_bTimerCall = 1;

	m_nMaxAllocator = check_allocator_pool_depth();

	return m_pQueue->open();
}


mmsv_User* mmsv_Acceptor::make_handler(void)
{
	/*
		종료 플래그가 설정된 경우 신규 접속은 받지 않음
	*/
	if(m_nCloseFlag == 1)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("mmsv_Acceptor::make_handler() == (m_nCloseFlag == 1)\n")));
		return 0;
	}
	
	/*
		MMSV_SERVER_MAX_USER 를 기준으로 동시접속자 제한 체크
	*/
	if((size_t)RunServerInfo.nMaxUserCount + 50 < m_nNowConnects)
	{
		return 0;
	}

	mmsv_User* pAio = 0;	
	ACE_NEW_RETURN(pAio, mmsv_User(this), 0);	

	if(!connect_manager(1, pAio))
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("mmsv_Acceptor::connect_manager(1) = -1\n")));
		delete pAio;
		return 0;
	}

	return pAio;
}

int mmsv_Acceptor::validate_connection(const ACE_Asynch_Accept::Result& result, const ACE_INET_Addr &remote, const ACE_INET_Addr &local)
{
	return 0;
}


/*
	인증된 접속자중 일정시간동안 Ping 패킷이 들어오지 않을 경우 종료처리
*/
void mmsv_Acceptor::handle_time_out(const ACE_Time_Value &tv, const void *arg)
{
	ACE_DEBUG((LM_INFO, "\n"));

	ACE_DEBUG((LM_INFO, "┌────────────────────────────────────────────────────────────────────────────┐\n"));	
	ACE_DEBUG((LM_INFO, "│%D\n"));

	mmsv_server_state();

	if(m_nCloseFlag == 1)
	{
		if(m_bTimerCall == 1)
		{
			m_pProactor->cancel_timer(*this);
			m_bTimerCall = 0;
		}
	}
	else
	{
		if(RunServerInfo.nLogLevel >= 2) m_pQueue->mmsv_state();
		connect_manager(0, NULL);		
	}

	ACE_DEBUG((LM_INFO, "│%D \n"));
	ACE_DEBUG((LM_INFO, "└────────────────────────────────────────────────────────────────────────────┘\n"));
}

void mmsv_Acceptor::mmsv_server_state()
{
	if(RunServerInfo.nLogLevel >= 1)
		ACE_DEBUG((LM_INFO, ACE_TEXT("│CurrUser(%d) : HighUser(%d) : HashMap(%d)\n"), m_nNowConnects, m_nHighConnects, (int)m_pUserHashMap->UserMapCount()));
	if(RunServerInfo.nLogLevel >= 2)
		ACE_DEBUG((LM_INFO, ACE_TEXT("│Queue Byte(%d) : Queue Count(%d)\n"), m_pQueue->msg_queue()->message_bytes(), m_pQueue->msg_queue()->message_count()));
	if(RunServerInfo.nLogLevel >= 3)
		ACE_DEBUG((LM_INFO, ACE_TEXT("│Allocator (%d F / %d T) : %.2f%% Free\n"), check_allocator_pool_depth(), m_nMaxAllocator, ((double)check_allocator_pool_depth() / (double)m_nMaxAllocator) * 100));
}

// 메모리 풀 상태 체크
size_t mmsv_Acceptor::check_allocator_pool_depth()
{
	return ((ACE_Dynamic_Cached_Allocator<ACE_Thread_Mutex>*)m_pAllocator)->pool_depth();
}


// 클라이언트 연결 관리
bool mmsv_Acceptor::connect_manager(int nCallMode, mmsv_User* pUser)
{
	ACE_GUARD_RETURN(ACE_Thread_Mutex, c_flag_manager_, m_pLock_C_Manager_, true);

	switch(nCallMode)
	{
		// 전체 핸들 유효성체크
		case 0:
		{
			int nCountCheck = 0;
			int nCountCheck_Use = 0;
			int nCountCheck_Try = 0;
			int nCountCheck_Guard = 0;
			int nCountCheck_Close = 0;

			mmsv_User* pCheckUser = 0;

			try
			{
				for(UNBOUNDED_SET::iterator iterator = m_pClients.begin(); iterator != m_pClients.end (); ++iterator, nCountCheck++)
				{
					pCheckUser = (*iterator);

					if(!strcmp(pCheckUser->m_pUserPoint, "mmsv"))
					{
						nCountCheck_Use = pCheckUser->connect_check(true);

						switch(nCountCheck_Use)
						{
							case 1:
								nCountCheck_Close++;
								break;
							case -1:
								nCountCheck_Guard++;
								break;
							case -2:
								nCountCheck_Try++;
								break;
						}
					}
					else
					{
						++iterator;

						if(m_pClients.remove(pCheckUser) != -1)
							--m_nNowConnects;

						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_Acceptor::connect_manager(0, NULL)━UserPoint Error\n")));
					}
				}
			}
			catch(...)
			{
				if(m_pClients.remove(pCheckUser) != -1)
					--m_nNowConnects;

				ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_Acceptor::connect_manager(0, NULL)━Catch(...) Error\n")));
			}

			if(RunServerInfo.nLogLevel >= 2)
				ACE_DEBUG((LM_INFO, ACE_TEXT("│Check Valid User [ %d complete / %d all ] [%d,%d,%d,%d]\n"), 
						nCountCheck, m_pClients.size(), nCountCheck, nCountCheck_Close, nCountCheck_Guard, nCountCheck_Try));			
			return true;
		}
		// 클라이언트 핸들 추가
		case 1:
		{
			if(m_pClients.insert(pUser) == -1)
				return false;
			else
			{
				++m_nNowConnects;

				if(m_nHighConnects < m_nNowConnects)
					m_nHighConnects = m_nNowConnects;

				return true;
			}
		}
		// 클라이언트 핸들 제거
		case 2:
		{
			if(m_pClients.remove(pUser) != -1)
				--m_nNowConnects;
	
			return true;
		}
	}
}


mmsv_User* mmsv_Acceptor::user_check(ACE_UINT64 nMemberIdx)
{
	return m_pUserHashMap->UserMapCheck(nMemberIdx);
}

void mmsv_Acceptor::user_insert(mmsv_User* pUser)
{
	m_pUserHashMap->UserMapInsert(pUser);
}

void mmsv_Acceptor::user_remove(mmsv_User* pUser)
{
	m_pUserHashMap->UserMapRemove(pUser);
}

// 요청된 접속요청과 모든 클라이언트를 닫음
void mmsv_Acceptor::close(void)
{
	m_nCloseFlag = 1;

	ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_Acceptor::close()━%D━\n")));

	try
	{
		if(m_bTimerCall == 1)
		{
			m_pProactor->cancel_timer(*this);
			m_bTimerCall = 0;
		}
		
		m_pQueue->flush();
		m_pQueue->wait();
		m_pQueue->mmsv_close();

		m_pClients.reset();

		delete m_pAllocator;
		delete m_pUserHashMap;


		m_nCloseComplete = 1;

		return;
	}
	catch(...)
	{
	}
}
