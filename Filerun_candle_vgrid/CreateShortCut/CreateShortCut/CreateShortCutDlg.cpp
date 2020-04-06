// CreateShortCutDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "CreateShortCut.h"
#include "CreateShortCutDlg.h"

#include <shobjidl.h>
#include <objidl.h>
#include <shlobj.h>
#include <Urlmon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ICONURL					"http://app.filerun.co.kr/app/Filerun.ico"
#define ICONNAME				"Filerun.ico"
#define SHORTCUTNAME			"파일런"
#define HOMEURL					"http://filerun.co.kr"
#define CLIENT_APP_PATH			"Filerun"

// CCreateShortCutDlg 대화 상자




CCreateShortCutDlg::CCreateShortCutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateShortCutDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCreateShortCutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreateShortCutDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CCreateShortCutDlg 메시지 처리기

BOOL CCreateShortCutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CString strTemp = AfxGetApp()->m_lpCmdLine;
	CoInitialize(0);
	CreateShortCut(strTemp.GetBuffer());
	strTemp.ReleaseBuffer();
	CoUninitialize();
	PostQuitMessage(0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCreateShortCutDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCreateShortCutDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CCreateShortCutDlg::CreateShortCut(char* strURL)
{	
	HRESULT hr;
	IShellLink *pSl;
	IPersistFile *pPf;
	WCHAR wszLnkFile[1024] = {0,};
	char szSrcFile[1024];
	char szLnkFile[1024];
	char szSystemPath[1024] = {0,};
	char szArgument[1024];
	char szDesc[1024];
	TCHAR strTemp[1024];
	CString strExplorePath;
	CFileFind ff;
	char	szFilePathIcon[1024];
	CFile	pFile;
	CString strFileData;
	char	szPath[1024];
	CString strFilePathTarget;
	

	//20080103 아이콘이 없으면 내려 받는다, jyh
	//::GetSystemDirectory(szPath, 1024);
	//sprintf_s(szFilePathIcon, 1024, "%s\\%s", szPath, ICONNAME);
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES, FALSE);
	sprintf_s(szFilePathIcon, 1024, "%s\\%s\\%s", szPath,CLIENT_APP_PATH, ICONNAME);
	
	if(!ff.FindFile(szFilePathIcon))
		URLDownloadToFile(NULL, ICONURL, szFilePathIcon, 0, NULL);

	//IShellLink 객체를 생성하고 포인터를 구한다.
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pSl);

	if(FAILED(hr))
		return FALSE;

	// 바탕화면 설치
	//20080425 ntdll.dll오류 수정, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_DESKTOP, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_PROGRAM_FILES, FALSE);
	strExplorePath.Format("%s\\Internet Explorer\\iexplore.exe", strTemp);
	//GetSystemDirectory(szSystemPath, MAX_PATH);
	//strcat_s(szSystemPath, 1024, "\\Nfile.ico");
	//숏컷의 대상체와 설명 설정
	strcpy_s(szSrcFile, strExplorePath.GetLength()+1, strExplorePath.GetBuffer());
	strExplorePath.ReleaseBuffer();
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	strcpy_s(szDesc, strlen(HOMEURL)+1, HOMEURL);
	strcpy_s(szArgument, strlen(strURL)+1, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szFilePathIcon, 0);
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//단축키 F4

	//저장하기 위해 IPersistFile 객체 생성
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//유니코드로 파일패스 변경후 저장
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, 1024);
	hr = pPf->Save(wszLnkFile, TRUE);

	//객체 해제
	pPf->Release();


	// 시작메뉴
	//20080425 ntdll.dll오류 수정, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_STARTMENU, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);
	//숏컷의 대상체와 설명 설정
	//lstrcpy(szSrcFile, "C:\\Program Files\\Internet Explorer\\iexplore.exe");
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	//lstrcpy(szDesc, "gfile.co.kr");
	//lstrcpy(szArgument, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szFilePathIcon, 0);
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//단축키 F4

	//저장하기 위해 IPersistFile 객체 생성
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//유니코드로 파일패스 변경후 저장
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, MAX_PATH);
	hr = pPf->Save(wszLnkFile, TRUE);

	//객체 해제
	pPf->Release();


	// 퀵런치
	//20080425 ntdll.dll오류 수정, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_APPDATA, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, SHORTCUTNAME);
	//GetSystemDirectory(szSystemPath, MAX_PATH);
	//strcat_s(szSystemPath, 1024, "\\Gfile16.ico");
	//숏컷의 대상체와 설명 설정
	//lstrcpy(szSrcFile, "C:\\Program Files\\Internet Explorer\\iexplore.exe");
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	//lstrcpy(szDesc, "gfile.co.kr");
	//lstrcpy(szArgument, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szFilePathIcon, 0);	//20080331 빠른실행 아이콘은 16*16 아이콘으로 생성, jyh
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//단축키 F4

	//저장하기 위해 IPersistFile 객체 생성
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//유니코드로 파일패스 변경후 저장
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, MAX_PATH);
	hr = pPf->Save(wszLnkFile, TRUE);

	//객체 해제
	pPf->Release();
	pSl->Release();

	//20080910 즐겨찾기에 추가, jyh
	strFileData.Format("[InternetShortcut]\r\nURL=%s\r\nIconIndex=0\r\nIconFile=%s", strURL, szFilePathIcon);
	
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_FAVORITES, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_FAVORITES, FALSE);
	strFilePathTarget.Format("%s\\%s.url", strTemp, SHORTCUTNAME);
	pFile.Open(strFilePathTarget, CFile::modeCreate | CFile::modeWrite);
	pFile.Write(strFileData, strFileData.GetLength());
	pFile.Close();

	////기존 바로가기 삭제
	////바탕화면
	//CFileFind findfile;

	////SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_DESKTOP, FALSE);
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// 퀵런치
	////SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_APPDATA, FALSE);
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	//strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, SHORTCUTNAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// 시작메뉴
	////SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_STARTMENU, FALSE);
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	return TRUE;
}