#include "stdafx.h"
#include <string.h>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"
#include <vector>


extern SERVER_RUNTIME_INFO RunServerInfo;

//
//���� �ڷ� ���ε� ����
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
		
		//�������� ������ �ε����� �˾ƿ´�.
		sprintf_s(m_pQuery1, 4096, "Select no, foldername, realname, size From %s Where bbs_no = %I64d and bbs_table = '%s'", TABLE_OPEN_DATA, pOpenUpList->nContentsIdx, szbbs_table);

		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENUP_RESULT_NOTFOUND;
		}

		std::vector<__int64>		v_n64FileIdx;		//���� �ε���
		std::vector<__int64>		v_n64FileSize;		//���� ������
		std::vector<std::string>	v_strFolderName;	//���� ��
		std::vector<std::string>	v_strFileName;		//���� ��
		std::vector<int>			v_nFolderDepth;		//���� depth
		std::string strTemp;

		v_n64FileIdx.push_back(_atoi64(pRow[0]));
		strTemp = pRow[1];
		v_strFolderName.push_back(strTemp);
		strTemp = pRow[2];
		v_strFileName.push_back(strTemp);
		v_n64FileSize.push_back(_atoi64(pRow[3]));
		v_nFolderDepth.push_back(1);	//ù��°�� ������ 1(���� ����)
		
		while((pRow = mysql_fetch_row(pResult)))
		{
			//printf("while�� ����\n");
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

			//���� ������ �������� ����
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

		//������ �ִ��� ���ؼ� ������ ������ ����
		if(v_strFolderName[0] == "/")
		{
			//������ ������ ���ϸ� ����(�ε���#<���ϸ�#<������)
			sprintf_s(szTemp, 2048, "%I64d#<%s#<%I64d", v_n64FileIdx[0], v_strFileName[0].c_str(), v_n64FileSize[0]);
			lstrcpy(pOpenUpList->szContentsInfo, szTemp);
			//printf("������ ����: %s\n", pOpenUpList->szContentsInfo);
			lstrcpy(pOpenUpList->szFolderName, v_strFolderName[0].c_str());
			//printf("���� �̸�: %s\n", pOpenUpList->szFolderName);
		}
		else
		{
			//������ ������ ����depth�� ���ϸ��� ��ħ.(�ε���#<����depth#<���ϸ�#<������#>�ε���#<����depth#<���ϸ�#<������......#>�ε���#<����depth#<���ϸ�#<������)
			for(int i=0; i<(int)v_n64FileIdx.size(); i++)
			{
				sprintf_s(szTemp, 2048, "%I64d#<%d#<%s#<%I64d", v_n64FileIdx[i], v_nFolderDepth[i], v_strFileName[i].c_str(), v_n64FileSize[i]);
				strTemp = szTemp;
				strContentsInfo += strTemp + SEP_ITEM;
			}

			//������ "#>"�� ���ش�.
			int len = (int)strContentsInfo.length();
			lstrcpy(szTemp, strContentsInfo.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenUpList->szContentsInfo, szTemp);
			//printf("������ ����: %s\n", pOpenUpList->szContentsInfo);

			//���� ���� ����
			for(int i=0; i<(int)v_strFolderName.size(); i++)
			{
				strTemp = v_strFolderName[i];
				strFolderName += strTemp + SEP_FIELD;
			}

			//������ "#<"�� ���ش�.
			len = (int)strFolderName.length();
			lstrcpy(szTemp, strFolderName.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenUpList->szFolderName, szTemp);
			//printf("���� ����: %s\n", pOpenUpList->szFolderName);
		}

		pOpenUpList->nResult = OPENUP_RESULT_OK;	

		pClient->send_packet_queue(OPEN_UPLIST, (char*)pOpenUpList, sizeof(OPENUPLIST));
	}

	catch(int nCode)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pOpenUpList->nResult = nCode;			
		pClient->send_packet_queue(OPEN_UPLIST, (char*)pOpenUpList, sizeof(OPENUPLIST));
	}
	catch(...)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadPre()��Catch(...)\n"))); 

		pOpenUpList->nResult = OPENUP_RESULT_ERROR;			
		pClient->send_packet_queue(OPEN_UPLIST, (char*)pOpenUpList, sizeof(OPENUPLIST));
	}
}


