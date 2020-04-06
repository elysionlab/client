/************************************************************************************
*
* MYSQL �����ͺ��̽� ���� ��� �۾�
*
* - �����ͺ��̽� ������ ������ �����带 ������� �ʰ� ť�� ���� ȣ��� ProcessFunc 
*   Ŭ�������� ȣ��ɶ� ����
* - ���� ����� mysql_ping�� �̿��� ���� Ȯ��
* - ���� �� ������ �������� �ʰ� Ÿ�Ӿƿ� �ð��� ������ �����
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

	// �⺻ ����/�Ҹ���
	DBbase(void);
	virtual ~DBbase(void);


public:

	MYSQL*	m_pMysql;

	//20080218 jyh
	// �����ͺ��̽� ����
	char	SP_MYSQL_HOST[25];
	char	SP_MYSQL_DB[51];
	char	SP_MYSQL_ID[51];
	char	SP_MYSQL_PW[51];
	

	// ����
	int		nCount;	
	char	m_pQuery1[4096];
	char	m_pQuery2[4096];
	char	m_pQuery3[4096];

/* 
	#### �����ͺ��̽� ����/����üũ
*/

public:

	void DB_Init();
	void DB_Clear();
	bool BIC_Init(char* host, char* db, char* id, char* pw);
	bool BIC_ConnectDB();
	bool BIC_CheckDB();


/* 
	#### ���� ���� �Լ���
*/

public:

	// insert, update  �� ���� ����ó��
	int ExecuteQuery(char* query);	
	// select �� ���� ����ó��(ī���͸� �ʿ��Ҷ�)
	int ExecuteQuery(char* query, int& cnt);	
	// select �� ���� ����ó��
	MYSQL_RES* ExecuteQueryR(char* query);
	// select �� ���� ����ó��(ī���Ͱ� �ʿ��Ҷ�)
	MYSQL_RES* ExecuteQueryR(char* query, int& cnt);

	
/* 
	#### ��ƿ �Լ���
*/
	// ����/���� �̸�/���� üũ(Ư������, ")
	void SpecialText(std::string& text);
	// std::string �� Ư�����ڿ� ����
	void eraseText(std::string& text, std::string word);
	// \ ����
	void SlashAdd(std::string& text);
	// \ ����
	char* SlashAdd(char* text);
	
};


#endif