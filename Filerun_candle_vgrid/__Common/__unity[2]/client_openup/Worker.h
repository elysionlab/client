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
	
	CRITICAL_SECTION	m_pSendSection;				// 동기화를 위한 변수 (네트웍 송수신관련)

	int		m_nReConnectCount;
	bool	m_bReConnectTrans;


public:

	
	void SendPacket(Packet* packet);
	void RecvData(Packet* pPacket);	



	//////////////////////////////////////////////////////////////////// 일반 함수

	// 로그인 관련 데이터 처리
	void Result_Login(Packet* pPacket);		



	//////////////////////////////////////////////////////////////////// 업로드 아이템 추가관련 함수
	// 전송받은 아이템 목록 추가
	void Result_OpenUpLoadList(Packet* pPacket);
	//////////////////////////////////////////////////////////////////// 업로드 관련 함수
	// 업로드 처리과정
	void Result_OpenUpLoadPro(Packet* pPacket);
	
	
	//////////////////////////////////////////////////////////////////// 문자열 처리  

	// 폴더/파일 이름/정보 체크(특수문자, ")
	void SpecialText(CString& text);
	// \ 삽입
	void SlashAdd(CString& text);
	// 콤마 삽입
	void AddComma(CString& text, __int64 size);

};
