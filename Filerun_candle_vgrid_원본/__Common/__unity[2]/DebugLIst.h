#pragma once
#include "afxcmn.h"


// DebugLIst ��ȭ �����Դϴ�.

class DebugLIst : public CDialog
{
	DECLARE_DYNAMIC(DebugLIst)

public:
	DebugLIst(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DebugLIst();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

	void InsertData(char* type, char* data);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_pList;
};
