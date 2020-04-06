#include "stdafx.h"
#include "down_window.h"
#include "FtpClient.h"
#include "Dlg_Slide.h"
#include <sys/stat.h>

#pragma warning(disable:4244)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 파일 전송 상태 설정
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool down_window::ReSetTransInfo()
{
	CString strBuf, strTime, strTimeOld, strSpeed;
	PRDOWNCTX pItemData = NULL;
	
	__int64 nFileSizeEnd = 0;
	__int64 nFileCountEnd = 0;
	int nTimeSec, nTimeMin, nTimeHour;
	int nTransIndex;


	if(m_bForceExit)
		return false;

	//if(m_nFileSizeEnd == 0)
	{
		// 전체 파일 체크
		for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
		{
			if((pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
			{
				if(	pItemData->nTransState == ITEM_STATE_WAIT ||
					pItemData->nTransState == ITEM_STATE_TRANS ||
					pItemData->nTransState == ITEM_STATE_STOP ||
					pItemData->nTransState == ITEM_STATE_COMPLETE)
				{
					//20090217 수정, jyh
					//nFileSizeEnd	+= pItemData->nFileSizeEnd;
					//m_nFileSizeEnd = nFileSizeEnd;
					nFileSizeEnd	+= pItemData->nFileSizeEnd;				
				}

				if(	pItemData->nTransState == ITEM_STATE_COMPLETE)
					nFileCountEnd++;
			}
		}
	}
	
	//nFileSizeEnd = m_nFileSizeEnd;	//20090217 수정, jyh

	// 파일정보	
	__int64 nFileCountEnd_ = m_pFtpClint->m_bFlag ? nFileCountEnd+1 : nFileCountEnd;
	//strBuf.Format("%I64d / %I64d (%s / %s)", nFileCountEnd_, m_nFileCount, SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal));
	//m_pStatic_FileCount.SetWindowText(strBuf);


	m_nTime = GetTickCount();

	// 전체 남은 시간 설정
	if(m_nTransSpeed > 0)
	{
		__int64 nTimeOver = (__int64)(((__int64)(m_nFileSizeTotal - nFileSizeEnd)) / m_nTransSpeed);
		nTimeSec	= (int)((nTimeOver) % 60);
		nTimeMin	= (int)(((nTimeOver) / 60) % 60);
		nTimeHour	= (int)(((nTimeOver) / 3600));		

		strBuf.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);
		m_pStatic_Time_Over.SetWindowText(strBuf);

		//20070919 jyh
		RECT rt = { 579, 54, 629, 66 };
		InvalidateRect(&rt);
	}		

	// 경과시간 설정	
	//m_pStatic_Time_Use.GetWindowText(strTimeOld);	

	if(m_nTimeFileStart != 0)
		m_nTimeFileTime = ((m_nTime - m_nTimeFileStart) / 1000);

	if(m_nTimeFileTime != 0)
	{
		nTimeSec	= (m_nTimeTotal + m_nTimeFileTime) % 60;
		nTimeMin	= ((m_nTimeTotal + m_nTimeFileTime) / 60) % 60;
		nTimeHour	= ((m_nTimeTotal + m_nTimeFileTime) / 3600);		
		strTime.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);		
		//m_pStatic_Time_Use.SetWindowText(strTime);
	}
	else
	{
		strTime = strTimeOld;
		//m_pStatic_Time_Use.SetWindowText(strTime);
	}

	// 전송중인 파일 설정
	if((nTransIndex = GetTransListIndex()) != -1)
	{
		try
		{
			pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(nTransIndex);
			
			if(lstrcmp(m_strPrevFileName.GetBuffer(), pItemData->pFileName))
			{
				//20070919 전송 파일, jyh
				int nlen = (int)strlen(pItemData->pFileName);
				char strtemp[256];

				strcpy_s(strtemp, strlen(pItemData->pFileName)+1, pItemData->pFileName);
				m_strPrevFileName = pItemData->pFileName;

				if(nlen > 32)
				{
					strtemp[28] = '.';
					strtemp[29] = '.';
					strtemp[30] = '.';
					strtemp[31] = 0;
				}

				strBuf.Format("%s'다운로드 중'", strtemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
			}

			m_strPrevFileName.ReleaseBuffer();

			//20070919 저장 경로, jyh
			if(lstrcmp(m_strPrevPath.GetBuffer(), pItemData->pFolderName))
			{
				char sPath[4096];

				strcpy_s(sPath, strlen(pItemData->pFolderName)+1, pItemData->pFolderName);
				m_strPrevPath = pItemData->pFolderName;

				int nIdx = (int)strlen(sPath);
				sPath[nIdx-1] = 0;
				m_pStatic_Path.SetWindowText(sPath);	//20070919 저장 경로, jyh
				//m_Edit_DownPath.SetWindowText(sPath);	

				RECT rt = { 90, 135, 451, 147 };
				InvalidateRect(&rt);
			}

			m_strPrevPath.ReleaseBuffer();
			int nPercent2 = abs((int)(pItemData->nFileSizeEnd * 100 / pItemData->nFileSizeReal));
			strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal));		
			m_pListCtrl.SetItemText(nTransIndex, 2, strBuf);
			/*CRect rt;
			m_pListCtrl.GetCellRect(nTransIndex, 2, LVIR_BOUNDS, rt);
			m_pListCtrl.InvalidateRect(&rt);*/

			if( pItemData->Grid_use == true )
			{
				strSpeed = SetSpeedFormatKGrid(m_nGridSpeed,m_nTime / 1000, m_nTimeFileStart / 1000);
			}
			else
			{
				strSpeed = SetSpeedFormat(pItemData->nFileSizeEnd - pItemData->nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);
			}

			if(m_nTimeFileStart != 0)
			{
				//속도 강제 조정(원본)
				//strSpeed = SetSpeedFormat(pItemData->nFileSizeEnd - pItemData->nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);
				//120%로 표시
//strSpeed = SetSpeedFormat((pItemData->nFileSizeEnd - pItemData->nFileSizeStart)*1.2, m_nTime / 1000, m_nTimeFileStart / 1000);
				if( pItemData->Grid_use == true )
				{
					strSpeed = SetSpeedFormatKGrid(m_nGridSpeed,m_nTime / 1000, m_nTimeFileStart / 1000);
				}else{
					strSpeed = SetSpeedFormat(pItemData->nFileSizeEnd - pItemData->nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);
				}
				if( (m_nTimeFileStart / 1000) != 0)
				{

					__int64	time;

					time = (m_nTime / 1000) - (m_nTimeFileStart / 1000);
					if(time < 1) time = 1;

					//size = size * bonus / time;

					// 남은시간 계산을 위한 마지막 속도
					m_nTransSpeed = (pItemData->nFileSizeEnd - pItemData->nFileSizeStart * 1 ) / time;
				}
				else
				{
					m_nTransSpeed = pItemData->nFileSizeEnd - pItemData->nFileSizeStart;
				}


				//20070919 속도 출력, jyh
				m_pStatic_Speed.SetWindowText(strSpeed);
				//20070919 jyh
				RECT rt = { 423, 54, 490, 66 };
				InvalidateRect(&rt);

				// 현재 속도 업데이트
				if((m_nTime - m_nTimeFileSpeed) > 3000  || (m_nTransSpeed_Last < m_nTransSpeed && (m_nTime - m_nTimeFileSpeed) > 1000))
				{
					//m_pListCtrl.SetItemText(nTransIndex, 2, strSpeed);	20070918 주석 처리, jyh

					if(m_nTransSpeed != 0)
					{
						m_nTimeFileSpeed	= m_nTime;
						m_nTransSpeed_Last	= m_nTransSpeed;
					}
				}
				else
				{
					//strSpeed = m_pListCtrl.GetItemText(nTransIndex, 2);	20070918 주석 처리, jyh
				}
			}

			//20070918 상태 출력, jyh
			//strBuf.Format("%d%%", abs((int)(pItemData->nFileSizeEnd  * 100 / pItemData->nFileSizeReal)));			
			if( pItemData->Grid_use == true )
			{
				strBuf = GetFileStatus(pItemData->nTransState);
				m_pListCtrl.SetItemText(nTransIndex, 3, strBuf);
				m_pCtrl_ProgressCur.SetPos(nPercent2);
				strBuf.Format("%s / %s (%d%%)", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
				m_pCtrl_ProgressCur.SetText(strBuf);

			}
			//strBuf = GetFileStatus(pItemData->nTransState);
			//m_pListCtrl.SetItemText(nTransIndex, 3, strBuf);
			
			//20070919 현재 파일 전송 상태 출력, jyh
			//20090216 세밀한 출력을 위해 FtpClient::FileTrans()에서 출력, jyh
			//m_pCtrl_ProgressCur.SetPos(nPercent2);
			//strBuf.Format("%s / %s (%d%%)", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			//m_pCtrl_ProgressCur.SetText(strBuf);
		
			if(m_nTransSpeed > 0)
			{
				__int64 nTimeOver = (__int64)((__int64)(pItemData->nFileSizeReal - pItemData->nFileSizeEnd) / m_nTransSpeed);
				nTimeSec	= (int)((nTimeOver) % 60);
				nTimeMin	= (int)(((nTimeOver) / 60) % 60);
				nTimeHour	= (int)(((nTimeOver) / 3600));		

				strBuf.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);
				//m_pListCtrl.SetItemText(nTransIndex, 4, strBuf);	20070918 주석 처리, jyh
			}	
		}
		catch(...)
		{
		}			
		

		/**********************************************************************************************/
		/*20070919 일시정지시 전체 파일 전송과 현재 파일 전송의 프로그레스바의 값을 같게 하기 위해서는*/
		/*전송중인 파일 설정 if문 안으로 넣어야 함, jyh                                               */
		/**********************************************************************************************/
		// 진행 퍼센트 정보
		if(nFileSizeEnd == 0 || m_nFileSizeTotal == 0)
		{
			m_pCtrl_ProgressTotal.SetPos(0);
		}
		else
		{
			//20090217 수정, jyh
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			if( pItemData->Grid_use == true )
			{
				m_pCtrl_ProgressTotal.SetPos(nPercent);

			}
			m_nPercent = nPercent;	//20080225 전체 파일의 전송 퍼센트, jyh

			if(m_pFtpClint->m_bFlag && !strSpeed.IsEmpty())
			{
				//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
				if( pItemData->Grid_use == true )
				{
					strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
					m_pCtrl_ProgressTotal.SetText(strBuf);
				}
			}
			else
			{
				//strBuf.Format("%d%%", nPercent);
				if( pItemData->Grid_use == true )
				{
					strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent/10000);
					m_pCtrl_ProgressTotal.SetText(strBuf);
					m_pCtrl_ProgressTotal.Invalidate();
				}
			}

			if(m_pFtpClint->m_bFlag)
			{
				strBuf.Format("[%d%%] %s", m_nPercent, CLIENT_DOWN_TITLE);
				SetWindowText(strBuf);
			}
		}
	}
	
	//20071005 완료된 목록을 다시 다운 받을때도 전체 프로그레스바가 진행되게
	if(nFileSizeEnd == 0 || m_nFileSizeTotal == 0)
	{
		m_pCtrl_ProgressTotal.SetPos(0);
	}
	else
	{
		//전체 파일 전송 상태
		//int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
		//m_pCtrl_ProgressTotal.SetPos(nPercent);

		//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
		//strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
		//m_pCtrl_ProgressTotal.SetText(strBuf);
		if( pItemData->Grid_use == true )
		{
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			m_pCtrl_ProgressTotal.SetPos(nPercent);

			//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
			strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
			m_pCtrl_ProgressTotal.SetText(strBuf);
		}

	}

	return true;
}


