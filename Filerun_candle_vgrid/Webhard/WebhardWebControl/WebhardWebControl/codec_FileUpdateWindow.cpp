// codec_FileUpdateWindow.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "codec_FileUpdateWindow.h"
#include "cocecDownLoadCallback.h"
#include <strsafe.h>


// Ccodec_FileUpdateWindow 대화 상자입니다.

IMPLEMENT_DYNAMIC(Ccodec_FileUpdateWindow, CDialog)

Ccodec_FileUpdateWindow::Ccodec_FileUpdateWindow(CWnd* pParent /*=NULL*/)
	: CDialog(Ccodec_FileUpdateWindow::IDD, pParent)
{

}

Ccodec_FileUpdateWindow::~Ccodec_FileUpdateWindow()
{
}

void Ccodec_FileUpdateWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Ccodec_FileUpdateWindow, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS2, &Ccodec_FileUpdateWindow::OnNMCustomdrawProgress1)
END_MESSAGE_MAP()


// Ccodec_FileUpdateWindow 메시지 처리기입니다.

BOOL Ccodec_FileUpdateWindow::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWindowText("코덱 파일 업데이트");

/*
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));
	
	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_SYSTEM, false);
	if(!lstrcmp(pPath, ""))		
		lstrcpy(pPath, "C:\\WINDOWS\\system32");

	m_pClientCodecPath = pPath;
	m_pClientCodecPath = m_pClientCodecPath + "\\xvid.ax";

	
	//DownTmp_Savefolder_load
	m_pClientCodecSetPath.Format("%sMas_base_codec.exe",DownTmp_Savefolder_load());
*/

	
	TCHAR tchBuffer[MAX_PATH] = {0};
    ExpandEnvironmentStrings("%ProgramFiles%", tchBuffer, MAX_PATH);
	m_pClientCodecPath.Format("%s\\K-Lite Codec Pack",tchBuffer);


	m_pClientCodecPath = m_pClientCodecPath + "\\psvince.dll";


	//DownTmp_Savefolder_load
	m_pClientCodecSetPath.Format("%sK-Lite_Codec_Pack_640_Basic.exe",DownTmp_Savefolder_load());

	CFileFind ff;
	if(!ff.FindFile(m_pClientCodecPath))
	{
		SetTimer(2, 100, NULL);
		return 1;
	}
	else
	{
		EndDialog(IDOK);
		return 1;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void Ccodec_FileUpdateWindow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//파일다운로드 타이머
	if(nIDEvent == 2)
	{
		int		dummy;
		HANDLE	hTrans;

		KillTimer(2);


		HWND hWndProgress = GetDlgItem(IDC_PROGRESS2)->m_hWnd;	
		m_pProgress.Attach(hWndProgress);	
		m_pProgress.SetRange(0, 100);  
		m_pProgress.SetPos(0);  
		m_pProgress.SetStep(1);
		m_pProgress.Detach();	



		hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcNewCodecImage, this, 0, (unsigned int *)&dummy);
		CloseHandle(hTrans);
	}

	//코덱설치 타이머
	if(nIDEvent == 5)
	{
		GetExitCodeProcess( execinfo_install.hProcess, &ec );

		if(ec != STILL_ACTIVE)
		{
			EndDialog(IDOK);
		}
	}


	CDialog::OnTimer(nIDEvent);
}

void Ccodec_FileUpdateWindow::OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}




void Ccodec_FileUpdateWindow::DownState(TCHAR* strState , const int nPercentDone)
{
	HWND hWndStatic = GetDlgItem(IDC_STATIC_STATE2)->m_hWnd;
	HWND hWndProgress = GetDlgItem(IDC_PROGRESS2)->m_hWnd;
	
	m_pCtrl.Attach(hWndStatic);
	m_pProgress.Attach(hWndProgress);	

	m_pCtrl.SetWindowText(strState);
	m_pCtrl.UpdateWindow();
	m_pProgress.SetPos(nPercentDone);

	m_pCtrl.Detach();
	m_pProgress.Detach();	
}

