#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "up_window.h"
#include "FtpClient.h"
#include "Crc32Dynamic.h"
#include "MD5_Client.h"
#include "Dlg_Confirm.h"
//20090303 �߰�, jyh
#include "HtmlViewer.h"

#include <sys/stat.h>
#include "../../Webhard/src_client/up_client/MD5.h"


//ĵ��̵��
#include "sfilter_filerun.h"

#include <strsafe.h>


#pragma warning(disable:4996)
#pragma warning(disable:4312)
#pragma warning(disable:4995)





Worker::Worker(up_window* pMainWnd) : NetworkController()
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
					m_nReConnectCount = 0;					
					m_bReConnectTrans = m_pMainWnd->m_bFileTrans;

					MMServerFree();

					m_pMainWnd->KillTimer(100);

					//20071220 �������۽� ���������� ������ ����� �������۵� ����� �Ѵ�.
					if(m_pMainWnd->m_bTransGura)
					{
						m_pMainWnd->KillTimer(999);
						m_pMainWnd->SetTransFlagGura(false);
					}
					else
					{
						m_pMainWnd->SetTransFlag(false);
					}
					
					//20071119 �޼��� ���� ���� ���� Ÿ�̸� ����(5��), jyh
					if(m_pMainWnd->m_bTransGura)
						m_pMainWnd->SetTimer(998, 300000, NULL);
					else
                        m_pMainWnd->SetTimer(99, 300000, NULL);

					//::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
					//20071119 PostMessage->SendMessage ����, jyh---------------------------------------------------------------------------------
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"���Ǽ������� ������ ����Ǿ� 5�� �ڿ� �ڵ� ����˴ϴ�.\r\n���� �ٷ� ���� �Ͻðڽ��ϱ�?", 
								(LPARAM)2);

					m_pMainWnd->m_pPtrTrans = NULL;		//20071119 ������ �������� �������� ����, jyh
					//20071119 ����Ʈ �ڽ��� ������ ���� ���� ����� ����, jyh
					CString strBuf;
					strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_STOP);
					m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 2, strBuf);
					m_pMainWnd->m_pWorker_Ctrl->m_bReConnectTrans = true;
					m_pMainWnd->m_pWorker_Ctrl->m_bConnectServer = false;	//20081025 ���� ����, jyh

					//��(�ٷ� ����)
					if(m_pMainWnd->m_nBtnState == BTN_STATE_YES)
					{
						//20071220 �������� ������..., jyh
						if(m_pMainWnd->m_bTransGura)
						{
							m_pMainWnd->KillTimer(998);	//������ 5������ ���õƴ� Ÿ�̸� ����
							m_pMainWnd->SetTimer(998, 500, NULL);
						}
						else
						{
							m_pMainWnd->KillTimer(99);	//������ 5������ ���õƴ� Ÿ�̸� ����
							m_pMainWnd->SetTimer(99, 1000, NULL);
						}						
					}
					//�ƴϿ�
					else
					{
						if(m_pMainWnd->m_bTransGura)
							m_pMainWnd->KillTimer(99);
						else
							m_pMainWnd->KillTimer(998);
					}

					//m_pMainWnd->SetTimer(99, 10000, NULL);
				}
				else
				{
					::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"�������� ������ ����Ǿ����ϴ�. \r\n����Ŀ� �ٽ� �̿��� �ֽʽÿ�.", 0);
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
		//20071210 �ؽð� �� ��û ���, jyh
		case LOAD_UPHASH:
		{
			Result_UpHash(pPacket);
			break;
		}
		// ���ε� ��� ��û
		case LOAD_UPLIST:
		{
			Result_Item_Add(pPacket);
			break;
		}
		// ���ε� ��ó�� ����
		case LOAD_UPPRE:
		{
			Result_UpLoadPre(pPacket);
			break;
		}
		// ���ε� ����
		case LOAD_UPLOAD:
		{
			Result_UpLoadPro(pPacket);
			break;
		}
		// ���ε� �Ϸ� �뺸 ���
		case LOAD_UPEND:
		{
			Result_UpLoadEnd(pPacket);
			break;
		}
		//
		case LOAD_UPCONTENTSCOMPLETE:
		{
			Result_UpLoadContentsComplete(pPacket);
			break;
		}
		// �ٸ���ǻ�� �ߺ�����
		case CHK_DUPLI_CLOSE:
		{
			m_pMainWnd->SetTransFlag(false);
			Result_Dupl_Close(pPacket);
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
				m_pMainWnd->m_bAuth			= true;
				m_pMainWnd->m_pUserID		= pLogin->pUserID;
				m_pMainWnd->m_pUserPW		= pLogin->pUserPW;
				
				m_pMainWnd->SetTimer(2, 1, NULL);		
				break;
			}
			case LOGIN_RESULT_FAIL_DUPC:	
			{
				Dlg_Confirm pWindow;
				pWindow.m_nCallMode = 2;
 				if(pWindow.DoModal() == IDOK)
				{
					pLogin->nProcess = 1;
					Packet* pPacket = new Packet;
					pPacket->Pack(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
					SendPacket(pPacket);
					break;
				}
				else
					::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"���α׷��� �����մϴ�", 0);
				break;
			}					
			case LOGIN_RESULT_FAIL_ID:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"�α��ο� ���� �߽��ϴ�!\r\n�������� �ʴ� ���̵� �Դϴ�.", 1);
				break;
			}		
			case LOGIN_RESULT_FAIL_PW:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"�α��ο� ���� �߽��ϴ�!\r\n��й�ȣ�� Ʋ���ϴ�.", 1);
				break;
			}		
			case LOGIN_RESULT_FAIL_STOP:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"�α��ο� ���� �߽��ϴ�!\r\nȸ������ ���� ���� �����Դϴ�.", 1);
				break;
			}		
			case LOGIN_RESULT_SERVERSTOP:
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"���� ���� �������Դϴ�! \r\n�ڼ��� ������ Ȩ�������� �����ϼ���!", 1);
				break;
			}	
			case LOGIN_RESULT_FAIL:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"�α��ο� ���� �߽��ϴ�!\r\n���� ������ ��ӵǸ� �����ڿ��� ������ �ֽʽÿ�.", 1);
				break;
			}			
		}	
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"�α��ο� ���� �߽��ϴ�!\r\n���� ������ ��ӵǸ� �����ڿ��� ������ �ֽʽÿ�.", 1);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 20071210 ���ε��� �ؽð� �� ��û ���, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_UpHash(Packet* pPacket)
{
	PUPHASH upHash;

	try
	{
		//AfxMessageBox("Result_UpHash() ����");
		upHash = (PUPHASH)pPacket->GetData();

		/*CString str;
		str.Format("nResult:%d, bbs_no:%d, nFileType:%d, szIndex:%s, szPath:%s, szCheckSum:%s",
			upHash->nResult, upHash->bbs_no, upHash->nFileType, upHash->szIndex, upHash->szPath, upHash->szCheckSum);
		AfxMessageBox(str);*/

		switch(upHash->nResult)
		{
			case UPLOAD_RESULT_HASH_EXIST:
			{
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
					(WPARAM)"�ش� ������ ���۱� ��ȣ ��û�� ���� ����� �� �����ϴ�!\r\n�ڼ��� ������ '���۱Ǻ�ȣ����'�� �������ֽñ� �ٶ��ϴ�.", 
					(LPARAM)1);
				break;
			}				
			case UPLOAD_RESULT_HASH_NOTEXIST:
			{
				Packet* pPacket1 = new Packet;
				pPacket1->Pack(LOAD_UPLIST, (char*)m_pMainWnd->m_pWebItem, lstrlen(m_pMainWnd->m_pWebItem)+1);
				SendPacket(pPacket1);
				delete [] m_pMainWnd->m_pWebItem;
				m_pMainWnd->m_pWebItem = NULL;
				break;
			}
			case UPLOAD_RESULT_HASH_REG_OK:
			{
				/*Packet* pPacket1 = new Packet;
				pPacket1->Pack(LOAD_UPHASHLIST, (char*)upHash, sizeof(upHash));
				SendPacket(pPacket1);
				delete [] m_pMainWnd->m_pWebItem;
				delete pPacket1;
				m_pMainWnd->m_pWebItem = NULL;*/
				//���ε� ����Ʈ�� ����� �Լ� ȣ��
				//AfxMessageBox("UPLOAD_RESULT_HASH_REG_OK ����");
				m_pMainWnd->GuraUpList(upHash);
				break;
			}
			case UPLOAD_RESULT_HASH_REG_FAIL:
			{
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
					(WPARAM)"���۱� ���� ��Ͽ� ���� �߽��ϴ�!\r\n��õ� �Ͻñ� �ٶ��ϴ�.", 
					(LPARAM)1);
				break;
			}
			case UPLOAD_RESULT_HASH_REG_EXIST:
			{
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
					(WPARAM)"�ش� ������ �̹� ���۱� ����� �� �����Դϴ�!\r\n�ڼ��� ������ '���۱Ǻ�ȣ����'�� �������ֽñ� �ٶ��ϴ�.", 
					(LPARAM)1);
				break;
			}
			//20090304 �߰�, jyh
			case UPLOAD_RESULT_HASH_NOTMD5SUM:
			{
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
					(WPARAM)"������ �ؽð� ��� ���з� ���� ������ ����� ���ѵ˴ϴ�!\r\n������ ���� �Ͻñ� �ٶ��ϴ�.", 
					(LPARAM)1);
				break;
			}
			case UPLOAD_RESULT_HASH_ERROR:
			{
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
					(WPARAM)"�˼����� ������ ���� ���ϵ�Ͽ� �����߽��ϴ�!\r\n������ ���� �Ͻñ� �ٶ��ϴ�.", 
					(LPARAM)1);
				break;
			}
		}
	}
	catch (...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ؽð� �� ��û ����", 1);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//20071211 �ؽð� ��� �� ������ ����Ʈ ��û ���, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void Worker::Result_UpHashList(Packet* pPacket)
//{
//	CString pPath;
//	if(pPacket->m_pHead->ds != 0)
//	{
//		char* pItemList = pPacket->GetData();
//
//		CToken*	pToken1 = new CToken(SEP_ITEM);	
//		CToken*	pToken2 = new CToken(SEP_FIELD);	
//
//		pToken1->Split(pItemList);
//
//		if(pToken1->GetCount() > 0)
//		{
//			//20071015 ������ ������ 8�� �̻��̸� ����Ʈ ���� ũ�� ����, jyh
//			if(m_pMainWnd->m_pListCtrl.GetItemCount() + pToken1->GetCount() > 8)
//				m_pMainWnd->m_pListCtrl.SetColumnWidth(0, 363);
//
//			// ���α׷��� �ֻ��� ������� ���
//			m_pMainWnd->ShowWindow(SW_RESTORE);
//			m_pMainWnd->SetForegroundWindow();
//			m_pMainWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//			m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);	
//
//			// ���۹��� ����� ť�� ���
//			for(size_t index_1 = 0; index_1 < pToken1->GetCount() ; index_1++)
//			{
//				pToken2->Split(pToken1->GetToken(index_1));
//
//				if(pToken2->GetCount() != 6)
//					continue;
//
//				PRUPCTX pNewItem		= new RUPCTX;
//				ZeroMemory(pNewItem, sizeof(pNewItem));
//
//				pNewItem->nTransState	= ITEM_STATE_WAIT;
//				pNewItem->nFileSizeStart= 0;
//				pNewItem->nFileSizeEnd	= 0;
//				pNewItem->nFileType		= atoi(pToken2->GetToken(0));
//				pNewItem->nBoardIndex	= _atoi64(pToken2->GetToken(1));
//				pNewItem->nFileIndex	= _atoi64(pToken2->GetToken(2));				
//				pNewItem->nFileSizeReal	= _atoi64(pToken2->GetToken(3));
//
//				strcpy(pNewItem->pFileName, pToken2->GetToken(4));		
//
//				CString pTemp = pToken2->GetToken(5);
//				pTemp.Replace("/", "\\");
//				strcpy(pToken2->GetToken(5), (LPSTR)(LPCTSTR)pTemp);
//				strcpy(pNewItem->pFullPath, pToken2->GetToken(5));
//				::PathRemoveFileSpec(pToken2->GetToken(5));	
//				strcpy(pNewItem->pFolderName, pToken2->GetToken(5));
//				m_pMainWnd->InsertItem(pNewItem);
//			}
//		}
//
//		delete pToken1;
//		delete pToken2;
//	}
//
//	//���� �Ǵ°� ó�� ���̰�...
//	if(m_pMainWnd->m_rDownAutoTrans == 1 && !m_pMainWnd->m_bFileTrans)	//20071030 ȯ�漳������ �ڵ����� ������ ����, jyh
//	{
//		m_pMainWnd->m_bTransGura = true;
//		m_pMainWnd->SetTransFlagGura(true);
//	}
//}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���ε� ��ó�� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_UpLoadPre(Packet* pPacket)
{
	CString strBuf;

	try
	{
		__int64 nSizeStart = 0;

		PUPLOADPRE pTransInfo = (PUPLOADPRE)pPacket->GetData();

		if(m_pMainWnd->m_pPtrTrans == NULL)
		{
			::SendMessage(m_pMainWnd->m_hWnd, MSG_UPLOAD, 0, 0);
			return;
		}

		if(pTransInfo->nResult == UPLOAD_RESULT_NEW)
		{
			m_bReConnectTrans = false;
			nSizeStart = pTransInfo->nFileSize_Complete;

			DWORD dwErrorCode = NO_ERROR;
			
			//�ؽð��� �����.
			CMD5 pMD5;
			CString strMD5 = pMD5.GetString(m_pMainWnd->m_pPtrTrans->pFullPath, dwErrorCode);

			if(dwErrorCode == NO_ERROR)
			{
				UPLOADPRO pTransInfoNew;
				pTransInfoNew.nResult			= 0;
				pTransInfoNew.nFileType			= pTransInfo->nFileType;
				pTransInfoNew.nBoardIndex		= pTransInfo->nBoardIndex;				
				pTransInfoNew.nFileIdx			= pTransInfo->nFileIdx;
				pTransInfoNew.nFileSize_Start	= nSizeStart;
				pTransInfoNew.nUploadSpeed		= 0;
				strcpy_s(pTransInfoNew.pFileCheckSum, strMD5.GetLength()+1, (LPSTR)(LPCTSTR)strMD5);
				ZeroMemory(pTransInfoNew.pServerInfo, sizeof(pTransInfoNew.pServerInfo));

				Packet* pPacket = new Packet;
				pPacket->Pack(LOAD_UPLOAD, (char*)&pTransInfoNew, sizeof(UPLOADPRO));
				SendPacket(pPacket);		
			}
			else
			{
				m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_ERROR;
				m_pMainWnd->m_pPtrTrans = NULL;		//20071219 �߰�, jyh

				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
							(WPARAM)"���ε忡 �����߽��ϴ�!\r\n���� ������ ��ӵǸ� �����ڿ��� �����ϼ���.\r\n\r\nError Code = CRC32", 
							(LPARAM)1);		
				::SendMessage(m_pMainWnd->m_hWnd, MSG_UPLOAD, 0, 0);
			}
			return;
		}
		else
		{
			switch(pTransInfo->nResult)
			{
				case UPLOAD_RESULT_ERROR:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���ܰ� �߻��߽��ϴ�. ����Ŀ� �ٽ� �õ��ϼ���.", (LPARAM)1);					
					break;
				}
				case UPLOAD_RESULT_NOTFOUND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n�ش� ������ �����Ǿ��ų� ���������� �ҷ����µ� �����߽��ϴ�.", (LPARAM)1);					
					break;
				}
				case UPLOAD_RESULT_SIZE:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n��Ͻ� ���ϻ������ ���� �����ϰ� ��� ���� ����� �ٸ��ϴ�.", (LPARAM)1);					
					break;
				}
				case UPLOAD_RESULT_PATH:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���ε尡 �Ϸ� �Ǿ��ų� ������ ��� ������ �������� �ʽ��ϴ�.", (LPARAM)1);					
					break;
				}
				case UPLOAD_RESULT_END:
				{
					m_pMainWnd->m_pPtrTrans->nFileSizeEnd = m_pMainWnd->m_pPtrTrans->nFileSizeReal;
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;					
					m_pMainWnd->m_pPtrTrans = NULL;	
					::SendMessage(m_pMainWnd->m_hWnd, MSG_UPLOAD, 0, 0);
					break;
				}
				case UPLOAD_RESULT_SERVICE_STOP:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
						(WPARAM)"�ش� ������ ������ ���ε� ������ ���� �Ǿ� �������� ��� �� �� �����ϴ�.\r\n"
								"�ڵ����� ���� �������� ���ε� �մϴ�.", (LPARAM)1);
														
					break;		
				}
				case UPLOAD_RESULT_DISKFULL:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
						(WPARAM)"���ε忡 �����߽��ϴ�! �Ҵ� ���� ��ũ �뷮�� �����մϴ�.\r\n"
								"�ڵ����� ���� �������� ���ε� �մϴ�.", (LPARAM)1);
					
					break;
				}
			}

			if(m_pMainWnd->m_pPtrTrans)
				m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;

			strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 2, strBuf);
			m_pMainWnd->m_pPtrTrans = NULL;
			m_pMainWnd->m_bFileTrans = false;
			//20071219 ���� �������� �ǳʶ�, jyh
			if(m_pMainWnd->DeferItem() > 0)
                m_pMainWnd->SetTimer(98, 100, NULL);
			else
                m_pMainWnd->SetTransFlag(false);	
		}
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε� ����", 1);
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���ε� ó�� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_UpLoadPro(Packet* pPacket)
{
	//int bReTransTime = 30000;
	CString strBuf;
	int nItemPatten = 0;		//20081025 ��õ�â������ ��ư ��(-1: ���, 1: �ǳʶٱ�, 2: ��õ�), jyh
	BOOL bRetryAlret = FALSE;	//20081025 ��õ� â������ ��ư ���� ����, jyh

	try
	{	
		PUPLOADPRO pTransInfo = (PUPLOADPRO)pPacket->GetData();

		if(m_pMainWnd->m_pPtrTrans == NULL)
		{
			::SendMessage(m_pMainWnd->m_hWnd, MSG_UPLOAD, 0, 0);
			return;
		}

		// ���� ���ε� ����
		if(pTransInfo->nResult == UPLOAD_RESULT_NEW || pTransInfo->nResult == UPLOAD_RESULT_TRICK)
		{
			bool bResult = false;
		
			m_pMainWnd->m_pPtrTrans->nFileSizeStart = pTransInfo->nFileSize_Start;
			m_pMainWnd->m_pPtrTrans->nFileSizeEnd	= pTransInfo->nFileSize_Start;

			Sleep(500);		//20071122 smi����(���뷮 ����) �ȿö󰡴� �� ����, jyh

			if(pTransInfo->nResult == UPLOAD_RESULT_NEW)
				bResult = m_pMainWnd->m_pFtpClint->UpLoad(pTransInfo->pServerInfo, m_pMainWnd->m_pPtrTrans->pFullPath, pTransInfo->nFileSize_Start);
			else if(pTransInfo->nResult == UPLOAD_RESULT_TRICK)
				bResult = m_pMainWnd->m_pFtpClint->UpLoadTrick(m_pMainWnd->m_pPtrTrans->pFullPath, m_pMainWnd->m_pPtrTrans->nFileSizeReal, pTransInfo->nFileSize_Start);

			if(!bResult)
			{
				if(m_pMainWnd->m_pPtrTrans)
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_ERROR;
				m_pMainWnd->m_pPtrTrans = NULL;

				::SendMessage(m_pMainWnd->m_hWnd, MSG_UPLOAD, 0, 0);
				return;
			}

			// ������� ������ ����
			int		nDummy;
			HANDLE	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTransLoop, m_pMainWnd, 0, (unsigned int *)&nDummy);
			CloseHandle(hTrans);	

			m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_TRANS;
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 2, m_pMainWnd->GetFileStatus(ITEM_STATE_TRANS));
			return;
		}

		else
		{
			/*switch(pTransInfo->nResult)
			{			
				case UPLOAD_RESULT_ERROR:
				case UPLOAD_RESULT_SIZE:
				case UPLOAD_RESULT_PATH:
				case UPLOAD_RESULT_END:
				{
					if(m_pMainWnd->m_pPtrTrans)
						m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				default:
				{
					if(m_pMainWnd->m_pPtrTrans)
					{
						m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_STOP;
						m_pMainWnd->SetTransFlag(false);
					}
					break;
				}
			}*/
			
			//20071219 �ּ� ó��, jyh
			//m_pMainWnd->m_pPtrTrans = NULL;
			
			switch(pTransInfo->nResult)
			{			
				case UPLOAD_RESULT_ERROR:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���ܰ� �߻��߽��ϴ�. ����Ŀ� �ٽ� �õ��ϼ���.", (LPARAM)1);						
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				case UPLOAD_RESULT_SIZE:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���۽��� ������ ����", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				case UPLOAD_RESULT_PATH:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���ε尡 �Ϸ� �Ǿ��ų� ������ ��� ������ �������� �ʽ��ϴ�.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				case UPLOAD_RESULT_END:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���ε尡 �Ϸ�� �����Դϴ�.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				case UPLOAD_RESULT_NOTFOUND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n�ش� ������ �����Ǿ��ų� ���������� �ҷ����µ� �����߽��ϴ�.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				case UPLOAD_RESULT_ERROR_DB:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n�ش� ���� ������ �������� ���߽��ϴ�. ����� �ٽ� �õ��ϼ���.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
				case UPLOAD_RESULT_ERROR_ALLO:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n���ϼ��� �Ҵ��û�� �����߽��ϴ�. ����� �ٽ� �õ��ϼ���.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}				
				case UPLOAD_RESULT_FTP_CONFAILED:	//20081027 ftp ���� ���� �߰�, jyh
				case UPLOAD_RESULT_ERROR_DELAY:		//ftp ���� ���� ����
				/*{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)(WPARAM)"���ϼ��� ������ �����Ǿ� ����� �ڵ����� �ٽ� �õ��մϴ�.", (LPARAM)1);										
					bReTransTime = 5000;
					break;
				}*/
				case UPLOAD_RESULT_ERROR_STOP:		//ftp ���� ��� ����
				/*{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε忡 �����߽��ϴ�!\r\n�Ҵ�� ���ϼ����� �ӽ� �������Դϴ�. ����� �ٽ� �õ��ϼ���.", (LPARAM)0);								
					return;
				}	*/
				// ���ϼ����� ���Ϸ� ���ε� �Ұ�(������)
				case UPLOAD_RESULT_SERVERBUSY:
				{
					//20071219 ���ε� ���н� nCountDefer���� 1 ���Ѵ�, jyh
					m_pMainWnd->m_pPtrTrans->nCountDefer++;

					if(m_pMainWnd->m_pPtrTrans->nCountDefer >= 3)
					{
						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
							(WPARAM)"�ش������� �������Ϸ� ������ �Ұ����մϴ�\r\n���� ������ �����մϴ�.", (LPARAM)1);	

						//20081027 ����, jyh
						if(pTransInfo->nResult == UPLOAD_RESULT_ERROR_DELAY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DELAY;	//���� ����
						else if(pTransInfo->nResult == UPLOAD_RESULT_SERVERBUSY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DEFER;	//����� �ʰ�
						else
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CONFAILED;//�������ӽ���
					}
					else 
					{
						char szText[MAX_PATH];
						sprintf_s(szText, MAX_PATH, "�ش� ������ �����Ϸ� ���ε尡 �����ǰ� �ֽ��ϴ�.\r\n"
							"30�� ���� �Է��� ���� ��� �ش� ������ ��õ� �մϴ�.\r\n"
							"���� ��õ� Ƚ��: %d", 3 - m_pMainWnd->m_pPtrTrans->nCountDefer);

						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)szText, (LPARAM)3);

						bRetryAlret = TRUE; //20081025 ��õ� â, jyh

						//20081027 ����, jyh
						if(pTransInfo->nResult == UPLOAD_RESULT_ERROR_DELAY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DELAY;	//���� ����
						else if(pTransInfo->nResult == UPLOAD_RESULT_SERVERBUSY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DEFER;	//����� �ʰ�
						else
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CONFAILED;//�������ӽ���

						//20071119 ����Ʈ �ڽ��� ������ ���� ���� ����� ����, jyh
						//strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
						//m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 2, strBuf);

						//�ǳʶٱ�(������ ����->���ϴ����� ����)
						if(m_pMainWnd->m_nBtnState == BTN_STATE_JUMP)
							nItemPatten = 1;
						//��õ�
						else if(m_pMainWnd->m_nBtnState == BTN_STATE_RETRY)
							nItemPatten = 2;
						//���
						else if(m_pMainWnd->m_nBtnState == BTN_STATE_CANCEL)
							nItemPatten = -1;

						//m_pMainWnd->m_pPtrTrans = NULL;

						//if(nItemPatten == -1)
						//	m_pMainWnd->SetTransFlag(false);
						//else if(nItemPatten == 0) 
						//{							
						//	//m_pMainWnd->SetTimer(98, 30000, NULL);
						//	::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW,
						//		(WPARAM)"�ǳʶ� �������� �����ϴ�.\r\n", (LPARAM)1);
						//	m_pMainWnd->SetTransFlag(false);
						//}
						//else
						//{
						//	m_pMainWnd->m_bFileTrans = false;
						//	m_pMainWnd->SetTimer(98, 100, NULL);	
						//}
					}
					
					break;					
				}

				default:
				{
					if(m_pMainWnd->m_pPtrTrans)
						m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_ERROR;
					
					break;
				}
			}

			strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 2, strBuf);
			//m_pMainWnd->m_pPtrTrans = NULL;
			m_pMainWnd->m_bFileTrans = false;

			//20081025 ��õ� �˸�â�� ���� ��������, jyh
			if(bRetryAlret)
			{				
				if(nItemPatten == -1)		//���
					m_pMainWnd->SetTransFlag(false);
				else if(nItemPatten == 1)	//�ǳʶٱ�
				{
					//m_pMainWnd->SetTimer(98, 30000, NULL);
					if(m_pMainWnd->DeferItem() <= 0)
					{
						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW,
							(WPARAM)"�ǳʶ� ������ �����ϴ�.\r\n", (LPARAM)1);
						m_pMainWnd->SetTransFlag(false);
					}
					else
						m_pMainWnd->SetTimer(98, 100, NULL);
				}
				else	//��õ�
				{
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_WAIT;
					m_pMainWnd->SetTimer(98, 100, NULL);
				}
			}
			else
			{
				//20071219 ���ε� ���н� ���� ���Ϸ� �ǳʶ�, jyh
				if(m_pMainWnd->DeferItem() > 0)
					m_pMainWnd->SetTimer(98, 100, NULL);
				else
					m_pMainWnd->SetTransFlag(false);
			}
		}
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε� ����_������", 1);
		m_pMainWnd->SetTransFlag(false);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���ε� �Ϸ� �뺸 ���
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_UpLoadEnd(Packet* pPacket)
{
	::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���ε� �Ϸ�!", 1);
}

