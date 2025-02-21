// Options.cpp,v 1.2 2004/01/04 21:17:14 bala Exp

#include "ace/OS.h"

#ifndef JAWS_BUILD_DLL
#define JAWS_BUILD_DLL
#endif /*JAWS_BUILD_DLL*/

#include "jaws3/Options.h"

JAWS_Options::JAWS_Options (void)
{
  this->cf_ = new JAWS_Config_File ("jaws.conf");
}

const char *
JAWS_Options::getenv (const char *key)
{
  const char *value = 0;
  if (this->cf_ == 0 || this->cf_->find (key, value) < 0)
    value = ACE_OS::getenv (key);

  return value;
}

