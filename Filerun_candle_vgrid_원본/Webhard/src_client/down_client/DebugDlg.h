#pragma once


// DebugDlg ��ȭ �����Դϴ�.

class DebugDlg : public CDialog
{
	DECLARE_DYNAMIC(DebugDlg)

public:
	DebugDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DebugDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
