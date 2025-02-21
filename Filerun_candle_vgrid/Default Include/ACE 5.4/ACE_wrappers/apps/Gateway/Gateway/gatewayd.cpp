// gatewayd.cpp,v 4.13 2003/12/30 23:18:58 shuston Exp

// ============================================================================
//
// = LIBRARY
//    gateway
//
// = FILENAME
//    peerd.h
//
// = DESCRIPTION
//    Driver for the gateway daemon (gatewayd).  Note that this is
//    completely generic code due to the Service Configurator
//    framework!
//
// = AUTHOR
//    Douglas C. Schmidt
//
// ============================================================================

#include "ace/OS_NS_unistd.h"
#include "ace/Service_Config.h"
#include "ace/Service_Object.h"
#include "ace/Log_Msg.h"
#include "Gateway.h"

ACE_RCSID (Gateway,
           gatewayd,
           "gatewayd.cpp,v 4.13 2003/12/30 23:18:58 shuston Exp")

int
main (int argc, char *argv[])
{
  if (ACE_OS::access (ACE_DEFAULT_SVC_CONF, F_OK) != 0)
    {
      // Use static linking.
      ACE_Service_Object_Ptr sp = ACE_SVC_INVOKE (Gateway);

      if (sp->init (argc - 1, argv + 1) == -1)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "%p\n",
                           "init"),
                          1);

      // Run forever, performing the configured services until we
      // are shut down by a SIGINT/SIGQUIT signal.

      ACE_Reactor::run_event_loop ();

      // Destructor of <ACE_Service_Object_Ptr> automagically call
      // <fini>.
    }
  else
    {
      if (ACE_Service_Config::open (argc, argv) == -1)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "%p\n",
                           "open"),
                          1);
      else // Use dynamic linking.

        // Run forever, performing the configured services until we are
        // shut down by a signal (e.g., SIGINT or SIGQUIT).

        ACE_Reactor::run_event_loop ();
    }
  return 0;
}
