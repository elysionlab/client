// TrayIconPosition.h: interface for the CTrayIconPosition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAYICONPOSITION_H__9FD8A5A1_419F_11D7_B751_00304F20BD3B__INCLUDED_)
#define AFX_TRAYICONPOSITION_H__9FD8A5A1_419F_11D7_B751_00304F20BD3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTrayIconPosition  
{
public:
	typedef enum Precision { Default=0,Low,Medium,High };

	typedef enum TrackType { UseBothTechniquesDirectPrefered, 
							 UseBothTechniquesVisualScanPrefered,
							 UseDirectOnly,
							 UseVisualScanOnly};
	

	//Second tracking method: it detects a tray icon scaning tray area pixel by pixel
	void InitializePositionTracking(HWND a_hwndOfIconOwner, int a_iIconID);
	BOOL GetTrayIconPosition(CPoint& a_ptPoint, TrackType a_eTrackType = UseBothTechniquesDirectPrefered, Precision a_ePrec = Default);
	void RestoreTrayIcon(HICON a_hIcon);

	//Those functions are for use with second tracking method - will cache tracking results
	void Invalidate();
	void SetPrecisionTimeOuts(int iLowSec, int iMedSec, int iHighSec);
	void SetDefaultPrecision(Precision newPrecision) { m_prDefaultPrecision = newPrecision; };
	Precision GetDefaultPrecision(void) { return m_prDefaultPrecision; };

	CTrayIconPosition();
	virtual ~CTrayIconPosition();

protected:
	HICON		m_hIconTemp;
	CRect		m_rtRectangleOfTheTray;
	Precision	m_prDefaultPrecision;
	CTime		m_tLastUpdate;
	CPoint		m_ptPosition;
	int			m_iTrayIconID;
	HWND		m_hWndOfIconOwner;
	int			m_iPrecisions[3];  //This is table of time outs in seconds of low,med and high precision modes.
	OSVERSIONINFO m_osVer;

	//Tracking method: attaches to Tray process and reads data directly, is fast and reliable but will fail if user uses non standard tray software
	static BOOL FindOutPositionOfIconDirectly(const HWND a_hWndOwner, const int a_iButtonID, CRect& a_rcIcon);

	BOOL FindOutPositionOfIcon(HICON icon);
	inline BOOL CheckIfColorIsBlackOrNearBlack(COLORREF crColor);
	//BOOL GetTrayIconPositionVisualScan(CPoint& a_ptPoint, Precision a_ePrec);
	
	static CRect GetTrayWndRect();
	static HWND  GetTrayNotifyWnd(BOOL a_bSeekForEmbedToolbar);
	static BOOL CALLBACK FindTrayWnd(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK FindToolBarInTrayWnd(HWND hwnd, LPARAM lParam);
};

#endif // !defined(AFX_TRAYICONPOSITION_H__9FD8A5A1_419F_11D7_B751_00304F20BD3B__INCLUDED_)
