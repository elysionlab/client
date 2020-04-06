#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


// ���� ���� ���� üũ
bool DBCtrl::CheckService()
{
	DBbase*		pDataBase = NULL;

	try
	{
		ACE_Time_Value	nCurTime = ACE_OS::gettimeofday();

		if((nCurTime.sec() - m_tCheckTime.sec()) < 10)
			return m_bServiceRun;

		sprintf(m_pQuery1, "Select ftp From %s Where flag_use = 'Y' And flag_state = 'Y'", TABLE_COMMON_FTP_SERVER);	
		//printf("���� ���� üũ\n");//test
		//printf("%s\n", m_pQuery1); //test
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		
		if(QUERY_COMPLETE != pDataBase->ExecuteQuery(m_pQuery1, nCount) || nCount == 0)
		{
			//printf("���� ������:%d\n", nCount);//test
			m_bServiceRun = false;
		}
		else
		{
			//printf("���� ������:%d\n", nCount);//test
			m_bServiceRun = true;
		}

		m_tCheckTime = ACE_OS::gettimeofday();
	}
	catch(...)
	{
	}
	
	return m_bServiceRun;
}

// ���� �ߺ�üũ �� üũ -> 20080123 ������ ������ ����, jyh
int DBCtrl::LoadFileCountCore(char* pSaveName)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	DBbase*		pDataBase = NULL;

	int			nResult = 0;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		// ���� ������ ���ε� �Ϸ�� ������ �����ϴ��� üũ 
		sprintf(m_pQuery1,  "Select count(*) From %s Where filename='%s'", TABLE_COMMON_FILES, pSaveName);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw 0;
		}
		nResult = atoi(pRow[0]);		
		SAFE_DELETEDB(pResult);	
	}
	catch(...)
	{
	}

	return nResult;
}

//20080123 ������ ������ ����, jyh
int DBCtrl::LoadFileCountCp(char* pCheckSum, __int64 nFileSize)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;

	int			nCountRef = 0;
	// ����ī��Ʈ
	/*sprintf(m_pQuery2,  "Select Count(*) From %s Where md5sum='%s' and size=%I64d and contents_complete = 1 " \
		"Union All Select Count(*) From %s Where md5sum='%s' and size=%I64d and contents_complete = 1", 
		TABLE_FILE_BBS, pCheckSum, nFileSize, TABLE_FILE_BLOG, pCheckSum, nFileSize);*/
	//20080911 ��α� ���̺� ����, jyh
	sprintf(m_pQuery2,  "Select Count(*) From %s Where md5sum='%s' and size=%I64d and contents_complete = 1",
		TABLE_FILE_BBS, pCheckSum, nFileSize);
	if((pResult = this->ExecuteQueryR(m_pQuery2)) != 0)
	{
		nCountRef = 0;

		if((pRow = mysql_fetch_row(pResult)) != 0)
			nCountRef += atoi(pRow[0]);

		/*if((pRow = mysql_fetch_row(pResult)) != 0)
			nCountRef += atoi(pRow[0]);*/

		SAFE_DELETEDB(pResult);	
	}

	return nCountRef;
}
//int DBCtrl::LoadFileCountCp(char* pCheckSum, __int64 nFileSize)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//
//	int			nCountRef = 0;
//	// ����ī��Ʈ
//	sprintf(m_pQuery2,  "Select Count(*) From %s Where md5sum='%s' and size=%I64d " \
//						"Union All Select Count(*) From %s Where md5sum='%s' and size=%I64d", 
//						TABLE_FILE_BBS, pCheckSum, nFileSize, TABLE_FILE_BLOG, pCheckSum, nFileSize);
//	if((pResult = this->ExecuteQueryR(m_pQuery2)) != 0)
//	{
//		nCountRef = 0;
//
//		if((pRow = mysql_fetch_row(pResult)) != 0)
//			nCountRef += atoi(pRow[0]);
//
//		if((pRow = mysql_fetch_row(pResult)) != 0)
//			nCountRef += atoi(pRow[0]);
//
//		SAFE_DELETEDB(pResult);	
//	}
//	return nCountRef;
//}

