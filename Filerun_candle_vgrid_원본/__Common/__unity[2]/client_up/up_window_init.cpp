#include "stdafx.h"
#include "up_window.h"
#include "FtpClient.h"
#include "shlwapi.h"
#include "Dlg_Confirm.h"
//20071031 추가, jyh
#include "MD5_Client.h"
#include ".\up_window.h"


#pragma warning(disable:4312)


//-----------------------------------------------

//20080426 비스타에서 낮은 권한 프로세서와 높은 권한 프로세서간 통신을 하기 위해 선언, jyh
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
	m_rDownOverWrite	= FILE_OVERWRITE_CONTINUE;	//FILE_OVERWRITE_CHECK; 20071002 무조건 이어 전송, jyh
	m_rUpOverWrite		= FILE_OVERWRITE_CONTINUE;	//FILE_OVERWRITE_CHECK; 20071002 무조건 이어 전송, jyh
	m_rUpEndAutoClose	= 0;
	m_rDownEndAutoClose	= 0;
	m_rUpAutoTrans		= 1;	//20071002 자동 이어 올리기의 기본값은 1, jyh
	m_rDownAutoTrans	= 1;	//20071002 자동 내려받기의 기본값은 1, jyh
	m_rUpTransSpeed		= 0;
	m_rDownTransSpeed	= 0;
	m_rUpExitCheck		= 0;
	m_rDownExitCheck	= 0;
	m_rDownTransBuffer	= 3;
	m_rUpTransBuffer	= 4;
	m_iStopItemIdx		= -1;	//20071030 전송 중지된 아이템의 인덱스, jyh

	m_pPtrTrans			= NULL;
	m_pAlert			= NULL;
	m_pBanner			= NULL;
	m_pBtnHelp			= NULL;	//20071004 문제해결 도움말, jyh
	m_pSendComplete		= NULL; //20090304 컨텐츠 업로드 완료 통보, jyh

	m_pSendKTHComplete1	= NULL;	//kth 업데이트 데이터송신
	m_pSendKTHComplete2	= NULL;	//kth 업데이트 데이터송신
	m_pSendKTHComplete3	= NULL;	//kth 업데이트 데이터송신
	m_pSendKTHComplete4	= NULL;	//kth 업데이트 데이터송신
	m_pSendKTHComplete5	= NULL;	//kth 업데이트 데이터송신

	m_pTray				= NULL;	//20080522 트레이 아이콘, jyh
	
	m_pWorker_Ctrl		= new Worker(this);
	m_pFtpClint			= new FtpClient();
	m_pFtpClint->m_pMainWindow = this;
	m_nCurTransFile = -1;						//20071119 현재 전송중인 파일 인덱스, jyh
	m_bTransGura		= false;	//20071211 구라 전송, jyh

	//20080121 송신 파일 사이즈 로컬 저장을 위한 변수
	m_nFileType			= 0;
	m_nBoardIndex		= 0;
	m_nFileIndex		= 0;
	m_nFileSizeEnd		= 0;
	m_nFileSizeReal		= 0;

	m_bMinWindow		= FALSE;		//20080131 UI가 최소창인지 여부, jyh
	m_nPercent			= 0;			//20080225 전체 파일의 전송 퍼센트, jyh

	
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
	DDX_Control(pDX, IDC_STATIC_SPEED, m_pStatic_Speed);			//20070919 전송 속도, jyh
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_pStatic_FileName);		//20070919 전송 파일, jyh
}


