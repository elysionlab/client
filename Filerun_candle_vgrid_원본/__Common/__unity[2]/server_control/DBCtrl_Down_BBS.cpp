#include <string>
#include "DBCtrl.h"
#include "mmsv_Aio_Acceptor.h"
#include "../Token.h"


extern SERVER_RUNTIME_INFO RunServerInfo;


// 파일다운로드 전에 파일의 정보와 해당 서버의 상태 요청
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

		/* 다운로드 요청받은 파일의 존재유무 체크 : 타입별로 다르게 접근 */
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
		sprintf(m_pQuery1,  "Select realsize, flag_upload From %s Where no = %I64d", pTableFile, pDownPre->nFileIdx);
		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			SAFE_DELETEDB(pResult);	
			throw DOWN_RESULT_NOTFOUND;
		}
		
		pDownPre->nFileRealSize	= _atoi64(pRow[0]);
		strcpy(pFlag, pRow[1]);
		SAFE_DELETEDB(pResult);	

		// 업로드가 완료된 파일인지 체크
		if(!strcmp(pFlag, "N"))
			throw DOWN_RESULT_UPLOAD;

		// 정상적으로 파일 다운로드 가능
		if(m_bServiceRun) pDownPre->nResult = DOWN_RESULT_OK;			
		// 서비스 중지된 상태
		else pDownPre->nResult = DOWN_RESULT_SERVICE_STOP;			

		pClient->send_packet_queue(LOAD_DOWNPRE, (char*)pDownPre, sizeof(LOADPREDOWN));
	}

	catch(int nCode)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPre()━Catch(nCode) = %d : %s\n"), nCode, pClient->getuserinfo())); 

		pDownPre->nResult = nCode;			
		pClient->send_packet_queue(LOAD_DOWNPRE, (char*)pDownPre, sizeof(LOADPREDOWN));
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPre()━Catch(...)\n"))); 

		pDownPre->nResult = DOWN_RESULT_ERROR;			
		pClient->send_packet_queue(LOAD_DOWNPRE, (char*)pDownPre, sizeof(LOADPREDOWN));
	}
}


