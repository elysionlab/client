// UpdateWindowDlg.cpp : ���� ����
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


// CUpdateWindowDlg ��ȭ ����


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
	//�ν��� �޼���
	ON_MESSAGE(WM_USER_INSTALL_END, OnInstallComplete)
END_MESSAGE_MAP()


// CUpdateWindowDlg �޽��� ó����

BOOL CUpdateWindowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetWindowText(CLIENT_UPDATE_TITLE);
	m_nSize = GetUpdateSize();
	m_progress.SetRange(0, 100);
	m_static_Text.SetWindowText("��ġ ������ ���� �޽��ϴ�!");
	SetTimer(1, 100, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CUpdateWindowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CUpdateWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUpdateWindowDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

//������Ʈ ���ϵ� �� ������
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

	//	//���� ���̸� ���´�.
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

	//���� ���̸� ���´�.
	TCHAR szContentLength[32];
	DWORD dwInfoSize = 32;

	pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL);
	nTotalSize += _tstoi(szContentLength);
	pFile->Close();
	pFile = NULL;

	return nTotalSize;
}

//���α׷����� ������Ʈ
void CUpdateWindowDlg::UpdateProg(int nReadSize, CString strFileName)
{
	m_nReadSize += nReadSize;
	m_nPos = m_nReadSize * 100 / m_nSize;
	m_progress.SetPos(m_nPos);
	
	CString str;
	//���� ������ ���
	str.Format("Downloaded... %dbyte of %dbyte", m_nReadSize, m_nSize);
	m_static_Text.SetWindowText(str);
}

void CUpdateWindowDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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