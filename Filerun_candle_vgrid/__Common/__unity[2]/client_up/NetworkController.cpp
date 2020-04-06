#include "StdAfx.h"
#include "Networkcontroller.h"
#include "up_window.h"
//#include "Resource.h"		//20080424 vs8�� ���������� ���� ����, jyh
#include <process.h>
#include <assert.h>

NetworkController* pNetWork;


// ��Ŀ ������ ���� ����Ʈ�μ����� ��Ȱ
static DWORD __stdcall WorkerThreadStartingPointSend(PVOID pvParam);
static DWORD __stdcall WorkerThreadStartingPointRecv(PVOID pvParam);

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define BEGINTHREADEX(psa, cbStack, pfnStartAddr, \
   pvParam, fdwCreate, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                      \
         (void *)        (psa),                     \
         (unsigned)      (cbStack),                 \
         (PTHREAD_START) (pfnStartAddr),            \
         (void *)        (pvParam),                 \
         (unsigned)      (fdwCreate),               \
         (unsigned *)    (pdwThreadId)))




NetworkController::NetworkController()
{
	m_hCloseSend					= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hCloseRecv					= CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hSendCheck					= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hRecvCheck					= CreateEvent(NULL, FALSE, FALSE, NULL);

	hProcEventPacket				= CreateEvent(NULL, FALSE, FALSE, NULL);

	m_Sock							= INVALID_SOCKET;
	m_bConnectServer				= false;
	m_bThreadSend					= false;
	m_bThreadRecv					= false;

	m_bExitCall						= false;

	pNetWork						= this;

	// ��Ʈ�� ���� ��ü ����
	PerIoContext* pSendPerIoCtx		= NULL;
	PerIoContext* pRecvPerIoCtx		= NULL;
		
	pServerCtx						= new PerSocketContext;
	pRecvPerIoCtx					= new PerIoContext;
	pSendPerIoCtx					= new PerIoContext;
	pServerCtx->Send_queue			= new List<Packet*>();
	pServerCtx->Recv_queue			= new List<Packet*>();
	pServerCtx->socket				= INVALID_SOCKET;
	
	pSendPerIoCtx->wsaBuf.buf		= pSendPerIoCtx->Buffer;
	pSendPerIoCtx->wsaBuf.len		= MAX_BUFFER;
	pSendPerIoCtx->BytesTransferred	= 0;
	pServerCtx->sendContext			= pSendPerIoCtx;
	
	pRecvPerIoCtx->wsaBuf.buf		= pRecvPerIoCtx->Buffer;
	pRecvPerIoCtx->wsaBuf.len		= MAX_BUFFER;	
	pRecvPerIoCtx->BytesTransferred = 0;
	pServerCtx->recvContext			= pRecvPerIoCtx;	

	InitializeCriticalSection(&pServerCtx->cs_Send);
	InitializeCriticalSection(&pServerCtx->cs_Recv);
	

	WSADATA	wsaData;	
	WSAStartup(WSVERS, &wsaData);

	InitializeCriticalSection( &m_cs );
}


NetworkController::~NetworkController(void)
{	
	pNetWork = NULL;

	CloseHandle(m_hCloseSend);
	CloseHandle(m_hCloseRecv);
	
	CloseHandle(m_hSendCheck);
	CloseHandle(m_hRecvCheck);

	CloseHandle(hProcEventPacket);

	// �Ҵ���� ��Ű��� ����ü, ��ü ����
	if(pServerCtx != NULL)
	{
		if(pServerCtx->Send_queue)
		{		
			pServerCtx->Send_queue->RemoveAll(true);
			SAFE_DELETE(pServerCtx->Send_queue);		
		}

		if(pServerCtx->Recv_queue)
		{		
			pServerCtx->Recv_queue->RemoveAll(true);
			SAFE_DELETE(pServerCtx->Recv_queue);		
		}
		
		DeleteCriticalSection(&pServerCtx->cs_Send);
		DeleteCriticalSection(&pServerCtx->cs_Recv);

		SAFE_DELETE(pServerCtx->recvContext);		
		SAFE_DELETE(pServerCtx->sendContext);		
		SAFE_DELETE(pServerCtx);					
	}


	WSACleanup();
	DeleteCriticalSection(&m_cs);
}