//20090303 ������ ���ε� �Ϸ�� �뺸 ����, jyh
void Worker::Result_UpLoadContentsComplete(Packet* pPacket)
{	
	::PostMessage(m_pMainWnd->m_hWnd, MSG_UPCONTENTSCOMPLETE, 0, 0);

	//PUPLOADEND pTransInfo = (PUPLOADEND)pPacket->GetData();
	//CString strData;
	//HWND hwnd;	
	//CoInitialize(NULL);
	//
	//CComPtr<IWebBrowser2> spIExplorer = NULL;
	//CComVariant varEmpty(0);

	//spIExplorer.CoCreateInstance(CLSID_InternetExplorer);

	//if(spIExplorer)
	//{
	//	spIExplorer->put_Visible(VARIANT_FALSE);
	//	strData.Format("%s%I64d", URL_SENDCONTENTS, pTransInfo->nBoardIndex);
	//	spIExplorer->Navigate(CComBSTR(strData), &varEmpty, &varEmpty, &varEmpty, &varEmpty);
	//}

	//for(int i=0; i<10; i++)
	//{
	//	Sleep(300);
	//	//IE �������� ����
	//	if(m_pMainWnd->m_nIEVer < 7)
	//		hwnd = FindWindow("IEFrame", "NFile CP Check - Microsoft Internet Explorer");
	//	else
	//		hwnd = FindWindow("IEFrame", "NFile CP Check - Windows Internet Explorer");

	//	if(hwnd)
	//	{
	//		::SendMessage(hwnd, WM_CLOSE, NULL, NULL);
	//		break;
	//	}
	//}
	//
	//CoUninitialize();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���۹��� ������ ��� �߰�
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Item_Add(Packet* pPacket)
{

	bool Copyright_check_DLL=true;



	UPLOADMUREKA	pTransMurekaInfo;

	HINSTANCE hDLL = NULL;
	HINSTANCE hModule = NULL;
	SFILTERInit		lpSFILTERInit = NULL;
	SFILTERUpload	lpSFILTERUpload = NULL;
	SFILTEREnd		lpSFILTEREnd = NULL;
	

	hDLL = LoadLibrary(_T("sffilerun.dll"));
	if(hDLL)
	{
		lpSFILTERInit			= (SFILTERInit)::GetProcAddress(hDLL, "Init");
		lpSFILTERUpload			= (SFILTERUpload)::GetProcAddress(hDLL, "Upload");
		lpSFILTEREnd			= (SFILTEREnd)::GetProcAddress(hDLL, "End");
		
		if(lpSFILTERInit && lpSFILTERUpload && lpSFILTEREnd)
		{
				Copyright_check_DLL=true;
		}
		else
		{
			// DLL �ε� ����
			char pResult[2048];
			::sprintf_s(pResult, 2048, "���۱� Ȯ�� ���α׷��� ���������� ������� �ʾ� ���α׷��� ����˴ϴ�.\r\n���Ϸ� Ŭ���̾�Ʈ�� �ٽ� ��ġ ���ּ���.");
			::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)1);

			m_pMainWnd->SetTimer(44, 100, NULL);
			return;
		}
	}
	else
	{
		// DLL �ε� ����
		char pResult[2048];
		::sprintf_s(pResult, 2048, "���۱� Ȯ�� ���α׷��� ���������� ������� �ʾ� ���α׷��� ����˴ϴ�.\r\n���Ϸ� Ŭ���̾�Ʈ�� �ٽ� ��ġ ���ּ���.");
		::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)1);

		m_pMainWnd->SetTimer(44, 100, NULL);
		return;
	}


	int nTransState_Set_value;

	if(Copyright_check_DLL==true)
	{


		CString pPath;
		if(pPacket->m_pHead->ds != 0)
		{
			char* pItemList = pPacket->GetData();

			CToken*	pToken1 = new CToken(SEP_ITEM);	
			CToken*	pToken2 = new CToken(SEP_FIELD);	

			pToken1->Split(pItemList);
			
			if(pToken1->GetCount() > 0)
			{
				//20080123 ������ ������ ���ε� �ϷḦ ��Ÿ���� ����, jyh
				CONITEM conitem;
				pToken2->Split(pToken1->GetToken(0));
				conitem.nBoardIndex = atoi(pToken2->GetToken(1));
				conitem.nItemCnt	= (int)pToken1->GetCount();
				m_pMainWnd->m_v_sConItem.push_back(conitem);

				//20071015 ������ ������ 7�� �̻��̸� ����Ʈ ���� ũ�� ����, jyh
				if(m_pMainWnd->m_pListCtrl.GetItemCount() + pToken1->GetCount() > 7)
					m_pMainWnd->m_pListCtrl.SetColumnWidth(0, 360);

				// ���α׷��� �ֻ��� ������� ���
				/*m_pMainWnd->ShowWindow(SW_RESTORE);
				m_pMainWnd->SetForegroundWindow();
				m_pMainWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
				m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);*/
				if(::GetForegroundWindow() != m_pMainWnd->m_hWnd)
				{ 
					HWND h_active_wnd = ::GetForegroundWindow(); 
					
					if(h_active_wnd != NULL)
					{ 
						DWORD thread_id = GetWindowThreadProcessId(h_active_wnd, NULL); 
						DWORD current_thread_id = GetCurrentThreadId(); 
						
						if(current_thread_id != thread_id)
						{ 
							if(AttachThreadInput(current_thread_id, thread_id, TRUE))
							{ 
								::BringWindowToTop(m_pMainWnd->m_hWnd); 
								AttachThreadInput(current_thread_id, thread_id, FALSE); 
							} 
						} 
					} 
				}


				// ���۹��� ����� ť�� ���
				for(size_t index_1 = 0; index_1 < pToken1->GetCount() ; index_1++)
				{
					pToken2->Split(pToken1->GetToken(index_1));

					if(pToken2->GetCount() != 8)
						continue;
					
					PRUPCTX pNewItem		= new RUPCTX;
					ZeroMemory(pNewItem, sizeof(RUPCTX));











					nTransState_Set_value=ITEM_STATE_WAIT;

					//���� ���ϸ� �ε�
					strcpy_s(pNewItem->pFullPath, strlen(pToken2->GetToken(5))+1, pToken2->GetToken(5));
//���͸�
					if(Viruschecking((LPTSTR)(LPCTSTR)pToken2->GetToken(5)))
					{
						nTransState_Set_value=ITEM_STATE_VIRUSFILE;
					}
					else
					{






						CString FilterStatus;
						LPSFILTER_RET p_sfilter_ret = NULL;
						LPSFILTER_RET_VIDEO p_sfilter_ret_video = NULL;
						LPSFILTER_RET_MUSIC p_sfilter_ret_music = NULL;

						lpSFILTERInit( 1, 0);
												
						int nRet = lpSFILTERUpload(
													&p_sfilter_ret,
													&p_sfilter_ret_music,
													&p_sfilter_ret_video,
													(LPTSTR)(LPCTSTR)m_pMainWnd->m_pUserID,
													(LPTSTR)(LPCTSTR)pToken2->GetToken(5),
													atoi(pToken2->GetToken(1)),
													(LPTSTR)(LPCTSTR)pToken2->GetToken(6)
												);


						if(nRet==0)
						{


							if(p_sfilter_ret->video_count+p_sfilter_ret->music_count>0)
							{




								//�Խñ� ��ȣ
								pTransMurekaInfo.nBoardIndex=_atoi64(pToken2->GetToken(1));
								//���ϰԽñ� ��ȣ
								pTransMurekaInfo.uFile_code=_atoi64(pToken2->GetToken(2));
								//�ؽ���
								sprintf(pTransMurekaInfo.uHash,"%s",p_sfilter_ret->sfilter_hash);
								//�ؽ���(������ü�ؽ�)
								sprintf(pTransMurekaInfo.uHash_master,"%s",p_sfilter_ret->sfilter_hash);

								//���ϰ���
								pTransMurekaInfo.uCounts=p_sfilter_ret->video_count+p_sfilter_ret->music_count;



								//�ʱ�ȭ - S
								pTransMurekaInfo.uVideo_id=0;
								pTransMurekaInfo.uMusic_id=0;
								pTransMurekaInfo.uMoney=0;

								strcpy(pTransMurekaInfo.uStatus,"");
								strcpy(pTransMurekaInfo.uTitle,"");
								strcpy(pTransMurekaInfo.uVideo_content_id,"");
								strcpy(pTransMurekaInfo.uVideo_code,"");
								strcpy(pTransMurekaInfo.uMusic_content_id,"");
								strcpy(pTransMurekaInfo.uMusic_company,"");
								strcpy(pTransMurekaInfo.uCp_id,"");
								strcpy(pTransMurekaInfo.uCp_company,"");
								strcpy(pTransMurekaInfo.uMovie_content_id,"");
								strcpy(pTransMurekaInfo.uMovie_code,"");

								pTransMurekaInfo.uMovie_cha=0;
								pTransMurekaInfo.uMovie_jibun=0;
								pTransMurekaInfo.uMovie_onair_date=0;
								//�ʱ�ȭ - E


								for(int i=0; i<p_sfilter_ret->video_count; i++)
								{
									if(p_sfilter_ret_video[i].rmi_return_code == 10) FilterStatus = _T("02");
//									if(p_sfilter_ret_video[i].rmi_return_code == 10) {	FilterStatus = _T("04");	pTransMurekaInfo.uMoney=0;	}
									else if(p_sfilter_ret_video[i].rmi_return_code == 11) FilterStatus = _T("00");
									else if(p_sfilter_ret_video[i].rmi_return_code == 12) FilterStatus = _T("01");
									else if(p_sfilter_ret_video[i].rmi_return_code == 13) FilterStatus = _T("04");
									else if(p_sfilter_ret_video[i].rmi_return_code == 14) FilterStatus = _T("04");
									else if(p_sfilter_ret_video[i].rmi_return_code == 15) FilterStatus = _T("02");
									else FilterStatus = _T("04");


									//���Ϻз�
									pTransMurekaInfo.uFiletype=2;

									pTransMurekaInfo.uVideo_id=_atoi64(p_sfilter_ret_video[i].content_id);
									pTransMurekaInfo.uMoney=atoi(p_sfilter_ret_video[i].price);
									sprintf(pTransMurekaInfo.uStatus,"%s",FilterStatus);
									sprintf(pTransMurekaInfo.uTitle,"%s",p_sfilter_ret_video[i].title);
									sprintf(pTransMurekaInfo.uCp_company,"%s",p_sfilter_ret_video[i].ch_name);
									sprintf(pTransMurekaInfo.uVideo_content_id,"%s",p_sfilter_ret_video[i].ch_id);
									sprintf(pTransMurekaInfo.uMovie_content_id,"%s",p_sfilter_ret_video[i].ch_content_id);
									sprintf(pTransMurekaInfo.uMovie_code,"%s",p_sfilter_ret_video[i].ch_content_id);
									sprintf(pTransMurekaInfo.uMovie_content_id,"%s",p_sfilter_ret_video[i].ch_content_id);

									pTransMurekaInfo.uMovie_cha=atoi(p_sfilter_ret_video[i].seq);
									pTransMurekaInfo.uMovie_jibun=atoi(p_sfilter_ret_video[i].site_ratio);
									pTransMurekaInfo.uMovie_onair_date=atoi(p_sfilter_ret_video[i].seq);
									sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);
									sprintf(pTransMurekaInfo.usmartblock_key,"%s",p_sfilter_ret_video[i].smartblock_key);

									//20150803
									pTransMurekaInfo.uTurn=i;


									//������ ����(120619 - �������Ϸ� ���ؼ� ������)
									Packet* pPacket_Mureka = new Packet;
									pPacket_Mureka->Pack(SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
									m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);


									
									if(FilterStatus=="02")
										nTransState_Set_value=ITEM_STATE_COPYRIGHT;

									if(p_sfilter_ret_video[i].rmi_return_code == 15) 
										nTransState_Set_value=ITEM_STATE_BLOCKFILE;




								}

								for(int i=0; i<p_sfilter_ret->music_count; i++)
								{
									if(p_sfilter_ret_music[i].rmi_return_code == 10) FilterStatus = _T("02");
									else if(p_sfilter_ret_music[i].rmi_return_code == 11) FilterStatus = _T("00");
									else if(p_sfilter_ret_music[i].rmi_return_code == 12) FilterStatus = _T("01");
									else if(p_sfilter_ret_music[i].rmi_return_code == 13) FilterStatus = _T("04");
									else if(p_sfilter_ret_music[i].rmi_return_code == 14) FilterStatus = _T("04");
									else if(p_sfilter_ret_video[i].rmi_return_code == 15) FilterStatus = _T("02");

									//���Ϻз�
									pTransMurekaInfo.uFiletype=1;
									pTransMurekaInfo.uMusic_id=_atoi64(p_sfilter_ret_music[i].ch_content_id);
									pTransMurekaInfo.uMoney=atoi(p_sfilter_ret_music[i].price);
									sprintf(pTransMurekaInfo.uStatus,"%s",FilterStatus);
									sprintf(pTransMurekaInfo.uTitle,"%s",p_sfilter_ret_music[i].title);
									sprintf(pTransMurekaInfo.uMusic_company,"%s",p_sfilter_ret_music[i].ch_name);
									sprintf(pTransMurekaInfo.uMusic_content_id,"%s",p_sfilter_ret_music[i].ch_id);
									sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_music[i].filtering_key);

									
									//������ ����(120619 - �������Ϸ� ���ؼ� ������)
									Packet* pPacket_Mureka = new Packet;
									pPacket_Mureka->Pack(SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
									m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);

									if(FilterStatus=="02")
										nTransState_Set_value=ITEM_STATE_COPYRIGHT;

									if(p_sfilter_ret_video[i].rmi_return_code == 15) 
										nTransState_Set_value=ITEM_STATE_BLOCKFILE;

								}

							}
						}
						else
						{
							nTransState_Set_value=ITEM_STATE_COPYRIGHT;
						}
						
						// ����ü �޸� ����
						lpSFILTEREnd(p_sfilter_ret, p_sfilter_ret_music, p_sfilter_ret_video);




					}






					pNewItem->nTransState	= nTransState_Set_value;
					pNewItem->nFileSizeStart= 0;
					pNewItem->nFileSizeEnd	= 0;
					pNewItem->nFileType		= atoi(pToken2->GetToken(0));
					pNewItem->nBoardIndex	= _atoi64(pToken2->GetToken(1));
					pNewItem->nFileIndex	= _atoi64(pToken2->GetToken(2));				
					pNewItem->nFileSizeReal	= _atoi64(pToken2->GetToken(3));
					
					strcpy_s(pNewItem->pFileName, strlen(pToken2->GetToken(4))+1, pToken2->GetToken(4));		

					CString pTemp = pToken2->GetToken(5);
					pTemp.Replace("/", "\\");
					strcpy_s(pToken2->GetToken(5), pTemp.GetLength()+1, (LPSTR)(LPCTSTR)pTemp);
//					���� ���ϸ� ����
					strcpy_s(pNewItem->pFullPath, strlen(pToken2->GetToken(5))+1, pToken2->GetToken(5));
					::PathRemoveFileSpec(pToken2->GetToken(5));	
					strcpy_s(pNewItem->pFolderName, strlen(pToken2->GetToken(5))+1, pToken2->GetToken(5));
					m_pMainWnd->InsertItem(pNewItem);
				}
			}

			delete pToken1;
			delete pToken2;

			// �ڵ� ���۽��� üũ
			//if(!m_pMainWnd->m_bFileTrans)
			if(m_pMainWnd->m_rDownAutoTrans == 1 && !m_pMainWnd->m_bFileTrans)	//20071030 ȯ�漳������ �ڵ����� ������ ����, jyh
				m_pMainWnd->SetTransFlag(true);
		}
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٸ� ��ǻ�Ϳ��� ������ �ٿ�ε� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Dupl_Close(Packet* pPacket)
{
	char pResult[2048];

	::sprintf_s(pResult, 2048, "�ٸ� ��ҿ��� �α��εǾ� ������ �����˴ϴ�.\r\n\r\n[%s]", (char*)pPacket->GetData());
	::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)1);
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



