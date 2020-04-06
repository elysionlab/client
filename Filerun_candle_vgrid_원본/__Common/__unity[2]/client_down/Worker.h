#pragma once
#include "NetworkController.h"


class down_window;
class Worker : public NetworkController
{


public:

	Worker(down_window* pMainWnd);
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
	// ���۹��� ������ ��� �߰�
	void Result_Item_Add(Packet* pPacket);


	//////////////////////////////////////////////////////////////////// �ٿ�ε� ���� �Լ�

	// �ٿ�ε带 �����ϱ� ���� �ش� ������ ���� �� �ٿ�ε� ���� ���� ��������
	void Result_DownLoadPre(Packet* pPacket);
	// �ٿ�ε带 ���� �ٿ�ε� ���� ���� ��û
	void Result_DownLoadPro(Packet* pPacket);
	// �ٿ�ε带 ���� �ٿ�ε� ���� ���� ��û
	void Result_DownLoadEnd(Packet* pPacket);

	// �ٸ� ��ǻ�Ϳ��� �ߺ� ����
	void Result_Dupl_Close(Packet* pPacket);



	//////////////////////////////////////////////////////////////////// ���ڿ� ó��  

	// ����/���� �̸�/���� üũ(Ư������, ")
	void SpecialText(CString& text);
	// \ ����
	void SlashAdd(CString& text);
	// �޸� ����
	void AddComma(CString& text, __int64 size);

	CString StrToCString(char* pData, int nCnt);

	
	//�·�ī ����üũ
	bool Mureka_files_check();

	//�·�ī ����üũ
	void iMBC_Logsend_history_Save(__int64 BoardIndex,__int64 FileIndex,CString logvalue);

};
