#include "stdafx.h"
#include <string.h>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"
#include <vector>


extern SERVER_RUNTIME_INFO RunServerInfo;

//
//오픈 자료 업로드 관련
//

void DBCtrl::OpenUpLoadList(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	char		szTableName[30];
	char		szbbs_table[30];
	POPENUPLIST pOpenUpList;

	try
	{
		pOpenUpList = (POPENUPLIST)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		switch(pOpenUpList->nContentsType)
		{
		case 0:
			lstrcpy(szTableName, TABLE_OPEN_NECESSARY);
			lstrcpy(szbbs_table, "necessary");
			break;
		case 1:
			lstrcpy(szTableName, TABLE_OPEN_CAPTION);
			lstrcpy(szbbs_table, "caption");
			break;
		case 2:
			lstrcpy(szTableName, TABLE_OPEN_AS);
			lstrcpy(szbbs_table, "contents_as");
			break;
		}
		
		//컨텐츠의 각파일 인덱스를 알아온다.
		sprintf_s(m_pQuery1, 4096, "Select no, foldername, realname, size From %s Where bbs_no = %I64d and bbs_table = '%s'", TABLE_OPEN_DATA, pOpenUpList->nContentsIdx, szbbs_table);

		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENUP_RESULT_NOTFOUND;
		}

		std::vector<__int64>		v_n64FileIdx;		//파일 인덱스
		std::vector<__int64>		v_n64FileSize;		//파일 사이즈
		std::vector<std::string>	v_strFolderName;	//폴더 명
		std::vector<std::string>	v_strFileName;		//파일 명
		std::vector<int>			v_nFolderDepth;		//파일 depth
		std::string strTemp;

		v_n64FileIdx.push_back(_atoi64(pRow[0]));
		strTemp = pRow[1];
		v_strFolderName.push_back(strTemp);
		strTemp = pRow[2];
		v_strFileName.push_back(strTemp);
		v_n64FileSize.push_back(_atoi64(pRow[3]));
		v_nFolderDepth.push_back(1);	//첫번째는 무조건 1(폴더 종류)
		
		while((pRow = mysql_fetch_row(pResult)))
		{
			//printf("while문 진입\n");
			//printf("fileIndex:%s, folderName:%s, fileName:%s, fileSize:%s\n", pRow[0], pRow[1], pRow[2], pRow[3]);
			v_n64FileIdx.push_back(_atoi64(pRow[0]));
			strTemp = pRow[1];

			bool bSameFolder = false;
			int i;

			for(i=0; i<(int)v_strFolderName.size(); i++)
			{
				if(v_strFolderName[i] == strTemp)
				{
					bSameFolder = true;
					v_nFolderDepth.push_back(i+1);

					break;
				}
			}

			//같은 폴더가 없을때만 저장
			if(!bSameFolder)
			{
				v_nFolderDepth.push_back(i+1);
				v_strFolderName.push_back(strTemp);
			}

			strTemp = pRow[2];
			v_strFileName.push_back(strTemp);
			v_n64FileSize.push_back(_atoi64(pRow[3]));
		}

		SAFE_DELETEDB(pResult);

		std::string strContentsInfo;
		std::string strFolderName;
		char	szTemp[2048];

		//폴더가 있는지 비교해서 컨텐츠 정보를 조합
		if(v_strFolderName[0] == "/")
		{
			//폴더가 없으면 파일명만 조합(인덱스#<파일명#<사이즈)
			sprintf_s(szTemp, 2048, "%I64d#<%s#<%I64d", v_n64FileIdx[0], v_strFileName[0].c_str(), v_n64FileSize[0]);
			lstrcpy(pOpenUpList->szContentsInfo, szTemp);
			//printf("컨텐츠 조합: %s\n", pOpenUpList->szContentsInfo);
			lstrcpy(pOpenUpList->szFolderName, v_strFolderName[0].c_str());
			//printf("폴더 이름: %s\n", pOpenUpList->szFolderName);
		}
		else
		{
			//폴더가 있으면 폴더depth와 파일명을 합침.(인덱스#<폴더depth#<파일명#<사이즈#>인덱스#<폴더depth#<파일명#<사이즈......#>인덱스#<폴더depth#<파일명#<사이즈)
			for(int i=0; i<(int)v_n64FileIdx.size(); i++)
			{
				sprintf_s(szTemp, 2048, "%I64d#<%d#<%s#<%I64d", v_n64FileIdx[i], v_nFolderDepth[i], v_strFileName[i].c_str(), v_n64FileSize[i]);
				strTemp = szTemp;
				strContentsInfo += strTemp + SEP_ITEM;
			}

			//마지막 "#>"를 없앤다.
			int len = (int)strContentsInfo.length();
			lstrcpy(szTemp, strContentsInfo.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenUpList->szContentsInfo, szTemp);
			//printf("컨텐츠 조합: %s\n", pOpenUpList->szContentsInfo);

			//폴더 종류 조합
			for(int i=0; i<(int)v_strFolderName.size(); i++)
			{
				strTemp = v_strFolderName[i];
				strFolderName += strTemp + SEP_FIELD;
			}

			//마지막 "#<"를 없앤다.
			len = (int)strFolderName.length();
			lstrcpy(szTemp, strFolderName.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenUpList->szFolderName, szTemp);
			//printf("폴더 조합: %s\n", pOpenUpList->szFolderName);
		}

		pOpenUpList->nResult = OPENUP_RESULT_OK;	

		pClient->send_packet_queue(OPEN_UPLIST, (char*)pOpenUpList, sizeof(OPENUPLIST));
	}

	catch(int nCode)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadPre()━Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pOpenUpList->nResult = nCode;			
		pClient->send_packet_queue(OPEN_UPLIST, (char*)pOpenUpList, sizeof(OPENUPLIST));
	}
	catch(...)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadPre()━Catch(...)\n"))); 

		pOpenUpList->nResult = OPENUP_RESULT_ERROR;			
		pClient->send_packet_queue(OPEN_UPLIST, (char*)pOpenUpList, sizeof(OPENUPLIST));
	}
}


