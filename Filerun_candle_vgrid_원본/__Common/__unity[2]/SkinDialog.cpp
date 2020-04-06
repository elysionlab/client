#include "stdafx.h"
#include "SkinDialog.h"


CSkinDialog::CSkinDialog(UINT nIDTemplate, CWnd* pParentWnd)
		: CDialog(nIDTemplate, pParentWnd)
{
	m_nButtonCount	= 0;
	m_bMaxWindow	= FALSE;
}

CSkinDialog::~CSkinDialog()
{
	Free();
}


BEGIN_MESSAGE_MAP(CSkinDialog, CDialog)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()




BOOL CSkinDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nTitleHeight = 0;
	return true;
}


void CSkinDialog::Free()
{
	m_pBmp_Max.DeleteObject();
	m_pBmp_Normal.DeleteObject();

	for(int i = 0; i < m_Buttons.GetSize(); i++)
		delete m_Buttons.GetAt(i);
	m_Buttons.RemoveAll();
}

void CSkinDialog::Skin_LoadBG(UINT nResourceID, int nXStretch/* = 0*/, int nYStretch/* = 0*/)
{
	Free();

	m_nXStretch	= nXStretch;
	m_nYStretch	= nYStretch;

	if(nResourceID != NULL)
	{
		m_pBmp_Normal.LoadBitmap(nResourceID);

		BITMAP bm;
		GetObject((HBITMAP)m_pBmp_Normal, sizeof(bm), &bm);
		m_nDefaultWidth		= bm.bmWidth;
		m_nDefaultHeight	= bm.bmHeight;
		
		m_hBmp_Normal	= (HBITMAP)m_pBmp_Normal;

		GetBigWindowSize();

		if(m_nXStretch != 0 && m_nYStretch != 0)
			MakeBigWindow();

		if(GetSafeHwnd()) 
		{
			HRGN hr = BitmapToRegion(m_hBmp_Normal, RGB(255, 0, 255));		
			if(hr != NULL) ::SetWindowRgn(GetSafeHwnd(), hr, TRUE);
			Invalidate();
		}
	}
	else
	{
	}

	// 윈도우 위치 재설정
	MoveWindow((GetSystemMetrics(SM_CXSCREEN) - m_nDefaultWidth) / 2, 
				(GetSystemMetrics(SM_CYSCREEN) - m_nDefaultHeight) / 2, 
				m_nDefaultWidth, m_nDefaultHeight);
}


void CSkinDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_RESTORE)
	{
		Invalidate();
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CSkinDialog::Skin_TitleHeight(int nHeight)
{
	m_nTitleHeight = nHeight;
}


void CSkinDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// 윈도우 드래그
	if((m_nTitleHeight == 0 || point.y < m_nTitleHeight) && !m_bMaxWindow)
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point);
}

void CSkinDialog::OnPaint() 
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* m_Btn = m_Buttons.GetAt(i);
		m_Btn->Invalidate();
	}

	CDialog::OnPaint();
}


void CSkinDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}

void CSkinDialog::Skin_InsertButton(UINT nResourceID, CString pBtnName, BOOL bCheckBox, CRect rRect)
{
	CSkinButton* m_pButton = new CSkinButton;
	m_pButton->Create("", WS_VISIBLE, rRect, this, 0);
	m_pButton->InitButton(pBtnName, m_nButtonCount, bCheckBox, nResourceID);

	m_Buttons.Add(m_pButton);
	m_nButtonCount++;
}


void CSkinDialog::Skin_SetButtonEnable(CString m_IDName, BOOL m_Enable)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* m_pButton = m_Buttons.GetAt(i);
		if(m_pButton->m_IDName == m_IDName)
		{
			m_pButton->EnableWindow(m_Enable);
			break;
		}
	}
}

void CSkinDialog::Skin_MoveCtrl(CString m_IDName, int nX, int nY, int nWidth, int nHeight)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* m_pButton = m_Buttons.GetAt(i);
		if(m_pButton->m_IDName == m_IDName)
		{
			m_pButton->MoveWindow(nX, nY, nWidth, nHeight);
			break;
		}
	}
}


void CSkinDialog::Skin_SetButtonVisible(CString m_IDName, BOOL m_Enable)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* m_pButton = m_Buttons.GetAt(i);
		if(m_pButton->m_IDName == m_IDName)
		{
			m_pButton->ShowWindow(m_Enable);
			break;
		}
	}
}

