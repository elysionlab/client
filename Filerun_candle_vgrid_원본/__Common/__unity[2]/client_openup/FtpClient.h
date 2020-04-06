#pragma once
#include "d:\Project\Nfile°ü·Ã_vs8\Nfile\src_client\OpenUp_client\OpenUp_clientDlg.h"


class FtpClient
{

public:


	FtpClient(void);
	~FtpClient(void);
	void DisConnect();

public:

	COpenUp_clientDlg*	m_pMainWindow;
	CFile			m_pFileData;
	SOCKET			m_DateSock;
	sockaddr_in		m_addr_datachannel;

	CString			m_ServerIP;
	UINT			m_ServerPORT;
	
	CString			m_pFilePath;
	__int64			m_nFileTransStart;
	__int64			m_nFileSize;;

	bool			m_bFlag;

public:

	bool FileCheckOpen();

	bool UpLoad(char* pServerInfo, char* pFilePath, __int64 nFileStartSize);
	bool UpLoadTrick(char* pFilePath, __int64 nFileSize, __int64 nFileStartSize);	
	
	void FileTrans();
	void FileTransTrick();

	friend unsigned int __stdcall ProcFileTrans(void *pParam);
	friend unsigned int __stdcall ProcFileTransTrick(void *pParam);
	
};

