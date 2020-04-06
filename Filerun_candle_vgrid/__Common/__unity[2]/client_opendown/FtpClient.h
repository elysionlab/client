#pragma once
#include "d:\Project\Nfile°ü·Ã_vs8\Nfile\src_client\OpenDown_client\OpenDown_clientDlg.h"

class FtpClient
{

public:


	FtpClient(void);
	~FtpClient(void);
	void DisConnect();

public:

	COpenDown_clientDlg*	m_pMainWindow;
	CFile			m_pFileData;
	SOCKET			m_DateSock;
	sockaddr_in		m_addr_datachannel;

	CString			m_ServerIP;
	UINT			m_ServerPORT;
	
	CString			m_pFilePath;
	__int64			m_nFileTransStart;

	bool			m_bFlag;
	bool			m_bRunThread;


public:

	bool FileCheckOpen();
	bool MakeFolder(char* pszDir);

	bool DownLoad(char* pServerInfo, char* pFilePath, __int64 nFileStartSize);
	
	void FileTrans();
	friend unsigned int __stdcall ProcFileTrans(void *pParam);
};

