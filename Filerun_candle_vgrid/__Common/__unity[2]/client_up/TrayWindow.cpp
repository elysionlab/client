// TrayWindow.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "TrayWindow.h"
#include ".\traywindow.h"
#include "config_client.h"
#include "up_window.h"	//20080225 jyh


// CTrayWindow ��ȭ �����Դϴ�.

CTrayWindow::CTrayWindow(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CTrayWindow::IDD, pParent)
{
	m_nFileTransPercent = 0;
	m_pTipPosition		= NULL;
}

CTrayWindow::~CTrayWindow()
{
	if(m_pTipPosition)
	{
		delete m_pTipPosition;
		m_pTipPosition = NULL;
	}
}

void CTrayWindow::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_static_text);
	DDX_Control(pDX, IDC_PROGRESS1, m_Ctrl_progressTotal);
	DDX_Control(pDX, IDC_STATIC_PERCENT, m_static_Percent);
}

// Dlg_Slide �޽��� ó�����Դϴ�.
BOOL CTrayWindow::OnInitDialog()
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

	int wndWidth = rt.right - rt.left;
	int wndHeight = rt.bottom - rt.top;
	MoveWindow(width-wndWidth, rectTo.top-wndHeight, wndWidth, wndHeight);
	m_Ctrl_progressTotal.MoveWindow(8, 30, 103, 12);
	m_static_Percent.MoveWindow(114, 30, 28, 12);
	
	return TRUE;
}

void CTrayWindow::InitWindow()
{
	up_window* pMainWindow = (up_window*)AfxGetApp()->m_pMainWnd;
	m_nFileTransPercent = pMainWindow->m_nPercent;
	Skin_LoadBG(IDB_BITMAP_BG_TRAY);
	Skin_TitleHeight(0);
	//Skin_InsertButton(IDB_BITMAP_BTN_EXIT, "BTN_EXIT", FALSE, DLGSLIDE_EXIT_RECT);

	m_static_text.SetWindowText(m_pInfo);
	m_Ctrl_progressTotal.SetStyle(PROGRESS_BITMAP);
	m_Ctrl_progressTotal.SetBitmap(IDB_BITMAP_TRAY_PROGRESSBAR1, IDB_BITMAP_TRAY_PROGRESSBAR2);
	m_Ctrl_progressTotal.SetRange(0, 100);	
	m_Ctrl_progressTotal.SetStep(1);
	m_Ctrl_progressTotal.SetPos(m_nFileTransPercent);
	CString str;
	str.Format("%d%%", m_nFileTransPercent);
	m_static_Percent.SetWindowText(str);
	m_static_Percent.Invalidate();
	//m_static_text.MoveWindow(DLGSLIDE_TEXT_RECT);
}

void CTrayWindow::SetTrayIconHandle(HWND hWnd)
{
	m_pTipPosition = new CTrayIconPosition;
	m_pTipPosition->InitializePositionTracking(hWnd, 1);
}

BEGIN_MESSAGE_MAP(CTrayWindow, CSkinDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CTrayWindow �޽��� ó�����Դϴ�.

HBRUSH CTrayWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
