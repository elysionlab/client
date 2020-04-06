#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		
#endif


#ifndef WINVER				
#define WINVER 0x0400		
#endif

#ifndef _WIN32_WINNT	
#define _WIN32_WINNT 0x0400		
#endif						

#ifndef _WIN32_WINDOWS		
#define _WIN32_WINDOWS 0x0410 
#endif

#ifndef _WIN32_IE			
#define _WIN32_IE 0x0400	
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	
#define _AFX_ALL_WARNINGS

#include <afxwin.h>       
#include <afxext.h>         

#include <afxdtctl.h>		
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>		
#endif 

//20080424 vs8로 컨버팅으로 인한 수정, jyh
//#include <afxsock.h>
#include <winsock2.h>



#include "Resource.h"
#include "config.h"
#include "config_protocol.h"
#include "config_struct.h"
#include <afxdhtml.h>



// 기본설정
#define LOGINCHECK_TIME				60000			// 중복접속 체크 간격(세션이 살아 있는지)
#define FIXMODECHECK_TIME			300000			// 정액모드 유효체크(라이트정액제 시간체크)
#define PACKETPOINT_TIME			30000			// 패킷포인트 존재 유무 체크 간격[패킷이 떨어지면 시작]
#define FIXMODE_ALERT_SIZE			10240000000		// 정액모드일 때 일정 사이즈마다 확인을 클릭해야만 다시 시작[10GB]
#define PACKET_P_TRANSSIZE			1048576			// 1패킷에 해당하는 전송사이즈[1MB]


// 파일 다운로드 정보 구조체
typedef struct RDOWNCONTEXT
{
	int				nTransState;			// 다운로드 상태
	int				nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	int				nCountDefer;			// 보류된 횟수
	int				nCountCloseSocket;		// 전송도중 끊어진 횟수
	int				nServerVolume;			//20080213 다운 받을 서버의 주소, jyh
	int				nFree;					//20080319 다운 상태(1=무료, 2=고속), jyh

	__int64			nBoardIndex;			// 게시물의 인덱스
	__int64			nBoardIndex_;			// 구매게시물의 인덱스
	__int64			nFileIndex;				// 자료의 인덱스
	__int64			nFileSizeReal;			// 실제 사이즈
	__int64			nFileSizeStart;			// 전송 시작시 사이즈(클라이언트 기준)
	__int64			nFileSizeEnd;			// 전송 종료시 사이즈(클라이언트 기준)

	char			pFileName[MAX_PATH];	// 파일 이름
	char			pFolderName[4096];		// 파일의 경로(게시판에서 선택된 경로)
	char			pSavePath[4096];		// 저장될 폴더(기본폴더 또는 선택된 폴더)
	char			pFullPath[4096];		// 저장될 전체 경로(파일 이름 포함)
	char			pServer_real_FileName[285];			
	bool			Grid_use;
	char			m_pServerInfo[50];	    //Grid 접속 오류시 원본 서버 접속시 필요 정보

} RDOWNCTX, *PRDOWNCTX;


// 파일 덮어쓰기, 이어받기 결정을 위한 구조체
typedef struct RFORSet
{	
	char			pFileName[4096];
	__int64			nFileSize;
	__int64			nTrSize;

} FILETRANSE_OVER, *PFILETRANSE_OVER;

//20080311 컨텐츠별 아이템 갯수 저장 구조체, jyh
typedef struct CONTENTSITEM
{
	int nBoardIndex;
	int nItemCnt;
} CONITEM;
