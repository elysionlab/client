// CreateShortCutDlg.cpp : ���� ����
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
#define SHORTCUTNAME			"���Ϸ�"
#define HOMEURL					"http://filerun.co.kr"
#define CLIENT_APP_PATH			"Filerun"

// CCreateShortCutDlg ��ȭ ����




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


// CCreateShortCutDlg �޽��� ó����

BOOL CCreateShortCutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CString strTemp = AfxGetApp()->m_lpCmdLine;
	CoInitialize(0);
	CreateShortCut(strTemp.GetBuffer());
	strTemp.ReleaseBuffer();
	CoUninitialize();
	PostQuitMessage(0);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CCreateShortCutDlg::OnPaint()
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
	

	//20080103 �������� ������ ���� �޴´�, jyh
	//::GetSystemDirectory(szPath, 1024);
	//sprintf_s(szFilePathIcon, 1024, "%s\\%s", szPath, ICONNAME);
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES, FALSE);
	sprintf_s(szFilePathIcon, 1024, "%s\\%s\\%s", szPath,CLIENT_APP_PATH, ICONNAME);
	
	if(!ff.FindFile(szFilePathIcon))
		URLDownloadToFile(NULL, ICONURL, szFilePathIcon, 0, NULL);

	//IShellLink ��ü�� �����ϰ� �����͸� ���Ѵ�.
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pSl);

	if(FAILED(hr))
		return FALSE;

	// ����ȭ�� ��ġ
	//20080425 ntdll.dll���� ����, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_DESKTOP, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_PROGRAM_FILES, FALSE);
	strExplorePath.Format("%s\\Internet Explorer\\iexplore.exe", strTemp);
	//GetSystemDirectory(szSystemPath, MAX_PATH);
	//strcat_s(szSystemPath, 1024, "\\Nfile.ico");
	//������ ���ü�� ���� ����
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
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);
	//������ ���ü�� ���� ����
	//lstrcpy(szSrcFile, "C:\\Program Files\\Internet Explorer\\iexplore.exe");
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
	//lstrcpy(szDesc, "gfile.co.kr");
	//lstrcpy(szArgument, strURL);
	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);
	pSl->SetIconLocation(szFilePathIcon, 0);
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//����Ű F4

	//�����ϱ� ���� IPersistFile ��ü ����
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//�����ڵ�� �����н� ������ ����
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, MAX_PATH);
	hr = pPf->Save(wszLnkFile, TRUE);

	//��ü ����
	pPf->Release();


	// ����ġ
	//20080425 ntdll.dll���� ����, jyh
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_APPDATA, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, SHORTCUTNAME);
	//GetSystemDirectory(szSystemPath, MAX_PATH);
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
	pSl->SetIconLocation(szFilePathIcon, 0);	//20080331 �������� �������� 16*16 ���������� ����, jyh
	//pSl->SetHotkey(HOTKEYF_CONTROL+HOTKEYF_ALT+0x6D);	//����Ű F4

	//�����ϱ� ���� IPersistFile ��ü ����
	hr = pSl->QueryInterface(IID_IPersistFile, (void**)&pPf);

	if(FAILED(hr))
	{
		pSl->Release();
		return FALSE;
	}

	//�����ڵ�� �����н� ������ ����
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, MAX_PATH);
	hr = pPf->Save(wszLnkFile, TRUE);

	//��ü ����
	pPf->Release();
	pSl->Release();

	//20080910 ���ã�⿡ �߰�, jyh
	strFileData.Format("[InternetShortcut]\r\nURL=%s\r\nIconIndex=0\r\nIconFile=%s", strURL, szFilePathIcon);
	
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_FAVORITES, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_FAVORITES, FALSE);
	strFilePathTarget.Format("%s\\%s.url", strTemp, SHORTCUTNAME);
	pFile.Open(strFilePathTarget, CFile::modeCreate | CFile::modeWrite);
	pFile.Write(strFileData, strFileData.GetLength());
	pFile.Close();

	////���� �ٷΰ��� ����
	////����ȭ��
	//CFileFind findfile;

	////SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_DESKTOP, FALSE);
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_DESKTOP, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// ����ġ
	////SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_APPDATA, FALSE);
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	//strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, SHORTCUTNAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// ���۸޴�
	////SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_STARTMENU, FALSE);
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, SHORTCUTNAME);

	//if(findfile.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	return TRUE;
}