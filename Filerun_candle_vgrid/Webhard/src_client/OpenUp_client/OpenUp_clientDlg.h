// OpenUp_clientDlg.h : 헤더 파일
//

#pragma once
#include "stdafx.h"
#include <vector>
#include "afxwin.h"
#include "afxcmn.h"
#include "d:\Project\Nfile관련_vs8\__Common\__unity[2]\client_openup\Worker.h"
#include "d:\Project\Nfile관련_vs8\__Common\__unity[2]\client_openup\FtpClient.h"
#include "d:\Project\Nfile관련_vs8\__common\__unity[2]\skinprogress.h"


class FtpClient;
// COpenUp_clientDlg 대화 상자
class COpenUp_clientDlg : public CDialog
{
// 생성입니다.
public:
	COpenUp_clientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPENUP_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	Worker*			m_pWorker_Ctrl;
	FtpClient*		m_pFtpClint;
	PROPENUPCTX		m_pPtrTrans;

	CString			m_strUserID;			
	CString			m_strUserPW;
	CString			m_strCommandLine;
	CString			m_strFolderName;		//서버의 폴더명
	CString			m_strRootFolder;		//루트 폴더
	CString			m_strUpPath;			//업로드 Path
	CString			m_strPrevFileName;		//20070928 다음 파일이름과 비교를 위해 현재 파일이름 저장, jyh
	CString			m_strPrevPath;			

	bool			m_bAuth;				//인증 여부
	bool			m_bForceExit;			

	DWORD			m_rUpTransBuffer;
	__int64			m_n64ContentsIdx;		//컨텐츠의 인덱스
	int				m_nServerIdx;			//서버 인덱스
	int				m_nContentsType;		//컨텐츠 종류(0:필수 자료, 1:자막 자료, 2:A/S 자료)
	int				m_nCurTransFile;		//현재 전송중인 파일 인덱스

	__int64			m_nTransSpeed;
	__int64			m_nTransSpeed_Last;
	__int64			m_nFileSizeTotal;		//전체 파일 사이즈
	DWORD			m_nTime;				// 체크당시 시간
	DWORD			m_nTimeTotal;			// 전체 소요 시간(누적)
	DWORD			m_nTimeFileTime;		// 현재 파일 시간
	DWORD			m_nTimeFileStart;		// 현재 파일 시작시간
	DWORD			m_nTimeFileSpeed;		// 파일 전송 속도 업데이트 시간
	DWORD			m_nTimeDownState;		// 다운로드 상태 전송 시간
	DWORD			m_nTimeTransButton;		// 전송 관련 버튼 활성화 여부

	std::vector<ROPENUPCTX>	m_v_PtrTrans;	//파일 목록 저장

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


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
