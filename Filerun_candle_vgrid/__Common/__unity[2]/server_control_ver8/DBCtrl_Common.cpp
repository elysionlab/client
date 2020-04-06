#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �����ͺ��̽� �⺻ ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool DBCtrl::Init(char* host, char* db, char* id, char* pw)
{
	if(!BIC_Init(host, db, id, pw))
		return false;

	//printf("DB_host: %s, DB_name: %s, DB_ID: %s, DB_PW: %s\n", host, db, id, pw);//test

	return true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ������ ���� : ���̵���
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DBCtrl::AuthUser(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	ACE_UINT64	nMemberIdx = 0;

	try
	{
		PINFOAUTH pLogin = (PINFOAUTH)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		// ���� ���˻��� üũ
		if(!CheckService())
		{
			pLogin->nResult = LOGIN_RESULT_SERVERSTOP;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}
		

		// ���̵� ���� ���� üũ : CP üũ
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


		// ������ ���� ��������
		sprintf_s(m_pQuery1, 4096, "Select %s, state, UNIX_TIMESTAMP() From %s Where %s = '%s'",  
							FIELD_MEMBER_PW, TABLE_MEMBER_BASE, FIELD_MEMBER_ID, pLogin->pUserID);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);				
			pLogin->nResult = LOGIN_RESULT_FAIL_ID;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}		
		
		// ��й�ȣ Ȯ��		
		if(strcmp(pRow[0], pLogin->pUserPW))
		{
			SAFE_DELETEDB(pResult);				
			pLogin->nResult = LOGIN_RESULT_FAIL_PW;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}

		// ����ȸ��üũ
		if(strcmp(pRow[1], "1") && _atoi64(pRow[1]) > _atoi64(pRow[2]))
		{
			SAFE_DELETEDB(pResult);				
			pLogin->nResult = LOGIN_RESULT_FAIL_STOP;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}
		SAFE_DELETEDB(pResult);	


		// �ߺ� ���� üũ (���� �α��� ��û�� ��� �ߺ����� �˸�)
		if(pClient->user_check(nMemberIdx) != 0 && pLogin->nProcess == 0) 
		{
			pLogin->nResult = LOGIN_RESULT_FAIL_DUPC;
			pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
			return 0;
		}

		// �α��� ���� ����		
		pLogin->nResult = LOGIN_RESULT_AUTH;	
		pClient->LogIn(pLogin->pUserID, nMemberIdx);		 
		pClient->send_packet_queue(CHK_AUTH, (char*)pLogin, sizeof(INFOAUTH));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��AuthUser()\n"))); 
	}

	return 0;
}

//20080122 ������ ����� ���� ���ÿ� ����� �ִ� ���� ������ ������Ʈ, jyh
void DBCtrl::UpLoad_Size(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	PUPLOADEND pTransEndInfo;
	char pTableFile[30];

	try
	{
		//printf("���ε� ������Ʈ �Լ� ����!\n");
		pTransEndInfo = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* ������� ���ε�� ���� ���� : Ÿ�Ժ��� �ٸ��� ���� */ 
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

		//printf("����No: %I64d, ���ÿ��� ���� �����ε���: %I64d", pClient->m_nFileNo, pTransEndInfo->nFileIdx);
		//if(pClient->m_nFileNo == pTransEndInfo->nFileIdx)// && pClient->m_nFileSizeBegin < pTransEndInfo->nFileSizeEnd)
		//{		
		sprintf_s(m_pQuery1, 4096, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, pTransEndInfo->nFileSizeEnd, pTransEndInfo->nFileIdx);
		ExecuteQuery(m_pQuery1);
		//}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadState()��Catch(...)\n"))); 
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���� ����� ��� ����
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



// ������ �������� üũ
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
			return 0; // ����
		}

		nReg = atol(pRow[0]);	
		nNow = atol(pRow[1]);	

		SAFE_DELETEDB(pResult);	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��Fix_Check\n"))); 
	}
	
	return 0; // ����
}
