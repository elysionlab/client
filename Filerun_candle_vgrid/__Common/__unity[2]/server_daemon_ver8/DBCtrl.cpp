#include "config_patten.h"
#include <string>
#include "DBCtrl.h"
#include "Process_Manager.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"


extern SERVER_RUNTIME_INFO RunServerInfo;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 데이터베이스 기본 설정
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool DBCtrl::Init(char* host, char* db, char* id, char* pw)
{
	if(!BIC_Init(host, db, id, pw))
		return false;

	return true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 스토러지 정보 업데이트
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DBCtrl::UpdateDiskAverage()
{
	MYSQL_RES	*pResult_Server;
	MYSQL_RES	*pResult_Sub;
	MYSQL_RES	*pResult_Sub2;
	MYSQL_ROW	pRow_Server;
	MYSQL_ROW	pRow_Sub;
	MYSQL_ROW	pRow_Sub2;
	int			num;
	
	ACE_SOCK_Connector	pConnector;
	ACE_SOCK_Stream		pPeer;	
	ACE_Time_Value		nTimeOut(5);
	ACE_Time_Value		nTimeOutTrans(3);

	char		cDatabuf_Recv[100];
	char		cDatabuf_Send[100];

	char		cDatabuf_AVG[100];

	int			nCountUp, nCountDown;

	

	// 스토로지 서버목록 생성
	sprintf_s(m_pQuery1, 4096, "Select ftp, ip, port From %s Where flag_use = 'Y' Order By ftp Asc", TABLE_COMMON_FTP_SERVER);
	if((pResult_Server = m_pFileDB->ExecuteQueryR(m_pQuery1, nCount)) == 0 || nCount == 0)
	{
		SAFE_DELETEDB(pResult_Server);		
		return;
	}
	
	
	// 각각의 서버로 연결해서 디스크 에버러지를 받아온다.
	while(pRow_Server = mysql_fetch_row(pResult_Server))
	{
		ZeroMemory(cDatabuf_Send, sizeof(cDatabuf_Send));
		ZeroMemory(cDatabuf_Recv, sizeof(cDatabuf_Recv));

		ACE_INET_Addr pAddr(SERVER_PORT_STORAGE, (const char*)pRow_Server[1]);

		nCountDown	= 0;
		nCountUp	= 0;
		
		if(pConnector.connect(pPeer, pAddr, &nTimeOut) == -1)
		{
			// 스토로지 서버 연결에 실패했을 경우
			sprintf_s(m_pQuery1, 4096, "Update %s Set flag_state = 'N' Where ftp = %s", TABLE_COMMON_FTP_SERVER, pRow_Server[0]);
			m_pFileDB->ExecuteQuery(m_pQuery1);
			continue;
		}
		else
		{	
			// 해당 서버의 에버러지 체크
			strcpy(cDatabuf_AVG, "AVG@AVG@\r\n");
			if((num = pPeer.send_n((const char*)cDatabuf_AVG, strlen(cDatabuf_AVG), &nTimeOutTrans)) == -1)
			{
				strcpy(cDatabuf_AVG, "EXIT@EXIT@\r\n");
				pPeer.send_n((const char*)cDatabuf_AVG, strlen(cDatabuf_AVG), &nTimeOutTrans);
				pPeer.close();
				continue;
			}
			if((num = pPeer.recv(cDatabuf_AVG, sizeof(cDatabuf_AVG), &nTimeOutTrans)) == -1)
			{
				strcpy(cDatabuf_AVG, "EXIT@EXIT@\r\n");
				pPeer.send_n((const char*)cDatabuf_AVG, strlen(cDatabuf_AVG), &nTimeOutTrans);
				pPeer.close();
				continue;
			}

			
			// 해당 서버의 볼륨별 사이즈 정보
			sprintf_s(m_pQuery2, 4096, "Select volume, volumepath From %s Where ftp = %s", TABLE_COMMON_FTP_VOLUME, pRow_Server[0]);
			if((pResult_Sub = m_pFileDB->ExecuteQueryR(m_pQuery2, nCount)) != 0 && nCount != 0)
			{
				while(pRow_Sub = mysql_fetch_row(pResult_Sub))
				{
					sprintf_s(cDatabuf_Send, 100, "SIZE@%s@\r\n", pRow_Sub[1]);
					if((num = pPeer.send_n((const char*)cDatabuf_Send, strlen(cDatabuf_Send), &nTimeOutTrans)) == -1)
						continue;
					if((num = pPeer.recv(cDatabuf_Recv, sizeof(cDatabuf_Recv), &nTimeOutTrans)) == -1)
						continue;

					sprintf_s(m_pQuery3, 4096, "Update %s Set freesize = %I64d Where volume = %s", TABLE_COMMON_FTP_VOLUME, _atoi64(cDatabuf_Recv), pRow_Sub[0]);
					m_pFileDB->ExecuteQuery(m_pQuery3);



					// 해당 볼륨의 계정별 정보
					sprintf_s(m_pQuery3, 4096, "Select account, id From %s Where ftp_volume = %s", TABLE_COMMON_FTP_ACCOUNT, pRow_Sub[0]);
					if((pResult_Sub2 = m_pFileDB->ExecuteQueryR(m_pQuery3, nCount)) != 0 && nCount != 0)
					{
						char cData_Up[100];
						char cData_Down[100];

						while(pRow_Sub2 = mysql_fetch_row(pResult_Sub2))
						{
							sprintf_s(cDatabuf_Send, 100, "UP@%s@\r\n", pRow_Sub2[1]);
							if((num = pPeer.send_n((const char*)cDatabuf_Send, strlen(cDatabuf_Send), &nTimeOutTrans)) == -1)
								continue;
							if((num = pPeer.recv(cData_Up, sizeof(cData_Up), &nTimeOutTrans)) == -1)
								continue;

							sprintf_s(cDatabuf_Send, 100, "DOWN@%s@\r\n", pRow_Sub2[1]);
							if((num = pPeer.send_n((const char*)cDatabuf_Send, strlen(cDatabuf_Send), &nTimeOutTrans)) == -1)
								continue;
							if((num = pPeer.recv(cData_Down, sizeof(cData_Down), &nTimeOutTrans)) == -1)
								continue;

							sprintf_s(m_pQuery3, 4096, "Update %s Set count_up=%d,count_down=%d Where account=%s", 
												TABLE_COMMON_FTP_ACCOUNT, atoi(cData_Up), atoi(cData_Down), pRow_Sub2[0]);
							m_pFileDB->ExecuteQuery(m_pQuery3);

							nCountDown	+= atoi(cData_Down);
							nCountUp	+= atoi(cData_Up);
						}
					}
					SAFE_DELETEDB(pResult_Sub2);		
				}
			}
			SAFE_DELETEDB(pResult_Sub);		
			

			strcpy(cDatabuf_Send, "EXIT@EXIT@\r\n");
			pPeer.send_n((const char*)cDatabuf_Send, strlen(cDatabuf_Send), &nTimeOutTrans);
			pPeer.close();		


			// 에버러지 정보 업데이트
			if(atof(cDatabuf_AVG) == 0 && (nCountDown != 0 || nCountUp != 0))
				continue;

			sprintf_s(m_pQuery3, 4096, "Update %s Set flag_state = 'Y', loadavg = %.02f Where ftp = %s", 
								TABLE_COMMON_FTP_SERVER, atof(cDatabuf_AVG), pRow_Server[0]);
			m_pFileDB->ExecuteQuery(m_pQuery3);
		}
	}

	SAFE_DELETEDB(pResult_Server);		
}






// 파일삭제처리
void DBCtrl::FileRemove()
{
	MYSQL_RES	*pResult;
	MYSQL_ROW	pRow;
	MYSQL_RES	*pResult_Sub;
	MYSQL_ROW	pRow_Sub;
	MYSQL_RES	*pResult_Sub2;
	MYSQL_ROW	pRow_Sub2;

	__int64		nCommonFileIdx;
	int			nCountRef;

	while(1)
	{
		sprintf_s(m_pQuery1, 4096, "Select idx, ftp_volume, filename, md5sum, size, flag_upload, flag_realupload From %s Order By idx Asc Limit 100", 
							TABLE_FILE_REMOVE);
		if((pResult = this->ExecuteQueryR(m_pQuery1, nCount)) == 0 || nCount == 0)
		{
			SAFE_DELETEDB(pResult);		
			return;
		}
		    
		while(pRow = mysql_fetch_row(pResult))
		{
			// 임시 파일만 삭제
			if(OPTION_USE_FILEUNITY == 1)
			{
				if(!strcmp(pRow[5], "N") && !strcmp(pRow[6], "Y"))
					RemoveFileofFTP(_atoi64(pRow[1]), pRow[2]);
			}

			// 참조 되지 않은 파일 삭제
			else
			{
				// 1. 업로드 미완료 & 실제업로드
				if(!strcmp(pRow[5], "N") && !strcmp(pRow[6], "Y"))
				{
					RemoveFileofFTP(_atoi64(pRow[1]), pRow[2]);
				}
				// 2. 업로드 미완료 & 트릭 업로드
				// 3. 업로드 완료 & 트릭 업로드 
				// 4. 업로드 완료 & 실제 업로드
				else if((!strcmp(pRow[5], "N") && !strcmp(pRow[6], "N")) ||
						(!strcmp(pRow[5], "Y") && !strcmp(pRow[6], "N")) ||
						(!strcmp(pRow[5], "Y") && !strcmp(pRow[6], "Y")))
				{
					// 참조카운트
					sprintf_s(m_pQuery2, 4096, "Select Count(*) From %s Where temp_name = '%s' " \
										"Union All Select Count(*) From %s Where temp_name = '%s'", 
										TABLE_FILE_BBS, pRow[2], TABLE_FILE_BLOG, pRow[2]);
					if((pResult_Sub2 = this->ExecuteQueryR(m_pQuery2)) != 0)
					{
						nCountRef = 0;

						if((pRow_Sub2 = mysql_fetch_row(pResult_Sub2)) == 0)
							continue;
						else
							nCountRef += atoi(pRow_Sub2[0]);

						if((pRow_Sub2 = mysql_fetch_row(pResult_Sub2)) == 0)
							continue;
						else
							nCountRef += atoi(pRow_Sub2[0]);

						if(nCountRef == 0)
						{
							//sprintf(m_pFileDB->m_pQuery2,  "Select fileidx From %s Where filename = '%s'",  TABLE_COMMON_FILES, pRow[2]);
							//if((pResult_Sub = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery2, m_pFileDB->nCount)) != 0 && 
							//	(pRow_Sub = mysql_fetch_row(pResult_Sub)) != 0)
							//{
							//	nCommonFileIdx = _atoi64(pRow_Sub[0]);
							//	SAFE_DELETEDB(pResult_Sub);

								//// 분배된 파일 삭제
								//sprintf(m_pFileDB->m_pQuery2,  "Select ftp_volume From %s Where file_idx = %I64d And flag_original = 'N'", 
								//					TABLE_COMMON_FILE_DIVISION, nCommonFileIdx);
								//if((pResult_Sub = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery2, m_pFileDB->nCount)) != 0)
								//{
								//	while(pRow_Sub = mysql_fetch_row(pResult_Sub))
								//		RemoveFileofFTP(_atoi64(pRow_Sub[0]), pRow[2]);
								//}							    	
								//SAFE_DELETEDB(pResult_Sub);	

								
								//// 공용파일 테이블 레코드 정리
								//sprintf(m_pFileDB->m_pQuery2, "Delete From %s Where fileidx = %I64d", TABLE_COMMON_FILES, nCommonFileIdx);
								//m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2);

								//// 분산파일 테이블 레코드 정리
								//sprintf(m_pFileDB->m_pQuery2, "Delete From %s Where file_idx = %I64d", TABLE_COMMON_FILE_DIVISION, nCommonFileIdx);
								//m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2);
							//}			
							//SAFE_DELETEDB(pResult_Sub);

							// 파일 삭제
							RemoveFileofFTP(_atoi64(pRow[1]), pRow[2]);
						}
					}

					SAFE_DELETEDB(pResult_Sub2);	
				}
			}

			// 삭제 테이블 레코드 삭제
			sprintf_s(m_pQuery2, 4096, "Delete From %s Where idx = %s", TABLE_FILE_REMOVE, pRow[0]);
			this->ExecuteQuery(m_pQuery2);
		}

		SAFE_DELETEDB(pResult);		
	}
}

/*
부하 파일 체크
1. 현재 분할된 파일의 유효성 검사(OPTION_FILE_UNITY_COUNT)
2. 체크 다운로드 수 초기화
3. 새롭게 추가될 분할 파일 체크(OPTION_FILE_UNITY_SERVER)
*/
void DBCtrl::FileDivision()
{
	DivisionVailed();

	NewDivision();

	sprintf_s(m_pQuery1, 4096, "Update %s Set count_criterion = 0", TABLE_COMMON_FILES);
	m_pFileDB->ExecuteQuery(m_pQuery1);	
}


void DBCtrl::NewDivision()
{
	MYSQL_RES	*pResult;
	MYSQL_ROW	pRow;

	char		pServerIP_Server[20];
	char		pServerID_Server[20];
	char		pServerPW_Server[20];
	int			nServerPT_Server;

	char		pServerIP_Client[20];
	char		pServerID_Client[20];
	char		pServerPW_Client[20];
	int			nServerPT_Client;

	int			nFtpCount;
	int			nFtpTransCount = 0;


	sprintf_s(m_pFileDB->m_pQuery1, 4096, "Select fileidx, filename, ftp_volume From %s Where division = 'N' And count_criterion > %d", 
									TABLE_COMMON_FILES, RunServerInfo.nDivisionCount);
	if((pResult = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery1, m_pFileDB->nCount)) == 0 || m_pFileDB->nCount == 0)
	{
		SAFE_DELETEDB(pResult);		
		return;
	}
    	
	while(pRow = mysql_fetch_row(pResult))
	{
		nFtpTransCount = 0;

		// 파일 존재 유무 체크
		if(LoadFtpInfo(_atoi64(pRow[2]), pServerIP_Server, nServerPT_Server, pServerID_Server, pServerPW_Server))
		{
			if(m_pFtp_Server->Connect(pServerIP_Server, nServerPT_Server, pServerID_Server, pServerPW_Server))
			{
				if(m_pFtp_Server->RemoteFileValid(pRow[1]) > 1048576)
				{
					m_pFtp_Server->DisConnect();

					nFtpCount = LoadFtpAcco(_atoi64(pRow[2]));

					for(int nIndex = 0; nIndex < nFtpCount; nIndex++)
					{
						// 해당 서버에 본 파일이 존재 하는지 체크
						if(LoadFtpInfo(m_nFtpIdxList[nIndex], pServerIP_Client, nServerPT_Client, pServerID_Client, pServerPW_Client))
						{
							if( m_pFtp_Client->Connect(pServerIP_Client, nServerPT_Client, pServerID_Client, pServerPW_Client) &&					
								m_pFtp_Server->Connect(pServerIP_Server, nServerPT_Server, pServerID_Server, pServerPW_Server))
							{
								if(m_pFtp_Server->FXP_Server(pRow[1]) && m_pFtp_Client->FXP_Client(pRow[1], m_pFtp_Server->m_sPassiveInfo))
								{
									if(m_pFtp_Server->FXP_Result())
									{
										m_pFtp_Server->DisConnect();
										m_pFtp_Client->DisConnect();

										if( m_pFtp_Client->Connect(pServerIP_Client, nServerPT_Client, pServerID_Client, pServerPW_Client) &&					
											m_pFtp_Server->Connect(pServerIP_Server, nServerPT_Server, pServerID_Server, pServerPW_Server))
										{
											if(m_pFtp_Server->RemoteFileValid(pRow[1]) == m_pFtp_Client->RemoteFileValid(pRow[1]))
											{
												nFtpTransCount++;
												sprintf_s(m_pFileDB->m_pQuery2, 4096, "Insert Into %s (file_idx, ftp_volume, flag_original) " \
																				"Values (%s, %I64d, 'N')",TABLE_COMMON_FILE_DIVISION, pRow[0], m_nFtpIdxList[nIndex]);
												m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2);
											}
										}
									}
								}
							}
						}
					}

					m_pFtp_Server->DisConnect();
					m_pFtp_Client->DisConnect();
				}
				else m_pFtp_Server->DisConnect();
			}			
		}

		if(nFtpTransCount > 0)
		{
			sprintf_s(m_pFileDB->m_pQuery2, 4096, "Insert Into %s (file_idx, ftp_volume, flag_original) Values (%s, %s, 'Y')",
											TABLE_COMMON_FILE_DIVISION, pRow[0], pRow[2]);
			m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2);

			sprintf_s(m_pFileDB->m_pQuery2, 4096, "Update %s Set division = 'Y', count_criterion = 0 Where fileidx = %s", TABLE_COMMON_FILES, pRow[0]);
			m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2);
		}
	}

	SAFE_DELETEDB(pResult);		
}


