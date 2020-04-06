#include "stdafx.h"
#include "Dlg_Confirm.h"
#include "config_client.h"


Dlg_Confirm::Dlg_Confirm(CWnd* pParent /*=NULL*/)
	: CSkinDialog(Dlg_Confirm::IDD, pParent)
{
	m_bCheck = FALSE;
}

Dlg_Confirm::~Dlg_Confirm()
{
}

void Dlg_Confirm::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CONFIRM, m_pCtrl_Text);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_pCtrl_Title);
}


BEGIN_MESSAGE_MAP(Dlg_Confirm, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()




BOOL Dlg_Confirm::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// 윈도우 초기화
	InitWindow();


	if(m_nCallMode == 1)
	{
		m_pBmp.LoadBitmap(IDB_BITMAP_CONFIRM_TITLE_1);	
		m_pInfo = "전송 프로그램을 종료 하시겠습니까?";
	}
	else if(m_nCallMode == 2)
	{
		m_pBmp.LoadBitmap(IDB_BITMAP_CONFIRM_TITLE_2);	
		m_pInfo = "이미 같은 아이디로 접속 중입니다\r\n이전 연결을 끊고 접속하려면 '예', 로그인을 취소하려면 '아니오'를 클릭해 주세요";
		SetTimer(1, 3000, NULL);
	}
	else if(m_nCallMode == 3)
	{
		m_pBmp.LoadBitmap(IDB_BITMAP_CONFIRM_TITLE_3);	
		m_pInfo = "동영상 재생을 위해 [곰플레이어]가 필요합니다\r\n설치하시겠습니까?";
	}
	else if(m_nCallMode == 4)
	{
		m_pBmp.LoadBitmap(IDB_BITMAP_CONFIRM_TITLE_4);		
		m_pInfo = "파일 전송중에는 종료할 수 없습니다\r\n" \
				  "'예'를 클릭해서 트레이 아이콘으로 보낼 수 있습니다";
	}
	
	m_pCtrl_Title.SetBitmap((HBITMAP)m_pBmp);
	m_pCtrl_Title.MoveWindow(DLGCONFIRM_TITLE_RECT);

	m_pCtrl_Text.SetWindowText(m_pInfo);
	m_pCtrl_Text.MoveWindow(DLGCONFIRM_TEXT_RECT);
	return TRUE;
}

void Dlg_Confirm::InitWindow()
{
	Skin_LoadBG(IDB_BITMAP_BG_CONFIRM);
	Skin_TitleHeight(0);

	Skin_InsertButton(IDB_BITMAP_BTN_CONFIRM_YES,	"BTN_YES",		FALSE, DLGCONFIRM_YES_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_CONFIRM_NO,	"BTN_NO",		FALSE, DLGCONFIRM_NO_RECT);

	if(m_nCallMode == 4)
		Skin_InsertButton(IDB_BITMAP_CHK_CLOSE_COOKIE,	"CHK_OK",		TRUE, DLGCONFIRM_CHK_OK_RECT);
}


void Dlg_Confirm::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		KillTimer(1);
		Skin_ButtonPressed("BTN_NO");
	}
}

bool Dlg_Confirm::Skin_ButtonPressed(CString m_ButtonName)
{
	//20071119 '확인'버튼 추가, jyh
	if(m_ButtonName == "BTN_YES")
	{
		EndDialog(IDOK);
		return false;
	}

	if(m_ButtonName == "BTN_NO")
	{
		EndDialog(IDCANCEL);
		return false;
	}

	if(m_ButtonName == "CHK_OK")
	{
		m_bCheck = Skin_GetCheck("CHK_OK");
		return false;
	}

	return true;
}

HBRUSH Dlg_Confirm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
    switch(nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			UINT nCtrlID = pWnd->GetDlgCtrlID();

			switch(nCtrlID)
			{				
				case IDC_STATIC_CONFIRM:
				{
					pDC->SetBkColor(DLGCONFIRM_BACK_COLOR);
					pDC->SetTextColor(DLGCONFIRM_TEXT_COLOR);
					return CreateSolidBrush(DLGCONFIRM_BACK_COLOR);
				}
			}
			
			pDC->SetBkMode(TRANSPARENT); // 배경을 투명하게
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
    }
    return hbr;
}