
#ifndef __CBASE64_H__
#define __CBASE64_H__

#pragma once 

class CBase64
{
public:
	CBase64();
	~CBase64();

	int base64_decode(char* text, unsigned char* dst, int numBytes);
	char* base64_encode(char *text, int numBytes);

};

#endif