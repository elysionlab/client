#include "stdafx.h"
#include "down_window.h"
#include "FtpClient.h"
#include "shlwapi.h"
#include "Dlg_Confirm.h"
#include ".\down_window.h"
//20071031 �߰�, jyh
#include <shobjidl.h>
#include <objidl.h>
#include <shlobj.h>

#pragma warning(disable:4312)
#pragma warning(disable:4996)


//20080426 ��Ÿ���� ���� ���� ���μ����� ���� ���� ���μ����� ����� �ϱ� ���� ����, jyh
typedef BOOL (WINAPI *PFNCHANGEWINDOWMESSAGEFILTER)(UINT message, DWORD dwFlag);

down_window::down_window(CWnd* pParent /*=NULL*/)
	: CSkinDialog(down_window::IDD, pParent)
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
	m_nFileSizeTotal	= 0;
	m_nFileCount		= 0;
	m_nFileSizeEnd		= 0;

	m_bFileTrans		= false;
	m_bAuth				= false;
	m_bForceExit		= false;

	m_bInitWindow		= false;
	m_bInitPreItem		= false;

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
	m_rCreateShortcut	= 1;	//20071002 �ٷΰ���, jyh
	
	m_pPtrTrans			= NULL;
	m_pAlert			= NULL;
	m_pBanner			= NULL;
	m_pBtnHelp			= NULL;	//20071004	�����ذ� ����, jyh
	m_pTray				= NULL;

	m_pSendiMBCComplete1	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendiMBCComplete2	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendiMBCComplete3	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendiMBCComplete4	= NULL;	//kth ������Ʈ �����ͼ۽�
	m_pSendiMBCComplete5	= NULL;	//kth ������Ʈ �����ͼ۽�

	m_pWorker_Ctrl		= new Worker(this);
	m_pFtpClint			= new FtpClient();
	m_pFtpClint->m_pMainWindow = this;

	m_iItemCnt			= 0;			//20071117 ���� ��û ���� ������ ��, jyh
	m_nCurTransFile		= -1;			//20071119 ���� �������� ���� �ε���, jyh
	m_bMinWindow		= FALSE;		//20080131 UI�� �ּ�â���� ����, jyh
	m_nPercent			= 0;			//20080225 ��ü ������ ���� �ۼ�Ʈ, jyh
	
	InitializeCriticalSection(&m_cs);
}

void down_window::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTCTRL, m_pListCtrl);
	//DDX_Control(pDX, IDC_PROGRESS1, m_pCtrl_ProgressTotal);
	//DDX_Control(pDX, IDC_STATIC_TIME_USE, m_pStatic_Time_Use);	//20070918 jyh
	DDX_Control(pDX, IDC_STATIC_TIME_OVER, m_pStatic_Time_Over);
	//DDX_Control(pDX, IDC_STATIC_COUNT, m_pStatic_FileCount);		//20070918 jyh
	DDX_Control(pDX, IDC_PROGRESS2, m_pCtrl_ProgressCur);
	DDX_Control(pDX, IDC_PROGRESS3, m_pCtrl_ProgressTotal);
	DDX_Control(pDX, IDC_STATIC_SPEED, m_pStatic_Speed);			//20070919 ���� �ӵ�, jyh
	DDX_Control(pDX, IDC_STATIC_DOWNPATH, m_pStatic_Path);			//20070919 ���� ���, jyh
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_pStatic_FileName);		//20070919 ���� ����, jyh
	//DDX_Control(pDX, IDC_GIFFIRST, m_Picture);						//20080322 gif �ִϸ��̼�, jyh
}


BEGIN_MESSAGE_MAP(down_window, CSkinDialog)

	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_WM_COPYDATA()

	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnNMDblclkFilelist)

	// �˸� ������ ����
	ON_MESSAGE(MSG_ALERTWINDOW, MSG_Window)
	// �ٿ�ε� ���ȣ��
	ON_MESSAGE(MSG_DOWNLOAD, MSG_DownLoad)
	// ���� �����, �̾�ޱ� ����
	ON_MESSAGE(MSG_FILE_OVERWRITE, MSG_OverWrite)
	// ���� ����
	ON_MESSAGE(MSG_FORCE_EXIT, MSG_ForceExit)

	ON_MESSAGE(WM_TRAYICON_NOTIFY, OnTrayIconNotify)	//Ʈ���� �����ܿ��� �߻��ϴ� �޽���
	ON_COMMAND(WM_TRAYICON_MENU_OPEN, OnTrayMenuOpen)	//����޴� ���ý�
	ON_COMMAND(WM_TRAYICON_MENU_HOME, OnTrayMenuHome)	//Ȩ����������޴� ���ý�
	ON_COMMAND(WM_TRAYICON_MENU_EXIT, OnTrayMenuExit)	//����޴� ���ý�

