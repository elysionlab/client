#include "stdafx.h"
#include "up_window.h"
#include "FtpClient.h"
#include "shlwapi.h"
#include "Dlg_Confirm.h"
//20071031 �߰�, jyh
#include "MD5_Client.h"
#include ".\up_window.h"


#pragma warning(disable:4312)


//-----------------------------------------------

//20080426 ��Ÿ���� ���� ���� ���μ����� ���� ���� ���μ����� ����� �ϱ� ���� ����, jyh
typedef BOOL (WINAPI *PFNCHANGEWINDOWMESSAGEFILTER)(UINT message, DWORD dwFlag);

up_window::up_window(CWnd* pParent /*=NULL*/)
	: CSkinDialog(up_window::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pUserID			= "";
	m_pUserPW			= "";
	m_nTransSpeed		= 0;
	m_nTransSpeed_Last	= 0;
	m_nTime				= 0;
	m_nTimeTotal		= 0;
	m_nTimeFileTime		= 0;
	m_nTimeFileStart	= 0;
	m_nTimeFileSpeed	= 0;
	m_nTimeTransButton	= 0;
	m_nTimeFileUpdate	= 0;
	m_nFileSizeTotal	= 0;
	m_nFileCount		= 0;

	m_bFileTrans		= false;
	m_bAuth				= false;
	m_bForceExit		= false;

	m_bInitWindow		= false;

	m_rPathBasic		= "";
	m_rPathLast			= "";
	m_rDownAddItemPath	= DOWN_ITEM_ADD_CHECK;
	m_rDownOverWrite	= FILE_OVERWRITE_CONTINUE;	//FILE_OVERWRITE_CHECK; 20071002 ������ �̾� ����, jyh
	m_rUpOverWrite		= FILE_OVERWRITE_CONTINUE;	//FILE_OVERWRITE_CHECK; 20071002 ������ �̾� ����, jyh
	m_rUpEndAutoClose	= 0;
	m_rDownEndAutoClose	= 0;
	m_rUpAutoTrans		= 1;	//20071002 �ڵ� �̾� �ø����� �⺻���� 1, jyh
	m_rDownAutoTrans	= 1;	//20071002 �ڵ� �����ޱ��� �⺻���� 1, jyh
	m_rUpTransSpeed		= 0;
	m_rDownTransSpeed	= 0;
	m_rUpExitCheck		= 0;
	m_rDownExitCheck	= 0;
	m_rDownTransBuffer	= 3;
	m_rUpTransBuffer	= 4;
	m_iStopItemIdx		= -1;	//20071030 ���� ������ �������� �ε���, jyh

	m_pPtrTrans			= NULL;
	m_pAlert			= NULL;
	m_pBanner			= NULL;
	m_pBtnHelp			= NULL;	//20071004 �����ذ� ����, jyh
	m_pSendComplete		= NULL; //20090304 ������ ���ε� �Ϸ� �뺸, jyh

	m_pSendKTHComplete1	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendKTHComplete2	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendKTHComplete3	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendKTHComplete4	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendKTHComplete5	= NULL;	//kth ������Ʈ �����ͼ۽�

	m_pTray				= NULL;	//20080522 Ʈ���� ������, jyh
	
	m_pWorker_Ctrl		= new Worker(this);
	m_pFtpClint			= new FtpClient();
	m_pFtpClint->m_pMainWindow = this;
	m_nCurTransFile = -1;						//20071119 ���� �������� ���� �ε���, jyh
	m_bTransGura		= false;	//20071211 ���� ����, jyh

	//20080121 �۽� ���� ������ ���� ������ ���� ����
	m_nFileType			= 0;
	m_nBoardIndex		= 0;
	m_nFileIndex		= 0;
	m_nFileSizeEnd		= 0;
	m_nFileSizeReal		= 0;

	m_bMinWindow		= FALSE;		//20080131 UI�� �ּ�â���� ����, jyh
	m_nPercent			= 0;			//20080225 ��ü ������ ���� �ۼ�Ʈ, jyh

	
	InitializeCriticalSection(&m_cs);
}

