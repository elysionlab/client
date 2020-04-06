#include "stdafx.h"
#include "HtmlViewer.h"
#include <comdef.h>


IMPLEMENT_DYNCREATE(HtmlViewer, CHtmlView)
HtmlViewer::HtmlViewer() : CHtmlView()
{
	complete = false;	
}

HtmlViewer::~HtmlViewer()
{	
}

void HtmlViewer::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HtmlViewer, CHtmlView)
END_MESSAGE_MAP()



BOOL HtmlViewer::PreTranslateMessage(MSG* pMsg)
{
	return CHtmlView::PreTranslateMessage(pMsg);
}


BOOL HtmlViewer::Invalidate()
{
	CFormView::Invalidate();
	return true;
}


CString HtmlViewer::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);
	
	BSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);
	
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString HtmlViewer::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);
	
	BSTR bstr;
	m_pBrowserApp->get_Type(&bstr);
	
	CString retVal(bstr);
	
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString HtmlViewer::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);
	
	BSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);
	
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString HtmlViewer::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);
	
	BSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);
	
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

void HtmlViewer::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
														LPCTSTR lpszTargetFrameName /* = NULL */ ,
														LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
														DWORD dwPostDataLen /* = 0 */)
{
	CString strURL(lpszURL);
	BSTR bstrURL = strURL.AllocSysString();
	
	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);
		
		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}
	
	m_pBrowserApp->Navigate(bstrURL, COleVariant((long) dwFlags, VT_I4), COleVariant(lpszTargetFrameName, VT_BSTR), 
		vPostData, COleVariant(lpszHeaders, VT_BSTR));
	
	SysFreeString(bstrURL); // Added this line to prevent leak.
}


BOOL HtmlViewer::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	
	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];
	
	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;
	
	delete [] lpszModule;
	return bRetVal;
}

BOOL HtmlViewer::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	
	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];
	
	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;
	
	delete [] lpszModule;
	return bRetVal;
}


// 문서 로딩이 끝났음을 알림..
void HtmlViewer::OnDocumentComplete(LPCTSTR lpszURL) 
{
	
	m_pHTML = GetHtmlDocument();			// WebBrowser의 Document객체를 얻어온다..	
	CHtmlView::OnDocumentComplete(lpszURL);
	SetBGImage();
	complete = true;    	
}


