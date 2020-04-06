#pragma once

#include "resource.h"     
#include <atlhost.h>
#include <afxcmn.h>
#include "Registry.h"
#include "WebBBS.h"
#include "config.h"


#define WM_USER_INSTALL_END WM_USER + 100


class FileUpdateWindow : 
	public CAxDialogImpl<FileUpdateWindow>
{

public:

	FileUpdateWindow()
	{
		m_pRegPath		= CLIENT_REG_PATH;
		g_fAbortDownload= 0;
	}

	~FileUpdateWindow()
	{
		//m_pBmp.DeleteObject();
	}

	enum { IDD = IDD_FILEDOWN_DLG };


	BEGIN_MSG_MAP(FileUpdateWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButton1)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_USER_INSTALL_END, OnInstallComplete)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP(CAxDialogImpl<FileUpdateWindow>)
	END_MSG_MAP()


public:

	CWebBBS*		m_pMain;

	CStatic		m_pCtrl;
	CProgressCtrl m_pProgress;

	LONG		g_fAbortDownload;
	
	CString		m_pRegPath;
	CString		m_pLocalAppPath;
	CString		m_pClientVerPath;
	CString		m_pServerVerPath;
	CString		m_pClientSetPath;
	CString		m_pVersionRemote;
	CString		m_pVersionLocal;



	/*
	CBitmap		m_pBmp;
	HBITMAP		m_hBmp;

	int			m_nDefaultWidth;
	int			m_nDefaultHeight;
	*/

public:

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInstallComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:

	void DownState(TCHAR* strState , const int nPercentDone);
	bool MakeFolder(char* pszDir);
	void WorkerThreadProc(int nMode);


	//HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010);


	friend unsigned int __stdcall ProcNewVersion(void *pParam);
	
};