//	ON_WM_PAINT()
ON_WM_PAINT()
END_MESSAGE_MAP()

//20080426 OS�� ��Ÿ����, jyh
bool down_window::IsVistaOS()
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
BOOL down_window::AddMessageFilter(UINT* pMessages)
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

BOOL down_window::OnInitDialog()
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
	
	// ������ �ʱ�ȭ
	InitWindow();

	SetWindowText(CLIENT_DOWN_TITLE);

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	////20071015 ���� ����ڵ��� �ڵ� �̾�ޱ� ���� 1�� ����� ����, jyh
	//if(m_pReg.LoadKey(CLIENT_REG_PATH, "rUpAutoTrans",	dwData))
	//{
	//	m_pReg.DeleteKey(CLIENT_REG_PATH, "rUpAutoTrans");
	//	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpAutoTrans_", 1);
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

	////20071016 �ŷ��� �� �ִ� ����Ʈ�� ���
	//dwData = 2;
	//m_pReg.SaveKey("Software\\Microsoft\\Windows\\CurrentVersion\\"
	//	"Internet Settings\\ZoneMap\\Domains\\mfile.co.kr", "http", dwData);

	// ������Ʈ�� ���� �ʱ�ȭ
	RegRead();

	//20071029 �ٷΰ��� �����, jyh
	if(m_rCreateShortcut)
		CreateShortcut(URL_HOMEPAGE);

	// ����Ʈ ��Ʈ�� �ʱ�ȭ
	InitListControl();

	// ���Ӹ�� �� �������� ���
	ReSetTransInfo();

	// üũ�ڽ� ����
	Skin_SetCheck("CHK_PROGRAM", m_rDownEndAutoClose);
	Skin_SetCheck("CHK_SYSTEM", FALSE);

	// �۾�ǥ���� �ּ�ȭ �޽����� ó���ϱ� ����
	SetWindowLong(m_hWnd, GWL_STYLE, WS_SYSMENU | WS_MINIMIZEBOX);	

	// �ʱ� ��ư ����
	SetTransFlag(false);

	// Ʈ���� �ʱ�ȭ
	m_pTray = new CBTray(GetSafeHwnd(), AfxGetInstanceHandle(), CLIENT_DOWN_TITLE);

	// ���� ���� �۾� ����
	SetTimer(1, 10, NULL);
	return TRUE;
}

//20090528 ���μ��� �����ϰ� ���� ����, jyh
BOOL down_window::SafeTerminateProcess(HANDLE hProcess, UINT uExitCode)
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

