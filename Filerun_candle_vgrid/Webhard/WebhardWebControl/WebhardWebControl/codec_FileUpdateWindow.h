#pragma once

#include <afxcmn.h>
#include "WebBBS.h"

// Ccodec_FileUpdateWindow 대화 상자입니다.

class Ccodec_FileUpdateWindow : public CDialog
{
	DECLARE_DYNAMIC(Ccodec_FileUpdateWindow)

public:
	Ccodec_FileUpdateWindow(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~Ccodec_FileUpdateWindow();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CODEC_FILEUPDATEWINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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
