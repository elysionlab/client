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
	void Result_Item_Add(Packet* pPacket);



	//////////////////////////////////////////////////////////////////// 업로드 관련 함수
	//20071210 업로드전 해시값 비교 요청 결과, jyh
	void Result_UpHash(Packet* pPacket);
	//20071211 해시값 등록 후 아이템 리스트 요청 결과, jyh
	//void Result_UpHashList(Packet* pPacket);
	// 업로드 전처리 과정
	void Result_UpLoadPre(Packet* pPacket);
	// 업로드 처리과정
	void Result_UpLoadPro(Packet* pPacket);
	// 업로드 완료 통보 결과
	void Result_UpLoadEnd(Packet* pPacket);
	//20090303 컨텐츠 업로드 완료시 통보 받음, jyh
	void Result_UpLoadContentsComplete(Packet* pPacket);

	// 다른 컴퓨터에서 중복 접속
	void Result_Dupl_Close(Packet* pPacket);


	//////////////////////////////////////////////////////////////////// 문자열 처리  

	// 폴더/파일 이름/정보 체크(특수문자, ")
	void SpecialText(CString& text);
	// \ 삽입
	void SlashAdd(CString& text);
	// 콤마 삽입
	void AddComma(CString& text, __int64 size);

	//뮤레카용
	CString StrToCString(char* pData, int nCnt);
	
	//뮤레카 파일체크
	bool Mureka_files_check();

	//////////////////////////////////////////////////////////////////// KTH_OSP
	//해시를 전송
	CString KTH_OSP_hash_send(char* pData,__int64 BoardIndex,__int64 FileIndex);

	//바이러스 체크
	int Viruschecking(CString strFile);

	//유해차단 체크
//	int MovieBlockchecking(CString strFile);

private:
	HINSTANCE Mureka_dll;

};
