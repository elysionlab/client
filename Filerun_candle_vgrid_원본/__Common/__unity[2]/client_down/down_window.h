

#ifndef _DOWNWINDOW_H
#define _DOWNWINDOW_H

#include <vector>
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "registry.h"
#include "Worker.h"
#include "SkinDialog.h"
#include "..\src_client\down_client\SkinListCtrl.h"
#include "SelectFolderDialog.h"
#include "BTray.h"
#include "Dlg_Alert.h"
#include "HtmlViewer.h"
#include "..\..\..\__common\__unity[2]\skinprogress.h"
#include "..\src_client\down_client\pictureex.h"

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

class FtpClient;
class down_window : public CSkinDialog
{


public:

	down_window(CWnd* pParent = NULL);
	enum { IDD = IDD_DOWN_CLIENT_DIALOG };


protected:

	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);		
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
	void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	afx_msg void OnTrayMenuOpen();	//����޴� ���ý�
	afx_msg void OnTrayMenuHome();	//Ȩ�������޴� ���ý�
	afx_msg void OnTrayMenuExit();	//����޴� ���ý�
	LRESULT OnTrayIconNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()


public:

	Worker*			m_pWorker_Ctrl;	
	FtpClient*		m_pFtpClint;
	CRegistry		m_pReg;
	CSkinListCtrl	m_pListCtrl;
	CImageList		m_pImageList;
	CImageList		m_pStateImageList;		//20080319 �ٿ� ���� �̹���(���, ����), jyh
	PRDOWNCTX		m_pPtrTrans;
	//SkinProgress	m_pCtrl_ProgressTotal;
	CRITICAL_SECTION m_cs;		
	CBTray*			m_pTray;
	Dlg_Alert*		m_pAlert;	

	HtmlViewer*		m_pBanner;
	HtmlViewer*		m_pBtnHelp;				//20071004	�����ذ� ����, jyh
	
	HtmlViewer*		m_pSendiMBCComplete1;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendiMBCComplete2;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendiMBCComplete3;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendiMBCComplete4;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh
	HtmlViewer*		m_pSendiMBCComplete5;		//20090304 ������ ���ε� �Ϸ� �뺸, jyh

	//CLabel			m_pStatic_Time_Use;		//20070918 jyh
	CLabel			m_pStatic_Time_Over;
	//CLabel			m_pStatic_FileCount;		//20070918 jyh
	CLabel			m_pStatic_Speed;			//20070919 ���� �ӵ�, jyh
	CLabel			m_pStatic_Path;				//20070919 ���� ���, jyh
	CLabel			m_pStatic_FileName;			//20070919 ���� ����, jyh


public:

	CString			m_pCommandLine;
	CString			m_pUserID;
	CString			m_pUserPW;
	CString			m_strPrevPath;		//20070919 ���� ���۰�ο� �񱳸� ���� ���� ��� ����, jyh
	CString			m_strPrevFileName;	//20070928 ���� �����̸��� �񱳸� ���� ���� �����̸� ����, jyh

	bool			m_bInitPreItem;
	bool			m_bInitWindow;

	__int64			m_nTransSpeed;
	__int64			m_nTransSpeed_Last;
	DWORD			m_nTime;			// üũ��� �ð�
	DWORD			m_nTimeTotal;		// ��ü �ҿ� �ð�(����)
	DWORD			m_nTimeFileTime;	// ���� ���� �ð�
	DWORD			m_nTimeFileStart;	// ���� ���� ���۽ð�
	DWORD			m_nTimeFileSpeed;	// ���� ���� �ӵ� ������Ʈ �ð�
	DWORD			m_nTimeDownState;	// �ٿ�ε� ���� ���� �ð�
	DWORD			m_nTimeTransButton;	// ���� ���� ��ư Ȱ��ȭ ����

	__int64			m_nFileCount;
	__int64			m_nFileSizeTotal;
	__int64			m_nFileSizeEnd;		//���� ���� ������ ����

	bool			m_bForceExit;
	bool			m_bFileTrans;
	bool			m_bAuth;

	CString			m_rPathBasic;
	CString			m_rPathLast;
	CString			m_rPathGomPlayer;
	DWORD			m_rDownOverWrite;	
	DWORD			m_rUpOverWrite;	
	DWORD			m_rUpEndAutoClose;
	DWORD			m_rDownEndAutoClose;
	DWORD			m_rUpAutoTrans;
	DWORD			m_rDownAutoTrans;
	DWORD			m_rDownAddItemPath;
	DWORD			m_rUpTransSpeed;
	DWORD			m_rDownTransSpeed;
	DWORD			m_rUpExitCheck;
	DWORD			m_rDownExitCheck;
	DWORD			m_rDownTransBuffer;
	DWORD			m_rUpTransBuffer;
	DWORD			m_rCreateShortcut;	//20071002 �ٷΰ���, jyh

	
	COLORREF		m_cProBar_BK;
	std::vector<CString>	m_v_sItem;	//20071117 ���� ���� ������ ��ȣ, jyh
	int				m_iItemCnt;			//20071117 ���� ��û ���� ������ ��, jyh
	int				m_nBtnState;		//20071119 ��ư ����, jyh
	int				m_nCurTransFile;	//20071119 ���� �������� ���� �ε���, jyh
	BOOL			m_bMinWindow;		//20080131 UI�� �ּ�â���� ����, jyh
	int				m_nPercent;			//20080225 ��ü ������ ���� �ۼ�Ʈ, jyh
	int				m_nNameAuth;		//20080313 �Ǹ� ���� ����(0: ���� �ȵ�, 1: ����), jyh
	std::vector<CONITEM> m_v_sConItem;	//20080311 �������� ������ ���� ����, jyh
	BOOL			m_bFreeDown;		//20080320 ���� �������� ������ �ٿ� ����, jyh
	int				m_nSpeed;			//20081022 ���� ���� ���� ���� ���ǵ�, jyh
	
	//�·�ī
	int Mureka_use;
	ACE_UINT16		m_nSvrResult;
	double          m_nGridSpeed;
	__int64          m_nGridSpeed_mem;

