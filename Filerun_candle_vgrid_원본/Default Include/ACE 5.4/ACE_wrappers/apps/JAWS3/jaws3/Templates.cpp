// Templates.cpp,v 1.2 2004/01/04 21:17:14 bala Exp

#include "ace/OS.h"
#include "ace/Reactor.h"
#include "ace/Select_Reactor.h"
#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"
#include "ace/LSOCK_Stream.h"

#ifndef JAWS_BUILD_DLL
#define JAWS_BUILD_DLL
#endif

#include "jaws3/Asynch_IO.h"
#include "jaws3/Concurrency.h"
#include "jaws3/Export.h"
#include "jaws3/IO.h"
#include "jaws3/Event_Dispatcher.h"
#include "jaws3/Event_Completer.h"
#include "jaws3/Options.h"
#include "jaws3/Protocol_Handler.h"
#include "jaws3/Reactive_IO.h"
#include "jaws3/Signal_Task.h"
#include "jaws3/Symbol_Table.h"
#include "jaws3/Synch_IO.h"
#include "jaws3/TPOOL_Concurrency.h"
#include "jaws3/TPR_Concurrency.h"
#include "jaws3/THYBRID_Concurrency.h"
#include "jaws3/Timer.h"
#include "jaws3/Task_Timer.h"

#define ACE_EHHTU_RW \
        ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_RW_MUTEX>
#define ACE_EHHTU_R \
        ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_RECURSIVE_MUTEX>

#define ACE_WHEEL_TEMPLATE_ARGS_RW \
        ACE_Event_Handler *, ACE_EHHTU_RW, ACE_SYNCH_RW_MUTEX
#define ACE_WHEEL_TEMPLATE_ARGS_R \
        ACE_Event_Handler *, ACE_EHHTU_R, ACE_SYNCH_RECURSIVE_MUTEX

#define ACE_TWT_RW \
        ACE_Timer_Wheel_T<ACE_WHEEL_TEMPLATE_ARGS_RW>
#define ACE_TWT_R \
        ACE_Timer_Wheel_T<ACE_WHEEL_TEMPLATE_ARGS_R>
#define ACE_TWIT_RW \
        ACE_Timer_Wheel_Iterator_T<ACE_WHEEL_TEMPLATE_ARGS_RW>
#define ACE_TWIT_R \
        ACE_Timer_Wheel_Iterator_T<ACE_WHEEL_TEMPLATE_ARGS_R>
#define ACE_TQT_RW \
        ACE_Timer_Queue_T<ACE_WHEEL_TEMPLATE_ARGS_RW>
#define ACE_TQT_R \
        ACE_Timer_Queue_T<ACE_WHEEL_TEMPLATE_ARGS_R>
#define ACE_TQIT_RW \
        ACE_Timer_Queue_Iterator_T<ACE_WHEEL_TEMPLATE_ARGS_RW>
#define ACE_TQIT_R \
        ACE_Timer_Queue_Iterator_T<ACE_WHEEL_TEMPLATE_ARGS_R>

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
template class ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_NULL_SYNCH>;
template class ACE_Atomic_Op<ACE_SYNCH_MUTEX, int>;
template class ACE_Node<void *>;
template class ACE_Unbounded_Queue<void *>;
template class ACE_Unbounded_Queue_Iterator<void *>;
template class ACE_Singleton<JAWS_Synch_IO, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Asynch_IO, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Reactive_IO, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_IO, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_TPOOL_Concurrency, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_TPR_Concurrency, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_THYBRID_Concurrency, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Concurrency, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Options, ACE_SYNCH_MUTEX>;
template class ACE_Select_Reactor_Token_T<ACE_Noop_Token>;
template class ACE_Select_Reactor_T<ACE_Select_Reactor_Noop_Token>;
template class ACE_Lock_Adapter<ACE_Select_Reactor_Noop_Token>;
template class ACE_EHHTU_RW;
template class ACE_EHHTU_R;
template class ACE_TWT_RW;
template class ACE_TWT_R;
template class ACE_TWIT_RW;
template class ACE_TWIT_R;
template class ACE_TQT_RW;
template class ACE_TQT_R;
template class ACE_TQIT_RW;
template class ACE_TQIT_R;
template class ACE_Thread_Timer_Queue_Adapter< ACE_TWT_RW >;
template class ACE_Thread_Timer_Queue_Adapter< ACE_TWT_R >;
template class ACE_Singleton<JAWS_Timer, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Task_Timer, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Signal_Task, ACE_SYNCH_MUTEX>;
template class ACE_Singleton<ACE_Message_Block, ACE_SYNCH_NULL_MUTEX>;
template class JAWS_SYMBOL_TABLE_ENTRY;
template class JAWS_SYMBOL_TABLE_BASE;
template class JAWS_SYMBOL_TABLE_ITERATOR_BASE;
template class JAWS_SYMBOL_TABLE_ITERATOR;
template class JAWS_SYMBOL_TABLE_REVERSE_ITERATOR;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

