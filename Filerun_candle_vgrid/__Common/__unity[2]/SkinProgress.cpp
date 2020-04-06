#include "stdafx.h"
#include "SkinProgress.h"


/* Constructor */
SkinProgress::SkinProgress()
{
	m_nMin = 0;
	m_nMax = 100;

	m_bDrawText = false;

	m_nPos = 0;
	m_nStep = 10;

	m_clrFore		= ::GetSysColor(COLOR_HIGHLIGHT);
	m_clrBk			= ::GetSysColor(COLOR_WINDOW);
	m_clrTextFore	= ::GetSysColor(COLOR_HIGHLIGHT);
	m_clrTextBk		= ::GetSysColor(COLOR_WINDOW);
	m_clrText = RGB(255, 0, 0);
	m_strText.Empty();
	m_nStyle = PROGRESS_NONE;	// 공통 ProgressCtrl
	m_bBold = TRUE;


	LONG nStyleValue = GetWindowLong(this->m_hWnd, GWL_STYLE);
	nStyleValue &= WS_POPUP; 
    nStyleValue &= ~WS_DLGFRAME; 
	nStyleValue &= ~WS_BORDER; 	
	nStyleValue &= ~WS_THICKFRAME; 
	nStyleValue &= ~WS_CAPTION; 
	
    SetWindowLong(this->m_hWnd, GWL_STYLE, nStyleValue) ; 
} /* end of Constructor */


/* Destructor */
SkinProgress::~SkinProgress()
{
} /* end of Destructor */



/* MESSAGE_MAP */
BEGIN_MESSAGE_MAP(SkinProgress, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/* OnPaint */
void SkinProgress::OnPaint() 
{
	if(m_nStyle == PROGRESS_NONE) {	// 일반 ProgressCtrl 일때
		CStatic::OnPaint();
		return;
	}

	try
	{
		CPaintDC dc(this);
		CRect rcClient, rcLeft, rcRight;
		GetClientRect(rcClient);
		rcLeft = rcRight = rcClient;


		/* 사용자가 지정한 위치 값(m_nPos)을 기준으로 */
		/* m_nMin과 m_nMax 범위에서 해당되는 퍼센트를 구한다 */
		double Fraction = (double)(m_nPos - m_nMin) / (double)(m_nMax - m_nMin);

		/* Progress 진행바를 그리기 위한 범위 값을 구한다 */
		rcLeft.right = rcLeft.left + (int)((rcLeft.right - rcLeft.left) * Fraction);

		/* rcLeft : 진행바 영역     */
		/* rcRight : 진행바를 제외한 영역 */
		rcLeft.right = (int)((rcClient.right - rcClient.left) * Fraction);
		rcRight.left = rcLeft.right;
		
		/* 1. SetBitmap 함수로 설정한 비트맵과 호환된는 DC를 만든다 */
		CDC memDC1, memDC2;
		if(memDC1.CreateCompatibleDC(&dc) && memDC2.CreateCompatibleDC(&dc))
		{
			CBitmap* pOldBitmap1 = (CBitmap*)memDC1.SelectObject(&m_Bitmap1);
			CBitmap* pOldBitmap2 = (CBitmap*)memDC2.SelectObject(&m_Bitmap2);

			/* 2. 컨트롤과 동일한 영역을 차지하는 비트맵과 DC를 만든다 */
			CDC leftDC, rightDC;
			leftDC.CreateCompatibleDC(&dc);
			rightDC.CreateCompatibleDC(&dc);

			CBitmap leftBmp, rightBmp;
			leftBmp.CreateCompatibleBitmap(&memDC1, rcClient.Width(), rcClient.Height());
			rightBmp.CreateCompatibleBitmap(&memDC2, rcClient.Width(), rcClient.Height());
	
			CBitmap* pOldleftBmp = (CBitmap*)leftDC.SelectObject(&leftBmp);
			CBitmap* pOldrightBmp = (CBitmap*)rightDC.SelectObject(&rightBmp);
			
			leftDC.StretchBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC1, 0, 0, m_nWidth, m_nHeight, SRCCOPY);					
			rightDC.StretchBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC2, 0, 0, m_nWidth, m_nHeight, SRCCOPY);					
			
			dc.BitBlt(0, 0, rcLeft.Width(), rcLeft.Height(), &leftDC, 0, 0, SRCCOPY);
			dc.BitBlt(rcRight.left, rcRight.top, rcRight.Width(), rcRight.Height(), &rightDC, 0, 0, SRCCOPY);

			leftDC.SelectObject(pOldleftBmp);
			rightDC.SelectObject(pOldrightBmp);
			memDC1.SelectObject(pOldBitmap1);					
			memDC2.SelectObject(pOldBitmap2);	
			leftBmp.DeleteObject();
			rightBmp.DeleteObject();
			


			/* 텍스트 출력 */
			//20071029 폰트를 새로 만든다, jyh
			//CFont* pFont = GetFont();		
			//CFont* pOldFont = (CFont*)dc.SelectObject(pFont);
			CFont font;

			if(m_bBold)
			{
				font.CreateFont(15, 6, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS, 
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "tahoma");
			}
			else
			{
				font.CreateFont(15, 6, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS, 
					CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "tahoma");
			}
			
			CFont* pOldFont = (CFont*)dc.SelectObject(&font);
			
			dc.SetBkMode(TRANSPARENT);

			CRgn rgn;
			rgn.CreateRectRgn(rcLeft.left, rcLeft.top, rcLeft.right, rcLeft.bottom);
			dc.SelectClipRgn(&rgn);
			dc.SetTextColor(m_clrTextBk);
			dc.DrawText(m_strText, rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			
			rgn.DeleteObject();
			rgn.CreateRectRgn(rcRight.left, rcRight.top, rcRight.right, rcRight.bottom);
			dc.SelectClipRgn(&rgn);
			dc.SetTextColor(m_clrTextFore);
			dc.DrawText(m_strText, rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			dc.SelectObject(pOldFont);
		}
	}		
	catch(...)
	{
	}
} /* end of OnPaint */


/*OnEraseBkgnd*/
BOOL SkinProgress::OnEraseBkgnd(CDC* pDC) 
{
	if(m_nStyle != PROGRESS_NONE)
		return TRUE;

	return CStatic::OnEraseBkgnd(pDC);
} /*end of OnEraseBkgnd*/


/*SetBitmap*/
void SkinProgress::SetBitmap(int nId1, int nId2)
{
	/* 비트맵 핸들을 얻는다 */
	HBITMAP hBmp1 = (HBITMAP)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nId1), IMAGE_BITMAP, 0, 0, 0);
	HBITMAP hBmp2 = (HBITMAP)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nId2), IMAGE_BITMAP, 0, 0, 0);

	m_Bitmap1.Detach();
	m_Bitmap1.Attach(hBmp1);

	m_Bitmap2.Detach();
	m_Bitmap2.Attach(hBmp2);

	BITMAP bmp;
	m_Bitmap1.GetObject(sizeof(bmp), &bmp);
	m_nWidth = bmp.bmWidth;
	m_nHeight = bmp.bmHeight;
} /* End of SetBitmap */


