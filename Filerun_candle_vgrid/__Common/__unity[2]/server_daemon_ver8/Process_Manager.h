#pragma once
#include "ace/Process.h"

class Process_Manager : public ACE_Process
{

public:

	Process_Manager(void)
	{
	}

	~Process_Manager(void)
	{
	}

	int CreateProcess(const ACE_TCHAR* strName, const ACE_TCHAR* strParam);
	

private:

	int prepare (ACE_Process_Options &options);

private:
  
	ACE_TCHAR programName_[256];
	ACE_TCHAR programParam_[256];
};
