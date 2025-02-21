// ACE_Test.cpp,v 4.3 2003/05/18 19:17:34 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    tests
//
// = DESCRIPTION
//    This simple test exercises and illustrates use of ACE value-added
//    functions.
//
// = AUTHOR
//    Steve Huston <shuston@riverace.com>
//
// ============================================================================

#include "test_config.h"

#if defined (ACE_WIN32)
#include "ace/ACE.h"
#endif /* ACE_WIN32 */

ACE_RCSID(tests, ACE_Test, "ACE_Test.cpp,v 4.3 2003/05/18 19:17:34 dhinton Exp")

// Test ACE::execname to be sure it finds .exe without regard to case.
int
execname_test (void)
{
  int error_count = 0;

  // This test is only interesting on Win32
#if defined (ACE_WIN32)
  const ACE_TCHAR *newname;
  const ACE_TCHAR *prog1 = ACE_TEXT ("myprog.exe");
  const ACE_TCHAR *prog2 = ACE_TEXT ("myprog.EXE");
  const ACE_TCHAR *prog3 = ACE_TEXT ("myprog");

  newname = ACE::execname (prog1);
  if (newname != prog1)   // Didn't find .exe correctly
    {
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("Name %s, not %s\n"), newname, prog1));
      delete [] ACE_const_cast (ACE_TCHAR *, newname);
      ++error_count;
    }

  newname = ACE::execname (prog2);
  if (newname != prog2)   // Didn't find .exe correctly
    {
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("Name %s, not %s\n"), newname, prog2));
      delete [] ACE_const_cast (ACE_TCHAR *, newname);
      ++error_count;
    }

  newname = ACE::execname (prog3);
  if (newname == prog3)   // Thought the name didn't need .exe
    {
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("Says .exe not needed for %s\n"),
                  newname));
      ++error_count;
    }
  else
    delete [] ACE_const_cast (ACE_TCHAR *, newname);
#endif /* ACE_WIN32 */

  return error_count;
}


int
run_main (int, ACE_TCHAR *[])
{
  ACE_START_TEST (ACE_TEXT ("ACE_Test"));

  int status = 0;
  int result;

  if ((result = execname_test ()) != 0)
    status = result;

  ACE_END_TEST;
  return status;
}
