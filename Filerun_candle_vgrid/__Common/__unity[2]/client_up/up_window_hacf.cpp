#include "stdafx.h"
#include "up_window.h"
#include "FtpClient.h"
//20071211 추가, jyh
#include "Dlg_Slide.h"
#include <time.h>

//20080121 파일 사이즈 로컬 저장을 위한 스레드의 프로시저, jyh
UINT SendSizeThread(LPVOID pParam)
{
	up_window* pUp_Window = (up_window*)pParam;
	CString strFilename;
	char sData[1024];
	char sDir[1024];
	
	::GetSystemDirectory(sDir, 1024);
	strFilename.Format("%s\\%s", sDir, SENDSIZE_FILENAME);

	while(pUp_Window->m_bFileTrans)
	{
		Sleep(10);

		if(pUp_Window->m_nFileType < 5 && pUp_Window->m_nFileSizeEnd > 0)
		{
			ZeroMemory(sData, sizeof(sData));
			sprintf_s(sData, 1024, "%d/%I64d/%I64d/%I64d",
				pUp_Window->m_nFileType, pUp_Window->m_nBoardIndex,
				pUp_Window->m_nFileIndex, pUp_Window->m_nFileSizeEnd);
			pUp_Window->m_fout.seekp(0);
			pUp_Window->m_fout.write(sData, lstrlen(sData));
			pUp_Window->m_fout.flush();
		}		
	}
	
	pUp_Window->m_fout.clear();
	pUp_Window->m_fout.close();
	DeleteFile(strFilename);
	AfxEndThread(0);

	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 파일 전송 상태 설정
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool up_window::ReSetTransInfo()
{
	CString strBuf, strTime, strTimeOld, strSpeed;
	PRUPCTX pItemData = NULL;

	__int64 nFileSizeEnd = 0;
	__int64 nFileCountEnd = 0;
	int nTimeSec, nTimeMin, nTimeHour;
	int nTransIndex;


	if(m_bForceExit)
		return false;


	// 전체 체크
	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if((pItemData = (PRUPCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
		{
			if(	pItemData->nTransState == ITEM_STATE_WAIT ||
				pItemData->nTransState == ITEM_STATE_TRANS ||
				pItemData->nTransState == ITEM_STATE_STOP ||
				pItemData->nTransState == ITEM_STATE_COMPLETE)
			{
				nFileSizeEnd	+= pItemData->nFileSizeEnd;
			}

			if(	pItemData->nTransState == ITEM_STATE_COMPLETE)
				nFileCountEnd++;
		}
	}
	
	
	// 파일정보	
	__int64 nFileCountEnd_ = m_pFtpClint->m_bFlag ? nFileCountEnd+1 : nFileCountEnd;
	strBuf.Format("%I64d / %I64d (%s / %s)", nFileCountEnd_, m_nFileCount, SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal));
	//m_pStatic_FileCount.SetWindowText(strBuf);	//20070918 jyh


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
	//m_pStatic_Time_Use.GetWindowText(strTimeOld);		//20070918 jyh

	if(m_nTimeFileStart != 0)
		m_nTimeFileTime = ((m_nTime - m_nTimeFileStart) / 1000);

	if(m_nTimeFileTime != 0)
	{
		nTimeSec	= (m_nTimeTotal + m_nTimeFileTime) % 60;
		nTimeMin	= ((m_nTimeTotal + m_nTimeFileTime) / 60) % 60;
		nTimeHour	= ((m_nTimeTotal + m_nTimeFileTime) / 3600);		
		strTime.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);		
		//m_pStatic_Time_Use.SetWindowText(strTime);	//20070918 jyh
	}
	else
	{
		strTime = strTimeOld;
		//m_pStatic_Time_Use.SetWindowText(strTime);	//20070918 jyh
	}

	// 전송중인 파일 설정
	if((nTransIndex = GetTransListIndex()) != -1)
	{
		try
		{
			pItemData = (PRUPCTX)m_pListCtrl.GetItemData(nTransIndex);
			//20090304 컨텐츠 업로드완료 통보, jyh
			m_nBoardIndex = pItemData->nBoardIndex;

			if(strcmp(m_strPrevFileName.GetBuffer(), pItemData->pFileName))
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

				strBuf.Format("%s'업로드 중'", strtemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
			}

			m_strPrevFileName.ReleaseBuffer();

			//전송
			int nPercent2 = abs((int)(pItemData->nFileSizeEnd * 100 / pItemData->nFileSizeReal));
			strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			//strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal));			
			m_pListCtrl.SetItemText(nTransIndex, 1, strBuf);

			if(m_nTimeFileStart != 0)
			{
				strSpeed = SetSpeedFormat(pItemData->nFileSizeEnd - pItemData->nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);
				//20070919 속도 출력, jyh
				m_pStatic_Speed.SetWindowText(strSpeed);
				//20070919 jyh
				RECT rt = { 423, 54, 490, 66 };
				InvalidateRect(&rt);

				if((m_nTime - m_nTimeFileSpeed) > 3000  || (m_nTransSpeed_Last < m_nTransSpeed && (m_nTime - m_nTimeFileSpeed) > 1000))
				{
					//m_pListCtrl.SetItemText(nTransIndex, 2, strSpeed);

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

				//20080121 전송 사이즈 로컬 저장, jyh
				if(pItemData->nFileSizeEnd > 0 && pItemData->nFileSizeEnd <= pItemData->nFileSizeReal)
				{
					m_nFileType			= pItemData->nFileType;
					m_nBoardIndex		= pItemData->nBoardIndex;
					m_nFileIndex		= pItemData->nFileIndex;
					m_nFileSizeEnd		= pItemData->nFileSizeEnd;
				}
				
				//20080121 전송 사이즈 로컬 저장을 위해 막음, jyh
				//// 전송완료된 사이즈 임시 전송
				//if((m_nTime - m_nTimeFileUpdate) > 3000)
				//{
				//	if(pItemData->nFileSizeEnd > 0 && pItemData->nFileSizeEnd <= pItemData->nFileSizeReal)
				//	{
				//		UPLOADEND pTransEndInfo;
				//		pTransEndInfo.nFileType		= pItemData->nFileType;
				//		pTransEndInfo.nBoardIndex	= pItemData->nBoardIndex;
				//		pTransEndInfo.nFileIdx		= pItemData->nFileIndex;
				//		pTransEndInfo.nFileSizeEnd	= pItemData->nFileSizeEnd;

				//		Packet* pPacket = new Packet;
				//		pPacket->Pack(LOAD_UPSTATE, (char*)&pTransEndInfo, sizeof(UPLOADEND));
				//		m_pWorker_Ctrl->SendPacket(pPacket);
				//		
				//		////test 1221
				//		//FILE* fp = NULL;
				//		////DWORD dwtime = GetTickCount();
				//		//SYSTEMTIME time;
				//		//GetSystemTime(&time);
				//		//char temp[64];
				//		//sprintf(temp, "%02d:%02d:%02d.%d\r\n", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
				//		//size_t len = lstrlen(temp);

				//		//fp = fopen("ReSetTransInfo_Log.txt", "a+");
				//		//fwrite(temp, sizeof(char), len, fp);
				//		//fclose(fp);
				//	}

				//	m_nTimeFileUpdate = m_nTime;
				//}
			}

			//20070918 상태 출력, jyh
			//strBuf.Format("%d%%", abs((int)(pItemData->nFileSizeEnd  * 100 / pItemData->nFileSizeReal)));		
			strBuf = GetFileStatus(pItemData->nTransState);
			//m_pListCtrl.SetItemText(nTransIndex, 2, strBuf);	

			//20070919 현재 파일 전송 상태 출력, jyh
			m_pCtrl_ProgressCur.SetPos(nPercent2);
			strBuf.Format("%s / %s (%d%%)", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			m_pCtrl_ProgressCur.SetText(strBuf);
		
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
			//전체 파일 전송 상태
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			m_pCtrl_ProgressTotal.SetPos(nPercent);
			m_nPercent = nPercent;	//20080225 전체 파일의 전송 퍼센트, jyh

			if(m_pFtpClint->m_bFlag && !strSpeed.IsEmpty())
			{
				//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
				strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
				m_pCtrl_ProgressTotal.SetText(strBuf);
			}
			else
			{
				//strBuf.Format("전체 %d%%", nPercent);
				strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
				m_pCtrl_ProgressTotal.SetText(strBuf);
			}

			if(m_pFtpClint->m_bFlag)
			{
				strBuf.Format("[%d%%] %s", nPercent, CLIENT_UP_TITLE);
				SetWindowText(strBuf);
			}
		}
	}

	return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 20071211 구라 전송을 위해, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool up_window::ReSetTransInfoGura()
{
	CString strBuf, strTime, strTimeOld, strSpeed;
	PRUPCTX pItemData = NULL;

	__int64 nFileSizeEnd = 0;
	__int64 nFileCountEnd = 0;
	int nTimeSec, nTimeMin, nTimeHour;
	int nTransIndex;


	if(m_bForceExit)
		return false;


	// 전체 체크
	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if((pItemData = (PRUPCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
		{
			if(	pItemData->nTransState == ITEM_STATE_WAIT ||
				pItemData->nTransState == ITEM_STATE_TRANS ||
				pItemData->nTransState == ITEM_STATE_STOP ||
				pItemData->nTransState == ITEM_STATE_COMPLETE)
			{
				nFileSizeEnd	+= pItemData->nFileSizeEnd;
			}

			if(	pItemData->nTransState == ITEM_STATE_COMPLETE)
				nFileCountEnd++;
		}
	}
	
	//m_nTime = GetTickCount();

	// 전체 남은 시간 설정
	//if(m_nTransSpeed > 0)
	//{

	srand((unsigned)time(NULL));

	DWORD dwBuf = 0; 

	switch(rand()%10)
	{
	case 0:
		dwBuf = 1148576;
		break;
	case 1:
		dwBuf = 1248576;
		break;
	case 2:
		dwBuf = 1098934;
		break;
	case 3:
		dwBuf = 1400040;
		break;
	case 4:
		dwBuf = 1348576;
		break;
	case 5:
		dwBuf = 1188600;
		break;
	case 6:
		dwBuf = 1302016;
		break;
	case 7:
		dwBuf = 1111576;
		break;
	case 8:
		dwBuf = 1079994;
		break;
	case 9:
		dwBuf = 1275522;
		break;
	}

	nFileSizeEnd += dwBuf;

	if(m_nFileSizeTotal < nFileSizeEnd)
		nFileSizeEnd = m_nFileSizeTotal;
	
	__int64 nTimeOver = (__int64)((m_nFileSizeTotal - nFileSizeEnd) / dwBuf);
	nTimeSec	= (int)((nTimeOver) % 60);
	nTimeMin	= (int)(((nTimeOver) / 60) % 60);
	nTimeHour	= (int)(((nTimeOver) / 3600));		

	strBuf.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);
	m_pStatic_Time_Over.SetWindowText(strBuf);

	//20070919 jyh
	RECT rt = { 579, 54, 629, 66 };
	InvalidateRect(&rt);
			
	
	// 전송중인 파일 설정
	if((nTransIndex = GetTransListIndex()) != -1)
	{
		try
		{
			pItemData = (PRUPCTX)m_pListCtrl.GetItemData(nTransIndex);

			if(strcmp(m_strPrevFileName.GetBuffer(), pItemData->pFileName))
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

				strBuf.Format("%s'업로드 중'", strtemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
			}
			
			m_strPrevFileName.ReleaseBuffer();

			if(pItemData->nFileSizeReal > pItemData->nFileSizeEnd)
			{
				if((pItemData->nFileSizeReal - pItemData->nFileSizeEnd) >= dwBuf)
                    pItemData->nFileSizeEnd += dwBuf;
				else
				{
					pItemData->nFileSizeEnd += pItemData->nFileSizeReal - pItemData->nFileSizeEnd;
					pItemData->nTransState = ITEM_STATE_COMPLETE;
				}
			}
			else
			{
				pItemData->nTransState = ITEM_STATE_COMPLETE;
			}

			//전송
			int nPercent2 = abs((int)(pItemData->nFileSizeEnd * 100 / pItemData->nFileSizeReal));
			strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			//strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal));			
			m_pListCtrl.SetItemText(nTransIndex, 1, strBuf);

			//if(m_nTimeFileStart != 0)
			//{
				strSpeed = SetSizeFormat(dwBuf);
				//20070919 속도 출력, jyh
				m_pStatic_Speed.SetWindowText(strSpeed);
				//20070919 jyh
				RECT rt = { 423, 54, 490, 66 };
				InvalidateRect(&rt);

				//if((m_nTime - m_nTimeFileSpeed) > 3000  || (m_nTransSpeed_Last < m_nTransSpeed && (m_nTime - m_nTimeFileSpeed) > 1000))
				//{
				//	//m_pListCtrl.SetItemText(nTransIndex, 2, strSpeed);

				//	if(m_nTransSpeed != 0)
				//	{
				//		m_nTimeFileSpeed	= m_nTime;
				//		m_nTransSpeed_Last	= m_nTransSpeed;
				//	}
				//}
				//else
				//{
				//	//strSpeed = m_pListCtrl.GetItemText(nTransIndex, 2);	20070918 주석 처리, jyh
				//}

				// 전송완료된 사이즈 임시 전송
				//if((m_nTime - m_nTimeFileUpdate) > 3000)
				//{
					//if(pItemData->nFileSizeEnd > 0 && pItemData->nFileSizeEnd <= pItemData->nFileSizeReal)
					//if(pItemData->nTransState == ITEM_STATE_COMPLETE)
					//{
					//	KillTimer(999);
					//	UPLOADEND TransEndInfo;
					//	ZeroMemory(&TransEndInfo, sizeof(TransEndInfo));
					//	TransEndInfo.nFileType		= pItemData->nFileType;
					//	TransEndInfo.nBoardIndex	= pItemData->nBoardIndex;
					//	TransEndInfo.nFileIdx		= pItemData->nHashIndex;	//구라 전송은 저작권 등록 테이블 인덱스를 보냄
					//	TransEndInfo.nFileSizeEnd	= pItemData->nFileSizeEnd;

					//	Packet* pPacket = new Packet;
					//	pPacket->Pack(LOAD_UPGURAEND, (char*)&TransEndInfo, sizeof(UPLOADEND));
					//	m_pWorker_Ctrl->SendPacket(pPacket);
					//	//파일 한개 전송 완료 후 다시 UpLoadProGura()호출
					//	UpLoadProGura();
					//							
					//	//FILE* fp = NULL;
					//	////DWORD dwtime = GetTickCount();
					//	//SYSTEMTIME time;
					//	//GetSystemTime(&time);
					//	//char temp[64];
					//	//sprintf(temp, "%02d:%02d:%02d.%d\r\n", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
					//	//size_t len = lstrlen(temp);

					//	//fp = fopen("ReSetTransInfo_Log.txt", "a+");
					//	//fwrite(temp, sizeof(char), len, fp);
					//	//fclose(fp);
					//}

				//}
			//}

			//20070918 상태 출력, jyh
			//strBuf.Format("%d%%", abs((int)(pItemData->nFileSizeEnd  * 100 / pItemData->nFileSizeReal)));		
			strBuf = GetFileStatus(pItemData->nTransState);
			m_pListCtrl.SetItemText(nTransIndex, 2, strBuf);	

			//20070919 현재 파일 전송 상태 출력, jyh
			m_pCtrl_ProgressCur.SetPos(nPercent2);
			strBuf.Format("%s / %s (%d%%)", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			m_pCtrl_ProgressCur.SetText(strBuf);
		
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
			//전체 파일 전송 상태
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			m_pCtrl_ProgressTotal.SetPos(nPercent);

			if(m_pFtpClint->m_bFlag && !strSpeed.IsEmpty())
			{
				//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
				strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
				m_pCtrl_ProgressTotal.SetText(strBuf);
			}
			else
			{
				//strBuf.Format("전체 %d%%", nPercent);
				strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
				m_pCtrl_ProgressTotal.SetText(strBuf);
			}

			if(m_pFtpClint->m_bFlag)
			{
				strBuf.Format("[%d%%] %s", nPercent, CLIENT_UP_TITLE);
				SetWindowText(strBuf);
			}
		}

		if(pItemData->nTransState == ITEM_STATE_COMPLETE)
		{
			KillTimer(999);
			UPLOADEND TransEndInfo;
			ZeroMemory(&TransEndInfo, sizeof(TransEndInfo));
			TransEndInfo.nFileType		= pItemData->nFileType;
			TransEndInfo.nBoardIndex	= pItemData->nBoardIndex;
			TransEndInfo.nFileIdx		= pItemData->nHashIndex;	//구라 전송은 저작권 등록 테이블 인덱스를 보냄
			TransEndInfo.nContentsCmpl	= 0;

			std::vector<CONITEM>::iterator iter;
			
			for(iter = m_v_sConItem.begin(); iter != m_v_sConItem.end(); iter++)
			{
				if(iter->nBoardIndex == pItemData->nBoardIndex)
				{
					iter->nItemCnt--;

					if(iter->nItemCnt == 0)
					{
						TransEndInfo.nContentsCmpl = 1;
						m_v_sConItem.erase(iter);
						break;
					}
				}
			}

			Packet* pPacket = new Packet;
			pPacket->Pack(LOAD_UPGURAEND, (char*)&TransEndInfo, sizeof(UPLOADEND));
			m_pWorker_Ctrl->SendPacket(pPacket);
			//파일 한개 전송 완료 후 m_pPtrTrans를 NULL로 만들고 다시 UpLoadProGura()호출
			m_pPtrTrans = NULL;
			UpLoadProGura();
		}
	}

	return true;
}

//20071217 구라 리스트 생성, jyh
void up_window::GuraUpList(PUPHASH upHash)
{
	//CString strPath = upHash->szPath;
	//레지스트리에서 strCopyrightPath 값을 불러온다.
	CString strPath;
	m_pReg.LoadKey(CLIENT_REG_PATH, "strCopyrightPath", strPath);
	
	if(strPath.Right(1) == "\\")
	{
		strPath.Delete(strPath.GetLength()-1, 1);
		GetFilePathInFolder(strPath);
	}
	else
		m_v_cstrGuraFilePath.push_back(strPath);
    
    if((int)m_v_cstrGuraFilePath.size() > 0)
	{
		//구라전송 완료시 컨텐츠 전송 완료인지 체크를 위해..
		CONITEM conitem;
		conitem.nBoardIndex = upHash->bbs_no;
		conitem.nItemCnt	= (int)m_v_cstrGuraFilePath.size();
		m_v_sConItem.push_back(conitem);

		//20071015 아이템 갯수가 8개 이상이면 리스트 뷰의 크기 조절, jyh
		if(m_pListCtrl.GetItemCount() + m_v_cstrGuraFilePath.size() > 8)
			m_pListCtrl.SetColumnWidth(0, 363);
	
		// 프로그램을 최상위 윈도우로 등록
		/*ShowWindow(SW_RESTORE);
		SetForegroundWindow();
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);*/
		if(::GetForegroundWindow() != this->m_hWnd)
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
						::BringWindowToTop(m_hWnd); 
						AttachThreadInput(current_thread_id, thread_id, FALSE); 
					} 
				} 
			} 
		}
	
		// 전송받은 목록을 큐에 등록
		for(size_t i = 0; i < m_v_cstrGuraFilePath.size() ; i++)
		{
			CFile file;
			file.Open(m_v_cstrGuraFilePath[i], CFile::modeRead);

			PRUPCTX pNewGuraItem		= new RUPCTX;
			ZeroMemory(pNewGuraItem, sizeof(RUPCTX));
	
			pNewGuraItem->nTransState	= ITEM_STATE_WAIT;
			pNewGuraItem->nHashIndex	= upHash->hash_idx;
			pNewGuraItem->nFileSizeStart= 0;
			pNewGuraItem->nFileSizeEnd	= 0;
			pNewGuraItem->nFileType		= upHash->nFileType;
			pNewGuraItem->nBoardIndex	= upHash->bbs_no;
			pNewGuraItem->nFileIndex	= 0;				
			pNewGuraItem->nFileSizeReal	= (__int64)file.GetLength();
	
			lstrcpy(pNewGuraItem->pFileName, file.GetFileName().GetBuffer());		
			lstrcpy(pNewGuraItem->pFullPath, m_v_cstrGuraFilePath[i].GetBuffer());
			::PathRemoveFileSpec(m_v_cstrGuraFilePath[i].GetBuffer());	
			lstrcpy(pNewGuraItem->pFolderName, m_v_cstrGuraFilePath[i].GetBuffer());
			InsertItem(pNewGuraItem);
		}
	}
		
	//전송 되는것 처럼 보이게...
	if(m_rDownAutoTrans == 1 && !m_bFileTrans)	//20071030 환경설정에서 자동전송 적용을 위해, jyh
	{
		m_bTransGura = true;
		SetTransFlagGura(true);
	}
}

void up_window::FileTransLoop()
{
	// 전송시작시간 설정
	m_nTimeFileUpdate = m_nTimeFileStart = GetTickCount();
	
	//20071128 ReSetTransInfo_Log 작성, jyh
	/*FILE* fp = NULL;
	SYSTEMTIME time;
	
	char temp[64];
	fp = fopen("ReSetTransInfo_Log.txt", "a+");
	

	while(m_pFtpClint->m_bFlag)
	{
		GetSystemTime(&time);
		sprintf(temp, "%02d:%02d:%02d.%d\r\n", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		size_t len = lstrlen(temp);
		fwrite(temp, sizeof(char), len, fp);
		
		m_pListCtrl.SetRedraw(FALSE);
		ReSetTransInfo();
		m_pListCtrl.SetRedraw(TRUE);		

		Sleep(500);
	}
	
	fclose(fp);*/

	//20080121 전송 파일 사이즈 로컬 저장을 위한 스레드 생성, jyh
	CWinThread* pSendSizeThread = AfxBeginThread(SendSizeThread, (LPVOID)this);

	while(m_pFtpClint->m_bFlag)
	{
		m_pListCtrl.SetRedraw(FALSE);
		ReSetTransInfo();
		m_pListCtrl.SetRedraw(TRUE);		

		Sleep(500);
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
	SetWindowText(CLIENT_UP_TITLE);



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

		m_pListCtrl.Invalidate();
		
		// 파일 전송완료 통보
		UPLOADEND pTransEndInfo;
		ZeroMemory(&pTransEndInfo, sizeof(pTransEndInfo));
		pTransEndInfo.nFileType		= m_pPtrTrans->nFileType;
		pTransEndInfo.nBoardIndex	= m_pPtrTrans->nBoardIndex;
		pTransEndInfo.nFileIdx		= m_pPtrTrans->nFileIndex;
		pTransEndInfo.nFileSizeEnd	= m_pPtrTrans->nFileSizeEnd;

		//20080123 컨텐츠 단위의 업로드를 위해, jyh
		std::vector<CONITEM>::iterator iter;

		for(iter = m_v_sConItem.begin(); iter != m_v_sConItem.end(); iter++)
		{
			if(iter->nBoardIndex == m_pPtrTrans->nBoardIndex)
			{
				iter->nItemCnt--;

				if(iter->nItemCnt == 0)
				{
					pTransEndInfo.nContentsCmpl = 1;
					m_v_sConItem.erase(iter);
					break;
				}
			}
		}

		Packet* pPacket = new Packet;
		pPacket->Pack(LOAD_UPEND, (char*)&pTransEndInfo, sizeof(UPLOADEND));
		m_pWorker_Ctrl->SendPacket(pPacket);

		//20080121 파일전송이 끝나면 로컬에 저장하던 파일도 끝낸다, jyh
		/*if(pSendSizeThread)
		{
			int iExit = 1;
			SendSizeThread(&iExit);
		}*/


		// 전송중인 파일이 없음으로 설정
		m_pPtrTrans	= NULL;		
	}

	// 중지 버튼을 클릭한 것이 아니면 다음 리스트가 있는지 체크
	if(m_bFileTrans)
		UpLoadPro();
}

unsigned int __stdcall ProcFileTransLoop(void *pParam)
{
	up_window* pProcess = (up_window*)pParam;
	pProcess->FileTransLoop();
	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 전송중인지 설정 : 버튼활성화 등등의 작업
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::SetTransFlag(bool bFlag)
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

	// 전송중으로 설정시
	if(bFlag) 
	{
		m_pPtrTrans = NULL;		//20081028 추가, jyh
		m_bFileTrans = true;
		UpLoadPro();
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
		//	m_pListCtrl.SetItemText(m_iStopItemIdx, 2, GetFileStatus(ITEM_STATE_STOP));	
		//}

		SetWindowText(CLIENT_UP_TITLE);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 20071211 구라 전송을 위해, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::SetTransFlagGura(bool bFlag)
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
	//Skin_SetButtonEnable("BTN_UP",		!bFlag);	//20080228 전송중에도 선택목록 삭제 되게, jyh
	//Skin_SetButtonEnable("BTN_DOWN",	!bFlag);		//20080228 전송중에도 선택목록 삭제 되게, jyh
	//Skin_SetButtonEnable("BTN_BOTTOM",	!bFlag);	//20080228 전송중에도 선택목록 삭제 되게, jyh

	// 전송중으로 설정시
	if(bFlag) 
	{
		m_pPtrTrans = NULL;		//20081028 추가, jyh
		m_bFileTrans = true;
		UpLoadProGura();	//구라 전송
	}
	// 전송종료로 설정시
	else
	{
		m_bFileTrans = false;	
		m_pFtpClint->m_bFlag = false;

		////20070918 파일 상태 중지로 변경, jyh
		//if(m_pPtrTrans)
		//{
		//	m_pPtrTrans->nTransState = ITEM_STATE_STOP;
		//	m_pListCtrl.SetItemText(m_iStopItemIdx, 2, GetFileStatus(ITEM_STATE_STOP));	
		//}

		SetWindowText(CLIENT_UP_TITLE);
	}
}

/*
	본함수는 전송시작을 클릭했을때와 하나의 파일이 전송이 완료되었을때 호출됨
*/
void up_window::UpLoadPro()
{
	ACE_UINT64	nItemFileSize = 0;
	int			nItemCheck = 0;
	PRUPCTX		pPtrCheck;

	if(!m_bForceExit && GetTransListIndex() == -1 && m_pPtrTrans == NULL)
	{
		for(int nProcessIndex = 0 ; nProcessIndex < m_pListCtrl.GetItemCount(); nProcessIndex++)
		{	
			pPtrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nProcessIndex);

			if(pPtrCheck == NULL)
				continue;

			if(pPtrCheck->nTransState == ITEM_STATE_COMPLETE)
			{
				//20071030 전송 상태와 파일 크기 출력, jyh
				CString strTemp;
				strTemp.Format("%s", SetSizeFormat(pPtrCheck->nFileSizeReal));
				m_pListCtrl.SetItemText(nProcessIndex, 1, strTemp);
				strTemp = GetFileStatus(pPtrCheck->nTransState);
				m_pListCtrl.SetItemText(nProcessIndex, 2, strTemp);

				continue;
			}

			if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
				pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				pPtrCheck->nTransState == ITEM_STATE_STOP)// ||
				//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20071119 추가, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_CONNECT/*20080228 추가, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20080228 추가, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 추가, jyh*/)
			{
				//20071119 현재 전송될 파일의 인덱스 값을 멤버변수로 저장, jyh
				m_nCurTransFile = nProcessIndex;
				// 해당파일이 존재하는지 체크
				CFileFind pFindFile;
				if(!pFindFile.FindFile(pPtrCheck->pFullPath))
				{
					pFindFile.Close();
					pPtrCheck->nTransState = ITEM_STATE_ERROR;

					char strResult[2048];
					sprintf_s(strResult, 2048, "업로드 파일이 존재하지 않습니다!\r\n%s", pPtrCheck->pFullPath);
					::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
					continue;
				}

				pFindFile.FindNextFile();
				nItemFileSize = pFindFile.GetLength();
				pFindFile.Close();
				


				// 이전 선택된 모든 아이템 선택해제
				while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
					m_pListCtrl.SetItemState(nItemCheck, 0, -1);

				// 전송될 아이템 포커서와 선택된 상태로 변경
				m_pListCtrl.SetItemState(nProcessIndex, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
				m_pListCtrl.EnsureVisible(nProcessIndex, TRUE);						

				// 전송중인 아이템 설정 : 전송중으로 설정
				m_pPtrTrans = pPtrCheck;
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;
				m_pListCtrl.SetItemText(m_nCurTransFile, 2, GetFileStatus(ITEM_STATE_CONNECT));
				
				
				// 업로드를 시작하기 전에 현재 업로드 할 파일의 상태 체크
				UPLOADPRE pTransInfo;
				pTransInfo.nFileType		= m_pPtrTrans->nFileType;
				pTransInfo.nBoardIndex		= m_pPtrTrans->nBoardIndex;
				pTransInfo.nFileIdx			= m_pPtrTrans->nFileIndex;
				pTransInfo.nFileSize		= nItemFileSize;
				pTransInfo.nFileSize_Complete = 0;
				pTransInfo.nResult			= 0;

				Packet* pPacket = new Packet;
				pPacket->Pack(LOAD_UPPRE, (char*)&pTransInfo, sizeof(UPLOADPRE));
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
	SetWindowText(CLIENT_UP_TITLE);

	// 전송완료된 목록이 하나이상 있을때만 처리(리스트가 없는 상태에서 전송 버튼을 클릭하는 경우를 제외하기 위함)
	if(m_pListCtrl.GetItemCount() <= 0)
		return;

	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if((pPtrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
		{
			if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
				pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				pPtrCheck->nTransState == ITEM_STATE_STOP||
				pPtrCheck->nTransState == ITEM_STATE_COPYRIGHT||
				pPtrCheck->nTransState == ITEM_STATE_VIRUSFILE)
			{
				return;
			}
		}
	}



	char strResult[2048];
	int nCountComplete	= 0;
	int nCountFailed	= 0;
	int nCountFailed_Copyright	= 0;
	int nCountFailed_Virus	= 0;

	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if(((PRUPCTX)m_pListCtrl.GetItemData(nIndex))->nTransState == ITEM_STATE_COMPLETE)
			nCountComplete++;
		else
		{
			if(((PRUPCTX)m_pListCtrl.GetItemData(nIndex))->nTransState == ITEM_STATE_COPYRIGHT)
				nCountFailed_Copyright++;
			
			if(((PRUPCTX)m_pListCtrl.GetItemData(nIndex))->nTransState == ITEM_STATE_VIRUSFILE)
				nCountFailed_Virus++;

			nCountFailed++;
		}
	}

	//20080131 최소창과 트레이 아이콘으로 실행중일때 전송완료되면 슬라이드 윈도를 띄운다, jyh
	if(m_pTray->IsActiveTray() || m_bMinWindow)
	{
		//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
		if(nCountFailed)
			sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어\r\n파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
		else
			sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d", nCountComplete);

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
			//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
					sprintf_s(strResult, 2048, "전송에 실패 또는 저작권제한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d\r\n저작권 업로드제한 파일: %d(업로드불가)\r\n악성코드 업로드제한 파일: %d(업로드불가)", nCountFailed,nCountFailed_Copyright,nCountFailed_Virus);
				else
					sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d\r\n\r\n 컴퓨터를 종료합니다!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d\r\n\r\n컴퓨터를 종료합니다!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SystemShutdownOTN();		
			SetTimer(44, 100, NULL);
		}
		else if(Skin_GetCheck("CHK_PROGRAM") == TRUE)
		{
			//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
					sprintf_s(strResult, 2048, "전송에 실패 또는 저작권제한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d / 저작권 업로드제한 파일: %d(업로드불가)", nCountFailed,nCountFailed_Copyright);
				else
					sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);

				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d\r\n\r\n 프로그램을 종료합니다!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d\r\n\r\n 프로그램을 종료합니다!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SetTimer(44, 100, NULL);
		}
		else
		{
			//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
					sprintf_s(strResult, 2048, "전송에 실패 또는 저작권제한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: % / 저작권 업로드제한 파일: %d(업로드불가)", nCountFailed,nCountFailed_Copyright);
				else
					sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}
			
			//sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
		}
	}
}

//20071211 구라 전송을 위해, jyh
void up_window::UpLoadProGura()
{
	ACE_UINT64	nItemFileSize = 0;
	int			nItemCheck = 0;
	PRUPCTX		pPtrCheck;

	if(!m_bForceExit && GetTransListIndex() == -1 && m_pPtrTrans == NULL)
	{
		for(int nProcessIndex = 0 ; nProcessIndex < m_pListCtrl.GetItemCount(); nProcessIndex++)
		{	
			pPtrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nProcessIndex);

			if(pPtrCheck == NULL)
				continue;

			if(pPtrCheck->nTransState == ITEM_STATE_COMPLETE)
			{
				//20071030 전송 상태와 파일 크기 출력, jyh
				CString strTemp;
				strTemp.Format("%s", SetSizeFormat(pPtrCheck->nFileSizeReal));
				m_pListCtrl.SetItemText(nProcessIndex, 1, strTemp);
				strTemp = GetFileStatus(pPtrCheck->nTransState);
				m_pListCtrl.SetItemText(nProcessIndex, 2, strTemp);

				continue;
			}

			if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
				pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				pPtrCheck->nTransState == ITEM_STATE_STOP ||
				pPtrCheck->nTransState == ITEM_STATE_CONNECT)// ||
				//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20071119 추가, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20081027 추가, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 추가, jyh*/)
			{
				//20071119 현재 전송될 파일의 인덱스 값을 멤버변수로 저장, jyh
				m_nCurTransFile = nProcessIndex;
				// 해당파일이 존재하는지 체크
				CFileFind pFindFile;
				if(!pFindFile.FindFile(pPtrCheck->pFullPath))
				{
					pFindFile.Close();
					pPtrCheck->nTransState = ITEM_STATE_ERROR;

					char strResult[2048];
					sprintf_s(strResult, 2048, "업로드 파일이 존재하지 않습니다!\r\n%s", pPtrCheck->pFullPath);
					::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
					continue;
				}

				pFindFile.FindNextFile();
				nItemFileSize = pFindFile.GetLength();
				pFindFile.Close();

				// 이전 선택된 모든 아이템 선택해제
				while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
					m_pListCtrl.SetItemState(nItemCheck, 0, -1);

				// 전송될 아이템 포커서와 선택된 상태로 변경
				m_pListCtrl.SetItemState(nProcessIndex, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
				m_pListCtrl.EnsureVisible(nProcessIndex, TRUE);						

				// 전송중인 아이템 설정 : 전송중으로 설정
				m_pPtrTrans = pPtrCheck;
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;
				m_pListCtrl.SetItemText(m_nCurTransFile, 2, GetFileStatus(ITEM_STATE_CONNECT));

				//구라 전송
				SetTimer(999, 1000, NULL);
				
				return;
			}
		}
	}



	// 전송중지로 설정
	SetTransFlagGura(false);

	// 전송 파일 갯수 변경
	ReSetTransInfoGura();

	// 기본 타이틀로 변경
	SetWindowText(CLIENT_UP_TITLE);

	// 전송완료된 목록이 하나이상 있을때만 처리(리스트가 없는 상태에서 전송 버튼을 클릭하는 경우를 제외하기 위함)
	if(m_pListCtrl.GetItemCount() <= 0)
		return;

	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if((pPtrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
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

	for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		if(((PRUPCTX)m_pListCtrl.GetItemData(nIndex))->nTransState == ITEM_STATE_COMPLETE)
			nCountComplete++;
		else
			nCountFailed++;
	}

	//20080131 최소창과 트레이 아이콘으로 실행중일때 전송완료되면 슬라이드 윈도를 띄운다, jyh
	if(m_pTray->IsActiveTray() || m_bMinWindow)
	{
		//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
		if(nCountFailed)
			sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어\r\n파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
		else
			sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d", nCountComplete);

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
			//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
			if(nCountFailed)
			{
				sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d\r\n\r\n 컴퓨터를 종료합니다!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d\r\n\r\n컴퓨터를 종료합니다!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SystemShutdownOTN();		
			SetTimer(44, 100, NULL);
		}
		else if(Skin_GetCheck("CHK_PROGRAM") == TRUE)
		{
			//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
			if(nCountFailed)
			{
				sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d\r\n\r\n 프로그램을 종료합니다!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d\r\n\r\n 프로그램을 종료합니다!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SetTimer(44, 100, NULL);
		}
		else
		{
			//20081028 전송 실패 파일이 하나라도 있으면 전송이 완료되지 않았다는 메세지를 띄운다, jyh
			if(nCountFailed)
			{
				sprintf_s(strResult, 2048, "전송에 실패한 파일이 있어 파일 전송을 완료하지 못했습니다!\r\n전송실패 파일: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "파일 전송이 완료되었습니다!\r\n전송성공 : %d", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "전송이 완료되었습니다!\r\n전송성공 : %d\r\n전송실패 : %d", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
		}
	}

	
}