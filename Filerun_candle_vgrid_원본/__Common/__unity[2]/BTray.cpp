#include "stdafx.h"
#include "BTray.h"
//#include "Resource.h"		//20080424 vs8�� ���������� ���� ����, jyh
#include "shellapi.h"


CBTray::CBTray(HWND hWnd, HINSTANCE hInstance, CString pTitle)
{
	m_hWnd = hWnd;
	m_hInstance = hInstance;
	m_pTitle = pTitle;

	TrayIcon_hIcons[0] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	TrayIcon_hIcons[1] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON2));
	TrayIcon_hIcons[2] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON3));
	TrayIcon_hIcons[3] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON4));
	TrayIcon_hIcons[4] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON5));
	TrayIcon_hIcons[5] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON6));
	TrayIcon_hIcons[6] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON7));
	TrayIcon_hIcons[7] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON8));
	TrayIcon_hIcons[8] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON9));
	TrayIcon_hIcons[9] = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON10));

	m_bTray = false;	//20080131 Ʈ���� ������ Ȱ��ȭ ����, jyh
	m_bTrayWndActive = false;	//20080201 Ʈ���� ���� Ȱ��ȭ ����, jyh
	m_pTrayWnd = NULL;			//20080131 jyh
}

CBTray::~CBTray()
{
}

void CBTray::TrayBegin()
{
	m_bTray = TRUE;	//20080131 Ʈ���� ������ Ȱ��, jyh
	IconAdd();
	//::ShowWindow(m_hWnd, SW_MINIMIZE);	//20080226 �ּ� ó��, jyh
	::ShowWindow(m_hWnd, SW_HIDE);
	m_pTrayWnd = new CTrayWindow;			//20080131 jyh
	m_pTrayWnd->m_pInfo = m_pTitle;			//20080131 jyh
	m_pTrayWnd->Create(IDD_DIALOG_TRAY);	//20080131 jyh
	m_pTrayWnd->SetTrayIconHandle(m_hWnd);	//20080225 Ʈ���� ������ �ڵ��� �Ѱ��ش�, jyh
}

void CBTray::TrayEnd()
{
	//IconDel();
	m_bTray = FALSE;	//20080131 Ʈ���� ������ ��Ȱ��, jyh
	::SetTimer(m_hWnd, 96, 500, NULL);
	::ShowWindow(m_hWnd, SW_SHOW);
	//::ShowWindow(m_hWnd, SW_RESTORE);
	m_pTrayWnd->CloseWindow();			//20080131 jyh
	delete m_pTrayWnd;					//20080131 jyh
	m_pTrayWnd = NULL;					//20080131 jyh
}

void CBTray::OnTrayIconNotify(UINT uMessage)
{
	switch(uMessage) {
		
		case WM_LBUTTONDBLCLK:
		{
			TrayEnd();
			break;
		}
		//case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			SetForegroundWindow(m_hWnd);	//20090314 �˾��޴��� ��Ŀ���� ������ �˾��޴� ������� �ϱ�����, jyh
			OnShowMenu();
			break;
		}
	}
}

void CBTray::OnShowMenu()
{
	POINT pt;
	HMENU hPopup = ::CreatePopupMenu();
	
	/*AppendMenu�� ���ؼ� �޴��� �߰��� �� �ִ�.*/
	//::AppendMenu(hPopup, MF_STRING, WM_TRAYICON_MENU_OPEN, TEXT("��������Ʈ��"));
	::AppendMenu(hPopup, MF_STRING, WM_TRAYICON_MENU_HOME, TEXT(CLIENT_HOMEPAGE));
	::AppendMenu(hPopup, MF_STRING, WM_TRAYICON_MENU_EXIT, TEXT("�� ��"));
	
	/*SetMenuDefaultItem �޴����� ���ϰ� ǥ���Ǵ� �޴� ���ʸ��콺 ����Ŭ���� ��Ÿ���� ������ �ش�*/
	::SetMenuDefaultItem(hPopup, 0, TRUE);
	::GetCursorPos(&pt);
	::SetForegroundWindow(m_hWnd);
	::TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	::DestroyMenu(hPopup);
}


void CBTray::IconAdd()
{
	NOTIFYICONDATA IconData;
	ZeroMemory(&IconData, sizeof(NOTIFYICONDATA));
	IconData.cbSize = sizeof(NOTIFYICONDATA);
	IconData.hWnd = m_hWnd;
	IconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	IconData.hIcon = TrayIcon_hIcons[0];
	IconData.uCallbackMessage = WM_TRAYICON_NOTIFY;
	//lstrcpy(IconData.szTip, m_pTitle);
	IconData.uID = 0;
	Shell_NotifyIcon(NIM_ADD, &IconData);

	::SetTimer(m_hWnd, 97, 200, NULL);
}


void CBTray::IconDel()
{
	::KillTimer(m_hWnd, 97);

	NOTIFYICONDATA IconData;
	ZeroMemory(&IconData, sizeof(NOTIFYICONDATA));
	IconData.cbSize = sizeof(NOTIFYICONDATA);
	IconData.hWnd = m_hWnd;
	IconData.uFlags = NULL;
	Shell_NotifyIcon(NIM_DELETE, &IconData);	
}

void CBTray::IconChange()
{
	TrayIcon_IconIndex++;
	if(TrayIcon_IconIndex > 9)
		TrayIcon_IconIndex = 0;

	NOTIFYICONDATA IconData;
	ZeroMemory(&IconData, sizeof(NOTIFYICONDATA));
	IconData.cbSize = sizeof(NOTIFYICONDATA);
	IconData.hWnd = m_hWnd;
	IconData.uFlags = NIF_ICON;
	IconData.hIcon = TrayIcon_hIcons[TrayIcon_IconIndex];		
	Shell_NotifyIcon(NIM_MODIFY, &IconData);
}

void CBTray::IconFix(HICON hIcon)
{
	NOTIFYICONDATA IconData;
	ZeroMemory(&IconData, sizeof(NOTIFYICONDATA));
	IconData.cbSize = sizeof(NOTIFYICONDATA);
	IconData.hWnd = m_hWnd;
	IconData.uFlags = NIF_ICON;
	IconData.hIcon = hIcon;		
	Shell_NotifyIcon(NIM_MODIFY, &IconData);
}
