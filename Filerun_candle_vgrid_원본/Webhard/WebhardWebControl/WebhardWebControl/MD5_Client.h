
#ifndef __MD5_HASH_H__
#define __MD5_HASH_H__

#pragma once



#define MAX_BUFFER_SIZE			10240
#define MAX_BUFFER_SIZE_HEAD	1048576



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
	CString GetString(CString sFileName, DWORD &dwErrorCode);

protected:

	void MD5Init(MD5_CTX* ctx);
	void MD5Update(MD5_CTX* ctx, unsigned char* input, unsigned int inputlen);
	void MD5Final(unsigned char* digest, MD5_CTX* ctx);
};


#endif
