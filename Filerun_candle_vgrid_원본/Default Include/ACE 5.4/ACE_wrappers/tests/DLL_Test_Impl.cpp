// DLL_Test_Impl.cpp,v 4.12 2003/11/14 00:31:58 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    DLL_Test.cpp
//
// = DESCRIPTION
//    This test illustrates the use of <ACE_DLL> wrapper class.
//
// = AUTHOR
//    Kirthika Parameswaran  <kirthika@cs.wustl.edu>
//
// ============================================================================

#include "DLL_Test_Impl.h"
#include "ace/ACE.h"
#include "ace/OS_Errno.h"
#include "ace/svc_export.h"
#include "ace/OS_NS_string.h"

ACE_RCSID (tests,
           DLL_Test_Impl,
           "DLL_Test_Impl.cpp,v 4.12 2003/11/14 00:31:58 dhinton Exp")

Hello_Impl::Hello_Impl (void)
{
  ACE_DEBUG ((LM_DEBUG, "Hello_Impl::Hello_Impl\n"));
}

Hello_Impl::~Hello_Impl (void)
{
  ACE_DEBUG ((LM_DEBUG, "Hello_Impl::~Hello_Impl\n"));
}

void
Hello_Impl::say_next (void)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("How are you?\n")));
}

ACE_TCHAR *
Hello_Impl::new_info (void)
{
  return ACE::strnew (ACE_TEXT ("Hello_Impl::new_info() allocated by ACE::strnew()"));
}

ACE_TCHAR *
Hello_Impl::malloc_info (void)
{
  return ACE_OS::strdup (ACE_TEXT ("Hello_Impl::new_info() allocated by ACE_OS::malloc()"));
}

void *
Hello_Impl::operator new (size_t bytes)
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::new\n"));
  return ::new char[bytes];
}

#if defined (ACE_HAS_NEW_NOTHROW)
  /// Overloaded new operator, nothrow_t variant.
void *
Hello_Impl::operator new (size_t bytes, const ACE_nothrow_t &nt)
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::new\n"));
  return ::new (nt) char[bytes];
}
#endif /* ACE_HAS_NEW_NOTHROW */

void
Hello_Impl::operator delete (void *ptr)
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::delete\n"));
  delete [] ((char *) ptr);
}

extern "C" ACE_Svc_Export Hello *
get_hello (void)
{
  Hello *hello = 0;

  ACE_NEW_RETURN (hello,
                  Hello_Impl,
                  NULL);

  return hello;
}

class Static_Constructor_Test
{
public:
  Static_Constructor_Test (void)
  {
    ACE_DEBUG ((LM_DEBUG,
                "Static_Constructor_Test::Static_Constructor_Test\n"));
  }
  ~Static_Constructor_Test (void)
  {
    ACE_DEBUG ((LM_DEBUG,
                "Static_Constructor_Test::~Static_Constructor_Test\n"));
  }
};

static Static_Constructor_Test the_instance;

#if !defined (ACE_LACKS_RTTI)
// Test dynamic cast
extern "C" ACE_Svc_Export int
dynamic_cast_test (Parent *target)
{
   return target == dynamic_cast<Child*>( target )? 0 : -1;
}  
#endif /* !ACE_LACKS_RTTI */
