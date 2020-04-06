
#pragma once

#include <atlbase.h>
/*
#pragma pack(1)

	typedef struct _tag_PACKFILEINFO {
		char szFilename[MAX_PATH];
		DWORD dwPosition;
		DWORD dwSize;
	} PACKFILEINFO, *PPACKFILEINFO;

#pragma pack()
*/

class CHelperObject
{
public:
	CHelperObject(void);
public:
	~CHelperObject(void);
public:
	static BOOL IsVistaOS(void);
public:
	static BOOL GetActiveXPath(LPCSTR strCLSID, LPSTR strRetActiveXPatch);
};
