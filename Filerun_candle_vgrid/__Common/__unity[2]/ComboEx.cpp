/*******************************************/
/*                                         */
/* - title : CComboEx.cpp                  */
/* - description : Combo Extends           */
/* - author : you !                        */
/* - date : 2002-09-13                     */
/*                                         */
/*******************************************/


#include "stdafx.h"
#include "ComboEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/* Constructor */
CComboEx::CComboEx()
{
	m_bMouseOnComboBox = FALSE;
	m_bLButton = FALSE;
	m_bFocus = FALSE;
	m_bIcon = FALSE;
} /* end of Constructor */


/* Destructor */
CComboEx::~CComboEx()
{ 
} /* end of Destructor */



/* MESSAGE_MAP */
BEGIN_MESSAGE_MAP(CComboEx, CComboBox)
	//{{AFX_MSG_MAP(CComboEx)
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/* MeasureItem */
void CComboEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	/* �޺��ڽ��� ���̸� �����Ѵ� */
	lpMeasureItemStruct->itemHeight = 18;
} /* end of MeasureItem */


/* OnPaint */
void CComboEx::OnPaint() 
{
	/* Do not call CComboBox::OnPaint() for painting messages */
	Default();

	CRect rcItem;
	GetClientRect(&rcItem);
	CClientDC dc(this);
	
	/* �޺��ڽ� �ܰ� ���� : 1��° ���� �׸��� */
	if(m_bMouseOnComboBox || m_bFocus)	/* ���콺�� �޺��ڽ� ���� �ȿ� �ִٸ�(ComboBox ����) */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
	else	/* ���콺�� �޺��ڽ� ���� �ۿ� �ִٸ� */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
		
	/* Rect�� ��ǥ ���� ��ü������ 1 ��ŭ ���δ� */
	rcItem.DeflateRect(1,1);	
	
	/* �޺��ڽ� �ܰ� ���� : 2��° ���� �׸��� */
	if(IsWindowEnabled())	/* Ȱ��ȭ �϶�  */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
	else					/* ��Ȱ��ȭ �϶� */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNHIGHLIGHT));

	rcItem.DeflateRect(1,1);

	/* rcItem ��ǥ���� Dropdown Button ��ǥ ������ �����Ѵ� */
	/* SM_CXHTHUMB : �޺��ڽ� Dropdown Button ������ */
	rcItem.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB); 

	/* �޺��ڽ� Dropdown Button �ܰ� ���� �׸��� : 1��° */
	dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
	
	rcItem.DeflateRect(1,1);

	/* �޺��ڽ� Dropdown Button �ܰ� ���� �׸��� : 2��° */
	dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
	
	if(!IsWindowEnabled()) /* ��Ȱ��ȭ �̸� */
		return;

	if(m_bMouseOnComboBox || m_bFocus) { /* ���콺�� �޺��ڽ� ���� �ȿ� �ִٸ� */
		if(!m_bLButton) {
			rcItem.top -= 1;
			rcItem.bottom += 1;
			dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW));
		}
		else {	/* Dropdown Button ���� */
			rcItem.top -= 1;
			rcItem.bottom += 1;
			rcItem.OffsetRect(1,1);
			dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
		}
	}
	else {	/* ���콺�� �޺��ڽ� ���� �ۿ� �ִٸ�(Flat) */
		rcItem.top -= 1;
		rcItem.bottom += 1;
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNHIGHLIGHT));

		/* Edit�� Dropdown Button ���̿� �ִ� �� ������ �׸��� */
		rcItem.left -= 1;
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNHIGHLIGHT));
	}
} /* end of OnPaint */


/* OnMouseMove */
void CComboEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(GetCapture() != this) {
		m_bMouseOnComboBox = TRUE;
		SetCapture();
		Invalidate();
	}
	else {
		POINT pt = point;
		ClientToScreen(&pt);
		CWnd* pWnd = WindowFromPoint(pt);	/* ���콺�� ��ġ�� ������ �ڵ��� ��´� */

		/* �޺��ڽ� ������ �ڵ��� �ƴϸ� ĸ�ĸ� �����Ѵ� */
		/* �׷��� �޺��ڽ� �ȿ� ������ �����찡 �����Ѵ� */
		/* �׷��Ƿ� ���콺�� �޺��ڽ��� ������ ���� ��ġ�Ѵٸ� ������ �ڵ� ���� �ٸ��� */
		/* ��, ���콺�� �޺��ڽ��� ������ �����쿡 ��ġ�Ѵٸ� ĸ�ĸ� �����Ѵ� */
		if(pWnd != this) {	
			ReleaseCapture();	/* ĸ�ĸ� �����Ѵ� */

			CRect rect;
			GetClientRect(&rect);

			if(!rect.PtInRect(point)) {	/* ���콺�� ���� �޺��ڽ� �ȿ� ��ġ���� �ʴ´ٸ� */
				m_bMouseOnComboBox = FALSE;
				Invalidate();
			}
		}
	}
	
	CComboBox::OnMouseMove(nFlags, point);
} /* end of OnMouseMove */