void up_window::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCTRL, m_pListCtrl);
	//DDX_Control(pDX, IDC_PROGRESS1, m_pCtrl_ProgressTotal);
	//DDX_Control(pDX, IDC_STATIC_TIME_USE, m_pStatic_Time_Use);
	DDX_Control(pDX, IDC_STATIC_TIME_OVER, m_pStatic_Time_Over);
	//DDX_Control(pDX, IDC_STATIC_COUNT, m_pStatic_FileCount);
	DDX_Control(pDX, IDC_PROGRESS2, m_pCtrl_ProgressCur);
	DDX_Control(pDX, IDC_PROGRESS3, m_pCtrl_ProgressTotal);
	DDX_Control(pDX, IDC_STATIC_SPEED, m_pStatic_Speed);			//20070919 ���� �ӵ�, jyh
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_pStatic_FileName);		//20070919 ���� ����, jyh
}


BEGIN_MESSAGE_MAP(up_window, CSkinDialog)

	ON_WM_TIMER()	
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_WM_COPYDATA()

	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnNMDblclkFilelist)

	//20090304 ������ ���ε�Ϸ� �뺸, jyh
	ON_MESSAGE(MSG_UPCONTENTSCOMPLETE, MSG_Complete)
	// �˸� ������ ����
	ON_MESSAGE(MSG_ALERTWINDOW, MSG_Window)
	// ���� �����, �̾�ޱ� ����
	ON_MESSAGE(MSG_FILE_OVERWRITE, MSG_OverWrite)
	// ���ε� ó��
	ON_MESSAGE(MSG_UPLOAD, MSG_UpLoad)
	// ���� ����
	ON_MESSAGE(MSG_FORCE_EXIT, MSG_ForceExit)

	ON_MESSAGE(WM_TRAYICON_NOTIFY, OnTrayIconNotify)	//Ʈ���� �����ܿ��� �߻��ϴ� �޽���
	ON_COMMAND(WM_TRAYICON_MENU_OPEN, OnTrayMenuOpen)	//����޴� ���ý�
	ON_COMMAND(WM_TRAYICON_MENU_HOME, OnTrayMenuHome)	//Ȩ����������޴� ���ý�
	ON_COMMAND(WM_TRAYICON_MENU_EXIT, OnTrayMenuExit)	//����޴� ���ý�

	ON_WM_PAINT()
END_MESSAGE_MAP()

//20080426 OS�� ��Ÿ����, jyh
bool up_window::IsVistaOS()
{
	BOOL bIsVista = FALSE;
	OSVERSIONINFO sInfo;
	sInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// 
	// OS VERSION CHECK.
	//
	if(::GetVersionEx(&sInfo))
	{
		if( sInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
			sInfo.dwMajorVersion >= 6 &&
			sInfo.dwMinorVersion >= 0)
		{ // Windows VISTA
				bIsVista = TRUE;
		}
	}

	return bIsVista;
}

//20080426 ��Ÿ �޼��� ����, jyh
BOOL up_window::AddMessageFilter(UINT* pMessages)
{
	HMODULE hUser32 = NULL;
	PFNCHANGEWINDOWMESSAGEFILTER pfnChangeWindowMessagesFilter = NULL;
	BOOL bRet = TRUE, bChangeFilter = FALSE;
	int i = 0;

	hUser32 = ::LoadLibrary("USER32.DLL");

	if(hUser32)
	{
		pfnChangeWindowMessagesFilter = (PFNCHANGEWINDOWMESSAGEFILTER)GetProcAddress((HINSTANCE)hUser32, "ChangeWindowMessageFilter");

		if(pfnChangeWindowMessagesFilter)
		{
			for(i=0; ; i++)
			{
				if(pMessages[i] == NULL)
					break;

				bChangeFilter = pfnChangeWindowMessagesFilter(pMessages[i], 1);

				if(bChangeFilter == FALSE)
					bRet = FALSE;
			}
		}
	}

	FreeLibrary(hUser32);

	return bRet;
}

