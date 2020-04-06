#include "stdafx.h"
#include "Dlg_Setup.h"
#include "registry.h"
#include "config_client.h"


Dlg_Setup::Dlg_Setup(CWnd* pParent /*=NULL*/)
	: CSkinDialog(Dlg_Setup::IDD, pParent)
{
	m_pMainWindow	= NULL;
}

Dlg_Setup::~Dlg_Setup()
{
}

void Dlg_Setup::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PATH, m_pCtrl_Path);
	/*DDX_Control(pDX, IDC_COMBO_SETUPT_1, m_pCombo_UpOver);
	DDX_Control(pDX, IDC_COMBO_SETUPT_2, m_pCombo_DownOver);
	DDX_Control(pDX, IDC_COMBO_SETUPT_3, m_pCombo_DownItemPath);*/

}


BEGIN_MESSAGE_MAP(Dlg_Setup, CSkinDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()





BOOL Dlg_Setup::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// 윈도우 초기화
	InitWindow();

	// 현재 설정 가져오기
	InitSetup();

	m_pCtrl_Path.MoveWindow(DLGSETUP_PATH_RECT);
	//20071031 마지막에 저장됐던 경로를 환경설정 창의 경로와 같게, jyh
	m_pCtrl_Path.SetWindowText(m_pMainWindow->m_rPathLast);

	/*m_pCombo_UpOver.MoveWindow(DLGSETUP_UP_RECT);
	m_pCombo_DownOver.MoveWindow(DLGSETUP_DOWN_RECT);
	m_pCombo_DownItemPath.MoveWindow(DLGSETUP_ITEMPATH_RECT);

	m_pCombo_UpOver.AddIcon(IDI_ICON_CHECK, "항상물어보기");
	m_pCombo_UpOver.AddIcon(IDI_ICON_CONTINUE, "이어서전송하기");
	m_pCombo_UpOver.AddIcon(IDI_ICON_OVERWRITE, "덮어쓰기");
	m_pCombo_UpOver.AddIcon(IDI_ICON_CANCEL, "취소하기");
	m_pCombo_UpOver.SetCurSel(m_pMainWindow->m_rUpOverWrite);
	m_pCombo_UpOver.SetTooltipText("선택하세요!");

	m_pCombo_DownOver.AddIcon(IDI_ICON_CHECK, "항상물어보기");
	m_pCombo_DownOver.AddIcon(IDI_ICON_CONTINUE, "이어서전송하기");
	m_pCombo_DownOver.AddIcon(IDI_ICON_OVERWRITE, "덮어쓰기");
	m_pCombo_DownOver.AddIcon(IDI_ICON_CANCEL, "취소하기");
	m_pCombo_DownOver.SetCurSel(m_pMainWindow->m_rDownOverWrite);
	m_pCombo_DownOver.SetTooltipText("선택하세요!");

	m_pCombo_DownItemPath.AddIcon(IDI_ICON_CHECK, "항상물어보기");
	m_pCombo_DownItemPath.AddIcon(IDI_ICON_LAST_PATH, "최근저장경로사용");
	m_pCombo_DownItemPath.AddIcon(IDI_ICON_BASIC_PATH, "기본저장경로사용");
	m_pCombo_DownItemPath.SetCurSel(m_pMainWindow->m_rDownAddItemPath);
	m_pCombo_DownItemPath.SetTooltipText("선택하세요!");*/
	return TRUE;
}

void Dlg_Setup::InitWindow()
{
	Skin_LoadBG(IDB_BITMAP_BG_SETUP);
	Skin_TitleHeight(0);

	Skin_InsertButton(IDB_BITMAP_BTN_SETUP_SERARCH,	"BTN_SEARCH",	FALSE,  DLGSETUP_SEARCH_RECT);
	/*Skin_InsertButton(IDB_BITMAP_BTN_SETUP_OK,		"BTN_OK",		FALSE,  DLGSETUP_OK_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_SETUP_CANCEL,	"BTN_CANCEL",	FALSE,  DLGSETUP_CANCEL_RECT);

	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_UP_CLOSE",	TRUE,   DLGSETUP_UPCLOSE_RECT);
	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_UP_TRANS",	TRUE,   DLGSETUP_UPTRANSE_RECT);
	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_DW_CLOSE",	TRUE,   DLGSETUP_DWCLOSE_RECT);
	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_DW_TRANS",	TRUE,   DLGSETUP_DWTRANS_RECT);*/
	//20070920 추가, jyh
	Skin_InsertButton(IDB_BITMAP_BTN_SETUP_APPLY,	"BTN_APPLY",	FALSE,  DLGSETUP_APPLY_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_SETUP_CLOSE,	"BTN_CLOSE",	FALSE,  DLGSETUP_CLOSE_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_SETUP_SERARCH,	"BTN_SEARCH",	FALSE,  DLGSETUP_SEARCH_RECT);
	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_DOWN_FOLDER",	TRUE,   DLGSETUP_DOWNFOLDER_RECT);
	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_SHORTCUT",	TRUE,   DLGSETUP_SHORTCUT_RECT);
	Skin_InsertButton(IDB_BITMAP_CHK_NORMAL,		"CHK_AUTODOWN",	TRUE,	DLGSETUP_AUTODOWN_RECT);
	Skin_InsertButton(IDB_BITMAP_BTN_EXIT,			"BTN_EXIT",		FALSE,	DLGSETUP_EXIT_RECT);
}


