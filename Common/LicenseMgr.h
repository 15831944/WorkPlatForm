#if !defined(AFX_FLEXNET_H__B5E5E50B_7926_4248_BEFC_AACC09F635CC__INCLUDED_)
#define AFX_FLEXNET_H__B5E5E50B_7926_4248_BEFC_AACC09F635CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "stdafx.h"
#include "License.h"

#pragma comment(lib, "LicenseDll.lib")

class CLicenseMgr
{
public:
	CLicenseMgr();
	~CLicenseMgr();
public:
	BOOL CheckOutLicense(const char *feature);
	BOOL CheckInLicense(const char *feature);
	BOOL multiToUTF8(const std::string& multiText, std::string& utf8Text);
	BOOL UTF8Tomulti(const std::string& utf8Text, std::string& multiText);
	BOOL multiToUnicode(const std::string& multiText, std::wstring& unicodeText);
	BOOL UnicodeTomulti(const std::wstring& unicodeText, std::string& multiText);
	BOOL UTF8ToUnicode(const std::string& utf8Text, std::wstring& unicodeText);
	BOOL UnicodeToUTF8(const std::wstring& unicodeText, std::string& utf8Text);
	BOOL UnicodeToGB2312(const std::wstring& unicodeText, std::string& gb2312Text);
	BOOL GB2312ToUnicode(std::string& gb2312Text, std::wstring& unicodeText);
private:
	std::string m_feature;
};

extern CLicenseMgr theLicenseMgr;

extern BOOL InitLicense( LPCTSTR lpszFeatureName );
extern void ReleaseLicense();

#endif