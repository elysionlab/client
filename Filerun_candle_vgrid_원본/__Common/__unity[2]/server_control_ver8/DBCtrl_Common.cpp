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

		// 서버 점검상태 체크
		if(!CheckService())
		{
			pLogin->nResult = LOGIN_RESULT_SERVERSTOP;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}
		

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


		// 나머지 정보 가져오기
		sprintf_s(m_pQuery1, 4096, "Select %s, state, UNIX_TIMESTAMP() From %s Where %s = '%s'",  
							FIELD_MEMBER_PW, TABLE_MEMBER_BASE, FIELD_MEMBER_ID, pLogin->pUserID);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);				
			pLogin->nResult = LOGIN_RESULT_FAIL_ID;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}		
		
		// 비밀번호 확인		
		if(strcmp(pRow[0], pLogin->pUserPW))
		{
			SAFE_DELETEDB(pResult);				
			pLogin->nResult = LOGIN_RESULT_FAIL_PW;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}

		// 정지회원체크
		if(strcmp(pRow[1], "1") && _atoi64(pRow[1]) > _atoi64(pRow[2]))
		{
			SAFE_DELETEDB(pResult);				
			pLogin->nResult = LOGIN_RESULT_FAIL_STOP;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}
		SAFE_DELETEDB(pResult);	


		// 중복 접속 체크 (최초 로그인 요청일 경우 중복접속 알림)
		if(pClient->user_check(nMemberIdx) != 0 && pLogin->nProcess == 0) 
		{
			pLogin->nResult = LOGIN_RESULT_FAIL_DUPC;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}

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

//20080122 비정상 종료로 인해 로컬에 저장돼 있던 파일 사이즈 업데이트, jyh
void DBCtrl::UpLoad_Size(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	PUPLOADEND pTransEndInfo;
	char pTableFile[30];

	try
	{
		//printf("업로드 스테이트 함수 진입!\n");
		pTransEndInfo = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* 현재까지 업로드된 상태 변경 : 타입별로 다르게 접근 */ 
		switch(pTransEndInfo->nFileType)
		{
		case 0:
			strcpy(pTableFile, TABLE_FILE_BBS);
			break;
		case 1:
		case 2:
			strcpy(pTableFile, TABLE_FILE_BLOG);
			break;
		default:
			return;
		}

		//printf("파일No: %I64d, 로컬에서 보낸 파일인덱스: %I64d", pClient->m_nFileNo, pTransEndInfo->nFileIdx);
		//if(pClient->m_nFileNo == pTransEndInfo->nFileIdx)// && pClient->m_nFileSizeBegin < pTransEndInfo->nFileSizeEnd)
		//{		
		sprintf_s(m_pQuery1, 4096, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, pTransEndInfo->nFileSizeEnd, pTransEndInfo->nFileIdx);
		ExecuteQuery(m_pQuery1);
		//}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadState()━Catch(...)\n"))); 
	}
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
