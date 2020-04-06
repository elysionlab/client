#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"


extern SERVER_RUNTIME_INFO RunServerInfo;

//20071210 업로드전 해시값 비교 요청, jyh
void DBCtrl::UpLoadHash_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	PUPHASH		upHash;
	std::string m_strResult;
	int nRet = 0;

	try
	{
		upHash = (PUPHASH)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
		//printf("link_idx: %d\n fileType: %d\n hashIndex: %s\n hashPath: %s\n checksum: %s\n\n",
		//	upHash->link_idx, upHash->nFileType, upHash->szIndex, upHash->szPath, upHash->szCheckSum);
		
		CToken* pTokenIdx = new CToken("/");
		CToken* pTokenCheckSum = new CToken("/");

		pTokenIdx->Split(upHash->szIndex);
		pTokenCheckSum->Split(upHash->szCheckSum);

		//printf("checksum: %s\n", upHash->szCheckSum);
		//printf("인덱스 갯수: %d\n", pTokenIdx->GetCount());
		//printf("해시 갯수: %d\n", pTokenCheckSum->GetCount());

		int cnt = 0;	//컨텐츠가 등록 완료 되었는지 비교하기 위해
		
		if(lstrcmp(upHash->szIndex, ""))
		{
			//저작권 파일 등록 페이지
			for(size_t i=0; i<pTokenIdx->GetCount(); i++)
			{
				sprintf_s(m_pQuery1, 4096, "Select idx From %s Where md5sum = '%s' and complete = 1", TABLE_HASH_COPYRIGHT, pTokenCheckSum->GetToken(i));
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

			//cnt값과 같아야 완전히 등록된 컨텐츠다
			if(cnt == pTokenIdx->GetCount())
			{
				//printf("cnt와 아이템수가 같다. UPLOAD_RESULT_HASH_REG_EXIST\n");
				//hash_copyright 테이블에서 삭제
				sprintf_s(m_pQuery2, 4096, "Delete From %s where link_idx = %d", TABLE_HASH_COPYRIGHT, upHash->link_idx);
				//printf("%s\n", m_pQuery2);
				nRet = ExecuteQuery(m_pQuery2);
				//hash_bbs 테이블에서도 삭제
				sprintf_s(m_pQuery2, 4096, "Delete From %s where idx = %d", TABLE_HASH_BBS, upHash->link_idx);
				nRet = ExecuteQuery(m_pQuery2);
				upHash->nResult = UPLOAD_RESULT_HASH_REG_EXIST;
			}

			if(upHash->nResult != UPLOAD_RESULT_HASH_REG_EXIST)
			{
				for(size_t j=0; j<pTokenIdx->GetCount(); j++)
				{
					sprintf_s(m_pQuery2, 4096, "Update %s Set md5sum = '%s' where idx = %d", TABLE_HASH_COPYRIGHT, pTokenCheckSum->GetToken(j), atoi(pTokenIdx->GetToken(j)));
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
			for(size_t i=0; i<pTokenCheckSum->GetCount(); i++)
			{
				//일반 파일 등록 페이지
				sprintf_s(m_pQuery1, 4096, "Select idx From %s Where md5sum = '%s' and disable = 0 and complete = 1", TABLE_HASH_COPYRIGHT, pTokenCheckSum->GetToken(i));

				if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
				{
					upHash->nResult = UPLOAD_RESULT_HASH_NOTEXIST;
					SAFE_DELETEDB(pResult);
				}
				else
				{
					//20080506 해시값이 등록되어 있으면 _webhard_bbs테이블에서 등록중이던 컨텐츠의 state값을 99으로 바꾼다
					sprintf_s(m_pQuery2, 4096, "Update %s SET state = 99 where idx = %d", TABLE_BBS, upHash->link_idx);
					nRet = ExecuteQuery(m_pQuery2);

					//해시값이 등록되어 있으면 _webhard_file_bbs테이블에서 등록중이던 컨텐츠 목록을 삭제한다
					sprintf_s(m_pQuery2, 4096, "Delete From %s where bbs_no = %d", TABLE_FILE_BBS, upHash->link_idx);
					nRet = ExecuteQuery(m_pQuery2);
					upHash->nResult = UPLOAD_RESULT_HASH_EXIST;
					SAFE_DELETEDB(pResult);

					break;
				}
			}			
		}
			
		pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));

		delete pTokenIdx;
		delete pTokenCheckSum;
	}
	catch (...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadHash()━Catch(...)\n"))); 

		upHash->nResult = UPLOAD_RESULT_HASH_ERROR;			
		pClient->send_packet_queue(LOAD_UPHASH, (char*)upHash, sizeof(UPHASH));
	}
}

//20071211 구라 전송 완료 패킷, jyh
void DBCtrl::UpLoadGuraEnd_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	//MYSQL_ROW	pRow;

	PUPLOADEND pGuraUploadEnd;
	int	nRet = 0;

	try
	{
		pGuraUploadEnd = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		sprintf_s(m_pQuery1, 4096, "Update %s Set complete = 1 where idx = %d", TABLE_HASH_COPYRIGHT, pGuraUploadEnd->nFileIdx);
		nRet = ExecuteQuery(m_pQuery1);

		//실패시 한번더...
		if(nRet != QUERY_COMPLETE)
		{
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw UPLOAD_RESULT_ERROR_DB;
		}

		if(pGuraUploadEnd->nContentsCmpl == 1)
		{
			sprintf_s(m_pQuery1, 4096, "Update %s Set state = 1 where idx = %d LIMIT 1", TABLE_HASH_BBS, pGuraUploadEnd->nBoardIndex);
			nRet = ExecuteQuery(m_pQuery1);
		}

		//실패시 한번더...
		if(nRet != QUERY_COMPLETE)
		{
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw UPLOAD_RESULT_ERROR_DB;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadGuraEnd()━Catch(...)\n"))); 
	}
}

