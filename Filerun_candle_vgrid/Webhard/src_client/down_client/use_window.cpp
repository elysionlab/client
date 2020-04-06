#include "stdafx.h"
#include "use_window.h"
#include "Dlg_Setup.h"
#include "Dlg_Confirm.h"
#include ".\use_window.h"
//20080313 추가, jyh
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
	down_window::OnInitDialog();

	return TRUE;
}


void use_window::InitWindow()
{
	//Skin_LoadBG(IDB_BITMAP_BG_MAIN, 450, 300);
	//Skin_LoadBG(IDB_BITMAP_BG_MAIN, 651, 481);
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

	//Skin_InsertButton(IDB_BITMAP_BTN_PATH,			"BTN_PATH",		FALSE);

	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_TOP,		"BTN_TOP",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_UP,		"BTN_UP",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_DOWN,	"BTN_DOWN",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_ARROW_BOTTOM,	"BTN_BOTTOM",	FALSE);

	Skin_InsertButton(IDB_BITMAP_BTN_SETUP,			"BTN_SETUP",	FALSE);
	
	Skin_InsertButton(IDB_BITMAP_BTN_PLAY,			"BTN_PLAY",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_START,			"BTN_START",	FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_STOP,			"BTN_STOP",		FALSE);
	Skin_InsertButton(IDB_BITMAP_BTN_EXIT2,			"BTN_EXIT2",	FALSE);

	Skin_InsertButton(IDB_BITMAP_CHK_PROGRAM,		"CHK_PROGRAM",	TRUE);
	Skin_InsertButton(IDB_BITMAP_CHK_SYSTEM,		"CHK_SYSTEM",	TRUE);

	//20070919 버튼 추가, jyh
	Skin_InsertButton(IDB_BITMAP_BTN_FOLDER_OPEN,	"BTN_FDOPEN",	FALSE);	//20070919 저장 폴더 열기, jyh
    //Skin_InsertButton(IDB_BITMAP_BTN_QUICK_DOWN,	"BTN_QUICKDOWN", FALSE);	//20080313 퀵 다운로드, jyh

	//Skin_SetButtonVisible("BTN_MAX_", FALSE);

	//m_pStatic_Time_Use.SetFontName("굴림");
	m_pStatic_Time_Over.SetFontName("sans-serif");
	//m_pStatic_FileCount.SetFontName("굴림");
	m_pStatic_Speed.SetFontName("sans-serif");	//20070919 전송 속도 텍스트 폰트 설정, jyh
	m_pStatic_FileName.SetFontBold(TRUE);		//20080909 폰트 굵게, jyh
	m_pStatic_FileName.SetFontName("sans_serif");//20080909 폰트 수정, jyh

	//// 프로그래스바 초기화
	//m_pCtrl_ProgressTotal.SetStyle(PROGRESS_BITMAP);
	//m_pCtrl_ProgressTotal.SetBitmap(IDB_BITMAP_PROGRESSBAR1, IDB_BITMAP_PROGRESSBAR2);
	//m_pCtrl_ProgressTotal.SetRange(0, 100);	
	//m_pCtrl_ProgressTotal.SetStep(1);
	//m_pCtrl_ProgressTotal.SetPos(0);
	//m_pCtrl_ProgressTotal.SetTextForeColor(RGB(0, 0, 0));
	//m_pCtrl_ProgressTotal.SetTextBkColor(RGB(0, 0, 0));
	//m_pCtrl_ProgressTotal.SetText("0%");

	// 전체 파일 프로그래스바 초기화
	m_pCtrl_ProgressTotal.SetStyle(PROGRESS_BITMAP);
	m_pCtrl_ProgressTotal.SetBitmap(IDB_BITMAP_PROGRESSBAR1, IDB_BITMAP_PROGRESSBAR2);
//	m_pCtrl_ProgressTotal.SetRange(0, 1000000);	
	m_pCtrl_ProgressTotal.SetRange(0, 100);	
	m_pCtrl_ProgressTotal.SetStep(1);
	m_pCtrl_ProgressTotal.SetPos(0);
	m_pCtrl_ProgressTotal.SetTextForeColor(RGB(204, 0, 0));
	m_pCtrl_ProgressTotal.SetTextBkColor(RGB(255, 255, 255));
	m_pCtrl_ProgressTotal.SetText("0B / 0B (0%)");

	//20070918 현재 파일 프로그레스바 초기화, jyh
	m_pCtrl_ProgressCur.SetStyle(PROGRESS_BITMAP);
	m_pCtrl_ProgressCur.SetBitmap(IDB_BITMAP_PROGRESSBAR1, IDB_BITMAP_PROGRESSBAR2);
