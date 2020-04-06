#include "stdafx.h"
#include <comutil.h>
#include "UploadSelect.h"
#include "SelectFolderDialog.h"
//20080612 �ؽð� ���� Ŭ���� �߰�, jyh
#include "MD5_Client.h"


void UploadSelect::ShowDlgFile()
{
	DWORD dFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | /*OFN_ALLOWMULTISELECT | */OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON;
	CFileDialog pWindow(TRUE, NULL, NULL, dFlag);

	CFileStatus statusFile; 
	int nSelectedCount = 0;
	CString pSelectedPath;
	CString pSelectedSize;
	POSITION startPosition;
	char pFileName[10000] = {0};
	CMD5 pMD5;	//20080612 �ؽð� ���� Ŭ����, jyh
	CString	strMD5CheckSum;	//20080612 �ؽð� ���� ����, jyh
	DWORD dwErrorCode = NO_ERROR;

	pWindow.m_ofn.lpstrFile = pFileName;
	pWindow.m_ofn.nMaxFile = sizeof(pFileName);
	pWindow.m_ofn.lpstrInitialDir = m_pRegPath;
	pWindow.m_ofn.lpstrTitle = "���ε� �Ͻ� ������ �������ּ���";

	if(IDOK == pWindow.DoModal())
	{		
		startPosition = pWindow.GetStartPosition();

        while(startPosition)
		{						
			pSelectedPath = pWindow.GetNextPathName(startPosition);	
			
			CFileFind mFind;
			if(mFind.FindFile(pSelectedPath))
			{
				mFind.FindNextFile();
				pSelectedSize.Format("%I64d", mFind.GetLength());
				//20080612 �ؽð� ����, jyh
				strMD5CheckSum = pMD5.GetString(mFind.GetFilePath(), dwErrorCode);
				//m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t(pSelectedSize), _bstr_t("-1"));
				m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t(pSelectedSize), _bstr_t("-1"), _bstr_t(strMD5CheckSum));
			}
			mFind.Close();	

			if(nSelectedCount == 0)
			{
				m_pRegPath.Format("%s", pSelectedPath);
				//20071217 ������Ʈ���� ���۱� ��Ͻ� �����ϴ� ���� ��θ� �����Ѵ�, jyh
				m_pReg.SaveKey(CLIENT_REG_PATH, "strCopyrightPath", pSelectedPath);
				::PathRemoveFileSpec((LPTSTR)(LPCTSTR)m_pRegPath);	
				m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", m_pRegPath);
			}

			nSelectedCount ++;
        }		
	}
}

//20071112 ���� �ڷ�� ���� ���� ��ȭ����, jyh
void UploadSelect::ShowDlgOpenFile()
{
	DWORD dFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | /*OFN_ALLOWMULTISELECT | */OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON;
	CFileDialog pWindow(TRUE, NULL, NULL, dFlag);

	CFileStatus statusFile; 
	int nSelectedCount = 0;
	CString pSelectedPath;
	CString pSelectedSize;
	POSITION startPosition;
	char pFileName[10000] = {0};

	pWindow.m_ofn.lpstrFile = pFileName;
	pWindow.m_ofn.nMaxFile = sizeof(pFileName);
	pWindow.m_ofn.lpstrInitialDir = m_pRegPath;
	pWindow.m_ofn.lpstrTitle = "���ε� �Ͻ� ������ �������ּ���";

	if(IDOK == pWindow.DoModal())
	{		
		startPosition = pWindow.GetStartPosition();

		while(startPosition)
		{						
			pSelectedPath = pWindow.GetNextPathName(startPosition);	

			CFileFind mFind;
			if(mFind.FindFile(pSelectedPath))
			{
				mFind.FindNextFile();

				//���� ����� 50MB �̻��� ���ε� ����
				if(mFind.GetLength() > 52428800)
				{
					AfxMessageBox("50MB �̻��� ���ε� �� �� �����ϴ�!");
					return;
				}

				pSelectedSize.Format("%I64d", mFind.GetLength());
				m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t(pSelectedSize), _bstr_t("-1"), _bstr_t("-1"));
			}
			mFind.Close();	

			if(nSelectedCount == 0)
			{
				m_pRegPath.Format("%s", pSelectedPath);
				::PathRemoveFileSpec((LPTSTR)(LPCTSTR)m_pRegPath);	
				m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", m_pRegPath);
			}

			nSelectedCount ++;
		}		
	}
}

