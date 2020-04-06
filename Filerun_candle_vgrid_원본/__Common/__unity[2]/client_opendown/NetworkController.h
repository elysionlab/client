#pragma once
#include "winsock2.h"
#include "d:\Project\Nfile관련_vs8\__Common\__unity[2]\Packet.h"
#include "d:\Project\Nfile관련_vs8\__Common\__unity[2]\Token.h"
#include "d:\Project\Nfile관련_vs8\__Common\__unity[2]\List.h"
//#include "DebugLIst.h"


void CALLBACK SendCompletedTCP(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK RecvCompletedTCP(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

#define WSVERS      MAKEWORD(2,2)
#define MAX_BUFFER	1048576	

typedef struct tagPerIoContext
{
	WSAOVERLAPPED	overlapped;
	WSABUF			wsaBuf;			
	char			Buffer[MAX_BUFFER];
	DWORD			BytesTransferred;	// 현재까지 수신된 바이트
	
} PerIoContext, *PPerIoContext;



typedef struct tagPerSocketContext
{
	SOCKET			socket;
	
	PPerIoContext	recvContext;
	PPerIoContext	sendContext;	
	List<Packet*>*	Send_queue;
	List<Packet*>*	Recv_queue;

	CRITICAL_SECTION cs_Send;
	CRITICAL_SECTION cs_Recv;

} PerSocketContext, *PPerSocketContext;




class COpenDown_clientDlg;
class NetworkController
{

public:


	NetworkController(void);
	~NetworkController(void);
	
	// 소켓생셩
	BOOL MMSocketCreate();								
	// 서버와 연결
	BOOL MMSocketConnectTCP();			
	// 소켓관련 초기화(이전 연결이 존재하면 종료처리)
	void MMSocketClose();	
	// 현재 연결중인 네트웍 관련 모두 해제
	void MMServerFree(void);
    
	// RECV 요청
	BOOL MMRecvPost();
	// Send 요청
	BOOL MMSendPost();	
	// Send 가능한지 체크
	BOOL MMCheckSend(Packet* packet);
    	
	void PrepareResponse(DWORD cbTransferred);
	virtual void RecvData(Packet* pPacket);
	DWORD GetMessageSize(const char* pBuffer, DWORD cbTransferred, DWORD cbOffset = 0);
	
	
	// 쓰레드 풀 생성
	BOOL MMCreateThreadPool(unsigned int nNumOfPooledThread = 0);	
	// 연결종료시 일정시간동안 쓰레드가 종료되지 않으면 쓰레드 강제종료
	void NetworkController::MMForceThreadKill(HANDLE hThread);
	// 송신 이벤트 처리 쓰레드
	void ProcessingSendThread(void);	
	// 수신 이벤트 처리 쓰레드
	void ProcessingRecvThread(void);	
	

	bool RecvQueuePut(Packet* pPacket);
	Packet* RecvQueueGet();

	void RecvPacketLoop();
	friend unsigned int __stdcall ProcRecvTrans(void *pParam);

public:

	bool				m_bExitCall;		// 외부에서 종료 요청이 들어왔을때

	COpenDown_clientDlg*	m_pMainWnd;
	//DebugLIst*			m_pDegDlg;
	
	SOCKET				m_Sock;				// 서버 소켓	
	SOCKADDR_IN			m_addr;				// 서버 정보	
	PPerSocketContext	pServerCtx;			// 소켓 컨텍스트	
	
	bool				m_bConnectServer;	// 연결 플래그
	bool				m_bThreadSend;		// Send Thread Alive
	bool				m_bThreadRecv;		// Recv Thread Alive

	char				m_pServerIp[25];	// 서버 아이피
	unsigned int		m_nServerPort;		// 서버 포트

	CRITICAL_SECTION	m_cs;			

	// 이벤트 핸들
	HANDLE				m_hCloseSend;		// Send 쓰레드 종료
	HANDLE				m_hCloseRecv;		// Recv 쓰레드 종료

	HANDLE				m_hSendCheck;		// 적체된 송신 걸어줌
	HANDLE				m_hRecvCheck;		// 연결되면 초기 수신 걸어줌
	HANDLE				hProcEventPacket;	// 수신 체크 쓰레드 
	
};