void down_window::OnTimer(UINT nIDEvent) 
{

	// ���� ������ ���� ���� �ʾ����� ���� ���� ����
	if(nIDEvent == 1)
	{
		KillTimer(1);

		if(m_rPathBasic == "")
		{
			m_pReg.SaveKey(CLIENT_REG_PATH, "rCreateShortcut",	m_rCreateShortcut);		//20071002 �ٷΰ���, jyh
			//20071017 �⺻ ��ġ ������ ����ȭ������, jyh
			//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)m_rPathBasic, CSIDL_DESKTOP, FALSE);
			//20080425 ntdll.dll���� ����, jyh
			TCHAR strTemp[1024];
			SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
			m_rPathBasic = strTemp;

			DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
			CSelectFolderDialog pSelectDialog(FALSE, m_rPathBasic, wFlag, NULL, this, "�⺻���� ����� ������ ������ �ּ���");
			
			if(pSelectDialog.DoModal() == IDCANCEL)	
			{
				Exit_Program(false, false);
				return;
			}
			
			m_rPathBasic = pSelectDialog.GetSelectedPath();
			m_rPathLast  = m_rPathBasic;

			if(ITEMLISTADD_OPTION == 1)
				m_rDownAddItemPath = 1;
			else if(ITEMLISTADD_OPTION == 2)
				m_rDownAddItemPath = 2;

			m_pReg.SaveKey(CLIENT_REG_PATH, "rPathBasic",		m_rPathBasic);
			m_pReg.SaveKey(CLIENT_REG_PATH, "rPathLast",		m_rPathLast);
			m_pReg.SaveKey(CLIENT_REG_PATH, "rDwonAddItemPath", m_rDownAddItemPath);			
		}

		int nRandServer = 10;

#ifndef _TEST
		/*srand((unsigned)time(NULL));
		nRandServer = (rand() % 2) + 2;*/
		//20071119 ���̵��� ù���ڸ� ���ؼ� ������ ������ �й�(�ߺ� ���̵� üũ�� ����), jyh
		char szTemp[MAX_PATH];
		ZeroMemory(szTemp, MAX_PATH);

		lstrcpy(szTemp, AfxGetApp()->m_lpCmdLine);
		int nAscii = szTemp[0];

		//a~m(13��)
		if(nAscii > 96 && nAscii < 110)
			nRandServer = 0;
		//n~z(13��), �׿�
		else
			nRandServer = 1;

		////a~f(6��)
		//if(nAscii > 96 && nAscii < 103)
		//	nRandServer = 0;
		////g~l(6��)
		//else if(nAscii > 102 && nAscii < 109)
		//	nRandServer = 1;
		////m~s(7��)
		//else if(nAscii > 108 && nAscii < 116)
		//	nRandServer = 2;
		////t~z(7��)
		//else if(nAscii > 115 && nAscii < 123)
		//	nRandServer = 3;
		////a~z�� ������ 0~9, A~Z�� 3�� ������ �Ҵ�
		//else
		//	nRandServer = 3;
#endif

		switch(nRandServer)
		{
			case 0:
				lstrcpy(m_pWorker_Ctrl->m_pServerIp, SERVER_IP_DOWN1);
				break;
			case 1:
				lstrcpy(m_pWorker_Ctrl->m_pServerIp, SERVER_IP_DOWN2);
				break;
			/*case 2:
				lstrcpy(m_pWorker_Ctrl->m_pServerIp, SERVER_IP_DOWN3);
				break;
			case 3:
				lstrcpy(m_pWorker_Ctrl->m_pServerIp, SERVER_IP_DOWN4);
				break;*/

			//20071030 �׽�Ʈ ����, jyh
			default:
				strcpy_s(m_pWorker_Ctrl->m_pServerIp, strlen(TESTSERVER_IP)+1, TESTSERVER_IP);
				break;
		}
		//AfxMessageBox(m_pWorker_Ctrl->m_pServerIp);	//�׽�Ʈ�� jyh
		//return;
		
		if(!m_pWorker_Ctrl->Init(m_pWorker_Ctrl->m_pServerIp, SERVER_PORT_DOWN))
		{
			::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"�ý��۰� ������ ���������� �̷������ �ʾ� �ٿ�ε尡 �ߴܵǾ����ϴ�.\r\n�ش� ������ ����������->�������� �ڷῡ�� 2�ϰ� ����ٿ�ε� �� �� �ֽ��ϴ�.\r\n����� �����������ڷᡯ���� �ٽ� �̿����ֽñ� �ٶ��ϴ�.", 1);
			Exit_Program(false, false);
			return;
		}

		//AfxMessageBox("���ӵ�!!");	//�׽�Ʈ�� jyh
		// ������ �Ϸ�Ǹ� Ŭ���̾�Ʈ ��� ������ �뺸
		SetTimer(100, (CONNECT_UPDATE_TIME * 1000), NULL);
		
		m_pCommandLine = (LPCTSTR)GetCommandLine();
		//AfxMessageBox(m_pCommandLine);//test
		m_pCommandLine = ::PathGetArgs(m_pCommandLine);
		
		CToken* pToken = new CToken(" ");	
		pToken->Split((LPSTR)(LPCTSTR)m_pCommandLine);	

		Download_type_link=false;
		//if(pToken->GetCount() == 2)
		if(pToken->GetCount() == 3)		//20080313 Ŀ�ǵ���ο� �Ǹ� ���� �ڵ� ���� 3��, jyh
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
	
		//121026 - IE������ ������ �ٿ�ε� - S 
		if(pToken->GetCount() == 2)		//��Ÿ���������� ����
		{
			//��ũŸ�� ����üũ
			Download_type_link=true;
			//��ũ���� ��ū���� ��ȭ
			CToken* url_pToken = new CToken("/");	
			//��ū����ó��
			url_pToken->Split((LPSTR)(LPCTSTR)pToken->GetToken(1));	
			//�ڷḦ ó��
			Download_List_Items.Format("%s%s%s%s%s%s%s%s%s",	"0", SEP_FIELD, 
																url_pToken->GetToken(4), SEP_FIELD, 
																url_pToken->GetToken(3), SEP_FIELD, 
																url_pToken->GetToken(5), SEP_FIELD, 
																url_pToken->GetToken(6)
			);




			INFOAUTH pLoginInfo;
			strcpy_s(pLoginInfo.pUserID, strlen(url_pToken->GetToken(2))+1, url_pToken->GetToken(2));
			strcpy_s(pLoginInfo.pUserPW, strlen(url_pToken->GetToken(url_pToken->GetCount()-1))+1, url_pToken->GetToken(url_pToken->GetCount()-1));
/*
//			150730 ��й�ȣ�� / �� ���ԵǼ� ������
			CToken* url_pToken_ETC = new CToken("/////");	
			url_pToken_ETC->Split((LPSTR)(LPCTSTR)pToken->GetToken(1));	
			strcpy_s(pLoginInfo.pUserPW, strlen(url_pToken_ETC->GetToken(1))+1, url_pToken_ETC->GetToken(1));
*/

			delete url_pToken;

			Packet* pPacket = new Packet;
			pPacket->Pack(CHK_AUTH, (char*)&pLoginInfo, sizeof(INFOAUTH));
			m_pWorker_Ctrl->SendPacket(pPacket);
			SAFE_DELETE(pToken);
			return;
		}
		//121026 - IE������ ������ �ٿ�ε� - E

		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���α׷��� �������� ������� ������� �ʾҽ��ϴ�", 1);
		SAFE_DELETE(pToken);
		Exit_Program(false, false);
		return;
	}

	else if(nIDEvent == 2)
	{		
		KillTimer(2);	
		ReSetTransInfo();
		ShowWindow(SW_RESTORE);
		SetForegroundWindow();
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);	
		
		//121026 - IE������ ������ �ٿ�ε� 
		if(Download_type_link)
			Download_List_update();


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

		if(m_pWorker_Ctrl->Init(m_pWorker_Ctrl->m_pServerIp, m_pWorker_Ctrl->m_nServerPort))	
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
			//if(m_pWorker_Ctrl->m_nReConnectCount > 30)
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

