#include "stdafx.h"
#include "Dlg_Exist.h"
#include "config_client.h"


Dlg_Exist::Dlg_Exist(CWnd* pParent /*=NULL*/)
	: CSkinDialog(Dlg_Exist::IDD, pParent)
{
	m_nAllApply = 0;
}

Dlg_Exist::~Dlg_Exist()
{
}

void Dlg_Exist::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_EXIST, m_pCtrl_Text);
	DDX_Control(pDX, IDC_STATIC_EXIST2, m_pCtrl_Edit_Path);
	DDX_Control(pDX, IDC_STATIC_EXIST3, m_pCtrl_Edit_Name);
}


BEGIN_MESSAGE_MAP(Dlg_Exist, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()




BOOL Dlg_Exist::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// 윈도우 초기화
	InitWindow();

	m_pCtrl_Edit_Name.MoveWindow(DLGEXIST_NAME_RECT);
	m_pCtrl_Edit_Path.MoveWindow(DLGEXIST_PATH_RECT);
	m_pCtrl_Text.MoveWindow(DLGEXIST_TEXT_RECT);

	CString strBuf = m_pInfo_FilePath;

	m_pInfo_FileName = strBuf.Mid(strBuf.ReverseFind('\\') + 1, MAX_PATH);
	m_pInfo_FilePath = strBuf.Mid(0, strBuf.ReverseFind('\\') + 1);
	
	m_pCtrl_Edit_Name.SetWindowText(m_pInfo_FileName);
	m_pCtrl_Edit_Path.SetWindowText(m_pInfo_FilePath);
	m_pCtrl_Text.SetWindowText(m_pInfo_Size);
	return TRUE;
}

void Dlg_Exist::InitWindow()
{
	Skin_LoadBG(IDB_BITMAP_BG_EXIST);
	Skin_TitleHeight(0);

	Skin_InsertButton(IDB_BITMAP_BTN_EXIST_KEEP,	"BTN_CONTINUE",	FALSE, DLGEXIST_KEEP_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_EXIST_OVERWRITE,"BTN_OVERWRITE",FALSE, DLGEXIST_OVERWRITE_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_EXIST_CANCEL,	"BTN_CANCEL",	FALSE, DLGEXIST_CANCEL_RECT);

	Skin_InsertButton(IDB_BITMAP_CHECK_EXIST_ALL,	"CHK_ALL_APPLY",TRUE,   DLGEXIST_ALL_RECT);
}


void Dlg_Exist::OnTimer(UINT nIDEvent) 
{
}

bool Dlg_Exist::Skin_ButtonPressed(CString m_ButtonName)
{
	if(m_ButtonName == "BTN_CONTINUE")
	{
		m_nResult = FILE_OVERWRITE_CONTINUE;
		EndDialog(IDOK);
		return false;
	}

	if(m_ButtonName == "BTN_OVERWRITE")
	{
		m_nResult = FILE_OVERWRITE_NEW;
		EndDialog(IDOK);
		return false;
	}

	if(m_ButtonName == "BTN_CANCEL")
	{
		m_nResult = FILE_OVERWRITE_CANCEL;
		EndDialog(IDOK);
		return false;
	}

	if(m_ButtonName == "CHK_ALL_APPLY")
	{
		m_nAllApply = Skin_GetCheck("CHK_ALL_APPLY");
		return false;
	}
	return true;
}

HBRUSH Dlg_Exist::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
    switch(nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			UINT nCtrlID = pWnd->GetDlgCtrlID();

			switch(nCtrlID)
			{				
				case IDC_STATIC_EXIST:
				{
					pDC->SetBkColor(DLGEXIST_BACK1_COLOR);
					pDC->SetTextColor(DLGEXIST_TEXT1_COLOR);
					return CreateSolidBrush(DLGEXIST_BACK1_COLOR);
				}
				case IDC_STATIC_EXIST2:
				case IDC_STATIC_EXIST3:
				{
					pDC->SetBkColor(DLGEXIST_BACK2_COLOR);
					pDC->SetTextColor(DLGEXIST_TEXT2_COLOR);
					return CreateSolidBrush(DLGEXIST_BACK2_COLOR);
				}
			}
			
			pDC->SetBkMode(TRANSPARENT); // 배경을 투명하게
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
    }
    return hbr;
}


BOOL Dlg_Exist::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
	    return TRUE;

	if(VK_F1 <= pMsg->wParam && VK_F10 >= pMsg->wParam)
		return true;

	switch(pMsg->message)
	{
		case WM_KEYDOWN:
			if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == 13 )    // ESC 처리
				return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}