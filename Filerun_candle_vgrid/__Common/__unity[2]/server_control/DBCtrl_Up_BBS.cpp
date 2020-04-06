#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"
#include <vector>


extern SERVER_RUNTIME_INFO RunServerInfo;

////20080825 �·�ī���� �����ϴ� ������ ������ ����, jyh
//void DBCtrl::UpLoadMureka_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//
//	PUPMUREKA	upMureka;
//	int nRet = 0;
//
//	try
//	{
//		upMureka = (PUPMUREKA)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
//
//		//_webhard_bbs�� state ���� 99�� ����, jyh
//		sprintf(m_pQuery1, "Update %s SET state = 99 where idx = %I64d", TABLE_BBS, upMureka->link_idx);
//		nRet = ExecuteQuery(m_pQuery1);
//	}
//	catch (...)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadMureka()��Catch(...), Contents_No: %I64d\n", upMureka->link_idx))); 
//	}
//}

////20071210 ���ε��� �ؽð� �� ��û, jyh
//void DBCtrl::UpLoadHash_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//
//	PUPHASH		upHash;
//	std::string m_strResult;
//	int nRet = 0;
//
//	try
//	{
//		upHash = (PUPHASH)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
//		//printf("link_idx: %d\n fileType: %d\n hashIndex: %s\n hashPath: %s\n checksum: %s\n\n",
//		//	upHash->link_idx, upHash->nFileType, upHash->szIndex, upHash->szPath, upHash->szCheckSum);
//		
//		CToken* pTokenIdx = new CToken("/");
//		CToken* pTokenCheckSum = new CToken("/");
//		
//		pTokenIdx->Split(upHash->szIndex);
//		pTokenCheckSum->Split(upHash->szCheckSum);
//		
//		//printf("checksum: %s\n", upHash->szCheckSum);
//		//printf("�ε��� ����: %d\n", pTokenIdx->GetCount());
//		//printf("�ؽ� ����: %d\n", pTokenCheckSum->GetCount());
//
//		int cnt = 0;	//�������� ��� �Ϸ� �Ǿ����� ���ϱ� ����
//		
//		if(lstrcmp(upHash->szIndex, ""))
//		{
//			//���۱� ���� ��� ������
//			for(size_t i=0; i<pTokenIdx->GetCount(); i++)
//			{
//				sprintf(m_pQuery1,  "Select idx From %s Where md5sum = '%s' and complete = 1", TABLE_HASH_COPYRIGHT, pTokenCheckSum->GetToken(i));
//				//printf("%s\n", m_pQuery1);
//
//				if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//				{
//					//printf("continue\n");
//					continue;
//				}
//				else
//				{
//					cnt++;
//					//printf("cnt: %d\n", cnt);
//				}
//
//				SAFE_DELETEDB(pResult);
//			}
//
//			//cnt���� ���ƾ� ������ ��ϵ� ��������
//			if(cnt == pTokenIdx->GetCount())
//			{
//				//printf("cnt�� �����ۼ��� ����. UPLOAD_RESULT_HASH_REG_EXIST\n");//test
//				//hash_copyright ���̺��� ����
//				sprintf(m_pQuery2, "Delete From %s where link_idx = %d", TABLE_HASH_COPYRIGHT, atoi(pTokenIdx->GetToken(0)));
//				//printf("%s\n", m_pQuery2);//test
//				nRet = ExecuteQuery(m_pQuery2);
//				//hash_bbs ���̺����� ����
//				sprintf(m_pQuery2, "Delete From %s where idx = %d", TABLE_HASH_BBS, atoi(pTokenIdx->GetToken(0)));
//				//printf("%s\n", m_pQuery2);//test
//				nRet = ExecuteQuery(m_pQuery2);
//				upHash->nResult = UPLOAD_RESULT_HASH_REG_EXIST;
//			}
//
//			if(upHash->nResult != UPLOAD_RESULT_HASH_REG_EXIST)
//			{
//				for(size_t j=0; j<pTokenIdx->GetCount(); j++)
//				{
//					//20080702 �ؽð��� ���ٸ� ��� ����, jyh
//					if(!lstrcmp(pTokenCheckSum->GetToken(j), ""))
//					{
//						//printf("UPLOAD_RESULT_HASH_REG_FAIL\n");
//						upHash->nResult = UPLOAD_RESULT_HASH_REG_FAIL;
//						break;
//					}
//
//					sprintf(m_pQuery2, "Update %s Set md5sum = '%s' where link_idx = %d", TABLE_HASH_COPYRIGHT, pTokenCheckSum->GetToken(j), atoi(pTokenIdx->GetToken(j)));
//					//printf("%d: %s\n", j, m_pQuery2);
//					nRet = ExecuteQuery(m_pQuery2);
//
//					if(nRet == QUERY_COMPLETE)
//					{
//						//printf("UPLOAD_RESULT_HASH_REG_OK\n");
//						upHash->nResult = UPLOAD_RESULT_HASH_REG_OK;
//					}
//					else
//					{
//						//printf("UPLOAD_RESULT_HASH_REG_FAIL\n");
//						upHash->nResult = UPLOAD_RESULT_HASH_REG_FAIL;
//						break;
//					}
//				}
//			}
//		}
//		else
//		{
//			for(size_t i=0; i<pTokenCheckSum->GetCount(); i++)
//			{
//				//�Ϲ� ���� ��� ������
//				sprintf(m_pQuery1,  "Select idx From %s Where md5sum = '%s' and disable = 0 and complete = 1", TABLE_HASH_COPYRIGHT, pTokenCheckSum->GetToken(i));
//
//				if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//				{
//					upHash->nResult = UPLOAD_RESULT_HASH_NOTEXIST;
//					SAFE_DELETEDB(pResult);	
//				}
//				else
//				{
//					//20080506 �ؽð��� ��ϵǾ� ������ _webhard_bbs���̺��� ������̴� �������� state���� 99���� �ٲ۴�
//					sprintf(m_pQuery2, "Update %s SET state = 99, is_del = '1' where idx = %d", TABLE_BBS, upHash->link_idx);
//					nRet = ExecuteQuery(m_pQuery2);
//
//					////�ؽð��� ��ϵǾ� ������ _webhard_file_bbs���̺��� ������̴� ������ ����� �����Ѵ�
//					//sprintf(m_pQuery2, "Delete From %s where bbs_no = %d", TABLE_FILE_BBS, upHash->link_idx);
//					//nRet = ExecuteQuery(m_pQuery2);
//					upHash->nResult = UPLOAD_RESULT_HASH_EXIST;
//					SAFE_DELETEDB(pResult);
//
//					break;
//				}	
//			}			
//		}
//			
//		pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
//		//printf("send packet: LOAD_UPHASH\n");
//		//printf("UPHASH ����ü ����\n");
//		//printf("nResult:%d, link_idx:%d, nFileType:%d, szIndex:%s, szPath:%s, szCheckSum:%s\n",
//		//		upHash->nResult, upHash->link_idx, upHash->nFileType, upHash->szIndex, upHash->szPath, upHash->szCheckSum);
//
//		delete pTokenIdx;
//		delete pTokenCheckSum;
//	}
//	catch (...)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadHash()��Catch(...)\n"))); 
//
//		upHash->nResult = UPLOAD_RESULT_HASH_ERROR;			
//		pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
//	}
//}

//20090304 ���ε��� �ؽð� �� ��û ����, jyh
void DBCtrl::UpLoadHash_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	PUPHASH		upHash;
	std::string strmd5sum;
	std::vector <std::string> v_md5sum;	//20090304 �������� �ؽð� ����, jyh
	__int64 n64Idx;	//20090304 �������� ���� ��ȣ(�ؽ� ���̺� ����� ���), jyh
	int nRet = 0;

	try
	{
		upHash = (PUPHASH)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
		//printf("bbs_no: %I64d, fileType: %d, hashIndex: %I64d\n",
		//	upHash->bbs_no, upHash->nFileType, upHash->hash_idx);

		//TABLE_FILE_BBS���� md5sum���� �����´�.
		sprintf(m_pQuery1, "Select md5sum From %s Where bbs_no = %I64d", TABLE_FILE_BBS, upHash->bbs_no);
		//printf("%s\n", m_pQuery1);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || nCount == 0)
		{
			//printf("UPLOAD_RESULT_HASH_ERROR\n");
			upHash->nResult = UPLOAD_RESULT_HASH_ERROR;
			SAFE_DELETEDB(pResult);
			pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
			//printf("send packet: LOAD_UPHASH\n");
			return;
		}
		else
		{
			while(pRow = mysql_fetch_row(pResult))
			{
				//printf("md5sum: %s\n", pRow[0]);
				if(!lstrcmp(pRow[0], ""))
				{
					upHash->nResult = UPLOAD_RESULT_HASH_NOTMD5SUM;
					SAFE_DELETEDB(pResult);
					pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
					//printf("send packet: LOAD_UPHASH\n");
					return;
				}

				strmd5sum = pRow[0];
				v_md5sum.push_back(strmd5sum);
			}

			SAFE_DELETEDB(pResult);
		}
				
		int cnt = 0;	//�������� ��� �Ϸ� �Ǿ����� ���ϱ� ����

		if(upHash->hash_idx > 0)
		{			
			//���۱� ���� ��� ������
			for(int i=0; i<(int)v_md5sum.size(); i++)
			{
				sprintf(m_pQuery1,  "Select idx From %s Where md5sum = '%s' and complete = 1", TABLE_HASH_COPYRIGHT, v_md5sum[i].c_str());
				//printf("%s\n", m_pQuery1);

				if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
				{
					//printf("continue\n");
					continue;
				}
				else
				{
					cnt++;
					//printf("cnt: %d\n", cnt);
				}

				SAFE_DELETEDB(pResult);
			}

			//cnt���� ���ƾ� ������ ��ϵ� ��������
			if(cnt == (int)v_md5sum.size())
			{
				//printf("cnt�� �����ۼ��� ����. UPLOAD_RESULT_HASH_REG_EXIST\n");//test
				//hash_copyright ���̺��� ����
				sprintf(m_pQuery2, "Delete From %s where link_idx = %I64d", TABLE_HASH_COPYRIGHT, upHash->hash_idx);
				//printf("%s\n", m_pQuery2);//test
				nRet = ExecuteQuery(m_pQuery2);
				//hash_bbs ���̺����� ����
				sprintf(m_pQuery2, "Delete From %s where idx = %I64d", TABLE_HASH_BBS, upHash->hash_idx);
				//printf("%s\n", m_pQuery2);//test
				nRet = ExecuteQuery(m_pQuery2);
				upHash->nResult = UPLOAD_RESULT_HASH_REG_EXIST;
			}

			if(upHash->nResult != UPLOAD_RESULT_HASH_REG_EXIST)
			{				
				for(int j=0; j<(int)v_md5sum.size(); j++)
				{
					//20080702 �ؽð��� ���ٸ� ��� ����, jyh
					if(!lstrcmp(v_md5sum[j].c_str(), ""))
					{
						//printf("UPLOAD_RESULT_HASH_REG_FAIL\n");
						upHash->nResult = UPLOAD_RESULT_HASH_NOTMD5SUM;
						break;
					}

					sprintf(m_pQuery2, "Update %s Set md5sum = '%s' where link_idx = %I64d", TABLE_HASH_COPYRIGHT, v_md5sum[j].c_str(), upHash->hash_idx);
					//printf("%d: %s\n", j, m_pQuery2);
					nRet = ExecuteQuery(m_pQuery2);

					if(nRet == QUERY_COMPLETE)
					{
						//printf("UPLOAD_RESULT_HASH_REG_OK\n");
						upHash->nResult = UPLOAD_RESULT_HASH_REG_OK;
					}
					else
					{
						//printf("UPLOAD_RESULT_HASH_REG_FAIL\n");
						upHash->nResult = UPLOAD_RESULT_HASH_REG_FAIL;
						break;
					}
				}
			}
		}
		else
		{
			for(int i=0; i<(int)v_md5sum.size(); i++)
			{
				//�Ϲ� ���� ��� ������
				sprintf(m_pQuery1,  "Select idx From %s Where md5sum = '%s' and disable = 0 and complete = 1", TABLE_HASH_COPYRIGHT, v_md5sum[i].c_str());
				//printf("%s\n", m_pQuery1);
				if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
				{
					//printf("UPLOAD_RESULT_HASH_NOTEXIST(���ϵ�� ����)\n");
					upHash->nResult = UPLOAD_RESULT_HASH_NOTEXIST;
					SAFE_DELETEDB(pResult);	
				}
				else
				{
					//20080506 �ؽð��� ��ϵǾ� ������ _webhard_bbs���̺��� ������̴� �������� state���� 99���� �ٲ۴�
					sprintf(m_pQuery2, "Update %s SET state = 99, is_del = '1' where idx = %d", TABLE_BBS, upHash->bbs_no);
					//printf("%s", m_pQuery2);
					nRet = ExecuteQuery(m_pQuery2);

					////�ؽð��� ��ϵǾ� ������ _webhard_file_bbs���̺��� ������̴� ������ ����� �����Ѵ�
					//sprintf(m_pQuery2, "Delete From %s where bbs_no = %d", TABLE_FILE_BBS, upHash->link_idx);
					//nRet = ExecuteQuery(m_pQuery2);
					upHash->nResult = UPLOAD_RESULT_HASH_EXIST;
					SAFE_DELETEDB(pResult);

					break;
				}	
			}				
		}

		pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
		//printf("send packet: LOAD_UPHASH\n");
		//printf("UPHASH ����ü ����\n");
		//printf("nResult:%d, link_idx:%d, nFileType:%d, szIndex:%s, szPath:%s, szCheckSum:%s\n",
		//		upHash->nResult, upHash->link_idx, upHash->nFileType, upHash->szIndex, upHash->szPath, upHash->szCheckSum);

		
	}
	catch (...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadHash()��Catch(...)\n"))); 

		upHash->nResult = UPLOAD_RESULT_HASH_ERROR;			
		pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
	}
}

