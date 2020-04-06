#pragma once
#include "SkinDialog.h"
#include "afxwin.h"


class Dlg_Exist : public CSkinDialog
{

public:

	Dlg_Exist(CWnd* pParent = NULL);  
	virtual ~Dlg_Exist();

	enum { IDD = IDD_DIALOG_EXIST };


protected:

	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

public:

	void InitWindow();
	bool Skin_ButtonPressed(CString m_ButtonName);

public:

	int			m_nResult;
	int			m_nAllApply;

	CString		m_pInfo_FilePath;
	CString		m_pInfo_FileName;
	CString		m_pInfo_Size;

	CStatic		m_pCtrl_Text;
	CStatic		m_pCtrl_Edit_Name;
	CStatic		m_pCtrl_Edit_Path;
};