/* OnCaptureChanged */
void CComboEx::OnCaptureChanged(CWnd *pWnd) 
{
	/* WM_CAPTURECHANGED �޼����� ȣ�� �ϴ� ������  */
	/*   1. ComboBox ���� ������ ����� */
	/*   2. ComboBox ������ Edit �������� �̵��Ҷ� */
	/*   3. ComboBox ������ Dropdown Button Click �Ҷ� */
	/* ��, ���콺�� ��ġ�� ������ �ڵ��� ����ȴٴ� ����� ���� �� �ִ� */
	/*     �޺��ڽ� ������ �ڵ鿡�� Edit, Dropdown Button ������.... */
	m_bMouseOnComboBox = FALSE;
} /* end of OnCaptureChanged*/


/* OnSetfocus*/
void CComboEx::OnSetfocus() 
{
	m_bFocus = TRUE;
	Invalidate();	
} /* end of OnSetfocus */


/*OnKillfocus*/
void CComboEx::OnKillfocus() 
{
	m_bFocus = FALSE;
	Invalidate();	
} /* end of OnKillfocus */


/*OnLButtonDown*/
void CComboEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLButton = TRUE;
	Invalidate();
	
	CComboBox::OnLButtonDown(nFlags, point);
} /* end of OnLButtonDown */


/*OnLButtonUp*/
void CComboEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLButton = FALSE;
	Invalidate();
	
	CComboBox::OnLButtonUp(nFlags, point);
} /* end of OnLButtonUp */


/* DrawItem */
void CComboEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	UINT action = lpDrawItemStruct->itemAction;
	UINT state = lpDrawItemStruct->itemState;
	CRect rcItem = lpDrawItemStruct->rcItem;

	/* Dropdown Item List ���� ���콺�� Ư�� �����ۿ� ��ġ�Ѵٸ� */
	if((state & ODS_SELECTED) && (action & (ODA_SELECT | ODA_DRAWENTIRE))) {
		CBrush brHighlight(::GetSysColor(COLOR_HIGHLIGHT));
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brHighlight);

		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		CPen* pOldPen = (CPen*)pDC->SelectObject(&penHighlight);

		pDC->Rectangle(&rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));

		/* SetItemData �Լ��� ����Ͽ� ������ �����͸� ��´� */
		_IconComboBoxData* pData = (_IconComboBoxData*)lpDrawItemStruct->itemData;
		ASSERT(pData != NULL);
		
		if(pData && lpDrawItemStruct->itemData != 0xFFFFFFFF) {
			/* ������ �׸��� */
			DrawIconEx(pDC->m_hDC, rcItem.left + 1, rcItem.top + 1, pData->m_hIcon, 16, 16, 0, NULL, DI_NORMAL);

			/* ���ڿ� �׸��� */
			rcItem.left += 19;
			pDC->DrawText(pData->m_strItem, rcItem, DT_SINGLELINE |DT_VCENTER ); 
		}

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	/* Dropdown Item List ���� ���콺�� Ư�� �����ۿ� ��ġ�ߴ� ���� ��ġ�̸� */
	if(!(state & ODS_SELECTED) && (action & (ODA_SELECT | ODA_DRAWENTIRE))) {
		CBrush brWindow(::GetSysColor(COLOR_WINDOW));
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brWindow);

		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));
		CPen* pOldPen = (CPen*)pDC->SelectObject(&penHighlight);

		pDC->Rectangle(&rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

		pDC->Rectangle(&rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

		/* SetItemData �Լ��� ����Ͽ� ������ �����͸� ��´� */
		_IconComboBoxData* pData = (_IconComboBoxData*)lpDrawItemStruct->itemData;
		ASSERT(pData != NULL);

		if(pData && lpDrawItemStruct->itemData != 0xFFFFFFFF) {
			/* ������ �׸��� */
			DrawIconEx(pDC->m_hDC, rcItem.left + 1, rcItem.top + 1, pData->m_hIcon, 16, 16, 0, NULL, DI_NORMAL);

			/* ���ڿ� �׸��� */
			rcItem.left += 19;
			pDC->DrawText(pData->m_strItem, rcItem, DT_SINGLELINE |DT_VCENTER ); 
		}

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}	
} /* end of OnLButtonUp */