//20071211 ���� ���� �Ϸ� ��Ŷ, jyh
void DBCtrl::UpLoadGuraEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	//MYSQL_ROW	pRow;

	PUPLOADEND pGuraUploadEnd;
	int	nRet = 0;

	try
	{
		pGuraUploadEnd = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		sprintf(m_pQuery1, "Update %s Set complete = 1 where link_idx = %I64d", TABLE_HASH_COPYRIGHT, pGuraUploadEnd->nFileIdx);
		//printf("%s\n", m_pQuery1);//test
		nRet = ExecuteQuery(m_pQuery1);

		//���н� �ѹ���...
		if(nRet != QUERY_COMPLETE)
		{
			//printf("%s\n", m_pQuery1);
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw UPLOAD_RESULT_ERROR_DB;
		}

		if(pGuraUploadEnd->nContentsCmpl == 1)
		{
			sprintf(m_pQuery1, "Update %s Set state = 1 where idx = %I64d LIMIT 1", TABLE_HASH_BBS, pGuraUploadEnd->nFileIdx);
			printf("%s\n", m_pQuery1);//test
			nRet = ExecuteQuery(m_pQuery1);
		}

		//���н� �ѹ���...
		if(nRet != QUERY_COMPLETE)
		{
			//printf("%s\n", m_pQuery1);
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw UPLOAD_RESULT_ERROR_DB;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadGuraEnd()��Catch(...)\n"))); 
	}
}

// ���ε� ������ ���� ��� �Ǵ� �ø��� ���� ���� ��������
void DBCtrl::UpLoadPre_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	ACE_UINT64	nSize;
	ACE_UINT64	nVolumeIdx;
	
	char		pFlag[2];
	char		pFlag_Real[2];
	char		pFlag_Unity[2];
	char		pTempName[256];
	char		pTableFile[30];
	char		pTableFilePath[30];

	char		pServerIP[80];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	PUPLOADPRE	pUploadPre;

	try
	{
		pUploadPre = (PUPLOADPRE)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* ���ε� ��û���� ������ �������� üũ : Ÿ�Ժ��� �ٸ��� ���� */
		switch(pUploadPre->nFileType)
		{
			case 0:
				strcpy(pTableFile, TABLE_FILE_BBS);
				strcpy(pTableFilePath, TABLE_FILE_BBS_PATH);
				break;
			case 1:
			case 2:
				strcpy(pTableFile, TABLE_FILE_BLOG);
				strcpy(pTableFilePath, TABLE_FILE_BLOG_PATH);
				break;
			default:
				throw UPLOAD_RESULT_NOTFOUND;
		}

		sprintf(m_pQuery1,  "Select size, realsize, temp_volume, flag_upload, flag_realupload, temp_name, flag_unity From %s Where no = %I64d",  
							pTableFile, pUploadPre->nFileIdx);
		//printf("%s\n", m_pQuery1);

		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw UPLOAD_RESULT_NOTFOUND;
		}
		
		nSize							= _atoi64(pRow[0]);	
		pUploadPre->nFileSize_Complete	= _atoi64(pRow[1]);
		nVolumeIdx						= _atoi64(pRow[2]);
		strcpy(pFlag,		pRow[3]);
		strcpy(pFlag_Real,	pRow[4]);		
		strcpy(pTempName,	pRow[5]);			
		strcpy(pFlag_Unity,	pRow[6]);	
		SAFE_DELETEDB(pResult);

		//printf("flag_upload: %s\n", pFlag);


		// ���ϻ����� üũ : ���������� ����� ��Ͻ� ���� ������� �����
		if(pUploadPre->nFileSize != nSize)
			throw UPLOAD_RESULT_SIZE;

		
		// ���ε尡 �Ϸ���� �ʰ� ������ �̾� �ø��� �϶� ������ �ٽ� üũ
		if(!strcmp(pFlag, "N") && pUploadPre->nFileSize_Complete > 0 && !strcmp(pFlag_Real, "Y"))
		{
			if(LoadFtpInfoUpload(nVolumeIdx, 0, pServerIP, nServerPT, pServerID, pServerPW))
			{
				// FTP������ ������ �������� üũ
				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
				{
					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pTempName);
					
					// ������ �������� ���� ���
					if(nServerFileSize == -2)
					{
						pUploadPre->nFileSize_Complete = 0;

						// ������ �⺻���� �ʱ�ȭ�ؼ� ���Ӱ� ������ �ø����� ����
						sprintf(m_pQuery1, "Update %s Set temp_name='',md5sum='',realsize=0 Where no=%I64d", pTableFile, pUploadPre->nFileIdx);
						ExecuteQuery(m_pQuery1);

						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��File Continue : Not Found : %s\n"), pClient->getuserinfo()));
					}
					
					// ������ �����ϰ� ����� 0�Ǵ� 0���� Ŀ����
					if(nServerFileSize >= 0 && nSize >= nServerFileSize && nServerFileSize != pUploadPre->nFileSize_Complete)
					{
						pUploadPre->nFileSize_Complete = nServerFileSize;

						// ��� ����������� ���ϼ����� ���� ����� �ٸ���� ������Ʈ
						sprintf(m_pQuery1, "Update %s Set realsize=%I64d Where no = %I64d", pTableFile, nServerFileSize, pUploadPre->nFileIdx);
						ExecuteQuery(m_pQuery1);

						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��File Continue : Size difference : %s\n"), pClient->getuserinfo()));
					}

					//20081113 ���ϼ����� ���� ����� ��� ��ϵ� ������� Ŭ��� ftp���� ���� �����ϰ�
					//����� realsize = 0���� ������Ʈ ���� ó������ �ٽ� �ø���, jyh
					if(nServerFileSize > nSize)
					{
						pClient->m_pFtp->RemoveFile(pTempName);
						pUploadPre->nFileSize_Complete = 0;
						sprintf(m_pQuery1, "Update %s Set realsize=0 Where no = %I64d", pTableFile, pUploadPre->nFileIdx);
						ExecuteQuery(m_pQuery1);

						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��File Continue : ���ϼ����� ���� ũ�Ⱑ �������� ���� ŭ Size difference : %s\n"), pClient->getuserinfo()));
					}

					pClient->m_pFtp->DisConnect();
				}
			}
		}
		
		// ���� ���ε� �Ϸ�
		if(!strcmp(pFlag, "Y"))
			throw UPLOAD_RESULT_END;
		else
		{
			// ���ε� �Ϸ��� �϶� �����н� �����ϴ��� üũ
			sprintf(m_pQuery1,  "Select t_files.no From %s t_pds, %s t_files Where t_pds.no = %I64d And t_pds.no = t_files.no",  
								pTableFile, pTableFilePath, pUploadPre->nFileIdx);
			if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			{
				SAFE_DELETEDB(pResult);	
				throw UPLOAD_RESULT_PATH;
			}
			SAFE_DELETEDB(pResult);	

			// ���������� ���� ���ε� ����
			if(m_bServiceRun) pUploadPre->nResult = UPLOAD_RESULT_NEW;			
			// ���� ������ ����
			else pUploadPre->nResult = UPLOAD_RESULT_SERVICE_STOP;		

			pClient->send_packet_queue(LOAD_UPPRE, (char*)pUploadPre, sizeof(UPLOADPRE));
		}
	}

	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pUploadPre->nResult = nCode;			
		pClient->send_packet_queue(LOAD_UPPRE, (char*)pUploadPre, sizeof(UPLOADPRE));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��Catch(...)\n"))); 

		pUploadPre->nResult = UPLOAD_RESULT_ERROR;			
		pClient->send_packet_queue(LOAD_UPPRE, (char*)pUploadPre, sizeof(UPLOADPRE));
	}
}

