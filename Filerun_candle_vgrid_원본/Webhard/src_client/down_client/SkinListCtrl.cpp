#include "stdafx.h"
#include "SkinListCtrl.h"


CSkinHeaderCtrl::CSkinHeaderCtrl()
{
    m_colorText   = RGB(61, 63, 66);
    m_colorBack   = RGB(238, 238, 238);
    m_colorBorder = RGB(171, 171, 171);

	InitHeaderImage();		//20080326 ��� �̹��� �ʱ�ȭ, jyh
}

CSkinHeaderCtrl::~CSkinHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSkinHeaderCtrl, CHeaderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

void CSkinHeaderCtrl::SetColors(const COLORREF colorBack, const COLORREF colorText)
{
    BYTE byRed    = GetRValue(colorBack);
    BYTE byGreen  = GetGValue(colorBack);
    BYTE byBlue   = GetBValue(colorBack);
    BYTE byReduce = 50;

    if(byRed >= byReduce)
        byRed = BYTE(byRed - byReduce);
    else
        byRed = 0;
    if(byGreen >= byReduce)
        byGreen = BYTE(byGreen - byReduce);
    else
        byGreen = 0;
    if(byBlue >= byReduce)
        byBlue = BYTE(byBlue - byReduce);
    else
        byBlue = 0;

    m_colorBack   = colorBack;
    m_colorText   = colorText;
    m_colorBorder = RGB(255,255,255);//RGB(byRed, byGreen, byBlue);
}

void CSkinHeaderCtrl::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMCUSTOMDRAW  *pNMCD = (NMCUSTOMDRAW*)pNMHDR;
    CBrush brush, *pOldBrush;
    CRect  rcDraw(pNMCD->rc);
	CRect  rcDraw1, rcDraw2;
    CPen   pen, *pOldPen;
    CDC    *pDC = CDC::FromHandle(pNMCD->hdc);
    UINT   nJustify;
    HDITEM hdi;

    char   szBuffer[MAX_PATH];
	ZeroMemory(szBuffer, sizeof(szBuffer));	//20070918 jyh
    int    nOffset;

	if(pNMCD->dwDrawStage == CDDS_PREPAINT || pNMCD->dwDrawStage == CDDS_PREERASE)
    {
        /*brush.CreateSolidBrush(m_colorBack);
        pOldBrush = (CBrush *)pDC->SelectObject(&brush);
        pen.CreatePen(PS_SOLID, 1, m_colorBorder);
        pOldPen = (CPen *)pDC->SelectObject(&pen);
        pDC->Rectangle(rcDraw);
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);*/
		
		CDC memdc;
		memdc.CreateCompatibleDC(pDC);
		memdc.SelectObject(&m_Headerbmp);

		for(int i=0; i<rcDraw.right; i++)
		{
			pDC->StretchBlt(rcDraw.left+i, rcDraw.top, m_HeaderbmpInfo.bmWidth, m_HeaderbmpInfo.bmHeight,
				&memdc,	0, 0, m_HeaderbmpInfo.bmWidth, m_HeaderbmpInfo.bmHeight, SRCCOPY);
		}

        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if(pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
    {
		rcDraw1 = rcDraw2 = rcDraw;

        brush.CreateSolidBrush(m_colorBack);
        pOldBrush = (CBrush *)pDC->SelectObject(&brush);
		//pen.CreatePen(PS_SOLID, 1, m_colorBorder);
        pen.CreatePen(PS_SOLID, 1, RGB(158, 158, 158));
        pOldPen = (CPen *)pDC->SelectObject(&pen);        

		rcDraw1.left = rcDraw1.right-1;
		rcDraw1.bottom = rcDraw1.bottom - 5;
		rcDraw1.top = rcDraw1.top + 3;
        pDC->Rectangle(rcDraw1);
		

		// ���� ���Ŀ� ���� �ؽ�Ʈ ���
		hdi.mask       = HDI_TEXT | HDI_FORMAT;
        hdi.fmt        = HDF_LEFT;
        hdi.pszText    = szBuffer;
        hdi.cchTextMax = MAX_PATH;

		pDC->SetTextColor(m_colorText);
        pDC->SetBkMode   (TRANSPARENT);               
        
        if(GetItem((int)pNMCD->dwItemSpec, &hdi))
        {
            nOffset = (pDC->GetTextExtent(_T(" ")).cx) << 1;
            rcDraw.DeflateRect(nOffset, 2, nOffset, 0);
            switch(hdi.fmt & HDF_JUSTIFYMASK)
            {
				case HDF_CENTER: 
					nJustify = DT_CENTER;
					break;
				case HDF_RIGHT:
					nJustify = DT_RIGHT;
					break;
				default:
					nJustify = DT_CENTER;//DT_LEFT;
					break;
            }
			rcDraw.bottom -= 4;
            pDC->DrawText(szBuffer, lstrlen(szBuffer), &rcDraw, nJustify | DT_SINGLELINE | DT_VCENTER);
        }
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
        *pResult = CDRF_SKIPDEFAULT;        
    }
    else
    {
        *pResult = CDRF_DODEFAULT;
    }
}

//20080326 ��� �̹��� �ʱ�ȭ, jyh
void CSkinHeaderCtrl::InitHeaderImage()
{
	//��� �̹���
	m_Headerbmp.LoadBitmap(IDB_BITMAP_LISTHEAD);
	m_Headerbmp.GetBitmap(&m_HeaderbmpInfo);
}







CSkinListCtrl::CSkinListCtrl()
{
    m_nColumnCount					= 1;

	m_colorOddText					= RGB(0, 0, 0);
	m_colorOddBg					= RGB(0, 0, 0);
	m_colorEvenText					= RGB(0, 0, 0);	
	m_colorEvenBg					= RGB(0, 0, 0);
    m_colorHiLText					= RGB(0, 0, 0);
	m_colorHiLBg					= RGB(0, 0, 0);
	m_colorBarLine					= RGB(0, 0, 0);
	m_colorBarBg1					= RGB(0, 0, 0);
	m_colorBarBg2					= RGB(0, 0, 0);
	m_colorBarText1					= RGB(0, 0, 0);
	m_colorBarText2					= RGB(0, 0, 0);

	m_colorComplete					= RGB(0, 0, 0);	//20080303 �Ϸ� ��� ��, jyh
	m_hFolderIcon					= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_FOLDER));
	//InitDownStateImage();		//20080325 �ٿ� ���� �̹��� �ʱ�ȭ, jyh
}

