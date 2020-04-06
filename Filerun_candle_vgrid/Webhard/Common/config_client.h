/*
 * Dlg_Alert INFO
 */

//#define DLGALERT_INFO_RECT				CRect(17, 50, 17 + 287, 50 + 64)
//#define DLGALERT_CLOSE_RECT				CRect(135, 133, 135 + 50, 133 + 21)
#define DLGALERT_INFO_RECT				CRect(22, 46, 22 + 453, 46 + 52)		//20070920 수정, jyh
#define DLGALERT_CLOSE_RECT				CRect(222, 118, 222 + 49, 118 + 20)		//20070920 수정, jyh

//재시도 알림창
#define DLGALERT_RETRY_RECT				CRect(168, 118, 168 + 61, 118 + 20)		//20071119 추가, jyh
#define DLGALERT_JUMP_RECT				CRect(231, 118, 231 + 61, 118 + 20)		//20071119 추가, jyh
#define DLGALERT_CANCEL_RECT			CRect(294, 118, 294 + 49, 118 + 20)		//20071119 추가, jyh
//세션 끊김 알림창
#define DLGALERT_YES_RECT				CRect(205, 118, 205 + 49, 118 + 20)		//20071119 추가, jyh
#define DLGALERT_NO_RECT				CRect(258, 118, 258 + 49, 118 + 20)		//20071119 추가, jyh

#define DLGALERT_BACK_COLOR				RGB(255, 255, 255)
#define DLGALERT_TEXT_COLOR				RGB(0, 0, 0)


/*
 * Dlg_Confirm INFO
 */
//#define DLGCONFIRM_TITLE_RECT			CRect(16, 14, 16 + 137, 14 + 13)
//#define DLGCONFIRM_TEXT_RECT			CRect(18, 52, 18 + 288, 52 + 59)
//#define DLGCONFIRM_YES_RECT				CRect(103, 133, 103 + 50, 133 + 21)
//#define DLGCONFIRM_NO_RECT				CRect(167, 133, 167 + 55, 133 + 21)
//#define DLGCONFIRM_OK_RECT				CRect(18, 104, 18 + 169, 104 + 13)
#define DLGCONFIRM_TITLE_RECT			CRect(16, 9, 16 + 137, 9 + 12)		//20070920 수정, jyh
#define DLGCONFIRM_TEXT_RECT			CRect(22, 48, 22 + 453, 48 + 50)		//20070920 수정, jyh
#define DLGCONFIRM_YES_RECT				CRect(194, 118, 194 + 49, 118 + 20)		//20070920 수정, jyh
#define DLGCONFIRM_NO_RECT				CRect(245, 118, 245 + 49, 118 + 20)		//20070920 수정, jyh
#define DLGCONFIRM_OK_RECT				CRect(222, 118, 222 + 49, 118 + 20)		//20071119 추가, jyh
#define DLGCONFIRM_CHK_OK_RECT			CRect(10, 123, 10 + 168, 123 + 10)		//20070920 수정, jyh
#define DLGCONFIRM_TEXT_COLOR			RGB(0, 0, 0)
#define DLGCONFIRM_BACK_COLOR			RGB(255, 255, 255)



/*
 * Dlg_Exist INFO
 */
#define DLGEXIST_NAME_RECT				CRect(80, 51, 80 + 213, 52 + 12)
#define DLGEXIST_PATH_RECT				CRect(80, 79, 80 + 213, 80 + 12)
#define DLGEXIST_TEXT_RECT				CRect(80, 107, 80 + 213, 107 + 12)
#define DLGEXIST_ALL_RECT				CRect(80, 133, 80 + 95, 133 + 13)
#define DLGEXIST_KEEP_RECT				CRect(44, 163, 44  + 68, 163 + 21)
#define DLGEXIST_OVERWRITE_RECT			CRect(126, 163, 126 + 68, 163 + 21)
#define DLGEXIST_CANCEL_RECT			CRect(208, 163, 208 + 68, 163 + 21)
#define DLGEXIST_BACK1_COLOR			RGB(255, 255, 255)
#define DLGEXIST_TEXT1_COLOR			RGB(0, 0, 0)
#define DLGEXIST_BACK2_COLOR			RGB(255, 255, 255)
#define DLGEXIST_TEXT2_COLOR			RGB(0, 0, 0)



/*
 * Dlg_Setup INFO
 */
#define DLGSETUP_UP_RECT				CRect(215, 114, 215 + 140, 114 + 15)
#define DLGSETUP_DOWN_RECT				CRect(215, 142, 215 + 140, 142 + 15)
#define DLGSETUP_ITEMPATH_RECT			CRect(215, 170, 215 + 140, 170 + 15)

//#define DLGSETUP_SEARCH_RECT			CRect(287, 202, 287 + 79, 202 + 17)
//#define DLGSETUP_PATH_RECT				CRect(113, 203, 113 + 167, 203 + 11)
#define DLGSETUP_SEARCH_RECT			CRect(322, 45, 383, 66)	//20070919 jyh
#define DLGSETUP_PATH_RECT				CRect(100, 51, 313, 63)	//20070919 jyh

//20070920 추가, jyh
#define DLGSETUP_APPLY_RECT				CRect(128, 179, 128 + 60, 179 + 30)
#define DLGSETUP_CLOSE_RECT				CRect(192, 179, 192 + 60, 179 + 30)
#define DLGSETUP_DOWNFOLDER_RECT		CRect(24, 97, 24 + 10, 97 + 10)
#define DLGSETUP_SHORTCUT_RECT			CRect(24, 119, 24 + 10, 119 + 10)
#define DLGSETUP_AUTODOWN_RECT			CRect(24, 141, 24 + 10, 141 + 10)
#define DLGSETUP_EXIT_RECT				CRect(354, 7, 354 + 18, 7 + 17)

//안씀----------------------------------------------
#define DLGSETUP_OK_RECT				CRect(129, 249, 153 + 50, 249 + 21)
#define DLGSETUP_CANCEL_RECT			CRect(192, 249, 192 + 50, 249 + 21)

#define DLGSETUP_UPCLOSE_RECT			CRect(47, 66, 47 + 11, 66 + 11)
#define DLGSETUP_UPTRANSE_RECT			CRect(47, 94, 47 + 11, 94 + 11)
#define DLGSETUP_DWCLOSE_RECT			CRect(99, 66, 99 + 11, 66 + 11)
#define DLGSETUP_DWTRANS_RECT			CRect(99, 94, 99 + 11, 94 + 11)
//-------------------------------------------------

#define DLGSETUP_BACK_COLOR				RGB(255, 255, 255)
#define DLGSETUP_TEXT_COLOR				RGB(0, 0, 0)
#define DLGSETUP_OPTION_TEXT_COLOR		RGB(59, 55, 19)

//20080131 Slide Dialog 추가, jyh
#define DLGSLIDE_EXIT_RECT				CRect(203, 7, 203 + 18, 7 + 17)
#define DLGSLIDE_TEXT_RECT				CRect(35, 45, 208, 95)


