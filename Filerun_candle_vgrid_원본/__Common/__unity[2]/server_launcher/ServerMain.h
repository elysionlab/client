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

	// ���μ��� ����
	void MakeNewProcess(ACE_TCHAR* strName, ACE_TCHAR* strParam, int nPort);
	// ��Ʈ�� ���� üũ
	void CtrlServerCheck();	

	friend unsigned int __stdcall ProcCheckControlServer(void *pParam);

};
