#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "d:\Project\Nfile����_vs8\Nfile\src_client\OpenDown_client\OpenDown_clientDlg.h"
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
	// ��������
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
// ���۹��� ��Ŷ ó��
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///////
void Worker::RecvData(Packet* pPacket)
{
	switch(pPacket->GetCode())
	{		
		// ���� ����
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
		// �������
		case CHK_AUTH:
		{
			Result_Login(pPacket);
			break;
		}
		//�ٿ�ε� ����Ʈ �޾ƿ�
		case OPEN_DOWNLIST:
		{
			Result_OpenDownLoadList(pPacket);
			break;
		}
		// ���� �ٿ�ε�
		case OPEN_DOWNLOAD:
		{
			Result_OpenDownLoadPro(pPacket);
			break;
		}
		// �ٿ�ε� �Ϸ� �뺸 ���
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
// �α��� ó��
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
				AfxMessageBox("�α��ο� ���� �߽��ϴ�! �������� �ʴ� ���̵� �Դϴ�.");
				m_pMainWnd->OnBnClickedCancel();
				break;
			}	

			default:
				break;
		}	
	}
	catch(...)
	{
		AfxMessageBox("�α��ο� ���� �߽��ϴ�!\r\n���� ������ ��ӵǸ� �����ڿ��� ������ �ֽʽÿ�.");
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٿ�ε� ����Ʈ �ޱ�
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

		//���� �����ޱ�� ������ �����.
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

				//������ �����.
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

			if(!lstrcmp(pOpendownList->szFolderName, "/"))	//����
			{
				pNewItem->nFileIndex = _atoi64(pToken2->GetToken(0));
				lstrcpy(pNewItem->pFileName, pToken2->GetToken(1));
				sprintf_s(pNewItem->pFullPath, 4096, "%s\\%s", m_pMainWnd->m_strDownPath, pNewItem->pFileName);
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(2));
				m_pMainWnd->m_nFileSizeTotal = pNewItem->nFileSizeReal;	//��ü ���� ����� ���Ѵ�.
				strBuf.Format("%s\\", m_pMainWnd->m_strDownPath);
				lstrcpy(pNewItem->pSavePath, strBuf.GetBuffer());
				strBuf.ReleaseBuffer();
			}
			else	//����
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
				m_pMainWnd->m_nFileSizeTotal += pNewItem->nFileSizeReal;	//��ü ���� ����� ���Ѵ�.
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

			//���� ��� ����
			m_pMainWnd->m_v_PtrTrans.push_back(*pNewItem);
			delete pNewItem;
		}

		delete pToken1;
		delete pToken2;

		//���� ����
		m_pMainWnd->OpenDownLoadPro();
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٿ�ε带 ���� �ٿ�ε� ���� ���� ��û
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

		// ���� �ٿ�ε� ����
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

				AfxMessageBox("���� ������ �� �� ���� ���α׷��� �����մϴ�!");
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
				// �˼� ���� ����
				case OPENDOWN_RESULT_ERROR:
				{
					AfxMessageBox("�˼� ���� ������ ���α׷��� �����մϴ�.");					
					break;
				}				
				// ������ ���� ���� ���� ����
				case OPENDOWN_RESULT_NOTFOUND:
				{
					AfxMessageBox("������ �������� �ʽ��ϴ�.");
					break;
				}
				// ���ϼ����� ���Ϸ� �ٿ�ε� �Ұ�(������)
				case OPENDOWN_RESULT_SERVICE_STOP:
				{
					AfxMessageBox("�˼��մϴ�!\r\n���� �ش����Ͽ� ���ϰ� �ʹ� �����ϴ�.\r\n����� �ٽ� �̿����ּ���.");
					break;
				}				
			}
		}
	}
	catch(...)
	{
		AfxMessageBox("�˼� ���� ������ ���α׷��� �����մϴ�.");
	}

	Packet* pPacketSend = new Packet;
	pPacketSend->Pack(OPEN_DOWNEND, (char*)szBuf, 32);
	SendPacket(pPacketSend);

	m_pMainWnd->OnBnClickedCancel();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٿ�ε� �Ϸ� �뺸 ���
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
// �ٸ� ��ǻ�Ϳ��� ������ �ٿ�ε� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void Worker::Result_Dupl_Close(Packet* pPacket)
//{
//	char pResult[2048];
//
//	::sprintf(pResult, "�ٸ� ��ҿ��� �α��εǾ� ������ �����˴ϴ�.\r\n\r\n[%s]", (char*)pPacket->GetData());
//	::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)0);
//}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ����/���� �̸�/���� üũ(Ư������ = \ / ? : " * < > ó���� \r\nó��)
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
// \ ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::SlashAdd(CString& text)
{
	text.Replace("'", "''");
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �޸� ����
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