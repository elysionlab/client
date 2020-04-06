// DebugDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "down_client.h"
#include "DebugDlg.h"


// DebugDlg 대화 상자입니다.

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


// DebugDlg 메시지 처리기입니다.
