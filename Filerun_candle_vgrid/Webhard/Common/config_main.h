#pragma once

#include "config.h"
#include "config_protocol.h"
#include "config_struct.h"

// 상위 우측의 뷰모드
#define LIST_VIEW					0
#define WEB_VIEW					1
#define SEARCH_VIEW					2

// 검색모드
#define SEARCH_FILE					0
#define SEARCH_FOLDER				1
#define SEARCH_ID					2


// 폴더의 쓰기 삭제
#define TARGET_WRITE_DELETE			1
#define TARGET_WRITE				2
#define TARGET_DISABLE				3


#define ROOT_TITLE1					"짱하드"
#define ROOT_TITLE1_2				"내 폴더"
#define ROOT_TITLE1_3				"내 친구"
#define ROOT_TITLE1_4				"내가 운영중인 클럽"
#define ROOT_TITLE1_5				"내가 가입한 클럽"
#define ROOT_TITLE2					"검색"



// 각 노드별 설정
#define ROOTIMTECNT					4	
#define TREE_ROOT_1					0	// 짱디스크
#define TREE_ROOT_2					1	// 짱검색
#define TREE_SUB_MYFOLDER			2   // 내 폴더
#define TREE_SUB_FRIEND				3	// 내 친구폴더
#define TREE_SUB_CLUB_OWNER			4	// 내가 주인인 클럽
#define TREE_SUB_CLUB_MEMBER		5	// 내가 가입한 클럽


// 드래그 가능한 아이템
#define TREE_SUB_MYFOLDER_CHILD		20	// 내 폴더의 자식폴더
#define TREE_SUB_FRIEND_CHILD_ID	21	// 내 친구 폴더의 아이디
#define TREE_SUB_FRIEND_CHILD_MAIN	22	// 내 친구 폴더의 루트폴더
#define TREE_SUB_FRIEND_CHILD_SUB	23	// 내 친구 폴더의 서브폴더
#define TREE_SUB_CLUB_CHILD			24  // 운영/가입 클럽목록


///////////////////////////////////////////////////////////////////////////////////////
// 이미지 리스트
///////////////////////////////////////////////////////////////////////////////////////
#define ITEM_IMG_PARENT				0	// 상위 노드로 이동
#define ITEM_IMG_TOP				1	// 최상위 노드
#define ITEM_IMG_MYFOLDER			3	// 내폴더 노드
#define ITEM_IMG_FRIEND				11	// 내친구 노드
#define ITEM_IMG_CLUB_OWNER			15	// 내가 주인인 클럽
#define ITEM_IMG_CLUB_MEMBER		17	// 내가 가입한 클럽
#define ITEM_IMG_CLUB_LIST			19	// 클럽리스트
#define ITEM_IMG_FOLDER_NORMAL		5	// 일반폴더
#define ITEM_IMG_FOLDER_SHARE		7	// 공유폴더
#define ITEM_IMG_FOLDER_SHARE19		9	// 공유폴더(19세이상)
#define ITEM_IMG_USER_LIST			13	// 공유추가된 아이디
#define ITEM_IMG_SEARCH				21	// 검색노드


///////////////////////////////////////////////////////////////////////////////////////
// 드래그 앤 드랍시 마우스 커서
///////////////////////////////////////////////////////////////////////////////////////
#define CURSOR_ENABLE				IDC_ICON
#define CURSOR_DISABLE				IDC_NO

#define MAX_PATH_INI				2048



///////////////////////////////////////////////////////////////////////////////////////
// 폴더정보(공유폴더 포함) 
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RFOLDERCONTEXT
{
	__int64			folder_idx;					// 폴더 idx
	__int64			folder_share_idx;			// 공유 idx
	__int64			folder_parentidx;			// 상위 idx
	char			folder_userid[31];			// 폴더 소유자
	char			folder_name[256];			// 폴더 이름
	char			folder_info[256];			// 폴더 정보	
	char			folder_makedate[26];		// 폴더 생성일
	int				folder_share;				// 폴더 공유 설정(0:공유안함, 1:공유, 2:특정아이디 공유) : 
	int				folder_type;				// 서브 폴더인지 결정
	int				folder_image;
	char			folder_path[2048];			// 경로 : SEP를 분리자로
	HTREEITEM		folder_item_parent;			// 부모(수정/삭제/추가시에 사용)
	HTREEITEM		folder_item;				// 자신
	int				folder_findEA;				// 검색결과 개수

	DWORD_PTR		next;						// 다음 폴더의 포인터
	
} FolderInfo, *PtrFolderInfo;


///////////////////////////////////////////////////////////////////////////////////////
// 파일정보
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RFILECONTEXT
{
	__int64			file_idx;					// 파일 idx
	__int64			file_folder_idx;			// 폴더 idx	
	__int64			file_user_idx;				// 회원 idx	
	__int64			file_size;					// 파일의 사이즈
	__int64			file_realsize;				// 파일의 실제 사이즈(서버에 저장되어 있는 파일의 크기)
	int				file_downcnt;				// 파일 다운로드 카운트[원본의 다운로드 카운터]
	char			file_realname[256];			// 파일의 실제 파일명	
	char			file_validdate[20];			// 파일의 유효기간
	char			file_info[1025];			// 파일 정보
	int				file_findEA;				// 검색결과 개수

	DWORD_PTR		next;						// 다음 폴더의 포인터
	
} FileInfo, *PtrFileInfo;

