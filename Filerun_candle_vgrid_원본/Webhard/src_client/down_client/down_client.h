#pragma once

#include "resource.h"		


class down_client : public CWinApp
{

public:

	down_client();
	virtual BOOL InitInstance();

	STDMETHOD(Update_Checking)(void);		//20071205 ��Ÿ ���� ���, jyh

	DECLARE_MESSAGE_MAP()
};

extern down_client theApp;
