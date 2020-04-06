#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "d:\Project\Nfile관련_vs8\Nfile\src_client\OpenDown_client\OpenDown_clientDlg.h"
#include "FtpClient.h"




Worker::Worker(COpenDown_clientDlg* pMainWnd) : NetworkController()
{
	m_nReConnectCount = 0;
	m_bReConnectTrans = false;
	
	m_bExitCall = false;
	m_pMainWnd = pMainWnd;
	InitializeCriticalSection(&m_pSendSection);

	//m_pDegDlg		= new DebugLIst();
	//m_pDegDlg->Create(IDD_DIALOG1);
	//m_pDegDlg->ShowWindow(SW_SHOWNORMAL);
}				

				
Worker::~Worker(void)
{
}


bool Worker::Init(char* pServerIp, unsigned int nServerPort)
{
	// 서버정보
	strcpy_s(m_pServerIp, strlen(pServerIp)+1, pServerIp);
	m_nServerPort = nServerPort;

	return MMSocketConnectTCP();	
}

void Worker::End()
{
	m_bExitCall = true;

	MMServerFree();	
	DeleteCriticalSection(&m_pSendSection);	
}


void Worker::SendPacket(Packet* packet)
{
	if(!m_bConnectServer) 
	{
		SAFE_DELETE(packet);
		return;
	}

	EnterCriticalSection(&m_pSendSection);
	MMCheckSend(packet);	
	LeaveCriticalSection(&m_pSendSection);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///////
// 전송받은 패킷 처리
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///////
void Worker::RecvData(Packet* pPacket)
{
	switch(pPacket->GetCode())
	{		
		// 연결 끊김
		case CLOSE_CLIENT:
		{
			if(!m_bExitCall)
			{
				if(m_pMainWnd->m_bAuth)
				{
					MMServerFree();
					m_pMainWnd->KillTimer(100);
				}
            }
			break;
		}		
		// 인증결과
		case CHK_AUTH:
		{
			Result_Login(pPacket);
			break;
		}
		//다운로드 리스트 받아옴
		case OPEN_DOWNLIST:
		{
			Result_OpenDownLoadList(pPacket);
			break;
		}
		// 실제 다운로드
		case OPEN_DOWNLOAD:
		{
			Result_OpenDownLoadPro(pPacket);
			break;
		}
		// 다운로드 완료 통보 결과
		/*case OPEN_DOWNEND:
		{
			Result_OpenDownLoadEnd(pPacket);
			break;
		}*/
		
		default:
		{
			break;
		}
	}

	SAFE_DELETE(pPacket);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 로그인 처리
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Login(Packet* pPacket)
{
	PINFOAUTH pLogin = (PINFOAUTH)pPacket->GetData();


	try
	{
		switch(pLogin->nResult)
		{			
			case LOGIN_RESULT_AUTH:
			{
				m_pMainWnd->m_bAuth	= true;
				m_pMainWnd->SetTimer(2, 10, NULL);		
				break;
			}
			case LOGIN_RESULT_FAIL_ID:
			{
				AfxMessageBox("로그인에 실패 했습니다! 존재하지 않는 아이디 입니다.");
				m_pMainWnd->OnBnClickedCancel();
				break;
			}	

			default:
				break;
		}	
	}
	catch(...)
	{
		AfxMessageBox("로그인에 실패 했습니다!\r\n같은 문제가 계속되면 관리자에게 문의해 주십시오.");
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다운로드 리스트 받기
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_OpenDownLoadList(Packet* pPacket)
{
	CString strBuf;
	POPENDOWNLIST pOpendownList = (POPENDOWNLIST)pPacket->GetData();

	if(pOpendownList->nResult == OPENDOWN_RESULT_OK)
	{
		CToken*	pToken1 = new CToken(SEP_ITEM);
		CToken*	pToken2 = new CToken(SEP_FIELD);

		pToken1->Split(pOpendownList->szContentsInfo);

		//폴더 내려받기면 폴더를 만든다.
		CString strFolderName;
		std::vector<CString>	v_strFolderName;
				
		if(lstrcmp(pOpendownList->szFolderName, "/"))
		{
			CToken*	pToken3 = new CToken(SEP_FIELD);
			pToken3->Split(pOpendownList->szFolderName);
			
			for(int i=0; i<(int)pToken3->GetCount(); i++)
			{
				CString token = pToken3->GetToken(i);
				token.Replace("/", "\\");
				v_strFolderName.push_back(token);
				strBuf = m_pMainWnd->m_strDownPath;
				int nPos = 0;

				//폴더를 만든다.
				while(nPos < token.GetLength())
				{
					strFolderName = token.Tokenize("\\", nPos);
					strBuf += "\\" + strFolderName;
					::CreateDirectory(strBuf.GetBuffer(), NULL);
					strBuf.ReleaseBuffer();
				}
			}

			delete pToken3;
		}		

		for(size_t i=0; i<pToken1->GetCount(); i++)
		{
			pToken2->Split(pToken1->GetToken(i));

			PROPENDOWNCTX pNewItem = new ROPENDOWNCTX;
			ZeroMemory(pNewItem, sizeof(ROPENDOWNCTX));

			if(!lstrcmp(pOpendownList->szFolderName, "/"))	//파일
			{
				pNewItem->nFileIndex = _atoi64(pToken2->GetToken(0));
				lstrcpy(pNewItem->pFileName, pToken2->GetToken(1));
				sprintf_s(pNewItem->pFullPath, 4096, "%s\\%s", m_pMainWnd->m_strDownPath, pNewItem->pFileName);
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(2));
				m_pMainWnd->m_nFileSizeTotal = pNewItem->nFileSizeReal;	//전체 파일 사이즈를 구한다.
				strBuf.Format("%s\\", m_pMainWnd->m_strDownPath);
				lstrcpy(pNewItem->pSavePath, strBuf.GetBuffer());
				strBuf.ReleaseBuffer();
			}
			else	//폴더
			{
				pNewItem->nFileIndex = _atoi64(pToken2->GetToken(0));
				strBuf = m_pMainWnd->m_strDownPath;
				int nDepth = atoi(pToken2->GetToken(1));
				strBuf += "\\" + v_strFolderName[nDepth-1];
				lstrcpy(pNewItem->pFileName, pToken2->GetToken(2));
				strBuf.Format("%s%s", strBuf, pNewItem->pFileName);
				lstrcpy(pNewItem->pFullPath, strBuf.GetBuffer());
				strBuf.ReleaseBuffer();
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(3));
				m_pMainWnd->m_nFileSizeTotal += pNewItem->nFileSizeReal;	//전체 파일 사이즈를 구한다.
				strBuf.Format("%s\\", m_pMainWnd->m_strDownPath);
				lstrcpy(pNewItem->pSavePath, strBuf.GetBuffer());
				strBuf.ReleaseBuffer();
			}
			
			char szTemp[4096];
			lstrcpy(szTemp, pNewItem->pFullPath);
			int nFullPathlen = lstrlen(pNewItem->pFullPath);
			int nFilenamelen = lstrlen(pNewItem->pFileName);
			szTemp[nFullPathlen-nFilenamelen] = '\0';
			lstrcpy(pNewItem->pFolderName, szTemp);
			pNewItem->nFileSizeStart = 0;
			pNewItem->nFileSizeEnd = 0;

			//파일 목록 저장
			m_pMainWnd->m_v_PtrTrans.push_back(*pNewItem);
			delete pNewItem;
		}

		delete pToken1;
		delete pToken2;

		//전송 시작
		m_pMainWnd->OpenDownLoadPro();
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다운로드를 위해 다운로드 서버 정보 요청
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_OpenDownLoadPro(Packet* pPacket)
{
	char szBuf[32];
	ZeroMemory(szBuf, 32);

	try
	{
		POPENDOWNLOAD pOpenDownPro = (POPENDOWNLOAD)pPacket->GetData();

		if(m_pMainWnd->m_pPtrTrans == NULL)
		{
			m_pMainWnd->OpenDownLoadPro();
			return;
		}

		// 실제 다운로드 구현
		if(pOpenDownPro->nResult == OPENDOWN_RESULT_OK)
		{
			m_pMainWnd->m_pPtrTrans->nFileSizeStart	= pOpenDownPro->nDownStartPoint;
			m_pMainWnd->m_pPtrTrans->nFileSizeEnd	= pOpenDownPro->nDownStartPoint;

			if(!m_pMainWnd->m_pFtpClint->DownLoad(pOpenDownPro->szServerInfo, 
				m_pMainWnd->m_pPtrTrans->pFullPath, pOpenDownPro->nDownStartPoint))
			{
				Packet* pPacketSend = new Packet;
				pPacketSend->Pack(OPEN_DOWNEND, (char*)szBuf, 32);
				SendPacket(pPacketSend);

				AfxMessageBox("파일 전송을 할 수 없어 프로그램을 종료합니다!");
				m_pMainWnd->OnBnClickedCancel();
            }

			int		nDummy;
			HANDLE	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTransLoop, m_pMainWnd, 0, (unsigned int *)&nDummy);
			CloseHandle(hTrans);	

			m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_TRANS;
			return;
		}
		else
		{
			switch(pOpenDownPro->nResult)
			{				
				// 알수 없는 에러
				case OPENDOWN_RESULT_ERROR:
				{
					AfxMessageBox("알수 없는 에러로 프로그램을 종료합니다.");					
					break;
				}				
				// 파일이 존재 하지 않은 상태
				case OPENDOWN_RESULT_NOTFOUND:
				{
					AfxMessageBox("파일이 존재하지 않습니다.");
					break;
				}
				// 파일서버의 부하로 다운로드 불가(대기상태)
				case OPENDOWN_RESULT_SERVICE_STOP:
				{
					AfxMessageBox("죄송합니다!\r\n현재 해당파일에 부하가 너무 많습니다.\r\n잠시후 다시 이용해주세요.");
					break;
				}				
			}
		}
	}
	catch(...)
	{
		AfxMessageBox("알수 없는 에러로 프로그램을 종료합니다.");
	}

	Packet* pPacketSend = new Packet;
	pPacketSend->Pack(OPEN_DOWNEND, (char*)szBuf, 32);
	SendPacket(pPacketSend);

	m_pMainWnd->OnBnClickedCancel();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다운로드 완료 통보 결과
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void Worker::Result_OpenDownLoadEnd(Packet* pPacket)
//{
//	try
//	{
//		PCHECKDOWNEND pDownEnd = (PCHECKDOWNEND)pPacket->GetData();
//	}
//	catch(...)
//	{
//	}
//}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다른 컴퓨터에서 정액제 다운로드 시작
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void Worker::Result_Dupl_Close(Packet* pPacket)
//{
//	char pResult[2048];
//
//	::sprintf(pResult, "다른 장소에서 로그인되어 접속이 해제됩니다.\r\n\r\n[%s]", (char*)pPacket->GetData());
//	::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)0);
//}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 폴더/파일 이름/정보 체크(특수문자 = \ / ? : " * < > 처리와 \r\n처리)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::SpecialText(CString& text)
{
	text.Trim();
	text.Replace("\r\n", " ");

	if(text == "..")
		text = ",,";

	text.Remove('\\');
	text.Remove('/');
	text.Remove('?');
	text.Remove(':');
	text.Remove('"');
	text.Remove('*');
	text.Remove('<');
	text.Remove('>');
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// \ 삽입
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::SlashAdd(CString& text)
{
	text.Replace("'", "''");
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 콤마 삽입
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::AddComma(CString& text, __int64 size)
{
	text.Format("%I64d", size);
	
	int nLength = text.GetLength();
	int nPos;

    if( nLength<0 )
        return;

	nPos = nLength;
    while(text.GetAt(0)=='-' ? nPos>4 : nPos>3 )
    {
        text.Insert( nPos-3, ',' );
        nPos = text.Find(',');
    }
}