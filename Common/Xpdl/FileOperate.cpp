#include "stdafx.h"
#include "FileOperate.h"
#include <Strsafe.h>
#include <Dbghelp.h>

#pragma warning( disable : 4996 )


/*
FileOperationFun:
lpszFrom:
lpszTo:
iOperateFlag: FO_COPY,	FO_DELETE,	FO_MOVE,	FO_RENAME
*/
BOOL FileOperationFun( LPCTSTR lpszFrom, LPCTSTR lpszTo, UINT iOperateFlag )
{
	int nFunRet = 0;
	char* pFrom = NULL;
	char* pTo = NULL;
	char* pFindDir = NULL;
	char* pFromDir = NULL;
	char* pToDir = NULL;
	HANDLE hFile = NULL;
	HANDLE hFileDir = NULL;
	if ( iOperateFlag == FO_COPY || iOperateFlag == FO_MOVE )
	{
		__try
		{
			if ( lpszFrom == NULL || lpszTo == NULL )
			{
				nFunRet = 1;
				__leave;
			}
			pFrom = new char[MAX_PATH*2];
			pTo = new char[MAX_PATH*2];
			ZeroMemory( pFrom, MAX_PATH*2 );
			ZeroMemory( pTo, MAX_PATH*2 );
			StringCbCopy( pFrom, MAX_PATH*2-4, lpszFrom );
			StringCbCopy( pTo, MAX_PATH*2-4, lpszTo );

			if ( pFrom[ _tcslen( pFrom )-1 ] == '\\' )
			{
				pFrom[ _tcslen( pFrom )-1 ] = '\0';
			}
			if ( pTo[ _tcslen( pTo )-1 ] == '\0' )
			{
				pTo[ _tcslen( pTo )-1 ] = '\0';
			}

			WIN32_FIND_DATA FIND_DATA = {0};
			hFile = FindFirstFile( pFrom, &FIND_DATA );
			if ( hFile == INVALID_HANDLE_VALUE )
			{
				nFunRet = 2;
				__leave;
			}
			if ( FIND_DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				pFindDir = new char[ MAX_PATH*2 ];
				pFromDir = new char[ MAX_PATH*2 ];
				pToDir = new char[ MAX_PATH*2 ];
				ZeroMemory( pFindDir, MAX_PATH*2 );
				ZeroMemory( pFromDir, MAX_PATH*2 );
				ZeroMemory( pToDir, MAX_PATH*2 );

				StringCbCopy( pFindDir, MAX_PATH*2-4, pFrom );
				StringCbCat( pFindDir, MAX_PATH*2-4, "\\*.*" );
				memset( &FIND_DATA, 0, sizeof( WIN32_FIND_DATA ) );
				hFileDir = FindFirstFile( pFindDir, &FIND_DATA );
				if ( hFileDir == INVALID_HANDLE_VALUE )
				{
					nFunRet = 3;
					__leave;
				}
				do 
				{
					if ( stricmp( FIND_DATA.cFileName, "." ) != 0 && stricmp( FIND_DATA.cFileName, ".." ) != 0 )
					{
						StringCbCopy( pFromDir, MAX_PATH*2-4, pFrom );
						StringCbCat( pFromDir, MAX_PATH*2-4, "\\" );
						StringCbCat( pFromDir, MAX_PATH*2-4, FIND_DATA.cFileName );
						StringCbCopy( pToDir, MAX_PATH*2-4, pTo );
						StringCbCat( pToDir, MAX_PATH*2-4, "\\" );
						StringCbCat( pToDir, MAX_PATH*2-4, FIND_DATA.cFileName );
						MakeSureDirectoryPathExists( pTo );
						if ( !FileOperationFun( pFromDir, pToDir, iOperateFlag ) )
						{
							nFunRet = 4;
							__leave;
						}
					}
				} while ( FindNextFile( hFileDir, &FIND_DATA ) );
			}
			else
			{
				SHFILEOPSTRUCT FILE_STRUCT = {0};
				FILE_STRUCT.wFunc = iOperateFlag;
				FILE_STRUCT.pFrom = pFrom;
				FILE_STRUCT.pTo = pTo;
				FILE_STRUCT.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
				MakeSureDirectoryPathExists( pTo );
				int nRet = SHFileOperation( &FILE_STRUCT );
				if ( nRet != 0 )
				{
					nFunRet = 5;
					__leave;
				}
			}
		}
		__except(1)
		{

		}
	}
	else if ( iOperateFlag == FO_DELETE )
	{
		__try
		{
			if ( lpszFrom == NULL )
			{
				nFunRet = 6;
				__leave;
			}
			pFrom = new char[MAX_PATH*2];
			ZeroMemory( pFrom, MAX_PATH*2 );
			StringCbCopy( pFrom, MAX_PATH*2-4, lpszFrom );

			if ( pFrom[_tcslen( pFrom )-1 ] == '\\' )
			{
				pFrom[ _tcslen( pFrom )-1 ] = '\0';
			}
			WIN32_FIND_DATA FIND_DATA = {0};
			hFile = FindFirstFile( pFrom, &FIND_DATA );
			if ( hFile == INVALID_HANDLE_VALUE )
			{
				nFunRet = 7;
				__leave;
			}
			if ( FIND_DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				pFindDir = new char[ MAX_PATH*2 ];
				ZeroMemory( pFindDir, MAX_PATH*2 );
				StringCbCopy( pFindDir, MAX_PATH*2-4, pFrom );
				StringCbCat( pFindDir, MAX_PATH*2-4, "\\*.*" );
				memset( &FIND_DATA, 0, sizeof( WIN32_FIND_DATA ) );
				hFileDir = FindFirstFile( pFindDir, &FIND_DATA );
				if ( hFileDir == INVALID_HANDLE_VALUE )
				{
					nFunRet = 8;
					__leave;
				}
				do 
				{
					if ( stricmp( FIND_DATA.cFileName, "." ) != 0 && stricmp( FIND_DATA.cFileName, ".." ) != 0 )
					{
						pFromDir = new char[ MAX_PATH*2 ];
						ZeroMemory( pFromDir, MAX_PATH*2 );
						StringCbCopy( pFromDir, MAX_PATH*2-4, pFrom );
						StringCbCat( pFromDir, MAX_PATH*2-4, "\\");
						StringCbCat( pFromDir, MAX_PATH*2-4, FIND_DATA.cFileName );
						if ( !FileOperationFun( pFromDir, NULL, iOperateFlag ) )
						{
							nFunRet = 9;
							__leave;
						}
					}
				} while ( FindNextFile( hFileDir, &FIND_DATA ) );

				if ( !RemoveDirectory( pFrom ) )
				{
					nFunRet = 10;
					__leave;
				}
			}
			else
			{
				SHFILEOPSTRUCT FILE_STRUCT = {0};
				FILE_STRUCT.wFunc = iOperateFlag;
				FILE_STRUCT.pFrom = pFrom;
				FILE_STRUCT.pTo = pTo;
				FILE_STRUCT.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT | FOF_ALLOWUNDO | FOF_NORECURSION;
				int nRet = SHFileOperation( &FILE_STRUCT );
				if ( nRet != 0 )
				{
					nFunRet = 10;
					__leave;
				}
			}
		}
		__except(1)
		{

		}
	}
	else if ( iOperateFlag == FO_RENAME )
	{
		//²»Ö§³Ö
	}
	else
	{

	}

	if ( pFrom != NULL )
	{
		delete [] pFrom;
		pFrom = NULL;
	}
	if ( pTo != NULL )
	{
		delete [] pTo;
		pTo = NULL;
	}
	if ( pFindDir != NULL )
	{
		delete [] pFindDir;
		pFindDir = NULL;
	}
	if ( pFromDir != NULL )
	{
		delete [] pFromDir;
		pFromDir = NULL;
	}
	if ( pToDir != NULL )
	{
		delete [] pToDir;
		pToDir = NULL;
	}
	if ( hFile != NULL && hFile != INVALID_HANDLE_VALUE )
	{
		FindClose( hFile );
		hFile = NULL;
	}
	if ( hFileDir != NULL && hFileDir != INVALID_HANDLE_VALUE )
	{
		FindClose( hFileDir );
		hFileDir = NULL;
	}
	return !nFunRet;
}