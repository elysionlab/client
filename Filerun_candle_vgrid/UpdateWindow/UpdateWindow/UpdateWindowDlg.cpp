// UpdateWindowDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "UpdateWindow.h"
#include "UpdateWindowDlg.h"
#include "config.h"
#include <WinInet.h>
#include <Urlmon.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUpdateWindowDlg 대화 상자


CUpdateWindowDlg::CUpdateWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateWindowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nSize = 0;
	m_nReadSize = 0;
	m_nPos = 0;
}

void CUpdateWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATE, m_static_Text);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CUpdateWindowDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//인스톨 메세지
	ON_MESSAGE(WM_USER_INSTALL_END, OnInstallComplete)
END_MESSAGE_MAP()


// CUpdateWindowDlg 메시지 처리기

BOOL CUpdateWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetWindowText(CLIENT_UPDATE_TITLE);
	m_nSize = GetUpdateSize();
	m_progress.SetRange(0, 100);
	m_static_Text.SetWindowText("설치 파일을 내려 받습니다!");
	SetTimer(1, 100, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUpdateWindowDlg::OnPaint()
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
HCURSOR CUpdateWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUpdateWindowDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nIDEvent == 1)
	{
		KillTimer(1);

		/*for(int i=0; i<m_strArrUpdateList.GetSize(); i++)
		m_pWebAccess->GetWebFile(NULL, UPDATESERVER_URL, INTERNET_DEFAULT_HTTP_PORT,
		m_strArrUpdateList.GetAt(i));*/
		CString strFile;
		strFile.Format(_T("/app/%s"), UPDATE_SETUP);
		m_pWebAccess->GetWebFile(NULL, SERVER_URL, INTERNET_DEFAULT_HTTP_PORT, strFile);
		
		TCHAR tszPath[1024];
		CString strPath;
		ZeroMemory(tszPath, sizeof(tszPath));
		SHGetSpecialFolderPath(NULL, tszPath, CSIDL_PROGRAM_FILES, FALSE);
		strPath.Format(_T("%s\\%s\\%s"), tszPath, CLIENT_APP_PATH, UPDATE_SETUP);

		::ShellExecute(this->m_hWnd, NULL, strPath, NULL, NULL, SW_SHOWNORMAL);
	}

	CDialog::OnTimer(nIDEvent);
}

//업데이트 파일들 총 사이즈
int CUpdateWindowDlg::GetUpdateSize()
{
	CInternetSession cis;
	CHttpFile* pFile;
	CString strResult;
	CString strListUrl;
	int nTotalSize = 0;

	//for(int i=0; i<m_strArrUpdateList.GetSize(); i++)
	//{
	//	strListUrl.Format(_T("%s%s"), HOME_URL, m_strArrUpdateList.GetAt(i));
	//	pFile = (CHttpFile*)cis.OpenURL(strListUrl, 1, INTERNET_FLAG_TRANSFER_BINARY | 
	//		INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 0, 0);

	//	if(pFile == NULL)
	//		return 0;

	//	//파일 길이를 얻어온다.
	//	TCHAR szContentLength[32];
	//	DWORD dwInfoSize = 32;
	//	
	//	pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL);
	//	nTotalSize += _tstoi(szContentLength);
	//	pFile->Close();
	//	pFile = NULL;
	//}

	pFile = (CHttpFile*)cis.OpenURL(UPDATE_SERVER_SETUP, 1, INTERNET_FLAG_TRANSFER_BINARY | 
		INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 0, 0);

	if(pFile == NULL)
		return 0;

	//파일 길이를 얻어온다.
	TCHAR szContentLength[32];
	DWORD dwInfoSize = 32;

	pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL);
	nTotalSize += _tstoi(szContentLength);
	pFile->Close();
	pFile = NULL;

	return nTotalSize;
}

//프로그레스바 업데이트
void CUpdateWindowDlg::UpdateProg(int nReadSize, CString strFileName)
{
	m_nReadSize += nReadSize;
	m_nPos = m_nReadSize * 100 / m_nSize;
	m_progress.SetPos(m_nPos);
	
	CString str;
	//파일 사이즈 출력
	str.Format("Downloaded... %dbyte of %dbyte", m_nReadSize, m_nSize);
	m_static_Text.SetWindowText(str);
}

void CUpdateWindowDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

LRESULT CUpdateWindowDlg::OnInstallComplete(WPARAM wParam, LPARAM lParam)
{
	if(((int)wParam) == 1)
		EndDialog(IDCANCEL);
	else if(((int)wParam) == 2)
	{
		TCHAR tszPath[MAX_PATH];
		ZeroMemory(tszPath, sizeof(tszPath));
		SHGetSpecialFolderPath(NULL, tszPath,  CSIDL_PROGRAM_FILES, false);
		CString strVerPath;
		strVerPath.Format("%s\\%s\\%s", tszPath, CLIENT_APP_PATH, VERSIONFILE);

		URLDownloadToFile(NULL, UPDATE_SERVER_VER, strVerPath, 0, NULL);
		EndDialog(IDOK);
	}
	
	return 0;
}