// 실제 다운로드를 위해 다운로드 정보 요청	
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
	//char		pMurekaFileID[33];	//20080821 뮤레카 파일 아이디, jyh

	int			nFilePayState;
	int			nFilePayMethod;

	ACE_UINT64	nFixDateCur;
	ACE_UINT64	nFixDateEnd;
	int			nFixTimeBegin;
	int			nFixTimeEnd;
	int			nFixTimeCur;

	char		pUseridCurrent[20];

	char		pServerIP[80];
	char		pServerID[20];
	char		pServerPW[20];
	int			nServerPT;

	PLOADDOWN	pDownPro;
	bool		bFreeDown = false;

	try
	{
		pDownPro = (PLOADDOWN)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		/* 다운로드 요청받은 파일의 존재유무 체크 : 타입별로 다르게 접근 */
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

		////20080821 뮤레카 파일 아이디(murekaid) 추가, jyh
		//sprintf(m_pQuery1,  "Select flag_upload, realname, md5sum, size, realsize, UNIX_TIMESTAMP(), userid, temp_name, temp_volume, murekaid From %s Where no = %I64d",  
		//					pTableFile, pDownPro->nFileIdx);
		sprintf(m_pQuery1,  "Select flag_upload, realname, md5sum, size, realsize, UNIX_TIMESTAMP(), userid, temp_name, temp_volume From %s Where no = %I64d",  
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
		
		//20080310 요청 서버값이 0이면 무료 다운로드 이므로 디비 서버값을 저장한다, jyh
		if(pDownPro->nServerVolume == 0)
		{
			nVolumeIdx	= atoi(pRow[8]);
			bFreeDown = true;
			//printf("서버 볼륨:%d, 무료 다운\n", pDownPro->nServerVolume);
		}
		else
		{
            nVolumeIdx	= pDownPro->nServerVolume;
			bFreeDown = false;
			//printf("서버 볼륨:%d, 유료 다운\n", pDownPro->nServerVolume);
		}

		//strcpy(pMurekaFileID, pRow[9]);		//20080821 뮤레카 파일 아이디, jyh

		SAFE_DELETEDB(pResult);
		//printf("다운 받을 서버 주소: %d\n", nVolumeIdx);

		/* 업로드가 완료된 파일인지 체크 */
		if(!strcmp(pFlag, "N"))
			throw DOWN_RESULT_UPLOAD;

		/* 요청한 전송시작점이 파일 크기 보다 큰지 체크 */
		if(nSizeReal <= pDownPro->nDownStartPoint)
			throw DOWN_RESULT_SIZEOVER;

		//20080310 무료 다운로드는 주말과 특정 시간대는 다운받지 못하게 한다, jyh-------------------------
		if(bFreeDown)
		{
			//int nUseAll;
			//int nWeekendUse;
			//int nTimeUse;
			//int nLimitUserUse;
			//int nStartTime;
			//int nEndTime;
			//int nLimitUser;

			//sprintf(m_pQuery1, "Select use_all, weekend_use, time_use, limituser_use, start_time, end_time, limituser "
			//					"From _webhard_free_config Where idx = 1");
			//if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
			//{
			//	nUseAll = atoi(pRow[0]);
			//	nWeekendUse = atoi(pRow[1]);
			//	nTimeUse = atoi(pRow[2]);
			//	nLimitUserUse = atoi(pRow[3]);
			//	nStartTime = atoi(pRow[4]);
			//	nEndTime = atoi(pRow[5]);
			//	nLimitUser = atoi(pRow[6]);
			//	//printf("UseAll:%d, WeekendUse:%d, TimeUse:%d, LimitUserUse:%d, StartTime:%d, EndTime:%d, LimitUser:%d\n",
			//	//		nUseAll, nWeekendUse, nTimeUse, nLimitUserUse, nStartTime, nEndTime, nLimitUser);
			//}

			//SAFE_DELETEDB(pResult);

			////무료 다운 설정값을 사용 하면...
			//if(nUseAll)
			//{
			//	//주말 무료다운 금지 
			//	if(nWeekendUse)
			//	{
			//		sprintf(m_pQuery1, "Select dayofweek(now())");	//요일을 숫자로 반환 1:일요일, 2:월요일...
			//		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
			//		{
			//			if(atoi(pRow[0]) == 1/*일요일*/ || atoi(pRow[0]) == 7/*토요일*/)
			//			{
			//				//printf("무료다운 주말 사용 금지: %d\n", atoi(pRow[0]));
			//				SAFE_DELETEDB(pResult);
			//				throw DOWN_RESULT_FREE_WEEKEND;		//주말 사용 금지 프로토콜 반환
			//			}
			//		}

			//		SAFE_DELETEDB(pResult);
			//	}
			//	
			//	//평일 특정 시간대 무료다운 금지
			//	if(nTimeUse)
			//	{
			//		sprintf(m_pQuery1, "Select hour(now())");		//시간만 반환 예)15:오후3시
			//		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
			//		{
			//			if(atoi(pRow[0]) >= nStartTime || atoi(pRow[0]) < nEndTime)
			//			{
			//				//printf("무료다운 특정 시간대 사용 금지: %d\n", atoi(pRow[0]));
			//				SAFE_DELETEDB(pResult);
			//				throw DOWN_RESULT_FREE_TIMEOVER;	//특정 시간대 사용 금지 프로토콜 반환
			//			}
			//		}

			//		SAFE_DELETEDB(pResult);
			//	}

			//	//무료 다운로드 인원 제한 체크
			//	if(nLimitUserUse)
			//	{
			//		sprintf(m_pQuery1, "Select SUM(count_down) From _webhard_ftp_account Where ftp_volume = %d", nVolumeIdx);
			//		if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
			//		{
			//			if(atoi(pRow[0]) >= nLimitUser)
			//			{
			//				//printf("무료다운 인원 제한으로 사용 불가: %d\n", atoi(pRow[0]));
			//				SAFE_DELETEDB(pResult);
			//				throw DOWN_RESULT_FREE_LIMIT_COUNT;		
			//			}
			//		}
			//		
			//		SAFE_DELETEDB(pResult);
			//	}
			//}
			
			pDownPro->nDownSpeed = 9;		//a4로 파일서버에 접속하기 위해, jyh
		}		
		//------------------------------------------------------------------------------------------------
		else
		{
			/* 해당 파일의 구매 형태에 따른 권한 체크 */
			switch(pDownPro->nFileType)
			{
				case 0:	// 거래소
				case 1:	// 블로그		
					for(int nFileTypeC = 0; nFileTypeC < 2; nFileTypeC++)
					{
						/*
						**** 로그인후 다른 아이디로 로그인한 경우를 확인하기 위해
						sprintf(m_pQuery1,  "Select state, method From %s " \
											"Where idx=%I64d And userid='%s' And link_idx=%I64d And target='%d' And expiredate > UNIX_TIMESTAMP() limit 1",  
											TABLE_PURCHASE, pDownPro->nBoardIndex_, pClient->m_pUserid, pDownPro->nBoardIndex, pDownPro->nFileType);
						*/

						sprintf(m_pQuery1,  "Select state, method, userid From %s " \
											"Where idx=%I64d And link_idx=%I64d And target='%d' And state=0 And expiredate > UNIX_TIMESTAMP() limit 1",  
											TABLE_PURCHASE, pDownPro->nBoardIndex_, pDownPro->nBoardIndex, pDownPro->nFileType);
						if((pResult = ExecuteQueryR(m_pQuery1, nCount)) != 0 && (pRow = mysql_fetch_row(pResult)) != 0)
							break;
					}
					if(pRow == 0)
					{
						ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPro_BBS()━기간만료 = %s"), m_pQuery1)); 
						SAFE_DELETEDB(pResult);	
						throw DOWN_RESULT_TIMEOVER;
					}
					
					nFilePayState	= atoi(pRow[0]);	
					nFilePayMethod	= atoi(pRow[1]);
					strcpy(pUseridCurrent, pRow[2]);
					SAFE_DELETEDB(pResult);	

					// 패킷제 다운로드 속도
					//20081022 스피드값을 어플에서 받아옴, jyh
					//pDownPro->nDownSpeed = 0;
					
					// 환불승인
					if(nFilePayState == 2)
						throw DOWN_RESULT_PAY_CANCEL;

					// 정액제 구매
					if(nFilePayMethod == 2)
					{
						sprintf(m_pQuery1,  "Select fix_date_start, fix_date_end, fix_time_start, fix_time_end, " \
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

						// 정액제 기간 만료 체크
						if(nFixDateEnd < nFixDateCur)
							throw DOWN_RESULT_FIX_DATEOVER;
						
						//정액제 가능한 시간인지 체크 
						if(nFixTimeBegin != nFixTimeEnd)
						{
							if(nFixTimeBegin < nFixTimeEnd && (nFixTimeCur < nFixTimeBegin || nFixTimeEnd < nFixTimeCur))
								throw DOWN_RESULT_FIX_TIMEOVER;
							if(nFixTimeBegin > nFixTimeEnd && nFixTimeCur < nFixTimeBegin && nFixTimeEnd < nFixTimeCur)
								throw DOWN_RESULT_FIX_TIMEOVER;
						}			

						// 정액제 다운로드 속도
						//20081022 스피드값을 어플에서 받아옴, jyh
						//pDownPro->nDownSpeed = 1;
					}
					break;
				case 2:	// 개인자료
					nFilePayMethod = 0;
					if(strcmp(pClient->m_pUserid, pClient->m_pUseridFile))
						throw DOWN_RESULT_OTHER_USER;
					break;
			}
		}
		
		//nVolumeIdx = LoadFtpInfoDownLoad(pSaveName);
		nVolumeIdx = LoadFtpInfoDownLoad(nVolumeIdx);	//20080204 수정, jyh
		if(nVolumeIdx == -1) throw DOWN_RESULT_NOTFOUND;
		else if(nVolumeIdx == -2) throw DOWN_RESULT_SERVERSTOP;		
		if(!LoadFtpCheck((__int64)nVolumeIdx)) throw DOWN_RESULT_SERVERBUSY;
		if(!LoadFtpInfoUpload((__int64)nVolumeIdx, pDownPro->nDownSpeed, pServerIP, nServerPT, pServerID, pServerPW)) throw DOWN_RESULT_SERVERSTOP;
		
		// FTP서버에 연결이 가능한지 체크
		for(int nProc = 0;; nProc++)
		{
			if(pClient->m_pFtp->Connect(pServerIP, nServerPT, pServerID, pServerPW)) 
			{
				//20080527 파일서버에 파일이 있는지 확인한다, jyh
				ACE_UINT64 nServerFileSize = pClient->m_pFtp->RemoteFileValid(pSaveName);

				if(nServerFileSize == -2)
					throw DOWN_RESULT_NOTFOUND;
				//------------------------------------------------------------------------

				if(pClient->m_pFtp->QueryTransfer(pSaveName, pDownPro->nDownStartPoint, 1))
				{
					strcpy(pDownPro->pServerInfo, (char*)pClient->m_pFtp->m_sPassiveInfo.c_str());
					//DownCountSet_Criterion(pSaveName);	20080204 _webhard_file은 필요 없음, jyh
					break;
				}
				else
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPro_BBS()━ftp 전송 실패: ftp_volume: %I64d, ftp_IP: %s\n"), nVolumeIdx, pServerIP));
					throw DOWN_RESULT_ERROR_DELAY;
				}
			}
			else
			{
				if(nProc >= 2)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPro_BBS()━ftp 접속 실패: ftp_volume: %I64d, ftp_IP: %s\n"), nVolumeIdx, pServerIP));
					throw DOWN_RESULT_FTP_CONFAILED;
				}
			}
		}

		// 다운로드 진행중인 파일 정보
		pClient->m_nFileNo		= pDownPro->nFileIdx;
		pClient->m_nFileSizeBegin = pDownPro->nDownStartPoint;
		pClient->m_nFilePayMethod = nFilePayMethod;

		////20080821 뮤레카 파일 아이디, jyh
		//lstrcpy(pDownPro->pMurekaFileID, pMurekaFileID);
		// 정상적으로 파일 다운로드 가능
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
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadPro()━Catch(...)\n"))); 
		pDownPro->nResult = DOWN_RESULT_ERROR;			
		pClient->send_packet_queue(LOAD_DOWNLOAD, (char*)pDownPro, sizeof(LOADDOWN));
	}
}

