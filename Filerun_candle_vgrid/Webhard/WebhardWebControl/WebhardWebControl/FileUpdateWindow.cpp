#include "stdafx.h"
#include "FileUpdateWindow.h"
#include "DownLoadCallback.h"
#include <IO.h>
#include "Wininet.h"
#include "Helpers/HelperObject.h"

LRESULT FileUpdateWindow::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<FileUpdateWindow>::OnInitDialog(uMsg, wParam, lParam, bHandled);

	SetWindowText(CLIENT_UPDATE_TITLE);
 
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));
	
	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);
	if(!lstrcmp(pPath, ""))		
		lstrcpy(pPath, "C:\\Program Files");

	m_pLocalAppPath = pPath;
	m_pLocalAppPath.Replace("/", "\\");
	m_pLocalAppPath.Format("%s\\%s\\", pPath, CLIENT_APP_PATH);	

	m_pClientVerPath = m_pLocalAppPath + "ver.ini";
	m_pServerVerPath = m_pLocalAppPath + "_ver.ini";
	m_pClientSetPath = m_pLocalAppPath + UPDATE_SETUP;

#pragma warning(disable:4996)
	sprintf(pPath, "%s", m_pLocalAppPath);
	MakeFolder(pPath);
#pragma warning(default:4996)
	

	// 서버 버전파일 다운로드 
	WorkerThreadProc(1);

	CFileFind ff;
	if(!ff.FindFile(m_pServerVerPath))
	{
		::MessageBox(m_hWnd, "서버 버전정보 파일을 열 수 없습니다!   ", "알림", MB_OK);
		EndDialog(IDCANCEL);
		ff.Close();
		return 1;
	}
	else
	{
		CFile fileData;
		int  num;
		char cbuf[100];
		ZeroMemory(cbuf, sizeof(cbuf));
		fileData.Open(m_pServerVerPath, CFile::modeRead);
		num = fileData.Read(cbuf, sizeof(cbuf));
		ff.Close();
		fileData.Close();
		m_pVersionRemote = cbuf;
	}


	if(!ff.FindFile(m_pClientVerPath))
	{		
		ff.Close();
		SetTimer(1, 100, NULL);
		return 1;
	}
	else
	{
		CFile fileData;
		int  num;
		char cbuf[100];
		ZeroMemory(cbuf, sizeof(cbuf));

		fileData.Open(m_pClientVerPath, CFile::modeRead);
		num = fileData.Read(cbuf, sizeof(cbuf));

		ff.Close();
		fileData.Close();

		m_pVersionLocal = cbuf;
		
		// 로컬 버전이 최신 버전일때
		if(m_pVersionRemote == m_pVersionLocal)
		{
			EndDialog(IDOK);
			return 1;
		}
		// 최신 버전이 존재할때
		else
		{
			SetTimer(1, 100, NULL);
			return 1;
		}
	}

	return 1; 
}


LRESULT FileUpdateWindow::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	int		dummy;
	HANDLE	hTrans;

	KillTimer(1);

	HWND hWndProgress = GetDlgItem(IDC_PROGRESS1);	
	m_pProgress.Attach(hWndProgress);	
	m_pProgress.SetRange(0, 100);  
	m_pProgress.SetPos(0);  		
	m_pProgress.SetStep(1);
	m_pProgress.Detach();	


	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcNewVersion, this, 0, (unsigned int *)&dummy);
	CloseHandle(hTrans);
	return 0;
}


