// OpenDown_clientDlg.h : ��� ����
//

#pragma once
#include "stdafx.h"
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\client_opendown\Worker.h"
#include "d:\Project\Nfile����_vs8\__Common\__unity[2]\client_opendown\FtpClient.h"
#include "d:\Project\Nfile����_vs8\__common\__unity[2]\skinprogress.h"


class FtpClient;
// COpenDown_clientDlg ��ȭ ����
class COpenDown_clientDlg : public CDialog
{
// �����Դϴ�.
public:
	COpenDown_clientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_OPENDOWN_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

public:
	Worker*			m_pWorker_Ctrl;
	FtpClient*		m_pFtpClint;
	PROPENDOWNCTX	m_pPtrTrans;

	CString			m_strUserID;			
	CString			m_strUserPW;
	CString			m_strCommandLine;
	CString			m_strDownPath;			//���� ��ġ
	CString			m_strFolderName;		//������ ������
	CString			m_strRootFolder;		//��Ʈ ����
	CString			m_strPrevFileName;		//20070928 ���� �����̸��� �񱳸� ���� ���� �����̸� ����, jyh
	CString			m_strPrevPath;			

	bool			m_bAuth;				//���� ����
	bool			m_bForceExit;			

	DWORD			m_rDownTransBuffer;
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

	std::vector<ROPENDOWNCTX>	m_v_PtrTrans;	//���� ��� ����

public:
	bool ReSetTransInfo();
	__int64 GetDiskFreeSpaceMMSV(CString targetfolder);
	void OpenDownPre();
	void OpenDownLoadPro();
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	SkinProgress m_progressbar;
	CStatic m_staticTime;		//���� �ð� ��Ʈ��
	CStatic m_staticFilename;	//���ϸ� ��Ʈ��
	CStatic m_staticFileSize;	//���� ������ ��Ʈ��
	CEdit m_editDownPath;		//���� ��ġ ��Ʈ��
};