void DBCtrl::DivisionVailed()
{
	MYSQL_RES	*pResult;
	MYSQL_RES	*pResult_Sub;
	MYSQL_ROW	pRow;
	MYSQL_ROW	pRow_Sub;


	sprintf_s(m_pFileDB->m_pQuery1, 4096, "Select fileidx, filename From %s Where division = 'Y' And count_criterion = 0", TABLE_COMMON_FILES);
	if((pResult = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery1, m_pFileDB->nCount)) == 0 || m_pFileDB->nCount == 0)
	{
		SAFE_DELETEDB(pResult);		
		return;
	}
    	
	while(pRow = mysql_fetch_row(pResult))
	{
		sprintf_s(m_pFileDB->m_pQuery2, 4096, "Update %s Set division = 'N', count_criterion = 0 Where fileidx = %s", TABLE_COMMON_FILES, pRow[0]);
		if(m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2) == QUERY_COMPLETE)
		{
			sprintf_s(m_pFileDB->m_pQuery2, 4096, "Select idx, ftp_volume From %s Where file_idx = %s And flag_original = 'N'", 
											TABLE_COMMON_FILE_DIVISION, pRow[0]);
			if((pResult_Sub = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery2, m_pFileDB->nCount)) == 0)
				continue;
		    	
			while(pRow_Sub = mysql_fetch_row(pResult_Sub))
				RemoveFileofFTP(_atoi64(pRow_Sub[1]), pRow[1]);

			// 임시이동파일의 삭제실패(다운로드중)일때를 대비 모두 제거
			sprintf_s(m_pFileDB->m_pQuery2, 4096, "Delete From %s Where file_idx = %s", TABLE_COMMON_FILE_DIVISION, pRow[0]);
			m_pFileDB->ExecuteQuery(m_pFileDB->m_pQuery2);

			SAFE_DELETEDB(pResult_Sub);				
		}
	}

	SAFE_DELETEDB(pResult);		
}