CSkinListCtrl::~CSkinListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSkinListCtrl, CListCtrl)
    ON_WM_PAINT()
    ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CSkinListCtrl::PreSubclassWindow() 
{
    CClientDC dc(this);

    ::ZeroMemory(&m_LogFont, sizeof(LOGFONT));
    m_LogFont.lfHeight  = -MulDiv(9, dc.GetDeviceCaps(LOGPIXELSY), 72);
    m_LogFont.lfWeight  = FW_NORMAL;
    m_LogFont.lfCharSet = ANSI_CHARSET;
    lstrcpy(m_LogFont.lfFaceName, _T("����"));
    
    m_HeaderCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
    EnableToolTips(TRUE);
    CListCtrl::PreSubclassWindow();
}

void CSkinListCtrl::SetListColor(const COLORREF colorOddText, const COLORREF colorOddBg, 
								 const COLORREF colorEvenText, const COLORREF colorEvenBg,
								 const COLORREF colorHiLText, const COLORREF colorHiLBg, 
								 const COLORREF colorBarLine, 
								 const COLORREF colorBarText1, const COLORREF colorBarBg1,
								 const COLORREF colorBarText2, const COLORREF colorBarBg2)
{
	m_colorOddText					= colorOddText;
	m_colorOddBg					= colorOddBg;
	m_colorEvenText					= colorEvenText;	
	m_colorEvenBg					= colorEvenBg;
	m_colorHiLText					= colorHiLText;
	m_colorHiLBg					= colorHiLBg;
	m_colorBarLine					= colorBarLine;
	m_colorBarText1					= colorBarText1;
	m_colorBarBg1					= colorBarBg1;
	m_colorBarText2					= colorBarText2;
	m_colorBarBg2					= colorBarBg2;	
}
    
	
    