void Dlg_Setup::InitSetup()
{
	m_pMainWindow->RegRead();

	//20070920 추가, jyh
	if(m_pMainWindow->m_rDownAddItemPath == 1)
		Skin_SetCheck("CHK_DOWN_FOLDER", FALSE);
	else
		Skin_SetCheck("CHK_DOWN_FOLDER", TRUE);

	//20071002 바로가기, jyh
	if(m_pMainWindow->m_rCreateShortcut == 1)
		Skin_SetCheck("CHK_SHORTCUT", TRUE);
	else
		Skin_SetCheck("CHK_SHORTCUT", FALSE);

	//20071002 자동 내려받기, jyh
	if(m_pMainWindow->m_rDownAutoTrans == 1)
		Skin_SetCheck("CHK_AUTODOWN", TRUE);
	else
		Skin_SetCheck("CHK_AUTODOWN", FALSE);

	/*Skin_SetCheck("CHK_UP_CLOSE", m_pMainWindow->m_rUpEndAutoClose);
	Skin_SetCheck("CHK_UP_TRANS", m_pMainWindow->m_rUpAutoTrans);
	Skin_SetCheck("CHK_DW_CLOSE", m_pMainWindow->m_rDownEndAutoClose);
	Skin_SetCheck("CHK_DW_TRANS", m_pMainWindow->m_rDownAutoTrans);*/
}


bool Dlg_Setup::Skin_ButtonPressed(CString m_ButtonName)
{
	//20070920 jyh
	if(m_ButtonName == "BTN_EXIT" || m_ButtonName == "BTN_CLOSE")
	{
		EndDialog(IDCANCEL);
		return false;
	}
	/*if(m_ButtonName == "BTN_EXIT" ||m_ButtonName == "BTN_CANCEL")
	{
		EndDialog(IDCANCEL);
		return false;
	}*/
	
	else if(m_ButtonName == "BTN_SEARCH")
	{
		DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		CSelectFolderDialog pSelectDialog(FALSE, m_pMainWindow->m_rPathLast, wFlag, NULL, this, "기본으로 저장될 폴더를 선택해 주세요");
		if(pSelectDialog.DoModal() == IDOK)	
			m_pCtrl_Path.SetWindowText(pSelectDialog.GetSelectedPath());

		RECT rt = { 99, 48, 314, 48 };
		InvalidateRect(&rt);
	}

	//else if(m_ButtonName == "BTN_OK")
	else if(m_ButtonName == "BTN_APPLY")
	{
		//20070920 jyh
		if(Skin_GetCheck("CHK_DOWN_FOLDER"))
			m_pMainWindow->m_rDownAddItemPath = 0;	//항상 물어보기
		else
			m_pMainWindow->m_rDownAddItemPath = 1;	//최근 저장경로 사용

		//20071002 바로가기, jyh
		if(Skin_GetCheck("CHK_SHORTCUT"))
		{
			//CreateShortcut(URL_HOMEPAGE);
			m_pMainWindow->m_rCreateShortcut = 1;
		}
		else
			m_pMainWindow->m_rCreateShortcut = 0;

		//20071002 자동 내려받기, jyh
		if(Skin_GetCheck("CHK_AUTODOWN"))
			m_pMainWindow->m_rDownAutoTrans = 1;
		else
			m_pMainWindow->m_rDownAutoTrans = 0;

		/*m_pMainWindow->m_rDownAddItemPath	= m_pCombo_DownItemPath.GetCurSel();
		m_pMainWindow->m_rDownOverWrite		= m_pCombo_DownOver.GetCurSel();
		m_pMainWindow->m_rUpOverWrite		= m_pCombo_UpOver.GetCurSel();

		m_pMainWindow->m_rUpEndAutoClose	= Skin_GetCheck("CHK_UP_CLOSE");
		m_pMainWindow->m_rUpAutoTrans		= Skin_GetCheck("CHK_UP_TRANS");
		m_pMainWindow->m_rDownEndAutoClose	= Skin_GetCheck("CHK_DW_CLOSE");
		m_pMainWindow->m_rDownAutoTrans		= Skin_GetCheck("CHK_DW_TRANS");*/

		m_pCtrl_Path.GetWindowText(m_pMainWindow->m_rPathLast);
		m_pMainWindow->RegWrite();

		EndDialog(IDOK);
		return true;
	}

	/*else if(m_ButtonName == "CHK_UP_CLOSE")
		return true;
	else if(m_ButtonName == "CHK_UP_TRANS")
		return true;
	else if(m_ButtonName == "CHK_DW_CLOSE")
		return true;
	else if(m_ButtonName == "CHK_DW_TRANS")
		return true;*/

	return true;
}


HBRUSH Dlg_Setup::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
    switch(nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			UINT nCtrlID = pWnd->GetDlgCtrlID();

			/*switch(nCtrlID)
			{				
				case IDC_STATIC_PATH:
				{
					pDC->SetBkColor(DLGSETUP_BACK_COLOR);
					pDC->SetTextColor(DLGSETUP_TEXT_COLOR);
					return CreateSolidBrush(DLGSETUP_BACK_COLOR);
				}
			}*/
			
			pDC->SetBkMode(TRANSPARENT); // 배경을 투명하게
			pDC->SetTextColor(DLGSETUP_OPTION_TEXT_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
    }
    return hbr;
}

