// test_URL.cpp,v 1.4 2003/11/10 01:48:03 dhinton Exp

#include "Locator_Request_Reply.h"

ACE_RCSID(Caching, test_URL, "test_URL.cpp,v 1.4 2003/11/10 01:48:03 dhinton Exp")

int main (int argc, char *argv[])
{
  ACE_UNUSED_ARG (argc);
  ACE_UNUSED_ARG (argv);

  ACE_URL_Locator_Request original, derived;

  ACE_URL_Property_Seq ouch1(3);
  ouch1[0].name ("name 1");
  ouch1[0].value ("value 1");
  ouch1[1].name ("name 2");
  ouch1[1].value ("value 2");
  ouch1[2].name ("name 3");
  ouch1[2].value ("value 3");
 
  original.url_query (1, ouch1, 3);

  original.dump ();
  derived.dump ();

  original.encode ();

  derived.decode ((void*) original.buffer ());

  derived.dump ();

  return 0;
}
