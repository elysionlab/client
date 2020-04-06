#include "stdafx.h"
#include "SelectFolderDialog.h"


#define IDLB_FOLDER_DIALOG_FILES		101
#define IDEB_FOLDER_DIALOG_SEL_FOLDER	102



IMPLEMENT_DYNAMIC(CSelectFolderDialog, CFileDialog)

WNDPROC CSelectFolderDialog::m_wndProc = NULL;


CSelectFolderDialog::CSelectFolderDialog(
						BOOL bShowFilesInDir /*= FALSE*/, 
						LPCSTR lpcstrInitialDir /*= NULL*/, 
						DWORD dwFlags /*= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/,
						LPCTSTR lpszFilter /*= NULL*/, 
						CWnd* pParentWnd /*= NULL*/,
						LPCTSTR lpszTitle)
					: CFileDialog(TRUE, NULL, 
					(bShowFilesInDir)? ((lpszFilter)? NULL : "*.*") : _T("*..*"), 
									dwFlags, lpszFilter, pParentWnd)
{
	if(lpcstrInitialDir)
		m_ofn.lpstrInitialDir = lpcstrInitialDir;
	
	m_bShowFilesInDir = bShowFilesInDir;

	m_pEdit = NULL;

	m_cstrTitle = lpszTitle;
	m_nSizeOfFolder = 0;	//20071129 폴더 용량 초기화, jyh
}

CSelectFolderDialog::~CSelectFolderDialog()
{
	if(m_pEdit)
		delete m_pEdit;
}

BEGIN_MESSAGE_MAP(CSelectFolderDialog, CFileDialog)
END_MESSAGE_MAP()


LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	CString cstrPath;
	CString strTempPath;
	CString strTempSelected;

	if(message ==  WM_COMMAND)
	{
		if (HIWORD(wParam) == BN_CLICKED)
		{
			if (LOWORD(wParam) == IDOK)
			{
				if (CFileDialog* pDlg = (CFileDialog*)CWnd::FromHandle(hwnd))
				{
					if(cstrPath.GetLength() == 0)
						((CSelectFolderDialog*)pDlg->GetDlgItem(IDEB_FOLDER_DIALOG_SEL_FOLDER))->GetWindowText(cstrPath);
					
					if(cstrPath.GetLength() == 0)
						cstrPath = ((CSelectFolderDialog*)pDlg->GetDlgItem(0))->GetFolderPath();

					// 선택된(selected) 폴더가 있을 경우 해당 폴더를 전체 경로에 추가
					CListCtrl* pListCtrl = (CListCtrl*)pDlg->GetDlgItem(lst2)->GetDlgItem(1);
					int nItemSelect = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
					if(nItemSelect != -1)
					{							
						strTempSelected = pListCtrl->GetItemText(nItemSelect, 0);						
						strTempPath =  cstrPath;

						if(strTempPath.Right(1) != '\\')
							strTempPath += '\\';
						strTempPath += strTempSelected;
                        
						if(_access(strTempPath, 00) == 0)
							cstrPath = strTempPath;
					}							
					
					int i = 0;

					for(i = 0; i < cstrPath.GetLength() - 1; i++)
					{
						if(cstrPath[i] == '\\' && cstrPath[i+1] == '\\')
						{
							::MessageBox(NULL, "업로드 가능한 폴더가 아닙니다", "알림", MB_OK | MB_ICONHAND);
							return NULL;
						}
					}
					
					if(cstrPath[i] == '\\')
						cstrPath = cstrPath.Left (cstrPath.GetLength() - 1);
					
					if(_access(cstrPath, 00) != 0)
					{
						::MessageBox(NULL, "업로드 가능한 폴더가 아닙니다", "알림", MB_OK | MB_ICONHAND);
						return NULL;
					}
					
					((CSelectFolderDialog*)pDlg->GetDlgItem(0))->SetSelectedPath(cstrPath + "\\");
					
					pDlg->EndDialog(IDOK);

					return NULL;
				}
			}
		}
	}

	return CallWindowProc(CSelectFolderDialog::m_wndProc, hwnd, message, wParam, lParam);
}


