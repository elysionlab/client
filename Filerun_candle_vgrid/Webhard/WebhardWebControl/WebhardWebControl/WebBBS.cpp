// WebBBS.cpp : CWebBBS�� �����Դϴ�.
#include "stdafx.h"
#include "WebBBS.h"

#include <shlobj.h>
//#include <knownfolders.h>
#include "Registry.h"
#include "UploadSelect.h"
#include "FileUpdateWindow.h"
#include <comutil.h>
#include <locale.h>
#include <string>
#include <sys/stat.h>
#include <IO.h>
//�߰�, jyh
#include "Helpers/HelperObject.h"
#include <strsafe.h>
//�߰� (avi������ �̹�����)
#include "MtoI_Module.h"
#include "codec_FileUpdateWindow.h"


//20080514 �����ڱ��� ���� üũ, jyh
typedef BOOL (WINAPI* LPIsUserAnAdmin)(void);
//20080514 ������ �ִ� ActiveX ��Ʈ���� ����, jyh
typedef HRESULT (WINAPI *REMOVECONTROLBYNAME)(
	LPCTSTR lpszFile,			//��Ʈ�� ���� ���� (���� OCX ���� ) �� ���� ��θ� ����
	LPCTSTR lpszCLSID,			//CLSID ���ڿ�
	LPCTSTR lpszTypeLibID,		//��Ʈ���� ���� ���̺귯�� CLSID �� NULL ��������
	BOOL bForceRemove,			//���� ���� ����
	DWORD dwIsDistUnit			//�����Ϸ��� ��Ʈ���� ���������� �Ϻκ����� ����
	);

// CWebBBS
#pragma warning(disable:4995)


STDMETHODIMP CWebBBS::CheckInstall(void)
{
	Fire_InstallCompleteEvent();

	return S_OK;
}

HRESULT CWebBBS::FileVersionCheck()
{
	HRESULT hr;

	if(FindDownWindow() != NULL || FindUpWindow() != NULL)
		return S_OK;

	FileUpdateWindow pFileUpdateWindow;
	pFileUpdateWindow.m_pMain = this;	
	INT_PTR nResponse = pFileUpdateWindow.DoModal();
	if(nResponse == IDOK)
	{
		hr = S_OK;
	}
	else if(nResponse == IDCANCEL)
	{
		hr = S_FALSE;
	}

	return hr;
}

HWND CWebBBS::FindDownWindow()
{
	HWND hCheckWnd = ::FindWindow(CLIENT_DOWN_CLASS, NULL);
	if(hCheckWnd)
		return hCheckWnd;
	else
		return NULL;
}

HWND CWebBBS::FindUpWindow()
{
	HWND hCheckWnd = ::FindWindow(CLIENT_UP_CLASS, NULL);
	if(hCheckWnd)
		return hCheckWnd;
	else
		return NULL;
}

STDMETHODIMP CWebBBS::AllowPopUp(BSTR strTemp)
{
	USES_CONVERSION;
	LPCSTR pStr = OLE2CA(strTemp);

	CRegistry pReg;	
	pReg.SaveKey("Software\\Microsoft\\Internet Explorer\\New Windows\\Allow", (LPCTSTR)(LPSTR)pStr, (DWORD)0);
	return S_OK;
}