void DBCtrl::OpenUpLoad(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	
	char		szContentsType[20];		//20080528 컨텐츠 종류, jyh
	char		szSaveName[512];
	char		szSaveCopyName[512];
	char		szTemp[512];
	char		szServerIP[20];
	char		szServerID[20];
	char		szServerPW[20];
	int			nServerPT;
	int			nVolume = 0;	//20071126 파일 서버 번호, jyh
	
	POPENUPLOAD	pOpenUpload;

	try
	{
		pOpenUpload = (POPENUPLOAD)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		sprintf_s(m_pQuery1, 4096, "Select ftp, realname, bbs_table From %s Where no = %I64d", TABLE_OPEN_DATA, pOpenUpload->nFileIdx);
		//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENUP_RESULT_NOTFOUND;
		}

		nVolume		= atoi(pRow[0]);
		lstrcpy(szSaveName, pRow[1]);
		lstrcpy(szSaveCopyName, pRow[1]);
		lstrcpy(szContentsType, pRow[2]);
		SAFE_DELETEDB(pResult);

		//printf("Volume: %d, realname: %s, bbs_table: %s\n", nVolume, szSaveName, szContentsType);

		sprintf_s(m_pQuery1, 4096, "Select ip, id, pw, port From %s Where ftp = %d And flag_use = 'Y'", TABLE_OPEN_SERVER, nVolume);
		//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENUP_RESULT_SERVICE_STOP;
		}

		lstrcpy(szServerIP, pRow[0]);
		lstrcpy(szServerID, pRow[1]);
		lstrcpy(szServerPW, pRow[2]);
		nServerPT = atoi(pRow[3]);
		SAFE_DELETEDB(pResult);

		//printf("ServerIP: %s, ServerID: %s, ServerPW: %s, ServerPT = %d\n", szServerIP, szServerID, szServerPW, nServerPT);

		// FTP서버에 연결이 가능한지 체크
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(szServerIP, nServerPT, szServerID, szServerPW)) 
			{
				//printf("ftp서버 연결 성공!\n");
				//20080528 오픈 자료실 컨텐츠 종류별 저장을 위해, jyh
				char szTempSaveName[1024];

				//20080609 자막 자료실의 파일을 제외한 다른 파일은 전송전에 같은 파일이 있는지 조사하고 있으면
				//copy_를 붙인다, jyh
				if(lstrcmp(szContentsType, "caption"))
				{
					while(1)
					{
						sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szContentsType, szSaveName);
						//printf("%s\n", szTempSaveName);

						if(pClient->m_pFtp->RemoteFileValid(szTempSaveName) > 0)
						{
							//printf("%s 파일 있음!\n", szSaveName);
							sprintf_s(szTemp, 512, "copy_%s", szSaveName);
							lstrcpy(szSaveName, szTemp);
							//printf("%s로 바꿈!\n", szSaveName);
						}
						else
							break;
					}
				}
				else
					sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szContentsType, szSaveName);

				//if(pClient->m_pFtp->QueryTransfer(szSaveName, 0, 0))
				if(pClient->m_pFtp->QueryTransfer(szTempSaveName, 0, 0))
				{
					lstrcpy(pOpenUpload->szServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
					//printf("ServerInfo: %s\n", pOpenUpload->szServerInfo);
					break;
				}
				else	
					throw OPENUP_RESULT_SERVICE_STOP;					
			}
			else
			{
				//printf("ftp서버 연결 실패!\n");
				//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋╋╋╋╋╋╋╋╋╋╋╋ %s : %s\n"), pServerIP, pServerID)); 	

				if(nProc >= 2)
					throw OPENUP_RESULT_SERVICE_STOP;
			}
		}

		//20080609 파일명이 변경 됐을때는 _opendata_file 테이블의 realname을 바꾼다, jyh
		if(lstrcmp(szSaveCopyName, szSaveName))
		{
			sprintf_s(m_pQuery1, 4096, "Update %s Set realname = '%s' Where no = %I64d", TABLE_OPEN_DATA, szSaveName, pOpenUpload->nFileIdx);
			//printf("%s\n", m_pQuery1);
			ExecuteQuery(m_pQuery1);
		}

		pOpenUpload->nResult = OPENUP_RESULT_OK;
		pClient->send_packet_queue(OPEN_UPLOAD, (char*)pOpenUpload, sizeof(OPENUPLOAD));		
	}
	catch(int nCode)
	{		
		pOpenUpload->nResult	= nCode;
		pClient->send_packet_queue(OPEN_UPLOAD, (char*)pOpenUpload, sizeof(OPENUPLOAD));
	}
	catch(...)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋UpLoadProcess()━Catch(...)\n"))); 		
		pOpenUpload->nResult	= OPENUP_RESULT_ERROR;
		pClient->send_packet_queue(OPEN_UPLOAD, (char*)pOpenUpload, sizeof(OPENUPLOAD));
	}
}

