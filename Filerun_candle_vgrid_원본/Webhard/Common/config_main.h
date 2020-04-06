#pragma once

#include "config.h"
#include "config_protocol.h"
#include "config_struct.h"

// ���� ������ ����
#define LIST_VIEW					0
#define WEB_VIEW					1
#define SEARCH_VIEW					2

// �˻����
#define SEARCH_FILE					0
#define SEARCH_FOLDER				1
#define SEARCH_ID					2


// ������ ���� ����
#define TARGET_WRITE_DELETE			1
#define TARGET_WRITE				2
#define TARGET_DISABLE				3


#define ROOT_TITLE1					"¯�ϵ�"
#define ROOT_TITLE1_2				"�� ����"
#define ROOT_TITLE1_3				"�� ģ��"
#define ROOT_TITLE1_4				"���� ����� Ŭ��"
#define ROOT_TITLE1_5				"���� ������ Ŭ��"
#define ROOT_TITLE2					"�˻�"



// �� ��庰 ����
#define ROOTIMTECNT					4	
#define TREE_ROOT_1					0	// ¯��ũ
#define TREE_ROOT_2					1	// ¯�˻�
#define TREE_SUB_MYFOLDER			2   // �� ����
#define TREE_SUB_FRIEND				3	// �� ģ������
#define TREE_SUB_CLUB_OWNER			4	// ���� ������ Ŭ��
#define TREE_SUB_CLUB_MEMBER		5	// ���� ������ Ŭ��


// �巡�� ������ ������
#define TREE_SUB_MYFOLDER_CHILD		20	// �� ������ �ڽ�����
#define TREE_SUB_FRIEND_CHILD_ID	21	// �� ģ�� ������ ���̵�
#define TREE_SUB_FRIEND_CHILD_MAIN	22	// �� ģ�� ������ ��Ʈ����
#define TREE_SUB_FRIEND_CHILD_SUB	23	// �� ģ�� ������ ��������
#define TREE_SUB_CLUB_CHILD			24  // �/���� Ŭ�����


///////////////////////////////////////////////////////////////////////////////////////
// �̹��� ����Ʈ
///////////////////////////////////////////////////////////////////////////////////////
#define ITEM_IMG_PARENT				0	// ���� ���� �̵�
#define ITEM_IMG_TOP				1	// �ֻ��� ���
#define ITEM_IMG_MYFOLDER			3	// ������ ���
#define ITEM_IMG_FRIEND				11	// ��ģ�� ���
#define ITEM_IMG_CLUB_OWNER			15	// ���� ������ Ŭ��
#define ITEM_IMG_CLUB_MEMBER		17	// ���� ������ Ŭ��
#define ITEM_IMG_CLUB_LIST			19	// Ŭ������Ʈ
#define ITEM_IMG_FOLDER_NORMAL		5	// �Ϲ�����
#define ITEM_IMG_FOLDER_SHARE		7	// ��������
#define ITEM_IMG_FOLDER_SHARE19		9	// ��������(19���̻�)
#define ITEM_IMG_USER_LIST			13	// �����߰��� ���̵�
#define ITEM_IMG_SEARCH				21	// �˻����


///////////////////////////////////////////////////////////////////////////////////////
// �巡�� �� ����� ���콺 Ŀ��
///////////////////////////////////////////////////////////////////////////////////////
#define CURSOR_ENABLE				IDC_ICON
#define CURSOR_DISABLE				IDC_NO

#define MAX_PATH_INI				2048



///////////////////////////////////////////////////////////////////////////////////////
// ��������(�������� ����) 
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RFOLDERCONTEXT
{
	__int64			folder_idx;					// ���� idx
	__int64			folder_share_idx;			// ���� idx
	__int64			folder_parentidx;			// ���� idx
	char			folder_userid[31];			// ���� ������
	char			folder_name[256];			// ���� �̸�
	char			folder_info[256];			// ���� ����	
	char			folder_makedate[26];		// ���� ������
	int				folder_share;				// ���� ���� ����(0:��������, 1:����, 2:Ư�����̵� ����) : 
	int				folder_type;				// ���� �������� ����
	int				folder_image;
	char			folder_path[2048];			// ��� : SEP�� �и��ڷ�
	HTREEITEM		folder_item_parent;			// �θ�(����/����/�߰��ÿ� ���)
	HTREEITEM		folder_item;				// �ڽ�
	int				folder_findEA;				// �˻���� ����

	DWORD_PTR		next;						// ���� ������ ������
	
} FolderInfo, *PtrFolderInfo;


