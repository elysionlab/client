#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


// ���ϴٿ�ε� ���� ������ ������ �ش� ������ ���� ��û
void DBCtrl::DownLoadPre_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	char		pFlag[2];
	char		pTableFile[30];

	PLOADPREDOWN	pDownPre;

	try
	{

		pDownPre = (PLOADPREDOWN)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* �ٿ�ε� ��û���� ������ �������� üũ : Ÿ�Ժ��� �ٸ��� ���� */
		switch(pDownPre->nFileType)
		{
			case 0:
				strcpy(pTableFile, TABLE_FILE_BBS);
				break;
			case 1:
			case 2:
				strcpy(pTableFile, TABLE_FILE_BLOG);
				break;
			default:
				throw DOWN_RESULT_NOTFOUND;
		}
		sprintf_s(m_pQuery1, 4096, "Select realsize, flag_upload From %s Where no = %I64d", pTableFile, pDownPre->nFileIdx);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw DOWN_RESULT_NOTFOUND;
		}
		
		pDownPre->nFileRealSize	= _atoi64(pRow[0]);
		lstrcpy(pFlag, pRow[1]);
		SAFE_DELETEDB(pResult);	

		// ���ε尡 �Ϸ�� �������� üũ
		if(!strcmp(pFlag, "N"))
			throw DOWN_RESULT_UPLOAD;

		// ���������� ���� �ٿ�ε� ����
		if(m_bServiceRun) pDownPre->nResult = DOWN_RESULT_OK;			
		// ���� ������ ����
		else pDownPre->nResult = DOWN_RESULT_SERVICE_STOP;			

		pClient->send_packet_queue(LOAD_DOWNPRE, (char*)pDownPre, sizeof(LOADPREDOWN));
	}

	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPre()��Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pDownPre->nResult = nCode;			
		pClient->send_packet_queue(LOAD_DOWNPRE, (char*)pDownPre, sizeof(LOADPREDOWN));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPre()��Catch(...)\n"))); 

		pDownPre->nResult = DOWN_RESULT_ERROR;			
		pClient->send_packet_queue(LOAD_DOWNPRE, (char*)pDownPre, sizeof(LOADPREDOWN));
	}
}


