#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "d:\Project\Nfile관련_vs8\Nfile\src_client\OpenUp_client\OpenUp_clientDlg.h"
#include "FtpClient.h"


Worker::Worker(COpenUp_clientDlg* pMainWnd) : NetworkController()
{
	m_nReConnectCount = 0;
	m_bReConnectTrans = false;

	m_bExitCall = false;
	m_pMainWnd = pMainWnd;
	InitializeCriticalSection(&m_pSendSection);
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
		// 업로드 목록 요청
		case OPEN_UPLIST:
		{
			Result_OpenUpLoadList(pPacket);
			break;
		}
		// 업로드 과정
		case OPEN_UPLOAD:
		{
			Result_OpenUpLoadPro(pPacket);
			break;
		}
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
	try
	{
		PINFOAUTH pLogin = (PINFOAUTH)pPacket->GetData();

		switch(pLogin->nResult)
		{
			//로긴 성공
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
// 업로드 리스트 받기
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_OpenUpLoadList(Packet* pPacket)
{
	CString strBuf;
	POPENUPLIST pOpenUpList = (POPENUPLIST)pPacket->GetData();

	if(pOpenUpList->nResult == OPENUP_RESULT_OK)
	{
		CToken*	pToken1 = new CToken(SEP_ITEM);
		CToken*	pToken2 = new CToken(SEP_FIELD);

		//폴더 내려받기면 각 파일의 폴더 조합을 위해 폴더별로 벡터에 저장한다.
		CString strFolderName;
		std::vector<CString>	v_strFolderName;
		bool bAllow = true;		//파일 확장자를 체크 해서 허용된 파일이면 true, 아니면 false
		
		if(lstrcmp(pOpenUpList->szFolderName, "/"))
		{
			CToken*	pToken3 = new CToken(SEP_FIELD);
			pToken3->Split(pOpenUpList->szFolderName);

			for(int i=0; i<(int)pToken3->GetCount(); i++)
			{
				CString token = pToken3->GetToken(i);
				token.Replace("/", "\\");
				v_strFolderName.push_back(token);
			}
			
			delete pToken3;
		}

		pToken1->Split(pOpenUpList->szContentsInfo);
		int nDepth;		//폴더 depth 종류

		for(size_t i=0; i<pToken1->GetCount(); i++)
		{
			pToken2->Split(pToken1->GetToken(i));

			PROPENUPCTX pNewItem = new ROPENUPCTX;
			ZeroMemory(pNewItem, sizeof(ROPENUPCTX));

			pNewItem->nFileIndex = _atoi64(pToken2->GetToken(0));
			
			int nPos = 0;
			CString	filename;

			if(!lstrcmp(pOpenUpList->szFolderName, "/"))
			{
				//파일
				strBuf = pToken2->GetToken(1);
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(2));
				m_pMainWnd->m_nFileSizeTotal += pNewItem->nFileSizeReal;	//전체 파일 사이즈를 구한다.
				lstrcpy(pNewItem->pFileName, strBuf);
				sprintf_s(pNewItem->pFullPath, 4096, "%s%s", m_pMainWnd->m_strUpPath.GetBuffer(), pNewItem->pFileName);
				m_pMainWnd->m_strUpPath.ReleaseBuffer();
			}
			else
			{
				//폴더
				nDepth = atoi(pToken2->GetToken(1));
				filename = pToken2->GetToken(2);
				lstrcpy(pNewItem->pFileName, filename.GetBuffer());
                filename.ReleaseBuffer();
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(3));
				m_pMainWnd->m_nFileSizeTotal += pNewItem->nFileSizeReal;	//전체 파일 사이즈를 구한다.
				strBuf.Format("%s%s%s", m_pMainWnd->m_strUpPath, v_strFolderName[nDepth-1], filename);
				lstrcpy(pNewItem->pFullPath, strBuf.GetBuffer());
				strBuf.ReleaseBuffer();
			}

			char szTemp[2048];

			//파일 확장자 체크해서 업로드 허용 파일만 업로드 한다.
			strBuf = pNewItem->pFileName;
			CString strExtension = strBuf.Right(3);
			strExtension = strExtension.MakeLower();
			//AfxMessageBox(strExtension);

			if(strExtension == "zip" || strExtension == "alz" || strExtension == "rar" || strExtension == "exe" ||
				strExtension == "com" || strExtension == "txt" || strExtension == "hwp" || strExtension == "doc" ||
				strExtension == "smi" || strExtension == "ims" || strExtension == "hlp" || strExtension == "mid" ||
				strExtension == "bin" || strExtension == "dll" || strExtension == "msi" || strExtension == "avi" ||
				strExtension == "wma" || strExtension == "wav" || strExtension == "iso" || strExtension == "lcd" ||
				strExtension == "img" || strExtension == "bmp" || strExtension == "jpg" || strExtension == "tif" ||
				strExtension == "pic" || strExtension == "gif")
			{
				//AfxMessageBox("전송 가능!");
				bAllow = true;
			}
			else
			{
				//AfxMessageBox("전송 불가!");
				bAllow = false;
				break;
			}

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

		if(bAllow)
		{
			//전송 시작
			m_pMainWnd->OpenUpLoadPro();
		}
		else
		{
			AfxMessageBox("업로드가 허용된 파일이 아닙니다! 프로그램을 종료합니다.");
			m_pMainWnd->OnBnClickedCancel();
		}
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 업로드 처리 과정
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_OpenUpLoadPro(Packet* pPacket)
{
	//int bReTransTime = 30000;
	char szBuf[32];
	ZeroMemory(szBuf, 32);

	try
	{	
		POPENUPLOAD pOpenUpPro = (POPENUPLOAD)pPacket->GetData();

		if(m_pMainWnd->m_pPtrTrans == NULL)
		{
			m_pMainWnd->OpenUpLoadPro();
			return;
		}

		// 실제 업로드 구현
		if(pOpenUpPro->nResult == OPENUP_RESULT_OK)
		{
			bool bResult = false;
		
			m_pMainWnd->m_pPtrTrans->nFileSizeStart = pOpenUpPro->nFileSizeStart;
			m_pMainWnd->m_pPtrTrans->nFileSizeEnd	= pOpenUpPro->nFileSizeStart;

			Sleep(500);		//20071122 smi파일(저용량 파일) 안올라가는 거 수정, jyh

			bResult = m_pMainWnd->m_pFtpClint->UpLoad(pOpenUpPro->szServerInfo, m_pMainWnd->m_pPtrTrans->pFullPath, pOpenUpPro->nFileSizeStart);
			
			if(!bResult)
			{
				Packet* pPacketSend = new Packet;
				pPacketSend->Pack(OPEN_UPEND, (char*)szBuf, 32);
				SendPacket(pPacketSend);

				AfxMessageBox("파일 전송을 할 수 없어 프로그램을 종료합니다!");
				m_pMainWnd->OnBnClickedCancel();
			}

			// 상태출력 쓰레드 시작
			int		nDummy;
			HANDLE	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTransLoop, m_pMainWnd, 0, (unsigned int *)&nDummy);
			CloseHandle(hTrans);	

			m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_TRANS;
			return;
		}
		else
		{
			switch(pOpenUpPro->nResult)
			{			
				case OPENUP_RESULT_ERROR:
				{
					AfxMessageBox("알수 없는 에러로 프로그램을 종료합니다.");
					break;
				}
				case OPENUP_RESULT_NOTFOUND:
				{
					AfxMessageBox("파일이 존재하지 않습니다.");
					break;
				}
				case OPENUP_RESULT_SERVICE_STOP:
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
	pPacketSend->Pack(OPEN_UPEND, (char*)szBuf, 32);
	SendPacket(pPacketSend);
	m_pMainWnd->OnBnClickedCancel();
}


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