#include "stdafx.h"
#include "up_window.h"
#include "FtpClient.h"
//20071211 �߰�, jyh
#include "Dlg_Slide.h"
#include <time.h>

//20080121 ���� ������ ���� ������ ���� �������� ���ν���, jyh
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
// ���� ���� ���� ����
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


	// ��ü üũ
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
	
	
	// ��������	
	__int64 nFileCountEnd_ = m_pFtpClint->m_bFlag ? nFileCountEnd+1 : nFileCountEnd;
	strBuf.Format("%I64d / %I64d (%s / %s)", nFileCountEnd_, m_nFileCount, SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal));
	//m_pStatic_FileCount.SetWindowText(strBuf);	//20070918 jyh


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

	// �������� ���� ����
	if((nTransIndex = GetTransListIndex()) != -1)
	{
		try
		{
			pItemData = (PRUPCTX)m_pListCtrl.GetItemData(nTransIndex);
			//20090304 ������ ���ε�Ϸ� �뺸, jyh
			m_nBoardIndex = pItemData->nBoardIndex;

			if(strcmp(m_strPrevFileName.GetBuffer(), pItemData->pFileName))
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

				strBuf.Format("%s'���ε� ��'", strtemp);
				m_pStatic_FileName.SetWindowText(strBuf);

				RECT rt = { 27, 53, 377, 65 };
				InvalidateRect(&rt);
			}

			m_strPrevFileName.ReleaseBuffer();

			//����
			int nPercent2 = abs((int)(pItemData->nFileSizeEnd * 100 / pItemData->nFileSizeReal));
			strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			//strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal));			
			m_pListCtrl.SetItemText(nTransIndex, 1, strBuf);

			if(m_nTimeFileStart != 0)
			{
				strSpeed = SetSpeedFormat(pItemData->nFileSizeEnd - pItemData->nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);
				//20070919 �ӵ� ���, jyh
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
					//strSpeed = m_pListCtrl.GetItemText(nTransIndex, 2);	20070918 �ּ� ó��, jyh
				}

				//20080121 ���� ������ ���� ����, jyh
				if(pItemData->nFileSizeEnd > 0 && pItemData->nFileSizeEnd <= pItemData->nFileSizeReal)
				{
					m_nFileType			= pItemData->nFileType;
					m_nBoardIndex		= pItemData->nBoardIndex;
					m_nFileIndex		= pItemData->nFileIndex;
					m_nFileSizeEnd		= pItemData->nFileSizeEnd;
				}
				
				//20080121 ���� ������ ���� ������ ���� ����, jyh
				//// ���ۿϷ�� ������ �ӽ� ����
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

			//20070918 ���� ���, jyh
			//strBuf.Format("%d%%", abs((int)(pItemData->nFileSizeEnd  * 100 / pItemData->nFileSizeReal)));		
			strBuf = GetFileStatus(pItemData->nTransState);
			//m_pListCtrl.SetItemText(nTransIndex, 2, strBuf);	

			//20070919 ���� ���� ���� ���� ���, jyh
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
			//��ü ���� ���� ����
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			m_pCtrl_ProgressTotal.SetPos(nPercent);
			m_nPercent = nPercent;	//20080225 ��ü ������ ���� �ۼ�Ʈ, jyh

			if(m_pFtpClint->m_bFlag && !strSpeed.IsEmpty())
			{
				//strBuf.Format("%d%% (%s)", nPercent, strSpeed);
				strBuf.Format("%s / %s (%d%%)", SetSizeFormat(nFileSizeEnd), SetSizeFormat(m_nFileSizeTotal), nPercent);
				m_pCtrl_ProgressTotal.SetText(strBuf);
			}
			else
			{
				//strBuf.Format("��ü %d%%", nPercent);
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
// 20071211 ���� ������ ����, jyh
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


	// ��ü üũ
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

	// ��ü ���� �ð� ����
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
			
	
	// �������� ���� ����
	if((nTransIndex = GetTransListIndex()) != -1)
	{
		try
		{
			pItemData = (PRUPCTX)m_pListCtrl.GetItemData(nTransIndex);

			if(strcmp(m_strPrevFileName.GetBuffer(), pItemData->pFileName))
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

				strBuf.Format("%s'���ε� ��'", strtemp);
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

			//����
			int nPercent2 = abs((int)(pItemData->nFileSizeEnd * 100 / pItemData->nFileSizeReal));
			strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			//strBuf.Format("%s/%s", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal));			
			m_pListCtrl.SetItemText(nTransIndex, 1, strBuf);

			//if(m_nTimeFileStart != 0)
			//{
				strSpeed = SetSizeFormat(dwBuf);
				//20070919 �ӵ� ���, jyh
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
				//	//strSpeed = m_pListCtrl.GetItemText(nTransIndex, 2);	20070918 �ּ� ó��, jyh
				//}

				// ���ۿϷ�� ������ �ӽ� ����
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
					//	TransEndInfo.nFileIdx		= pItemData->nHashIndex;	//���� ������ ���۱� ��� ���̺� �ε����� ����
					//	TransEndInfo.nFileSizeEnd	= pItemData->nFileSizeEnd;

					//	Packet* pPacket = new Packet;
					//	pPacket->Pack(LOAD_UPGURAEND, (char*)&TransEndInfo, sizeof(UPLOADEND));
					//	m_pWorker_Ctrl->SendPacket(pPacket);
					//	//���� �Ѱ� ���� �Ϸ� �� �ٽ� UpLoadProGura()ȣ��
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

			//20070918 ���� ���, jyh
			//strBuf.Format("%d%%", abs((int)(pItemData->nFileSizeEnd  * 100 / pItemData->nFileSizeReal)));		
			strBuf = GetFileStatus(pItemData->nTransState);
			m_pListCtrl.SetItemText(nTransIndex, 2, strBuf);	

			//20070919 ���� ���� ���� ���� ���, jyh
			m_pCtrl_ProgressCur.SetPos(nPercent2);
			strBuf.Format("%s / %s (%d%%)", SetSizeFormat(pItemData->nFileSizeEnd), SetSizeFormat(pItemData->nFileSizeReal), nPercent2);
			m_pCtrl_ProgressCur.SetText(strBuf);
		
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
			//��ü ���� ���� ����
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
				//strBuf.Format("��ü %d%%", nPercent);
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
			TransEndInfo.nFileIdx		= pItemData->nHashIndex;	//���� ������ ���۱� ��� ���̺� �ε����� ����
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
			//���� �Ѱ� ���� �Ϸ� �� m_pPtrTrans�� NULL�� ����� �ٽ� UpLoadProGura()ȣ��
			m_pPtrTrans = NULL;
			UpLoadProGura();
		}
	}

	return true;
}

//20071217 ���� ����Ʈ ����, jyh
void up_window::GuraUpList(PUPHASH upHash)
{
	//CString strPath = upHash->szPath;
	//������Ʈ������ strCopyrightPath ���� �ҷ��´�.
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
		//�������� �Ϸ�� ������ ���� �Ϸ����� üũ�� ����..
		CONITEM conitem;
		conitem.nBoardIndex = upHash->bbs_no;
		conitem.nItemCnt	= (int)m_v_cstrGuraFilePath.size();
		m_v_sConItem.push_back(conitem);

		//20071015 ������ ������ 8�� �̻��̸� ����Ʈ ���� ũ�� ����, jyh
		if(m_pListCtrl.GetItemCount() + m_v_cstrGuraFilePath.size() > 8)
			m_pListCtrl.SetColumnWidth(0, 363);
	
		// ���α׷��� �ֻ��� ������� ���
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
	
		// ���۹��� ����� ť�� ���
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
		
	//���� �Ǵ°� ó�� ���̰�...
	if(m_rDownAutoTrans == 1 && !m_bFileTrans)	//20071030 ȯ�漳������ �ڵ����� ������ ����, jyh
	{
		m_bTransGura = true;
		SetTransFlagGura(true);
	}
}

void up_window::FileTransLoop()
{
	// ���۽��۽ð� ����
	m_nTimeFileUpdate = m_nTimeFileStart = GetTickCount();
	
	//20071128 ReSetTransInfo_Log �ۼ�, jyh
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

	//20080121 ���� ���� ������ ���� ������ ���� ������ ����, jyh
	CWinThread* pSendSizeThread = AfxBeginThread(SendSizeThread, (LPVOID)this);

	while(m_pFtpClint->m_bFlag)
	{
		m_pListCtrl.SetRedraw(FALSE);
		ReSetTransInfo();
		m_pListCtrl.SetRedraw(TRUE);		

		Sleep(500);
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
	SetWindowText(CLIENT_UP_TITLE);



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

		m_pListCtrl.Invalidate();
		
		// ���� ���ۿϷ� �뺸
		UPLOADEND pTransEndInfo;
		ZeroMemory(&pTransEndInfo, sizeof(pTransEndInfo));
		pTransEndInfo.nFileType		= m_pPtrTrans->nFileType;
		pTransEndInfo.nBoardIndex	= m_pPtrTrans->nBoardIndex;
		pTransEndInfo.nFileIdx		= m_pPtrTrans->nFileIndex;
		pTransEndInfo.nFileSizeEnd	= m_pPtrTrans->nFileSizeEnd;

		//20080123 ������ ������ ���ε带 ����, jyh
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

		//20080121 ���������� ������ ���ÿ� �����ϴ� ���ϵ� ������, jyh
		/*if(pSendSizeThread)
		{
			int iExit = 1;
			SendSizeThread(&iExit);
		}*/


		// �������� ������ �������� ����
		m_pPtrTrans	= NULL;		
	}

	// ���� ��ư�� Ŭ���� ���� �ƴϸ� ���� ����Ʈ�� �ִ��� üũ
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
// ���������� ���� : ��ưȰ��ȭ ����� �۾�
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::SetTransFlag(bool bFlag)
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

	// ���������� ������
	if(bFlag) 
	{
		m_pPtrTrans = NULL;		//20081028 �߰�, jyh
		m_bFileTrans = true;
		UpLoadPro();
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
		//	m_pListCtrl.SetItemText(m_iStopItemIdx, 2, GetFileStatus(ITEM_STATE_STOP));	
		//}

		SetWindowText(CLIENT_UP_TITLE);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 20071211 ���� ������ ����, jyh
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::SetTransFlagGura(bool bFlag)
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
	//Skin_SetButtonEnable("BTN_UP",		!bFlag);	//20080228 �����߿��� ���ø�� ���� �ǰ�, jyh
	//Skin_SetButtonEnable("BTN_DOWN",	!bFlag);		//20080228 �����߿��� ���ø�� ���� �ǰ�, jyh
	//Skin_SetButtonEnable("BTN_BOTTOM",	!bFlag);	//20080228 �����߿��� ���ø�� ���� �ǰ�, jyh

	// ���������� ������
	if(bFlag) 
	{
		m_pPtrTrans = NULL;		//20081028 �߰�, jyh
		m_bFileTrans = true;
		UpLoadProGura();	//���� ����
	}
	// ��������� ������
	else
	{
		m_bFileTrans = false;	
		m_pFtpClint->m_bFlag = false;

		////20070918 ���� ���� ������ ����, jyh
		//if(m_pPtrTrans)
		//{
		//	m_pPtrTrans->nTransState = ITEM_STATE_STOP;
		//	m_pListCtrl.SetItemText(m_iStopItemIdx, 2, GetFileStatus(ITEM_STATE_STOP));	
		//}

		SetWindowText(CLIENT_UP_TITLE);
	}
}

/*
	���Լ��� ���۽����� Ŭ���������� �ϳ��� ������ ������ �Ϸ�Ǿ����� ȣ���
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
				//20071030 ���� ���¿� ���� ũ�� ���, jyh
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
				//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20071119 �߰�, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_CONNECT/*20080228 �߰�, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20080228 �߰�, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 �߰�, jyh*/)
			{
				//20071119 ���� ���۵� ������ �ε��� ���� ��������� ����, jyh
				m_nCurTransFile = nProcessIndex;
				// �ش������� �����ϴ��� üũ
				CFileFind pFindFile;
				if(!pFindFile.FindFile(pPtrCheck->pFullPath))
				{
					pFindFile.Close();
					pPtrCheck->nTransState = ITEM_STATE_ERROR;

					char strResult[2048];
					sprintf_s(strResult, 2048, "���ε� ������ �������� �ʽ��ϴ�!\r\n%s", pPtrCheck->pFullPath);
					::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
					continue;
				}

				pFindFile.FindNextFile();
				nItemFileSize = pFindFile.GetLength();
				pFindFile.Close();
				


				// ���� ���õ� ��� ������ ��������
				while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
					m_pListCtrl.SetItemState(nItemCheck, 0, -1);

				// ���۵� ������ ��Ŀ���� ���õ� ���·� ����
				m_pListCtrl.SetItemState(nProcessIndex, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
				m_pListCtrl.EnsureVisible(nProcessIndex, TRUE);						

				// �������� ������ ���� : ���������� ����
				m_pPtrTrans = pPtrCheck;
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;
				m_pListCtrl.SetItemText(m_nCurTransFile, 2, GetFileStatus(ITEM_STATE_CONNECT));
				
				
				// ���ε带 �����ϱ� ���� ���� ���ε� �� ������ ���� üũ
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



	// ���������� ����
	SetTransFlag(false);

	// ���� ���� ���� ����
	ReSetTransInfo();

	// �⺻ Ÿ��Ʋ�� ����
	SetWindowText(CLIENT_UP_TITLE);

	// ���ۿϷ�� ����� �ϳ��̻� �������� ó��(����Ʈ�� ���� ���¿��� ���� ��ư�� Ŭ���ϴ� ��츦 �����ϱ� ����)
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

	//20080131 �ּ�â�� Ʈ���� ���������� �������϶� ���ۿϷ�Ǹ� �����̵� ������ ����, jyh
	if(m_pTray->IsActiveTray() || m_bMinWindow)
	{
		//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
		if(nCountFailed)
			sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־�\r\n���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
		else
			sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d", nCountComplete);

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
			//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
					sprintf_s(strResult, 2048, "���ۿ� ���� �Ǵ� ���۱����� ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d\r\n���۱� ���ε����� ����: %d(���ε�Ұ�)\r\n�Ǽ��ڵ� ���ε����� ����: %d(���ε�Ұ�)", nCountFailed,nCountFailed_Copyright,nCountFailed_Virus);
				else
					sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n\r\n ��ǻ�͸� �����մϴ�!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d\r\n\r\n��ǻ�͸� �����մϴ�!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SystemShutdownOTN();		
			SetTimer(44, 100, NULL);
		}
		else if(Skin_GetCheck("CHK_PROGRAM") == TRUE)
		{
			//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
					sprintf_s(strResult, 2048, "���ۿ� ���� �Ǵ� ���۱����� ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d / ���۱� ���ε����� ����: %d(���ε�Ұ�)", nCountFailed,nCountFailed_Copyright);
				else
					sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);

				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n\r\n ���α׷��� �����մϴ�!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d\r\n\r\n ���α׷��� �����մϴ�!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SetTimer(44, 100, NULL);
		}
		else
		{
			//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
			if(nCountFailed)
			{
				if(nCountFailed_Copyright)
					sprintf_s(strResult, 2048, "���ۿ� ���� �Ǵ� ���۱����� ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: % / ���۱� ���ε����� ����: %d(���ε�Ұ�)", nCountFailed,nCountFailed_Copyright);
				else
					sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}
			
			//sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
		}
	}
}

