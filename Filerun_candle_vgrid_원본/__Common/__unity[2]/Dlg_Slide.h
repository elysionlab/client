#pragma once
#include "SkinDialog.h"
#include "afxwin.h"


// Dlg_Slide ��ȭ �����Դϴ�.

class Dlg_Slide : public CSkinDialog
{

public:
	Dlg_Slide(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Dlg_Slide();

	int m_WndHeight;
	bool m_bUp;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SLIDE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void InitWindow();
	bool Skin_ButtonPressed(CString m_ButtonName);
	CStatic m_static_text;
	CString	m_pInfo;
};
