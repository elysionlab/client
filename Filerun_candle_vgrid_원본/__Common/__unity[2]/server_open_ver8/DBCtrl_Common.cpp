#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 데이터베이스 기본 설정
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool DBCtrl::Init(char* host, char* db, char* id, char* pw)
{
	if(!BIC_Init(host, db, id, pw))
		return false;

	//printf("DB_host: %s, DB_name: %s, DB_ID: %s, DB_PW: %s\n", host, db, id, pw);//test

	return true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 접속자 인증 : 아이디기반
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DBCtrl::AuthUser(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	ACE_UINT64	nMemberIdx = 0;

	try
	{
		PINFOAUTH pLogin = (PINFOAUTH)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		// 아이디 존재 유무 체크 : CP 체크
		sprintf_s(m_pQuery1, 4096, "Select idx From %s Where %s = '%s' And state != 0", 
							TABLE_MEMBER_BASE, FIELD_MEMBER_ID, pLogin->pUserID);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || nCount == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			pLogin->nResult = LOGIN_RESULT_FAIL_ID;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}

		nMemberIdx = _atoi64(pRow[0]);
		SAFE_DELETEDB(pResult);

		// 로그인 성공 설정		
		pLogin->nResult = LOGIN_RESULT_AUTH;	
		pClient->LogIn(pLogin->pUserID, nMemberIdx);		 
		pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Common━AuthUser()\n"))); 
	}

	return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 파일 사이즈별 출력 포멧
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string DBCtrl::SetSizeFormat(__int64 realsize)
{
	std::string bSize;
	char sstr[20];
	double pointsize;
	__int64 size = realsize;


	if(size >= 1024000000)		// GB
	{
		pointsize = ((double)size / (1024000000));
		sprintf_s(sstr, 20, "%.1f GB", pointsize);
	}
	else if(size >= 1024000 && size < 1024000000)	// MB
	{
		pointsize = ((double)size / (1024000));
		sprintf_s(sstr, 20, "%.1f MB", pointsize);
	}
	else if(size < 1024000 && size >= 1024)
	{
		pointsize = ((double)size / 1024);
		sprintf_s(sstr, 20, "%.1f KB", pointsize);
	}
	else
	{
		pointsize = (double)size;
		sprintf_s(sstr, 20, "%.1f Byte", pointsize);
	}

	bSize = bSize + sstr;

    return bSize;
}



// 정액제 가능한지 체크
int DBCtrl::Fix_Check(mmsv_User* pClient, char* pUserID)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	DWORD		nNow;
	DWORD		nReg;

	try
	{
		sprintf_s(m_pQuery1, 4096, "Select fix_end, UNIX_TIMESTAMP() From %s Where userid = '%s'",  TABLE_MEMBER_PAY, pUserID);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{			
			SAFE_DELETEDB(pResult);				
			return 0; // 에러
		}

		nReg = atol(pRow[0]);	
		nNow = atol(pRow[1]);	

		SAFE_DELETEDB(pResult);	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Common━Fix_Check\n"))); 
	}
	
	return 0; // 에러
}

