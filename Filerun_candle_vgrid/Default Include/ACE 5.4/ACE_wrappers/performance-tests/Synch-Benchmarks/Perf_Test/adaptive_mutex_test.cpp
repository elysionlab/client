// adaptive_mutex_test.cpp,v 1.2 2003/11/09 04:12:09 dhinton Exp

#define  ACE_BUILD_SVC_DLL
#include "Adaptive_Lock_Performance_Test_Base.h"
#include "ace/Lock_Adapter_T.h"

ACE_RCSID(Synch_Benchmarks, adaptive_mutex_test, "adaptive_mutex_test.cpp,v 1.2 2003/11/09 04:12:09 dhinton Exp")

#if defined (ACE_HAS_THREADS)

class ACE_Svc_Export Adaptive_Mutex_Test : public Adaptive_Lock_Performance_Test_Base
{
public:
  virtual int init (int, char *[]);
};

int
Adaptive_Mutex_Test::init (int, char *[])
{
  ACE_Lock *lock;
  ACE_NEW_RETURN (lock,
                  ACE_Lock_Adapter<ACE_Thread_Mutex> (),
                  -1);

  return this->set_lock (lock);
}

ACE_SVC_FACTORY_DECLARE (Adaptive_Mutex_Test)
ACE_SVC_FACTORY_DEFINE  (Adaptive_Mutex_Test)

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Lock_Adapter<ACE_Thread_Mutex>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Lock_Adapter<ACE_Thread_Mutex>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
#endif /* ACE_HAS_THREADS */