BOOL up_window::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	//20080426 ��Ÿ�� �޼��� ���͸� �Ѵ�, jyh
	if(IsVistaOS())
	{
		UINT uMsgs[2] = {WM_COPYDATA, NULL};
		AddMessageFilter(uMsgs);

		//20090528 ��Ű�� ���� ���α׷��� ã�´�,jyh
		HWND hWnd = NULL;
		hWnd = ::FindWindow(NULL, "NavvyMan_LPC");
		DWORD dwPID;
		HANDLE hProcess;

		if(hWnd)
		{
			GetWindowThreadProcessId(hWnd, &dwPID);
			hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
			SafeTerminateProcess(hProcess, 1);
		}
	}

	//20090303 ���ͳ� ���� üũ
	m_nIEVer = GetVersionIE();
	// ������ �ʱ�ȭ
	InitWindow();	

	SetWindowText(CLIENT_UP_TITLE);

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	////20071015 ���� ����ڵ��� �ڵ� �̾�ø��� ���� 1�� ����� ����, jyh
	//DWORD dwData;
	//if(m_pReg.LoadKey(CLIENT_REG_PATH, "rUpAutoTrnas",	dwData))
	//{
	//	m_pReg.DeleteKey(CLIENT_REG_PATH, "rUpAutoTrnas");
	//	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpAutoTrnas_",	1);
	//}

	//20071031 ��ġ �������� icon �ٿ�ε�, jyh
	/*CString shortcuticon;
	CString systempath;
	CFileFind find;

	GetSystemDirectory(systempath.GetBuffer(MAX_PATH), MAX_PATH);
	systempath.ReleaseBuffer();
	shortcuticon.Format("%s\\Mfile.ico", systempath);

	if(!find.FindFile(shortcuticon))
	URLDownloadToFile(NULL, "http://mfile.co.kr/mmsv/Mfile.ico", shortcuticon, 0, NULL);*/

	// ������Ʈ�� ���� �ʱ�ȭ
	RegRead();

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	InitListControl();

	// ���Ӹ�� �� �������� ���
	ReSetTransInfo();

	// üũ�ڽ� ����
	Skin_SetCheck("CHK_PROGRAM", m_rUpEndAutoClose);
	Skin_SetCheck("CHK_SYSTEM", FALSE);

	// �۾�ǥ���� �ּ�ȭ �޽����� ó���ϱ� ����
	SetWindowLong(m_hWnd, GWL_STYLE, WS_SYSMENU | WS_MINIMIZEBOX);	

	// �ʱ� ��ư ����
	SetTransFlag(false);

	// Ʈ���� �ʱ�ȭ
	m_pTray = new CBTray(GetSafeHwnd(), AfxGetInstanceHandle(), CLIENT_UP_TITLE);

	// ���� ���� �۾� ����
	SetTimer(1, 10, NULL);
	return TRUE;
}

//20090528 ���μ��� �����ϰ� ���� ����, jyh
BOOL up_window::SafeTerminateProcess(HANDLE hProcess, UINT uExitCode)
{
	DWORD dwTID, dwCode, dwErr = 0;
	HANDLE hProcessDup = INVALID_HANDLE_VALUE;
	HANDLE hRT = NULL;
	HINSTANCE hKernel = GetModuleHandle("Kernel32");
	BOOL bSuccess = FALSE;
	BOOL bDup = DuplicateHandle(GetCurrentProcess(), hProcess, GetCurrentProcess(),
		&hProcessDup, PROCESS_ALL_ACCESS, FALSE, 0);

	if(GetExitCodeProcess((bDup) ? hProcessDup : hProcess, &dwCode) && (dwCode == STILL_ACTIVE))
	{
		FARPROC pfnExitProc;
		pfnExitProc = GetProcAddress(hKernel, "ExitProcess");
		hRT = CreateRemoteThread((bDup) ? hProcessDup : hProcess, NULL, 0,
			(LPTHREAD_START_ROUTINE)pfnExitProc, (PVOID)uExitCode, 0, &dwTID);

		if(hRT == NULL)
			dwErr = GetLastError();
	}
	else
	{
		dwErr = ERROR_PROCESS_ABORTED;
	}

	if(hRT)
	{
		WaitForSingleObject((bDup) ? hProcessDup : hProcess, INFINITE);
		CloseHandle(hRT);
		bSuccess = TRUE;
	}

	if(bDup)
		CloseHandle(hProcessDup);

	if(!bSuccess)
		SetLastError(dwErr);

	return bSuccess;
}