LRESULT FileUpdateWindow::OnInstallComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(((int)wParam) == 1)
	{
		try
		{			
			CFile::Remove(m_pClientVerPath);
			CFile::Remove(m_pServerVerPath);
		}
		catch(...)
		{
		}

		EndDialog(IDCANCEL);
		return 0;
	}
	else if(((int)wParam) == 2)
	{
		// 현재 버전 업데이트
		CFileFind ff;
		CFile fileData;

		if(!ff.FindFile(m_pClientVerPath))
		{
			fileData.Open(m_pClientVerPath, CFile::modeCreate | CFile::modeWrite);
			fileData.Write(m_pVersionRemote, m_pVersionRemote.GetLength());

			ff.Close();
			fileData.Close();
		}
		else
		{
			fileData.Open(m_pClientVerPath, CFile::modeWrite );		
			fileData.Write(m_pVersionRemote, m_pVersionRemote.GetLength());

			ff.Close();
			fileData.Close();
		}
	}

	EndDialog(IDOK);
	return 0;
}


void FileUpdateWindow::DownState(TCHAR* strState , const int nPercentDone)
{
	HWND hWndStatic = GetDlgItem(IDC_STATIC_STATE);
	HWND hWndProgress = GetDlgItem(IDC_PROGRESS1);
	
	m_pCtrl.Attach(hWndStatic);
	m_pProgress.Attach(hWndProgress);	

	m_pCtrl.SetWindowText(strState);
	m_pCtrl.UpdateWindow();
	m_pProgress.SetPos(nPercentDone);

	m_pCtrl.Detach();
	m_pProgress.Detach();	
}

void FileUpdateWindow::WorkerThreadProc(int nMode)
{
	CCallback pCallBack;
	HRESULT   hr;
    pCallBack.m_pDlg = this;
	
	if(nMode == 1)
	{
		DeleteUrlCacheEntry(UPDATE_SERVER_VER);
		hr = URLDownloadToFile(NULL, UPDATE_SERVER_VER, m_pServerVerPath, 0, NULL);
	}
	else	
	{
		try
		{			
			CFile::Remove(m_pClientSetPath);
		}
		catch(...)
		{
		}

		DeleteUrlCacheEntry(UPDATE_SERVER_SETUP);
		hr = URLDownloadToFile(NULL, UPDATE_SERVER_SETUP, m_pClientSetPath, 0, &pCallBack);
	}

    if(SUCCEEDED(hr))
	{
		if(nMode == 2)
		{
			HWND hWndStatic = GetDlgItem(IDC_STATIC_STATE);
			m_pCtrl.Attach(hWndStatic);
			m_pCtrl.SetWindowText("설치중입니다..");
			m_pCtrl.UpdateWindow();
			m_pCtrl.Detach();

			ShellExecute(m_hWnd, NULL, m_pClientSetPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
    else
    {
		if(nMode == 1)
			::MessageBox(m_hWnd, "버전파일 다운로드에 실패했습니다!   ", "알림", MB_OK);
		else 
			::MessageBox(m_hWnd, "설치파일 다운로드에 실패했습니다!   ", "알림", MB_OK);
		EndDialog(IDCANCEL);
    }
}


unsigned int __stdcall ProcNewVersion(void *pParam)
{	
	FileUpdateWindow *pWindown = (FileUpdateWindow*)pParam;
	pWindown->WorkerThreadProc(2);
		
	return 1;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 로컬 폴더 생성
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FileUpdateWindow::MakeFolder(char* pszDir)
{
    bool bRet = false;
    int nLen = (int)strlen(pszDir);
    char* pszSubDir = NULL;

    _finddata_t fdata;
    long hFind;
    for(int i = nLen - 1; i >= 0; i--)
    {
		if(pszDir[i] == '\\' && pszDir[i-1] != ':')
        {			
            pszSubDir = new char[i+1];
            memset(pszSubDir, 0, i+1);
            memcpy(pszSubDir, pszDir, i);
            if(hFind = _findfirst(pszSubDir, &fdata) == -1L)
            {
                if(!MakeFolder(pszSubDir))
                {
                    delete pszSubDir;
                    return bRet;
                }
            }
            delete pszSubDir;
            break;
        }
    }
#pragma warning(disable:4800)
    bRet = (bool)::CreateDirectory(pszDir, NULL);
#pragma warning(default:4800)
    return bRet;
}