//20080128 �ٷΰ��� ��â���� �ߵ��� ����, jyh
STDMETHODIMP CWebBBS::MakeShortCut(BSTR strURL)
{
	USES_CONVERSION;
	LPCSTR szArgument = OLE2CA(strURL);

	HRESULT hr;
	IShellLink *pSl;
	IPersistFile *pPf;
	WCHAR wszLnkFile[1024] = {0,};
	char szSrcFile[1024];
	char szLnkFile[1024];
	char szSystemPath[1024] = {0,};
	char szIconPath[1024] = {0,};
	char szDesc[1024];
	TCHAR strTemp[1024];
	CString strFilePathTarget;
	CString strExplorePath;
	CFileFind ff;
	char	szFilePathIcon[1024];
	CString IconUrl = ICONURL;

	//20080425 ��Ÿ���� üũ, jyh
	if(CHelperObject::IsVistaOS()) 
	{
		//20080514 ������ �������� ���� ���̸� ���� ������� �ٷΰ��� ����, jyh 
		LPIsUserAnAdmin IsUserAnAdmin = NULL;
		IsUserAnAdmin = (LPIsUserAnAdmin)GetProcAddress(GetModuleHandle("shell32"), "IsUserAnAdmin");

		if(IsUserAnAdmin)
		{
			if(!IsUserAnAdmin())
			{
				CString strLocalPath;
				PWSTR pwPath = NULL;
				char pPath[MAX_PATH];
				ZeroMemory(pPath, sizeof(pPath));
				//20080424 ��Ÿ�� ���� ���ϴ� �Լ�(SHGetKnownFolderPath())�� ���� ����, jyh
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

				strLocalPath.Format("%s\\%s", pPath, CREATESHORTCUTFILENAME);
				DeleteFile(strLocalPath);
				::DeleteUrlCacheEntry(CREATESHORTCUTURL); 

				hr = URLDownloadToFile(NULL, CREATESHORTCUTURL, strLocalPath, 0, NULL);

				if(hr == S_OK)
				{
					//AfxMessageBox("CreateShortCut.exe �ٿ� ����!");
					::ShellExecute(m_hWnd, NULL, strLocalPath, URL_HOMEPAGE, NULL, SW_SHOWNORMAL);
					Sleep(3000);

					return S_OK;
				}
				else
					return S_FALSE;
			}			
		}		
	}

	//20080103 �������� ������ ���� �޴´�, jyh	---------------------//
	::GetSystemDirectory(szFilePathIcon,sizeof(szFilePathIcon));
	sprintf_s(szFilePathIcon, 1024, "%s\\%s", szFilePathIcon, ICONNAME);

	if(!ff.FindFile(szFilePathIcon))
		URLDownloadToFile(NULL, IconUrl, szFilePathIcon, 0, NULL);
	//----------------------------------------------------------------//

	//IShellLink ��ü�� �����ϰ� �����͸� ���Ѵ�.
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pSl);

	if(FAILED(hr))
		return S_FALSE;

	// ����ȭ�� ��ġ
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
		return S_FALSE;
	}

	//�����ڵ�� �����н� ������ ����
	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, 1024);
	hr = pPf->Save(wszLnkFile, TRUE);

	//��ü ����
	pPf->Release();


	// ���۸޴�
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_STARTMENU, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	strFilePathTarget.Format("%s\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);
	//������ ���ü�� ���� ����
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
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
	//SHGetSpecialFolderPath(NULL, (LPSTR)(LPCTSTR)strFilePathTarget, CSIDL_APPDATA, FALSE);
	SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);
	//GetSystemDirectory(szSystemPath, 1024);
	//strcat_s(szSystemPath, 1024, "\\Gfile16.ico");
	//������ ���ü�� ���� ����
	strcpy_s(szLnkFile, strFilePathTarget.GetLength()+1, strFilePathTarget.GetBuffer());
	strFilePathTarget.ReleaseBuffer();
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
	pSl->Release();

	CFile	pFile;
	CString strFileData;

	strFileData.Format("[InternetShortcut]\r\nURL=%s\r\nIconIndex=0\r\nIconFile=%s", szArgument, szFilePathIcon);

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

	//if(ff.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// ����ġ
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_APPDATA, FALSE);
	//strFilePathTarget.Format("%s\\Microsoft\\Internet Explorer\\Quick Launch\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);

	//if(ff.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	//// ���۸޴�
	//SHGetSpecialFolderPath(NULL, strTemp, CSIDL_STARTMENU, FALSE);
	//strFilePathTarget.Format("%s\\%s.lnk", strTemp, CLIENT_SERVICE_NAME);

	//if(ff.FindFile(strFilePathTarget))
	//	DeleteFile(strFilePathTarget);

	return S_OK;
}

STDMETHODIMP CWebBBS::ShowDlgFile(void)
{
	UploadSelect pUploadSelect;
	pUploadSelect.m_pMain = this;	
	pUploadSelect.ShowDlgFile();
	return S_OK;
}

STDMETHODIMP CWebBBS::ShowDlgFolder(void)
{
	UploadSelect pUploadSelect;
	pUploadSelect.m_pMain = this;	
	pUploadSelect.ShowDlgFolder();
	return S_OK;
}

STDMETHODIMP CWebBBS::SelectComplete(BSTR strPath, BSTR strSize, BSTR strMode, BSTR strMD5)
{
	Fire_UpSelectComplete(strPath, strSize, strMode, strMD5);	
	return S_OK;
}

