// DebugDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "down_client.h"
#include "DebugDlg.h"


// DebugDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DebugDlg, CDialog)
DebugDlg::DebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DebugDlg::IDD, pParent)
{
}

DebugDlg::~DebugDlg()
{
}

void DebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DebugDlg, CDialog)
END_MESSAGE_MAP()


// DebugDlg �޽��� ó�����Դϴ�.
