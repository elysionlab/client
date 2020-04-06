/*

컨트롤 서버

*/

#include "config_patten.h"
#include "ace/OS.h"
#include "ace/streams.h"
#include "ace/Proactor.h"
#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"
#include "ace/Get_Opt.h"
#include "mmsv_Aio_Daemon.h"
//20080219 jyh
#include <vector>


SERVER_RUNTIME_INFO RunServerInfo;
std::vector<ACE_TString> g_v_strDBip;		//20080219 jyh

bool InitDefault;
bool GetHostIP();
int ConfigUpdate();
int GetDBServerList();		//20080220 DB서버 목록을 읽는다, jyh

ACE_TCHAR pArgv[255];
ACE_TString strLogFile;
ACE_TCHAR config_file[MAXPATHLEN];
ACE_Configuration_Heap config;	


#if defined (ACE_HAS_IP_MULTICAST)
#if defined (ACE_HAS_WINNT4)



int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{	
	InitDefault = false;

	if(argc != 2) ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("not found server mode!%p\n")), -1);			
	ACE_OS::strcpy(pArgv, argv[1]);

	
	/* 구동전으로 설정 */
	RunServerInfo.nAcceptRun = 0;

	ACE_OS_String::strcpy (config_file, ACE_TEXT ("server.ini"));
	if(config.open () == -1)
	{		
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("error ACE_Configuration_Heap open()\n")), -1);		
	}

	if(ConfigUpdate()==-1) return -1;

	InitDefault = true;

	/* 포트 설정 */
	if(!ACE_OS::strcmp(pArgv, "ExplorerServer"))
		RunServerInfo.nServerPORT		= SERVER_PORT_EXPLORER;
	else if(!ACE_OS::strcmp(pArgv, "DownServer"))
		RunServerInfo.nServerPORT		= SERVER_PORT_DOWN;
	else if(!ACE_OS::strcmp(pArgv, "UpServer"))
		RunServerInfo.nServerPORT		= SERVER_PORT_UP;

	/* 로그설정 설정 */
	ACE_TCHAR strLogMakeFile[1024];
	time_t nTime = ACE_OS::time(0);	
	ACE_OS::sprintf(strLogMakeFile, "%s(%d).txt", strLogFile.c_str(), nTime);	
	ACE_OSTREAM_TYPE *output = new std::ofstream(strLogMakeFile);
	ACE_LOG_MSG->msg_ostream(output, 1);
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);	
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR);
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::LOGGER);


	/* 아이피/포트 설정 : 데이터 베이스 초기화 전에 호출해줘야 함 */
	if(!GetHostIP())
	{
		ACE_DEBUG((LM_WARNING, "*** Local IP Address Check : Faild\n"));
		return -1;
	}

	mmsv_Aio_Daemon m_pDaemon;
	if(m_pDaemon.open() == -1)
	{
		ACE_DEBUG((LM_ERROR, ACE_TEXT("*** Server Start : Faild! ***\n")));
		return 0;
	}


	ACE_DEBUG((LM_INFO, "┌────────────────────────────────────────────────────────────────────────────┐\n"));	
	ACE_DEBUG((LM_INFO, "│Server Start : Success!(%D)\n"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("│Allocator Depth : %d \n"), m_pDaemon.m_pAcceptor->check_allocator_pool_depth()));
	ACE_DEBUG((LM_INFO, "└────────────────────────────────────────────────────────────────────────────┘\n\n"));
		
	while(1)
	{		
		//ACE_OS::sleep(120);
		ACE_OS::sleep(180);		//20080220 갱신 주기 3분으로, jyh
		ConfigUpdate();
#ifndef _UPSERVER
		GetDBServerList();		//20080220 DB서버 목록 갱신(*Down서버만 적용 할 것!!*), jyh
#endif
	}

	return 0;
}

