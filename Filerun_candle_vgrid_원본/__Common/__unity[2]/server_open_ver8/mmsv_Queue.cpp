#include "mmsv_Queue.h"
#include "mmsv_Aio_Acceptor.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


mmsv_Queue::mmsv_Queue(mmsv_Acceptor* acc/* = 0*/)
:	m_pAcceptor(acc),
	m_nRunThread(0),
	m_nPacketPutCnt(0),
	m_pDataBase(0)
{
	m_tPutTimeOut.set(1);
	
	m_bCoreSvc = false;
	ACE_NEW_NORETURN(m_pQueueCore, QUEUE_MT());
	m_pQueueCore->high_water_mark((size_t)RunServerInfo.nMaxSockBuffer * 10240);
}

mmsv_Queue::~mmsv_Queue(void)
{
}


int mmsv_Queue::open(void*)
{	
	msg_queue()->high_water_mark((size_t)RunServerInfo.nMaxSockBuffer * 5000);

	activate(THR_NEW_LWP, RunServerInfo.nQueueMaxThreads + 1);
	while(m_nRunThread < RunServerInfo.nQueueMaxThreads + 1)
		ACE_OS::sleep(1);

	return 1;
}

int mmsv_Queue::mmsv_db_set(char* pIP, char* pName, char* pID, char* pPW)
{
	try
	{
		lstrcpy(m_pDBIP,		pIP);
		lstrcpy(m_pDBNAME,	pName);
		lstrcpy(m_pDBID,		pID);
		lstrcpy(m_pDBPW,		pPW);
	}
	catch(...)
	{
		return -1;
	}

	return 0;
}


void mmsv_Queue::mmsv_state()
{
	ACE_GUARD(ACE_Thread_Mutex, c_flag_mon_, m_pLock_Flag_);

	double nPFS = 0;

	if(m_nPacketPutCnt != 0)
		nPFS = (double)m_nPacketPutCnt / (double)CONNECT_CHECK_TIME;
	
	ACE_DEBUG((LM_INFO, ACE_TEXT("��PFS (%.2f) : Total(%Q P / %d S)\n"), nPFS, m_nPacketPutCnt, CONNECT_CHECK_TIME));

	m_nPacketPutCnt = 0;
}


int mmsv_Queue::mmsv_close()
{
	m_pQueueCore->flush();
	delete m_pQueueCore;
	delete this;
	return 0;
}


/* ��ŷ ������� ���� */
int mmsv_Queue::put(ACE_Message_Block* mblk, ACE_Time_Value* timeout/* = 0*/)
{
	int nResult = 0;
	int nCode;
	ACE_Message_Block* pMBlockBuffer;

	ACE_GUARD_RETURN(ACE_Thread_Mutex, c_flag_mon_, m_pLock_Flag_, 0);

	if(mblk->msg_type() == ACE_Message_Block::MB_DATA)
	{
		pMBlockBuffer = mblk->cont();
		PHEAD pHead = (PHEAD)pMBlockBuffer->rd_ptr();
		nCode = pHead->code;

		switch(nCode)
		{
			case USER_PTR_UPDATE:
			case CHK_AUTH:
			case LOAD_UPLIST:
			case LOAD_DOWNLIST:	
			{
				m_pQueueCore->enqueue_tail(mblk);		
				return nResult;
			}
			default: 
				break;
		}		
	}

	if(timeout == 0)
	{
		if((nResult = putq(mblk, &m_tPutTimeOut)) == -1)
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::put()��EWOULDBLOCK : Use Time(%d)\n"), (int)(m_tPutTimeOut.sec())));
	}
	else
	{
		if((nResult = putq(mblk, timeout)) == -1)
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::put()��EWOULDBLOCK\n")));
	}
	
	m_nPacketPutCnt++;

	return nResult;
}