void NetworkController::PrepareResponse(DWORD cbTransferred)
{
	bool bDone = false;
	DWORD pProcessPoint = 0;
	DWORD pProcessPointOffset = 0;
	Packet* pPacket;

	// ��Ŷ�� ���������� ���۹޾Ҵ��� üũ	
	EnterCriticalSection(&pServerCtx->cs_Recv);

	if(cbTransferred == 0)
	{
		if(!m_bExitCall)
		{
			pPacket = new Packet();
			pPacket->Pack(CLOSE_CLIENT);
			RecvQueuePut(pPacket);
		}
		LeaveCriticalSection(&pServerCtx->cs_Recv);	
		return;
	}

	pServerCtx->recvContext->BytesTransferred += cbTransferred;

	try
	{
		if(pServerCtx->recvContext->BytesTransferred >= MMSV_PACKET_BASE_SIZE)
		{
			while(!bDone)
			{
				while(1)
				{
					pProcessPoint = GetMessageSize(pServerCtx->recvContext->Buffer, pServerCtx->recvContext->BytesTransferred, pProcessPointOffset);
					
					PHEAD pHead = (PHEAD)pServerCtx->recvContext->Buffer;
					if(pProcessPoint == 0 || pHead->ps == pProcessPoint)
						break;
					else
					{
						pProcessPointOffset = pProcessPoint;
						pProcessPoint = 0;
					}
				}
				pProcessPointOffset = 0;

				// �޽����� ���� ��������
				if(pProcessPoint == 0)
					bDone = true;

				// �ϳ��� ��Ŷ�� ���������� ���۹޾�����
				else if(pProcessPoint == pServerCtx->recvContext->BytesTransferred)
				{						
					pPacket = new Packet();
					if(pPacket->Unpack(pServerCtx->recvContext->Buffer, pProcessPoint) == 0)
						RecvQueuePut(pPacket);

					// ���� ���� ���� ��ġ�� ó������ ����					
					ZeroMemory(pServerCtx->recvContext->Buffer, pServerCtx->recvContext->BytesTransferred);
					pServerCtx->recvContext->BytesTransferred = 0;

					bDone = true;
				}

				// ��Ŷ�� �پ ������ ���
				else if(pProcessPoint < pServerCtx->recvContext->BytesTransferred)
				{		
					pPacket = new Packet();
					if(pPacket->Unpack(pServerCtx->recvContext->Buffer, pProcessPoint) == 0)
						RecvQueuePut(pPacket);
					
					// ���� ���� ���� ��ġ�� ó������ ����
					char* pBuffer = new char[MAX_BUFFER];
					CopyMemory(	pBuffer, pServerCtx->recvContext->Buffer + pProcessPoint, 
								pServerCtx->recvContext->BytesTransferred - pProcessPoint);
					ZeroMemory( pServerCtx->recvContext->Buffer, sizeof(char)*MAX_BUFFER);
					CopyMemory(	pServerCtx->recvContext->Buffer, pBuffer, 
								pServerCtx->recvContext->BytesTransferred - pProcessPoint);
					delete pBuffer;
					pServerCtx->recvContext->BytesTransferred -= pProcessPoint;				
					bDone = false;
				}		
			}	
		}
	}
	catch(...)
	{
	}

	LeaveCriticalSection(&pServerCtx->cs_Recv);	

	// ���� ���� ��û
	SetEvent(m_hRecvCheck);	
}

void NetworkController::RecvData(Packet* pPacket)
{
}

