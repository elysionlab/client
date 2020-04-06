// OpenDown_clientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "OpenDown_client.h"
#include "OpenDown_clientDlg.h"
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenDown_clientDlg 대화 상자




COpenDown_clientDlg::COpenDown_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenDown_clientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pWorker_Ctrl		= new Worker(this);
	m_pFtpClint			= new FtpClient();
	m_pFtpClint->m_pMainWindow = this;

	m_bAuth			= false;
	m_bForceExit	= false;
	m_pPtrTrans		= NULL;
	m_nServerIdx	= 0;
	m_nFileSizeTotal = 0;
	m_nCurTransFile = -1;					//20071119 현재 전송중인 파일 인덱스, jyh
	m_nTransSpeed	= 0;
	m_nTimeFileStart = 0;
}

void COpenDown_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressbar);
	DDX_Control(pDX, IDC_STATIC_TIME, m_staticTime);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_staticFilename);
	DDX_Control(pDX, IDC_STATIC_FILESIZE, m_staticFileSize);
	DDX_Control(pDX, IDC_EDIT1, m_editDownPath);
}

BEGIN_MESSAGE_MAP(COpenDown_clientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &COpenDown_clientDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COpenDown_clientDlg 메시지 처리기

BOOL COpenDown_clientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_strCommandLine = AfxGetApp()->m_lpCmdLine;

	if(!lstrcmp(m_strCommandLine, ""))
	{
		AfxMessageBox("정상적인 실행이 아닙니다! 프로그램을 종료합니다.");
		EndDialog(IDCANCEL);
	}

	m_progressbar.SetStyle(PROGRESS_BITMAP);
	m_progressbar.SetBitmap(IDB_BMP_PROGRESS1, IDB_BMP_PROGRESS2);
	m_progressbar.SetRange(0, 100);
	m_progressbar.SetStep(1);
	m_progressbar.SetPos(0);
	m_progressbar.SetTextForeColor(RGB(27, 55, 104));
	m_progressbar.SetTextBkColor(RGB(255, 255, 255));
	m_progressbar.SetText("0%");

	m_staticTime.SetWindowText("00:00:00");			//남은 시간 출력

	BROWSEINFO bi;
	TCHAR szBuff[MAX_PATH] = {0};
	TCHAR szDisp[MAX_PATH] = {0};
	LPITEMIDLIST root;
	lstrcpy(szBuff, "폴더를 선택 하세요!");

	ZeroMemory(&bi, sizeof(BROWSEINFO));
	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_DESKTOP, &root);
	bi.hwndOwner = this->GetSafeHwnd();	//20070509 이렇게 함으로써 모달 대화상자가 됨, jyh
	bi.pidlRoot = root;
	bi.lpszTitle = szBuff;
	bi.ulFlags = BIF_USENEWUI;	//20070403 jyh

	LPITEMIDLIST pidl;

	pidl = SHBrowseForFolder(&bi);

	if(pidl == NULL)
	{
		EndDialog(IDCANCEL);
	}

	TCHAR szPath[MAX_PATH] = {0};

	SHGetPathFromIDList(pidl, szPath);
	m_strDownPath.Format("%s", szPath);
	m_editDownPath.SetWindowText(m_strDownPath);		//저장 위치 출력

	CToken* pToken = new CToken("/");	
	pToken->Split(m_strCommandLine.GetBuffer());	
	m_strCommandLine.ReleaseBuffer();

	m_strUserID = pToken->GetToken(0);
	m_strUserPW = pToken->GetToken(1);
	//컨텐츠 종류
	if(!lstrcmp(pToken->GetToken(2), "necessary"))
		m_nContentsType = 0;
	else if(!lstrcmp(pToken->GetToken(2), "caption"))
		m_nContentsType = 1;
	else if(!lstrcmp(pToken->GetToken(2), "contents_as"))
		m_nContentsType = 2;
	//컨텐츠 인덱스
	m_n64ContentsIdx = _atoi64(pToken->GetToken(3));

	// 서버 연결 작업 진행
	SetTimer(1, 10, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COpenDown_clientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COpenDown_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenDown_clientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nIDEvent == 1)
	{
		KillTimer(1);
		lstrcpy(m_pWorker_Ctrl->m_pServerIp, SERVER_IP_OPEN);

		if(!m_pWorker_Ctrl->Init(m_pWorker_Ctrl->m_pServerIp, SERVER_PORT_OPEN))	
		{
			AfxMessageBox("Failed to connect!");
			OnBnClickedCancel();
		}

		// 연결이 완료되면 클라이언트 살아 있음을 통보
		SetTimer(100, (CONNECT_UPDATE_TIME * 1000), NULL);

		INFOAUTH pLoginInfo;
		ZeroMemory(&pLoginInfo, sizeof(pLoginInfo));

		lstrcpy(pLoginInfo.pUserID, m_strUserID.GetBuffer());
		lstrcpy(pLoginInfo.pUserPW, m_strUserPW.GetBuffer());

		Packet* pPacket = new Packet;
		pPacket->Pack(CHK_AUTH, (char*)&pLoginInfo, sizeof(INFOAUTH));
		m_pWorker_Ctrl->SendPacket(pPacket);

		return;
	}
	else if(nIDEvent == 2)
	{		
		KillTimer(2);
		OPENDOWNLIST OpenDownList;
		ZeroMemory(&OpenDownList, sizeof(OPENDOWNLIST));

		OpenDownList.nContentsType = m_nContentsType;
		OpenDownList.nContentsIdx = m_n64ContentsIdx;

		Packet* pPacket = new Packet;
		pPacket->Pack(OPEN_DOWNLIST, (char*)&OpenDownList, sizeof(OPENDOWNLIST));
		m_pWorker_Ctrl->SendPacket(pPacket);

		return;
	}
	else if(nIDEvent == 100)
	{
		Packet* pPacket = new Packet;
		pPacket->Pack(USER_PTR_UPDATE);
		m_pWorker_Ctrl->SendPacket(pPacket);	
		return;
	}

	CDialog::OnTimer(nIDEvent);
}

