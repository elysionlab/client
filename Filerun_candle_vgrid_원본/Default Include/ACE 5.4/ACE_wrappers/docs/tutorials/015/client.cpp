
// client.cpp,v 1.5 2003/11/09 20:44:19 dhinton Exp

/* The Client object will implement the nasty details of connecting to
   communicating with the server
*/
#include "Client_i.h"
#include "ace/OS_NS_string.h"

int main(int argc, char *argv[])
{
    // How many messages will we send?
    int mcount = argc > 1 ? ACE_OS::atoi(argv[1]) : 3;

    // Construct a Client with our desired endpoint.
    Client client(ACE_DEFAULT_SERVER_PORT,ACE_DEFAULT_SERVER_HOST);

    // Attempt to open the connection to the server.
    if( client.open() == -1 )
    {
        ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "Client::open()"), -1);
    }

    // Send a few messages to the server and get some responses...
    for( int i = 0 ; i < mcount ; ++i )
    {
        // Since we'll be using a Protocol Stream (even though we
        // don't know that at this level) we require the use of
        // ACE_Message_Block objects to send/receive data.
        ACE_Message_Block * message = new ACE_Message_Block( 1024 );

        // Construct a silly message to send to the server.
        // Notice that we're careful to add one to the strlen() so
        // that we also send the end-of-string NULL character.
        ACE_OS::sprintf (message->wr_ptr (),
                         "This is message %d. It is a bit verbose because "
                          "we want there to be an opportunity for some compression to occur. "
                          "That's why it has a whofflly style and goes on and on and on.",
                         i);
        message->wr_ptr (ACE_OS::strlen (message->rd_ptr ())+1);

        // client will take ownership of the message block so that
        // we don't have to remember to release().  We *do* have
        // to remember not to use it after put() since it may be
        // released almost immediately.
        if (client.put( message ) < 0)
        {
            fprintf(stderr, "%s:%d: client put failed.\n", __FILE__, __LINE__);
            return -1;
        }

        ACE_Message_Block * response;

        // get() takes an ACE_Message_Block pointer reference.  We then
        // assume ownership of it and must release() when we're done.

        if( client.get( response ) == -1 )
        {
            ACE_DEBUG ((LM_INFO, "(%P|%t) Failed to get response from server\n" ));
            break;
        }

        ACE_DEBUG ((LM_INFO, "(%P|%t) The server's response:  (%s)\n",
                    response->rd_ptr()));

            // Now that we're through with the response we have to
            // release() it to avoid memory leaks.
        response->release();
    }

    ACE_DEBUG ((LM_INFO, "(%P|%t) Shutting down the stream\n" ));

        // Before we exit, it's a good idea to properly close() the connection.
    if( client.close() == -1 )
    {
        ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "Client::close()"), -1);
    }

    return(0);
}
