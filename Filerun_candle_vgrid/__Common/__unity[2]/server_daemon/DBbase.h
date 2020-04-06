/************************************************************************************
*
* MYSQL 데이터베이스 관련 모든 작업
*
* - 데이터베이스 쿼리는 별도의 쓰레드를 사용하지 않고 큐에 의해 호출된 ProcessFunc 
*   클래스에서 호출될때 실행
* - 쿼리 실행시 mysql_ping을 이용해 연결 확인
* - 쿼리 후 연결은 종료하지 않고 타임아웃 시간에 연결이 종료됨
*************************************************************************************/

#pragma once


#ifndef __DBBASE_H__
#define __DBBASE_H__


#include "ace/Log_Msg.h"
#include "ace/OS.h"
#if defined (ACE_HAS_WINNT4)
#include <winsock.h>
#include <string>
#else

#endif

#include "mysql.h"
#include "config.h"


#define SAFE_DELETEDB(p) {if(p) {mysql_free_result(p); (p)=NULL;}}

class DBbase
{

public:

	// 기본 생성/소멸자
	DBbase(void);
	virtual ~DBbase(void);


public:

	MYSQL*	m_pMysql;

	//20080218 jyh
	// 데이터베이스 정보
	char	SP_MYSQL_HOST[25];
	char	SP_MYSQL_DB[51];
	char	SP_MYSQL_ID[51];
	char	SP_MYSQL_PW[51];
	

	// 쿼리
	int		nCount;	
	char	m_pQuery1[4096];
	char	m_pQuery2[4096];
	char	m_pQuery3[4096];

/* 
	#### 데이터베이스 연결/연결체크
*/

public:

	void DB_Init();
	void DB_Clear();
	bool BIC_Init(char* host, char* db, char* id, char* pw);
	bool BIC_ConnectDB();
	bool BIC_CheckDB();


/* 
	#### 쿼리 관련 함수들
*/

public:

	// insert, update  를 위한 쿼리처리
	int ExecuteQuery(char* query);	
	// select 를 위한 쿼리처리(카운터만 필요할때)
	int ExecuteQuery(char* query, int& cnt);	
	// select 를 위한 쿼리처리
	MYSQL_RES* ExecuteQueryR(char* query);
	// select 를 위한 쿼리처리(카운터가 필요할때)
	MYSQL_RES* ExecuteQueryR(char* query, int& cnt);

	
/* 
	#### 유틸 함수들
*/
	// 폴더/파일 이름/정보 체크(특수문자, ")
	void SpecialText(std::string& text);
	// std::string 의 특정문자열 삭제
	void eraseText(std::string& text, std::string word);
	// \ 삽입
	void SlashAdd(std::string& text);
	// \ 삽입
	char* SlashAdd(char* text);
	
};


#endif