CString Worker::StrToCString(char* pData, int nCnt)
{
	char tmp[1024];
	
	memcpy(tmp, pData, nCnt);
	tmp[nCnt] = NULL;
	
	CString strRet(tmp);
	
	strRet.TrimLeft();
	strRet.TrimRight();
	
	return strRet;
}





//ĵ��̵�� ����üũ
bool Worker::Mureka_files_check()
{
	return true;

	//��ġ ���� üũ - S
	CString strFileName;
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));
	::SHGetSpecialFolderPath(NULL,pPath,CSIDL_PROGRAM_FILES,false);
	//��ġ ���� üũ - E



	char mureka_files[23][50];
	char mureka_files_size[23][50];
	strcpy_s(mureka_files[0],"Ark32.dll");
	strcpy_s(mureka_files_size[0],"796168");
	strcpy_s(mureka_files[1],"avcodec-53.dll");
	strcpy_s(mureka_files_size[1],"12642304");
	strcpy_s(mureka_files[2],"avdevice-53.dll");
	strcpy_s(mureka_files_size[2],"53760");
	strcpy_s(mureka_files[3],"avformat-53.dll");
	strcpy_s(mureka_files_size[3],"1244672");
	strcpy_s(mureka_files[4],"avutil-51.dll");
	strcpy_s(mureka_files_size[4],"126464");
	strcpy_s(mureka_files[5],"az7z.dll");
	strcpy_s(mureka_files_size[5],"668616");
	strcpy_s(mureka_files[6],"AzCDImage.dll");
	strcpy_s(mureka_files_size[6],"472336");
	strcpy_s(mureka_files[7],"AZMain.dll");
	strcpy_s(mureka_files_size[7],"817152");
	strcpy_s(mureka_files[8],"cabinet.dll");
	strcpy_s(mureka_files_size[8],"59904");
	strcpy_s(mureka_files[9],"FilemanCMC.dll");
	strcpy_s(mureka_files_size[9],"557056");
	strcpy_s(mureka_files[10],"FCvScan.dll");
	strcpy_s(mureka_files_size[10],"204800");
	strcpy_s(mureka_files[11],"FCvServer.ini");
	strcpy_s(mureka_files_size[11],"47");
	strcpy_s(mureka_files[12],"ippcore-5.2.dll");
	strcpy_s(mureka_files_size[12],"73728");
	strcpy_s(mureka_files[13],"ippi-5.2.dll");
	strcpy_s(mureka_files_size[13],"327680");
	strcpy_s(mureka_files[14],"ippipx-5.2.dll");
	strcpy_s(mureka_files_size[14],"4698112");
	strcpy_s(mureka_files[15],"libguide40.dll");
	strcpy_s(mureka_files_size[15],"217088");
	strcpy_s(mureka_files[16],"MediaInfo.dll");
	strcpy_s(mureka_files_size[16],"2938992");
	strcpy_s(mureka_files[17],"MurekaAVD.exe");
	strcpy_s(mureka_files_size[17],"266240");
	strcpy_s(mureka_files[18],"MurekaWma.dll");
	strcpy_s(mureka_files_size[18],"131072");
	strcpy_s(mureka_files[19],"swscale-2.dll");
	strcpy_s(mureka_files_size[19],"245248");
	strcpy_s(mureka_files[20],"UNACEV2.DLL");
	strcpy_s(mureka_files_size[20],"77312");
	strcpy_s(mureka_files[21],"UnEGG32.dll");
	strcpy_s(mureka_files_size[21],"632832");
	strcpy_s(mureka_files[22],"unrar4.dll");
	strcpy_s(mureka_files_size[22],"162304");


	//����üũ
	CFileFind pFileFind;
	for(int rof=0;rof<22;rof++)
	{
		strFileName.Format("%s\\%s\\%s", pPath, CLIENT_APP_PATH,mureka_files[rof]);
		if(!pFileFind.FindFile((LPCTSTR)strFileName))
		{
			pFileFind.Close();
			return false;
		}
		else
		{
			pFileFind.FindNextFile();
			if(_atoi64(mureka_files_size[rof])!=(__int64)pFileFind.GetLength())
			{
				pFileFind.Close();
				return false;
			}
		}
	}
	pFileFind.Close();


	return true;
}



