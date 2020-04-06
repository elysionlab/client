#pragma once
#include "SkinDialog.h"
#include "afxwin.h"
#include "TrayIconPosition.h"		//20080225 jyh
#include "afxcmn.h"
#include "..\..\..\__common\__unity[2]\skinprogress.h"


// CTrayWindow 대화 상자입니다.

class CTrayWindow : public CSkinDialog
{
public:
	CTrayWindow(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTrayWindow();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void InitWindow();
	void SetTrayIconHandle(HWND hWnd);	//20080225 트래이 아이콘 핸들 저장, jyh
	CStatic m_static_text;		//진행 정보
	CString m_pInfo;
	int m_nFileTransPercent;
	CTrayIconPosition* m_pTipPosition;	//20080225 트래이 아이콘 위치 찾는 클래스, jyh
	SkinProgress m_Ctrl_progressTotal;
	CStatic m_static_Percent;
};
