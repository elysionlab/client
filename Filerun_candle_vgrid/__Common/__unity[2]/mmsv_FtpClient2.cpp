#include "mmsv_FtpClient2.h"



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP ������ ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::Connect(char* pAddress, int nPort, char* pUserID, char* pUserPW)
{
	ACE_INET_Addr pAddr(nPort, pAddress);

	strcpy(m_pUserID, pUserID);
	strcpy(m_pUserPW, pUserPW);
	
	DisConnect();

	ACE_Time_Value nTimeOut(10);
	if(pConnector.connect(pPeer, pAddr, &nTimeOut) == -1)
	{
		m_bConnected = 0;
		return 0;
	}
	else
	{
		m_bConnected = 1;
	}

	
	ReadStr();

	if(!Login())
	{
		pPeer.close();
		m_bConnected = 0;

		return 0;
	}

	return m_bConnected;
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP ������ ���� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::DisConnect()
{	
	if(m_bConnected == 1)
	{
		WriteStr("QUIT");

		pPeer.close();
		m_bConnected = 0;	
	}
	
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP ���� �α���
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::Login()
{
	CString temp;
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

	if(!ReadStr())
		return 0;

	return 1;
}






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP ������ ���������� üũ
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::isConnect()
{
	return m_bConnected;
}



// ������ ���� ���� ���� üũ
bool mmsv_FtpClient2::RemoteFileCheck(CString fullpath)
{
	// ������ ���� ������ üũ �����Ѵ� : RNFR
	fullpath = "SIZE " + fullpath;
	
	if(!WriteStr(fullpath))
		return false;

	return FileSizeCheck();
}
	
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ftp ������ ���� ���� ������ ��û ��� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool mmsv_FtpClient2::FileSizeCheck()
{
	int retcode;

	if(!ReadStr2()) 
		return 0;

	retcode = atoi((LPSTR)(LPCTSTR)(m_strFtp.Mid(0, 3)));

	// ������ ���� ���� ����
	if(retcode == 550)		
	{
		return false;
	}
	// ������ ����
	else if(retcode == 213)	
	{
		return true;
	}
	// �˼����� ����
	else					
	{
		return false;
	}	
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP �������� �ٿ�ε� (�ٿ�ε� �ޱ��� ���� �� ���� ���� ���� üũ)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool mmsv_FtpClient2::LOAD_FileList(CString& strList)
{
	m_sPassiveInfo = "";

	// �ٿ�ε� Ÿ�� ����
	if(!SendCmd("TYPE I")) 
		return false;

	if(!SendCmd("PASV"))
		return false;

	if(!WriteStr("LIST"))
		return false;

	DisConnect();




	UINT	m_ServerPORT;
	CString m_ServerIP;
	
	int i;
	i = m_sPassiveInfo.ReverseFind(',');
	m_ServerPORT = atol(m_sPassiveInfo.Right(m_sPassiveInfo.GetLength()-(i+1)));
	m_sPassiveInfo = m_sPassiveInfo.Left(i);
	i = m_sPassiveInfo.ReverseFind(',');
	
	m_ServerPORT	+= 256 * atol( m_sPassiveInfo.Right(m_sPassiveInfo.GetLength()-(i+1))); 
	m_ServerIP		= m_sPassiveInfo.Left(i);

	while(1) 
	{
		if((i=m_ServerIP.Find(","))==-1) 
			break;

		m_ServerIP.SetAt(i,'.');
	}

	ACE_SOCK_Connector	pConnector2;
	ACE_SOCK_Stream		pPeer2;	
	ACE_INET_Addr pAddr(m_ServerPORT, (LPSTR)(LPCTSTR)m_ServerIP);

	ACE_Time_Value nTimeOutCnnt(10);
	if(pConnector2.connect(pPeer2, pAddr, &nTimeOutCnnt) == -1)
	{
		return false;
	}
	else
	{
		int num;
		char cDatabuf[409600];
		ACE_Time_Value recvTimeout(10);
		
		
		while(1)
		{
			ZeroMemory(cDatabuf, sizeof(cDatabuf));
			num = pPeer2.recv(cDatabuf, sizeof(cDatabuf), &recvTimeout);

			if(num == -1 || num == 0)
			{
				break;
			}
			else
			{
				strList = strList + cDatabuf;
			}		
		}
	}

	pPeer2.close();	
	return true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ش缭���� ���ϻ���
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::RemoveFile(CString savefilename)
{
	savefilename = "DELE " + savefilename;
	
	if(SendCmd(savefilename)) return 1;
	else return 0;
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FTP ������ ������ CMD ó��
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::SendCmd(CString command)
{
	if(command != "" && !WriteStr(command)) 
		return 0;

	if((!ReadStr()) || (m_fc != 2)) 
		return 0;

	return 1;
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ftp ������ Cmd ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::WriteStr(CString command)
{
	ACE_Time_Value sendTimeout(10);
	size_t num	= 0;
	size_t total = 0;

	try 
	{
		command = command + "\r\n";

		while(1)
		{
			if((num = pPeer.send_n(((const char*)(LPSTR)(LPCTSTR)command) + total, command.GetLength(), &sendTimeout)) == -1)
			{
				return 0;
			}
			else
			{
				total += num;

				if(total >= command.GetLength())
					break;
			}
		}
	}
	catch(...)
	{
		return 0;
	}
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ������ ���� CMD ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::ReadStr() 
{
	int retcode;

	if(!ReadStr2()) 
		return 0;

	if(m_strFtp.GetLength() < 4 || m_strFtp.GetAt(3) != '-') 
		return 1;

	retcode = atoi((LPSTR)(LPCTSTR)m_strFtp);
	return 1;
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ������ ���� ���� ������ ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int mmsv_FtpClient2::ReadStr2() 
{
	int		nReplyCode = 0;
	int		num	= 0;
	char	cDatabuf[1024];

	m_strFtp.Empty();
		
	ACE_Time_Value recvTimeout(10);
	ZeroMemory(cDatabuf, sizeof(cDatabuf));

	if((num = pPeer.recv(cDatabuf, sizeof(cDatabuf), &recvTimeout)) == -1)
		return 0;
	else
		m_strFtp = cDatabuf;


	if(m_strFtp.GetLength() > 0 )
		m_fc = m_strFtp.GetAt(0) - 48;


	nReplyCode = atoi((LPSTR)(LPCTSTR)m_strFtp.Mid(0, 3));

	switch(nReplyCode)
	{
		// ���� ����� �޾� �ö� �ѿ� ���� ��������
		case 213: case 550: 
			m_fc = 2;
			return 1;
		// �̾� �ޱ⸦ �õ��Ҷ� �ѿ� ���� ��������
		case 350:
			m_fc = 2;		// ������ �������ش� : ���� �ƴ�
			return 1;

		case 227:
		{
			size_t	i, j;
			if((i = m_strFtp.Find("(")) == -1  ||  (j = m_strFtp.Find(")") ) == -1 ) 
				return 0;

			m_sPassiveInfo = m_strFtp.Mid(i+1, (j-i)-1);
			return 1;
		}

		default:
			break;
	}

	return 1;
}








