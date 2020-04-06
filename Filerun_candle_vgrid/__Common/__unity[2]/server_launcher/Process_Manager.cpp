#include "Process_Manager.h"
#include "ace/OS.h"

int Process_Manager::CreateProcess(const ACE_TCHAR* strName, const ACE_TCHAR* strParam)
{
	ACE_OS::strcpy (programName_, strName);
	ACE_OS::strcpy (programParam_, strParam);

	ACE_Process_Options options;
    pid_t pid = this->spawn(options);
    if(pid == ACE_INVALID_PID)
		return -1;
	else
		return 1;
}

int Process_Manager::prepare (ACE_Process_Options &options)
{
	options.command_line ("%s %s", this->programName_, this->programParam_);
	
	#if !defined (ACE_WIN32) && !defined (ACE_LACKS_PWD_FUNCTIONS)
		return this->setUserID (options);
	#else
		return 0;
	#endif
}