// ���� �ٿ�ε带 ���� �ٿ�ε� ���� ��û	
void DBCtrl::DownLoadPro_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	int			nVolumeIdx = -1;
	ACE_UINT64	nSizeReal;
	ACE_UINT64	nDownTime = 0;
	char		pSaveName[256];
	char		pRealName[256];
	char		pFlag[2];
	char		pTableFile[30];

	int			nFilePayState;
	int			nFilePayMethod;

	ACE_UINT64	nFixDateCur;
	ACE_UINT64	nFixDateEnd;
	int			nFixTimeBegin;
	int			nFixTimeEnd;
	int			nFixTimeCur;

	char		pUseridCurrent[20];

	char		pServerIP[20];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	PLOADDOWN	pDownPro;
	bool		bFreeDown = false;

	try
	{
		pDownPro = (PLOADDOWN)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* �ٿ�ε� ��û���� ������ �������� üũ : Ÿ�Ժ��� �ٸ��� ���� */
		switch(pDownPro->nFileType)
		{
			case 0:
				strcpy(pTableFile, TABLE_FILE_BBS);
				break;
			case 1:
			case 2:
				strcpy(pTableFile, TABLE_FILE_BLOG);
				break;
			default:
				throw DOWN_RESULT_NOTFOUND;
		}

		sprintf_s(m_pQuery1, 4096, "Select flag_upload, realname, md5sum, size, realsize, UNIX_TIMESTAMP(), userid, temp_name, temp_volume From %s Where no = %I64d",  
							pTableFile, pDownPro->nFileIdx);
		/*sprintf(m_pQuery1,  "Select flag_upload, realname, md5sum, size, realsize, UNIX_TIMESTAMP(), userid, temp_name From %s Where no = %I64d",  
			pTableFile, pDownPro->nFileIdx);*/
	
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw DOWN_RESULT_NOTFOUND;
		}		
		strcpy(pFlag,		pRow[0]);
		strcpy(pRealName,	pRow[1]);
		nSizeReal	= _atoi64(pRow[4]);	
		nDownTime	= _atoi64(pRow[5]);	
		strcpy(pClient->m_pUseridFile,	pRow[6]);
		strcpy(pSaveName, pRow[7]);

		//20080310 ��û �������� 0�̸� ���� �ٿ�ε� �̹Ƿ� ��� �������� �����Ѵ�, jyh
		if(pDownPro->nServerVolume == 0)
		{
			nVolumeIdx	= atoi(pRow[8]);
			bFreeDown = true;
			//printf("���� ����:%d, ���� �ٿ�\n", pDownPro->nServerVolume);
		}
		else
		{
            nVolumeIdx	= pDownPro->nServerVolume;
			bFreeDown = false;
			//printf("���� ����:%d, ���� �ٿ�\n", pDownPro->nServerVolume);
		}

		SAFE_DELETEDB(pResult);
		//printf("�ٿ� ���� ���� �ּ�: %d\n", nVolumeIdx);

		/* ���ε尡 �Ϸ�� �������� üũ */
		if(!strcmp(pFlag, "N"))
			throw DOWN_RESULT_UPLOAD;

		/* ��û�� ���۽������� ���� ũ�� ���� ū�� üũ */
		if(nSizeReal <= pDownPro->nDownStartPoint)
			throw DOWN_RESULT_SIZEOVER;

		//20080310 ���� �ٿ�ε�� �ָ��� Ư�� �ð���� �ٿ���� ���ϰ� �Ѵ�, jyh-------------------------
		if(bFreeDown)
		{
			int nUseAll;
			int nWeekendUse;
			int nTimeUse;
			int nLimitUserUse;
			int nStartTime;
			int nEndTime;
			int nLimitUser;

			sprintf_s(m_pQuery1, 4096, "Select use_all, weekend_use, time_use, limituser_use, start_time, end_time, limituser "
								"From _webhard_free_config Where idx = 1");
			if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
			{
				nUseAll = atoi(pRow[0]);
				nWeekendUse = atoi(pRow[1]);
				nTimeUse = atoi(pRow[2]);
				nLimitUserUse = atoi(pRow[3]);
				nStartTime = atoi(pRow[4]);
				nEndTime = atoi(pRow[5]);
				nLimitUser = atoi(pRow[6]);
				//printf("UseAll:%d, WeekendUse:%d, TimeUse:%d, LimitUserUse:%d, StartTime:%d, EndTime:%d, LimitUser:%d\n",
				//		nUseAll, nWeekendUse, nTimeUse, nLimitUserUse, nStartTime, nEndTime, nLimitUser);
			}

			SAFE_DELETEDB(pResult);

			//���� �ٿ� �������� ��� �ϸ�...
			if(nUseAll)
			{
				//�ָ� ����ٿ� ���� 
				if(nWeekendUse)
				{
					sprintf_s(m_pQuery1, 4096, "Select dayofweek(now())");	//������ ���ڷ� ��ȯ 1:�Ͽ���, 2:������...
					if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
					{
						if(atoi(pRow[0]) == 1/*�Ͽ���*/ || atoi(pRow[0]) == 7/*�����*/)
						{
							//printf("����ٿ� �ָ� ��� ����: %d\n", atoi(pRow[0]));
							SAFE_DELETEDB(pResult);
							throw DOWN_RESULT_FREE_WEEKEND;		//�ָ� ��� ���� �������� ��ȯ
						}
					}

					SAFE_DELETEDB(pResult);
				}
				
				//���� Ư�� �ð��� ����ٿ� ����
				if(nTimeUse)
				{
					sprintf_s(m_pQuery1, 4096, "Select hour(now())");		//�ð��� ��ȯ ��)15:����3��
					if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
					{
						if(atoi(pRow[0]) >= nStartTime || atoi(pRow[0]) < nEndTime)
						{
							//printf("����ٿ� Ư�� �ð��� ��� ����: %d\n", atoi(pRow[0]));
							SAFE_DELETEDB(pResult);
							throw DOWN_RESULT_FREE_TIMEOVER;	//Ư�� �ð��� ��� ���� �������� ��ȯ
						}
					}

					SAFE_DELETEDB(pResult);
				}

				//���� �ٿ�ε� �ο� ���� üũ
				if(nLimitUserUse)
				{
					sprintf_s(m_pQuery1, 4096, "Select SUM(count_down) From _webhard_ftp_account Where ftp_volume = %d", nVolumeIdx);
					if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
					{
						if(atoi(pRow[0]) >= nLimitUser)
						{
							//printf("����ٿ� �ο� �������� ��� �Ұ�: %d\n", atoi(pRow[0]));
							SAFE_DELETEDB(pResult);
							throw DOWN_RESULT_FREE_LIMIT_COUNT;		
						}
					}
					
					SAFE_DELETEDB(pResult);
				}
			}
			
			pDownPro->nDownSpeed = 4;		//a5�� ���ϼ����� �����ϱ� ����, jyh
		}		
		//------------------------------------------------------------------------------------------------
		else
		{
			/* �ش� ������ ���� ���¿� ���� ���� üũ */
			switch(pDownPro->nFileType)
			{
				case 0:	// �ŷ���
				case 1:	// ��α�		
					for(int nFileTypeC = 0; nFileTypeC < 2; nFileTypeC++)
					{
						/*
						**** �α����� �ٸ� ���̵�� �α����� ��츦 Ȯ���ϱ� ����
						sprintf(m_pQuery1,  "Select state, method From %s " \
											"Where idx=%I64d And userid='%s' And link_idx=%I64d And target='%d' And expiredate > UNIX_TIMESTAMP() limit 1",  
											TABLE_PURCHASE, pDownPro->nBoardIndex_, pClient->m_pUserid, pDownPro->nBoardIndex, pDownPro->nFileType);
						*/

						sprintf_s(m_pQuery1, 4096, "Select state, method, userid From %s " \
											"Where idx=%I64d And link_idx=%I64d And target='%d' And state=0 And expiredate > UNIX_TIMESTAMP() limit 1",  
											TABLE_PURCHASE, pDownPro->nBoardIndex_, pDownPro->nBoardIndex, pDownPro->nFileType);
						if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
							break;
					}
					if(pRow == 0)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPro_BBS()���Ⱓ���� = %s"), m_pQuery1)); 
						SAFE_DELETEDB(pResult);	
						throw DOWN_RESULT_TIMEOVER;
					}
					
					nFilePayState	= atoi(pRow[0]);	
					nFilePayMethod	= atoi(pRow[1]);
					strcpy(pUseridCurrent, pRow[2]);
					SAFE_DELETEDB(pResult);	

					// ��Ŷ�� �ٿ�ε� �ӵ�
					pDownPro->nDownSpeed = 0;
					
					// ȯ�ҽ���
					if(nFilePayState == 2)
						throw DOWN_RESULT_PAY_CANCEL;

					// ������ ����
					if(nFilePayMethod == 2)
					{
						sprintf_s(m_pQuery1, 4096, "Select fix_date_start, fix_date_end, fix_time_start, fix_time_end, " \
											"UNIX_TIMESTAMP(), FROM_UNIXTIME(UNIX_TIMESTAMP(),'%%H') " \
											"From %s Where %s = '%s'",  
											TABLE_MEMBER_PAY, FIELD_MEMBER_ID, pUseridCurrent);
						if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
							throw DOWN_RESULT_INFOFAIL;
											
						nFixDateEnd		= _atoi64(pRow[1]);
						nFixDateCur		= _atoi64(pRow[4]);
						nFixTimeBegin	= atoi(pRow[2]);
						nFixTimeEnd		= atoi(pRow[3]);
						nFixTimeCur		= atoi(pRow[5]);
						SAFE_DELETEDB(pResult);	

						// ������ �Ⱓ ���� üũ
						if(nFixDateEnd < nFixDateCur)
							throw DOWN_RESULT_FIX_DATEOVER;
						
						//������ ������ �ð����� üũ 
						if(nFixTimeBegin != nFixTimeEnd)
						{
							if(nFixTimeBegin < nFixTimeEnd && (nFixTimeCur < nFixTimeBegin || nFixTimeEnd < nFixTimeCur))
								throw DOWN_RESULT_FIX_TIMEOVER;
							if(nFixTimeBegin > nFixTimeEnd && nFixTimeCur < nFixTimeBegin && nFixTimeEnd < nFixTimeCur)
								throw DOWN_RESULT_FIX_TIMEOVER;
						}			

						// ������ �ٿ�ε� �ӵ�
						pDownPro->nDownSpeed = 1;
					}
					break;
				case 2:	// �����ڷ�
					nFilePayMethod = 0;
					if(strcmp(pClient->m_pUserid, pClient->m_pUseridFile))
						throw DOWN_RESULT_OTHER_USER;
					break;
			}
		}
		
		//nVolumeIdx = LoadFtpInfoDownLoad(pSaveName);
		nVolumeIdx = LoadFtpInfoDownLoad(nVolumeIdx);	//20080204 ����, jyh
		if(nVolumeIdx == -1) throw DOWN_RESULT_NOTFOUND;
		else if(nVolumeIdx == -2) throw DOWN_RESULT_SERVERSTOP;		
		if(!LoadFtpCheck((__int64)nVolumeIdx)) throw DOWN_RESULT_SERVERBUSY;
		if(!LoadFtpInfoUpload((__int64)nVolumeIdx, pDownPro->nDownSpeed, pServerIP, nServerPT, pServerID, pServerPW)) throw DOWN_RESULT_SERVERSTOP;
		
		// FTP������ ������ �������� üũ
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW)) 
			{
				//20080527 ���ϼ����� ������ �ִ��� Ȯ���Ѵ�, jyh
				ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pSaveName);

				if(nServerFileSize == -2)
					throw DOWN_RESULT_NOTFOUND;
				//------------------------------------------------------------------------

				if(pClient->m_pFtp->QueryTransfer(pSaveName, pDownPro->nDownStartPoint, 1))
				{
					strcpy(pDownPro->pServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
					//DownCountSet_Criterion(pSaveName);	20080204 _webhard_filed�� �ʿ� ����, jyh
					break;
				}
				else	
					throw DOWN_RESULT_SERVERSTOP;					
			}
			else
			{
				if(nProc >= 2)
					throw DOWN_RESULT_SERVERSTOP;
			}
		}	

		// �ٿ�ε� �������� ���� ����
		pClient->m_nFileNo		= pDownPro->nFileIdx;
		pClient->m_nFileSizeBegin = pDownPro->nDownStartPoint;
		pClient->m_nFilePayMethod = nFilePayMethod;

		// ���������� ���� �ٿ�ε� ����
		pDownPro->nResult = DOWN_RESULT_OK;			
		pClient->send_packet_queue(LOAD_DOWNLOAD, (char*)pDownPro, sizeof(LOADDOWN));				
	}
	catch(int nCode)
	{
		pDownPro->nResult = nCode;			
		pClient->send_packet_queue(LOAD_DOWNLOAD, (char*)pDownPro, sizeof(LOADDOWN));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPro()��Catch(...)\n"))); 
		pDownPro->nResult = DOWN_RESULT_ERROR;			
		pClient->send_packet_queue(LOAD_DOWNLOAD, (char*)pDownPro, sizeof(LOADDOWN));
	}
}

