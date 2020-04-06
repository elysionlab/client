#pragma once

#include "resource.h"	



class up_client : public CWinApp
{

public:

	up_client();
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern up_client theApp;
