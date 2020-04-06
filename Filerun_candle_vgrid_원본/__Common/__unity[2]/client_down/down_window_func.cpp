#include "stdafx.h"
#include "down_window.h"
#include "shlwapi.h"
#include "Dlg_Setup.h"
#include "Dlg_Exist.h"
#include "Dlg_Confirm.h"
#include "SelectFolderDialog.h"




bool down_window::Skin_ButtonPressed(CString m_ButtonName){return true;}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ÿ�� ��ũ�� �ܿ��뷮 üũ
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
// �������� ������ �ε���.. 
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
// ���۽��� ������ ��������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DWORD  down_window::DeferItem()
{
	EnterCriticalSection(&m_cs);

	PRDOWNCTX pPtrCheck = NULL;
	PRDOWNCTX pPtrCurrCheck = NULL;	//20071119 jyh
	int nCnt = 0;					//20071119 ���� �Խù��� ���� ��, jyh	
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
			//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20081027 �߰�, jyh*/ ||
			//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20081027 �߰�, jyh*/ ||
			//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 �߰�, jyh*/)
		{
			nCountPoss++;
		}

		//20071010 ������ ������ �̵��� ���� ������ �Խù� �ε��� ��ȣ�� ��, jyh
		/*if(pPtrCurrCheck->nBoardIndex == pPtrCheck->nBoardIndex)
		{
			pPtrCheck->nTransState = pPtrCurrCheck->nTransState;
			m_pListCtrl.SetItemText(nIndex, 2, GetFileStatus(pPtrCheck->nTransState));
			
			nCnt++;
		}*/
	}	

	LeaveCriticalSection(&m_cs);
	
	////20071010 ���� �Խù� �� ��ŭ �̵�(���� ���ϱ��� nCnt+1), jyh
	//for(int i=0; i<nCnt+1; i++)
	//{
	//	MoveItem(MOVE_ML);
	//	// ���� ���õ� ��� ������ ��������
	//	//20080320 �ٿ� ���� �̹��� �������� ���� ����, jyh
	//	while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
	//		m_pListCtrl.SetItemState(nItemCheck, 0, -1);
	//	
	//	// ���۵� ������ ��Ŀ���� ���õ� ���·� ����
	//	m_pListCtrl.SetItemState(nIndexCurrent, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
	//	m_pListCtrl.EnsureVisible(nIndexCurrent, TRUE);
	//}

	//return nCountPoss - nCnt;	//20071219 ���� 0�̸� ���� �������� ����(������ ������ ������ -nCnt�� ����� ��), jyh
	return nCountPoss;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ����Ʈ ��Ʈ�ѿ� ������ ����
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

			// �ڸ������̸� ������ ������ �����ϴ��� üũ�� �������� �̵�			
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

			//20080319 �ٿ� ���� �̹���, jyh
			nItem = m_pListCtrl.InsertItem(nInsertPos, pNewItem->pFileName, sfi.iIcon);
			m_pListCtrl.SetItemText(nItem, 0, "");
			m_pListCtrl.SetItemText(nItem, 1, pNewItem->pFileName);
			// ���� ũ��	
			strBuf.Format("%s/%s", SetSizeFormat(pNewItem->nFileSizeEnd), SetSizeFormat(pNewItem->nFileSizeReal));
			m_pListCtrl.SetItemText(nItem, 2, strBuf);

			CString strStatus = GetFileStatus(pNewItem->nTransState);
			// ���� ����
			m_pListCtrl.SetItemText(nItem, 3, strStatus);
		
			// ���� �ӵ�
			//m_pListCtrl.SetItemText(nItem, 3, "");
			// ���� �ð�
			//m_pListCtrl.SetItemText(nItem, 4, "00:00:00");

			// ���� ����
			//m_pListCtrl.SetItemText(nItem, 5, pNewItem->pFolderName);

			m_pListCtrl.SetItemData(nItem, (DWORD_PTR)pNewItem);

			m_pListCtrl.SetRedraw(TRUE);

		}
		LeaveCriticalSection(&m_cs);	
	}


	// ���� ���� ���� ����
	ReSetTransInfo();
}