/* AddIcom */
int CComboEx::AddIcon(int nIconId, CString str)
{
	int offset;

	/* ���ҽ� ID�� �������� Icon �ڵ��� ��´� */
	HICON hIcon = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIconId),
		IMAGE_ICON, 16, 16, 0);

	ICONINFO ii;
	ZeroMemory(&ii, sizeof(ICONINFO));	/* ����ü�� �ʱ�ȭ ��Ų��  */
	::GetIconInfo(hIcon, &ii);			/* Icon ������ ��´� */

	int nWidth = (BYTE)(ii.xHotspot * 2);	
	int nHeight = (BYTE)(ii.yHotspot * 2);	

	/* 16 * 16 Icon�� �߰��Ѵ� */
	if((nWidth == 16) && (nHeight == 16)) {
		/* �������� �߰��Ͽ� �ε����� ��´� */
		offset = CComboBox::AddString(_T(""));

		_IconComboBoxData* pData = new _IconComboBoxData;
		ASSERT(pData);

		pData->m_hIcon = hIcon;
		pData->m_nIconId = nIconId;
		pData->m_strItem = str;

		/* �ش� �ε����� �����͸� �����Ѵ� */
#pragma warning(disable:4311)
		SetItemData(offset, (DWORD)pData);
#pragma warning(default:4311)

		m_bIcon = TRUE;	/* Dropdown List Width�� ����ϱ� ���Ͽ� */
	}

	return offset;	/* �ε����� �����Ѵ� */
} /* end of AddIcon */


/* InsertIcon */
int CComboEx::InsertIcon(int nIndex, int nIconId, CString str)
{
	int offset;

	/* ���ҽ� ID�� �������� Icon �ڵ��� ��´� */
	HICON hIcon = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIconId),
		IMAGE_ICON, 16, 16, 0);

	ICONINFO ii;
	ZeroMemory(&ii, sizeof(ICONINFO));	/* ����ü�� �ʱ�ȭ ��Ų�� */
	::GetIconInfo(hIcon, &ii);			/* Icon ������ ��´� */

	int nWidth = (BYTE)(ii.xHotspot * 2);	/* Icon Width */
	int nHeight = (BYTE)(ii.yHotspot * 2);	/* Icon Height */

	/* 16 * 16 Icon�� �߰��Ѵ� */
	if((nWidth == 16) && (nHeight == 16)) {

		/* Insert ��ų Index�� ������ ����� */
		if(0 <= nIndex && nIndex <= GetCount())
			offset = CComboBox::InsertString(nIndex, _T("")); /* �������� �߰��Ͽ� �ε����� ��´� */
		else
			return -1;

		_IconComboBoxData* pData = new _IconComboBoxData;
		ASSERT(pData);

		pData->m_hIcon = hIcon;
		pData->m_nIconId = nIconId;
		pData->m_strItem = str;

		/* �ش� �ε����� �����͸� �����Ѵ� */
#pragma warning(disable:4311)
		SetItemData(offset, (DWORD)pData);
#pragma warning(default:4311)

		m_bIcon = TRUE;	/* Dropdown List Width�� ����ϱ� ���Ͽ� */
	}

	return offset;	/* �ε����� �����Ѵ� */
} /* end of InsertIcon */


/* DeleteItem �����Լ��� ��ü�� �Ҹ�ɶ� ȣ��ȴ� */
void CComboEx::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	_IconComboBoxData* pData = (_IconComboBoxData*)lpDeleteItemStruct->itemData;

	/* ������ �ڵ��� �Ҹ��Ѵ� */
	::DestroyIcon(pData->m_hIcon);

	/* �޸𸮸� �����Ѵ� */
	delete pData;
	
	CComboBox::DeleteItem(lpDeleteItemStruct);
} /* end of DeleteItem */


