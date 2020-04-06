#pragma once
#include "winsock2.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\Packet.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\Token.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\List.h"
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
	DWORD			BytesTransferred;	// ������� ���ŵ� ����Ʈ
	
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
	
	// ���ϻ���
	BOOL MMSocketCreate();								
	// ������ ����
	BOOL MMSocketConnectTCP();			
	// ���ϰ��� �ʱ�ȭ(���� ������ �����ϸ� ����ó��)
	void MMSocketClose();	
	// ���� �������� ��Ʈ�� ���� ��� ����
	void MMServerFree(void);
    
	// RECV ��û
	BOOL MMRecvPost();
	// Send ��û
	BOOL MMSendPost();	
	// Send �������� üũ
	BOOL MMCheckSend(Packet* packet);
    	
	void PrepareResponse(DWORD cbTransferred);
	virtual void RecvData(Packet* pPacket);
	DWORD GetMessageSize(const char* pBuffer, DWORD cbTransferred, DWORD cbOffset = 0);
	
	
	// ������ Ǯ ����
	BOOL MMCreateThreadPool(unsigned int nNumOfPooledThread = 0);	
	// ��������� �����ð����� �����尡 ������� ������ ������ ��������
	void NetworkController::MMForceThreadKill(HANDLE hThread);
	// �۽� �̺�Ʈ ó�� ������
	void ProcessingSendThread(void);	
	// ���� �̺�Ʈ ó�� ������
	void ProcessingRecvThread(void);	
	

	bool RecvQueuePut(Packet* pPacket);
	Packet* RecvQueueGet();

	void RecvPacketLoop();
	friend unsigned int __stdcall ProcRecvTrans(void *pParam);

public:

	bool				m_bExitCall;		// �ܺο��� ���� ��û�� ��������

	COpenDown_clientDlg*	m_pMainWnd;
	//DebugLIst*			m_pDegDlg;
	
	SOCKET				m_Sock;				// ���� ����	
	SOCKADDR_IN			m_addr;				// ���� ����	
	PPerSocketContext	pServerCtx;			// ���� ���ؽ�Ʈ	
	
	bool				m_bConnectServer;	// ���� �÷���
	bool				m_bThreadSend;		// Send Thread Alive
	bool				m_bThreadRecv;		// Recv Thread Alive

	char				m_pServerIp[25];	// ���� ������
	unsigned int		m_nServerPort;		// ���� ��Ʈ

	CRITICAL_SECTION	m_cs;			

	// �̺�Ʈ �ڵ�
	HANDLE				m_hCloseSend;		// Send ������ ����
	HANDLE				m_hCloseRecv;		// Recv ������ ����

	HANDLE				m_hSendCheck;		// ��ü�� �۽� �ɾ���
	HANDLE				m_hRecvCheck;		// ����Ǹ� �ʱ� ���� �ɾ���
	HANDLE				hProcEventPacket;	// ���� üũ ������ 
	
};
