#ifndef FEELSOFT_TOOLS_TOKEN
#define FEELSOFT_TOOLS_TOKEN


#pragma once
#include <string>

#define MAX_TOKEN 1000000
static char *EmptyString = "";


class CTokenBig
{

protected:

	char m_strSep[50];							// ������	
	char* m_Token[MAX_TOKEN];					// �߶��� ��ū���� ������
	size_t m_nCount;							// ��ū�� ����	

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

	// ���� �ڸ��� ������ �ϴ� �Լ�
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

	// ������ ���ڿ��� ã�� - �����Լ�
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

	// ������ ��ū�� ��ȯ
	char* CTokenBig::GetToken(size_t nIndex)
	{
		if(nIndex >= m_nCount)	return EmptyString;

		return m_Token[nIndex];
	}

	// �����ڸ� ����
	void CTokenBig::SetSeperator(char* strSep)
	{
		strcpy(m_strSep, strSep);
	}

	// ��ū�� ������ ��ȯ
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