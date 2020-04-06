#include "config_patten.h"
#include "ace/OS.h"
#include "ace/streams.h"
#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"
#include "ServerMain.h"
//20080219 jyh
#include <vector>


SERVER_RUNTIME_INFO RunServerInfo;
std::vector<ACE_TString> g_v_strDBip;	//20080219 jyh

// #########################################
bool bRunService	= true;
// #########################################



int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{	

	static const char szProgName[] = CLIENT_LAUNCHER_MUTEX;
    CreateMutex(NULL, TRUE, szProgName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
		return -1;

	/* ���������� ���� */
	RunServerInfo.nAcceptRun = 0;

	ACE_TCHAR config_file[MAXPATHLEN];
	ACE_OS_String::strcpy (config_file, ACE_TEXT ("server.ini"));


	ACE_Configuration_Heap config;	
	if(config.open () == -1)
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("error ACE_Configuration_Heap open()\n")), -1);		

	ACE_Ini_ImpExp config_importer (config);
	if(config_importer.import_config (config_file) == -1)
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("not found config file!%p\n")), -1);		


	/*
		�����ͺ��̽� ���� ��������
	*/
	ACE_Configuration_Section_Key database_section;
	if(config.open_section (config.root_section (), ACE_TEXT ("DataBase"), 0, database_section) == -1)
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open database_section\n")), -1);		


	ACE_TString strDBip, strDBname, strDBid, strDBpw;
	//if( (config.get_string_value(database_section, ACE_TEXT ("DATABASE_IP"), strDBip) == -1) ||
	//	(config.get_string_value(database_section, ACE_TEXT ("DATABASE_DB"), strDBname) == -1) ||
	//	(config.get_string_value(database_section, ACE_TEXT ("DATABASE_ID"), strDBid) == -1) ||
	//	(config.get_string_value(database_section, ACE_TEXT ("DATABASE_PW"), strDBpw) == -1))
	//	ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);		

	///* �����ͺ��̽� ���� ���� */
	//strcpy(RunServerInfo.pDBInfo[0], strDBip.c_str());
	//strcpy(RunServerInfo.pDBInfo[1], strDBname.c_str());
	//strcpy(RunServerInfo.pDBInfo[2], strDBid.c_str());
	//strcpy(RunServerInfo.pDBInfo[3], strDBpw.c_str());

	//20080218 DB �� ��ŭ �о���δ�, jyh
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

	//g_v_strDBip�� ù��° ���� �־��ش�.
	lstrcpy(RunServerInfo.pDBInfo[0], g_v_strDBip[0].c_str());

	/* �����ͺ��̽� ���� ���� */
	if((config.get_string_value(database_section, ACE_TEXT("DATABASE_DB"), strDBname) == -1) ||
		(config.get_string_value(database_section, ACE_TEXT("DATABASE_ID"), strDBid) == -1)	||
		(config.get_string_value(database_section, ACE_TEXT("DATABASE_PW"), strDBpw) == -1))
	{	
		//printf("��ó ������ ������ ���̽� ����\n");
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Can't open value in database_section\n")), -1);		
	}

	lstrcpy(RunServerInfo.pDBInfo[1], strDBname.c_str());
	lstrcpy(RunServerInfo.pDBInfo[2], strDBid.c_str());
	lstrcpy(RunServerInfo.pDBInfo[3], strDBpw.c_str());

	config.remove_section(config.root_section (), ACE_TEXT ("DataBase"), 0);

	ServerMain server;
	server.Worker_Init();


	while(1)
	{
		ACE_OS::sleep(10);

		if(!bRunService)
			break;
	}

	bRunService = false;
	return 0;
}


