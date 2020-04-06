#include "stdafx.h"
#include "Crc32Dynamic.h"
#include <fstream>


CCrc32Dynamic::CCrc32Dynamic() : m_pdwCrc32Table(NULL)
{
}


CCrc32Dynamic::~CCrc32Dynamic()
{
	Free();
}


void CCrc32Dynamic::Init(void)
{
	DWORD dwPolynomial = 0xEDB88320;
	int i, j;

	Free();
	m_pdwCrc32Table = new DWORD[256];

	DWORD dwCrc;
	for(i = 0; i < 256; i++)
	{
		dwCrc = i;
		for(j = 8; j > 0; j--)
		{
			if(dwCrc & 1)
				dwCrc = (dwCrc >> 1) ^ dwPolynomial;
			else
				dwCrc >>= 1;
		}
		m_pdwCrc32Table[i] = dwCrc;
	}
}


void CCrc32Dynamic::Free(void)
{
	delete m_pdwCrc32Table;
	m_pdwCrc32Table = NULL;
}

DWORD CCrc32Dynamic::FileCrc32Assembly(LPCTSTR szFilename, DWORD &dwCrc32) const
{
	_ASSERTE(szFilename);
	_ASSERTE(lstrlen(szFilename));

	DWORD dwErrorCode = NO_ERROR;

	CFile pFileData;
	ULONGLONG nFileSize = 0;
	BYTE buffer[MAX_BUFFER_SIZE];
	DWORD dwBytesRead;
	BOOL  bRun = TRUE;


	dwCrc32 = 0xFFFFFFFF;

	try
	{
		if(m_pdwCrc32Table == NULL)
			throw 0;

		if(!pFileData.Open(szFilename,  CFile::modeRead | CFile::shareDenyNone))
		{
			dwErrorCode = GetLastError();
		}
		else
		{
			dwBytesRead = pFileData.Read(buffer, sizeof(buffer));

			while(dwBytesRead > 0)
			{
				__asm
				{
					push esi
					push edi

					mov eax, dwCrc32		
					mov ecx, [eax]			

					mov ebx, this			
					mov edi, [ebx]CCrc32Dynamic.m_pdwCrc32Table

					lea esi, buffer			
					mov ebx, dwBytesRead	
					lea edx, [esi + ebx]		

				crc32loop:
					xor eax, eax			
					mov bl, byte ptr [esi]		
					
					mov al, cl					
					inc esi					

					xor al, bl				
					shr ecx, 8

					mov ebx, [edi + eax * 4]
					xor ecx, ebx				

					cmp edx, esi			
					jne crc32loop

				
					pop edi
					pop esi

					mov eax, dwCrc32		
					mov [eax], ecx			
				}



				nFileSize += dwBytesRead;

				if(nFileSize > MAX_BUFFER_SIZE_HEAD)
				{
					if(pFileData.GetLength() > MAX_BUFFER_SIZE_HEAD + MAX_BUFFER_SIZE && bRun)
					{
						pFileData.Seek(pFileData.SeekToEnd() - MAX_BUFFER_SIZE, CFile::begin);
						bRun = FALSE;
					}
					else
					{
						break;
					}
				}
					
				dwBytesRead = pFileData.Read(buffer, sizeof(buffer));
			}
		}

		pFileData.Close();

	}
	catch(...)
	{
		dwErrorCode = ERROR_CRC;
	}

	dwCrc32 = ~dwCrc32;
	return dwErrorCode;
}
