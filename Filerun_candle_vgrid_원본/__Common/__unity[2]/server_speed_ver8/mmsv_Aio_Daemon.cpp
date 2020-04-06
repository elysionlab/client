#include "mmsv_Aio_Daemon.h"


extern SERVER_RUNTIME_INFO RunServerInfo;

mmsv_Aio_Daemon::mmsv_Aio_Daemon()
{
	m_pAddr_listen.set_port_number(RunServerInfo.nServerPORT);
	m_pAcceptor = new mmsv_Acceptor(this);
	m_pProactor = new ACE_Proactor();

	if(m_pAcceptor->mmsv_init(m_pProactor) == -1)
	{
		ACE_DEBUG((LM_ERROR, "%M Acceptor::mmsv_init(m_pProactor) : Faild\n"));
	}
}

mmsv_Aio_Daemon::~mmsv_Aio_Daemon(void)
{	
}

int mmsv_Aio_Daemon::open(void*)
{	
	ACE_DEBUG((LM_INFO, ACE_TEXT("*** Acceptor::mmsv_init() Success!\n")));

	if(m_pAcceptor->open(m_pAddr_listen, 0, 0, ACE_DEFAULT_BACKLOG, 1, m_pProactor, 1) == -1) 
	{
		ACE_DEBUG((LM_ERROR, "%M Acceptor::open() Faild\n"));
		return -1;
	}

	ACE_DEBUG((LM_INFO, ACE_TEXT("*** Acceptor::open() Success!\n")));

	return activate (THR_NEW_LWP, MAX_THREADS);
}

int mmsv_Aio_Daemon::svc(void)
{
	m_pProactor->proactor_run_event_loop();	
	return 0;
}

void mmsv_Aio_Daemon::mmsv_close()
{
	// Listen Socket Close
	m_pAcceptor->close();

	/*
		정상적으로 종료될때까지 대기
	*/
	while(1)
	{
		ACE_OS::sleep(1);
		if(m_pAcceptor->m_nCloseComplete == 1)
			break;
	}

	m_pProactor->proactor_end_event_loop();
	wait();

	delete m_pAcceptor;
	delete m_pProactor;

	ACE_DEBUG((LM_INFO, ACE_TEXT("*** mmsv_Aio_Daemon::mmsv_close()!\n")));
}

