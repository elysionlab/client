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

	afx_msg void OnTrayMenuOpen();	//����޴� ���ý�
	afx_msg void OnTrayMenuHome();	//Ȩ�������޴� ���ý�
	afx_msg void OnTrayMenuExit();	//����޴� ���ý�
	LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()


public:

	Worker*			m_pWorker_Ctrl;	
	FtpClient*		m_pFtpClint;
	CRegistry		m_pReg;
	CSkinListCtrl	m_pListCtrl;
	CImageList		m_pImageList;
	CImageList		m_pStateImageList;		//20080331 ����Ʈ ��Ʈ���� row ���̸� �ø��� ����, jyh
	PRUPCTX			m_pPtrTrans;
	//SkinProgress	m_pCtrl_ProgressTotal;
	CRITICAL_SECTION m_cs;			
	CBTray*			m_pTray;
	Dlg_Alert*		m_pAlert;
	
	HtmlViewer*		m_pBanner;
	HtmlViewer*		m_pBtnHelp;				//20071004 �����ذ� ����, jyh
	HtmlViewer*		m_pSendComplete;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendKTHComplete1;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendKTHComplete2;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendKTHComplete3;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendKTHComplete4;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendKTHComplete5;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	
	//CLabel			m_pStatic_Time_Use;
	CLabel			m_pStatic_Time_Over;
	//CLabel			m_pStatic_FileCount;
	CLabel			m_pStatic_Speed;			//20070919 ���� �ӵ�, jyh
	CLabel			m_pStatic_FileName;			//20070919 ���� ����, jyh

public:

	CString			m_pCommandLine;

	CString			m_pUserID;
	CString			m_pUserPW;
	CString			m_strPrevFileName;	//20070928 ���� �����̸��� �񱳸� ���� ���� �����̸� ����, jyh

	bool			m_bInitWindow;

	__int64			m_nTransSpeed;
	__int64			m_nTransSpeed_Last;
	DWORD			m_nTime;			// üũ��� �ð�
	DWORD			m_nTimeTotal;		// ��ü �ҿ� �ð�(����)
	DWORD			m_nTimeFileTime;	// ���� ���� �ð�
	DWORD			m_nTimeFileStart;	// ���� ���� ���۽ð�
	DWORD			m_nTimeFileSpeed;	// ���� ���� �ӵ� ������Ʈ �ð�
	DWORD			m_nTimeFileUpdate;	// ���� ���� ���� ���� �ð�
	DWORD			m_nTimeTransButton;	// ���� ���� ��ư Ȱ��aȭ ����
	

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

	int				m_iStopItemIdx;		//20071030 ���� ������ ������ �ε���, jyh

	COLORREF		m_cProBar_BK;
	int				m_nBtnState;		//20071119 ��ư ����, jyh
	int				m_nCurTransFile;	//20071119 ���� �������� ���� �ε���, jyh
	char*			m_pWebItem;			//20071210 ����Ʈ�ѿ��� ���� ���ε� ������, jyh
	bool			m_bTransGura;		//20071211 ���� ����, jyh
	std::vector<CString> m_v_cstrGuraFilePath;	//20071212 ���� ���� ���� full path, jyh
	std::vector<CONITEM> m_v_sConItem;			//20071220 �������� ������ ���� ����, jyh
	//20080121 �۽� ���� ������ ���� ������ ���� ����
	fstream			m_fout;
	int				m_nFileType;
	__int64			m_nBoardIndex;
	__int64			m_nFileIndex;
	__int64			m_nFileSizeEnd;
	__int64			m_nFileSizeReal;

	BOOL			m_bMinWindow;		//20080131 UI�� �ּ�â���� ����, jyh
	int				m_nPercent;			//20080225 ��ü ������ ���� �ۼ�Ʈ, jyh
	int				m_nIEVer;			//20090303 ����� ���ͳ� �ͽ��÷η� ����, jyh
	
		
public:

	afx_msg LRESULT MSG_Complete(WPARAM wParam, LPARAM lParam);	//20090304 ������ ���ε�Ϸ� �뺸, jyh
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

	DWORD DeferItem();	//20071219 ���� ���� ������ ��������, jyh
	void InsertItem(PRUPCTX pNewItem);
	void MoveItem(int mode);
	void DeleteItem(int mode);
	void DeleteList(int nMode);
	void Exit_Program(bool bRegWrite, bool bTrayCall);
	bool IsVistaOS();	//20080426 OS�� ��Ÿ����, jyh
	BOOL AddMessageFilter(UINT* pMessages);	//20080426 ��Ÿ �޼��� ����, jyh
	//20090303 ������� �ͽ��÷η� ���� �˾ƿ��� �Լ�, jyh
	int	GetVersionIE();
	BOOL SafeTerminateProcess(HANDLE hProcess, UINT uExitCode);	//20090528 ���μ��� �����ϰ� ��������, jyh

public:

	void UpLoadPro();
	void UpLoadProGura();	//20071211 ���� ������ ����, jyh
	afx_msg void OnCbnSelchangeComboSpeed();
	

	bool ReSetTransInfo();
	bool ReSetTransInfoGura();			//20071211 ���� ������ ����, jyh
	void GuraUpList(PUPHASH upHash);	//20071217 ���� ����Ʈ ����, jyh
	void SetTransFlag(bool bFlag);
	void SetTransFlagGura(bool bFlag);	//20071211 ���� ������ ����, jyh
	int  GetTransListIndex();		//�������� �������� �ε����� ��´�.
	void SystemShutdownOTN();

	void FileTransLoop();
	friend unsigned int __stdcall ProcFileTransLoop(void *pParam);

public:

	void RegWrite();
	void RegRead();
	void GetFilePathInFolder(CString folderpath);	//20071212 ���� ���� ���ϰ�θ� vector������ �ִ´�, jyh

	CString SetSizeFormat(__int64 nRealSize);	
	CString SetNumFormat(__int64 nNum);
	CString SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec);
	CString GetFileStatus(int nIdx);	//20071030 ���� ���� ��ȯ, jyh
    
	SkinProgress m_pCtrl_ProgressCur;
	SkinProgress m_pCtrl_ProgressTotal;
	//afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnPaint();
};


#endif /* _UPWINDOW_H */