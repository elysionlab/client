// Task_Ex_Test.cpp,v 1.3 2003/12/03 10:13:27 kobica Exp

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    Task_Ex_Test.cpp
//
// = DESCRIPTION
//    This test program illustrates the ACE_Task_Ex class which has the ACE_Message_Queue_Ex
//    that has the capability to hold user-defined messages instead of ACE_Message_Block
//    
//
// = AUTHOR
//    Kobi Cohen-Arazi <kobi-co@barak-online.net>
//
// ============================================================================

#include "test_config.h"
#include "Task_Ex_Test.h"
#include "ace/Task_Ex_T.h"
#include "ace/Log_Msg.h"
#include "ace/Auto_Ptr.h"

ACE_RCSID(tests, Task_Test_Ex, "Task_Ex_Test.cpp,v 1.3 2003/12/03 10:13:27 kobica Exp")

#if defined (ACE_HAS_THREADS)

/// default params
const ACE_INT32 PRODUCER_THREADS_NO=20;
const ACE_INT32 CONSUMER_THREADS_NO=20;
const ACE_INT32 NUMBER_OF_MSGS=2000;

/// @class Consumer consumes user defined Msgs
class Consumer : public ACE_Task_Ex<ACE_MT_SYNCH, User_Defined_Msg>
{
public:
  /// activate/spawn the threads.
  int open (void*);

  /// svc thread entry point
  virtual int svc (void);
private:

};

int Consumer::open (void*)
{
  if(this->activate (THR_NEW_LWP | THR_JOINABLE,
                     CONSUMER_THREADS_NO)==-1)
  {
    ACE_ERROR_RETURN((LM_ERROR,
                      ACE_TEXT("Consumer::open Error spanwing thread %p\n"),
                      "err="),
                     -1);
  }
  return 0;
}

int Consumer::svc ()
{
  User_Defined_Msg* pMsg=0;
  while(this->getq (pMsg)!=-1)
  {
    ACE_ASSERT (pMsg!=0);
    auto_ptr<User_Defined_Msg> pAuto(pMsg);
    ACE_DEBUG((LM_DEBUG, 
               ACE_TEXT("Consumer::svc got msg id=%d\n"),
               pMsg->msg_id ()));
    if(pMsg->msg_id ()==NUMBER_OF_MSGS-1)
      break;
  }

  ACE_DEBUG((LM_INFO, 
             ACE_TEXT("Consumer::svc ended thread %t\n")));

  return 0;
}


/// producer function produces user defined messages.
ACE_THR_FUNC_RETURN producer (void *arg)
{
  Consumer* c = ACE_static_cast (Consumer*, arg);
  ACE_ASSERT(c!=0);
  if (c==0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("producer Error casting to consumer\n")));
    return (ACE_THR_FUNC_RETURN)-1;
  }
  for (int i=0;i!=NUMBER_OF_MSGS;++i)
  {
    User_Defined_Msg* pMsg=0;
    ACE_NEW_NORETURN(pMsg, User_Defined_Msg(i));
    if (pMsg==0)
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("producer Error allocating data %p\n"),
                 "err="));
      return (ACE_THR_FUNC_RETURN)-1;
    }
    if(c->putq (pMsg)==-1)
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("producer Error putq data %p\n"),
                 "err="));
      return (ACE_THR_FUNC_RETURN)-1;
    }
  }
  return 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Task_Ex <ACE_MT_SYNCH, User_Defined_Msg>;
template class ACE_Message_Queue_Ex <User_Defined_Msg, ACE_MT_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Task_Ex <ACE_MT_SYNCH, User_Defined_Msg>
#pragma instantiate ACE_Message_Queue_Ex <User_Defined_Msg, ACE_MT_SYNCH>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */


#endif /* ACE_HAS_THREADS */

int
run_main (int, ACE_TCHAR *[])
{
  ACE_START_TEST (ACE_TEXT ("Task_Ex_Test"));

#if defined (ACE_HAS_THREADS)

  Consumer c;
  if(c.open (0)==-1)
    ACE_ERROR_RETURN((LM_ERROR, 
                      ACE_TEXT ("main Error opening consumer\n")),-1);
        
  
  int result=ACE_Thread_Manager::instance()->spawn_n (PRODUCER_THREADS_NO,
                                                      ACE_THR_FUNC(producer),
                                                      ACE_static_cast(void*,&c));
  if (result==-1)
  {
    ACE_ERROR_RETURN((LM_ERROR, 
                      ACE_TEXT ("main Error spawning threads %p\n"), 
                      "err="),-1);
  }

  // wait all threads
  int wait_result=ACE_Thread_Manager::instance()->wait();
  if (wait_result==-1)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("main Error Thread_Manager->wait %p\n"),
               "err="));
    return -1;
  }
#else
  ACE_ERROR ((LM_INFO,
              ACE_TEXT ("threads not supported on this platform\n")));
#endif /* ACE_HAS_THREADS */
  ACE_END_TEST;
  return 0;
}