BEGIN_MESSAGE_MAP(up_window, CSkinDialog)

	ON_WM_TIMER()	
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_WM_COPYDATA()

	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnNMDblclkFilelist)

	//20090304 컨텐츠 업로드완료 통보, jyh
	ON_MESSAGE(MSG_UPCONTENTSCOMPLETE, MSG_Complete)
	// 알림 윈도우 띄우기
	ON_MESSAGE(MSG_ALERTWINDOW, MSG_Window)
	// 파일 덮어쓰기, 이어받기 결정
	ON_MESSAGE(MSG_FILE_OVERWRITE, MSG_OverWrite)
	// 업로드 처리
	ON_MESSAGE(MSG_UPLOAD, MSG_UpLoad)
	// 접속 종료
	ON_MESSAGE(MSG_FORCE_EXIT, MSG_ForceExit)

	ON_MESSAGE(WM_TRAYICON_NOTIFY, OnTrayIconNotify)	//트레이 아이콘에서 발생하는 메시지
	ON_COMMAND(WM_TRAYICON_MENU_OPEN, OnTrayMenuOpen)	//열기메뉴 선택시
	ON_COMMAND(WM_TRAYICON_MENU_HOME, OnTrayMenuHome)	//홈페이지열기메뉴 선택시
	ON_COMMAND(WM_TRAYICON_MENU_EXIT, OnTrayMenuExit)	//종료메뉴 선택시

	ON_WM_PAINT()
END_MESSAGE_MAP()

//20080426 OS가 비스타인지, jyh
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

//20080426 비스타 메세지 필터, jyh
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

	//20080426 비스타면 메세지 필터를 한다, jyh
	if(IsVistaOS())
	{
		UINT uMsgs[2] = {WM_COPYDATA, NULL};
		AddMessageFilter(uMsgs);

		//20090528 랭키툴 실행 프로그램을 찾는다,jyh
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

	//20090303 인터넷 버전 체크
	m_nIEVer = GetVersionIE();
	// 윈도우 초기화
	InitWindow();	

	SetWindowText(CLIENT_UP_TITLE);

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

	////20071015 기존 사용자들의 자동 이어올리기 값을 1로 만들기 위해, jyh
	//DWORD dwData;
	//if(m_pReg.LoadKey(CLIENT_REG_PATH, "rUpAutoTrnas",	dwData))
	//{
	//	m_pReg.DeleteKey(CLIENT_REG_PATH, "rUpAutoTrnas");
	//	m_pReg.SaveKey(CLIENT_REG_PATH, "rUpAutoTrnas_",	1);
	//}

	//20071031 패치 서버에서 icon 다운로드, jyh
	/*CString shortcuticon;
	CString systempath;
	CFileFind find;

	GetSystemDirectory(systempath.GetBuffer(MAX_PATH), MAX_PATH);
	systempath.ReleaseBuffer();
	shortcuticon.Format("%s\\Mfile.ico", systempath);

	if(!find.FindFile(shortcuticon))
	URLDownloadToFile(NULL, "http://mfile.co.kr/mmsv/Mfile.ico", shortcuticon, 0, NULL);*/

	// 레지스트리 정보 초기화
	RegRead();

	// 리스트 컨트롤 초기화
	InitListControl();

	// 접속모드 및 접속정보 출력
	ReSetTransInfo();

	// 체크박스 설정
	Skin_SetCheck("CHK_PROGRAM", m_rUpEndAutoClose);
	Skin_SetCheck("CHK_SYSTEM", FALSE);

	// 작업표시줄 최소화 메시지를 처리하기 위해
	SetWindowLong(m_hWnd, GWL_STYLE, WS_SYSMENU | WS_MINIMIZEBOX);	

	// 초기 버튼 설정
	SetTransFlag(false);

	// 트레이 초기화
	m_pTray = new CBTray(GetSafeHwnd(), AfxGetInstanceHandle(), CLIENT_UP_TITLE);

	// 서버 연결 작업 진행
	SetTimer(1, 10, NULL);
	return TRUE;
}

