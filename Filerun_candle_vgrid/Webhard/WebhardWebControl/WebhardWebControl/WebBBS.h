// WebBBS.h : CWebBBS�� �����Դϴ�.
#pragma once
#include "resource.h"       // �� ��ȣ�Դϴ�.
#include <atlctl.h>
#include "WebhardWebControl.h"
#include "_IWebBBSEvents_CP.h"
//20071112 �߰�, jyh
#include "config.h"
#include <afxinet.h>
#include <Urlmon.h>
#include "Helpers/HelperObject.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "���� ������ COM ��ü�� ��ü DCOM ������ �������� �ʴ� Windows Mobile �÷����� ���� Windows CE �÷������� ����� �������� �ʽ��ϴ�. ATL�� ���� ������ COM ��ü�� ������ �����ϰ� ���� ������ COM ��ü ������ ����� �� �ֵ��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA�� �����Ͻʽÿ�. rgs ������ ������ ���� DCOM Windows CE�� �ƴ� �÷������� �����Ǵ� ������ ������ ���̹Ƿ� 'Free'�� �����Ǿ� �ֽ��ϴ�."
#endif


// CWebBBS
class ATL_NO_VTABLE CWebBBS :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IWebBBS, &IID_IWebBBS, &LIBID_WebhardWebControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CWebBBS>,
	public IOleControlImpl<CWebBBS>,
	public IOleObjectImpl<CWebBBS>,
	public IOleInPlaceActiveObjectImpl<CWebBBS>,
	public IViewObjectExImpl<CWebBBS>,
	public IOleInPlaceObjectWindowlessImpl<CWebBBS>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CWebBBS>,
	public CProxy_IWebBBSEvents<CWebBBS>,
	public IPersistStorageImpl<CWebBBS>,
	public ISpecifyPropertyPagesImpl<CWebBBS>,
	public IQuickActivateImpl<CWebBBS>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CWebBBS>,
#endif
	public IProvideClassInfo2Impl<&CLSID_WebBBS, &__uuidof(_IWebBBSEvents), &LIBID_WebhardWebControlLib>,
#ifdef _WIN32_WCE // ��Ʈ���� ����� �ε��Ϸ��� Windows CE�� IObjectSafety�� �ʿ��մϴ�.
	public IObjectSafetyImpl<CWebBBS, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CWebBBS, &CLSID_WebBBS>,
	public CComControl<CWebBBS>,
	public IObjectSafetyImpl<CWebBBS, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>
{
public:


	CWebBBS()
	{
		char pPath[MAX_PATH];
		ZeroMemory(pPath, sizeof(pPath));

		SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);
		if(!lstrcmp(pPath, ""))		
			lstrcpy(pPath, "C:\\Program Files");

		m_pAppPath = pPath;
		m_pAppPath.Replace("/", "\\");
		m_pAppPath.Format("%s\\%s\\", pPath, CLIENT_APP_PATH);
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_WEBBBS)


BEGIN_COM_MAP(CWebBBS)
	COM_INTERFACE_ENTRY(IWebBBS)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
#ifdef _WIN32_WCE // ��Ʈ���� ����� �ε��Ϸ��� Windows CE�� IObjectSafety�� �ʿ��մϴ�.
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
	COM_INTERFACE_ENTRY(IObjectSafety)
#endif
END_COM_MAP()

BEGIN_CATEGORY_MAP(CWebBBS)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()

BEGIN_PROP_MAP(CWebBBS)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// ���� �׸�
	// PROP_ENTRY("�Ӽ� ����", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CWebBBS)
	CONNECTION_POINT_ENTRY(__uuidof(_IWebBBSEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CWebBBS)
	CHAIN_MSG_MAP(CComControl<CWebBBS>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// ó���� ������Ÿ��:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IWebBBS,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

public:
	CString			m_pAppPath;

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IWebBBS
public:
		HRESULT OnDraw(ATL_DRAWINFO& di)
		{
		RECT& rc = *(RECT*)di.prcBounds;
		// Ŭ�� ������ di.prcBounds�� ������ �簢������ �����մϴ�.
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

		if (hRgnNew != NULL)
		{
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
		}

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 8.0 : WebBBS");
#ifndef _WIN32_WCE
		TextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			pszText,
			lstrlen(pszText));
#else
		ExtTextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			ETO_OPAQUE,
			NULL,
			pszText,
			ATL::lstrlen(pszText),
			NULL);
#endif

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		return S_OK;
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	STDMETHOD(CheckInstall)(void);
	STDMETHOD(AllowPopUp)(BSTR strTemp);
	STDMETHOD(MakeShortCut)(BSTR strURL);

	STDMETHOD(ShowDlgFile)(void);
	STDMETHOD(ShowDlgFolder)(void);

	//20090304 �ؽð� �߰�, jyh
	//STDMETHOD(SelectComplete)(BSTR strPath, BSTR strSize, BSTR strMode);
	STDMETHOD(SelectComplete)(BSTR strPath, BSTR strSize, BSTR strMode, BSTR strMD5);

	STDMETHOD(UpLoad)(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx);
	//20071217 ���۱� ��� ������ �������� ���� ���� BSTR strHashIdx �߰�, jyh
	STDMETHOD(UpLoadHash)(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx, BSTR strHashIdx);
	//20080221 ���� ���� �ε��� ���� �߰�, jyh
	STDMETHOD(DownLoad)(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx, BSTR strIdx_, BSTR strSeverIdx, BSTR strNameAuth);
	//20071105 ���� �ڷ�� �Լ�, jyh
	STDMETHOD(OpenUpLoad)(BSTR file_name);		//20071112 ���� �ڷ�� ���ε� �Լ�, jyh
	STDMETHOD(OpenDownLoad)(BSTR file_name);	//20071112 ���� �ڷ�� �ٿ�ε� �Լ�, jyh
	STDMETHOD(ShowDlgOpenFile)(void);			//20071112 ���� �ڷ�� ���� ���� ��ȭ����, jyh
	STDMETHOD(ShowDlgOpenFolder)(void);			//20071130 ���� �ڷ�� ���� ���� ��ȭ����, jyh
	STDMETHOD(UpdateOpenCheck)(void);			//20071105 ���� �ڷ�� ��Ʈ�ѷ� ������Ʈ üũ, jyh
	STDMETHOD(VersionCheck)(void);
	STDMETHOD(FileVersionCheckVista)(void);		//20071205 ��Ÿ ���� ���, jyh

	HRESULT FileVersionCheck();
	HWND FindDownWindow();
	HWND FindUpWindow();
public:
	STDMETHOD(Movie_to_Images)(SHORT load_count, BSTR url_domain, BSTR url_path, BSTR* Save_files_data);
	STDMETHOD(MtoI_exe)(BSTR Movie_filename, SHORT load_count, BSTR url_domain, BSTR url_path);
	STDMETHOD(Movie_Capture)(BSTR Movie_filename, SHORT load_count, BSTR url_domain, BSTR url_path, SHORT Image_show_info, SHORT Image_show_size, BSTR* Save_files_data);
	STDMETHOD(DownLoads)(BSTR strUserID, BSTR strUserPW, BSTR strType, BSTR strIdx, BSTR strIdx_, BSTR strSeverIdx, BSTR strNameAuth, BSTR strFileAuth);
};

OBJECT_ENTRY_AUTO(__uuidof(WebBBS), CWebBBS)