/* Progress에 출력할 텍스트를 설정한다 */
void SkinProgress::SetText(CString strText)
{
	m_strText = strText;
	
	Invalidate();
} /* end of SetText */


/* Setpos */
int SkinProgress::SetPos(int nPos)
{
	if(m_nMin <= nPos && nPos <= m_nMax) {
		m_nPos = nPos;
		Invalidate();
	}

	return 1;
} /* end of SEtPos*/


/* SEtting Range */
void SkinProgress::SetRange(int nLower, int nUpper)
{
	if(nLower <= nUpper) {
		m_nMin = nLower;
		m_nMax = nUpper;
	}
} /* end of Setting Range */


/* SetStep */
int SkinProgress::SetStep(int nStep)
{
	m_nStep = nStep;
	return 1;
} /* end of setStep */


/* StepIt */
int SkinProgress::StepIt()
{
	m_nPos += m_nStep;
	
	if(m_nPos > m_nMax)
		m_nPos = m_nMin + (m_nPos - m_nMax);

	Invalidate();
	return 1;
} /* end of setIt */


/* 진행바 색상 */
void SkinProgress::SetForeColor(COLORREF clrFore)
{
	m_clrFore = clrFore;
} /* SetForeColor */


/* 진행바를 제외한 부분 색상 */
void SkinProgress::SetBkColor(COLORREF clrBk)
{
	m_clrBk = clrBk;
} /* SetBkcolor */


/* 진행바에 위치한 문자열 색상 */
void SkinProgress::SetTextForeColor(COLORREF clrTextFore)
{
	m_clrTextFore = clrTextFore;
} /* end fo SetTextForeColor */


/* 진행바를 제외한 부분에 위치한 문자열 색상 */
void SkinProgress::SetTextBkColor(COLORREF clrTextBk)
{
	m_clrTextBk = clrTextBk;
} /* end of setTextBkcolor */


/* 비트맵 진행바에서 진행률을 나타내는 문자열 색상 */
void SkinProgress::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
} /* SetTextColor */


/* ProgressCtrl 성격을 결정한다 */
void SkinProgress::SetStyle(int nStyle)
{
	m_nStyle = nStyle;
} /* end of SetStyle */

void SkinProgress::SetBold(BOOL bBold)
{
	m_bBold = bBold;
}