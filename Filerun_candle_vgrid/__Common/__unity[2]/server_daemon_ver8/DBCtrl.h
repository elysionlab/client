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

#include "../DBbase.h"
#include "../mmsv_FtpClient.h"

class DBCtrl : public DBbase
{

public:

	// �⺻ ����/�Ҹ���
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

	// �����ͺ��̽� �ʱ�ȭ
	bool Init(char* host, char* db, char* id, char* pw);

public:
	
	// ���䷯�� ���� ������Ʈ
	void UpdateDiskAverage();

	// ���� ���� ����
	void FileDivision();
	void DivisionVailed();
	void NewDivision();

	// ���ϻ���ó��
	void FileRemove();

	// FTP ���� ���� ��û
	bool LoadFtpInfo(ACE_UINT64 nVolumeIdx, char* pServerIP, int &nServerPORT, char* pServerID, char* pServerPW);
	// �̵��� FTP ���� �Ҵ�
	int	 LoadFtpAcco(ACE_UINT64 nVolumeIdx);
	// FTP �������� ���� ����
	bool RemoveFileofFTP(ACE_UINT64 nVolumeIdx, char* pFileName);


public:


	ACE_UINT64	m_nFtpIdxList[50];

	DBbase*				m_pFileDB;

	mmsv_FtpClient*		m_pFtp_Server;
	mmsv_FtpClient*		m_pFtp_Client;

};


#endif