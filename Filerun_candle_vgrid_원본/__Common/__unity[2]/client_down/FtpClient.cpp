#include "StdAfx.h"
#include "FtpClient.h"
#include <io.h>
#include "v_down.h"
#include <afxwin.h>
#pragma comment(lib, "../../../Default Include/Vgrid/v_down.lib")


FtpClient::FtpClient()
{
	m_bRunThread = false;

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
	m_DateSock = INVALID_SOCKET;
	m_bFlag = false;
}


bool FtpClient::DownLoadEx(char* pServerInfo, char* pFilePath, __int64 nFileStartSize)
{
	//bool Grid_use=false;

	int nCheckIndex;

	CString strServer;
	strServer.Empty();
	strServer.Format("%s",pServerInfo);

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



	//m_pMainWindow->m_pPtrTrans->Grid_use = false;

	if( m_pMainWindow->m_pPtrTrans->Grid_use == true )
	{
		int		dummy;
		HANDLE	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTransEx, this, 0, (unsigned int *)&dummy);
		CloseHandle(hTrans);

		m_bFlag	= true;
		return true;
	}


	return DownLoad(pServerInfo,pFilePath,nFileStartSize);
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

		char pBuf[2048];
		sprintf_s(pBuf, 2048, "파일열기에 실패했습니다.\r\n다른 프로그램에서 파일을 사용중일 가능성이 높습니다.\r\nPath(O) : %s", m_pFilePath);
		::SendMessage(m_pMainWindow->m_hWnd, MSG_ALERTWINDOW, (WPARAM)(char*)pBuf, 1);
		return false;
	}
	else if(m_nFileTransStart > 0)
		m_pFileData.Seek(m_nFileTransStart, CFile::begin); 

	return true;
}