void DBCtrl::OpneUpLoadEnd(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS)
{
	MYSQL_RES	*pResult = 0;
	
	try
	{
		// 연결된 FTP 세션 종료
		pClient->m_pFtp->DisConnect();
		//printf("ftp 세션 종료\n");

		char* pData = new char[nPS];
		memcpy(pData, (char*)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE), nPS);
		//printf("nPs: %d, pData: %s\n", nPS, pData);
		__int64 n64FileIdx = _atoi64(pData);
		delete pData;

		sprintf_s(m_pQuery1, 4096, "Update %s Set upload_complete = 'Y' Where no = %I64d", TABLE_OPEN_DATA, n64FileIdx);
		//printf("%s\n", m_pQuery1);
		ExecuteQuery(m_pQuery1);
	}
	catch(...)
	{		
	}
}

//
//오픈 자료 다운로드 관련
//

void DBCtrl::OpenDownLoadList(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;


	char		szTableName[30];
	char		szbbs_table[30];
	char		szServerIP[20];
	char		szServerID[20];
	char		szServerPW[20];
	int			nServerPT;
	int			nVolume;

	POPENDOWNLIST	pOpenDownList;

	try
	{
		pOpenDownList = (POPENDOWNLIST)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		//컨텐츠 타입(0:필수 자료, 1:자막 자료, 2:A/S 자료)
		switch(pOpenDownList->nContentsType)
		{
		case 0:
			lstrcpy(szTableName, TABLE_OPEN_NECESSARY);
			lstrcpy(szbbs_table, "necessary");
			break;
		case 1:
			lstrcpy(szTableName, TABLE_OPEN_CAPTION);
			lstrcpy(szbbs_table, "caption");
			break;
		case 2:
			lstrcpy(szTableName, TABLE_OPEN_AS);
			lstrcpy(szbbs_table, "contents_as");
			break;
		}

		////다운로드 요청받은 컨텐츠의 사이즈를 알아온다.
		//sprintf(m_pQuery1, "Select file_1_size From %s Where no = %I64d", szTableName, pOpenDownList->nContentsIdx);

		//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		//{
		//	SAFE_DELETEDB(pResult);	
		//	throw OPENDOWN_RESULT_NOTFOUND;
		//}

		//pOpenDownList->nContentsSize = _atoi64(pRow[0]);	
		//SAFE_DELETEDB(pResult);	

		//각 파일의 세부 정보를 알아온다.(파일 인덱스, 폴더명, 파일명, 사이즈)
		sprintf_s(m_pQuery1, 4096, "Select no, foldername, realname, size, ftp From %s Where bbs_no = %I64d and bbs_table = '%s'", TABLE_OPEN_DATA, pOpenDownList->nContentsIdx, szbbs_table);

		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENDOWN_RESULT_NOTFOUND;
		}

		nVolume = atoi(pRow[4]);	//서버 번호

		std::vector<__int64>		v_n64FileIdx;		//파일 인덱스
		std::vector<__int64>		v_n64FileSize;		//파일 사이즈
		std::vector<std::string>	v_strFolderName;	//폴더 명
		std::vector<std::string>	v_strFileName;		//파일 명
		std::vector<int>			v_nFolderDepth;		//파일 depth
		std::string strTemp;
		
		//printf("fileIdx: %s, foldername: %s, filename: %s, size: %s\n", pRow[0], pRow[1], pRow[2], pRow[3]);

		v_n64FileIdx.push_back(_atoi64(pRow[0]));
		strTemp = pRow[1];
		v_strFolderName.push_back(strTemp);
		strTemp = pRow[2];
		v_strFileName.push_back(strTemp);
		v_n64FileSize.push_back(_atoi64(pRow[3]));
		v_nFolderDepth.push_back(1);	//첫번째는 무조건 1(폴더 종류)
		
		while((pRow = mysql_fetch_row(pResult)))
		{
			//printf("while문 진입\n");
			//printf("fileIdx: %s, foldername: %s, filename: %s, size: %s\n", pRow[0], pRow[1], pRow[2], pRow[3]);
			v_n64FileIdx.push_back(_atoi64(pRow[0]));
			strTemp = pRow[1];

			bool bSameFolder = false;
			int i;

			for(i=0; i<(int)v_strFolderName.size(); i++)
			{
				if(v_strFolderName[i] == strTemp)
				{
					bSameFolder = true;
					v_nFolderDepth.push_back(i+1);

					break;
				}
			}
			
			//같은 폴더가 없을때만 저장
			if(!bSameFolder)
			{
				v_nFolderDepth.push_back(i+1);
				v_strFolderName.push_back(strTemp);
			}

			strTemp = pRow[2];
			v_strFileName.push_back(strTemp);
			v_n64FileSize.push_back(_atoi64(pRow[3]));
		}

		SAFE_DELETEDB(pResult);

		//DB의 파일사이즈와 실제 ftp에 있는 파일사이즈가 같은지 비교후 다르면 실제 ftp에 있는 파일사이즈로 수정
		sprintf_s(m_pQuery1, 4096, "Select ip, id, pw, port From %s Where ftp = %d And flag_use = 'Y'", TABLE_OPEN_SERVER, nVolume);

		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENDOWN_RESULT_SERVICE_STOP;
		}

		lstrcpy(szServerIP, pRow[0]);
		lstrcpy(szServerID, pRow[1]);
		lstrcpy(szServerPW, pRow[2]);
		nServerPT = atoi(pRow[3]);
		SAFE_DELETEDB(pResult);

		// FTP서버에 연결이 가능한지 체크
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(szServerIP, nServerPT, szServerID, szServerPW)) 
			{
				//printf("ftp서버 연결 성공!\n");

				for(int i=0; i<(int)v_strFileName.size(); i++)
				{
					char szTempSaveName[1024];

					sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szbbs_table, v_strFileName[i].c_str());
					//printf("%s\n", szTempSaveName);
					//20080527 파일서버에 파일이 있는지 확인한다, jyh
					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(szTempSaveName);
					//ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(szSaveName);
					//printf("nServerFileSize: %I64d\n", nServerFileSize);

					if(nServerFileSize == -2)
						throw OPENDOWN_RESULT_NOTFOUND;
					//------------------------------------------------------------------------

					//사이즈가 다르면 ftp에 있는 파일 사이즈를 기준으로 DB값 수정
					if(nServerFileSize != v_n64FileSize[i])
					{
						v_n64FileSize[i] = nServerFileSize;

						sprintf_s(m_pQuery1, 4096, "Update %s Set size = %I64d Where no = %I64d", TABLE_OPEN_DATA, nServerFileSize, v_n64FileIdx[i]);
						//printf("%s\n", m_pQuery1);
						ExecuteQuery(m_pQuery1);
					}
				}

				break;
			}
			else
			{
				//printf("ftp서버 연결 실패!\n");
				if(nProc >= 2)
					throw OPENDOWN_RESULT_SERVICE_STOP;
			}
		}


		std::string strContentsInfo;
		std::string strFolderName;
		char	szTemp[2048];

		//폴더가 있는지 비교해서 컨텐츠 정보를 조합
		if(v_strFolderName[0] == "/")
		{
			//폴더가 없으면 파일명만 조합(인덱스#<파일명#<사이즈)
			sprintf_s(szTemp, 2048, "%I64d#<%s#<%I64d", v_n64FileIdx[0], v_strFileName[0].c_str(), v_n64FileSize[0]);
			lstrcpy(pOpenDownList->szContentsInfo, szTemp);
			//printf("컨텐츠 조합: %s\n", pOpenDownList->szContentsInfo);
			lstrcpy(pOpenDownList->szFolderName, v_strFolderName[0].c_str());
			//printf("폴더 이름: %s\n", pOpenDownList->szFolderName);
		}
		else
		{			
			//폴더가 있으면 폴더depth와 파일명을 합침.(인덱스#<폴더depth#<파일명#<사이즈#>인덱스#<폴더depth#<파일명#<사이즈......#>인덱스#<폴더depth#<파일명#<사이즈)
			for(int i=0; i<(int)v_n64FileIdx.size(); i++)
			{
				sprintf_s(szTemp, 2048, "%I64d#<%d#<%s#<%I64d", v_n64FileIdx[i], v_nFolderDepth[i], v_strFileName[i].c_str(), v_n64FileSize[i]);
				strTemp = szTemp;
				strContentsInfo += strTemp + SEP_ITEM;
			}

			//마지막 "#>"를 없앤다.
			int len = (int)strContentsInfo.length();
			lstrcpy(szTemp, strContentsInfo.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenDownList->szContentsInfo, szTemp);
			//printf("컨텐츠 조합: %s\n", pOpenDownList->szContentsInfo);

			//폴더 종류 조합
			for(int i=0; i<(int)v_strFolderName.size(); i++)
			{
				strTemp = v_strFolderName[i];
				strFolderName += strTemp + SEP_FIELD;
			}

			//마지막 "#<"를 없앤다.
			len = (int)strFolderName.length();
			lstrcpy(szTemp, strFolderName.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenDownList->szFolderName, szTemp);
			//printf("폴더 조합: %s\n", pOpenDownList->szFolderName);
		}
				
		pOpenDownList->nResult = OPENDOWN_RESULT_OK;
		
		pClient->send_packet_queue(OPEN_DOWNLIST, (char*)pOpenDownList, sizeof(OPENDOWNLIST));
	}
	catch(int nCode)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPre()━Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pOpenDownList->nResult = nCode;			
		pClient->send_packet_queue(OPEN_DOWNLIST, (char*)pOpenDownList, sizeof(OPENDOWNLIST));
	}
	catch(...)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPre()━Catch(...)\n"))); 

		pOpenDownList->nResult = OPENDOWN_RESULT_ERROR;			
		pClient->send_packet_queue(OPEN_DOWNLIST, (char*)pOpenDownList, sizeof(OPENDOWNLIST));
	}
}