//// ���� ���� ���ε带 ���� ���ε� ���� ���� ��������
//void DBCtrl::UpLoadProcess_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//
//	ACE_UINT64	nSize;
//	ACE_UINT64	nSizeReal;
//	char		pFlag_Upload[2];
//	char		pFlag_RealUpload[2];
//	char		pFlag_Unity[2];
//	char		pFlag_CheckSum[40];
//	char		pSaveName[256];	
//	char		pTempName[256];
//	char		pTempTimeStamp[50];
//	char		pTableFile[30];
//
//	char		pServerIP[20];
//	char		pServerID[20];
//	char		pServerPW[20];
//	int			nServerPT;
//
//	PUPLOADPRO	pUploadPor;
//
//	try
//	{
//		pUploadPor = (PUPLOADPRO)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
//
//		if(pUploadPor->nUploadSpeed < 0) pUploadPor->nUploadSpeed = 0;
//		if(pUploadPor->nUploadSpeed > 9) pUploadPor->nUploadSpeed = 9;
//
//		/* ���� ���ε� �� ���� üũ : Ÿ�Ժ��� �ٸ��� ���� */ 
//		switch(pUploadPor->nFileType)
//		{
//		case 0:
//			strcpy(pTableFile, TABLE_FILE_BBS);
//			break;
//		case 1:
//		case 2:
//			strcpy(pTableFile, TABLE_FILE_BLOG);
//			break;
//		default:
//			throw UPLOAD_RESULT_NOTFOUND;
//		}
//		sprintf(m_pQuery1,  "Select size, realsize, flag_upload, md5sum, flag_realupload, temp_name, flag_unity, UNIX_TIMESTAMP() " \
//			"From %s Where no = %I64d", pTableFile, pUploadPor->nFileIdx);
//		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//		{
//			SAFE_DELETEDB(pResult);	
//			throw UPLOAD_RESULT_NOTFOUND;
//		}
//
//		nSize		= _atoi64(pRow[0]);	
//		nSizeReal	= _atoi64(pRow[1]);
//		strcpy(pFlag_Upload, pRow[2]);
//		strcpy(pFlag_CheckSum, pRow[3]);
//		strcpy(pFlag_RealUpload, pRow[4]);
//		strcpy(pSaveName, pRow[5]);		
//		strcpy(pFlag_Unity, pRow[6]);
//		strcpy(pTempTimeStamp, pRow[7]);		
//		SAFE_DELETEDB(pResult);	
//
//		// ���ε��� �Ϸ�� ����� ���� ������ Ȯ���� ���� ����
//		pClient->m_nFileNo			= pUploadPor->nFileIdx;
//		pClient->m_nFileSizeBegin	= nSizeReal;
//
//
//		// �Ϸ�� �������� üũ
//		if(!strcmp(pFlag_Upload, "Y"))
//			throw UPLOAD_RESULT_END;
//
//		// ���� ���ۻ���� ���� ���� ��������� ���� ū���
//		if(pUploadPor->nFileSize_Start > nSizeReal)
//			throw UPLOAD_RESULT_SIZE;
//
//		// �ʱ� ��ϵ� CheckSum �� ���� �ٸ���� �ٸ�����
//		if(strcmp(pFlag_CheckSum, "") && strcmp(pFlag_CheckSum, pUploadPor->pFileCheckSum))
//		{			
//			ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadProcess()��md5sum error : OLD = %s, NEW = %s : %s\n"), pFlag_CheckSum, pUploadPor->pFileCheckSum, pClient->getuserinfo())); 		
//			strcpy(pFlag_CheckSum, "");
//		}
//
//		// CheckSum ���� ��ϵ��� �ʾ����� ���
//		if(!strcmp(pFlag_CheckSum, ""))
//		{
//			sprintf(m_pQuery1, "Update %s Set md5sum = '%s', upload_date = UNIX_TIMESTAMP() Where no = %I64d", 
//				pTableFile, pUploadPor->pFileCheckSum, pUploadPor->nFileIdx);
//			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//				throw UPLOAD_RESULT_ERROR_DB;
//		}
//
//		// ���ϸ��� �����ϸ� pFlag_RealUpload ������ ���� ���� ���ε� �Ǵ� Ʈ�����ε� ����
//		if(!strcmp(pSaveName, ""))
//		{
//			__int64 nCheckResult = -1;	
//
//			// �ӽ� ���ϸ� ����
//			if(OPTION_MIN_FILEUNITY < nSize)
//			{
//				sprintf(pTempName, "%s_%I64d", pUploadPor->pFileCheckSum, nSize);
//				strcpy(pFlag_Unity, "Y");
//
//				int nSameFileCountCore = LoadFileCountCore(pTempName);
//				int nSameFileCountBBS = LoadFileCountCp(pUploadPor->pFileCheckSum, nSize);
//				if(nSameFileCountCore != 0 && nSameFileCountBBS != 0 && RunServerInfo.nLimit_DuplicateFile > 1 && nSameFileCountBBS <= (nSameFileCountCore * RunServerInfo.nLimit_DuplicateFile))
//					nCheckResult = LoadNewFileCheck(pTempName, nSize);
//			}
//			else
//			{
//				sprintf(pTempName, "%s_%I64d_%s", pUploadPor->pFileCheckSum, nSize, pTempTimeStamp);
//				strcpy(pFlag_Unity, "N");
//			}			
//
//			// ���� ���� üũ
//			if(nCheckResult == -1)
//			{				
//				// ���� ���ε�
//				strcpy(pFlag_RealUpload, "Y");				
//
//				// ���ϸ� ����
//				if(OPTION_MIN_FILEUNITY < nSize)
//					sprintf(pSaveName, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
//				else
//					strcpy(pSaveName, pTempName);
//
//				// FTP ���� �Ҵ�
//				__int64 nAllotment = 0;
//
//				// ���� �����ڰ� ���ε� �ߴ� ������ ������� �ش� �������� ���ε�, �� �ش� ������ ���°� ���ε� ������ ���¿�����
//				if(pClient->m_nLastVolumeUpload != 0 && LoadFtpAllotmentQueue(pClient->m_nLastVolumeUpload))
//					nAllotment = pClient->m_nLastVolumeUpload;
//
//				if(nAllotment == 0)
//				{
//					if((nAllotment = LoadFtpAllotment()) == -1)
//						throw UPLOAD_RESULT_ERROR_ALLO;
//					else
//						pClient->m_nLastVolumeUpload = nAllotment;
//				}
//
//				sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
//					pTableFile, nAllotment, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
//				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//					throw UPLOAD_RESULT_ERROR_DB;
//			}
//			else
//			{
//				// ���� ���ε� ���� ����
//				strcpy(pFlag_RealUpload, "N");
//
//				// ���ϸ� ����
//				strcpy(pSaveName, pTempName);
//
//				sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_realupload = 'N', flag_unity='%s' Where no = %I64d", 
//					pTableFile, nCheckResult, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
//				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//					throw UPLOAD_RESULT_ERROR_DB;				
//			}			
//		}
//
//		// ���� ���ε��� ��� �ش� ������ ���� �� ������ ä�� ����
//		if(!strcmp(pFlag_RealUpload, "Y"))
//		{
//			pUploadPor->nResult	= UPLOAD_RESULT_NEW;
//
//			sprintf(m_pQuery1,  "Select temp_volume From %s Where no = %I64d",  pTableFile, pUploadPor->nFileIdx);
//			if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//			{
//				SAFE_DELETEDB(pResult);	
//				throw UPLOAD_RESULT_ERROR_ALLO;
//			}
//
//			ACE_UINT64 nVolumeIdx = _atoi64(pRow[0]);	
//			SAFE_DELETEDB(pResult);	
//
//			if(!LoadFtpCheck(nVolumeIdx)) throw UPLOAD_RESULT_SERVERBUSY;
//			if(!LoadFtpInfoUpload(nVolumeIdx, pUploadPor->nUploadSpeed, pServerIP, nServerPT, pServerID, pServerPW))
//				throw UPLOAD_RESULT_ERROR_STOP;
//
//
//			// FTP������ ������ �������� üũ
//			for(int nProc = 0;; nProc++)
//			{
//				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW)) 
//				{
//					if(pClient->m_pFtp->QueryTransfer(pSaveName, pUploadPor->nFileSize_Start, 0))
//					{
//						strcpy(pUploadPor->pServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
//						break;
//					}
//					else	
//						throw UPLOAD_RESULT_ERROR_DELAY;					
//				}
//				else
//				{
//					ACE_DEBUG((LM_WARNING, ACE_TEXT("������������������������ %s : %s\n"), pServerIP, pServerID)); 	
//
//					if(nProc >= 2)
//						throw UPLOAD_RESULT_ERROR_DELAY;
//				}
//			}			
//		}
//		else
//		{
//			pUploadPor->nResult	= UPLOAD_RESULT_TRICK;
//		}		
//		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));		
//	}
//	catch(int nCode)
//	{		
//		pUploadPor->nResult	= nCode;
//		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));
//	}
//	catch(...)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadProcess()��Catch(...)\n"))); 		
//		pUploadPor->nResult	= UPLOAD_RESULT_ERROR;
//		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));
//	}
//}

// ���� ���� ���ε带 ���� ���ε� ���� ���� ��������(�Ѽ��� ���ε�� ���� ����)
void DBCtrl::UpLoadProcess_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_RES	*pResult_1 = 0;
	MYSQL_ROW	pRow;
	MYSQL_ROW	pRow_1;
//	MYSQL_FIELD pField;

	ACE_UINT64	nSize;
	ACE_UINT64	nSizeReal;
	
	char		pFlag_Upload[2];
	char		pFlag_RealUpload[2];
	char		pFlag_Unity[2];
	char		pFlag_CheckSum[40];
	//char		pFlag_MurekaID[40];	//20080821 �·�ī ���� ���̵�, jyh
	char		pSaveName[256];	
	char		pTempName[256];
	char		pTempTimeStamp[50];
	char		pTableFile[30];

	char		pServerIP[80];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;
	int			nTempVolume = 0;	//20071126 ���� ���� ��ȣ, jyh
	int			nIdx;			//20071126 ������ �ε���, jyh
	int			nbbsNo;			//20071126 �Խù� �ε���, jyh
