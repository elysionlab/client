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


bool GetHostIP();


SERVER_RUNTIME_INFO RunServerInfo;


#if defined (ACE_HAS_IP_MULTICAST)
#if defined (ACE_HAS_WINNT4)


int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	if(argc != 2)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("not found server mode!%p\n")), -1);		
	}

	/* 구동전으로 설정 */
	RunServerInfo.nAcceptRun = 0;

	ACE_TCHAR config_file[MAXPATHLEN];
	ACE_OS_String::strcpy (config_file, ACE_TEXT ("server.ini"));

	ACE_Configuration_Heap config;	
	if(config.open () == -1)
	{		
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("error ACE_Configuration_Heap open()\n")), -1);		
	}

	ACE_Ini_ImpExp config_importer (config);
	if(config_importer.import_config (config_file) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("not found config file!%p\n")), -1);		
	}

	/*
		서버 기본정보 설정
	*/
	ACE_Configuration_Section_Key server_section;
	if(config.open_section (config.root_section (), argv[1], 0, server_section) == -1)
	{
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open server_section\n")), -1);		
	}

	ACE_TString strLogFile, strMaxWorker, strMaxUser, strRunMode, strBufferSize, strAllocator;
	if( (config.get_string_value(server_section, ACE_TEXT ("LOG_FILE"), strLogFile) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_WORKER"), strMaxWorker) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_USER"), strMaxUser) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_BUFFER"), strBufferSize) == -1) ||
		(config.get_string_value(server_section, ACE_TEXT ("MAX_ALLOCATOR"), strAllocator) == -1))
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in server_section\n")), -1);		

	/* 서버 기본정보 설정 */
	RunServerInfo.nMaxUserCount		= ACE_OS::atoi(strMaxUser.c_str());
	RunServerInfo.nQueueMaxThreads	= ACE_OS::atoi(strMaxWorker.c_str());
	RunServerInfo.nServerRunMode	= ACE_OS::atoi(strRunMode.c_str());
	RunServerInfo.nMaxSockBuffer	= ACE_OS::atoi(strBufferSize.c_str());
	RunServerInfo.nMaxUserAllocator	= ACE_OS::atoi(strAllocator.c_str());	
	RunServerInfo.nServerPORT		= SERVER_PORT_SPEED;
	config.remove_section(config.root_section (), argv[1], 0);


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

	char c;
	while(1)
	{		
		cin.clear ();
		cin >> c;
		if(c == 's')
		{
			continue;
		}
		else if(c == 'x')
		{
			m_pDaemon.mmsv_close();
			break;
		}
	}

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




