// MtoI_Module.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WebBBS.h"
#include "MtoI_Module.h"
#include <atlimage.h>
#include <afxinet.h>    // MFC Internet support
#include <dshow.h>
#include <strsafe.h>

#pragma comment(lib, "strmiids.lib") 


// MtoI_Module 대화 상자입니다.

IMPLEMENT_DYNAMIC(MtoI_Module, CDialog)

MtoI_Module::MtoI_Module(CWnd* pParent /*=NULL*/)
	: CDialog(MtoI_Module::IDD, pParent)
{
	Step_state=1;
	Step_state_cancel=0;
}

MtoI_Module::~MtoI_Module()
{
}

void MtoI_Module::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MtoI_Module, CDialog)
	ON_BN_CLICKED(IDCANCEL, &MtoI_Module::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// MtoI_Module 메시지 처리기입니다.

void MtoI_Module::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(Step_state==1)
	{
		Step_state_cancel=1;
		Step_state=0;
		HWND hWndStatic = GetDlgItem(IDC_STATIC_TITLE)->m_hWnd;
		m_pCtrl.Attach(hWndStatic);
		m_pCtrl.SetWindowText("이미지 추출 취소.");
		m_pCtrl.UpdateWindow();
		m_pCtrl.Detach();
	}
}

void MtoI_Module::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int		dummy;
	HWND hWndProgress = GetDlgItem(IDC_PROGRESS_SHOW)->m_hWnd;	




	switch(nIDEvent)
	{
		case 1:
			KillTimer(1);

			m_pProgress.Attach(hWndProgress);
			m_pProgress.SetRange(0, 100);
			m_pProgress.SetPos(0);
			m_pProgress.SetStep(1);
			m_pProgress.Detach();

			SetTimer(4, 100, NULL);

			hTrans = (HANDLE)_beginthreadex(NULL, 0, MOVIEtoImageLoad, this, 0, (unsigned int *)&dummy);
			CloseHandle(hTrans);
		break;
		case 2:
			KillTimer(2);
			End_Mtol();
		break;
		case 4:
			if(Step_state==2)
			{
				KillTimer(4);
				SetTimer(2, 100, NULL);
			}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL MtoI_Module::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText("동영상 스크린 캡쳐");


	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	HWND hWndStatic = GetDlgItem(IDC_STATIC_FILENAME)->m_hWnd;
	m_pCtrl.Attach(hWndStatic);
	m_pCtrl.SetWindowText(File_name);
	m_pCtrl.UpdateWindow();
	m_pCtrl.Detach();


	//이미지 추출 시작
	SetTimer(1, 100, NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}




CString MtoI_Module::Save_File(CString Avi_filename , int image_count, CString recieve_http_domain, CString recieve_http_url)
{
	IGraphBuilder *pGraphBuilder;
	IBasicVideo *pBasicVideo;
	IMediaPosition *pMediaPosition;
	long lBytes = 0;
	REFTIME llPos,Show_save_Point_mem,Show_save_Point;
	HRESULT hr;

	CString				save_file_Name,tmp_Save_image_folder,Savewebfile_name;
	CImage				Save_Images;


	
	//재생 시간
	REFTIME Self_play_time;
	//이미지 크기
	long movie_size_w=0;
	long movie_size_h=0;

	//송신 파라메터
	CString Movie_info_string,Movie_info_string_all,Movie_info_show_switch;

	//이미지 랜덤용
	time_t timer;
	struct tm timeinfo;

	
	
///////상태 표현을 위한 변수 - S
	HWND hWndStatic = GetDlgItem(IDC_STATIC_TITLE)->m_hWnd;
	HWND hWndProgress = GetDlgItem(IDC_PROGRESS_SHOW)->m_hWnd;

	CString strState;
	int nPercentDone;
///////상태 표현을 위한 변수 - E


	//리턴할 값
	Savewebfile_name="";

	CoInitialize(NULL);

	//핸들 세팅
	CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,IID_IGraphBuilder,(LPVOID *)&pGraphBuilder);
	pGraphBuilder->RenderFile(Avi_filename.AllocSysString(),NULL);
	pGraphBuilder->QueryInterface(IID_IBasicVideo,(LPVOID *)&pBasicVideo);
	pGraphBuilder->QueryInterface(IID_IMediaPosition,(LPVOID *)&pMediaPosition);




	//전체 사이즈체크
	hr = pMediaPosition->get_Duration(&llPos);
	if (FAILED(hr))
	{

		pMediaPosition->Release();
		pBasicVideo->Release();
		pGraphBuilder->Release();

		CoUninitialize();



		if(IsVistaOS()) 
		{
			m_pCtrl.Attach(hWndStatic);
			m_pCtrl.SetWindowText("캡쳐 이미지를 읽어 올수없습니다.");
			m_pCtrl.UpdateWindow();
			m_pCtrl.Detach();

			::MessageBox(m_hWnd, "캡쳐 할수 없는 동영상 입니다", "알림", MB_ICONINFORMATION|MB_OK);
		}
		else
		{
			m_pCtrl.Attach(hWndStatic);
			m_pCtrl.SetWindowText("프레임 정보를 읽어올수 없습니다.");
			m_pCtrl.UpdateWindow();
			m_pCtrl.Detach();

			::MessageBox(m_hWnd, "캡쳐 할수 없는 동영상 입니다", "오류", MB_ICONSTOP|MB_OK);
		}

		Step_state_cancel=1;
		Step_state=2;

		return "";
	}



	//캡처 가능 유무 체크
	hr = pBasicVideo->GetCurrentImage(&lBytes, NULL);
	if (FAILED(hr))
	{
		pMediaPosition->Release();
		pBasicVideo->Release();
		pGraphBuilder->Release();

		CoUninitialize();
		




		m_pCtrl.Attach(hWndStatic);
		m_pCtrl.SetWindowText("캡쳐 이미지를 읽어 올수없습니다.");
		m_pCtrl.UpdateWindow();
		m_pCtrl.Detach();


		::MessageBox(m_hWnd, "캡쳐 할수 없는 동영상 입니다", "알림", MB_ICONINFORMATION|MB_OK);
		
		Step_state_cancel=1;
		Step_state=2;

		return"";
	}



	//캡처할 이미지가 있을만한 크기인지 체크
	if (lBytes < 1)
	{
			
		pMediaPosition->Release();
		pBasicVideo->Release();
		pGraphBuilder->Release();

		CoUninitialize();


		m_pCtrl.Attach(hWndStatic);
		m_pCtrl.SetWindowText("캡쳐 이미지를 읽어 올수없습니다.");
		m_pCtrl.UpdateWindow();
		m_pCtrl.Detach();

		::MessageBox(m_hWnd, "캡쳐 할수 없는 동영상 입니다", "알림", MB_ICONINFORMATION|MB_OK);
		
		Step_state_cancel=1;
		Step_state=2;

		return"";
	}


	//현재 사이즈 로딩
	pBasicVideo->GetVideoSize(&movie_size_w,&movie_size_h);

	//체크 완료후 핸들 닫음
	pMediaPosition->Release();
	pBasicVideo->Release();
	pGraphBuilder->Release();




	//업로드 가능한 폴더 찾기
	tmp_Save_image_folder=img_Savefolder_load();

	//로딩할 프레임세팅을 위한 초기설정
	Show_save_Point_mem=(REFTIME)0;
	Show_save_Point=llPos/(REFTIME)image_count;

	if(llPos>10)	Show_save_Point_mem=10;
	
	




	//파일 크기 알아내기 - S
	__int64 Movie_file_size;
	CFileFind mFind;
	Movie_file_size=0;
	if(mFind.FindFile(Avi_filename))
	{
		mFind.FindNextFile();
		Movie_file_size=mFind.GetLength();
	}
	mFind.Close();	
	//파일 크기 알아내기 - E



	//이미지 정보 데이터
	Movie_info_string.Format("|%s|%I64d|%d|%d|%d",End_string_Datas(Avi_filename,"\\"),Movie_file_size,movie_size_w,movie_size_h,(int)llPos);

	//표시 유무값 설정
	Movie_info_show_switch.Format("|%d|%d|",Save_image_show_info,Save_image_show_size);
	


	//이미지 로딩을 위한 변수세팅
	BITMAPFILEHEADER bfh;
	int nPalleteSize;
	DWORD dw = 0;
	HANDLE File_handle;
	BITMAPINFOHEADER *bih;


	CString Local_pc_ip;
	try
	{
		PHOSTENT hostInfo;
		char buf[255];
		if((gethostname(buf, sizeof(buf)) == 0) && ((hostInfo = gethostbyname(buf)) != 0))
			Local_pc_ip = inet_ntoa(*(struct in_addr*)*hostInfo->h_addr_list);
	}
	catch(...)
	{
		Local_pc_ip.Format("");
	}
	Local_pc_ip.Replace(".","");





	BYTE *pBmp_info = NULL;
	pBmp_info = new BYTE[lBytes];


	for(int rof=0;rof<image_count;rof++)
	{
		if(Step_state==1)
		{

			//핸들 재생성
			CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,IID_IGraphBuilder,(LPVOID *)&pGraphBuilder);
			pGraphBuilder->RenderFile(Avi_filename.AllocSysString(),NULL);
			pGraphBuilder->QueryInterface(IID_IBasicVideo,(LPVOID *)&pBasicVideo);
			pGraphBuilder->QueryInterface(IID_IMediaPosition,(LPVOID *)&pMediaPosition);

///////캡쳐 - S

			//캡쳐할 위치이동
			pMediaPosition->put_CurrentPosition(Show_save_Point_mem);

			//현재 위치의 시간
			pMediaPosition->get_CurrentPosition(&Self_play_time);

			//이미지 캐싱
			pBasicVideo->GetCurrentImage(&lBytes,(long *)pBmp_info);

///////캡쳐 - E

			//핸들 재생성 닫기
			pMediaPosition->Release();
			pBasicVideo->Release();
			pGraphBuilder->Release();





			//저장할 파일명세팅
			save_file_Name.Format("%s/tmp_%d.jpg",tmp_Save_image_folder,rof);

//////////////////////////////////S BMP 저장

			bih = (BITMAPINFOHEADER *)pBmp_info;
			nPalleteSize = 0;
			if(bih->biBitCount<=8)	nPalleteSize = (2 << bih->biBitCount) * 3;

			ZeroMemory(&bfh,sizeof(bfh));
			bfh.bfType = 'MB';
			bfh.bfSize = sizeof(bfh)+lBytes;
			bfh.bfOffBits = sizeof(bfh)+sizeof(BITMAPINFOHEADER)+nPalleteSize;
			//저장
			dw = 0;
			File_handle = CreateFile(save_file_Name+".bmp",GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
			WriteFile(File_handle,&bfh,sizeof(bfh),&dw,0);
			WriteFile(File_handle,pBmp_info,lBytes,&dw,0);
			CloseHandle(File_handle);

//////////////////////////////////E BMP 저장

			//jpg로 변환 - S
			Save_Images.Load(save_file_Name+".bmp");
			Save_Images.Save(save_file_Name);
			Save_Images.Destroy();
			//jpg로 변환 - E

			//웹에 업로드를 위한 이미지명 생성 - S
			timer = time(NULL);
			localtime_s(&timeinfo,&timer);  

			srand((unsigned) time(NULL));
			// 첫부분의 숫자는 거의 랜덤하지 않기에, 앞부분에서 랜덤한 개수로 뽑아서 버림
			for (int i = 0; i < (rand() % RAND_MAX); i++) (rand() % RAND_MAX);



			CString savefilename_make;
			savefilename_make.Format("Aimage_%d%d%d%d%d%d_%d_%d_%d0%s.jpg",
				timeinfo.tm_year,
				timeinfo.tm_mon,
				timeinfo.tm_mday,
				timeinfo.tm_hour,
				timeinfo.tm_min,
				timeinfo.tm_sec,
				rand(),
				rof,
				rand(),
				Local_pc_ip);
			//웹에 업로드를 위한 이미지명 생성 - E



			Movie_info_string_all.Format("%s|%d",Movie_info_string,(int)Self_play_time);
			//웹에 이미지 업로드
			HTTP_file_upload(save_file_Name, recieve_http_domain, recieve_http_url,savefilename_make,Movie_info_string_all,Movie_info_show_switch);


			//리턴할 데이터 생성
			Savewebfile_name.Format("%s@%s",Savewebfile_name,savefilename_make);


			//로컬에 이미지 삭제
			CFile::Remove(save_file_Name+".bmp");
			CFile::Remove(save_file_Name);




			Show_save_Point_mem+=Show_save_Point;

			if(Step_state==1)
			{
	//////상태 표현 - S
				strState.Format("이미지 추출 %d / %d",rof+1,image_count);

				nPercentDone=(rof*100)/image_count;

				m_pCtrl.Attach(hWndStatic);
				m_pProgress.Attach(hWndProgress);

				m_pCtrl.SetWindowText(strState);
				m_pCtrl.UpdateWindow();
				m_pProgress.SetPos(nPercentDone);

				m_pCtrl.Detach();
				m_pProgress.Detach();
	//////상태 표현 - E
			}

		}

	}

	//비트맵 변수값제거
	delete pBmp_info;





	if(!Step_state)
		Step_state=2;


	CoUninitialize();

	return Savewebfile_name;
}


