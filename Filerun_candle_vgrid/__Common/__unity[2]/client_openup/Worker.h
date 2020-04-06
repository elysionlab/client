#pragma once
#include "NetworkController.h"


class COpenUp_clientDlg;
class Worker : public NetworkController
{


public:

	Worker(COpenUp_clientDlg* pMainWnd);
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



	//////////////////////////////////////////////////////////////////// ���ε� ������ �߰����� �Լ�
	// ���۹��� ������ ��� �߰�
	void Result_OpenUpLoadList(Packet* pPacket);
	//////////////////////////////////////////////////////////////////// ���ε� ���� �Լ�
	// ���ε� ó������
	void Result_OpenUpLoadPro(Packet* pPacket);
	
	
	//////////////////////////////////////////////////////////////////// ���ڿ� ó��  

	// ����/���� �̸�/���� üũ(Ư������, ")
	void SpecialText(CString& text);
	// \ ����
	void SlashAdd(CString& text);
	// �޸� ����
	void AddComma(CString& text, __int64 size);

};
