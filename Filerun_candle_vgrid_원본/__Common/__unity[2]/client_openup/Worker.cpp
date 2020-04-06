#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "d:\Project\Nfile����_vs8\Nfile\src_client\OpenUp_client\OpenUp_clientDlg.h"
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
		// ���ε� ��� ��û
		case OPEN_UPLIST:
		{
			Result_OpenUpLoadList(pPacket);
			break;
		}
		// ���ε� ����
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
// �α��� ó��
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Login(Packet* pPacket)
{
	try
	{
		PINFOAUTH pLogin = (PINFOAUTH)pPacket->GetData();

		switch(pLogin->nResult)
		{
			//�α� ����
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
// ���ε� ����Ʈ �ޱ�
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_OpenUpLoadList(Packet* pPacket)
{
	CString strBuf;
	POPENUPLIST pOpenUpList = (POPENUPLIST)pPacket->GetData();

	if(pOpenUpList->nResult == OPENUP_RESULT_OK)
	{
		CToken*	pToken1 = new CToken(SEP_ITEM);
		CToken*	pToken2 = new CToken(SEP_FIELD);

		//���� �����ޱ�� �� ������ ���� ������ ���� �������� ���Ϳ� �����Ѵ�.
		CString strFolderName;
		std::vector<CString>	v_strFolderName;
		bool bAllow = true;		//���� Ȯ���ڸ� üũ �ؼ� ���� �����̸� true, �ƴϸ� false
		
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
		int nDepth;		//���� depth ����

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
				//����
				strBuf = pToken2->GetToken(1);
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(2));
				m_pMainWnd->m_nFileSizeTotal += pNewItem->nFileSizeReal;	//��ü ���� ����� ���Ѵ�.
				lstrcpy(pNewItem->pFileName, strBuf);
				sprintf_s(pNewItem->pFullPath, 4096, "%s%s", m_pMainWnd->m_strUpPath.GetBuffer(), pNewItem->pFileName);
				m_pMainWnd->m_strUpPath.ReleaseBuffer();
			}
			else
			{
				//����
				nDepth = atoi(pToken2->GetToken(1));
				filename = pToken2->GetToken(2);
				lstrcpy(pNewItem->pFileName, filename.GetBuffer());
                filename.ReleaseBuffer();
				pNewItem->nFileSizeReal = _atoi64(pToken2->GetToken(3));
				m_pMainWnd->m_nFileSizeTotal += pNewItem->nFileSizeReal;	//��ü ���� ����� ���Ѵ�.
				strBuf.Format("%s%s%s", m_pMainWnd->m_strUpPath, v_strFolderName[nDepth-1], filename);
				lstrcpy(pNewItem->pFullPath, strBuf.GetBuffer());
				strBuf.ReleaseBuffer();
			}

			char szTemp[2048];

			//���� Ȯ���� üũ�ؼ� ���ε� ��� ���ϸ� ���ε� �Ѵ�.
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
				//AfxMessageBox("���� ����!");
				bAllow = true;
			}
			else
			{
				//AfxMessageBox("���� �Ұ�!");
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

			//���� ��� ����
			m_pMainWnd->m_v_PtrTrans.push_back(*pNewItem);
			delete pNewItem;			
		}

		delete pToken1;
		delete pToken2;

		if(bAllow)
		{
			//���� ����
			m_pMainWnd->OpenUpLoadPro();
		}
		else
		{
			AfxMessageBox("���ε尡 ���� ������ �ƴմϴ�! ���α׷��� �����մϴ�.");
			m_pMainWnd->OnBnClickedCancel();
		}
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���ε� ó�� ����
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

		// ���� ���ε� ����
		if(pOpenUpPro->nResult == OPENUP_RESULT_OK)
		{
			bool bResult = false;
		
			m_pMainWnd->m_pPtrTrans->nFileSizeStart = pOpenUpPro->nFileSizeStart;
			m_pMainWnd->m_pPtrTrans->nFileSizeEnd	= pOpenUpPro->nFileSizeStart;

			Sleep(500);		//20071122 smi����(���뷮 ����) �ȿö󰡴� �� ����, jyh

			bResult = m_pMainWnd->m_pFtpClint->UpLoad(pOpenUpPro->szServerInfo, m_pMainWnd->m_pPtrTrans->pFullPath, pOpenUpPro->nFileSizeStart);
			
			if(!bResult)
			{
				Packet* pPacketSend = new Packet;
				pPacketSend->Pack(OPEN_UPEND, (char*)szBuf, 32);
				SendPacket(pPacketSend);

				AfxMessageBox("���� ������ �� �� ���� ���α׷��� �����մϴ�!");
				m_pMainWnd->OnBnClickedCancel();
			}

			// ������� ������ ����
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
					AfxMessageBox("�˼� ���� ������ ���α׷��� �����մϴ�.");
					break;
				}
				case OPENUP_RESULT_NOTFOUND:
				{
					AfxMessageBox("������ �������� �ʽ��ϴ�.");
					break;
				}
				case OPENUP_RESULT_SERVICE_STOP:
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
	pPacketSend->Pack(OPEN_UPEND, (char*)szBuf, 32);
	SendPacket(pPacketSend);
	m_pMainWnd->OnBnClickedCancel();
}


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