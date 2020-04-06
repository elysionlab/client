#pragma once

#include "resource.h"     
#include <atlhost.h>
#include <afxcmn.h>
#include <afxext.h>   
#include "Registry.h"
#include "WebBBS.h"
#include "config.h"


class UploadSelect : public CDialogImplBase
{

public:

	UploadSelect()
	{
		m_pRegPath.Empty();
		m_pReg.LoadKey(CLIENT_REG_PATH, "strWebPath", m_pRegPath);

		if(m_pRegPath.IsEmpty())
		{
			TCHAR pDeskTopPath[MAX_PATH];
			SHGetSpecialFolderPath(NULL, pDeskTopPath, CSIDL_DESKTOP, FALSE);
			m_pRegPath = pDeskTopPath;
		}
	}
	~UploadSelect() {}

	BEGIN_MSG_MAP(UploadSelect)
	END_MSG_MAP()

public:

	CWebBBS*		m_pMain;
	CRegistry	m_pReg;	
	CString		m_pRegPath;
	
public:

	void ShowDlgFile();
	void ShowDlgOpenFile();		//20071112 ���� �ڷ�� ���� ���� ��ȭ����, jyh
	void ShowDlgFolder();
	void ShowDlgOpenFolder();	//20071129 ���� �ڷ�� ���� ���� ��ȭ����, jyh
	void FolderSubInfo(CString strTargetFolder, CString strRoot);
};