///////////////////////////////////////////////////////////////////////////
// ó���ؾ��� ��Ŷ ��ġ ����
///////////////////////////////////////////////////////////////////////////
DWORD NetworkController::GetMessageSize(const char* pBuffer, DWORD cbTransferred, DWORD cbOffset/* = 0*/) 
{
   for(DWORD nIndex = cbOffset; nIndex < cbTransferred; ++nIndex)
   {
      if(pBuffer[nIndex] == 'E')
      {
         if(nIndex + 1 < cbTransferred && pBuffer[nIndex + 1] == 'P')
         {
			 if(nIndex + 1 + 1< cbTransferred && pBuffer[nIndex + 2] == 'C')
		 	 {
				return nIndex + 1 + 1 + 1 + 1; 
			 }
         }
      }
   }
   return 0;
}


// Ŭ���̾�Ʈ�� ������ ����
// tcp�� udp�� ��찡 �ٸ���
BOOL NetworkController::MMSocketCreate()		
{
	m_Sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if(m_Sock == INVALID_SOCKET)
	{		
		char szTemp[100];
		sprintf_s(szTemp, 100, "Failed in Socket Creation: %d", WSAGetLastError());
		MessageBox(NULL, szTemp, "Socket Creation", MB_OK|MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}



// ������ ���� ����, udp�� ���� �ʿ����
BOOL NetworkController::MMSocketConnectTCP()
{
	// �������ῡ ���õ� ��� ���� ����
	MMServerFree();	

	// ������ ����
	MMSocketCreate();	
	
	// ���� ���ε� ���� ����, ����
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(m_nServerPort);		
	m_addr.sin_addr.S_un.S_addr = inet_addr(m_pServerIp);
		
	// 3ȸ ����õ�
	for(int nIndex = 0; nIndex < 3; nIndex++)
	{
		// ������ ����!
		int nRet = WSAConnect(m_Sock,
							 (struct sockaddr FAR *)&m_addr, 
							 sizeof(struct sockaddr),
							 NULL, 
							 NULL, 
							 NULL, 
							 NULL
							);
		
		if(nRet == SOCKET_ERROR)
		{
			m_bConnectServer = false;
			if(nIndex == 2) return false;
		}
		else 
			break;
	}

	closesocket(pServerCtx->socket);				
	pServerCtx->socket = m_Sock;

	// ��Ʈ�� ���������� ����
	m_bConnectServer = true;


	// Thread Pool �ʱ�ȭ
	if(!MMCreateThreadPool())
	{
		return FALSE;
	}	
	else
	{
		// �ʱ� ��/���� �ɾ���
		SetEvent(m_hRecvCheck);	
		SetEvent(m_hSendCheck);	
	}

	return TRUE;
}


// ���ϰ��� �ʱ�ȭ(���� ������ �����ϸ� ����ó��)
void NetworkController::MMSocketClose()
{
	__try
	{
		EnterCriticalSection(&m_cs);
		
		if(m_Sock != INVALID_SOCKET && pServerCtx != NULL)
		{
			
			LINGER LingerStruct;
			LingerStruct.l_onoff = 1;
			LingerStruct.l_linger = 0;

			if(pServerCtx->socket != INVALID_SOCKET) 
			{
				setsockopt(pServerCtx->socket, SOL_SOCKET,SO_LINGER, (char*)&LingerStruct, sizeof(LingerStruct));
			}

			closesocket(pServerCtx->socket);
			pServerCtx->socket	= INVALID_SOCKET;
			m_Sock				= INVALID_SOCKET;
		}
	}		
	__finally	
	{			
		LeaveCriticalSection(&m_cs);
	}
}





// ���� �������� ��Ʈ�� ���� ��� ����
void NetworkController::MMServerFree()
{	
	if(m_bConnectServer)
	{		
		SetEvent(m_hCloseRecv);	
		SetEvent(m_hCloseSend);	
		SetEvent(hProcEventPacket);	

		while(1)
		{			
			if(!m_bThreadSend && !m_bThreadRecv) break;
			else Sleep(30);
		}
	}

	pServerCtx->Send_queue->RemoveAll(true);
	pServerCtx->Recv_queue->RemoveAll(true);

	// ����� ������ �����ϸ� ���� ����
	MMSocketClose();			

	// ���� ����� ����
	m_bConnectServer = false;	
}




// Send ������ ���� ����Ʈ�μ����� ��Ȱ
DWORD __stdcall WorkerThreadStartingPointSend(PVOID pvParam)
{
	NetworkController* piProcessThread = (NetworkController*)pvParam;
	piProcessThread->ProcessingSendThread();
	return 0;
}

// Recv ������ ���� ����Ʈ�μ����� ��Ȱ
DWORD __stdcall WorkerThreadStartingPointRecv(PVOID pvParam)
{
	NetworkController* piProcessThread = (NetworkController*)pvParam;
	piProcessThread->ProcessingRecvThread();
	return 0;
}




// Worker Thread Ǯ�� ����
BOOL NetworkController::MMCreateThreadPool(unsigned int nNumOfPooledThread)
{
	int m_CreatedThreadNumber;
	DWORD dwThreadIdSend = 0;	
	DWORD dummy;

	HANDLE	hRecvTrans;	

	if(nNumOfPooledThread == 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		m_CreatedThreadNumber = si.dwNumberOfProcessors * 2 + 2;
	}
	else
	{
		m_CreatedThreadNumber = nNumOfPooledThread;
	}

	hRecvTrans = (HANDLE)_beginthreadex(NULL, 0, ProcRecvTrans, this, 0, (unsigned int *)&dummy);
	CloseHandle(hRecvTrans);

	// �۽� ������	
	if(!m_bThreadSend) CloseHandle(BEGINTHREADEX(NULL, 0, WorkerThreadStartingPointSend, this, 0, &dwThreadIdSend));
	// ���� ������
	if(!m_bThreadRecv) CloseHandle(BEGINTHREADEX(NULL, 0, WorkerThreadStartingPointRecv, this, 0, &dwThreadIdSend));
	
	return TRUE;
}



// ��������� �����ð����� �����尡 ������� ������ ������ ��������
void NetworkController::MMForceThreadKill(HANDLE hThread)
{
	DWORD dwExitCode;
	
	// �������� ���� �ڵ带 Ȯ���ؼ� �����尡 ��� �ִ��� Ȯ��
	GetExitCodeThread(hThread, &dwExitCode);
	
	if(dwExitCode == STILL_ACTIVE)
		TerminateThread(hThread , 0);
}

// �۽� �̺�Ʈ ó�� ������
void NetworkController::ProcessingSendThread(void)
{	
	DWORD nIndex = 0;
	HANDLE	EventArray[2];	
	EventArray[0] = m_hCloseSend;	
	EventArray[1] = m_hSendCheck;			
	
	__try		
	{
		m_bThreadSend = true;

		while(true)	
		{
			nIndex = WSAWaitForMultipleEvents(2, EventArray, FALSE, WSA_INFINITE, TRUE);
			
			if(nIndex == WSA_WAIT_EVENT_0)			// ���� ó��
			{				
				__leave;				
			}
			else if(nIndex == WSA_WAIT_EVENT_0 + 1)	// ���ۿϷ�
			{
				MMSendPost();
			}
			else if(nIndex == WAIT_IO_COMPLETION)
			{
				continue;
			}
		}
	}
	__finally	
	{		
		m_bThreadSend = false;
	}	
}

// ���� �̺�Ʈ ó�� ������
void NetworkController::ProcessingRecvThread(void)
{	
	DWORD			nIndex = 0;
	HANDLE			EventArray[2];	
	EventArray[0]	= m_hCloseRecv;	
	EventArray[1]	= m_hRecvCheck;	
	
	__try		
	{
		// ������ ����
		m_bThreadRecv = false;

		while(true)	
		{
			nIndex = WSAWaitForMultipleEvents(2, EventArray, FALSE, WSA_INFINITE, TRUE);
			
			if(nIndex == WSA_WAIT_EVENT_0)			// ���� ó��
			{				
				__leave;				
			}
			else if(nIndex == WSA_WAIT_EVENT_0 + 1)	// �ʱ� ����
			{
				MMRecvPost();
			}
			else if(nIndex == WAIT_IO_COMPLETION)
			{
				continue;
			}
		}
	}
	__finally	
	{		
		m_bThreadRecv = false;
	}	
}


// Send �������� üũ
BOOL NetworkController::MMCheckSend(Packet* packet)
{
	if(m_Sock == INVALID_SOCKET) 
		return false; 

	if(!m_bConnectServer) 
	{
		SAFE_DELETE(packet);
		return false;
	}
					
	EnterCriticalSection(&pServerCtx->cs_Send);
	pServerCtx->Send_queue->AddTail(packet);			
	LeaveCriticalSection(&pServerCtx->cs_Send);

	SetEvent(m_hSendCheck);
	return true;
}



// Send ��û
BOOL NetworkController::MMSendPost()
{
	__try
	{
		DWORD dwSendBytes=0;
		DWORD dwFlags=0;	

		if(pServerCtx != NULL && m_Sock != INVALID_SOCKET && m_bConnectServer)
		{
			EnterCriticalSection(&pServerCtx->cs_Send);
			
			if(pServerCtx->Send_queue->GetCount() <= 0)
			{
				LeaveCriticalSection(&pServerCtx->cs_Send);
				return true;
			}

			Packet* packet = (Packet*) pServerCtx->Send_queue->GetAt2(0);

			if(packet == NULL)
			{
				LeaveCriticalSection(&pServerCtx->cs_Send);
				return true;
			}

			pServerCtx->Send_queue->RemoveAt(0);

			if(m_bConnectServer) 
			{
				ZeroMemory(pServerCtx->sendContext->Buffer, sizeof(char)*MAX_BUFFER);
				CopyMemory(pServerCtx->sendContext->Buffer, packet->GetPacket(), (size_t)packet->GetLength());	
				pServerCtx->sendContext->wsaBuf.len = packet->GetLength();					
				ZeroMemory(&pServerCtx->sendContext->overlapped,sizeof(WSAOVERLAPPED));


				pServerCtx->sendContext->overlapped.hEvent = (HANDLE)0;
				int ret = WSASend(	pServerCtx->socket,
									&(pServerCtx->sendContext->wsaBuf),
									1,
									&dwSendBytes,
									dwFlags,
									&(pServerCtx->sendContext->overlapped),
									(LPWSAOVERLAPPED_COMPLETION_ROUTINE) SendCompletedTCP);

				if(SOCKET_ERROR==ret)
				{
					int ErrCode=WSAGetLastError();
					if(ErrCode!=WSA_IO_PENDING)
					{
						LeaveCriticalSection(&pServerCtx->cs_Send);
						return FALSE;
					}
				}		
			}

			SAFE_DELETE(packet);

			LeaveCriticalSection(&pServerCtx->cs_Send);
		}
		return TRUE;
	}
	__finally
	{			
	}
}

 

// RECV ��û
BOOL NetworkController::MMRecvPost()
{
	__try
	{
		DWORD dwRecvBytes	= 0;
		DWORD dwFlags		= 0;

		if(m_Sock == INVALID_SOCKET)
			return false;

		pServerCtx->recvContext->wsaBuf.buf = pServerCtx->recvContext->Buffer + pServerCtx->recvContext->BytesTransferred;
		pServerCtx->recvContext->wsaBuf.len = MAX_BUFFER - pServerCtx->recvContext->BytesTransferred;

		ZeroMemory(&pServerCtx->recvContext->overlapped, sizeof(WSAOVERLAPPED));

		int ret = WSARecv(	pServerCtx->socket, 
							&(pServerCtx->recvContext->wsaBuf), 
							1,
							&dwRecvBytes, 
							&dwFlags, 
							&(pServerCtx->recvContext->overlapped),
							(LPWSAOVERLAPPED_COMPLETION_ROUTINE)RecvCompletedTCP);

		if(SOCKET_ERROR == ret)
		{
			int ErrCode = WSAGetLastError();			
			if(ErrCode != WSA_IO_PENDING)
			{	
				return FALSE;
			}
		}
		return TRUE;
	}
	__finally
	{
	}

	return true;
}





bool NetworkController::RecvQueuePut(Packet* pPacket)
{
	try
	{
		EnterCriticalSection(&pServerCtx->cs_Recv);
		pServerCtx->Recv_queue->AddTail(pPacket);	
		LeaveCriticalSection(&pServerCtx->cs_Recv);
	}
	catch(...)
	{
		LeaveCriticalSection(&pServerCtx->cs_Recv);
		return false;
	}

	return true;
}

Packet* NetworkController::RecvQueueGet()
{	
	Packet* pPacket = NULL;
	EnterCriticalSection(&pServerCtx->cs_Recv);
	if(pServerCtx->Recv_queue->GetCount() > 0) 
	{
		pPacket = pServerCtx->Recv_queue->GetAt2(0);
		pServerCtx->Recv_queue->RemoveAt(0);
	}
	LeaveCriticalSection(&pServerCtx->cs_Recv);

	return pPacket;
}


void NetworkController::RecvPacketLoop()
{
	Packet* pPacket;
	DWORD	dwResult;

	while(1)
	{
		dwResult = WaitForSingleObject(hProcEventPacket, 100);
		if(dwResult == WAIT_OBJECT_0) 
		{
			break;
		}

		while(1)
		{
			try
			{
				pPacket = RecvQueueGet();
				if(pPacket == NULL) 
				{
					break;		
				}
				
				RecvData(pPacket);
				
			}
			catch(...)
			{	
				break;
			}
		}
	}
}



unsigned int __stdcall ProcRecvTrans(void *pParam)
{
	NetworkController *pNetWork = (NetworkController*)pParam;
	pNetWork->RecvPacketLoop();
	return 1;
}



// callback functions...
// ���� �߿��� �ۼ��� callback�Լ�
// tcp ���� �Ϸ�� os �� ���� ȣ��Ǵ� callback �Լ�
void CALLBACK RecvCompletedTCP(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{			
	__try		
	{			
		if(pNetWork)
		{
			// ����üũ
			if(dwError != 0)	
			{
				// �������� ������ ������ ������
				if(dwError == 10053 || dwError == 10054)
					pNetWork->PrepareResponse(0);
				return;
			}

			// �������� üũ
			if(cbTransferred == 0)
			{
				pNetWork->PrepareResponse(0);
				return;
			}

			// ������� ���� ���¿��� �ݹ��Լ� ȣ��� ���
			if(!pNetWork->m_bConnectServer) 
			{
				return;
			}
			
			// ��Ŷó��
			pNetWork->PrepareResponse(cbTransferred);			
		}
	}
	__finally
	{
	}
}



// TCP �۽� ó���Ǵ� �Լ�. �۽� �Ϸᰡ �Ǹ� OS�� ���� �Ҹ���.
void CALLBACK SendCompletedTCP(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{			
	__try		
	{		
		if(pNetWork)
		{
			// ����üũ
			if(dwError != 0)	
			{
				// �������� ������ ������ ������
				if(dwError == 10053 || dwError == 10054)
				{
				}
				return;
			}

			// ����� ���°� �ƴϸ� ó������ ����
			if(!pNetWork->m_bConnectServer) 
			{
				return;
			}

			// ���۽����̸� ����
			if(cbTransferred == 0)
			{
				return;
			}			
				
			// ��ü�Ǿ� �ִ� Send ��û�� �ִ��� üũ
			SetEvent(pNetWork->m_hSendCheck);
		}
	}
	__finally
	{			
	}
}