void CSkinListCtrl::SetHeaderColors(const COLORREF colorBack, const COLORREF colorText)
{
    m_HeaderCtrl.SetColors(colorBack, colorText);
}

void CSkinListCtrl::OnPaint() 
{	
	m_HeaderCtrl.Invalidate(FALSE);
	Default();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ����Ʈ ��Ʈ�� �÷��� ���
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void CSkinListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lpLvDraw = (LPNMLVCUSTOMDRAW)pNMHDR;

	switch(lpLvDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}
	case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			int			nOffset;
			CDC			*pDC = CDC::FromHandle(lpLvDraw->nmcd.hdc);
			CRect		rcHighlight;
			CRect		rcBounds, rcLabel, rcIcon, rcSub, rcColumn;
			CString		strLabel;
			CImageList* pImageList;
			COLORREF	colorText;

			// DC ���� ����
			int			nSavedDC = pDC->SaveDC();
			UINT		nJustify;
			CBrush		brush;


			int iCol = lpLvDraw->iSubItem;
			int iRow = (int)lpLvDraw->nmcd.dwItemSpec;

			if(iRow >= GetItemCount())
				return;


			CFont* pFont = GetFont();
			CFont* pOldFont = (CFont*)pDC->SelectObject(pFont);

			//20080303 ���� �Ϸ�� ����� ȸ������ ǥ���ϱ� ���� ���°� ����, jyh
			int nState;
			
			if(!GetItemData(iRow))
				return;

			memcpy(&nState, (const void*)GetItemData(iRow), sizeof(int));
			//-------------------------------------------------------------------

			// �������� ���ÿ��� ����
			LV_ITEM pLItem;
			pLItem.mask      = LVIF_IMAGE | LVIF_STATE;
			pLItem.iItem     = iRow;
			pLItem.iSubItem  = 0;
			pLItem.stateMask = 0xFFFF;
			GetItem(&pLItem);
			
			BOOL bHighlight = GetItemState(iRow, LVIS_SELECTED);			
			strLabel		= GetItemText(iRow, iCol);
			

			// �׸� ������ ����
			GetItemRect(iRow, rcBounds, LVIR_BOUNDS);
			GetItemRect(iRow, rcLabel,  LVIR_LABEL);
			GetItemRect(iRow, rcIcon,   LVIR_ICON);
			GetSubItemRect(iRow, iCol, LVIR_LABEL, rcSub);
			rcColumn = rcBounds;

			nOffset			= (pDC->GetTextExtent(_T(" "), 1).cx) << 1;
			rcHighlight     = rcBounds;
			rcHighlight.left= rcLabel.left;

			// ������
			pDC->SetBkMode(TRANSPARENT);
			if(bHighlight)
			{
				brush.CreateSolidBrush(m_colorHiLBg);        
				colorText = m_colorHiLText;				
			}
			else
			{
				//20080303 ���� �Ϸ�� ������� üũ, jyh
				if(nState == ITEM_STATE_COMPLETE)
					colorText = m_colorComplete;
				else
				{
					brush.CreateSolidBrush(iRow % 2 ? m_colorOddBg : m_colorEvenBg);				
					colorText = iRow % 2 ? m_colorOddText : m_colorEvenText;	
				}
							
			}
			pDC->FillRect(rcSub, &brush);

			if(brush.m_hObject)
				brush.DeleteObject();

			rcColumn.right = rcColumn.left + GetColumnWidth(0);
			rcLabel.DeflateRect(1, 0, 1, 0);
			
			if(iCol == 1)
			{
				if(pLItem.state & LVIS_STATEIMAGEMASK)
				{
					int nImage = ((pLItem.state & LVIS_STATEIMAGEMASK) >> 12) - 1;
					pImageList = GetImageList(LVSIL_STATE);
					if(pImageList)
					{
						//pImageList->Draw(pDC, nImage, CPoint(rcColumn.left+5, rcColumn.top+1), ILD_TRANSPARENT);
						pImageList->Draw(pDC, nImage, CPoint(rcSub.left+5, rcSub.top+3), ILD_TRANSPARENT);
					}
				}
				
				pImageList = GetImageList(LVSIL_SMALL);
				if(pImageList)
				{
					/*pImageList->Draw(pDC, pLItem.iImage, CPoint(rcIcon.left+7, rcIcon.top),
						(bHighlight ? ILD_BLEND50 : 0) | ILD_TRANSPARENT |
						(pLItem.state & LVIS_OVERLAYMASK));*/
					pImageList->Draw(pDC, pLItem.iImage, CPoint(rcSub.left+4, rcSub.top+2),
						(bHighlight ? ILD_BLEND50 : 0) | ILD_TRANSPARENT |
						(pLItem.state & LVIS_OVERLAYMASK));
				}

				rcSub.DeflateRect(rcIcon.right+4, 2, 0, 0);
				pDC->SetTextColor(colorText);
				pDC->DrawText(strLabel, strLabel.GetLength(), rcSub, DT_SINGLELINE | 
					DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS |
					DT_WORDBREAK);
			}
			else
			{
				LV_COLUMN pColum;
				pColum.mask = LVCF_FMT | LVCF_WIDTH;

				GetColumn(iCol, &pColum);
				switch(pColum.fmt & LVCFMT_JUSTIFYMASK)
				{
				case LVCFMT_RIGHT:
					nJustify = DT_RIGHT;
					break;
				case LVCFMT_CENTER:
					nJustify = DT_CENTER;
					break;
				default:
					nJustify = DT_LEFT;
					break;
				}

				pDC->SetTextColor(colorText);
				rcSub.DeflateRect(1, 0, 1, 0);

				//20080325 �ٿ� ���� �̹��� �׸���, jyh
				if(iCol == 0)
				{
					CRect rt = rcColumn;
					rt.top += 3;
					PRDOWNCTX pItem = (PRDOWNCTX)GetItemData(iRow);
					//DrawStateBitmap(pDC, rt, pItem->nFree);
				}

				////20080916 ����Ʈ ��Ʈ���� ������ ���ֱ� ����, jyh
				//if(iCol == 2)
				//{
				//	PRDOWNCTX pItem = (PRDOWNCTX)GetItemData(iRow);
				//	int nPercent2 = abs((int)(pItem->nFileSizeEnd * 100 / pItem->nFileSizeReal));
				//	CString strBuf;
				//	CRect rcText = rcSub;
				//	rcText.top = rcText.top + 2;
				//	strBuf.Format("%s/%s", SetSizeFormat(pItem->nFileSizeEnd), SetSizeFormat(pItem->nFileSizeReal));
				//	pDC->DrawText(strBuf, rcText, DT_RIGHT|DT_RIGHT|DT_SINGLELINE);
				//}
										
				if(iCol == 4)
				{
					int nState;
					memcpy(&nState, (const void*)GetItemData(iRow), sizeof(int));
					bool bTrans = false;


					CRgn rgnText;
					CPen* pOldPen;
					CPen penSolidB, penSolidW;							

					penSolidB.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
					pOldPen = pDC->SelectObject(&penSolidB);
					pDC->Rectangle(rcSub);
					pDC->SelectObject(pOldPen);                        

					rcSub.DeflateRect(1, 1);

					penSolidW.CreatePen(PS_SOLID, 1, m_colorBarLine);
					pOldPen = pDC->SelectObject(&penSolidW);
					pDC->Rectangle(rcSub);
					pDC->SelectObject(pOldPen);

					rcSub.DeflateRect(1, 1);

						PRDOWNCTX pItem = (PRDOWNCTX)GetItemData(iRow);
						int nPercent2 = abs((int)(pItem->nFileSizeEnd * 100 / pItem->nFileSizeReal));

					CRect rcLeft, rcRight, rcText;
					rcText = rcLeft = rcRight = rcSub;
					//rcLeft.right = rcLeft.left + MulDiv(atoi(strLabel), rcSub.Width(), 100);
					rcLeft.right = rcLeft.left + (rcSub.Width() * nPercent2 / 100); 
					rcRight.left = rcLeft.right;
					rcText.top -= 1;
					rcText.bottom -= 1;	

					// ����� ������ ����
					pDC->FillSolidRect(rcLeft, m_colorBarBg2);		
					// ���� ������ ����
					pDC->FillSolidRect(rcRight, m_colorBarBg1);   

					/*switch(nState)
					{
					case ITEM_STATE_TRANS:
						{
							break;
						}
					case ITEM_STATE_CONNECT:
						{
							strLabel = "������";
							break;
						}
					case ITEM_STATE_WAIT:
						{
							strLabel = "���";
							break;
						}
					case ITEM_STATE_STOP:
						{
							strLabel = "����";
							break;
						}
					case ITEM_STATE_CANCEL:
						{
							strLabel = "���";
							break;
						}
					case ITEM_STATE_ERROR:
						{
							strLabel = "����";
							break;
						}
					case ITEM_STATE_COMPLETE:
						{
							strLabel = "�Ϸ�";
							break;
						}
					case ITEM_STATE_DEFER:
						{
							strLabel = "����� �ʰ�";
							break;
						}	
					}*/
					
					rgnText.CreateRectRgnIndirect(rcLeft);
					pDC->SelectClipRgn(&rgnText);							
					pDC->SetTextColor(m_colorBarText2);			
					pDC->DrawText(strLabel, rcText, DT_VCENTER|DT_CENTER|DT_SINGLELINE);

					rgnText.SetRectRgn(rcRight);
					pDC->SelectClipRgn(&rgnText);							
					pDC->SetTextColor(m_colorBarText1);				
					pDC->DrawText(strLabel, rcText, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
					pDC->SelectClipRgn(NULL);
				}
				else if(iCol == 5)
				{		
					DrawIconEx(pDC->m_hDC, rcSub.left + 1, rcSub.top + 1, m_hFolderIcon, 16, 16, 0, NULL, DI_NORMAL);

					rcSub.left += 19;
					pDC->DrawText(strLabel, strLabel.GetLength(), rcSub, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
				}
				else
					pDC->DrawText(strLabel, strLabel.GetLength(), rcSub, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
			}

			pDC->SelectObject(pOldFont);

			// �������� ��Ŀ���� ������ ������ ��Ŀ�� ������ �׸���
			if(pLItem.state & LVIS_FOCUSED && (GetFocus() == this))
				pDC->DrawFocusRect(rcHighlight);

			// DC�� ������Ų��
			pDC->RestoreDC(nSavedDC);

			*pResult= CDRF_SKIPDEFAULT;
			break;
		}
	}
}
//void CSkinListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMLVCUSTOMDRAW lpLvDraw = (LPNMLVCUSTOMDRAW)pNMHDR;
//
//    switch(lpLvDraw->nmcd.dwDrawStage)
//	{
//		case CDDS_PREPAINT:
//		{
//			*pResult = CDRF_NOTIFYSUBITEMDRAW;
//			break;
//		}
//		case CDDS_ITEMPREPAINT:
//		{
// 			*pResult = CDRF_NOTIFYSUBITEMDRAW;
//			break;
//		}
//		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
//		{
//			int			nOffset;
//			CDC			*pDC = CDC::FromHandle(lpLvDraw->nmcd.hdc);
//			CRect		rcHighlight;
//			CRect		rcBounds, rcLabel, rcIcon, rcSub, rcColumn;
//			CString		strLabel;
//			CImageList* pImageList;
//			COLORREF	colorText;
//
//			// DC ���� ����
//			int			nSavedDC = pDC->SaveDC();
//			UINT		nJustify;
//			CBrush		brush;
//
//
//			int iCol = lpLvDraw->iSubItem;
//			int iRow = (int)lpLvDraw->nmcd.dwItemSpec;
//
//			
//			CFont* pFont = GetFont();
//			CFont* pOldFont = (CFont*)pDC->SelectObject(pFont);
//
//
//			// �������� ���ÿ��� ����
//			LV_ITEM pLItem;
//			pLItem.mask      = LVIF_IMAGE | LVIF_STATE;
//			pLItem.iItem     = iRow;
//			pLItem.iSubItem  = 0;
//			pLItem.stateMask = 0xFFFF;
//			GetItem(&pLItem);			
//
//			BOOL bHighlight = GetItemState(iRow, LVIS_SELECTED);			
//			strLabel		= GetItemText(iRow, iCol);
//			
//			// �׸� ������ ����
//			GetItemRect(iRow, rcBounds, LVIR_BOUNDS);
//			GetItemRect(iRow, rcLabel,  LVIR_LABEL);
//			GetItemRect(iRow, rcIcon,   LVIR_ICON);
//			GetSubItemRect(iRow, iCol, LVIR_LABEL, rcSub);
//			rcColumn = rcBounds;
//			
//			nOffset			= (pDC->GetTextExtent(_T(" "), 1).cx) << 1;
//			rcHighlight     = rcBounds;
//			rcHighlight.left= rcLabel.left;
//
//			// ������
//			pDC->SetBkMode(TRANSPARENT);
//			if(bHighlight)
//			{
//				brush.CreateSolidBrush(m_colorHiLBg);        
//				colorText = m_colorHiLText;				
//			}
//			else
//			{
//				brush.CreateSolidBrush(iRow % 2 ? m_colorOddBg : m_colorEvenBg);				
//				colorText = iRow % 2 ? m_colorOddText : m_colorEvenText;				
//			}
//			pDC->FillRect(rcSub, &brush);
//
//			if(brush.m_hObject)
//				brush.DeleteObject();
//
//			rcColumn.right = rcColumn.left + GetColumnWidth(0);
//			rcLabel.DeflateRect(1, 0, 1, 0);
//
//			if(iCol == 0)
//			{
//				if(pLItem.state & LVIS_STATEIMAGEMASK)
//				{
//					int nImage = ((pLItem.state & LVIS_STATEIMAGEMASK) >> 12) - 1;
//					pImageList = GetImageList(LVSIL_STATE);
//					if(pImageList)
//					{
//						pImageList->Draw(pDC, nImage, CPoint(rcColumn.left, rcColumn.top), ILD_TRANSPARENT);
//					}
//				}
//		    
//				pImageList = GetImageList(LVSIL_SMALL);
//				if(pImageList)
//				{
//					pImageList->Draw(pDC, pLItem.iImage, CPoint(rcIcon.left, rcIcon.top),
//									(bHighlight ? ILD_BLEND50 : 0) | ILD_TRANSPARENT |
//									(pLItem.state & LVIS_OVERLAYMASK));
//				}
//				
//				rcSub.DeflateRect(3, 0, 0, 0);
//				pDC->SetTextColor(colorText);
//				pDC->DrawText(strLabel, strLabel.GetLength(), rcSub, DT_SINGLELINE | 
//								DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS |
//								DT_WORDBREAK);
//			}
//			else
//			{
//				LV_COLUMN pColum;
//				pColum.mask = LVCF_FMT | LVCF_WIDTH;
//
//				GetColumn(iCol, &pColum);
//				switch(pColum.fmt & LVCFMT_JUSTIFYMASK)
//				{
//					case LVCFMT_RIGHT:
//						nJustify = DT_RIGHT;
//						break;
//					case LVCFMT_CENTER:
//						nJustify = DT_CENTER;
//						break;
//					default:
//						nJustify = DT_LEFT;
//						break;
//				}
//				
//				pDC->SetTextColor(colorText);
//				rcSub.DeflateRect(1, 0, 1, 0);
//				if(iCol ==2)
//					int i = 0;
//				
//				if(iCol == 3)
//				{
//					int nState;
//					memcpy(&nState, (const void*)GetItemData(iRow), sizeof(int));
//					bool bTrans = false;
//					
//					
//					CRgn rgnText;
//					CPen* pOldPen;
//					CPen penSolidB, penSolidW;							
//
//					penSolidB.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
//					pOldPen = pDC->SelectObject(&penSolidB);
//					pDC->Rectangle(rcSub);
//					pDC->SelectObject(pOldPen);                        
//
//					rcSub.DeflateRect(1, 1);
//
//					penSolidW.CreatePen(PS_SOLID, 1, m_colorBarLine);
//					pOldPen = pDC->SelectObject(&penSolidW);
//					pDC->Rectangle(rcSub);
//					pDC->SelectObject(pOldPen);
//
//					rcSub.DeflateRect(1, 1);
//
//					CRect rcLeft, rcRight, rcText;
//					rcText = rcLeft = rcRight = rcSub;
//					rcLeft.right = rcLeft.left + MulDiv(atoi(strLabel), rcSub.Width(), 100);
//					rcRight.left = rcLeft.right;
//					rcText.top -= 1;
//					rcText.bottom -= 1;	
//
//					// ����� ������ ����
//					pDC->FillSolidRect(rcLeft, m_colorBarBg2);		
//					// ���� ������ ����
//					pDC->FillSolidRect(rcRight, m_colorBarBg1);   
//
//					switch(nState)
//					{
//						case ITEM_STATE_TRANS:
//						{
//							break;
//						}
//						case ITEM_STATE_CONNECT:
//						{
//							strLabel = "������";
//							break;
//						}
//						case ITEM_STATE_WAIT:
//						{
//							strLabel = "���";
//							break;
//						}
//						case ITEM_STATE_STOP:
//						{
//							strLabel = "����";
//							break;
//						}
//						case ITEM_STATE_CANCEL:
//						{
//							strLabel = "���";
//							break;
//						}
//						case ITEM_STATE_ERROR:
//						{
//							strLabel = "����";
//							break;
//						}
//						case ITEM_STATE_COMPLETE:
//						{
//							strLabel = "�Ϸ�";
//							break;
//						}
//						case ITEM_STATE_DEFER:
//						{
//							strLabel = "����";
//							break;
//						}	
//					}
//
//					rgnText.CreateRectRgnIndirect(rcLeft);
//					pDC->SelectClipRgn(&rgnText);							
//					pDC->SetTextColor(m_colorBarText2);			
//					pDC->DrawText(strLabel, rcText, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
//
//					rgnText.SetRectRgn(rcRight);
//					pDC->SelectClipRgn(&rgnText);							
//					pDC->SetTextColor(m_colorBarText1);				
//					pDC->DrawText(strLabel, rcText, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
//					pDC->SelectClipRgn(NULL);
//				}
//				else if(iCol == 5)
//				{		
//					DrawIconEx(pDC->m_hDC, rcSub.left + 1, rcSub.top + 1, m_hFolderIcon, 16, 16, 0, NULL, DI_NORMAL);
//
//					rcSub.left += 19;
//					pDC->DrawText(strLabel, strLabel.GetLength(), rcSub, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
//				}
//				else
//					pDC->DrawText(strLabel, strLabel.GetLength(), rcSub, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
//			}
//
//			pDC->SelectObject(pOldFont);
//
//			// �������� ��Ŀ���� ������ ������ ��Ŀ�� ������ �׸���
//			if(pLItem.state & LVIS_FOCUSED && (GetFocus() == this))
//				pDC->DrawFocusRect(rcHighlight);
//		    
//			// DC�� ������Ų��
//			pDC->RestoreDC(nSavedDC);
//
//			*pResult= CDRF_SKIPDEFAULT;
//			break;
//		}
//	}
//}



BOOL CSkinListCtrl::GetCellRect(int iRow, int iCol, int nArea, CRect &rect)
{
	if(iCol)
		return GetSubItemRect(iRow, iCol, nArea, rect);
	
	if(GetHeaderCtrl()->GetItemCount() == 1)
		return GetItemRect(iRow, rect, nArea);
	
	CRect rCol1;
	if(!GetSubItemRect(iRow, 1, nArea, rCol1))
		return FALSE;
	
	if(!GetItemRect(iRow, rect, nArea))
		return FALSE;
	
	rect.right = rCol1.left;
	
	return TRUE;
}


void CSkinListCtrl::OnDestroy() 
{
    m_HeaderCtrl.UnsubclassWindow();

    CListCtrl::OnDestroy();
}

void CSkinListCtrl::SetToolTips(BOOL bEnable)
{
    EnableToolTips(bEnable);
}

int CSkinListCtrl::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
    int   nRow = -1, nCol = 1;
    CRect rcCell(1, 1, 1, 1);
    nRow = GetRowFromPoint(point, &rcCell, &nCol);

    if(nRow == -1) 
        return -1;

    pTI->hwnd     = m_hWnd;
    pTI->uId      = (UINT)((nRow << 10) + (nCol & 0x3ff) + 1);
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->rect     = rcCell;

    return (int)pTI->uId;
}