CString Worker::KTH_OSP_hash_send(char* pData,__int64 BoardIndex,__int64 FileIndex)
{
	static int cnt_nums=0;
	CString strData,Filehash;

	CMD5_kth md5;
	Filehash = md5.GetHashFromFile(pData);

	strData.Format("%sbbs=%I64d&fidx=%I64d&hashs=%s", KTH_HASH_URL_SENDCONTENTS, BoardIndex,FileIndex, Filehash);

	try
	{
		CoInitialize(NULL);	
		cnt_nums++;
		switch(cnt_nums)
		{
			case 1:
			m_pMainWnd->m_pSendKTHComplete1->Navigate(strData);
			break;
			case 2:
			m_pMainWnd->m_pSendKTHComplete2->Navigate(strData);
			break;
			case 3:
			m_pMainWnd->m_pSendKTHComplete3->Navigate(strData);
			break;
			case 4:
			m_pMainWnd->m_pSendKTHComplete4->Navigate(strData);
			break;
			case 5:
			m_pMainWnd->m_pSendKTHComplete5->Navigate(strData);
			break;
		}

		if(cnt_nums==5)	cnt_nums=0;

		CoUninitialize();

	}
	catch(...)
	{
	}

	return 0;
}



int Worker::Viruschecking(CString strFile)
{

	strFile.Replace( "/", "\\" );

	HINSTANCE hDLL = NULL;
	SFILTERVirusUpdateNow lpSFILTERVirusUpdateNow = NULL;
	hDLL = LoadLibrary(_T("sffilerun.dll"));
	if(hDLL)
	{
		int nRet;

		nRet=-1000;
		//������Ʈ üũ
		lpSFILTERVirusUpdateNow = (SFILTERVirusUpdateNow)::GetProcAddress(hDLL, "VirusUpdateNow");
		if(lpSFILTERVirusUpdateNow)
			nRet = lpSFILTERVirusUpdateNow();


		//������Ʈ ����
		if(nRet<0)
		{
			FreeLibrary(hDLL);
			return 1;
		}



		SFILTERVirusScan_Init lpSFILTERVirusScan_Init = NULL;
		SFILTERVirusScan_Scan lpSFILTERVirusScan_Scan = NULL;
		SFILTERVirusScan_Uninit lpSFILTERVirusScan_Uninit = NULL;

		lpSFILTERVirusScan_Init = (SFILTERVirusScan_Init)::GetProcAddress(hDLL, "VirusScan_Init");
		lpSFILTERVirusScan_Scan = (SFILTERVirusScan_Scan)::GetProcAddress(hDLL, "VirusScan_Scan");
		lpSFILTERVirusScan_Uninit= (SFILTERVirusScan_Uninit)::GetProcAddress(hDLL, "VirusScan_Uninit");


		if(lpSFILTERVirusScan_Init && lpSFILTERVirusScan_Scan && lpSFILTERVirusScan_Uninit)
		{
			do
			{
				int nRet;
				nRet = lpSFILTERVirusScan_Init(false);
				if(nRet < 0)
				{
					FreeLibrary(hDLL);
					return 1;
				}
				TCHAR strVirusCode[MAX_PATH +2];
				memset(strVirusCode,0,sizeof(strVirusCode));
				nRet = lpSFILTERVirusScan_Scan((LPTSTR)(LPCTSTR)strFile,strVirusCode);

				if(nRet < 0)
				{
					lpSFILTERVirusScan_Uninit();
					FreeLibrary(hDLL);
					return 1;
				}


				nRet = lpSFILTERVirusScan_Uninit();
				if(nRet < 0)
				{
					FreeLibrary(hDLL);
					return 1;
				}
			}while(0);
		}
		else
		{
			FreeLibrary(hDLL);
			return 1;
		}





		FreeLibrary(hDLL);




		return 0;
	}
	else
	{
		return 5;
	// DLL �ε� ���� ó��
	}






















	/*
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	typedef BOOL (*FUCNSCANSINGLEFILE)(AVINFO AvInfo, PSCANINFO pScanInfo);

	AVINFO AvInfo;

	CHAR szCurrentPath[MAX_PATH];

	DWORD dwStartTickCount = 0;
	DWORD dwEndTickCount = 0;
	DWORD dwTickCount = 0;
	DWORD dwSecond = 0;
	DWORD dwMilisecond = 0;

	FUCNSCANSINGLEFILE pFuncScanSingleFile;

	HMODULE hModule = NULL;

	PCHAR pBuff = NULL;

	SCANINFO ScanInfo;

	dwStartTickCount = GetTickCount();

	ZeroMemory(&AvInfo, AVINFO_SIZE);
	ZeroMemory(szCurrentPath, MAX_PATH);
	ZeroMemory(&ScanInfo, SCANINFO_SIZE);


	if (strFile.GetLength() <= 0)
	{
		return 6;
	}

	hModule = LoadLibrary("TvScan.dll");
	if (hModule == NULL)
	{
		return 5;
	}

	pFuncScanSingleFile = (FUCNSCANSINGLEFILE)GetProcAddress(hModule, "ScanSingleFile");
	if (pFuncScanSingleFile == NULL)
	{
		FreeLibrary(hModule);
		return 4;
	}

	GetModuleFileName(NULL, szCurrentPath, MAX_PATH);
	pBuff = strrchr(szCurrentPath, '\\');
	if (pBuff == NULL)
	{
		FreeLibrary(hModule);
		return 3;
	}

	*pBuff = NULL;

	strcpy_s(AvInfo.szAVPath, MAX_PATH, szCurrentPath);
	sprintf_s(AvInfo.szVDBPath, MAX_PATH, "%s\\Vdb", szCurrentPath);

	strncpy_s(ScanInfo.szFileName, MAX_PATH * 2, strFile.GetBuffer(strFile.GetLength()), strFile.GetLength());

	pFuncScanSingleFile(AvInfo, &ScanInfo);

	if (pFuncScanSingleFile(AvInfo, &ScanInfo) == FALSE)
	{
		return 2;
	}

	FreeLibrary(hModule);

	if (ScanInfo.dwInfected == SCAN_INFECTED)
	{
		return 1;
	}
	else
		return 0;
		*/
}