// ���� ���� �ٿ�ε� ���� ������ �뺸��
void DBCtrl::DownLoad_State_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
}

// �ٿ�ε� �Ϸ� ���� ó��
void DBCtrl::DownLoadEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;

	double		nUsePacket = 0;
	double		nCommission = 0;
	__int64		nDownTransSize = 0;
	__int64		nDownLoadSize = 0;	

	PCHECKDOWNEND	pDownEnd;

	try
	{
		pDownEnd = (PCHECKDOWNEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		nDownTransSize = pClient->trans_result();	

		// �ٿ�ε� ���� ������
		if(pClient->m_nFileSizeBegin >= 0) pDownEnd->nSizeBegin = pClient->m_nFileSizeBegin;
		if(pDownEnd->nSizeEnd < pDownEnd->nSizeBegin) pDownEnd->nSizeEnd = pDownEnd->nSizeBegin;
		else nDownLoadSize = pDownEnd->nSizeEnd - pDownEnd->nSizeBegin;		
		nDownLoadSize = (nDownLoadSize > nDownTransSize) ? nDownLoadSize : nDownTransSize;

		// �α׻���
		sprintf_s(m_pQuery1, 4096, "Insert Into %s (file_no, board_idx, userid, userid_seller, type, method, size, ip, regdate) " \
							"Values (%I64d, %I64d, '%s', '%s', %d, %d, %I64d, '%s', UNIX_TIMESTAMP())", 
							TABLE_LOG_DOWNLOAD, pDownEnd->nBoardIndex, pDownEnd->nFileIdx, pClient->m_pUserid, pClient->m_pUseridFile, 
							pDownEnd->nFileType, pClient->m_nFilePayMethod, nDownLoadSize, pClient->m_pAddr_Remote.get_host_addr());
		//ExecuteQuery(m_pQuery1);

		pClient->m_nFileNo		= 0;
		pClient->m_nFileSizeBegin = 0;
		ZeroMemory(pClient->m_pUseridFile, sizeof(pClient->m_pUseridFile));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadEnd()��Catch(...)\n"))); 
	}
}

