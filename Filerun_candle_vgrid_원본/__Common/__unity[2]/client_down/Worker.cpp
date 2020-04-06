#pragma once
#include "StdAfx.h"
#include "Worker.h"
#include "down_window.h"
#include "FtpClient.h"
#include "Dlg_Confirm.h"

//캔들미디어
#include "sfilter_filerun.h"

#pragma warning(disable:4996)

//IMBC DLL호출
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
					m_nReConnectCount = 0;					
					m_bReConnectTrans = m_pMainWnd->m_bFileTrans;
					
					MMServerFree();

					m_pMainWnd->SetTransFlag(false);

					m_pMainWnd->KillTimer(100);

					//20071119 메세지 띄우기 전에 연결 타이머 설정(5분), jyh
					m_pMainWnd->SetTimer(99, 300000, NULL);

					//::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW,
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"세션 서버와의 연결이 종료되어 5분 뒤에 자동 연결됩니다.\r\n지금 바로 연결 하시겠습니까?", 
								(LPARAM)2);

					m_pMainWnd->m_pPtrTrans = NULL;		//20071119 연결이 끊겼을때 재전송을 위해, jyh
					
					//20071119 리스트 박스의 아이템 전송 상태 출력을 위해, jyh
					CString strBuf;
					strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_STOP);
					m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
					m_pMainWnd->m_pWorker_Ctrl->m_bReConnectTrans = true;
					m_pMainWnd->m_pWorker_Ctrl->m_bConnectServer = false;	//20081025 서버 연결, jyh
	
					//예(바로 연결)
					if(m_pMainWnd->m_nBtnState == BTN_STATE_YES)
					{
						m_pMainWnd->KillTimer(99);	//이전에 5분으로 셋팅됐던 타이머 제거
						m_pMainWnd->SetTimer(99, 1000, NULL);
					}
					//아니오
					else
						m_pMainWnd->KillTimer(99);

					//m_pMainWnd->SetTimer(99, 10000, NULL);
				}
				else
				{
					::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"서버와의 연결이 종료되었습니다. \r\n잠시후에 다시 이용해 주십시오.", 0);
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


		// 다운로드 목록 요청
		case LOAD_DOWNLIST:
		{
			Result_Item_Add(pPacket);
			break;
		}

		// 다운로드 전 파일 상태 체크
		case LOAD_DOWNPRE:
		{
			Result_DownLoadPre(pPacket);
			break;
		}
		// 실제 다운로드
		case LOAD_DOWNLOAD:
		{
			Result_DownLoadPro(pPacket);
			break;
		}
		// 다운로드 완료 통보 결과
		case LOAD_DOWNEND:
		{
			Result_DownLoadEnd(pPacket);
			break;
		}

		// 다른컴퓨터 중복접속
		case CHK_DUPLI_CLOSE:
		{
			//20071130 관리자 로그인이면 중복접속 무시, jyh
			if(!lstrcmp(m_pMainWnd->m_pUserID, "admin"))
				break;

			m_pMainWnd->SetTransFlag(false);
			Result_Dupl_Close(pPacket);
			//20071121 컨트롤 종료, jyh
			::SendMessage(m_pMainWnd->m_hWnd, WM_CLOSE, 0, 0);
			break;
		}
		// 저작권 자료 처리
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
				m_pMainWnd->m_bAuth			= true;
				m_pMainWnd->m_pUserID		= pLogin->pUserID;
				m_pMainWnd->m_pUserPW		= pLogin->pUserPW;
				m_pMainWnd->m_nSpeed		= pLogin->nSpeed;	//20081022 스피드 값, jyh

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
					::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"프로그램을 종료합니다", 0);
				break;
			}		
			case LOGIN_RESULT_FAIL_ID:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"로그인에 실패 했습니다!\r\n존재하지 않는 아이디 입니다.", 1);
				break;
			}		
			case LOGIN_RESULT_FAIL_PW:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"로그인에 실패 했습니다!\r\n비밀번호가 틀립니다.", 1);
				break;
			}		
			case LOGIN_RESULT_FAIL_STOP:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"로그인에 실패 했습니다!\r\n회원님은 현재 정지 상태입니다.", 1);
				break;
			}		
			case LOGIN_RESULT_SERVERSTOP:
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"현재 서버 점검중입니다! \r\n자세한 사항은 홈페이지를 참고하세요!", 1);
				break;
			}	
			case LOGIN_RESULT_FAIL:	
			{
				::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"로그인에 실패 했습니다!\r\n같은 문제가 계속되면 관리자에게 문의해 주십시오.", 1);
				break;
			}		
		}	
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_FORCE_EXIT, (WPARAM)"로그인에 실패 했습니다!\r\n같은 문제가 계속되면 관리자에게 문의해 주십시오.", 1);
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다운로드를 시작하기 전에 해당 파일의 정보 및 다운로드 서버 정보 가져오기
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

			//20090218 파일 사이즈가 0일때만 로컬 파일 체크
			if(m_pMainWnd->m_pPtrTrans->nFileSizeEnd == 0)
			{
				// 로컬에 같은 이름의 파일이 존재하는지 체크
				CFileFind pFileFind;
				CString strFName = m_pMainWnd->m_pPtrTrans->pFullPath;

				if(pFileFind.FindFile((LPCTSTR)strFName))
				{	
					pFileFind.FindNextFile();
					nTrsStartSize = (__int64)pFileFind.GetLength();

					//20090217 수정, jyh--------------------------------------------------
					m_pMainWnd->m_pPtrTrans->nFileSizeEnd = nTrsStartSize;	//로컬 파일의 사이즈를 더한다.
					m_pMainWnd->m_nFileSizeEnd += nTrsStartSize;
					//-------------------------------------------------------
					
					if(nTrsStartSize > 0)
					{
						// 완료된 파일인지 체크
						if(m_pMainWnd->m_pPtrTrans->nFileSizeReal <= nTrsStartSize)
						{
							m_pMainWnd->m_pPtrTrans->nTransState	= ITEM_STATE_COMPLETE;
							m_pMainWnd->m_pPtrTrans->nFileSizeEnd = m_pMainWnd->m_pPtrTrans->nFileSizeReal;

							//20090217 수정, jyh--------------------------------------------------
							//현재 파일 전송 프로그레스
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

							//전체파일 전송 프로그레스
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
			
			//20090217 수정, jyh--------------------------------------------------
			//현재 파일 전송 프로그레스
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

			//전체파일 전송 프로그레스
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
		

			// 다운로드를 시작하기 전에 타겟 디스크에 잔여 용량이 전송받을 용량보다 많은지 체크
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
				sprintf_s(pCancelState, 2048, "로컬디스크의 용량이 부족합니다! 확인후 다시 시도해 주십시오.\r\n\r\n[%s] 드라이브의 남은 용량을 확인하세요!", strTargetVolume);
				::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pCancelState, (LPARAM)0);										
				return;
			}	

			// 파일 시스템 사이즈 체크
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
					sprintf_s(pCancelState, 1024, "현재 파일 시스템이 %s 이므로 4GB이상의 파일은 다운로드 할 수 없습니다.\r\n4GB이상의 파일 다운로드는 NTFS만 가능합니다.\r\n\r\n본 파일은 전송취소합니다!", fileSystem);
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pCancelState, (LPARAM)0);
					::SendMessage(m_pMainWnd->m_hWnd, MSG_DOWNLOAD, 0, 0);
					return;
				}					
			}
							
			// 다운로드 가능한 서버 데이터채널 요청		
			LOADDOWN pDownInfo;
			pDownInfo.nFileType			= m_pMainWnd->m_pPtrTrans->nFileType;
			pDownInfo.nServerVolume		= m_pMainWnd->m_pPtrTrans->nServerVolume;
			pDownInfo.nBoardIndex_		= m_pMainWnd->m_pPtrTrans->nBoardIndex_;
			pDownInfo.nBoardIndex		= m_pMainWnd->m_pPtrTrans->nBoardIndex;
			pDownInfo.nFileIdx			= m_pMainWnd->m_pPtrTrans->nFileIndex;
			pDownInfo.nDownSpeed		= m_pMainWnd->m_nSpeed;		//20081022 스피드값 입력, jyh
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
				// 알수 없는 에러
				case DOWN_RESULT_ERROR:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"죄송합니다!\r\n현재 해당파일에 다운로드가 너무 많습니다.\r\n잠시후 다시 이용해주세요.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					break;
				}				
				// 파일이 존재 하지 않은 상태
				case DOWN_RESULT_NOTFOUND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"파일이 존재하지 않습니다.\r\n파일이 삭제되었거나 기간이 만료된 파일입니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					break;
				}
				// 업로드가 완료되지 않은 상태
				case DOWN_RESULT_UPLOAD:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"업로드가 완료되지 않은 파일입니다.\r\n완료된 파일이면 관리자에게 문의 바랍니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					break;
				}				
				// 서비스가 중지된 상태
				case DOWN_RESULT_SERVICE_STOP:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"서비스 점검중으로 설정되었습니다!\r\n점검중에는 전송할 수 없습니다.\r\n홈페이지를 참고하세요.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					break;
				}
			}

			strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
			m_pMainWnd->m_pPtrTrans = NULL;
			m_pMainWnd->m_bFileTrans = false;
			
			//20071219 다운로드 실패시 다음 파일로 건너뜀, jyh
			if(m_pMainWnd->DeferItem() > 0)
				m_pMainWnd->SetTimer(98, 100, NULL);
			else
				m_pMainWnd->SetTransFlag(false);			
		}
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"다운로드 실패", 1);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다운로드를 위해 다운로드 서버 정보 요청
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_DownLoadPro(Packet* pPacket)
{
	CString strBuf;
	int nItemPatten = 0;		//20081025 재시도창에서의 버튼 값(-1: 취소, 1: 건너뛰기, 2: 재시도), jyh
	BOOL bRetryAlret = FALSE;	//20081025 재시도 창에서의 버튼 선택 여부, jyh

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


				// 전송을 시작하지 못했으므로 종료사이즈 전송
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
				// 알수 없는 에러
				case DOWN_RESULT_ERROR:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"죄송합니다!\r\n현재 해당파일에 다운로드가 너무 많습니다.\r\n잠시후 다시 이용해주세요.", (LPARAM)1);					
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}				
				// 파일이 존재 하지 않은 상태
				case DOWN_RESULT_NOTFOUND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"파일이 존재하지 않습니다.\r\n파일이 삭제되었거나 기간이 만료된 파일입니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				// 업로드가 완료되지 않은 상태
				case DOWN_RESULT_UPLOAD:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"업로드가 완료되지 않은 파일입니다.\r\n완료된 파일이면 운영자에게 문의 바랍니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				// 업로드된 사이즈보다 큰사이즈 요구시
				case DOWN_RESULT_SIZEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"업로드된 사이즈 보다 큰 다운로드 시작 지점입니다.\r\n같은 문제가 계속되면 운영자에게 문의 바랍니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				case DOWN_RESULT_ERROR_DELAY:	//20081027 ftp 파일 전송 실패, jyh
				case DOWN_RESULT_FTP_CONFAILED:	//20081027 파일서버 접속 실패, jyh
				// 파일이 존재하는 서버의 운영 중지 상태
				case DOWN_RESULT_SERVERSTOP:
				// 파일서버의 부하로 다운로드 불가(대기상태)
				case DOWN_RESULT_SERVERBUSY:
				{
					m_pMainWnd->m_pPtrTrans->nCountDefer++;
					if(m_pMainWnd->m_pPtrTrans->nCountDefer >= 3)
					{
						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"해당파일의 서버부하로 전송이 불가능합니다\r\n다음 파일을 전송합니다.", (LPARAM)1);
						
						//20081027 수정, jyh
						if(pDownPro->nResult == DOWN_RESULT_ERROR_DELAY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DELAY;	//전송 실패
						else if(pDownPro->nResult == DOWN_RESULT_SERVERBUSY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DEFER;	//사용자 초과
						else
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CONFAILED;//서버접속실패
					}
					else 
					{
						char szText[MAX_PATH];
						sprintf_s(szText, MAX_PATH, "해당 파일에 사용자가 많아 다운로드가 지연되고 있습니다.\r\n"
							"30초 동안 입력이 없을 경우 해당 파일을 재시도 합니다.\r\n"
							"남은 재시도 횟수: %d", 3 - m_pMainWnd->m_pPtrTrans->nCountDefer);

						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)szText, (LPARAM)3);
						
						bRetryAlret = TRUE; //20081025 재시도 창, jyh
						
						//20081027 수정, jyh
						if(pDownPro->nResult == DOWN_RESULT_ERROR_DELAY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DELAY;	//전송 실패
						else if(pDownPro->nResult == DOWN_RESULT_SERVERBUSY)
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_DEFER;	//사용자 초과
						else
							m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CONFAILED;//서버접속실패

						//20071119 리스트 박스의 아이템 전송 상태 출력을 위해, jyh
						strBuf = m_pMainWnd->GetFileStatus(ITEM_STATE_DEFER);
						m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
						
						//건너뛰기(컨텐츠 단위->파일단위로 수정)
						if(m_pMainWnd->m_nBtnState == BTN_STATE_JUMP)
							nItemPatten = 1;
						//재시도
						else if(m_pMainWnd->m_nBtnState == BTN_STATE_RETRY)
							nItemPatten = 2;
						//취소
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
						//			(WPARAM)"건너뛸 파일이 없습니다.\r\n", (LPARAM)1);
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
				// 회원결제 정보 체크 에러
				case DOWN_RESULT_INFOFAIL:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, 
								(WPARAM)"다운로드를 위해 권한을 체크하는데 실패 했습니다.\r\n잠시후에 다시 시도하세요.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_STOP;
					break;
				}				
				// 구매자료 기간 경과
				case DOWN_RESULT_TIMEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"해당 컨텐츠는 구매기간이 만료 되었습니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}		
				// 정액제 가능 시간 아님
				case DOWN_RESULT_FIX_TIMEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"정액제 사용시간이 아닙니다.\r\n사용시간을 확인해주세요.", (LPARAM)0);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
                    //m_pMainWnd->SetTimer(98, 500, NULL);
					m_pMainWnd->m_pPtrTrans = NULL;
					m_pMainWnd->SetTransFlag(false);
					return;
				}		
				// 정액제 기간 만료
				case DOWN_RESULT_FIX_DATEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"정액제 사용시간이 초과되었습니다.", (LPARAM)0);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);
					m_pMainWnd->m_pPtrTrans = NULL;
					m_pMainWnd->SetTransFlag(false);
					return;
				}	
				// 개인자료실에서 본인의 자료가 아닐때
				case DOWN_RESULT_OTHER_USER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"블로그 개인자료의 다운로드는 소유자만 다운로드 가능합니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				// 구매후 환불승인된 경우 
				case DOWN_RESULT_PAY_CANCEL:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"본 자료는 환불처리된 자료입니다.", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_CANCEL;
					//m_pMainWnd->SetTimer(98, 500, NULL);	
					break;
				}
				//20080311 무료 다운로드 주말 사용 금지, jyh
				case DOWN_RESULT_FREE_WEEKEND:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"주말(토요일, 일요일)은 무료 다운로드를 할 수 없습니다!", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_NOFREEDOWN;
					break;
				}
				//20080311 무료 다운로드 특정 시간대 사용 금지, jyh
				case DOWN_RESULT_FREE_TIMEOVER:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"평일 새벽 3시 부터 오후 5시 까지는 무료 다운로드를 할 수 없습니다!", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_NOFREEDOWN;
					break;
				}
				//20080311 무료 다운로드 인원 제한으로 사용 불가, jyh
				case DOWN_RESULT_FREE_LIMIT_COUNT:
				{
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"해당 파일 서버에 사용자가 많아 무료 다운로드를 할 수 없습니다!", (LPARAM)1);
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_NOFREEDOWN;
					break;
                }
			}

			strBuf = m_pMainWnd->GetFileStatus(m_pMainWnd->m_pPtrTrans->nTransState);
			m_pMainWnd->m_pListCtrl.SetItemText(m_pMainWnd->m_nCurTransFile, 3, strBuf);
			//m_pMainWnd->m_pPtrTrans = NULL;
			m_pMainWnd->m_bFileTrans = false;

			//20081025 재시도 알림창에 의한 선택인지, jyh
			if(bRetryAlret)
			{				
				if(nItemPatten == -1)		//취소
					m_pMainWnd->SetTransFlag(false);
				else if(nItemPatten == 1)	//건너뛰기
				{
					//m_pMainWnd->SetTimer(98, 30000, NULL);
					if(m_pMainWnd->DeferItem() <= 0)
					{
						::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW,
							(WPARAM)"건너뛸 파일이 없습니다.\r\n", (LPARAM)1);
						m_pMainWnd->SetTransFlag(false);
					}
					else
						m_pMainWnd->SetTimer(98, 100, NULL);
				}
				else	//재시도
				{
					m_pMainWnd->m_pPtrTrans->nTransState = ITEM_STATE_WAIT;
					m_pMainWnd->SetTimer(98, 100, NULL);
				}
			}
			else
			{
				//20071219 다운로드 실패시 다음 파일로 건너뜀, jyh
				if(m_pMainWnd->DeferItem() > 0)
					m_pMainWnd->SetTimer(98, 100, NULL);
				else
					m_pMainWnd->SetTransFlag(false);
			}			
		}
	}
	catch(...)
	{
		::PostMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)"다운로드 실패", 1);
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다운로드 완료 통보 결과
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
// 전송받은 아이템 목록 추가 :: bbs 전용
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Item_Add(Packet* pPacket)
{
	CString pPath;
	CString pFileName;
	CString pFileFolder;
	CString pPartnerContentID;
	CONITEM conitem;


	bool Copyright_check_DLL=false,Copyright_Muraka_stop,Mureka_paylog_re_First;


/////////////////캔들미디어 모듈 로딩 - S

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
					// DLL 로드 실패
					char pResult[2048];
					::sprintf_s(pResult, 2048, "저작권 확인 프로그램이 정상적으로 실행되지 않아 프로그램이 종료됩니다.\r\n파일런 클라이언트를 다시 설치 해주세요.");
					::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)1);

					m_pMainWnd->SetTimer(44, 100, NULL);
					return;
				}
			}
			else
			{
				// DLL 로드 실패
				char pResult[2048];
				::sprintf_s(pResult, 2048, "저작권 확인 프로그램이 정상적으로 실행되지 않아 프로그램이 종료됩니다.\r\n파일런 클라이언트를 다시 설치 해주세요.");
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
				//20080311 컨텐츠 단위의 다운로드 완료를 나타내기 위해, jyh-----------
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

				//20080314 컨텐츠가 퀵다운로드 전환일땐 저장경로를 띄우지 않는다, jyh
				if(!bExist)
				{
					m_pMainWnd->m_v_sConItem.push_back(conitem);
					//---------------------------------------------------------------------

					//20071015 아이템 갯수가 7개 이상이면 리스트 뷰의 크기 조절, jyh
					if(m_pMainWnd->m_pListCtrl.GetItemCount() + pToken1->GetCount() > 7)
						m_pMainWnd->m_pListCtrl.SetColumnWidth(1, 382);

					// 프로그램을 최상위 윈도우로 등록
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

					// 아이템이 추가될 때 저장경로 체크
					if( /*m_pMainWnd->m_pListCtrl.GetItemCount() > 0 && */
						(m_pMainWnd->m_rDownAddItemPath == DOWN_ITEM_ADD_CHECK || 
						(m_pMainWnd->m_rDownAddItemPath == DOWN_ITEM_ADD_LAST && 
						m_pMainWnd->m_rPathLast == "")))
					{			
						DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
						CSelectFolderDialog pSelectDialog(FALSE, m_pMainWnd->m_rPathLast, wFlag, NULL, m_pMainWnd, "저장될 폴더를 선택해 주세요");
										
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
							//20071011 저장 경로 출력, jyh
							CString strTemp;
							m_pMainWnd->m_pStatic_Path.GetWindowText(strTemp);

							if(strTemp == "")
							{
								char sPath[4096];

								strcpy_s(sPath, strPath.GetLength()+1, strPath.GetBuffer());
								strPath.ReleaseBuffer();

								int nIdx = (int)strlen(sPath);
								sPath[nIdx-1] = 0;

								m_pMainWnd->m_pStatic_Path.SetWindowText(sPath);	//20070919 저장 경로, jyh

								RECT rt = { 90, 135, 451, 147 };
								InvalidateRect((HWND)*m_pMainWnd, &rt, FALSE);
							}
						}
					}
				}


				//20080311 퀵다운으로 전환하는 파일을 모두 찾아서 서버볼륨을 넣는다.(컨텐츠 단위), jyh
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
						
							//20080319 서버 볼륨값이 0이면 무료, jyh
							if(pItem->nServerVolume == 0)
								pItem->nFree = 1;	//무료
							else
								pItem->nFree = 2;	//고속

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
					m_pMainWnd->m_pPtrTrans = NULL;		//20080324 고속 전환시 꼭 필요함!!, jyh
				}
				//----------------------------------------------------------------------------------------
				else
				{
					// 전송받은 목록을 큐에 등록
					for(size_t index_1 = 0; index_1 < pToken1->GetCount() ; index_1++)
					{
						pToken2->Split(pToken1->GetToken(index_1));

						//20080213 전송 받을 서버 주소 포함, jyh
						//if(pToken2->GetCount() != 7)
						//if(pToken2->GetCount() != 9)

						if(pToken2->GetCount() != 19)
							continue;






						//컨텐츠제휴코드값
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

								///////현재 시간정보 - S
								timer = time(NULL);
								localtime_s(&timeinfo,&timer);
								CString Now_time_String;
								Now_time_String.Format("%04d%02d%02d%02d%02d%02d",(timeinfo.tm_year+1900),timeinfo.tm_mon+1,timeinfo.tm_mday,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
								///////현재 시간정보 - E



								strcpy_s(PayMureka_data.pPayDate,Now_time_String.GetLength()+1,Now_time_String);	//날짜
								strcpy_s(PayMureka_data.pOspUserID,m_pMainWnd->m_pUserID.GetLength()+1,m_pMainWnd->m_pUserID);	//아이디
								strcpy_s(PayMureka_data.pOspUPLOADERUserID,strlen((LPTSTR)(LPCTSTR)pToken2->GetToken(10))+1,(LPTSTR)(LPCTSTR)pToken2->GetToken(10));	//업로더 아이디
								strcpy_s(PayMureka_data.pOspPayNumber,strlen((LPTSTR)(LPCTSTR)pToken2->GetToken(1))+1,(LPTSTR)(LPCTSTR)pToken2->GetToken(1));//구매 번호




								if(p_sfilter_ret->video_count+p_sfilter_ret->music_count>0)
								{


									//게시글 번호
									pTransMurekaInfo.nBoardIndex=_atoi64(pToken2->GetToken(2));
									//파일게시글 번호
									pTransMurekaInfo.uFile_code=_atoi64(pToken2->GetToken(3));
									//해쉬값
									sprintf(pTransMurekaInfo.uHash,"%s",p_sfilter_ret->sfilter_hash);
									//해쉬값(파일자체해시)
									sprintf(pTransMurekaInfo.uHash_master,"%s",p_sfilter_ret->sfilter_hash);

									//파일개수
									pTransMurekaInfo.uCounts=p_sfilter_ret->video_count+p_sfilter_ret->music_count;



									//초기화 - S
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
									//초기화 - E




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

												//파일분류
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


												sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);// 필터링 키
												sprintf(pTransMurekaInfo.uDownpurchase,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(1));//osp구매번호
												sprintf(pTransMurekaInfo.uDownloader,"%s",m_pMainWnd->m_pUserID);//다운로더
												sprintf(pTransMurekaInfo.uUploader,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(10));//업로더
												pTransMurekaInfo.uRedownload=0;//다시받기


												sprintf(PayMureka_data.pVideoID,"%s",p_sfilter_ret_video[i].content_id);

												sprintf(PayMureka_data.pPayPrice,"%s",p_sfilter_ret_video[i].price);

												sprintf(PayMureka_data.pFiltering_key,"%s",pTransMurekaInfo.ufiltering_key);
												
												sprintf(PayMureka_data.pRightID,"%s",pTransMurekaInfo.uVideo_content_id);

												sprintf(PayMureka_data.pTitle,"%s",pTransMurekaInfo.uTitle);

												pTransMurekaInfo.uTurn=i;

				////////////////////////////다운로드시 해시값 변화전달을 위해서 - S
												Packet* pPacket_Mureka = new Packet;
												//데이터 저장
												pPacket_Mureka->Pack(DOWN_SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
												m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);
				////////////////////////////다운로드시 해시값 변화전달을 위해서 - E


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


											//파일분류
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


											sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);// 필터링 키
											sprintf(pTransMurekaInfo.uDownpurchase,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(1));//osp구매번호
											sprintf(pTransMurekaInfo.uDownloader,"%s",m_pMainWnd->m_pUserID);//다운로더
											sprintf(pTransMurekaInfo.uUploader,"%s",(LPTSTR)(LPCTSTR)pToken2->GetToken(10));//업로더
											pTransMurekaInfo.uRedownload=0;//다시받기





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
											else/////////////////////////////////캔들미디어 데이터 전송 
											{


			/////////////////////////////강제로 정산데이터 전송(다시 받기인경우 전송하지 않음) - S
												if(m_pMainWnd->Mureka_use==3&&PayMureka_data.pVideoID&&atoi(PayMureka_data.pPayPrice))
												{
													//저작권이 아니엇다가 바뀌는경우
													Mureka_paylog_re_First=true;
												}
			/////////////////////////////강제로 정산데이터 전송(다시 받기인경우 전송하지 않음) - E


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
														pTransMurekaInfo.uRedownload=1;//다시받기

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



			////////////////////////////다운로드시 해시값 변화전달을 위해서 - S
											Packet* pPacket_Mureka = new Packet;
											//데이터 저장
											pPacket_Mureka->Pack(DOWN_SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
											m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);
			////////////////////////////다운로드시 해시값 변화전달을 위해서 - E






										}
									}

									for(int i=0; i<p_sfilter_ret->music_count; i++)
									{
										if(p_sfilter_ret_music[i].rmi_return_code == 10) FilterStatus = _T("02");
										else if(p_sfilter_ret_music[i].rmi_return_code == 11) FilterStatus = _T("00");
										else if(p_sfilter_ret_music[i].rmi_return_code == 12) FilterStatus = _T("01");
										else if(p_sfilter_ret_music[i].rmi_return_code == 13) FilterStatus = _T("04");
										else if(p_sfilter_ret_music[i].rmi_return_code == 14) FilterStatus = _T("04");


										//파일분류
										pTransMurekaInfo.uFiletype=1;
										pTransMurekaInfo.uMusic_id=_atoi64(p_sfilter_ret_music[i].ch_content_id);
										pTransMurekaInfo.uMoney=atoi(p_sfilter_ret_music[i].price);
										sprintf(pTransMurekaInfo.uStatus,"%s",FilterStatus);
										sprintf(pTransMurekaInfo.uTitle,"%s",p_sfilter_ret_music[i].title);
										sprintf(pTransMurekaInfo.uMusic_company,"%s",p_sfilter_ret_music[i].ch_name);
										sprintf(pTransMurekaInfo.uMusic_content_id,"%s",p_sfilter_ret_music[i].ch_id);
										sprintf(pTransMurekaInfo.ufiltering_key,"%s",p_sfilter_ret_video[i].filtering_key);


										
										//데이터 저장(120619 - 압축파일로 인해서 수정함)
										Packet* pPacket_Mureka = new Packet;
										pPacket_Mureka->Pack(SAVE_MUREKA_DATA, (char*)&pTransMurekaInfo, sizeof(UPLOADMUREKA));
										m_pMainWnd->m_pWorker_Ctrl->SendPacket(pPacket_Mureka);


									}


								}
							}
							else
								Copyright_Muraka_stop=true;
							
							// 구조체 메모리 해제
							lpSFILTEREnd(p_sfilter_ret, p_sfilter_ret_music, p_sfilter_ret_video);

						}






									
						PRDOWNCTX pNewItem		= new RDOWNCTX;
						ZeroMemory(pNewItem, sizeof(RDOWNCTX));





						//저작권제한처리
						if(Copyright_Muraka_stop==true)
							pNewItem->nTransState	= ITEM_STATE_COPYRIGHT;
						else
							pNewItem->nTransState	= ITEM_STATE_WAIT;







						pNewItem->nFileType		= atoi(pToken2->GetToken(0));
						pNewItem->nBoardIndex_	= _atoi64(pToken2->GetToken(1));
						pNewItem->nBoardIndex	= _atoi64(pToken2->GetToken(2));
						pNewItem->nFileIndex	= _atoi64(pToken2->GetToken(3));
						pNewItem->nFileSizeReal	= _atoi64(pToken2->GetToken(4));
						pNewItem->nServerVolume	= atoi(pToken2->GetToken(7));		//20080213 다운 받을 서버의 주소, jyh
						pNewItem->nFileSizeStart= 0;
						pNewItem->nFileSizeEnd	= 0;
						pNewItem->Grid_use		= true;//그리드사용
