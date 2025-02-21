// Main.cpp,v 4.3 2003/11/01 11:15:25 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    Main.cpp
//
// = DESCRIPTION
//     This is a wrapper for the test programs.  It obviates the test cpp's
//     from having to always include OS.h.
//
// = AUTHOR
//    Don Hinton <dhinton@dresystems.com>
//
// ============================================================================

#include "ace/OS_main.h"
#if defined (ACE_HAS_WINCE)
#  include "ace/ACE.h"
#endif /* ACE_HAS_WINCE */
int run_main (int argc, ACE_TCHAR *argv[]);

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  return run_main (argc, argv);
}
