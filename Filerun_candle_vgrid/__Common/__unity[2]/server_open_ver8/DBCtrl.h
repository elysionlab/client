/************************************************************************************
*
* MYSQL �����ͺ��̽� ���� ��� �۾�
*
* - �����ͺ��̽� ������ ������ �����带 ������� �ʰ� ť�� ���� ȣ��� ProcessFunc 
*   Ŭ�������� ȣ��ɶ� ����
* - ���� ����� mysql_ping�� �̿��� ���� Ȯ��
* - ���� �� ������ �������� �ʰ� Ÿ�Ӿƿ� �ð��� ������ �����
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

	// �⺻ ����/�Ҹ���
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

	// �����ͺ��̽� �ʱ�ȭ
	bool Init(char* host, char* db, char* id, char* pw);


/* 
	#### ���� ���� �Լ��� 
*/

	// uid / password �̿��� ����
	int AuthUser(mmsv_User* pClient, ACE_Message_Block* pDataBlock);

	//20080506 �����ڷ�� ���ε� ���� �Լ���, jyh
	void OpenUpLoadList(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	void OpenUpLoad(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	void OpneUpLoadEnd(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS);

	//20080506 �����ڷ�� �ٿ�ε� ���� �Լ���, jyh
	void OpenDownLoadList(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	void OpenDownLoad(mmsv_User* pClient, ACE_Message_Block* pDataBlock);
	void OpenDownLoadEnd(mmsv_User* pClient, ACE_Message_Block* pDataBlock);



/* 
	#### ��Ŷ ó��, ��������Ʈ ó��, ������ ���� �Լ���
*/

	// ������ �������� üũ
	int  Fix_Check(mmsv_User* pClient, char* pUserID);
	

	// ���ε� ������ ����
	double UploadCommission(char* pUserID, __int64 nDownSize);


	// ���� ���� ���� üũ
	bool CheckService();
	
	// ���� �ߺ�üũ �� üũ
	int LoadFileCountCore(char* pSaveName);
	int LoadFileCountCp(char* pCheckSum, __int64 nFileSize);
	// �ߺ����� �ʴ� ����
	int LoadFtpAllotmentCriNew(char* pCheckSum, __int64 nFileSize);
	// ���� �ߺ�üũ
	__int64 LoadNewFileCheck(char* pSaveName, __int64 nFileSize, __int64 nVolumeIdx, bool bTrick=0);
	//__int64 LoadNewFileCheck(char* pSaveName, __int64 nFileSize);
	// ���ο� ���� ���
	int SetNewFileComplete(char* pSaveName, __int64 nFileSize, __int64 nVolume);
	// FTP ���� �Ҵ� ��û
	__int64 LoadFtpAllotment();
	// FTP ���� �Ҵ� ��û(���� �������� ���ε� ���� ť�� �������)
	bool LoadFtpAllotmentQueue(__int64 nVolume);
	// FTP ���� ���� ���� üũ
	bool LoadFtpCheck(ACE_UINT64 nVolumeIdx);
    // FTP ���� ���� ��û
	bool LoadFtpInfoUpload(ACE_UINT64 nVolumeIdx, ACE_UINT16 nSpeed, char* pServerIP, int &nServerPORT, char* pServerID, char* pServerPW);
	// �ٿ�ε� ���� ���� ��û
	int LoadFtpInfoDownLoad(int nVolumeIdx);
	//int LoadFtpInfoDownLoad(char* pSaveName);
	// �ٿ�ε� ��û �������� count_criterion ����
	void DownCountSet_Criterion(char *pSaveName);


/* 
	#### ��ƿ �Լ���
*/
	std::string SetSizeFormat(__int64 realsize);


public:	

	SYSTEMTIME		m_systime;

	bool			m_bServiceRun;
	ACE_Time_Value	m_tCheckTime;	

	// �ٿ�ε忡�� ������ �߰� ��û�� �߰��Ǵ� ����
	int				m_nDownItemCount;
};


#endif