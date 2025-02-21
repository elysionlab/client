
// mmap.cpp,v 1.3 2003/11/09 20:44:19 dhinton Exp

#include "mmap.h"

#include "ace/Log_Msg.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_unistd.h"

SharedData::SharedData(int _initialize)
    : available_(0)
{
    if( _initialize )
    {
        ACE_OS::sprintf(buf_,"UNSET\n");
    }
}

void SharedData::set(void)
{
    ACE_OS::sprintf(buf_,"My PID is (%d)\n",ACE_OS::getpid());
}

void SharedData::show(void)
{
    ACE_DEBUG ((LM_INFO, "(%P|%t) Shared Data text is (%s)\n",
                buf_ ));
}

int SharedData::available(void)
{
    return available_;
}

void SharedData::available(int _available)
{
    available_ = _available;
}