LRESULT down_window::MSG_Window(WPARAM wParam, LPARAM lParam)
{
	Dlg_Alert pWindow;
	m_pAlert = &pWindow;
	pWindow.m_pInfo = (char*)wParam;
	pWindow.m_bAutoClose = (BOOL)lParam;
	//pWindow.DoModal();
	//20071119 ����, jyh
	int ret = (int)pWindow.DoModal();

	if(ret == IDRETRY)			//��õ�
		m_nBtnState = BTN_STATE_RETRY;
	else if(ret == IDIGNORE)	//�ǳʶٱ�
		m_nBtnState = BTN_STATE_JUMP;
	else if(ret == IDYES)		//��
		m_nBtnState = BTN_STATE_YES;
	else if(ret == IDNO)		//�ƴϿ�
		m_nBtnState = BTN_STATE_NO;
	else if(ret == IDCANCEL)	//���
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

	// �׻������̸� ��������
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
// �������� ����Ŭ�� ������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void down_window::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->iItem != -1)
	{
		PRDOWNCTX pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(pNMLV->iItem);
		
		// ������ġ�� Ŭ���� ��� �ش� ���� ����
		if(pNMLV->iSubItem == 5)
			ShellExecute(m_hWnd, "explore", pItemData->pFolderName, NULL, NULL, SW_SHOWNORMAL);	
		// �������� ����
		else
			ShellExecute(m_hWnd, NULL, pItemData->pFullPath, NULL, NULL, SW_SHOWNORMAL);	
	}
	
	*pResult = 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���� ����� ��� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString down_window::SetSizeFormat(__int64 nRealSize)
{	
	char pTemp[256];
	::StrFormatByteSize64(nRealSize, pTemp, 256);	
	CString bSize = pTemp;
	bSize.Replace("����Ʈ", "B");	
    return bSize;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���� ����
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
// ���� ������ ���� �ӵ� 
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
// ���� ������ ���� �ӵ� 
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

		// �����ð� ����� ���� ������ �ӵ�
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
// ������ �̵�
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

	//20080227 ������ ���� ����, jyh------------------------------------------------
	if(m_pPtrTrans)
	{
		if(sitem == m_nCurTransFile && m_pPtrTrans->nTransState == ITEM_STATE_TRANS)
		{
			MSG_Window((WPARAM)"�������� ������ �̵��� �� �����ϴ�!\n���� ���� �� �̵� �ϼ���!", 1);
			return;
		}
		else if(sitem < m_nCurTransFile)
		{
			MSG_Window((WPARAM)"���� ���� ���� �׸��� �̵��� �� �����ϴ�!\n���� ���� �� �̵� �ϼ���!", 1);
			return;
		}

		// ��ĭ����
		if(mode == MOVE_MU)
		{
			if(sitem - m_nCurTransFile == 1)
			{
				MSG_Window((WPARAM)"�������� ���� ���δ� �̵��� �� �����ϴ�!\n���� ���� �� �̵� �ϼ���!", 1);
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

		//20080320 ���� ���� ����� ���� ����, jyh
		if(index > m_pListCtrl.GetItemCount()-1)
			break;
		
		LVITEM lviimg;
		lviimg.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		lviimg.iItem = sitem;
		lviimg.iSubItem = 0;
		lviimg.stateMask = 0xFFFF;		// get all state flags
		m_pListCtrl.GetItem(&lviimg);

		LV_ITEM lvi;
		//20080320 �ٿ� ���� �̹��� ���� ����, jyh
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
		//20080320 �ٿ� ���� �̹��� ���� ����, jyh
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
		
		// �̵��ϱ� ���� ������ ����
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
// ������ ���� : ��庰
// mode
// 0 : ��ü ����
// 1 : ���� ��� ����
// 2 : �Ϸ� ��� ����
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

			//if(!m_bFileTrans) 20080227 �ּ� ó��, jyh
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
					//20080227 �������� �������� üũ, jyh
					else if(sitem == m_nCurTransFile)
					{
						if(m_bFileTrans)
						{
							MSG_Window((WPARAM)"�������� ������ ������ �� �����ϴ�!\n���� ���� �� ���� �ϼ���!", 1);
							LeaveCriticalSection(&m_cs);	
							return;
						}
					}
					//20080228 �������� ���Ϻ��� �ε����� �������� �������� �ε����� ���� ���ش�, jyh
					else if(sitem < m_nCurTransFile)
						m_nCurTransFile--;

					m_nFileCount--;
					m_nFileSizeTotal -= ((PRDOWNCTX)m_pListCtrl.GetItemData(sitem))->nFileSizeReal;
					m_nFileSizeEnd -= ((PRDOWNCTX)m_pListCtrl.GetItemData(sitem))->nFileSizeEnd; //20090217 ����, jyh
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
					//20080228 �������� ���Ϻ��� �ε����� �������� �������� �ε����� ���� ���ش�, jyh
					if(i < m_nCurTransFile)
                        m_nCurTransFile--;

					m_nFileCount--;
					m_nFileSizeTotal -= ((PRDOWNCTX)m_pListCtrl.GetItemData(i))->nFileSizeReal;
					m_nFileSizeEnd -= ((PRDOWNCTX)m_pListCtrl.GetItemData(i))->nFileSizeEnd;	//20090217 ����, jyh
					delete (PRDOWNCTX)m_pListCtrl.GetItemData(i);
					m_pListCtrl.DeleteItem(i);
				}
			}		
			break;
	}

	LeaveCriticalSection(&m_cs);	

	// ���� ���� ���� ����
	ReSetTransInfo();
}

//20071030 ���� ���� ��ȯ, jyh
CString down_window::GetFileStatus(int nIdx)
{
	CString strBuf;
	
	switch(nIdx)
	{
	case ITEM_STATE_WAIT:
		strBuf.Format("���");
		break;
	case ITEM_STATE_TRANS:
		strBuf.Format("������");
		break;
	case ITEM_STATE_STOP:
		strBuf.Format("����");
		break;
	case ITEM_STATE_CANCEL:
		strBuf.Format("���");
		break;
	case ITEM_STATE_ERROR:
		strBuf.Format("����");
		break;
	case ITEM_STATE_COMPLETE:
		strBuf.Format("�Ϸ�");
		break;
	case ITEM_STATE_CONNECT:
		strBuf.Format("������");
		break;
	case ITEM_STATE_DEFER:
		strBuf.Format("����� �ʰ�");
		break;
	//20080311 ����ٿ� �Ұ�, jyh
	case ITEM_STATE_NOFREEDOWN:
		strBuf.Format("����ٿ� �Ұ�");
		break;
		//20081027 �߰�, jyh
	case ITEM_STATE_DELAY:
		strBuf.Format("���� ����");
		break;
	case ITEM_STATE_CONFAILED:
		strBuf.Format("�������ӽ���");
		break;
		
	case ITEM_STATE_COPYRIGHT:
		strBuf.Format("���۱�����");
		break;		
	}

	return strBuf;
}