void FtpClient::FileTransEx()
{	
	int n_SetPeer=0;

	LPSTR strSIP=m_ServerIP.GetBuffer(m_ServerIP.GetLength());
	m_ServerIP.ReleaseBuffer();

	CString m_strTargetPath;
	m_strTargetPath.Format("%s",m_pMainWindow->m_pPtrTrans->pServer_real_FileName);
	LPSTR strTargetPath=m_strTargetPath.GetBuffer(m_strTargetPath.GetLength());
	m_strTargetPath.ReleaseBuffer();

	if( 0x00 == m_pMainWindow->m_pPtrTrans) return;


	//파일명 추출
	CString file_md5,file_load2,file_load;
	int target_pos;
	file_load=m_pMainWindow->m_pPtrTrans->pServer_real_FileName;

	if(!file_load.GetLength())	return;

	target_pos=file_load.Find("/",file_load.Find("/") + 1);
	file_load2=file_load.Right(file_load.GetLength()-target_pos-1);
	file_md5=file_load2.Left(file_load2.Find("_"));


	InitNodeLib("filerun-0000-0000");
	SetDownMainSvrInfo(strSIP,62120,5);

	SetDownloadFileEx(
					(m_pMainWindow->m_pPtrTrans->pFolderName), 
					((LPSTR)(LPCSTR)m_strTargetPath), 
					(m_pMainWindow->m_pPtrTrans->pFileName), 
					m_pMainWindow->m_pPtrTrans->nFileSizeReal,
					(LPSTR)(LPCSTR)file_md5,
					10
				);

	int nPeer = OnStart_Download(1);

	//그리드 다운로드 실패시
	if(  nPeer < 0)
	{
		OnStop_Download();

		m_pMainWindow->m_pPtrTrans->Grid_use = false ; 	

		if(m_pMainWindow->m_nSvrResult == DOWN_RESULT_OK)
		{

			m_pMainWindow->m_pFtpClint->DownLoad(m_pMainWindow->m_pPtrTrans->m_pServerInfo, 
				m_pMainWindow->m_pPtrTrans->pFullPath, m_pMainWindow->m_pPtrTrans->nFileSizeEnd);
		}
		else if(m_pMainWindow->m_nSvrResult == DOWN_RESULT_SERVERSTOP)
		{
			::SendMessage(m_pMainWindow->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"파일서버의 부하로 본 파일의 다운로드는 나중에 다시 시도해주십시요", (LPARAM)1);
			m_pMainWindow->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
			m_pMainWindow->SetTimer(98, 500, NULL);	

			m_pMainWindow->m_pPtrTrans = NULL;	
			m_pMainWindow->SetTransFlag(false);
		}
		else if(m_pMainWindow->m_nSvrResult == DOWN_RESULT_SERVERBUSY)
		{

			::SendMessage(m_pMainWindow->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"파일서버의 부하로 본 파일의 다운로드는 나중에 다시 시도해주십시요", (LPARAM)1);
			m_pMainWindow->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
			m_pMainWindow->SetTimer(98, 500, NULL);	

			m_pMainWindow->m_pPtrTrans = NULL;	
			m_pMainWindow->SetTransFlag(false);

		}else
		{

			m_pMainWindow->m_pPtrTrans = NULL;	
			m_pMainWindow->SetTransFlag(false);

			return;
		}

		return ;
	}

	m_pMainWindow->m_nGridSpeed = 0;
	m_pMainWindow->m_nGridSpeed_mem = 0;




	int nCheck=0;
	__int64	&nSizePtr = m_pMainWindow->m_pPtrTrans->nFileSizeEnd;
	m_pMainWindow->ReSetTransInfo();

	//현재 사이즈
	nSizePtr =  (__int64)GetStartPoint();

	//속도저장용
	double speed_check;


	while( m_bFlag )
	{
		int nSTATUS = GetStatus();
		

		//다운로드 준비중/다운로드중
		if (nSTATUS == 0 || nSTATUS == 1 )
		{
			speed_check=(double)GetSpeed()/(1024);
			if(speed_check<0)	speed_check=0;
			m_pMainWindow->m_nGridSpeed = speed_check;
			nSizePtr =  (__int64)GetValidDownSize()+(__int64)GetStartPoint();


		}
		else
		{
			//다운로드 오류
			if(nSTATUS == DOWN_ERROR)
			{
				m_bFlag = false;
				OnStop_Download();
				m_pMainWindow->m_pPtrTrans->Grid_use = false;

				return;
			}


			speed_check=(double)GetSpeed()/(1024);
			if(speed_check<0)	speed_check=0;
			m_pMainWindow->m_nGridSpeed = speed_check;

			if (nSTATUS == DOWN_PAUSE )
			{
				nSizePtr =  GetValidDownSize();
				m_pMainWindow->m_pPtrTrans->nFileSizeEnd = nSizePtr;

				m_bFlag = false; 
				OnStop_Download();

				return;
			}
			else
			{
				nSizePtr = GetValidDownSize();

				if(nSTATUS == DOWN_FINISH)
				{
					m_bFlag=false;
					m_pMainWindow->m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;
					m_pMainWindow->m_pPtrTrans->Grid_use = true;
					m_pMainWindow->m_pPtrTrans->nFileSizeEnd = nSizePtr;
					OnStop_Download();
					return;
				}
			}

		}
		Sleep(1000);
	}

	Sleep(1000);

	if( !m_bFlag )
	{
//		nSizePtr =  _ReceiveSize();
		m_pMainWindow->ReSetTransInfo();
		OnStop_Download();
	}

















/*
	while( m_bFlag )
	{
		int nSTATUS = _Status();

		if (nSTATUS == SENDING_GRID || nSTATUS == FILE_EXIST_GRID || nSTATUS == READY_GRID  ||  nSTATUS == CONNECTING_GRID )
		{
			int nRemainTime;

			m_pMainWindow->m_nGridSpeed = _Speed(&nRemainTime);
			nSizePtr =  _ReceiveSize();

		}
		else
		{

			if(nSTATUS == FILE_INIT_GRID)
			{

				OnStop_Download();

				m_pMainWindow->SetTransFlag(false);
				Sleep(500);
				m_pMainWindow->SetTransFlag(true);

				m_bFlag = FALSE;

				break;
			}

			int nRemainTime;

			m_pMainWindow->m_nGridSpeed = _Speed(&nRemainTime);

			if (nSTATUS == ABORT_GRID || nSTATUS == SERVER_FAIL_GRID ||  nSTATUS == FILE_WRITE_GRID || nSTATUS == SPACELACK_GRID )
			{
				//nSizePtr = _GetCurrentFileSize();
				nSizePtr =  _ReceiveSize();
				m_pMainWindow->m_pPtrTrans->nFileSizeEnd = nSizePtr;

				m_bFlag = false; 
				OnStop_Download();



				if( nSTATUS == SERVER_FAIL_GRID || nSTATUS == FILE_WRITE_GRID || nSTATUS == SPACELACK_GRID || nSTATUS == FILE_INIT_GRID )
				{
					m_pMainWindow->m_pPtrTrans->Grid_use = false;
				}

				return;
			}
			else
			{
				//nSizePtr =  _ReceiveSize();
				nSizePtr = _GetCurrentFileSize();

				if(nSTATUS == COMPLETE_GRID)
				{
					m_bFlag=false;
					m_pMainWindow->m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;
					m_pMainWindow->m_pPtrTrans->Grid_use = true;
					m_pMainWindow->m_pPtrTrans->nFileSizeEnd = nSizePtr;
					OnStop_Download();
					return;
				}
			}

		}
		Sleep(200);
	}

	Sleep(300);

	if( !m_bFlag )
	{
//		nSizePtr =  _ReceiveSize();
		m_pMainWindow->ReSetTransInfo();
		OnStop_Download();
	}
*/
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
				//20090217 수정, jyh-----------------------------------
				m_pMainWindow->m_nFileSizeEnd += nRecvSize;

				if(nFlushCnt%50 == 0 || nSizePtr == m_pMainWindow->m_pPtrTrans->nFileSizeReal)
				{
					//현재 파일 전송 프로그레스
					int nCurPercent = abs((int)(nSizePtr * 1000000 / m_pMainWindow->m_pPtrTrans->nFileSizeReal));


					CString strBuf;
//					m_pMainWindow->m_pCtrl_ProgressCur.SetPos(nCurPercent);
					m_pMainWindow->m_pCtrl_ProgressCur.SetPos(nCurPercent/10000);
					
					/*strBuf.Format("%s / %s (%d%%)", m_pMainWindow->SetSizeFormat(nSizePtr),
								m_pMainWindow->SetSizeFormat(m_pMainWindow->m_pPtrTrans->nFileSizeReal), nPercent2/10000);*/
					CString strCurSize, strSize;
					if((nSizePtr/1024)/1000000)
						strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((nSizePtr/1024)/1000000)%1000, ((nSizePtr/1024)/1000)%1000, (nSizePtr/1024)%1000);
					else
					{
						if((nSizePtr/1024)/1000)
							strCurSize.Format("%I64d,%03I64dKB", (nSizePtr/1024)/1000, (nSizePtr/1024)%1000);
						else
						{
							if(nSizePtr/1024)
								strCurSize.Format("%I64dKB", nSizePtr/1024);
							else
								strCurSize.Format("%I64dB", nSizePtr);
						}
					}
					
					if((m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)/1000000)
						strSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)/1000000)%1000,
																((m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)/1000)%1000,
																(m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)%1000);
					else
					{
						if((m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)/1000)
							strSize.Format("%I64d,%03I64dKB", (m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)/1000,
															(m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)%1000);
						else
						{
							if(m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024)
								strSize.Format("%I64dKB", m_pMainWindow->m_pPtrTrans->nFileSizeReal/1024);
							else
								strSize.Format("%I64dB", m_pMainWindow->m_pPtrTrans->nFileSizeReal);
						}
					}
					
					strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, nCurPercent/10000);
					//strBuf.Format("%s(%d%%)", strCurSize, nCurPercent/10000);
					m_pMainWindow->m_pCtrl_ProgressCur.SetText(strBuf);

					//전체파일 전송 프로그레스
					int nTotalPercent = abs((int)((m_pMainWindow->m_nFileSizeEnd * 1000000) / m_pMainWindow->m_nFileSizeTotal));		
					//m_pMainWindow->m_pCtrl_ProgressTotal.SetPos(nTotalPercent);
					m_pMainWindow->m_pCtrl_ProgressTotal.SetPos(nTotalPercent/10000);
					

					if((m_pMainWindow->m_nFileSizeEnd/1024)/1000000)
						strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWindow->m_nFileSizeEnd/1024)/1000000)%1000,
																	((m_pMainWindow->m_nFileSizeEnd/1024)/1000)%1000,
																	(m_pMainWindow->m_nFileSizeEnd/1024)%1000);
					else
					{
						if((m_pMainWindow->m_nFileSizeEnd/1024)/1000)
							strCurSize.Format("%I64d,%03I64dKB", (m_pMainWindow->m_nFileSizeEnd/1024)/1000,
																(m_pMainWindow->m_nFileSizeEnd/1024)%1000);
						else
						{
							if(m_pMainWindow->m_nFileSizeEnd/1024)
								strCurSize.Format("%I64dKB", m_pMainWindow->m_nFileSizeEnd/1024);
							else
								strCurSize.Format("%I64dB", m_pMainWindow->m_nFileSizeEnd);
						}
					}

					if((m_pMainWindow->m_nFileSizeTotal/1024)/1000000)
						strSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWindow->m_nFileSizeTotal/1024)/1000000)%1000,
																((m_pMainWindow->m_nFileSizeTotal/1024)/1000)%1000,
																(m_pMainWindow->m_nFileSizeTotal/1024)%1000);
					else
					{
						if((m_pMainWindow->m_nFileSizeTotal/1024)/1000)
							strSize.Format("%I64d,%03I64dKB", (m_pMainWindow->m_nFileSizeTotal/1024)/1000,
															(m_pMainWindow->m_nFileSizeTotal/1024)%1000);
						else
						{
							if((m_pMainWindow->m_nFileSizeTotal/1024))
								strSize.Format("%I64dKB", m_pMainWindow->m_nFileSizeTotal/1024);
							else
								strSize.Format("%I64dB", m_pMainWindow->m_nFileSizeTotal);
						}
					}
					
					//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
					strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, nTotalPercent/10000);
					m_pMainWindow->m_pCtrl_ProgressTotal.SetText(strBuf);
				}
				//-----------------------------------------------------------------
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


unsigned int __stdcall ProcFileTransEx(void *pParam)
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	FtpClient* pProcess = (FtpClient*)pParam;

	pProcess->m_bRunThread = true;
	pProcess->FileTransEx();
	pProcess->m_bRunThread = false;

	return 0;
}


