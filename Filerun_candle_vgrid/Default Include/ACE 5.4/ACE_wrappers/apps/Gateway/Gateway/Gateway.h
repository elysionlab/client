// -*- C++ -*-
//
// Gateway.h,v 4.7 2003/11/01 11:15:21 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    gateway
//
// = FILENAME
//    Gateway.h
//
// = DESCRIPTION
//    Since the Gateway is an <ACE_Service_Object>, this file defines
//    the entry point into the Service Configurator framework.
//
// = AUTHOR
//    Doug Schmidt
//
// ============================================================================

#ifndef ACE_GATEWAY
#define ACE_GATEWAY

#include "ace/svc_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

ACE_SVC_FACTORY_DECLARE (Gateway)

#endif /* ACE_GATEWAY */