// 현재 파일 다운로드 중인 사이즈 통보시
void DBCtrl::DownLoad_State_BBS(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
}

// 다운로드 완료 종료 처리
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

		// 다운로드 받은 사이즈
		if(pClient->m_nFileSizeBegin >= 0) pDownEnd->nSizeBegin = pClient->m_nFileSizeBegin;
		if(pDownEnd->nSizeEnd < pDownEnd->nSizeBegin) pDownEnd->nSizeEnd = pDownEnd->nSizeBegin;
		else nDownLoadSize = pDownEnd->nSizeEnd - pDownEnd->nSizeBegin;		
		nDownLoadSize = (nDownLoadSize > nDownTransSize) ? nDownLoadSize : nDownTransSize;

		// 로그생성
		sprintf(m_pQuery1, "Insert Into %s (file_no, board_idx, userid, userid_seller, type, method, size, ip, regdate) " \
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
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadEnd()━Catch(...)\n"))); 
	}
}

// 다운로드 아이템 추가
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
		if(pToken->GetCount() == 4)		//20080213 서버 주소 추가, jyh
		{			
			// 요청 타입에 맞는 테이블 쿼리
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
/*
//원본
			sprintf(m_pQuery1,  "Select no, size, realname, foldername From %s Where bbs_no = %s And flag_upload = 'Y' " \
								"order by depth asc, foldername asc, realname asc",  
								pTableFile, pToken->GetToken(2));
*/




			sprintf(m_pQuery1,	"Select " \
								"	tb_files_bbs.no," \
								"	tb_files_bbs.size," \
								"	tb_files_bbs.realname," \
								"	tb_files_bbs.foldername," \
								"	tb_files_bbs.temp_name," \
								"	(" \
								"		select " \
								"			tb_volume.volumepath " \
								"		from " \
								"			_webhard_ftp_volume as tb_volume" \
								"		where " \
								"			tb_volume.volume=tb_files_bbs.temp_volume" \
								"		limit " \
								"			0,1" \
								"	) as temp_path," \

								"	ifnull((" \
								"		SELECT " \
								"			tb_mureka.u_hash" \
								"		FROM " \
								"			mureka_copyright_hash  AS tb_mureka" \
								"		WHERE " \
								"			tb_mureka.u_board_idx  = tb_files_bbs.bbs_no and " \
								"			tb_mureka.u_file_idx = tb_files_bbs.no " \
								"		LIMIT 0 , 1" \
								"	),'') AS mureka_hash," \

								"	tb_files_bbs.temp_volume," \

								"	tb_files_bbs.userid," \



								"	ifnull((" \
								"		SELECT " \
								"			tb_bbs.cash" \
								"		FROM " \
								"			_webhard_bbs  AS tb_bbs" \
								"		WHERE " \
								"			tb_bbs.idx  = tb_files_bbs.bbs_no " \
								"		LIMIT 0 , 1" \
								"	),0) AS bbs_cash," \


								"	ifnull((" \
								"		SELECT " \
								"			tb_mureka.u_hash_org" \
								"		FROM " \
								"			mureka_copyright_hash  AS tb_mureka" \
								"		WHERE " \
								"			tb_mureka.u_board_idx  = tb_files_bbs.bbs_no and " \
								"			tb_mureka.u_file_idx = tb_files_bbs.no " \
								"		LIMIT 0 , 1" \
								"	),'') AS mureka_hash_org," \




								"ifnull((" \
								"	SELECT " \
								"		tb_bbs.title" \
								"	FROM " \
								"		_webhard_bbs  AS tb_bbs" \
								"	WHERE " \
								"		tb_bbs.idx  = tb_files_bbs.bbs_no " \
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
								"		tb_bbs.idx  = tb_files_bbs.bbs_no " \
								"	LIMIT 0 , 1" \
								"),'') AS bbs_code_cate," \


								"	ifnull((" \
								"		SELECT " \
								"			tb_mureka.u_video_content_id" \
								"		FROM " \
								"			mureka_copyright_hash  AS tb_mureka" \
								"		WHERE " \
								"			tb_mureka.u_board_idx  = tb_files_bbs.bbs_no and " \
								"			tb_mureka.u_file_idx = tb_files_bbs.no " \
								"		LIMIT 0 , 1" \
								"	),'') AS mureka_video_content_id," \



								"	ifnull((" \
								"		SELECT " \
								"			tb_mureka.u_money" \
								"		FROM " \
								"			mureka_copyright_hash  AS tb_mureka" \
								"		WHERE " \
								"			tb_mureka.u_board_idx  = tb_files_bbs.bbs_no and " \
								"			tb_mureka.u_file_idx = tb_files_bbs.no " \
								"		LIMIT 0 , 1" \
								"	),'') AS mureka_money," \



								"	ifnull((" \
								"		SELECT " \
								"			tb_mureka.u_filtering_key" \
								"		FROM " \
								"			mureka_copyright_hash  AS tb_mureka" \
								"		WHERE " \
								"			tb_mureka.u_board_idx  = tb_files_bbs.bbs_no and " \
								"			tb_mureka.u_file_idx = tb_files_bbs.no " \
								"		LIMIT 0 , 1" \
								"	),'') AS mureka_rehash," \



								"	ifnull((" \
								"		SELECT " \
								"			tb_mureka.u_smartblock_key" \
								"		FROM " \
								"			mureka_copyright_hash  AS tb_mureka" \
								"		WHERE " \
								"			tb_mureka.u_board_idx  = tb_files_bbs.bbs_no and " \
								"			tb_mureka.u_file_idx = tb_files_bbs.no " \
								"		LIMIT 0 , 1" \
								"	),'') AS mureka_1hash" \




								" From " \
								"	%s as tb_files_bbs  " \
								"Where " \
								"	tb_files_bbs.bbs_no = %s And " \
								"	tb_files_bbs.flag_upload = 'Y' " \

								"order by " \
								"	tb_files_bbs.depth asc, " \
								"	tb_files_bbs.foldername asc, " \
								"	tb_files_bbs.realname asc" ,
								pTableFile, 
								pToken->GetToken(2)
							);

			
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

				m_strResult = m_strResult + pToken->GetToken(0) + SEP_FIELD;//0
				m_strResult = m_strResult + pToken->GetToken(1) + SEP_FIELD;//1
				m_strResult = m_strResult + pToken->GetToken(2) + SEP_FIELD;//2
				m_strResult = m_strResult + pRow[0] + SEP_FIELD;			//3
				m_strResult = m_strResult + pRow[1] + SEP_FIELD;			//4
				m_strResult = m_strResult + pRow[2] + SEP_FIELD;			//5
				//20080213 서버 주소 추가, jyh
				//m_strResult = m_strResult + pRow[3];
/*20100812 추가
				m_strResult = m_strResult + pRow[3] + SEP_FIELD;
				m_strResult = m_strResult + pToken->GetToken(3);
*/
				m_strResult = m_strResult + pRow[3] + SEP_FIELD;			//6
//				m_strResult = m_strResult + pToken->GetToken(3)+ SEP_FIELD;
				m_strResult = m_strResult + pRow[7]+ SEP_FIELD;			//7
				m_strResult = m_strResult + pRow[5]+"/"+pRow[4]+ SEP_FIELD;			//8
				m_strResult = m_strResult + pRow[6]+ SEP_FIELD;			//9
				m_strResult = m_strResult + pRow[8]+ SEP_FIELD;			//10
				m_strResult = m_strResult + pRow[9]+ SEP_FIELD;			//11
				m_strResult = m_strResult + pRow[10]+ SEP_FIELD;			//12
				m_strResult = m_strResult + pRow[11]+ SEP_FIELD;			//13
				m_strResult = m_strResult + pRow[12]+ SEP_FIELD;			//14
				m_strResult = m_strResult + pRow[13]+ SEP_FIELD;			//15
				m_strResult = m_strResult + pRow[14]+ SEP_FIELD;			//16
				m_strResult = m_strResult + pRow[15]+ SEP_FIELD;			//17
				m_strResult = m_strResult + pRow[16];			//18

			}
			SAFE_DELETEDB(pResult);	
			if(m_strResult.length() > 0)
				pClient->send_packet_queue(LOAD_DOWNLIST, (char*)m_strResult.c_str(), (int)m_strResult.length() + 1);
		}		
		delete pToken;	
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadItem()━Catch(...)\n"))); 
	}
}