int mmsv_Queue::svc(void)
{
	int nResult;
	int nPacketCode = -1;
	bool bCoreSvc = false;
	DBCtrl* pDataBase;
	mmsv_User* pClient;
	ACE_Message_Block* pMBlock_Buf, *pMBlock_Buf2;
	ACE_NEW_RETURN(pDataBase, DBCtrl(), 0);	
	
	ACE_Time_Value tQueryTime;	

	if(!pDataBase->Init(m_pDBIP, m_pDBNAME, m_pDBID, m_pDBPW))
	{
		ACE_DEBUG((LM_WARNING, "*** Queue svc(%t) : DataBase Init Faild\n"));
		++m_nRunThread;
		return 0;
	}
	else
	{
		ACE_DEBUG((LM_INFO, "*** Queue svc(%t) : Init Success!\n"));
		++m_nRunThread;

		if(OPTION_USE_FILEUNITY == 1)
		{
			pDataBase->m_pFileDB->BIC_Init(RunServerInfo.pDBInfoCommon[0], RunServerInfo.pDBInfoCommon[1], 
										   RunServerInfo.pDBInfoCommon[2], RunServerInfo.pDBInfoCommon[3]);
		}
	}
	

	m_pLock_Flag_Act.acquire();
	if(!m_bCoreSvc)
	{
		bCoreSvc = true;
		m_bCoreSvc = true;
	}
	m_pLock_Flag_Act.release();

	if(bCoreSvc)
	{
		for(ACE_Message_Block* mbdata; m_pQueueCore->dequeue_head(mbdata) != -1; )
		{
			tQueryTime = ACE_OS::gettimeofday();

			try
			{			
				pClient = ACE_reinterpret_cast(mmsv_User*, mbdata->rd_ptr());

				if(mbdata->cont()->total_length() > RunServerInfo.nMaxSockBuffer)
				{	
					ACE_NEW_NORETURN(pMBlock_Buf, ACE_Message_Block(mbdata->total_length() + 1));				
					pMBlock_Buf2 = mbdata->cont();

					while(1)
					{
						pMBlock_Buf->copy(pMBlock_Buf2->rd_ptr(), pMBlock_Buf2->length());

						if((pMBlock_Buf2 = pMBlock_Buf2->cont()) == 0)
							break;
					}

					ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��core��To Big Message Block\n")));

					pClient->m_bDelayQuery = 1;
					nResult = PacketSnq(pDataBase, pClient, pMBlock_Buf, nPacketCode);
					pClient->m_bDelayQuery = 0;
					pMBlock_Buf->release();
				}
				else
				{
					pClient->m_bDelayQuery = 1;
					nResult = PacketSnq(pDataBase, pClient, mbdata->cont(), nPacketCode);
					pClient->m_bDelayQuery = 0;
				}		

				pClient->closeclient(3);
			}
			catch(...)
			{
				ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��core��catch()\n")));
			}

			if(((ACE_OS::gettimeofday() - tQueryTime).sec()) > 5)
				ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��core��PacketSnq : Slow Query : Code(%d) : %d��\n"), nPacketCode, (int)(ACE_OS::gettimeofday() - tQueryTime).sec()));

			mbdata->release();
		}
	}
	else
	{
		for(ACE_Message_Block* mbdata; getq(mbdata) != -1; )
		{	
			tQueryTime = ACE_OS::gettimeofday();

			try
			{			
				/* �ý��� ó�� ��û */
				if(mbdata->msg_type() == ACE_Message_Block::MB_PROTO)
				{
					PacketSnqSystem(pDataBase, mbdata, nPacketCode);
				}

				/* �Ϲ� ��Ŷ ó�� */
				else if(mbdata->msg_type() == ACE_Message_Block::MB_DATA)
				{
					pClient = ACE_reinterpret_cast(mmsv_User*, mbdata->rd_ptr());

					if(mbdata->cont()->total_length() > RunServerInfo.nMaxSockBuffer)
					{	
						ACE_NEW_NORETURN(pMBlock_Buf, ACE_Message_Block(mbdata->total_length() + 1));				
						pMBlock_Buf2 = mbdata->cont();

						while(1)
						{
							pMBlock_Buf->copy(pMBlock_Buf2->rd_ptr(), pMBlock_Buf2->length());

							if((pMBlock_Buf2 = pMBlock_Buf2->cont()) == 0)
								break;
						}

						ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��To Big Message Block\n")));

						pClient->m_bDelayQuery = 1;
						nResult = PacketSnq(pDataBase, pClient, pMBlock_Buf, nPacketCode);
						pClient->m_bDelayQuery = 0;
						pMBlock_Buf->release();
					}
					else
					{
						pClient->m_bDelayQuery = 1;
						nResult = PacketSnq(pDataBase, pClient, mbdata->cont(), nPacketCode);
						pClient->m_bDelayQuery = 0;
					}		

					
					if(nResult == -1)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��Force Close\n")));

						if(pClient->closeclient(3) != -1)
							pClient->closeclient(2);	
						else
							ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��pClient->closeclient(3) == -1\n")));
					}
					else
					{
						pClient->closeclient(3);
					}
				}			
			}
			catch(...)
			{
				ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��catch()\n")));
			}

			if(((ACE_OS::gettimeofday() - tQueryTime).sec()) > 5)
			{
				if(mbdata->msg_type() == ACE_Message_Block::MB_PROTO)
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��PacketSnqSystem : Slow Query : %d��\n"), (int)((ACE_OS::gettimeofday() - tQueryTime)).sec()));
				else if(mbdata->msg_type() == ACE_Message_Block::MB_DATA)
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��mmsv_Queue::svc()��PacketSnq : Slow Query : Code(%d) : %d��\n"), nPacketCode, (int)(ACE_OS::gettimeofday() - tQueryTime).sec()));
			}

			mbdata->release();		
		}
	}


	ACE_DEBUG((LM_NOTICE, "*** Queue svc() End!\n"));
	delete pDataBase;

	return 0;
}