void CSkinDialog::Skin_SetCheck(CString m_IDName, BOOL bCheck)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* m_pButton = m_Buttons.GetAt(i);
		if(m_pButton->m_IDName == m_IDName)
		{
			m_pButton->m_Check = bCheck;
			m_pButton->Invalidate();
			break;
		}
	}
}

BOOL CSkinDialog::Skin_GetCheck(CString m_IDName)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* m_pButton = m_Buttons.GetAt(i);
		if(m_pButton->m_IDName == m_IDName)
		{
			return m_pButton->m_Check;
		}
	}

	return FALSE;
}

void CSkinDialog::OnSize(UINT nType, int cx, int cy) 
{
	if(GetSafeHwnd())
	{
		if(cx < m_nDefaultWidth)		cx = m_nDefaultWidth;
		if(cy < m_nDefaultHeight)		cy = m_nDefaultHeight;		
	}		

	CDialog::OnSize(nType, cx, cy);
}

void CSkinDialog::ShowMaxWindow()
{
	CRect	nRectWindow;
	
	

	GetWindowRect(nRectWindow);
	m_nPosX = nRectWindow.left;
	m_nPosY = nRectWindow.top;
	
	if(GetSafeHwnd()) 
	{
		HRGN hr = BitmapToRegion(m_hBmp_Max, RGB(255, 0, 255));		
		if(hr != NULL) 
			::SetWindowRgn(GetSafeHwnd(), hr, TRUE);	
	}	
	
	m_bMaxWindow = TRUE;
	Invalidate();
	MoveWindow(0, 0, m_nMaxWidth, m_nMaxHeight);	
}

void CSkinDialog::ShowNormalWindow()
{
	m_bMaxWindow = FALSE;

	if(GetSafeHwnd()) 
	{
		HRGN hr = BitmapToRegion(m_hBmp_Normal, RGB(255, 0, 255));		
		if(hr != NULL) 
			::SetWindowRgn(GetSafeHwnd(), hr, TRUE);	
	}

	Invalidate();
	MoveWindow(m_nPosX, m_nPosY, m_nDefaultWidth, m_nDefaultHeight);	
}


BOOL CSkinDialog::OnEraseBkgnd(CDC* pDC) 
{
	HBITMAP hOldBmp;
	HDC		hMemdc;


	if((!m_bMaxWindow && m_hBmp_Normal) || (m_bMaxWindow && m_hBmp_Max))
	{
		hMemdc = CreateCompatibleDC(pDC->m_hDC); 

		if(hMemdc)
		{
			if(m_bMaxWindow) 
				hOldBmp = (HBITMAP)SelectObject(hMemdc, m_hBmp_Max);
			else
				hOldBmp = (HBITMAP)SelectObject(hMemdc, m_hBmp_Normal);

			if(hOldBmp)
			{
				if(m_bMaxWindow) 
					BitBlt(pDC->m_hDC, 0, 0, m_nMaxWidth, m_nMaxHeight, hMemdc, 0, 0, SRCCOPY);					
				else
					BitBlt(pDC->m_hDC, 0, 0, m_nDefaultWidth, m_nDefaultHeight, hMemdc, 0, 0, SRCCOPY);					

				SelectObject(hMemdc, hOldBmp);
				DeleteDC(hMemdc);
				DeleteObject(hOldBmp);
				return TRUE;
			}
			else
			{
				DeleteDC(hMemdc);
			}
		}
	}

	return CDialog::OnEraseBkgnd(pDC);
}





