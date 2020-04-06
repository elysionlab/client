#pragma once
#include "SkinDialog.h"
#include "afxwin.h"


// Dlg_Slide 대화 상자입니다.

class Dlg_Slide : public CSkinDialog
{

public:
	Dlg_Slide(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~Dlg_Slide();

	int m_WndHeight;
	bool m_bUp;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SLIDE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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
