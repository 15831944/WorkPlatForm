#pragma once
#include <vector>
using namespace std;
#include <comdef.h>
using namespace ATL;

//�ڴ����ݶ�
typedef struct _DataBuf
{
	struct _DataBuf() : pData(NULL), nCount(0)
	{
	}
	~_DataBuf()
	{
		Clear();
	}

	void Clear(void)
	{
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
		nCount = 0;
	}

	char * pData; //�ڴ����ݶ��׵�ַ
	int nCount; //���ݶγ���
} DATABUF;

extern int CString2Vector(vector<CString>& vDest, LPCTSTR lpszSrc, LPCTSTR lpszSpliter = ",");
extern int Buffer2Vector(vector<DATABUF *>& vDest, char * pSrc, int nSrcSize
						 , char * pSpliter, int nSpliterSize);
extern int CString2VectorWithEmpty(vector<CString>& vDest, LPCTSTR lpszSrc, LPCTSTR lpszSpliter);
extern BOOL ConvertBSTR2CString(CString& strResult, const BSTR& pSrc);
extern CComVariant CString2Variant(LPCSTR lpszValue);
extern CComVariant CStringW2Variant(LPCWSTR lpszValue);
extern vector<CStringW> CStringW2Vector(LPCWSTR lpszSrc, LPCWSTR lpszSplit = L",");

#ifndef _CSTRING_TOOL_EX_
#define _CSTRING_TOOL_EX_

#include <cstringt.h>

//
// CStringתCStringA
//
#ifndef CStrT2CStrA
#ifdef _UNICODE
#define CStrT2CStrA(cstr) CStrW2CStrA((cstr))
#else
#define CStrT2CStrA(cstr) (cstr)
#endif
#endif

//
// CStringתCStringW
//
#ifndef CStrT2CStrW
#ifdef _UNICODE
#define CStrT2CStrW(cstr) (cstr)
#else
#define CStrT2CStrW(cstr) CStrA2CStrW((cstr))
#endif
#endif

//
// CStringAתCString
//
#ifndef CStrA2CStrT
#ifdef _UNICODE
#define CStrA2CStrT(cstr) CStrA2CStrW((cstr))
#else
#define CStrA2CStrT(cstr) (cstr)
#endif
#endif

//
// CStringWתCString
//
#ifndef CStrW2CStrT
#ifdef _UNICODE
#define CStrW2CStrT(cstr) (cstr)
#else
#define CStrW2CStrT(cstr) CStrW2CStrA((cstr))
#endif
#endif

//
// CStringAתCStringW
//
CStringW CStrA2CStrW(const CStringA &cstrSrcA);

//
// CStringWתCStringA
//
CStringA CStrW2CStrA(const CStringW &cstrSrcW);

#endif

extern std::string ws2s_1(const std::wstring& ws);
extern std::wstring s2ws_1(const std::string& s);

extern CString Variant2CString( const _variant_t& var );