int mmsv_Queue::PacketSnqSystem(DBCtrl* pDataBase, ACE_Message_Block* pDataBlock, int &nCode)
{
	PHEAD pHead;

	if(pDataBlock == 0)
		return 0;

	try
	{	
		pHead = (PHEAD)pDataBlock->rd_ptr();
		nCode = pHead->code;

		switch(pHead->code)
		{
			default:
			{
				break;
			}
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��PacketSnqSystem() -> catch()\n")));
	}

	return 0;
}

int mmsv_Queue::PacketSnq(DBCtrl* pDataBase, mmsv_User* pClient, ACE_Message_Block* pDataBlock, int &nCode)
{
	int nHeadSet = 0;
	nCode = -1;
	PHEAD pHead;

	if(pDataBlock == 0)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��PacketSnq() -> pDataBlock = 0\n")));
		return -1;
	}

	// ��ȿ�� üũ
	if(strcmp(pClient->m_pUserPoint, "mmsv"))
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��PacketSnq() -> UserPoint Error\n")));
		return 0;
	}

	try
	{	
		pHead = (PHEAD)pDataBlock->rd_ptr();
		nCode = pHead->code;
		nHeadSet = 1;


		// ������ ��û �۾�
		pClient->update_packet(nCode);

		// ������ ���� ���� ó���Ǵ� ��������
		switch(pHead->code)
		{
			case USER_PTR_UPDATE:
			{
				return 0;
			}						
			// ���� ��û
			case CHK_AUTH:		
			{
				pDataBase->AuthUser(pClient, pDataBlock);	
				return 0;
			}
		}

		// ���� �Ϸ� �� ��û������ ��������
		if(pClient->m_bSession == 1)
		{
			if(RunServerInfo.nServerRunMode == MMSV_SERVER_MODE_OPEN)
			{
				switch(pHead->code)
				{
					//20080513 ���� �ڷ� ���ε� �������� �߰�, jyh
				case OPEN_UPLIST:
					{
						//printf("OPEN_UPLIST ��Ŷ ����\n");
						pDataBase->OpenUpLoadList(pClient, pDataBlock);
						return 0;
					}
				case OPEN_UPLOAD:
					{
						//printf("OPEN_UPLOAD ��Ŷ ����\n");
						pDataBase->OpenUpLoad(pClient, pDataBlock);
						return 0;
					}
				case OPEN_UPEND:
					{
						//printf("OPEN_UPEND ��Ŷ ����\n");
						pDataBase->OpneUpLoadEnd(pClient, pDataBlock, pHead->ps);
						return 0;
					}
					//20080513 ���� �ڷ� �ٿ�ε� �������� �߰�, jyh
				case OPEN_DOWNLIST:
					{
						//printf("OPEN_DOWNLIST ��Ŷ ����\n");
						pDataBase->OpenDownLoadList(pClient, pDataBlock);
						return 0;
					}							
				case OPEN_DOWNLOAD:
					{
						//printf("OPEN_DOWNLOAD ��Ŷ ����\n");
						pDataBase->OpenDownLoad(pClient, pDataBlock);
						return 0;
					}
				case OPEN_DOWNEND:
					{
						//printf("OPEN_DOWNEND ��Ŷ ����\n");
						pDataBase->OpenDownLoadEnd(pClient, pDataBlock);
						return 0;
					}

				default:
					break;
				}
			}
		}
	}
	catch(...)
	{
		if(nHeadSet == 1) 
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��packetSnq() catch(...)��IP[%s] ID[%s] CODE[%d]\n"), 
											pClient->m_pAddr_Remote.get_host_addr(), pClient->m_pUserid, pHead->code));
		}
		else 
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��packetSnq() catch(...)��IP[%s] ID[%s]\n"), 
											pClient->m_pAddr_Remote.get_host_addr(), pClient->m_pUserid));
		}
		return 0;
	}

	/* 
		�̺κб��� Return �� ���� �ʾ����� 
		�˼����� ���������̰ų� �������� ���� �÷��׿��� ȣ��� ������ �Ǵ�
	*/

	if(nHeadSet == 1) 
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��packetSnq() bad protocol��IP[%s] ID[%s] AUTH[%d] CODE[%d]\n"), 
										pClient->m_pAddr_Remote.get_host_addr(), 
										pClient->m_pUserid, (int)pClient->m_bSession, pHead->code));
	}
	else 
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��packetSnq() bad protocol��IP[%s] ID[%s] AUTH[%d]\n"), 
										pClient->m_pAddr_Remote.get_host_addr(), 
										pClient->m_pUserid, (int)pClient->m_bSession)); 
	}
	return 0;
}