//20080124 ���� �ߺ�üũ ����, jyh
__int64 DBCtrl::LoadNewFileCheck(char* pSaveName, __int64 nFileSize, __int64 nVolumeIdx, bool bTrick)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	DBbase*		pDataBase = NULL;

	__int64		nVolume = -1;
	__int64		nFileIdx = 0;

	char		pServerIP[80];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		// ���� ������ ���ε� �Ϸ�� ������ �����ϴ��� üũ 
		/*sprintf(m_pQuery1,  "Select fileidx, ftp_volume From %s Where filename = '%s'", TABLE_COMMON_FILES, pSaveName);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw -1;
		}

		nFileIdx = _atoi64(pRow[0]);		
		nVolume = _atoi64(pRow[1]);		
		SAFE_DELETEDB(pResult);	*/

		//printf("LoadNewFileCheck():���� �ߺ�üũ\n");//test
		
		nVolume = nVolumeIdx;

		sprintf(m_pQuery1,  "Select no From %s Where temp_name = '%s' and temp_volume = %I64d and size = %I64d", TABLE_FILE_BBS, pSaveName, nVolume, nFileSize);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			return -1;
		}

		SAFE_DELETEDB(pResult);	

		if(bTrick)
		{
			LoadFtpInfoUpload(nVolume, 0, pServerIP, nServerPT, pServerID, pServerPW);
			
			for(int nProc = 0; nProc < 10; nProc++)
			{
				if(m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
				{
					ACE_UINT64 nServerFileSize = m_pFtp->RemoteFileValid(pSaveName);

					if(nServerFileSize == -2 || (nServerFileSize >= 0 && nServerFileSize != nFileSize))
					{
						// ���� ����� �ٸ� ������ ����
						if(nServerFileSize > 0)
							m_pFtp->RemoveFile(pSaveName);

						// �ھ�ڵ� ����
						/*sprintf(m_pQuery1,  "Delete From %s Where fileidx=%I64d", TABLE_COMMON_FILES, nFileIdx);
						pDataBase->ExecuteQuery(m_pQuery1);*/
						m_pFtp->DisConnect();		
						return -1;
					}
					else if(nServerFileSize == nFileSize)
					{
						m_pFtp->DisConnect();		
						return nVolume;
					}
					else if(nServerFileSize == -1)
					{
						nProc = 0;
						m_pFtp->DisConnect();
						continue;
					}															
					else
					{
						m_pFtp->DisConnect();
						break;
					}
				}
				//20081027 ftp ���� ����, jyh
				else
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("��LoadNewFileCheck()��ftp ���� ���� : ftp_volume : %I64d, ftp_IP : %s\n"), nVolume, pServerIP));
					throw UPLOAD_RESULT_FTP_CONFAILED;
				}
			}
		}
		else
		{
			// ���� ������ ��Ȯ�� ������� ���� �ϴ��� üũ
			if(LoadFtpInfoUpload(nVolume, 0, pServerIP, nServerPT, pServerID, pServerPW))
			{
				for(int nProc = 0; nProc < 10; nProc++)
				{
					if(m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
					{
						ACE_UINT64 nServerFileSize = m_pFtp->RemoteFileValid(pSaveName);

						if(nServerFileSize == -2 || (nServerFileSize >= 0 && nServerFileSize != nFileSize))
						{
							// ���� ����� �ٸ� ������ ����
							if(nServerFileSize > 0)
								m_pFtp->RemoveFile(pSaveName);

							// �ھ�ڵ� ����
							/*sprintf(m_pQuery1,  "Delete From %s Where fileidx=%I64d", TABLE_COMMON_FILES, nFileIdx);
							pDataBase->ExecuteQuery(m_pQuery1);*/
							m_pFtp->DisConnect();		
							return -1;
						}
						else if(nServerFileSize == nFileSize)
						{
							m_pFtp->DisConnect();		
							return nVolume;
						}
						else if(nServerFileSize == -1)
						{
							nProc = 0;
							m_pFtp->DisConnect();
							continue;
						}															
						else
						{
							m_pFtp->DisConnect();
							break;
						}
					}
					//20081027 ftp ���� ����, jyh
					else
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("��LoadNewFileCheck()��ftp ���� ���� : ftp_volume : %I64d, ftp_IP : %s\n"), nVolume, pServerIP));
						throw UPLOAD_RESULT_FTP_CONFAILED;
					}
				}		
			}
		}
				
	}
	catch(...)
	{
	}

	//return nVolume;
	return -1;
}
//__int64 DBCtrl::LoadNewFileCheck(char* pSaveName, __int64 nFileSize)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//	DBbase*		pDataBase = NULL;
//
//	__int64		nVolume = -1;
//	__int64		nFileIdx = 0;
//
//	char		pServerIP[20];
//	char		pServerID[20];
//	char		pServerPW[20];
//	int			nServerPT;
//
//	try
//	{
//		if(OPTION_USE_FILEUNITY == 1) 
//			pDataBase = m_pFileDB;
//		else
//			pDataBase = this;
//
//		// ���� ������ ���ε� �Ϸ�� ������ �����ϴ��� üũ 
//		sprintf(m_pQuery1,  "Select fileidx, ftp_volume From %s Where filename = '%s'", TABLE_COMMON_FILES, pSaveName);
//		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//		{
//			SAFE_DELETEDB(pResult);	
//			throw -1;
//		}
//		
//		nFileIdx = _atoi64(pRow[0]);		
//		nVolume = _atoi64(pRow[1]);		
//		SAFE_DELETEDB(pResult);	
//
//		
//		// ���� ������ ��Ȯ�� ������� ���� �ϴ��� üũ
//		if(LoadFtpInfoUpload(nVolume, 0, pServerIP, nServerPT, pServerID, pServerPW))
//		{
//			for(int nProc = 0; nProc < 10; nProc++)
//			{
//				if(m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW))
//				{
//					ACE_UINT64 nServerFileSize = m_pFtp->RemoteFileValid(pSaveName);
//
//					if(nServerFileSize == -2 || (nServerFileSize >= 0 && nServerFileSize != nFileSize))
//					{
//						// ���� ����� �ٸ� ������ ����
//						if(nServerFileSize > 0)
//							m_pFtp->RemoveFile(pSaveName);
//
//						// �ھ�ڵ� ����
//						sprintf(m_pQuery1,  "Delete From %s Where fileidx=%I64d", TABLE_COMMON_FILES, nFileIdx);
//						pDataBase->ExecuteQuery(m_pQuery1);
//						m_pFtp->DisConnect();		
//						return -1;
//					}
//					else if(nServerFileSize == nFileSize)
//					{
//						m_pFtp->DisConnect();		
//						return nVolume;
//					}
//					else if(nServerFileSize == -1)
//					{
//						nProc = 0;
//						m_pFtp->DisConnect();
//						continue;
//					}															
//					else
//					{
//						m_pFtp->DisConnect();
//						break;
//					}
//				}
//			}		
//		}		
//	}
//	catch(...)
//	{
//	}
//
//	return nVolume;
//}