//						pNewItem->Grid_use		= false;

						strcpy_s(pNewItem->pServer_real_FileName, strlen((LPSTR)pToken2->GetToken(8))+1, (LPSTR)pToken2->GetToken(8));	

						//20080319 서버 볼륨값이 0이면 무료, jyh
						if(pNewItem->nServerVolume == 0)
							pNewItem->nFree = 1;	//무료
						else
							pNewItem->nFree = 2;	//고속

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
						
						// 전체 경로 지정
						pPath = pNewItem->pSavePath;

						if(pPath.Right(1) != '\\')
							pPath += '\\';

						if(pFileFolder != '\\')
							pPath += pFileFolder;
						
						//20070920 전송경로 출력, jyh
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
		// 자동 전송시작 체크
		//if(!m_pMainWnd->m_bFileTrans)
		if(m_pMainWnd->m_rDownAutoTrans == 1 && !m_pMainWnd->m_bFileTrans)	//20071030 환경설정에서 자동전송 적용을 위해, jyh
			m_pMainWnd->SetTransFlag(true);

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 다른 컴퓨터에서 정액제 다운로드 시작
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Worker::Result_Dupl_Close(Packet* pPacket)
{
	char pResult[2048];

	::sprintf_s(pResult, 2048, "다른 장소에서 로그인되어 접속이 해제됩니다.\r\n\r\n[%s]", (char*)pPacket->GetData());
	::SendMessage(m_pMainWnd->m_hWnd, MSG_ALERTWINDOW, (WPARAM)pResult, (LPARAM)1);
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



//캔들미디어 파일체크
bool Worker::Mureka_files_check()
{
	return true;

	//설치 폴더 체크 - S
	CString strFileName;
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));
	::SHGetSpecialFolderPath(NULL,pPath,CSIDL_PROGRAM_FILES,false);
	//설치 폴더 체크 - E



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


	//파일체크
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