//	int			nMinIdx;		//20080129 �Խù��� �ε����� �ּҰ�, jyh

	PUPLOADPRO	pUploadPor;

	try
	{
		pUploadPor = (PUPLOADPRO)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		//printf("pUploadPor->nResult:%d, pUploadPor->nFileType:%d, pUploadPor->nBoardIndex:%I64d, pUploadPor->nFileIdx:%I64d, pUploadPor->nFileSize_Start:%I64d, pUploadPor->nUploadSpeed:%d, pUploadPor->pFileCheckSum:%s, pUploadPor->pServerInfo:%s\n",
		//	pUploadPor->nResult, pUploadPor->nFileType, pUploadPor->nBoardIndex, pUploadPor->nFileIdx, pUploadPor->nFileSize_Start, pUploadPor->nUploadSpeed, pUploadPor->pFileCheckSum, pUploadPor->pServerInfo);//test

		//if(pUploadPor->nUploadSpeed < 0) pUploadPor->nUploadSpeed = 0;
		//if(pUploadPor->nUploadSpeed > 9) pUploadPor->nUploadSpeed = 9;
		pUploadPor->nUploadSpeed = 100;

		/* ���� ���ε� �� ���� üũ : Ÿ�Ժ��� �ٸ��� ���� */ 
		switch(pUploadPor->nFileType)
		{
			case 0:
				strcpy(pTableFile, TABLE_FILE_BBS);
				break;
			case 1:
			case 2:
				strcpy(pTableFile, TABLE_FILE_BLOG);
				break;
			default:
				throw UPLOAD_RESULT_NOTFOUND;
		}
		/*sprintf(m_pQuery1,  "Select size, realsize, flag_upload, md5sum, flag_realupload, temp_name, flag_unity, UNIX_TIMESTAMP() " \
							"From %s Where no = %I64d", pTableFile, pUploadPor->nFileIdx);*/
		////20080821 �·�ī ���� ���̵� �߰�, jyh
		//sprintf(m_pQuery1,  "Select size, realsize, flag_upload, md5sum, flag_realupload, temp_name, flag_unity, UNIX_TIMESTAMP(), idx, bbs_no, temp_volume, murekaid " \
		//	"From %s Where no = %I64d", pTableFile, pUploadPor->nFileIdx);
		sprintf(m_pQuery1,  "Select size, realsize, flag_upload, md5sum, flag_realupload, temp_name, flag_unity, UNIX_TIMESTAMP(), idx, bbs_no, temp_volume " \
			"From %s Where no = %I64d", pTableFile, pUploadPor->nFileIdx);
		
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw UPLOAD_RESULT_NOTFOUND;
		}
		
		nSize		= _atoi64(pRow[0]);	
		nSizeReal	= _atoi64(pRow[1]);
		strcpy(pFlag_Upload, pRow[2]);
		strcpy(pFlag_CheckSum, pRow[3]);
		strcpy(pFlag_RealUpload, pRow[4]);
		strcpy(pSaveName, pRow[5]);		
		strcpy(pFlag_Unity, pRow[6]);
		strcpy(pTempTimeStamp, pRow[7]);
		nIdx		= atoi(pRow[8]);	//20071126 ������ �ε���, jyh
		nbbsNo		= atoi(pRow[9]);	//20071126 �Խù� �ε���, jyh
		nTempVolume	= atoi(pRow[10]);	//20080212 ���� �ּ�, jyh
		//strcpy(pFlag_MurekaID, pRow[11]);	//20080821 �·�ī ���� ���̵�, jyh
		SAFE_DELETEDB(pResult);	

		// ���ε��� �Ϸ�� ����� ���� ������ Ȯ���� ���� ����
		pClient->m_nFileNo			= pUploadPor->nFileIdx;
		pClient->m_nFileSizeBegin	= nSizeReal;


		// �Ϸ�� �������� üũ
		if(!strcmp(pFlag_Upload, "Y"))
			throw UPLOAD_RESULT_END;

		// ���� ���ۻ���� ���� ���� ��������� ���� ū���
		if(pUploadPor->nFileSize_Start > nSizeReal)
			throw UPLOAD_RESULT_SIZE;

		// �ʱ� ��ϵ� CheckSum �� ���� �ٸ���� �ٸ�����
		if(strcmp(pFlag_CheckSum, "") && strcmp(pFlag_CheckSum, pUploadPor->pFileCheckSum))
		{			
			ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadProcess()��md5sum error : OLD = %s, NEW = %s : %s\n"), pFlag_CheckSum, pUploadPor->pFileCheckSum, pClient->getuserinfo())); 		
			strcpy(pFlag_CheckSum, "");
		}

		// CheckSum ���� ��ϵ��� �ʾ����� ���
		if(!strcmp(pFlag_CheckSum, ""))
		{
			sprintf(m_pQuery1, "Update %s Set md5sum = '%s', upload_date = UNIX_TIMESTAMP() Where no = %I64d", 
								pTableFile, pUploadPor->pFileCheckSum, pUploadPor->nFileIdx);
			//printf("%s\n", m_pQuery1);
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw UPLOAD_RESULT_ERROR_DB;
		}

		////20080821 �·�ī ���� ���̵� ��ϵ��� �ʾ����� ���, jyh
		//if(!strcmp(pFlag_MurekaID, ""))
		//{
		//	sprintf(m_pQuery1, "Update %s Set murekaid = '%s' Where no = %I64d", 
		//	pTableFile, pUploadPor->pMurekaFileID, pUploadPor->nFileIdx);
		//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
		//		throw UPLOAD_RESULT_ERROR_DB;
		//}

		// ���ϸ��� �����ϸ� pFlag_RealUpload ������ ���� ���� ���ε� �Ǵ� Ʈ�����ε� ����
		if(!strcmp(pSaveName, ""))
		{
			__int64 nCheckResult = -1;	
            
			// �ӽ� ���ϸ� ����
			//20080123 ���뷮 ���ϵ� Ʈ�����ε带 ����, jyh
			sprintf(pTempName, "%s_%I64d", pUploadPor->pFileCheckSum, nSize);
			strcpy(pFlag_Unity, "Y");
			
			int nSameFileCountBBS = LoadFileCountCp(pUploadPor->pFileCheckSum, nSize);

			//20080124 ���ε� �Ϸ�� ���� ������ 3���� ������ �������� ������ �������ε� ������ Ʈ�� ���ε�, jyh

			//20100818 - ������ ���°�츦 �����ϰ� ��� Ʈ�� ���ε��� - ent24
			//if((nSameFileCountBBS % RunServerInfo.nLimit_DuplicateFile) == 0)
			if(0)
			{
				//printf("���� ���ε�\n");
				//���� ���ε�
				strcpy(pFlag_RealUpload, "Y");				

				// ���ϸ� ����
				//if(OPTION_MIN_FILEUNITY < nSize)
					sprintf(pSaveName, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
				//else
				//	strcpy(pSaveName, pTempName);
				
				//20080212 ���� �Ҵ��� �ȵ������� �Ҵ� �Ѵ�, jyh
				if(nTempVolume == 0)
				{
					if((nTempVolume = (int)LoadFtpAllotment()) == -1)
						throw UPLOAD_RESULT_ERROR_ALLO;

					//20080511 ���ε�� ������ȣ�� �޶����°� ����, jyh
					sprintf(m_pQuery2, "Update %s Set temp_volume = %d Where bbs_no = %I64d", pTableFile, nTempVolume, pUploadPor->nBoardIndex);
					//printf("%s\n", m_pQuery2);
					if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
						throw UPLOAD_RESULT_ERROR_DB;
				}
				
				//20080511 ���ε�� ������ȣ�� �޶����°� ����, jyh
				/*sprintf(m_pQuery1, "Update %s Set temp_volume = %d, temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
					pTableFile, nTempVolume, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);*/
				sprintf(m_pQuery1, "Update %s Set temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
					pTableFile, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
				//printf("%s\n", m_pQuery1);
				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
					throw UPLOAD_RESULT_ERROR_DB;
			}
			else
			{
				//printf("Ʈ�� ���ε�\n");
				bool bReal = false; //���ϼ����� ���� ������ ������ ���� ���ε带 �ؾ��Ѵ�, jyh
				//Ʈ�� ���ε�
				strcpy(pFlag_RealUpload, "N");

				//20080529 ù��° ���Ͽ� ���� �Ҵ��� �ȵ� ���, jyh
				if(nTempVolume == 0)
				{
					sprintf(m_pQuery1,  "Select temp_volume From %s Where md5sum='%s' and flag_realupload = 'Y' and contents_complete = 1 order by (no) desc", pTableFile, pUploadPor->pFileCheckSum);

					if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
					{
						SAFE_DELETEDB(pResult);	
						
						//20080529 Ʈ�����ε�� �����޴� ������ ������ ù��° ������ ��� ������ ���Ҵ� �޴´�, jyh
						//�ؿ��� �������� ������ ������ ����� �ִ��� �˻��ϰ� ������ ����� ������ ���� ���ε�� �����ϴ�
						//��ƾ�� �ֱ� ������ �ϴ� ���ο� ���� �Ҵ�
						if((nTempVolume = (int)LoadFtpAllotment()) == -1)
							throw UPLOAD_RESULT_ERROR_ALLO;
			
						sprintf(m_pQuery2, "Update %s Set temp_volume = %d Where bbs_no = %I64d", pTableFile, nTempVolume, pUploadPor->nBoardIndex);
						//printf("%s\n", m_pQuery2);
						if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
							throw UPLOAD_RESULT_ERROR_DB;
					}
					else
					{
						//20080212 ���� �Ҵ��� �ȵ������� �Ҵ� �Ѵ�, jyh
						nTempVolume = atoi(pRow[0]);

						//printf("Ʈ�����ε�� �������� ����: %d\n", nTempVolume);

						//20080522 Ʈ�����ε� ������ �������� ������ flag_state = 'N' OR flag_use = 'N' �̸� �ٸ������� ���Ҵ� �޴´�, jyh
						sprintf(m_pQuery2, "Select flag_state, flag_use From %s Where ftp = %d", TABLE_COMMON_FTP_SERVER, nTempVolume);
						//printf("%s\n", m_pQuery2);
						if((pResult_1 = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow_1 = mysql_fetch_row(pResult_1)) == 0)
						{
							SAFE_DELETEDB(pResult_1);	
							throw UPLOAD_RESULT_ERROR_DB;
						}
						else
						{
							char szFlagState[2];
							char szFlagUse[2];

							lstrcpy(szFlagState, pRow_1[0]);
							lstrcpy(szFlagUse, pRow_1[1]);

							if(!lstrcmp(szFlagState, "N") || !lstrcmp(szFlagUse, "N"))
							{
								//printf("flag_state: %s, flag_use: %s\n", szFlagState, szFlagUse);
								//�ؿ��� �������� ������ ������ ����� �ִ��� �˻��ϰ� ������ ����� ������ ���� ���ε�� �����ϴ�
								//��ƾ�� �ֱ� ������ �ϴ� ���ο� ���� �Ҵ�
								if((nTempVolume = (int)LoadFtpAllotment()) == -1)
									throw UPLOAD_RESULT_ERROR_ALLO;

								//printf("���� ���Ҵ�: %d\n", nTempVolume);
							}
							
							SAFE_DELETEDB(pResult_1);
						}

						SAFE_DELETEDB(pResult);
						
						sprintf(m_pQuery2, "Update %s Set temp_volume = %d Where bbs_no = %I64d", pTableFile, nTempVolume, pUploadPor->nBoardIndex);
						//printf("%s\n", m_pQuery2);
						if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
							throw UPLOAD_RESULT_ERROR_DB;
					}	
				}
				
				//�������� ������ ������ ����� �ִ��� �˻��ϰ�
				//������ ����� ������ ���� ���ε�� ����~~, jyh
				if(LoadNewFileCheck(pTempName, nSize, (__int64)nTempVolume, 1) < 0)
				{
					//printf("Ʈ������ ���� ���ε�� ����\n");
					//���� ���ε�
					strcpy(pFlag_RealUpload, "Y");				

					// ���ϸ� ����
					//if(OPTION_MIN_FILEUNITY < nSize)
					sprintf(pSaveName, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
					//else
					//	strcpy(pSaveName, pTempName);
								
					/*sprintf(m_pQuery1, "Update %s Set temp_volume = %d, temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
						pTableFile, nTempVolume, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);*/
					sprintf(m_pQuery1, "Update %s Set temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
						pTableFile, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
					//printf("%s\n", m_pQuery1);
					if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
						throw UPLOAD_RESULT_ERROR_DB;

					bReal = true;
				}
				
				if(!bReal)
				{
					// ���ϸ� ����
					strcpy(pSaveName, pTempName);

					sprintf(m_pQuery1, "Update %s Set temp_name = '%s', flag_realupload = 'N', flag_unity='%s' Where no = %I64d", 
						pTableFile, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
					//printf("%s\n", m_pQuery1);
					if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
						throw UPLOAD_RESULT_ERROR_DB;
				}				
			}		
		}

		// ���� ���ε��� ��� �ش� ������ ���� �� ������ ä�� ����
		if(!strcmp(pFlag_RealUpload, "Y"))
		{
			//printf("���� ���ε�!\n");//test
			pUploadPor->nResult	= UPLOAD_RESULT_NEW;

			/*sprintf(m_pQuery1,  "Select temp_volume From %s Where no = %I64d",  pTableFile, pUploadPor->nFileIdx);
			if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			{
				SAFE_DELETEDB(pResult);	
				throw UPLOAD_RESULT_ERROR_ALLO;
			}
			
			ACE_UINT64 nVolumeIdx = _atoi64(pRow[0]);	
			SAFE_DELETEDB(pResult);	*/

			if(!LoadFtpCheck((__int64)nTempVolume)) throw UPLOAD_RESULT_SERVERBUSY;
			//printf("pUploadPor->nUploadSpeed:%d\n", pUploadPor->nUploadSpeed);//test
            if(!LoadFtpInfoUpload((__int64)nTempVolume, pUploadPor->nUploadSpeed, pServerIP, nServerPT, pServerID, pServerPW))
				throw UPLOAD_RESULT_ERROR_STOP;

			

			// FTP������ ������ �������� üũ
			for(int nProc = 0; nProc<10; nProc++)
			{
				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW)) 
				{
					//printf("ftp ���� ����!\n");//test
					if(pClient->m_pFtp->QueryTransfer(pSaveName, pUploadPor->nFileSize_Start, 0))
					{
						//printf("���ε� ��û ����!\n");//test
						strcpy(pUploadPor->pServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
						break;
					}
					else
					{
						//printf("���ε� ��û ����!\n");//test
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPro_BBS()��ftp ���� ���� : ftp_volume: %d, ftp_IP: %s\n"), nTempVolume, pServerIP));
						throw UPLOAD_RESULT_ERROR_DELAY;
					}
				}
				else
				{
					if(nProc >= 2)
					{
						//printf("ftp ���� ����!\n");//test
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPro_BBS()��ftp ���� ���� : ftp_volume: %d, ftp_IP: %s\n"), nTempVolume, pServerIP));
						throw UPLOAD_RESULT_FTP_CONFAILED;
					}
				}
			}			
		}
		else
		{
			pUploadPor->nResult	= UPLOAD_RESULT_TRICK;
		}		
		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));		
	}
	catch(int nCode)
	{		
		pUploadPor->nResult	= nCode;
		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPro_BBS()��Catch(...)\n"))); 		
		pUploadPor->nResult	= UPLOAD_RESULT_ERROR;
		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));
	}
}

// ���ε� �������
void DBCtrl::UpLoadState_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	PUPLOADEND pTransEndInfo;
	char pTableFile[30];

	try
	{
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
		
		if(pClient->m_nFileNo == pTransEndInfo->nFileIdx && pClient->m_nFileSizeBegin < pTransEndInfo->nFileSizeEnd)
		{	
			sprintf(m_pQuery1, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, pTransEndInfo->nFileSizeEnd, pTransEndInfo->nFileIdx);
			ExecuteQuery(m_pQuery1);
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadState()��Catch(...)\n"))); 
	}
}