// ���ο� ���� ���
int DBCtrl::SetNewFileComplete(char* pSaveName, __int64 nFileSize, __int64 nVolume)
{
	DBbase*		pDataBase = NULL;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		sprintf(m_pQuery1,  "Insert Into %s (ftp_volume, filename, regdate) Values(%I64d, '%s', UNIX_TIMESTAMP())",  TABLE_COMMON_FILES, nVolume, pSaveName);
		pDataBase->ExecuteQuery(m_pQuery1);
		/*
		if(LoadNewFileCheck(pSaveName, nFileSize) == -1)
		{
			sprintf(m_pQuery1,  "Insert Into %s (ftp_volume, filename, regdate) Values(%I64d, '%s', UNIX_TIMESTAMP())",  
								TABLE_COMMON_FILES, nVolume, pSaveName);
			if(pDataBase->ExecuteQuery(m_pQuery1) != QUERY_COMPLETE)
				return 0;
			else
				return 1;
		}
		else
		{
			return 1;
		}
		*/
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��SetNewFileComplete()\n"))); 
	}

	return 0;
}


// FTP ���� ���� ��û
__int64 DBCtrl::LoadFtpAllotment()
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	DBbase*		pDataBase = NULL;

	__int64	nResult = -1;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;


		//20080620 ������ ��� ���� �뷮�� 50GB�̻��� ���߿� �Ҵ�, jyh
		sprintf(m_pQuery1,  "Select f_volume.volume From %s f_volume, %s f_server Where " \
							"f_volume.ftp = f_server.ftp And f_volume.flag_upload = 'Y' " \
							"And f_server.flag_state = 'Y' And f_server.flag_use = 'Y' " \
							"And f_volume.freesize > 5120 Order By f_server.loadavg Asc, " \
							"f_volume.freesize Desc limit 1",  
							TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_SERVER);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw -1;
		}
		
		nResult = _atoi64(pRow[0]);		
		SAFE_DELETEDB(pResult);	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpAllotment()\n"))); 
	}

	return nResult;
}

