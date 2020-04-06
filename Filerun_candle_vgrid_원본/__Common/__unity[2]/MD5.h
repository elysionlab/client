
#ifndef __MD5_HASH_H__
#define __MD5_HASH_H__

#pragma once

#include "ace/OS.h"
#if defined (ACE_HAS_WINNT4)
#	include <string>
#else

#endif


class CMD5  
{
	typedef struct 
	{
		unsigned int state[4];								/* state (ABCD) */
		unsigned int count[2];								/* number of bits, modulo 2^64 (lsb first) */
		unsigned char buffer[64];							/* input buffer */
	} 
	MD5_CTX;

public:

	CMD5();
	virtual ~CMD5();
	std::string GetString(std::string s);

protected:

	void MD5Init(MD5_CTX* ctx);
	void MD5Update(MD5_CTX* ctx, unsigned char* input, unsigned int inputlen);
	void MD5Final(unsigned char* digest, MD5_CTX* ctx);
};


#endif
