#include "StdAfx.h"
#include "FtpClient.h"
#include <io.h>

FtpClient::FtpClient()
{
	WSADATA	wsaData;	
	//20080424 vs8로 컨버팅으로 인한 수정, jyh
	//WSAStartup(MAKEWORD(2,0), &wsaData);
	WSAStartup(MAKEWORD(2,2), &wsaData);
	DisConnect();
}


FtpClient::~FtpClient(void)
{
	DisConnect();
	WSACleanup();
}


void FtpClient::DisConnect()
{
	m_bFlag = false;
}

bool FtpClient::UpLoad(char* pServerInfo, char* pFilePath, __int64 nFileStartSize)
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
	CFileFind pFileFind;
	if(!pFileFind.FindFile(m_pFilePath))
	{
		pFileFind.Close();
		closesocket(m_DateSock);
		return false;
	}
	pFileFind.Close();



	if(!m_pFileData.Open(m_pFilePath,  CFile::modeRead | CFile::shareDenyNone))
	{
		closesocket(m_DateSock);
		return false;
	}

	m_pFileData.Seek(m_nFileTransStart, CFile::begin); 
	return true;
}


void FtpClient::FileTrans()
{
	char		cDatabuf[81920];
	long		nSendOption = 0;	
	long		nOffSet	 = 0;
	long		nReadSize= 0;
	long		nSendSize= 0;
	long		nSendSizeBuf = 0;

	FD_SET		WriteSet;
    int			nCheckVal = 0;
	struct		timeval nTval;
	
	switch(m_pMainWindow->m_rUpTransBuffer)
	{
		case 0:
		{
			nSendOption = 2048;
			break;
		}
		case 1:
		{
			nSendOption = 4096;
			break;
		}
		case 2:
		{
			nSendOption = 8192;
			break;
		}
		case 3:
		{
			nSendOption = 32768;
			break;
		}
		case 4:
		{
			nSendOption = 81920;
			break;
		}
		default:
		{
			nSendOption = 8192;
			break;
		}
	}


	if(m_pMainWindow->m_pPtrTrans)
	{
		__int64	&nSizePtr = m_pMainWindow->m_pPtrTrans->nFileSizeEnd;


		while(m_bFlag && !m_pMainWindow->m_bForceExit) 
		{	
			nReadSize		= 0;
			nSendSize		= 0;
			nOffSet			= 0;

			if(!(nReadSize = (long)m_pFileData.Read(cDatabuf, sizeof(cDatabuf)))) 
			{
				Sleep(200);	
				break;
			}

			while(m_bFlag && !m_pMainWindow->m_bForceExit && nReadSize > nOffSet) 
			{
				FD_ZERO(&WriteSet);
				FD_SET(m_DateSock, &WriteSet);

				nTval.tv_sec = 3;
				nTval.tv_usec = 0;

				nCheckVal = select(0, NULL, &WriteSet, NULL, &nTval);

				// 타임아웃
				if(nCheckVal == 0)
				{
					m_bFlag = false;
					break;
				}

				// 소켓에러
				else if(nCheckVal == SOCKET_ERROR)
				{
					if(WSAGetLastError() == WSAEWOULDBLOCK)
					{						
						Sleep(50);
						continue;
					}
					else
					{				
						m_bFlag = false;
						break;
					}	
				}

				else if(FD_ISSET(m_DateSock, &WriteSet))
				{
					nSendSizeBuf = (long)((nReadSize - nOffSet) >= nSendOption) ? nSendOption : nReadSize - nOffSet;	
					if((nSendSize = (long)send(m_DateSock, cDatabuf + nOffSet, (int)nSendSizeBuf, 0)) > 0)
					{
						nOffSet		+= nSendSize;
						nSizePtr	+= nSendSize;	
					}
					else
					{
						if(nSendSize == SOCKET_ERROR)
						{
							if(WSAGetLastError() == WSAEWOULDBLOCK)
							{							
								Sleep(50);
								continue;
							}
							else
							{				
								m_bFlag = false;
								break;
							}	
						}
						else
						{
							m_bFlag = false;
							break;
						}
					}
				}
			}	
		}
	}

	
	m_pFileData.Close();
	closesocket(m_DateSock);
	
	m_bFlag = false;
}


bool FtpClient::UpLoadTrick(char* pFilePath, __int64 nFileSize, __int64 nFileStartSize)
{
	m_pFilePath			= pFilePath;
	m_nFileSize			= nFileSize;
	m_nFileTransStart	= nFileStartSize;
	
	if(!FileCheckOpen())
		return false;

	int		dummy;
	HANDLE	hTrans;
	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTransTrick, this, 0, (unsigned int *)&dummy);
	CloseHandle(hTrans);

	m_bFlag	= true;
	return true;
}