// 업로드 이전에 파일 등록 또는 올리던 파일 정보 가져오기
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

	char		pServerIP[20];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	PUPLOADPRE	pUploadPre;

	try
	{
		pUploadPre = (PUPLOADPRE)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* 업로드 요청받은 파일의 존재유무 체크 : 타입별로 다르게 접근 */
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

		sprintf_s(m_pQuery1, 4096, "Select size, realsize, temp_volume, flag_upload, flag_realupload, temp_name, flag_unity From %s Where no = %I64d",  
							pTableFile, pUploadPre->nFileIdx);

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


		// 파일사이즈 체크 : 로컬파일의 사이즈가 등록시 파일 사이즈와 변경됨
		if(pUploadPre->nFileSize != nSize)
			throw UPLOAD_RESULT_SIZE;

		
		// 업로드가 완료되지 않고 실제로 이어 올리기 일때 사이즈 다시 체크
		if(!strcmp(pFlag, "N") && pUploadPre->nFileSize_Complete > 0 && !strcmp(pFlag_Real, "Y"))
		{
			if(LoadFtpInfoUpload(nVolumeIdx, 0, pServerIP, nServerPT, pServerID, pServerPW))
			{
				// FTP서버에 연결이 가능한지 체크
				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
				{
					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pTempName);
					pClient->m_pFtp->DisConnect();

					// 파일이 존재하지 않을 경우
					if(nServerFileSize == -2)
					{
						pUploadPre->nFileSize_Complete = 0;

						// 파일의 기본값을 초기화해서 새롭게 파일을 올리도록 설정
						sprintf_s(m_pQuery1, 4096, "Update %s Set temp_name='',md5sum='',realsize=0 Where no=%I64d", pTableFile, pUploadPre->nFileIdx);
						ExecuteQuery(m_pQuery1);

						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadPre()━File Continue : Not Found : %s\n"), pClient->getuserinfo()));
					}

					// 파일이 존재하고 사이즈가 0또는 0보다 커야함
					if(nServerFileSize >= 0 && nSize >= nServerFileSize && nServerFileSize != pUploadPre->nFileSize_Complete)
					{
						pUploadPre->nFileSize_Complete = nServerFileSize;

						// 디비 실제사이즈와 파일서버의 실제 사이즈가 다를경우 업데이트
						sprintf_s(m_pQuery1, 4096, "Update %s Set realsize=%I64d Where no = %I64d", pTableFile, nServerFileSize, pUploadPre->nFileIdx);
						ExecuteQuery(m_pQuery1);

						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadPre()━File Continue : Size difference : %s\n"), pClient->getuserinfo()));
					}
				}				
			}
		}
		
		// 파일 업로드 완료
		if(!strcmp(pFlag, "Y"))
			throw UPLOAD_RESULT_END;
		else
		{
			// 업로드 완료전 일때 파일패스 존재하는지 체크
			sprintf_s(m_pQuery1, 4096, "Select t_files.no From %s t_pds, %s t_files Where t_pds.no = %I64d And t_pds.no = t_files.no",  
								pTableFile, pTableFilePath, pUploadPre->nFileIdx);
			if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			{
				SAFE_DELETEDB(pResult);	
				throw UPLOAD_RESULT_PATH;
			}
			SAFE_DELETEDB(pResult);	

			// 정상적으로 파일 업로드 가능
			if(m_bServiceRun) pUploadPre->nResult = UPLOAD_RESULT_NEW;			
			// 서비스 중지된 상태
			else pUploadPre->nResult = UPLOAD_RESULT_SERVICE_STOP;		

			pClient->send_packet_queue(LOAD_UPPRE, (char*)pUploadPre, sizeof(UPLOADPRE));
		}
	}

	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadPre()━Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pUploadPre->nResult = nCode;			
		pClient->send_packet_queue(LOAD_UPPRE, (char*)pUploadPre, sizeof(UPLOADPRE));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadPre()━Catch(...)\n"))); 

		pUploadPre->nResult = UPLOAD_RESULT_ERROR;			
		pClient->send_packet_queue(LOAD_UPPRE, (char*)pUploadPre, sizeof(UPLOADPRE));
	}
}

//// 실제 파일 업로드를 위해 업로드 서버 정보 가져오기
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
//		/* 현재 업로드 된 상태 체크 : 타입별로 다르게 접근 */ 
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
//		// 업로드중 완료된 사이즈를 전송 받을때 확인을 위해 설정
//		pClient->m_nFileNo			= pUploadPor->nFileIdx;
//		pClient->m_nFileSizeBegin	= nSizeReal;
//
//
//		// 완료된 파일인지 체크
//		if(!strcmp(pFlag_Upload, "Y"))
//			throw UPLOAD_RESULT_END;
//
//		// 전송 시작사이즈가 파일 현재 저장사이즈 보다 큰경우
//		if(pUploadPor->nFileSize_Start > nSizeReal)
//			throw UPLOAD_RESULT_SIZE;
//
//		// 초기 등록된 CheckSum 가 값이 다를경우 다른파일
//		if(strcmp(pFlag_CheckSum, "") && strcmp(pFlag_CheckSum, pUploadPor->pFileCheckSum))
//		{			
//			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadProcess()━md5sum error : OLD = %s, NEW = %s : %s\n"), pFlag_CheckSum, pUploadPor->pFileCheckSum, pClient->getuserinfo())); 		
//			strcpy(pFlag_CheckSum, "");
//		}
//
//		// CheckSum 값이 등록되지 않았으면 등록
//		if(!strcmp(pFlag_CheckSum, ""))
//		{
//			sprintf(m_pQuery1, "Update %s Set md5sum = '%s', upload_date = UNIX_TIMESTAMP() Where no = %I64d", 
//				pTableFile, pUploadPor->pFileCheckSum, pUploadPor->nFileIdx);
//			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//				throw UPLOAD_RESULT_ERROR_DB;
//		}
//
//		// 파일명이 존재하면 pFlag_RealUpload 설정에 따라 실제 업로드 또는 트릭업로드 설정
//		if(!strcmp(pSaveName, ""))
//		{
//			__int64 nCheckResult = -1;	
//
//			// 임시 파일명 설정
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
//			// 동일 파일 체크
//			if(nCheckResult == -1)
//			{				
//				// 실제 업로드
//				strcpy(pFlag_RealUpload, "Y");				
//
//				// 파일명 설정
//				if(OPTION_MIN_FILEUNITY < nSize)
//					sprintf(pSaveName, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
//				else
//					strcpy(pSaveName, pTempName);
//
//				// FTP 서버 할당
//				__int64 nAllotment = 0;
//
//				// 현재 접속자가 업로드 했던 볼륨이 있을경우 해당 볼륨으로 업로드, 단 해당 볼륨의 상태가 업로드 가능한 상태여야함
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
//				// 실제 업로드 하지 않음
//				strcpy(pFlag_RealUpload, "N");
//
//				// 파일명 설정
//				strcpy(pSaveName, pTempName);
//
//				sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_realupload = 'N', flag_unity='%s' Where no = %I64d", 
//					pTableFile, nCheckResult, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
//				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//					throw UPLOAD_RESULT_ERROR_DB;				
//			}			
//		}
//
//		// 실제 업로드의 경우 해당 서버와 연결 후 데이터 채널 오픈
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
//			// FTP서버에 연결이 가능한지 체크
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
//					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋╋╋╋╋╋╋╋╋╋╋╋ %s : %s\n"), pServerIP, pServerID)); 	
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
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadProcess()━Catch(...)\n"))); 		
//		pUploadPor->nResult	= UPLOAD_RESULT_ERROR;
//		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));
//	}
//}