void CSelectFolderDialog::OnInitDone()
{
	BOOL bXPType = FALSE;	
	int	 topVal = 12;	
	CWnd* pFD = GetParent();	

	/////////////////////////////////	
	CWnd *wndTemp;	
	wndTemp = pFD->GetDlgItem(edt1);	

	if(!wndTemp)		
	{		
		bXPType = TRUE;		
		topVal = 4;		
	}
	/////////////////////////////////	
	
	CRect rectCancelPB; 		
	pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rectCancelPB);	
	pFD->ScreenToClient(rectCancelPB);	
	///////////////		
	
	CRect rectOKPB; 		
	pFD->GetDlgItem(IDOK)->GetWindowRect(rectOKPB);		
	pFD->ScreenToClient(rectOKPB);		
	pFD->GetDlgItem(IDOK)->SetWindowPos(0,rectCancelPB.left - rectOKPB.Width() - 4, rectCancelPB.top, 0,0, SWP_NOZORDER | SWP_NOSIZE);	
	///////////////	
	CRect rectList2LC; 		
	pFD->GetDlgItem(lst1)->GetWindowRect(rectList2LC);		
	pFD->ScreenToClient(rectList2LC);		
	pFD->GetDlgItem(lst1)->SetWindowPos(0,0,0,rectList2LC.Width(), abs(rectList2LC.top - (rectCancelPB.top - 45)) + 25, SWP_NOMOVE | SWP_NOZORDER);		
	SetControlText(IDOK, _T("선택"));		
	pFD->SetWindowText(_T(m_cstrTitle));
#pragma warning(disable:4311)
#pragma warning(disable:4312)
	m_wndProc = (WNDPROC)SetWindowLong(pFD->m_hWnd, GWL_WNDPROC, (LONG)WindowProcNew);	
