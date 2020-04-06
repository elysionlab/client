// Dlg_Slide.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Dlg_Slide.h"
#include "config_client.h"


// Dlg_Slide 대화 상자입니다.

Dlg_Slide::Dlg_Slide(CWnd* pParent /*=NULL*/)
	: CSkinDialog(Dlg_Slide::IDD, pParent)
{
	m_bUp = TRUE;
}

Dlg_Slide::~Dlg_Slide()
{
}

void Dlg_Slide::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_static_text);
}


BEGIN_MESSAGE_MAP(Dlg_Slide, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// Dlg_Slide 메시지 처리기입니다.
BOOL Dlg_Slide::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// 윈도우 초기화
	InitWindow();

	//클라이언트 화면 해상도 구함
	HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	int width = GetDeviceCaps(hDC, HORZRES);
	int height = GetDeviceCaps(hDC, VERTRES);
	RECT rt;
	RECT rectTo;

	GetWindowRect(&rt);

	//작업 표시줄 크기 구함
	HWND hShell_TrayWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL); 
	::GetWindowRect(hShell_TrayWnd, &rectTo);

	//슬라이딩 윈도 포지션 세팅 & 최상위 윈도로 만든다.
	SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), 0, 0, 0, 0, SWP_HIDEWINDOW);
	//최상위 윈도 속성을 없앤다.
	SetWindowPos(CWnd::FromHandle(HWND_NOTOPMOST), 0, 0, 0, 0, SWP_HIDEWINDOW);

	//작업표시줄 윈도 포지션 세팅
	::SetWindowPos(hShell_TrayWnd, HWND_TOPMOST, rectTo.left, rectTo.top,
					rectTo.right, rectTo.bottom, SWP_SHOWWINDOW);

	int wndWidth = rt.right - rt.left;
	m_WndHeight = rt.bottom - rt.top;
	MoveWindow(width-wndWidth, rectTo.top, wndWidth, m_WndHeight);

	SetTimer(1, 50, NULL);

	return TRUE;
}

void Dlg_Slide::InitWindow()
{
	Skin_LoadBG(IDB_BITMAP_BG_SLIDE);
	Skin_TitleHeight(0);
	Skin_InsertButton(IDB_BITMAP_BTN_EXIT, "BTN_EXIT", FALSE, DLGSLIDE_EXIT_RECT);

	m_static_text.SetWindowText(m_pInfo);
	m_static_text.MoveWindow(DLGSLIDE_TEXT_RECT);
}

void Dlg_Slide::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	static int nYpos = 0;
	static int nCnt = 0;

	if(nIDEvent == 1)
	{
		if(m_bUp)
		{
			if(nYpos < m_WndHeight)
			{
				RECT rt;
				GetWindowRect(&rt);
				rt.top -= 5;
				rt.bottom -= 5;
				MoveWindow(&rt);
				nYpos += 5;
			}
			else
			{
				m_bUp = FALSE;
			}
		}
		else
		{
			if(nCnt > 80)
			{
				if(nYpos > 0)
				{
					RECT rt;
					GetWindowRect(&rt);
					rt.top += 5;
					rt.bottom += 5;
					MoveWindow(&rt);
					nYpos -= 5;
				}
				else
				{
					KillTimer(1);
					EndDialog(IDOK);
				}
			}
			else
				nCnt++;			
		}
	}

	CSkinDialog::OnTimer(nIDEvent);
}

bool Dlg_Slide::Skin_ButtonPressed(CString m_ButtonName)
{
	if(m_ButtonName == "BTN_EXIT")
	{
		KillTimer(1);
		EndDialog(IDOK);
		return true;
	}
	
	return false;
}

HBRUSH Dlg_Slide::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	switch(nCtlColor)
	{
	case CTLCOLOR_STATIC:
		{
			UINT nCtrlID = pWnd->GetDlgCtrlID();

			switch(nCtrlID)
			{				
			case IDC_STATIC_NOTICE_INFO:
				{
					pDC->SetBkColor(DLGALERT_BACK_COLOR);
					pDC->SetTextColor(DLGALERT_TEXT_COLOR);
					return CreateSolidBrush(DLGALERT_BACK_COLOR);
				}
			}

			pDC->SetBkMode(TRANSPARENT); // 배경을 투명하게
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