void down_window::FileTransLoop()
{
	// 전송시작시간 설정
	m_nTimeDownState = m_nTimeFileStart = GetTickCount();

	while(m_pFtpClint->m_bFlag)
	{
		//m_pListCtrl.SetRedraw(FALSE);
		ReSetTransInfo();
		//m_pListCtrl.SetRedraw(TRUE);		

//		Sleep(500);
		Sleep(1000);
	}

	Sleep(500);

	// 완료시간 설정
	m_nTimeTotal		+= m_nTimeFileTime;
	m_nTimeFileTime		= 0;
	m_nTimeFileStart	= 0;
	m_nTimeFileSpeed	= 0;
	m_nTransSpeed_Last	= 0;

	// 최종결과 업데이트
	ReSetTransInfo();

	// 기본 타이틀로 변경
	SetWindowText(CLIENT_DOWN_TITLE);



	// 전송결과 처리
	if(m_pPtrTrans && !m_bForceExit)
	{
		//20070918 주석 처리, jyh
		//if(GetTransListIndex() != -1)
		//	m_pListCtrl.SetItemText(GetTransListIndex(), 4, "00:00:00");	

		if(m_pPtrTrans->nFileSizeReal == m_pPtrTrans->nFileSizeEnd)
			m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;
		else
			m_pPtrTrans->nTransState = ITEM_STATE_STOP;

		//m_pListCtrl.Invalidate();


		// 파일 전송완료 통보
		CHECKDOWNEND pTransEndInfo;
		pTransEndInfo.nFileType		= m_pPtrTrans->nFileType;
		pTransEndInfo.nBoardIndex_	= m_pPtrTrans->nBoardIndex_;
		pTransEndInfo.nBoardIndex	= m_pPtrTrans->nBoardIndex;
		pTransEndInfo.nFileIdx		= m_pPtrTrans->nFileIndex;
		pTransEndInfo.nSizeBegin	= m_pPtrTrans->nFileSizeStart;
		pTransEndInfo.nSizeEnd		= m_pPtrTrans->nFileSizeEnd;

		//20080311 컨텐츠 단위의 다운로드를 위해, jyh
		std::vector<CONITEM>::iterator iter;

		for(iter = m_v_sConItem.begin(); iter != m_v_sConItem.end(); iter++)
		{
			if(iter->nBoardIndex == m_pPtrTrans->nBoardIndex)
			{
				iter->nItemCnt--;

				if(iter->nItemCnt == 0)
				{
					m_v_sConItem.erase(iter);
					break;
				}
			}
		}

		//Packet* pPacket = new Packet;
		//pPacket->Pack(LOAD_DOWNEND, (char*)&pTransEndInfo, sizeof(CHECKDOWNEND));
		//m_pWorker_Ctrl->SendPacket(pPacket);


		// 전송중인 파일이 없음으로 설정
		m_pPtrTrans	= NULL;		
	}

	// 중지 버튼을 클릭한 것이 아니면 다음 리스트가 있는지 체크
	if(m_bFileTrans)
		DownLoadPro();
}