#pragma warning(default:4312)
#pragma warning(default:4311)


	///////////////////////////////	
	CRect rectstc3ST; 		
	pFD->GetDlgItem(stc3)->GetWindowRect(rectstc3ST);	
	pFD->ScreenToClient(rectstc3ST);	
	pFD->GetDlgItem(stc3)->SetWindowText("선택된 폴더 경로");		
	pFD->GetDlgItem(stc3)->SetWindowPos(0, rectstc3ST.left, rectstc3ST.top + topVal, rectstc3ST.Width() + 20, rectstc3ST.Height(), SWP_NOZORDER);	
	///////////////	
	
	if (bXPType)		
	{		
		wndTemp = pFD->GetDlgItem(cmb13);		
		HideControl(cmb13);		
		topVal = 26;		
	}	
	else		
	{		
		HideControl(edt1);		
		topVal = 30;	
	}	
	
	CRect rectCurrFolderEB;	
	wndTemp->GetWindowRect(rectCurrFolderEB);	
	pFD->ScreenToClient(rectCurrFolderEB);	
	rectCurrFolderEB.left = rectstc3ST.left;	
	rectCurrFolderEB.right -= 70;	
	rectCurrFolderEB.top += topVal;	
	rectCurrFolderEB.bottom += topVal;	


	m_pEdit = new CEdit();	
	m_pEdit->Create(WS_TABSTOP | WS_VISIBLE | WS_CHILD, rectCurrFolderEB, pFD, IDEB_FOLDER_DIALOG_SEL_FOLDER);	
	m_pEdit->SetWindowText(m_ofn.lpstrInitialDir);	
	m_pEdit->SetFont(pFD->GetDlgItem(cmb2)->GetFont());	
	//m_pEdit->EnableWindow(FALSE);
	
	//Draw the thick sunkun edge	
	m_pEdit->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);		

	//Set the Tab order after the Folders Listbox	
	m_pEdit->SetWindowPos(pFD->GetDlgItem(lst1), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);	
	if(!m_bShowFilesInDir || (m_ofn.lpstrFilter == NULL) || (strlen(m_ofn.lpstrFilter) == 0))		
	{		
		//File Types Static text and Combo box		
		HideControl(stc2);		
		HideControl(cmb1);		
	}		
	
	if(m_bShowFilesInDir && (m_ofn.lpstrFilter != NULL) && (strlen(m_ofn.lpstrFilter) != 0))	
	{		
		//Increase the height of the dialog		
		CRect rectDialog;		
		pFD->GetWindowRect(rectDialog);		
		rectDialog.bottom += 35;		
		pFD->SetWindowPos(0, 0, 0, rectDialog.Width(), rectDialog.Height(), SWP_NOZORDER | SWP_NOMOVE);		

		CRect rectST;		
		pFD->GetDlgItem(stc2)->GetWindowRect(rectST);		
		pFD->ScreenToClient(rectST);		
		rectST.top += 25;		
		rectST.bottom += 25;		
		pFD->GetDlgItem(stc2)->SetWindowPos(0, rectST.left, rectST.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);	

		CRect rectComboDL;		
		pFD->GetDlgItem(cmb1)->GetWindowRect(rectComboDL);		
		pFD->ScreenToClient(rectComboDL);		
		rectComboDL.left = rectST.left;		
		rectComboDL.top = rectST.top + rectST.Height();		
		rectComboDL.bottom = rectST.bottom + rectST.Height();		
		pFD->GetDlgItem(cmb1)->SetWindowPos(0, rectComboDL.left, rectComboDL.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);		
		pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rectCancelPB);		
		pFD->ScreenToClient(rectCancelPB);		
		rectCancelPB.top = rectST.bottom - 2;		
		rectCancelPB.bottom = rectCancelPB.top + rectCancelPB.Height();		
		pFD->GetDlgItem(IDCANCEL)->SetWindowPos(0, rectCancelPB.left, rectCancelPB.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);		
		pFD->GetDlgItem(IDOK)->GetWindowRect(rectOKPB);		
		pFD->ScreenToClient(rectOKPB);		
		pFD->GetDlgItem(IDOK)->SetWindowPos(0,rectCancelPB.left - rectOKPB.Width() - 4, rectCancelPB.top, 0,0, SWP_NOZORDER | SWP_NOSIZE);		
		rectCurrFolderEB.right = rectCancelPB.right;		
		m_pEdit->SetWindowPos(pFD->GetDlgItem(lst1), 0, 0, rectCurrFolderEB.Width(), rectCurrFolderEB.Height(), SWP_NOMOVE | SWP_NOZORDER);		
	}	
	
	pFD->CenterWindow();
}

void CSelectFolderDialog::OnFolderChange()
{
	if(::IsWindow(GetParent()->m_hWnd))
	{
		m_pEdit->SetWindowText(GetFolderPath());
	}
}


void CSelectFolderDialog::SetSelectedPath(LPCSTR lpcstrPath)
{
	m_cstrPath = lpcstrPath;
}

CString CSelectFolderDialog::GetSelectedPath() const
{
	return m_cstrPath;
}

void CSelectFolderDialog::SetTitle(CString cstrTitle)
{
	m_cstrTitle = cstrTitle;
}

//20071129 폴더 용량 구하기, jyh
ULONGLONG CSelectFolderDialog::GetFolderSize(CString folderpath)
{
	CString		path;
	CFileFind	ff;
	bool bret = true;

	path.Format("%s\\*.*", folderpath);

	if( ff.FindFile(path) )
	{
		while(bret)
		{
#pragma warning(disable:4800)
			bret = ff.FindNextFile();
#pragma warning(default:4800)
			if(ff.IsDots())
			{
				//continue;
			}
			else if(!ff.IsHidden() && ff.IsDirectory())
			{
				CString fp =ff.GetFilePath(); 
				GetFolderSize(fp);
			}
			else
				m_nSizeOfFolder += ff.GetLength();   
		}
	}

	ff.Close();

	return m_nSizeOfFolder;
}