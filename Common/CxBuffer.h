#pragma once
class CCxBuffer
{
public:
	CCxBuffer( DWORD dwSize = 0 );
	~CCxBuffer(void);
	void Release(void);
	BOOL Create( DWORD dwSize );
	void Empty(void);
	BOOL CopyTo( char* pTo, DWORD dwSize );
	BOOL Copy( char* pFrom, DWORD dwSize );
public:
	char* m_pBuf;
	DWORD m_dwSize;
};

