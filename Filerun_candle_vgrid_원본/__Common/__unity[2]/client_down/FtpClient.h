#pragma once
#include "down_window.h"

class FtpClient
{

public:


	FtpClient(void);
	~FtpClient(void);
	void DisConnect();

public:

	down_window*	m_pMainWindow;
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
	bool DownLoadEx(char* pServerInfo, char* pFilePath, __int64 nFileStartSize);
	
	void FileTrans();
	void FileTransEx();
	friend unsigned int __stdcall ProcFileTrans(void *pParam);
	friend unsigned int __stdcall ProcFileTransEx(void *pParam);
};

