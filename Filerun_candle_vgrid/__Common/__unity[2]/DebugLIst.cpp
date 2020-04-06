// DebugLIst.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
//#include "down_client.h"
#include "DebugLIst.h"


// DebugLIst ��ȭ �����Դϴ�.

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
	m_pList.InsertColumn(0, "�з�", LVCFMT_LEFT, 100, -1);
	m_pList.InsertColumn(1, "��", LVCFMT_LEFT, 300, -1);
	return TRUE;
}


// DebugLIst �޽��� ó�����Դϴ�.
void DebugLIst::InsertData(char* type, char* data)
{
	int nInsertPos = -1;
	int nItem = -1;
	CString strBuf, strBufSub, strFileType, strNewPath;

	strBuf.Format("%d", data);
	nItem = m_pList.InsertItem(m_pList.GetItemCount(), type);	
	m_pList.SetItemText(nItem, 1, data);
}