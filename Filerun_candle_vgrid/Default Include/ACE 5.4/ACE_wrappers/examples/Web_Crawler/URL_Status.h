/* -*- C++ -*- */
// URL_Status.h,v 1.4 2003/11/01 11:15:24 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    examples/Web_Crawler
//
// = FILENAME
//    URL_Status.h
//
// = AUTHOR
//    Douglas C. Schmidt <schmidt@cs.wustl.edu>
//
// ============================================================================

#ifndef _URL_STATUS_H
#define _URL_STATUS_H

#include "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class URL_Status
{
  // = TITLE
public:
  enum STATUS_CODE
  {
    STATUS_OK = 200,
    STATUS_CREATED = 201,
    STATUS_ACCEPTED = 202,
    STATUS_NO_CONTENT = 204,
    STATUS_MOVED_PERMANENTLY = 301,
    STATUS_MOVED_TEMPORARILY = 302,
    STATUS_NOT_MODIFIED = 304,
    STATUS_BAD_REQUEST = 400,
    STATUS_UNAUTHORIZED = 401,
    STATUS_FORBIDDEN = 403,
    STATUS_NOT_FOUND = 404,
    STATUS_INTERNAL_SERVER_ERROR = 500,
    STATUS_NOT_IMPLEMENTED = 501,
    STATUS_BAD_GATEWAY = 502,
    STATUS_SERVICE_UNAVAILABLE = 503,
    STATUS_INSUFFICIENT_DATA = 399
  };

  URL_Status (STATUS_CODE = STATUS_INSUFFICIENT_DATA);
  URL_Status (const URL_Status &);

  STATUS_CODE status (void) const;
  void status (int);
  void status (STATUS_CODE);
  int destroy (void);
private:
  STATUS_CODE status_;  
};

#endif /* _URL_STATUS_H */
