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

//20080424 vs8�� ���������� ���� ����, jyh
//#include <afxsock.h>
#include <winsock2.h>



#include "Resource.h"
#include "config.h"
#include "config_protocol.h"
#include "config_struct.h"
#include <afxdhtml.h>



// �⺻����
#define LOGINCHECK_TIME				60000			// �ߺ����� üũ ����(������ ��� �ִ���)
#define FIXMODECHECK_TIME			300000			// ���׸�� ��ȿüũ(����Ʈ������ �ð�üũ)
#define PACKETPOINT_TIME			30000			// ��Ŷ����Ʈ ���� ���� üũ ����[��Ŷ�� �������� ����]
#define FIXMODE_ALERT_SIZE			10240000000		// ���׸���� �� ���� ������� Ȯ���� Ŭ���ؾ߸� �ٽ� ����[10GB]
#define PACKET_P_TRANSSIZE			1048576			// 1��Ŷ�� �ش��ϴ� ���ۻ�����[1MB]


// ���� �ٿ�ε� ���� ����ü
typedef struct RDOWNCONTEXT
{
	int				nTransState;			// �ٿ�ε� ����
	int				nFileType;				// �ش� ������ Ÿ�� : 0=�ŷ���,1=��α�,2=�����ڷ�
	int				nCountDefer;			// ������ Ƚ��
	int				nCountCloseSocket;		// ���۵��� ������ Ƚ��
	int				nServerVolume;			//20080213 �ٿ� ���� ������ �ּ�, jyh
	int				nFree;					//20080319 �ٿ� ����(1=����, 2=���), jyh

	__int64			nBoardIndex;			// �Խù��� �ε���
	__int64			nBoardIndex_;			// ���ŰԽù��� �ε���
	__int64			nFileIndex;				// �ڷ��� �ε���
	__int64			nFileSizeReal;			// ���� ������
	__int64			nFileSizeStart;			// ���� ���۽� ������(Ŭ���̾�Ʈ ����)
	__int64			nFileSizeEnd;			// ���� ����� ������(Ŭ���̾�Ʈ ����)

	char			pFileName[MAX_PATH];	// ���� �̸�
	char			pFolderName[4096];		// ������ ���(�Խ��ǿ��� ���õ� ���)
	char			pSavePath[4096];		// ����� ����(�⺻���� �Ǵ� ���õ� ����)
	char			pFullPath[4096];		// ����� ��ü ���(���� �̸� ����)
	char			pServer_real_FileName[285];			
	bool			Grid_use;
	char			m_pServerInfo[50];	    //Grid ���� ������ ���� ���� ���ӽ� �ʿ� ����

} RDOWNCTX, *PRDOWNCTX;


// ���� �����, �̾�ޱ� ������ ���� ����ü
typedef struct RFORSet
{	
	char			pFileName[4096];
	__int64			nFileSize;
	__int64			nTrSize;

} FILETRANSE_OVER, *PFILETRANSE_OVER;

//20080311 �������� ������ ���� ���� ����ü, jyh
typedef struct CONTENTSITEM
{
	int nBoardIndex;
	int nItemCnt;
} CONITEM;