//20071217 ���۱� ��� ������ �������� ���� ���� BSTR strHashIdx �߰�, jyh
STDMETHODIMP CWebBBS::UpLoadHash(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx, BSTR strHashIdx)
{
	USES_CONVERSION;

	bool bFindWindow = false;

	/*if(!FileVersionCheck())
	return S_FALSE;*/
	//20071205 ��Ÿ ���� ��� ����, jyh
	if(VersionCheck())
		return S_FALSE;

	HWND hUpWnd = FindUpWindow();

	if(hUpWnd == NULL)
	{
		CString pAppPathExe, pParam;
		pAppPathExe = m_pAppPath + CLIENT_UP_NAME;
		pParam.Format("%s %s", OLE2CA(strUserID), OLE2CA(strUserPW));
		::ShellExecute(NULL, NULL, pAppPathExe, pParam, NULL, SW_SHOWNORMAL);
		Sleep(500);
	}

	for(int i = 0; i < 30; i++)
	{
		hUpWnd = FindUpWindow();

		if(hUpWnd)
		{
			bFindWindow = true;

			CString pItem;

			//20071217 ���۱� ������� ���� hash_copyright �ε��� �߰�, jyh
			pItem.Format("%s%s%s%s%s", OLE2CA(strType), SEP_FIELD, OLE2CA(strIdx), SEP_FIELD, OLE2CA(strHashIdx));

			COPYDATASTRUCT pCopyData;
			pCopyData.dwData = 0;
			pCopyData.cbData = pItem.GetLength() + 1;			
			pCopyData.lpData = (LPSTR)(LPCTSTR)pItem;
			if(::SendMessage(hUpWnd, WM_COPYDATA, (WPARAM)this, (LPARAM)(LPVOID)&pCopyData) == 1)
				break;
		}
		else
		{
			if(bFindWindow) 
				break;
		}
		Sleep(300);
	}
	return S_OK;
}

STDMETHODIMP CWebBBS::UpLoad(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx)
{
	USES_CONVERSION;

	bool bFindWindow = false;

	/*if(!FileVersionCheck())
	return S_FALSE;*/
	//20071205 ��Ÿ ���� ��� ����, jyh
	if(VersionCheck())
		return S_FALSE;

	HWND hUpWnd = FindUpWindow();

	if(hUpWnd == NULL)
	{
		CFileFind ff;
		CString pAppPathExe, pParam;
		pAppPathExe = m_pAppPath + CLIENT_UP_NAME;
		pParam.Format("%s %s", OLE2CA(strUserID), OLE2CA(strUserPW));

		for(int i=0; i<30; i++)
		{
			if(ff.FindFile(pAppPathExe))
			{
				::ShellExecute(NULL, NULL, pAppPathExe, pParam, NULL, SW_SHOWNORMAL);
				ff.Close();
				Sleep(500);
				break;
			}

			Sleep(300);
		}		
	}

	for(int i = 0; i < 30; i++)
	{
		hUpWnd = FindUpWindow();

		if(hUpWnd)
		{
			bFindWindow = true;

			CString pItem;

			pItem.Format("%s%s%s", OLE2CA(strType), SEP_FIELD, OLE2CA(strIdx));

			COPYDATASTRUCT pCopyData;
			pCopyData.dwData = 0;
			pCopyData.cbData = pItem.GetLength() + 1;			
			pCopyData.lpData = (LPSTR)(LPCTSTR)pItem;
			if(::SendMessage(hUpWnd, WM_COPYDATA, (WPARAM)this, (LPARAM)(LPVOID)&pCopyData) == 1)
				break;
		}
		else
		{
			if(bFindWindow) 
				break;
		}
		Sleep(300);
	}
	return S_OK;
}