// FTP ���� �Ҵ� ��û(���� �������� ���ε� ���� ť�� �������)
bool DBCtrl::LoadFtpAllotmentQueue(__int64 nVolume)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	DBbase*		pDataBase = NULL;

	bool bResult = false;
	__int64 nFreeSize = 0;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;


		sprintf(m_pQuery1,  "Select f_volume.freesize From %s f_volume, %s f_server Where " \
							"f_volume.volume=%I64d and f_volume.ftp = f_server.ftp And f_volume.flag_upload = 'Y' " \
							"And f_server.flag_state = 'Y' And f_server.flag_use = 'Y' ", 
							TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_SERVER, nVolume);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);				
		}
		else
		{
			nFreeSize = _atoi64(pRow[0]);		
			SAFE_DELETEDB(pResult);

			// ���� ������ 100GB �̻� ����
			if(nFreeSize > (1024 * 100))
				bResult = true;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpAllotmentQueue()\n"))); 
	}

	return bResult;
}

// FTP ���� ���� ��û
bool DBCtrl::LoadFtpInfoUpload(ACE_UINT64 nVolumeIdx, ACE_UINT16 nSpeed, char* pServerIP, int &nServerPORT, char* pServerID, char* pServerPW)
{
	//printf("LoadFtpInfoUpload() ����!\n");//test
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	DBbase*		pDataBase = NULL;

	char		pFlag_Use[2];
	char		pFlag_State[2];

	//printf("nVolumeIdx:%I64d, nSpeed:%d, pServerIP:%s, nServerPORT:%d, pServerID:%s, pServerPW:%s\n", 
	//	nVolumeIdx, nSpeed, pServerIP, nServerPORT, pServerID, pServerPW);//test

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		sprintf(m_pQuery1,  "Select f_server.port, f_server.ip, f_account.passwd, f_account.id, f_server.flag_state, f_server.flag_use " \
							"From %s f_volume, %s f_account, %s f_server Where " \
							"f_volume.volume = %I64d And f_volume.ftp = f_server.ftp And " \
							"f_volume.volume = f_account.ftp_volume And f_account.speed = %d",  
							TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_ACCOUNT, TABLE_COMMON_FTP_SERVER, nVolumeIdx, nSpeed);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			//printf("LoadFtpInfoUpload() ������ ����!\n");//test
			SAFE_DELETEDB(pResult);
			throw UPLOAD_RESULT_ERROR;
		}

		nServerPORT = atoi(pRow[0]);
		strcpy(pServerIP, pRow[1]);
		strcpy(pServerPW, pRow[2]);
		strcpy(pServerID, pRow[3]);
		strcpy(pFlag_Use, pRow[4]);
		strcpy(pFlag_State, pRow[5]);
		
		SAFE_DELETEDB(pResult);

		//printf("ftp���� - nTempVolume: %I64d, pServerIP: %s, pServerPT: %d, pServerID: %s, pServerPW: %s, pFlag_Use: %s, pFlag_State: %s\n", 
		//	nVolumeIdx, pServerIP, nServerPORT, pServerID, pServerPW, pFlag_Use, pFlag_State);//test

		if(!strcmp(pFlag_Use, "N") || !strcmp(pFlag_State, "N"))
			return false;
        
		return true;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoUpload()\n"))); 
	}

	return false;
}

