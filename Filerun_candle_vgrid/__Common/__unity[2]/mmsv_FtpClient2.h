
#ifndef _mmsv_FtpClient2_H
#define _mmsv_FtpClient2_H

#pragma once

#include "afxwin.h"
#include "afxcmn.h"

#include "ace/OS.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"

class mmsv_FtpClient2
{

public:

	mmsv_FtpClient2(void)
	:	m_bConnected(0)
	{
	}
	~mmsv_FtpClient2(void)
	{
	}


public:

	CString				m_sPassiveInfo;
	CString				m_strFtp;

	int					m_fc;	

	ACE_SOCK_Connector	pConnector;
	ACE_SOCK_Stream		pPeer;	
	
	char				m_pUserID[35];
	char				m_pUserPW[35];
	int					m_bConnected;

public:

	// ftp ������ ����
	int Connect(char* pAddress, int nPort, char* pUserID, char* pUserPW);	
	// ftp ���� �α���
	int Login();
	// ftp ������ ���� ����
	int DisConnect();
	// ftp ������ ���������� üũ
	int isConnect();	
	
	
	
	// ������ ���� ���� ���� üũ
	bool RemoteFileCheck(CString fullpath);
	bool FileSizeCheck();	

	// ���� ����Ʈ
	bool LOAD_FileList(CString& strList);
	// ���� ����
	int RemoveFile(CString savefilename);
	

	// ftp ������ ������ Cmd ó��
	int SendCmd(CString command);
	// ftp ������ Cmd ����
	int WriteStr(CString command);
	// ftp ������ ���� �޽��� ����(CMD)
	int ReadStr(); 
	// ftp ������ ���� ���� ����
	int ReadStr2(); 

};


#endif /* _mmsv_FtpClient2_H */