void up_window::OnTimer(UINT nIDEvent) 
{
	// ���� ������ ���� ���� �ʾ����� ���� ���� ����
	if(nIDEvent == 1)
	{
		KillTimer(1);

		int nRandServer = 10;

#ifndef _TEST
		nRandServer = 0;
#endif

		switch(nRandServer)
		{
		case 0:
			strcpy_s(m_pWorker_Ctrl->m_pServerIp, strlen(SERVER_IP_UP)+1, SERVER_IP_UP);
			break;

			//20071030 �׽�Ʈ ����, jyh
		default:
			strcpy_s(m_pWorker_Ctrl->m_pServerIp, strlen(TESTSERVER_IP)+1, TESTSERVER_IP);
			break;
		}

		//AfxMessageBox(m_pWorker_Ctrl->m_pServerIp);	//�׽�Ʈ��, jyh
		//AfxMessageBox(SERVER_PORT_UP);	//�׽�Ʈ��, jyh

		if(!m_pWorker_Ctrl->Init(m_pWorker_Ctrl->m_pServerIp, SERVER_PORT_UP))
		{
			::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���� ���� �������Դϴ�! \r\n�ڼ��� ������ Ȩ�������� �����ϼ���!", 1);
			Exit_Program(true, false);
			return;
		}
		
		// ������ �Ϸ�Ǹ� Ŭ���̾�Ʈ ��� ������ �뺸
		SetTimer(100, (CONNECT_UPDATE_TIME * 1000), NULL);

		m_pCommandLine = (LPCTSTR)GetCommandLine();
		m_pCommandLine = ::PathGetArgs(m_pCommandLine);

		CToken* pToken = new CToken(" ");	
		pToken->Split((LPSTR)(LPCTSTR)m_pCommandLine);	

		if(pToken->GetCount() == 2)
		{
			INFOAUTH pLoginInfo;
			strcpy_s(pLoginInfo.pUserID, strlen(pToken->GetToken(0))+1, pToken->GetToken(0));
			strcpy_s(pLoginInfo.pUserPW, strlen(pToken->GetToken(1))+1, pToken->GetToken(1));

			Packet* pPacket = new Packet;
			pPacket->Pack(CHK_AUTH, (char*)&pLoginInfo, sizeof(INFOAUTH));
			m_pWorker_Ctrl->SendPacket(pPacket);
			SAFE_DELETE(pToken);
			return;
		}

		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���α׷��� �������� ������� ������� �ʾҽ��ϴ�" , 1);
		SAFE_DELETE(pToken);
		Exit_Program(false, false);
		return;
	}

	else if(nIDEvent == 2)
	{
		KillTimer(2);
		//20080121 ���� ���� ������ ���� ������ ���� ������ �����.
		CString strFilename;
		CString strSendSize;
		char sDir[MAX_PATH];
		char sData[MAX_PATH];
		int pos = 0;
		ZeroMemory(sData, sizeof(sData));

		::GetSystemDirectory(sDir, MAX_PATH);
		strFilename.Format("%s\\%s", sDir, SENDSIZE_FILENAME);

		ifstream fin(strFilename);

		if(fin)
		{
			fin.read(sData, sizeof(sData));

			if(lstrcmp(sData, ""))
			{
				strSendSize = sData;
				
				UPLOADEND TransEndInfo;
				ZeroMemory(&TransEndInfo, sizeof(TransEndInfo));
				TransEndInfo.nFileType		= atoi(strSendSize.Tokenize("/", pos).GetBuffer());
				TransEndInfo.nBoardIndex	= _atoi64(strSendSize.Tokenize("/", pos).GetBuffer());
				TransEndInfo.nFileIdx		= _atoi64(strSendSize.Tokenize("/", pos).GetBuffer());
				TransEndInfo.nFileSizeEnd	= _atoi64(strSendSize.Tokenize("/", pos).GetBuffer());

				Packet* pPacket = new Packet;
				pPacket->Pack(UPLOAD_SIZE, (char*)&TransEndInfo, sizeof(UPLOADEND));
				m_pWorker_Ctrl->SendPacket(pPacket);
				fin.close();
				DeleteFile(strFilename);
			}
		}
		
        m_fout.open(strFilename.GetBuffer(), ios::binary|ios::out|ios::trunc);

		ReSetTransInfo();	
		return;
	}

	else if(nIDEvent == 44)
	{
		KillTimer(44);
		Exit_Program(true, false);
		return;
	}

	else if(nIDEvent == 96)
	{		
		KillTimer(96);
		m_pTray->IconDel();
		return;
	}
	else if(nIDEvent == 97)
	{		
		if(m_bFileTrans)
			m_pTray->IconChange();
		else
			m_pTray->IconFix(m_hIcon);
		return;
	}

	else if(nIDEvent == 98)
	{
		KillTimer(98);
		if(!m_bFileTrans)
		{
			m_pWorker_Ctrl->m_bReConnectTrans = true;
			SetTransFlag(true);
		}
		return;
	}

	else if(nIDEvent == 99)
	{
		m_pWorker_Ctrl->m_nReConnectCount++;

		//20071123 _TEST �ɼ��϶� �׽�Ʈ ������ �����ϰ� ����, jyh
		char serverIP[32];
		ZeroMemory(serverIP, 32);

		lstrcpy(serverIP, TESTSERVER_IP);

#ifndef _TEST
		lstrcpy(serverIP, SERVER_IP_UP);
#endif

		if(m_pWorker_Ctrl->Init(serverIP, SERVER_PORT_UP))	
		{
			KillTimer(99);
			m_pWorker_Ctrl->m_nReConnectCount = 0;

			SetTimer(100, CONNECT_UPDATE_TIME * 1000, NULL);

			INFOAUTH pLoginInfo;
			strcpy_s(pLoginInfo.pUserID, m_pUserID.GetLength()+1, m_pUserID);
			strcpy_s(pLoginInfo.pUserPW, m_pUserPW.GetLength()+1, m_pUserPW);

			Packet* pPacket = new Packet;
			pPacket->Pack(CHK_AUTH, (char*)&pLoginInfo, sizeof(INFOAUTH));
			m_pWorker_Ctrl->SendPacket(pPacket);

			//20071010 ����, jyh
			//Sleep(5000);
			Sleep(1000);

			if(m_pWorker_Ctrl->m_bReConnectTrans)
			{
				SetTransFlag(true);
			}

			if(m_pAlert)
			{
				m_pAlert->EndDialog(IDOK);
				m_pAlert = NULL;
			}
		}
		else 
		{
			//if(m_pWorker_Ctrl->m_nReConnectCount > 5)	//20071219 ��õ� Ƚ���� 30������ 5������ ����, jyh
			{
				if(m_pAlert)
				{
					m_pAlert->EndDialog(IDOK);
					m_pAlert = NULL;
				}

				//Exit_Program(true, false);
			}
		}
		return;
	}

	else if(nIDEvent == 100)
	{
		Packet* pPacket = new Packet;
		pPacket->Pack(USER_PTR_UPDATE);
		m_pWorker_Ctrl->SendPacket(pPacket);	
		return;
	}
	//20071220 ���������� ���������� ������ ��������, jyh
	else if(nIDEvent == 998)
	{
		KillTimer(998);
		SetTimer(100, CONNECT_UPDATE_TIME * 1000, NULL);

		INFOAUTH pLoginInfo;
		strcpy_s(pLoginInfo.pUserID, m_pUserID.GetLength()+1, m_pUserID);
		strcpy_s(pLoginInfo.pUserPW, m_pUserPW.GetLength()+1, m_pUserPW);

		Packet* pPacket = new Packet;
		pPacket->Pack(CHK_AUTH, (char*)&pLoginInfo, sizeof(INFOAUTH));
		m_pWorker_Ctrl->SendPacket(pPacket);

		//20071010 ����, jyh
		//Sleep(5000);
		Sleep(1000);
		SetTransFlagGura(true);
	}
	//20071211 ���� ����, jyh
	else if(nIDEvent == 999)
	{
		m_pPtrTrans->nTransState = ITEM_STATE_TRANS;
		ReSetTransInfoGura();
	}
	//20080226 Ʈ���� ������ ���� ���콺 Ŀ���� �ִ��� �˾ƺ��� ����, jyh
	else if(nIDEvent == 777)
	{
		//down_window* pMainWindow = (down_window*)AfxGetApp()->m_pMainWnd;
		CPoint ptIcon;
		POINT pt;
		int nIconFound;

		if(!m_pTray->m_pTrayWnd)
			return;
		else
		{
			nIconFound = m_pTray->m_pTrayWnd->m_pTipPosition->GetTrayIconPosition(ptIcon, CTrayIconPosition::UseDirectOnly);
			GetCursorPos(&pt);
			//m_pTray->m_pTrayWnd->m_nFileTransPercent = m_nPercent;
		}

		if(nIconFound == -1)
		{
			AfxMessageBox("There is no icon to track!\nInitialize CTrayIconPosition before!");
			m_pTray->m_pTrayWnd->ShowWindow(SW_HIDE);
			return;
		}
		else
		{
			if(ptIcon.x > pt.x || ptIcon.x + 16 < pt.x || ptIcon.y > pt.y || ptIcon.y + 16 < pt.y)
			{
				m_pTray->m_pTrayWnd->ShowWindow(SW_HIDE);
			}
			else
			{
				m_pTray->m_pTrayWnd->ShowWindow(SW_SHOW);
				m_pTray->m_pTrayWnd->m_Ctrl_progressTotal.SetPos(m_nPercent);
				CString str;
				str.Format("%d%%", m_nPercent);
				m_pTray->m_pTrayWnd->m_static_Percent.SetWindowText(str);
				//m_pTray->m_pTrayWnd->m_static_Percent.Invalidate();
				RECT rt = {114, 30, 142, 41};
				m_pTray->m_pTrayWnd->InvalidateRect(&rt);
			}
		}
	}
}