//20080221 ���� ���� �ε��� ���� �߰�, jyh
STDMETHODIMP CWebBBS::DownLoad(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx, BSTR strIdx_, BSTR strSeverIdx, BSTR strNameAuth)
{
	USES_CONVERSION;

	bool bFindWindow = false;

	/*if(!FileVersionCheck())
	return S_FALSE;*/
	//20071205 ��Ÿ ���� ��� ����, jyh
	if(VersionCheck())
		return S_FALSE;

	HWND hDownWnd = FindDownWindow();

	if(hDownWnd == NULL)
	{
		CString pAppPathExe, pParam;
		pAppPathExe = m_pAppPath + CLIENT_DOWN_NAME;
		//20080314 �Ǹ� ������ �߰�, jyh
		//pParam.Format("%s %s", OLE2CA(strUserID), OLE2CA(strUserPW));
		pParam.Format("%s %s %s", OLE2CA(strUserID), OLE2CA(strUserPW), OLE2CA(strNameAuth));

		CFileFind ff;

		for(int i=0; i<30; i++)
		{
			if(ff.FindFile(pAppPathExe))
			{
				::ShellExecute(NULL, NULL, pAppPathExe, pParam, NULL, SW_SHOWNORMAL);
				ff.Close();
				Sleep(500);
				break;
			}

			Sleep(300);
		}		
	}

	for(int i = 0; i < 30; i++)
	{
		hDownWnd = FindDownWindow();

		if(hDownWnd)
		{
			bFindWindow = true;

			CString pItem;
			//20080221 ���� ���� �ε����� �߰��ؼ� ��Ʈ�ѿ� ������, jyh
			pItem.Format("%s%s%s%s%s%s%s", OLE2CA(strType), SEP_FIELD, OLE2CA(strIdx_), SEP_FIELD, OLE2CA(strIdx), SEP_FIELD, OLE2CA(strSeverIdx));

			COPYDATASTRUCT pCopyData;
			pCopyData.dwData = 0;
			pCopyData.cbData = pItem.GetLength() + 1;			
			pCopyData.lpData = (LPSTR)(LPCTSTR)pItem;
			if(::SendMessage(hDownWnd, WM_COPYDATA, (WPARAM)this, (LPARAM)(LPVOID)&pCopyData) == 1)
				break;
		}
		else
		{
			if(bFindWindow) 
				break;
		}
		Sleep(300);
	}
	return S_OK;
}

//20071105 ���� �ڷ�� ���ε� �Լ�, jyh
STDMETHODIMP CWebBBS::OpenUpLoad(BSTR file_name)
{
	if(VersionCheck())
		return S_FALSE;

	CString strFileName;
	char FileName[10240];
	ZeroMemory(FileName, sizeof(FileName));

	::WideCharToMultiByte(CP_ACP, 0, file_name, -1, FileName, 10240, NULL, NULL);

	//20080425 ��Ÿ �������� ����, jyh
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));

	Sleep(500);
	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);
	strFileName.Format("%s\\%s\\%s", pPath, CLIENT_APP_PATH, UPLOADER_EXE);
	::ShellExecute(NULL, NULL, strFileName, (LPCTSTR)FileName, NULL, SW_SHOWNORMAL);

	//CString path, file;

	//if(GetSystemDirectory(path.GetBuffer(MAX_PATH), MAX_PATH))
	//	file.Format("%s\\%s", path, UPLOADER_EXE);

	//CFileFind ff;

	//if(ff.FindFile(file))
	//{
	//	::ShellExecute(NULL, NULL, (LPCTSTR)file, (LPCTSTR)FileName, NULL, SW_SHOWNORMAL);
	//	Sleep(500);
	//}
	//else
	//{
	//	CString strTemp;
	//	strTemp.Format("%s ������ ã�� �� �����ϴ�!", UPLOADER_EXE);
	//	AfxMessageBox(strTemp);
	//	return 0;
	//}

	return 0;
}

//20071105 ���� �ڷ�� �ٿ�ε� �Լ�, jyh
STDMETHODIMP CWebBBS::OpenDownLoad(BSTR file_name)
{
	if(VersionCheck())
		return S_FALSE;

	CString strFileName;
	char FileName[10240];
	ZeroMemory(FileName, sizeof(FileName));

	::WideCharToMultiByte(CP_ACP, 0, file_name, -1, FileName, 10240, NULL, NULL);

	//20080425 ��Ÿ �������� ����, jyh
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));

	Sleep(500);
	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);
	strFileName.Format("%s\\%s\\%s", pPath, CLIENT_APP_PATH, DOWNLOADER_EXE);
	::ShellExecute(NULL, NULL, strFileName, (LPCTSTR)FileName, NULL, SW_SHOWNORMAL);

	//CString path, file;

	//if(GetSystemDirectory(path.GetBuffer(MAX_PATH), MAX_PATH))
	//	file.Format("%s\\%s", path, DOWNLOADER_EXE);

	//CFileFind ff;

	//if(ff.FindFile(file))
	//{
	//	::ShellExecute(NULL, NULL, (LPCTSTR)file, (LPCTSTR)FileName, NULL, SW_SHOWNORMAL);
	//	Sleep(500);
	//}
	//else
	//{
	//	CString strTemp;
	//	strTemp.Format("%s ������ ã�� �� �����ϴ�!", DOWNLOADER_EXE);
	//	AfxMessageBox(strTemp);
	//	return 0;
	//}

	return 0;
}