//	m_pCtrl_ProgressCur.SetRange(0, 1000000);	
	m_pCtrl_ProgressCur.SetRange(0, 100);	
	m_pCtrl_ProgressCur.SetStep(1);
	m_pCtrl_ProgressCur.SetPos(0);
	m_pCtrl_ProgressCur.SetTextForeColor(RGB(204, 0, 0));
	m_pCtrl_ProgressCur.SetTextBkColor(RGB(255, 255, 255));
	m_pCtrl_ProgressCur.SetText("0B / 0B (0%)");

	// 텍스트 설정
	//m_pStatic_Time_Use.SetWindowText("00:00:00");
	m_pStatic_Time_Over.SetWindowText("00:00:00");
	//m_pStatic_FileCount.SetWindowText("0 (0MB / 0MB)");
	m_pStatic_Speed.SetWindowText("0B/sec");	//20070919 전송속도, jyh

	m_bInitWindow	= true;

	//20090312 수정, jyh---------------------------------------------------------------
	// 리스트 컨트롤 재설정
	//m_pListCtrl.MoveWindow(10, 205, cx - 10 - 11 , cy - 205 - 79);
	m_pListCtrl.MoveWindow(12, 169, 626 , 162);

	// 버튼 컨트롤 재설정
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
	//Skin_MoveCtrl("BTN_DONEDEL",	74, cy - 67, 94, 21);
	//Skin_MoveCtrl("BTN_PATH",		312, cy - 77, 94, 22);
	Skin_MoveCtrl("BTN_SELDEL",		112, 341, 86, 20);
	Skin_MoveCtrl("BTN_DONEDEL",	202, 341, 86, 20);

	/*Skin_MoveCtrl("BTN_TOP",		167, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_UP",			194, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_DOWN",		221 , cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_BOTTOM",		248, cy - 67, 23, 21);*/
	Skin_MoveCtrl("BTN_TOP",		16, 341, 24, 24);
	Skin_MoveCtrl("BTN_UP",			40, 341, 24, 24);
	Skin_MoveCtrl("BTN_DOWN",		64, 341, 24, 24);
	Skin_MoveCtrl("BTN_BOTTOM",		88, 341, 24, 24);

	Skin_MoveCtrl("BTN_SETUP",		551, 130, 76, 20);

	/*Skin_MoveCtrl("BTN_PLAY",		cx - 241, cy - 67, 61, 39);
	Skin_MoveCtrl("BTN_START",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_STOP",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_EXIT2",		cx - 71, cy - 67, 61, 39);*/
	Skin_MoveCtrl("BTN_START",		477, 343, 86, 43);
	Skin_MoveCtrl("BTN_STOP",		477, 343, 86, 43);
	Skin_MoveCtrl("BTN_PLAY",		403, 343, 69, 43);
	Skin_MoveCtrl("BTN_EXIT2",		568, 343, 69, 43);


	/*Skin_MoveCtrl("CHK_PROGRAM",	110, cy - 34, 50, 13); 
	Skin_MoveCtrl("CHK_SYSTEM",		165, cy - 34, 75, 13);*/
	Skin_MoveCtrl("CHK_PROGRAM",	204, 373, 83, 11); 
	Skin_MoveCtrl("CHK_SYSTEM",		306, 373, 72, 11);

	//20070918 현재 파일 프로그레스바 재설정, jyh
	m_pCtrl_ProgressCur.MoveWindow(87, 79, 540, 16);
	// 전체 파일 프로그래스바 재설정
	//m_pCtrl_ProgressTotal.MoveWindow(86, 167, cx - 86 - 17, 17);
	m_pCtrl_ProgressTotal.MoveWindow(87, 105, 540, 16);

	// 텍스트 출력 위치 재설정	
	/*m_pStatic_Time_Use.MoveWindow(85, 146, 70, 11);
	m_pStatic_Time_Over.MoveWindow(227, 146, 70, 11);
	m_pStatic_FileCount.MoveWindow(346, 146, 196, 11);*/
	m_pStatic_Time_Over.MoveWindow(579, 54, 50, 12);
	m_pStatic_Speed.MoveWindow(423, 54, 67, 12);	//20070919 전송 속도 출력 위치, jyh
	m_pStatic_Path.MoveWindow(90, 135, 361, 12);	//20070919 저장 경로 출력 위치, jyh
	m_pStatic_FileName.MoveWindow(27, 53, 350, 12);		//20070919 전송 파일 출력 위치, jyh

	//m_Picture.MoveWindow(22, 118, 47, 17);			//20080322 gif 애니매이션, jyh

	//20070919 추가된 버튼 위치 재설정, jyh
	Skin_MoveCtrl("BTN_FDOPEN",	458, 130, 90, 20);
	////20080313 퀵다운 버튼, jyh
	//Skin_MoveCtrl("BTN_QUICKDOWN", 421, cy - 67, 116, 39);

	// 배너 컨트롤
	if(!m_pBanner)
	{
		m_pBanner = new HtmlViewer;
		m_pBanner->Create(NULL, _T("BANNER"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 0xFFFF);
	}

	//20071004 문제해결 도움말, jyh
	if(!m_pBtnHelp)
	{
		m_pBtnHelp = new HtmlViewer;
		m_pBtnHelp->Create(NULL, _T("BTNHELP"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}


////////// - kth 데이터 전송용 프레임- S
	if(!m_pSendiMBCComplete1)
	{
		m_pSendiMBCComplete1 = new HtmlViewer;
		m_pSendiMBCComplete1->Create(NULL, _T("SENDKTHCOMPLETE1"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendiMBCComplete2)
	{
		m_pSendiMBCComplete2 = new HtmlViewer;
		m_pSendiMBCComplete2->Create(NULL, _T("SENDKTHCOMPLETE2"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendiMBCComplete3)
	{
		m_pSendiMBCComplete3 = new HtmlViewer;
		m_pSendiMBCComplete3->Create(NULL, _T("SENDKTHCOMPLETE3"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendiMBCComplete4)
	{
		m_pSendiMBCComplete4 = new HtmlViewer;
		m_pSendiMBCComplete4->Create(NULL, _T("SENDKTHCOMPLETE4"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
	if(!m_pSendiMBCComplete5)
	{
		m_pSendiMBCComplete5 = new HtmlViewer;
		m_pSendiMBCComplete5->Create(NULL, _T("SENDKTHCOMPLETE5"), WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}
////////// - kth 데이터 전송용 프레임- E







	//20070918 배너 위치, jyh
	//m_pBanner->MoveWindow(9, 46, cx - 9 - 9, 80);
	//m_pBanner->MoveWindow(12, 409, 625, 59);
	m_pBanner->MoveWindow(12, 409, 625, 169);
	m_pBanner->Navigate(CString(URL_BANNER) + "download");	
	m_pBanner->UpdateData();

	//20071004 문제해결 도움말 버튼 위치, jyh
	m_pBtnHelp->MoveWindow(16, 367, 92, 20);
	//20071108 명령인수에서 유저ID 받아오기, jyh
	//20080404 id와 pw를 조합해서 Base64로 인코딩 한 후 웹에 보내준다, jyh
	CString strTemp = AfxGetApp()->m_lpCmdLine;
	CString strUserId;
	CString	strUserPw;
	CString strNameAuth;
	CString strCombine;
	int nPos = 0;

	if(strTemp == "")
	{
		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"프로그램이 정상적인 방법으로 실행되지 않았습니다", 1);
		Exit_Program(false, false);
		return;
	}

	strUserId = strTemp.Tokenize(" ", nPos);
	strUserPw = strTemp.Tokenize(" ", nPos);
	strNameAuth = strTemp.Tokenize(" ", nPos);
	m_nNameAuth = atoi(strNameAuth.GetBuffer());
	strNameAuth.ReleaseBuffer();
	strCombine.Format("%s|%s", strUserId, strUserPw);

	CBase64 base64;
	char* szEncodedText;

	szEncodedText = base64.base64_encode(strCombine.GetBuffer(), strCombine.GetLength());
	strCombine.ReleaseBuffer();

	strTemp.Format("%s%s", URL_HELP, szEncodedText);

	//AfxMessageBox(strTemp);
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
    m_pImageList.Attach((HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("*.txt"), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
	m_pStateImageList.Create(1, 18, ILC_COLORDDB, 1, 0);	//20080319 다운 상태 이미지, jyh
	m_pListCtrl.SetImageList(&m_pImageList, LVSIL_SMALL);
	m_pListCtrl.SetImageList(&m_pStateImageList, LVSIL_STATE);		//20080319 다운 상태 이미지, jyh
	m_pImageList.Detach();
		
	m_pListCtrl.SetHeaderColors(RGB(238, 238, 238), RGB(0, 0, 0));	//20070917 리스트 헤더 색 지정, jyh
	m_pListCtrl.SetToolTips();
	m_pListCtrl.SetColumnCount(2);
	m_pListCtrl.SetCallbackMask(LVIS_FOCUSED);

	//20070917 LVS_EX_GRIDLINES 스타일 추가, jyh
    m_pListCtrl.SetExtendedStyle(m_pListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB/* | LVS_EX_GRIDLINES*/);

	//20070917 리스트 뷰 색 변경, jyh
	m_pListCtrl.SetListColor(	RGB(0, 0, 0), RGB(255, 255, 255),			// 홀수라인의 텍스트/배경
								RGB(0, 0, 0),	RGB(255, 255, 255),			// 짝수라인의 텍스트/배경

								RGB(204, 64, 0), RGB(255, 255, 255),	 		// 선택라인의 텍스트/배경
								RGB(0, 0, 0),							// 전송상태의 외곽라인
								RGB(0, 0, 0), RGB(255, 255, 255),			// 전송되지 않은 부분의 텍스트/배경
								RGB(255, 255, 255), RGB(0, 0, 0)			// 전송된 부분의 텍스트/배경
							);
	//20080303 완료 목록 색 지정, jyh
	m_pListCtrl.SetCompleteTextColor(RGB(98, 98, 98));

	//m_pListCtrl.SetListColor(	RGB(0, 0, 0), RGB(255, 255, 255),			// 홀수라인의 텍스트/배경
	//							RGB(0, 0, 0),	RGB(255, 255, 255),			// 짝수라인의 텍스트/배경
	//							
	//							RGB(247, 93, 0), RGB(255, 255, 255),	 	// 선택라인의 텍스트/배경
	//							RGB(18, 128, 215),							// 전송상태의 외곽라인
	//							RGB(0, 0, 0), RGB(177, 224, 252),			// 전송되지 않은 부분의 텍스트/배경
	//							RGB(255, 255, 255), RGB(16, 158, 234)		// 전송된 부분의 텍스트/배경
	//						);

	//20070917 칼럼 변경, jyh
	/*m_pListCtrl.InsertColumn(1, "", LVCFMT_CENTER, 50, -1);
	m_pListCtrl.InsertColumn(2, "파일이름", LVCFMT_CENTER, 384, -1);*/
	//m_pListCtrl.InsertColumn(0, "형태", LVCFMT_CENTER, 50, -1);
	m_pListCtrl.InsertColumn(0, "", LVCFMT_CENTER, 0, -1);
	m_pListCtrl.InsertColumn(1, "파일이름", LVCFMT_CENTER, 400, -1);
	m_pListCtrl.InsertColumn(2, "크기", LVCFMT_RIGHT, 120, -1);
	m_pListCtrl.InsertColumn(3, "상태", LVCFMT_CENTER, 102, -1);

	/*m_pListCtrl.InsertColumn(0, "파일이름", LVCFMT_LEFT, 170, -1);
	m_pListCtrl.InsertColumn(1, "크기", LVCFMT_RIGHT, 100, -1);
	m_pListCtrl.InsertColumn(2, "속도", LVCFMT_RIGHT, 0, -1);
	m_pListCtrl.InsertColumn(3, "상태", LVCFMT_CENTER, 80, -1);
	m_pListCtrl.InsertColumn(4, "남은시간", LVCFMT_CENTER, 80, -1);
	m_pListCtrl.InsertColumn(5, "저장위치", LVCFMT_LEFT, 90, -1);*/		
}


void use_window::ChnPosition(int cx, int cy)
{
	// 리스트 컨트롤 재설정
	//m_pListCtrl.MoveWindow(10, 205, cx - 10 - 11 , cy - 205 - 79);
	m_pListCtrl.MoveWindow(12, 169, 626 , 182);

	// 버튼 컨트롤 재설정
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
	//Skin_MoveCtrl("BTN_DONEDEL",	74, cy - 67, 94, 21);
	//Skin_MoveCtrl("BTN_PATH",		312, cy - 77, 94, 22);
	Skin_MoveCtrl("BTN_SELDEL",		112, 341, 86, 20);
	Skin_MoveCtrl("BTN_DONEDEL",	202, 341, 86, 20);

	/*Skin_MoveCtrl("BTN_TOP",		167, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_UP",			194, cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_DOWN",		221 , cy - 67, 23, 21);
	Skin_MoveCtrl("BTN_BOTTOM",		248, cy - 67, 23, 21);*/
	Skin_MoveCtrl("BTN_TOP",		16, 341, 24, 24);
	Skin_MoveCtrl("BTN_UP",			40, 341, 24, 24);
	Skin_MoveCtrl("BTN_DOWN",		64, 341, 24, 24);
	Skin_MoveCtrl("BTN_BOTTOM",		88, 341, 24, 24);

	Skin_MoveCtrl("BTN_SETUP",		551, 130, 76, 20);
	
	/*Skin_MoveCtrl("BTN_PLAY",		cx - 241, cy - 67, 61, 39);
	Skin_MoveCtrl("BTN_START",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_STOP",		cx - 171, cy - 67, 91, 39);
	Skin_MoveCtrl("BTN_EXIT2",		cx - 71, cy - 67, 61, 39);*/
	Skin_MoveCtrl("BTN_START",		477, 343, 86, 43);
	Skin_MoveCtrl("BTN_STOP",		477, 343, 86, 43);
	Skin_MoveCtrl("BTN_PLAY",		403, 343, 69, 43);
	Skin_MoveCtrl("BTN_EXIT2",		568, 343, 69, 43);


	/*Skin_MoveCtrl("CHK_PROGRAM",	110, cy - 34, 50, 13); 
	Skin_MoveCtrl("CHK_SYSTEM",		165, cy - 34, 75, 13);*/
	Skin_MoveCtrl("CHK_PROGRAM",	204, 373, 83, 11); 
	Skin_MoveCtrl("CHK_SYSTEM",		306, 373, 72, 11);

	//20070918 현재 파일 프로그레스바 재설정, jyh
	m_pCtrl_ProgressCur.MoveWindow(87, 79, 540, 16);
	// 전체 파일 프로그래스바 재설정
	//m_pCtrl_ProgressTotal.MoveWindow(86, 167, cx - 86 - 17, 17);
	m_pCtrl_ProgressTotal.MoveWindow(87, 105, 540, 16);
	
	// 텍스트 출력 위치 재설정	
	/*m_pStatic_Time_Use.MoveWindow(85, 146, 70, 11);
	m_pStatic_Time_Over.MoveWindow(227, 146, 70, 11);
	m_pStatic_FileCount.MoveWindow(346, 146, 196, 11);*/
	m_pStatic_Time_Over.MoveWindow(579, 54, 50, 12);
	m_pStatic_Speed.MoveWindow(423, 54, 67, 12);	//20070919 전송 속도 출력 위치, jyh
	m_pStatic_Path.MoveWindow(90, 135, 361, 12);	//20070919 저장 경로 출력 위치, jyh
	m_pStatic_FileName.MoveWindow(27, 53, 350, 12);		//20070919 전송 파일 출력 위치, jyh

	//m_Picture.MoveWindow(22, 118, 47, 17);			//20080322 gif 애니매이션, jyh
	
	//20070919 추가된 버튼 위치 재설정, jyh
	Skin_MoveCtrl("BTN_FDOPEN",	458, 130, 90, 20);
	////20080313 퀵다운 버튼, jyh
	//Skin_MoveCtrl("BTN_QUICKDOWN", 421, cy - 67, 116, 39);

	// 배너 컨트롤
	if(!m_pBanner)
	{
		m_pBanner = new HtmlViewer;
		m_pBanner->Create(NULL, _T("BANNER"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 0xFFFF);
	}

	//20071004 문제해결 도움말, jyh
	if(!m_pBtnHelp)
	{
		m_pBtnHelp = new HtmlViewer;
		m_pBtnHelp->Create(NULL, _T("BTNHELP"), WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	}

	//20070918 배너 위치, jyh
	//m_pBanner->MoveWindow(9, 46, cx - 9 - 9, 80);
	m_pBanner->MoveWindow(12, 409, 625, 59);
	m_pBanner->Navigate(CString(URL_BANNER) + "download");	
	m_pBanner->UpdateData();

	//20071004 문제해결 도움말 버튼 위치, jyh
	m_pBtnHelp->MoveWindow(16, 367, 92, 20);
	//20071108 명령인수에서 유저ID 받아오기, jyh
	//20080404 id와 pw를 조합해서 Base64로 인코딩 한 후 웹에 보내준다, jyh
	CString strTemp = AfxGetApp()->m_lpCmdLine;
	CString strUserId;
	CString	strUserPw;
	CString strNameAuth;
	CString strCombine;
	int nPos = 0;
	
	if(strTemp == "")
	{
		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"프로그램이 정상적인 방법으로 실행되지 않았습니다", 1);
		Exit_Program(false, false);
		return;
	}

	strUserId = strTemp.Tokenize(" ", nPos);
	strUserPw = strTemp.Tokenize(" ", nPos);
	strNameAuth = strTemp.Tokenize(" ", nPos);
	m_nNameAuth = atoi(strNameAuth.GetBuffer());
	strNameAuth.ReleaseBuffer();
	strCombine.Format("%s|%s", strUserId, strUserPw);

	CBase64 base64;
	char* szEncodedText;

	szEncodedText = base64.base64_encode(strCombine.GetBuffer(), strCombine.GetLength());
	strCombine.ReleaseBuffer();
	
	strTemp.Format("%s%s", URL_HELP, szEncodedText);
	
	//AfxMessageBox(strTemp);
	m_pBtnHelp->Navigate(strTemp);
	//m_pBtnHelp->Navigate("http://mfile.co.kr/mmsv/help.php");
	m_pBtnHelp->UpdateData();

	//// 리스트 컨트롤 컬럼 넓이
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

	else if(m_ButtonName == "BTN_PLAY")
	{
		int nItemCheck = -1;
		PRDOWNCTX pItemData = NULL;
		CString strBuf, strFileType;

		// 파일 선택 체크
		if((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) == -1)		
		{
			MSG_Window((WPARAM)"재생할 동영상 파일을 선택해주세요.", 1);
			return true;
		}

		// 재생가능한 사이즈 체크
		pItemData = (PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck);
		if(pItemData->nFileSizeEnd == 0)
		{
			MSG_Window((WPARAM)"선택하신 파일의 사이즈가 0Byte입니다\r\n다운로드를 시작 후 '재생'버튼을 클릭해주세요.", 1);
			return true;
		}
		ShellExecute(m_hWnd, NULL, pItemData->pFullPath, NULL, NULL, SW_SHOWNORMAL);
		//기존 곰플레이 재생을 단순 실행으로 변경 
/*
		// 곰플레이어 설치 체크
		if(m_rPathGomPlayer.IsEmpty())
		{
			Dlg_Confirm pWindow;
			pWindow.m_nCallMode = 3;
 			if(pWindow.DoModal() == IDOK)
				m_pBanner->Navigate("http://app.ipop.co.kr/gom/GOMPLAYERSETUP.EXE");	
			return true;
		}		

		ShellExecute(m_hWnd, NULL, m_rPathGomPlayer, pItemData->pFullPath, NULL, SW_SHOWNORMAL);	
*/
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
		::ShowWindow(this->m_hWnd, SW_MINIMIZE);
	}

	else if(m_ButtonName == "BTN_TRAY")
	{
		::SetTimer(this->m_hWnd, 777, 700, NULL);	//20080226 트래이 윈도우 타이머 설정, jyh
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
			//20080320 다운 상태 이미지 버그 수정, jyh
			for(int nIndex = 0; nIndex < m_pListCtrl.GetItemCount() ; nIndex++)
				m_pListCtrl.SetItemState(nIndex, 0, -1);
		}
		
		return true;
	}

	//20080228 전송중에도 선택목록 삭제 되게, jyh
	else if(m_ButtonName == "BTN_SELDEL")// && !m_bFileTrans)
	{
		DeleteList(1);
		return true;
	}

	//20071002 전송중에도 완료목록 삭제 되게, jyh
	else if(m_ButtonName == "BTN_DONEDEL")// && !m_bFileTrans)
	{
		DeleteList(2);
		return true;
	}

	else if(m_ButtonName == "BTN_PATH" && !m_bFileTrans)
	{		
		int nBanCount = 0;
		int nSelectCount = 0;
		int nItemCheck = -1;


		if((nItemCheck = m_pListCtrl.GetNextItem(-1, LVNI_SELECTED)) == -1)		
		{
			MSG_Window((WPARAM)"선택된 파일이 없습니다.\r\n파일을 선택하신 후 다시 실행해주세요.", 1);
			return true;
		}

		do
		{
			//20080320 다운 상태 이미지 버그 수정, jyh
			if(((PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck))->nTransState != ITEM_STATE_WAIT)
			{
				m_pListCtrl.SetItemState(nItemCheck, 0, -1);
				nBanCount++;
			}			
			else
			{
				nSelectCount++;
			}
		}
		while((nItemCheck = m_pListCtrl.GetNextItem(nItemCheck, LVNI_SELECTED)) != -1);
			
		if(nBanCount > 0)
		{
			MSG_Window((WPARAM)"대기중인 파일만 저장경로 변경이 가능합니다.", 1);
			return true;
		}

		if(nSelectCount == 0)		
		{
			MSG_Window((WPARAM)"선택된 파일이 없습니다.\r\n파일을 선택하신 후 다시 실행해주세요.", 1);
			return true;
		}


		DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		CSelectFolderDialog pSelectDialog(FALSE, m_rPathBasic, wFlag, NULL, this, "저장될 폴더를 선택해 주세요");
		if(pSelectDialog.DoModal() == IDOK)	
		{
			CString strNewPath, strNewFolder;
			CString pBasePath = pSelectDialog.GetSelectedPath();

			if(pBasePath.Right(1) != "\\")
				pBasePath += "\\";

			nItemCheck = -1;
			while((nItemCheck = m_pListCtrl.GetNextItem(nItemCheck, LVNI_SELECTED)) != -1)
			{
				strNewPath = pBasePath;
				strNewPath += ((PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck))->pFolderName;
				
				if(strNewPath.Right(1) != "\\")
					strNewPath += "\\";

				strNewFolder = strNewPath;

				strNewPath += ((PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck))->pFileName;
				strNewPath.Replace("/", "\\");

				strcpy_s(((PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck))->pSavePath, strNewFolder.GetLength()+1, (LPSTR)(LPCTSTR)strNewFolder);
				strcpy_s(((PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck))->pFullPath, strNewPath.GetLength()+1, (LPSTR)(LPCTSTR)strNewPath);

				//m_pListCtrl.SetItemText(nItemCheck, 5, strNewFolder);	//20070918 주석 처리, jyh
			}

			m_rPathLast = pBasePath;
			m_pReg.SaveKey(CLIENT_REG_PATH, "rPathLast", m_rPathLast);
		}
		return true;
	}

	else if(m_ButtonName == "BTN_TOP" && !m_bFileTrans)
	{
		MoveItem(MOVE_MF);
		return true;
	}

	//20080228 전송중에도 선택 이동 되게, jyh
	else if(m_ButtonName == "BTN_UP")// && !m_bFileTrans)
	{
		MoveItem(MOVE_MU);
		return true;
	}

	//20080228 전송중에도 선택 이동 되게, jyh
	else if(m_ButtonName == "BTN_DOWN")// && !m_bFileTrans)
	{
		MoveItem(MOVE_MD);
		return true;
	}

	//20080228 전송중에도 선택 이동 되게, jyh
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
		if((GetTickCount() - m_nTimeTransButton) > 10)	//20071002 다운로드 버튼 인식 시간 수정, jyh
		{
			m_nTimeTransButton = GetTickCount();
			
			PRDOWNCTX pPtrCheck;
			int nTItemCount = 0;

			for(int nProcessIndex = 0 ; nProcessIndex < m_pListCtrl.GetItemCount(); nProcessIndex++)
			{	
				pPtrCheck = (PRDOWNCTX)m_pListCtrl.GetItemData(nProcessIndex);

				//20080228 사용자 초과로 다운중지 이후 재시도시 다운 되게
				//ITEM_STATE_DEFER, ITEM_STATE_CONNECT, ITEM_STATE_DELAY, ITEM_STATE_CONFAILED 추가, jyh
				if(pPtrCheck != NULL && 
				   (pPtrCheck->nTransState == ITEM_STATE_WAIT || pPtrCheck->nTransState == ITEM_STATE_TRANS ||
				   pPtrCheck->nTransState == ITEM_STATE_STOP || pPtrCheck->nTransState == ITEM_STATE_CONNECT))
					nTItemCount++;
			}

			if(nTItemCount == 0)
			{
				::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"전송 가능한 파일이 존재하지 않습니다!", 1);
				return true;
			}
			
			//SetTransFlag(true);
			//20071119 서버와 연결여부에 따라, jyh
			if(m_pWorker_Ctrl->m_bConnectServer)
				SetTransFlag(true);
			else	//연결이 끊겼으면 소켓을 다시 접속해야 하기 때문에 연결 타이머(99) 설정
				SetTimer(99, 1000, NULL);
		}
	}

	else if(m_ButtonName == "BTN_STOP")
	{
		//if((GetTickCount() - m_nTimeTransButton) > 2000)
		if((GetTickCount() - m_nTimeTransButton) > 10)	//20071002 일시정지 버튼 인식 시간 수정, jyh
		{
			m_nTimeTransButton = GetTickCount();

			//20070918 파일 상태 중지로 변경, jyh
			if(m_pPtrTrans)
			{
				m_pPtrTrans->nTransState = ITEM_STATE_STOP;
				m_pListCtrl.SetItemText(m_nCurTransFile, 3, GetFileStatus(ITEM_STATE_STOP));	
			}

			SetTransFlag(false);
		}
	}

	//20070919 저장 폴더 열기, jyh
	else if(m_ButtonName == "BTN_FDOPEN")
	{
		int nItemCheck = -1;
		CString strPath;

		nItemCheck = m_pListCtrl.GetNextItem(nItemCheck, LVNI_SELECTED);

		if(nItemCheck < 0)
		{
			MSG_Window((WPARAM)"전송중이거나 전송된 파일이 없습니다!", 1);
			return false;
		}

		strPath = ((PRDOWNCTX)m_pListCtrl.GetItemData(nItemCheck))->pFolderName;

		ShellExecute(m_hWnd, "open", strPath, NULL, NULL, SW_SHOWNORMAL);
	}
	////20080313 퀵 다운로드, jyh
	//else if(m_ButtonName == "BTN_QUICKDOWN")
	//{
	//	int nSelItem;
	//	//SetTransFlag(false);

	//	POSITION pos = m_pListCtrl.GetFirstSelectedItemPosition();

	//	if(pos)
	//	{
	//		while(pos)
	//		{
	//			nSelItem = m_pListCtrl.GetNextSelectedItem(pos);
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		::SendMessage(m_hWnd, MSG_ALERTWINDOW, (WPARAM)"고속 다운으로 전환할 파일을 선택 하세요!", 1);
	//		return true;
	//	}

	//	CBase64 base64;
	//	CString strTemp;
	//	char* szEncodedText;
	//	
	//	strTemp.Format("%s|%s", m_pUserID, m_pUserPW);
	//	szEncodedText = base64.base64_encode(strTemp.GetBuffer(), strTemp.GetLength());
	//	strTemp.ReleaseBuffer();
	//	__int64 nConidx = ((PRDOWNCTX)m_pListCtrl.GetItemData(nSelItem))->nBoardIndex;
	//	strTemp.Format("http://gfile.co.kr/main/popup/pquick.php?id=%s&fidx=%I64d", szEncodedText, nConidx);

	//	CComVariant varEmpty(0);
	//	CComPtr<IWebBrowser2> spIExplorer = NULL;

	//	spIExplorer.CoCreateInstance(CLSID_InternetExplorer);

	//	if(spIExplorer == NULL)
	//		return false;

	//	spIExplorer->put_StatusBar(VARIANT_FALSE);
	//	spIExplorer->put_Width(500);
	//	spIExplorer->put_Height(370);
	//	spIExplorer->put_ToolBar(VARIANT_FALSE);
	//	spIExplorer->put_MenuBar(VARIANT_FALSE);
	//	//CComBSTR bstr(strTemp);
	//	spIExplorer->Navigate(CComBSTR(strTemp), &varEmpty, &varEmpty, &varEmpty, &varEmpty);
	//	spIExplorer->put_Visible(VARIANT_TRUE);
	//}

	else if(m_ButtonName == "BTN_REFILL")
	{
	}	

	else if(m_ButtonName == "CHK_PROGRAM")
	{
		m_rDownEndAutoClose = Skin_GetCheck("CHK_PROGRAM");
		m_pReg.SaveKey(CLIENT_REG_PATH, "rDownEndAutoClose",m_rDownEndAutoClose);
		return true;
	}

	else if(m_ButtonName == "CHK_SYSTEM")
	{
		return true;
	}

	return true;
}

BEGIN_MESSAGE_MAP(use_window, down_window)
ON_WM_QUERYOPEN()
//ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//20080131 최소화 된 창이 원래 크기로 복구될때 발생하는 메세지, jyh
BOOL use_window::OnQueryOpen()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMinWindow = FALSE;
	KillTimer(777);			//20080226 jyh

	return down_window::OnQueryOpen();
}

//void use_window::OnLButtonDown(UINT nFlags, CPoint point)
//{//9, 430, 116, 20
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//	if(point.x >= 9 && point.x <= 125 && point.y >= 430 && point.y <= 450)
//	{
//		ShellExecute(m_hWnd, "open", "iexplore.exe", "http://gfile.co.kr/mmsv/help.php?id=kyakya80", NULL, SW_SHOWNORMAL);
//		//AfxMessageBox("클릭!!");
//	}
//
//	down_window::OnLButtonDown(nFlags, point);
//}
