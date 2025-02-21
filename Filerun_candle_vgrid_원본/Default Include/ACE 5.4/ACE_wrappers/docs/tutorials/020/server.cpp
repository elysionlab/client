
// server.cpp,v 1.5 2003/11/09 20:44:19 dhinton Exp

#include "mmap.h"

#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"

int
main (int, char *[])
{
        /*
          The default behavior of the memory map wrapper is to create
          the file if it doesn't exist.  This is a minor change from
          the SV shared memory wrapper.
        */
    ACE_Shared_Memory_MM shm_server (SHM_KEY, SHMSZ);
    char *shm = (char *) shm_server.malloc ();
    char *s   = shm;

    ACE_DEBUG ((LM_INFO, "(%P|%t) Memory Mapped file is at 0x%x\n",
                shm ));

    for (char c = 'a'; c <= 'z'; c++)
        *s++ = c;

    *s = '\0';

    while (*shm != '*')
        ACE_OS::sleep (1);

    for (s = shm; *s != '\0'; s++)
    {
        putchar (*s);
    }

    putchar ('\n');

    if (shm_server.remove () < 0)
        ACE_ERROR ((LM_ERROR, "%p\n", "remove"));

    return 0;
}
