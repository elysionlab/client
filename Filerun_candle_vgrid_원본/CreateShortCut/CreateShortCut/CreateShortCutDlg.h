// CreateShortCutDlg.h : ��� ����
//

#pragma once


// CCreateShortCutDlg ��ȭ ����
class CCreateShortCutDlg : public CDialog
{
// �����Դϴ�.
public:
	CCreateShortCutDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATESHORTCUT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

public:
	bool CreateShortCut(char* strURL);


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