void FtpClient::FileTransTrick()
{
	__int64 nTransCompliteSize = m_nFileTransStart;
	__int64	nSendSize = 0;

	char		cDatabuf[8192];
	int			num_read = 0;
	int			num_send = 0;
	int			offset	 = 0;	
	int			nDelayTime = 1000;
	bool		bConnectSpeed = true;


	if(bConnectSpeed)
	{
		m_DateSock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_DateSock == INVALID_SOCKET)
		{
			closesocket(m_DateSock);
			bConnectSpeed = false;
		}
		else
		{
			m_addr_datachannel.sin_family = AF_INET;
			//20071123 _TEST 옵션일때 테스트 서버로 접속하게 수정, jyh
			m_addr_datachannel.sin_addr.S_un.S_addr = inet_addr((const char*)(LPSTR)(LPCTSTR)TESTSERVER_IP);
#ifndef _TEST
			m_addr_datachannel.sin_addr.S_un.S_addr = inet_addr((const char*)(LPSTR)(LPCTSTR)SERVER_IP_SPEED);
#endif			
			m_addr_datachannel.sin_port = htons(SERVER_PORT_SPEED);	

			if(connect(m_DateSock, (sockaddr*)&m_addr_datachannel, sizeof(m_addr_datachannel)) == SOCKET_ERROR)
			{
				if(connect(m_DateSock, (sockaddr*)&m_addr_datachannel, sizeof(m_addr_datachannel)) == SOCKET_ERROR)
				{
					closesocket(m_DateSock);
					bConnectSpeed = false;
				}
			}
		}
	}


	DWORD nConnectTime	= GetTickCount();
	
	//20080110 업로드 속도 변경을 위해, jyh
	__int64 nSendSizeNew = 0;

	while(m_bFlag && !m_pMainWindow->m_bForceExit) 
	{	

		if(bConnectSpeed)
		{
			while((nConnectTime + nDelayTime) > GetTickCount())			
			{
				if(!(num_read = m_pFileData.Read(cDatabuf, 8192))) 
				{
					break;
				}
				
				while(m_bFlag && !m_pMainWindow->m_bForceExit) 
				{
					num_send = send(m_DateSock, cDatabuf + offset, num_read, 0);

					if(num_send == SOCKET_ERROR) 
					{					
						if(WSAGetLastError() == WSAEWOULDBLOCK)
						{
							Sleep(50);
							continue;
						}
						else
						{				
							m_bFlag = false;
							break;
						}	
					}

					// 모두 보냈는지 체크 : 모두 전송되지 않았으면 남은 용량 전송
					else if(num_read != num_send)
					{			
						offset = num_send;
						num_read = num_read - num_send;
						continue;
					}	
					else if(num_read == num_send)
					{
						nSendSize += num_send;
					}

					offset = 0;
					break;
				}	
				CString test;
				test.Format("%I64d\n", nSendSize);
				//AfxMessageBox(test);
				//TRACE(test);
			}

			bConnectSpeed = false;
			closesocket(m_DateSock);

			/*if(nSendSize > 7200000)
				nSendSize = 7200000;*/
		}
		else
		{
			Sleep(nDelayTime);		

			/*if(nSendSize == 0)
				nSendSize = 820000;

			nTransCompliteSize += nSendSize;

			if(m_pMainWindow->m_pPtrTrans)
			m_pMainWindow->m_pPtrTrans->nFileSizeEnd += nSendSize;	

			if(nTransCompliteSize >= m_nFileSize)
			{
			if(m_pMainWindow->m_pPtrTrans)
			m_pMainWindow->m_pPtrTrans->nFileSizeEnd = m_nFileSize;	
			break;
			}*/

			//20080110 업로드 속도 변경, jyh
			//7MB 이상은 자기속도, 2MB~7MB는 곱하기 1.5배, 2MB이하는 2MB~2.5MB
			if(nSendSize < 7200000 && nSendSize > 2097152)
			{
				nSendSizeNew = (__int64)(nSendSize * 1.5);
			}
			else if(nSendSize <= 2097152)
			{
				//nSendSizeNew = 2609152;//ent24수정 업로드 속도 조정
				nSendSizeNew = 10200000;
				
			}
			else
				nSendSizeNew = nSendSize;
			
			srand((unsigned)time(NULL));
			int nSpeed = rand()%5;
			
			switch(nSpeed)
			{
			case 0:
				nSendSizeNew += 100000;
				break;
			case 1:
				nSendSizeNew += 50000;
				break;
			case 2:
				nSendSizeNew -= 50000;
				break;
			case 3:
				nSendSizeNew -= 10000;
				break;
			case 4:
				break;
			}
		}				
		
		nTransCompliteSize += nSendSizeNew;

		if(m_pMainWindow->m_pPtrTrans)
			m_pMainWindow->m_pPtrTrans->nFileSizeEnd += nSendSizeNew;	

		if(nTransCompliteSize >= m_nFileSize)
		{
			if(m_pMainWindow->m_pPtrTrans)
				m_pMainWindow->m_pPtrTrans->nFileSizeEnd = m_nFileSize;	
			break;
		}
	}	

	if(bConnectSpeed)
		closesocket(m_DateSock);

	m_pFileData.Close();
	m_bFlag = false;
}

unsigned int __stdcall ProcFileTrans(void *pParam)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	FtpClient* pProcess = (FtpClient*)pParam;
	pProcess->FileTrans();
	return 0;
}

unsigned int __stdcall ProcFileTransTrick(void *pParam)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	FtpClient* pProcess = (FtpClient*)pParam;
	pProcess->FileTransTrick();
	return 0;
}
