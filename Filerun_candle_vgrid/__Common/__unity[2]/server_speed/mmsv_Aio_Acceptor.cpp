#include "ace/Malloc_T.h"
#include "ace/Synch.h"
#include "ace/Proactor.h"
#include "mmsv_Aio_Daemon.h"
#include "mmsv_Aio_Acceptor.h"
#include "ace/os_include/arpa/os_inet.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


mmsv_Acceptor::mmsv_Acceptor(mmsv_Aio_Daemon* pDaemon) 
:	m_pDaemon(pDaemon),
	m_nNowConnects(0),
	m_nCloseComplete(0)
{
}


int mmsv_Acceptor::mmsv_init(ACE_Proactor*	pProactor)
{	
	m_pProactor = pProactor;

	ACE_NEW_RETURN(m_pAllocator, ACE_Dynamic_Cached_Allocator<ACE_Thread_Mutex>
					(RunServerInfo.nMaxUserCount * RunServerInfo.nMaxUserAllocator, (size_t)RunServerInfo.nMaxSockBuffer + 1), -1);

	// 유효성 체크 타이머 설정
	ACE_Time_Value tCheck(MMSV_CHECK_USER_SEC);		
    m_pProactor->schedule_timer(*this, 0, tCheck, tCheck);
	m_bTimerCall = 1;

	return 1;
}


mmsv_User* mmsv_Acceptor::make_handler(void)
{
	mmsv_User* pAio;
	ACE_NEW_RETURN(pAio, mmsv_User(this), 0);	

	m_nNowConnects++;
	return pAio;
}

int mmsv_Acceptor::validate_connection(const ACE_Asynch_Accept::Result& result, const ACE_INET_Addr &remote, const ACE_INET_Addr &local)
{
	return 0;
}

void mmsv_Acceptor::close(void)
{
	if(m_bTimerCall == 1)
	{
		m_pProactor->cancel_timer(*this);
		m_bTimerCall = 0;
	}
	
	delete m_pAllocator;
	m_nCloseComplete = 1;
}

void mmsv_Acceptor::closeuser()
{
	m_nNowConnects--;
}

/*
	인증된 접속자중 일정시간동안 Ping 패킷이 들어오지 않을 경우 종료처리
*/
void mmsv_Acceptor::handle_time_out(const ACE_Time_Value &tv, const void *arg)
{
	char pBuf[100], pBuf2[100];	
	sprintf(pBuf, "│ User Count = %d\n", m_nNowConnects);
	sprintf(pBuf2, "│ Pool Count = %d\n", ((ACE_Dynamic_Cached_Allocator<ACE_Thread_Mutex>*)m_pAllocator)->pool_depth());

	ACE_DEBUG((LM_INFO, "\n┌───────────────────────────────────────────────────────────┐\n"));	
	ACE_DEBUG((LM_INFO, "│%D\n"));
	ACE_DEBUG((LM_INFO, pBuf));
	ACE_DEBUG((LM_INFO, pBuf2));
	ACE_DEBUG((LM_INFO, "│%D \n"));
	ACE_DEBUG((LM_INFO, "└───────────────────────────────────────────────────────────┘\n\n"));
}