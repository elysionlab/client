#include "mmsv_FtpClient.h"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP 서버와 연결
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::Connect(char* pAddress, int nPort, char* pUserID, char* pUserPW)
{
	try
	{
		ACE_INET_Addr pAddr(nPort, pAddress);

		strcpy(m_pServer, pAddress);
		strcpy(m_pUserID, pUserID);
		strcpy(m_pUserPW, pUserPW);
		
		DisConnect();

		ACE_Time_Value nTimeOut(OPTION_FTP_CONNECT_TIME);
		if(pConnector.connect(pPeer, pAddr, &nTimeOut) == -1)
		{
			m_bConnected = 0;
			return 0;
		}
		else
		{
			m_sPassiveInfo = "";
			m_bConnected = 1;
		}
		
		
		if(!ReadStr())
		{
			DisConnect(false);
			return 0;
		}

		if(!Login())
		{
			DisConnect(false);
			return 0;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::Connect━%D━Error\n"))); 
		m_bConnected = 0;
	}

	return m_bConnected;
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP 서버와 연결 종료
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::DisConnect(bool bWriteCall/* = true*/)
{		
	try
	{
		if(m_bConnected == 1)
		{
			if(bWriteCall)
				WriteStr("QUIT");

			pPeer.close();
			m_bConnected = 0;	
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::DisConnect━%D━Error\n"))); 
	}
	
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP 서버 로그인
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::Login()
{
	int nResult = 1;

	try
	{
		std::string temp;
		temp = "USER ";
		temp = temp + m_pUserID;

		if(!WriteStr(temp))
			return 0;
		
		if(!ReadStr())
			return 0;

		temp = "PASS ";
		temp = temp + m_pUserPW;

		if(!WriteStr(temp))
			return 0;

		if(ReadStr() != 230)
			return 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::Login━%D━Error\n"))); 
		nResult = 0;
	}

	return nResult;
}






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP 서버와 연결중인지 체크
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::isConnect()
{
	return m_bConnected;
}



// 서버의 파일 존재 유무 체크
__int64 mmsv_FtpClient::RemoteFileValid(std::string fullpath)
{
	__int64 nResult = 1;

	try
	{
		// 파일의 존재 유무는 체크 가능한다 : RNFR
		fullpath = "SIZE " + fullpath;
		
		if(!WriteStr(fullpath))
			return -1;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::RemoteFileValid━%D━Error\n"))); 
		nResult = -1;
	}

	if(nResult != -1) return ReadSize();
	return nResult;
}




// FXP Server
int mmsv_FtpClient::FXP_Server(char* pFileName)
{
	int nResult = 1;

	try
	{
		std::string savename = pFileName;

		// 다운로드 타입 결정
		if(!SendCmd("TYPE I")) 
			return 0;

		if(!SendCmd("PASV"))
			return 0;

		if(!WriteStr("RETR " + savename))
			return 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::FXP_Server━%D━Error\n"))); 
		nResult = 0;
	}
	
	return nResult;
}


// FXP Client
int mmsv_FtpClient::FXP_Client(char* pFileName, std::string pAddress)
{
	int nResult = 1;

	try
	{
		std::string savename = pFileName;

		// 다운로드 타입 결정
		if(!SendCmd("TYPE I")) 
			return 0;

		std::string cmd_port; 
		cmd_port = "PORT " + pAddress;
		if(!SendCmd(cmd_port))
			return 0;

		if(!WriteStr("STOR " + savename))
			return 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::FXP_Client━%D━Error\n")));
		nResult = 0;
	}

	return nResult;
}

// FXP Result 
int mmsv_FtpClient::FXP_Result()
{
	int nResult = 1;
	try
	{
		if(FXP_Recv() == 150)
		{
			if(FXP_Recv() != 226)
				return 0;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::FXP_Client━%D━Error\n")));
		nResult = 0;
	}

	return nResult;
}

// FXP Recv
int mmsv_FtpClient::FXP_Recv()
{
	int		num	= 0;
	char	cDatabuf[1024];
	int		nResult = 1;

	try
	{
		ACE_Time_Value	recvTimeout(600);
		ZeroMemory(cDatabuf, sizeof(cDatabuf));

		m_retmsg.empty();

		if((num = pPeer.recv(cDatabuf, sizeof(cDatabuf), &recvTimeout)) == -1)
				return 0;
			
		else
			m_retmsg = cDatabuf;

		return atoi((char*)(m_retmsg.substr(0, 3)).c_str());
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::FXP_Client━%D━Error\n")));
		nResult = 0;
	}
	return nResult;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP 서버에서 업로드, 다운로드 요청
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::QueryTransfer(char* pFileName, __int64 startsize, int nDownMode)
{
	int nResult = 1;
	std::string savename = pFileName;

	try
	{
		// 다운로드 타입 결정
		if(!SendCmd("TYPE I")) 
			throw 1;

		char snSize[31];
		sprintf(snSize, "REST %I64d", startsize);
		std::string cmd_rest(snSize);
		if(!SendCmd(cmd_rest))
			throw 2;

		if(!SendCmd("PASV"))
			throw 3;

		// 다운로드 모드일때
		if(nDownMode == 1)
		{
			if(!WriteStr("RETR " + savename))
				throw 4;
		}
		// 업로드 모드일때
		else
		{
			if(!WriteStr("STOR " + savename))
				throw 5;
		}
	}
	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::QueryTransfer━%D━Error : %s : %s : %s : %d\n"), m_pServer, m_pUserID, pFileName, nCode));
		nResult = 0;
	}
	catch(...)
	{		
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::QueryTransfer━%D━Error : %s\n"), pFileName));
		nResult = 0;
	}

	if(nResult) 
	{
		// 업로드 모드일때
		//if(nDownMode != 1)
		//	DisConnect();
	}

	return nResult;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ftp 서버로 부터 다운로드 결과체크
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
__int64 mmsv_FtpClient::CheckDownLoad()
{
	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 해당서버에 파일삭제
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::RemoveFile(char* pFileName)
{
	try
	{
		std::string savefilename = pFileName;
		savefilename = "DELE " + savefilename;
		
		return SendCmd(savefilename); 
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::RemoveFile━%D━Error\n")));
	}
	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 파일명 변경
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::RenameFile(char* pFileNameOld, char* pFileNameNew)
{
	std::string strOldFile = pFileNameOld;
	strOldFile = "RNFR " + strOldFile;

	std::string strNewFile = pFileNameNew;
	strNewFile = "RNTO " + strNewFile;
	

	try
	{
		if(SendCmd(strOldFile))
		{
			if(SendCmd(strNewFile))
				return 1;
			else
				return 0;
		}	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::RenameFile━%D━Error\n")));
	}

	return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP 서버로 전송할 CMD 처리
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::SendCmd(std::string command)
{
	try
	{
		if(command == "")
			return 0;

		if(!WriteStr(command)) 
			return 0;

		if(!ReadStr()) 
			return 0;

		return 1;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::SendCmd━%D━Error\n")));
	}

	return 1;
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ftp 서버로 Cmd 전송
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::WriteStr(std::string command)
{
	ACE_Time_Value sendTimeout(OPTION_FTP_QUERY_TIME);
	size_t num	= 0;
	size_t total = 0;

	if(m_bConnected == 0)
		return 0;

	command = command + "\r\n";
	ZeroMemory(m_pLastCmd, sizeof(m_pLastCmd));
	strcpy(m_pLastCmd, command.c_str());

	try 
	{
		while(1)
		{
			if((num = pPeer.send_n(((const char*)m_pLastCmd) + total, strlen(m_pLastCmd), &sendTimeout)) == -1)
			{
				DisConnect(false);
				return 0;
			}
			else
			{
				total += num;

				if(total >= strlen(m_pLastCmd))
					break;
			}
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::WriteStr━%D━Error\n")));
	}
	return 1;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ftp 서버로 부터 파일 사이즈 요청 결과 받음
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
__int64 mmsv_FtpClient::ReadSize()
{
	int retcode;

	try
	{
		if(!ReadStr()) 
			return -1;

		retcode = atoi((char*)(m_retmsg.substr(0, 3)).c_str());

		// 파일이 존재 하지 않음
		if(retcode == 550)		
		{
			return -2;
		}
		// 파일이 존재
		else if(retcode == 213)	
		{
			return _atoi64((char*)(m_retmsg.erase(0, 4)).c_str());
		}
		// 알수없는 에러
		else					
		{
			return -1;
		}	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::ReadSize━%D━Error\n")));
	}
	return -1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 서버로 부터 CMD 받음
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient::ReadStr(int nTimeOut/* = OPTION_FTP_QUERY_TIME*/) 
{
	int		nReplyCode	= 0;
	int		nReadSize	= 0;
	char	cDatabuf[1024];

	if(m_bConnected == 0)
		return 0;

	ACE_Time_Value recvTimeout(OPTION_FTP_QUERY_TIME);

	try
	{
		ZeroMemory(cDatabuf, sizeof(cDatabuf));
		m_retmsg.empty();

		if((nReadSize = pPeer.recv(cDatabuf, sizeof(cDatabuf), &recvTimeout)) == -1)
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::ReadStr━-1━%s━%s\n"), m_pLastCmd, m_pServer));
			DisConnect(false);
			return 0;
		}
		else
		{
			if(nReadSize == 0)
			{
				ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::ReadStr━0━%s━%s\n"), m_pLastCmd, m_pServer));
				DisConnect(false);
				return 0;
			}
			else
			{
				m_retmsg = cDatabuf;
			}
		}

		nReplyCode = atoi((char*)(m_retmsg.substr(0, 3)).c_str());

		if(nReplyCode == 227)
		{
			size_t	i, j;

			if((i = m_retmsg.find("(")) == -1  ||  (j = m_retmsg.find(")") ) == -1) 
				return 0;

			m_sPassiveInfo = m_retmsg.substr(i + 1, (j - i) - 1);
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::ReadStr━%D━Error\n")));
		nReplyCode = 0;
	}

	return nReplyCode;
}


// ftp 서버로 부터 다운로드 받은 사이즈 체크
__int64 mmsv_FtpClient::DownLoadSize()
{
	int		nReadSize	= 0;
	char	cDatabuf[2048];
	std::string strSize;

	if(m_bConnected == 0)
		return -1;

	try
	{
		ZeroMemory(cDatabuf, sizeof(cDatabuf));
		m_retmsg.empty();

		ACE_Time_Value recvTimeout(1);
			
		if((nReadSize = pPeer.recv(cDatabuf, sizeof(cDatabuf), &recvTimeout)) == -1)
		{		
			WriteStr("ABOR");
			return -3;
		}
		else
		{
			if(nReadSize == 0)
			{
				DisConnect(false);
				return -1;
			}
			else
			{
				m_retmsg = cDatabuf;
				
				size_t	i, j;

				if((i = m_retmsg.find("mmsv_begin")) == -1  ||  (j = m_retmsg.find("mmsv_end") ) == -1) 
					return -2;

				strSize = m_retmsg.substr(i + 11, (j - i) - 2);

				DisConnect();
				return _atoi64(strSize.c_str());
			}
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_FtpClient::DownLoadSize━%D━Error\n")));
	}

	DisConnect();
	return -1;
}