STDMETHODIMP CWebBBS::UpdateOpenCheck(void)
{
	CString sysdir;
	CString OpenUploader;
	CString OpenDownloader;
	CString checker;

	if(!GetSystemDirectory(sysdir.GetBuffer(MAX_PATH), MAX_PATH))
		sysdir = "c:";

	OpenUploader.Format("%s\\%s", sysdir, UPLOADER_EXE);
	OpenDownloader.Format("%s\\%s", sysdir, DOWNLOADER_EXE);
	checker.Format("%s\\%s", sysdir, CHECKFILE);

	CString OpenUploaderurl = OPENUPLOADERURL;
	CString OpenDownloaderurl = OPENDOWNLOADERURL;
	CString checkurl = CHECKURL;

	CFileFind ff;
	bool bUpdate = false;
	ULONGLONG downlength = 0;

	if(ff.FindFile(checker))
	{
		ff.FindNextFile();
		downlength = ff.GetLength();
	}

	CInternetSession cis;
	CHttpFile* pFile;
	CString strResult;

	pFile = (CHttpFile*)cis.OpenURL(checkurl, 1, INTERNET_FLAG_TRANSFER_BINARY | 
		INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 0, 0);

	if(pFile == NULL)
		return 0;

	//���� ���̸� ���´�.
	TCHAR szContentLength[32];
	DWORD dwInfoSize = 32;

	pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL);
	if(downlength != (ULONGLONG)_ttol(szContentLength))
		bUpdate = true;

	pFile->Close();
	//delete pFile;
	pFile = NULL;

	if(bUpdate)
	{
		DeleteFile(OpenUploader);
		URLDownloadToFile(NULL, OpenUploaderurl, OpenUploader, 0, NULL);

		DeleteFile(OpenDownloader);
		URLDownloadToFile(NULL, OpenDownloaderurl, OpenDownloader, 0, NULL);

		DeleteFile(checker);
		URLDownloadToFile(NULL, checkurl, checker, 0, NULL);
	}

	return 0;
}

//20071112 ���� �ڷ�� ���� ���� ��ȭ����, jyh
STDMETHODIMP CWebBBS::ShowDlgOpenFile(void)
{
	UploadSelect pUploadSelect;
	pUploadSelect.m_pMain = this;	
	pUploadSelect.ShowDlgOpenFile();
	return S_OK;
}

//20071130 ���� �ڷ�� ���� ���� ��ȭ����, jyh
STDMETHODIMP CWebBBS::ShowDlgOpenFolder(void)
{
	UploadSelect pUploadSelect;
	pUploadSelect.m_pMain = this;	
	pUploadSelect.ShowDlgOpenFolder();
	return S_OK;
}