//20080204 _webhard_file_bbs�� ��ϵ� ���� ���� ��û���� ����, jyh
// FTP ���� ���� ��û
int DBCtrl::LoadFtpInfoDownLoad(int nVolumeIdx)
{
	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
	MYSQL_RES	*pResult_Server = 0;
	MYSQL_ROW	pRow_Server;
	DBbase*		pDataBase = NULL;

	//int			nVolumeIdx;
	__int64		nFileIdx = 0;
//	char		pDivision[2];
	//bool		bFindFile = false;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		// ������ ���� ��ȸ
		sprintf(m_pQuery2,  "Select f_server.flag_state, f_server.flag_use " \
			"From %s f_volume, %s f_server Where f_volume.volume = %d And f_volume.ftp = f_server.ftp ", 
			TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_SERVER, nVolumeIdx);
		if((pResult_Server = pDataBase->ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow_Server = mysql_fetch_row(pResult_Server)) == 0)
		{
			SAFE_DELETEDB(pResult_Server);
			SAFE_DELETEDB(pResult);
			//continue;
			return -1;	
		}			
		if(!strcmp(pRow_Server[0], "N") || !strcmp(pRow_Server[1], "N"))
		{
			SAFE_DELETEDB(pResult_Server);
			SAFE_DELETEDB(pResult);
			//continue;
			return -2;	//���� ��� ����, jyh
		}
		SAFE_DELETEDB(pResult_Server);					
		SAFE_DELETEDB(pResult);
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoDownLoad()\n"))); 
	}

	return nVolumeIdx;
}
//// FTP ���� ���� ��û
//int DBCtrl::LoadFtpInfoDownLoad(char* pSaveName)
//{
//	MYSQL_RES	*pResult = 0;
//	MYSQL_ROW	pRow;
//	MYSQL_RES	*pResult_Server = 0;
//	MYSQL_ROW	pRow_Server;
//	DBbase*		pDataBase = NULL;
//
//	int			nVolumeIdx;
//	__int64		nFileIdx = 0;
//	char		pDivision[2];
//	bool		bFindFile = false;
//
//	try
//	{
//		if(OPTION_USE_FILEUNITY == 1) 
//			pDataBase = m_pFileDB;
//		else
//			pDataBase = this;
//		
//		sprintf(m_pQuery1,  "Select ftp_volume From %s Where filename = '%s' Order By rand()",  TABLE_COMMON_FILES, pSaveName);
//		pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount);
//		if(pResult != 0) 
//		{
//			while(pRow = mysql_fetch_row(pResult))
//			{
//				bFindFile = true;
//
//				// ���õ� ����
//				nVolumeIdx = atoi(pRow[0]);
//
//				// ������ ���� ��ȸ
//				sprintf(m_pQuery2,  "Select f_server.flag_state, f_server.flag_use " \
//							"From %s f_volume, %s f_server Where f_volume.volume = %d And f_volume.ftp = f_server.ftp ", 
//							TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_SERVER, nVolumeIdx);
//				if((pResult_Server = pDataBase->ExecuteQueryR(m_pQuery2, nCount)) == 0 || (pRow_Server = mysql_fetch_row(pResult_Server)) == 0)
//				{
//					SAFE_DELETEDB(pResult_Server);	
//					continue;
//				}			
//				if(!strcmp(pRow_Server[0], "N") || !strcmp(pRow_Server[1], "N"))
//				{
//					SAFE_DELETEDB(pResult_Server);	
//					continue;
//				}
//				SAFE_DELETEDB(pResult_Server);					
//				SAFE_DELETEDB(pResult);
//				return nVolumeIdx;;	
//			}
//		}		
//
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoDownLoad() = NULL [%s]\n"), pSaveName)); 
//		SAFE_DELETEDB(pResult);				
//			
//		// ������ ���������� �������϶�
//		if(bFindFile) return -2;
//		// ������ �������� ������
//		else -1;
//
//		/*
//		sprintf(m_pQuery1,  "Select ftp_volume From %s Where filename = '%s' Order By rand() Limit 1",  TABLE_COMMON_FILES, pSaveName);
//		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//		{
//			ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoDownLoad() = NULL [%s]\n"), pSaveName)); 
//			SAFE_DELETEDB(pResult);				
//			return -1;
//		}			
//		nVolumeIdx = atoi(pRow[0]);
//		SAFE_DELETEDB(pResult);
//		return nVolumeIdx;;	
//		
//		sprintf(m_pQuery1,  "Select division, fileidx, ftp_volume From %s Where filename = '%s'",  TABLE_COMMON_FILES, pSaveName);
//		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//		{
//			ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoDownLoad() = NULL [%s]\n"), pSaveName)); 
//			SAFE_DELETEDB(pResult);				
//			return -1;
//		}			
//
//		strcpy(pDivision, pRow[0]);		
//		nFileIdx = _atoi64(pRow[1]);
//		nVolumeIdx = atoi(pRow[2]);
//		SAFE_DELETEDB(pResult);	
//		
//		if(!strcmp(pDivision, "N"))
//			return nVolumeIdx;
//		else
//		{
//			sprintf(m_pQuery1,  "Select ftp_volume From %s Where file_idx = %I64d Order By rand() Limit 1",  
//								TABLE_COMMON_FILE_DIVISION, nFileIdx);
//			if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
//			{
//				ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoDownLoad(), rand() failed\n"))); 
//				return -1;
//			}			
//			nVolumeIdx = atoi(pRow[0]);		
//			SAFE_DELETEDB(pResult);	
//			return nVolumeIdx;
//		}
//		*/
//	}
//	catch(...)
//	{
//		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpInfoDownLoad()\n"))); 
//	}
//	return -1;
//}


