// Upcall.cpp,v 1.4 2004/01/08 23:20:07 shuston Exp

#include "ace/OS_NS_sys_time.h"
#include "ace/Log_Msg.h"
#include "Upcall.h"
#include "PTimerDispatcher.h"

// Listing 2 code/ch20
// The signature of this method changed at ACE 5.4. The 'recurring_timer'
// parameter was added.
int 
UpcallHandler::timeout (PTimerQueue &,
                        PCB *handler,
                        const void *arg,
                        int /* recurring_timer */,
                        const ACE_Time_Value &)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::timeout"));

  return (*handler).handleEvent (arg);
}

#if 0
// This method was removed at ACE 5.4. Replaced by cancel_type() and
// cancel_timer().
int
UpcallHandler::cancellation (PTimerQueue &,
                             PCB *handler)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::cancellation"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d has been cancelled\n"),
              handler->getID ()));

  return handler->handleCancel ();
}
#endif /* 0 */

// This method is called when the timer is canceled
int
UpcallHandler::deletion (PTimerQueue &,
                         PCB *handler,
                         const void *)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::deletion"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d has been deleted\n"),
              handler->getID ()));

  return handler->handleClose ();
}
// Listing 2

// *** The rest of the UpcallHandler methods were added for ACE 5.4 ***

// This method is called when a timer is registered.
int
UpcallHandler::registration (PTimerQueue &,
                             PCB *handler,
                             const void *)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::registration"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d has been registered.\n"),
              handler->getID ()));
  return 0;
}

// This method is called at expiration time, before the actual upcall
// to the handler is made. ACE uses this to adjust reference counts
// when needed.
int
UpcallHandler::preinvoke (PTimerQueue &,
                          PCB *handler,
                          const void *,
                          int,
                          const ACE_Time_Value &,
                          const void *&)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::preinvoke"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d is about to upcalled.\n"),
              handler->getID ()));
  return 0;
}

// This method is called at expiration time, after the actual upcall
// to the handler returns. ACE uses this to adjust reference counts
// when needed.
int
UpcallHandler::postinvoke (PTimerQueue &,
                           PCB *handler,
                           const void *,
                           int,
                           const ACE_Time_Value &,
                           const void *)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::postinvoke"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d returned from upcall.\n"),
              handler->getID ()));
  return 0;
}

// This method is called when a handler is cancelled
int
UpcallHandler::cancel_type (PTimerQueue &,
                            PCB *handler,
                            int dont_call,
                            int &)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::cancel_type"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d has been cancelled\n"),
              handler->getID ()));
  if (!dont_call)
    return handler->handleCancel ();
  return 0;
}

// This method is called when a timer is cancelled
int
UpcallHandler::cancel_timer (PTimerQueue &,
                             PCB *handler,
                             int dont_call,
                             int)
{
  ACE_TRACE (ACE_TEXT ("UpcallHandler::cancel_timer"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Handler %d has been cancelled\n"),
              handler->getID ()));
  if (!dont_call)
    return handler->handleCancel ();
  return 0;
}


// Listing 3 code/ch20
int ACE_TMAIN (int, ACE_TCHAR *[])
{
  PCB cb1, cb2;
  cb1.setID (1);
  cb2.setID (2);
  int arg1 = 1, arg2 = 2;

  PTimerQueue *timerQueue;

  ACE_NEW_RETURN (timerQueue, PTimerHeap (), -1);

  PTimer::instance ()->set (timerQueue);

  ACE_Time_Value tv = ACE_OS::gettimeofday ();
  tv += 20L;

  // Schedule two different timers to go off.
  PTimer::instance ()->schedule (&cb1, &arg1, tv, 1);
  PTimer::instance ()->schedule (&cb2, &arg2, tv, 2);

  // Run the timer event loop forever.
  PTimer::instance ()->wait_for_event ();

  return 0;
}
// Listing 3

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Free_List<ACE_Timer_Node_T<PCB*> >;
template class ACE_Locked_Free_List<ACE_Timer_Node_T<PCB*>, ACE_Null_Mutex>;
template class ACE_Timer_Node_T<PCB*>;
template class ACE_Timer_Heap_T<PCB*, UpcallHandler, ACE_Null_Mutex>;
template class ACE_Timer_Heap_Iterator_T<PCB*, UpcallHandler, ACE_Null_Mutex>;
template class ACE_Timer_Queue_T<PCB*, UpcallHandler, ACE_Null_Mutex>;
template class ACE_Timer_Queue_Iterator_T<PCB*, UpcallHandler, ACE_Null_Mutex>;
template class ACE_Node<ACE_Timer_Node_T<PCB*>*>;
template class ACE_Unbounded_Set<ACE_Timer_Node_T<PCB*>*>;
template class ACE_Unbounded_Set_Iterator<ACE_Timer_Node_T<PCB*>*>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Free_List<ACE_Timer_Node_T<PCB*> >
#pragma instantiate ACE_Locked_Free_List<ACE_Timer_Node_T<PCB*>, ACE_Null_Mutex>
#pragma instantiate ACE_Timer_Node_T<PCB*>
#pragma instantiate ACE_Timer_Heap_T<PCB*, UpcallHandler, ACE_Null_Mutex>
#pragma instantiate ACE_Timer_Heap_Iterator_T<PCB*, UpcallHandler, ACE_Null_Mutex>
#pragma instantiate ACE_Timer_Queue_T<PCB*, UpcallHandler, ACE_Null_Mutex>
#pragma instantiate ACE_Timer_Queue_Iterator_T<PCB*, UpcallHandler, ACE_Null_Mutex>
#pragma instantiate ACE_Node<ACE_Timer_Node_T<PCB*>*>
#pragma instantiate ACE_Unbounded_Set<ACE_Timer_Node_T<PCB*>*>
#pragma instantiate ACE_Unbounded_Set_Iterator<ACE_Timer_Node_T<PCB*>*>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION*/
