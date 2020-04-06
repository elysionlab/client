#pragma once


#ifndef __CONFIG_STRUCT__
#define __CONFIG_STRUCT__

#include "ace/basic_types.h"


#define MAX_NAME_LEN		35
#define MAX_PWD_LEN			100 	


typedef struct
{
	unsigned int	code;		
	unsigned int	ps;		
	unsigned int	ds;		
} 
HEAD, *PHEAD;

typedef struct
{
	char	key[4];	
} 
TAIL, *PTAIL;


#define szEndTitle			"EPC"


// 서버 운영 설정
typedef struct
{
	//20080218 jyh
	char			pDBInfo[4][50];
	char			pDBInfoCommon[4][50];	
	char			pServerIp[80];

	ACE_UINT32		nAcceptRun;
	ACE_UINT32		nServerPORT;	
	ACE_UINT32		nQueueMaxThreads;
	ACE_UINT32		nServerRunMode;
	ACE_UINT32		nMaxUserCount;
	ACE_UINT32		nMaxUserAllocator;
	ACE_UINT32		nMaxSockBuffer;

	ACE_UINT32		nDivisionTime;
	ACE_UINT32		nDivisionCount;
	ACE_UINT32		nDivisionServer;

	ACE_UINT32		nFtpInfoUpdateTime;
	ACE_UINT32		nFileRemoveTime;
	ACE_UINT32		nExpFileValidTime;

	ACE_UINT32		nLogLevel;
	ACE_UINT32		nLimit_LoadAvg;
	ACE_UINT32		nLimit_Count;

	ACE_UINT32		nLimit_DuplicateFile;
} 
SERVER_RUNTIME_INFO;






// 회원인증
typedef struct
{
	char			pUserID[MAX_NAME_LEN];   
	char			pUserPW[MAX_PWD_LEN];		

	ACE_UINT16		nResult;	
	ACE_UINT16		nProcess;	
	ACE_UINT64		nCash_base;
	ACE_UINT64		nCash_partner;
	ACE_UINT64		nCash_bonus;
	ACE_UINT32		nCoupon;
	ACE_UINT32		nSpeed;		//20081022 회원의 스피드 값, jyh
} 
INFOAUTH,   *PINFOAUTH;







/* 업로드 관련 구조체 */

// 업로드 이전 사항체크
typedef struct 
{
	ACE_UINT16		nResult;				// 요청결과
	ACE_UINT32		nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT64		nBoardIndex;			// 게시물의 인덱스
	ACE_UINT64		nFileIdx;				// 파일 인덱스	
	ACE_UINT64		nFileSize;				// 로컬 파일사이즈		
	ACE_UINT64		nFileSize_Complete;		// 디비 완료사이즈
} 
UPLOADPRE,   *PUPLOADPRE;


// 실제 업로드
typedef struct 
{
	ACE_UINT16		nResult;				// 요청결과(업로드모드)
	ACE_UINT32		nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT64		nBoardIndex;			// 게시물의 인덱스
	ACE_UINT64		nFileIdx;				// 파일 인덱스
	ACE_UINT64		nFileSize_Start;		// 파일 시작 사이즈(이어올리기여부)
	ACE_UINT16		nUploadSpeed;			// 업로드 계정
	char			pFileCheckSum[33];		// 파일 체크섬	
	char			pServerInfo[50];		// FTP 정보
} 
UPLOADPRO, *PUPLOADPRO;


// 파일 업로드가 완료되었다고 통보
typedef struct 
{
	ACE_UINT32		nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT32		nContentsCmpl;			//20080123 컨텐츠 업로드 완료시 1, jyh
	ACE_UINT64		nBoardIndex;			// 게시물의 인덱스
	ACE_UINT64		nFileIdx;				// 파일 인덱스
	ACE_UINT64		nFileSizeEnd;		    // 완료된 사이즈		
} 
UPLOADEND,   *PUPLOADEND;

//20071210 업로드전 해시값 비교 요청, jyh
typedef struct  
{
	int		nResult;				//요청 결과 반환 성공: 1, 그외 실패
	int		nFileType;				//해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT64	bbs_no;				//게시물 번호
	ACE_UINT64	hash_idx;			//저작권 등록 페이지의 인덱스
	//char	szPath[1024];			//해시값 등록 폴더 및 파일 경로
	//char	szCheckSum[2048];		//파일의 해시값
}
UPHASH,	*PUPHASH;


//20080507 오픈 자료 업로드 리스트
typedef struct  
{
	ACE_UINT16		nResult;				//요청결과
	ACE_UINT16		nContentsType;			//컨텐츠 종류(0:필수 자료, 1:자막 자료, 2:A/S 자료)
	ACE_UINT64		nContentsIdx;			//컨텐츠 인덱스
	char			szFolderName[512];		//폴더명
	char			szContentsInfo[2048];	//파일의 정보
}
OPENUPLIST, *POPENUPLIST;

//20080506 오픈 자료 업로드 구조체
typedef struct 
{
	ACE_UINT16		nResult;			//요청결과
	ACE_UINT64		nFileIdx;			//파일 인덱스
	ACE_UINT64		nFileSize;			//파일 사이즈
	ACE_UINT64		nFileSizeStart;		//파일 시작 사이즈(이어올리기여부)
	char			szServerInfo[50];	//파일 서버 정보
}
OPENUPLOAD, *POPENUPLOAD;