/*
int Worker::MovieBlockchecking(CString strFile)
{
	//
	return 0;

	HMODULE m_hModule;
	FuncScanInit m_lpFuncScanInit;
	FuncScanSetOption m_lpFuncScanSetOption;
	FuncScanMovieScanList m_lpFuncScanMovieScanList;
	FuncScanClean m_lpFuncScanClean;

	PMOVIEDATA m_pMovieData;
	DBINFO m_DBInfo;


	m_hModule = LoadLibrary(_T("TvMScan.dll"));
	if (m_hModule == NULL)
		return 1;

	m_lpFuncScanInit = (FuncScanInit)GetProcAddress(m_hModule, "Init");
	if (m_lpFuncScanInit == NULL)
	{
		FreeLibrary(m_hModule);
		return 2;
	}

	m_lpFuncScanSetOption = (FuncScanSetOption)GetProcAddress(m_hModule, "SetOption");
	if (m_lpFuncScanSetOption == NULL)
	{
		FreeLibrary(m_hModule);
		return 3;
	}

	m_lpFuncScanMovieScanList = (FuncScanMovieScanList)GetProcAddress(m_hModule, "MovieScanList");
	if (m_lpFuncScanMovieScanList == NULL)
	{
		FreeLibrary(m_hModule);
		return 4;
	}

	m_lpFuncScanClean = (FuncScanClean)GetProcAddress(m_hModule, "CleanMovie");
	if (m_lpFuncScanClean == NULL)
	{
		FreeLibrary(m_hModule);
		return 5;
	}
	


	//��ġ ���� üũ - S
	CString strFileName;
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));
	::SHGetSpecialFolderPath(NULL,pPath,CSIDL_PROGRAM_FILES,false);
	strFileName.Format("%s\\%s", pPath, CLIENT_APP_PATH);
	//��ġ ���� üũ - E



	//DB ���
	StringCchPrintf(m_DBInfo.szDBPath, MAX_PATH * 2, _T("%s\\videos.InL"), strFileName);

	//�ø���Ű
	StringCchCopy(m_DBInfo.szLKey, MAX_PATH, _T("166319E10B9E8931"));
	

	///////////////////////////////////////////////////////////////////////////////
	// �ʱ�ȭ ���� ����� �մϴ�.
	INT nRet = m_lpFuncScanInit(m_DBInfo);
	
	// ���� �޼���
	switch (nRet)
	{
		case ERROR_SERIAL:
			return 4;
			break;

		case ERROR_DB_NOT_FOUND:
			return 5;
			break;

		case ERROR_DB:
			return 6;
	}

	// �ɼ� ������ - ����ġ�� / ���� ����
	//
	// 1������ 0 - ����ġ��,1 - �ڵ� ġ�� / 2������ 0 - ���� ���� , 1 - ���� ��
	m_lpFuncScanSetOption(0, 0); // ����ġ�� & ���� �� ��

	int nCount=1,nItem=0;

	m_pMovieData = new MOVIEDATA [nCount];

	ZeroMemory(m_pMovieData, MOVIEDATA_SIZE * nCount);
	StringCchCopy(m_pMovieData[nItem].szFilePath, MAX_PATH *2, strFile.GetBuffer(strFile.GetLength()));
	OutputDebugString(m_pMovieData[nItem].szFilePath);

	//�˻�
	m_lpFuncScanMovieScanList(m_pMovieData, nCount);

	for (nItem = 0; nItem < nCount; nItem ++)
	{
		if (m_pMovieData[nItem].dwInfected == SCAN_INFECTED)
		{
			return 100;
		}
	}



	return 0;
}
*/
