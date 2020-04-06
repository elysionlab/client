#ifndef _SKINDIALOG_H
#define _SKINDIALOG_H

#pragma once
#include "SkinButton.h"
#include "SkinProgress.h"
#include "Label.h"
#include "ComboEx.h"
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

class CSkinDialog : public CDialog
{

public:

	CSkinDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	~CSkinDialog();


protected:

	void Free();
	void LoadBitmap(CBitmap* pBitmap);
	

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void OnSysCommand(UINT nID, LPARAM lParam);	

	int	 m_nButtonCount;
	CTypedPtrArray<CPtrArray, CSkinButton*> m_Buttons;

	DECLARE_MESSAGE_MAP()

public:
	
	void Skin_LoadBG(UINT nResourceID, int nXStretch = 0, int nYStretch = 0);
	void Skin_InsertButton(UINT nResourceID, CString pBtnName, BOOL bCheckBox, CRect rRect = CRect(0,0,0,0));
	void Skin_MoveCtrl(CString m_IDName, int nX, int nY, int nWidth, int nHeight);
	virtual bool Skin_ButtonPressed(CString m_ButtonName);
	virtual bool Skin_PressButton(int m_ID);
	void Skin_SetButtonEnable(CString m_IDName, BOOL m_Enable);
	void Skin_SetButtonVisible(CString m_IDName, BOOL m_Enable);
	void Skin_SetCheck(CString m_IDName, BOOL bCheck);
	BOOL Skin_GetCheck(CString m_IDName);
	void Skin_TitleHeight(int nHeight);
	HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor = 0, COLORREF cTolerance = 0x101010);
	void ShowMaxWindow();
	void ShowNormalWindow();
	void MakeBigWindow();
	void GetBigWindowSize();

protected:

	CBitmap		m_pBmp_Max;
	CBitmap		m_pBmp_Normal;
	HBITMAP		m_hBmp_Max;
	HBITMAP		m_hBmp_Normal;	
	
	BOOL		m_bMaxWindow;

	int			m_nTitleHeight;
	
	int			m_nXStretch;
	int			m_nYStretch;
	
	int			m_nDefaultWidth;
	int			m_nDefaultHeight;

	int			m_nMaxWidth;
	int			m_nMaxHeight;

	int			m_nPosX;
	int			m_nPosY;


};

#endif /* _SKINDIALOG_H */