void CSkinDialog::MakeBigWindow()
{
	CDC		*pDC = GetDC();    
	CDC		newDC;
	
	HDC		hBmpDC;
	HBITMAP hOldBmp;
	HBITMAP hOldBmp2;

	
	

	hBmpDC = CreateCompatibleDC(pDC->m_hDC); 
	newDC.CreateCompatibleDC(pDC);

	if(hBmpDC && newDC)
	{
		m_pBmp_Max.CreateCompatibleBitmap(pDC, m_nMaxWidth, m_nMaxHeight);
		hOldBmp = (HBITMAP)SelectObject(hBmpDC, m_hBmp_Normal);
		hOldBmp2 = (HBITMAP)SelectObject(newDC, (HBITMAP)m_pBmp_Max);

		if(hOldBmp)
		{
			// 좌상단 부분
			StretchBlt(	newDC.m_hDC, 0, 0, m_nXStretch, m_nYStretch, hBmpDC, 
						0, 0, m_nXStretch, m_nYStretch, SRCCOPY);
			// 우상단 부분
			StretchBlt(	newDC.m_hDC, m_nMaxWidth - (m_nDefaultWidth - m_nXStretch), 0, m_nDefaultWidth - m_nXStretch, m_nYStretch, hBmpDC, 
						m_nXStretch, 0, m_nDefaultWidth - m_nXStretch, m_nYStretch, SRCCOPY);
			// 좌하단 부분
			StretchBlt(	newDC.m_hDC, 0, m_nMaxHeight - (m_nDefaultHeight - m_nYStretch), m_nXStretch, m_nDefaultHeight - m_nYStretch, hBmpDC, 
						0, m_nYStretch, m_nXStretch, m_nDefaultHeight - m_nYStretch, SRCCOPY);
			// 우하단 부분
			StretchBlt(	newDC.m_hDC, m_nMaxWidth - (m_nDefaultWidth - m_nXStretch), m_nMaxHeight - (m_nDefaultHeight - m_nYStretch), m_nDefaultWidth - m_nXStretch, m_nDefaultHeight - m_nYStretch, hBmpDC, 
						m_nXStretch, m_nYStretch, m_nDefaultWidth - m_nXStretch, m_nDefaultHeight - m_nYStretch, SRCCOPY);



			// 중상단 부분
			StretchBlt(	newDC.m_hDC, m_nXStretch, 0, m_nMaxWidth - m_nDefaultWidth, m_nYStretch, hBmpDC, 
						m_nXStretch, 0, 1, m_nYStretch, SRCCOPY);
			// 중하단 부분
			StretchBlt(	newDC.m_hDC, m_nXStretch, m_nMaxHeight - (m_nDefaultHeight - m_nYStretch), m_nMaxWidth - m_nDefaultWidth, m_nDefaultHeight - m_nYStretch, hBmpDC, 
						m_nXStretch, m_nYStretch, 1, m_nDefaultHeight - m_nYStretch, SRCCOPY);

			// 중좌측 부분
			StretchBlt(	newDC.m_hDC, 0, m_nYStretch, m_nXStretch, m_nMaxHeight - m_nDefaultHeight, hBmpDC, 
						0, m_nYStretch, m_nXStretch, 1, SRCCOPY);
			// 중우측 부분
			StretchBlt(	newDC.m_hDC, m_nMaxWidth - (m_nDefaultWidth - m_nXStretch), m_nYStretch, m_nDefaultWidth - m_nXStretch, m_nMaxHeight - m_nDefaultHeight, hBmpDC, 
						m_nXStretch, m_nYStretch, m_nDefaultWidth - m_nXStretch, 1, SRCCOPY);



			// 중간 부분
			StretchBlt(	newDC.m_hDC, m_nXStretch, m_nYStretch, m_nMaxWidth - m_nDefaultWidth, m_nMaxHeight - m_nDefaultHeight, hBmpDC, 
						m_nXStretch, m_nYStretch, 1, 1, SRCCOPY);


			SelectObject(hBmpDC, hOldBmp);
			DeleteDC(hBmpDC);		
			DeleteObject(hOldBmp);

			SelectObject(newDC, hOldBmp2);
			DeleteDC(newDC);		
			DeleteObject(hOldBmp2);

			m_hBmp_Max	= (HBITMAP)m_pBmp_Max;
		}

		else
		{
			DeleteDC(hBmpDC);		
			DeleteDC(newDC);
		}	
	}
}

void CSkinDialog::GetBigWindowSize()
{
	RECT rTray;
	int	nTrayWndHeight = 0;

	HWND hWndTrayWnd = ::FindWindow("Shell_TrayWnd", "");
	if(hWndTrayWnd)
	{
		::GetWindowRect(hWndTrayWnd, &rTray);
		nTrayWndHeight = rTray.bottom - rTray.top - 2;
	}

	m_nMaxWidth		= GetSystemMetrics(SM_CXSCREEN);
	m_nMaxHeight	= GetSystemMetrics(SM_CYSCREEN) - nTrayWndHeight;
}


HBRUSH CSkinDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    return hbr;
}


bool CSkinDialog::Skin_ButtonPressed(CString m_ButtonName)
{
	return true;
}

bool CSkinDialog::Skin_PressButton(int m_ID)
{
	CSkinButton* mBtn = m_Buttons.GetAt(m_ID);
	return Skin_ButtonPressed(mBtn->m_IDName);
}



HRGN CSkinDialog::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor/* = 0*/, COLORREF cTolerance/* = 0x101010*/)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
					sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}	
	}

	return hRgn;
}
