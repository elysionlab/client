// OpenUp_clientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "OpenUp_client.h"
#include "OpenUp_clientDlg.h"
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenUp_clientDlg 대화 상자




COpenUp_clientDlg::COpenUp_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenUp_clientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pWorker_Ctrl		= new Worker(this);
	m_pFtpClint			= new FtpClient();
	m_pFtpClint->m_pMainWindow = this;

	m_bAuth				= false;
	m_bForceExit		= false;
	m_pPtrTrans			= NULL;
	m_nServerIdx		= 0;
	m_nFileSizeTotal	= 0;
	m_nCurTransFile		= -1;					//20071119 현재 전송중인 파일 인덱스, jyh
	m_nTransSpeed		= 0;
	m_nTimeFileStart	= 0;
}

void COpenUp_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressbar);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_staticFilename);
	DDX_Control(pDX, IDC_STATIC_TIME, m_staticTime);
	DDX_Control(pDX, IDC_STATIC_FILESIZE, m_staticFileSize);
}

BEGIN_MESSAGE_MAP(COpenUp_clientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &COpenUp_clientDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COpenUp_clientDlg 메시지 처리기

BOOL COpenUp_clientDlg::OnInitDialog()
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
	m_strUpPath = pToken->GetToken(4);

	// 서버 연결 작업 진행
	SetTimer(1, 10, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COpenUp_clientDlg::OnPaint()
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
HCURSOR COpenUp_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenUp_clientDlg::OnTimer(UINT_PTR nIDEvent)
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
		ZeroMemory(&pLoginInfo, sizeof(INFOAUTH));

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
		OPENUPLIST OpenUpList;
		ZeroMemory(&OpenUpList, sizeof(OPENUPLIST));

		OpenUpList.nContentsType = m_nContentsType;
		OpenUpList.nContentsIdx = m_n64ContentsIdx;

		Packet* pPacket = new Packet;
		pPacket->Pack(OPEN_UPLIST, (char*)&OpenUpList, sizeof(OPENUPLIST));
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

void COpenUp_clientDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Exit_Program();
	OnCancel();
}

bool COpenUp_clientDlg::ReSetTransInfo()
{
	CString strBuf, strTime, strTimeOld, strSpeed;

	__int64 nFileSizeEnd = 0;
	__int64 nFileCountEnd = 0;
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
	COpenUp_clientDlg* pProcess = (COpenUp_clientDlg*)pParam;
	pProcess->FileTransLoop();
	return 0;
}


void COpenUp_clientDlg::FileTransLoop()
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
		char* strBuf = new char[32];
		sprintf_s(strBuf, 32, "%I64d", m_pPtrTrans->nFileIndex);
		
		Packet* pPacket = new Packet;
		pPacket->Pack(OPEN_UPEND, (char*)strBuf, lstrlen(strBuf)+1);
		m_pWorker_Ctrl->SendPacket(pPacket);
		delete [] strBuf;

		// 전송중인 파일이 없음으로 설정
		m_pPtrTrans	= NULL;		
	}

	// 중지 버튼을 클릭한 것이 아니면 다음 리스트가 있는지 체크
	OpenUpLoadPro();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 프로그램 종료 : 파일을 받고 있는지 체크 후 처리 해줘야 한다
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void COpenUp_clientDlg::Exit_Program()
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
CString COpenUp_clientDlg::SetSpeedFormat(__int64 transsize, DWORD nowsec, DWORD startsec)
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
CString COpenUp_clientDlg::SetSizeFormat(const __int64 nRealSize)
{	
	char pTemp[256];
	::StrFormatByteSize64(nRealSize, pTemp, 256);	
	CString bSize = pTemp;
	bSize.Replace("바이트", "B");	
	return bSize;
}


void COpenUp_clientDlg::OpenUpPre()
{		
	// 다운로드 가능한 서버 데이터채널 요청		
	OPENUPLOAD OpenUpInfo;
	OpenUpInfo.nFileIdx			= m_pPtrTrans->nFileIndex;
	OpenUpInfo.nFileSizeStart	= 0;
	OpenUpInfo.nFileSize		= 0;
	OpenUpInfo.nResult			= 0;
	ZeroMemory(OpenUpInfo.szServerInfo, sizeof(OpenUpInfo.szServerInfo));

	Packet* pPacket = new Packet;
	pPacket->Pack(OPEN_UPLOAD, (char*)&OpenUpInfo, sizeof(OPENUPLOAD));
	m_pWorker_Ctrl->SendPacket(pPacket);
}

void COpenUp_clientDlg::OpenUpLoadPro()
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
				m_pPtrTrans = (PROPENUPCTX)&m_v_PtrTrans[i];
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;

				// 다운로드를 시작하기 전에 현재 다운로드 받을 파일의 상태 체크
				OpenUpPre();

				return;
			}
		}
	}

	// 전송중지로 설정
	//SetTransFlag(false);
	// 전송 파일 갯수 변경
	ReSetTransInfo();
}

void COpenUp_clientDlg::SetTransFlag(bool bFlag)
{
	// 전송중으로 설정시
	if(bFlag) 
	{
		OpenUpLoadPro();
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

