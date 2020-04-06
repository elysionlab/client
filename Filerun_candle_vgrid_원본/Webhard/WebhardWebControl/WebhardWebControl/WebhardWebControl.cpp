// WebhardWebControl.cpp : DLL ���������� �����Դϴ�.


#include "stdafx.h"
#include "resource.h"
#include "WebhardWebControl.h"

//�߰�, jyh
#include "Helpers/Helpers.h"

CString			m_pUserID;
CString			m_pUserPW;

class CWebhardWebControlModule : public CAtlDllModuleT< CWebhardWebControlModule >
{
public :
	DECLARE_LIBID(LIBID_WebhardWebControlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WEBHARDWEBCONTROL, "{B324E3E1-F057-4813-AB06-7C4DD2FC66A1}")
};

CWebhardWebControlModule _AtlModule;

class CWebhardWebControlApp : public CWinApp
{
public:

// �������Դϴ�.
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CWebhardWebControlApp, CWinApp)
END_MESSAGE_MAP()

CWebhardWebControlApp theApp;

BOOL CWebhardWebControlApp::InitInstance()
{
	m_pUserID = "";
	m_pUserPW = "";

    return CWinApp::InitInstance();
}

int CWebhardWebControlApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}


// DLL�� OLE�� ���� ��ε�� �� �ִ��� �����ϴ� �� ���˴ϴ�.
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}


// Ŭ���� ���͸��� ��ȯ�Ͽ� ��û�� ������ ��ü�� ����ϴ�.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - �ý��� ������Ʈ���� �׸��� �߰��մϴ�.
STDAPI DllRegisterServer(void)
{
    // ��ü, ���� ���̺귯�� �� ���� ���̺귯���� ��� �ִ� ��� �������̽��� ����մϴ�.
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - �ý��� ������Ʈ������ �׸��� �����մϴ�.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

