/* -*- c++ -*- */
// Reaper.h,v 1.2 2003/11/09 04:12:07 dhinton Exp

#ifndef JAWS_REAPER_H
#define JAWS_REAPER_H

#include "ace/Singleton.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Task.h"

#include "JAWS/Export.h"

// A reaper class to reap the threads.

class JAWS_Concurrency_Base;

class JAWS_Export JAWS_Reaper : public ACE_Task<ACE_MT_SYNCH>
{
  // = TITLE
  //   Reap threads for the concurrency strategies
  //
  // = DESCRIPTION
  //   The JAWS_Reaper uses the default Thread Manager (while each
  //   concurrency strategy uses their own).  The idea is that the
  //   reaper will spawn a thread to reap the threads of a concurrency
  //   strategy.  This allows the main thread to reap the threads of
  //   the reaper before exiting.

public:
  JAWS_Reaper (JAWS_Concurrency_Base *concurrency);
  virtual ~JAWS_Reaper (void);

  virtual int open (void * = 0);
  virtual int svc (void);

private:
  JAWS_Concurrency_Base *concurrency_;
  int waiting_;
  ACE_SYNCH_MUTEX lock_;

};

#endif /* JAWS_REAPER_H */
