
#include "StdAfx.h"
#include "HelperObject.h"

CHelperObject::CHelperObject(void)
{
}

CHelperObject::~CHelperObject(void)
{
}

BOOL CHelperObject::IsVistaOS(void)
{
	BOOL bIsVista = FALSE;
	OSVERSIONINFO sInfo;
	sInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	// 
	// OS VERSION CHECK.
	//
	if(::GetVersionEx(&sInfo)) {
		if( sInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
			sInfo.dwMajorVersion >= 6 &&
			sInfo.dwMinorVersion >= 0) { // Windows VISTA
			bIsVista = TRUE;

		}
	}

	return bIsVista;
}


BOOL CHelperObject::GetActiveXPath(LPCSTR strCLSID, LPSTR strRetActiveXPatch)
{
	CRegKey reg;
	char	strKeyPath [ 256 ];

	ZeroMemory( strKeyPath, sizeof(char) * 256 );

	wsprintfA( strKeyPath, "CLSID\\%s\\InprocServer32", strCLSID );
	
	if(reg.Open(HKEY_CLASSES_ROOT, strKeyPath, KEY_QUERY_VALUE) 
		!= ERROR_SUCCESS) {
		return FALSE;
	}

	char*	pstrValue = new char[1024];
	DWORD	dwCount = 1023;
	int		nError;
	
	//reg.QueryValue(pstrValue, "", &dwCount);
	reg.QueryStringValue("", pstrValue, &dwCount);
	delete[] pstrValue;
	
	pstrValue = new char[dwCount+1];
	
	//nError = reg.QueryValue(pstrValue, "", &dwCount);
	nError = reg.QueryStringValue("", pstrValue, &dwCount);
	if(nError == ERROR_SUCCESS) {
		lstrcpy( strRetActiveXPatch, pstrValue );

		reg.Close();
	} else {
		delete[] pstrValue;
		reg.Close();
		return FALSE;
	}

	
	char* pstrFileName = strrchr( strRetActiveXPatch, '\\');

	if( pstrFileName ) {
		*pstrFileName = (char)'\0';
	}

	return TRUE;
}