STDMETHODIMP CWebBBS::VersionCheck(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	HRESULT	hr;
	//20071205 OS�� VISTA���� ��, jyh
	if(!CHelperObject::IsVistaOS())
		//VISTA�� �ƴ� ���
		hr = FileVersionCheck();
	else
	{
		//20080514 ������ �������� ���� ���̸� ���� ������Ʈ, jyh 
		LPIsUserAnAdmin IsUserAnAdmin = NULL;
		IsUserAnAdmin = (LPIsUserAnAdmin)GetProcAddress(GetModuleHandle("shell32"), "IsUserAnAdmin");

		if(IsUserAnAdmin)
		{
			if(IsUserAnAdmin())
				hr = FileVersionCheck();
			else
				hr = FileVersionCheckVista();
		}
	}

	////20080514 ������ �ִ� ActiveX ��Ʈ���� ����, jyh---------------------------------------------
	//char	szFileName[1024];
	//TCHAR	strTemp[MAX_PATH];
	//CString strFilePath;
	//HMODULE	hMod;
	//REMOVECONTROLBYNAME	pfn =  NULL;

	//hMod = LoadLibrary("OCCACHE.DLL");

	//if(hMod == NULL)
	//{
	//	// Error loading module -- fail as securely as possible
	//	return hr;
	//}

	//GetWindowsDirectory(strTemp, MAX_PATH);
	//sprintf_s(szFileName, 1024, "%s\\Downloaded Program Files\\%s", strTemp, CLIENT_WEBCTRL_NAME);
	//pfn = (REMOVECONTROLBYNAME)GetProcAddress(hMod, "RemoveControlByName");

	//if(pfn)
	//{
	//	//NfileWebControl2 ����
	//	//(*pfn)(szFileName, "{209739BB-1636-4A43-80B6-4DB48FB2F3E9}", NULL, TRUE, TRUE);
	//}
	//FreeLibrary(hMod);
	////----------------------------------------------------------------------------------------------

	//HRESULT		hr;
	//HWND		hwnd = NULL;
	//BIND_OPTS3*	bo;
	//WCHAR		wszCLSID[50] = {0,};
	//WCHAR		wszMonikerName[300] = {0,};
	//IWebBBS*	objElevator = NULL;
	//const GUID	guidCurrentObject = CLSID_WebBBS;
	//const IID	iidCurrentObject = IID_IWebBBS;

	//CoInitialize(NULL);
	//
	////20071205 OS�� VISTA���� ��, jyh
	//if(!CHelperObject::IsVistaOS())
	//{
	//	//VISTA�� �ƴ� ���
	//	hr = FileVersionCheck();

	//	return hr;
	//}

	//StringFromGUID2(guidCurrentObject, wszCLSID, sizeof(wszCLSID)/sizeof(wszCLSID[0]));
	//hr = StringCchPrintfW(wszMonikerName, sizeof(wszMonikerName)/sizeof(wszMonikerName[0]),
	//	L"Elevation:Administrator!new:%s", wszCLSID);

	//bo = new BIND_OPTS3;
	//memset(bo, 0, sizeof(bo));

	//bo->cbStruct			= sizeof(bo);
	//bo->hwnd				= hwnd;
	//bo->dwClassContext		= CLSCTX_LOCAL_SERVER;

	////
	////ELEVATION
	////

	//hr = CoGetObject(wszMonikerName, bo, iidCurrentObject, (void**)&objElevator);

	////
	////EXECUTE
	////

	//if(SUCCEEDED(hr))
	//{
	//	//AfxMessageBox("���� ��� ����!");
	//	//do work
	//	hr = objElevator->FileVersionCheckVista();
	//	//release
	//	objElevator->Release();
	//	delete bo;
	//	bo = NULL;

	//	return hr;
	//}
	//else
	//{
	//	//AfxMessageBox("���� ��� ����!");
	//	//Elevation ����
	//	delete bo;
	//	bo = NULL;
	//	return S_FALSE;
	//}

	return hr;
}

STDMETHODIMP CWebBBS::FileVersionCheckVista(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	if(FindDownWindow() != NULL || FindUpWindow() != NULL)
		return S_OK;

	HRESULT		hr;
	CString strLocalPath;
	CString strVerPath;
	PWSTR pwPath = NULL;
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));

	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);
	strVerPath.Format("%s\\%s\\ver.ini", pPath, CLIENT_APP_PATH);

	CString strResult;
	CFile file;
	CFileFind ff;

	//SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROFILE, false);
	//20080424 ��Ÿ�� ���� ���ϴ� �Լ�(SHGetKnownFolderPath())�� ���� ����, jyh
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

	/*if(!SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &pwPath))
	{
	hr = StringCchLengthW(pwPath, STRSAFE_MAX_CCH, &len);

	if(SUCCEEDED(hr))
	WideCharToMultiByte(CP_ACP, 0, pwPath, (int)len, pPath, MAX_PATH, NULL, NULL);
	}*/

	//ver.ini�� �ִ��� ���� Ȯ��
	if(ff.FindFile(strVerPath))
	{
		strLocalPath.Format("%s\\%s", pPath, "ver.ini");
		URLDownloadToFile(NULL, UPDATE_SERVER_VER, strLocalPath, 0, NULL);

		CFile ClientVer;
		CFile ServerVer;
		char webVer[32] = {0,};
		char localVer[32] = {0,};
		ZeroMemory(webVer, sizeof(webVer));
		ZeroMemory(localVer, sizeof(localVer));

		ClientVer.Open(strVerPath, CFile::modeRead);
		ClientVer.Read(localVer, sizeof(localVer));
		ClientVer.Close();
		ServerVer.Open(strLocalPath, CFile::modeRead);
		ServerVer.Read(webVer, sizeof(webVer));
		ServerVer.Close();

		if(!lstrcmp(webVer, localVer))
			return S_OK;
	}

	strLocalPath.Format("%s\\%s", pPath, UPDATEWINDOWFILENAME);
	DeleteFile(strLocalPath);
	::DeleteUrlCacheEntry(UPDATEWINDOWURL); 

	hr = URLDownloadToFile(NULL, UPDATEWINDOWURL, strLocalPath, 0, NULL);

	if(hr == S_OK)
	{
		//AfxMessageBox("UpdateWindow.exe �ٿ� ����!");
		//::ShellExecute(m_hWnd, NULL, strLocalPath, NULL, NULL, SW_SHOWNORMAL);
		//Sleep(3000);

		//20090328 UpdateWindow�� ����ɶ��� �޼����� �޾Ƽ� ó��, jyh
		SHELLEXECUTEINFO shi;
		ZeroMemory(&shi, sizeof(SHELLEXECUTEINFO));
		shi.cbSize = sizeof(SHELLEXECUTEINFO);
		shi.lpFile = strLocalPath;
		shi.nShow = SW_SHOW;
		shi.fMask = SEE_MASK_NOCLOSEPROCESS;
		shi.lpVerb = "open";
		ShellExecuteEx(&shi);

		DWORD dwResult = ::WaitForSingleObject(shi.hProcess, 12000);

		while(1)
		{
			if(dwResult == WAIT_OBJECT_0)
			{
				//20090328 ��ġ�Ϸ����� �ߴ������� �˱����� ver.ini�� �˻��Ѵ�, jyh
				if(ff.FindFile(strVerPath))
				{
					strLocalPath.Format("%s\\%s", pPath, "ver.ini");
					URLDownloadToFile(NULL, UPDATE_SERVER_VER, strLocalPath, 0, NULL);

					CFile ClientVer;
					CFile ServerVer;
					char webVer[32] = {0,};
					char localVer[32] = {0,};
					ZeroMemory(webVer, sizeof(webVer));
					ZeroMemory(localVer, sizeof(localVer));

					ClientVer.Open(strVerPath, CFile::modeRead);
					ClientVer.Read(localVer, sizeof(localVer));
					ClientVer.Close();
					ServerVer.Open(strLocalPath, CFile::modeRead);
					ServerVer.Read(webVer, sizeof(webVer));
					ServerVer.Close();

					if(!lstrcmp(webVer, localVer))
						hr = S_OK;		//������Ʈ ����
					else
						hr = S_FALSE;
				}
				else
					hr = S_FALSE;

				break;
			}
			else
				dwResult = ::WaitForSingleObject(shi.hProcess, 12000);
		}
	}
	else
		hr = S_FALSE;

	return hr;
}