///////////////////////////////////////////////////////////////////////////////////////
// ��������
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RFILECONTEXT
{
	__int64			file_idx;					// ���� idx
	__int64			file_folder_idx;			// ���� idx	
	__int64			file_user_idx;				// ȸ�� idx	
	__int64			file_size;					// ������ ������
	__int64			file_realsize;				// ������ ���� ������(������ ����Ǿ� �ִ� ������ ũ��)
	int				file_downcnt;				// ���� �ٿ�ε� ī��Ʈ[������ �ٿ�ε� ī����]
	char			file_realname[256];			// ������ ���� ���ϸ�	
	char			file_validdate[20];			// ������ ��ȿ�Ⱓ
	char			file_info[1025];			// ���� ����
	int				file_findEA;				// �˻���� ����

	DWORD_PTR		next;						// ���� ������ ������
	
} FileInfo, *PtrFileInfo;

///////////////////////////////////////////////////////////////////////////////////////
// ����ǻ�� ����/����
///////////////////////////////////////////////////////////////////////////////////////
typedef struct MYCOMCONTEXT
{	
	HTREEITEM		my_item;					// �ڽ�
	char			fullpath[1024];				// ����/������ ������
	
} MyComContext, *PMyComContext;

#define FF_JDISKFOLDER				1			// ¯��ũ�� ����	
#define FF_JDISKFILE				2			// ¯��ũ�� ����
#define FF_JDISKFIND				3			// ¯��ũ�� �˻�
#define FF_JDISKSHARE				4			// ¯��ũ�� ��������
#define FF_MYCOMFOLDER				5			// ����ǻ���� ����
#define FF_MYCOMFILE				6			// ����ǻ���� ����

///////////////////////////////////////////////////////////////////////////////////////
// ���� / ���� �̵�/���縦 ���� ����Ʈ
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RFILEFOLDERCONTEXT
{
	int				ffmode;						// ¯��ũ�� ����:0, ����:1
												// ����ǻ���� ����:2, ����:3

	char			spath[4096];				// ������ ��ġ
	bool			isCut;						// �߶󳻱� ���� üũ
	__int64			targetidx;					// �̵�(����)�� ������ ����/���� ���̺��� idx

	HTREEITEM		m_itemold;					// ������ �ִ� ������ ( ���ΰ�ħ�� ���� )
	HTREEITEM		m_itemnew;					// ����� �ִ� ������ ( ���ΰ�ħ�� ���� )

	DWORD_PTR		ffdata;						// ���� �Ǵ� ������ ������
	DWORD_PTR		next;						// ���� ������ ������
	
} RFFContext, *PRFFContext;

///////////////////////////////////////////////////////////////////////////////////////
// �巡�� �� ������� ���� ���
///////////////////////////////////////////////////////////////////////////////////////
#define DRAG_REMOTE_FOLDER			0
#define DRAG_LOCAL_FOLDER			1
#define DRAG_REMOTE_FILE			2
#define DRAG_LOCAL_FILE				3
#define DRAG_SEARCH					4


///////////////////////////////////////////////////////////////////////////////////////
// ���� / ���� �̵�/���縦 ���� ����Ʈ
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RDNDCONTEXT
{
	int				m_dragmode;					// �巡�� �� ���
	int				m_dropmode;					// ����� ���
	
	HTREEITEM		m_itemold;					// ������ �ִ� ������ ( ���ΰ�ħ�� ���� )
	HTREEITEM		m_itemnew;					// ����� �ִ� ������ ( ���ΰ�ħ�� ���� )

	int				m_nDragIndex;
	int				m_nDropIndex;
	
} RDropContext, *PRDropContext;









///////////////////////////////////////////////////////////////////////////////////////
// �α��� ����
///////////////////////////////////////////////////////////////////////////////////////
typedef struct RSESSION
{
	// ��������
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


	// ������ ����
	DWORD			bSavePassword;
	DWORD			nWindow_X;
	DWORD			nWindow_Y;
	DWORD			nWindow_W;
	DWORD			nWindow_H;
	DWORD			nTopPaneHeight;	// ù��° Row ���� : �⺻��
	DWORD			nLTopPaneWidth;	// ù��° Col ���� : �⺻��
	DWORD			bLocalView;		// �� ��ǻ�͸� �����ٰ����� �⺻�� : ������

	// �⺻����
	char			pWebUrl[409600];
	char			pLocalPath[MAX_PATH];	

	// �̵� ���縦 ���� ����ü
	PRFFContext		pQueuePtr;		

	// ������ ����
	HTREEITEM		pRootItem[ROOTIMTECNT];			// ����Ʈ Ʈ�� ���� ������
	char			pRootItemName[ROOTIMTECNT][30];

	// �̹��� ����
	int				nImage_Count;

	// ���� �����ӿ��� ���� �巡�� �� ���
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
