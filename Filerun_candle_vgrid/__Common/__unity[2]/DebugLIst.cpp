// DebugLIst.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "down_client.h"
#include "DebugLIst.h"


// DebugLIst 대화 상자입니다.

IMPLEMENT_DYNAMIC(DebugLIst, CDialog)
DebugLIst::DebugLIst(CWnd* pParent /*=NULL*/)
	: CDialog(DebugLIst::IDD, pParent)
{	
}

DebugLIst::~DebugLIst()
{
}

void DebugLIst::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_pList);
}


BEGIN_MESSAGE_MAP(DebugLIst, CDialog)
END_MESSAGE_MAP()

BOOL DebugLIst::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pList.InsertColumn(0, "분류", LVCFMT_LEFT, 100, -1);
	m_pList.InsertColumn(1, "상세", LVCFMT_LEFT, 300, -1);
	return TRUE;
}


// DebugLIst 메시지 처리기입니다.
void DebugLIst::InsertData(char* type, char* data)
{
	int nInsertPos = -1;
	int nItem = -1;
	CString strBuf, strBufSub, strFileType, strNewPath;

	strBuf.Format("%d", data);
	nItem = m_pList.InsertItem(m_pList.GetItemCount(), type);	
	m_pList.SetItemText(nItem, 1, data);
}