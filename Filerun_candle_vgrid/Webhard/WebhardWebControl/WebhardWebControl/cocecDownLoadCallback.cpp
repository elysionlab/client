#include "stdafx.h"
#include "cocecDownLoadCallback.h"
#include "codec_FileUpdateWindow.h"
#include <shlwapi.h>                  


CcodecCallback::CcodecCallback() : m_bUseTimeout(FALSE), m_pDlg(NULL)
{
}

CcodecCallback::~CcodecCallback()
{
}

HRESULT CcodecCallback::OnProgress ( ULONG ulProgress,   ULONG ulProgressMax,
                                ULONG ulStatusCode, LPCWSTR wszStatusText )
{
	

	static CString sIEStatusMsg;
	static TCHAR   szCustomStatusMsg [256];
	static TCHAR   szAmtDownloaded [256], szTotalSize [256];

    UNREFERENCED_PARAMETER(ulStatusCode);

/*
    if(0 != m_pDlg->g_fAbortDownload)
	{
        return E_ABORT;
	}
*/
	
    if(m_bUseTimeout && CTime::GetCurrentTime() > m_timeToStop)
        return E_ABORT;

    if(NULL != wszStatusText)
	{
		sIEStatusMsg = wszStatusText;
	}
    else
    {
		sIEStatusMsg.Empty();
	}
	

    StrFormatByteSize(ulProgress, szAmtDownloaded, 256);
    StrFormatByteSize(ulProgressMax, szTotalSize, 256);
    
    if(0 != ulProgressMax)
	{
		wsprintf(szCustomStatusMsg, _T("Downloaded.. %s of %s       "), szAmtDownloaded, szTotalSize);
	}
    else
    {
		wsprintf(szCustomStatusMsg, _T("Downloaded.. %s"), szAmtDownloaded);
	}


    if(0 != ulProgressMax)
		m_pDlg->DownState(szCustomStatusMsg, int(100.0 * ulProgress / ulProgressMax));
    else
        m_pDlg->DownState(szCustomStatusMsg, 0);

    return S_OK;
}