// �ٿ�ε� ������ �߰�
void DBCtrl::DownLoadItem_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	char		pTableFile[30];

	std::string m_strResult;

	try
	{
		CToken*	pToken = new CToken(SEP_FIELD);	

		char* pData = new char[nPS];
		memcpy(pData, (char*)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE), nPS);
		pToken->Split(pData);
		delete pData;
		
		//if(pToken->GetCount() == 3)
		if(pToken->GetCount() == 4)		//20080213 ���� �ּ� �߰�, jyh
		{			
			// ��û Ÿ�Կ� �´� ���̺� ����
			switch(atoi(pToken->GetToken(0)))
			{
				case 0:
					strcpy(pTableFile, TABLE_FILE_BBS);
					break;
				case 1:
				case 2:
					strcpy(pTableFile, TABLE_FILE_BLOG);
					break;
				default:
					delete pToken;				
					return;
			}

			sprintf_s(m_pQuery1, 4096, "Select no, size, realname, foldername From %s Where bbs_no = %s And flag_upload = 'Y' " \
								"order by depth asc, foldername asc, realname asc",  
								pTableFile, pToken->GetToken(2));
			
			if((pResult = ExecuteQueryR(m_pQuery1)) == 0)
			{
				delete pToken;				
				return;
			}

			// ��� ����
			m_strResult.clear();
			while(pRow = mysql_fetch_row(pResult))
			{				
				if(m_strResult != "")
					m_strResult = m_strResult + SEP_ITEM;

				m_strResult = m_strResult + pToken->GetToken(0) + SEP_FIELD;
				m_strResult = m_strResult + pToken->GetToken(1) + SEP_FIELD;
				m_strResult = m_strResult + pToken->GetToken(2) + SEP_FIELD;
				m_strResult = m_strResult + pRow[0] + SEP_FIELD;
				m_strResult = m_strResult + pRow[1] + SEP_FIELD;
				m_strResult = m_strResult + pRow[2] + SEP_FIELD;
				//20080213 ���� �ּ� �߰�, jyh
				//m_strResult = m_strResult + pRow[3];
				m_strResult = m_strResult + pRow[3] + SEP_FIELD;
				m_strResult = m_strResult + pToken->GetToken(3);

			}
			SAFE_DELETEDB(pResult);	
			if(m_strResult.length() > 0)
				pClient->send_packet_queue(LOAD_DOWNLIST, (char*)m_strResult.c_str(), (int)m_strResult.length() + 1);
		}		
		delete pToken;	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadItem()��Catch(...)\n"))); 
	}
}