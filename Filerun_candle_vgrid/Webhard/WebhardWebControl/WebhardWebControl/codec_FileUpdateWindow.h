#pragma once

#include <afxcmn.h>
#include "WebBBS.h"

// Ccodec_FileUpdateWindow ��ȭ �����Դϴ�.

class Ccodec_FileUpdateWindow : public CDialog
{
	DECLARE_DYNAMIC(Ccodec_FileUpdateWindow)

public:
	Ccodec_FileUpdateWindow(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Ccodec_FileUpdateWindow();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CODEC_FILEUPDATEWINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	LONG		g_fAbortDownload;

	CStatic		m_pCtrl;
	CProgressCtrl m_pProgress;

	CString		m_pClientCodecPath;
	CString		m_pClientCodecSetPath;
	
	DWORD ec;
	SHELLEXECUTEINFO execinfo_install;
	

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	CWebBBS*		m_pMain;
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
	CString DownTmp_Savefolder_load(void);
	void DownState(TCHAR* strState , const int nPercentDone);
	bool MakeFolder(char* pszDir);
	void WorkerThreadProc(int nMode);

	friend unsigned int __stdcall ProcNewCodecImage(void *pParam);
};
