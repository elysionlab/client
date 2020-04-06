#include "stdafx.h"
#include "down_window.h"
#include "shlwapi.h"
#include "Dlg_Setup.h"
#include "Dlg_Exist.h"
#include "Dlg_Confirm.h"
#include "SelectFolderDialog.h"




bool down_window::Skin_ButtonPressed(CString m_ButtonName){return true;}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 타겟 디스크의 잔여용량 체크
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
__int64 down_window::GetDiskFreeSpaceMMSV(CString targetfolder)
{
	ULARGE_INTEGER  dwSPC;
	ULARGE_INTEGER  dwBPS;
	ULARGE_INTEGER  dwFC;

	CString targetdisk;
	targetdisk = targetfolder.Left(targetfolder.Find("\\")+1);
	GetDiskFreeSpaceEx(targetdisk, &dwSPC, &dwBPS, &dwFC);
	return (__int64)dwFC.QuadPart;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 전송중인 아이템 인덱스.. 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int  down_window::GetTransListIndex()
{
	EnterCriticalSection(&m_cs);

	if(m_bFileTrans && m_pPtrTrans != NULL)
	{
		for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
		{
			if((PRDOWNCTX)m_pListCtrl.GetItemData(nIndex) == m_pPtrTrans)
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
// 전송실패 아이템 순위변경
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DWORD  down_window::DeferItem()
{
	EnterCriticalSection(&m_cs);

	PRDOWNCTX pPtrCheck = NULL;
	PRDOWNCTX pPtrCurrCheck = NULL;	//20071119 jyh
	int nCnt = 0;					//20071119 같은 게시물의 파일 수, jyh	
	int nItemCheck = 0;
	int nCountPoss = 0;
	int nIndexCurrent = m_nCurTransFile;
	if(nIndexCurrent < 0) return -1;
	
	for(int nIndex = nIndexCurrent+1; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
	{
		pPtrCheck = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndex);
		pPtrCurrCheck = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndexCurrent);	//20071119 jyh

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

		//20071010 컨텐츠 단위로 이동을 위해 파일의 게시물 인덱스 번호를 비교, jyh
		/*if(pPtrCurrCheck->nBoardIndex == pPtrCheck->nBoardIndex)
		{
			pPtrCheck->nTransState = pPtrCurrCheck->nTransState;
			m_pListCtrl.SetItemText(nIndex, 2, GetFileStatus(pPtrCheck->nTransState));
			
			nCnt++;
		}*/
	}	

	LeaveCriticalSection(&m_cs);
	
	////20071010 같은 게시물 수 만큼 이동(현재 파일까지 nCnt+1), jyh
	//for(int i=0; i<nCnt+1; i++)
	//{
	//	MoveItem(MOVE_ML);
	//	// 이전 선택된 모든 아이템 선택해제
	//	//20080320 다운 상태 이미지 없어지는 버그 수정, jyh
	//	while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
	//		m_pListCtrl.SetItemState(nItemCheck, 0, -1);
	//	
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
void down_window::InsertItem(PRDOWNCTX pNewItem)
{
	int nInsertPos = -1;
	int nItem = -1;
	SHFILEINFO sfi;	
	PRDOWNCTX pDownCtxTmp = NULL;
	CString strBuf, strBufSub, strFileType, strNewPath;

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

			// 자막파일이면 동영상 파일이 존재하는지 체크후 이전으로 이동			
			if(strBuf.Right(4) == _T(".smi"))
			{
				try
				{
					for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
					{
						pDownCtxTmp = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndex);

						if(pDownCtxTmp)
						{
							strBufSub = pDownCtxTmp->pFullPath;
						
							if((strBufSub.Right(4) == _T(".avi") || strBufSub.Right(4) == _T(".asf") || strBufSub.Right(4) == _T(".wmv"))
								&& strBuf.GetLength() == strBufSub.GetLength() && strBuf.Left(strBuf.GetLength() - 4) == strBufSub.Left(strBufSub.GetLength() - 4))
							{
								nInsertPos = nIndex;
								break;
							}
						}						
					}
				}
				catch(...)
				{
				}
			}

			if(nInsertPos == -1)
				nInsertPos = m_pListCtrl.GetItemCount();

			//20080319 다운 상태 이미지, jyh
			nItem = m_pListCtrl.InsertItem(nInsertPos, pNewItem->pFileName, sfi.iIcon);
			m_pListCtrl.SetItemText(nItem, 0, "");
			m_pListCtrl.SetItemText(nItem, 1, pNewItem->pFileName);
			// 파일 크기	
			strBuf.Format("%s/%s", SetSizeFormat(pNewItem->nFileSizeEnd), SetSizeFormat(pNewItem->nFileSizeReal));
			m_pListCtrl.SetItemText(nItem, 2, strBuf);

			CString strStatus = GetFileStatus(pNewItem->nTransState);
			// 전송 상태
			m_pListCtrl.SetItemText(nItem, 3, strStatus);
		
			// 전송 속도
			//m_pListCtrl.SetItemText(nItem, 3, "");
			// 남은 시간
			//m_pListCtrl.SetItemText(nItem, 4, "00:00:00");

			// 저장 폴더
			//m_pListCtrl.SetItemText(nItem, 5, pNewItem->pFolderName);

			m_pListCtrl.SetItemData(nItem, (DWORD_PTR)pNewItem);

			m_pListCtrl.SetRedraw(TRUE);

		}
		LeaveCriticalSection(&m_cs);	
	}


	// 전송 파일 갯수 변경
	ReSetTransInfo();
}


LRESULT down_window::MSG_Window(WPARAM wParam, LPARAM lParam)
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

LRESULT down_window::MSG_DownLoad(WPARAM wParam, LPARAM lParam)
{
	DownLoadPro();				
	return 0;
}

LRESULT down_window::MSG_OverWrite(WPARAM wParam, LPARAM lParam)
{
	__int64 nTotalSize = *(__int64*)lParam;

	Dlg_Exist pWindow;
	pWindow.m_pInfo_FilePath	= (char*)wParam;
	pWindow.m_pInfo_Size		= SetSizeFormat(nTotalSize);
	pWindow.DoModal();	

	// 항상적용이면 변수적용
	if(pWindow.m_nAllApply)
	{
		m_rDownOverWrite = pWindow.m_nResult;
		m_pReg.SaveKey(CLIENT_REG_PATH, "rDownOverWrite",	m_rDownOverWrite);
	}

	return pWindow.m_nResult;
}


LRESULT down_window::MSG_ForceExit(WPARAM wParam, LPARAM lParam)
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
void down_window::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->iItem != -1)
	{
		PRDOWNCTX pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(pNMLV->iItem);
		
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
CString down_window::SetSizeFormat(__int64 nRealSize)
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
CString down_window::SetNumFormat(__int64 nNum)
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
CString down_window::SetSpeedFormatKGrid(double tranSpeed,DWORD nowsec, DWORD startsec)
{
	CString bSize;

	__int64	time;
	float bonus = 1;


	if(tranSpeed < 1024)
	{
		bSize.Format("%.1fKB/sec",tranSpeed);
	}
	else if(tranSpeed > 1024 && tranSpeed < 20240)
		bSize.Format("%.2fMB/sec",tranSpeed/1024);


	return bSize;

}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 현재 파일의 전송 속도 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString down_window::SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec)
{
	// 10Mbps=10000kbps=1250kB/s
	// 1Mbps=1,000kbps=128,000Byte/s
	
	CString bSize;
	__int64 size = transsize;
	__int64	time;
	float bonus = (float)1.1;

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
void down_window::MoveItem(int mode)
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

		//20080320 파일 순서 변경시 버그 수정, jyh
		if(index > m_pListCtrl.GetItemCount()-1)
			break;
		
		LVITEM lviimg;
		lviimg.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lviimg.iItem = sitem;
		lviimg.iSubItem = 0;
		lviimg.stateMask = 0xFFFF;		// get all state flags
		m_pListCtrl.GetItem(&lviimg);

		LV_ITEM lvi;
		//20080320 다운 상태 이미지 버그 수정, jyh
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM ;
		
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
			
		PRDOWNCTX newitem = new RDOWNCTX;
		memcpy(newitem, (PRDOWNCTX)m_pListCtrl.GetItemData(sitem), sizeof(RDOWNCTX));
		
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
		//20080320 다운 상태 이미지 버그 수정, jyh
		lvi.state = lviimg.state;
		lvi.stateMask = lviimg.stateMask;
        		
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
void down_window::DeleteList(int mode)
{
	int sitem;	

	EnterCriticalSection(&m_cs);

	switch(mode)
	{
		case 0: 

			for(int i = 0; i < m_pListCtrl.GetItemCount() ; i++)
				delete (PRDOWNCTX)m_pListCtrl.GetItemData(i);

			m_pListCtrl.DeleteAllItems();
			m_nFileSizeTotal = 0;
			m_nFileSizeEnd = 0;
			m_nFileCount = 0;
			break;

		case 1:

			//if(!m_bFileTrans) 20080227 주석 처리, jyh
			//{
				if((m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) == -1)		
				{
					LeaveCriticalSection(&m_cs);	
					return;
				}

				while(1)
				{
					if((sitem = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) == -1)		
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
					m_nFileSizeTotal -= ((PRDOWNCTX)m_pListCtrl.GetItemData(sitem))->nFileSizeReal;
					m_nFileSizeEnd -= ((PRDOWNCTX)m_pListCtrl.GetItemData(sitem))->nFileSizeEnd; //20090217 수정, jyh
					delete (PRDOWNCTX)m_pListCtrl.GetItemData(sitem);
					m_pListCtrl.DeleteItem(sitem);				
				}
			//}
			break;

		case 2:		

			for(int i = m_pListCtrl.GetItemCount()-1; i >= 0  ; i--)
			{
				if(((PRDOWNCTX)m_pListCtrl.GetItemData(i))->nTransState == ITEM_STATE_COMPLETE)				
				{
					//20080228 전송중인 파일보다 인덱스가 작을때만 전송중인 인덱스도 같이 빼준다, jyh
					if(i < m_nCurTransFile)
                        m_nCurTransFile--;

					m_nFileCount--;
					m_nFileSizeTotal -= ((PRDOWNCTX)m_pListCtrl.GetItemData(i))->nFileSizeReal;
					m_nFileSizeEnd -= ((PRDOWNCTX)m_pListCtrl.GetItemData(i))->nFileSizeEnd;	//20090217 수정, jyh
					delete (PRDOWNCTX)m_pListCtrl.GetItemData(i);
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
CString down_window::GetFileStatus(int nIdx)
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
	//20080311 무료다운 불가, jyh
	case ITEM_STATE_NOFREEDOWN:
		strBuf.Format("무료다운 불가");
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
	}

	return strBuf;
}