// 실제 파일 업로드를 위해 업로드 서버 정보 가져오기(한서버 업로드로 수정 버전)
void DBCtrl::UpLoadProcess_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_RES	*pResult_1 = 0;
	MYSQL_ROW	pRow;
	MYSQL_ROW	pRow_1;
	MYSQL_FIELD pField;

	ACE_UINT64	nSize;
	ACE_UINT64	nSizeReal;
	
	char		pFlag_Upload[2];
	char		pFlag_RealUpload[2];
	char		pFlag_Unity[2];
	char		pFlag_CheckSum[40];
	char		pSaveName[256];	
	char		pTempName[256];
	char		pTempTimeStamp[50];
	char		pTableFile[30];

	char		pServerIP[20];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;
	int			nTempVolume = 0;	//20071126 파일 서버 번호, jyh
	int			nIdx;			//20071126 파일의 인덱스, jyh
	int			nbbsNo;			//20071126 게시물 인덱스, jyh
	int			nMinIdx;		//20080129 게시물의 인덱스중 최소값, jyh

	PUPLOADPRO	pUploadPor;

	try
	{
		pUploadPor = (PUPLOADPRO)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		if(pUploadPor->nUploadSpeed < 0) pUploadPor->nUploadSpeed = 0;
		if(pUploadPor->nUploadSpeed > 9) pUploadPor->nUploadSpeed = 9;

		/* 현재 업로드 된 상태 체크 : 타입별로 다르게 접근 */ 
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
		sprintf_s(m_pQuery1, 4096, "Select size, realsize, flag_upload, md5sum, flag_realupload, temp_name, flag_unity, UNIX_TIMESTAMP(), idx, bbs_no, temp_volume " \
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
		nIdx		= atoi(pRow[8]);	//20071126 파일의 인덱스, jyh
		nbbsNo		= atoi(pRow[9]);	//20071126 게시물 인덱스, jyh
		nTempVolume	= atoi(pRow[10]);	//20080212 서버 주소, jyh
		SAFE_DELETEDB(pResult);	

		// 업로드중 완료된 사이즈를 전송 받을때 확인을 위해 설정
		pClient->m_nFileNo			= pUploadPor->nFileIdx;
		pClient->m_nFileSizeBegin	= nSizeReal;


		// 완료된 파일인지 체크
		if(!strcmp(pFlag_Upload, "Y"))
			throw UPLOAD_RESULT_END;

		// 전송 시작사이즈가 파일 현재 저장사이즈 보다 큰경우
		if(pUploadPor->nFileSize_Start > nSizeReal)
			throw UPLOAD_RESULT_SIZE;

		// 초기 등록된 CheckSum 가 값이 다를경우 다른파일
		if(strcmp(pFlag_CheckSum, "") && strcmp(pFlag_CheckSum, pUploadPor->pFileCheckSum))
		{			
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadProcess()━md5sum error : OLD = %s, NEW = %s : %s\n"), pFlag_CheckSum, pUploadPor->pFileCheckSum, pClient->getuserinfo())); 		
			strcpy(pFlag_CheckSum, "");
		}

		// CheckSum 값이 등록되지 않았으면 등록
		if(!strcmp(pFlag_CheckSum, ""))
		{
			sprintf_s(m_pQuery1, 4096, "Update %s Set md5sum = '%s', upload_date = UNIX_TIMESTAMP() Where no = %I64d", 
								pTableFile, pUploadPor->pFileCheckSum, pUploadPor->nFileIdx);
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw UPLOAD_RESULT_ERROR_DB;
		}

		// 파일명이 존재하면 pFlag_RealUpload 설정에 따라 실제 업로드 또는 트릭업로드 설정
		if(!strcmp(pSaveName, ""))
		{
			__int64 nCheckResult = -1;	
            
			// 임시 파일명 설정
			//20080123 저용량 파일도 트릭업로드를 위해, jyh
			sprintf_s(pTempName, 256, "%s_%I64d", pUploadPor->pFileCheckSum, nSize);
			strcpy(pFlag_Unity, "Y");

			//20080511 버그 수정, jyh-------------------------------------------
			////20080212 트릭과 실제 업로드를 구분하기 전에 무조건 idx값이 최소인 파일의 서버 주소를 따른다, jyh
			//// FTP 서버 할당 ---> DB에서 temp_volume을 읽어오는 걸로 수정, jyh
			//// 이전 파일이 저장된 서버가 업로드 중지중 이더라도 무조건 그 서버로 저장
			//sprintf(m_pQuery1,  "Select idx From %s Where bbs_no = %I64d order by (idx) asc", pTableFile, pUploadPor->nBoardIndex);
			////20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
			//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			//{
			//	SAFE_DELETEDB(pResult);	
			//	throw UPLOAD_RESULT_NOTFOUND;
			//}
			//else
			//{
			//	nMinIdx = atoi(pRow[0]);
			//	SAFE_DELETEDB(pResult);
			//}
			//
			//if(nIdx > nMinIdx)
			//{
			//	//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
			//	sprintf(m_pQuery1,  "Select temp_volume From %s Where bbs_no = %d && idx = %d", pTableFile, nbbsNo, nMinIdx);

			//	if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			//	{
			//		SAFE_DELETEDB(pResult);	
			//		throw UPLOAD_RESULT_NOTFOUND;
			//	}
			//	else
			//	{
			//		nTempVolume	= atoi(pRow[0]);	
			//		SAFE_DELETEDB(pResult);	
			//	}	
			//}
			//-------------------------------------------------------------------

			int nSameFileCountBBS = LoadFileCountCp(pUploadPor->pFileCheckSum, nSize);

			//20080124 업로드 완료된 같은 파일이 3으로 나눠서 나머지가 없으면 실제업로드 있으면 트릭 업로드, jyh
			if((nSameFileCountBBS % RunServerInfo.nLimit_DuplicateFile) == 0)
			{
				//printf("실제 업로드\n");
				//실제 업로드
				strcpy(pFlag_RealUpload, "Y");				

				// 파일명 설정
				//if(OPTION_MIN_FILEUNITY < nSize)
					sprintf_s(pSaveName, 256, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
				//else
				//	strcpy(pSaveName, pTempName);

				//// FTP 서버 할당 ---> DB에서 temp_volume을 읽어오는 걸로 수정, jyh
				//// 이전 파일이 저장된 서버가 업로드 중지중 이더라도 무조건 그 서버로 저장
				//sprintf(m_pQuery1,  "Select idx From %s Where bbs_no = %I64d order by (idx) asc", pTableFile, pUploadPor->nBoardIndex);
				////20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
				//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
				//{
				//	SAFE_DELETEDB(pResult);	
				//	throw UPLOAD_RESULT_NOTFOUND;
				//}
				//else
				//{
				//	nMinIdx = atoi(pRow[0]);
				//	SAFE_DELETEDB(pResult);
				//}
				//
				//if(nIdx > nMinIdx)
				//{
				//	//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
				//	sprintf(m_pQuery1,  "Select temp_volume From %s Where bbs_no = %d && idx = %d", pTableFile, nbbsNo, nMinIdx);

				//	if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
				//	{
				//		SAFE_DELETEDB(pResult);	
				//		throw UPLOAD_RESULT_NOTFOUND;
				//	}
				//	else
				//	{
				//		nTempVolume	= atoi(pRow[0]);	
				//		SAFE_DELETEDB(pResult);	
				//	}					
				//}
				//else
				//{
				//	if((nTempVolume = (int)LoadFtpAllotment()) == -1)
				//		throw UPLOAD_RESULT_ERROR_ALLO;
				//}

				//20080212 서버 할당이 안됐을때만 할당 한다, jyh
				if(nTempVolume == 0)
				{
					if((nTempVolume = (int)LoadFtpAllotment()) == -1)
						throw UPLOAD_RESULT_ERROR_ALLO;

					//20080511 업로드시 서버번호가 달라지는거 수정, jyh
					sprintf_s(m_pQuery2, 4096, "Update %s Set temp_volume = %d Where bbs_no = %I64d", pTableFile, nTempVolume, pUploadPor->nBoardIndex);
					if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
						throw UPLOAD_RESULT_ERROR_DB;
				}
				
				//20080511 업로드시 서버번호가 달라지는거 수정, jyh
				/*sprintf(m_pQuery1, "Update %s Set temp_volume = %d, temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
					pTableFile, nTempVolume, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);*/
				sprintf_s(m_pQuery1, 4096, "Update %s Set temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
					pTableFile, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
					throw UPLOAD_RESULT_ERROR_DB;
			}
			else
			{
				//printf("트릭 업로드\n");
				bool bReal = false; //파일서버에 실제 파일이 없을시 실제 업로드를 해야한다, jyh
				//트릭 업로드
				strcpy(pFlag_RealUpload, "N");

				//20080529 첫번째 파일에 서버 할당이 안된 경우, jyh
				if(nTempVolume == 0)
				{
					sprintf_s(m_pQuery1, 4096, "Select temp_volume From %s Where md5sum='%s' and flag_realupload = 'Y' and contents_complete = 1 order by (no) desc", pTableFile, pUploadPor->pFileCheckSum);

					if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
					{
						SAFE_DELETEDB(pResult);	

						//20080529 트릭업로드로 배정받는 서버가 없을시 첫번째 파일일 경우 서버를 재할당 받는다, jyh
						if((nTempVolume = (int)LoadFtpAllotment()) == -1)
							throw UPLOAD_RESULT_ERROR_ALLO;

						sprintf_s(m_pQuery2, 4096, "Update %s Set temp_volume = %d Where bbs_no = %I64d", pTableFile, nTempVolume, pUploadPor->nBoardIndex);
						if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
							throw UPLOAD_RESULT_ERROR_DB;
					}
					else
					{
						//20080212 서버 할당이 안됐을때만 할당 한다, jyh
						nTempVolume = atoi(pRow[0]);

						//printf("트릭업로드로 배정받은 서버: %d\n", nTempVolume);

						//20080522 트릭업로드 일지라도 배정받은 서버가 flag_state = 'N' OR flag_use = 'N' 이면 다른서버로 재할당 받는다, jyh
						sprintf_s(m_pQuery2, 4096, "Select flag_state, flag_use From %s Where ftp = %d", TABLE_COMMON_FTP_SERVER, nTempVolume);
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
								if((nTempVolume = (int)LoadFtpAllotment()) == -1)
									throw UPLOAD_RESULT_ERROR_ALLO;

								//printf("서버 재할당: %d\n", nTempVolume);
							}

							SAFE_DELETEDB(pResult_1);
						}

						SAFE_DELETEDB(pResult);

						sprintf_s(m_pQuery2, 4096, "Update %s Set temp_volume = %d Where bbs_no = %I64d", pTableFile, nTempVolume, pUploadPor->nBoardIndex);
						if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
							throw UPLOAD_RESULT_ERROR_DB;
					}	
				}

				//배정받은 서버에 파일이 제대로 있는지 검사하고
				//파일이 제대로 없으면 실제 업로드로 변신~~, jyh
				if(LoadNewFileCheck(pTempName, nSize, (__int64)nTempVolume, 1) < 0)
				{
					//printf("트릭에서 실제 업로드로 변신\n");
					//실제 업로드
					strcpy(pFlag_RealUpload, "Y");				

					// 파일명 설정
					//if(OPTION_MIN_FILEUNITY < nSize)
					sprintf_s(pSaveName, 4096, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
					//else
					//	strcpy(pSaveName, pTempName);

					/*sprintf(m_pQuery1, "Update %s Set temp_volume = %d, temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
						pTableFile, nTempVolume, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);*/
					sprintf_s(m_pQuery1, 4096, "Update %s Set temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
						pTableFile, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
					if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
						throw UPLOAD_RESULT_ERROR_DB;

					bReal = true;
				}

				if(!bReal)
				{
					// 파일명 설정
					strcpy(pSaveName, pTempName);

					sprintf_s(m_pQuery1, 4096, "Update %s Set temp_name = '%s', flag_realupload = 'N', flag_unity='%s' Where no = %I64d", 
						pTableFile, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
					if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
						throw UPLOAD_RESULT_ERROR_DB;
				}				
			}
			
			/*if(OPTION_MIN_FILEUNITY < nSize)
			{
				sprintf(pTempName, "%s_%I64d", pUploadPor->pFileCheckSum, nSize);
				strcpy(pFlag_Unity, "Y");

				int nSameFileCountCore = LoadFileCountCore(pTempName);
				int nSameFileCountBBS = LoadFileCountCp(pUploadPor->pFileCheckSum, nSize);
				if(nSameFileCountCore != 0 && nSameFileCountBBS != 0 && RunServerInfo.nLimit_DuplicateFile > 1 && nSameFileCountBBS <= (nSameFileCountCore * RunServerInfo.nLimit_DuplicateFile))
					nCheckResult = LoadNewFileCheck(pTempName, nSize);
			}
			else
			{
				sprintf(pTempName, "%s_%I64d_%s", pUploadPor->pFileCheckSum, nSize, pTempTimeStamp);
				strcpy(pFlag_Unity, "N");
			}			*/
			
			//// 동일 파일 체크
			//if(nCheckResult == -1)
			//{				
			//	// 실제 업로드
			//	strcpy(pFlag_RealUpload, "Y");				

			//	// 파일명 설정
			//	if(OPTION_MIN_FILEUNITY < nSize)
			//		sprintf(pSaveName, "_tmp_%s_%I64d_%I64d", pUploadPor->pFileCheckSum, nSize, pUploadPor->nFileIdx);
			//	else
			//		strcpy(pSaveName, pTempName);

			//	// FTP 서버 할당 ---> DB에서 temp_volume을 읽어오는 걸로 수정, jyh
			//	// 이전 파일이 저장된 서버가 업로드 중지중 이더라도 무조건 그 서버로 저장
			//	if(nIdx > 1)
			//	{
			//		//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
			//		sprintf(m_pQuery1,  "Select temp_volume From %s Where bbs_no = %d && idx = 1", pTableFile, nbbsNo);
			//		
			//		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			//		{
			//			SAFE_DELETEDB(pResult);	
			//			throw UPLOAD_RESULT_NOTFOUND;
			//		}
			//		nTempVolume		= atoi(pRow[0]);	
			//		SAFE_DELETEDB(pResult);	
			//	}
			//	
			//	int nAllotment = 0;
			//	
			//	if(nTempVolume != 0)
			//		nAllotment = nTempVolume;
			//	else
			//	{
			//		if((nAllotment = LoadFtpAllotment()) == -1)
			//			throw UPLOAD_RESULT_ERROR_ALLO;
			//	}

			//	// 현재 접속자가 업로드 했던 볼륨이 있을경우 해당 볼륨으로 업로드, 단 해당 볼륨의 상태가 업로드 가능한 상태여야함
			//	/*if(pClient->m_nLastVolumeUpload != 0 && LoadFtpAllotmentQueue(pClient->m_nLastVolumeUpload))
			//		nAllotment = pClient->m_nLastVolumeUpload;
			//					
			//	if(nAllotment == 0)
			//	{
			//		if((nAllotment = LoadFtpAllotment()) == -1)
			//			throw UPLOAD_RESULT_ERROR_ALLO;
			//		else
			//			pClient->m_nLastVolumeUpload = nAllotment;
			//	}*/
			//	
			//	//char temp[264];
			//	//printf("이전파일이 저장된 서버볼륨:%d\n", nTempVolume);
			//	//printf("현재 파일이 저장될 서버볼륨:%d\n", nAllotment);	//20071126 파일이 저장될 서버 번호, jyh

			//	sprintf(m_pQuery1, "Update %s Set temp_volume = %d, temp_name = '%s', flag_realupload = 'Y', flag_unity='%s' Where no = %I64d", 
			//						pTableFile, nAllotment, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
			//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
			//		throw UPLOAD_RESULT_ERROR_DB;
			//}
			//else
			//{
			//	// 실제 업로드 하지 않음
			//	strcpy(pFlag_RealUpload, "N");

			//	// 파일명 설정
			//	strcpy(pSaveName, pTempName);

			//	sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_realupload = 'N', flag_unity='%s' Where no = %I64d", 
			//						pTableFile, nCheckResult, pSaveName, pFlag_Unity, pUploadPor->nFileIdx);
			//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
			//		throw UPLOAD_RESULT_ERROR_DB;				
			//}			
		}

		// 실제 업로드의 경우 해당 서버와 연결 후 데이터 채널 오픈
		if(!strcmp(pFlag_RealUpload, "Y"))
		{
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
            if(!LoadFtpInfoUpload((__int64)nTempVolume, pUploadPor->nUploadSpeed, pServerIP, nServerPT, pServerID, pServerPW))
				throw UPLOAD_RESULT_ERROR_STOP;

			

			// FTP서버에 연결이 가능한지 체크
			for(int nProc = 0; nProc<10; nProc++)
			{
				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW)) 
				{
					if(pClient->m_pFtp->QueryTransfer(pSaveName, pUploadPor->nFileSize_Start, 0))
					{
						strcpy(pUploadPor->pServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
						break;
					}
					else	
						throw UPLOAD_RESULT_ERROR_DELAY;					
				}
				else
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋╋╋╋╋╋╋╋╋╋╋╋ %s : %s\n"), pServerIP, pServerID)); 	

					if(nProc >= 2)
						throw UPLOAD_RESULT_ERROR_DELAY;
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
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadProcess()━Catch(...)\n"))); 		
		pUploadPor->nResult	= UPLOAD_RESULT_ERROR;
		pClient->send_packet_queue(LOAD_UPLOAD, (char*)pUploadPor, sizeof(UPLOADPRO));
	}
}

// 업로드 진행상태
void DBCtrl::UpLoadState_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	PUPLOADEND pTransEndInfo;
	char pTableFile[30];

	try
	{
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
		
		if(pClient->m_nFileNo == pTransEndInfo->nFileIdx && pClient->m_nFileSizeBegin < pTransEndInfo->nFileSizeEnd)
		{	
			sprintf_s(m_pQuery1, 4096, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, pTransEndInfo->nFileSizeEnd, pTransEndInfo->nFileIdx);
			ExecuteQuery(m_pQuery1);
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadState()━Catch(...)\n"))); 
	}
}

// 업로드 완료/중단 요청
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

	char		pServerIP[20];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	bool		bProcComplete;

	try
	{
		pClient->m_nFileNo			= 0;
		pClient->m_nFileSizeBegin	= 0;

		pTransEndInfo = (PUPLOADEND)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		// 연결된 FTP 세션 종료
		pClient->m_pFtp->DisConnect();	

		/* 업로드 요청받은 파일의 존재유무 체크 : 타입별로 다르게 접근 */
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

		// 디비에 저장된 파일 상태 조회
		sprintf_s(m_pQuery1, 4096, "Select size, flag_realupload, temp_name, temp_volume, md5sum, realsize, flag_unity From %s Where no = %I64d",  
							pTableFile, pTransEndInfo->nFileIdx);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
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
			실제 업로드인 경우 FTP서버의 실제 업로드 사이즈 체크
			- 트릭 업로드의 경우 통과
			- 실패시 클라이언트에서 통보받은 사이즈로 설정
		*/	
		if(!strcmp(pFlag_RealUpload, "Y"))
		{
			if(!LoadFtpInfoUpload(nVolumeIdx, 0, pServerIP, nServerPT, pServerID, pServerPW))
				throw pTransEndInfo->nFileIdx;

			// 기본은 클라이언트에서 전송받은 용량
			nSizeReal = pTransEndInfo->nFileSizeEnd;
	
			for(int nProc = 0; nProc < 20; nProc++)
			{
				if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
				{
					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pSaveName);
					
					if(nServerFileSize == -2)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Not Found\n")));					
						pClient->m_pFtp->DisConnect();		
						break;
					}
					else if(nServerFileSize == -1)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Check Error : #nProc(%d)\n"), nProc));		
						pClient->m_pFtp->DisConnect();				
						continue;
					}
					else if(nServerFileSize >= 0 && nServerFileSize != pTransEndInfo->nFileSizeEnd)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Size Difference : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
						pClient->m_pFtp->DisConnect();		
						nSizeReal = nServerFileSize;
						ACE_OS::sleep(1);						
						continue;
					}										
					else if(nServerFileSize == pTransEndInfo->nFileSizeEnd)
					{
						if(nProc != 0)
							ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Size Complete : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
						nSizeReal = nServerFileSize;
						pClient->m_pFtp->DisConnect();		
						break;
					}
					pClient->m_pFtp->DisConnect();
				}				
			}
		}
		else
		{
			nSizeReal = pTransEndInfo->nFileSizeEnd;			
		}
		

		/*
			1. 업로드 완료시
				- 로컬경로 레코드 삭제				
				- 실제 파일명으로 변경
				- flag_upload 'Y'로 변경
				- 실제파일사이즈를 파일 사이즈로 업데이트
				
			2. 실제 업로드 완료시 추가 작업
				- FTP서버의 파일명 실제 파일명으로 변경
				- 이전 올라온 파일을 사용할 경우 완료된 파일 삭제
		*/		
		if(nSizeReal > nSize) nSizeReal = nSizeReal_Old;
		if(nSizeReal == nSize)
		{
			bool bChangeFile = false;
			char pNew_savename[256];
			//ACE_UINT64	nNew_VolumeIdx;
			__int64 nCheckResult = -1;
			
			//20080123 저용량파일도 트릭업로드 하기 위해, jyh
			sprintf_s(pNew_savename, 256, "%s_%I64d", pFlag_CheckSum, nSize);
			nCheckResult = LoadNewFileCheck(pNew_savename, nSize, nVolumeIdx);
			//printf("중복 파일 조사: %s(서버:%I64d)\n", pNew_savename, nCheckResult);

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

			// 실제업로드일 경우 파일명 변경 또는 임시 파일 삭제
			if(!strcmp(pFlag_RealUpload, "Y"))
			{
				// 현재 파일 삭제
				if(bChangeFile)
				{
					// FTP서버에 연결이 가능한지 체크
					if(!pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
						throw UPLOAD_RESULT_ERROR;

					pClient->m_pFtp->RemoveFile(pSaveName);
					pClient->m_pFtp->DisConnect();
					//printf("RemoveFile: %s\n", pSaveName);
				}
				// 파일명 변경
				else
				{	
					bProcComplete = false;
					for(int nProc = 0; nProc < 10; nProc++)
					{
						// FTP서버에 연결이 가능한지 체크
						if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
						{
							if(pClient->m_pFtp->RenameFile(pSaveName, pNew_savename))
							{
								//printf("RenameFile: %s, PrevRenameFile: %s\n", pSaveName, pNew_savename);
								pClient->m_pFtp->DisConnect();
								bProcComplete = true;
								break;
							}
							pClient->m_pFtp->DisConnect();
						}
					}

					if(!bProcComplete)
						throw UPLOAD_RESULT_ERROR;

					////20080123 컨텐츠 단위로 파일등록이 완료 됐다면 _webhard_file에 등록, jyh
					//sprintf(m_pQuery1,  "Select no From %s Where bbs_no = %I64d and contents_complete = 1",  
					//						pTableFile, pTransEndInfo->nBoardIndex);
					//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
					//{
					//	SAFE_DELETEDB(pResult);	
					//	throw pTransEndInfo->nFileIdx;
					//}
					//else
     //				{
					//	// 새로운 파일 업로드 등록
					//	SetNewFileComplete(pNew_savename, nSize, nNew_VolumeIdx);
					//	SAFE_DELETEDB(pResult);
					//}
				}

				sprintf_s(m_pQuery1, 4096, "Update %s Set temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d ",
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
				// - flag_upload 'Y'로 업데이트
				// - 실제파일사이즈를 파일 사이즈로 업데이트
				// - 실제 파일명으로 파일명 업데이트
				// - FTP 서버 변경되었으면 업데이트
				/*sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d", 
									pTableFile, nNew_VolumeIdx, pNew_savename, nSize, pTransEndInfo->nFileIdx);*/
				sprintf_s(m_pQuery1, 4096, "Update %s Set temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d", 
					pTableFile, pNew_savename, nSize, pTransEndInfo->nFileIdx);
				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
					throw pTransEndInfo->nFileIdx;
			}

			////20080123 컨텐츠 단위의 업로드가 완료되면 contents_complete을 1로 업데이트, jyh
			//if(pTransEndInfo->nContentsCmpl == 1)
			//{
			//	sprintf(m_pQuery1, "Update %s Set contents_complete = 1 Where bbs_no = %I64d", pTableFile, pTransEndInfo->nBoardIndex);
			//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
			//		throw pTransEndInfo->nBoardIndex;

			//	//컨텐츠의 사이즈를 알아온다.
			//	sprintf(m_pQuery1, "Select size From %s Where idx = %I64d", TABLE_BBS, pTransEndInfo->nBoardIndex);
			//	if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
			//	{
			//		SAFE_DELETEDB(pResult);	
			//		throw pTransEndInfo->nBoardIndex;
			//	}

			//	ACE_UINT64 nContentsSize = _atoi64(pRow[0]);
			//	SAFE_DELETEDB(pResult);

			//	//컨텐츠의 첫번째 md5sum을 알아온다.
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

			//	//20080214 _webhard_contents_unique 에 값 작성, jyh
			//	sprintf(m_pQuery1, "Insert into %s set bbs_no = %I64d, ftp_volume = %I64d, hash_size = '%s'", 
			//			TABLE_CONTENTS_UNIQUE, pTransEndInfo->nBoardIndex, nVolumeIdx, szHash_Size);
			//	if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
			//		throw pTransEndInfo->nBoardIndex;
			//}

			/* 업로드 완료로 로컬경로 레코드 삭제 */
			sprintf_s(m_pQuery1, 4096, "Delete From %s Where no = %I64d", pTableFilePath, pTransEndInfo->nFileIdx);
			ExecuteQuery(m_pQuery1);

			/* 현재 파일이 소속된 모든 파일이 등록 완료일 경우 : 해당 게시물을 등록 완료로 처리 */
			sprintf_s(m_pQuery1, 4096, "Select * From %s Where bbs_no = %I64d and flag_upload='N'", pTableFile, pTransEndInfo->nBoardIndex);
			for(int nProc = 0; nProc < 10; nProc++)
			{
				if(ExecuteQuery(m_pQuery1, nCount) == QUERY_COMPLETE)
				{
					if(nCount == 0)
					{
						for(int nProcSub = 0; nProcSub < 10; nProcSub++)
						{
							//20080123 컨텐츠 단위의 업로드가 완료되면 contents_complete을 1로 업데이트, jyh------------------------------------------
							sprintf_s(m_pQuery2, 4096, "Update %s Set contents_complete = 1 Where bbs_no = %I64d", pTableFile, pTransEndInfo->nBoardIndex);
							if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
								throw pTransEndInfo->nBoardIndex;

							//컨텐츠의 사이즈를 알아온다.
							sprintf_s(m_pQuery2, 4096, "Select size From %s Where idx = %I64d", TABLE_BBS, pTransEndInfo->nBoardIndex);
							if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
							{
								SAFE_DELETEDB(pResult);	
								throw pTransEndInfo->nBoardIndex;
							}

							ACE_UINT64 nContentsSize = _atoi64(pRow[0]);
							SAFE_DELETEDB(pResult);

							//컨텐츠의 첫번째 md5sum을 알아온다.
							sprintf_s(m_pQuery2, 4096, "Select md5sum From %s Where bbs_no = %I64d order by (idx) asc", pTableFile, pTransEndInfo->nBoardIndex);
							if((pResult = ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
							{
								SAFE_DELETEDB(pResult);	
								throw pTransEndInfo->nBoardIndex;
							}

							char szHash_Size[MAX_PATH];

							sprintf_s(szHash_Size, MAX_PATH, "%s_%I64d", pRow[0], nContentsSize);
							SAFE_DELETEDB(pResult);
							//printf("szHash_Size: %s", szHash_Size);

							//20080214 _webhard_contents_unique 에 값 작성, jyh
							sprintf_s(m_pQuery2, 4096, "Insert into %s set bbs_no = %I64d, ftp_volume = %I64d, hash_size = '%s'", 
								TABLE_CONTENTS_UNIQUE, pTransEndInfo->nBoardIndex, nVolumeIdx, szHash_Size);
							if(ExecuteQuery(m_pQuery2) != QUERY_COMPLETE)
								throw pTransEndInfo->nBoardIndex;
							//---------------------------------------------------------------------------------------------------------------------------

							sprintf_s(m_pQuery2, 4096, "Update %s Set state=1 Where state=0 and idx = %I64d", pTableBoard, pTransEndInfo->nBoardIndex);
							if(ExecuteQuery(m_pQuery2) == QUERY_COMPLETE)
								break;
						}

						for(int nProcSub = 0; nProcSub < 10; nProcSub++)
						{
							sprintf_s(m_pQuery2, 4096, "Insert Into %s Set board_idx=%I64d, type='%d', regdate=UNIX_TIMESTAMP()", 
												TABLE_FILE_UPLOAD_COMPLETE, pTransEndInfo->nBoardIndex, pTransEndInfo->nFileType);
							if(ExecuteQuery(m_pQuery2) == QUERY_COMPLETE)
								break;
						}
					}
					break;
				}
			}			
		}
		/* 파일 업로드가 완료되지 않은 경우 : 현재까지 업로드된 사이즈 업데이트 */
		else
		{
			sprintf_s(m_pQuery1, 4096, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, nSizeReal, pTransEndInfo->nFileIdx);
			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				throw pTransEndInfo->nFileIdx;
		}
	}
	catch(ACE_UINT64 nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd()━Catch(nCode) File Index : %Q : %s\n"), nCode, pClient->getuserinfo())); 
	}
	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd()━Catch(nCode) Error Code : %d\n"), nCode, pClient->getuserinfo())); 
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd()━Catch(...)\n"))); 
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
//		// 연결된 FTP 세션 종료
//		pClient->m_pFtp->DisConnect();	
//
//		/* 업로드 요청받은 파일의 존재유무 체크 : 타입별로 다르게 접근 */
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
//		// 디비에 저장된 파일 상태 조회
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
//		실제 업로드인 경우 FTP서버의 실제 업로드 사이즈 체크
//		- 트릭 업로드의 경우 통과
//		- 실패시 클라이언트에서 통보받은 사이즈로 설정
//		*/	
//		if(!strcmp(pFlag_RealUpload, "Y"))
//		{
//			if(!LoadFtpInfoUpload(nVolumeIdx, 0, pServerIP, nServerPT, pServerID, pServerPW))
//				throw pTransEndInfo->nFileIdx;
//
//			// 기본은 클라이언트에서 전송받은 용량
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
//						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Not Found\n")));					
//						pClient->m_pFtp->DisConnect();		
//						break;
//					}
//					else if(nServerFileSize == -1)
//					{
//						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Check Error : #nProc(%d)\n"), nProc));		
//						pClient->m_pFtp->DisConnect();				
//						continue;
//					}
//					else if(nServerFileSize >= 0 && nServerFileSize != pTransEndInfo->nFileSizeEnd)
//					{
//						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Size Difference : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
//						pClient->m_pFtp->DisConnect();		
//						nSizeReal = nServerFileSize;
//						ACE_OS::sleep(1);						
//						continue;
//					}										
//					else if(nServerFileSize == pTransEndInfo->nFileSizeEnd)
//					{
//						if(nProc != 0)
//							ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd_BBS()━File Size Complete : #nProc(%d) FileSize_Server(%Q) : FileSize_Client(%Q)\n"), nProc, nServerFileSize, pTransEndInfo->nFileSizeEnd));					
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
//		1. 업로드 완료시
//		- 로컬경로 레코드 삭제				
//		- 실제 파일명으로 변경
//		- flag_upload 'Y'로 변경
//		- 실제파일사이즈를 파일 사이즈로 업데이트
//
//		2. 실제 업로드 완료시 추가 작업
//		- FTP서버의 파일명 실제 파일명으로 변경
//		- 이전 올라온 파일을 사용할 경우 완료된 파일 삭제
//		*/		
//		if(nSizeReal > nSize) nSizeReal = nSizeReal_Old;
//		if(nSizeReal == nSize)
//		{
//			bool bChangFile = false;
//			char pNew_savename[256];
//			ACE_UINT64	nNew_VolumeIdx;
//			__int64 nCheckResult = -1;
//
//			//20080123 저용량파일도 트릭업로드 하기 위해, jyh
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
//			// 실제업로드일 경우 파일명 변경 또는 임시 파일 삭제
//			if(!strcmp(pFlag_RealUpload, "Y"))
//			{
//				// 현재 파일 삭제
//				if(bChangFile)
//				{
//					// FTP서버에 연결이 가능한지 체크
//					if(!pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
//						throw UPLOAD_RESULT_ERROR;
//
//					pClient->m_pFtp->RemoveFile(pSaveName);
//					pClient->m_pFtp->DisConnect();
//				}
//				// 파일명 변경
//				else
//				{	
//					bProcComplete = false;
//					for(int nProc = 0; nProc < 10; nProc++)
//					{
//						// FTP서버에 연결이 가능한지 체크
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
//					////20080123 컨텐츠 단위로 파일등록이 완료 됐다면 _webhard_file에 등록, jyh
//					//sprintf(m_pQuery1,  "Select no From %s Where bbs_no = %I64d and contents_complete = 1",  
//					//						pTableFile, pTransEndInfo->nBoardIndex);
//					//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//					//{
//					//	SAFE_DELETEDB(pResult);	
//					//	throw pTransEndInfo->nFileIdx;
//					//}
//					//else
//					//				{
//					//	// 새로운 파일 업로드 등록
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
//			// - flag_upload 'Y'로 업데이트
//			// - 실제파일사이즈를 파일 사이즈로 업데이트
//			// - 실제 파일명으로 파일명 업데이트
//			// - FTP 서버 변경되었으면 업데이트
//			sprintf(m_pQuery1, "Update %s Set temp_volume = %I64d, temp_name = '%s', flag_upload = 'Y', realsize = %I64d Where no = %I64d", 
//				pTableFile, nNew_VolumeIdx, pNew_savename, nSize, pTransEndInfo->nFileIdx);
//			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//				throw pTransEndInfo->nFileIdx;
//
//			//20080123 컨텐츠 단위의 업로드가 완료되면 contents_complete을 1로 업데이트
//			if(pTransEndInfo->nContentsCmpl == 1)
//			{
//				sprintf(m_pQuery1, "Update %s Set contents_complete = 1 Where bbs_no = %I64d", pTableFile, pTransEndInfo->nBoardIndex);
//				if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//					throw pTransEndInfo->nBoardIndex;	
//			}
//
//			/* 업로드 완료로 로컬경로 레코드 삭제 */
//			sprintf(m_pQuery1, "Delete From %s Where no = %I64d", pTableFilePath, pTransEndInfo->nFileIdx);
//			ExecuteQuery(m_pQuery1);
//
//			/* 현재 파일이 소속된 모든 파일이 등록 완료일 경우 : 해당 게시물을 등록 완료로 처리 */
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
//		/* 파일 업로드가 완료되지 않은 경우 : 현재까지 업로드된 사이즈 업데이트 */
//		else
//		{
//			sprintf(m_pQuery1, "Update %s Set realsize = %I64d Where no = %I64d", pTableFile, nSizeReal, pTransEndInfo->nFileIdx);
//			if(ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
//				throw pTransEndInfo->nFileIdx;
//		}
//	}
//	catch(ACE_UINT64 nCode)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd()━Catch(nCode) File Index : %Q : %s\n"), nCode, pClient->getuserinfo())); 
//	}
//	catch(int nCode)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd()━Catch(nCode) Error Code : %d\n"), nCode, pClient->getuserinfo())); 
//	}
//	catch(...)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadEnd()━Catch(...)\n"))); 
//	}
//}






// 업로드 아이템 추가
void DBCtrl::UpLoadItem_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	
	char		pTableFile[30];
	char		pTableFilePath[30];

	std::string m_strResult;

	try
	{
		CToken*	pToken = new CToken(SEP_FIELD);	

		char* pData = new char[nPS];
		memcpy(pData, (char*)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE), nPS);
		pToken->Split(pData);
		delete pData;
		
		if(pToken->GetCount() == 2)
		{			
			// 요청 타입에 맞는 테이블 쿼리
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

			sprintf_s(m_pQuery1, 4096, "Select t_file.no, t_file.size, t_file.realname, t_path.localname " \
								"From %s t_file, %s t_path Where t_file.bbs_no = %s And t_file.no = t_path.no " \
								"order by depth asc, foldername asc, realname asc",  
								pTableFile, pTableFilePath, pToken->GetToken(1));
			
			if((pResult = ExecuteQueryR(m_pQuery1)) == 0)
			{
				delete pToken;				
				return;
			}

			// 목록 생성
			m_strResult.clear();
			while(pRow = mysql_fetch_row(pResult))
			{				
				if(m_strResult != "")
					m_strResult = m_strResult + SEP_ITEM;

				m_strResult = m_strResult + pToken->GetToken(0) + SEP_FIELD;
				m_strResult = m_strResult + pToken->GetToken(1) + SEP_FIELD;
				m_strResult = m_strResult + pRow[0] + SEP_FIELD;
				m_strResult = m_strResult + pRow[1] + SEP_FIELD;
				m_strResult = m_strResult + pRow[2] + SEP_FIELD;
				m_strResult = m_strResult + pRow[3];				
			}
			SAFE_DELETEDB(pResult);	
			if(m_strResult.length() > 0)
				pClient->send_packet_queue(LOAD_UPLIST, (char*)m_strResult.c_str(), (int)m_strResult.length() + 1);
		}		
		delete pToken;				
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadItem()━Catch(...)\n"))); 
	}	
}


