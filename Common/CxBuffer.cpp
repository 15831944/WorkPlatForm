#include "StdAfx.h"
#include "CxBuffer.h"


CCxBuffer::CCxBuffer( DWORD dwSize )
{
	m_dwSize = 0;
	m_pBuf = NULL;
	Create( dwSize );
}


CCxBuffer::~CCxBuffer(void)
{
	Release();
}

void CCxBuffer::Release( void )
{
	if ( m_pBuf != NULL )
	{
		delete[] m_pBuf; m_pBuf = NULL;
	}
	m_dwSize = 0;
}

BOOL CCxBuffer::Create( DWORD dwSize )
{
	Release();
	if ( dwSize == 0 ) return FALSE;
	
	m_dwSize = dwSize;
	m_pBuf = new char[ m_dwSize + 1 ];
	if ( m_pBuf == NULL )
	{
		m_dwSize = 0;
		return FALSE;
	}
	Empty();
	return TRUE;
}

void CCxBuffer::Empty( void )
{
	if ( m_pBuf == NULL || m_dwSize == 0 ) return;
	memset( m_pBuf, NULL, m_dwSize + 1 );
}

BOOL CCxBuffer::CopyTo( char* pTo, DWORD dwSize )
{
	if ( pTo == NULL || dwSize == 0 ) return FALSE;
	DWORD dwCopySize = min( m_dwSize, dwSize );

	if ( dwSize > m_dwSize )
	{
		memset( pTo + m_dwSize + 1, NULL, dwSize - m_dwSize );
	}
	memcpy( pTo, m_pBuf, dwCopySize );
	return TRUE;
}

BOOL CCxBuffer::Copy( char* pFrom, DWORD dwSize )
{
	Empty();

	if ( pFrom == NULL || dwSize == 0 ) return FALSE;
	DWORD dwCopySize = min( m_dwSize, dwSize );

	if ( m_dwSize > dwSize )
	{
		memset( m_pBuf + dwSize + 1, NULL, m_dwSize - dwSize );
	}
	memcpy( m_pBuf, pFrom, dwCopySize );
	return TRUE;
}