//웹에 파일업로드
bool MtoI_Module::HTTP_file_upload(CString upload_filename, CString recieve_http_domain, CString recieve_http_url,CString savefilename_make,CString request_datas,CString show_switch)
{
	CInternetSession internetSession;
	CHttpConnection* httpConnection;
	CHttpFile*   httpFile;


	httpConnection = internetSession.GetHttpConnection(recieve_http_domain, (INTERNET_PORT)80,NULL ,NULL);
	httpFile = httpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,recieve_http_url);


	/* 헤더 */
	httpFile->AddRequestHeaders("Accept: text/*\r\nContent-Type: multipart/form-data; boundary=---------------------------7d13a23b368\r\nUser-Agent: VCApp\r\n");




	CString sPrefix,sSuffix;
	sPrefix.Format(_T("\r\n-----------------------------7d13a23b368\r\n"));
	sPrefix.Format(_T("%sContent-Disposition: form-data; name=\"Savefile_names\"\r\n\r\n%s"), sPrefix,savefilename_make);


	sPrefix.Format(_T("%s\r\n-----------------------------7d13a23b368\r\n"),sPrefix);
	sPrefix.Format(_T("%sContent-Disposition: form-data; name=\"Show_swtich\"\r\n\r\n%s"), sPrefix,show_switch);


	sPrefix.Format(_T("%s\r\n-----------------------------7d13a23b368\r\n"),sPrefix);
	sPrefix.Format(_T("%sContent-Disposition: form-data; name=\"Request_datas\"\r\n\r\n%s"), sPrefix,request_datas);
	

	sPrefix.Format(_T("%s\r\n-----------------------------7d13a23b368\r\n"),sPrefix);
	sPrefix.Format(_T("%sContent-Disposition: form-data; name=\"image_files\"; filename=\"%s\"\r\n"), sPrefix,upload_filename);
	sPrefix.Format(_T("%sContent-Type: application/octet-stream\r\n\r\n"), sPrefix);

	sSuffix = _T("\r\n\r\n-----------------------------7d13a23b368--\r\n");

	DWORD dwPrefix = sPrefix.GetLength();
	DWORD dwSuffix = sSuffix.GetLength();

	CFile postFile;
	postFile.Open(upload_filename, CFile::modeRead|CFile::typeBinary);

	postFile.SeekToEnd();
	DWORD dwFileSize;
	dwFileSize = (DWORD)postFile.GetPosition();
	postFile.SeekToBegin();

	BYTE * szPostData, * pCurrentByte;
	DWORD dwPostDataLen = dwPrefix + dwFileSize + dwSuffix;

	szPostData = pCurrentByte = new BYTE[dwPostDataLen];
	memcpy((void *)szPostData, (const void *)(LPCTSTR)sPrefix, dwPrefix);
	pCurrentByte += dwPrefix;

	pCurrentByte += postFile.Read((void *)pCurrentByte, dwFileSize);

	memcpy((void *)pCurrentByte, (const void *)(LPCTSTR)sSuffix, dwSuffix);


	/* 요청할 데이터의 전체 크기 세팅 */
	httpFile->SendRequestEx(dwPostDataLen);

	/* Prefix + FileData + Suffix 전송(한번에) */
	httpFile->Write(&szPostData[0], dwPostDataLen);
	Sleep(10);

	/* 전송 완료 */
	httpFile->EndRequest();
	DWORD dwRet;
	httpFile->QueryInfoStatusCode(dwRet);
	TCHAR szResp[1024];
	while (httpFile->ReadString(szResp, 1024))
	TRACE("%s\n", szResp);
	delete (LPVOID)szPostData;

	return true;
}