BOOL CSkinListCtrl::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
    // �޽����� ANSI �� UNICODE ������ �ڵ��� �ʿ���
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    CString strTipText;
    UINT    nID = (UINT)pNMHDR->idFrom;

    if(nID == 0)
        return FALSE; // ������ �����Ǿ���

    int nRow   = ((nID-1) >> 10) & 0x3fffff ;
    int nCol   = (nID-1) & 0x3ff;
    strTipText = GetItemText(nRow, nCol);
	
	if(lstrlen(strTipText) >= 80)
		return FALSE;

#ifndef _UNICODE
    if(pNMHDR->code == TTN_NEEDTEXTA)
        lstrcpyn(pTTTA->szText, strTipText, MAX_PATH);
    else
        _mbstowcsz(pTTTW->szText, strTipText, MAX_PATH);
#else
    if(pNMHDR->code == TTN_NEEDTEXTA)
        _wcstombsz(pTTTA->szText, strTipText, MAX_PATH);
    else
        lstrcpyn(pTTTW->szText, strTipText, MAX_PATH);
#endif

    *pResult = 0;

    return TRUE;
}

int CSkinListCtrl::GetRowFromPoint(CPoint &point, CRect *rcCell, int *nCol) const
{
	int   nColumn, nRow, nBottom, nWidth;
    CRect rcItem, rcClient;

	// ListCtrl�� LVS_REPORT Ÿ���� �� Ȯ��
	if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return -1;

	// ���� ���̴� ó���� ���� Row ���� ��´�
	nRow    = GetTopIndex();
	nBottom = nRow + GetCountPerPage();
	if(nBottom > GetItemCount())
		nBottom = GetItemCount();

    // ���̴� Row��ŭ ������ ����
	for(; nRow <= nBottom ; nRow++)
	{
		GetItemRect(nRow, &rcItem, LVIR_BOUNDS);
		if(rcItem.PtInRect(point))
		{
			// Column�� ã�´�
			for(nColumn = 0 ; nColumn < m_nColumnCount ; nColumn++)
			{
				nWidth = GetColumnWidth(nColumn);
				if(point.x >= rcItem.left && 
                   point.x <= (rcItem.left + nWidth))
				{
					GetClientRect(&rcClient);
					if(nCol)
                        *nCol = nColumn;
					rcItem.right = rcItem.left + nWidth;

					// ������ ���� Ŭ���̾�Ʈ ������ ����� �ʰ�
					if(rcItem.right > rcClient.right) 
						rcItem.right = rcClient.right;
					*rcCell = rcItem;
					return nRow;
				}
				rcItem.left += nWidth;
			}
		}
	}
	return -1;
}

