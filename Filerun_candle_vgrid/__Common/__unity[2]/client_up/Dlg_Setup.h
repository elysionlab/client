#pragma once
#include "SkinDialog.h"
#include "SelectFolderDialog.h"
#include "afxwin.h"
#include "up_window.h"

class Dlg_Setup : public CSkinDialog
{

public:

	Dlg_Setup(CWnd* pParent = NULL);  
	virtual ~Dlg_Setup();

	enum { IDD = IDD_DIALOG_SETUP };


protected:

	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
	

public:

	up_window*		m_pMainWindow;
	CStatic			m_pCtrl_Path;
	CComboEx		m_pCombo_UpOver;
	CComboEx		m_pCombo_DownOver;
	CComboEx		m_pCombo_DownItemPath;

public:

	void InitSetup();
	void InitWindow();
	bool Skin_ButtonPressed(CString m_ButtonName);

};
