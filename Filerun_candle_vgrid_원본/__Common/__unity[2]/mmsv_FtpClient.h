
#ifndef _MMSV_FTPCLIENT_H
#define _MMSV_FTPCLIENT_H

#pragma once
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#if defined (ACE_HAS_WINNT4)
#	include <string>
#else

#endif

#include "config_patten.h"
#include "config.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"

class mmsv_FtpClient
{

public:

	mmsv_FtpClient(void)
	:	m_bConnected(0)
	{
		ZeroMemory(m_pServer, sizeof(m_pServer));
		ZeroMemory(m_pUserID, sizeof(m_pUserID));
		ZeroMemory(m_pUserPW, sizeof(m_pUserPW));
		ZeroMemory(m_pLastCmd, sizeof(m_pLastCmd));		
	}
	~mmsv_FtpClient(void)
	{
		DisConnect();
	}


public:

	ACE_SOCK_Connector	pConnector;
	ACE_SOCK_Stream		pPeer;	
	
	char				m_pServer[30];
	char				m_pUserID[35];
	char				m_pUserPW[35];
	int					m_bConnected;

	char				m_pLastCmd[1024];

public:

	std::string			m_retmsg;
	std::string			m_sPassiveInfo;


public:

	// ftp ������ ����
	int Connect(char* pAddress, int nPort, char* pUserID, char* pUserPW);	
	// ftp ���� �α���
	int Login();
	// ftp ������ ���� ����
	int DisConnect(bool bWriteCall = true);
	// ftp ������ ���������� üũ
	int isConnect();	
	
	
	
	// ������ ���� ���� ���� üũ
	__int64 RemoteFileValid(std::string fullpath);
	// ftp ������ ���� ���� ������ ��û ��� ����
	__int64 ReadSize();	
	// ���� �ٿ�ε带 ���� FTP���� ���� �ޱ�
	int QueryTransfer(char* pFileName, __int64 startsize, int nDownMode);	
	// ���� ����
	int RemoveFile(char* pFileName);
	// ���ϸ� ����
	int RenameFile(char* pFileNameOld, char* pFileNameNew);


	// FXP Server
	int FXP_Server(char* pFileName);	
	// FXP Client
	int FXP_Client(char* pFileName, std::string pAddress);	
	// FXP Result 
	int FXP_Result();
	// FXP Recv
	int FXP_Recv();
	

	// ftp ������ ���� �ٿ�ε� ���üũ
	__int64 CheckDownLoad();

	// ftp ������ ������ Cmd ó��
	int SendCmd(std::string command);
	// ftp ������ Cmd ����
	int WriteStr(std::string command);
	// ftp ������ ���� �޽��� ����(CMD)
	int ReadStr(int nTimeOut = OPTION_FTP_QUERY_TIME); 	
	// ftp ������ ���� �ٿ�ε� ���� ������ üũ
	__int64 DownLoadSize();

};


#endif /* _MMSV_FTPCLIENT_H */