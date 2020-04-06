#pragma once
#include "ServerMain.h"
#include "config_patten.h"
#include "Process_Manager.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"


extern SERVER_RUNTIME_INFO RunServerInfo;
extern bool bRunService;

void ServerMain::Worker_Init()
{
	int		dummy;
	HANDLE	hTrans;

	hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcCheckControlServer, this, 0, (unsigned int *)&dummy);
	CloseHandle(hTrans);
}

void ServerMain::Process_ControlServer()
{
	while(bRunService)
	{
		CtrlServerCheck();
		ACE_OS::sleep(30);
	}
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 컨트롤 서버 구동
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ServerMain::CtrlServerCheck()
{	
	if(OPTION_USE_SPEED)
		MakeNewProcess(ACE_TEXT(SPEED_FILE_NAME), ACE_TEXT("SpeedServer"), SERVER_PORT_SPEED);

	MakeNewProcess(ACE_TEXT(DOWN_FILE_NAME), ACE_TEXT("DownServer"), SERVER_PORT_DOWN);
	MakeNewProcess(ACE_TEXT(UP_FILE_NAME), ACE_TEXT("UpServer"), SERVER_PORT_UP);
}

void ServerMain::MakeNewProcess(ACE_TCHAR* strName, ACE_TCHAR* strParam, int nPort)
{
	ACE_SOCK_Connector	pConnector;
	ACE_SOCK_Stream		pPeer;	

	ACE_INET_Addr pAddr(nPort, (const char*)ACE_TEXT("127.0.0.1"));
	ACE_Time_Value nTimeOut(10);

	if(pConnector.connect(pPeer, pAddr, &nTimeOut) == -1)
	{
		Process_Manager pManager;
		pManager.CreateProcess(strName, strParam);
		return;
	}
	else
	{
		pPeer.close();
	}

	return;
}



unsigned int __stdcall ProcCheckControlServer(void *pParam)
{
	ServerMain* pProcess = (ServerMain*)pParam;
	pProcess->Process_ControlServer();
	return 0;
}
