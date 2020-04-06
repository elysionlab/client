#include "DBbase.h"
//20080218 jyh
#include "ace/Configuration.h"
#include<vector>
#include <time.h>

extern std::vector<ACE_TString> g_v_strDBip;		//20080219 jyh

#pragma warning(disable: 4996)

DBbase::DBbase(void)
:	m_pMysql(0)
{
	DB_Init();	
}

DBbase::~DBbase(void)
{
	DB_Clear();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 데이터베이스 초기화
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DBbase::DB_Init()
{
	m_pMysql = mysql_init((MYSQL*) 0);	

	if(OPTION_MYSQL_41_OVER == 1)
		mysql_options(m_pMysql, MYSQL_SET_CHARSET_NAME, "euckr");	
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 데이터베이스 해제
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DBbase::DB_Clear()
{
	if(m_pMysql)
		mysql_close(m_pMysql);	

	m_pMysql = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 데이터베이스 기본 설정
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool DBbase::BIC_Init(char* host, char* db, char* id, char* pw)
{
	lstrcpy(SP_MYSQL_HOST, host);
	lstrcpy(SP_MYSQL_DB, db);
	lstrcpy(SP_MYSQL_ID, id);
	lstrcpy(SP_MYSQL_PW, pw);
	
	return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 연결확인 : 재연결
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool DBbase::BIC_CheckDB()
{
	if(m_pMysql == 0)
	{
		DB_Init();	

		if(!m_pMysql)
			return false;
	}

	try
	{
		if(mysql_ping(m_pMysql) != 0)
			return BIC_ConnectDB();
		else
			return true;
	}
	catch(...)
	{		
		return BIC_ConnectDB();
	}
	return true;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 서버와 연결
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool DBbase::BIC_ConnectDB()
{
	try
	{		
		for(int nConnectCnt = 0; nConnectCnt < 2; nConnectCnt++)
		{
			if(m_pMysql == 0)
			{
				DB_Init();	

				if(!m_pMysql)
					return false;
			}

#ifndef _UPSERVER
			//20080218 DB에 랜덤 연결(*Down 서버에만 적용 할것*)-----------------------
			int idx = 0;
						
			while(1)
			{	
				
				srand((unsigned)time(NULL));
				idx = rand()%g_v_strDBip.size();
				//printf("RandDB: %s, DBipSize: %d\n", g_v_strDBip[idx].c_str(), (int)g_v_strDBip.size());

				lstrcpy(SP_MYSQL_HOST, g_v_strDBip[idx].c_str());

				if(!mysql_real_connect(m_pMysql, SP_MYSQL_HOST, SP_MYSQL_ID, SP_MYSQL_PW, SP_MYSQL_DB, MYSQL_PORT, NULL, NULL))
				{
					fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(m_pMysql));
					g_v_strDBip.erase(g_v_strDBip.begin() + idx);
				}
				else
				{
					//printf("DB 연결 성공!\n");
					break;
				}
			}
			//---------------------------------------------------------------------------
#else			
			//*Up 서버 빌드시 적용할 것!!--------------------------------------------
			m_pMysql = mysql_real_connect(m_pMysql, SP_MYSQL_HOST, SP_MYSQL_ID, SP_MYSQL_PW, SP_MYSQL_DB, MYSQL_PORT, NULL, NULL);
			//printf("접속DB: %s\n", SP_MYSQL_HOST);
#endif

			if(m_pMysql)
			{
				if(mysql_ping(m_pMysql) == 0)
				{
					return true;
				}				
			}

			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━DataBase Connection Faild : %d\n"), nConnectCnt));
			
			DB_Clear();
		}

		DB_Clear();	
		return false;
	}
	catch(...)
	{
		return false;
	}

	return false;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// insert, update  를 위한 쿼리처리
// reutrn value
// 0 : 정상처리
// 1 : 디비 연결에러
// 2 : 쿼리에러
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DBbase::ExecuteQuery(char* query)
{	
	try
	{
		if(!BIC_CheckDB())
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query) -> BIC_CheckDB()\n╋QUERY━%s\n"), query));
			return QUERY_DISCNNT;
		}

		if(mysql_query(m_pMysql, query))
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query) -> mysql_query()\n╋QUERY━%s\n"), query));
			return QUERY_BADREG;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query) -> catch(...)\n╋QUERY━%s\n"), query));
		return QUERY_DISCNNT;
	}	

	return QUERY_COMPLETE;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// select 를 위한 쿼리처리(카운터만 필요할때)
// reutrn value
// 0 : 정상처리
// 1 : 디비 연결에러
// 2 : 쿼리에러
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DBbase::ExecuteQuery(char* query, int& cnt)
{
	MYSQL_RES	*result = NULL;

	try
	{
		if(!BIC_CheckDB())
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query, int& cnt) -> BIC_CheckDB()\n╋QUERY━%s\n"), query));
			return QUERY_DISCNNT;
		}

		if(mysql_query(m_pMysql,query))
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query, int& cnt) -> mysql_query()\n╋QUERY━%s\n"), query));
			return QUERY_BADREG;
		}

		if((result = mysql_store_result(m_pMysql)) == 0)
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query, int& cnt) -> result == 0\n╋QUERY━%s\n"), query));
			return QUERY_BADREG;
		}

		cnt = (int)mysql_num_rows(result);
		SAFE_DELETEDB(result);
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQuery(char* query, int& cnt) -> catch(...)\n╋QUERY━%s\n"), query));
		return QUERY_DISCNNT;
	}

	return QUERY_COMPLETE;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// select 를 위한 쿼리처리