//20080220 DB서버 목록을 읽는다.
int GetDBServerList()
{
	ACE_Ini_ImpExp config_importer (config);
	if(config_importer.import_config (config_file) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("not found config file!%p\n")), -1);		
	}

	ACE_Configuration_Section_Key database_section;
	if(config.open_section (config.root_section (), ACE_TEXT ("DataBase"), 0, database_section) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open database_section\n")), -1);		
	}

	ACE_TString strDBip;	
	char szTemp[50];
	int idx = 2;

	while(1)
	{
		sprintf(szTemp, "DATABASE_IP%d", idx);
		if(config.get_string_value(database_section, ACE_TEXT(szTemp), strDBip) == -1)
			break;

		int nCnt = 0;

		for(int i=0; i<(int)g_v_strDBip.size(); i++)
		{
			if(!lstrcmp(g_v_strDBip[i].c_str(), strDBip.c_str()))
				nCnt++;
		}

		if(nCnt == 0)
			g_v_strDBip.push_back(strDBip);

		idx++;
	}

	if(g_v_strDBip.empty())
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);

	//g_v_strDBip의 첫번째 값만 넣어준다.
	strcpy(RunServerInfo.pDBInfo[0], g_v_strDBip[0].c_str());
	config.remove_section(config.root_section (), ACE_TEXT ("DataBase"), 0);

	return 0;
}