BOOL down_window::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CDialog::OnCopyData(pWnd, pCopyDataStruct);
	
	if(m_bAuth)
	{
		if(pCopyDataStruct->cbData > 1)
		{
////////// ��ū�� �ʿ� �����͸� �����ϰ� �ٽ� ��ū ���� - S
			char* pWebItem_tmp = new char[pCopyDataStruct->cbData];
			strcpy_s(pWebItem_tmp, strlen((char*)(pCopyDataStruct->lpData))+1, (char*)(pCopyDataStruct->lpData));

			CToken*	Datas_Tokens = new CToken(SEP_FIELD);
			Datas_Tokens->Split(pWebItem_tmp);

			//�·�ī ���������
			Mureka_use=atoi(Datas_Tokens->GetToken(4));

			CString pItem_tmp;
			pItem_tmp.Format("%s%s%s%s%s%s%s",	(LPSTR)Datas_Tokens->GetToken(0), SEP_FIELD, 
												(LPSTR)Datas_Tokens->GetToken(1), SEP_FIELD, 
												(LPSTR)Datas_Tokens->GetToken(2), SEP_FIELD, 
												(LPSTR)Datas_Tokens->GetToken(3)
											);
			delete Datas_Tokens;

			char* pWebItem = new char[pCopyDataStruct->cbData];
			strcpy_s(pWebItem, pItem_tmp.GetLength()+1, (LPSTR)(LPCTSTR)pItem_tmp);

////////// ��ū�� �ʿ� �����͸� �����ϰ� �ٽ� ��ū ���� - E


/*
			char* pWebItem = new char[pCopyDataStruct->cbData];
			strcpy_s(pWebItem, strlen((char*)(pCopyDataStruct->lpData))+1, (char*)(pCopyDataStruct->lpData));
*/

			//20071117 ��Ͽ� �ִ� ���������� ����, jyh-------------
			CString strItem = pWebItem;
			
			for(int i=0; i<(int)m_v_sItem.size(); i++)
			{
				if(m_v_sItem[i] == strItem)
				{
					::SendMessage(m_hWnd, MSG_ALERTWINDOW, 
					(WPARAM)"�̹� ��Ͽ� �ִ� �������Դϴ�.", (LPARAM)1);
					
					return 1;
				}
			}
			
			m_v_sItem.push_back(strItem);
			//-------------------------------------------------------
			
			Packet* pPacket = new Packet;				
			pPacket->Pack(LOAD_DOWNLIST, (char*)pWebItem, pCopyDataStruct->cbData + 1);
			m_pWorker_Ctrl->SendPacket(pPacket);
			delete pWebItem;
		}
		return 1;
    }
	else
	{
		return 0;
	}

	return 1;
}