// ���ε� �Ϸ�/�ߴ� ��û
void DBCtrl::UpLoadEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	PUPLOADEND pTransEndInfo;

	ACE_UINT64	nSize;
	ACE_UINT64	nSizeReal;
	ACE_UINT64	nSizeReal_Old;
	ACE_UINT64	nVolumeIdx;
	char		pFlag_RealUpload[2];
	char		pFlag_Unity[2];
	char		pSaveName[256];
	char		pFlag_CheckSum[40];	

	char		pTableBoard[30];
	char		pTableFile[30];
	char		pTableFilePath[30];

	char		pServerIP[80];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	bool		bProcComplete;

	//ent24�߰� - S
	char Category_code[5];
	char category_Cashing_code[5];
	char category_Cashing_Tablename[30];
	int return_value;
	int Copyright_cash;
	//ent24�߰� - E


	try
	{
		pClient->m_nFileNo			= 0;
		pClient->m_nFileSizeBegin	= 0;

		pTransEndInfo = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		// ����� FTP ���� ����
		pClient->m_pFtp->DisConnect();	

		/* ���ε� ��û���� ������ �������� üũ : Ÿ�Ժ��� �ٸ��� ���� */
		switch(pTransEndInfo->nFileType)
		{
			case 0:
				strcpy(pTableBoard, TABLE_BBS);
				strcpy(pTableFile, TABLE_FILE_BBS);
				strcpy(pTableFilePath, TABLE_FILE_BBS_PATH);
				break;
			case 1:
			case 2:
				strcpy(pTableBoard, TABLE_BLOG);
				strcpy(pTableFile, TABLE_FILE_BLOG);
				strcpy(pTableFilePath, TABLE_FILE_BLOG_PATH);
				break;
			default:
				throw pTransEndInfo->nFileIdx;
		}

		// ��� ����� ���� ���� ��ȸ
		sprintf(m_pQuery1,  "Select size, flag_realupload, temp_name, temp_volume, md5sum, realsize, flag_unity From %s Where no = %I64d",  
							pTableFile, pTransEndInfo->nFileIdx);
		//printf("%s\n", m_pQuery1);//test
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			//printf("���� ��� ����\n");//test
			SAFE_DELETEDB(pResult);	
			throw pTransEndInfo->nFileIdx;
		}
		
		nSize		= _atoi64(pRow[0]);
		strcpy(pFlag_RealUpload, pRow[1]);
		strcpy(pSaveName, pRow[2]);
		nVolumeIdx = _atoi64(pRow[3]);
		strcpy(pFlag_CheckSum, pRow[4]);
		nSizeReal_Old = _atoi64(pRow[5]);
		strcpy(pFlag_Unity, pRow[6]);
		SAFE_DELETEDB(pResult);

		/*
			���� ���ε��� ��� FTP������ ���� ���ε� ������ üũ
			- Ʈ�� ���ε��� ��� ���
			- ���н� Ŭ���̾�Ʈ���� �뺸���� ������� ����
		*/	
		if(!strcmp(pFlag_RealUpload, "Y"))
		{
			if(!LoadFtpInfoUpload(nVolumeIdx, 0, pServerIP, nServerPT, pServerID, pServerPW))
				throw pTransEndInfo->nFileIdx;

			// �⺻�� Ŭ���̾�Ʈ���� ���۹��� �뷮
			nSizeReal = pTransEndInfo->nFileSizeEnd;
	
			for(int nProc = 0; nProc < 20; nProc++)
			{
				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
				{
					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pSaveName);
					
					if(nServerFileSize == -2)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Not Found\n")));					
						pClient->m_pFtp->DisConnect();		
						break;
					}
					else if(nServerFileSize == -1)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Check Error : #nProc(%d)\n"), nProc));		
						pClient->m_pFtp->DisConnect();				
						continue;
					}
					else if(nServerFileSize >= 0 && nServerFileSize != pTransEndInfo->nFileSizeEnd)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Size Difference : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
						pClient->m_pFtp->DisConnect();		
						nSizeReal = nServerFileSize;
						ACE_OS::sleep(1);						
						continue;
					}										
					else if(nServerFileSize == pTransEndInfo->nFileSizeEnd)
					{
						if(nProc != 0)
							ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Size Complete : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
						nSizeReal = nServerFileSize;
						pClient->m_pFtp->DisConnect();		
						break;
					}
					pClient->m_pFtp->DisConnect();
				}

				Sleep(500);	//20081027 �߰�, jyh
			}
		}
		else
		{
			nSizeReal = pTransEndInfo->nFileSizeEnd;			
		}
		

		/*
			1. ���ε� �Ϸ��
				- ���ð�� ���ڵ� ����				
				- ���� ���ϸ����� ����
				- flag_upload 'Y'�� ����
				- �������ϻ���� ���� ������� ������Ʈ
				
			2. ���� ���ε� �Ϸ�� �߰� �۾�
				- FTP������ ���ϸ� ���� ���ϸ����� ����
				- ���� �ö�� ������ ����� ��� �Ϸ�� ���� ����
		*/		
		if(nSizeReal > nSize) nSizeReal = nSizeReal_Old;
		if(nSizeReal == nSize)
		{
			bool bChangeFile = false;
			char pNew_savename[256];
			//ACE_UINT64	nNew_VolumeIdx;
			__int64 nCheckResult = -1;
			
			//20080123 ���뷮���ϵ� Ʈ�����ε� �ϱ� ����, jyh
			sprintf(pNew_savename, "%s_%I64d", pFlag_CheckSum, nSize);
			nCheckResult = LoadNewFileCheck(pNew_savename, nSize, nVolumeIdx);
			//printf("�ߺ� ���� ����: %s(����:%I64d)\n", pNew_savename, nCheckResult);

			//int nSameFileCountCore = LoadFileCountCore(pNew_savename);
			//int nSameFileCountBBS = LoadFileCountCp(pFlag_CheckSum, nSize);
			//if(nSameFileCountCore != 0 && nSameFileCountBBS != 0 && RunServerInfo.nLimit_DuplicateFile > 1 && nSameFileCountBBS <= (nSameFileCountCore * RunServerInfo.nLimit_DuplicateFile))
			//	nCheckResult = LoadNewFileCheck(pNew_savename, nSize);
			/*if(OPTION_MIN_FILEUNITY < nSize) 
			{
				sprintf(pNew_savename, "%s_%I64d", pFlag_CheckSum, nSize);

				int nSameFileCountCore = LoadFileCountCore(pNew_savename);
				int nSameFileCountBBS = LoadFileCountCp(pFlag_CheckSum, nSize);
				if(nSameFileCountCore != 0 && nSameFileCountBBS != 0 && RunServerInfo.nLimit_DuplicateFile > 1 && nSameFileCountBBS <= (nSameFileCountCore * RunServerInfo.nLimit_DuplicateFile))
					nCheckResult = LoadNewFileCheck(pNew_savename, nSize);
			}
			else
			{
				strcpy(pNew_savename, pSaveName);
				nCheckResult = LoadNewFileCheck(pNew_savename, nSize);
			}*/
				
			if(nCheckResult != -1)
			{
				bChangeFile = true;
				//nNew_VolumeIdx = nCheckResult;
			}
			else
			{
				bChangeFile = false;
				//nNew_VolumeIdx = nVolumeIdx;				
			}

			// �������ε��� ��� ���ϸ� ���� �Ǵ� �ӽ� ���� ����
			if(!strcmp(pFlag_RealUpload, "Y"))
			{
				// ���� ���� ����
				if(bChangeFile)
				{
					// FTP������ ������ �������� üũ
					if(!pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
					{
						//20081027 ftp ���� ����, jyh
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��ftp ���� ���� : ftp_volume : %I64d, ftp_IP : %s\n"), nVolumeIdx, pServerIP));
						throw UPLOAD_RESULT_FTP_CONFAILED;
					}

					pClient->m_pFtp->RemoveFile(pSaveName);
					pClient->m_pFtp->DisConnect();
					//printf("RemoveFile: %s\n", pSaveName);
				}
				// ���ϸ� ����
				else
				{	
					bProcComplete = false;
					for(int nProc = 0; nProc < 10; nProc++)
					{
						//printf("check: %d [%s | %d %s %s]\n", nProc,pServerIP, nServerPT, pServerID, pServerPW);

						// FTP������ ������ �������� üũ
						if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
						{
							//printf("a : RenameFile: %s, PrevRenameFile: %s\n", pSaveName, pNew_savename);


							if(pClient->m_pFtp->RenameFile(pSaveName, pNew_savename))
							{
								//printf("RenameFile: %s, PrevRenameFile: %s\n", pSaveName, pNew_savename);

								pClient->m_pFtp->DisConnect();
								bProcComplete = true;
								break;
							}
							pClient->m_pFtp->DisConnect();
						}
					
						Sleep(500);	//20081027 �߰�, jyh
					}

					if(!bProcComplete)
						throw UPLOAD_RESULT_ERROR;

					////20080123 ������ ������ ���ϵ���� �Ϸ� �ƴٸ� _webhard_file�� ���, jyh
					//sprintf(m_pQuery1,  "Select no From %s Where bbs_no = %I64d and contents_complete = 1",  
					//						pTableFile, pTransEndInfo->nBoardIndex);
					//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
					//{
					//	SAFE_DELETEDB(pResult);	
					//	throw pTransEndInfo->nFileIdx;
					//}
					//else
     //				{
					//	// ���ο� ���� ���ε� ���
					//	SetNewFileComplete(pNew_savename, nSize, nNew_VolumeIdx);
					//	SAFE_DELETEDB(pResult);
					//}
				}

				sprintf(m_pQuery1, "Update %s Set temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d ",
					pTableFile, pNew_savename, nSize, pTransEndInfo->nFileIdx);
				ExecuteQuery(m_pQuery1);
				
				/*if(OPTION_MIN_FILEUNITY < nSize) 
				{
					sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s' Where flag_upload = 'Y' And md5sum = '%s' And size = %I64d",
										TABLE_FILE_BBS, nVolumeIdx, pNew_savename, pFlag_CheckSum, nSize);
					ExecuteQuery(m_pQuery1);

					sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s' Where flag_upload = 'Y' And md5sum = '%s' And size = %I64d",
										TABLE_FILE_BLOG, nVolumeIdx, pNew_savename, pFlag_CheckSum, nSize);
					ExecuteQuery(m_pQuery1);
				}*/
			}
			else
			{
				// - flag_upload 'Y'�� ������Ʈ
				// - �������ϻ���� ���� ������� ������Ʈ
				// - ���� ���ϸ����� ���ϸ� ������Ʈ
				// - FTP ���� ����Ǿ����� ������Ʈ
				/*sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d", 
									pTableFile, nNew_VolumeIdx, pNew_savename, nSize, pTransEndInfo->nFileIdx);*/
				sprintf(m_pQuery1, "Update %s Set temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d", 
					pTableFile, pNew_savename, nSize, pTransEndInfo->nFileIdx);
				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
					throw pTransEndInfo->nFileIdx;
			}

			////20080123 ������ ������ ���ε尡 �Ϸ�Ǹ� contents_complete�� 1�� ������Ʈ, jyh
			//if(pTransEndInfo->nContentsCmpl == 1)
			//{
			//	sprintf(m_pQuery1, "Update %s Set contents_complete = 1 Where bbs_no = %I64d", pTableFile, pTransEndInfo->nBoardIndex);
			//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
			//		throw pTransEndInfo->nBoardIndex;

			//	//�������� ����� �˾ƿ´�.
			//	sprintf(m_pQuery1, "Select size From %s Where idx = %I64d", TABLE_BBS, pTransEndInfo->nBoardIndex);
			//	if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			//	{
			//		SAFE_DELETEDB(pResult);	
			//		throw pTransEndInfo->nBoardIndex;
			//	}

			//	ACE_UINT64 nContentsSize = _atoi64(pRow[0]);
			//	SAFE_DELETEDB(pResult);

			//	//�������� ù��° md5sum�� �˾ƿ´�.
			//	sprintf(m_pQuery1,  "Select md5sum From %s Where bbs_no = %I64d order by (idx) asc", pTableFile, pTransEndInfo->nBoardIndex);
			//	if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			//	{
			//		SAFE_DELETEDB(pResult);	
			//		throw pTransEndInfo->nBoardIndex;
			//	}

			//	char szHash_Size[MAX_PATH];

			//	sprintf(szHash_Size, "%s_%I64d", pRow[0], nContentsSize);
			//	SAFE_DELETEDB(pResult);
			//	//printf("szHash_Size: %s", szHash_Size);

			//	//20080214 _webhard_contents_unique �� �� �ۼ�, jyh
			//	sprintf(m_pQuery1, "Insert into %s set bbs_no = %I64d, ftp_volume = %I64d, hash_size = '%s'", 
			//			TABLE_CONTENTS_UNIQUE, pTransEndInfo->nBoardIndex, nVolumeIdx, szHash_Size);
			//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
			//		throw pTransEndInfo->nBoardIndex;
			//}

			/* ���ε� �Ϸ�� ���ð�� ���ڵ� ���� */
			sprintf(m_pQuery1, "Delete From %s Where no = %I64d", pTableFilePath, pTransEndInfo->nFileIdx);
			ExecuteQuery(m_pQuery1);

			/* ���� ������ �Ҽӵ� ��� ������ ��� �Ϸ��� ��� : �ش� �Խù��� ��� �Ϸ�� ó�� */
			sprintf(m_pQuery1, "Select * From %s Where bbs_no = %I64d and flag_upload='N'", pTableFile, pTransEndInfo->nBoardIndex);
			for(int nProc = 0; nProc < 10; nProc++)
			{
				if(ExecuteQuery(m_pQuery1, nCount) == QUERY_COMPLETE)
				{
					if(nCount == 0)
					{
						for(int nProcSub = 0; nProcSub < 10; nProcSub++)
						{
							//20080123 ������ ������ ���ε尡 �Ϸ�Ǹ� contents_complete�� 1�� ������Ʈ, jyh------------------------------------------
							sprintf(m_pQuery2, "Update %s Set contents_complete = 1 Where bbs_no = %I64d", pTableFile, pTransEndInfo->nBoardIndex);
							if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
								throw pTransEndInfo->nBoardIndex;

							//�������� ����� �˾ƿ´�.
							//20081006 ���� ����, jyh
							//sprintf(m_pQuery2, "Select size From %s Where idx = %I64d", TABLE_BBS, pTransEndInfo->nBoardIndex);
                            sprintf(m_pQuery2, "Select size, userid, code_cate, link_idx From %s Where idx = %I64d", TABLE_BBS, pTransEndInfo->nBoardIndex);
							if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
							{
								SAFE_DELETEDB(pResult);	
								throw pTransEndInfo->nBoardIndex;
							}

							ACE_UINT64 nContentsSize = _atoi64(pRow[0]);
							

							//ent24ī�װ� ĳ���� ���� ī�װ������� - S
							strcpy(Category_code,pRow[2]);
							//ent24ī�װ� ĳ���� ���� ī�װ������� - E


							//20081006 ���� �߰�, jyh-------------------------------------------------------------------
							if(!lstrcmp(pRow[2], "ADT"))
							{
								sprintf(m_pQuery3, "Update _seller Set contents_up = contents_up + 1, adult_up = adult_up + 1 Where userid = '%s'", pRow[1]);
							}
							else
							{
								sprintf(m_pQuery3, "Update _seller Set contents_up = contents_up + 1 Where userid = '%s'", pRow[1]);
							}
							
							//printf("%s\n", m_pQuery3);//test
							if(ExecuteQuery(m_pQuery3) != QUERY_COMPLETE)
								throw pTransEndInfo->nBoardIndex;

							//20081028 ���� �߰�, jyh
							if(pRow[3] > 0)
								sprintf(m_pQuery3, "Update %s Set state = 2 Where idx = %s", TABLE_BBS_REQ, pRow[3]);

							//printf("%s\n", m_pQuery3);//test
							if(ExecuteQuery(m_pQuery3) != QUERY_COMPLETE)
								throw pTransEndInfo->nBoardIndex;

							SAFE_DELETEDB(pResult);
							//----------------------------------------------------------------------------------------

							//�������� ù��° md5sum�� �˾ƿ´�.
							sprintf(m_pQuery2,  "Select md5sum From %s Where bbs_no = %I64d order by (idx) asc", pTableFile, pTransEndInfo->nBoardIndex);
							//printf("%s\n", m_pQuery2);//test
							if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
							{
								//printf("���� ����!, ������ ��ȣ:%I64d\n", pTransEndInfo->nBoardIndex);//test
								SAFE_DELETEDB(pResult);	
								throw pTransEndInfo->nBoardIndex;
							}

							char szHash_Size[MAX_PATH];

							sprintf(szHash_Size, "%s_%I64d", pRow[0], nContentsSize);
							SAFE_DELETEDB(pResult);
							//printf("szHash_Size: %s\n", szHash_Size);//test

							//20080214 _webhard_contents_unique �� �� �ۼ�, jyh
							sprintf(m_pQuery2, "Insert into %s set bbs_no = %I64d, ftp_volume = %I64d, hash_size = '%s'", 
								TABLE_CONTENTS_UNIQUE, pTransEndInfo->nBoardIndex, nVolumeIdx, szHash_Size);
							//printf("%s\n", m_pQuery2);//test
							if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
							{
								//printf("���� ����! ������ ��ȣ: %I64d\n", pTransEndInfo->nBoardIndex);//test
								throw pTransEndInfo->nBoardIndex;
							}
							//---------------------------------------------------------------------------------------------------------------------------


//�·�ī ������ ���� �ݾ� �ε� - S
							Copyright_cash=0;
							//�ݾ� �ε�
							sprintf(m_pQuery2,  "SELECT sum(u_money) as total_money FROM %s WHERE u_board_idx='%I64d' group by u_board_idx",	TABLE_MUREKA_COPYRIGHT_HASH,
																																				pTransEndInfo->nBoardIndex);
							if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
								Copyright_cash=0;
							else
								Copyright_cash=atoi(pRow[0]);
//�·�ī ������ ���� �ݾ� �ε� - E



							//ent24�߰� - ���ε�Ϸ�� �ֻ����� �ø� - S-------------------------------------------------------------------------------------------
							//�������� ù��° md5sum�� �˾ƿ´�.
							sprintf(m_pQuery2,  "Select IfNull(Max(sort) + 1, 1) As sort_num From %s", pTableBoard);
							//printf("%s\n", m_pQuery2);//test
							if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
							{
								//printf("���� ����!, ������ ��ȣ:%I64d\n", pTransEndInfo->nBoardIndex);//test
								SAFE_DELETEDB(pResult);	
								throw pTransEndInfo->nBoardIndex;
							}
							//ent24�߰� - ���ε�Ϸ�� �ֻ����� �ø� - E-------------------------------------------------------------------------------------------



							//20081008 ���ε� �Ϸ�� sanc = 0 �߰�, jyh
							//���� -  ent24�Ʒ����� ����
							//sprintf(m_pQuery2, "Update %s Set state=1 Where state=0 and idx = %I64d", pTableBoard, pTransEndInfo->nBoardIndex);


							if(Copyright_cash)
								sprintf(m_pQuery2, "Update %s Set state = 1, sanc = 0, sort=%s,cash='%d' Where state=0 and idx = %I64d", pTableBoard,pRow[0], Copyright_cash,pTransEndInfo->nBoardIndex);
							else
								sprintf(m_pQuery2, "Update %s Set state = 1, sanc = 0, sort=%s Where state=0 and idx = %I64d", pTableBoard,pRow[0], pTransEndInfo->nBoardIndex);
							
							/*
							if(ExecuteQuery(m_pQuery2) == QUERY_COMPLETE)
								break;
							*/
							return_value=ExecuteQuery(m_pQuery2);

							



							if(return_value == QUERY_COMPLETE)
								break;



							Sleep(500);		//20081027 �߰�, jyh
						}

						for(int nProcSub = 0; nProcSub < 10; nProcSub++)
						{
							sprintf(m_pQuery2, "Insert Into %s Set board_idx = %I64d, type = '%d', regdate=UNIX_TIMESTAMP()", 
												TABLE_FILE_UPLOAD_COMPLETE, pTransEndInfo->nBoardIndex, pTransEndInfo->nFileType);
							if(ExecuteQuery(m_pQuery2) == QUERY_COMPLETE)
								break;

							Sleep(500);		//20081027 �߰�, jyh
						}

						//20090303 ������ ���ε� �Ϸ�� ���ε� ��Ʈ�ѿ� ���ε� �Ϸ� �˷���, jyh
						pClient->send_packet_queue(LOAD_UPCONTENTSCOMPLETE, (char*)pTransEndInfo, sizeof(UPLOADEND));
					}
					break;
				}
			}			
		}
		/* ���� ���ε尡 �Ϸ���� ���� ��� : ������� ���ε�� ������ ������Ʈ */
		else
		{
			sprintf(m_pQuery1, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, nSizeReal, pTransEndInfo->nFileIdx);
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw pTransEndInfo->nFileIdx;
		}
	}
	catch(ACE_UINT64 nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd()��Catch(nCode) File Index : %Q : %s\n"), nCode, pClient->getuserinfo())); 
	}
	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd()��Catch(nCode) Error Code : %d\n"), nCode, pClient->getuserinfo())); 
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd()��Catch(...)\n"))); 
	}
}
//void DBCtrl::UpLoadEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//
//	PUPLOADEND pTransEndInfo;
//
//	ACE_UINT64	nSize;
//	ACE_UINT64	nSizeReal;
//	ACE_UINT64	nSizeReal_Old;
//	ACE_UINT64	nVolumeIdx;
//	char		pFlag_RealUpload[2];
//	char		pFlag_Unity[2];
//	char		pSaveName[256];
//	char		pFlag_CheckSum[40];	
//
//	char		pTableBoard[30];
//	char		pTableFile[30];
//	char		pTableFilePath[30];
//
//	char		pServerIP[20];
//	char		pServerID[20];
//	char		pServerPW[20];
//	int			nServerPT;
//
//	bool		bProcComplete;
//
//	try
//	{
//		pClient->m_nFileNo			= 0;
//		pClient->m_nFileSizeBegin	= 0;
//
//		pTransEndInfo = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
//
//		// ����� FTP ���� ����
//		pClient->m_pFtp->DisConnect();	
//
//		/* ���ε� ��û���� ������ �������� üũ : Ÿ�Ժ��� �ٸ��� ���� */
//		switch(pTransEndInfo->nFileType)
//		{
//		case 0:
//			strcpy(pTableBoard, TABLE_BBS);
//			strcpy(pTableFile, TABLE_FILE_BBS);
//			strcpy(pTableFilePath, TABLE_FILE_BBS_PATH);
//			break;
//		case 1:
//		case 2:
//			strcpy(pTableBoard, TABLE_BLOG);
//			strcpy(pTableFile, TABLE_FILE_BLOG);
//			strcpy(pTableFilePath, TABLE_FILE_BLOG_PATH);
//			break;
//		default:
//			throw pTransEndInfo->nFileIdx;
//		}
//
//		// ��� ����� ���� ���� ��ȸ
//		sprintf(m_pQuery1,  "Select size, flag_realupload, temp_name, temp_volume, md5sum, realsize, flag_unity From %s Where no = %I64d",  
//			pTableFile, pTransEndInfo->nFileIdx);
//		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//		{
//			SAFE_DELETEDB(pResult);	
//			throw pTransEndInfo->nFileIdx;
//		}
//
//		nSize		= _atoi64(pRow[0]);
//		strcpy(pFlag_RealUpload, pRow[1]);
//		strcpy(pSaveName, pRow[2]);
//		nVolumeIdx = _atoi64(pRow[3]);
//		strcpy(pFlag_CheckSum, pRow[4]);
//		nSizeReal_Old = _atoi64(pRow[5]);
//		strcpy(pFlag_Unity, pRow[6]);
//		SAFE_DELETEDB(pResult);
//
//		/*
//		���� ���ε��� ��� FTP������ ���� ���ε� ������ üũ
//		- Ʈ�� ���ε��� ��� ���
//		- ���н� Ŭ���̾�Ʈ���� �뺸���� ������� ����
//		*/	
//		if(!strcmp(pFlag_RealUpload, "Y"))
//		{
//			if(!LoadFtpInfoUpload(nVolumeIdx, 0, pServerIP, nServerPT, pServerID, pServerPW))
//				throw pTransEndInfo->nFileIdx;
//
//			// �⺻�� Ŭ���̾�Ʈ���� ���۹��� �뷮
//			nSizeReal = pTransEndInfo->nFileSizeEnd;
//
//			for(int nProc = 0; nProc < 20; nProc++)
//			{
//				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
//				{
//					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pSaveName);
//
//					if(nServerFileSize == -2)
//					{
//						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Not Found\n")));					
//						pClient->m_pFtp->DisConnect();		
//						break;
//					}
//					else if(nServerFileSize == -1)
//					{
//						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Check Error : #nProc(%d)\n"), nProc));		
//						pClient->m_pFtp->DisConnect();				
//						continue;
//					}
//					else if(nServerFileSize >= 0 && nServerFileSize != pTransEndInfo->nFileSizeEnd)
//					{
//						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Size Difference : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
//						pClient->m_pFtp->DisConnect();		
//						nSizeReal = nServerFileSize;
//						ACE_OS::sleep(1);						
//						continue;
//					}										
//					else if(nServerFileSize == pTransEndInfo->nFileSizeEnd)
//					{
//						if(nProc != 0)
//							ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd_BBS()��File Size Complete : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
//						nSizeReal = nServerFileSize;
//						pClient->m_pFtp->DisConnect();		
//						break;
//					}
//					pClient->m_pFtp->DisConnect();
//				}				
//			}
//		}
//		else
//		{
//			nSizeReal = pTransEndInfo->nFileSizeEnd;			
//		}
//
//
//		/*
//		1. ���ε� �Ϸ��
//		- ���ð�� ���ڵ� ����				
//		- ���� ���ϸ����� ����
//		- flag_upload 'Y'�� ����
//		- �������ϻ���� ���� ������� ������Ʈ
//
//		2. ���� ���ε� �Ϸ�� �߰� �۾�
//		- FTP������ ���ϸ� ���� ���ϸ����� ����
//		- ���� �ö�� ������ ����� ��� �Ϸ�� ���� ����
//		*/		
//		if(nSizeReal > nSize) nSizeReal = nSizeReal_Old;
//		if(nSizeReal == nSize)
//		{
//			bool bChangFile = false;
//			char pNew_savename[256];
//			ACE_UINT64	nNew_VolumeIdx;
//			__int64 nCheckResult = -1;
//
//			//20080123 ���뷮���ϵ� Ʈ�����ε� �ϱ� ����, jyh
//			sprintf(pNew_savename, "%s_%I64d", pFlag_CheckSum, nSize);
//
//			int nSameFileCountCore = LoadFileCountCore(pNew_savename);
//			int nSameFileCountBBS = LoadFileCountCp(pFlag_CheckSum, nSize);
//			if(nSameFileCountCore != 0 && nSameFileCountBBS != 0 && RunServerInfo.nLimit_DuplicateFile > 1 && nSameFileCountBBS <= (nSameFileCountCore * RunServerInfo.nLimit_DuplicateFile))
//				nCheckResult = LoadNewFileCheck(pNew_savename, nSize);
//			/*if(OPTION_MIN_FILEUNITY < nSize) 
//			{
//			sprintf(pNew_savename, "%s_%I64d", pFlag_CheckSum, nSize);
//
//			int nSameFileCountCore = LoadFileCountCore(pNew_savename);
//			int nSameFileCountBBS = LoadFileCountCp(pFlag_CheckSum, nSize);
//			if(nSameFileCountCore != 0 && nSameFileCountBBS != 0 && RunServerInfo.nLimit_DuplicateFile > 1 && nSameFileCountBBS <= (nSameFileCountCore * RunServerInfo.nLimit_DuplicateFile))
//			nCheckResult = LoadNewFileCheck(pNew_savename, nSize);
//			}
//			else
//			{
//			strcpy(pNew_savename, pSaveName);
//			nCheckResult = LoadNewFileCheck(pNew_savename, nSize);
//			}*/
//
//			if(nCheckResult != -1)
//			{
//				bChangFile = true;
//				nNew_VolumeIdx = nCheckResult;
//			}
//			else
//			{
//				bChangFile = false;
//				nNew_VolumeIdx = nVolumeIdx;				
//			}
//
//
//			// �������ε��� ��� ���ϸ� ���� �Ǵ� �ӽ� ���� ����
//			if(!strcmp(pFlag_RealUpload, "Y"))
//			{
//				// ���� ���� ����
//				if(bChangFile)
//				{
//					// FTP������ ������ �������� üũ
//					if(!pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
//						throw UPLOAD_RESULT_ERROR;
//
//					pClient->m_pFtp->RemoveFile(pSaveName);
//					pClient->m_pFtp->DisConnect();
//				}
//				// ���ϸ� ����
//				else
//				{	
//					bProcComplete = false;
//					for(int nProc = 0; nProc < 10; nProc++)
//					{
//						// FTP������ ������ �������� üũ
//						if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
//						{
//							if(pClient->m_pFtp->RenameFile(pSaveName, pNew_savename))
//							{
//								pClient->m_pFtp->DisConnect();
//								bProcComplete = true;
//								break;
//							}
//							pClient->m_pFtp->DisConnect();
//						}
//					}
//
//					if(!bProcComplete)
//						throw UPLOAD_RESULT_ERROR;
//
//					////20080123 ������ ������ ���ϵ���� �Ϸ� �ƴٸ� _webhard_file�� ���, jyh
//					//sprintf(m_pQuery1,  "Select no From %s Where bbs_no = %I64d and contents_complete = 1",  
//					//						pTableFile, pTransEndInfo->nBoardIndex);
//					//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//					//{
//					//	SAFE_DELETEDB(pResult);	
//					//	throw pTransEndInfo->nFileIdx;
//					//}
//					//else
//					//				{
//					//	// ���ο� ���� ���ε� ���
//					//	SetNewFileComplete(pNew_savename, nSize, nNew_VolumeIdx);
//					//	SAFE_DELETEDB(pResult);
//					//}
//				}
//
//				//if(OPTION_MIN_FILEUNITY < nSize) 
//				//{
//				sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s' Where flag_upload = 'Y' And md5sum = '%s' And size = %I64d",
//					TABLE_FILE_BBS, nNew_VolumeIdx, pNew_savename, pFlag_CheckSum, nSize);
//				ExecuteQuery(m_pQuery1);
//
//				sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s' Where flag_upload = 'Y' And md5sum = '%s' And size = %I64d",
//					TABLE_FILE_BLOG, nNew_VolumeIdx, pNew_savename, pFlag_CheckSum, nSize);
//				ExecuteQuery(m_pQuery1);
//				//}
//			}
//
//
//			// - flag_upload 'Y'�� ������Ʈ
//			// - �������ϻ���� ���� ������� ������Ʈ
//			// - ���� ���ϸ����� ���ϸ� ������Ʈ
//			// - FTP ���� ����Ǿ����� ������Ʈ
//			sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d", 
//				pTableFile, nNew_VolumeIdx, pNew_savename, nSize, pTransEndInfo->nFileIdx);
//			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//				throw pTransEndInfo->nFileIdx;
//
//			//20080123 ������ ������ ���ε尡 �Ϸ�Ǹ� contents_complete�� 1�� ������Ʈ
//			if(pTransEndInfo->nContentsCmpl == 1)
//			{
//				sprintf(m_pQuery1, "Update %s Set contents_complete = 1 Where bbs_no = %I64d", pTableFile, pTransEndInfo->nBoardIndex);
//				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//					throw pTransEndInfo->nBoardIndex;	
//			}
//
//			/* ���ε� �Ϸ�� ���ð�� ���ڵ� ���� */
//			sprintf(m_pQuery1, "Delete From %s Where no = %I64d", pTableFilePath, pTransEndInfo->nFileIdx);
//			ExecuteQuery(m_pQuery1);
//
//			/* ���� ������ �Ҽӵ� ��� ������ ��� �Ϸ��� ��� : �ش� �Խù��� ��� �Ϸ�� ó�� */
//			sprintf(m_pQuery1, "Select * From %s Where bbs_no = %I64d and flag_upload='N'", pTableFile, pTransEndInfo->nBoardIndex);
//			for(int nProc = 0; nProc < 10; nProc++)
//			{
//				if(ExecuteQuery(m_pQuery1, nCount) == QUERY_COMPLETE)
//				{
//					if(nCount == 0)
//					{
//						for(int nProcSub = 0; nProcSub < 10; nProcSub++)
//						{
//							sprintf(m_pQuery2, "Update %s Set state=1 Where idx = %I64d", pTableBoard, pTransEndInfo->nBoardIndex);
//							if(ExecuteQuery(m_pQuery2) == QUERY_COMPLETE)
//								break;
//						}
//
//						for(int nProcSub = 0; nProcSub < 10; nProcSub++)
//						{
//							sprintf(m_pQuery2, "Insert Into %s Set board_idx=%I64d, type='%d', regdate=UNIX_TIMESTAMP()", 
//								TABLE_FILE_UPLOAD_COMPLETE, pTransEndInfo->nBoardIndex, pTransEndInfo->nFileType);
//							if(ExecuteQuery(m_pQuery2) == QUERY_COMPLETE)
//								break;
//						}
//					}
//					break;
//				}
//			}			
//		}
//		/* ���� ���ε尡 �Ϸ���� ���� ��� : ������� ���ε�� ������ ������Ʈ */
//		else
//		{
//			sprintf(m_pQuery1, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, nSizeReal, pTransEndInfo->nFileIdx);
//			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//				throw pTransEndInfo->nFileIdx;
//		}
//	}
//	catch(ACE_UINT64 nCode)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd()��Catch(nCode) File Index : %Q : %s\n"), nCode, pClient->getuserinfo())); 
//	}
//	catch(int nCode)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd()��Catch(nCode) Error Code : %d\n"), nCode, pClient->getuserinfo())); 
//	}
//	catch(...)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadEnd()��Catch(...)\n"))); 
//	}
//}






