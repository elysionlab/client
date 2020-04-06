#pragma once
#include "SkinDialog.h"
#include "afxwin.h"

class Dlg_Alert : public CSkinDialog
{

public:

	Dlg_Alert(CWnd* pParent = NULL);  
	virtual ~Dlg_Alert();

	enum { IDD = IDD_DIALOG_NOTICE };


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

	int			m_bAutoClose;
	CString		m_pInfo;
	CStatic		m_pCtrl_Text;
};
