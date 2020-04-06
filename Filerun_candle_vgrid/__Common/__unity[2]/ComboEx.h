/*******************************************/
/*                                         */
/* - title : CComboEx.h                    */
/* - description : Combo Extends           */
/* - author : you !                        */
/* - date : 2002-09-13                     */
/*                                         */
/*******************************************/


#if !defined(AFX_COMBOEX_H__DBB08486_A30A_4D62_BF5A_790447D57A39__INCLUDED_)
#define AFX_COMBOEX_H__DBB08486_A30A_4D62_BF5A_790447D57A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboEx window

class CComboEx : public CComboBox
{
// Construction
public:
	CComboEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboEx)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTooltipText(int nId);
	void SetTooltipText(CString strTooltip);
	void SetCurSelIcon(int nIconId);
	int GetCurSelIcon();
	void DeleteIcon(int nIndex);
	int InsertIcon(int nIndex, int nIconId, CString str);
	int AddIcon(int nIconId, CString str);
	virtual ~CComboEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboEx)
	afx_msg void OnPaint();
	afx_msg void OnSetfocus();
	afx_msg void OnKillfocus();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CToolTipCtrl m_Tooltip;

	BOOL m_bMouseOnComboBox;
	BOOL m_bLButton;
	BOOL m_bFocus;
	BOOL m_bIcon;
};

typedef struct _IconComboBox {
	HICON	m_hIcon;	// Icon Handle
	int		m_nIconId;	// Icon Resource Id
	CString m_strItem;
} _IconComboBoxData;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOEX_H__DBB08486_A30A_4D62_BF5A_790447D57A39__INCLUDED_)