//업로드 가능한 폴더 찾기
CString MtoI_Module::img_Savefolder_load(void)
{
	HRESULT		hr;
	CString strLocalPath;
	PWSTR pwPath = NULL;
	char pPath[MAX_PATH];
	ZeroMemory(pPath, sizeof(pPath));

	SHGetSpecialFolderPath(NULL, pPath,  CSIDL_PROGRAM_FILES, false);

	typedef UINT (CALLBACK* LPFNDLLFUNC)(GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);
 
	HINSTANCE	hInst;
	size_t		len;
	LPFNDLLFUNC pGetKnownFldpathFnPtr;

	GUID FOLDERID_LocalAppDataLow = {0xA520A1A4, 0x1780, 0x4FF6, {0xBD, 0x18, 0x16, 0x73, 0x43, 0xC5, 0xAF, 0x16}};
	hInst = LoadLibrary( _T("shell32.dll") );

	pGetKnownFldpathFnPtr = ( LPFNDLLFUNC )GetProcAddress( hInst, "SHGetKnownFolderPath" );

	if(pGetKnownFldpathFnPtr && SUCCEEDED(pGetKnownFldpathFnPtr(FOLDERID_LocalAppDataLow, 0, NULL, &pwPath)))
	{
		hr = StringCchLengthW(pwPath, STRSAFE_MAX_CCH, &len);

		if(SUCCEEDED(hr))
			WideCharToMultiByte(CP_ACP, 0, pwPath, (int)len, pPath, MAX_PATH, NULL, NULL);

		CoTaskMemFree(pwPath);
	}

	FreeLibrary(hInst);

	strLocalPath.Format("%s\\", pPath);
	return strLocalPath;
}