void Ccodec_FileUpdateWindow::WorkerThreadProc(int nMode)
{
	CcodecCallback pCallBack;
	HRESULT   hr;
    pCallBack.m_pDlg = this;

	try
	{			
		CFile::Remove(m_pClientCodecSetPath);
	}
	catch(...)
	{
	}


	hr = URLDownloadToFile(NULL, UPDATE_SERVER_CODEC, m_pClientCodecSetPath, 0, &pCallBack);

    if(SUCCEEDED(hr))
	{
		if(nMode == 2)
		{
			HWND hWndStatic = GetDlgItem(IDC_STATIC_STATE2)->m_hWnd;
			m_pCtrl.Attach(hWndStatic);
			m_pCtrl.SetWindowText("설치중입니다..");
			m_pCtrl.UpdateWindow();
			m_pCtrl.Detach();


	 

			// 실행을 위해 구조체 세트
			ZeroMemory( &execinfo_install, sizeof(execinfo_install) );
			execinfo_install.cbSize = sizeof(execinfo_install);
			execinfo_install.lpVerb = "open";
			execinfo_install.lpFile = m_pClientCodecSetPath;    
			execinfo_install.lpParameters = "";
			execinfo_install.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
			execinfo_install.nShow = SW_SHOWDEFAULT;

			int r = (int)ShellExecuteEx( &execinfo_install );

			SetTimer(5, 50, NULL);


/*
			ShellExecute(m_hWnd, NULL, m_pClientCodecSetPath, NULL, NULL, SW_HIDE);

			m_pCtrl.Attach(hWndStatic);
			m_pCtrl.SetWindowText("설치를 진행 중입니다..");
			m_pCtrl.UpdateWindow();
			m_pCtrl.Detach();
			Sleep(3000);
			EndDialog(IDOK);
*/

		}
	}
    else
    {
		if(nMode == 2)
			::MessageBox(m_hWnd, "코덱파일 다운로드에 실패했습니다!   ", "알림", MB_OK);

		EndDialog(IDCANCEL);
    }
}

//업로드 가능한 폴더 찾기
CString Ccodec_FileUpdateWindow::DownTmp_Savefolder_load(void)
{
	HRESULT		hr;
	CString strLocalPath;
	PWSTR pwPath = NULL;
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));

	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);

	typedef UINT (CALLBACK* LPFNDLLFUNC)(GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);
 
	HINSTANCE	hInst;
	size_t		len;
	LPFNDLLFUNC pGetKnownFldpathFnPtr;

	GUID FOLDERID_LocalAppDataLow = {0xA520A1A4, 0x1780, 0x4FF6, {0xBD, 0x18, 0x16, 0x73, 0x43, 0xC5, 0xAF, 0x16}};
	hInst = LoadLibrary( _T("shell32.dll") );

	pGetKnownFldpathFnPtr = ( LPFNDLLFUNC )GetProcAddress( hInst, "SHGetKnownFolderPath" );

	if(pGetKnownFldpathFnPtr && SUCCEEDED(pGetKnownFldpathFnPtr(FOLDERID_LocalAppDataLow, 0, NULL, &pwPath)))
	{
		hr = StringCchLengthW(pwPath, STRSAFE_MAX_CCH, &len);

		if(SUCCEEDED(hr))
			WideCharToMultiByte(CP_ACP, 0, pwPath, (int)len, pPath, MAX_PATH, NULL, NULL);

		CoTaskMemFree(pwPath);
	}

	FreeLibrary(hInst);

	strLocalPath.Format("%s\\", pPath);
	return strLocalPath;
}




unsigned int __stdcall ProcNewCodecImage(void *pParam)
{	
	Ccodec_FileUpdateWindow *pWindown = (Ccodec_FileUpdateWindow*)pParam;
	pWindown->WorkerThreadProc(2);

	return 1;
}
