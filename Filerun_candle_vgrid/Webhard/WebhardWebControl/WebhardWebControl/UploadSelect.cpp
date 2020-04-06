#include "stdafx.h"
#include <comutil.h>
#include "UploadSelect.h"
#include "SelectFolderDialog.h"
//20080612 해시값 추출 클래스 추가, jyh
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
	CMD5 pMD5;	//20080612 해시값 추출 클래스, jyh
	CString	strMD5CheckSum;	//20080612 해시값 저장 변수, jyh
	DWORD dwErrorCode = NO_ERROR;

	pWindow.m_ofn.lpstrFile = pFileName;
	pWindow.m_ofn.nMaxFile = sizeof(pFileName);
	pWindow.m_ofn.lpstrInitialDir = m_pRegPath;
	pWindow.m_ofn.lpstrTitle = "업로드 하실 파일을 선택해주세요";

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
				//20080612 해시값 추출, jyh
				strMD5CheckSum = pMD5.GetString(mFind.GetFilePath(), dwErrorCode);
				//m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t(pSelectedSize), _bstr_t("-1"));
				m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t(pSelectedSize), _bstr_t("-1"), _bstr_t(strMD5CheckSum));
			}
			mFind.Close();	

			if(nSelectedCount == 0)
			{
				m_pRegPath.Format("%s", pSelectedPath);
				//20071217 레지스트리에 저작권 등록시 선택하는 파일 경로를 저장한다, jyh
				m_pReg.SaveKey(CLIENT_REG_PATH, "strCopyrightPath", pSelectedPath);
				::PathRemoveFileSpec((LPTSTR)(LPCTSTR)m_pRegPath);	
				m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", m_pRegPath);
			}

			nSelectedCount ++;
        }		
	}
}

//20071112 오픈 자료실 파일 열기 대화상자, jyh
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
	pWindow.m_ofn.lpstrTitle = "업로드 하실 파일을 선택해주세요";

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

				//파일 사이즈가 50MB 이상은 업로드 금지
				if(mFind.GetLength() > 52428800)
				{
					AfxMessageBox("50MB 이상은 업로드 할 수 없습니다!");
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
	CSelectFolderDialog pSelectDialog(FALSE, m_pRegPath, wFlag, NULL, NULL, "업로드 하실 폴더를 선택해주세요");
	if(pSelectDialog.DoModal() == IDOK)	
	{
		pSelectedPath = pSelectDialog.GetSelectedPath();
		//20090324 드라이브는 선택되지 않게 한다, jyh
		int nCnt = 0, nPos = 0;
		while((nPos = pSelectedPath.Find("\\", nPos)) > 0)
		{
			nPos++;
			nCnt++;
		}

		if(nCnt == 1)
		{
			AfxMessageBox("드라이브는 선택할 수 없습니다!");
			return;
		}

		m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", pSelectedPath);
		//20071217 레지스트리에 저작권 등록시 선택하는 파일 경로를 저장한다, jyh
		m_pReg.SaveKey(CLIENT_REG_PATH, "strCopyrightPath", pSelectedPath);

		if(pSelectedPath.Right(1) == '\\')
			pSelectedPath = pSelectedPath.Left(pSelectedPath.GetLength() -1);
		m_pMain->SelectComplete(_bstr_t(pSelectedPath), _bstr_t("-1"), _bstr_t("-1"), _bstr_t("-1"));
		FolderSubInfo(pSelectedPath, pSelectedPath);
	}
}

//20071112 오픈 자료실 폴더 열기 대화상자, jyh
void UploadSelect::ShowDlgOpenFolder()
{
	CString pSelectedPath;

	DWORD wFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	CSelectFolderDialog pSelectDialog(FALSE, m_pRegPath, wFlag, NULL, NULL, "업로드 하실 폴더를 선택해주세요");
	if(pSelectDialog.DoModal() == IDOK)	
	{
		pSelectedPath = pSelectDialog.GetSelectedPath();
		m_pReg.SaveKey(CLIENT_REG_PATH, "strWebPath", pSelectedPath);

		if(pSelectedPath.Right(1) == '\\')
			pSelectedPath = pSelectedPath.Left(pSelectedPath.GetLength() -1);

		ULONGLONG nFolderSize = 0;
		nFolderSize = pSelectDialog.GetFolderSize(pSelectedPath);

		//폴더 사이즈가 50MB 이상은 업로드 금지
		if(nFolderSize > 52428800)
		{
			AfxMessageBox("50MB 이상은 업로드 할 수 없습니다!");
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
	CMD5		pMD5;	//20080612 해시값 추출 클래스, jyh
	CString		strMD5CheckSum;	//20080612 해시값 저장 변수, jyh
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
			//20080612 해시값을 추출, jyh
			strMD5CheckSum = pMD5.GetString(mFind.GetFilePath(), dwErrorCode);
			pSize.Format("%I64d", mFind.GetLength());
			m_pMain->SelectComplete(_bstr_t(mFind.GetFilePath()), _bstr_t(pSize), _bstr_t(strRoot), _bstr_t(strMD5CheckSum));
        }
    }
	mFind.Close();	
}







