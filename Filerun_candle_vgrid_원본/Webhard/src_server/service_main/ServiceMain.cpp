#include <windows.h>
#include <stdio.h>
#include "process.h"
#include "../../Common/config.h"


SERVICE_STATUS_HANDLE	g_hXSS;			//���� ȯ�� �۷ι� �ڵ�
DWORD					g_XSS;			//������ ���� ���¸� �����ϴ� ����
BOOL					g_bPause;		//���񽺰� �����ΰ� �ƴѰ�
HANDLE					g_hExitEvent;	//���񽺸� ���� ��ų�� �̺�Ʈ�� ����Ͽ� �����带 �����Ѵ�

char					m_ServiceName[512];
bool					bRunService = true;


unsigned int __stdcall ProcCheck(void *pParam);

//���� ���� ��ü
void XServiceStart(DWORD argc, LPTSTR* argv);
//���� ���� 
void XSCHandler(DWORD opcode);
//���� ȯ�� ����
void XSetStatus(DWORD dwState, DWORD dwAccept = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_PAUSE_CONTINUE);




void main()
{
	SYSTEM_INFO		si;
	char			cModuleName[MAX_PATH];	
	char			m_Call_Daemon[MAX_PATH];	
	char			m_Call_Launcher[MAX_PATH];	

	GetSystemInfo(&si);
	GetModuleFileName(GetModuleHandle(NULL), cModuleName, MAX_PATH);
	*(strrchr(cModuleName, '\\') + 1) = 0;	
	SetCurrentDirectory(cModuleName);

	sprintf(m_Call_Daemon, "%s%s", cModuleName, DAEMON_FILE_NAME);
	sprintf(m_Call_Launcher, "%s%s", cModuleName, LAUNCHER_FILE_NAME);


	// �Ʒ� ����� �����ص� â�� ��µ��� ����
	//STARTUPINFO			startup; 
	//startup.dwFlags     = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	//startup.wShowWindow = SW_HIDE;


	// �Ʒ� ����� ����Ǵ� ���α׷��� �ܼ��� ��Ÿ��
	// ������ �����ϸ� �������� �α��� ������ Ȯ�ΰ�����
	STARTUPINFO startup = {0};
	PROCESS_INFORMATION proInfo;
	startup.cb = sizeof(STARTUPINFO);	 

	if(OPTION_USE_DAEMON)
		CreateProcess(NULL, m_Call_Daemon, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &proInfo);

	if(OPTION_USE_LAUNCHER)
		CreateProcess(NULL, m_Call_Launcher, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &proInfo);



	strcpy(m_ServiceName, WINDOWS_SERVICENAME);	
	SERVICE_TABLE_ENTRY stbl[] = 
	{		
		{m_ServiceName, (LPSERVICE_MAIN_FUNCTION)XServiceStart },
		{NULL, NULL}
	};

	if(!StartServiceCtrlDispatcher(stbl))
	{		
		return;
	}	
}


void XSetStatus(DWORD dwState, DWORD dwAccept)
{
	SERVICE_STATUS	ss;
	ss.dwServiceType			= SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState			= dwState;
	ss.dwControlsAccepted		= dwAccept;
	ss.dwWin32ExitCode			= 0;
	ss.dwServiceSpecificExitCode= 0;
	ss.dwCheckPoint				= 0;
	ss.dwWaitHint				= 0;
	
	//���� ���� ����
	g_XSS = dwState;
	SetServiceStatus(g_hXSS, &ss);
}	

void XServiceStart(DWORD argc, LPTSTR* argv)
{
	g_hXSS = RegisterServiceCtrlHandler(m_ServiceName, (LPHANDLER_FUNCTION)XSCHandler);
	if(g_hXSS == 0) return;
	XSetStatus(SERVICE_START_PENDING);
	g_bPause = FALSE;
	XSetStatus(SERVICE_RUNNING);	

	Sleep(2000);

	XSetStatus(SERVICE_STOPPED);
}

void XSCHandler(DWORD opcode)
{
	if(opcode == g_XSS)
		return;

	switch(opcode)
	{
		case SERVICE_CONTROL_PAUSE:
			XSetStatus(SERVICE_PAUSE_PENDING,0);
			g_bPause = TRUE;
			XSetStatus(SERVICE_PAUSED);
			break;
		case SERVICE_CONTROL_CONTINUE:
			XSetStatus(SERVICE_CONTINUE_PENDING, 0);
			g_bPause = FALSE;
			XSetStatus(SERVICE_RUNNING);
			break;
		case SERVICE_CONTROL_STOP:
			XSetStatus(SERVICE_STOP_PENDING, 0);
			bRunService = false;
			break;
		case SERVICE_CONTROL_INTERROGATE:
		default:
			XSetStatus(g_XSS);
			break;
	}
}