//20090528 프로세스 안전하게 강제 종료, jyh
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
	// 저장 폴더가 지정 되지 않았으면 저장 폴더 지정
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

			//20071030 테스트 서버, jyh
		default:
			strcpy_s(m_pWorker_Ctrl->m_pServerIp, strlen(TESTSERVER_IP)+1, TESTSERVER_IP);
			break;
		}

		//AfxMessageBox(m_pWorker_Ctrl->m_pServerIp);	//테스트용, jyh
		//AfxMessageBox(SERVER_PORT_UP);	//테스트용, jyh

		if(!m_pWorker_Ctrl->Init(m_pWorker_Ctrl->m_pServerIp, SERVER_PORT_UP))
		{
			::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"현재 서버 점검중입니다! \r\n자세한 사항은 홈페이지를 참고하세요!", 1);
			Exit_Program(true, false);
			return;
		}
		
		// 연결이 완료되면 클라이언트 살아 있음을 통보
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

		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"프로그램이 정상적인 방법으로 실행되지 않았습니다" , 1);
		SAFE_DELETE(pToken);
		Exit_Program(false, false);
		return;
	}

	else if(nIDEvent == 2)
	{
		KillTimer(2);
		//20080121 전송 파일 사이즈 로컬 저장을 위해 파일을 만든다.
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

		//20071123 _TEST 옵션일때 테스트 서버로 접속하게 수정, jyh
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

			//20071010 수정, jyh
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
			//if(m_pWorker_Ctrl->m_nReConnectCount > 5)	//20071219 재시도 횟수를 30번에서 5번으로 수정, jyh
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
	//20071220 구라전송중 윈도서버와 연결이 끊겼을때, jyh
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

		//20071010 수정, jyh
		//Sleep(5000);
		Sleep(1000);
		SetTransFlagGura(true);
	}
	//20071211 구라 전송, jyh
	else if(nIDEvent == 999)
	{
		m_pPtrTrans->nTransState = ITEM_STATE_TRANS;
		ReSetTransInfoGura();
	}
	//20080226 트레이 아이콘 위에 마우스 커서가 있는지 알아보기 위해, jyh
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


// 메인 프로그램에서 아이템 추가 요청
BOOL up_window::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CDialog::OnCopyData(pWnd, pCopyDataStruct);

	if(m_bAuth)
	{
		/* 웹컨트롤에서 업로드 아이템 추가/완료 통보 */
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
				//20071210 업로드전 해시값 저장 테이블의 해시값과 비교요청 패킷 전송, jyh
				//해시값을 만든다.
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
				////레지스트리에서 strCopyrightPath 값을 불러온다.
				//CString strPath;
				//m_pReg.LoadKey(CLIENT_REG_PATH, "strCopyrightPath", strPath);

				//lstrcpy(upHash.szPath, strPath.GetBuffer());
				//strPath.ReleaseBuffer();

				//CString strMD5;

				////nPos = 0;
				////int nIdx = atoi(strIndex.Tokenize("/", nPos));
				//
				//if(strPath.Right(1) == "\\")	//폴더
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
				//else	//파일
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

			//if(hRB)	//20070919 주석 처리, jyh
			//	return hRB;
			
			pDC->SetBkMode(TRANSPARENT); // 배경을 투명하게
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
		//ChnPosition(cx, cy);	//20090312 수정, jyh
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
// 프로그램 종료 : 파일을 받고 있는지 체크 후 처리 해줘야 한다
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void up_window::Exit_Program(bool bRegWrite, bool bTrayCall)
{
	// 트레이에서 호출할때는 체크 하지 않음
	if(!bTrayCall && m_bFileTrans)
	{
		bool bGoTray = (bool)m_rUpExitCheck;

		if(m_rUpExitCheck == 0)
		{
			Dlg_Confirm pWindow;
			pWindow.m_nCallMode = 4;
			if(pWindow.DoModal() == IDOK)
			{
				SetTimer(777, 700, NULL);	//20080226 전송중에 닫기 버튼은 트레이 아이콘으로 가기 때문에 타이머 설정, jyh
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


	// 정보 저장
	if(bRegWrite) 
		RegWrite();

	// 리스트에 추가된 모든 아이템 삭제 (delete)
	DeleteList(0);

	//20071128 트레이 아이콘 제거, jyh
	if(m_pTray)
		m_pTray->IconDel();

	// 배너제거
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

	// 프로그램 종료
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
			if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == 13 )    // ESC 처리
				return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 시스템 종료
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
// 트레이 아이콘
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

//20071212 폴더 안의 파일경로를 vector변수에 넣는다, jyh
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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CSkinDialog::OnPaint()을(를) 호출하지 마십시오.
}
