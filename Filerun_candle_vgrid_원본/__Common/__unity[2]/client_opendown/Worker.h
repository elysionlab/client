#pragma once
#include "NetworkController.h"


class COpenDown_clientDlg;
class Worker : public NetworkController
{


public:

	Worker(COpenDown_clientDlg* pMainWnd);
	~Worker(void);

	bool Init(char* pServerIp, unsigned int nServerPort);
	void End();


public:
	
	CRITICAL_SECTION	m_pSendSection;				// ����ȭ�� ���� ���� (��Ʈ�� �ۼ��Ű���)
	

	int		m_nReConnectCount;
	bool	m_bReConnectTrans;
	
	



public:

	
	void SendPacket(Packet* packet);
	void RecvData(Packet* pPacket);	



	//////////////////////////////////////////////////////////////////// �Ϲ� �Լ�

	// �α��� ���� ������ ó��
	void Result_Login(Packet* pPacket);		



	//////////////////////////////////////////////////////////////////// �ٿ�ε� ������ �߰����� �Լ�
	// �ٿ�ε� ��� �ޱ�
	void Result_OpenDownLoadList(Packet* pPacket);


	//////////////////////////////////////////////////////////////////// �ٿ�ε� ���� �Լ�
	// �ٿ�ε带 ���� �ٿ�ε� ���� ���� ��û
	void Result_OpenDownLoadPro(Packet* pPacket);
	// �ٿ�ε带 ���� �ٿ�ε� ���� ���� ��û
	//void Result_OpenDownLoadEnd(Packet* pPacket);

	// �ٸ� ��ǻ�Ϳ��� �ߺ� ����
	//void Result_Dupl_Close(Packet* pPacket);



	//////////////////////////////////////////////////////////////////// ���ڿ� ó��  

	// ����/���� �̸�/���� üũ(Ư������, ")
	void SpecialText(CString& text);
	// \ ����
	void SlashAdd(CString& text);
	// �޸� ����
	void AddComma(CString& text, __int64 size);

};
