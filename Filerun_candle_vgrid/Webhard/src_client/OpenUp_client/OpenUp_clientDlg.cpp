// OpenUp_clientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "OpenUp_client.h"
#include "OpenUp_clientDlg.h"
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenUp_clientDlg ��ȭ ����




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
	m_nCurTransFile		= -1;					//20071119 ���� �������� ���� �ε���, jyh
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


// COpenUp_clientDlg �޽��� ó����

BOOL COpenUp_clientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_strCommandLine = AfxGetApp()->m_lpCmdLine;

	if(!lstrcmp(m_strCommandLine, ""))
	{
		AfxMessageBox("�������� ������ �ƴմϴ�! ���α׷��� �����մϴ�.");
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

	m_staticTime.SetWindowText("00:00:00");			//���� �ð� ���


	CToken* pToken = new CToken("/");	
	pToken->Split(m_strCommandLine.GetBuffer());	
	m_strCommandLine.ReleaseBuffer();

	m_strUserID = pToken->GetToken(0);
	m_strUserPW = pToken->GetToken(1);
	//������ ����
	if(!lstrcmp(pToken->GetToken(2), "necessary"))
		m_nContentsType = 0;
	else if(!lstrcmp(pToken->GetToken(2), "caption"))
		m_nContentsType = 1;
	else if(!lstrcmp(pToken->GetToken(2), "contents_as"))
		m_nContentsType = 2;
	//������ �ε���
	m_n64ContentsIdx = _atoi64(pToken->GetToken(3));
	m_strUpPath = pToken->GetToken(4);

	// ���� ���� �۾� ����
	SetTimer(1, 10, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void COpenUp_clientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR COpenUp_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenUp_clientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(nIDEvent == 1)
	{
		KillTimer(1);
		lstrcpy(m_pWorker_Ctrl->m_pServerIp, SERVER_IP_OPEN);

		if(!m_pWorker_Ctrl->Init(m_pWorker_Ctrl->m_pServerIp, SERVER_PORT_OPEN))	
		{
			AfxMessageBox("Failed to connect!");
			OnBnClickedCancel();
		}

		// ������ �Ϸ�Ǹ� Ŭ���̾�Ʈ ��� ������ �뺸
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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


	// ��ü ���� üũ
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

	//��ü ���� ������ ���
	strBuf = SetSizeFormat(m_nFileSizeTotal);
	m_staticFileSize.SetWindowText(strBuf);
	m_staticFileSize.Invalidate();

	// ��ü ���� �ð� ����
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

	// ����ð� ����	
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

	// �������� ���� ����
	if(m_nCurTransFile != -1)
	{
		try
		{
			if(m_nTimeFileStart != 0)
				strSpeed = SetSpeedFormat(m_v_PtrTrans[m_nCurTransFile].nFileSizeEnd - m_v_PtrTrans[m_nCurTransFile].nFileSizeStart, m_nTime / 1000, m_nTimeFileStart / 1000);

			if(lstrcmp(m_strPrevFileName.GetBuffer(), m_v_PtrTrans[m_nCurTransFile].pFileName))
			{				
				//20070919 ���� ����, jyh
				m_strPrevFileName = m_v_PtrTrans[m_nCurTransFile].pFileName;
				m_staticFilename.SetWindowText(m_strPrevFileName);
				m_staticFilename.Invalidate();
			}

			m_strPrevFileName.ReleaseBuffer();
		}
		catch(...)
		{
		}			

		// ���� �ۼ�Ʈ ����
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
	// ���۽��۽ð� ����
	m_nTimeDownState = m_nTimeFileStart = GetTickCount();

	while(m_pFtpClint->m_bFlag)
	{
		ReSetTransInfo();
		Sleep(500);
	}

	Sleep(500);

	// �Ϸ�ð� ����
	m_nTimeTotal		+= m_nTimeFileTime;
	m_nTimeFileTime		= 0;
	m_nTimeFileStart	= 0;
	m_nTimeFileSpeed	= 0;
	m_nTransSpeed_Last	= 0;

	// ������� ������Ʈ
	ReSetTransInfo();

	// ���۰�� ó��
	if(m_pPtrTrans && !m_bForceExit)
	{
		if(m_pPtrTrans->nFileSizeReal == m_pPtrTrans->nFileSizeEnd)
			m_pPtrTrans->nTransState = ITEM_STATE_COMPLETE;
		else
			m_pPtrTrans->nTransState = ITEM_STATE_STOP;

		// ���� ���ۿϷ� �뺸
		char* strBuf = new char[32];
		sprintf_s(strBuf, 32, "%I64d", m_pPtrTrans->nFileIndex);
		
		Packet* pPacket = new Packet;
		pPacket->Pack(OPEN_UPEND, (char*)strBuf, lstrlen(strBuf)+1);
		m_pWorker_Ctrl->SendPacket(pPacket);
		delete [] strBuf;

		// �������� ������ �������� ����
		m_pPtrTrans	= NULL;		
	}

	// ���� ��ư�� Ŭ���� ���� �ƴϸ� ���� ����Ʈ�� �ִ��� üũ
	OpenUpLoadPro();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���α׷� ���� : ������ �ް� �ִ��� üũ �� ó�� ����� �Ѵ�
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
// ���� ������ ���� �ӵ� 
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

		// �����ð� ����� ���� ������ �ӵ�
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
// ���� ����� ��� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CString COpenUp_clientDlg::SetSizeFormat(const __int64 nRealSize)
{	
	char pTemp[256];
	::StrFormatByteSize64(nRealSize, pTemp, 256);	
	CString bSize = pTemp;
	bSize.Replace("����Ʈ", "B");	
	return bSize;
}


void COpenUp_clientDlg::OpenUpPre()
{		
	// �ٿ�ε� ������ ���� ������ä�� ��û		
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
				m_v_PtrTrans[i].nTransState == ITEM_STATE_DEFER/*20071119 �߰�, jyh*/ ||
				m_v_PtrTrans[i].nTransState == ITEM_STATE_CONNECT/*20080228 �߰�, jyh*/)
			{
				//20071119 ���� ���۵� ������ �ε��� ���� ��������� ����, jyh
				m_nCurTransFile = i;
				// �������� ������ ���� : ���������� ����
				m_pPtrTrans = (PROPENUPCTX)&m_v_PtrTrans[i];
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;

				// �ٿ�ε带 �����ϱ� ���� ���� �ٿ�ε� ���� ������ ���� üũ
				OpenUpPre();

				return;
			}
		}
	}

	// ���������� ����
	//SetTransFlag(false);
	// ���� ���� ���� ����
	ReSetTransInfo();
}

void COpenUp_clientDlg::SetTransFlag(bool bFlag)
{
	// ���������� ������
	if(bFlag) 
	{
		OpenUpLoadPro();
	}
	// ��������� ������
	else
	{
		m_pFtpClint->m_bFlag = false;

		//20070918 ���� ���� ������ ����, jyh
		if(m_pPtrTrans)
		{
			m_pPtrTrans->nTransState = ITEM_STATE_STOP;
		}
	}
}

