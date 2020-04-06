#include "stdafx.h"
#include "up_window.h"
#include "shlwapi.h"
#include "Dlg_Setup.h"
#include "Dlg_Exist.h"
#include "Dlg_Confirm.h"

#pragma comment(lib, "version.lib")


bool up_window::Skin_ButtonPressed(CString m_ButtonName){return true;}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 전송중인 아이템 인덱스.. 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int  up_window::GetTransListIndex()
{
	EnterCriticalSection(&m_cs);

	if(m_bFileTrans && m_pPtrTrans != NULL)
	{
		for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
		{
			if((PRUPCTX)m_pListCtrl.GetItemData(nIndex) == m_pPtrTrans)
			{
				LeaveCriticalSection(&m_cs);	
				return nIndex;
			}
		}
	}
	LeaveCriticalSection(&m_cs);	
	return -1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 20071219 전송실패 아이템 순위변경, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DWORD  up_window::DeferItem()
{
	EnterCriticalSection(&m_cs);

	PRUPCTX pPtrCheck = NULL;
	PRUPCTX pPtrCurrCheck = NULL;	//20071119 jyh
	int nCnt = 0;					//20071119 같은 게시물의 파일 수, jyh	
	int nItemCheck = 0;
	int nCountPoss = 0;
	int nIndexCurrent = m_nCurTransFile;
	if(nIndexCurrent < 0) return -1;

	for(int nIndex = nIndexCurrent+1; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		pPtrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nIndex);
		pPtrCurrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nIndexCurrent);	//20071119 jyh

		if(pPtrCheck == NULL)
			continue;

		if(pPtrCheck->nTransState == ITEM_STATE_COMPLETE)
			continue;

		if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
			pPtrCheck->nTransState == ITEM_STATE_TRANS ||
			pPtrCheck->nTransState == ITEM_STATE_STOP)// ||
			//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20081027 추가, jyh*/ ||
			//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20081027 추가, jyh*/ ||
			//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 추가, jyh*/)
		{
			nCountPoss++;
		}

		////20071010 컨텐츠 단위로 이동을 위해 파일의 게시물 인덱스 번호를 비교, jyh
		//if(pPtrCurrCheck->nBoardIndex == pPtrCheck->nBoardIndex)
		//{
		//	pPtrCheck->nTransState = pPtrCurrCheck->nTransState;
		//	m_pListCtrl.SetItemText(nIndex, 2, GetFileStatus(pPtrCheck->nTransState));
		//	
		//	nCnt++;
		//}
	}	

	LeaveCriticalSection(&m_cs);	

	////20071010 같은 게시물 수 만큼 이동(현재 파일까지 nCnt+1), jyh
	//for(int i=0; i<nCnt+1; i++)
	//{
	//	MoveItem(MOVE_ML);
	//	// 이전 선택된 모든 아이템 선택해제
	//	while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
	//		m_pListCtrl.SetItemState(nItemCheck, 0, -1);

	//	// 전송될 아이템 포커서와 선택된 상태로 변경
	//	m_pListCtrl.SetItemState(nIndexCurrent, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
	//	m_pListCtrl.EnsureVisible(nIndexCurrent, TRUE);		
	//}

	//return nCountPoss - nCnt;	//20071219 값이 0이면 다음 컨텐츠가 없음(컨텐츠 단위기 때문에 -nCnt를 해줘야 함), jyh

	return nCountPoss;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 리스트 컨트롤에 아이템 삽입
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::InsertItem(PRUPCTX pNewItem)
{
	int nItem = -1;
	SHFILEINFO sfi;	

	CString strBuf, strFileType;

	ZeroMemory(&sfi, sizeof(SHFILEINFO));		

	if(strcmp(pNewItem->pFullPath, ""))
	{
		strBuf = pNewItem->pFullPath;
		strFileType.Format("*%s", strBuf.Right(4));
		SHGetFileInfo(strFileType, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_TYPENAME);


		EnterCriticalSection(&m_cs);
		{
			m_nFileCount++;
			m_nFileSizeTotal += pNewItem->nFileSizeReal;

			m_pListCtrl.SetRedraw(FALSE);

			nItem = m_pListCtrl.InsertItem(m_pListCtrl.GetItemCount(), pNewItem->pFileName, sfi.iIcon);

			// 파일 크기	
			strBuf.Format("%s/%s", SetSizeFormat(pNewItem->nFileSizeEnd), SetSizeFormat(pNewItem->nFileSizeReal));
			m_pListCtrl.SetItemText(nItem, 1, strBuf);

			CString strStatus = GetFileStatus(pNewItem->nTransState);
			// 전송 상태
			m_pListCtrl.SetItemText(nItem, 2, strStatus);
			//// 전송 속도
			//m_pListCtrl.SetItemText(nItem, 3, "");
			//// 남은 시간
			//m_pListCtrl.SetItemText(nItem, 4, "00:00:00");

			//// 저장 폴더
			//m_pListCtrl.SetItemText(nItem, 5, pNewItem->pFolderName);

			m_pListCtrl.SetItemData(nItem, (DWORD_PTR)pNewItem);

			m_pListCtrl.SetRedraw(TRUE);

		}
		LeaveCriticalSection(&m_cs);	
	}


	// 전송 파일 갯수 변경
	ReSetTransInfo();
}

