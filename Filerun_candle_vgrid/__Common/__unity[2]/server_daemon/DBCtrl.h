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

#include "../DBbase.h"
#include "../mmsv_FtpClient.h"

class DBCtrl : public DBbase
{

public:

	// 기본 생성/소멸자
	DBCtrl(void)
	{
		m_pFtp_Server = new mmsv_FtpClient();
		m_pFtp_Client = new mmsv_FtpClient();
		m_pFileDB	  = new DBbase();
	}
	virtual ~DBCtrl(void)
	{
		delete m_pFtp_Server;
		delete m_pFtp_Client;
		delete m_pFileDB;
	}


public:

	// 데이터베이스 초기화
	bool Init(char* host, char* db, char* id, char* pw);

public:
	
	// 스토러지 정보 업데이트
	void UpdateDiskAverage();

	// 파일 서버 분할
	void FileDivision();
	void DivisionVailed();
	void NewDivision();

	// 파일삭제처리
	void FileRemove();

	// FTP 서버 정보 요청
	bool LoadFtpInfo(ACE_UINT64 nVolumeIdx, char* pServerIP, int &nServerPORT, char* pServerID, char* pServerPW);
	// 이동할 FTP 서버 할당
	int	 LoadFtpAcco(ACE_UINT64 nVolumeIdx);
	// FTP 서버에서 파일 삭제
	bool RemoveFileofFTP(ACE_UINT64 nVolumeIdx, char* pFileName);


public:


	ACE_UINT64	m_nFtpIdxList[50];

	DBbase*				m_pFileDB;

	mmsv_FtpClient*		m_pFtp_Server;
	mmsv_FtpClient*		m_pFtp_Client;

};


#endif