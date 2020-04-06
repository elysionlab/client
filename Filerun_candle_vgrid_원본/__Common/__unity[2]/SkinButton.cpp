#include "stdafx.h"
#include "SkinButton.h"
#include "SkinDialog.h"


CSkinButton::CSkinButton()
{
	m_CheckedButton = m_Check = m_MouseOnButton = FALSE;
	m_hWnd			= NULL;
}


BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CSkinButton::InitButton(CString pIDName, int nID, BOOL bCheckBox, UINT nResourceID)
{
	m_CheckedButton		= bCheckBox;
	m_IDName			= pIDName;
	m_ID				= nID;

	CBitmap m_pBmp;
	m_pBmp.LoadBitmap(nResourceID);

	BITMAP bm;
	GetObject((HBITMAP)m_pBmp, sizeof(bm), &bm);

	if(bCheckBox)
		CopyFrom(CRect(0, 0, bm.bmWidth / 2, bm.bmHeight), m_pBmp, bCheckBox);
	else
		CopyFrom(CRect(0, 0, bm.bmWidth / 4, bm.bmHeight), m_pBmp, bCheckBox);
}


void CSkinButton::PreSubclassWindow() 
{
	UINT nBS;

	nBS = GetButtonStyle();
	if (nBS & BS_DEFPUSHBUTTON) m_MouseOnButton = TRUE;
	SetButtonStyle(nBS | BS_OWNERDRAW);
	CButton::PreSubclassWindow();
	ModifyStyle(WS_TABSTOP, 0);
}



void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	HBITMAP pTempBmp;

	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	BOOL bIsPressed  = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);
	
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);

	if(!m_CheckedButton)
	{
		if(m_Check && !m_MouseOnButton) 
			pTempBmp = (HBITMAP)m_Bmp_Down;			
		else if(bIsPressed) 
			pTempBmp = (HBITMAP)m_Bmp_Down;
		else if(m_MouseOnButton) 
			pTempBmp = (HBITMAP)m_Bmp_Over;			
		else if(bIsDisabled) 
			pTempBmp = (HBITMAP)m_Bmp_Disabled;			
		else 
			pTempBmp = (HBITMAP)m_Bmp_Normal;			
	}
	else
	{
		if(m_Check == TRUE) 
			pTempBmp = (HBITMAP)m_Bmp_Over;	
		else 
			pTempBmp = (HBITMAP)m_Bmp_Normal;			
	}

	DrawTransparentBitmap(pDC->m_hDC, (HBITMAP)pTempBmp, 0, 0, RGB(255, 0, 255));
	memdc.SelectObject(pTempBmp);
	memdc.DeleteDC();
}

void CSkinButton::CopyFrom(CRect r, CBitmap& m_N, BOOL bCheckBtn)
{
	CDC* dc = GetDC();	
	CRect cr;

	cr.SetRect( 0,0,r.Width(),r.Height());
	CopyBitmap(dc, m_Bmp_Normal, m_N, cr);
	
	cr.SetRect( r.Width()*1,0,r.Width()+r.Width()*1,r.Height());
	CopyBitmap(dc, m_Bmp_Over, m_N, cr);

	if(bCheckBtn == FALSE)
	{
		cr.SetRect( r.Width()*2,0,r.Width()+r.Width()*2,r.Height());
		CopyBitmap(dc, m_Bmp_Down, m_N, cr);
		
		cr.SetRect( r.Width()*3,0,r.Width()+r.Width()*3,r.Height());
		CopyBitmap(dc, m_Bmp_Disabled, m_N, cr);
	}
	
	ModifyStyle(WS_BORDER | WS_TABSTOP , 0);
}

void CSkinButton::CopyBitmap(CDC* dc, CBitmap& mRes, const CBitmap& hbmp, RECT r)
{
	if(!hbmp.m_hObject) return;
	int w = r.right - r.left,
		h = r.bottom - r.top;

	CDC memdc, hDC;
	
	mRes.CreateCompatibleBitmap(dc, w, h);

	hDC.CreateCompatibleDC(dc);
	hDC.SelectObject((HBITMAP) mRes);

	memdc.CreateCompatibleDC(dc);
	memdc.SelectObject(hbmp);

	hDC.StretchBlt(0, 0, w, h, &memdc, r.left, r.top, w, h, SRCCOPY); 
	hDC.DeleteDC();
	memdc.DeleteDC();
}

void CSkinButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect r;
	GetClientRect(&r);
	if(r.PtInRect(point))
	{
		m_MouseOnButton = FALSE;

		if(m_CheckedButton)
		{
			if(m_Check == TRUE) 
				m_Check = FALSE;
			else
				m_Check = TRUE;
		}

		Invalidate();
		CSkinDialog* m_Dlg = (CSkinDialog*)GetParent();
		if(!m_Dlg->Skin_PressButton(m_ID))
		{
			return;
		}
	}
	CButton::OnLButtonUp(nFlags, point);
}

void CSkinButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd* pWnd;
	CWnd* pParent;


	CButton::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) return;

	pWnd = GetActiveWindow();
	pParent = GetOwner();

	if ((GetCapture() != this) && (pWnd != NULL && pParent != NULL)) 
	{
		m_MouseOnButton = TRUE;
		SetCapture();
		Invalidate();
	}
	else
	{
		POINT p2 = point;
		ClientToScreen(&p2);
		CWnd* wndUnderMouse = WindowFromPoint(p2);
		if (wndUnderMouse && wndUnderMouse->m_hWnd != this->m_hWnd)
		{
			if (m_MouseOnButton == TRUE)
			{
				m_MouseOnButton = FALSE;
				Invalidate();
			}
			if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
		}
	}
}

BOOL CSkinButton::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CSkinButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	m_MouseOnButton = FALSE;
	Invalidate();
}

void CSkinButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_MouseOnButton == TRUE)
	{
		ReleaseCapture();
		Invalidate();
	}
	CButton::OnCaptureChanged(pWnd);
}



void CSkinButton::DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart,
                           short yStart, COLORREF cTransparentColor)
{
   BITMAP     bm;
   COLORREF   cColor;
   HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
   HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
   HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
   POINT      ptSize;

   hdcTemp = CreateCompatibleDC(hdc);
   SelectObject(hdcTemp, hBitmap);   // Select the bitmap

   GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
   ptSize.x = bm.bmWidth;            // Get width of bitmap
   ptSize.y = bm.bmHeight;           // Get height of bitmap
   DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device

                                     // to logical points

   // Create some DCs to hold temporary data.
   hdcBack   = CreateCompatibleDC(hdc);
   hdcObject = CreateCompatibleDC(hdc);
   hdcMem    = CreateCompatibleDC(hdc);
   hdcSave   = CreateCompatibleDC(hdc);

   // Create a bitmap for each DC. DCs are required for a number of
   // GDI functions.

   // Monochrome DC
   bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

   // Monochrome DC
   bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

   bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
   bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

   // Each DC must select a bitmap object to store pixel data.
   bmBackOld   = (HBITMAP) SelectObject(hdcBack, bmAndBack);
   bmObjectOld = (HBITMAP) SelectObject(hdcObject, bmAndObject);
   bmMemOld    = (HBITMAP) SelectObject(hdcMem, bmAndMem);
   bmSaveOld   = (HBITMAP) SelectObject(hdcSave, bmSave);

   // Set proper mapping mode.
   SetMapMode(hdcTemp, GetMapMode(hdc));

   // Save the bitmap sent here, because it will be overwritten.
   BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

   // Set the background color of the source DC to the color.
   // contained in the parts of the bitmap that should be transparent
   cColor = SetBkColor(hdcTemp, cTransparentColor);

   // Create the object mask for the bitmap by performing a BitBlt
   // from the source bitmap to a monochrome bitmap.
   BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
          SRCCOPY);

   // Set the background color of the source DC back to the original
   // color.
   SetBkColor(hdcTemp, cColor);

   // Create the inverse of the object mask.
   BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
          NOTSRCCOPY);

   // Copy the background of the main DC to the destination.
   BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
          SRCCOPY);

   // Mask out the places where the bitmap will be placed.
   BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

   // Mask out the transparent colored pixels on the bitmap.
   BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

   // XOR the bitmap with the background on the destination DC.
   BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

   // Copy the destination to the screen.
   BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
          SRCCOPY);

   // Place the original bitmap back into the bitmap sent here.
   BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

   // Delete the memory bitmaps.
   DeleteObject(SelectObject(hdcBack, bmBackOld));
   DeleteObject(SelectObject(hdcObject, bmObjectOld));
   DeleteObject(SelectObject(hdcMem, bmMemOld));
   DeleteObject(SelectObject(hdcSave, bmSaveOld));

   // Delete the memory DCs.
   DeleteDC(hdcMem);
   DeleteDC(hdcBack);
   DeleteDC(hdcObject);
   DeleteDC(hdcSave);
   DeleteDC(hdcTemp);
 }
