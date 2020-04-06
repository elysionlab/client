#ifndef _SKINDBUTTON_H
#define _SKINDBUTTON_H

#pragma once
#include <afxwin.h>
#include <afxcmn.h>


class CSkinButton : public CButton
{

public:

	CSkinButton(); 


public:

	HWND		m_hWnd;
	BOOL		m_Check;
	BOOL		m_CheckedButton;
	CString		m_IDName;
	int			m_ID;


public:

	void InitButton(CString pIDName, int nID, BOOL bCheckBox, UINT nResourceID);
	

protected:

	BOOL m_MouseOnButton;

	CBitmap m_Bmp_Normal, m_Bmp_Over, m_Bmp_Down, m_Bmp_Disabled;

	void CopyFrom(CRect r, CBitmap& m_N, BOOL bCheckBtn);
	void CopyBitmap(CDC* dc, CBitmap& mRes, const CBitmap& hbmp, RECT r);
	void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart, short yStart, COLORREF cTransparentColor);

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);	
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};


#endif /* _SKINDBUTTON_H */