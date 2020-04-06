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


// ���� � ����
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






// ȸ������
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
	ACE_UINT32		nSpeed;		//20081022 ȸ���� ���ǵ� ��, jyh
} 
INFOAUTH,   *PINFOAUTH;







/* ���ε� ���� ����ü */

// ���ε� ���� ����üũ
typedef struct 
{
	ACE_UINT16		nResult;				// ��û���
	ACE_UINT32		nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT64		nBoardIndex;			// �Խù��� �ε���
	ACE_UINT64		nFileIdx;				// ���� �ε���	
	ACE_UINT64		nFileSize;				// ���� ���ϻ�����		
	ACE_UINT64		nFileSize_Complete;		// ��� �Ϸ������
} 
UPLOADPRE,   *PUPLOADPRE;


// ���� ���ε�
typedef struct 
{
	ACE_UINT16		nResult;				// ��û���(���ε���)
	ACE_UINT32		nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT64		nBoardIndex;			// �Խù��� �ε���
	ACE_UINT64		nFileIdx;				// ���� �ε���
	ACE_UINT64		nFileSize_Start;		// ���� ���� ������(�̾�ø��⿩��)
	ACE_UINT16		nUploadSpeed;			// ���ε� ����
	char			pFileCheckSum[33];		// ���� üũ��	
	char			pServerInfo[50];		// FTP ����
} 
UPLOADPRO, *PUPLOADPRO;


// ���� ���ε尡 �Ϸ�Ǿ��ٰ� �뺸
typedef struct 
{
	ACE_UINT32		nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT32		nContentsCmpl;			//20080123 ������ ���ε� �Ϸ�� 1, jyh
	ACE_UINT64		nBoardIndex;			// �Խù��� �ε���
	ACE_UINT64		nFileIdx;				// ���� �ε���
	ACE_UINT64		nFileSizeEnd;		    // �Ϸ�� ������		
} 
UPLOADEND,   *PUPLOADEND;

//20071210 ���ε��� �ؽð� �� ��û, jyh
typedef struct  
{
	int		nResult;				//��û ��� ��ȯ ����: 1, �׿� ����
	int		nFileType;				//�ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT64	bbs_no;				//�Խù� ��ȣ
	ACE_UINT64	hash_idx;			//���۱� ��� �������� �ε���
	//char	szPath[1024];			//�ؽð� ��� ���� �� ���� ���
	//char	szCheckSum[2048];		//������ �ؽð�
}
UPHASH,	*PUPHASH;


//20080507 ���� �ڷ� ���ε� ����Ʈ
typedef struct  
{
	ACE_UINT16		nResult;				//��û���
	ACE_UINT16		nContentsType;			//������ ����(0:�ʼ� �ڷ�, 1:�ڸ� �ڷ�, 2:A/S �ڷ�)
	ACE_UINT64		nContentsIdx;			//������ �ε���
	char			szFolderName[512];		//������
	char			szContentsInfo[2048];	//������ ����
}
OPENUPLIST, *POPENUPLIST;

//20080506 ���� �ڷ� ���ε� ����ü
typedef struct 
{
	ACE_UINT16		nResult;			//��û���
	ACE_UINT64		nFileIdx;			//���� �ε���
	ACE_UINT64		nFileSize;			//���� ������
	ACE_UINT64		nFileSizeStart;		//���� ���� ������(�̾�ø��⿩��)
	char			szServerInfo[50];	//���� ���� ����
}
OPENUPLOAD, *POPENUPLOAD;




//20080509 ���� �ڷ� �ٿ�ε� ����Ʈ
typedef struct  
{
	ACE_UINT16		nResult;				//��û���
	ACE_UINT16		nContentsType;			//������ ����(0:�ʼ� �ڷ�, 1:�ڸ� �ڷ�, 2:A/S �ڷ�)
	ACE_UINT64		nContentsIdx;			//������ �ε���
	char			szFolderName[512];		//������
	char			szContentsInfo[2048];	//������ ����
}
OPENDOWNLIST, *POPENDOWNLIST;

