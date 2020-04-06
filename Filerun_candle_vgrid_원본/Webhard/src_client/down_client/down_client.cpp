#include "stdafx.h"
#include "down_client.h"
#include "use_window.h"
#include <strsafe.h>
#include "Wininet.h"


BEGIN_MESSAGE_MAP(down_client, CWinApp)
END_MESSAGE_MAP()

down_client::down_client()
{
}



down_client theApp;



BOOL down_client::InitInstance()
{
	WNDCLASS    wndClass;
	::GetClassInfo(AfxGetInstanceHandle(), "#32770", &wndClass);
    wndClass.lpszClassName = CLIENT_DOWN_CLASS;
    AfxRegisterClass(&wndClass);


	//20080424 vs8로 컨버팅으로 인한 수정, jyh
	/*if(!AfxSocketInit())
	{
		MessageBox(NULL, "소켓초기화 에러     ", CLIENT_SERVICE_NAME, MB_OK|MB_ICONSTOP);
		return FALSE;
	}*/

	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();

	int nCheckCount = 0;
	static const char szProgName[] = CLIENT_DOWN_MUTEX;
    CreateMutex(NULL, TRUE, szProgName);

	
//121026 - IE제외한 브라우져 다운로드 - S 
	CString Cmd_line_argv;
	Cmd_line_argv = (LPCTSTR)GetCommandLine();
	Cmd_line_argv = ::PathGetArgs(Cmd_line_argv);

	CToken* pToken = new CToken(" ");	
	pToken->Split((LPSTR)(LPCTSTR)Cmd_line_argv);	
//121026 - IE제외한 브라우져 다운로드 - E


	while(1)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			if(nCheckCount== 3)
			{
				return FALSE;
			}
			else
			{

//121026 - IE제외한 브라우져 다운로드 - S 
				//121026 - IE제외한 브라우져 다운로드 - S 
				if(pToken->GetCount() == 2)		//기타브라우져에서 실행
				{
					//링크값을 토큰으로 변화
					CToken* url_pToken = new CToken("/");	
					//토큰구분처리
					url_pToken->Split((LPSTR)(LPCTSTR)pToken->GetToken(1));	

					//실행된 프로세스 검색
					HWND hDownWnd = ::FindWindow(CLIENT_DOWN_CLASS, NULL);
					if(hDownWnd)
					{

						CString pItem;

						//자료를 처리
						pItem.Format("%s%s%s%s%s%s%s%s%s",	"0",	SEP_FIELD, 
																	url_pToken->GetToken(4), SEP_FIELD, 
																	url_pToken->GetToken(3), SEP_FIELD, 
																	url_pToken->GetToken(5), SEP_FIELD, 
																	url_pToken->GetToken(6)
						);

						COPYDATASTRUCT pCopyData;
						pCopyData.dwData = 5;
						pCopyData.cbData = pItem.GetLength() + 1;
						pCopyData.lpData = (LPSTR)(LPCTSTR)pItem;
						::SendMessage(hDownWnd, WM_COPYDATA, (WPARAM)this, (LPARAM)(LPVOID)&pCopyData);
					}
					return FALSE;
				}
//121026 - IE제외한 브라우져 다운로드 - E 


				nCheckCount ++;
				Sleep(500);
			}
		}
		else
		{
//121026 - IE제외한 브라우져 다운로드 - S 
			if(pToken->GetCount() == 2)		//기타브라우져에서 실행
			{
				//업데이트 체크(타브라우져용)
				if(Update_Checking())
					return FALSE;
			}
//121026 - IE제외한 브라우져 다운로드 - E 

			break;
		}
	}


	use_window dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}


//121026 - IE제외한 브라우져 다운로드 - S 
STDMETHODIMP down_client::Update_Checking()
{


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
	//20080424 비스타용 폴더 구하는 함수(SHGetKnownFolderPath())를 쓰기 위해, jyh
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

	//_ver.ini가 있는지 부터 확인
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

		//20090328 UpdateWindow가 종료될때의 메세지를 받아서 처리, jyh
		SHELLEXECUTEINFO shi;

		ZeroMemory(&shi, sizeof(SHELLEXECUTEINFO));
		shi.cbSize = sizeof(SHELLEXECUTEINFO);
		shi.lpFile = strLocalPath;
		shi.nShow = SW_SHOW;
		shi.fMask = SEE_MASK_NOCLOSEPROCESS;
		shi.lpVerb = "open";
		ShellExecuteEx(&shi);

		//실행후에 프로그램종료(이미 프로그램실행중이므로 업데이트시 오류를 막기 위해서 )
		DWORD dwResult = ::WaitForSingleObject(shi.hProcess, 1000);

		return S_FALSE;

	}
	else
		hr = S_FALSE;

	return hr;


}

//121026 - IE제외한 브라우져 다운로드 - E 
