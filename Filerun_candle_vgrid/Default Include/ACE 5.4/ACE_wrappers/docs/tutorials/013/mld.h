
// mld.h,v 1.7 2003/11/09 20:44:19 dhinton Exp

#ifndef MLD_H
#define MLD_H

#include "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Singleton.h"
#include "ace/Atomic_Op.h"
#include "ace/Mutex.h"

/*
   This is a cheap memory leak detector.  Each class I want to watch over
   contains an mld object.  The mld object's ctor increments a global counter
   while the dtor decrements it.  If the counter is non-zero when the program
   is ready to exit then there may be a leak.
 */

class mld
{
public:
    mld (void);
    ~mld (void);

    static int value (void);

protected:
    static ACE_Atomic_Op < ACE_Mutex, int >counter_;
};

// ================================================

/*
   Just drop 'MLD' anywhere in your class definition to get cheap memory leak
   detection for your class.
 */
#define MLD            mld mld_

/*
   Use 'MLD_COUNTER' in main() to see if things are OK.
 */
#define MLD_COUNTER    mld::value()

// ================================================

#endif
