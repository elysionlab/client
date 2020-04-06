#include "stdafx.h"
#include "up_client.h"
#include "use_window.h"


BEGIN_MESSAGE_MAP(up_client, CWinApp)
END_MESSAGE_MAP()


up_client::up_client()
{
}

up_client theApp;


BOOL up_client::InitInstance()
{
	WNDCLASS    wndClass;
	::GetClassInfo(AfxGetInstanceHandle(), "#32770", &wndClass);
    wndClass.lpszClassName = CLIENT_UP_CLASS;
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
	static const char szProgName[] = CLIENT_UP_MUTEX;
    CreateMutex(NULL, TRUE, szProgName);

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
				nCheckCount ++;
				Sleep(500);
			}
		}
		else
			break;
	}

	use_window dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