// 전체 삭제
void HtmlViewer::DelAll()
{
	CComPtr<IHTMLElement> pBody;
	
	TRY
	{
		if(m_pHTML)
		{
			if(FAILED(m_pHTML->get_body(&pBody)))					return ;
					
			HRESULT h = pBody->put_innerHTML(L"");

			if(pBody)	ScrollEnd(pBody);
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH	
}


// 전체 다시 그리기
void HtmlViewer::ReSet()
{		
	CComPtr<IHTMLElement> pBody;
	CComQIPtr<IHTMLElement2, &IID_IHTMLElement2> pElement2;
	
	TRY
	{
		if(m_pHTML && complete)
		{	

			if(FAILED(m_pHTML->get_body(&pBody)))					return;
			
			if(FAILED(pBody->QueryInterface(&pElement2)))		    return;
			if(!pElement2)	return;
			
			pElement2->put_scrollTop(100);
			pElement2->put_scrollTop(0);			
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH	
}


// 한줄 추가하기..
bool HtmlViewer::AddHTML(LPCTSTR HTML)
{
	CComPtr<IHTMLElement> pBody;
	CString strHTML= HTML;
	
	TRY
	{
		if(m_pHTML)
		{
			if(FAILED(m_pHTML->get_body(&pBody)))	
				return false;

			if( !strHTML.IsEmpty() )
			{
				BSTR bstrHTML = strHTML.AllocSysString();
				//TRACE("1번줄 \r\n");
				BSTR bstrWhere = CString("beforeEnd").AllocSysString();
				//TRACE("2번줄 \r\n");				
				HRESULT h = pBody->insertAdjacentHTML(bstrWhere, bstrHTML);
				//TRACE("3번줄 \r\n");
				SysFreeString(bstrHTML);
				//TRACE("4번줄 \r\n");
				SysFreeString(bstrWhere);
				//TRACE("5번줄 \r\n");
			}

			if(pBody)	ScrollEnd(pBody);
			//TRACE("6번줄 \r\n");
		}
		
		//TRACE("7번줄 \r\n");
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH

	strHTML.Empty();
	return true;
}



bool HtmlViewer::ScrollEnd(CComPtr<IHTMLElement> pElement)
{
	CComQIPtr<IHTMLElement2, &IID_IHTMLElement2> pElement2;
	
	if(FAILED(pElement->QueryInterface(&pElement2)))		return false;

	if(!pElement)	return false;

	long nScrollHeight, nClientHeight;
	pElement2->get_scrollHeight(&nScrollHeight);
	pElement2->get_clientHeight(&nClientHeight);

	long nValue = nScrollHeight - nClientHeight;

	if(nValue > 0)	pElement2->put_scrollTop(nValue);

	return true;
}



bool HtmlViewer::SaveHistory(LPCTSTR file)
{	
	CComPtr<IHTMLElement> pBody;
	
	TRY
	{
		if(m_pHTML)
		{			
			if(FAILED(m_pHTML->get_body(&pBody)))					return false;
					
			BSTR bstrText;
			pBody->get_innerText(&bstrText);

			CString strText;
			CTime tNow = CTime::GetCurrentTime();

			strText += _T("Love Campus Chatting History..\r\n") + tNow.Format(_T("일시: %Y-%m-%d\r\n\r\n"));
			strText += bstrText;
			strText += _T("\r\n\r\nEnd History..");

			CFile f(file, CFile::modeCreate | CFile::modeWrite);

			// VC.NET 에선 WriteHuge가 Deprecate 되었더군요..
			//f.WriteHuge(strText.GetBuffer(strText.GetLength()), strText.GetLength());
			f.Write(strText.GetBuffer(strText.GetLength()), strText.GetLength());

			SysFreeString(bstrText);
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	AND_CATCH(CFileException, pEx)
	{
		pEx->ReportError();
	}
	END_CATCH

	return true;
}

CString HtmlViewer::GetHistory()
{
	CString strText;
	CComPtr<IHTMLElement> pBody;
	
	TRY
	{
		if(m_pHTML)
		{
			if(FAILED(m_pHTML->get_body(&pBody)))					return strText;

			BSTR bstrText;
			pBody->get_innerText(&bstrText);

			CTime tNow = CTime::GetCurrentTime();

			strText = _T("Love Campus Chatting History..\r\n") + tNow.Format(_T("일시: %Y-%m-%d\r\n\r\n"));
			strText += bstrText;
			strText += _T("\r\n\r\nEnd History..");

			SysFreeString(bstrText);
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH

	return strText;

}

bool HtmlViewer::SetBGImage()
{
	CComPtr<IHTMLElement> pElement;
	CComPtr<IHTMLBodyElement> pBody;

	TRY
	{
		if(m_pHTML)
		{			
			if(FAILED(m_pHTML->get_body((IHTMLElement**)&pElement)))	return false;
			if(FAILED(pElement->QueryInterface(&pBody)))				return false;
					
			BSTR bstrURL = CString(Bgimage).AllocSysString();
			pBody->put_background(bstrURL);

			SysFreeString(bstrURL);
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH

	return true;
}

bool HtmlViewer::OpenWindow(const char* szUrl, char* bScrollBar, UINT nWidth, UINT nHeight)
{
	HRESULT hr;

	TRY
	{
		m_pHTML = GetHtmlDocument();
		//CComPtr<IHTMLWindow2> pWindow2;
		//m_pHTML->get_parentWindow(&pWindow2);

		if(m_pHTML)
		{			
			CComPtr<IDispatch> spNewDisp;
			char pSet[1024];
			sprintf_s(pSet, 1024, "fullscreen=no,toolbar=no,location=no,directories=no,status=no,menubar=no,resizable=no,scrollbars=%s,width=%d,height=%d", bScrollBar, nWidth, nHeight);
			hr = m_pHTML->open(_bstr_t(szUrl), (_variant_t)_bstr_t("_blank"), (_variant_t)_bstr_t(pSet), (_variant_t)TRUE, &spNewDisp);
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH

	return true;
}

bool HtmlViewer::SetBGColor(LPCTSTR strColor)
{
	CComPtr<IHTMLElement> pElement;
	CComPtr<IHTMLBodyElement> pBody;

	TRY
	{
		m_pHTML = GetHtmlDocument();

		if(m_pHTML)
		{			
			if(FAILED(m_pHTML->get_body((IHTMLElement**)&pElement)))	return false;
			if(FAILED(pElement->QueryInterface(&pBody)))				return false;
					
			VARIANT vColor;
			vColor.vt = VT_BSTR;
			vColor.bstrVal = CString(strColor).AllocSysString();
			pBody->put_bgColor(vColor);
			SysFreeString(vColor.bstrVal);
		}
	}
	CATCH(CMemoryException, pEx)
	{
		AfxAbort();
	}
	END_CATCH

	return true;
}