void DBCtrl::OpenUpLoad(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	
	char		szContentsType[20];		//20080528 ������ ����, jyh
	char		szSaveName[512];
	char		szSaveCopyName[512];
	char		szTemp[512];
	char		szServerIP[20];
	char		szServerID[20];
	char		szServerPW[20];
	int			nServerPT;
	int			nVolume = 0;	//20071126 ���� ���� ��ȣ, jyh
	
	POPENUPLOAD	pOpenUpload;

	try
	{
		pOpenUpload = (POPENUPLOAD)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		sprintf_s(m_pQuery1, 4096, "Select ftp, realname, bbs_table From %s Where no = %I64d", TABLE_OPEN_DATA, pOpenUpload->nFileIdx);
		//20071126 ���� ���� ��ȣ(temp_volume)�� ���´�, jyh
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
		//20071126 ���� ���� ��ȣ(temp_volume)�� ���´�, jyh
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

		// FTP������ ������ �������� üũ
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(szServerIP, nServerPT, szServerID, szServerPW)) 
			{
				//printf("ftp���� ���� ����!\n");
				//20080528 ���� �ڷ�� ������ ������ ������ ����, jyh
				char szTempSaveName[1024];

				//20080609 �ڸ� �ڷ���� ������ ������ �ٸ� ������ �������� ���� ������ �ִ��� �����ϰ� ������
				//copy_�� ���δ�, jyh
				if(lstrcmp(szContentsType, "caption"))
				{
					while(1)
					{
						sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szContentsType, szSaveName);
						//printf("%s\n", szTempSaveName);

						if(pClient->m_pFtp->RemoteFileValid(szTempSaveName) > 0)
						{
							//printf("%s ���� ����!\n", szSaveName);
							sprintf_s(szTemp, 512, "copy_%s", szSaveName);
							lstrcpy(szSaveName, szTemp);
							//printf("%s�� �ٲ�!\n", szSaveName);
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
				//printf("ftp���� ���� ����!\n");
				//ACE_DEBUG((LM_WARNING, ACE_TEXT("������������������������ %s : %s\n"), pServerIP, pServerID)); 	

				if(nProc >= 2)
					throw OPENUP_RESULT_SERVICE_STOP;
			}
		}

		//20080609 ���ϸ��� ���� �������� _opendata_file ���̺��� realname�� �ٲ۴�, jyh
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
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("��UpLoadProcess()��Catch(...)\n"))); 		
		pOpenUpload->nResult	= OPENUP_RESULT_ERROR;
		pClient->send_packet_queue(OPEN_UPLOAD, (char*)pOpenUpload, sizeof(OPENUPLOAD));
	}
}

