// OpenUp_client.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// COpenUp_clientApp:
// �� Ŭ������ ������ ���ؼ��� OpenUp_client.cpp�� �����Ͻʽÿ�.
//

class COpenUp_clientApp : public CWinApp
{
public:
	COpenUp_clientApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern COpenUp_clientApp theApp;