#include "StdAfx.h"
#include "FtpClient.h"
#include <io.h>


FtpClient::FtpClient()
{
	m_bRunThread = false;

	WSADATA	wsaData;	
	WSAStartup(MAKEWORD(2,0), &wsaData);
	DisConnect();
}

FtpClient::~FtpClient(void)
{
	DisConnect();
	WSACleanup();
}

void FtpClient::DisConnect()
{
	m_DateSock = INVALID_SOCKET;
	m_bFlag = false;
}


bool FtpClient::DownLoad(char* pServerInfo, char* pFilePath, __int64 nFileStartSize)
{
	int nCheckIndex;

	CString strServer = pServerInfo;
	m_pFilePath		  = pFilePath;
	m_nFileTransStart = nFileStartSize;


	nCheckIndex		= strServer.ReverseFind(',');
	m_ServerPORT	= atol(strServer.Right(strServer.GetLength() - (nCheckIndex + 1)));
	strServer		= strServer.Left(nCheckIndex);
	nCheckIndex		= strServer.ReverseFind(',');
	m_ServerPORT	+= 256 * atol(strServer.Right(strServer.GetLength() - (nCheckIndex + 1)));
	m_ServerIP		= strServer.Left(nCheckIndex);

	while(1) 
	{
		if((nCheckIndex = m_ServerIP.Find(",")) == -1) 
			break;

		m_ServerIP.SetAt(nCheckIndex, '.');
	}

	CString pFolderPath, strBuf;
	
	strBuf = pFilePath;
	pFolderPath = strBuf.Mid(0, strBuf.ReverseFind('\\') + 1);

	if(MakeFolder((LPSTR)(LPCTSTR)pFolderPath))
		return false;




	m_DateSock = socket(AF_INET, SOCK_STREAM, 0);
	if(m_DateSock == INVALID_SOCKET)
	{
		closesocket(m_DateSock);
		return false;
	}
	
	m_addr_datachannel.sin_family = AF_INET;
	m_addr_datachannel.sin_addr.S_un.S_addr = inet_addr((const char*)(LPSTR)(LPCTSTR)m_ServerIP);
	m_addr_datachannel.sin_port = htons(m_ServerPORT);	

	
	if(connect(m_DateSock, (sockaddr*)&m_addr_datachannel, sizeof(m_addr_datachannel)) == SOCKET_ERROR)
	{
		if(connect(m_DateSock, (sockaddr*)&m_addr_datachannel, sizeof(m_addr_datachannel)) == SOCKET_ERROR)
		{
			closesocket(m_DateSock);
			return false;
		}
	}

	if(!FileCheckOpen())
		return false;

	int		dummy;
	HANDLE	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTrans, this, 0, (unsigned int *)&dummy);
	CloseHandle(hTrans);
	
	m_bFlag	= true;
	return true;
}



bool FtpClient::FileCheckOpen()
{
	if(m_pFileData.m_hFile != CFile::hFileNull)
		m_pFileData.Close();
	
	if(!m_pFileData.Open((LPCTSTR)m_pFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone))
	{
		closesocket(m_DateSock);

		CString strBuf;
		strBuf.Format("파일열기에 실패했습니다.\r\n다른 프로그램에서 파일을 사용중일 가능성이 높습니다.\r\nPath(O) : %s", m_pFilePath);
		AfxMessageBox(strBuf);
		return false;
	}
	else if(m_nFileTransStart > 0)
		m_pFileData.Seek(m_nFileTransStart, CFile::begin); 

	return true;
}



void FtpClient::FileTrans()
{	
	char*		cDatabuf;
	int			nBufferSize = 0;	
	int			nRecvSize	= 0;	
	int			nFlushCnt = 0;

	FD_SET		ReadSet;
    int			nCheckVal = 0;
	struct		timeval nTval;


	switch(m_pMainWindow->m_rDownTransBuffer)
	{
		case 0:
		{
			nBufferSize = 1024;
			break;
		}
		case 1:
		{
			nBufferSize = 2048;
			break;
		}
		case 2:
		{
			nBufferSize = 4096;
			break;
		}
		case 3:
		{
			nBufferSize = 8192;
			break;
		}
		case 4:
		{
			nBufferSize = 16384;
			break;
		}
		default:
		{
			nBufferSize = 1024;
			break;
		}
	}

	cDatabuf = new char[nBufferSize];
 

	if(m_pMainWindow->m_pPtrTrans)
	{
		__int64	&nSizePtr = m_pMainWindow->m_pPtrTrans->nFileSizeEnd;

		while(m_bFlag && !m_pMainWindow->m_bForceExit) 
		{		
			FD_ZERO(&ReadSet);
			FD_SET(m_DateSock, &ReadSet);

			nTval.tv_sec = 3;
			nTval.tv_usec = 0;

			nCheckVal = select(0, &ReadSet, NULL, NULL, &nTval);

			// 타임아웃
			if(nCheckVal == 0)
			{
				break;
			}

			// 소켓에러
			else if(nCheckVal == SOCKET_ERROR)
			{
				break;	
			}

			else if(FD_ISSET(m_DateSock, &ReadSet))
			{
				nRecvSize = recv(m_DateSock, cDatabuf, nBufferSize, 0);

				if(nRecvSize == 0) 
				{
					if(m_pMainWindow->m_pPtrTrans != NULL)
					{						
						m_pMainWindow->m_pPtrTrans->nCountCloseSocket++;
						if(m_pMainWindow->m_pPtrTrans->nCountCloseSocket >= 3)
							m_pMainWindow->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					}
					break;
				}
				
				// 외부에서 종료 요청
				if(!m_bFlag)
					break;

				m_pFileData.Write(cDatabuf, nRecvSize);

				nFlushCnt++;
				if(nFlushCnt > 2000)
				{
					m_pFileData.Flush();
					nFlushCnt = 0;
				}

				nSizePtr += nRecvSize;
			}
		}	
	}
	
	m_pFileData.Flush();
	m_pFileData.Close();
	closesocket(m_DateSock);	

	m_bFlag = false;

	delete cDatabuf;
}


bool  FtpClient::MakeFolder(char* pszDir)
{
    bool bRet = false;
    int nLen = (int)strlen(pszDir);
    char* pszSubDir = NULL;

    _finddata_t fdata;
    long hFind;
    for (int i = nLen - 1; i >= 0; i--)
    {
		if (pszDir[i] == '\\' && pszDir[i-1] != ':')
        {			
            pszSubDir = new char[i+1];
            memset(pszSubDir, 0, i+1);
            memcpy(pszSubDir, pszDir, i);
            if (hFind = _findfirst(pszSubDir, &fdata) == -1L)
            {
                if (!MakeFolder(pszSubDir))
                {
                    delete pszSubDir;
                    return bRet;
                }
            }
            delete pszSubDir;
            break;
        }
    }
    bRet = (bool)::CreateDirectory(pszDir, NULL);
    return bRet;
}



unsigned int __stdcall ProcFileTrans(void *pParam)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	FtpClient* pProcess = (FtpClient*)pParam;

	pProcess->m_bRunThread = true;
	pProcess->FileTrans();
	pProcess->m_bRunThread = false;

	return 0;
}