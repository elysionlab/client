#pragma once
#include "DBCtrl.h"
#include "ace/Thread_Manager.h"

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
	void Process_FileDivision();
	void Process_StoreServer();
	void Process_FileRemove();

	friend unsigned int __stdcall ProcCheckFileDivision(void *pParam);
	friend unsigned int __stdcall ProcCheckStoreServer(void *pParam);
	friend unsigned int __stdcall ProcCheckFileRemove(void *pParam);

};
