#ifndef FEELSOFT_TOOLS_TOKEN
#define FEELSOFT_TOOLS_TOKEN


#pragma once
#include <string>

#define MAX_TOKEN 1000000
static char *EmptyString = "";


class CTokenBig
{

protected:

	char m_strSep[50];							// 구분자	
	char* m_Token[MAX_TOKEN];					// 잘라진 토큰들의 포인터
	size_t m_nCount;							// 토큰의 갯수	

public:

	CTokenBig::CTokenBig(void)
	{
		strcpy(m_strSep, " ");
		m_nCount = 0;	
	}

	CTokenBig::CTokenBig(char* strSep)
	{
		strcpy(m_strSep, strSep);
		m_nCount = 0;	
	}

	CTokenBig::~CTokenBig(void)
	{
		ReleaseAllToken();
	}

	// 실제 자르는 역할을 하는 함수
	void CTokenBig::Split(char* strString)
	{
		ReleaseAllToken();

		char* pString = strString;		
		size_t nSep = strlen(m_strSep);
		size_t nPos = 0, nToken = 0;

		while(true)
		{		
			int nR = FindString(pString + nPos, m_strSep);

			if(nR != -1)	
			{
				int nLenToken = sizeof(*strString) * nR;

				m_Token[nToken] = new char[nLenToken + sizeof(*strString)];
				memcpy(m_Token[nToken], pString+nPos, nLenToken);
				m_Token[nToken++][nLenToken] = 0;
				nPos += (nSep + nR);
			}
			else
			{
				int nLenToken = lstrlen(pString+nPos);

				m_Token[nToken] = new char[nLenToken+sizeof(*strString)];
				memcpy(m_Token[nToken], pString+nPos, nLenToken);
				m_Token[nToken++][nLenToken] = 0;
				
				break;
			}
		}

		m_nCount = nToken;
	}

	// 지정된 문자열을 찾음 - 내부함수
	int CTokenBig::FindString(char* strMain, char* strSep)
	{
		char* pMain = (LPSTR)strMain;		
		size_t nLenSep = strlen(strSep);

		while(memcmp(pMain, strSep, nLenSep * sizeof(*strMain)) != 0)
		{
			if(*(pMain++) == 0)	return -1;
		}
		return (int)(pMain - strMain);
	}

	// 지정된 토큰을 반환
	char* CTokenBig::GetToken(size_t nIndex)
	{
		if(nIndex >= m_nCount)	return EmptyString;

		return m_Token[nIndex];
	}

	// 구분자를 설정
	void CTokenBig::SetSeperator(char* strSep)
	{
		strcpy(m_strSep, strSep);
	}

	// 토큰의 갯수를 반환
	size_t CTokenBig::GetCount(void)
	{
		return m_nCount;
	}

	void CTokenBig::ReleaseAllToken()
	{
		for(size_t i = 0; i < m_nCount; i++)
		{
			if(m_Token[i])	delete[] m_Token[i];
		}
	}
};

#endif