//20071211 ���� ������ ����, jyh
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
				//20071030 ���� ���¿� ���� ũ�� ���, jyh
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
				//pPtrCheck->nTransState == ITEM_STATE_DEFER/*20071119 �߰�, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_DELAY/*20081027 �߰�, jyh*/ ||
				//pPtrCheck->nTransState == ITEM_STATE_CONFAILED/*20081027 �߰�, jyh*/)
			{
				//20071119 ���� ���۵� ������ �ε��� ���� ��������� ����, jyh
				m_nCurTransFile = nProcessIndex;
				// �ش������� �����ϴ��� üũ
				CFileFind pFindFile;
				if(!pFindFile.FindFile(pPtrCheck->pFullPath))
				{
					pFindFile.Close();
					pPtrCheck->nTransState = ITEM_STATE_ERROR;

					char strResult[2048];
					sprintf_s(strResult, 2048, "���ε� ������ �������� �ʽ��ϴ�!\r\n%s", pPtrCheck->pFullPath);
					::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
					continue;
				}

				pFindFile.FindNextFile();
				nItemFileSize = pFindFile.GetLength();
				pFindFile.Close();

				// ���� ���õ� ��� ������ ��������
				while((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) != -1)		
					m_pListCtrl.SetItemState(nItemCheck, 0, -1);

				// ���۵� ������ ��Ŀ���� ���õ� ���·� ����
				m_pListCtrl.SetItemState(nProcessIndex, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED);
				m_pListCtrl.EnsureVisible(nProcessIndex, TRUE);						

				// �������� ������ ���� : ���������� ����
				m_pPtrTrans = pPtrCheck;
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;
				m_pListCtrl.SetItemText(m_nCurTransFile, 2, GetFileStatus(ITEM_STATE_CONNECT));

				//���� ����
				SetTimer(999, 1000, NULL);
				
				return;
			}
		}
	}



	// ���������� ����
	SetTransFlagGura(false);

	// ���� ���� ���� ����
	ReSetTransInfoGura();

	// �⺻ Ÿ��Ʋ�� ����
	SetWindowText(CLIENT_UP_TITLE);

	// ���ۿϷ�� ����� �ϳ��̻� �������� ó��(����Ʈ�� ���� ���¿��� ���� ��ư�� Ŭ���ϴ� ��츦 �����ϱ� ����)
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

	//20080131 �ּ�â�� Ʈ���� ���������� �������϶� ���ۿϷ�Ǹ� �����̵� ������ ����, jyh
	if(m_pTray->IsActiveTray() || m_bMinWindow)
	{
		//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
		if(nCountFailed)
			sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־�\r\n���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
		else
			sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d", nCountComplete);

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
			//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
			if(nCountFailed)
			{
				sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n\r\n ��ǻ�͸� �����մϴ�!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d\r\n\r\n��ǻ�͸� �����մϴ�!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SystemShutdownOTN();		
			SetTimer(44, 100, NULL);
		}
		else if(Skin_GetCheck("CHK_PROGRAM") == TRUE)
		{
			//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
			if(nCountFailed)
			{
				sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n\r\n ���α׷��� �����մϴ�!", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d\r\n\r\n ���α׷��� �����մϴ�!", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);

			SetTimer(44, 100, NULL);
		}
		else
		{
			//20081028 ���� ���� ������ �ϳ��� ������ ������ �Ϸ���� �ʾҴٴ� �޼����� ����, jyh
			if(nCountFailed)
			{
				sprintf_s(strResult, 2048, "���ۿ� ������ ������ �־� ���� ������ �Ϸ����� ���߽��ϴ�!\r\n���۽��� ����: %d", nCountFailed);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 0);
			}
			else
			{
				sprintf_s(strResult, 2048, "���� ������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d", nCountComplete);
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
			}

			//sprintf_s(strResult, 2048, "������ �Ϸ�Ǿ����ϴ�!\r\n���ۼ��� : %d\r\n���۽��� : %d", nCountComplete, nCountFailed);
			//::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)strResult, 1);
		}
	}

	
}