// Message_Queue.cpp,v 1.2 2004/01/02 23:12:19 shuston Exp

#include "ace/SOCK_Acceptor.h"
#include "ace/Acceptor.h"
#include "Message_Receiver.h"

// Listing 5 code/ch12
int 
HA_CommandHandler::svc (void)
{
  while(1)
    {
      ACE_Message_Block *mb;
      if (this->getq (mb) == -1)
	break;
      if (mb->msg_type () == ACE_Message_Block::MB_HANGUP)
        {
          mb->release ();
          break;
        }
      else
        {
          // Get header pointer, then move past header to payload.
          DeviceCommandHeader *dch
            = (DeviceCommandHeader*)mb->rd_ptr ();
          mb->rd_ptr (sizeof (DeviceCommandHeader));
          ACE_DEBUG ((LM_DEBUG, 
                      ACE_TEXT ("Message for device #%d with ")
                      ACE_TEXT ("command payload of:\n%s"),
                      dch->deviceId_, mb->rd_ptr ()));
          this->rep_.update_device (dch->deviceId_,
                                    mb->rd_ptr ());
          mb->release ();
        }
    }

  ACE_Reactor::instance ()->end_reactor_event_loop ();

  return 0;
}
// Listing 5

// Listing 4 code/ch12
ACE_Message_Block * 
Message_Receiver::shut_down_message (void)
{
  ACE_Message_Block *mb;
  ACE_NEW_RETURN
    (mb, ACE_Message_Block (0, ACE_Message_Block::MB_HANGUP), 0);
  return mb;
}
// Listing 4
    
int 
Message_Receiver::read_header (DeviceCommandHeader *dch)
{
    ssize_t result = 
        this->peer ().recv_n (dch, sizeof (DeviceCommandHeader));
    if (result <= 0)
        ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                           ACE_TEXT ("Recieve Failure")),
                          -1);
    return 0;
}
// Listing 3 code/ch12
int 
Message_Receiver::copy_payload (ACE_Message_Block *mb,
                                int payload_length)
{
  int result = 
    this->peer ().recv_n (mb->wr_ptr (), payload_length);
    
    if (result <= 0)
      {
        mb->release ();
        return result;
      }

    mb->wr_ptr (payload_length);
    return 0;
}
// Listing 3
// Listing 2 code/ch12
int 
Message_Receiver::handle_input (ACE_HANDLE)
{
  DeviceCommandHeader dch;
  if (this->read_header (&dch) < 0)
    return -1;
    
  if (dch.deviceId_ < 0)
    {
      // Handle shutdown.
      this->handler_->putq (shut_down_message ());
      return -1;
    }
    
  ACE_Message_Block *mb;
  ACE_NEW_RETURN
    (mb, ACE_Message_Block (dch.length_ + sizeof dch), -1);
  // Copy the header.
  mb->copy ((const char*)&dch, sizeof dch);
  // Copy the payload.
  if (this->copy_payload (mb, dch.length_) < 0)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                       ACE_TEXT ("Recieve Failure")), -1);
  // Pass it off to the handler thread.
  this->handler_->putq (mb);
  return 0;
}
// Listing 2

static void report_usage (int argc, ACE_TCHAR *argv[])
{
  if (argc < 2)
    {
      ACE_DEBUG ((LM_ERROR, ACE_TEXT ("%s port\n"), argv[1]));
      ACE_OS::exit (-1);
    }
}


class Acceptor : public ACE_Acceptor<Message_Receiver, ACE_SOCK_ACCEPTOR> 
{
public:
  Acceptor(HA_CommandHandler *handler) : handler_(handler)
  { }

protected:
  virtual int make_svc_handler (Message_Receiver *&mr)
  {
    ACE_NEW_RETURN (mr, Message_Receiver (handler_), -1);
    return 0;
  }

private:
  HA_CommandHandler *handler_;
};

int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  report_usage (argc, argv);

  u_short port = ACE_OS::atoi (argv[1]);
    
  HA_Device_Repository rep;
  HA_CommandHandler handler (rep);
  ACE_ASSERT(handler.activate()==0);
  //start up the handler.
    
  Acceptor acceptor (&handler);
  ACE_INET_Addr addr (port);
  if (acceptor.open (addr) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                       ACE_TEXT ("Failed to open connection")), -1);
    
  ACE_Reactor::run_event_loop ();
  //run the reactive event loop
    
  handler.wait ();
  //reap the handler before exiting.

  return 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor<Message_Receiver, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor<Message_Receiver, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
