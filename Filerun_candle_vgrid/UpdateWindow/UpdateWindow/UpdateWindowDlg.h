// UpdateWindowDlg.h : ��� ����
//

#pragma once
#include "WebAccess.h"
#include "resource.h"

#define WM_USER_INSTALL_END WM_USER + 100

// CUpdateWindowDlg ��ȭ ����
class CUpdateWindowDlg : public CDialog
{
// �����Դϴ�.
public:
	CUpdateWindowDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_UPDATEWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

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

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