//20080509 오픈 자료 다운로드 리스트
typedef struct  
{
	ACE_UINT16		nResult;				//요청결과
	ACE_UINT16		nContentsType;			//컨텐츠 종류(0:필수 자료, 1:자막 자료, 2:A/S 자료)
	ACE_UINT64		nContentsIdx;			//컨텐츠 인덱스
	char			szFolderName[512];		//폴더명
	char			szContentsInfo[2048];	//파일의 정보
}
OPENDOWNLIST, *POPENDOWNLIST;

//20080509 오픈 자료 다운로드 구조체
typedef struct 
{
	ACE_UINT16		nResult;			//요청결과
	ACE_UINT64		nFileIdx;			//파일 인덱스
	ACE_UINT64		nFileSize;			//파일 사이즈
	ACE_UINT64		nDownStartPoint;	//다운 받을 파일의 파일 포인터 위치
	char			szServerInfo[50];	//파일 서버 정보
}
OPENDOWNLOAD, *POPENDOWNLOAD;




/* 다운로드 관련 구조체 */

// 파일다운로드 전에 파일의 정보와 해당 서버의 상태 요청
typedef struct 
{
	ACE_INT16		nResult;
	ACE_UINT32		nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT64		nBoardIndex_;			// 구매테이블의 인덱스
	ACE_UINT64		nBoardIndex;			// 게시물의 인덱스
	ACE_UINT64		nFileIdx;				// 파일 인덱스
	ACE_UINT64		nFileRealSize;			// 실제 파일사이즈
} 
LOADPREDOWN,   *PLOADPREDOWN;

// 파일다운로드를 위한 채널 정보 요청
typedef struct 
{
	ACE_INT16		nResult;
	ACE_UINT32		nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT32		nServerVolume;			//20080213 다운 받을 서버의 주소, jyh
	ACE_UINT64		nBoardIndex_;			// 구매테이블의 인덱스
	ACE_UINT64		nBoardIndex;			// 게시물의 인덱스
	ACE_UINT64		nFileIdx;	
	ACE_UINT16		nDownSpeed;		
	ACE_UINT64		nDownStartPoint;
	char			pServerInfo[50];
} 
LOADDOWN,   *PLOADDOWN;

// 다운로드 완료후 공유포인트와 다운로드횟수 증가
typedef struct 
{
	ACE_UINT32		nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	ACE_UINT64		nBoardIndex_;			// 구매테이블의 인덱스
	ACE_UINT64		nBoardIndex;			// 게시물의 인덱스
	ACE_UINT64		nFileIdx;
	ACE_UINT64		nSizeBegin;
	ACE_UINT64		nSizeEnd;
} 
CHECKDOWNEND,   *PCHECKDOWNEND;

// 실제
typedef struct 
{
	ACE_UINT64 nDownLoaded_Now;
} 
DOWNLOADED_NOW,   *PDOWNLOADED_NOW;


// 파일 업로드시 뮤레카 체크
typedef struct 
{
	ACE_UINT64		nBoardIndex;						//게시물의 인덱스
	ACE_UINT64		uFile_code;							//파일게시글 번호
	ACE_UINT64		uVideo_id;							//비디오 ID
	ACE_UINT64		uMusic_id;							//음악 ID

	int				nResult;							// 체크값

	int				uFiletype;							//파일분류
	int				uMoney;								//금액
	int				uCounts;							//총개수

	char			uCp_id[55];					//CP사 ID
	char			uStatus[7];					//상태값
	char			uHash[70];					//해쉬값
	char			uHash_master[70];					//해쉬값
	char			uTitle[205];				//제목
	char			uVideo_content_id[205];		//비디오컨텐츠아이디
	char			uVideo_code[55];			//비디오코드
	char			uMusic_content_id[65];		//음악컨텐츠 아이디
	char			uMusic_company[25];			//음악업체명
	char			uCp_company[205];			//제휴사명
	char			uMovie_content_id[25];		//영제협컨텐츠ID
	char			uMovie_code[25];			//영제협시퀸스번호

	int				uMovie_cha;					//회차
	int				uMovie_jibun;				//OSP지분률
	int				uMovie_onair_date;			//방영일/개봉일

	//로그저장 필요항목(120829)
	char			ufiltering_key[58];			// 필터링 키
	char			usmartblock_key[58];			// 필터링 키
	char			uDownpurchase[20];			//osp구매번호
	char			uDownloader[100];			//다운로더
	char			uUploader[100];				//업로더
	int				uRedownload;				//다시받기
	int				uTurn;						//순번 - 캔들용 동시 값때문에
	
} 
UPLOADMUREKA,   *PUPLOADMUREKA;







// 파일 업로드시 뮤레카 체크
typedef struct 
{
	char pVideoID[22];
	char pPayDate[20];
	char pOspUserID[100];
	char pPayPrice[8];
	char pOspPayPrice[8];	
	char pOspPayPrice_one[8];	
	char pOspPayNumber[20];
	char pTitle[205];
	char pCha[12];
	char pOnAirDate[10];
	char pRightID[255];
	char pOspJibun[12];
	char pMurekaHash[70];
	char pFiltering_key[58];
	char pOspUPLOADERUserID[100];
} 
PAYLOGMUREKA;




#endif