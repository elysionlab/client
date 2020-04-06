// OpenDown_clientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "OpenDown_client.h"
#include "OpenDown_clientDlg.h"
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenDown_clientDlg ��ȭ ����




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
	m_nCurTransFile = -1;					//20071119 ���� �������� ���� �ε���, jyh
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


// COpenDown_clientDlg �޽��� ó����

BOOL COpenDown_clientDlg::OnInitDialog()
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

	BROWSEINFO bi;
	TCHAR szBuff[MAX_PATH] = {0};
	TCHAR szDisp[MAX_PATH] = {0};
	LPITEMIDLIST root;
	lstrcpy(szBuff, "������ ���� �ϼ���!");

	ZeroMemory(&bi, sizeof(BROWSEINFO));
	SHGetSpecialFolderLocation(this->m_hWnd, CSIDL_DESKTOP, &root);
	bi.hwndOwner = this->GetSafeHwnd();	//20070509 �̷��� �����ν� ��� ��ȭ���ڰ� ��, jyh
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
	m_editDownPath.SetWindowText(m_strDownPath);		//���� ��ġ ���

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

	// ���� ���� �۾� ����
	SetTimer(1, 10, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void COpenDown_clientDlg::OnPaint()
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
HCURSOR COpenDown_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenDown_clientDlg::OnTimer(UINT_PTR nIDEvent)
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// ���������� ������
	if(bFlag) 
	{
		OpenDownLoadPro();
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

void COpenDown_clientDlg::OpenDownPre()
{
	CString strBuf;
	__int64 nTrsStartSize = 0;

	// ���ÿ� ���� �̸��� ������ �����ϴ��� üũ
	CFileFind pFileFind;
	CString strFName = m_pPtrTrans->pFullPath;

	if(pFileFind.FindFile((LPCTSTR)strFName))
	{	
		pFileFind.FindNextFile();
		nTrsStartSize = (__int64)pFileFind.GetLength();

		if(nTrsStartSize == m_pPtrTrans->nFileSizeReal)
		{
			if(AfxMessageBox("���� ������ �����մϴ�. ���� ���ðڽ��ϱ�?", MB_YESNO | MB_ICONWARNING) == IDYES)
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
		// �Ϸ�� �������� üũ
		else if(m_pPtrTrans->nFileSizeReal < nTrsStartSize)
		{
			if(AfxMessageBox("���� ���� ���Ϻ��� ũ�Ⱑ ū ������ �����մϴ�. ���� ���ðڽ��ϱ�?", MB_YESNO | MB_ICONWARNING) == IDYES)
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

	// �ٿ�ε带 �����ϱ� ���� Ÿ�� ��ũ�� �ܿ� �뷮�� ���۹��� �뷮���� ������ üũ
	__int64 nFreeSize = GetDiskFreeSpaceMMSV((LPCTSTR)(LPSTR)m_pPtrTrans->pFullPath);

	if(nFreeSize <= (m_pPtrTrans->nFileSizeReal - nTrsStartSize))
	{
		SetTransFlag(false);

		CString strTargetPath = m_pPtrTrans->pFullPath;
		CString strTargetVolume = strTargetPath.Left(strTargetPath.Find("\\"));

		m_pPtrTrans	= NULL;	

		CString strCancelState;
		strCancelState.Format("���õ�ũ�� �뷮�� �����մϴ�! Ȯ���� �ٽ� �õ��� �ֽʽÿ�.\r\n\r\n[%s] ����̺��� ���� �뷮�� Ȯ���ϼ���!", strTargetVolume);
		AfxMessageBox(strCancelState);										
		OnBnClickedCancel();
	}	

	// �ٿ�ε� ������ ���� ������ä�� ��û		
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
				m_v_PtrTrans[i].nTransState == ITEM_STATE_DEFER/*20071119 �߰�, jyh*/ ||
				m_v_PtrTrans[i].nTransState == ITEM_STATE_CONNECT/*20080228 �߰�, jyh*/)
			{
				//20071119 ���� ���۵� ������ �ε��� ���� ��������� ����, jyh
				m_nCurTransFile = i;
				// �������� ������ ���� : ���������� ����
				m_pPtrTrans = (PROPENDOWNCTX)&m_v_PtrTrans[i];
				m_pPtrTrans->nTransState = ITEM_STATE_CONNECT;

				// �ٿ�ε带 �����ϱ� ���� ���� �ٿ�ε� ���� ������ ���� üũ
				OpenDownPre();
				
				return;
			}
		}
	}

	// ���������� ����
	//SetTransFlag(false);
	// ���� ���� ���� ����
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

			//20070919 ���� ���, jyh
			if(lstrcmp(m_strPrevPath.GetBuffer(), m_v_PtrTrans[m_nCurTransFile].pFolderName))
			{
				char sPath[4096];

				lstrcpy(sPath, m_v_PtrTrans[m_nCurTransFile].pFolderName);
				m_strPrevPath = m_v_PtrTrans[m_nCurTransFile].pFolderName;

				int nIdx = (int)strlen(sPath);
				sPath[nIdx-1] = '\0';
				m_editDownPath.SetWindowText(sPath);	//20070919 ���� ���, jyh
				m_editDownPath.Invalidate();
			}

			m_strPrevPath.ReleaseBuffer();
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
	COpenDown_clientDlg* pProcess = (COpenDown_clientDlg*)pParam;
	pProcess->FileTransLoop();
	return 0;
}


void COpenDown_clientDlg::FileTransLoop()
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
		char strBuf[32];

		Packet* pPacket = new Packet;
		pPacket->Pack(OPEN_DOWNEND, (char*)strBuf, 32);
		m_pWorker_Ctrl->SendPacket(pPacket);

		// �������� ������ �������� ����
		m_pPtrTrans	= NULL;		
	}

	// ���� ��ư�� Ŭ���� ���� �ƴϸ� ���� ����Ʈ�� �ִ��� üũ
	OpenDownLoadPro();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���α׷� ���� : ������ �ް� �ִ��� üũ �� ó�� ����� �Ѵ�
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
// ���� ������ ���� �ӵ� 
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
CString COpenDown_clientDlg::SetSizeFormat(const __int64 nRealSize)
{	
	char pTemp[256];
	::StrFormatByteSize64(nRealSize, pTemp, 256);	
	CString bSize = pTemp;
	bSize.Replace("����Ʈ", "B");	
	return bSize;
}