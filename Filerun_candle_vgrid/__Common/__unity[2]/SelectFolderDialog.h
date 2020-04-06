/*
Usage Examples
--------------

CSelectFolderDialog oSelectFolderDialog(FALSE, NULL,
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						NULL, NULL);

CSelectFolderDialog oSelectFolderDialog(FALSE, "c:\\my documents",
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						NULL, NULL);

CSelectFolderDialog oSelectFolderDialog(TRUE, "c:\\my documents",
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						NULL, NULL);

CSelectFolderDialog oSelectFolderDialog(TRUE, "c:\\my documents",
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	"Microsoft Word Documents (*.doc)|*.doc|Microsoft Excel Worksheets (*.xls)|*.xls|", NULL);

CSelectFolderDialog oSelectFolderDialog(TRUE, "c:\\my documents",
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	"HTML Files (*.html, *.htm)|*.html;*.htm||", NULL);
*/

#include <vector>

#if !defined(AFX_MYFD_H__F9CB9441_F91B_11D1_8610_0040055C08D9__INCLUDED_)
#define AFX_MYFD_H__F9CB9441_F91B_11D1_8610_0040055C08D9__INCLUDED_

#pragma once



class CSelectFolderDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CSelectFolderDialog)

public:

	CSelectFolderDialog(	BOOL bShowFilesInDir = FALSE, 
							LPCSTR lpcstrInitialDir = NULL, 
							DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL,
							LPCTSTR lpszTitle = NULL);
	~CSelectFolderDialog();

	static WNDPROC m_wndProc;
	virtual void OnInitDone();

	void OnFolderChange();

	void SetSelectedPath(LPCSTR lpcstrPath);
	CString GetSelectedPath() const;

	void SetTitle(CString cstrTitle);
	ULONGLONG GetFolderSize(CString folderpath);	//20071129 폴더 용량 구하는 함수, jyh
		
protected:

	DECLARE_MESSAGE_MAP()


private:

	BOOL m_bShowFilesInDir;
	CString m_cstrPath;
	CEdit *m_pEdit;
	CString m_cstrTitle;
	ULONGLONG m_nSizeOfFolder;	//20071129 폴더 용량, jyh
};


#endif
