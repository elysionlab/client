#pragma once
#include "NetworkController.h"


class up_window;
class Worker : public NetworkController
{


public:

	Worker(up_window* pMainWnd);
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
	void Result_Item_Add(Packet* pPacket);



	//////////////////////////////////////////////////////////////////// ���ε� ���� �Լ�
	//20071210 ���ε��� �ؽð� �� ��û ���, jyh
	void Result_UpHash(Packet* pPacket);
	//20071211 �ؽð� ��� �� ������ ����Ʈ ��û ���, jyh
	//void Result_UpHashList(Packet* pPacket);
	// ���ε� ��ó�� ����
	void Result_UpLoadPre(Packet* pPacket);
	// ���ε� ó������
	void Result_UpLoadPro(Packet* pPacket);
	// ���ε� �Ϸ� �뺸 ���
	void Result_UpLoadEnd(Packet* pPacket);
	//20090303 ������ ���ε� �Ϸ�� �뺸 ����, jyh
	void Result_UpLoadContentsComplete(Packet* pPacket);

	// �ٸ� ��ǻ�Ϳ��� �ߺ� ����
	void Result_Dupl_Close(Packet* pPacket);


	//////////////////////////////////////////////////////////////////// ���ڿ� ó��  

	// ����/���� �̸�/���� üũ(Ư������, ")
	void SpecialText(CString& text);
	// \ ����
	void SlashAdd(CString& text);
	// �޸� ����
	void AddComma(CString& text, __int64 size);

	//�·�ī��
	CString StrToCString(char* pData, int nCnt);
	
	//�·�ī ����üũ
	bool Mureka_files_check();

	//////////////////////////////////////////////////////////////////// KTH_OSP
	//�ؽø� ����
	CString KTH_OSP_hash_send(char* pData,__int64 BoardIndex,__int64 FileIndex);

	//���̷��� üũ
	int Viruschecking(CString strFile);

	//�������� üũ
//	int MovieBlockchecking(CString strFile);

private:
	HINSTANCE Mureka_dll;

};
