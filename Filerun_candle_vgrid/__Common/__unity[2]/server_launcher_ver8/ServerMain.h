#pragma once
#include "ace/OS.h"


class ServerMain
{

public:

	ServerMain(void)
	{
	}
	~ServerMain(void)
	{
	}

public:
	
	HANDLE	m_hThread;

public:

	void Worker_Init();	
	void Process_ControlServer();

	// 프로세스 실행
	void MakeNewProcess(ACE_TCHAR* strName, ACE_TCHAR* strParam, int nPort);
	// 컨트롤 서버 체크
	void CtrlServerCheck();	

	friend unsigned int __stdcall ProcCheckControlServer(void *pParam);

};
