#include "icon.h"
#include <string>
using namespace std;

#define WIDTHBYTES(bits)      ((((bits) + 31)>>5)<<2)

class CExtract
{
public:
	CExtract(const char * pszExeFile);
	~CExtract();
	BOOL ExtractIcon(int iID,const char *pszOut);
public:
	BOOL         m_bUseDefaultIcon;
private:
	BOOL AdjustIconImagePointers( LPICONIMAGE lpImage );
	BOOL SaveIconResource(const char *pszIcoFile);
	void ClearData();
private:
	string m_strExeFile;
	LPICONRESOURCE m_LpIconResource;
	HMODULE      m_hLibrary;
	HRSRC        m_hRsrc;
};