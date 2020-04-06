#ifndef _UPWINDOW_H
#define _UPWINDOW_H

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "registry.h"
#include "Worker.h"
#include "SkinDialog.h"
#include "..\src_client\up_client\SkinListCtrl.h"
#include "SelectFolderDialog.h"
#include "BTray.h"
#include "Dlg_Alert.h"
#include "HtmlViewer.h"
#include "..\..\..\__common\__unity[2]\skinprogress.h"
#include <fstream>
using namespace std;


class FtpClient;
class up_window : public CSkinDialog
{

public:

	up_window(CWnd* pParent = NULL);

	enum { IDD = IDD_UP_CLIENT_DIALOG };


protected:

	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
	void OnSysCommand(UINT nID, LPARAM lParam);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	afx_msg void OnTrayMenuOpen();	//열기메뉴 선택시
	afx_msg void OnTrayMenuHome();	//홈페이지메뉴 선택시
	afx_msg void OnTrayMenuExit();	//종료메뉴 선택시
	LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()


public:

	Worker*			m_pWorker_Ctrl;	
	FtpClient*		m_pFtpClint;
	CRegistry		m_pReg;
	CSkinListCtrl	m_pListCtrl;
	CImageList		m_pImageList;
	CImageList		m_pStateImageList;		//20080331 리스트 컨트롤의 row 높이를 늘리기 위해, jyh
	PRUPCTX			m_pPtrTrans;
	//SkinProgress	m_pCtrl_ProgressTotal;
	CRITICAL_SECTION m_cs;			
	CBTray*			m_pTray;
	Dlg_Alert*		m_pAlert;
	
	HtmlViewer*		m_pBanner;
	HtmlViewer*		m_pBtnHelp;				//20071004 문제해결 도움말, jyh
	HtmlViewer*		m_pSendComplete;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendKTHComplete1;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendKTHComplete2;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendKTHComplete3;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendKTHComplete4;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendKTHComplete5;		//20090304 컨텐츠 업로드 완료 통보, jyh
	
	//CLabel			m_pStatic_Time_Use;
	CLabel			m_pStatic_Time_Over;
	//CLabel			m_pStatic_FileCount;
	CLabel			m_pStatic_Speed;			//20070919 전송 속도, jyh
	CLabel			m_pStatic_FileName;			//20070919 전송 파일, jyh

public:

	CString			m_pCommandLine;

	CString			m_pUserID;
	CString			m_pUserPW;
	CString			m_strPrevFileName;	//20070928 다음 파일이름과 비교를 위해 현재 파일이름 저장, jyh

	bool			m_bInitWindow;

	__int64			m_nTransSpeed;
	__int64			m_nTransSpeed_Last;
	DWORD			m_nTime;			// 체크당시 시간
	DWORD			m_nTimeTotal;		// 전체 소요 시간(누적)
	DWORD			m_nTimeFileTime;	// 현재 파일 시간
	DWORD			m_nTimeFileStart;	// 현재 파일 시작시간
	DWORD			m_nTimeFileSpeed;	// 파일 전송 속도 업데이트 시간
	DWORD			m_nTimeFileUpdate;	// 파일 전송 상태 전송 시간
	DWORD			m_nTimeTransButton;	// 전송 관련 버튼 활성a화 여부
	

	__int64			m_nFileCount;
	__int64			m_nFileSizeTotal;	

	bool			m_bForceExit;
	bool			m_bFileTrans;
	bool			m_bAuth;

	CString			m_rPathBasic;
	CString			m_rPathLast;
	CString			m_rPathGomPlayer;
	DWORD			m_rDownOverWrite;	
	DWORD			m_rUpOverWrite;	
	DWORD			m_rUpEndAutoClose;
	DWORD			m_rDownEndAutoClose;
	DWORD			m_rUpAutoTrans;
	DWORD			m_rDownAutoTrans;
	DWORD			m_rDownAddItemPath;
	DWORD			m_rUpTransSpeed;
	DWORD			m_rDownTransSpeed;
	DWORD			m_rUpExitCheck;
	DWORD			m_rDownExitCheck;
	DWORD			m_rDownTransBuffer;
	DWORD			m_rUpTransBuffer;

