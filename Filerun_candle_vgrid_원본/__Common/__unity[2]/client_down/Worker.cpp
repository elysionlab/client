#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "down_window.h"
#include "FtpClient.h"
#include "Dlg_Confirm.h"

//ĵ��̵��
#include "sfilter_filerun.h"

#pragma warning(disable:4996)

//IMBC DLLȣ��
//typedef CString (WINAPI* SENDMBCLOG_OSP) (CString orderid , CString ospid, CString contentsid, CString sellerid, CString buyerid, CString price, CString mdate, CString payid, CString webck, CString contents_name, CString contents_num, CString bbs_title ,CString bbs_num, CString file_name);



Worker::Worker(down_window* pMainWnd) : NetworkController()
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
					m_nReConnectCount = 0;					
					m_bReConnectTrans = m_pMainWnd->m_bFileTrans;
					
					MMServerFree();

					m_pMainWnd->SetTransFlag(false);

					m_pMainWnd->KillTimer(100);

					//20071119 �޼��� ���� ���� ���� Ÿ�̸� ����(5��), jyh
					m_pMainWnd->SetTimer(99, 300000, NULL);

					//::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW,
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"���� �������� ������ ����Ǿ� 5�� �ڿ� �ڵ� ����˴ϴ�.\r\n���� �ٷ� ���� �Ͻðڽ��ϱ�?", 
								(LPARAM)2);

					m_pMainWnd->m_pPtrTrans = NULL;		//20071119 ������ �������� �������� ����, jyh
					
					//20071119 ����Ʈ �ڽ��� ������ ���� ���� ����� ����, jyh
					CString strBuf;
					strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_STOP);
					m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
					m_pMainWnd->m_pWorker_Ctrl->m_bReConnectTrans = true;
					m_pMainWnd->m_pWorker_Ctrl->m_bConnectServer = false;	//20081025 ���� ����, jyh
	
					//��(�ٷ� ����)
					if(m_pMainWnd->m_nBtnState == BTN_STATE_YES)
					{
						m_pMainWnd->KillTimer(99);	//������ 5������ ���õƴ� Ÿ�̸� ����
						m_pMainWnd->SetTimer(99, 1000, NULL);
					}
					//�ƴϿ�
					else
						m_pMainWnd->KillTimer(99);

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


		// �ٿ�ε� ��� ��û
		case LOAD_DOWNLIST:
		{
			Result_Item_Add(pPacket);
			break;
		}

		// �ٿ�ε� �� ���� ���� üũ
		case LOAD_DOWNPRE:
		{
			Result_DownLoadPre(pPacket);
			break;
		}
		// ���� �ٿ�ε�
		case LOAD_DOWNLOAD:
		{
			Result_DownLoadPro(pPacket);
			break;
		}
		// �ٿ�ε� �Ϸ� �뺸 ���
		case LOAD_DOWNEND:
		{
			Result_DownLoadEnd(pPacket);
			break;
		}

		// �ٸ���ǻ�� �ߺ�����
		case CHK_DUPLI_CLOSE:
		{
			//20071130 ������ �α����̸� �ߺ����� ����, jyh
			if(!lstrcmp(m_pMainWnd->m_pUserID, "admin"))
				break;

			m_pMainWnd->SetTransFlag(false);
			Result_Dupl_Close(pPacket);
			//20071121 ��Ʈ�� ����, jyh
			::SendMessage(m_pMainWnd->m_hWnd, WM_CLOSE, 0, 0);
			break;
		}
		// ���۱� �ڷ� ó��
		case SAVE_MUREKA_DATA:
		{
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
	PINFOAUTH pLogin = (PINFOAUTH)pPacket->GetData();



	try
	{
		switch(pLogin->nResult)
		{
			
			case LOGIN_RESULT_AUTH:
			{
				m_pMainWnd->m_bAuth			= true;
				m_pMainWnd->m_pUserID		= pLogin->pUserID;
				m_pMainWnd->m_pUserPW		= pLogin->pUserPW;
				m_pMainWnd->m_nSpeed		= pLogin->nSpeed;	//20081022 ���ǵ� ��, jyh

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


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٿ�ε带 �����ϱ� ���� �ش� ������ ���� �� �ٿ�ε� ���� ���� ��������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_DownLoadPre(Packet* pPacket)
{
	CString strBuf;
	
	try
	{
		PLOADPREDOWN pPredown = (PLOADPREDOWN)pPacket->GetData();

		if(m_pMainWnd->m_pPtrTrans == NULL)
		{
			::SendMessage(m_pMainWnd->m_hWnd, MSG_DOWNLOAD, 0, 0);
			return;
		}

		if(pPredown->nResult == DOWN_RESULT_OK)
		{
			__int64 nTrsStartSize = 0;

			//20090218 ���� ����� 0�϶��� ���� ���� üũ
			if(m_pMainWnd->m_pPtrTrans->nFileSizeEnd == 0)
			{
				// ���ÿ� ���� �̸��� ������ �����ϴ��� üũ
				CFileFind pFileFind;
				CString strFName = m_pMainWnd->m_pPtrTrans->pFullPath;

				if(pFileFind.FindFile((LPCTSTR)strFName))
				{	
					pFileFind.FindNextFile();
					nTrsStartSize = (__int64)pFileFind.GetLength();

					//20090217 ����, jyh--------------------------------------------------
					m_pMainWnd->m_pPtrTrans->nFileSizeEnd = nTrsStartSize;	//���� ������ ����� ���Ѵ�.
					m_pMainWnd->m_nFileSizeEnd += nTrsStartSize;
					//-------------------------------------------------------
					
					if(nTrsStartSize > 0)
					{
						// �Ϸ�� �������� üũ
						if(m_pMainWnd->m_pPtrTrans->nFileSizeReal <= nTrsStartSize)
						{
							m_pMainWnd->m_pPtrTrans->nTransState	= ITEM_STATE_COMPLETE;
							m_pMainWnd->m_pPtrTrans->nFileSizeEnd = m_pMainWnd->m_pPtrTrans->nFileSizeReal;

							//20090217 ����, jyh--------------------------------------------------
							//���� ���� ���� ���α׷���
							CString strBuf, strCurSize, strSize;
							int nCurPercent = abs((int)((m_pMainWnd->m_pPtrTrans->nFileSizeEnd * 1000000) / m_pMainWnd->m_pPtrTrans->nFileSizeReal));		
							m_pMainWnd->m_pCtrl_ProgressCur.SetPos(nCurPercent);

							if((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000000)
								strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000000)%1000,
																			((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000)%1000,
																			(m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)%1000);
							else
							{
								if((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000)
									strCurSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000,
																		(m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)%1000);
								else
								{
									if(m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)
										strCurSize.Format("%I64dKB", m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024);
									else
										strCurSize.Format("%I64dB", m_pMainWnd->m_pPtrTrans->nFileSizeEnd);
								}
							}

							if((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000000)
								strSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000000)%1000,
																		((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000)%1000,
																		(m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)%1000);
							else
							{
								if((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000)
									strSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000,
																	(m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)%1000);
								else
								{
									if(m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)
										strSize.Format("%I64dKB", m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024);
									else
										strSize.Format("%I64dB", m_pMainWnd->m_pPtrTrans->nFileSizeReal);
								}
							}

							strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, nCurPercent/10000);
							//strBuf.Format("%s(%d%%)", strCurSize, nCurPercent/10000);
							m_pMainWnd->m_pCtrl_ProgressCur.SetText(strBuf);

							//��ü���� ���� ���α׷���
							int nTotalPercent = abs((int)((m_pMainWnd->m_nFileSizeEnd * 1000000) / m_pMainWnd->m_nFileSizeTotal));		
							m_pMainWnd->m_pCtrl_ProgressTotal.SetPos(nTotalPercent);

							if((m_pMainWnd->m_nFileSizeEnd/1024)/1000000)
								strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_nFileSizeEnd/1024)/1000000)%1000,
																			((m_pMainWnd->m_nFileSizeEnd/1024)/1000)%1000,
																			(m_pMainWnd->m_nFileSizeEnd/1024)%1000);
							else
							{
								if((m_pMainWnd->m_nFileSizeEnd/1024)/1000)
									strCurSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_nFileSizeEnd/1024)/1000,
																		(m_pMainWnd->m_nFileSizeEnd/1024)%1000);
								else
								{
									if(m_pMainWnd->m_nFileSizeEnd/1024)
										strCurSize.Format("%I64dKB", m_pMainWnd->m_nFileSizeEnd/1024);
									else
										strCurSize.Format("%I64dB", m_pMainWnd->m_nFileSizeEnd);
								}
							}

							if((m_pMainWnd->m_nFileSizeTotal/1024)/1000000)
								strSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_nFileSizeTotal/1024)/1000000)%1000,
																		((m_pMainWnd->m_nFileSizeTotal/1024)/1000)%1000,
																		(m_pMainWnd->m_nFileSizeTotal/1024)%1000);
							else
							{
								if((m_pMainWnd->m_nFileSizeTotal/1024)/1000)
									strSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_nFileSizeTotal/1024)/1000,
																	(m_pMainWnd->m_nFileSizeTotal/1024)%1000);
								else
								{
									if(m_pMainWnd->m_nFileSizeTotal/1024)
										strSize.Format("%I64dKB", m_pMainWnd->m_nFileSizeTotal/1024);
									else
										strSize.Format("%I64dB", m_pMainWnd->m_nFileSizeTotal);
								}
							}

							//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
							strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, nTotalPercent/10000);
							m_pMainWnd->m_pCtrl_ProgressTotal.SetText(strBuf);
							//----------------------------------------------------------------	

							m_pMainWnd->m_pPtrTrans = NULL;
							::SendMessage(m_pMainWnd->m_hWnd, MSG_DOWNLOAD, 0, 0);

							pFileFind.Close();	
							return;
						}
					}
					m_bReConnectTrans = false;
				}
				pFileFind.Close();
			}
			
			//20090217 ����, jyh--------------------------------------------------
			//���� ���� ���� ���α׷���
			CString strBuf, strCurSize, strSize;
			int nCurPercent = abs((int)((m_pMainWnd->m_pPtrTrans->nFileSizeEnd * 1000000) / m_pMainWnd->m_pPtrTrans->nFileSizeReal));		
			m_pMainWnd->m_pCtrl_ProgressCur.SetPos(nCurPercent);

			if((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000000)
				strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000000)%1000,
															((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000)%1000,
															(m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)%1000);
			else
			{
				if((m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000)
					strCurSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)/1000,
														(m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)%1000);
				else
				{
					if(m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024)
						strCurSize.Format("%I64dKB", m_pMainWnd->m_pPtrTrans->nFileSizeEnd/1024);
					else
						strCurSize.Format("%I64dB", m_pMainWnd->m_pPtrTrans->nFileSizeEnd);
				}
			}

			if((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000000)
				strSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000000)%1000,
														((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000)%1000,
														(m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)%1000);
			else
			{
				if((m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000)
					strSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)/1000,
													(m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)%1000);
				else
				{
					if(m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024)
						strSize.Format("%I64dKB", m_pMainWnd->m_pPtrTrans->nFileSizeReal/1024);
					else
						strSize.Format("%I64dB", m_pMainWnd->m_pPtrTrans->nFileSizeReal);
				}
			}

			strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, nCurPercent/10000);
			//strBuf.Format("%s(%d%%)", strCurSize, nCurPercent/10000);
			m_pMainWnd->m_pCtrl_ProgressCur.SetText(strBuf);

			//��ü���� ���� ���α׷���
			int nTotalPercent = abs((int)((m_pMainWnd->m_nFileSizeEnd * 1000000) / m_pMainWnd->m_nFileSizeTotal));		
			m_pMainWnd->m_pCtrl_ProgressTotal.SetPos(nTotalPercent);

			if((m_pMainWnd->m_nFileSizeEnd/1024)/1000000)
				strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_nFileSizeEnd/1024)/1000000)%1000,
															((m_pMainWnd->m_nFileSizeEnd/1024)/1000)%1000,
															(m_pMainWnd->m_nFileSizeEnd/1024)%1000);
			else
			{
				if((m_pMainWnd->m_nFileSizeEnd/1024)/1000)
					strCurSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_nFileSizeEnd/1024)/1000,
														(m_pMainWnd->m_nFileSizeEnd/1024)%1000);
				else
				{
					if(m_pMainWnd->m_nFileSizeEnd/1024)
						strCurSize.Format("%I64dKB", m_pMainWnd->m_nFileSizeEnd/1024);
					else
						strCurSize.Format("%I64dB", m_pMainWnd->m_nFileSizeEnd);
				}
			}

			if((m_pMainWnd->m_nFileSizeTotal/1024)/1000000)
				strSize.Format("%I64d,%03I64d,%03I64dKB", ((m_pMainWnd->m_nFileSizeTotal/1024)/1000000)%1000,
														((m_pMainWnd->m_nFileSizeTotal/1024)/1000)%1000,
														(m_pMainWnd->m_nFileSizeTotal/1024)%1000);
			else
			{
				if((m_pMainWnd->m_nFileSizeTotal/1024)/1000)
					strSize.Format("%I64d,%03I64dKB", (m_pMainWnd->m_nFileSizeTotal/1024)/1000,
													(m_pMainWnd->m_nFileSizeTotal/1024)%1000);
				else
				{
					if(m_pMainWnd->m_nFileSizeTotal/1024)
						strSize.Format("%I64dKB", m_pMainWnd->m_nFileSizeTotal/1024);
					else
						strSize.Format("%I64dB", m_pMainWnd->m_nFileSizeTotal);
				}
			}

			//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
			strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, nTotalPercent/10000);
			m_pMainWnd->m_pCtrl_ProgressTotal.SetText(strBuf);
			//----------------------------------------------------------------	
		

			// �ٿ�ε带 �����ϱ� ���� Ÿ�� ��ũ�� �ܿ� �뷮�� ���۹��� �뷮���� ������ üũ
			__int64 nFreeSize = m_pMainWnd->GetDiskFreeSpaceMMSV((LPCTSTR)(LPSTR)m_pMainWnd->m_pPtrTrans->pFullPath);

			if(nFreeSize <= (m_pMainWnd->m_pPtrTrans->nFileSizeReal - nTrsStartSize))
			{
				m_pMainWnd->SetTransFlag(false);
				
				m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_STOP;
				m_pMainWnd->SetTransFlag(false);
				

				CString strTargetPath = (LPCTSTR)(LPSTR)m_pMainWnd->m_pPtrTrans->pFullPath;
				CString strTargetVolume = strTargetPath.Left(strTargetPath.Find("\\"));

				m_pMainWnd->m_pPtrTrans	= NULL;	

				char pCancelState[2048];
				sprintf_s(pCancelState, 2048, "���õ�ũ�� �뷮�� �����մϴ�! Ȯ���� �ٽ� �õ��� �ֽʽÿ�.\r\n\r\n[%s] ����̺��� ���� �뷮�� Ȯ���ϼ���!", strTargetVolume);
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pCancelState, (LPARAM)0);										
				return;
			}	

			// ���� �ý��� ������ üũ
			if(m_pMainWnd->m_pPtrTrans->nFileSizeReal >= 4294000000)
			{
				CString strTargetPath = (LPCTSTR)(LPSTR)m_pMainWnd->m_pPtrTrans->pFullPath;
				CString strTargetVolume = strTargetPath.Left(strTargetPath.Find("\\") + 1);


				DWORD volumeSerial, maxFileNameLength;
				char nameBuffer[101] = "", fileSystem[101] = "";

				BOOL bResult = GetVolumeInformation(
								strTargetVolume, (LPTSTR)nameBuffer, 100, &volumeSerial, &maxFileNameLength, NULL, fileSystem, 100);

				if(bResult != 0 && strcmp(fileSystem, "NTFS"))
				{
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					m_pMainWnd->m_pPtrTrans	= NULL;	

					char pCancelState[1024];
					sprintf_s(pCancelState, 1024, "���� ���� �ý����� %s �̹Ƿ� 4GB�̻��� ������ �ٿ�ε� �� �� �����ϴ�.\r\n4GB�̻��� ���� �ٿ�ε�� NTFS�� �����մϴ�.\r\n\r\n�� ������ ��������մϴ�!", fileSystem);
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pCancelState, (LPARAM)0);
					::SendMessage(m_pMainWnd->m_hWnd, MSG_DOWNLOAD, 0, 0);
					return;
				}					
			}
							
			// �ٿ�ε� ������ ���� ������ä�� ��û		
			LOADDOWN pDownInfo;
			pDownInfo.nFileType			= m_pMainWnd->m_pPtrTrans->nFileType;
			pDownInfo.nServerVolume		= m_pMainWnd->m_pPtrTrans->nServerVolume;
			pDownInfo.nBoardIndex_		= m_pMainWnd->m_pPtrTrans->nBoardIndex_;
			pDownInfo.nBoardIndex		= m_pMainWnd->m_pPtrTrans->nBoardIndex;
			pDownInfo.nFileIdx			= m_pMainWnd->m_pPtrTrans->nFileIndex;
			pDownInfo.nDownSpeed		= m_pMainWnd->m_nSpeed;		//20081022 ���ǵ尪 �Է�, jyh
			pDownInfo.nDownStartPoint	= m_pMainWnd->m_pPtrTrans->nFileSizeEnd;
			pDownInfo.nResult			= 0;
			ZeroMemory(pDownInfo.pServerInfo, sizeof(pDownInfo.pServerInfo));

			Packet* pPacket = new Packet;
			pPacket->Pack(LOAD_DOWNLOAD, (char*)&pDownInfo, sizeof(LOADDOWN));
			SendPacket(pPacket);
			return;
		}
		else
		{
			switch(pPredown->nResult)
			{				
				// �˼� ���� ����
				case DOWN_RESULT_ERROR:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"�˼��մϴ�!\r\n���� �ش����Ͽ� �ٿ�ε尡 �ʹ� �����ϴ�.\r\n����� �ٽ� �̿����ּ���.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					break;
				}				
				// ������ ���� ���� ���� ����
				case DOWN_RESULT_NOTFOUND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"������ �������� �ʽ��ϴ�.\r\n������ �����Ǿ��ų� �Ⱓ�� ����� �����Դϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					break;
				}
				// ���ε尡 �Ϸ���� ���� ����
				case DOWN_RESULT_UPLOAD:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"���ε尡 �Ϸ���� ���� �����Դϴ�.\r\n�Ϸ�� �����̸� �����ڿ��� ���� �ٶ��ϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					break;
				}				
				// ���񽺰� ������ ����
				case DOWN_RESULT_SERVICE_STOP:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"���� ���������� �����Ǿ����ϴ�!\r\n�����߿��� ������ �� �����ϴ�.\r\nȨ�������� �����ϼ���.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
			}

			strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
			m_pMainWnd->m_pPtrTrans = NULL;
			m_pMainWnd->m_bFileTrans = false;
			
			//20071219 �ٿ�ε� ���н� ���� ���Ϸ� �ǳʶ�, jyh
			if(m_pMainWnd->DeferItem() > 0)
				m_pMainWnd->SetTimer(98, 100, NULL);
			else
				m_pMainWnd->SetTransFlag(false);			
		}
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ٿ�ε� ����", 1);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٿ�ε带 ���� �ٿ�ε� ���� ���� ��û
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_DownLoadPro(Packet* pPacket)
{
	CString strBuf;
	int nItemPatten = 0;		//20081025 ��õ�â������ ��ư ��(-1: ���, 1: �ǳʶٱ�, 2: ��õ�), jyh
	BOOL bRetryAlret = FALSE;	//20081025 ��õ� â������ ��ư ���� ����, jyh

	try
	{
		PLOADDOWN pDownPro = (PLOADDOWN)pPacket->GetData();

		if(m_pMainWnd->m_pPtrTrans == NULL)
		{
			::SendMessage(m_pMainWnd->m_hWnd, MSG_DOWNLOAD, 0, 0);
			return;
		}

		m_pMainWnd->m_nSvrResult = pDownPro->nResult;


		if(pDownPro->nResult == DOWN_RESULT_OK)
		{

			m_pMainWnd->m_pPtrTrans->nFileSizeStart	= pDownPro->nDownStartPoint;
			m_pMainWnd->m_pPtrTrans->nFileSizeEnd	= pDownPro->nDownStartPoint;
			
			strcpy_s(m_pMainWnd->m_pPtrTrans->m_pServerInfo,pDownPro->pServerInfo);



//			m_pMainWindow->m_pPtrTrans->Grid_use = false;
			if(!m_pMainWnd->m_pFtpClint->DownLoadEx(pDownPro->pServerInfo, 
				m_pMainWnd->m_pPtrTrans->pFullPath, pDownPro->nDownStartPoint))
/*
			if(!m_pMainWnd->m_pFtpClint->DownLoad(pDownPro->pServerInfo, 
				m_pMainWnd->m_pPtrTrans->pFullPath, pDownPro->nDownStartPoint))
*/
			{


				// ������ �������� �������Ƿ� ��������� ����
				CHECKDOWNEND pTransEndInfo;
				pTransEndInfo.nFileType		= m_pMainWnd->m_pPtrTrans->nFileType;
				pTransEndInfo.nBoardIndex_	= m_pMainWnd->m_pPtrTrans->nBoardIndex_;
				pTransEndInfo.nBoardIndex	= m_pMainWnd->m_pPtrTrans->nBoardIndex;
				pTransEndInfo.nFileIdx		= m_pMainWnd->m_pPtrTrans->nFileIndex;
				pTransEndInfo.nSizeBegin	= 0;
				pTransEndInfo.nSizeEnd		= 0;

				Packet* pPacketSend = new Packet;
				pPacket->Pack(LOAD_DOWNEND, (char*)&pTransEndInfo, sizeof(CHECKDOWNEND));
				SendPacket(pPacketSend);

				if(m_pMainWnd->m_pPtrTrans)
				{
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_ERROR;
					strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_ERROR);
					m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
				}

				m_pMainWnd->m_pPtrTrans = NULL;
				m_pMainWnd->SetTimer(98, 5000, NULL);
				return;
			}

			int		nDummy;
			HANDLE	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcFileTransLoop, m_pMainWnd, 0, (unsigned int *)&nDummy);
			CloseHandle(hTrans);	

			m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_TRANS;
			strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_TRANS);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);	
			return;
		}
		else
		{
			switch(pDownPro->nResult)
			{				
				// �˼� ���� ����
				case DOWN_RESULT_ERROR:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"�˼��մϴ�!\r\n���� �ش����Ͽ� �ٿ�ε尡 �ʹ� �����ϴ�.\r\n����� �ٽ� �̿����ּ���.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}				
				// ������ ���� ���� ���� ����
				case DOWN_RESULT_NOTFOUND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"������ �������� �ʽ��ϴ�.\r\n������ �����Ǿ��ų� �Ⱓ�� ����� �����Դϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				// ���ε尡 �Ϸ���� ���� ����
				case DOWN_RESULT_UPLOAD:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"���ε尡 �Ϸ���� ���� �����Դϴ�.\r\n�Ϸ�� �����̸� ��ڿ��� ���� �ٶ��ϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				// ���ε�� ������� ū������ �䱸��
				case DOWN_RESULT_SIZEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"���ε�� ������ ���� ū �ٿ�ε� ���� �����Դϴ�.\r\n���� ������ ��ӵǸ� ��ڿ��� ���� �ٶ��ϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				case DOWN_RESULT_ERROR_DELAY:	//20081027 ftp ���� ���� ����, jyh
				case DOWN_RESULT_FTP_CONFAILED:	//20081027 ���ϼ��� ���� ����, jyh
				// ������ �����ϴ� ������ � ���� ����
				case DOWN_RESULT_SERVERSTOP:
				// ���ϼ����� ���Ϸ� �ٿ�ε� �Ұ�(������)
				case DOWN_RESULT_SERVERBUSY:
				{
					m_pMainWnd->m_pPtrTrans->nCountDefer++;
					if(m_pMainWnd->m_pPtrTrans->nCountDefer >= 3)
					{
						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"�ش������� �������Ϸ� ������ �Ұ����մϴ�\r\n���� ������ �����մϴ�.", (LPARAM)1);
						
						//20081027 ����, jyh
						if(pDownPro->nResult == DOWN_RESULT_ERROR_DELAY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DELAY;	//���� ����
						else if(pDownPro->nResult == DOWN_RESULT_SERVERBUSY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DEFER;	//����� �ʰ�
						else
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CONFAILED;//�������ӽ���
					}
					else 
					{
						char szText[MAX_PATH];
						sprintf_s(szText, MAX_PATH, "�ش� ���Ͽ� ����ڰ� ���� �ٿ�ε尡 �����ǰ� �ֽ��ϴ�.\r\n"
							"30�� ���� �Է��� ���� ��� �ش� ������ ��õ� �մϴ�.\r\n"
							"���� ��õ� Ƚ��: %d", 3 - m_pMainWnd->m_pPtrTrans->nCountDefer);

						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)szText, (LPARAM)3);
						
						bRetryAlret = TRUE; //20081025 ��õ� â, jyh
						
						//20081027 ����, jyh
						if(pDownPro->nResult == DOWN_RESULT_ERROR_DELAY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DELAY;	//���� ����
						else if(pDownPro->nResult == DOWN_RESULT_SERVERBUSY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DEFER;	//����� �ʰ�
						else
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CONFAILED;//�������ӽ���

						//20071119 ����Ʈ �ڽ��� ������ ���� ���� ����� ����, jyh
						strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_DEFER);
						m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
						
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
						//else if(nItemPatten == 1) 
						//{
						//	//m_pMainWnd->SetTimer(98, 30000, NULL);
						//	if(m_pMainWnd->DeferItem() <= 0)
						//	{
						//		::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW,
						//			(WPARAM)"�ǳʶ� ������ �����ϴ�.\r\n", (LPARAM)1);
						//		m_pMainWnd->SetTransFlag(false);
						//	}							
						//}
						//else
						//{
						//	m_pMainWnd->m_bFileTrans = false;
						//	m_pMainWnd->SetTimer(98, 100, NULL);	
						//}
					}

					break;
				}				
				// ȸ������ ���� üũ ����
				case DOWN_RESULT_INFOFAIL:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"�ٿ�ε带 ���� ������ üũ�ϴµ� ���� �߽��ϴ�.\r\n����Ŀ� �ٽ� �õ��ϼ���.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_STOP;
					break;
				}				
				// �����ڷ� �Ⱓ ���
				case DOWN_RESULT_TIMEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ش� �������� ���űⰣ�� ���� �Ǿ����ϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}		
				// ������ ���� �ð� �ƴ�
				case DOWN_RESULT_FIX_TIMEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"������ ���ð��� �ƴմϴ�.\r\n���ð��� Ȯ�����ּ���.", (LPARAM)0);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
                    //m_pMainWnd->SetTimer(98, 500, NULL);
					m_pMainWnd->m_pPtrTrans = NULL;
					m_pMainWnd->SetTransFlag(false);
					return;
				}		
				// ������ �Ⱓ ����
				case DOWN_RESULT_FIX_DATEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"������ ���ð��� �ʰ��Ǿ����ϴ�.", (LPARAM)0);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					m_pMainWnd->m_pPtrTrans = NULL;
					m_pMainWnd->SetTransFlag(false);
					return;
				}	
				// �����ڷ�ǿ��� ������ �ڷᰡ �ƴҶ�
				case DOWN_RESULT_OTHER_USER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"��α� �����ڷ��� �ٿ�ε�� �����ڸ� �ٿ�ε� �����մϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				// ������ ȯ�ҽ��ε� ��� 
				case DOWN_RESULT_PAY_CANCEL:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�� �ڷ�� ȯ��ó���� �ڷ��Դϴ�.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				//20080311 ���� �ٿ�ε� �ָ� ��� ����, jyh
				case DOWN_RESULT_FREE_WEEKEND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ָ�(�����, �Ͽ���)�� ���� �ٿ�ε带 �� �� �����ϴ�!", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_NOFREEDOWN;
					break;
				}
				//20080311 ���� �ٿ�ε� Ư�� �ð��� ��� ����, jyh
				case DOWN_RESULT_FREE_TIMEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���� ���� 3�� ���� ���� 5�� ������ ���� �ٿ�ε带 �� �� �����ϴ�!", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_NOFREEDOWN;
					break;
				}
				//20080311 ���� �ٿ�ε� �ο� �������� ��� �Ұ�, jyh
				case DOWN_RESULT_FREE_LIMIT_COUNT:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ش� ���� ������ ����ڰ� ���� ���� �ٿ�ε带 �� �� �����ϴ�!", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_NOFREEDOWN;
					break;
                }
			}

			strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
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
				//20071219 �ٿ�ε� ���н� ���� ���Ϸ� �ǳʶ�, jyh
				if(m_pMainWnd->DeferItem() > 0)
					m_pMainWnd->SetTimer(98, 100, NULL);
				else
					m_pMainWnd->SetTransFlag(false);
			}			
		}
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ٿ�ε� ����", 1);
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ٿ�ε� �Ϸ� �뺸 ���
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_DownLoadEnd(Packet* pPacket)
{
	try
	{
		PCHECKDOWNEND pDownEnd = (PCHECKDOWNEND)pPacket->GetData();
	}
	catch(...)
	{
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���۹��� ������ ��� �߰� :: bbs ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Item_Add(Packet* pPacket)
{
	CString pPath;
	CString pFileName;
	CString pFileFolder;
	CString pPartnerContentID;
	CONITEM conitem;


	bool Copyright_check_DLL=false,Copyright_Muraka_stop,Mureka_paylog_re_First;


/////////////////ĵ��̵�� ��� �ε� - S

	time_t timer;
	struct tm timeinfo;

	HINSTANCE hDLL = NULL;
	SFILTERInit		lpSFILTERInit = NULL;
	SFILTERDownload	lpSFILTERDownload = NULL;
	SFILTERVideoPaidReport	lpSFILTERVideoPaidReport = NULL;
	SFILTEREnd		lpSFILTEREnd = NULL;
	UPLOADMUREKA	pTransMurekaInfo;


		if(m_pMainWnd->Mureka_use)
		{

			hDLL = LoadLibrary(_T("sffilerun.dll"));

			if(hDLL)
			{
				lpSFILTERInit	 = (SFILTERInit)::GetProcAddress(hDLL, "Init");
				lpSFILTERDownload= (SFILTERDownload)::GetProcAddress(hDLL, "Download");
				lpSFILTERVideoPaidReport= (SFILTERVideoPaidReport)::GetProcAddress(hDLL, "VideoPaidReport");
				lpSFILTEREnd	 = (SFILTEREnd)::GetProcAddress(hDLL, "End");
				
				if(lpSFILTERInit && lpSFILTERDownload && lpSFILTEREnd)
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

		}





		if(pPacket->m_pHead->ds > 0)
		{
			char* pItemList = pPacket->GetData();

			CToken*	pToken1 = new CToken(SEP_ITEM);	
			CToken*	pToken2 = new CToken(SEP_FIELD);	

			pToken1->Split(pItemList);

			if(pToken1->GetCount() > 0)
			{
				//20080311 ������ ������ �ٿ�ε� �ϷḦ ��Ÿ���� ����, jyh-----------
				pToken2->Split(pToken1->GetToken(0));
				conitem.nBoardIndex = atoi(pToken2->GetToken(2));
				conitem.nItemCnt	= (int)pToken1->GetCount();

				BOOL bExist = FALSE;
				int size = (int)m_pMainWnd->m_v_sConItem.size();

				for(int i=0; i<(int)m_pMainWnd->m_v_sConItem.size(); i++)
				{
					if(m_pMainWnd->m_v_sConItem[i].nBoardIndex == conitem.nBoardIndex)
					{
						bExist = TRUE;
						break;
					}
				}

				//20080314 �������� ���ٿ�ε� ��ȯ�϶� �����θ� ����� �ʴ´�, jyh
				if(!bExist)
				{
					m_pMainWnd->m_v_sConItem.push_back(conitem);
					//---------------------------------------------------------------------

					//20071015 ������ ������ 7�� �̻��̸� ����Ʈ ���� ũ�� ����, jyh
					if(m_pMainWnd->m_pListCtrl.GetItemCount() + pToken1->GetCount() > 7)
						m_pMainWnd->m_pListCtrl.SetColumnWidth(1, 382);

					// ���α׷��� �ֻ��� ������� ���
					/*m_pMainWnd->SetForegroundWindow();
					m_pMainWnd->ShowWindow(SW_RESTORE);
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

					// �������� �߰��� �� ������ üũ
					if( /*m_pMainWnd->m_pListCtrl.GetItemCount() > 0 && */
						(m_pMainWnd->m_rDownAddItemPath == DOWN_ITEM_ADD_CHECK || 
						(m_pMainWnd->m_rDownAddItemPath == DOWN_ITEM_ADD_LAST && 
						m_pMainWnd->m_rPathLast == "")))
					{			
						DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
						CSelectFolderDialog pSelectDialog(FALSE, m_pMainWnd->m_rPathLast, wFlag, NULL, m_pMainWnd, "����� ������ ������ �ּ���");
										
						if(pSelectDialog.DoModal() == IDCANCEL)	
						{
							delete pToken1;
							delete pToken2;
							return;
						}
						else
						{
							CString strPath = pSelectDialog.GetSelectedPath();
							m_pMainWnd->m_rPathLast = strPath;
							m_pMainWnd->m_pReg.SaveKey(CLIENT_REG_PATH, "rPathLast", strPath);
							//20071011 ���� ��� ���, jyh
							CString strTemp;
							m_pMainWnd->m_pStatic_Path.GetWindowText(strTemp);

							if(strTemp == "")
							{
								char sPath[4096];

								strcpy_s(sPath, strPath.GetLength()+1, strPath.GetBuffer());
								strPath.ReleaseBuffer();

								int nIdx = (int)strlen(sPath);
								sPath[nIdx-1] = 0;

								m_pMainWnd->m_pStatic_Path.SetWindowText(sPath);	//20070919 ���� ���, jyh

								RECT rt = { 90, 135, 451, 147 };
								InvalidateRect((HWND)*m_pMainWnd, &rt, FALSE);
							}
						}
					}
				}


				//20080311 ���ٿ����� ��ȯ�ϴ� ������ ��� ã�Ƽ� ���������� �ִ´�.(������ ����), jyh
				if(bExist)
				{
					int nCnt = 0;
					
					for(int i=0; i<m_pMainWnd->m_pListCtrl.GetItemCount(); i++)
					{
						PRDOWNCTX pItem = (PRDOWNCTX)m_pMainWnd->m_pListCtrl.GetItemData(i);
						if(pItem->nBoardIndex == conitem.nBoardIndex)
						{
							pItem->nBoardIndex_ = _atoi64(pToken2->GetToken(1));
							pItem->nServerVolume = atoi(pToken2->GetToken(7));
						
							//20080319 ���� �������� 0�̸� ����, jyh
							if(pItem->nServerVolume == 0)
								pItem->nFree = 1;	//����
							else
								pItem->nFree = 2;	//���

							//m_pMainWnd->m_pListCtrl.SetItemState(i, INDEXTOSTATEIMAGEMASK(pItem->nFree), LVIS_STATEIMAGEMASK);
							m_pMainWnd->m_pListCtrl.SetItemData(i, (DWORD_PTR)pItem);
							nCnt++;
						}

						if(nCnt == conitem.nItemCnt)
							break;
					}
					
					m_pMainWnd->m_pListCtrl.RedrawWindow();
					m_pMainWnd->m_pFtpClint->m_bFlag = false;
					Sleep(500);
					m_pMainWnd->m_pPtrTrans = NULL;		//20080324 ��� ��ȯ�� �� �ʿ���!!, jyh
				}
				//----------------------------------------------------------------------------------------
				else
				{
					// ���۹��� ����� ť�� ���
					for(size_t index_1 = 0; index_1 < pToken1->GetCount() ; index_1++)
					{
						pToken2->Split(pToken1->GetToken(index_1));

						//20080213 ���� ���� ���� �ּ� ����, jyh
						//if(pToken2->GetCount() != 7)
						//if(pToken2->GetCount() != 9)

						if(pToken2->GetCount() != 19)
							continue;






						//�����������ڵ尪
						pPartnerContentID.Format("%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(15));






						Copyright_Muraka_stop=false;


						if(m_pMainWnd->Mureka_use&&strcmp((LPTSTR)(LPCTSTR)pToken2->GetToken(9),"")!=0)
						{






							CString FilterStatus;
							LPSFILTER_RET p_sfilter_ret = NULL;
							LPSFILTER_RET_VIDEO p_sfilter_ret_video = NULL;
							LPSFILTER_RET_MUSIC p_sfilter_ret_music = NULL;

							lpSFILTERInit( 1, 0);
													
							int nRet =	lpSFILTERDownload(
															&p_sfilter_ret,
															&p_sfilter_ret_music,
															&p_sfilter_ret_video,
															(LPTSTR)(LPCTSTR)m_pMainWnd->m_pUserID,
															(LPTSTR)(LPCTSTR)pToken2->GetToken(9),
															atoi(pToken2->GetToken(2)),
															(LPTSTR)(LPCTSTR)pToken2->GetToken(18),
															(LPTSTR)(LPCTSTR)pToken2->GetToken(17),
															(LPTSTR)(LPCTSTR)pToken2->GetToken(13)
								);




							if(nRet==0)
							{


								PAYLOGMUREKA PayMureka_data;
								Mureka_paylog_re_First=false;

								///////���� �ð����� - S
								timer = time(NULL);
								localtime_s(&timeinfo,&timer);
								CString Now_time_String;
								Now_time_String.Format("%04d%02d%02d%02d%02d%02d",(timeinfo.tm_year+1900),timeinfo.tm_mon+1,timeinfo.tm_mday,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
								///////���� �ð����� - E



								strcpy_s(PayMureka_data.pPayDate,Now_time_String.GetLength()+1,Now_time_String);	//��¥
								strcpy_s(PayMureka_data.pOspUserID,m_pMainWnd->m_pUserID.GetLength()+1,m_pMainWnd->m_pUserID);	//���̵�
								strcpy_s(PayMureka_data.pOspUPLOADERUserID,strlen((LPTSTR)(LPCTSTR)pToken2->GetToken(10))+1,(LPTSTR)(LPCTSTR)pToken2->GetToken(10));	//���δ� ���̵�
								strcpy_s(PayMureka_data.pOspPayNumber,strlen((LPTSTR)(LPCTSTR)pToken2->GetToken(1))+1,(LPTSTR)(LPCTSTR)pToken2->GetToken(1));//���� ��ȣ




								if(p_sfilter_ret->video_count+p_sfilter_ret->music_count>0)
								{


									//�Խñ� ��ȣ
									pTransMurekaInfo.nBoardIndex=_atoi64(pToken2->GetToken(2));
									//���ϰԽñ� ��ȣ
									pTransMurekaInfo.uFile_code=_atoi64(pToken2->GetToken(3));
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




									if(p_sfilter_ret->video_count>0)
									{
										for(int i=0; i<p_sfilter_ret->video_count; i++)
										{
											if(p_sfilter_ret_video[i].rmi_return_code == 10)
											{




												if(p_sfilter_ret_video[i].rmi_return_code == 10) FilterStatus = _T("02");
												else if(p_sfilter_ret_video[i].rmi_return_code == 11) FilterStatus = _T("00");
												else if(p_sfilter_ret_video[i].rmi_return_code == 12) FilterStatus = _T("01");
												else if(p_sfilter_ret_video[i].rmi_return_code == 13) FilterStatus = _T("04");
												else if(p_sfilter_ret_video[i].rmi_return_code == 14) FilterStatus = _T("04");

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


												sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);// ���͸� Ű
												sprintf(pTransMurekaInfo.uDownpurchase,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(1));//osp���Ź�ȣ
												sprintf(pTransMurekaInfo.uDownloader,"%s",m_pMainWnd->m_pUserID);//�ٿ�δ�
												sprintf(pTransMurekaInfo.uUploader,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(10));//���δ�
												pTransMurekaInfo.uRedownload=0;//�ٽùޱ�


												sprintf(PayMureka_data.pVideoID,"%s",p_sfilter_ret_video[i].content_id);

												sprintf(PayMureka_data.pPayPrice,"%s",p_sfilter_ret_video[i].price);

												sprintf(PayMureka_data.pFiltering_key,"%s",pTransMurekaInfo.ufiltering_key);
												
												sprintf(PayMureka_data.pRightID,"%s",pTransMurekaInfo.uVideo_content_id);

												sprintf(PayMureka_data.pTitle,"%s",pTransMurekaInfo.uTitle);

												pTransMurekaInfo.uTurn=i;

				////////////////////////////�ٿ�ε�� �ؽð� ��ȭ������ ���ؼ� - S
												Packet* pPacket_Mureka = new Packet;
												//������ ����
												pPacket_Mureka->Pack(DOWN_SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
												m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);
				////////////////////////////�ٿ�ε�� �ؽð� ��ȭ������ ���ؼ� - E


												Copyright_Muraka_stop=true;
											}
										}
									}

									if(Copyright_Muraka_stop==false)
									{

										for(int i=0; i<p_sfilter_ret->video_count; i++)
										{
											if(p_sfilter_ret_video[i].rmi_return_code == 10) FilterStatus = _T("02");
											else if(p_sfilter_ret_video[i].rmi_return_code == 11) FilterStatus = _T("00");
											else if(p_sfilter_ret_video[i].rmi_return_code == 12) FilterStatus = _T("01");
											else if(p_sfilter_ret_video[i].rmi_return_code == 13) FilterStatus = _T("04");
											else if(p_sfilter_ret_video[i].rmi_return_code == 14) FilterStatus = _T("04");


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


											sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);// ���͸� Ű
											sprintf(pTransMurekaInfo.uDownpurchase,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(1));//osp���Ź�ȣ
											sprintf(pTransMurekaInfo.uDownloader,"%s",m_pMainWnd->m_pUserID);//�ٿ�δ�
											sprintf(pTransMurekaInfo.uUploader,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(10));//���δ�
											pTransMurekaInfo.uRedownload=0;//�ٽùޱ�





											sprintf(PayMureka_data.pVideoID,"%s",p_sfilter_ret_video[i].content_id);

											sprintf(PayMureka_data.pPayPrice,"%s",p_sfilter_ret_video[i].price);

											sprintf(PayMureka_data.pFiltering_key,"%s",pTransMurekaInfo.ufiltering_key);
											
											sprintf(PayMureka_data.pRightID,"%s",pTransMurekaInfo.uVideo_content_id);

											sprintf(PayMureka_data.pTitle,"%s",pTransMurekaInfo.uTitle);

											pTransMurekaInfo.uTurn=i;
												
											if(FilterStatus=="02")
											{
												Copyright_Muraka_stop=true;
											}
											else/////////////////////////////////ĵ��̵�� ������ ���� 
											{


			/////////////////////////////������ ���굥���� ����(�ٽ� �ޱ��ΰ�� �������� ����) - S
												if(m_pMainWnd->Mureka_use==3&&PayMureka_data.pVideoID&&atoi(PayMureka_data.pPayPrice))
												{
													//���۱��� �ƴϾ��ٰ� �ٲ�°��
													Mureka_paylog_re_First=true;
												}
			/////////////////////////////������ ���굥���� ����(�ٽ� �ޱ��ΰ�� �������� ����) - E


												if(Copyright_Muraka_stop!=true&&strcmp(PayMureka_data.pVideoID,"")!=0&&atoi(PayMureka_data.pPayPrice)&&FilterStatus=="01")
												{

													int rtn_values;

													if(m_pMainWnd->Mureka_use==1||Mureka_paylog_re_First==true)
													{
														rtn_values=lpSFILTERVideoPaidReport(
																					PayMureka_data.pVideoID,
																					PayMureka_data.pPayDate,
																					PayMureka_data.pPayPrice,
																					PayMureka_data.pOspUserID,
																					PayMureka_data.pOspPayNumber,
																					PayMureka_data.pOspUPLOADERUserID,
																					PayMureka_data.pFiltering_key,
																					PayMureka_data.pRightID,
																					atoi(pToken2->GetToken(2)),
																					PayMureka_data.pTitle
														);
													}
													else
													{
														pTransMurekaInfo.uRedownload=1;//�ٽùޱ�

														rtn_values=lpSFILTERVideoPaidReport(
																					PayMureka_data.pVideoID,
																					PayMureka_data.pPayDate,
																					"0",
																					PayMureka_data.pOspUserID,
																					PayMureka_data.pOspPayNumber,
																					PayMureka_data.pOspUPLOADERUserID,
																					PayMureka_data.pFiltering_key,
																					PayMureka_data.pRightID,
																					atoi(pToken2->GetToken(2)),
																					PayMureka_data.pTitle
														);
													}





												}

											}



			////////////////////////////�ٿ�ε�� �ؽð� ��ȭ������ ���ؼ� - S
											Packet* pPacket_Mureka = new Packet;
											//������ ����
											pPacket_Mureka->Pack(DOWN_SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
											m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);
			////////////////////////////�ٿ�ε�� �ؽð� ��ȭ������ ���ؼ� - E






										}
									}

									for(int i=0; i<p_sfilter_ret->music_count; i++)
									{
										if(p_sfilter_ret_music[i].rmi_return_code == 10) FilterStatus = _T("02");
										else if(p_sfilter_ret_music[i].rmi_return_code == 11) FilterStatus = _T("00");
										else if(p_sfilter_ret_music[i].rmi_return_code == 12) FilterStatus = _T("01");
										else if(p_sfilter_ret_music[i].rmi_return_code == 13) FilterStatus = _T("04");
										else if(p_sfilter_ret_music[i].rmi_return_code == 14) FilterStatus = _T("04");


										//���Ϻз�
										pTransMurekaInfo.uFiletype=1;
										pTransMurekaInfo.uMusic_id=_atoi64(p_sfilter_ret_music[i].ch_content_id);
										pTransMurekaInfo.uMoney=atoi(p_sfilter_ret_music[i].price);
										sprintf(pTransMurekaInfo.uStatus,"%s",FilterStatus);
										sprintf(pTransMurekaInfo.uTitle,"%s",p_sfilter_ret_music[i].title);
										sprintf(pTransMurekaInfo.uMusic_company,"%s",p_sfilter_ret_music[i].ch_name);
										sprintf(pTransMurekaInfo.uMusic_content_id,"%s",p_sfilter_ret_music[i].ch_id);
										sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);


										
										//������ ����(120619 - �������Ϸ� ���ؼ� ������)
										Packet* pPacket_Mureka = new Packet;
										pPacket_Mureka->Pack(SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
										m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);


									}


								}
							}
							else
								Copyright_Muraka_stop=true;
							
							// ����ü �޸� ����
							lpSFILTEREnd(p_sfilter_ret, p_sfilter_ret_music, p_sfilter_ret_video);

						}






									
						PRDOWNCTX pNewItem		= new RDOWNCTX;
						ZeroMemory(pNewItem, sizeof(RDOWNCTX));





						//���۱�����ó��
						if(Copyright_Muraka_stop==true)
							pNewItem->nTransState	= ITEM_STATE_COPYRIGHT;
						else
							pNewItem->nTransState	= ITEM_STATE_WAIT;







						pNewItem->nFileType		= atoi(pToken2->GetToken(0));
						pNewItem->nBoardIndex_	= _atoi64(pToken2->GetToken(1));
						pNewItem->nBoardIndex	= _atoi64(pToken2->GetToken(2));
						pNewItem->nFileIndex	= _atoi64(pToken2->GetToken(3));
						pNewItem->nFileSizeReal	= _atoi64(pToken2->GetToken(4));
						pNewItem->nServerVolume	= atoi(pToken2->GetToken(7));		//20080213 �ٿ� ���� ������ �ּ�, jyh
						pNewItem->nFileSizeStart= 0;
						pNewItem->nFileSizeEnd	= 0;
						pNewItem->Grid_use		= true;//�׸�����
//						pNewItem->Grid_use		= false;

						strcpy_s(pNewItem->pServer_real_FileName, strlen((LPSTR)pToken2->GetToken(8))+1, (LPSTR)pToken2->GetToken(8));	

						//20080319 ���� �������� 0�̸� ����, jyh
						if(pNewItem->nServerVolume == 0)
							pNewItem->nFree = 1;	//����
						else
							pNewItem->nFree = 2;	//���

						pFileFolder = pToken2->GetToken(6);
						pFileFolder.Replace('/', '\\');

						strcpy_s(pNewItem->pFileName, strlen((LPSTR)pToken2->GetToken(5))+1, (LPSTR)pToken2->GetToken(5));								

						if(/*m_pMainWnd->m_pListCtrl.GetItemCount() > 0 && */m_pMainWnd->m_rPathLast != "" && 
							(m_pMainWnd->m_rDownAddItemPath == DOWN_ITEM_ADD_CHECK || m_pMainWnd->m_rDownAddItemPath == DOWN_ITEM_ADD_LAST))
						{
							strcpy_s(pNewItem->pSavePath, m_pMainWnd->m_rPathLast.GetLength()+1, (LPSTR)(LPCTSTR)m_pMainWnd->m_rPathLast);				
						}
						else
						{
							m_pMainWnd->m_rPathLast = m_pMainWnd->m_rPathBasic;
							m_pMainWnd->m_pReg.SaveKey(CLIENT_REG_PATH, "rPathLast", m_pMainWnd->m_rPathLast);
							strcpy_s(pNewItem->pSavePath, m_pMainWnd->m_rPathBasic.GetLength()+1, (LPSTR)(LPCTSTR)m_pMainWnd->m_rPathBasic);								
						}
						
						// ��ü ��� ����
						pPath = pNewItem->pSavePath;

						if(pPath.Right(1) != '\\')
							pPath += '\\';

						if(pFileFolder != '\\')
							pPath += pFileFolder;
						
						//20070920 ���۰�� ���, jyh
						char sPath[1024];
						m_pMainWnd->m_pStatic_Path.GetWindowText(sPath, 1024);

						if(!lstrcmp(sPath, ""))
						{
							strcpy_s(sPath, pPath.GetLength()+1, pPath.GetBuffer());
							int len = (int)strlen(sPath);
							sPath[len-1] = 0;
							m_pMainWnd->m_pStatic_Path.SetWindowText(sPath);
							pPath.ReleaseBuffer();
						}

						strcpy_s(pNewItem->pFolderName, pPath.GetLength()+1, (LPSTR)(LPCTSTR)pPath);	

						if(pPath.Right(1) != "\\")
							pPath += "\\";

						pPath += pNewItem->pFileName;
						pPath.Replace("/", "\\");
						strcpy_s(pNewItem->pFullPath, pPath.GetLength()+1, (LPSTR)(LPCTSTR)pPath);					
						m_pMainWnd->InsertItem(pNewItem);
					}
				}
			}
			delete pToken1;
			delete pToken2;

		}
		// �ڵ� ���۽��� üũ
		//if(!m_pMainWnd->m_bFileTrans)
		if(m_pMainWnd->m_rDownAutoTrans == 1 && !m_pMainWnd->m_bFileTrans)	//20071030 ȯ�漳������ �ڵ����� ������ ����, jyh
			m_pMainWnd->SetTransFlag(true);

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
	strcpy_s(mureka_files[9],"FilejuriCMC.dll");
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

/*
void Worker::iMBC_Logsend_history_Save(__int64 BoardIndex,__int64 FileIndex,CString logvalue)
{
	static int cnt_nums=0;
	CString strData,Filehash;
//AfxMessageBox("1");
	strData.Format("%sbbs=%I64d&fidx=%I64d&logs=%s", IMBC_LOG_URL_SENDCONTENTS, BoardIndex,FileIndex, logvalue);

	try
	{
		CoInitialize(NULL);	
		cnt_nums++;
		switch(cnt_nums)
		{
			case 1:
			m_pMainWnd->m_pSendiMBCComplete1->Navigate(strData);
			break;
			case 2:
			m_pMainWnd->m_pSendiMBCComplete2->Navigate(strData);
			break;
			case 3:
			m_pMainWnd->m_pSendiMBCComplete3->Navigate(strData);
			break;
			case 4:
			m_pMainWnd->m_pSendiMBCComplete4->Navigate(strData);
			break;
			case 5:
			m_pMainWnd->m_pSendiMBCComplete5->Navigate(strData);
			break;
		}

		if(cnt_nums==5)	cnt_nums=0;

		CoUninitialize();

	}
	catch(...)
	{
	}

}
*/
