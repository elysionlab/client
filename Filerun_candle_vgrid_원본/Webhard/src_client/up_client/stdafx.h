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
//20071224 SHGetSpecialFolderPath()가 필요로 하는 헤더파일 추가, jyh
#include <shlobj.h>



#include "Resource.h"
#include "config.h"
#include "config_protocol.h"
#include "config_struct.h"

//20090303 추가, jyh
//IWebBrowser2 COM을 사용하기 위해 꼭 선언
#pragma warning(disable:4192)
#pragma warning(disable:4146)
#pragma warning(disable:4278)
#import <mshtml.tlb> // Internet Explorer 5
#import <shdocvw.dll>
#include <afxdlgs.h>
#include <afxdhtml.h>
#include <afxhtml.h>



// 파일 전송 관련 구조체
typedef struct RUPCONTEXT
{	
	int				nTransState;			// 다운로드 상태
	int				nFileType;				// 해당 파일의 타입 : 0=거래소,1=블로그,2=개인자료
	int				nCountDefer;			//20071207 보류된 횟수, jyh

	__int64			nHashIndex;				//20071217 해시값 등록 테이블(hash_copyright)의 idx(구라 전송을 위해), jyh
	__int64			nBoardIndex;			// 게시물의 인덱스
	__int64			nFileIndex;				// 자료의 인덱스
	__int64			nFileSizeReal;			// 실제 사이즈
	__int64			nFileSizeStart;			// 전송 시작시 사이즈(클라이언트 기준)
	__int64			nFileSizeEnd;			// 전송 종료시 사이즈(클라이언트 기준)

	char			pFileName[MAX_PATH];	// 파일 이름
	char			pFolderName[4096];		// 파일의 경로(게시판에서 선택된 경로)
	char			pSavePath[4096];		// 저장될 폴더(기본폴더 또는 선택된 폴더)
	char			pFullPath[4096];		// 저장될 전체 경로(파일 이름 포함)

} RUPCTX, *PRUPCTX;


// 파일 덮어쓰기, 이어받기 결정을 위한 구조체
typedef struct RFORSet
{	
	char			pFileName[4096];
	__int64			nFileSize;
	__int64			nTrSize;

} FILETRANSE_OVER, *PFILETRANSE_OVER;

//20071220 컨텐츠별 아이템 갯수 저장 구조체, jyh
typedef struct CONTENTSITEM
{
	__int64 nBoardIndex;
	int nItemCnt;
} CONITEM;





