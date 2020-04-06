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
	/* 콤보박스의 높이를 지정한다 */
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
	
	/* 콤보박스 외곽 라인 : 1번째 라인 그리기 */
	if(m_bMouseOnComboBox || m_bFocus)	/* 마우스가 콤복박스 영역 안에 있다면(ComboBox 오목) */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
	else	/* 마우스가 콤복박스 영역 밖에 있다면 */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
		
	/* Rect의 좌표 값을 전체적으로 1 만큼 줄인다 */
	rcItem.DeflateRect(1,1);	
	
	/* 콤보박스 외곽 라인 : 2번째 라인 그리기 */
	if(IsWindowEnabled())	/* 활성화 일때  */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
	else					/* 비활성화 일때 */
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNHIGHLIGHT));

	rcItem.DeflateRect(1,1);

	/* rcItem 좌표값을 Dropdown Button 좌표 값으로 조정한다 */
	/* SM_CXHTHUMB : 콤보박스 Dropdown Button 사이즈 */
	rcItem.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB); 

	/* 콤보박스 Dropdown Button 외곽 라인 그리기 : 1번째 */
	dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
	
	rcItem.DeflateRect(1,1);

	/* 콤보박스 Dropdown Button 외곽 라인 그리기 : 2번째 */
	dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNFACE));
	
	if(!IsWindowEnabled()) /* 비활성화 이면 */
		return;

	if(m_bMouseOnComboBox || m_bFocus) { /* 마우스가 콤복박스 영역 안에 있다면 */
		if(!m_bLButton) {
			rcItem.top -= 1;
			rcItem.bottom += 1;
			dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW));
		}
		else {	/* Dropdown Button 오목 */
			rcItem.top -= 1;
			rcItem.bottom += 1;
			rcItem.OffsetRect(1,1);
			dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
		}
	}
	else {	/* 마우스가 콤복박스 영역 밖에 있다면(Flat) */
		rcItem.top -= 1;
		rcItem.bottom += 1;
		dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNHIGHLIGHT));

		/* Edit와 Dropdown Button 사이에 있는 빈 공간을 그리기 */
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
		CWnd* pWnd = WindowFromPoint(pt);	/* 마우스가 위치한 윈도우 핸들을 얻는다 */

		/* 콤보박스 윈도우 핸들이 아니면 캡쳐를 헤제한다 */
		/* 그러나 콤보박스 안에 에디터 윈도우가 존재한다 */
		/* 그러므로 마우스가 콤보박스의 에디터 위에 위치한다면 윈도우 핸들 값은 다르다 */
		/* 즉, 마우스가 콤보박스의 에디터 윈도우에 위치한다면 캡쳐만 해제한다 */
		if(pWnd != this) {	
			ReleaseCapture();	/* 캡쳐를 헤제한다 */

			CRect rect;
			GetClientRect(&rect);

			if(!rect.PtInRect(point)) {	/* 마우스가 현재 콤보박스 안에 위치하지 않는다면 */
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
	/* WM_CAPTURECHANGED 메세지가 호출 하는 순간은  */
	/*   1. ComboBox 영역 밖으로 벗어날때 */
	/*   2. ComboBox 영역의 Edit 영역으로 이동할때 */
	/*   3. ComboBox 영역의 Dropdown Button Click 할때 */
	/* 즉, 마우스가 위치한 윈도우 핸들이 변경된다는 결론을 얻을 수 있다 */
	/*     콤보박스 윈도우 핸들에서 Edit, Dropdown Button 등으로.... */
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

	/* Dropdown Item List 에서 마우스가 특정 아이템에 위치한다면 */
	if((state & ODS_SELECTED) && (action & (ODA_SELECT | ODA_DRAWENTIRE))) {
		CBrush brHighlight(::GetSysColor(COLOR_HIGHLIGHT));
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brHighlight);

		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		CPen* pOldPen = (CPen*)pDC->SelectObject(&penHighlight);

		pDC->Rectangle(&rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));

		/* SetItemData 함수를 사용하여 설정한 데이터를 얻는다 */
		_IconComboBoxData* pData = (_IconComboBoxData*)lpDrawItemStruct->itemData;
		ASSERT(pData != NULL);
		
		if(pData && lpDrawItemStruct->itemData != 0xFFFFFFFF) {
			/* 아이콘 그리기 */
			DrawIconEx(pDC->m_hDC, rcItem.left + 1, rcItem.top + 1, pData->m_hIcon, 16, 16, 0, NULL, DI_NORMAL);

			/* 문자열 그리기 */
			rcItem.left += 19;
			pDC->DrawText(pData->m_strItem, rcItem, DT_SINGLELINE |DT_VCENTER ); 
		}

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	/* Dropdown Item List 에서 마우스가 특정 아이템에 위치했던 예전 위치이면 */
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

		/* SetItemData 함수를 사용하여 설정한 데이터를 얻는다 */
		_IconComboBoxData* pData = (_IconComboBoxData*)lpDrawItemStruct->itemData;
		ASSERT(pData != NULL);

		if(pData && lpDrawItemStruct->itemData != 0xFFFFFFFF) {
			/* 아이콘 그리기 */
			DrawIconEx(pDC->m_hDC, rcItem.left + 1, rcItem.top + 1, pData->m_hIcon, 16, 16, 0, NULL, DI_NORMAL);

			/* 문자열 그리기 */
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

	/* 리소스 ID를 기준으로 Icon 핸들을 얻는다 */
	HICON hIcon = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIconId),
		IMAGE_ICON, 16, 16, 0);

	ICONINFO ii;
	ZeroMemory(&ii, sizeof(ICONINFO));	/* 구조체를 초기화 시킨다  */
	::GetIconInfo(hIcon, &ii);			/* Icon 정보를 얻는다 */

	int nWidth = (BYTE)(ii.xHotspot * 2);	
	int nHeight = (BYTE)(ii.yHotspot * 2);	

	/* 16 * 16 Icon만 추가한다 */
	if((nWidth == 16) && (nHeight == 16)) {
		/* 아이템을 추가하여 인덱스를 얻는다 */
		offset = CComboBox::AddString(_T(""));

		_IconComboBoxData* pData = new _IconComboBoxData;
		ASSERT(pData);

		pData->m_hIcon = hIcon;
		pData->m_nIconId = nIconId;
		pData->m_strItem = str;

		/* 해당 인덱스에 데이터를 설정한다 */
#pragma warning(disable:4311)
		SetItemData(offset, (DWORD)pData);
#pragma warning(default:4311)

		m_bIcon = TRUE;	/* Dropdown List Width를 계산하기 위하여 */
	}

	return offset;	/* 인덱스를 리턴한다 */
} /* end of AddIcon */


