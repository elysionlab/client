
#ifndef _MMSV_QUEUE_H
#define _MMSV_QUEUE_H

#pragma once

#include "config_patten.h"
#include "ace/Task.h"
#include "mmsv_User.h"
#include "DBCtrl.h"


typedef ACE_Message_Queue<ACE_MT_SYNCH> QUEUE_MT;

class mmsv_Acceptor;
class mmsv_Queue : public ACE_Task<ACE_MT_SYNCH>
{

public:

	mmsv_Queue(mmsv_Acceptor* acc = 0);
	~mmsv_Queue(void);

public:

	mmsv_Acceptor*		m_pAcceptor;

	DBCtrl*				m_pDataBase;

	QUEUE_MT*			m_pQueueCore;

	ACE_Thread_Mutex	m_pLock_Flag_;
	ACE_Thread_Mutex	m_pLock_Flag_Act;

	ACE_Time_Value		m_tPutTimeOut;
	ACE_Time_Value		m_tTaskStateTime;

	__int64				m_nPacketPutCnt;

	bool				m_bCoreSvc;
	char				m_pDBIP		[50];
	char				m_pDBNAME	[50];
	char				m_pDBID		[50];
	char				m_pDBPW		[50];

	UINT				m_nRunThread;

public:

	void mmsv_state();
	int  mmsv_close();
	int  mmsv_db_set(char* pIP, char* pName, char* pID, char* pPW);
	int  PacketSnqSystem(DBCtrl* pDataBase, ACE_Message_Block* pDataBlock, int &nCode);
	int  PacketSnq(DBCtrl* pDataBase, mmsv_User* pClient, ACE_Message_Block* pDataBlock, int &nCode);

public:

	virtual int open(void* = 0);
	virtual int svc();
	virtual int put(ACE_Message_Block* mblk, ACE_Time_Value* timeout = 0);
		
};

#endif /* _MMSV_QUEUE_H */