//20080509 ���� �ڷ� �ٿ�ε� ����ü
typedef struct 
{
	ACE_UINT16		nResult;			//��û���
	ACE_UINT64		nFileIdx;			//���� �ε���
	ACE_UINT64		nFileSize;			//���� ������
	ACE_UINT64		nDownStartPoint;	//�ٿ� ���� ������ ���� ������ ��ġ
	char			szServerInfo[50];	//���� ���� ����
}
OPENDOWNLOAD, *POPENDOWNLOAD;




/* �ٿ�ε� ���� ����ü */

// ���ϴٿ�ε� ���� ������ ������ �ش� ������ ���� ��û
typedef struct 
{
	ACE_INT16		nResult;
	ACE_UINT32		nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT64		nBoardIndex_;			// �������̺��� �ε���
	ACE_UINT64		nBoardIndex;			// �Խù��� �ε���
	ACE_UINT64		nFileIdx;				// ���� �ε���
	ACE_UINT64		nFileRealSize;			// ���� ���ϻ�����
} 
LOADPREDOWN,   *PLOADPREDOWN;

// ���ϴٿ�ε带 ���� ä�� ���� ��û
typedef struct 
{
	ACE_INT16		nResult;
	ACE_UINT32		nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT32		nServerVolume;			//20080213 �ٿ� ���� ������ �ּ�, jyh
	ACE_UINT64		nBoardIndex_;			// �������̺��� �ε���
	ACE_UINT64		nBoardIndex;			// �Խù��� �ε���
	ACE_UINT64		nFileIdx;	
	ACE_UINT16		nDownSpeed;		
	ACE_UINT64		nDownStartPoint;
	char			pServerInfo[50];
} 
LOADDOWN,   *PLOADDOWN;

// �ٿ�ε� �Ϸ��� ��������Ʈ�� �ٿ�ε�Ƚ�� ����
typedef struct 
{
	ACE_UINT32		nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	ACE_UINT64		nBoardIndex_;			// �������̺��� �ε���
	ACE_UINT64		nBoardIndex;			// �Խù��� �ε���
	ACE_UINT64		nFileIdx;
	ACE_UINT64		nSizeBegin;
	ACE_UINT64		nSizeEnd;
} 
CHECKDOWNEND,   *PCHECKDOWNEND;

// ����
typedef struct 
{
	ACE_UINT64 nDownLoaded_Now;
} 
DOWNLOADED_NOW,   *PDOWNLOADED_NOW;


// ���� ���ε�� �·�ī üũ
typedef struct 
{
	ACE_UINT64		nBoardIndex;						//�Խù��� �ε���
	ACE_UINT64		uFile_code;							//���ϰԽñ� ��ȣ
	ACE_UINT64		uVideo_id;							//���� ID
	ACE_UINT64		uMusic_id;							//���� ID

	int				nResult;							// üũ��

	int				uFiletype;							//���Ϻз�
	int				uMoney;								//�ݾ�
	int				uCounts;							//�Ѱ���

	char			uCp_id[55];					//CP�� ID
	char			uStatus[7];					//���°�
	char			uHash[70];					//�ؽ���
	char			uHash_master[70];					//�ؽ���
	char			uTitle[205];				//����
	char			uVideo_content_id[205];		//�������������̵�
	char			uVideo_code[55];			//�����ڵ�
	char			uMusic_content_id[65];		//���������� ���̵�
	char			uMusic_company[25];			//���Ǿ�ü��
	char			uCp_company[205];			//���޻��
	char			uMovie_content_id[25];		//������������ID
	char			uMovie_code[25];			//��������������ȣ

	int				uMovie_cha;					//ȸ��
	int				uMovie_jibun;				//OSP���з�
	int				uMovie_onair_date;			//�濵��/������

	//�α����� �ʿ��׸�(120829)
	char			ufiltering_key[58];			// ���͸� Ű
	char			usmartblock_key[58];			// ���͸� Ű
	char			uDownpurchase[20];			//osp���Ź�ȣ
	char			uDownloader[100];			//�ٿ�δ�
	char			uUploader[100];				//���δ�
	int				uRedownload;				//�ٽùޱ�
	int				uTurn;						//���� - ĵ��� ���� ��������
	
} 
UPLOADMUREKA,   *PUPLOADMUREKA;







// ���� ���ε�� �·�ī üũ
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