HBRUSH down_window::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

void down_window::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch(nID) 
	{
		case SC_CLOSE: 			
			Exit_Program(false, false);		
			return;
		/*case SC_MINIMIZE:
			ShowWindow(SW_MINIMIZE);	
			return;*/
	}
	CSkinDialog::OnSysCommand(nID, lParam);
}


void down_window::InitWindow(){}
void down_window::ChnPosition(int cx, int cy){}
HBRUSH down_window::DrawCtrl(CDC* pDC, UINT nCtrlID){return NULL;}


void down_window::OnSize(UINT nType, int cx, int cy) 
{
	CSkinDialog::OnSize(nType, cx, cy);

	if(GetSafeHwnd() && m_bInitWindow)
	{
		//ChnPosition(cx, cy);	//20090312 ����, jyh
	}		
}

void down_window::InitListControl(){}


void down_window::RegRead()
{
	m_pReg.LoadKey(CLIENT_REG_PATH, "rPathBasic",		m_rPathBasic);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rPathLast",		m_rPathLast);
	//m_pReg.LoadKey(CLIENT_REG_PATH, "rDownOverWrite",	m_rDownOverWrite);		//20071002 ������ �̾ �����ϱ� ����, jyh
	//m_pReg.LoadKey(CLIENT_REG_PATH, "rUpOverWrite",		m_rUpOverWrite);	//20071002 ������ �̾ �����ϱ� ����, jyh
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownAddItemPath", m_rDownAddItemPath);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpEndAutoClose",	m_rUpEndAutoClose);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownEndAutoClose",m_rDownEndAutoClose);
	//m_pReg.LoadKey(CLIENT_REG_PATH, "rUpAutoTrans",		m_rUpAutoTrans);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownAutoTrnas",	m_rDownAutoTrans);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpTransSpeed",	m_rUpTransSpeed);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownTransSpeed",	m_rDownTransSpeed);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpExitCheck",		m_rUpExitCheck);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownExitCheck",	m_rDownExitCheck);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rDownTransBuffer",	m_rDownTransBuffer);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rUpTransBuffer",	m_rUpTransBuffer);
	m_pReg.LoadKey(CLIENT_REG_PATH, "rCreateShortcut",	m_rCreateShortcut);	//20071002 �ٷΰ���, jyh

	m_pReg.LoadKey("Software\\GRETECH\\GomPlayer", "ProgramPath", m_rPathGomPlayer);
}

