#ifndef FEELSOFT_TOOLS_TOKEN
#define FEELSOFT_TOOLS_TOKEN


#pragma once
#include <string>

#define MAX_TOKEN 500
static char *EmptyString = "";


class CToken
{

protected:

	char m_strSep[50];							// ������	
	char* m_Token[MAX_TOKEN];					// �߶��� ��ū���� ������
	size_t m_nCount;							// ��ū�� ����	

public:

	CToken::CToken(void)
	{
		lstrcpy(m_strSep, " ");
		m_nCount = 0;	
	}

	CToken::CToken(char* strSep)
	{
		lstrcpy(m_strSep, strSep);
		m_nCount = 0;	
	}

	CToken::~CToken(void)
	{
		ReleaseAllToken();
	}

	// ���� �ڸ��� ������ �ϴ� �Լ�
	void CToken::Split(char* strString)
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

	// ������ ���ڿ��� ã�� - �����Լ�
	int CToken::FindString(char* strMain, char* strSep)
	{
		char* pMain = (LPSTR)strMain;		
		size_t nLenSep = strlen(strSep);

		while(memcmp(pMain, strSep, nLenSep * sizeof(*strMain)) != 0)
		{
			if(*(pMain++) == 0)	return -1;
		}
		return (int)(pMain - strMain);
	}

	// ������ ��ū�� ��ȯ
	char* CToken::GetToken(size_t nIndex)
	{
		if(nIndex >= m_nCount)	return EmptyString;

		return m_Token[nIndex];
	}

	// �����ڸ� ����
	void CToken::SetSeperator(char* strSep)
	{
		lstrcpy(m_strSep, strSep);
	}

	// ��ū�� ������ ��ȯ
	size_t CToken::GetCount(void)
	{
		return m_nCount;
	}

	void CToken::ReleaseAllToken()
	{
		for(size_t i = 0; i < m_nCount; i++)
		{
			if(m_Token[i])	delete[] m_Token[i];
		}

		m_nCount = 0;
	}
};

#endif