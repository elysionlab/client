#pragma once
#include "ServerMain.h"
#include "config_patten.h"

extern SERVER_RUNTIME_INFO RunServerInfo;
extern bool bRunService;

void ServerMain::Worker_Init()
{
	int		dummy;
	HANDLE	hTrans;

	if(OPTION_USE_FTPDIVISION == 1)
	{
		hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcCheckFileDivision, this, 0, (unsigned int *)&dummy);
		CloseHandle(hTrans);
	}

	if(OPTION_USE_FTPINFO_UPDATE == 1)
	{
		hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcCheckStoreServer, this, 0, (unsigned int *)&dummy);
		CloseHandle(hTrans);
	}

	if(OPTION_USE_FILEREMOVE == 1)
	{
		hTrans = (HANDLE)_beginthreadex(NULL, 0, ProcCheckFileRemove, this, 0, (unsigned int *)&dummy);
		CloseHandle(hTrans);
	}
}


void ServerMain::Process_FileDivision()
{
	DBCtrl* pDB = new DBCtrl();
	
	pDB->Init(RunServerInfo.pDBInfo[0], RunServerInfo.pDBInfo[1], RunServerInfo.pDBInfo[2], RunServerInfo.pDBInfo[3]);
	pDB->m_pFileDB->BIC_Init(RunServerInfo.pDBInfoCommon[0], RunServerInfo.pDBInfoCommon[1], RunServerInfo.pDBInfoCommon[2], RunServerInfo.pDBInfoCommon[3]);

	while(bRunService)
	{		
		ACE_DEBUG((LM_INFO, ACE_TEXT("* 부하파일 분할 체크 시작 : %D\n")));
		pDB->FileDivision();
		ACE_DEBUG((LM_INFO, ACE_TEXT("* 부하파일 분할 체크 종료 : %D\n")));
		ACE_OS::sleep(RunServerInfo.nDivisionTime);
	}

	delete pDB;
}

void ServerMain::Process_StoreServer()
{
	DBCtrl* pDB = new DBCtrl();

	pDB->Init(RunServerInfo.pDBInfo[0], RunServerInfo.pDBInfo[1], RunServerInfo.pDBInfo[2], RunServerInfo.pDBInfo[3]);
	pDB->m_pFileDB->BIC_Init(RunServerInfo.pDBInfoCommon[0], RunServerInfo.pDBInfoCommon[1], RunServerInfo.pDBInfoCommon[2], RunServerInfo.pDBInfoCommon[3]);

	while(bRunService)
	{
		ACE_DEBUG((LM_INFO, ACE_TEXT("* 파일서버 체크 시작 : %D\n")));
		pDB->UpdateDiskAverage();
		ACE_DEBUG((LM_INFO, ACE_TEXT("* 파일서버 체크 종료 : %D\n")));
		ACE_OS::sleep(RunServerInfo.nFtpInfoUpdateTime);
	}

	delete pDB;
}


void ServerMain::Process_FileRemove()
{
	DBCtrl* pDB = new DBCtrl();
	pDB->Init(RunServerInfo.pDBInfo[0], RunServerInfo.pDBInfo[1], RunServerInfo.pDBInfo[2], RunServerInfo.pDBInfo[3]);
	pDB->m_pFileDB->BIC_Init(RunServerInfo.pDBInfoCommon[0], RunServerInfo.pDBInfoCommon[1], RunServerInfo.pDBInfoCommon[2], RunServerInfo.pDBInfoCommon[3]);

	while(bRunService)
	{
		ACE_DEBUG((LM_INFO, ACE_TEXT("* 삭제파일 체크 시작 : %D\n")));
		pDB->FileRemove();
		ACE_DEBUG((LM_INFO, ACE_TEXT("* 삭제파일 체크 종료 : %D\n")));
		ACE_OS::sleep(RunServerInfo.nFileRemoveTime);
	}

	delete pDB;
}





unsigned int __stdcall ProcCheckFileDivision(void *pParam)
{
	ServerMain* pProcess = (ServerMain*)pParam;
	pProcess->Process_FileDivision();
	return 0;
}


unsigned int __stdcall ProcCheckStoreServer(void *pParam)
{
	ServerMain* pProcess = (ServerMain*)pParam;
	pProcess->Process_StoreServer();
	return 0;
}

unsigned int __stdcall ProcCheckFileRemove(void *pParam)
{
	ServerMain* pProcess = (ServerMain*)pParam;
	pProcess->Process_FileRemove();
	return 0;
}
