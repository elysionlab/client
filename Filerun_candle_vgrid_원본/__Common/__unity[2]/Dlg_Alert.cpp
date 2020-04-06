#include "stdafx.h"
#include "Dlg_Alert.h"
#include "config_client.h"


Dlg_Alert::Dlg_Alert(CWnd* pParent /*=NULL*/)
	: CSkinDialog(Dlg_Alert::IDD, pParent)
{
	m_bAutoClose	= 0;
}

Dlg_Alert::~Dlg_Alert()
{
}

void Dlg_Alert::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NOTICE_INFO, m_pCtrl_Text);
}


BEGIN_MESSAGE_MAP(Dlg_Alert, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()




BOOL Dlg_Alert::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// 윈도우 초기화
	InitWindow();

	m_pCtrl_Text.SetWindowText(m_pInfo);
	m_pCtrl_Text.MoveWindow(DLGALERT_INFO_RECT);

	if(m_bAutoClose == 1)
		SetTimer(1, 8000, NULL);
	//20081025 세션 연결 끊김 메세지 일때 5분 동안 띄운다, jyh
	else if(m_bAutoClose == 2)
		SetTimer(2, 300000, NULL);
	//20071119 재시도창 일때 30초 동안 띄운다, jyh
	else if(m_bAutoClose == 3)
		SetTimer(3, 30000, NULL);

	return TRUE;
}

void Dlg_Alert::InitWindow()
{
	Skin_LoadBG(IDB_BITMAP_BG_NOTICE);
	Skin_TitleHeight(0);
	//Skin_InsertButton(IDB_BITMAP_BTN_SETUP_OK,	"BTN_EXIT",		FALSE, DLGALERT_CLOSE_RECT);
	//20071119 수정, jyh
	if(m_bAutoClose == 0 || m_bAutoClose == 1)
		Skin_InsertButton(IDB_BITMAP_BTN_SETUP_OK,	"BTN_EXIT",	FALSE, DLGALERT_CLOSE_RECT);
	else if(m_bAutoClose == 2)	
	{
		Skin_InsertButton(IDB_BITMAP_BTN_CONFIRM_YES,	"BTN_YES",		FALSE, DLGALERT_YES_RECT);
		Skin_InsertButton(IDB_BITMAP_BTN_CONFIRM_NO,	"BTN_NO",		FALSE, DLGALERT_NO_RECT);
	}
	else if(m_bAutoClose == 3)
	{
		Skin_InsertButton(IDB_BITMAP_BTN_ALRET_RETRY,		"BTN_RETRY",		FALSE, DLGALERT_RETRY_RECT);
		Skin_InsertButton(IDB_BITMAP_BTN_ALRET_JUMP,		"BTN_JUMP",			FALSE, DLGALERT_JUMP_RECT);
		Skin_InsertButton(IDB_BITMAP_BTN_SETUP_CANCEL,		"BTN_CANCEL",		FALSE, DLGALERT_CANCEL_RECT);
	}
}


void Dlg_Alert::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		KillTimer(1);
		Skin_ButtonPressed("BTN_EXIT");
	}
	//20071119 서버와 연결이 끊어 졌을때 기본 종료는 '아니오' 버튼이다, jyh
	else if(nIDEvent == 2)
	{
		KillTimer(2);
		Skin_ButtonPressed("BTN_NO");
	}
	//20081028 재시도창 기본 종료는 '재시도' 버튼이다, jyh
	else if(nIDEvent == 3)
	{
		KillTimer(3);
		Skin_ButtonPressed("BTN_RETRY");
	}
}

bool Dlg_Alert::Skin_ButtonPressed(CString m_ButtonName)
{
	if(m_ButtonName == "BTN_EXIT")
	{
		EndDialog(IDOK);
		return false;
	}
	//20071119 추가, jyh
	else if(m_ButtonName == "BTN_RETRY")
		EndDialog(IDRETRY);
	else if(m_ButtonName == "BTN_JUMP")
		EndDialog(IDIGNORE);
	else if(m_ButtonName == "BTN_YES")
		EndDialog(IDYES);
	else if(m_ButtonName == "BTN_NO")
		EndDialog(IDNO);
	else if(m_ButtonName == "BTN_CANCEL")
		EndDialog(IDCANCEL);

	return true;
}

HBRUSH Dlg_Alert::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
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
    return hbr;
}