void down_window::RegWrite()
{
	m_pReg.SaveKey(CLIENT_REG_PATH, "rPathBasic",		m_rPathBasic);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rPathLast",		m_rPathLast);
	//m_pReg.SaveKey(CLIENT_REG_PATH, "rDownOverWrite",	m_rDownOverWrite);
	//m_pReg.SaveKey(CLIENT_REG_PATH, "rUpOverWrite",		m_rUpOverWrite);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownAddItemPath", m_rDownAddItemPath);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpEndAutoClose",	m_rUpEndAutoClose);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownEndAutoClose",m_rDownEndAutoClose);
	//m_pReg.SaveKey(CLIENT_REG_PATH, "rUpAutoTrans",		m_rUpAutoTrans);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownAutoTrnas",	m_rDownAutoTrans);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpTransSpeed",	m_rUpTransSpeed);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownTransSpeed",	m_rDownTransSpeed);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpExitCheck",		m_rUpExitCheck);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownExitCheck",	m_rDownExitCheck);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rDownTransBuffer",	m_rDownTransBuffer);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpTransBuffer",	m_rUpTransBuffer);
	m_pReg.SaveKey(CLIENT_REG_PATH, "rCreateShortcut",	m_rCreateShortcut);		//20071002 �ٷΰ���, jyh
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���α׷� ���� : ������ �ް� �ִ��� üũ �� ó�� ����� �Ѵ�
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void down_window::Exit_Program(bool bRegWrite, bool bTrayCall)
{
	// Ʈ���̿��� ȣ���Ҷ��� üũ ���� ����
	if(!bTrayCall && m_bFileTrans)
	{
		bool bGoTray = (bool)m_rDownExitCheck;

		if(m_rDownExitCheck == 0)
		{
			Dlg_Confirm pWindow;
			pWindow.m_nCallMode = 4;
 			if(pWindow.DoModal() == IDOK)
			{
				SetTimer(777, 700, NULL);	//20080226 �����߿� �ݱ� ��ư�� Ʈ���� ���������� ���� ������ Ÿ�̸� ����, jyh
				bGoTray = true;

				if(pWindow.m_bCheck == TRUE)
					m_rDownExitCheck = 1;
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
	SAFE_DELETE(m_pSendiMBCComplete1);	//20090304	jyh	
	SAFE_DELETE(m_pSendiMBCComplete2);	//20090304	jyh	
	SAFE_DELETE(m_pSendiMBCComplete3);	//20090304	jyh	
	SAFE_DELETE(m_pSendiMBCComplete4);	//20090304	jyh	
	SAFE_DELETE(m_pSendiMBCComplete5);	//20090304	jyh	

	m_pWorker_Ctrl->End();
	SAFE_DELETE(m_pFtpClint);
	SAFE_DELETE(m_pWorker_Ctrl);
	
	DeleteCriticalSection(&m_cs);

	// ���α׷� ����
	EndDialog(IDOK);
}



BOOL down_window::PreTranslateMessage(MSG* pMsg)
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
void down_window::SystemShutdownOTN()
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
LRESULT down_window::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	m_pTray->OnTrayIconNotify((UINT)lParam);
	return S_OK;
}
void down_window::OnTrayMenuOpen()
{
	m_pTray->TrayEnd();
}
void down_window::OnTrayMenuHome()
{
	ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), URL_HOMEPAGE, NULL, SW_SHOWMAXIMIZED);
}
void down_window::OnTrayMenuExit()
{
	m_pTray->IconDel();
	Exit_Program(true, true);
}

//20071002 �ٷΰ���, jyh
bool down_window::CreateShortcut(char* strURL)
{
	HRESULT hr;
	IShellLink *pSl;
	IPersistFile *pPf;
	WCHAR wszLnkFile[1024] = {0,};
	char szSrcFile[1024];
	char szLnkFile[1024];
	char szSystemPath[1024] = {0,};
	char szIconPath[1024] = {0,};
	char szArgument[1024];
	char szDesc[1024];
	TCHAR strTemp[1024];
	CString strFilePathTarget;
	CString strExplorePath;
	CFileFind findfile;

	//IShellLink ��ü�� �����ϰ� �����͸� ���Ѵ�.
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pSl);

	if(FAILED(hr))
		return FALSE;

	// ����ȭ�� ��ġ
	//20080425 ntdll.dll���� ����, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_DESKTOP, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_PROGRAM_FILES, FALSE);
	strExplorePath.Format("%s\\Internet Explorer\\iexplore.exe", strTemp);

//	GetSystemDirectory(szSystemPath, 1024);
//	sprintf_s(szIconPath, 1024, "%s\\%s", szSystemPath, ICONNAME);
	sprintf_s(szIconPath, 1024, "%s\\%s\\%s", strTemp,CLIENT_APP_PATH, ICONNAME);

	//������ ���ü�� ���� ����
	strcpy_s(szSrcFile, strExplorePath.GetLength()+1, strExplorePath.GetBuffer());
	strExplorePath.ReleaseBuffer();
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	strcpy_s(szDesc, lstrlen(URL_HOMEPAGE)+1, URL_HOMEPAGE);
	strcpy_s(szArgument, lstrlen(strURL)+1, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szIconPath, 0);
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//����Ű F4

	//�����ϱ� ���� IPersistFile ��ü ����
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//�����ڵ�� �����н� ������ ����
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, 1024);
	hr = pPf->Save(wszLnkFile, TRUE);

	//��ü ����
	pPf->Release();


	// ���۸޴�
	//20080425 ntdll.dll���� ����, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_STARTMENU, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);
	//������ ���ü�� ���� ����
	//lstrcpy(szSrcFile, "C:\\Program Files\\Internet Explorer\\iexplore.exe");
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	//lstrcpy(szDesc, "gfile.co.kr");
	//lstrcpy(szArgument, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szIconPath, 0);
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//����Ű F4

	//�����ϱ� ���� IPersistFile ��ü ����
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//�����ڵ�� �����н� ������ ����
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, 1024);
	hr = pPf->Save(wszLnkFile, TRUE);

	//��ü ����
	pPf->Release();


	// ����ġ
	//20080425 ntdll.dll���� ����, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_APPDATA, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);
	//GetSystemDirectory(szSystemPath, 1024);
	//strcat_s(szSystemPath, 1024, "\\Gfile16.ico");
	//������ ���ü�� ���� ����
	//lstrcpy(szSrcFile, "C:\\Program Files\\Internet Explorer\\iexplore.exe");
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	//lstrcpy(szDesc, "gfile.co.kr");
	//lstrcpy(szArgument, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szIconPath, 0);	//20080331 �������� �������� 16*16 ���������� ����, jyh
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//����Ű F4

	//�����ϱ� ���� IPersistFile ��ü ����
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//�����ڵ�� �����н� ������ ����
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, 1024);
	hr = pPf->Save(wszLnkFile, TRUE);
	
	//��ü ����
	pPf->Release();
	pSl->Release();

	CFile	pFile;
	CString strFileData;

	strFileData.Format("[InternetShortcut]\r\nURL=%s\r\nIconIndex=0\r\nIconFile=%s", szArgument, szSystemPath);

	//20080910 ���ã�⿡ �߰�, jyh
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_FAVORITES, FALSE);
	strFilePathTarget.Format("%s\\%s.url", strTemp, CLIENT_SERVICE_NAME);
	pFile.Open(strFilePathTarget, CFile::modeCreate | CFile::modeWrite);
	pFile.Write(strFileData, strFileData.GetLength());
	pFile.Close();

	////20080514 ���� �ٷΰ��� ����, jyh
	////����ȭ��
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// ����ġ
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	//strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// ���۸޴�
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	return TRUE;
}

