#include "stdafx.h"
#include "down_window.h"
#include "FtpClient.h"
#include "Dlg_Slide.h"
#include <sys/stat.h>

#pragma warning(disable:4244)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���� ���� ���� ����
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
		// ��ü ���� üũ
		for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount(); nIndex++)
		{
			if((pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(nIndex)) != 0)
			{
				if(	pItemData->nTransState == ITEM_STATE_WAIT ||
					pItemData->nTransState == ITEM_STATE_TRANS ||
					pItemData->nTransState == ITEM_STATE_STOP ||
					pItemData->nTransState == ITEM_STATE_COMPLETE)
				{
					//20090217 ����, jyh
					//nFileSizeEnd	+= pItemData->nFileSizeEnd;
					//m_nFileSizeEnd = nFileSizeEnd;
					nFileSizeEnd	+= pItemData->nFileSizeEnd;				
				}

				if(	pItemData->nTransState == ITEM_STATE_COMPLETE)
					nFileCountEnd++;
			}
		}
	}
	
	//nFileSizeEnd = m_nFileSizeEnd;	//20090217 ����, jyh

	// ��������	
	__int64 nFileCountEnd_ = m_pFtpClint->m_bFlag ? nFileCountEnd+1 : nFileCountEnd;
	//strBuf.Format("%I64d / %I64d (%s / %s)", nFileCountEnd_, m_nFileCount, SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal));
	//m_pStatic_FileCount.SetWindowText(strBuf);


	m_nTime = GetTickCount();

	// ��ü ���� �ð� ����
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

	// ����ð� ����	
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

	// �������� ���� ����
	if((nTransIndex = GetTransListIndex()) != -1)
	{
		try
		{
			pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(nTransIndex);
			
			if(lstrcmp(m_strPrevFileName.GetBuffer(), pItemData->pFileName))
			{
				//20070919 ���� ����, jyh
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

				strBuf.Format("%s'�ٿ�ε� ��'", strtemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
			}

			m_strPrevFileName.ReleaseBuffer();

			//20070919 ���� ���, jyh
			if(lstrcmp(m_strPrevPath.GetBuffer(), pItemData->pFolderName))
			{
				char sPath[4096];

				strcpy_s(sPath, strlen(pItemData->pFolderName)+1, pItemData->pFolderName);
				m_strPrevPath = pItemData->pFolderName;

				int nIdx = (int)strlen(sPath);
				sPath[nIdx-1] = 0;
				m_pStatic_Path.SetWindowText(sPath);	//20070919 ���� ���, jyh
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
				//�ӵ� ���� ����(����)
				//strSpeed = SetSpeedFormat(pItemData->nFileSizeEnd - pItemData->nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);
				//120%�� ǥ��
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

					// �����ð� ����� ���� ������ �ӵ�
					m_nTransSpeed = (pItemData->nFileSizeEnd - pItemData->nFileSizeStart * 1 ) / time;
				}
				else
				{
					m_nTransSpeed = pItemData->nFileSizeEnd - pItemData->nFileSizeStart;
				}


				//20070919 �ӵ� ���, jyh
				m_pStatic_Speed.SetWindowText(strSpeed);
				//20070919 jyh
				RECT rt = { 423, 54, 490, 66 };
				InvalidateRect(&rt);

				// ���� �ӵ� ������Ʈ
				if((m_nTime - m_nTimeFileSpeed) > 3000  || (m_nTransSpeed_Last < m_nTransSpeed && (m_nTime - m_nTimeFileSpeed) > 1000))
				{
					//m_pListCtrl.SetItemText(nTransIndex, 2, strSpeed);	20070918 �ּ� ó��, jyh

					if(m_nTransSpeed != 0)
					{
						m_nTimeFileSpeed	= m_nTime;
						m_nTransSpeed_Last	= m_nTransSpeed;
					}
				}
				else
				{
					//strSpeed = m_pListCtrl.GetItemText(nTransIndex, 2);	20070918 �ּ� ó��, jyh
				}
			}

			//20070918 ���� ���, jyh
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
			
			//20070919 ���� ���� ���� ���� ���, jyh
			//20090216 ������ ����� ���� FtpClient::FileTrans()���� ���, jyh
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
				//m_pListCtrl.SetItemText(nTransIndex, 4, strBuf);	20070918 �ּ� ó��, jyh
			}	
		}
		catch(...)
		{
		}			
		

		/**********************************************************************************************/
		/*20070919 �Ͻ������� ��ü ���� ���۰� ���� ���� ������ ���α׷������� ���� ���� �ϱ� ���ؼ���*/
		/*�������� ���� ���� if�� ������ �־�� ��, jyh                                               */
		/**********************************************************************************************/
		// ���� �ۼ�Ʈ ����
		if(nFileSizeEnd == 0 || m_nFileSizeTotal == 0)
		{
			m_pCtrl_ProgressTotal.SetPos(0);
		}
		else
		{
			//20090217 ����, jyh
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			if( pItemData->Grid_use == true )
			{
				m_pCtrl_ProgressTotal.SetPos(nPercent);

			}
			m_nPercent = nPercent;	//20080225 ��ü ������ ���� �ۼ�Ʈ, jyh

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
	
	//20071005 �Ϸ�� ����� �ٽ� �ٿ� �������� ��ü ���α׷����ٰ� ����ǰ�
	if(nFileSizeEnd == 0 || m_nFileSizeTotal == 0)
	{
		m_pCtrl_ProgressTotal.SetPos(0);
	}
	else
	{
		//��ü ���� ���� ����
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
	// ���۽��۽ð� ����
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

	// �Ϸ�ð� ����
	m_nTimeTotal		+= m_nTimeFileTime;
	m_nTimeFileTime		= 0;
	m_nTimeFileStart	= 0;
	m_nTimeFileSpeed	= 0;
	m_nTransSpeed_Last	= 0;

	// ������� ������Ʈ
	ReSetTransInfo();

	// �⺻ Ÿ��Ʋ�� ����
	SetWindowText(CLIENT_DOWN_TITLE);



	// ���۰�� ó��
	if(m_pPtrTrans && !m_bForceExit)
	{
		//20070918 �ּ� ó��, jyh
		//if(GetTransListIndex() != -1)
		//	m_pListCtrl.SetItemText(GetTransListIndex(), 4, "00:00:00");	

		if(m_pPtrTrans->nFileSizeReal == m_pPtrTrans->nFileSizeEnd)
			m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;
		else
			m_pPtrTrans->nTransState = ITEM_STATE_STOP;

		//m_pListCtrl.Invalidate();


		// ���� ���ۿϷ� �뺸
		CHECKDOWNEND pTransEndInfo;
		pTransEndInfo.nFileType		= m_pPtrTrans->nFileType;
		pTransEndInfo.nBoardIndex_	= m_pPtrTrans->nBoardIndex_;
		pTransEndInfo.nBoardIndex	= m_pPtrTrans->nBoardIndex;
		pTransEndInfo.nFileIdx		= m_pPtrTrans->nFileIndex;
		pTransEndInfo.nSizeBegin	= m_pPtrTrans->nFileSizeStart;
		pTransEndInfo.nSizeEnd		= m_pPtrTrans->nFileSizeEnd;

		//20080311 ������ ������ �ٿ�ε带 ����, jyh
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


		// �������� ������ �������� ����
		m_pPtrTrans	= NULL;		
	}

	// ���� ��ư�� Ŭ���� ���� �ƴϸ� ���� ����Ʈ�� �ִ��� üũ
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
// ���������� ���� : ��ưȰ��ȭ ����� �۾�
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void down_window::SetTransFlag(bool bFlag)
{
	// ���� ���� ��ư�� ��Ȳ�� �°� ����
	Skin_SetButtonVisible("BTN_STOP",	bFlag);
	Skin_SetButtonVisible("BTN_START",	!bFlag);
	Skin_SetButtonEnable("BTN_STOP",	bFlag);
	Skin_SetButtonEnable("BTN_START",	!bFlag);	

	Skin_SetButtonEnable("BTN_SELALL",	!bFlag);
	//Skin_SetButtonEnable("BTN_SELDEL",	!bFlag);	//20080228 �����߿��� ���ø�� ���� �ǰ�, jyh
	//Skin_SetButtonEnable("BTN_DONEDEL",	!bFlag);	//20071002 �����߿��� �Ϸ��� ���� �ǰ�, jyh
	Skin_SetButtonEnable("BTN_TOP",		!bFlag);
	//Skin_SetButtonEnable("BTN_UP",		!bFlag);	//20080228 �����߿��� ���� �̵� �ǰ�, jyh
	//Skin_SetButtonEnable("BTN_DOWN",	!bFlag);		//20080228 �����߿��� ���� �̵� �ǰ�, jyh
	//Skin_SetButtonEnable("BTN_BOTTOM",	!bFlag);	//20080228 �����߿��� ���� �̵� �ǰ�, jyh
	Skin_SetButtonEnable("BTN_PATH",	!bFlag);
	////20080313 ó�� ���۽� �Ǹ� ������ �� ȸ���� Ȱ��ȭ �ϰ� , jyh
	//if(m_nNameAuth == 0)
	//	Skin_SetButtonEnable("BTN_QUICKDOWN", false);		

	// ���������� ������
	if(bFlag) 
	{
		m_pPtrTrans = NULL;		//20081028 �߰�, jyh
		m_bFileTrans = true;
		DownLoadPro();
	}
	// ��������� ������
	else
	{
		m_bFileTrans = false;	

		if(m_pFtpClint)
			m_pFtpClint->m_bFlag = false;

		////20070918 ���� ���� ������ ����, jyh
		//if(m_pPtrTrans)
		//{
		//	m_pPtrTrans->nTransState = ITEM_STATE_STOP;
		//	m_pListCtrl.SetItemText(m_nCurTransFile, 3, GetFileStatus(ITEM_STATE_STOP));	
		//}

		SetWindowText(CLIENT_DOWN_TITLE);
	}
}

////20080320 �ٿ�ε� ���� �̹��� ���, jyh
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
				//20071030 ���� ���¿� ���� ũ�� ���, jyh
				CString strTemp, strBuf;
				strTemp.Format("%s", SetSizeFormat(pPtrCheck->nFileSizeReal));
				m_pListCtrl.SetItemText(nProcessIndex, 2, strTemp);
				strTemp = GetFileStatus(pPtrCheck->nTransState);
				m_pListCtrl.SetItemText(nProcessIndex, 3, strTemp);

				//20080313 �Ϸ�ÿ��� ���ϸ� ���, jyh
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

				strBuf.Format("%s'�Ϸ�'", sztemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
				
				continue;
			}

			if(	pPtrCheck->nTransState == ITEM_STATE_WAIT ||
				pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				pPtrCheck->nTransState == ITEM_STATE_STOP )
			{
				//20071119 ���� ���۵� ������ �ε��� ���� ��������� ����, jyh
				m_nCurTransFile = nProcessIndex;
				// ���� ���õ� ��� ������ ��������
				
				while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
					m_pListCtrl.SetItemState(nItemCheck, 0, -1);
				
				// ���۵� ������ ��Ŀ���� ���õ� ���·� ����
				m_pListCtrl.SetItemState(nProcessIndex, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
				m_pListCtrl.EnsureVisible(nProcessIndex, TRUE);						

				// �������� ������ ���� : ���������� ����
				m_pPtrTrans = pPtrCheck;
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;
				CString strTemp;
				strTemp = GetFileStatus(pPtrCheck->nTransState);
				m_pListCtrl.SetItemText(nProcessIndex, 3, strTemp);

				////20080313 ���� ���� ������ ���������� 0 �̸� ���ٿ� ��ư Ȱ��ȭ, jyh
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

				////20080322 gif �ִϸ��̼�, jyh
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
				
				//20090217 ����, jyh--------------------------------------------------
				//���� ���� ���� ���α׷���
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
								
				// �ٿ�ε带 �����ϱ� ���� ���� �ٿ�ε� ���� ������ ���� üũ
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



	// ���������� ����
	SetTransFlag(false);

	// ���� ���� ���� ����
	ReSetTransInfo();

	// �⺻ Ÿ��Ʋ�� ����
	SetWindowText(CLIENT_DOWN_TITLE);

	// ���ۿϷ�� ����� �ϳ��̻� �������� ó��(����Ʈ�� ���� ���¿��� ���� ��ư�� Ŭ���ϴ� ��츦 �����ϱ� ����)
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


	//20090217 ���� �ð��� 0����, jyh
	m_pStatic_Time_Over.SetWindowText("00:00:00");
	RECT rt = { 579, 54, 629, 66 };
	InvalidateRect(&rt);
	
	//20080131 �ּ�â�� Ʈ���� ���������� �������϶� ���ۿϷ�Ǹ� �����̵� ������ ����, jyh
	if(m_pTray->IsActiveTray() || m_bMinWindow)
	{
		//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
		if(nCountFailed)
			sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־�\r\n���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
		else
			sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d", nCountComplete);

		//sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d", nCountComplete, nCountFailed);
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
		// �ٿ�ε尡 �Ϸ� �Ǿ��� ���̻� �ٿ�ε� ���� ���� ���� �� üũ
		if(Skin_GetCheck("CHK_SYSTEM") == TRUE)
		{	
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
				{
					sprintf_s(strResult, 2048, "���ۿ� ���� �Ǵ� ���۱����� ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n"
						"�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.(���۱����� ��������)\r\n"
						"����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.\r\n���۽��� ����: %d / ���۱� �ٿ�ε����� ����: %d(�ٿ�ε�Ұ�)", nCountFailed,nCountFailed_Copyright);
				}
				else
				{
					sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n"
						"�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.\r\n"
						"����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.\r\n���۽��� ����: %d", nCountFailed);
				}
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n��ǻ�͸� �����մϴ�!", nCountComplete);
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
					sprintf_s(strResult, 2048, "���ۿ� ���� �Ǵ� ���۱����� ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n"
						"�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.(���۱����� ��������)\r\n"
						"����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.\r\n���۽��� ����: %d / ���۱� �ٿ�ε����� ����: %d(�ٿ�ε�Ұ�)", nCountFailed,nCountFailed_Copyright);
				}
				else
				{
					sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n"
						"�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.\r\n"
						"����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.\r\n���۽��� ����: %d", nCountFailed);
				}
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���α׷��� �����մϴ�!", nCountComplete);
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
					sprintf_s(strResult, 2048, "���ۿ� ���� �Ǵ� ���۱����� ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n"
						"�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.(���۱����� ��������)\r\n"
						"����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.\r\n���۽��� ����: %d / ���۱� �ٿ�ε����� ����: %d(�ٿ�ε�Ұ�)", nCountFailed,nCountFailed_Copyright);
				}
				else
				{
					sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n"
						"�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.\r\n"
						"����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.\r\n���۽��� ����: %d", nCountFailed);
				}
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}
		}	
	}	
}

