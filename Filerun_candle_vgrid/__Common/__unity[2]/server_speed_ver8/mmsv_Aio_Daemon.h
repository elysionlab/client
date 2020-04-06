#ifndef _MMSV_AIO_DAEMON_H
#define _MMSV_AIO_DAEMON_H

#pragma once

#include "config_patten.h"
#include "ace/Proactor.h"
#include "ace/Task.h"
#include "mmsv_Aio_Acceptor.h"

class mmsv_Aio_Daemon : public ACE_Task<ACE_NULL_SYNCH>
{

public:

	enum { MAX_THREADS = 1 };

	mmsv_Aio_Daemon(void);
	~mmsv_Aio_Daemon(void);
	
	mmsv_Acceptor*	m_pAcceptor;
	ACE_Proactor*	m_pProactor;

	ACE_INET_Addr	m_pAddr_listen;
	ACE_INET_Addr	m_pAddr_gate;

public:

	virtual int open(void* = 0);
	virtual int svc();
	void mmsv_close();
};


#endif /* _MMSV_AIO_DAEMON_H */