int ConfigUpdate()
{
	ACE_Ini_ImpExp config_importer (config);
	if(config_importer.import_config (config_file) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("not found config file!%p\n")), -1);		
	}

	if(!InitDefault)
	{
		/*
			데이터베이스 정보 가져오기
		*/
		ACE_Configuration_Section_Key database_section;
		if(config.open_section (config.root_section (), ACE_TEXT ("DataBase"), 0, database_section) == -1)
		{
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open database_section\n")), -1);		
		}

		ACE_TString strDBip, strDBname, strDBid, strDBpw;
		////------------------------------------------------------------------------------------------
		//if( (config.get_string_value(database_section, ACE_TEXT ("DATABASE_IP"), strDBip) == -1) ||
		//	(config.get_string_value(database_section, ACE_TEXT ("DATABASE_DB"), strDBname) == -1) ||
		//	(config.get_string_value(database_section, ACE_TEXT ("DATABASE_ID"), strDBid) == -1) ||
		//	(config.get_string_value(database_section, ACE_TEXT ("DATABASE_PW"), strDBpw) == -1))
		//	
		//{	
		//	ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);		
		//}
		//
		//// 데이터베이스 정보 설정 
		//strcpy(RunServerInfo.pDBInfo[0], strDBip.c_str());
		//strcpy(RunServerInfo.pDBInfo[1], strDBname.c_str());
		//strcpy(RunServerInfo.pDBInfo[2], strDBid.c_str());
		//strcpy(RunServerInfo.pDBInfo[3], strDBpw.c_str());	
		////-------------------------------------------------------------------------------------------
	
		//20080218 DB 수 만큼 읽어들인다, jyh--------------------------------------------------
		char szTemp[50];
		int idx = 0;

		while(1)
		{
			idx++;
			sprintf(szTemp, "DATABASE_IP%d", idx);
			if(config.get_string_value(database_section, ACE_TEXT(szTemp), strDBip) == -1)
				break;

			g_v_strDBip.push_back(strDBip);
		}

		if(g_v_strDBip.empty())
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);

		//g_v_strDBip의 첫번째 값만 넣어준다.
		lstrcpy(RunServerInfo.pDBInfo[0], g_v_strDBip[0].c_str());
		g_v_strDBip.erase(g_v_strDBip.begin());	//첫번째 DB주소는 메인DB이기 때문에 Down서버 목록에서 지운다 

		/* 데이터베이스 정보 설정 */
		if((config.get_string_value(database_section, ACE_TEXT("DATABASE_DB"), strDBname) == -1) ||
			(config.get_string_value(database_section, ACE_TEXT("DATABASE_ID"), strDBid) == -1)	||
            (config.get_string_value(database_section, ACE_TEXT("DATABASE_PW"), strDBpw) == -1))
		{	
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);		
		}
		
		lstrcpy(RunServerInfo.pDBInfo[1], strDBname.c_str());
		lstrcpy(RunServerInfo.pDBInfo[2], strDBid.c_str());
		lstrcpy(RunServerInfo.pDBInfo[3], strDBpw.c_str());
		//---------------------------------------------------------------------------------------------
	
		if( (config.get_string_value(database_section, ACE_TEXT ("C_DATABASE_IP"), strDBip) == -1) ||
			(config.get_string_value(database_section, ACE_TEXT ("C_DATABASE_DB"), strDBname) == -1) ||
			(config.get_string_value(database_section, ACE_TEXT ("C_DATABASE_ID"), strDBid) == -1) ||
			(config.get_string_value(database_section, ACE_TEXT ("C_DATABASE_PW"), strDBpw) == -1))
			
		{	
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);		
		}

		/* 데이터베이스 정보 설정 */
		lstrcpy(RunServerInfo.pDBInfoCommon[0], strDBip.c_str());
		lstrcpy(RunServerInfo.pDBInfoCommon[1], strDBname.c_str());
		lstrcpy(RunServerInfo.pDBInfoCommon[2], strDBid.c_str());
		lstrcpy(RunServerInfo.pDBInfoCommon[3], strDBpw.c_str());	


		config.remove_section(config.root_section (), ACE_TEXT ("DataBase"), 0);
	}

	/*
		서버 기본정보 설정
	*/
	ACE_Configuration_Section_Key server_section;
	if(config.open_section (config.root_section (), pArgv, 0, server_section) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open server_section\n")), -1);		
	}

	ACE_TString strLogLevel,strMaxWorker, strMaxUser, strRunMode, strBufferSize, strAllocator;
	ACE_TString strLimitFreeDownCount, strFreeDownWeekendUse, strFreeDownTimeUse, strFreeDownTimeStart, strFreeDownTimeEnd;
	
	if( (config.get_string_value(server_section, ACE_TEXT ("LOG_FILE"), strLogFile) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("LOG_LEVEL"), strLogLevel) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_WORKER"), strMaxWorker) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_USER"), strMaxUser) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("SERVER_RUN_MODE"), strRunMode) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_BUFFER"), strBufferSize) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_ALLOCATOR"), strAllocator) == -1))
	{	
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in server_section\n")), -1);		
	}

	/* 서버 기본정보 설정 */	
	RunServerInfo.nLogLevel			= ACE_OS::atoi(strLogLevel.c_str());
	RunServerInfo.nMaxUserCount		= ACE_OS::atoi(strMaxUser.c_str());
	RunServerInfo.nQueueMaxThreads	= ACE_OS::atoi(strMaxWorker.c_str());
	RunServerInfo.nServerRunMode	= ACE_OS::atoi(strRunMode.c_str());
	RunServerInfo.nMaxSockBuffer	= ACE_OS::atoi(strBufferSize.c_str());
	RunServerInfo.nMaxUserAllocator	= ACE_OS::atoi(strAllocator.c_str());

	ACE_TString strLimitAvg, strLimitCount, strLimitDuplicateFile;

	if(config.get_string_value(server_section, ACE_TEXT ("LIMIT_AVG"), strLimitAvg) == -1) RunServerInfo.nLimit_LoadAvg = 10;
	else RunServerInfo.nLimit_LoadAvg = ACE_OS::atoi(strLimitAvg.c_str());	

	if(config.get_string_value(server_section, ACE_TEXT ("LIMIT_COUNT"), strLimitCount) == -1) RunServerInfo.nLimit_Count = 100;
	else RunServerInfo.nLimit_Count	= ACE_OS::atoi(strLimitCount.c_str());	

	if(!strcmp(pArgv, "UpServer")) 
	{
		if(config.get_string_value(server_section, ACE_TEXT ("LIMIT_DUPI_FILE"), strLimitDuplicateFile) == -1) RunServerInfo.nLimit_DuplicateFile = 3;
		else RunServerInfo.nLimit_DuplicateFile	= ACE_OS::atoi(strLimitDuplicateFile.c_str());	
	}
	

	config.remove_section(config.root_section (), pArgv, 0);

	return 0;
}

/* 로컬아이피 정보 체크 */
bool GetHostIP()
{
	char		name[255];
	struct		hostent* hostinfo;
	WSADATA		wsData;
	bool		bFindIp = false;

	WSAStartup(WINSOCK_VERSION, &wsData);

	if( gethostname ( name, sizeof(name)) == 0)
	{
		if((hostinfo = gethostbyname(name)) != NULL)
		{
			for(int i = 0; hostinfo->h_addr_list[i] != NULL; i++)
			{		
				// 사설아이피 인지 체크
				strcpy(RunServerInfo.pServerIp, inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[i]));

				if(!strncmp(RunServerInfo.pServerIp, "127.", 4))
					continue;
				if(!strncmp(RunServerInfo.pServerIp, "10.", 3))
					continue;
				//if(!strncmp(RunServerInfo.pServerIp, "192.168.", 8))
				//	continue;

				bFindIp = true;
				break;
			}
		}
	}
	WSACleanup();

	return bFindIp;
}


#else
#endif
#else
int ACE_TMAIN (int, ACE_TCHAR *[])
{
  ACE_DEBUG ((LM_DEBUG, "This Program does not work on this platform.\rn"));
  return 1;
}
#endif /* ACE_HAS_IP_MULTICAST */