////20080325 �ٿ�ε� ���� �̹��� �ʱ�ȭ, jyh
//void CSkinListCtrl::InitDownStateImage()
//{	
//	//�ٿ� �̹���
//	m_DownStateBmp.LoadBitmap(IDB_BITMAP_STATE);
//	m_DownStateBmp.GetBitmap(&m_bmpinfo);
//}

////20080325 �ٿ�ε� ���� �̹��� �׸���, jyh
//void CSkinListCtrl::DrawStateBitmap(CDC* pdc, CRect& rt, int nState)
//{	
//	CDC dcmem;
//	dcmem.CreateCompatibleDC(pdc);
//	dcmem.SelectObject(&m_DownStateBmp);
//
//	if(nState == 1)	//����
//		pdc->StretchBlt(rt.left+5, rt.top, m_bmpinfo.bmWidth/2, m_bmpinfo.bmHeight, &dcmem,
//		0, 0, m_bmpinfo.bmWidth/2, m_bmpinfo.bmHeight, SRCCOPY);
//	else	//���
//		pdc->StretchBlt(rt.left+5, rt.top, m_bmpinfo.bmWidth/2, m_bmpinfo.bmHeight, &dcmem,
//		m_bmpinfo.bmWidth/2, 0, m_bmpinfo.bmWidth/2, m_bmpinfo.bmHeight, SRCCOPY);
//}


//BOOL CSkinListCtrl::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	return CListCtrl::OnEraseBkgnd(pDC);
//}