unsigned int __stdcall MOVIEtoImageLoad(void *pParam)
{	
	MtoI_Module *pWindown = (MtoI_Module*)pParam;
	pWindown->ThreadMOVIEtoImage();

	return 1;
}

void MtoI_Module::ThreadMOVIEtoImage(void)
{
	HWND hWndStatic = GetDlgItem(IDC_STATIC_TITLE)->m_hWnd;
	m_pCtrl.Attach(hWndStatic);
	m_pCtrl.SetWindowText("이미지 추출초기화.");
	m_pCtrl.UpdateWindow();
	m_pCtrl.Detach();


	//파일존재 유무체크
	
	CFileFind Local_Find_f;
	if(Local_Find_f.FindFile(File_name))
	{	

		//이미지 저장
		Save_IMAGE_File_name=Save_File(File_name , Save_image_count, Save_domain, Save_url);

		//정상 종료 인경우
		if(Step_state==1)
		{
			
			HWND hWndProgress = GetDlgItem(IDC_PROGRESS_SHOW)->m_hWnd;
			m_pProgress.Attach(hWndProgress);
			m_pProgress.SetPos(100);
			m_pProgress.Detach();

			Step_state=2;
		}
	}
	else
	{
		::MessageBox(m_hWnd, "파일이 존재 하지 않습니다.", "오류", MB_ICONSTOP|MB_OK);
		Step_state_cancel=1;
		Step_state=2;
	}
}

void MtoI_Module::End_Mtol(void)
{
	KillTimer(2);

	if(hTrans)
		CloseHandle(hTrans);

	if(Step_state_cancel)	EndDialog(1);
	else					EndDialog(0);
}

BOOL MtoI_Module::IsVistaOS(void)
{
	BOOL bIsVista = FALSE;
	OSVERSIONINFO sInfo;
	sInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	// 
	// OS VERSION CHECK.
	//
	if(::GetVersionEx(&sInfo)) {
		if( sInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
			sInfo.dwMajorVersion >= 6 &&
			sInfo.dwMinorVersion >= 0) { // Windows VISTA
			bIsVista = TRUE;

		}
	}

	return bIsVista;
}

CString MtoI_Module::End_string_Datas(CString All_string, CString Find_string)
{
	int pos,show_rof;
	CString return_string;


	show_rof=All_string.GetLength();

	for(int rof=0;rof<show_rof;rof++)
	{
		pos=All_string.Find(Find_string);

		if(pos!=-1)
		{
			All_string=All_string.Mid(pos+1);
			return_string=All_string;
		}
		else break;
	}

	return return_string;
}
