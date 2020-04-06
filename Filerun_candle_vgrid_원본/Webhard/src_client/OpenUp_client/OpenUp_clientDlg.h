// OpenUp_clientDlg.h : ��� ����
//

#pragma once
#include "stdafx.h"
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\client_openup\Worker.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\client_openup\FtpClient.h"
#include "d:\Project\Nfile����_vs8\__common\__unity[2]\skinprogress.h"


class FtpClient;
// COpenUp_clientDlg ��ȭ ����
class COpenUp_clientDlg : public CDialog
{
// �����Դϴ�.
public:
	COpenUp_clientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_OPENUP_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

public:
	Worker*			m_pWorker_Ctrl;
	FtpClient*		m_pFtpClint;
	PROPENUPCTX		m_pPtrTrans;

	CString			m_strUserID;			
	CString			m_strUserPW;
	CString			m_strCommandLine;
	CString			m_strFolderName;		//������ ������
	CString			m_strRootFolder;		//��Ʈ ����
	CString			m_strUpPath;			//���ε� Path
	CString			m_strPrevFileName;		//20070928 ���� �����̸��� �񱳸� ���� ���� �����̸� ����, jyh
	CString			m_strPrevPath;			

	bool			m_bAuth;				//���� ����
	bool			m_bForceExit;			

	DWORD			m_rUpTransBuffer;
	__int64			m_n64ContentsIdx;		//�������� �ε���
	int				m_nServerIdx;			//���� �ε���
	int				m_nContentsType;		//������ ����(0:�ʼ� �ڷ�, 1:�ڸ� �ڷ�, 2:A/S �ڷ�)
	int				m_nCurTransFile;		//���� �������� ���� �ε���

	__int64			m_nTransSpeed;
	__int64			m_nTransSpeed_Last;
	__int64			m_nFileSizeTotal;		//��ü ���� ������
	DWORD			m_nTime;				// üũ��� �ð�
	DWORD			m_nTimeTotal;			// ��ü �ҿ� �ð�(����)
	DWORD			m_nTimeFileTime;		// ���� ���� �ð�
	DWORD			m_nTimeFileStart;		// ���� ���� ���۽ð�
	DWORD			m_nTimeFileSpeed;		// ���� ���� �ӵ� ������Ʈ �ð�
	DWORD			m_nTimeDownState;		// �ٿ�ε� ���� ���� �ð�
	DWORD			m_nTimeTransButton;		// ���� ���� ��ư Ȱ��ȭ ����

	std::vector<ROPENUPCTX>	m_v_PtrTrans;	//���� ��� ����

public:
	bool ReSetTransInfo();
	void OpenUpPre();
	void OpenUpLoadPro();
	void SetTransFlag(bool bFlag);
	void Exit_Program();
	void FileTransLoop();
	friend unsigned int __stdcall ProcFileTransLoop(void *pParam);
	CString SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec);
	CString SetSizeFormat(const __int64 nRealSize);


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	SkinProgress m_progressbar;
	CStatic m_staticFilename;
	CStatic m_staticTime;
	CStatic m_staticFileSize;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
};