unsigned int __stdcall ProcFileTransLoop(void *pParam)
{
	down_window* pProcess = (down_window*)pParam;
	pProcess->FileTransLoop();
	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 전송중인지 설정 : 버튼활성화 등등의 작업
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void down_window::SetTransFlag(bool bFlag)
{
	// 먼저 관련 버튼을 상황에 맞게 설정
	Skin_SetButtonVisible("BTN_STOP",	bFlag);
	Skin_SetButtonVisible("BTN_START",	!bFlag);
	Skin_SetButtonEnable("BTN_STOP",	bFlag);
	Skin_SetButtonEnable("BTN_START",	!bFlag);	

	Skin_SetButtonEnable("BTN_SELALL",	!bFlag);
	//Skin_SetButtonEnable("BTN_SELDEL",	!bFlag);	//20080228 전송중에도 선택목록 삭제 되게, jyh
	//Skin_SetButtonEnable("BTN_DONEDEL",	!bFlag);	//20071002 전송중에도 완료목록 삭제 되게, jyh
	Skin_SetButtonEnable("BTN_TOP",		!bFlag);
	//Skin_SetButtonEnable("BTN_UP",		!bFlag);	//20080228 전송중에도 선택 이동 되게, jyh
	//Skin_SetButtonEnable("BTN_DOWN",	!bFlag);		//20080228 전송중에도 선택 이동 되게, jyh
	//Skin_SetButtonEnable("BTN_BOTTOM",	!bFlag);	//20080228 전송중에도 선택 이동 되게, jyh
	Skin_SetButtonEnable("BTN_PATH",	!bFlag);
	////20080313 처음 시작시 실명 인증이 된 회원만 활성화 하고 , jyh
	//if(m_nNameAuth == 0)
	//	Skin_SetButtonEnable("BTN_QUICKDOWN", false);		

	// 전송중으로 설정시
	if(bFlag) 
	{
		m_pPtrTrans = NULL;		//20081028 추가, jyh
		m_bFileTrans = true;
		DownLoadPro();
	}
	// 전송종료로 설정시
	else
	{
		m_bFileTrans = false;	

		if(m_pFtpClint)
			m_pFtpClint->m_bFlag = false;

		////20070918 파일 상태 중지로 변경, jyh
		//if(m_pPtrTrans)
		//{
		//	m_pPtrTrans->nTransState = ITEM_STATE_STOP;
		//	m_pListCtrl.SetItemText(m_nCurTransFile, 3, GetFileStatus(ITEM_STATE_STOP));	
		//}

		SetWindowText(CLIENT_DOWN_TITLE);
	}
}

////20080320 다운로드 상태 이미지 출력, jyh
//void down_window::DrawDownState()
//{
//	if(m_nCurTransFile == -1)
//		return;
//
//	CClientDC dc(this);
//	CBitmap DownStateBmp;
//	DownStateBmp.LoadBitmap(IDB_BITMAP_STATE);
//	BITMAP bmpinfo;
//	DownStateBmp.GetBitmap(&bmpinfo);
//
//	CDC dcmem;
//	dcmem.CreateCompatibleDC(&dc);
//	dcmem.SelectObject(&DownStateBmp);
//
//	if(m_bFreeDown)
//		dc.StretchBlt(22, 117, bmpinfo.bmWidth/2, bmpinfo.bmHeight, &dcmem,
//						0, 0, bmpinfo.bmWidth/2, bmpinfo.bmHeight, SRCCOPY);
//	else
//		dc.StretchBlt(22, 117, bmpinfo.bmWidth/2, bmpinfo.bmHeight, &dcmem,
//						26, 0, bmpinfo.bmWidth/2, bmpinfo.bmHeight, SRCCOPY);
//}

void down_window::DownLoadPro()
{
	ACE_UINT64	nItemFileSize = 0;
	int			nItemCheck = 0;
	PRDOWNCTX	pPtrCheck;
	
	if(!m_bForceExit && GetTransListIndex() == -1 && m_pPtrTrans == NULL)
	{
		for(int nProcessIndex = 0 ; nProcessIndex < m_pListCtrl.GetItemCount(); nProcessIndex++)
		{	
			pPtrCheck = (PRDOWNCTX)m_pListCtrl.GetItemData(nProcessIndex);
			
			if(pPtrCheck == NULL)
				continue;

			if(pPtrCheck->nTransState == ITEM_STATE_COMPLETE)
			{
				//20071030 전송 상태와 파일 크기 출력, jyh
				CString strTemp, strBuf;
				strTemp.Format("%s", SetSizeFormat(pPtrCheck->nFileSizeReal));
				m_pListCtrl.SetItemText(nProcessIndex, 2, strTemp);
				strTemp = GetFileStatus(pPtrCheck->nTransState);
				m_pListCtrl.SetItemText(nProcessIndex, 3, strTemp);

				//20080313 완료시에도 파일명 출력, jyh
				int nlen = (int)strlen(pPtrCheck->pFileName);
				char sztemp[256];

				strcpy_s(sztemp, strlen(pPtrCheck->pFileName)+1, pPtrCheck->pFileName);
				m_strPrevFileName = pPtrCheck->pFileName;

				if(nlen > 32)
				{
					sztemp[28] = '.';
					sztemp[29] = '.';
					sztemp[30] = '.';
					sztemp[31] = 0;
				}

				strBuf.Format("%s'완료'", sztemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
				
				continue;
			}

			if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
				pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				pPtrCheck->nTransState == ITEM_STATE_STOP )
			{
				//20071119 현재 전송될 파일의 인덱스 값을 멤버변수로 저장, jyh
				m_nCurTransFile = nProcessIndex;
				// 이전 선택된 모든 아이템 선택해제
				
				while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
					m_pListCtrl.SetItemState(nItemCheck, 0, -1);
				
				// 전송될 아이템 포커서와 선택된 상태로 변경
				m_pListCtrl.SetItemState(nProcessIndex, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
				m_pListCtrl.EnsureVisible(nProcessIndex, TRUE);						

				// 전송중인 아이템 설정 : 전송중으로 설정
				m_pPtrTrans = pPtrCheck;
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;
				CString strTemp;
				strTemp = GetFileStatus(pPtrCheck->nTransState);
				m_pListCtrl.SetItemText(nProcessIndex, 3, strTemp);

				////20080313 전송 받을 파일의 서버볼륨이 0 이면 퀵다운 버튼 활성화, jyh
				//if(m_nNameAuth == 1)
				//{
				//	if(((PRDOWNCTX)m_pListCtrl.GetItemData(m_nCurTransFile))->nServerVolume == 0)
				//	{
				//		m_bFreeDown = TRUE;
				//		Skin_SetButtonEnable("BTN_QUICKDOWN", true);
				//	}
				//	else
				//	{
				//		m_bFreeDown = FALSE;
				//		Skin_SetButtonEnable("BTN_QUICKDOWN", false);
				//	}
				//}

				////20080322 gif 애니매이션, jyh
				//if(((PRDOWNCTX)m_pListCtrl.GetItemData(m_nCurTransFile))->nServerVolume == 0)
				//{
				//	if(m_Picture.Load(MAKEINTRESOURCE(IDR_SLOW),_T("GIF")))
				//		m_Picture.Draw();
				//}
				//else
				//{
				//	if(m_Picture.Load(MAKEINTRESOURCE(IDR_QUICK),_T("GIF")))
				//		m_Picture.Draw();
				//}
				
				//20090217 수정, jyh--------------------------------------------------
				//현재 파일 전송 프로그레스
				CString strBuf, strCurSize, strSize;
				int nPercent = abs((int)((pPtrCheck->nFileSizeEnd * 1000000) / pPtrCheck->nFileSizeReal));		
				m_pCtrl_ProgressCur.SetPos(nPercent);
				
				if((pPtrCheck->nFileSizeEnd/1024)/1000000)
					strCurSize.Format("%I64d,%03I64d,%03I64dKB", ((pPtrCheck->nFileSizeEnd/1024)/1000000)%1000,
																((pPtrCheck->nFileSizeEnd/1024)/1000)%1000,
																(pPtrCheck->nFileSizeEnd/1024)%1000);
				else
				{
					if((pPtrCheck->nFileSizeEnd/1024)/1000)
						strCurSize.Format("%I64d,%03I64dKB", (pPtrCheck->nFileSizeEnd/1024)/1000,
															(pPtrCheck->nFileSizeEnd/1024)%1000);
					else
					{
						if(pPtrCheck->nFileSizeEnd/1024)
							strCurSize.Format("%I64dKB", pPtrCheck->nFileSizeEnd/1024);
						else
							strCurSize.Format("%I64dB", pPtrCheck->nFileSizeEnd);
					}
				}

				if((pPtrCheck->nFileSizeReal/1024)/1000000)
					strSize.Format("%I64d,%03I64d,%03I64dKB", ((pPtrCheck->nFileSizeReal/1024)/1000000)%1000,
															((pPtrCheck->nFileSizeReal/1024)/1000)%1000,
															(pPtrCheck->nFileSizeReal/1024)%1000);
				else
				{
					if((pPtrCheck->nFileSizeReal/1024)/1000)
						strSize.Format("%I64d,%03I64dKB", (pPtrCheck->nFileSizeReal/1024)/1000,
						(pPtrCheck->nFileSizeReal/1024)%1000);
					else
					{
						if(pPtrCheck->nFileSizeReal/1024)
							strSize.Format("%I64dKB", pPtrCheck->nFileSizeReal/1024);
						else
							strSize.Format("%I64dB", pPtrCheck->nFileSizeReal);
					}
				}

				strBuf.Format("%s / %s (%d%%)", strCurSize, strSize, 0);
				//strBuf.Format("%s(%d%%)", strCurSize, nCurPercent/10000);
				m_pCtrl_ProgressCur.SetText(strBuf);
				//---------------------------------------------------------------------
								
				// 다운로드를 시작하기 전에 현재 다운로드 받을 파일의 상태 체크
				LOADPREDOWN pTransInfo;
				pTransInfo.nFileType		= m_pPtrTrans->nFileType;
				pTransInfo.nBoardIndex_		= m_pPtrTrans->nBoardIndex_;
				pTransInfo.nBoardIndex		= m_pPtrTrans->nBoardIndex;
				pTransInfo.nFileIdx			= m_pPtrTrans->nFileIndex;
				pTransInfo.nFileRealSize	= 0;
				pTransInfo.nResult			= 0;
					
				Packet* pPacket = new Packet;
				pPacket->Pack(LOAD_DOWNPRE, (char*)&pTransInfo, sizeof(LOADPREDOWN));
				m_pWorker_Ctrl->SendPacket(pPacket);		
				return;
			}
		}
	}



	// 전송중지로 설정
	SetTransFlag(false);

	// 전송 파일 갯수 변경
	ReSetTransInfo();

	// 기본 타이틀로 변경
	SetWindowText(CLIENT_DOWN_TITLE);

	// 전송완료된 목록이 하나이상 있을때만 처리(리스트가 없는 상태에서 전송 버튼을 클릭하는 경우를 제외하기 위함)
	if(m_pListCtrl.GetItemCount() <= 0)
		return;

	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if((pPtrCheck = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
		{
			if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
				pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				pPtrCheck->nTransState == ITEM_STATE_STOP)
			{
				return;
			}
		}
	}


	char strResult[2048];
	int nCountComplete	= 0;
	int nCountFailed	= 0;
	int nCountFailed_Copyright	= 0;

	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if((pPtrCheck = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
		{
			if(pPtrCheck->nTransState == ITEM_STATE_COMPLETE)
				nCountComplete++;
			else
			{
				if(((PRDOWNCTX)m_pListCtrl.GetItemData(nIndex))->nTransState == ITEM_STATE_COPYRIGHT)
					nCountFailed_Copyright++;

				nCountFailed++;
			}
		}
	}


	//20090217 남은 시간을 0으로, jyh
	m_pStatic_Time_Over.SetWindowText("00:00:00");
	RECT rt = { 579, 54, 629, 66 };
	InvalidateRect(&rt);
	
	//20080131 최소창과 트레이 아이콘으로 실행중일때 전송완료되면 슬라이드 윈도를 띄운다, jyh
	if(m_pTray->IsActiveTray() || m_bMinWindow)
	{
		//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
		if(nCountFailed)
			sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어\r\n파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
		else
			sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d", nCountComplete);

		//sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d", nCountComplete, nCountFailed);
		Dlg_Slide dlg;
		dlg.m_pInfo = strResult;
		dlg.DoModal();

		if(Skin_GetCheck("CHK_SYSTEM") == TRUE)
		{
			SetTimer(44, 100, NULL);
			SystemShutdownOTN();
		}
        else if(Skin_GetCheck("CHK_PROGRAM") == TRUE)
			SetTimer(44, 100, NULL);
	}
	else
	{
		// 다운로드가 완료 되었고 더이상 다운로드 받을 것이 없을 때 체크
		if(Skin_GetCheck("CHK_SYSTEM") == TRUE)
		{	
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
				{
					sprintf_s(strResult, 2048, "전송에 실패 또는 저작권제한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n"
						"해당 파일은 마이페이지->내가받은 자료에서 2일간 무료다운로드 할 수 있습니다.(저작권제한 파일제외)\r\n"
						"잠시후 ‘내가받은자료’에서 다시 이용해주시기 바랍니다.\r\n전송실패 파일: %d / 저작권 다운로드제한 파일: %d(다운로드불가)", nCountFailed,nCountFailed_Copyright);
				}
				else
				{
					sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n"
						"해당 파일은 마이페이지->내가받은 자료에서 2일간 무료다운로드 할 수 있습니다.\r\n"
						"잠시후 ‘내가받은자료’에서 다시 이용해주시기 바랍니다.\r\n전송실패 파일: %d", nCountFailed);
				}
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n컴퓨터를 종료합니다!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			SystemShutdownOTN();		
			SetTimer(44, 100, NULL);
		}
		else if(Skin_GetCheck("CHK_PROGRAM") == TRUE)
		{
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
				{
					sprintf_s(strResult, 2048, "전송에 실패 또는 저작권제한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n"
						"해당 파일은 마이페이지->내가받은 자료에서 2일간 무료다운로드 할 수 있습니다.(저작권제한 파일제외)\r\n"
						"잠시후 ‘내가받은자료’에서 다시 이용해주시기 바랍니다.\r\n전송실패 파일: %d / 저작권 다운로드제한 파일: %d(다운로드불가)", nCountFailed,nCountFailed_Copyright);
				}
				else
				{
					sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n"
						"해당 파일은 마이페이지->내가받은 자료에서 2일간 무료다운로드 할 수 있습니다.\r\n"
						"잠시후 ‘내가받은자료’에서 다시 이용해주시기 바랍니다.\r\n전송실패 파일: %d", nCountFailed);
				}
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n프로그램을 종료합니다!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}
			
			SetTimer(44, 100, NULL);
		}
		else
		{
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
				{
					sprintf_s(strResult, 2048, "전송에 실패 또는 저작권제한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n"
						"해당 파일은 마이페이지->내가받은 자료에서 2일간 무료다운로드 할 수 있습니다.(저작권제한 파일제외)\r\n"
						"잠시후 ‘내가받은자료’에서 다시 이용해주시기 바랍니다.\r\n전송실패 파일: %d / 저작권 다운로드제한 파일: %d(다운로드불가)", nCountFailed,nCountFailed_Copyright);
				}
				else
				{
					sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n"
						"해당 파일은 마이페이지->내가받은 자료에서 2일간 무료다운로드 할 수 있습니다.\r\n"
						"잠시후 ‘내가받은자료’에서 다시 이용해주시기 바랍니다.\r\n전송실패 파일: %d", nCountFailed);
				}
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}
		}	
	}	
}