void down_window::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CSkinDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
}



//121026 - IE������ ������ �ٿ�ε� 
void down_window::Download_List_update()
{

////////// ��ū�� �ʿ� �����͸� �����ϰ� �ٽ� ��ū ���� - S
	char* pWebItem_tmp = new char[Download_List_Items.GetLength()+1];
	strcpy(pWebItem_tmp,Download_List_Items);

	CToken*	Datas_Tokens = new CToken(SEP_FIELD);
	Datas_Tokens->Split(pWebItem_tmp);

	//�·�ī ���������
	Mureka_use=atoi(Datas_Tokens->GetToken(4));

	CString pItem_tmp;
	pItem_tmp.Format("%s%s%s%s%s%s%s",	(LPSTR)Datas_Tokens->GetToken(0), SEP_FIELD, 
										(LPSTR)Datas_Tokens->GetToken(1), SEP_FIELD, 
										(LPSTR)Datas_Tokens->GetToken(2), SEP_FIELD, 
										(LPSTR)Datas_Tokens->GetToken(3)
									);
	delete Datas_Tokens;

	char* pWebItem = new char[Download_List_Items.GetLength()];
	strcpy_s(pWebItem, pItem_tmp.GetLength()+1, (LPSTR)(LPCTSTR)pItem_tmp);

////////// ��ū�� �ʿ� �����͸� �����ϰ� �ٽ� ��ū ���� - E

	//20071117 ��Ͽ� �ִ� ���������� ����, jyh-------------
	CString strItem = pWebItem;
	int No_update_list=0;
	
	for(int i=0; i<(int)m_v_sItem.size(); i++)
	{
		if(m_v_sItem[i] == strItem)
		{
			::SendMessage(m_hWnd, MSG_ALERTWINDOW, 
			(WPARAM)"�̹� ��Ͽ� �ִ� �������Դϴ�.", (LPARAM)1);

			No_update_list=1;
		}
	}
	
	if(!No_update_list)
	{
		m_v_sItem.push_back(strItem);
		//-------------------------------------------------------

		Packet* pPacket = new Packet;				
		pPacket->Pack(LOAD_DOWNLIST, (char*)pWebItem, Download_List_Items.GetLength());
		m_pWorker_Ctrl->SendPacket(pPacket);
		delete pWebItem;
	}

}