//20090304 컨텐츠 업로드완료 통보, jyh
LRESULT up_window::MSG_Complete(WPARAM wParam, LPARAM lParam)
{
	CString strData;
	strData.Format("%s%I64d", URL_SENDCONTENTS, m_nBoardIndex);
	m_pSendComplete->Navigate(strData);
	TRACE(strData);

	return 0;
}


LRESULT up_window::MSG_Window(WPARAM wParam, LPARAM lParam)
{
	Dlg_Alert pWindow;
	m_pAlert = &pWindow;
	pWindow.m_pInfo = (char*)wParam;
	pWindow.m_bAutoClose = (BOOL)lParam;
	//pWindow.DoModal();
	//20071119 수정, jyh
	int ret = (int)pWindow.DoModal();

	if(ret == IDRETRY)			//재시도
		m_nBtnState = BTN_STATE_RETRY;
	else if(ret == IDIGNORE)	//건너뛰기
		m_nBtnState = BTN_STATE_JUMP;
	else if(ret == IDYES)		//예
		m_nBtnState = BTN_STATE_YES;
	else if(ret == IDNO)		//아니오
		m_nBtnState = BTN_STATE_NO;
	else if(ret == IDCANCEL)	//취소
		m_nBtnState = BTN_STATE_CANCEL;

	m_pAlert = NULL;
	return 0;
}

LRESULT up_window::MSG_UpLoad(WPARAM wParam, LPARAM lParam)
{
	UpLoadPro();				
	return 0;
}


LRESULT up_window::MSG_OverWrite(WPARAM wParam, LPARAM lParam)
{
	__int64 nTotalSize = *(__int64*)lParam;

	Dlg_Exist pWindow;
	pWindow.m_pInfo_FilePath	= (char*)wParam;
	pWindow.m_pInfo_Size		= SetSizeFormat(nTotalSize);
	pWindow.DoModal();	

	// 항상적용이면 변수적용
	if(pWindow.m_nAllApply)
	{
		m_rUpOverWrite = pWindow.m_nResult;
		m_pReg.SaveKey(CLIENT_REG_PATH, "rUpOverWrite",		m_rUpOverWrite);
	}

	return pWindow.m_nResult;
}