// ���� ���α׷����� ������ �߰� ��û
BOOL up_window::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CDialog::OnCopyData(pWnd, pCopyDataStruct);

	if(m_bAuth)
	{
		/* ����Ʈ�ѿ��� ���ε� ������ �߰�/�Ϸ� �뺸 */
		if(pCopyDataStruct->dwData == 0)
		{
			if(pCopyDataStruct->cbData > 1)
			{
				//char temp[1024];
				//lstrcpy(temp, (char*)(pCopyDataStruct->lpData));
				//AfxMessageBox(temp);
				//return true;
				//char* pWebItem = new char[pCopyDataStruct->cbData];
				m_pWebItem = new char[pCopyDataStruct->cbData];
				ZeroMemory(m_pWebItem, sizeof(m_pWebItem));
				lstrcpy(m_pWebItem, (char*)(pCopyDataStruct->lpData));
				//20071210 ���ε��� �ؽð� ���� ���̺��� �ؽð��� �񱳿�û ��Ŷ ����, jyh
				//�ؽð��� �����.
				CMD5 pMD5;
				DWORD dwErrorCode = NO_ERROR;
				UPHASH upHash;
				ZeroMemory(&upHash, sizeof(UPHASH));
				CString strToken = m_pWebItem;
				int nPos = 0;
				CString strTemp = strToken.Tokenize("#<", nPos);
				upHash.nFileType = atoi(strTemp.GetBuffer());
				strTemp = strToken.Tokenize("#<", nPos);
				upHash.bbs_no = atoi(strTemp.GetBuffer());
				strTemp = strToken.Tokenize("#<", nPos);
				nPos = 0;

				CString strIndex = strTemp;

				if(strIndex != "")
					upHash.hash_idx = _atoi64(strTemp);
				

				////-------------------------------------------------------
				////������Ʈ������ strCopyrightPath ���� �ҷ��´�.
				//CString strPath;
				//m_pReg.LoadKey(CLIENT_REG_PATH, "strCopyrightPath", strPath);

				//lstrcpy(upHash.szPath, strPath.GetBuffer());
				//strPath.ReleaseBuffer();

				//CString strMD5;

				////nPos = 0;
				////int nIdx = atoi(strIndex.Tokenize("/", nPos));
				//
				//if(strPath.Right(1) == "\\")	//����
				//{
				//	strPath.Delete(strPath.GetLength()-1, 1);
				//	GetFilePathInFolder(strPath);

				//	for(int i=0; i<(int)m_v_cstrGuraFilePath.size(); i++)
				//	{
				//		strMD5 += pMD5.GetString(m_v_cstrGuraFilePath[i], dwErrorCode);
				//		strMD5 += "/";
				//	}

				//	strMD5.Delete(strMD5.GetLength()-1, 1);
				//}
				//else	//����
				//{
				//	m_v_cstrGuraFilePath.push_back(strPath);
				//	strMD5 = pMD5.GetString(strPath, dwErrorCode);
				//}

				//lstrcpy(upHash.szCheckSum, (LPSTR)(LPCTSTR)strMD5);
				//m_v_cstrGuraFilePath.clear();
				//---------------------------------------------------------------------

				Packet* pPacket = new Packet;
				pPacket->Pack(LOAD_UPHASH, (char*)&upHash, sizeof(upHash));
				m_pWorker_Ctrl->SendPacket(pPacket);
				

				//pPacket->Pack(LOAD_UPLIST, (char*)pWebItem, pCopyDataStruct->cbData + 1);
				//m_pWorker_Ctrl->SendPacket(pPacket);
				//delete pWebItem;
			}
			return TRUE;
		}		
	}	
	else
	{
		return FALSE;
	}

	return FALSE;
}


