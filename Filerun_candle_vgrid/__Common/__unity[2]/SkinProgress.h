#if !defined(AFX_PROGRESSEX_H__A74E4BFE_0DF7_4431_AE1D_5FB41E0C7C59__INCLUDED_)
#define AFX_PROGRESSEX_H__A74E4BFE_0DF7_4431_AE1D_5FB41E0C7C59__INCLUDED_


#pragma once

#define PROGRESS_NONE	1
#define PROGRESS_TEXT	2
#define PROGRESS_BITMAP	3

class SkinProgress : public CStatic
{
/* Construction */
public:
	SkinProgress();
	~SkinProgress();

/* Attributes */
public:

/* Operations */
public:


/* Implementation */
public:

	int SetPos(int nPos);
	void SetRange(int nLower, int nUpper);
	int SetStep(int nStep);
	int StepIt();
	void SetTextBkColor(COLORREF clrTextBk);
	void SetTextForeColor(COLORREF clrTextFore);
	void SetBkColor(COLORREF clrBk);
	void SetForeColor(COLORREF clrFore);
	void SetTextColor(COLORREF clrText);

	void SetStyle(int nStyle);
	void SetText(CString strText);
	void SetBitmap(int nId1, int nId2);
	void SetBold(BOOL bBold = TRUE);	//jyh

	// Generated message map functions
protected:
	//{{AFX_MSG(SkinProgress)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CBitmap m_Bitmap1;
	CBitmap m_Bitmap2;
	int m_nHeight;
	int m_nWidth;

	int m_nMin;
	int m_nMax;

	int m_nPos;
	int m_nStep;

	int m_nStyle;

	COLORREF m_clrBk;
	COLORREF m_clrFore;
	COLORREF m_clrTextBk;
	COLORREF m_clrTextFore;
	COLORREF m_clrText;

	bool m_bDrawText;
	bool m_bBold;

	CString m_strText;
};

#endif
