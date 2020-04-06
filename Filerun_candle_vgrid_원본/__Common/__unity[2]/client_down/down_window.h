

#ifndef _DOWNWINDOW_H
#define _DOWNWINDOW_H

#include <vector>
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "registry.h"
#include "Worker.h"
#include "SkinDialog.h"
#include "..\src_client\down_client\SkinListCtrl.h"
#include "SelectFolderDialog.h"
#include "BTray.h"
#include "Dlg_Alert.h"
#include "HtmlViewer.h"
#include "..\..\..\__common\__unity[2]\skinprogress.h"
#include "..\src_client\down_client\pictureex.h"

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

class FtpClient;
class down_window : public CSkinDialog
{


public:

	down_window(CWnd* pParent = NULL);
	enum { IDD = IDD_DOWN_CLIENT_DIALOG };


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
	CImageList		m_pStateImageList;		//20080319 다운 상태 이미지(고속, 무료), jyh
	PRDOWNCTX		m_pPtrTrans;
	//SkinProgress	m_pCtrl_ProgressTotal;
	CRITICAL_SECTION m_cs;		
	CBTray*			m_pTray;
	Dlg_Alert*		m_pAlert;	

	HtmlViewer*		m_pBanner;
	HtmlViewer*		m_pBtnHelp;				//20071004	문제해결 도움말, jyh
	
	HtmlViewer*		m_pSendiMBCComplete1;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendiMBCComplete2;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendiMBCComplete3;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendiMBCComplete4;		//20090304 컨텐츠 업로드 완료 통보, jyh
	HtmlViewer*		m_pSendiMBCComplete5;		//20090304 컨텐츠 업로드 완료 통보, jyh

	//CLabel			m_pStatic_Time_Use;		//20070918 jyh
	CLabel			m_pStatic_Time_Over;
	//CLabel			m_pStatic_FileCount;		//20070918 jyh
	CLabel			m_pStatic_Speed;			//20070919 전송 속도, jyh
	CLabel			m_pStatic_Path;				//20070919 저장 경로, jyh
	CLabel			m_pStatic_FileName;			//20070919 전송 파일, jyh


public:

	CString			m_pCommandLine;
	CString			m_pUserID;
	CString			m_pUserPW;
	CString			m_strPrevPath;		//20070919 다음 전송경로와 비교를 위해 현재 경로 저장, jyh
	CString			m_strPrevFileName;	//20070928 다음 파일이름과 비교를 위해 현재 파일이름 저장, jyh

	bool			m_bInitPreItem;
	bool			m_bInitWindow;

	__int64			m_nTransSpeed;
	__int64			m_nTransSpeed_Last;
	DWORD			m_nTime;			// 체크당시 시간
	DWORD			m_nTimeTotal;		// 전체 소요 시간(누적)
	DWORD			m_nTimeFileTime;	// 현재 파일 시간
	DWORD			m_nTimeFileStart;	// 현재 파일 시작시간
	DWORD			m_nTimeFileSpeed;	// 파일 전송 속도 업데이트 시간
	DWORD			m_nTimeDownState;	// 다운로드 상태 전송 시간
	DWORD			m_nTimeTransButton;	// 전송 관련 버튼 활성화 여부

	__int64			m_nFileCount;
	__int64			m_nFileSizeTotal;
	__int64			m_nFileSizeEnd;		//전송 파일 사이즈 누적

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
	DWORD			m_rCreateShortcut;	//20071002 바로가기, jyh

	
	COLORREF		m_cProBar_BK;
	std::vector<CString>	m_v_sItem;	//20071117 전송 받은 아이템 번호, jyh
	int				m_iItemCnt;			//20071117 전송 요청 받은 아이템 수, jyh
	int				m_nBtnState;		//20071119 버튼 상태, jyh
	int				m_nCurTransFile;	//20071119 현재 전송중인 파일 인덱스, jyh
	BOOL			m_bMinWindow;		//20080131 UI가 최소창인지 여부, jyh
	int				m_nPercent;			//20080225 전체 파일의 전송 퍼센트, jyh
	int				m_nNameAuth;		//20080313 실명 인증 여부(0: 인증 안됨, 1: 인증), jyh
	std::vector<CONITEM> m_v_sConItem;	//20080311 컨텐츠별 아이템 갯수 저장, jyh
	BOOL			m_bFreeDown;		//20080320 현재 전송중인 파일의 다운 상태, jyh
	int				m_nSpeed;			//20081022 파일 서버 접속 계정 스피드, jyh
	
	//뮤레카
	int Mureka_use;
	ACE_UINT16		m_nSvrResult;
	double          m_nGridSpeed;
	__int64          m_nGridSpeed_mem;

public:

	afx_msg LRESULT MSG_Window(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_DownLoad(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_OverWrite(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_ForceExit(WPARAM wParam, LPARAM lParam);



public:

	virtual void InitWindow();
	virtual void InitListControl();
	virtual void ChnPosition(int cx, int cy);
	virtual HBRUSH DrawCtrl(CDC* pDC, UINT nCtrlID);
	virtual bool Skin_ButtonPressed(CString m_ButtonName);

	void InsertItem(PRDOWNCTX pNewItem);
	void MoveItem(int mode);
	void DeleteItem(int mode);
	void DeleteList(int nMode);
	void Exit_Program(bool bRegWrite, bool bTrayCall);		

public:
		
	void DownLoadPro();
	afx_msg void OnCbnSelchangeComboSpeed();
	
	bool ReSetTransInfo();
	void SetTransFlag(bool bFlag);	
	int  GetTransListIndex();		//전송중인 아이템 인덱스
	DWORD DeferItem();
	void SystemShutdownOTN();

	void FileTransLoop();
	friend unsigned int __stdcall ProcFileTransLoop(void *pParam);

	//121026 - IE제외한 브라우져 다운로드 - 특수데이터처리(URL다운로드)
	void Download_List_update();
	CString Download_List_Items;
	bool Download_type_link;

public:	

	CString SetSizeFormat(__int64 nRealSize);
	CString SetNumFormat(__int64 nNum);
	CString SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec);
	CString SetSpeedFormatKGrid(double tranSpeed,DWORD nowsec, DWORD startsec);
	__int64 GetDiskFreeSpaceMMSV(CString targetfolder);
	
	void RegWrite();
	void RegRead();
	bool CreateShortcut(char* strTemp);	//20071002 바로가기, jyh
	CString GetFileStatus(int nIdx);	//20071030 파일 상태 반환, jyh
	//void DrawDownState();				//20080320 다운로드 상태 이미지 출력, jyh
	bool IsVistaOS();					//20080426 OS가 비스타인지, jyh
	BOOL AddMessageFilter(UINT* pMessages);	//20080426 비스타 메세지 필터, jyh
	BOOL SafeTerminateProcess(HANDLE hProcess, UINT uExitCode);	//20090528 프로세스 안전하게 강제종료, jyh
	
	SkinProgress m_pCtrl_ProgressCur;
	SkinProgress m_pCtrl_ProgressTotal;
	//CPictureEx m_Picture;				//20080322 gif 애니매이션, jyh
	afx_msg void OnPaint();
};


#endif /* _DOWNWINDOW_H */