// ���ε� ������ �߰�
void DBCtrl::UpLoadItem_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_RES	*pResult2 = 0;
	MYSQL_RES	*pResult3 = 0;
	MYSQL_ROW	pRow;
	MYSQL_ROW	pRow2;
	MYSQL_ROW	pRow3;
	
	char		pTableFile[30];
	char		pTableFilePath[30];

	//ent24�߰� - S
	char Category_code[5];
	char category_Cashing_code[5];
	char category_Cashing_Tablename[30];
	int return_value;
	//ent24�߰� - E


	std::string m_strResult;

	try
	{
		CToken*	pToken = new CToken(SEP_FIELD);	

		char* pData = new char[nPS];
		memcpy(pData, (char*)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE), nPS);
		pToken->Split(pData);
		delete pData;
		
		//20080821 ���ε� Ÿ�� ���� �߰�, jyh
        //if(pToken->GetCount() == 3)
		//printf("�����ۼ�:%d\n", pToken->GetCount());//test
		if(pToken->GetCount() == 2)
		{			
			// ��û Ÿ�Կ� �´� ���̺� ����
			switch(atoi(pToken->GetToken(0)))
			{
				case 0:
					strcpy(pTableFile, TABLE_FILE_BBS);
					strcpy(pTableFilePath, TABLE_FILE_BBS_PATH);
					break;
				case 1:
				case 2:
					strcpy(pTableFile, TABLE_FILE_BLOG);
					strcpy(pTableFilePath, TABLE_FILE_BLOG_PATH);
					break;
				default:
					delete pToken;				
					return;
			}

			sprintf(m_pQuery1,  "Select  " \

								" t_file.no, t_file.size, t_file.realname, t_path.localname, " \


								"ifnull((" \
								"	SELECT " \
								"		tb_bbs.title" \
								"	FROM " \
								"		_webhard_bbs  AS tb_bbs" \
								"	WHERE " \
								"		tb_bbs.idx  = t_file.bbs_no " \
								"	LIMIT 0 , 1" \
								"),'') AS bbs_title," \


								"ifnull((" \
								"	SELECT " \
								"		(" \
								"			ifnull((" \
								"				SELECT " \
								"					tb_bbs_category.title_cate" \
								"				FROM " \
								"					_webhard_bbs_category  AS tb_bbs_category" \
								"				WHERE " \
								"					tb_bbs.code_cate  = tb_bbs_category.code_cate " \
								"				limit " \
								"					0,1" \
								"			),'')" \
								"		)" \
								"	FROM " \
								"		_webhard_bbs  AS tb_bbs" \
								"	WHERE " \
								"		tb_bbs.idx  = t_file.bbs_no " \
								"	LIMIT 0 , 1" \
								"),'') AS bbs_code_cate " \




								"From %s t_file, %s t_path Where t_file.bbs_no = %s And t_file.no = t_path.no " \
								"order by depth asc, foldername asc, realname asc",  
								pTableFile, pTableFilePath, pToken->GetToken(1));

			// ��� ����
			m_strResult.clear();			
			if((pResult = ExecuteQueryR(m_pQuery1)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			{
				//printf("���ε� �н��� ����!\n");//test	
				//20081111 ��Ʈ�� �������� ���� ���ε� �Ϸ� �����ӿ��� �ұ��ϰ� _webhard_bbs�� state�� 1�� �ٲ��� �ʰ�
				//_webhard_contents_unique�� ���� ������ �ʴ� ���� ó��, jyh
				//_webhard_file_bbs�� contents_complete = 1�� ����
				sprintf(m_pQuery2, "Select no From %s Where bbs_no = %s And contents_complete = 0", pTableFile, pToken->GetToken(1));
				if((pResult2 = ExecuteQueryR(m_pQuery2, nCount)) != 0 || (pRow2 = mysql_fetch_row(pResult2)) == 0)
				{
					sprintf(m_pQuery3, "Update %s Set contents_complete = 1 Where bbs_no = %s", pTableFile, pToken->GetToken(1));
					if(ExecuteQuery(m_pQuery3) != QUERY_COMPLETE)
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadItem()��Catch(...)\n")));
				}

				SAFE_DELETEDB(pResult2);

				//20090520 _webhard_contents_unique�� insert �ϱ��� ���� bbs_no�� �ִ��� �˻��Ѵ�, jyh
				sprintf(m_pQuery3, "Select idx From %s Where bbs_no = %s", TABLE_CONTENTS_UNIQUE, pToken->GetToken(1));
				if((pResult3 = ExecuteQueryR(m_pQuery3, nCount)) == 0 || (pRow3 = mysql_fetch_row(pResult3)) == 0)
				{
					//_webhard_contents_unique�� �� ������ ���� �������� ����� ���´�.
					sprintf(m_pQuery2, "Select size From %s Where idx = %s", TABLE_BBS, pToken->GetToken(1));
					if((pResult2 = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow2 = mysql_fetch_row(pResult2)) == 0)
					{
						SAFE_DELETEDB(pResult2);	
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadItem()��Catch(...)\n")));
					}

					ACE_UINT64 nContentsSize = _atoi64(pRow2[0]);
					SAFE_DELETEDB(pResult2);

					//�������� ù��° md5sum�� �˾ƿ´�.
					sprintf(m_pQuery2,  "Select md5sum, temp_volume From %s Where bbs_no = %s order by (idx) asc", pTableFile, pToken->GetToken(1));
					//printf("%s\n", m_pQuery2);//test
					if((pResult2 = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow2 = mysql_fetch_row(pResult2)) == 0)
					{
						SAFE_DELETEDB(pResult2);
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadItem()��Catch(...)\n")));
					}

					char szHash_Size[MAX_PATH];
					sprintf(szHash_Size, "%s_%I64d", pRow2[0], nContentsSize);
					ACE_UINT64	nVolumeIdx = _atoi64(pRow2[1]);
					SAFE_DELETEDB(pResult2);

					//20080214 _webhard_contents_unique�� �� �ۼ�, jyh
					sprintf(m_pQuery2, "Insert into %s set bbs_no = %s, ftp_volume = %I64d, hash_size = '%s'", 
						TABLE_CONTENTS_UNIQUE, pToken->GetToken(1), nVolumeIdx, szHash_Size);
					//printf("%s\n", m_pQuery2);//test
					if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadItem()��Catch(...)\n")));
					}

					SAFE_DELETEDB(pResult2);
				}

				SAFE_DELETEDB(pResult3);
				



				//ent24�߰� - ���ε�Ϸ�� �ֻ����� �ø� - S-------------------------------------------------------------------------------------------
				//�������� ù��° md5sum�� �˾ƿ´�.
				sprintf(m_pQuery2,  "Select IfNull(Max(sort) + 1, 1) As sort_num From %s", TABLE_BBS);
				//printf("%s\n", m_pQuery2);//test
				if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
				{
					//printf("���� ����!, ������ ��ȣ:%I64d\n", pTransEndInfo->nBoardIndex);//test
					SAFE_DELETEDB(pResult);	
					//throw pTransEndInfo->nBoardIndex;
				}
				//ent24�߰� - ���ε�Ϸ�� �ֻ����� �ø� - E-------------------------------------------------------------------------------------------



				//20081008 ���ε� �Ϸ�� sanc = 0 �߰�, jyh
				//sprintf(m_pQuery2, "Update %s Set state=1 Where state=0 and idx = %I64d", pTableBoard, pTransEndInfo->nBoardIndex);
				sprintf(m_pQuery2, "Update %s Set state = 1, sanc = 0 ,sort=%s Where state=0 and idx = %s", TABLE_BBS, pRow[0], pToken->GetToken(1));

				if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadItem()��Catch(...)\n")));
				}








				pClient->send_packet_queue(LOAD_UPEND);

				delete pToken;
				return;
			}

			//printf("���ε� �н�: %s\n", pRow[3]);//test

			if(m_strResult != "")
				m_strResult = m_strResult + SEP_ITEM;

			m_strResult = m_strResult + pToken->GetToken(0) + SEP_FIELD;
			m_strResult = m_strResult + pToken->GetToken(1) + SEP_FIELD;
			m_strResult = m_strResult + pRow[0] + SEP_FIELD;
			m_strResult = m_strResult + pRow[1] + SEP_FIELD;
			m_strResult = m_strResult + pRow[2] + SEP_FIELD;
			m_strResult = m_strResult + pRow[3] + SEP_FIELD;
			m_strResult = m_strResult + pRow[4] + SEP_FIELD;
			m_strResult = m_strResult + pRow[5];
			
			while(pRow = mysql_fetch_row(pResult))
			{				
				if(m_strResult != "")
					m_strResult = m_strResult + SEP_ITEM;

				m_strResult = m_strResult + pToken->GetToken(0) + SEP_FIELD;
				m_strResult = m_strResult + pToken->GetToken(1) + SEP_FIELD;
				m_strResult = m_strResult + pRow[0] + SEP_FIELD;
				m_strResult = m_strResult + pRow[1] + SEP_FIELD;
				m_strResult = m_strResult + pRow[2] + SEP_FIELD;
				m_strResult = m_strResult + pRow[3] + SEP_FIELD;
				m_strResult = m_strResult + pRow[4] + SEP_FIELD;
				m_strResult = m_strResult + pRow[5];				
			}

			SAFE_DELETEDB(pResult);

			//printf("��� ����: %s\n", m_strResult);//test
			if(m_strResult.length() > 0)
				pClient->send_packet_queue(LOAD_UPLIST, (char*)m_strResult.c_str(), (int)m_strResult.length() + 1);
		}		
		delete pToken;				
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadItem()��Catch(...)\n"))); 
	}	
}





