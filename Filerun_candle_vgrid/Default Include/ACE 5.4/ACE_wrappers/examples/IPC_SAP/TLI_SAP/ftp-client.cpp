// ftp-client.cpp,v 4.8 2003/11/01 11:15:23 dhinton Exp

#include "ace/TLI_Connector.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_unistd.h"

ACE_RCSID(TLI_SAP, ftp_client, "ftp-client.cpp,v 4.8 2003/11/01 11:15:23 dhinton Exp")

#if defined (ACE_HAS_TLI)

int
main (int argc, char *argv[])
{
  if (argc < 2)
    ACE_OS::fprintf (stderr, "Usage: %s filename [server-host port-number]\n", argv[0]), ACE_OS::exit (1);

  const char *filename  = argv[1];
  const char *host_name = argc < 3 ? ACE_DEFAULT_SERVER_HOST : argv[2];
  u_short port = argc < 4 ? ACE_DEFAULT_SERVER_PORT : ACE_OS::atoi (argv[3]);

  ACE_TLI_Stream client;
  ACE_TLI_Connector con;
  int fd;
  char buf[BUFSIZ];

  if (con.connect (client, ACE_INET_Addr (port, host_name)) == -1)
    ACE_OS::t_error ((char *) host_name), ACE_OS::exit (1);

  if ((fd = ACE_OS::open (filename, O_RDONLY)) == -1)
    ACE_OS::perror (filename), ACE_OS::exit (1);

  for (int n; (n = ACE_OS::read (fd, buf, sizeof buf)) > 0; )
    if (client.send_n (buf, n) != n)
      ACE_OS::t_error ("client.send error");

  if (client.close () == -1)
    ACE_OS::t_error ("cli_close"), ACE_OS::exit (1);

  return 0;
}
#else
int main (int, char *[])
{
  ACE_ERROR_RETURN ((LM_ERROR,
                     "your platform isn't configured to support TLI\n"),
                    1);
}
#endif /* ACE_HAS_TLI */