public:

	afx_msg LRESULT MSG_Window(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_DownLoad(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_OverWrite(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MSG_ForceExit(WPARAM wParam, LPARAM lParam);



public:

	virtual void InitWindow();
	virtual void InitListControl();
	virtual void ChnPosition(int cx, int cy);
	virtual HBRUSH DrawCtrl(CDC* pDC, UINT nCtrlID);
	virtual bool Skin_ButtonPressed(CString m_ButtonName);

	void InsertItem(PRDOWNCTX pNewItem);
	void MoveItem(int mode);
	void DeleteItem(int mode);
	void DeleteList(int nMode);
	void Exit_Program(bool bRegWrite, bool bTrayCall);		

public:
		
	void DownLoadPro();
	afx_msg void OnCbnSelchangeComboSpeed();
	
	bool ReSetTransInfo();
	void SetTransFlag(bool bFlag);	
	int  GetTransListIndex();		//�������� ������ �ε���
	DWORD DeferItem();
	void SystemShutdownOTN();

	void FileTransLoop();
	friend unsigned int __stdcall ProcFileTransLoop(void *pParam);

	//121026 - IE������ ������ �ٿ�ε� - Ư��������ó��(URL�ٿ�ε�)
	void Download_List_update();
	CString Download_List_Items;
	bool Download_type_link;

public:	

	CString SetSizeFormat(__int64 nRealSize);
	CString SetNumFormat(__int64 nNum);
	CString SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec);
	CString SetSpeedFormatKGrid(double tranSpeed,DWORD nowsec, DWORD startsec);
	__int64 GetDiskFreeSpaceMMSV(CString targetfolder);
	
	void RegWrite();
	void RegRead();
	bool CreateShortcut(char* strTemp);	//20071002 �ٷΰ���, jyh
	CString GetFileStatus(int nIdx);	//20071030 ���� ���� ��ȯ, jyh
	//void DrawDownState();				//20080320 �ٿ�ε� ���� �̹��� ���, jyh
	bool IsVistaOS();					//20080426 OS�� ��Ÿ����, jyh
	BOOL AddMessageFilter(UINT* pMessages);	//20080426 ��Ÿ �޼��� ����, jyh
	BOOL SafeTerminateProcess(HANDLE hProcess, UINT uExitCode);	//20090528 ���μ��� �����ϰ� ��������, jyh
	
	SkinProgress m_pCtrl_ProgressCur;
	SkinProgress m_pCtrl_ProgressTotal;
	//CPictureEx m_Picture;				//20080322 gif �ִϸ��̼�, jyh
	afx_msg void OnPaint();
};


#endif /* _DOWNWINDOW_H */