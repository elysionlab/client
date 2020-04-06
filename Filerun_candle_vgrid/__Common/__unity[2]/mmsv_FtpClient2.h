
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

	// ftp 서버와 연결
	int Connect(char* pAddress, int nPort, char* pUserID, char* pUserPW);	
	// ftp 서버 로그인
	int Login();
	// ftp 서버와 연결 종료
	int DisConnect();
	// ftp 서버와 연결중인지 체크
	int isConnect();	
	
	
	
	// 서버의 파일 존재 유무 체크
	bool RemoteFileCheck(CString fullpath);
	bool FileSizeCheck();	

	// 파일 리스트
	bool LOAD_FileList(CString& strList);
	// 파일 삭제
	int RemoveFile(CString savefilename);
	

	// ftp 서버로 전송할 Cmd 처리
	int SendCmd(CString command);
	// ftp 서버로 Cmd 전송
	int WriteStr(CString command);
	// ftp 서버로 부터 메시지 받음(CMD)
	int ReadStr(); 
	// ftp 서버로 부터 파일 받음
	int ReadStr2(); 

};


#endif /* _mmsv_FtpClient2_H */