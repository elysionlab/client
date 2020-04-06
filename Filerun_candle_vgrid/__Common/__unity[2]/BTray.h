#if !defined(AFX_BTRAY_H__6087E78C_62E6_4F10_8D4B_3A85616D914F__INCLUDED_)
#define AFX_BTRAY_H__6087E78C_62E6_4F10_8D4B_3A85616D914F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TrayWindow.h"		//20080131 jyh

#define WM_TRAYICON_NOTIFY			(WM_USER + 10001)
#define WM_TRAYICON_MENU_OPEN		(WM_USER + 10002)
#define WM_TRAYICON_MENU_EXIT		(WM_USER + 10003)
#define WM_TRAYICON_MENU_HOME		(WM_USER + 10004)

class CBTray  
{
public:

	CBTray(HWND hWnd, HINSTANCE hInstance, CString pTitle);
	virtual ~CBTray();	
	
	void TrayBegin();
	void TrayEnd();
	void OnTrayIconNotify(UINT uMessage);
	
	void IconAdd();
	void IconDel();
	void IconChange();
	void IconFix(HICON hIcon);
	void OnShowMenu();
	BOOL IsActiveTray() { return m_bTray; }	//20080131 트레이 아이콘 활성화 여부, jyh
	
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	CString m_pTitle;
    UINT TrayIcon_IconIndex;
    HICON TrayIcon_hIcons[10];
	BOOL m_bTray;						//20080131 트레이 아이콘 활성화 여부, jyh
	CTrayWindow* m_pTrayWnd;			//20080131 jyh
	BOOL		m_bTrayWndActive;		//20080201 트래이 윈도 활성화 여부, jyh
};

#endif // !defined(AFX_BTRAY_H__6087E78C_62E6_4F10_8D4B_3A85616D914F__INCLUDED_)