HBRUSH up_window::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
    switch(nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			UINT nCtrlID = pWnd->GetDlgCtrlID();

			HBRUSH hRB = DrawCtrl(pDC, nCtrlID);

			//if(hRB)	//20070919 �ּ� ó��, jyh
			//	return hRB;
			
			pDC->SetBkMode(TRANSPARENT); // ����� �����ϰ�
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
    }
    return hbr;
}


void up_window::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch(nID) 
	{
		case SC_CLOSE: 			
			Exit_Program(false, false);		
			return;
		//case SC_MINIMIZE:
			//ShowWindow(SW_MINIMIZE);	
			//return;
	}
	CSkinDialog::OnSysCommand(nID, lParam);
}

void up_window::ChnPosition(int cx, int cy){}
HBRUSH up_window::DrawCtrl(CDC* pDC, UINT nCtrlID){return NULL;}
void up_window::InitWindow(){}

void up_window::OnSize(UINT nType, int cx, int cy) 
{
	CSkinDialog::OnSize(nType, cx, cy);

	if(GetSafeHwnd() && m_bInitWindow)
	{
		//ChnPosition(cx, cy);	//20090312 ����, jyh
	}	
}


void up_window::InitListControl(){}


void up_window::RegRead()
{
	m_pReg.LoadKey(CLIENT_REG_PATH, "rPathBasic",		m_rPathBasic);
	//m_pReg.LoadKey(CLIENT_REG_PATH, "rPathLast",		m_rPathLast);
	//m_pReg.LoadKey(CLIENT_REG_PATH, "rDownOverWrite",	m_rDownOverWrite);
	//m_pReg.LoadKey(CLIENT_REG_PATH, "rUpOverWrite",		m_rUpOverWrite);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDwonAddItemPath", m_rDownAddItemPath);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpEndAutoClose",	m_rUpEndAutoClose);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownEndAutoClose",m_rDownEndAutoClose);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpAutoTrans",		m_rUpAutoTrans);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpTransSpeed",	m_rUpTransSpeed);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownTransSpeed",	m_rDownTransSpeed);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpExitCheck",		m_rUpExitCheck);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownExitCheck",	m_rDownExitCheck);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownTransBuffer",	m_rDownTransBuffer);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpTransBuffer",	m_rUpTransBuffer);

	m_pReg.LoadKey("Software\\GRETECH\\GomPlayer", "ProgramPath", m_rPathGomPlayer);
}