#pragma warning(default:4995)


STDMETHODIMP CWebBBS::Movie_to_Images(SHORT load_count, BSTR url_domain, BSTR url_path, BSTR* Save_files_data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	


/*
	CString savefilenames;
	savefilenames="";

	MtoI_Module movie_image;
	//�ʱ� ���� ����.
	movie_image.File_name=(CString)Movie_filename;	//���� ������ ���ϸ�
	movie_image.Save_domain=(CString)url_domain;	//������ ������
	movie_image.Save_url=(CString)url_path;			//������ ��ġ
	movie_image.Save_image_count=(int)load_count;	//���尳��

	//110610-�߰�
	movie_image.Save_image_show_info=0;	//����ǥ������
	movie_image.Save_image_show_size=0;	//�ð�ǥ������

	//���Ű� üũ
	if(movie_image.File_name.GetLength()&&movie_image.Save_domain.GetLength()&&movie_image.Save_url.GetLength()&&movie_image.Save_image_count)
	{
		Ccodec_FileUpdateWindow pFileUpdateWindow;
		pFileUpdateWindow.m_pMain = this;
		INT_PTR nResponse_codec = pFileUpdateWindow.DoModal();
		if(nResponse_codec == IDOK)
		{
		}

		//���� ����
		INT_PTR nResponse = movie_image.DoModal();
		if(!nResponse)
		{
			*Save_files_data=movie_image.Save_IMAGE_File_name.AllocSysString();
		}
	}
	else
	{
		::MessageBox(m_hWnd, "�����Ҽ� ���� �������Դϴ�.", "����", MB_OK);
	}

*/


/*
	CString Open_file_title,savefilenames;
	char szFiilter[] = "���AVI���� (*.avi)|*.avi|";

	CFileDialog Files_dlg(TRUE, "*.avi", NULL, OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_EXPLORER|OFN_FILEMUSTEXIST, szFiilter,NULL);
	Open_file_title="�̹����� �ε��� AVI������ �����ϼ���.."; 

	char buffer[15360] = {0}; //����
	Files_dlg.m_ofn.lpstrTitle = Open_file_title;
    Files_dlg.m_ofn.lpstrFile = buffer; //���� ������
    Files_dlg.m_ofn.nMaxFile = 15360; //���� ũ�� 

	savefilenames="";

	if(IDOK == Files_dlg.DoModal())
	{
		Movie_to_Image movie_image;
		savefilenames=movie_image.Save_File(Files_dlg.GetPathName(),(int)load_count,(CString)url_domain,(CString)url_path);
	}
	*Save_files_data=savefilenames.AllocSysString();
*/
	return S_OK;
}