void DBCtrl::OpneUpLoadEnd(mmsv_User* pClient, ACE_Message_Block* pDataBlock, unsigned int nPS)
{
	MYSQL_RES	*pResult = 0;
	
	try
	{
		// ����� FTP ���� ����
		pClient->m_pFtp->DisConnect();
		//printf("ftp ���� ����\n");

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
//���� �ڷ� �ٿ�ε� ����
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

		//������ Ÿ��(0:�ʼ� �ڷ�, 1:�ڸ� �ڷ�, 2:A/S �ڷ�)
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

		////�ٿ�ε� ��û���� �������� ����� �˾ƿ´�.
		//sprintf(m_pQuery1, "Select file_1_size From %s Where no = %I64d", szTableName, pOpenDownList->nContentsIdx);

		//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		//{
		//	SAFE_DELETEDB(pResult);	
		//	throw OPENDOWN_RESULT_NOTFOUND;
		//}

		//pOpenDownList->nContentsSize = _atoi64(pRow[0]);	
		//SAFE_DELETEDB(pResult);	

		//�� ������ ���� ������ �˾ƿ´�.(���� �ε���, ������, ���ϸ�, ������)
		sprintf_s(m_pQuery1, 4096, "Select no, foldername, realname, size, ftp From %s Where bbs_no = %I64d and bbs_table = '%s'", TABLE_OPEN_DATA, pOpenDownList->nContentsIdx, szbbs_table);

		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw OPENDOWN_RESULT_NOTFOUND;
		}

		nVolume = atoi(pRow[4]);	//���� ��ȣ

		std::vector<__int64>		v_n64FileIdx;		//���� �ε���
		std::vector<__int64>		v_n64FileSize;		//���� ������
		std::vector<std::string>	v_strFolderName;	//���� ��
		std::vector<std::string>	v_strFileName;		//���� ��
		std::vector<int>			v_nFolderDepth;		//���� depth
		std::string strTemp;
		
		//printf("fileIdx: %s, foldername: %s, filename: %s, size: %s\n", pRow[0], pRow[1], pRow[2], pRow[3]);

		v_n64FileIdx.push_back(_atoi64(pRow[0]));
		strTemp = pRow[1];
		v_strFolderName.push_back(strTemp);
		strTemp = pRow[2];
		v_strFileName.push_back(strTemp);
		v_n64FileSize.push_back(_atoi64(pRow[3]));
		v_nFolderDepth.push_back(1);	//ù��°�� ������ 1(���� ����)
		
		while((pRow = mysql_fetch_row(pResult)))
		{
			//printf("while�� ����\n");
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
			
			//���� ������ �������� ����
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

		//DB�� ���ϻ������ ���� ftp�� �ִ� ���ϻ���� ������ ���� �ٸ��� ���� ftp�� �ִ� ���ϻ������ ����
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

		// FTP������ ������ �������� üũ
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(szServerIP, nServerPT, szServerID, szServerPW)) 
			{
				//printf("ftp���� ���� ����!\n");

				for(int i=0; i<(int)v_strFileName.size(); i++)
				{
					char szTempSaveName[1024];

					sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szbbs_table, v_strFileName[i].c_str());
					//printf("%s\n", szTempSaveName);
					//20080527 ���ϼ����� ������ �ִ��� Ȯ���Ѵ�, jyh
					ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(szTempSaveName);
					//ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(szSaveName);
					//printf("nServerFileSize: %I64d\n", nServerFileSize);

					if(nServerFileSize == -2)
						throw OPENDOWN_RESULT_NOTFOUND;
					//------------------------------------------------------------------------

					//����� �ٸ��� ftp�� �ִ� ���� ����� �������� DB�� ����
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
				//printf("ftp���� ���� ����!\n");
				if(nProc >= 2)
					throw OPENDOWN_RESULT_SERVICE_STOP;
			}
		}


		std::string strContentsInfo;
		std::string strFolderName;
		char	szTemp[2048];

		//������ �ִ��� ���ؼ� ������ ������ ����
		if(v_strFolderName[0] == "/")
		{
			//������ ������ ���ϸ� ����(�ε���#<���ϸ�#<������)
			sprintf_s(szTemp, 2048, "%I64d#<%s#<%I64d", v_n64FileIdx[0], v_strFileName[0].c_str(), v_n64FileSize[0]);
			lstrcpy(pOpenDownList->szContentsInfo, szTemp);
			//printf("������ ����: %s\n", pOpenDownList->szContentsInfo);
			lstrcpy(pOpenDownList->szFolderName, v_strFolderName[0].c_str());
			//printf("���� �̸�: %s\n", pOpenDownList->szFolderName);
		}
		else
		{			
			//������ ������ ����depth�� ���ϸ��� ��ħ.(�ε���#<����depth#<���ϸ�#<������#>�ε���#<����depth#<���ϸ�#<������......#>�ε���#<����depth#<���ϸ�#<������)
			for(int i=0; i<(int)v_n64FileIdx.size(); i++)
			{
				sprintf_s(szTemp, 2048, "%I64d#<%d#<%s#<%I64d", v_n64FileIdx[i], v_nFolderDepth[i], v_strFileName[i].c_str(), v_n64FileSize[i]);
				strTemp = szTemp;
				strContentsInfo += strTemp + SEP_ITEM;
			}

			//������ "#>"�� ���ش�.
			int len = (int)strContentsInfo.length();
			lstrcpy(szTemp, strContentsInfo.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenDownList->szContentsInfo, szTemp);
			//printf("������ ����: %s\n", pOpenDownList->szContentsInfo);

			//���� ���� ����
			for(int i=0; i<(int)v_strFolderName.size(); i++)
			{
				strTemp = v_strFolderName[i];
				strFolderName += strTemp + SEP_FIELD;
			}

			//������ "#<"�� ���ش�.
			len = (int)strFolderName.length();
			lstrcpy(szTemp, strFolderName.c_str());
			szTemp[len-2] = '\0';

			lstrcpy(pOpenDownList->szFolderName, szTemp);
			//printf("���� ����: %s\n", pOpenDownList->szFolderName);
		}
				
		pOpenDownList->nResult = OPENDOWN_RESULT_OK;
		
		pClient->send_packet_queue(OPEN_DOWNLIST, (char*)pOpenDownList, sizeof(OPENDOWNLIST));
	}
	catch(int nCode)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPre()��Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pOpenDownList->nResult = nCode;			
		pClient->send_packet_queue(OPEN_DOWNLIST, (char*)pOpenDownList, sizeof(OPENDOWNLIST));
	}
	catch(...)
	{
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPre()��Catch(...)\n"))); 

		pOpenDownList->nResult = OPENDOWN_RESULT_ERROR;			
		pClient->send_packet_queue(OPEN_DOWNLIST, (char*)pOpenDownList, sizeof(OPENDOWNLIST));
	}
}