// ���ε�� �·�ī ���۱� �ڷ� ���
void DBCtrl::UpLoadState_Copyright_data(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	PUPLOADMUREKA pTransMurekaInfo;

	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

//printf("���۱�üũ ���\n");
/*
FILE *fp;
fp=fopen("b.txt","a");
fprintf(fp,"���۱�üũ\n\n");
fclose(fp);
*/
	try
	{

		pTransMurekaInfo = (PUPLOADMUREKA)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		//���� �ڷ� �Է�üũ
		sprintf(m_pQuery1,  "SELECT u_code FROM %s WHERE u_board_idx='%I64d' AND u_file_idx='%I64d' and u_video_id='%I64d' and u_turn='%d' limit 0,1",	TABLE_MUREKA_COPYRIGHT_HASH,
																												pTransMurekaInfo->nBoardIndex,
																												pTransMurekaInfo->uFile_code,
																												pTransMurekaInfo->uVideo_id,
																												pTransMurekaInfo->uTurn);

/*
FILE *fp;
fp=fopen("b.txt","a");
fprintf(fp,"%s\n\n",m_pQuery1);
fclose(fp);
*/

		if((pResult = ExecuteQueryR(m_pQuery1)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			//���۱� �ڷ� ��� ����
			sprintf(m_pQuery1,	"insert %s set " \
								"u_board_idx='%I64d',"\
								"u_filetype='%d',"\
								"u_status='%s',"\
								"u_hash='%s',"\
								"u_money='%d',"\
								"u_file_idx='%I64d',"\
								"u_title='%s',"\
								"u_date=unix_timestamp(),"\
								"u_video_id='%I64d',"\
								"u_video_content_id='%s',"\
								"u_video_code='%s',"\
								"u_music_id='%I64d',"\
								"u_music_content_id='%s',"\
								"u_music_company='%s',"\
								"u_cp_id='%s',"\
								"u_cp_company='%s',"\
								"u_movie_content_id='%s',"\
								"u_movie_code='%s',"\
								"u_movie_cha='%d',"\
								"u_movie_jibun='%d',"\
								"u_movie_onair_date='%d',"\
								"u_counts='%d',"\
								"u_filtering_key='%s',"\
								"u_smartblock_key='%s',"\
								"u_hash_org='%s',"\
								"u_turn='%d'",
								TABLE_MUREKA_COPYRIGHT_HASH,
								pTransMurekaInfo->nBoardIndex,
								pTransMurekaInfo->uFiletype,
								pTransMurekaInfo->uStatus,
								pTransMurekaInfo->uHash,
								pTransMurekaInfo->uMoney,
								pTransMurekaInfo->uFile_code,
								pTransMurekaInfo->uTitle,
								pTransMurekaInfo->uVideo_id,
								pTransMurekaInfo->uVideo_content_id,
								pTransMurekaInfo->uVideo_code,
								pTransMurekaInfo->uMusic_id,
								pTransMurekaInfo->uMusic_content_id,
								pTransMurekaInfo->uMusic_company,
								pTransMurekaInfo->uCp_id,
								pTransMurekaInfo->uCp_company,
								pTransMurekaInfo->uMovie_content_id,
								pTransMurekaInfo->uMovie_code,
								pTransMurekaInfo->uMovie_cha,
								pTransMurekaInfo->uMovie_jibun,
								pTransMurekaInfo->uMovie_onair_date,
								pTransMurekaInfo->uCounts,
								pTransMurekaInfo->ufiltering_key,
								pTransMurekaInfo->usmartblock_key,
								pTransMurekaInfo->uHash_master,
								pTransMurekaInfo->uTurn
					);
			

/*
fp=fopen("b.txt","a");
fprintf(fp,"%s\n\n",m_pQuery1);
fclose(fp);
*/
			ExecuteQuery(m_pQuery1);
		}

		//������ ������(���� ����� ���ؼ������°�) - ����� �ǹ� ����
		pClient->send_packet_queue(SAVE_MUREKA_DATA, (char*)pTransMurekaInfo, sizeof(PUPLOADMUREKA));	

		SAFE_DELETEDB(pResult);
	}
	catch(...)
	{
		pTransMurekaInfo->nResult=0;
		pClient->send_packet_queue(SAVE_MUREKA_DATA, (char*)pTransMurekaInfo, sizeof(PUPLOADMUREKA));		

		ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadState_Enswer()��Catch(...)\n"))); 
	}
}


