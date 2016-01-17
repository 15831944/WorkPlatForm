#include "stdafx.h"
#include "ClassExtract.h"
#include "Tools.h"

typedef struct _TDGEnumFunParm
{ 
	_TDGEnumFunParm(int iUseIndex)
	{
		m_iNum = -1;
		m_pszID = NULL;
		m_iUseIndex = iUseIndex;
		m_bIsNumName = false;
		m_bOk = false;
	}
	int m_iNum;
	int m_iUseIndex;
	char *m_pszID;
	WORD  m_IDReturn;
	bool  m_bIsNumName;
	bool  m_bOk;
}EnumFunParm;
WORD DIBNumColors( LPSTR lpbi )
{
	WORD wBitCount;
	DWORD dwClrUsed;

	dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

	if (dwClrUsed)
		return (WORD) dwClrUsed;

	wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

	switch (wBitCount)
	{
	case 1: return 2;
	case 4: return 16;
	case 8:	return 256;
	default:return 0;
	}
	return 0;
}
WORD PaletteSize( LPSTR lpbi )
{
	return ( DIBNumColors( lpbi ) * sizeof( RGBQUAD ) );
}
LPSTR FindDIBBits( LPSTR lpbi )
{
	return ( lpbi + *(LPDWORD)lpbi + PaletteSize( lpbi ) );
}
DWORD BytesPerLine( LPBITMAPINFOHEADER lpBMIH )
{
	return WIDTHBYTES(lpBMIH->biWidth * lpBMIH->biPlanes * lpBMIH->biBitCount);
}

BOOL WriteICOHeader( HANDLE hFile, UINT nNumEntries )
{
	WORD    Output;
	DWORD	dwBytesWritten;

	// Write 'reserved' WORD
	Output = 0;
	if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
		return FALSE;
	// Did we write a WORD?
	if( dwBytesWritten != sizeof( WORD ) )
		return FALSE;
	// Write 'type' WORD (1)
	Output = 1;
	if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
		return FALSE;
	// Did we write a WORD?
	if( dwBytesWritten != sizeof( WORD ) )
		return FALSE;
	// Write Number of Entries
	Output = (WORD)nNumEntries;
	if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
		return FALSE;
	// Did we write a WORD?
	if( dwBytesWritten != sizeof( WORD ) )
		return FALSE;
	return TRUE;
}
/****************************************************************************
*
*     FUNCTION: CalculateImageOffset
*
*     PURPOSE:  Calculates the file offset for an icon image
*
*     PARAMS:   LPICONRESOURCE lpIR   - pointer to icon resource
*               UINT           nIndex - which image?
*
*     RETURNS:  DWORD - the file offset for that image
*
* History:
*                July '95 - Created
*
\****************************************************************************/
DWORD CalculateImageOffset( LPICONRESOURCE lpIR, UINT nIndex )
{
	DWORD	dwSize;
	UINT    i;

	// Calculate the ICO header size
	dwSize = 3 * sizeof(WORD);
	// Add the ICONDIRENTRY's
	dwSize += lpIR->nNumImages * sizeof(ICONDIRENTRY);
	// Add the sizes of the previous images
	for(i=0;i<nIndex;i++)
		dwSize += lpIR->IconImages[i].dwNumBytes;
	// we're there - return the number
	return dwSize;
}