LRESULT up_window::MSG_ForceExit(WPARAM wParam, LPARAM lParam)
{
	ReplyMessage(0);
	SetTransFlag(false);

	Dlg_Alert pWindow;
	pWindow.m_pInfo = (char*)wParam;
	pWindow.m_bAutoClose = TRUE;
	pWindow.DoModal();	

	Exit_Program(true, false);
	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 아이템을 더블클릭 했을때
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->iItem != -1)
	{
		PRUPCTX pItemData = (PRUPCTX)m_pListCtrl.GetItemData(pNMLV->iItem);
		
		// 파일위치를 클릭할 경우 해당 폴더 오픈
		if(pNMLV->iSubItem == 5)
			ShellExecute(m_hWnd, "explore", pItemData->pFolderName, NULL, NULL, SW_SHOWNORMAL);	
		// 나머지는 실행
		else
			ShellExecute(m_hWnd, NULL, pItemData->pFullPath, NULL, NULL, SW_SHOWNORMAL);	
	}
	
	*pResult = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 파일 사이즈별 출력 포멧
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString up_window::SetSizeFormat(__int64 nRealSize)
{
	char pTemp[256];
	::StrFormatByteSize64(nRealSize, pTemp, 256);	
	CString bSize = pTemp;
	bSize.Replace("바이트", "B");	
    return bSize;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 숫자 포멧
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString up_window::SetNumFormat(__int64 nNum)
{
	CString strBuf;
	TCHAR szOUT[20];

	NUMBERFMT nFmt = { 0, 0, 3, ".", ",", 1 };	
	strBuf.Format("%I64d", nNum);
	::GetNumberFormat(NULL, NULL, strBuf, &nFmt, szOUT, 20);
	strBuf = szOUT;
	return strBuf;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 현재 파일의 전송 속도 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString up_window::SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec)
{
	// 10Mbps=10000kbps=1250kB/s
	// 1Mbps=1,000kbps=128,000Byte/s
	
	CString bSize;
	__int64 size = transsize;
	__int64	time;
	float bonus = 1;

	if(startsec != 0)
	{

		time = nowsec - startsec;
		if(time < 1) time = 1;

		size = (__int64)(size * bonus / time);

		// 남은시간 계산을 위한 마지막 속도
		m_nTransSpeed = size;
	}
	else
	{
		size = m_nTransSpeed;
	}

	/*
	// MCPS
	if(size >= 128000 * 8)		
	{
		pointsize = ((double)size / (128000 * 8));
		bSize.Format("%.1f MB/sec", pointsize);
	}
	// KCPS
	else if(size >= 128 * 8 && size < 128000 * 8)	
	{
		pointsize = ((double)size / (128 * 8));
		bSize.Format("%.1f KB/sec", pointsize);
	}
	// CPS
	else				
	{
		pointsize = (double)size * 8;
		bSize.Format("%.1f B/sec", pointsize);
	}
	*/

	bSize.Format("%s/sec", SetSizeFormat(size));
	return bSize;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 아이템 이동
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::MoveItem(int mode)
{	
	DWORD_PTR chitem[100];
	for(int i = 0 ; i < 100 ; i++)
		chitem[i] = -1;

	int sitem;
	int lastmoveitem = -1;
	int firstmoveitem = -1;

	if((sitem = m_pListCtrl.GetNextItem(-1,LVNI_SELECTED)) == -1)		
		return;

	//20080227 실행중 순서 변경, jyh------------------------------------------------
	if(m_pPtrTrans)
	{
		if(sitem == m_nCurTransFile && m_pPtrTrans->nTransState == ITEM_STATE_TRANS)
		{
			MSG_Window((WPARAM)"전송중인 파일은 이동할 수 없습니다!\n전송 중지 후 이동 하세요!", 1);
			return;
		}
		else if(sitem < m_nCurTransFile)
		{
			MSG_Window((WPARAM)"전송 파일 이전 항목은 이동할 수 없습니다!\n전송 중지 후 이동 하세요!", 1);
			return;
		}

		// 한칸위로
		if(mode == MOVE_MU)
		{
			if(sitem - m_nCurTransFile == 1)
			{
				MSG_Window((WPARAM)"전송중인 파일 위로는 이동할 수 없습니다!\n전송 중지 후 이동 하세요!", 1);
				return;
			}
		}
	}
	//---------------------------------------------------------------------------------

	int index = 0;
	int oldrealitem;


	EnterCriticalSection(&m_cs);


	while(1)
	{			

		if(sitem == -1 || sitem == 0 && mode == MOVE_MF || sitem == 0 && mode == MOVE_MU || 
			sitem == m_pListCtrl.GetItemCount()-1 && mode == MOVE_ML || sitem == m_pListCtrl.GetItemCount()-1 && mode == MOVE_MD)		
			break;

		if(index != 0 && sitem == firstmoveitem)
			break;
		
		LVITEM lviimg;
		lviimg.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lviimg.iItem = sitem;
		lviimg.iSubItem = 0;
		lviimg.stateMask = 0xFFFF;		// get all state flags
		m_pListCtrl.GetItem(&lviimg);

		LV_ITEM lvi;	
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;	

		switch(mode)
		{
			case MOVE_MF:
				lvi.iItem = index;
				break;
			case MOVE_MU:
				lvi.iItem = sitem - 1;
				break;
			case MOVE_ML:
				lvi.iItem = m_pListCtrl.GetItemCount();
				break;
			case MOVE_MD:
				lvi.iItem = sitem + 1 + index + m_pListCtrl.GetSelectedCount();
				break;
		}	
			
		PRUPCTX newitem = new RUPCTX;
		ZeroMemory(newitem, sizeof(newitem));
		memcpy(newitem, (PRUPCTX)m_pListCtrl.GetItemData(sitem), sizeof(RUPCTX));
		
		switch(mode)
		{
			case MOVE_MF:
				oldrealitem = sitem+1;
				break;
			case MOVE_MU:
				oldrealitem = sitem+1;
				break;
			case MOVE_ML:
				oldrealitem = sitem;
				break;
			case MOVE_MD:
				oldrealitem = sitem;
				break;
		}	
		
		
		lvi.iSubItem = 0;
		lvi.iImage = lviimg.iImage;
		CString sndt = m_pListCtrl.GetItemText(sitem, 0);
		lvi.pszText =  (LPSTR)(LPCTSTR)sndt; 						
		
		int iItem = m_pListCtrl.InsertItem(&lvi);

		m_pListCtrl.SetItemText(iItem, 1, m_pListCtrl.GetItemText(oldrealitem, 1));
		m_pListCtrl.SetItemText(iItem, 2, m_pListCtrl.GetItemText(oldrealitem, 2));
		m_pListCtrl.SetItemText(iItem, 3, m_pListCtrl.GetItemText(oldrealitem, 3));
		//m_pListCtrl.SetItemText(iItem, 4, m_pListCtrl.GetItemText(oldrealitem, 4));
		//m_pListCtrl.SetItemText(iItem, 5, m_pListCtrl.GetItemText(oldrealitem, 5));
		m_pListCtrl.SetItemData(iItem, (DWORD_PTR)newitem);
		
		chitem[index] = (DWORD_PTR)newitem;
		
		// 이동하기 전의 아이템 삭제
		m_pListCtrl.DeleteItem(oldrealitem);

		index++;
		sitem = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED);
	}

	for(int i = 0 ; i < 100 ; i++)
	{
		if(chitem[i] == -1)
			break;
		else
		{
			for(int j = 0 ; j < m_pListCtrl.GetItemCount() ; j++)
			{
				if(m_pListCtrl.GetItemData(j) == chitem[i])
				{
					m_pListCtrl.SetItemState(j, LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
					break;
				}
			}
		}
	}	

	LeaveCriticalSection(&m_cs);	
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 아이템 삭제 : 모드별
// mode
// 0 : 전체 삭제
// 1 : 선택 목록 삭제
// 2 : 완료 목록 삭제
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::DeleteList(int mode)
{
	int sitem;	

	EnterCriticalSection(&m_cs);

	switch(mode)
	{
		case 0: 

			for(int i = 0; i < m_pListCtrl.GetItemCount() ; i++)
				delete (PRUPCTX)m_pListCtrl.GetItemData(i);


			m_pListCtrl.DeleteAllItems();
			m_nFileSizeTotal = 0;
			m_nFileCount = 0;
			break;

		case 1:

			//if(!m_bFileTrans) 20080227 주석 처리, jyh
			//{
				if((m_pListCtrl.GetNextItem(-1,LVNI_SELECTED)) == -1)		
				{
					LeaveCriticalSection(&m_cs);	
					return;
				}

				while(1)
				{
					if((sitem = m_pListCtrl.GetNextItem(-1,LVNI_SELECTED)) == -1)		
						break;
					//20080227 전송중인 파일인지 체크, jyh
					else if(sitem == m_nCurTransFile)
					{
						if(m_bFileTrans)
						{
							MSG_Window((WPARAM)"전송중인 파일은 삭제할 수 없습니다!\n전송 중지 후 삭제 하세요!", 1);
							LeaveCriticalSection(&m_cs);	
							return;
						}						
					}
					//20080228 전송중인 파일보다 인덱스가 작을때만 전송중인 인덱스도 같이 빼준다, jyh
					else if(sitem < m_nCurTransFile)
						m_nCurTransFile--;

					m_nFileCount--;
					m_nFileSizeTotal -= ((PRUPCTX)m_pListCtrl.GetItemData(sitem))->nFileSizeReal;
					delete (PRUPCTX)m_pListCtrl.GetItemData(sitem);
					m_pListCtrl.DeleteItem(sitem);				
				}
			//}
			break;

		case 2:		

			for(int i = m_pListCtrl.GetItemCount()-1; i >= 0  ; i--)
			{
				if(((PRUPCTX)m_pListCtrl.GetItemData(i))->nTransState == ITEM_STATE_COMPLETE)
				{
					//20080228 전송중인 파일보다 인덱스가 작을때만 전송중인 인덱스도 같이 빼준다, jyh
					if(i < m_nCurTransFile)
						m_nCurTransFile--;

					m_nFileCount--;
					m_nFileSizeTotal -= ((PRUPCTX)m_pListCtrl.GetItemData(i))->nFileSizeReal;
					delete (PRUPCTX)m_pListCtrl.GetItemData(i);
					m_pListCtrl.DeleteItem(i);
				}
			}		
			break;
	}

	LeaveCriticalSection(&m_cs);	

	// 전송 파일 갯수 변경
	ReSetTransInfo();
}

//20071030 파일 상태 반환, jyh
CString up_window::GetFileStatus(int nIdx)
{
	CString strBuf;

	switch(nIdx)
	{
	case ITEM_STATE_WAIT:
		strBuf.Format("대기");
		break;
	case ITEM_STATE_TRANS:
		strBuf.Format("전송중");
		break;
	case ITEM_STATE_STOP:
		strBuf.Format("중지");
		break;
	case ITEM_STATE_CANCEL:
		strBuf.Format("취소");
		break;
	case ITEM_STATE_ERROR:
		strBuf.Format("실패");
		break;
	case ITEM_STATE_COMPLETE:
		strBuf.Format("완료");
		break;
	case ITEM_STATE_CONNECT:
		strBuf.Format("연결중");
		break;
	case ITEM_STATE_DEFER:
		strBuf.Format("사용자 초과");
		break;
	//20081027 추가, jyh
	case ITEM_STATE_DELAY:
		strBuf.Format("전송 실패");
		break;
	case ITEM_STATE_CONFAILED:
		strBuf.Format("서버접속실패");
		break;
		
	case ITEM_STATE_COPYRIGHT:
		strBuf.Format("저작권제한");
		break;
	case ITEM_STATE_VIRUSFILE:
		strBuf.Format("악성코드제한");
		break;
	case ITEM_STATE_BLOCKFILE:
		strBuf.Format("유해제한");
		break;		
		
	}

	return strBuf;
}

//20090303 사용자의 익스플로러 버전 알아오는 함수, jyh
int	up_window::GetVersionIE()
{
	int nVerIE = 0;
	// 버전정보를 담을 버퍼
	TCHAR* buffer = NULL;
	// 버전을 확인할 파일
	CString strIEPath;
	TCHAR tszTemp[1024];

	SHGetSpecialFolderPath(NULL, tszTemp, CSIDL_PROGRAM_FILES, FALSE);
	strIEPath.Format("%s\\Internet Explorer\\iexplore.exe", tszTemp);

	DWORD infoSize = 0;

	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
	infoSize = GetFileVersionInfoSize(strIEPath, 0);

	if(infoSize == 0)
		return 0;

	// 버퍼할당
	buffer = new TCHAR[infoSize];

	if(buffer)
	{
		// 버전정보데이터를 가져옵니다.
		if(GetFileVersionInfo(strIEPath, 0, infoSize, buffer) != 0)
		{
			VS_FIXEDFILEINFO* pFineInfo = NULL;
			UINT bufLen = 0;

			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
			if(VerQueryValue(buffer, "\\", (LPVOID*)&pFineInfo, &bufLen) != 0)
			{    
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				nVerIE = majorVer;
			}
		}

		delete[] buffer;
	}

	return nVerIE;
}