/* DeleteIcon */
void CComboEx::DeleteIcon(int nIndex)
{
	/* �ش� �ε����� �������� �޺��ڽ����� ������ �Ѵ� */
	/* �޸� ���Ŵ� DeleteItem �����Լ����� ó���Ѵ� */
	DeleteString(nIndex);
} /* DeleteIcon */


/* GetCurSelIcon */
int CComboEx::GetCurSelIcon()
{
	int nIndex = GetCurSel();

	/* ���� ���õ� �����ۿ��� SetItemData �Լ��� ����Ͽ� ������ �����͸� ��´� */
	_IconComboBoxData* pData = (_IconComboBoxData*)GetItemData(nIndex);

	return pData->m_nIconId;	/* �������� ID�� �����Ѵ� */
} /* end of GetCurSelIcon */


/*SetCurSelIcon*/
void CComboEx::SetCurSelIcon(int nIconId)
{
	/* ��ϵ� ��� �����͸� ó������ ���Ѵ� */
	for(int nIndex = 0; nIndex < GetCount(); nIndex++) {
		_IconComboBoxData* pData = (_IconComboBoxData*)GetItemData(nIndex);

		if(pData->m_nIconId == nIconId)	{ /* ������ ID�� �����ϸ� */
			SetCurSel(nIndex);	/* �ش� �ε����� Focus ������ �Ѵ� */
			break;
		}
	}
} /* end of SetCurSelIcon */


/*OnDropdown*/
void CComboEx::OnDropdown() 
{
	CString str;
	int nWidth = 0;

	CClientDC dc(this);
	
	/* ���ҽ����� ������ ��Ʈ�� ��� CDC�� �����Ѵ� */
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);

	for(int index = 0; index < GetCount(); index++) {
		if(m_bIcon) {
			_IconComboBoxData* pData = (_IconComboBoxData*)GetItemData(index);

			/* ���ҽ��� ������ ��Ʈ�� �������� ���ڿ��� ����� ��´� */
			CSize sz = dc.GetTextExtent(pData->m_strItem);

			if(sz.cx > nWidth)	/* ���� ������ ū ���� ���Ѵ� */
				nWidth = sz.cx;
		}
		else {
			/* ���ڿ��� ��´� */
			GetLBText(index, str);

			/* ���ҽ��� ������ ��Ʈ�� �������� ���ڿ��� ����� ��´� */
			CSize sz = dc.GetTextExtent(str);	

			if(sz.cx > nWidth)	/* ���� ������ ū ���� ���Ѵ� */
				nWidth = sz.cx;
		}
	}

	nWidth += GetSystemMetrics(SM_CXVSCROLL);	/* VScrroll ���� ũ�⸦ �� �Ѵ� */
	nWidth += dc.GetTextExtent(_T("0")).cx;			/* ���� ������ ���Ѵ�(���� ���� !!) */

	if(m_bIcon)
		nWidth += 16;

	SetDroppedWidth(nWidth);	/* Dropdown List Width�� �����Ѵ� */
} /* end of OnDropdown */


/*PreSubclassWindow*/
void CComboEx::PreSubclassWindow() 
{
	/* ���� ��Ʈ���� ���� ��Ű�� �ʱ�ȭ �Ѵ� */
	m_Tooltip.Create(this);
	m_Tooltip.AddTool(this, "ComboBox");

	CComboBox::PreSubclassWindow();
} /* end of PreSubclassWindow */


/*PreTranslateMessage*/
BOOL CComboEx::PreTranslateMessage(MSG* pMsg) 
{
	/* ���� ��Ʈ�Ѱ� ���콺 �޽����� �����Ѵ� */
	m_Tooltip.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
} /* end of PreTranslateMessage */


/* SetToolTipText */
void CComboEx::SetTooltipText(CString strTooltip)
{
	/* ������ �����Ѵ� */
	m_Tooltip.UpdateTipText(strTooltip, this);
} /* end of SetTooltipText */


/*SetTooltipText*/
void CComboEx::SetTooltipText(int nId)
{
	CString strTooltip;

	/* ���ҽ����� ���ڿ��� �о� ���δ� */
	strTooltip.LoadString(nId);
	if(strTooltip.IsEmpty())
		return;

	/* ������ �����Ѵ� */
	m_Tooltip.UpdateTipText(strTooltip, this);
} /* end of SetTooltipText */
 
