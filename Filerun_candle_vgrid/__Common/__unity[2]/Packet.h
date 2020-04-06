#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


#ifndef FEELSOFT_TOOLS_PACKET
#define FEELSOFT_TOOLS_PACKET


//const char szEndTitle[4] = "EPC";

class Packet
{


public:

	// ��� ����
	char* m_pPacket;	// ��Ŷ
	HEAD* m_pHead;		// �ش�
	TAIL* m_pTail;		// ����
	
private:

	char* m_pData;		// ������

	// �޽��
public:

	Packet::Packet() : m_pPacket(NULL), m_pHead(NULL), m_pData(NULL), m_pTail(NULL) 
	{
	}
	
	Packet::~Packet() 
	{
		SAFE_DELETE(m_pPacket);
	}

	char* GetData() {  return m_pData; }
	int   GetCode() {  return m_pHead->code; }
	char* GetPacket() { return m_pPacket; }
	int   GetLength() { return m_pHead->ps; }	
	static int MinSize() { return sizeof(HEAD)+sizeof(TAIL); }

	// ��Ŷ �ۼ�
	int Packet::Pack(short code, const char* data = NULL, int datalen = 0)
	{
		try 
		{
			SAFE_DELETE(m_pPacket);

			m_pPacket = new char[sizeof(HEAD) + datalen + sizeof(TAIL)];
			memset(m_pPacket, 0, sizeof(HEAD) + datalen + sizeof(TAIL));


			// Header ����
			m_pHead			= (PHEAD)m_pPacket;
			m_pHead->code	= (unsigned int)code;
			m_pHead->ds		= (unsigned int)datalen;
			m_pHead->ps		= sizeof(HEAD) + (unsigned int)datalen + sizeof(TAIL);

			// Data ����
			if(datalen > 0 && data != NULL)
			{
				m_pData = (char*) m_pPacket+sizeof(HEAD);
				memcpy(m_pData, data, datalen);
			} 
			else if(data == NULL || datalen <= 0) 
			{
				m_pData = NULL;
			}
			
			// Tail ����
			m_pTail = (PTAIL) (m_pPacket + sizeof(HEAD) + datalen);		
			lstrcpy(m_pTail->key, szEndTitle);

			// ��ȿ�� ����
			IsValid();

		}
		catch(...) 
		{			
			delete m_pPacket;
			m_pPacket = NULL;m_pHead = NULL;m_pTail = NULL;m_pData = NULL; 
			return 103;
		}

		return 0;
	}

	// ��Ŷ �м�
	int Packet::Unpack(const char* buf, int buflen) {
		
		try 
		{
			SAFE_DELETE(m_pPacket);

			m_pPacket = new char[buflen];
			memcpy(m_pPacket, buf, buflen);

			// Header ����
			m_pHead = (PHEAD) m_pPacket;

			// Data ���� 
			if (m_pHead->ds > 0)
				m_pData = (char*) (m_pPacket + sizeof(HEAD));
			else
				m_pData = NULL;

			// Tail ����
			m_pTail = (PTAIL) (m_pPacket + sizeof(HEAD) + m_pHead->ds);

			// ��ȿ�� ����
			IsValid();

		} 
		catch(...) 
		{
			delete m_pPacket;
			m_pPacket = NULL;m_pHead = NULL;m_pTail = NULL;m_pData = NULL; 
			return 103;
		} 

		return 0;
	}


	// ��Ŷ �м�
	int Packet::Unpack(char* hbuf, char* dbuf, int buflen) {
		
		try 
		{	
			SAFE_DELETE(m_pPacket);

			m_pPacket = new char[buflen];

			memcpy(m_pPacket, hbuf, sizeof(HEAD));
			memcpy(m_pPacket + sizeof(HEAD), dbuf, buflen - sizeof(HEAD));

			// Header ����
			m_pHead = (PHEAD) m_pPacket;

			// Data ���� 
			if (m_pHead->ds > 0)
				m_pData = (char*) (m_pPacket + sizeof(HEAD));
			else
				m_pData = NULL;

			// Tail ����
			m_pTail = (PTAIL) (m_pPacket + sizeof(HEAD) + m_pHead->ds);

			// ��ȿ�� ����
			IsValid();

		} 
		catch(...) 
		{
			delete m_pPacket;
			m_pPacket = NULL;m_pHead = NULL;m_pTail = NULL;m_pData = NULL; 
			return 103;
		} 

		return 0;
	}


	// ��ȿ�� �˻�
	int Packet::IsValid()
	{
		// �ش��� ������ �����ڿ� ��ġ���� �ʴ� ���
		//if ( strcmp(szPacketID, m_pHead->key) && strcmp(szEndTitle, m_pTail->key) )
		//	throw;
		
		// ��Ŷ�� ũ��� ���۹��� ����Ÿ�� ũ�Ⱑ �ٸ� ��� 
		int tlen = m_pHead->ds+sizeof(HEAD)+sizeof(TAIL);
		if (tlen != m_pHead->ps)
			throw;
		
		// ��Ŷ�� �ּ� �ִ� ũ�⸦ �̴� �Ǵ� �ʰ��� ���
		if (tlen < sizeof(HEAD)+sizeof(TAIL))
			throw;

		// ��Ŷ���� üũ
		// DES ������� ���ڵ�
		// ASE ������� ���ڵ�

		return 0;
	}

	int Packet::Attach(Packet* src) { 
		
		try 
		{
			Pack(src->m_pHead->code, src->m_pData, src->m_pHead->ds);

		} 
		catch (...) 
		{
			return 103;
		}
		return 0;
	}
};


#endif