// 다운로드시 뮤레카 저작권 자료 등록
void DBCtrl::DownLoadState_Copyright_data(mmsv_User* pClient, ACE_Message_Block* pDataBlock)
{
	PUPLOADMUREKA pTransMurekaInfo;

	MYSQL_RES	*pResult = 0;
	MYSQL_ROW	pRow;
	//(120829)
	char		Mureka_idx_code[30];

//printf("저작권체크 기록\n");


	try
	{

		pTransMurekaInfo = (PUPLOADMUREKA)(pDataBlock->rd_ptr() + MMSV_PACKET_HEAD_SIZE);

		//기존 자료 입력체크
		
		/*
		120704
		sprintf(m_pQuery1,  "SELECT * FROM %s WHERE u_board_idx='%I64d' AND u_file_idx='%I64d' limit 0,1",	TABLE_MUREKA_COPYRIGHT_HASH,
																												pTransMurekaInfo->nBoardIndex,
																												pTransMurekaInfo->uFile_code);
		*/
		sprintf(m_pQuery1,  "SELECT * FROM %s WHERE u_board_idx='%I64d' AND u_file_idx='%I64d'and u_video_id='%I64d' and u_turn='%d' limit 0,1",	TABLE_MUREKA_COPYRIGHT_HASH,
																												pTransMurekaInfo->nBoardIndex,
																												pTransMurekaInfo->uFile_code,
																												pTransMurekaInfo->uVideo_id,
																												pTransMurekaInfo->uTurn);


		if((pResult = ExecuteQueryR(m_pQuery1)) == 0 || (pRow = mysql_fetch_row(pResult)) == 0)
		{
			//저작권 자료 디비에 저장
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
								pTransMurekaInfo->uHash_master,
								pTransMurekaInfo->uTurn
					);
					ExecuteQuery(m_pQuery1);


			//로그저장용 고유번호저장(120829)
			sprintf(Mureka_idx_code,"%I64d",mysql_insert_id(m_pMysql));


			//저작권 자료 업데이트 인경우저장
			sprintf(m_pQuery1,	"insert %s set " \
								"u_board_idx='%I64d',"\
								"u_file_idx='%I64d'"
								,
								TABLE_MUREKA_COPYRIGHT_HASH_NEWUPDATE,
								pTransMurekaInfo->nBoardIndex,
								pTransMurekaInfo->uFile_code
					);
					ExecuteQuery(m_pQuery1);



		}
		else
		{






			if((strcmp(pRow[3],pTransMurekaInfo->uStatus)!=0&&strcmp(pTransMurekaInfo->uStatus,"")!=0)||(strcmp(pTransMurekaInfo->uStatus,"01")==0&&atoi(pRow[5])!=pTransMurekaInfo->uMoney))
			{
/*
				//저작권 자료 디비에 저장
				sprintf(m_pQuery1,	"update %s set " \
									"u_filetype='%d',"\
									"u_status='%s',"\
									"u_money='%d',"\
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
									"u_counts='%d'"\


									" where "\
									"u_board_idx='%I64d'"\
									" and "\
									"u_file_idx='%I64d'"

									,

									TABLE_MUREKA_COPYRIGHT_HASH,
									pTransMurekaInfo->uFiletype,
									pTransMurekaInfo->uStatus,
									pTransMurekaInfo->uMoney,
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
									
									pTransMurekaInfo->nBoardIndex,
									pTransMurekaInfo->uFile_code
						);
*/
				//저작권 자료 디비에 저장
				sprintf(m_pQuery1,	"update %s set " \
									"u_filetype='%d',"\
									"u_status='%s',"\
									"u_money='%d',"\
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
									"u_counts='%d'"\


									" where "\
									"u_code='%s'"

									,

									TABLE_MUREKA_COPYRIGHT_HASH,
									pTransMurekaInfo->uFiletype,
									pTransMurekaInfo->uStatus,
									pTransMurekaInfo->uMoney,
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
									
									pRow[0]
						);
						ExecuteQuery(m_pQuery1);


				//로그저장용 고유번호저장(120829)
				strcpy(Mureka_idx_code,pRow[0]);



						
				//저작권 자료 업데이트 인경우저장
				sprintf(m_pQuery1,	"insert %s set " \
									"u_board_idx='%I64d',"\
									"u_file_idx='%I64d'"
									,
									TABLE_MUREKA_COPYRIGHT_HASH_NEWUPDATE,
									pTransMurekaInfo->nBoardIndex,
									pTransMurekaInfo->uFile_code
						);
						ExecuteQuery(m_pQuery1);




			}


		}
 

		//다운로드시 모든값 저장(120829)
		sprintf(m_pQuery1,	"insert mureka_copyright_download_history set " \
							"u_status='%s',"\
							"u_video_id='%I64d',"\
							"u_audio_id='%I64d',"\
							"u_mureka_hash='%s',"\
							"u_money='%d',"\
							"u_down_purchase='%s',"\
							"u_bbs_idx='%I64d',"\
							"u_bbs_file_idx='%I64d',"\
							"u_mureka_idx='%s',"\
							"u_uploader='%s',"\
							"u_downloader='%s',"\
							"u_filteringkey='%s',"\
							"u_title='%s',"\
							"u_company='%s',"\
							"u_redownload='%d',"\
							"u_date=unix_timestamp()",
							pTransMurekaInfo->uStatus,
							pTransMurekaInfo->uVideo_id,
							pTransMurekaInfo->uMusic_id,
							pTransMurekaInfo->uHash,
							pTransMurekaInfo->uMoney,
							pTransMurekaInfo->uDownpurchase,
							pTransMurekaInfo->nBoardIndex,
							pTransMurekaInfo->uFile_code,
							pRow[0],
							pTransMurekaInfo->uUploader,
							pTransMurekaInfo->uDownloader,
							pTransMurekaInfo->ufiltering_key,
							pTransMurekaInfo->uTitle,
							pTransMurekaInfo->uCp_company,
							pTransMurekaInfo->uRedownload
		);
		ExecuteQuery(m_pQuery1);






		//데이터 재전송(추후 사용을 위해서보내는것) - 현재는 의미 없음
		pClient->send_packet_queue(DOWN_SAVE_MUREKA_DATA, (char*)pTransMurekaInfo, sizeof(PUPLOADMUREKA));	

		SAFE_DELETEDB(pResult);


	}
	catch(...)
	{
		pTransMurekaInfo->nResult=0;
		pClient->send_packet_queue(SAVE_MUREKA_DATA, (char*)pTransMurekaInfo, sizeof(PUPLOADMUREKA));		

		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋DownLoadState_Copyright_data()━Catch(...)\n"))); 
	}
}