/* InsertIcon */
int CComboEx::InsertIcon(int nIndex, int nIconId, CString str)
{
	int offset;

	/* 리소스 ID를 기준으로 Icon 핸들을 얻는다 */
	HICON hIcon = (HICON)::LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(nIconId),
		IMAGE_ICON, 16, 16, 0);

	ICONINFO ii;
	ZeroMemory(&ii, sizeof(ICONINFO));	/* 구조체를 초기화 시킨다 */
	::GetIconInfo(hIcon, &ii);			/* Icon 정보를 얻는다 */

	int nWidth = (BYTE)(ii.xHotspot * 2);	/* Icon Width */
	int nHeight = (BYTE)(ii.yHotspot * 2);	/* Icon Height */

	/* 16 * 16 Icon만 추가한다 */
	if((nWidth == 16) && (nHeight == 16)) {

		/* Insert 시킬 Index가 범위에 벗어나면 */
		if(0 <= nIndex && nIndex <= GetCount())
			offset = CComboBox::InsertString(nIndex, _T("")); /* 아이템을 추가하여 인덱스를 얻는다 */
		else
			return -1;

		_IconComboBoxData* pData = new _IconComboBoxData;
		ASSERT(pData);

		pData->m_hIcon = hIcon;
		pData->m_nIconId = nIconId;
		pData->m_strItem = str;

		/* 해당 인덱스에 데이터를 설정한다 */
#pragma warning(disable:4311)
		SetItemData(offset, (DWORD)pData);
#pragma warning(default:4311)

		m_bIcon = TRUE;	/* Dropdown List Width를 계산하기 위하여 */
	}

	return offset;	/* 인덱스를 리턴한다 */
} /* end of InsertIcon */


