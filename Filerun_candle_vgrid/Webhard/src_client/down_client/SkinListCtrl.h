#if !defined(AFX_MIRLISTCTRL_H__9EDE1EC9_55B7_48A4_9451_D9FAB6F6724B__INCLUDED_)
#define AFX_MIRLISTCTRL_H__9EDE1EC9_55B7_48A4_9451_D9FAB6F6724B__INCLUDED_

#pragma once

class CSkinHeaderCtrl : public CHeaderCtrl
{

public:

	CSkinHeaderCtrl();
	virtual ~CSkinHeaderCtrl();

public:

	void SetColors(const COLORREF colorBack, const COLORREF colorText);
	void InitHeaderImage();				//20080326 헤더 이미지 초기화, jyh

protected:

	COLORREF m_colorText, m_colorBack, m_colorBorder;
	CBitmap			m_Headerbmp;		//20080326 헤더 이미지, jyh
	BITMAP			m_HeaderbmpInfo;	//20080326 헤더 이미지 정보, jyh

protected:

	afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
};




class CSkinListCtrl : public CListCtrl
{

public:

	CSkinListCtrl();
	virtual ~CSkinListCtrl();

protected:

	virtual void PreSubclassWindow();

public:

	void	SetToolTips(BOOL bEnable = TRUE);
	int     OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
	void    SetHeaderColors(const COLORREF colorBack, const COLORREF colorText);

	void	SetListColor(const COLORREF colorOddText, const COLORREF colorOddBg, 
						 const COLORREF colorEvenText, const COLORREF colorEvenBg,
						 const COLORREF colorHiLText, const COLORREF colorHiLBg, 
						 const COLORREF colorBarLine, 
						 const COLORREF colorBarText1, const COLORREF colorBarBg1,
						 const COLORREF colorBarText2, const COLORREF colorBarBg2);
    
	inline  void SetColumnCount(int nCount) { m_nColumnCount = nCount; }
	//20080303 완료 목록 텍스트 색 설정, jyh
	void	SetCompleteTextColor(const COLORREF colorComplete) { m_colorComplete = colorComplete; }
	//void	InitDownStateImage();								//20080325 다운로드 상태 이미지 초기화, jyh
	//void	DrawStateBitmap(CDC* pdc, CRect& rt, int nState);	//20080325 다운로드 상태 이미지 그리기, jyh


	BOOL GetCellRect(int iRow, int iCol, int nArea, CRect &rect);

protected:

	CSkinHeaderCtrl m_HeaderCtrl;

	HICON			m_hFolderIcon;
    int				m_nColumnCount;
    LOGFONT			m_LogFont;
    COLORREF		m_colorOddText, m_colorEvenText, m_colorOddBg, m_colorEvenBg;
    COLORREF		m_colorHiLText, m_colorHiLBg;
	COLORREF		m_colorBarLine, m_colorBarBg1, m_colorBarBg2, m_colorBarText1, m_colorBarText2;
	COLORREF		m_colorComplete;	//20080303 완료 목록 색, jyh
	CBitmap			m_DownStateBmp;		//20080325 다운 상태 이미지, jyh
	BITMAP			m_bmpinfo;			//20080325 이미지 정보, jyh
	    

protected:
	
	int		GetRowFromPoint(CPoint &point, CRect *rcCell, int *nCol) const;
	virtual void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnPaint();
	afx_msg void OnDestroy();

    afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


#endif