// reutrn value
// 0 : 정상처리
// 1 : 디비 연결에러
// 2 : 쿼리에러
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSQL_RES* DBbase::ExecuteQueryR(char* query)
{
	MYSQL_RES* result = 0;
	
	try
	{
		if(!BIC_CheckDB())
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query) -> BIC_CheckDB()\n╋QUERY━%s\n"), query));
			return 0;
		}

		if(mysql_query(m_pMysql,query))
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query) -> mysql_query()\n╋QUERY━%s\n"), query));
			return 0;
		}

		if((result = mysql_store_result(m_pMysql)) == 0)
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query) -> result = 0\n╋QUERY━%s\n"), query));
			return 0;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query) -> catch(...)\n╋QUERY━%s\n"), query));
		result = 0;
	}
	return result;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// select 를 위한 쿼리처리(카운터가 필요할때)
// reutrn value
// 0 : 정상처리
// 1 : 디비 연결에러
// 2 : 쿼리에러
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSQL_RES* DBbase::ExecuteQueryR(char* query, int& cnt)
{
	MYSQL_RES* result = 0;
	
	try
	{
		if(!BIC_CheckDB())
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query, int& cnt) -> BIC_CheckDB()\n╋QUERY━%s\n"), query));
			return 0;
		}

		if(mysql_query(m_pMysql,query))
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query, int& cnt) -> mysql_query()\n╋QUERY━%s\n"), query));
			return 0;
		}

		if((result = mysql_store_result(m_pMysql)) == 0)
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query, int& cnt) -> result = 0\n╋QUERY━%s\n"), query));
			return 0;
		}

		cnt = (int)mysql_num_rows(result);	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DB━%D━ExecuteQueryR(char* query, int& cnt) -> catch(...) \n╋QUERY━%s\n"), query));
		result = 0;
	}
	return result;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 폴더/파일 이름/정보 체크(특수문자 = \ / ? : " * < > 처리와 \r\n처리)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DBbase::SpecialText(std::string& text)
{	

	if(text == "..")
	{
		text = ",,";
		return;
	}

	eraseText(text, "\\");
	eraseText(text, "/");
	eraseText(text, "?");
	eraseText(text, ":");
	eraseText(text, "\"");
	eraseText(text, "*");
	eraseText(text, "<");
	eraseText(text, ">");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// std::string 의 특정문자열 삭제
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DBbase::eraseText(std::string& text, std::string word)
{
	int index = 0;

	while(1)
	{
		index = (int)text.find(word, 0);

		if(index == -1)
			return;
		
		text = text.replace(index , 1, "");
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// \ 삽입
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DBbase::SlashAdd(std::string& text)
{
	int index = 0;

	while(1)
	{
		index = (int)text.find("'", index);

		if(index == -1)
			break;

		text = text.replace(index , 1, "''");

		index += 2;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// \ 삽입
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
char* DBbase::SlashAdd(char* text)
{
	int index = 0;

	std::string strString = text;

	while(1)
	{
		index = (int)strString.find("'", index);

		if(index == -1)
			break;

		strString = strString.replace(index , 1, "\\'");

		index += 2;
	}

	lstrcpy(text, strString.c_str());
	return text;
}

#pragma warning(default: 4996)



// \ 삽입
	void SlashAdd(char* text);