bool DBCtrl::LoadFtpInfo(ACE_UINT64 nVolumeIdx, char* pServerIP, int &nServerPORT, char* pServerID, char* pServerPW)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	try
	{
		sprintf_s(m_pFileDB->m_pQuery1, 4096, "Select f_server.port, f_server.ip, f_account.passwd, f_account.id " \
							"From %s f_volume, %s f_account, %s f_server Where " \
							"f_volume.volume = %I64d And f_volume.ftp = f_server.ftp And " \
							"f_volume.volume = f_account.ftp_volume And f_account.speed = 100",  
							TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_ACCOUNT, TABLE_COMMON_FTP_SERVER, nVolumeIdx);
		if((pResult = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery1, m_pFileDB->nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw UPLOAD_RESULT_ERROR;
		}

		nServerPORT = atoi(pRow[0]);
		strcpy(pServerIP, pRow[1]);
		strcpy(pServerPW, pRow[2]);
		strcpy(pServerID, pRow[3]);
		
		SAFE_DELETEDB(pResult);	

		return true;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Common━%D━LoadFtpInfo()\n"))); 
	}

	return false;
}



// 이동할 FTP 서버 할당
int	DBCtrl::LoadFtpAcco(ACE_UINT64 nVolumeIdx)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	int			nResult = 0;

	try
	{
		sprintf_s(m_pFileDB->m_pQuery1, 4096, "Select f_volume.volume From %s f_volume, %s f_server Where " \
										"f_volume.ftp = f_server.ftp And f_volume.flag_upload = 'Y' " \
										"And f_server.flag_state = 'Y' And f_server.flag_use = 'Y' And f_volume.volume != %I64d " \
										"Order By f_server.loadavg Asc, f_volume.freesize Desc limit %d",  
										TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_SERVER, nVolumeIdx, RunServerInfo.nDivisionServer - 1);
		if((pResult = m_pFileDB->ExecuteQueryR(m_pFileDB->m_pQuery1, m_pFileDB->nCount)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			return nResult;
		}

		while(pRow = mysql_fetch_row(pResult))
		{
			m_nFtpIdxList[nResult] = _atoi64(pRow[0]);
			nResult++;
		}

		SAFE_DELETEDB(pResult);	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Common━%D━LoadFtpInfo()\n"))); 
	}

	return nResult;
}



// FTP 서버에서 파일 삭제
bool DBCtrl::RemoveFileofFTP(ACE_UINT64 nVolumeIdx, char* pFileName)
{
	char		pServerIP[20];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	bool		bResult = false;

	if(LoadFtpInfo(nVolumeIdx, pServerIP, nServerPT, pServerID, pServerPW))
	{
		if(m_pFtp_Server->Connect(pServerIP, nServerPT, pServerID, pServerPW))
		{
			m_pFtp_Server->RemoveFile(pFileName);
			bResult = true;
		}
	}

	return bResult;
}

