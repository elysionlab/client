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
	
	CRITICAL_SECTION	m_pSendSection;				// 동기화를 위한 변수 (네트웍 송수신관련)
	

	int		m_nReConnectCount;
	bool	m_bReConnectTrans;
	
	



public:

	
	void SendPacket(Packet* packet);
	void RecvData(Packet* pPacket);	



	//////////////////////////////////////////////////////////////////// 일반 함수

	// 로그인 관련 데이터 처리
	void Result_Login(Packet* pPacket);		



	//////////////////////////////////////////////////////////////////// 다운로드 아이템 추가관련 함수
	// 전송받은 아이템 목록 추가
	void Result_Item_Add(Packet* pPacket);


	//////////////////////////////////////////////////////////////////// 다운로드 관련 함수

	// 다운로드를 시작하기 전에 해당 파일의 정보 및 다운로드 서버 정보 가져오기
	void Result_DownLoadPre(Packet* pPacket);
	// 다운로드를 위해 다운로드 서버 정보 요청
	void Result_DownLoadPro(Packet* pPacket);
	// 다운로드를 위해 다운로드 서버 정보 요청
	void Result_DownLoadEnd(Packet* pPacket);

	// 다른 컴퓨터에서 중복 접속
	void Result_Dupl_Close(Packet* pPacket);



	//////////////////////////////////////////////////////////////////// 문자열 처리  

	// 폴더/파일 이름/정보 체크(특수문자, ")
	void SpecialText(CString& text);
	// \ 삽입
	void SlashAdd(CString& text);
	// 콤마 삽입
	void AddComma(CString& text, __int64 size);

	CString StrToCString(char* pData, int nCnt);

	
	//뮤레카 파일체크
	bool Mureka_files_check();

	//뮤레카 파일체크
	void iMBC_Logsend_history_Save(__int64 BoardIndex,__int64 FileIndex,CString logvalue);

};