// �ٿ�ε� ���� ���� ���� üũ
bool DBCtrl::LoadFtpCheck(ACE_UINT64 nVolumeIdx)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	DBbase*		pDataBase = NULL;

	double		nAvg;
	int			nCountDown;
	int			nCountUp;

	try
	{
		if(OPTION_USE_FILEUNITY == 1) 
			pDataBase = m_pFileDB;
		else
			pDataBase = this;

		sprintf(m_pQuery1,  "Select f_server.loadavg, sum(f_account.count_up), sum(f_account.count_down) " \
							"From %s f_volume, %s f_account, %s f_server Where " \
							"f_volume.volume = %I64d And f_volume.ftp = f_server.ftp And " \
							"f_volume.volume = f_account.ftp_volume group by f_account.ftp_volume",  
							TABLE_COMMON_FTP_VOLUME, TABLE_COMMON_FTP_ACCOUNT, TABLE_COMMON_FTP_SERVER, nVolumeIdx);
		if((pResult = pDataBase->ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw 0;
		}
		nAvg = atof(pRow[0]);
		nCountUp = atoi(pRow[1]);
		nCountDown = atoi(pRow[2]);			
		SAFE_DELETEDB(pResult);	
		//printf("LoadFtpCheck() nAvg: %f, nCountUp: %d, nCountDown: %d\n", nAvg, nCountUp, nCountDown);//test

		if( RunServerInfo.nServerRunMode == MMSV_SERVER_MODE_DOWN && (
			nAvg > RunServerInfo.nLimit_LoadAvg || 
			(int)RunServerInfo.nLimit_Count < nCountDown))
			return false;

		if( RunServerInfo.nServerRunMode == MMSV_SERVER_MODE_UP && (
			nAvg > RunServerInfo.nLimit_LoadAvg || 
			(int)RunServerInfo.nLimit_Count < nCountUp))
		{
			//printf("���ε� ���� �Ҹ��� return false!\n");//test
			return false;
		}

		//printf("���ε� ���� ���� return true!\n");//test
		return true;
	}
	catch(...)
	{
		//printf("LoadFtpCheck() catch(...)\n");
		ACE_DEBUG((LM_WARNING, ACE_TEXT("��Common��LoadFtpCheck()\n"))); 
	}

	//printf("LoadeFtpCheck() return false\n");
	return false;
}


// �ٿ�ε� ī��Ʈ ����(��û��)
void DBCtrl::DownCountSet_Criterion(char *pSaveName)
{
	DBbase*		pDataBase = NULL;		

	if(OPTION_USE_FILEUNITY == 1) 
		pDataBase = m_pFileDB;
	else
		pDataBase = this;

	sprintf(m_pQuery1, "Update %s Set count_criterion = count_criterion + 1 Where filename = '%s'", TABLE_COMMON_FILES, pSaveName);
	pDataBase->ExecuteQuery(m_pQuery1);
}
