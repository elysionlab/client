#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


#ifndef FEELSOFT_TOOLS_PACKET
#define FEELSOFT_TOOLS_PACKET


//const char szEndTitle[4] = "EPC";

class Packet
{


public:

	// 멤버 변수
	char* m_pPacket;	// 패킷
	HEAD* m_pHead;		// 해더
	TAIL* m_pTail;		// 테일
	
private:

	char* m_pData;		// 데이터

	// 메쏘드
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

	// 패킷 작성
	int Packet::Pack(short code, const char* data = NULL, int datalen = 0)
	{
		try 
		{
			SAFE_DELETE(m_pPacket);

			m_pPacket = new char[sizeof(HEAD) + datalen + sizeof(TAIL)];
			memset(m_pPacket, 0, sizeof(HEAD) + datalen + sizeof(TAIL));


			// Header 섹션
			m_pHead			= (PHEAD)m_pPacket;
			m_pHead->code	= (unsigned int)code;
			m_pHead->ds		= (unsigned int)datalen;
			m_pHead->ps		= sizeof(HEAD) + (unsigned int)datalen + sizeof(TAIL);

			// Data 섹션
			if(datalen > 0 && data != NULL)
			{
				m_pData = (char*) m_pPacket+sizeof(HEAD);
				memcpy(m_pData, data, datalen);
			} 
			else if(data == NULL || datalen <= 0) 
			{
				m_pData = NULL;
			}
			
			// Tail 섹션
			m_pTail = (PTAIL) (m_pPacket + sizeof(HEAD) + datalen);		
			lstrcpy(m_pTail->key, szEndTitle);

			// 유효성 검증
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

	// 패킷 분석
	int Packet::Unpack(const char* buf, int buflen) {
		
		try 
		{
			SAFE_DELETE(m_pPacket);

			m_pPacket = new char[buflen];
			memcpy(m_pPacket, buf, buflen);

			// Header 섹션
			m_pHead = (PHEAD) m_pPacket;

			// Data 섹션 
			if (m_pHead->ds > 0)
				m_pData = (char*) (m_pPacket + sizeof(HEAD));
			else
				m_pData = NULL;

			// Tail 섹션
			m_pTail = (PTAIL) (m_pPacket + sizeof(HEAD) + m_pHead->ds);

			// 유효성 검증
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


	// 패킷 분석
	int Packet::Unpack(char* hbuf, char* dbuf, int buflen) {
		
		try 
		{	
			SAFE_DELETE(m_pPacket);

			m_pPacket = new char[buflen];

			memcpy(m_pPacket, hbuf, sizeof(HEAD));
			memcpy(m_pPacket + sizeof(HEAD), dbuf, buflen - sizeof(HEAD));

			// Header 섹션
			m_pHead = (PHEAD) m_pPacket;

			// Data 섹션 
			if (m_pHead->ds > 0)
				m_pData = (char*) (m_pPacket + sizeof(HEAD));
			else
				m_pData = NULL;

			// Tail 섹션
			m_pTail = (PTAIL) (m_pPacket + sizeof(HEAD) + m_pHead->ds);

			// 유효성 검증
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


	// 유효성 검사
	int Packet::IsValid()
	{
		// 해더와 테일의 구분자와 일치하지 않는 경우
		//if ( strcmp(szPacketID, m_pHead->key) && strcmp(szEndTitle, m_pTail->key) )
		//	throw;
		
		// 패킷의 크기와 전송받은 데이타의 크기가 다른 경우 
		int tlen = m_pHead->ds+sizeof(HEAD)+sizeof(TAIL);
		if (tlen != m_pHead->ps)
			throw;
		
		// 패킷의 최소 최대 크기를 미달 또는 초과한 경우
		if (tlen < sizeof(HEAD)+sizeof(TAIL))
			throw;

		// 패킷변조 체크
		// DES 방식으로 인코딩
		// ASE 방식으로 디코딩

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