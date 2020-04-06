#pragma once
#include "afxcmn.h"


// DebugLIst 대화 상자입니다.

class DebugLIst : public CDialog
{
	DECLARE_DYNAMIC(DebugLIst)

public:
	DebugLIst(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DebugLIst();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

	void InsertData(char* type, char* data);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_pList;
};
