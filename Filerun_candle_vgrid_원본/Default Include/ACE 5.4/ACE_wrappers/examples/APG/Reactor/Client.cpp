/**
 * Client.cpp,v 1.3 2004/01/07 22:40:16 shuston Exp
 *
 * A simple client program using ACE_Svc_Handler and ACE_Connector.
 */

#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_sys_time.h"
#include "Client.h"

// Listing 2 code/ch07
int Client::open (void *p)
{
  ACE_Time_Value iter_delay (2);   // Two seconds
  if (super::open (p) == -1)
    return -1;
  this->notifier_.reactor (this->reactor ());
  this->msg_queue ()->notification_strategy (&this->notifier_);
  return this->reactor ()->schedule_timer
    (this, 0, ACE_Time_Value::zero, iter_delay);
}
// Listing 2
    
// Listing 3 code/ch07
int Client::handle_input (ACE_HANDLE)
{
  char buf[64];
  ssize_t recv_cnt = this->peer ().recv (buf, sizeof (buf) - 1);
  if (recv_cnt > 0)
    {
      ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%*C"),
                  ACE_static_cast (int, recv_cnt),
                  buf));
      return 0;
    }

  if (recv_cnt == 0 || ACE_OS::last_error () != EWOULDBLOCK)
    {
      this->reactor ()->end_reactor_event_loop ();
      return -1;
    }
  return 0;
}
// Listing 3

// Listing 4 code/ch07
int Client::handle_timeout(const ACE_Time_Value &, const void *)
{
  if (this->iterations_ >= ITERATIONS)
    {
      this->peer ().close_writer ();
      return 0;
    }

  ACE_Message_Block *mb;
  char msg[128];
  ACE_OS::sprintf (msg, "Iteration %d\n", this->iterations_);
  ACE_NEW_RETURN (mb, ACE_Message_Block (msg), -1);
  this->putq (mb);
  return 0;
}
// Listing 4

// Listing 5 code/ch07
int Client::handle_output (ACE_HANDLE)
{
  ACE_Message_Block *mb;
  ACE_Time_Value nowait (ACE_OS::gettimeofday ());
  while (-1 != this->getq (mb, &nowait))
    {
      ssize_t send_cnt =
        this->peer ().send (mb->rd_ptr (), mb->length ());
      if (send_cnt == -1)
        ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("(%P|%t) %p\n"),
                    ACE_TEXT ("send")));
      else
        mb->rd_ptr (ACE_static_cast (size_t, send_cnt));
      if (mb->length () > 0)
        {
          this->ungetq (mb);
          break;
        }
      mb->release ();
    }
  if (this->msg_queue ()->is_empty ())
    this->reactor ()->cancel_wakeup
      (this, ACE_Event_Handler::WRITE_MASK);
  else
    this->reactor ()->schedule_wakeup
      (this, ACE_Event_Handler::WRITE_MASK);
  return 0;
}
// Listing 5

// Listing 6 code/ch07
int ACE_TMAIN (int, ACE_TCHAR *[])
{
  ACE_INET_Addr port_to_connect ("HAStatus", ACE_LOCALHOST);
  ACE_Connector<Client, ACE_SOCK_CONNECTOR> connector;
  Client client;
  Client *pc = &client;
  if (connector.connect (pc, port_to_connect) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                       ACE_TEXT ("connect")), 1);

  ACE_Reactor::instance ()->run_reactor_event_loop ();
  return (0);
}
// Listing 6

// Listing 7 code/ch07
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Connector<Client, ACE_SOCK_CONNECTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
template class ACE_NonBlocking_Connect_Handler<Client>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
# pragma instantiate ACE_Connector<Client, ACE_SOCK_CONNECTOR>
# pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
# pragma instantiate ACE_NonBlocking_Connect_Handler<Client>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
// Listing 7