void DBCtrl::OpenDownLoad(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	
	char		szContentsType[20];		//20080528 컨텐츠 종류, jyh
	char		szSaveName[512];
	char		szServerIP[20];
	char		szServerID[20];
	char		szServerPW[20];
	int			nServerPT;
	int			nVolume = 0;	//20071126 파일 서버 번호, jyh

	POPENDOWNLOAD	pOpenDownPro;

	try
	{
		pOpenDownPro = (POPENDOWNLOAD)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
		
		sprintf_s(m_pQuery1, 4096, "Select ftp, realname, bbs_table From %s Where no = %I64d", TABLE_OPEN_DATA, pOpenDownPro->nFileIdx);
		//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENDOWN_RESULT_NOTFOUND;
		}

		nVolume		= atoi(pRow[0]);
		lstrcpy(szSaveName, pRow[1]);
		lstrcpy(szContentsType, pRow[2]);
		SAFE_DELETEDB(pResult);

		//printf("Volume: %d, realname: %s, bbs_table: %s\n", nVolume, szSaveName, szContentsType);

		sprintf_s(m_pQuery1, 4096, "Select ip, id, pw, port From %s Where ftp = %d And flag_use = 'Y'", TABLE_OPEN_SERVER, nVolume);
		//20071126 파일 서버 번호(temp_volume)를 얻어온다, jyh
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENDOWN_RESULT_SERVICE_STOP;
		}

		lstrcpy(szServerIP, pRow[0]);
		lstrcpy(szServerID, pRow[1]);
		lstrcpy(szServerPW, pRow[2]);
		nServerPT = atoi(pRow[3]);
		SAFE_DELETEDB(pResult);

		//printf("ServerIP: %s, ServerID: %s, ServerPW: %s, ServerPT = %d\n", szServerIP, szServerID, szServerPW, nServerPT);

		// FTP서버에 연결이 가능한지 체크
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(szServerIP, nServerPT, szServerID, szServerPW)) 
			{
				//printf("ftp서버 연결 성공!\n");
				//20080528 오픈 자료실 컨텐츠 종류별 저장을 위해, jyh
				char szTempSaveName[1024];

				sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szContentsType, szSaveName);
				//printf("%s\n", szTempSaveName);
				//20080527 파일서버에 파일이 있는지 확인한다, jyh
				ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(szTempSaveName);
				//ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(szSaveName);
				//printf("nServerFileSize: %I64d\n", nServerFileSize);

				if(nServerFileSize == -2)
					throw OPENDOWN_RESULT_NOTFOUND;
				//------------------------------------------------------------------------

				//if(pClient->m_pFtp->QueryTransfer(szSaveName, pOpenDownPro->nDownStartPoint, 1))
				if(pClient->m_pFtp->QueryTransfer(szTempSaveName, pOpenDownPro->nDownStartPoint, 1))
				{
					lstrcpy(pOpenDownPro->szServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
					printf("ServerInfo: %s\n", pOpenDownPro->szServerInfo);
					break;
				}
				else	
					throw OPENDOWN_RESULT_SERVICE_STOP;					
			}
			else
			{
				//printf("ftp서버 연결 실패!\n");
				if(nProc >= 2)
					throw OPENDOWN_RESULT_SERVICE_STOP;
			}
		}	

		// 다운로드 진행중인 파일 정보
		pClient->m_nFileNo		= pOpenDownPro->nFileIdx;
		pClient->m_nFileSizeBegin = pOpenDownPro->nDownStartPoint;
		
		// 정상적으로 파일 다운로드 가능
		pOpenDownPro->nResult = OPENDOWN_RESULT_OK;			
		pClient->send_packet_queue(OPEN_DOWNLOAD, (char*)pOpenDownPro, sizeof(OPENDOWNLOAD));				
	}
	catch(int nCode)
	{
		pOpenDownPro->nResult = nCode;			
		pClient->send_packet_queue(OPEN_DOWNLOAD, (char*)pOpenDownPro, sizeof(OPENDOWNLOAD));
	}
	catch(...)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPro()━Catch(...)\n"))); 
		pOpenDownPro->nResult = OPENDOWN_RESULT_ERROR;			
		pClient->send_packet_queue(OPEN_DOWNLOAD, (char*)pOpenDownPro, sizeof(OPENDOWNLOAD));
	}
}

void DBCtrl::OpenDownLoadEnd(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	// 연결된 FTP 세션 종료
	pClient->m_pFtp->DisConnect();
	//printf("ftp 세션 종료\n");
}