void DBCtrl::OpenDownLoad(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	
	char		szContentsType[20];		//20080528 ������ ����, jyh
	char		szSaveName[512];
	char		szServerIP[20];
	char		szServerID[20];
	char		szServerPW[20];
	int			nServerPT;
	int			nVolume = 0;	//20071126 ���� ���� ��ȣ, jyh

	POPENDOWNLOAD	pOpenDownPro;

	try
	{
		pOpenDownPro = (POPENDOWNLOAD)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);
		
		sprintf_s(m_pQuery1, 4096, "Select ftp, realname, bbs_table From %s Where no = %I64d", TABLE_OPEN_DATA, pOpenDownPro->nFileIdx);
		//20071126 ���� ���� ��ȣ(temp_volume)�� ���´�, jyh
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
		//20071126 ���� ���� ��ȣ(temp_volume)�� ���´�, jyh
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

		// FTP������ ������ �������� üũ
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(szServerIP, nServerPT, szServerID, szServerPW)) 
			{
				//printf("ftp���� ���� ����!\n");
				//20080528 ���� �ڷ�� ������ ������ ������ ����, jyh
				char szTempSaveName[1024];

				sprintf_s(szTempSaveName, 1024, "public_html/%s/%s", szContentsType, szSaveName);
				//printf("%s\n", szTempSaveName);
				//20080527 ���ϼ����� ������ �ִ��� Ȯ���Ѵ�, jyh
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
				//printf("ftp���� ���� ����!\n");
				if(nProc >= 2)
					throw OPENDOWN_RESULT_SERVICE_STOP;
			}
		}	

		// �ٿ�ε� �������� ���� ����
		pClient->m_nFileNo		= pOpenDownPro->nFileIdx;
		pClient->m_nFileSizeBegin = pOpenDownPro->nDownStartPoint;
		
		// ���������� ���� �ٿ�ε� ����
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
		//ACE_DEBUG((LM_WARNING, ACE_TEXT("��DownLoadPro()��Catch(...)\n"))); 
		pOpenDownPro->nResult = OPENDOWN_RESULT_ERROR;			
		pClient->send_packet_queue(OPEN_DOWNLOAD, (char*)pOpenDownPro, sizeof(OPENDOWNLOAD));
	}
}

void DBCtrl::OpenDownLoadEnd(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	// ����� FTP ���� ����
	pClient->m_pFtp->DisConnect();
	//printf("ftp ���� ����\n");
}

