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


	//20080424 vs8�� ���������� ���� ����, jyh
	/*if(!AfxSocketInit())
	{
		MessageBox(NULL, "�����ʱ�ȭ ����     ", CLIENT_SERVICE_NAME, MB_OK|MB_ICONSTOP);
		return FALSE;
	}*/

	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();

	int nCheckCount = 0;
	static const char szProgName[] = CLIENT_DOWN_MUTEX;
    CreateMutex(NULL, TRUE, szProgName);

	
//121026 - IE������ ������ �ٿ�ε� - S 
	CString Cmd_line_argv;
	Cmd_line_argv = (LPCTSTR)GetCommandLine();
	Cmd_line_argv = ::PathGetArgs(Cmd_line_argv);

	CToken* pToken = new CToken(" ");	
	pToken->Split((LPSTR)(LPCTSTR)Cmd_line_argv);	
//121026 - IE������ ������ �ٿ�ε� - E


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

//121026 - IE������ ������ �ٿ�ε� - S 
				//121026 - IE������ ������ �ٿ�ε� - S 
				if(pToken->GetCount() == 2)		//��Ÿ���������� ����
				{
					//��ũ���� ��ū���� ��ȭ
					CToken* url_pToken = new CToken("/");	
					//��ū����ó��
					url_pToken->Split((LPSTR)(LPCTSTR)pToken->GetToken(1));	

					//����� ���μ��� �˻�
					HWND hDownWnd = ::FindWindow(CLIENT_DOWN_CLASS, NULL);
					if(hDownWnd)
					{

						CString pItem;

						//�ڷḦ ó��
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
//121026 - IE������ ������ �ٿ�ε� - E 


				nCheckCount ++;
				Sleep(500);
			}
		}
		else
		{
//121026 - IE������ ������ �ٿ�ε� - S 
			if(pToken->GetCount() == 2)		//��Ÿ���������� ����
			{
				//������Ʈ üũ(Ÿ��������)
				if(Update_Checking())
					return FALSE;
			}
//121026 - IE������ ������ �ٿ�ε� - E 

			break;
		}
	}


	use_window dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}


//121026 - IE������ ������ �ٿ�ε� - S 
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

	//_ver.ini�� �ִ��� ���� Ȯ��
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

		//20090328 UpdateWindow�� ����ɶ��� �޼����� �޾Ƽ� ó��, jyh
		SHELLEXECUTEINFO shi;

		ZeroMemory(&shi, sizeof(SHELLEXECUTEINFO));
		shi.cbSize = sizeof(SHELLEXECUTEINFO);
		shi.lpFile = strLocalPath;
		shi.nShow = SW_SHOW;
		shi.fMask = SEE_MASK_NOCLOSEPROCESS;
		shi.lpVerb = "open";
		ShellExecuteEx(&shi);

		//�����Ŀ� ���α׷�����(�̹� ���α׷��������̹Ƿ� ������Ʈ�� ������ ���� ���ؼ� )
		DWORD dwResult = ::WaitForSingleObject(shi.hProcess, 1000);

		return S_FALSE;

	}
	else
		hr = S_FALSE;

	return hr;


}

//121026 - IE������ ������ �ٿ�ε� - E 
