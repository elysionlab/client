#pragma once

#include "resource.h"       // main symbols

//#include <atlhost.h>
#include <afxcmn.h>


// MtoI_Module ��ȭ �����Դϴ�.

class MtoI_Module : public CDialog
{
	DECLARE_DYNAMIC(MtoI_Module)

public:
	MtoI_Module(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~MtoI_Module();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MOVIETOIMAGES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
public:
	virtual BOOL OnInitDialog();
	//������
	void ThreadMOVIEtoImage(void);


private:
	//����
	CString Save_File(CString Avi_filename , int image_count, CString recieve_http_domain, CString recieve_http_url);

	//�̹�������
	CString img_Savefolder_load(void);
	//��������
	bool HTTP_file_upload(CString upload_filename, CString recieve_http_domain, CString recieve_http_url,CString savefilename_make,CString request_datas,CString show_switch);
	//������
	friend unsigned int __stdcall MOVIEtoImageLoad(void *pParam);

	//������ �ڵ�
	HANDLE	hTrans;
	//����������
	int Step_state;
	int Step_state_cancel;
	int Counts;

public:


	
	//���̾�α�
	//����afxcmn.h
	CStatic		m_pCtrl;
	//�Ķ�׷���
	CProgressCtrl m_pProgress;

	//��������
	CString File_name;
	CString Save_domain;
	CString Save_url;
	int Save_image_count;
	int Save_image_show_info;
	int Save_image_show_size;

	//����� �̹��� ��ġ ����
	CString Save_IMAGE_File_name;

public:
	void End_Mtol(void);
public:
	BOOL IsVistaOS(void);
public:
	CString End_string_Datas(CString All_string, CString Find_string);
};