STDMETHODIMP CWebBBS::MtoI_exe(BSTR Movie_filename, SHORT load_count, BSTR url_domain, BSTR url_path)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

	return S_OK;
}

STDMETHODIMP CWebBBS::Movie_Capture(BSTR Movie_filename, SHORT load_count, BSTR url_domain, BSTR url_path, SHORT Image_show_info, SHORT Image_show_size, BSTR* Save_files_data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

	CString savefilenames;
	savefilenames="";

	MtoI_Module movie_image;
	//�ʱ� ���� ����.
	movie_image.File_name=(CString)Movie_filename;	//���� ������ ���ϸ�
	movie_image.Save_domain=(CString)url_domain;	//������ ������
	movie_image.Save_url=(CString)url_path;			//������ ��ġ
	movie_image.Save_image_count=(int)load_count;	//���尳��

	//110610-�߰�
	movie_image.Save_image_show_info=0;	//����ǥ������
	movie_image.Save_image_show_size=0;	//�ð�ǥ������

	//���Ű� üũ
	if(movie_image.File_name.GetLength()&&movie_image.Save_domain.GetLength()&&movie_image.Save_url.GetLength()&&movie_image.Save_image_count)
	{
		Ccodec_FileUpdateWindow pFileUpdateWindow;
		pFileUpdateWindow.m_pMain = this;
		INT_PTR nResponse_codec = pFileUpdateWindow.DoModal();
		if(nResponse_codec == IDOK)
		{
		}

		//���� ����
		INT_PTR nResponse = movie_image.DoModal();
		if(!nResponse)
		{
			*Save_files_data=movie_image.Save_IMAGE_File_name.AllocSysString();
		}
	}
	else
	{
		::MessageBox(m_hWnd, "�����Ҽ� ���� �������Դϴ�.", "����", MB_OK);
	}
	return S_OK;
}

STDMETHODIMP CWebBBS::DownLoads(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx, BSTR strIdx_, BSTR strSeverIdx, BSTR strNameAuth, BSTR strFileAuth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here












	USES_CONVERSION;

	bool bFindWindow = false;

	/*if(!FileVersionCheck())
	return S_FALSE;*/
	//20071205 ��Ÿ ���� ��� ����, jyh
	if(VersionCheck())
		return S_FALSE;

	HWND hDownWnd = FindDownWindow();

	if(hDownWnd == NULL)
	{
		CString pAppPathExe, pParam;
		pAppPathExe = m_pAppPath + CLIENT_DOWN_NAME;
		//20080314 �Ǹ� ������ �߰�, jyh
		//pParam.Format("%s %s", OLE2CA(strUserID), OLE2CA(strUserPW));
		pParam.Format("%s %s %s", OLE2CA(strUserID), OLE2CA(strUserPW), OLE2CA(strNameAuth));

		CFileFind ff;

		for(int i=0; i<30; i++)
		{
			if(ff.FindFile(pAppPathExe))
			{
				::ShellExecute(NULL, NULL, pAppPathExe, pParam, NULL, SW_SHOWNORMAL);
				ff.Close();
				Sleep(500);
				break;
			}

			Sleep(300);
		}		
	}

	for(int i = 0; i < 30; i++)
	{
		hDownWnd = FindDownWindow();

		if(hDownWnd)
		{
			bFindWindow = true;

			CString pItem;
			//20080221 ���� ���� �ε����� �߰��ؼ� ��Ʈ�ѿ� ������, jyh
			pItem.Format("%s%s%s%s%s%s%s%s%s",	OLE2CA(strType), SEP_FIELD, 
												OLE2CA(strIdx_), SEP_FIELD, 
												OLE2CA(strIdx), SEP_FIELD, 
												OLE2CA(strSeverIdx), SEP_FIELD, 
												OLE2CA(strFileAuth)
											);

			COPYDATASTRUCT pCopyData;
			pCopyData.dwData = 5;
			pCopyData.cbData = pItem.GetLength() + 1;			
			pCopyData.lpData = (LPSTR)(LPCTSTR)pItem;
			if(::SendMessage(hDownWnd, WM_COPYDATA, (WPARAM)this, (LPARAM)(LPVOID)&pCopyData) == 1)
				break;
		}
		else
		{
			if(bFindWindow) 
				break;
		}
		Sleep(300);
	}











	return S_OK;
}
