// Codecs_Test.cpp,v 4.5 2003/05/18 19:17:34 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    Codecs_Test.cpp
//
// = DESCRIPTION
//    Checks the functionality of the ACE Codecs class.
//
// = AUTHORS
//    Krishnakumar B <kitty@cs.wustl.edu>
//
// ============================================================================

#include "test_config.h"
#include "ace/Codecs.h"
#include "ace/Auto_Ptr.h"
#include "ace/ACE.h"

ACE_RCSID(tests, Codecs_Test, "Codecs_Test.cpp,v 4.5 2003/05/18 19:17:34 dhinton Exp")

// Don't change the strings thinking that they are typos

const ACE_Byte normal_stream[] = "This is a sample test stream, to test simple Base64 encoding";

const ACE_Byte one_padded_stream[] = "This stream is different from the above in that, it results in one padding character to be adde";

const ACE_Byte two_padded_stream[] = "This stream is different from the above in that, it results in two padding characters to be addedddd";

int
encode_decode_stream (const ACE_Byte* stream, size_t length)
{
  size_t encode_len = 0;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Input stream = %s\n"),
              stream));

  ACE_Byte* encodeBuf = ACE_Base64::encode (stream, length,
                                            &encode_len);
  if (encodeBuf == 0)
    {
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("Error in encoding stream\n")));
      return -1;
    }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Base64 encoded stream = %s\n"),
              encodeBuf));


  ACE_Auto_Basic_Array_Ptr<ACE_Byte> cleanup_encodeBuf (encodeBuf);

  size_t decode_len = 0;
  ACE_Byte* decodeBuf = ACE_Base64::decode (encodeBuf, &decode_len);

  if (decodeBuf == 0)
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("Error in decoding encoded stream\n")));
      return -1;
    }

  ACE_Auto_Basic_Array_Ptr<ACE_Byte> cleanup_decodeBuf (decodeBuf);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Decoded Base64 encoded stream = %s\n"),
              decodeBuf));

  for (size_t i = 0; i < length; ++i)
    if (decodeBuf[i] != stream[i])
    {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("Encoded->Decoded stream differs from original stream\n")));
      return -1;
    }
  return 0;
}

int
run_main (int argc, ACE_TCHAR *argv[])
{
  ACE_UNUSED_ARG (argc);
  ACE_UNUSED_ARG (argv);

  ACE_START_TEST (ACE_TEXT ("Codecs_Test"));
  int status = 0;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("This is ACE Version %u.%u.%u\n\n"),
              ACE::major_version (),
              ACE::minor_version(),
              ACE::beta_version()));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Testing ACE Base64 - normal stream\n\n")));

  status = encode_decode_stream (normal_stream, sizeof (normal_stream) - 1);

  if (status == 0) {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Testing ACE Base64 - one padded stream\n\n")));
    status = encode_decode_stream (one_padded_stream,
                                   sizeof (one_padded_stream) - 1);
  }
  if (status == 0) {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Testing ACE Base64 - two padded stream\n\n")));
    status = encode_decode_stream (two_padded_stream,
                                   sizeof (two_padded_stream) - 1);
  }
  ACE_END_TEST;
  return status;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Auto_Basic_Array_Ptr<ACE_Byte>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Auto_Basic_Array_Ptr<ACE_Byte>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
