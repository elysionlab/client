// OpenDown_client.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// COpenDown_clientApp:
// �� Ŭ������ ������ ���ؼ��� OpenDown_client.cpp�� �����Ͻʽÿ�.
//

class COpenDown_clientApp : public CWinApp
{
public:
	COpenDown_clientApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern COpenDown_clientApp theApp;