void COpenDown_clientDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Exit_Program();
	OnCancel();
}

__int64 COpenDown_clientDlg::GetDiskFreeSpaceMMSV(CString targetfolder)
{
	ULARGE_INTEGER  dwSPC;
	ULARGE_INTEGER  dwBPS;
	ULARGE_INTEGER  dwFC;

	CString targetdisk;
	targetdisk = targetfolder.Left(targetfolder.Find("\\")+1);
	GetDiskFreeSpaceEx(targetdisk, &dwSPC, &dwBPS, &dwFC);
	return (__int64)dwFC.QuadPart;
}

void COpenDown_clientDlg::SetTransFlag(bool bFlag)
{
	// 전송중으로 설정시
	if(bFlag) 
	{
		OpenDownLoadPro();
	}
	// 전송종료로 설정시
	else
	{
		m_pFtpClint->m_bFlag = false;

		//20070918 파일 상태 중지로 변경, jyh
		if(m_pPtrTrans)
		{
			m_pPtrTrans->nTransState = ITEM_STATE_STOP;
		}
	}
}

void COpenDown_clientDlg::OpenDownPre()
{
	CString strBuf;
	__int64 nTrsStartSize = 0;

	// 로컬에 같은 이름의 파일이 존재하는지 체크
	CFileFind pFileFind;
	CString strFName = m_pPtrTrans->pFullPath;

	if(pFileFind.FindFile((LPCTSTR)strFName))
	{	
		pFileFind.FindNextFile();
		nTrsStartSize = (__int64)pFileFind.GetLength();

		if(nTrsStartSize == m_pPtrTrans->nFileSizeReal)
		{
			if(AfxMessageBox("같은 파일이 존재합니다. 덮어 쓰시겠습니까?", MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				nTrsStartSize = 0;
			}
			else
			{
				m_pPtrTrans->nTransState	= ITEM_STATE_COMPLETE;
				m_pPtrTrans->nFileSizeEnd = m_pPtrTrans->nFileSizeReal;
				nTrsStartSize = m_pPtrTrans->nFileSizeReal;
				//m_pPtrTrans = NULL;
			}
		}
		// 완료된 파일인지 체크
		else if(m_pPtrTrans->nFileSizeReal < nTrsStartSize)
		{
			if(AfxMessageBox("내려 받을 파일보다 크기가 큰 파일이 존재합니다. 덮어 쓰시겠습니까?", MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				nTrsStartSize = 0;
			}
			else
			{
				m_pPtrTrans->nTransState	= ITEM_STATE_COMPLETE;
				m_pPtrTrans->nFileSizeEnd = m_pPtrTrans->nFileSizeReal;
				nTrsStartSize = m_pPtrTrans->nFileSizeReal;
				//m_pPtrTrans = NULL;
			}
		}
	}

	pFileFind.Close();	

	// 다운로드를 시작하기 전에 타겟 디스크에 잔여 용량이 전송받을 용량보다 많은지 체크
	__int64 nFreeSize = GetDiskFreeSpaceMMSV((LPCTSTR)(LPSTR)m_pPtrTrans->pFullPath);

	if(nFreeSize <= (m_pPtrTrans->nFileSizeReal - nTrsStartSize))
	{
		SetTransFlag(false);

		CString strTargetPath = m_pPtrTrans->pFullPath;
		CString strTargetVolume = strTargetPath.Left(strTargetPath.Find("\\"));

		m_pPtrTrans	= NULL;	

		CString strCancelState;
		strCancelState.Format("로컬디스크의 용량이 부족합니다! 확인후 다시 시도해 주십시오.\r\n\r\n[%s] 드라이브의 남은 용량을 확인하세요!", strTargetVolume);
		AfxMessageBox(strCancelState);										
		OnBnClickedCancel();
	}	

	// 다운로드 가능한 서버 데이터채널 요청		
	OPENDOWNLOAD OpenDownInfo;
	OpenDownInfo.nFileIdx			= m_pPtrTrans->nFileIndex;
	OpenDownInfo.nDownStartPoint	= nTrsStartSize;
	OpenDownInfo.nFileSize			= 0;
	OpenDownInfo.nResult			= 0;
	ZeroMemory(OpenDownInfo.szServerInfo, sizeof(OpenDownInfo.szServerInfo));

	Packet* pPacket = new Packet;
	pPacket->Pack(OPEN_DOWNLOAD, (char*)&OpenDownInfo, sizeof(OPENDOWNLOAD));
	m_pWorker_Ctrl->SendPacket(pPacket);
}

void COpenDown_clientDlg::OpenDownLoadPro()
{
	ACE_UINT64		nItemFileSize = 0;
	int				nItemCheck = 0;

	if(!m_bForceExit && m_pPtrTrans == NULL)
	{
		for(int i=0; i<(int)m_v_PtrTrans.size(); i++)
		{	
			if(m_v_PtrTrans[i].nTransState == ITEM_STATE_COMPLETE)
			{
				continue;
			}

			if(	m_v_PtrTrans[i].nTransState == ITEM_STATE_WAIT ||
				m_v_PtrTrans[i].nTransState == ITEM_STATE_TRANS ||
				m_v_PtrTrans[i].nTransState == ITEM_STATE_STOP ||
				m_v_PtrTrans[i].nTransState == ITEM_STATE_DEFER/*20071119 추가, jyh*/ ||
				m_v_PtrTrans[i].nTransState == ITEM_STATE_CONNECT/*20080228 추가, jyh*/)
			{
				//20071119 현재 전송될 파일의 인덱스 값을 멤버변수로 저장, jyh
				m_nCurTransFile = i;
				// 전송중인 아이템 설정 : 전송중으로 설정
				m_pPtrTrans = (PROPENDOWNCTX)&m_v_PtrTrans[i];
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;

				// 다운로드를 시작하기 전에 현재 다운로드 받을 파일의 상태 체크
				OpenDownPre();
				
				return;
			}
		}
	}

	// 전송중지로 설정
	//SetTransFlag(false);
	// 전송 파일 갯수 변경
	ReSetTransInfo();
}

bool COpenDown_clientDlg::ReSetTransInfo()
{
	CString strBuf, strTime, strTimeOld, strSpeed;

	__int64 nFileSizeEnd = 0;
	__int64	nFileCountEnd = 0;

	int nTimeSec, nTimeMin, nTimeHour;

	if(m_bForceExit)
		return false;


	// 전체 파일 체크
	for(int i=0; i<(int)m_v_PtrTrans.size(); i++)
	{
		if(	m_v_PtrTrans[i].nTransState == ITEM_STATE_WAIT ||
			m_v_PtrTrans[i].nTransState == ITEM_STATE_TRANS ||
			m_v_PtrTrans[i].nTransState == ITEM_STATE_STOP ||
			m_v_PtrTrans[i].nTransState == ITEM_STATE_COMPLETE)
		{
			nFileSizeEnd += m_v_PtrTrans[i].nFileSizeEnd;
		}

		if(m_v_PtrTrans[i].nTransState == ITEM_STATE_COMPLETE)
			nFileCountEnd++;		
	}

	m_nTime = GetTickCount();

	//전체 파일 사이즈 출력
	strBuf = SetSizeFormat(m_nFileSizeTotal);
	m_staticFileSize.SetWindowText(strBuf);
	m_staticFileSize.Invalidate();

	// 전체 남은 시간 설정
	if(m_nTransSpeed > 0)
	{
		__int64 nTimeOver = (__int64)(((__int64)(m_nFileSizeTotal - nFileSizeEnd)) / m_nTransSpeed);
		nTimeSec	= (int)((nTimeOver) % 60);
		nTimeMin	= (int)(((nTimeOver) / 60) % 60);
		nTimeHour	= (int)(((nTimeOver) / 3600));		

		strBuf.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);
		m_staticTime.SetWindowText(strBuf);
		m_staticTime.Invalidate();
	}		

	// 경과시간 설정	
	//m_pStatic_Time_Use.GetWindowText(strTimeOld);	

	if(m_nTimeFileStart != 0)
		m_nTimeFileTime = ((m_nTime - m_nTimeFileStart) / 1000);

	if(m_nTimeFileTime != 0)
	{
		nTimeSec	= (m_nTimeTotal + m_nTimeFileTime) % 60;
		nTimeMin	= ((m_nTimeTotal + m_nTimeFileTime) / 60) % 60;
		nTimeHour	= ((m_nTimeTotal + m_nTimeFileTime) / 3600);		
		strTime.Format("%02d:%02d:%02d", nTimeHour, nTimeMin, nTimeSec);		
		//m_pStatic_Time_Use.SetWindowText(strTime);
	}
	else
	{
		strTime = strTimeOld;
		//m_pStatic_Time_Use.SetWindowText(strTime);
	}

	// 전송중인 파일 설정
	if(m_nCurTransFile != -1)
	{
		try
		{
			if(m_nTimeFileStart != 0)
				strSpeed = SetSpeedFormat(m_v_PtrTrans[m_nCurTransFile].nFileSizeEnd - m_v_PtrTrans[m_nCurTransFile].nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);

			if(lstrcmp(m_strPrevFileName.GetBuffer(), m_v_PtrTrans[m_nCurTransFile].pFileName))
			{
				//20070919 전송 파일, jyh
				m_strPrevFileName = m_v_PtrTrans[m_nCurTransFile].pFileName;
				m_staticFilename.SetWindowText(m_strPrevFileName);
				m_staticFilename.Invalidate();
			}

			m_strPrevFileName.ReleaseBuffer();

			//20070919 저장 경로, jyh
			if(lstrcmp(m_strPrevPath.GetBuffer(), m_v_PtrTrans[m_nCurTransFile].pFolderName))
			{
				char sPath[4096];

				lstrcpy(sPath, m_v_PtrTrans[m_nCurTransFile].pFolderName);
				m_strPrevPath = m_v_PtrTrans[m_nCurTransFile].pFolderName;

				int nIdx = (int)strlen(sPath);
				sPath[nIdx-1] = '\0';
				m_editDownPath.SetWindowText(sPath);	//20070919 저장 경로, jyh
				m_editDownPath.Invalidate();
			}

			m_strPrevPath.ReleaseBuffer();
		}
		catch(...)
		{
		}			

		// 진행 퍼센트 정보
		if(nFileSizeEnd == 0)
		{
			m_progressbar.SetPos(0);
		}
		else
		{
			int nPercent = (int)((nFileSizeEnd * 100) / m_nFileSizeTotal);		
			m_progressbar.SetPos(nPercent);
			strBuf.Format("%d%%", nPercent);
			m_progressbar.SetText(strBuf);
			m_progressbar.Invalidate();
		}
	}

	if(nFileCountEnd == (int)m_v_PtrTrans.size())
		OnBnClickedCancel();

	return true;
}

unsigned int __stdcall ProcFileTransLoop(void *pParam)
{
	COpenDown_clientDlg* pProcess = (COpenDown_clientDlg*)pParam;
	pProcess->FileTransLoop();
	return 0;
}


void COpenDown_clientDlg::FileTransLoop()
{
	// 전송시작시간 설정
	m_nTimeDownState = m_nTimeFileStart = GetTickCount();

	while(m_pFtpClint->m_bFlag)
	{
		ReSetTransInfo();
		Sleep(500);
	}

	Sleep(500);

	// 완료시간 설정
	m_nTimeTotal		+= m_nTimeFileTime;
	m_nTimeFileTime		= 0;
	m_nTimeFileStart	= 0;
	m_nTimeFileSpeed	= 0;
	m_nTransSpeed_Last	= 0;

	// 최종결과 업데이트
	ReSetTransInfo();

	// 전송결과 처리
	if(m_pPtrTrans && !m_bForceExit)
	{
		if(m_pPtrTrans->nFileSizeReal == m_pPtrTrans->nFileSizeEnd)
			m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;
		else
			m_pPtrTrans->nTransState = ITEM_STATE_STOP;

		// 파일 전송완료 통보
		char strBuf[32];

		Packet* pPacket = new Packet;
		pPacket->Pack(OPEN_DOWNEND, (char*)strBuf, 32);
		m_pWorker_Ctrl->SendPacket(pPacket);

		// 전송중인 파일이 없음으로 설정
		m_pPtrTrans	= NULL;		
	}

	// 중지 버튼을 클릭한 것이 아니면 다음 리스트가 있는지 체크
	OpenDownLoadPro();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 프로그램 종료 : 파일을 받고 있는지 체크 후 처리 해줘야 한다
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void COpenDown_clientDlg::Exit_Program()
{
	m_bForceExit	= true;
	SetTransFlag(false);
	Sleep(500);

	m_pWorker_Ctrl->End();
	SAFE_DELETE(m_pFtpClint);
	SAFE_DELETE(m_pWorker_Ctrl);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 현재 파일의 전송 속도 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString COpenDown_clientDlg::SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec)
{
	// 10Mbps=10000kbps=1250kB/s
	// 1Mbps=1,000kbps=128,000Byte/s

	CString bSize;
	__int64 size = transsize;
	__int64	time;
	float bonus = (float)1.1;

	if(startsec != 0)
	{
		time = nowsec - startsec;
		if(time < 1) time = 1;

		size = (__int64)(size * bonus / time);

		// 남은시간 계산을 위한 마지막 속도
		m_nTransSpeed = size;
	}
	else
	{
		size = m_nTransSpeed;
	}	

	bSize.Format("%s/sec", SetSizeFormat(size));	
	return bSize;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 파일 사이즈별 출력 포멧
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString COpenDown_clientDlg::SetSizeFormat(const __int64 nRealSize)
{	
	char pTemp[256];
	::StrFormatByteSize64(nRealSize, pTemp, 256);	
	CString bSize = pTemp;
	bSize.Replace("바이트", "B");	
	return bSize;
}