// UpdateWindowDlg.h : 헤더 파일
//

#pragma once
#include "WebAccess.h"
#include "resource.h"

#define WM_USER_INSTALL_END WM_USER + 100

// CUpdateWindowDlg 대화 상자
class CUpdateWindowDlg : public CDialog
{
// 생성입니다.
public:
	CUpdateWindowDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_UPDATEWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	CWebAccess* m_pWebAccess;
	int m_nSize;
	int m_nReadSize;
	int m_nPos;
	CString	m_pLocalAppPath;
	CString	m_pClientVerPath;
	CString	m_pServerVerPath;
	CString	m_pClientSetPath;

	int GetUpdateSize();
	void UpdateProg(int nReadSize, CString strFileName);

	afx_msg LRESULT OnInstallComplete(WPARAM wParam, LPARAM lParam);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_static_Text;
	CProgressCtrl m_progress;
	afx_msg void OnDestroy();
};