/* DeleteItem 가상함수는 개체가 소멸될때 호출된다 */
void CComboEx::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	_IconComboBoxData* pData = (_IconComboBoxData*)lpDeleteItemStruct->itemData;

	/* 아이콘 핸들을 소멸한다 */
	::DestroyIcon(pData->m_hIcon);

	/* 메모리를 해제한다 */
	delete pData;
	
	CComboBox::DeleteItem(lpDeleteItemStruct);
} /* end of DeleteItem */


/* DeleteIcon */
void CComboEx::DeleteIcon(int nIndex)
{
	/* 해당 인덱스의 아이템을 콤보박스에만 삭제를 한다 */
	/* 메모리 제거는 DeleteItem 가상함수에서 처리한다 */
	DeleteString(nIndex);
} /* DeleteIcon */


/* GetCurSelIcon */
int CComboEx::GetCurSelIcon()
{
	int nIndex = GetCurSel();

	/* 현재 선택된 아이템에서 SetItemData 함수를 사용하여 설정된 데이터를 얻는다 */
	_IconComboBoxData* pData = (_IconComboBoxData*)GetItemData(nIndex);

	return pData->m_nIconId;	/* 아이콘의 ID를 리턴한다 */
} /* end of GetCurSelIcon */


/*SetCurSelIcon*/
void CComboEx::SetCurSelIcon(int nIconId)
{
	/* 등록된 모든 데이터를 처음부터 비교한다 */
	for(int nIndex = 0; nIndex < GetCount(); nIndex++) {
		_IconComboBoxData* pData = (_IconComboBoxData*)GetItemData(nIndex);

		if(pData->m_nIconId == nIconId)	{ /* 동일한 ID가 존재하면 */
			SetCurSel(nIndex);	/* 해당 인덱스에 Focus 설정을 한다 */
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
	
	/* 리소스에서 설정한 폰트를 얻어 CDC에 설정한다 */
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);

	for(int index = 0; index < GetCount(); index++) {
		if(m_bIcon) {
			_IconComboBoxData* pData = (_IconComboBoxData*)GetItemData(index);

			/* 리소스에 설정된 폰트를 기준으로 문자열의 사이즈를 얻는다 */
			CSize sz = dc.GetTextExtent(pData->m_strItem);

			if(sz.cx > nWidth)	/* 가장 사이즈 큰 값을 구한다 */
				nWidth = sz.cx;
		}
		else {
			/* 문자열을 얻는다 */
			GetLBText(index, str);

			/* 리소스에 설정된 폰트를 기준으로 문자열의 사이즈를 얻는다 */
			CSize sz = dc.GetTextExtent(str);	

			if(sz.cx > nWidth)	/* 가장 사이즈 큰 값을 구한다 */
				nWidth = sz.cx;
		}
	}

	nWidth += GetSystemMetrics(SM_CXVSCROLL);	/* VScrroll 바의 크기를 더 한다 */
	nWidth += dc.GetTextExtent(_T("0")).cx;			/* 여유 공간을 더한다(생략 가능 !!) */

	if(m_bIcon)
		nWidth += 16;

	SetDroppedWidth(nWidth);	/* Dropdown List Width를 설정한다 */
} /* end of OnDropdown */


/*PreSubclassWindow*/
void CComboEx::PreSubclassWindow() 
{
	/* 툴팁 컨트롤을 생성 시키며 초기화 한다 */
	m_Tooltip.Create(this);
	m_Tooltip.AddTool(this, "ComboBox");

	CComboBox::PreSubclassWindow();
} /* end of PreSubclassWindow */


/*PreTranslateMessage*/
BOOL CComboEx::PreTranslateMessage(MSG* pMsg) 
{
	/* 툴팁 컨트롤과 마우스 메시지를 연결한다 */
	m_Tooltip.RelayEvent(pMsg);
	return CComboBox::PreTranslateMessage(pMsg);
} /* end of PreTranslateMessage */


/* SetToolTipText */
void CComboEx::SetTooltipText(CString strTooltip)
{
	/* 툴팁을 변경한다 */
	m_Tooltip.UpdateTipText(strTooltip, this);
} /* end of SetTooltipText */


/*SetTooltipText*/
void CComboEx::SetTooltipText(int nId)
{
	CString strTooltip;

	/* 리소스에서 문자열을 읽어 들인다 */
	strTooltip.LoadString(nId);
	if(strTooltip.IsEmpty())
		return;

	/* 툴팁을 변경한다 */
	m_Tooltip.UpdateTipText(strTooltip, this);
} /* end of SetTooltipText */
 
