#pragma once
#include "SkinDialog.h"
#include "afxwin.h"


class Dlg_Confirm : public CSkinDialog
{

public:

	Dlg_Confirm(CWnd* pParent = NULL);  
	virtual ~Dlg_Confirm();

	enum { IDD = IDD_DIALOG_CONFIRM };


protected:

	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

public:

	void InitWindow();
	bool Skin_ButtonPressed(CString m_ButtonName);

public:

	CBitmap		m_pBmp;
	BOOL		m_bCheck;
	int			m_nCallMode;
	CString		m_pInfo;
	CStatic		m_pCtrl_Text;
	CStatic		m_pCtrl_Title;
};
