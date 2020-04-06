#pragma once

#include "resource.h"       // main symbols

//#include <atlhost.h>
#include <afxcmn.h>


// MtoI_Module 대화 상자입니다.

class MtoI_Module : public CDialog
{
	DECLARE_DYNAMIC(MtoI_Module)

public:
	MtoI_Module(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~MtoI_Module();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MOVIETOIMAGES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
public:
	virtual BOOL OnInitDialog();
	//스레드
	void ThreadMOVIEtoImage(void);


private:
	//저장
	CString Save_File(CString Avi_filename , int image_count, CString recieve_http_domain, CString recieve_http_url);

	//이미지저장
	CString img_Savefolder_load(void);
	//웹에저장
	bool HTTP_file_upload(CString upload_filename, CString recieve_http_domain, CString recieve_http_url,CString savefilename_make,CString request_datas,CString show_switch);
	//스레드
	friend unsigned int __stdcall MOVIEtoImageLoad(void *pParam);

	//스래드 핸들
	HANDLE	hTrans;
	//진행유무값
	int Step_state;
	int Step_state_cancel;
	int Counts;

public:


	
	//다이얼로그
	//상태afxcmn.h
	CStatic		m_pCtrl;
	//파라그레스
	CProgressCtrl m_pProgress;

	//세팅정보
	CString File_name;
	CString Save_domain;
	CString Save_url;
	int Save_image_count;
	int Save_image_show_info;
	int Save_image_show_size;

	//저장된 이미지 위치 정보
	CString Save_IMAGE_File_name;

public:
	void End_Mtol(void);
public:
	BOOL IsVistaOS(void);
public:
	CString End_string_Datas(CString All_string, CString Find_string);
};