void UploadSelect::ShowDlgFolder()
{
	CString pSelectedPath;

	DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	CSelectFolderDialog pSelectDialog(FALSE, m_pRegPath, wFlag, NULL, NULL, "���ε� �Ͻ� ������ �������ּ���");
	if(pSelectDialog.DoModal() == IDOK)	
	{
		pSelectedPath = pSelectDialog.GetSelectedPath();
		//20090324 ����̺�� ���õ��� �ʰ� �Ѵ�, jyh
		int nCnt = 0, nPos = 0;
		while((nPos = pSelectedPath.Find("\\", nPos)) > 0)
		{
			nPos++;
			nCnt++;
		}

		if(nCnt == 1)
		{
			AfxMessageBox("����̺�� ������ �� �����ϴ�!");
			return;
		}

		m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", pSelectedPath);
		//20071217 ������Ʈ���� ���۱� ��Ͻ� �����ϴ� ���� ��θ� �����Ѵ�, jyh
		m_pReg.SaveKey(CLIENT_REG_PATH, "strCopyrightPath", pSelectedPath);

		if(pSelectedPath.Right(1) == '\\')
			pSelectedPath = pSelectedPath.Left(pSelectedPath.GetLength() -1);
		m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t("-1"), _bstr_t("-1"), _bstr_t("-1"));
		FolderSubInfo(pSelectedPath, pSelectedPath);
	}
}

//20071112 ���� �ڷ�� ���� ���� ��ȭ����, jyh
void UploadSelect::ShowDlgOpenFolder()
{
	CString pSelectedPath;

	DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	CSelectFolderDialog pSelectDialog(FALSE, m_pRegPath, wFlag, NULL, NULL, "���ε� �Ͻ� ������ �������ּ���");
	if(pSelectDialog.DoModal() == IDOK)	
	{
		pSelectedPath = pSelectDialog.GetSelectedPath();
		m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", pSelectedPath);

		if(pSelectedPath.Right(1) == '\\')
			pSelectedPath = pSelectedPath.Left(pSelectedPath.GetLength() -1);

		ULONGLONG nFolderSize = 0;
		nFolderSize = pSelectDialog.GetFolderSize(pSelectedPath);

		//���� ����� 50MB �̻��� ���ε� ����
		if(nFolderSize > 52428800)
		{
			AfxMessageBox("50MB �̻��� ���ε� �� �� �����ϴ�!");
			return;
		}

		m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t("-1"), _bstr_t("-1"), _bstr_t("-1"));
		FolderSubInfo(pSelectedPath, pSelectedPath);
	}
}


void UploadSelect::FolderSubInfo(CString strTargetFolder, CString strRoot)
{
	CFileFind	mFind;	
	CString		pCheckPath;
	CString		pSize;
	CFileStatus statusFile;
	CMD5		pMD5;	//20080612 �ؽð� ���� Ŭ����, jyh
	CString		strMD5CheckSum;	//20080612 �ؽð� ���� ����, jyh
	DWORD dwErrorCode = NO_ERROR;

	if(strTargetFolder.Right(1) != "\\")
		strTargetFolder += "\\";

	pCheckPath = strTargetFolder + "*.*";

	BOOL bFind = mFind.FindFile(pCheckPath); 
	while(bFind)
	{
		bFind = mFind.FindNextFile();
        
		if(mFind.IsDots())
			continue;

		if(!mFind.IsHidden() && mFind.IsDirectory())
        {				
			m_pMain->SelectComplete(_bstr_t(mFind.GetFilePath()), _bstr_t("-1"), _bstr_t(strRoot), _bstr_t("-1"));
			FolderSubInfo(mFind.GetFilePath(), strRoot);
        }
    }
	mFind.Close();

	bFind = mFind.FindFile(pCheckPath); 
	while(bFind)
	{
		bFind = mFind.FindNextFile();
        
		if(mFind.IsDots())
			continue;

        if(!mFind.IsHidden() && !mFind.IsSystem() && !mFind.IsDirectory())
        {
			//20080612 �ؽð��� ����, jyh
			strMD5CheckSum = pMD5.GetString(mFind.GetFilePath(), dwErrorCode);
			pSize.Format("%I64d", mFind.GetLength());
			m_pMain->SelectComplete(_bstr_t(mFind.GetFilePath()), _bstr_t(pSize), _bstr_t(strRoot), _bstr_t(strMD5CheckSum));
        }
    }
	mFind.Close();	
}