///////////////////////////////////////////////////////////////////////////////////////
// 내컴퓨터 파일/폴더
///////////////////////////////////////////////////////////////////////////////////////
typedef struct MYCOMCONTEXT
{	
	HTREEITEM		my_item;					// 자신
	char			fullpath[1024];				// 파일/폴더의 절대경로
	
} MyComContext, *PMyComContext;

#define FF_JDISKFOLDER				1			// 짱디스크의 폴더	
#define FF_JDISKFILE				2			// 짱디스크의 파일
#define FF_JDISKFIND				3			// 짱디스크의 검색
#define FF_JDISKSHARE				4			// 짱디스크의 공유폴더
#define FF_MYCOMFOLDER				5			// 내컴퓨터의 폴더
#define FF_MYCOMFILE				6			// 내컴퓨터의 파일

///////////////////////////////////////////////////////////////////////////////////////
// 파일 / 폴더 이동/복사를 위한 리스트
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RFILEFOLDERCONTEXT
{
	int				ffmode;						// 짱디스크의 폴더:0, 파일:1
												// 내컴퓨터의 폴더:2, 파일:3

	char			spath[4096];				// 현재의 위치
	bool			isCut;						// 잘라내기 인지 체크
	__int64			targetidx;					// 이동(복사)할 목적지 파일/폴더 테이블의 idx

	HTREEITEM		m_itemold;					// 원본이 있는 아이템 ( 새로고침을 위해 )
	HTREEITEM		m_itemnew;					// 대상이 있는 아이템 ( 새로고침을 위해 )

	DWORD_PTR		ffdata;						// 파일 또는 폴더의 포인터
	DWORD_PTR		next;						// 다음 폴더의 포인터
	
} RFFContext, *PRFFContext;

///////////////////////////////////////////////////////////////////////////////////////
// 드래그 앤 드랍에서 사용될 모드
///////////////////////////////////////////////////////////////////////////////////////
#define DRAG_REMOTE_FOLDER			0
#define DRAG_LOCAL_FOLDER			1
#define DRAG_REMOTE_FILE			2
#define DRAG_LOCAL_FILE				3
#define DRAG_SEARCH					4


///////////////////////////////////////////////////////////////////////////////////////
// 파일 / 폴더 이동/복사를 위한 리스트
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RDNDCONTEXT
{
	int				m_dragmode;					// 드래그 된 모드
	int				m_dropmode;					// 드랍될 모드
	
	HTREEITEM		m_itemold;					// 원본이 있는 아이템 ( 새로고침을 위해 )
	HTREEITEM		m_itemnew;					// 대상이 있는 아이템 ( 새로고침을 위해 )

	int				m_nDragIndex;
	int				m_nDropIndex;
	
} RDropContext, *PRDropContext;









///////////////////////////////////////////////////////////////////////////////////////
// 로그인 정보
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RSESSION
{
	// 계정정보
	bool			bAuth;
	bool			bAdult;

	char			pUserID[35];
	char			pUserPW[35];
	char			pFixInfo[100];
	DWORD			nLoginMode;		

	int				nLoginResult;

	__int64			nDiskHave;
	__int64			nDiskUse;
	__int64			nHavePacket;


	// 윈도우 정보
	DWORD			bSavePassword;
	DWORD			nWindow_X;
	DWORD			nWindow_Y;
	DWORD			nWindow_W;
	DWORD			nWindow_H;
	DWORD			nTopPaneHeight;	// 첫번째 Row 높이 : 기본값
	DWORD			nLTopPaneWidth;	// 첫번째 Col 넓이 : 기본값
	DWORD			bLocalView;		// 내 컴퓨터를 보여줄것인지 기본값 : 보여줌

	// 기본정보
	char			pWebUrl[409600];
	char			pLocalPath[MAX_PATH];	

	// 이동 복사를 위한 구조체
	PRFFContext		pQueuePtr;		

	// 아이템 관련
	HTREEITEM		pRootItem[ROOTIMTECNT];			// 리모트 트리 영역 아이템
	char			pRootItemName[ROOTIMTECNT][30];

	// 이미지 갯수
	int				nImage_Count;

	// 메인 프레임에서 사용될 드래그 앤 드랍
	PRDropContext	pDragInfo;
	CWnd*			pDragWnd;			// drag wnd
	CWnd*			pDropWnd;			// drop wnd
	BOOL			bDragging;			// is dragging
	CImageList* 	pDragImage;			// drag img
	int				nDragWType;
	int				nDropWType;
	int				nDragIndex;	
	int				nDropIndex;
	HTREEITEM		pDragitem;	
	HTREEITEM		pDropitem;	
	int				m_nowCursor;
	bool			bDropAble;

	
} RCSESSION;
