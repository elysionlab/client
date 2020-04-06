
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

	// ftp 서버와 연결
	int Connect(char* pAddress, int nPort, char* pUserID, char* pUserPW);	
	// ftp 서버 로그인
	int Login();
	// ftp 서버와 연결 종료
	int DisConnect(bool bWriteCall = true);
	// ftp 서버와 연결중인지 체크
	int isConnect();	
	
	
	
	// 서버의 파일 존재 유무 체크
	__int64 RemoteFileValid(std::string fullpath);
	// ftp 서버로 부터 파일 사이즈 요청 결과 받음
	__int64 ReadSize();	
	// 파일 다운로드를 위한 FTP서버 정보 받기
	int QueryTransfer(char* pFileName, __int64 startsize, int nDownMode);	
	// 파일 삭제
	int RemoveFile(char* pFileName);
	// 파일명 변경
	int RenameFile(char* pFileNameOld, char* pFileNameNew);


	// FXP Server
	int FXP_Server(char* pFileName);	
	// FXP Client
	int FXP_Client(char* pFileName, std::string pAddress);	
	// FXP Result 
	int FXP_Result();
	// FXP Recv
	int FXP_Recv();
	

	// ftp 서버로 부터 다운로드 결과체크
	__int64 CheckDownLoad();

	// ftp 서버로 전송할 Cmd 처리
	int SendCmd(std::string command);
	// ftp 서버로 Cmd 전송
	int WriteStr(std::string command);
	// ftp 서버로 부터 메시지 받음(CMD)
	int ReadStr(int nTimeOut = OPTION_FTP_QUERY_TIME); 	
	// ftp 서버로 부터 다운로드 받은 사이즈 체크
	__int64 DownLoadSize();

};


#endif /* _MMSV_FTPCLIENT_H */