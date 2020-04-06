#include "stdafx.h"
#include "up_window.h"
#include "shlwapi.h"
#include "Dlg_Setup.h"
#include "Dlg_Exist.h"
#include "Dlg_Confirm.h"

#pragma comment(lib, "version.lib")


bool up_window::Skin_ButtonPressed(CString m_ButtonName){return true;}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �������� ������ �ε���.. 
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
// 20071219 ���۽��� ������ ��������, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DWORD  up_window::DeferItem()
{
	EnterCriticalSection(&m_cs);

	PRUPCTX pPtrCheck = NULL;
	PRUPCTX pPtrCurrCheck = NULL;	//20071119 jyh
	int nCnt = 0;					//20071119 ���� �Խù��� ���� ��, jyh	
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
			//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20081027 �߰�, jyh*/ ||
			//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20081027 �߰�, jyh*/ ||
			//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 �߰�, jyh*/)
		{
			nCountPoss++;
		}

		////20071010 ������ ������ �̵��� ���� ������ �Խù� �ε��� ��ȣ�� ��, jyh
		//if(pPtrCurrCheck->nBoardIndex == pPtrCheck->nBoardIndex)
		//{
		//	pPtrCheck->nTransState = pPtrCurrCheck->nTransState;
		//	m_pListCtrl.SetItemText(nIndex, 2, GetFileStatus(pPtrCheck->nTransState));
		//	
		//	nCnt++;
		//}
	}	

	LeaveCriticalSection(&m_cs);	

	////20071010 ���� �Խù� �� ��ŭ �̵�(���� ���ϱ��� nCnt+1), jyh
	//for(int i=0; i<nCnt+1; i++)
	//{
	//	MoveItem(MOVE_ML);
	//	// ���� ���õ� ��� ������ ��������
	//	while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
	//		m_pListCtrl.SetItemState(nItemCheck, 0, -1);

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

			// ���� ũ��	
			strBuf.Format("%s/%s", SetSizeFormat(pNewItem->nFileSizeEnd), SetSizeFormat(pNewItem->nFileSizeReal));
			m_pListCtrl.SetItemText(nItem, 1, strBuf);

			CString strStatus = GetFileStatus(pNewItem->nTransState);
			// ���� ����
			m_pListCtrl.SetItemText(nItem, 2, strStatus);
			//// ���� �ӵ�
			//m_pListCtrl.SetItemText(nItem, 3, "");
			//// ���� �ð�
			//m_pListCtrl.SetItemText(nItem, 4, "00:00:00");

			//// ���� ����
			//m_pListCtrl.SetItemText(nItem, 5, pNewItem->pFolderName);

			m_pListCtrl.SetItemData(nItem, (DWORD_PTR)pNewItem);

			m_pListCtrl.SetRedraw(TRUE);

		}
		LeaveCriticalSection(&m_cs);	
	}


	// ���� ���� ���� ����
	ReSetTransInfo();
}

//20090304 ������ ���ε�Ϸ� �뺸, jyh
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

	// �׻������̸� ��������
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
// �������� ����Ŭ�� ������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->iItem != -1)
	{
		PRUPCTX pItemData = (PRUPCTX)m_pListCtrl.GetItemData(pNMLV->iItem);
		
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
CString up_window::SetSizeFormat(__int64 nRealSize)
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
// ���� ������ ���� �ӵ� 
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

			//if(!m_bFileTrans) 20080227 �ּ� ó��, jyh
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
					//20080228 �������� ���Ϻ��� �ε����� �������� �������� �ε����� ���� ���ش�, jyh
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

	// ���� ���� ���� ����
	ReSetTransInfo();
}

//20071030 ���� ���� ��ȯ, jyh
CString up_window::GetFileStatus(int nIdx)
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
	case ITEM_STATE_VIRUSFILE:
		strBuf.Format("�Ǽ��ڵ�����");
		break;
	case ITEM_STATE_BLOCKFILE:
		strBuf.Format("��������");
		break;		
		
	}

	return strBuf;
}

//20090303 ������� �ͽ��÷η� ���� �˾ƿ��� �Լ�, jyh
int	up_window::GetVersionIE()
{
	int nVerIE = 0;
	// ���������� ���� ����
	TCHAR* buffer = NULL;
	// ������ Ȯ���� ����
	CString strIEPath;
	TCHAR tszTemp[1024];

	SHGetSpecialFolderPath(NULL, tszTemp, CSIDL_PROGRAM_FILES, FALSE);
	strIEPath.Format("%s\\Internet Explorer\\iexplore.exe", tszTemp);

	DWORD infoSize = 0;

	// ���Ϸκ��� ���������������� ũ�Ⱑ �������� ���մϴ�.
	infoSize = GetFileVersionInfoSize(strIEPath, 0);

	if(infoSize == 0)
		return 0;

	// �����Ҵ�
	buffer = new TCHAR[infoSize];

	if(buffer)
	{
		// �������������͸� �����ɴϴ�.
		if(GetFileVersionInfo(strIEPath, 0, infoSize, buffer) != 0)
		{
			VS_FIXEDFILEINFO* pFineInfo = NULL;
			UINT bufLen = 0;

			// buffer�� ���� VS_FIXEDFILEINFO ������ �����ɴϴ�.
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