void up_window::RegWrite()
{
	m_pReg.SaveKey(CLIENT_REG_PATH, "rPathBasic",		m_rPathBasic);
	//m_pReg.SaveKey(CLIENT_REG_PATH, "rPathLast",		m_rPathLast);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownOverWrite",	m_rDownOverWrite);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpOverWrite",		m_rUpOverWrite);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDwonAddItemPath", m_rDownAddItemPath);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpEndAutoClose",	m_rUpEndAutoClose);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownEndAutoClose",m_rDownEndAutoClose);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpAutoTrans",		m_rUpAutoTrans);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpTransSpeed",	m_rUpTransSpeed);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownTransSpeed",	m_rDownTransSpeed);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpExitCheck",		m_rUpExitCheck);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownExitCheck",	m_rDownExitCheck);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownTransBuffer",	m_rDownTransBuffer);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpTransBuffer",	m_rUpTransBuffer);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���α׷� ���� : ������ �ް� �ִ��� üũ �� ó�� ����� �Ѵ�
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::Exit_Program(bool bRegWrite, bool bTrayCall)
{
	// Ʈ���̿��� ȣ���Ҷ��� üũ ���� ����
	if(!bTrayCall && m_bFileTrans)
	{
		bool bGoTray = (bool)m_rUpExitCheck;

		if(m_rUpExitCheck == 0)
		{
			Dlg_Confirm pWindow;
			pWindow.m_nCallMode = 4;
			if(pWindow.DoModal() == IDOK)
			{
				SetTimer(777, 700, NULL);	//20080226 �����߿� �ݱ� ��ư�� Ʈ���� ���������� ���� ������ Ÿ�̸� ����, jyh
				bGoTray = true;

				if(pWindow.m_bCheck == TRUE)
					m_rUpExitCheck = 1;
			}
			else
			{
				return;
			}
		}

		if(bGoTray)
		{
			m_pTray->TrayBegin();
			return;
		}		
	}



	m_bForceExit	= true;
	SetTransFlag(false);
	Sleep(500);


	// ���� ����
	if(bRegWrite) 
		RegWrite();

	// ����Ʈ�� �߰��� ��� ������ ���� (delete)
	DeleteList(0);

	//20071128 Ʈ���� ������ ����, jyh
	if(m_pTray)
		m_pTray->IconDel();

	// �������
	m_pBanner->DelAll();
	m_pBtnHelp->DelAll();		//20071004	jyh
	m_pBanner->Stop();
	m_pBtnHelp->Stop();			//20071004	jyh
	SAFE_DELETE(m_pBanner);
	SAFE_DELETE(m_pBtnHelp);	//20071004	jyh
	SAFE_DELETE(m_pSendComplete);	//20090304	jyh
	SAFE_DELETE(m_pSendKTHComplete1);	//20090304	jyh	
	SAFE_DELETE(m_pSendKTHComplete2);	//20090304	jyh	
	SAFE_DELETE(m_pSendKTHComplete3);	//20090304	jyh	
	SAFE_DELETE(m_pSendKTHComplete4);	//20090304	jyh	
	SAFE_DELETE(m_pSendKTHComplete5);	//20090304	jyh	
	
	m_pWorker_Ctrl->End();
	SAFE_DELETE(m_pFtpClint);
	SAFE_DELETE(m_pWorker_Ctrl);

	DeleteCriticalSection(&m_cs);

	// ���α׷� ����
	EndDialog(IDOK);
}


BOOL up_window::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
	    return TRUE;

	if(VK_F1 <= pMsg->wParam && VK_F10 >= pMsg->wParam)
		return true;

	switch(pMsg->message)
	{
		case WM_KEYDOWN:
			if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == 13 )    // ESC ó��
				return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �ý��� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::SystemShutdownOTN()
{
   HANDLE hToken; 
   TOKEN_PRIVILEGES tkp; 
 
   if(!OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
      return;
 
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
 
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
 
   if(GetLastError() != ERROR_SUCCESS) 
      return;
 
   if(!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)) 
      return; 

   return;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ʈ���� ������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LRESULT up_window::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	m_pTray->OnTrayIconNotify((UINT)lParam);
	return S_OK;
}
void up_window::OnTrayMenuOpen()
{
	m_pTray->TrayEnd();
}
void up_window::OnTrayMenuHome()
{
	ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), URL_HOMEPAGE, NULL, SW_SHOWMAXIMIZED);
}
void up_window::OnTrayMenuExit()
{
	m_pTray->IconDel();
	Exit_Program(true, true);
}

//20071212 ���� ���� ���ϰ�θ� vector������ �ִ´�, jyh
void up_window::GetFilePathInFolder(CString folderpath)
{
	CString		path;
	CFileFind	ff;
	bool bret = true;

	path.Format("%s\\*.*", folderpath);

	if( ff.FindFile(path) )
	{
		while(bret)
		{
			bret = ff.FindNextFile();
			if(ff.IsDots())
			{
				//continue;
			}
			else if(!ff.IsHidden() && ff.IsDirectory())
			{
				CString fp =ff.GetFilePath(); 
				GetFilePathInFolder(fp);
			}
			else
				m_v_cstrGuraFilePath.push_back(ff.GetFilePath());   
		}
	}

	ff.Close();
}
void up_window::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CSkinDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
}
