#if !defined(AFX_CHATVIEW_H__38DB8F8D_3F65_4448_911D_7D039097F441__INCLUDED_)
#define AFX_CHATVIEW_H__38DB8F8D_3F65_4448_911D_7D039097F441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <atlbase.h>
#include <Mshtml.h>
#include <afxhtml.h>

class HtmlViewer : public CHtmlView
{

public:

	HtmlViewer(); 
	virtual ~HtmlViewer();
	DECLARE_DYNCREATE(HtmlViewer)

public:

	CString			Bgimage;
	bool			complete;

public:
	
	bool AddHTML(LPCTSTR strHTML);
	void ReSet();
	void DelAll();	
	bool SaveHistory(LPCTSTR file);	
	bool SetBGImage();
	//bool SaveHistory(LPCTSTR strID, LPCTSTR file);
	//bool AddHTML(LPCTSTR strID, LPCTSTR strHTML);
	//bool AddHTML(CString &strID, CString &strHTML);
	//CString GetHistory(LPCTSTR strID);
	CString GetHistory();

	bool SetBGColor(LPCTSTR strColor);
	bool OpenWindow(const char* szUrl, char* bScrollBar, UINT nWidth, UINT nHeight);													
		
	CString GetFullName() const;
	CString GetType() const;
	CString GetLocationName() const;
	CString GetLocationURL() const;
	
	void Navigate(LPCTSTR lpszURL, DWORD dwFlags = 0 ,  
		LPCTSTR lpszTargetFrameName = NULL ,  
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,  
		DWORD dwPostDataLen = 0);
	
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);
	BOOL Invalidate();
	
protected:
	
	CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> m_pHTML;
	bool ScrollEnd(CComPtr<IHTMLElement> pElement);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG * pMsg);	

	DECLARE_MESSAGE_MAP()
};


#endif