BOOL CALLBACK MyEnumProcedure( HMODULE  hModule, LPCTSTR  lpszType, LPTSTR  lpszName, LONG_PTR lParam )	
{
	EnumFunParm * param =(EnumFunParm*)lParam;
	param->m_iNum++;
	if (param->m_iNum == param->m_iUseIndex)
	{
		if( HIWORD(lpszName) == 0 )
		{
			param->m_IDReturn = (WORD)lpszName;
			param->m_bIsNumName = true;
		}
		else
		{
			param->m_bIsNumName = false;
			param->m_pszID = _strdup(lpszName);
		}
		param->m_bOk = true;
		return FALSE;
	}
	param->m_bOk = false;
	return TRUE;
}
CExtract::CExtract(const char * pszExeFile)
{
	m_hLibrary = NULL;
	m_LpIconResource = NULL;
	m_hRsrc = NULL;

	if (NULL == pszExeFile)
		m_strExeFile = "";
	else
		m_strExeFile = pszExeFile;

	// Load the DLL/EXE - NOTE: must be a 32bit EXE/DLL for this to work
	m_hLibrary = LoadLibraryEx( m_strExeFile.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE );
}
CExtract::~CExtract()
{
	if (m_hLibrary)
	{
		FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}
	ClearData();
}
void CExtract::ClearData()
{
	m_hRsrc = NULL;
	m_bUseDefaultIcon = FALSE;
	if (m_LpIconResource)
	{
		for (UINT i =0; i< m_LpIconResource->nNumImages;i++ )
		{
			free(m_LpIconResource->IconImages[i].lpBits);
			m_LpIconResource->IconImages[i].lpBits = NULL;
		}
		free(m_LpIconResource);
		m_LpIconResource = NULL;
	}
}
BOOL CExtract::ExtractIcon(int iID,const char *pszOut)
{
	ClearData();

	if( NULL == m_hLibrary )
	{
		// 找不到图标时使用默认文档图标
		char szPath[1024] ={0};
		ExpandEnvironmentStrings("%SystemRoot%\\system32\\shell32.dll",szPath,1024);
		iID = 0; 
		m_hLibrary = LoadLibraryEx( szPath, NULL, LOAD_LIBRARY_AS_DATAFILE );
		if( NULL == m_hLibrary )
		  return FALSE;
		m_strExeFile = szPath;
		m_bUseDefaultIcon = TRUE;
		ZTools::WriteZToolsFormatLog("【%s】文件加载失败，使用【%s】的第0个图标代替\n",m_strExeFile.c_str(),szPath);
	}

	if (iID < 0)
	{
		iID = 0 - iID;
		m_hRsrc = FindResource( m_hLibrary, MAKEINTRESOURCE(iID), RT_GROUP_ICON );
		if( NULL == m_hRsrc)
			return FALSE;
	}
	else
	{
		EnumFunParm Param(iID);
		EnumResourceNames(m_hLibrary, RT_GROUP_ICON,MyEnumProcedure,(LONG_PTR)&Param);
		if (!Param.m_bOk)
			return FALSE;

		LPCTSTR strID;
		if (Param.m_bIsNumName)
			strID = MAKEINTRESOURCE(Param.m_IDReturn);
		else
			strID = Param.m_pszID;
		m_hRsrc = FindResource( m_hLibrary, strID, RT_GROUP_ICON );
		if (Param.m_pszID)
			free(Param.m_pszID);
		if( NULL == m_hRsrc)
			return FALSE;
	}
	

	// Find the group icon resource
	

	HGLOBAL        	hGlobal = NULL;
	if( (hGlobal = LoadResource( m_hLibrary, m_hRsrc )) == NULL )
		return FALSE;

	LPMEMICONDIR    lpIcon = NULL;
	if( (lpIcon = (LPMEMICONDIR)LockResource(hGlobal)) == NULL )
		return FALSE;

	// Allocate enough memory for the images
	m_LpIconResource = (LPICONRESOURCE)malloc( sizeof(ICONRESOURCE) + lpIcon->idCount * sizeof(ICONIMAGE));
	if( NULL == m_LpIconResource)
		return FALSE;

	// Fill in local struct members
	m_LpIconResource->nNumImages = lpIcon->idCount;
	lstrcpy( m_LpIconResource->szOriginalDLLFileName, m_strExeFile.c_str());
	lstrcpy( m_LpIconResource->szOriginalICOFileName, "" );

	// Loop through the images
	for(UINT i = 0; i < m_LpIconResource->nNumImages; i++ )
	{
		// Get the individual image
		if( (m_hRsrc = FindResource( m_hLibrary, MAKEINTRESOURCE(lpIcon->idEntries[i].nID), RT_ICON )) == NULL )
			return FALSE;

		if( (hGlobal = LoadResource( m_hLibrary, m_hRsrc )) == NULL )
			return FALSE;

		// Store a copy of the resource locally
		m_LpIconResource->IconImages[i].dwNumBytes = SizeofResource( m_hLibrary, m_hRsrc );
		m_LpIconResource->IconImages[i].lpBits = (LPBYTE)malloc( m_LpIconResource->IconImages[i].dwNumBytes );
		memcpy( m_LpIconResource->IconImages[i].lpBits, LockResource( hGlobal ), m_LpIconResource->IconImages[i].dwNumBytes );
		// Adjust internal pointers
		if( ! AdjustIconImagePointers( &(m_LpIconResource->IconImages[i]) ) )
			return FALSE;
	}

	return SaveIconResource(pszOut);
}
BOOL CExtract::AdjustIconImagePointers( LPICONIMAGE lpImage )
{
	// Sanity check
	if( lpImage==NULL )
		return FALSE;
	// BITMAPINFO is at beginning of bits
	lpImage->lpbi = (LPBITMAPINFO)lpImage->lpBits;
	// Width - simple enough
	lpImage->Width = lpImage->lpbi->bmiHeader.biWidth;
	// Icons are stored in funky format where height is doubled - account for it
	lpImage->Height = (lpImage->lpbi->bmiHeader.biHeight)/2;
	// How many colors?
	lpImage->Colors = lpImage->lpbi->bmiHeader.biPlanes * lpImage->lpbi->bmiHeader.biBitCount;
	// XOR bits follow the header and color table
	lpImage->lpXOR = (LPBYTE)FindDIBBits((LPSTR)lpImage->lpbi);
	// AND bits follow the XOR bits
	lpImage->lpAND = lpImage->lpXOR + (lpImage->Height*BytesPerLine((LPBITMAPINFOHEADER)(lpImage->lpbi)));
	return TRUE;
}
BOOL CExtract::SaveIconResource(const char *pszIcoFile)
{
	// open the file
	HANDLE    	hFile;
	if( (hFile = CreateFile( pszIcoFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
		return FALSE;

	// Write the header
	if( ! WriteICOHeader( hFile, m_LpIconResource->nNumImages ) )
	{
		CloseHandle( hFile );
		return FALSE;
	}
	// Write the ICONDIRENTRY's
	DWORD    	dwBytesWritten;
	for( UINT i=0; i<m_LpIconResource->nNumImages; i++ )
	{
		ICONDIRENTRY    ide;
		// Convert internal format to ICONDIRENTRY
		ide.bWidth = m_LpIconResource->IconImages[i].Width;
		ide.bHeight = m_LpIconResource->IconImages[i].Height;
		ide.bReserved = 0;
		ide.wPlanes = m_LpIconResource->IconImages[i].lpbi->bmiHeader.biPlanes;
		ide.wBitCount = m_LpIconResource->IconImages[i].lpbi->bmiHeader.biBitCount;
		if( (ide.wPlanes * ide.wBitCount) >= 8 )
			ide.bColorCount = 0;
		else
			ide.bColorCount = 1 << (ide.wPlanes * ide.wBitCount);
		ide.dwBytesInRes = m_LpIconResource->IconImages[i].dwNumBytes;
		ide.dwImageOffset = CalculateImageOffset( m_LpIconResource, i );
		// Write the ICONDIRENTRY out to disk
		if( ! WriteFile( hFile, &ide, sizeof( ICONDIRENTRY ), &dwBytesWritten, NULL ) )
			return FALSE;
		// Did we write a full ICONDIRENTRY ?
		if( dwBytesWritten != sizeof( ICONDIRENTRY ) )
			return FALSE;
	}
	// Write the image bits for each image
	for( UINT i=0; i<m_LpIconResource->nNumImages; i++ )
	{
		DWORD dwTemp = m_LpIconResource->IconImages[i].lpbi->bmiHeader.biSizeImage;

		// Set the sizeimage member to zero
		m_LpIconResource->IconImages[i].lpbi->bmiHeader.biSizeImage = 0;
		// Write the image bits to file
		if( ! WriteFile( hFile, m_LpIconResource->IconImages[i].lpBits, m_LpIconResource->IconImages[i].dwNumBytes, &dwBytesWritten, NULL ) )
			return FALSE;
		if( dwBytesWritten != m_LpIconResource->IconImages[i].dwNumBytes )
			return FALSE;
		// set it back
		m_LpIconResource->IconImages[i].lpbi->bmiHeader.biSizeImage = dwTemp;
	}
	CloseHandle( hFile );
	return TRUE;
}