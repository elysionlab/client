// UpdateWindow.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CUpdateWindowApp:
// �� Ŭ������ ������ ���ؼ��� UpdateWindow.cpp�� �����Ͻʽÿ�.
//

class CUpdateWindowApp : public CWinApp
{
public:
	CUpdateWindowApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CUpdateWindowApp theApp;