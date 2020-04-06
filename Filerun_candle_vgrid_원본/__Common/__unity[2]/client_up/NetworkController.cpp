#include "StdAfx.h"
#include "Networkcontroller.h"
#include "up_window.h"
//#include "Resource.h"		//20080424 vs8로 컨버팅으로 인한 수정, jyh
#include <process.h>
#include <assert.h>

NetworkController* pNetWork;


// 워커 쓰레드 시작 포인트로서만의 역활
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

	// 네트웍 관련 객체 생성
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

	// 할당받은 통신관련 구조체, 객체 해제
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

	// 패킷의 마지막까지 전송받았는지 체크	
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

				// 메시지를 받지 못했을때
				if(pProcessPoint == 0)
					bDone = true;

				// 하나의 패킷을 정상적으로 전송받았을때
				else if(pProcessPoint == pServerCtx->recvContext->BytesTransferred)
				{						
					pPacket = new Packet();
					if(pPacket->Unpack(pServerCtx->recvContext->Buffer, pProcessPoint) == 0)
						RecvQueuePut(pPacket);

					// 다음 전송 받을 위치를 처음으로 지정					
					ZeroMemory(pServerCtx->recvContext->Buffer, pServerCtx->recvContext->BytesTransferred);
					pServerCtx->recvContext->BytesTransferred = 0;

					bDone = true;
				}

				// 패킷이 붙어서 들어왔을 경우
				else if(pProcessPoint < pServerCtx->recvContext->BytesTransferred)
				{		
					pPacket = new Packet();
					if(pPacket->Unpack(pServerCtx->recvContext->Buffer, pProcessPoint) == 0)
						RecvQueuePut(pPacket);
					
					// 다음 전송 받을 위치를 처음으로 지정
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

	// 다음 수신 요청
	SetEvent(m_hRecvCheck);	
}

void NetworkController::RecvData(Packet* pPacket)
{
}

///////////////////////////////////////////////////////////////////////////
// 처리해야할 패킷 위치 지정
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


// 클라이언트의 소켓을 생성
// tcp와 udp의 경우가 다르게
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



// 서버에 실제 연결, udp의 경우는 필요없슴
BOOL NetworkController::MMSocketConnectTCP()
{
	// 이전연결에 관련된 모든 정보 제거
	MMServerFree();	

	// 소켓을 생성
	MMSocketCreate();	
	
	// 서버 바인딩 정보 추출, 생성
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(m_nServerPort);		
	m_addr.sin_addr.S_un.S_addr = inet_addr(m_pServerIp);
		
	// 3회 연결시도
	for(int nIndex = 0; nIndex < 3; nIndex++)
	{
		// 서버에 연결!
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

	// 네트웍 연결중으로 설정
	m_bConnectServer = true;


	// Thread Pool 초기화
	if(!MMCreateThreadPool())
	{
		return FALSE;
	}	
	else
	{
		// 초기 송/수신 걸어줌
		SetEvent(m_hRecvCheck);	
		SetEvent(m_hSendCheck);	
	}

	return TRUE;
}


// 소켓관련 초기화(이전 연결이 존재하면 종료처리)
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





// 현재 연결중인 네트웍 관련 모두 해제
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

	// 연결된 소켓이 존재하면 연결 종료
	MMSocketClose();			

	// 연결 종료로 설정
	m_bConnectServer = false;	
}




// Send 쓰레드 시작 포인트로서만의 역활
DWORD __stdcall WorkerThreadStartingPointSend(PVOID pvParam)
{
	NetworkController* piProcessThread = (NetworkController*)pvParam;
	piProcessThread->ProcessingSendThread();
	return 0;
}

// Recv 쓰레드 시작 포인트로서만의 역활
DWORD __stdcall WorkerThreadStartingPointRecv(PVOID pvParam)
{
	NetworkController* piProcessThread = (NetworkController*)pvParam;
	piProcessThread->ProcessingRecvThread();
	return 0;
}




// Worker Thread 풀을 만듬
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

	// 송신 쓰레드	
	if(!m_bThreadSend) CloseHandle(BEGINTHREADEX(NULL, 0, WorkerThreadStartingPointSend, this, 0, &dwThreadIdSend));
	// 수신 쓰레드
	if(!m_bThreadRecv) CloseHandle(BEGINTHREADEX(NULL, 0, WorkerThreadStartingPointRecv, this, 0, &dwThreadIdSend));
	
	return TRUE;
}



// 연결종료시 일정시간동안 쓰레드가 종료되지 않으면 쓰레드 강제종료
void NetworkController::MMForceThreadKill(HANDLE hThread)
{
	DWORD dwExitCode;
	
	// 쓰레드의 종료 코드를 확인해서 쓰레드가 살아 있는지 확인
	GetExitCodeThread(hThread, &dwExitCode);
	
	if(dwExitCode == STILL_ACTIVE)
		TerminateThread(hThread , 0);
}

// 송신 이벤트 처리 쓰레드
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
			
			if(nIndex == WSA_WAIT_EVENT_0)			// 종료 처리
			{				
				__leave;				
			}
			else if(nIndex == WSA_WAIT_EVENT_0 + 1)	// 전송완료
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

// 수신 이벤트 처리 쓰레드
void NetworkController::ProcessingRecvThread(void)
{	
	DWORD			nIndex = 0;
	HANDLE			EventArray[2];	
	EventArray[0]	= m_hCloseRecv;	
	EventArray[1]	= m_hRecvCheck;	
	
	__try		
	{
		// 쓰레드 시작
		m_bThreadRecv = false;

		while(true)	
		{
			nIndex = WSAWaitForMultipleEvents(2, EventArray, FALSE, WSA_INFINITE, TRUE);
			
			if(nIndex == WSA_WAIT_EVENT_0)			// 종료 처리
			{				
				__leave;				
			}
			else if(nIndex == WSA_WAIT_EVENT_0 + 1)	// 초기 수신
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


// Send 가능한지 체크
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



// Send 요청
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

 

// RECV 요청
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
// 가장 중요한 송수신 callback함수
// tcp 수신 완료시 os 에 의해 호출되는 callback 함수
void CALLBACK RecvCompletedTCP(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{			
	__try		
	{			
		if(pNetWork)
		{
			// 에러체크
			if(dwError != 0)	
			{
				// 서버에서 강제로 연결을 종료함
				if(dwError == 10053 || dwError == 10054)
					pNetWork->PrepareResponse(0);
				return;
			}

			// 연결종료 체크
			if(cbTransferred == 0)
			{
				pNetWork->PrepareResponse(0);
				return;
			}

			// 연결되지 않은 상태에서 콜백함수 호출된 경우
			if(!pNetWork->m_bConnectServer) 
			{
				return;
			}
			
			// 패킷처리
			pNetWork->PrepareResponse(cbTransferred);			
		}
	}
	__finally
	{
	}
}



// TCP 송신 처리되는 함수. 송신 완료가 되면 OS에 의해 불린다.
void CALLBACK SendCompletedTCP(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{			
	__try		
	{		
		if(pNetWork)
		{
			// 에러체크
			if(dwError != 0)	
			{
				// 서버에서 강제로 연결을 종료함
				if(dwError == 10053 || dwError == 10054)
				{
				}
				return;
			}

			// 연결된 상태가 아니면 처리하지 않음
			if(!pNetWork->m_bConnectServer) 
			{
				return;
			}

			// 전송실패이면 리턴
			if(cbTransferred == 0)
			{
				return;
			}			
				
			// 적체되어 있는 Send 요청이 있는지 체크
			SetEvent(pNetWork->m_hSendCheck);
		}
	}
	__finally
	{			
	}
}

