// Dlg_Slide.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Dlg_Slide.h"
#include "config_client.h"


// Dlg_Slide ��ȭ �����Դϴ�.

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


// Dlg_Slide �޽��� ó�����Դϴ�.
BOOL Dlg_Slide::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// ������ �ʱ�ȭ
	InitWindow();

	//Ŭ���̾�Ʈ ȭ�� �ػ� ����
	HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	int width = GetDeviceCaps(hDC, HORZRES);
	int height = GetDeviceCaps(hDC, VERTRES);
	RECT rt;
	RECT rectTo;

	GetWindowRect(&rt);

	//�۾� ǥ���� ũ�� ����
	HWND hShell_TrayWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL); 
	::GetWindowRect(hShell_TrayWnd, &rectTo);

	//�����̵� ���� ������ ���� & �ֻ��� ������ �����.
	SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), 0, 0, 0, 0, SWP_HIDEWINDOW);
	//�ֻ��� ���� �Ӽ��� ���ش�.
	SetWindowPos(CWnd::FromHandle(HWND_NOTOPMOST), 0, 0, 0, 0, SWP_HIDEWINDOW);

	//�۾�ǥ���� ���� ������ ����
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
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

			pDC->SetBkMode(TRANSPARENT); // ����� �����ϰ�
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}
