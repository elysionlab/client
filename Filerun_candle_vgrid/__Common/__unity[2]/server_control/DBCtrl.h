/************************************************************************************
*
* MYSQL 데이터베이스 관련 모든 작업
*
* - 데이터베이스 쿼리는 별도의 쓰레드를 사용하지 않고 큐에 의해 호출된 ProcessFunc 
*   클래스에서 호출될때 실행
* - 쿼리 실행시 mysql_ping을 이용해 연결 확인
* - 쿼리 후 연결은 종료하지 않고 타임아웃 시간에 연결이 종료됨
*************************************************************************************/

#ifndef __DBCTRL_H__
#define __DBCTRL_H__

#pragma once

#include "mmsv_User.h"
#include "ace/message_block.h"
#include "../DBbase.h"
#include "../mmsv_FtpClient.h"


class DBCtrl : public DBbase
{

public:

	// 기본 생성/소멸자
	DBCtrl(void)
	{
		m_bServiceRun	= true;
		m_tCheckTime	= ACE_OS::gettimeofday();
		m_pFtp			= new mmsv_FtpClient();
		ACE_NEW_NORETURN(m_pFileDB, DBbase());	
	}
	virtual ~DBCtrl(void)
	{
		delete m_pFtp;
		delete m_pFileDB;		
	}

public:

	mmsv_FtpClient*	m_pFtp;
	DBbase*			m_pFileDB;

	// 데이터베이스 초기화
	bool Init(char* host, char* db, char* id, char* pw);


/* 
	#### 인증 관련 함수들 
*/

	// uid / password 이용한 인증
	int AuthUser(mmsv_User* pClient, ACE_Message_Block* pDataBlock);


/* 
	#### 업로드 관련 함수들 : BBS 
*/	
	//20080122 비정상 종료로 인해 로컬에 저장돼 있던 파일 사이즈 업데이트, jyh
	void UpLoad_Size(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	////20080825 뮤레카에서 차단하는 컨텐츠 차단을 위해, jyh
	//void UpLoadMureka_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	//20071210 업로드전 해시값 비교 요청, jyh
	void UpLoadHash_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	//20071211 구라 전송 완료 패킷, jyh
	void UpLoadGuraEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 업로드 이전에 파일 등록 또는 올리던 파일 정보 가져오기
	void UpLoadPre_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 실제 파일 업로드를 위해 업로드 서버 정보 가져오기
	void UpLoadProcess_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 업로드 완료/중단 요청
	void UpLoadEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 업로드 진행상태
	void UpLoadState_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);

	// 업로드 아이템 추가
	void UpLoadItem_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS);

	// 업로드시 뮤레카 저작권 체크
	void UpLoadState_Copyright_data(mmsv_User* pClient, ACE_Message_Block* pDataBlock);


/* 
	#### 다운로드 관련 함수들 : BBS 
*/	
	
	// 파일다운로드 전에 파일의 정보와 해당 서버의 상태 요청
	void DownLoadPre_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 실제 다운로드를 위해 다운로드 정보 요청	
	void DownLoadPro_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 현재 파일 다운로드 중인 사이즈 통보시
	void DownLoad_State_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	// 다운로드 완료 종료 처리
	void DownLoadEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock);

	// 다운로드 아이템 추가
	void DownLoadItem_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS);


	// 업로드시 뮤레카 저작권 체크
	void DownLoadState_Copyright_data(mmsv_User* pClient, ACE_Message_Block* pDataBlock);


/* 
	#### 패킷 처리, 공유포인트 처리, 정액제 관련 함수들
*/

	// 정액제 가능한지 체크
	int  Fix_Check(mmsv_User* pClient, char* pUserID);
	

	// 업로드 수수료 적립
	double UploadCommission(char* pUserID, __int64 nDownSize);


	// 서버 점검 상태 체크
	bool CheckService();
	
	// 파일 중복체크 수 체크
	int LoadFileCountCore(char* pSaveName);
	int LoadFileCountCp(char* pCheckSum, __int64 nFileSize);
	// 중복되지 않는 서버
	int LoadFtpAllotmentCriNew(char* pCheckSum, __int64 nFileSize);
	// 파일 중복체크
	__int64 LoadNewFileCheck(char* pSaveName, __int64 nFileSize, __int64 nVolumeIdx, bool bTrick=0);
	//__int64 LoadNewFileCheck(char* pSaveName, __int64 nFileSize);
	// 새로운 파일 등록
	int SetNewFileComplete(char* pSaveName, __int64 nFileSize, __int64 nVolume);
	// FTP 서버 할당 요청
	__int64 LoadFtpAllotment();
	// FTP 서버 할당 요청(현재 접속으로 업로드 중인 큐가 있을경우)
	bool LoadFtpAllotmentQueue(__int64 nVolume);
	// FTP 서버 전송 가능 체크
	bool LoadFtpCheck(ACE_UINT64 nVolumeIdx);
    // FTP 서버 정보 요청
	bool LoadFtpInfoUpload(ACE_UINT64 nVolumeIdx, ACE_UINT16 nSpeed, char* pServerIP, int &nServerPORT, char* pServerID, char* pServerPW);
	// 다운로드 서버 정보 요청
	int LoadFtpInfoDownLoad(int nVolumeIdx);
	//int LoadFtpInfoDownLoad(char* pSaveName);
	// 다운로드 요청 성공으로 count_criterion 증가
	void DownCountSet_Criterion(char *pSaveName);


/* 
	#### 유틸 함수들
*/
	std::string SetSizeFormat(__int64 realsize);


public:	

	SYSTEMTIME		m_systime;

	bool			m_bServiceRun;
	ACE_Time_Value	m_tCheckTime;	

	// 다운로드에서 아이템 추가 요청시 추가되는 갯수
	int				m_nDownItemCount;
};


#endif