	int				m_iStopItemIdx;		//20071030 전송 중지된 아이템 인덱스, jyh

	COLORREF		m_cProBar_BK;
	int				m_nBtnState;		//20071119 버튼 상태, jyh
	int				m_nCurTransFile;	//20071119 현재 전송중인 파일 인덱스, jyh
	char*			m_pWebItem;			//20071210 웹컨트롤에서 받은 업로드 아이템, jyh
	bool			m_bTransGura;		//20071211 구라 전송, jyh
	std::vector<CString> m_v_cstrGuraFilePath;	//20071212 구라 전송 파일 full path, jyh
	std::vector<CONITEM> m_v_sConItem;			//20071220 컨텐츠별 아이템 갯수 저장, jyh
	//20080121 송신 파일 사이즈 로컬 저장을 위한 변수
	fstream			m_fout;
	int				m_nFileType;
	__int64			m_nBoardIndex;
	__int64			m_nFileIndex;
	__int64			m_nFileSizeEnd;
	__int64			m_nFileSizeReal;

	BOOL			m_bMinWindow;		//20080131 UI가 최소창인지 여부, jyh
	int				m_nPercent;			//20080225 전체 파일의 전송 퍼센트, jyh
	int				m_nIEVer;			//20090303 사용자 인터넷 익스플로러 버전, jyh
	
		
public:

	afx_msg LRESULT MSG_Complete(WPARAM wParam, LPARAM lParam);	//20090304 컨텐츠 업로드완료 통보, jyh
	afx_msg LRESULT MSG_UpLoad(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_Window(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_OverWrite(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_ForceExit(WPARAM wParam, LPARAM lParam);
    
public:

	virtual void InitWindow();
	virtual void InitListControl();
	virtual void ChnPosition(int cx, int cy);
	virtual HBRUSH DrawCtrl(CDC* pDC, UINT nCtrlID);
	virtual bool Skin_ButtonPressed(CString m_ButtonName);

	DWORD DeferItem();	//20071219 전송 실패 아이템 순위변경, jyh
	void InsertItem(PRUPCTX pNewItem);
	void MoveItem(int mode);
	void DeleteItem(int mode);
	void DeleteList(int nMode);
	void Exit_Program(bool bRegWrite, bool bTrayCall);
	bool IsVistaOS();	//20080426 OS가 비스타인지, jyh
	BOOL AddMessageFilter(UINT* pMessages);	//20080426 비스타 메세지 필터, jyh
	//20090303 사용자의 익스플로러 버전 알아오는 함수, jyh
	int	GetVersionIE();
	BOOL SafeTerminateProcess(HANDLE hProcess, UINT uExitCode);	//20090528 프로세스 안전하게 강제종료, jyh

public:

	void UpLoadPro();
	void UpLoadProGura();	//20071211 구라 전송을 위해, jyh
	afx_msg void OnCbnSelchangeComboSpeed();
	

	bool ReSetTransInfo();
	bool ReSetTransInfoGura();			//20071211 구라 전송을 위해, jyh
	void GuraUpList(PUPHASH upHash);	//20071217 구라 리스트 생성, jyh
	void SetTransFlag(bool bFlag);
	void SetTransFlagGura(bool bFlag);	//20071211 구라 전송을 위해, jyh
	int  GetTransListIndex();		//전송중인 아이템의 인덱스를 얻는다.
	void SystemShutdownOTN();

	void FileTransLoop();
	friend unsigned int __stdcall ProcFileTransLoop(void *pParam);

public:

	void RegWrite();
	void RegRead();
	void GetFilePathInFolder(CString folderpath);	//20071212 폴더 안의 파일경로를 vector변수에 넣는다, jyh

	CString SetSizeFormat(__int64 nRealSize);	
	CString SetNumFormat(__int64 nNum);
	CString SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec);
	CString GetFileStatus(int nIdx);	//20071030 파일 상태 반환, jyh
    
	SkinProgress m_pCtrl_ProgressCur;
	SkinProgress m_pCtrl_ProgressTotal;
	//afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnPaint();
};


#endif /* _UPWINDOW_H */