#ifndef _CRC32DYNAMIC_H_
#define _CRC32DYNAMIC_H_

#ifndef _QWORD_DEFINED
#define _QWORD_DEFINED
typedef __int64 QWORD, *LPQWORD;
#endif

#define MAKEQWORD(a, b)	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) ((DWORD)(l))
#define HIDWORD(l) ((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

#define MAX_BUFFER_SIZE			10240
#define MAX_BUFFER_SIZE_HEAD	1048576



class CCrc32Dynamic
{
public:
	CCrc32Dynamic();
	virtual ~CCrc32Dynamic();

	void Init(void);
	void Free(void);
	DWORD FileCrc32Assembly(LPCTSTR szFilename, DWORD &dwCrc32) const;

protected:

	DWORD *m_pdwCrc32Table;
};

#endif
