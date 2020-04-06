#pragma once
#include "SkinDialog.h"
#include "afxwin.h"
#include "TrayIconPosition.h"		//20080225 jyh
#include "afxcmn.h"
#include "..\..\..\__common\__unity[2]\skinprogress.h"


// CTrayWindow ��ȭ �����Դϴ�.

class CTrayWindow : public CSkinDialog
{
public:
	CTrayWindow(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTrayWindow();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_TRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void InitWindow();
	void SetTrayIconHandle(HWND hWnd);	//20080225 Ʈ���� ������ �ڵ� ����, jyh
	CStatic m_static_text;		//���� ����
	CString m_pInfo;
	int m_nFileTransPercent;
	CTrayIconPosition* m_pTipPosition;	//20080225 Ʈ���� ������ ��ġ ã�� Ŭ����, jyh
	SkinProgress m_Ctrl_progressTotal;
	CStatic m_static_Percent;
};
