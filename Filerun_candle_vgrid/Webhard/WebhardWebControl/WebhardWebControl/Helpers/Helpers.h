// Helpers.h : Declarations of object safety category helper functions

// ***REQUIRES** new Win32 SDK to build; available on MSDN Professional July 1996 and later

#include "comcat.h"

HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid);
