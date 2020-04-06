#include "stdafx.h"
#include "use_window.h"
#include "Dlg_Setup.h"
#include "Dlg_Confirm.h"
#include ".\use_window.h"
#include "CBase64.h"

use_window::use_window(void)
{
}


use_window::~use_window(void)
{
}


BOOL use_window::OnInitDialog()
{
	m_hBrushCtrl_1	= CreateSolidBrush(RGB(255, 255, 255));
	up_window::OnInitDialog();

	return TRUE;
}


void use_window::InitWindow()
{
	//Skin_LoadBG(IDB_BITMAP_BG_MAIN, 450, 300);
	//Skin_LoadBG(IDB_BITMAP_BG_MAIN, 651, 458);
	Skin_LoadBG(IDB_BITMAP_BG_MAIN, 651, 600);
	Skin_TitleHeight(0);

	Skin_InsertButton(IDB_BITMAP_BTN_EXIT,			"BTN_EXIT",		FALSE);
	//Skin_InsertButton(IDB_BITMAP_BTN_MAX,			"BTN_MAX",		FALSE);
	//Skin_InsertButton(IDB_BITMAP_BTN_MAX_,			"BTN_MAX_",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_MINIMIZE,		"BTN_MINI",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_TRAY,			"BTN_TRAY",		FALSE);

	//Skin_InsertButton(IDB_BITMAP_BTN_REFILL,		"BTN_REFILL",	FALSE);

	//Skin_InsertButton(IDB_BITMAP_BTN_SELALL,		"BTN_SELALL",	FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_SEL_DEL,		"BTN_SELDEL",	FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_DONE_DEL,		"BTN_DONEDEL",	FALSE);

	//Skin_InsertButton(IDB_BITMAP_BTN_PATH,		"BTN_PATH",		FALSE,  CRect(322, 318, 322 + 92, 318 + 24));

	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_TOP,		"BTN_TOP",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_UP,		"BTN_UP",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_DOWN,	"BTN_DOWN",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_BOTTOM,	"BTN_BOTTOM",	FALSE);

	//Skin_InsertButton(IDB_BITMAP_BTN_SETUP,			"BTN_SETUP",	FALSE);

	//Skin_InsertButton(IDB_BITMAP_BTN_PLAY,			"BTN_PLAY",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_START,			"BTN_START",	FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_STOP,			"BTN_STOP",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_EXIT2,			"BTN_EXIT2",	FALSE);

	Skin_InsertButton(IDB_BITMAP_CHK_PROGRAM,		"CHK_PROGRAM",	TRUE);
	Skin_InsertButton(IDB_BITMAP_CHK_SYSTEM,		"CHK_SYSTEM",	TRUE);

	//Skin_SetButtonVisible("BTN_MAX_", FALSE);

	//m_pStatic_Time_Use.SetFontName("����");
	m_pStatic_Time_Over.SetFontName("sans-serif");
	//m_pStatic_FileCount.SetFontName("����");
	m_pStatic_Speed.SetFontName("sans-serif");	//20070919 ���� �ӵ� �ؽ�Ʈ ��Ʈ ����, jyh
	m_pStatic_FileName.SetFontBold(TRUE);		//20080909 ��Ʈ ����, jyh
	m_pStatic_FileName.SetFontName("sans_serif");//20080909 ��Ʈ ����, jyh

	// ���α׷����� �ʱ�ȭ
	/*m_pCtrl_ProgressTotal.SetStyle(PROGRESS_BITMAP);
	m_pCtrl_ProgressTotal.SetBitmap(IDB_BITMAP_PROGRESSBAR1, IDB_BITMAP_PROGRESSBAR2);
	m_pCtrl_ProgressTotal.SetRange(0, 100);	
	m_pCtrl_ProgressTotal.SetStep(1);
	m_pCtrl_ProgressTotal.SetPos(0);
	m_pCtrl_ProgressTotal.SetTextForeColor(RGB(0, 0, 0));
	m_pCtrl_ProgressTotal.SetTextBkColor(RGB(0, 0, 0));
	m_pCtrl_ProgressTotal.SetText("0%");*/
	m_pCtrl_ProgressTotal.SetStyle(PROGRESS_BITMAP);
	m_pCtrl_ProgressTotal.SetBitmap(IDB_BITMAP_PROGRESSBAR1, IDB_BITMAP_PROGRESSBAR2);
	m_pCtrl_ProgressTotal.SetRange(0, 100);	
	m_pCtrl_ProgressTotal.SetStep(1);
	m_pCtrl_ProgressTotal.SetPos(0);
	m_pCtrl_ProgressTotal.SetTextForeColor(RGB(204, 0, 0));
	m_pCtrl_ProgressTotal.SetTextBkColor(RGB(255, 255, 255));
	m_pCtrl_ProgressTotal.SetText("0B / 0B (0%)");

	//20070918 ���� ���� ���α׷����� �ʱ�ȭ, jyh
	m_pCtrl_ProgressCur.SetStyle(PROGRESS_BITMAP);
	m_pCtrl_ProgressCur.SetBitmap(IDB_BITMAP_PROGRESSBAR1, IDB_BITMAP_PROGRESSBAR2);
	m_pCtrl_ProgressCur.SetRange(0, 100);	
	m_pCtrl_ProgressCur.SetStep(1);
	m_pCtrl_ProgressCur.SetPos(0);
	m_pCtrl_ProgressCur.SetTextForeColor(RGB(204, 0, 0));
	m_pCtrl_ProgressCur.SetTextBkColor(RGB(255, 255, 255));
	m_pCtrl_ProgressCur.SetText("0B / 0B (0%)");

	// �ؽ�Ʈ ����
	//m_pStatic_Time_Use.SetWindowText("00:00:00");
	m_pStatic_Time_Over.SetWindowText("00:00:00");
	//m_pStatic_FileCount.SetWindowText("0 (0MB / 0MB)");
	m_pStatic_Speed.SetWindowText("0B/sec");	//20070919 ���ۼӵ�, jyh

	m_bInitWindow	= true;

	//20090312 ����, jyh-------------------------------------------------------------
	// ����Ʈ ��Ʈ�� �缳��
	//m_pListCtrl.MoveWindow(10, 205, cx - 10 - 11 , cy - 205 - 79);
	m_pListCtrl.MoveWindow(12, 145, 626 , 162);

	// ��ư ��Ʈ�� �缳��
	/*Skin_MoveCtrl("BTN_EXIT",		cx - 37, 14, 21, 19);
	Skin_MoveCtrl("BTN_MAX",		cx - 62, 14, 21, 19);
	Skin_MoveCtrl("BTN_MAX_",		cx - 62, 14, 21, 19);
	Skin_MoveCtrl("BTN_MINI",		cx - 87, 14, 21, 19);
	Skin_MoveCtrl("BTN_TRAY",		cx - 113, 14, 21, 19);*/
	Skin_MoveCtrl("BTN_MINI",		579, 11, 18, 17);
	Skin_MoveCtrl("BTN_TRAY",		599, 11, 18, 17);
	Skin_MoveCtrl("BTN_EXIT",		619, 11, 18, 17);

	//Skin_MoveCtrl("BTN_REFILL",		cx - 156, 58, 136, 47);

	//Skin_MoveCtrl("BTN_SELALL",		18, cy - 77, 94, 22);
	//Skin_MoveCtrl("BTN_SELDEL",		10, cy - 67, 57, 21);
	//Skin_MoveCtrl("BTN_DONEDEL",	10, cy - 67, 94, 21);
	//Skin_MoveCtrl("BTN_PATH",		312, cy - 77, 94, 22);
	Skin_MoveCtrl("BTN_SELDEL",		112, 318, 86, 20);
	Skin_MoveCtrl("BTN_DONEDEL",	202, 318, 86, 20);

	/*Skin_MoveCtrl("BTN_TOP",		103, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_UP",			130, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_DOWN",		157, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_BOTTOM",		184, cy - 67, 23, 21);*/
	Skin_MoveCtrl("BTN_TOP",		16, 318, 24, 24);
	Skin_MoveCtrl("BTN_UP",			40, 318, 24, 24);
	Skin_MoveCtrl("BTN_DOWN",		64, 318, 24, 24);
	Skin_MoveCtrl("BTN_BOTTOM",		88, 318, 24, 24);

	//Skin_MoveCtrl("BTN_SETUP",		10, cy - 39, 81, 21);

	//Skin_MoveCtrl("BTN_PLAY",		cx - 241, cy - 67, 61, 39);
	/*Skin_MoveCtrl("BTN_START",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_STOP",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_EXIT2",		cx - 71, cy - 67, 61, 39);*/
	Skin_MoveCtrl("BTN_START",		477, 323, 86, 43);
	Skin_MoveCtrl("BTN_STOP",		477, 323, 86, 43);
	Skin_MoveCtrl("BTN_EXIT2",		568, 323, 69, 43);

	/*Skin_MoveCtrl("CHK_PROGRAM",	95, cy - 34, 50, 13); 
	Skin_MoveCtrl("CHK_SYSTEM",		150, cy - 34, 75, 13);*/
	Skin_MoveCtrl("CHK_PROGRAM",	204, 349, 83, 11); 
	Skin_MoveCtrl("CHK_SYSTEM",		306, 349, 72, 11);

	//20070918 ���� ���� ���α׷����� �缳��, jyh
	m_pCtrl_ProgressCur.MoveWindow(87, 79, 540, 16);
	//��ü ���� ���α׷����� �缳��
	//m_pCtrl_ProgressTotal.MoveWindow(86, 167, cx - 86 - 17, 17);
	m_pCtrl_ProgressTotal.MoveWindow(87, 105, 540, 16);

	// �ؽ�Ʈ ��� ��ġ �缳��
	m_pStatic_Time_Over.MoveWindow(579, 54, 50, 12);
	//m_pStatic_Time_Use.MoveWindow(85, 146, 70, 11);
	//m_pStatic_Time_Over.MoveWindow(227, 146, 70, 11);
	//m_pStatic_FileCount.MoveWindow(346, 146, 196, 11);
	m_pStatic_Speed.MoveWindow(423, 54, 67, 12);	//20070919 ���� �ӵ� ��� ��ġ, jyh
	m_pStatic_FileName.MoveWindow(27, 53, 350, 12);		//20070919 ���� ���� ��� ��ġ, jyh

	// ��� ��Ʈ��
	if(!m_pBanner)
	{
		m_pBanner = new HtmlViewer;
		m_pBanner->Create(NULL, _T("BANNER"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 0xFFFF);
	}

	//20071004 �����ذ� ����, jyh
	if(!m_pBtnHelp)
	{
		m_pBtnHelp = new HtmlViewer;
		m_pBtnHelp->Create(NULL, _T("BTNHELP"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}

	if(!m_pSendComplete)
	{
		m_pSendComplete = new HtmlViewer;
		m_pSendComplete->Create(NULL, _T("SENDCOMPLETE"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}




////////// - kth ������ ���ۿ� ������- S
	if(!m_pSendKTHComplete1)
	{
		m_pSendKTHComplete1 = new HtmlViewer;
		m_pSendKTHComplete1->Create(NULL, _T("SENDKTHCOMPLETE1"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete2)
	{
		m_pSendKTHComplete2 = new HtmlViewer;
		m_pSendKTHComplete2->Create(NULL, _T("SENDKTHCOMPLETE2"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete3)
	{
		m_pSendKTHComplete3 = new HtmlViewer;
		m_pSendKTHComplete3->Create(NULL, _T("SENDKTHCOMPLETE3"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete4)
	{
		m_pSendKTHComplete4 = new HtmlViewer;
		m_pSendKTHComplete4->Create(NULL, _T("SENDKTHCOMPLETE4"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete5)
	{
		m_pSendKTHComplete5 = new HtmlViewer;
		m_pSendKTHComplete5->Create(NULL, _T("SENDKTHCOMPLETE5"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
////////// - kth ������ ���ۿ� ������- E






	//20070918 ��� ��ġ, jyh
	//m_pBanner->MoveWindow(9, 46, cx - 9 - 9, 80);
	//m_pBanner->MoveWindow(12, 385, 625, 59);
	m_pBanner->MoveWindow(12, 385, 625, 169);
	m_pBanner->Navigate(CString(URL_BANNER_UP) + "upload");
	m_pBanner->UpdateData();

	//20071004 �����ذ� ���� ��ư ��ġ, jyh
	m_pBtnHelp->MoveWindow(16, 343, 92, 20);

	//20071108 ����μ����� ����ID �޾ƿ���, jyh
	CString strTemp = AfxGetApp()->m_lpCmdLine;
	CString strUserId;
	CString strUserPw;
	CString strCombine;
	int nPos = 0;

	if(strTemp == "")
	{
		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���α׷��� �������� ������� ������� �ʾҽ��ϴ�" , 1);
		Exit_Program(false, false);
		return;
	}

	strUserId = strTemp.Tokenize(" ", nPos);
	strUserPw = strTemp.Tokenize(" ", nPos);
	strCombine.Format("%s|%s", strUserId, strUserPw);

	CBase64 base64;
	char* szEncodedText;

	szEncodedText = base64.base64_encode(strCombine.GetBuffer(), strCombine.GetLength());
	strCombine.ReleaseBuffer();

	strTemp.Format("%s%s", URL_HELP, szEncodedText);
	m_pBtnHelp->Navigate(strTemp);
	//m_pBtnHelp->Navigate("http://mfile.co.kr/mmsv/help.php");
	m_pBtnHelp->UpdateData();
	//------------------------------------------------------------------------------------------

	CRect rc;
	GetClientRect(rc);
	SendMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
}

void use_window::InitListControl()
{
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
    m_pImageList.Attach((HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("*.txt"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
	m_pStateImageList.Create(1, 18, ILC_COLORDDB, 1, 0);			//20080331 ����Ʈ ��Ʈ���� row ���̸� �ø��� ����, jyh
	m_pListCtrl.SetImageList(&m_pImageList, LVSIL_SMALL);
	m_pListCtrl.SetImageList(&m_pStateImageList, LVSIL_STATE);		//20080331 ����Ʈ ��Ʈ���� row ���̸� �ø��� ����, jyh
	m_pImageList.Detach();	

	m_pListCtrl.SetHeaderColors(RGB(238, 238, 238), RGB(0, 0, 0));	//20070917 ����Ʈ ��� �� ����, jyh
	m_pListCtrl.SetColumnCount(3);
	m_pListCtrl.SetCallbackMask(LVIS_FOCUSED);
	//20070917 LVS_EX_GRIDLINES ��Ÿ�� �߰�, jyh
    m_pListCtrl.SetExtendedStyle(m_pListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB/* | LVS_EX_GRIDLINES*/);

	m_pListCtrl.SetListColor(	RGB(0, 0, 0), RGB(255, 255, 255),			// Ȧ�������� �ؽ�Ʈ/���
								RGB(0, 0, 0),	RGB(255, 255, 255),			// ¦�������� �ؽ�Ʈ/���

								RGB(204, 64, 0), RGB(255, 255, 255),	 	// ���ö����� �ؽ�Ʈ/���
								RGB(255, 255, 255),							// ���ۻ����� �ܰ�����
								RGB(0, 0, 0), RGB(255, 255, 255),			// ���۵��� ���� �κ��� �ؽ�Ʈ/���
								RGB(0, 0, 0), RGB(255, 255, 255)		// ���۵� �κ��� �ؽ�Ʈ/���
							);
	//20080303 �Ϸ� ��� �� ����, jyh
	m_pListCtrl.SetCompleteTextColor(RGB(98, 98, 98));

	//20070917 Į�� ����, jyh
	m_pListCtrl.InsertColumn(1, "�����̸�", LVCFMT_CENTER, 378, -1);
	m_pListCtrl.InsertColumn(2, "ũ��", LVCFMT_RIGHT, 140, -1);
	m_pListCtrl.InsertColumn(3, "����", LVCFMT_CENTER, 104, -1);
	
	//m_pListCtrl.SetListColor(	RGB(0, 0, 0), RGB(255, 255, 255),			// Ȧ�������� �ؽ�Ʈ/���
	//							RGB(0, 0, 0),	RGB(255, 255, 255),			// ¦�������� �ؽ�Ʈ/���
	//							
	//							RGB(247, 93, 0), RGB(255, 255, 255),	 	// ���ö����� �ؽ�Ʈ/���
	//							RGB(18, 128, 215),							// ���ۻ����� �ܰ�����
	//							RGB(0, 0, 0), RGB(177, 224, 252),			// ���۵��� ���� �κ��� �ؽ�Ʈ/���
	//							RGB(255, 255, 255), RGB(16, 158, 234)		// ���۵� �κ��� �ؽ�Ʈ/���
	//						);

	//m_pListCtrl.InsertColumn(0, "�����̸�", LVCFMT_LEFT, 170, -1);
	//m_pListCtrl.InsertColumn(1, "ũ��", LVCFMT_RIGHT, 100, -1);
	//m_pListCtrl.InsertColumn(2, "�ӵ�", LVCFMT_RIGHT, 0, -1);
	//m_pListCtrl.InsertColumn(3, "����", LVCFMT_CENTER, 80, -1);
	//m_pListCtrl.InsertColumn(4, "�����ð�", LVCFMT_CENTER, 80, -1);
	//m_pListCtrl.InsertColumn(5, "������ġ", LVCFMT_LEFT, 90, -1);		
}


void use_window::ChnPosition(int cx, int cy) 
{
	// ����Ʈ ��Ʈ�� �缳��
	//m_pListCtrl.MoveWindow(10, 205, cx - 10 - 11 , cy - 205 - 79);
	m_pListCtrl.MoveWindow(12, 145, 626 , 162);

	// ��ư ��Ʈ�� �缳��
	/*Skin_MoveCtrl("BTN_EXIT",		cx - 37, 14, 21, 19);
	Skin_MoveCtrl("BTN_MAX",		cx - 62, 14, 21, 19);
	Skin_MoveCtrl("BTN_MAX_",		cx - 62, 14, 21, 19);
	Skin_MoveCtrl("BTN_MINI",		cx - 87, 14, 21, 19);
	Skin_MoveCtrl("BTN_TRAY",		cx - 113, 14, 21, 19);*/
	Skin_MoveCtrl("BTN_MINI",		579, 11, 18, 17);
	Skin_MoveCtrl("BTN_TRAY",		599, 11, 18, 17);
	Skin_MoveCtrl("BTN_EXIT",		619, 11, 18, 17);
	
	//Skin_MoveCtrl("BTN_REFILL",		cx - 156, 58, 136, 47);

	//Skin_MoveCtrl("BTN_SELALL",		18, cy - 77, 94, 22);
	//Skin_MoveCtrl("BTN_SELDEL",		10, cy - 67, 57, 21);
	//Skin_MoveCtrl("BTN_DONEDEL",	10, cy - 67, 94, 21);
	//Skin_MoveCtrl("BTN_PATH",		312, cy - 77, 94, 22);
	Skin_MoveCtrl("BTN_SELDEL",		112, 318, 86, 20);
	Skin_MoveCtrl("BTN_DONEDEL",	202, 318, 86, 20);

	/*Skin_MoveCtrl("BTN_TOP",		103, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_UP",			130, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_DOWN",		157, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_BOTTOM",		184, cy - 67, 23, 21);*/
	Skin_MoveCtrl("BTN_TOP",		16, 318, 24, 24);
	Skin_MoveCtrl("BTN_UP",			40, 318, 24, 24);
	Skin_MoveCtrl("BTN_DOWN",		64, 318, 24, 24);
	Skin_MoveCtrl("BTN_BOTTOM",		88, 318, 24, 24);

	//Skin_MoveCtrl("BTN_SETUP",		10, cy - 39, 81, 21);

	//Skin_MoveCtrl("BTN_PLAY",		cx - 241, cy - 67, 61, 39);
	/*Skin_MoveCtrl("BTN_START",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_STOP",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_EXIT2",		cx - 71, cy - 67, 61, 39);*/
	Skin_MoveCtrl("BTN_START",		477, 323, 86, 43);
	Skin_MoveCtrl("BTN_STOP",		477, 323, 86, 43);
	Skin_MoveCtrl("BTN_EXIT2",		568, 323, 69, 43);

	/*Skin_MoveCtrl("CHK_PROGRAM",	95, cy - 34, 50, 13); 
	Skin_MoveCtrl("CHK_SYSTEM",		150, cy - 34, 75, 13);*/
	Skin_MoveCtrl("CHK_PROGRAM",	204, 349, 83, 11); 
	Skin_MoveCtrl("CHK_SYSTEM",		306, 349, 72, 11);

	//20070918 ���� ���� ���α׷����� �缳��, jyh
	m_pCtrl_ProgressCur.MoveWindow(87, 79, 540, 16);
	//��ü ���� ���α׷����� �缳��
	//m_pCtrl_ProgressTotal.MoveWindow(86, 167, cx - 86 - 17, 17);
	m_pCtrl_ProgressTotal.MoveWindow(87, 105, 540, 16);
	
	// �ؽ�Ʈ ��� ��ġ �缳��
	m_pStatic_Time_Over.MoveWindow(579, 54, 50, 12);
	//m_pStatic_Time_Use.MoveWindow(85, 146, 70, 11);
	//m_pStatic_Time_Over.MoveWindow(227, 146, 70, 11);
	//m_pStatic_FileCount.MoveWindow(346, 146, 196, 11);
	m_pStatic_Speed.MoveWindow(423, 54, 67, 12);	//20070919 ���� �ӵ� ��� ��ġ, jyh
	m_pStatic_FileName.MoveWindow(27, 53, 350, 12);		//20070919 ���� ���� ��� ��ġ, jyh

	// ��� ��Ʈ��
	if(!m_pBanner)
	{
		m_pBanner = new HtmlViewer;
		m_pBanner->Create(NULL, _T("BANNER"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 0xFFFF);
	}

	//20071004 �����ذ� ����, jyh
	if(!m_pBtnHelp)
	{
		m_pBtnHelp = new HtmlViewer;
		m_pBtnHelp->Create(NULL, _T("BTNHELP"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}

	if(!m_pSendComplete)
	{
		m_pSendComplete = new HtmlViewer;
		m_pSendComplete->Create(NULL, _T("SENDCOMPLETE"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}




////////// - kth ������ ���ۿ� ������- S
	if(!m_pSendKTHComplete1)
	{
		m_pSendKTHComplete1 = new HtmlViewer;
		m_pSendKTHComplete1->Create(NULL, _T("SENDKTHCOMPLETE1"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete2)
	{
		m_pSendKTHComplete2 = new HtmlViewer;
		m_pSendKTHComplete2->Create(NULL, _T("SENDKTHCOMPLETE2"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete3)
	{
		m_pSendKTHComplete3 = new HtmlViewer;
		m_pSendKTHComplete3->Create(NULL, _T("SENDKTHCOMPLETE3"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete4)
	{
		m_pSendKTHComplete4 = new HtmlViewer;
		m_pSendKTHComplete4->Create(NULL, _T("SENDKTHCOMPLETE4"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendKTHComplete5)
	{
		m_pSendKTHComplete5 = new HtmlViewer;
		m_pSendKTHComplete5->Create(NULL, _T("SENDKTHCOMPLETE5"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
////////// - kth ������ ���ۿ� ������- E








	//20070918 ��� ��ġ, jyh
	//m_pBanner->MoveWindow(9, 46, cx - 9 - 9, 80);
	m_pBanner->MoveWindow(12, 385, 625, 59);
	m_pBanner->Navigate(CString(URL_BANNER_UP) + "upload");
	m_pBanner->UpdateData();

	//20071004 �����ذ� ���� ��ư ��ġ, jyh
	m_pBtnHelp->MoveWindow(16, 343, 92, 20);
	
	//20071108 ����μ����� ����ID �޾ƿ���, jyh
	CString strTemp = AfxGetApp()->m_lpCmdLine;
	CString strUserId;
	CString strUserPw;
	CString strCombine;
	int nPos = 0;
	
	if(strTemp == "")
	{
		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���α׷��� �������� ������� ������� �ʾҽ��ϴ�" , 1);
		Exit_Program(false, false);
		return;
	}

	strUserId = strTemp.Tokenize(" ", nPos);
	strUserPw = strTemp.Tokenize(" ", nPos);
	strCombine.Format("%s|%s", strUserId, strUserPw);

	CBase64 base64;
	char* szEncodedText;

	szEncodedText = base64.base64_encode(strCombine.GetBuffer(), strCombine.GetLength());
	strCombine.ReleaseBuffer();

	strTemp.Format("%s%s", URL_HELP, szEncodedText);
	m_pBtnHelp->Navigate(strTemp);
	//m_pBtnHelp->Navigate("http://mfile.co.kr/mmsv/help.php");
	m_pBtnHelp->UpdateData();

	//// ����Ʈ ��Ʈ�� �÷� ����
	//if(m_bMaxWindow)
	//{
	//	m_pListCtrl.SetColumnWidth(0, 200);
	//	m_pListCtrl.SetColumnWidth(5, m_nMaxWidth - 200 - 120 - 76 - 67 - 80 - 60);
	//}
	//else
	//{
	//	m_pListCtrl.SetColumnWidth(0, 150);
	//	m_pListCtrl.SetColumnWidth(5, 180);
	//}
}


HBRUSH use_window::DrawCtrl(CDC* pDC, UINT nCtrlID)
{
	switch(nCtrlID)
	{				
		case IDC_STATIC_TIME_USE:
		case IDC_STATIC_TIME_OVER:
		case IDC_STATIC_COUNT:
		{
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextColor(RGB(0, 0, 0));
			return m_hBrushCtrl_1;
		}
	}

	return NULL;
}

bool use_window::Skin_ButtonPressed(CString m_ButtonName)
{
	if(m_ButtonName == "BTN_EXIT" || m_ButtonName == "BTN_EXIT2")
	{
		if(!m_bFileTrans)
		{
			Dlg_Confirm pWindow;
			pWindow.m_nCallMode = 1;
 			if(pWindow.DoModal() == IDOK)
			{
				Exit_Program(true, true);
				return true;
			}
			else
				return true;
		}
		else
			Exit_Program(true, false);
		return true;
	}
	
	else if(m_ButtonName == "BTN_MAX")
	{
		ShowMaxWindow();
		Skin_SetButtonVisible("BTN_MAX", FALSE);
		Skin_SetButtonVisible("BTN_MAX_", TRUE);
	}

	else if(m_ButtonName == "BTN_MAX_")
	{
		ShowNormalWindow();
		Skin_SetButtonVisible("BTN_MAX", TRUE);
		Skin_SetButtonVisible("BTN_MAX_", FALSE);
	}

	else if(m_ButtonName == "BTN_MINI")
	{
		m_bMinWindow = TRUE;		//20080131 jyh
		::ShowWindow(this->m_hWnd, SW_SHOWMINIMIZED);
	}

	else if(m_ButtonName == "BTN_TRAY")
	{
		::SetTimer(this->m_hWnd, 777, 700, NULL);	//20080226 Ʈ���� ������ Ÿ�̸� ����, jyh
		m_pTray->TrayBegin();
	}

	else if(m_ButtonName == "BTN_SELALL")
	{
		if((m_pListCtrl.GetNextItem(-1,LVNI_SELECTED)) == -1)		
		{
			for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount() ; nIndex++)
				m_pListCtrl.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
		else
		{
			for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount() ; nIndex++)
				m_pListCtrl.SetItemState(nIndex, 0, -1);
		}
		
		return true;
	}

	//20080228 �����߿��� ���ø�� ���� �ǰ�, jyh
	else if(m_ButtonName == "BTN_SELDEL")// && !m_bFileTrans)
	{
		DeleteList(1);
		return true;
	}

	//20071002 �����߿��� �Ϸ��� ���� �ǰ�, jyh
	else if(m_ButtonName == "BTN_DONEDEL")// && !m_bFileTrans)
	{
		DeleteList(2);
		return true;
	}

	else if(m_ButtonName == "BTN_TOP" && !m_bFileTrans)
	{
		MoveItem(MOVE_MF);
		return true;
	}

	//20080228 �����߿��� ���� �̵� �ǰ�, jyh
	else if(m_ButtonName == "BTN_UP")// && !m_bFileTrans)
	{
		MoveItem(MOVE_MU);
		return true;
	}

	//20080228 �����߿��� ���� �̵� �ǰ�, jyh
	else if(m_ButtonName == "BTN_DOWN")// && !m_bFileTrans)
	{
		MoveItem(MOVE_MD);
		return true;
	}

	//20080228 �����߿��� ���� �̵� �ǰ�, jyh
	else if(m_ButtonName == "BTN_BOTTOM")// && !m_bFileTrans)
	{
		MoveItem(MOVE_ML);
		return true;
	}

	else if(m_ButtonName == "BTN_SETUP")
	{
		Dlg_Setup dlg_setup;
		dlg_setup.m_pMainWindow = this;
		dlg_setup.DoModal();
		return true;
	}

	else if(m_ButtonName == "BTN_START")
	{		
		//if((GetTickCount() - m_nTimeTransButton) > 2000)
		if((GetTickCount() - m_nTimeTransButton) > 10)	//20071002 ���ε� ��ư �ν� �ð� ����, jyh
		{
			m_nTimeTransButton = GetTickCount();

			PRUPCTX pPtrCheck;
			int nTItemCount = 0;

			for(int nProcessIndex = 0 ; nProcessIndex < m_pListCtrl.GetItemCount(); nProcessIndex++)
			{	
				pPtrCheck = (PRUPCTX)m_pListCtrl.GetItemData(nProcessIndex);

				//20080228 ����� �ʰ��� ���ε����� ���� ��õ��� ���ε� �ǰ� 
				//ITEM_STATE_DEFER, ITEM_STATE_CONNECT, ITEM_STATE_DELAY, ITEM_STATE_CONFAILED �߰�, jyh
				if(pPtrCheck != NULL && 
					(pPtrCheck->nTransState == ITEM_STATE_WAIT || pPtrCheck->nTransState == ITEM_STATE_TRANS ||
					pPtrCheck->nTransState == ITEM_STATE_STOP || pPtrCheck->nTransState == ITEM_STATE_CONNECT))
					nTItemCount++;
			}

			if(nTItemCount == 0)
			{
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"���� ������ ������ �������� �ʽ��ϴ�!", 1);
				return true;
			}

			//20071211 ���� ��������..
			if(m_bTransGura)
			{				
				if(m_pWorker_Ctrl->m_bConnectServer)
					SetTransFlagGura(true);
				else
					SetTimer(998, 500, NULL);
			}
			else
			{
				if(m_pWorker_Ctrl->m_bConnectServer)
					SetTransFlag(true);
				else
					SetTimer(99, 1000, NULL);
			}

			return true;
		}
	}

	else if(m_ButtonName == "BTN_STOP")
	{
		//if((GetTickCount() - m_nTimeTransButton) > 2000)
		if((GetTickCount() - m_nTimeTransButton) > 10)	//20071002 �Ͻ����� ��ư �ν� �ð� ����, jyh
		{
			m_iStopItemIdx = GetTransListIndex();		//20071030 ���� ������ ������ �ε��� ����, jyh
			m_nTimeTransButton = GetTickCount();

			//20070918 ���� ���� ������ ����, jyh
			if(m_pPtrTrans)
			{
				m_pPtrTrans->nTransState = ITEM_STATE_STOP;
				m_pListCtrl.SetItemText(m_iStopItemIdx, 2, GetFileStatus(ITEM_STATE_STOP));	
			}

			//20071211 ���� ��������..
			if(m_bTransGura)
			{
				KillTimer(999);
				SetTransFlagGura(false);
				return true;
			}

			SetTransFlag(false);
		}
	}

	else if(m_ButtonName == "BTN_REFILL")
	{
	}	


	else if(m_ButtonName == "CHK_PROGRAM")
	{
		m_rUpEndAutoClose = Skin_GetCheck("CHK_PROGRAM");
		m_pReg.SaveKey(CLIENT_REG_PATH, "rUpEndAutoClose",	m_rUpEndAutoClose);
		return true;
	}

	else if(m_ButtonName == "CHK_SYSTEM")
	{
		return true;
	}

	return true;
}

BEGIN_MESSAGE_MAP(use_window, up_window)
ON_WM_QUERYOPEN()
END_MESSAGE_MAP()

//20080131 �ּ�ȭ �� â�� ���� ũ��� �����ɶ� �߻��ϴ� �޼���, jyh
BOOL use_window::OnQueryOpen()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_bMinWindow = FALSE;
	KillTimer(777);			//20080226 jyh

	return up_window::OnQueryOpen();
}
