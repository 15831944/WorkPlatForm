#include "stdafx.h"
#include "ConvertData.h"
#include "PtrVectorHeader.h"

int CString2Vector(vector<CString>& vDest, LPCTSTR lpszSrc, LPCTSTR lpszSpliter)
{
	vDest.clear();

	CString strSrc = lpszSrc == NULL ? "" : lpszSrc;
	if (strSrc.IsEmpty()) return 0;

	CString strSpliter = lpszSpliter == NULL ? "" : lpszSpliter;
	if (strSpliter.IsEmpty()) return 0;

	CString token;
	int nPos = 0;
	token = strSrc.Tokenize((LPCTSTR)strSpliter, nPos);
	while (token != "")
	{
		vDest.push_back(token);
		token = strSrc.Tokenize((LPCTSTR)strSpliter, nPos);
	}
	return (int)vDest.size();
}

/*
功能：
	根据指定的分割符号，分割内存数据。注意，分割后，需要释放 vDest 中的数据
参数：
	vDest：分割后的数据。
	pSrc：要分割的内存数据。
	nSrcSize：要分割的内存数据字节个数。
	pSpliter：分割标志（字符数组）。支持特殊字符，比如 NULL 等
	nSpliterSize：分割标志个数。
返回：
	分割后的数据个数。
	vDest[i]->pData 指向分割好的每块数据。
	vDest[i]->nCount 分割好的每块数据字节个数。

调用例子：

	char buf[100];
	memset(buf, NULL, 100);
	。。。//原始数据赋值

	char spliter[3];
	spliter[0] = ','; //遇到逗号则分割
	spliter[1] = '?'; //遇到问号则分割
	spliter[2] = ':'; //遇到冒号则分割

	vector<DATABUF *> vDest;
	Buffer2Vector(vDest, buf, 100, spliter, 3);
	。。。//处理分好段的数据
	PtrVectorClear(vDest);
*/
int Buffer2Vector(vector<DATABUF *>& vDest, char * pSrc, int nSrcSize
	, char * pSpliter, int nSpliterSize)
{
	PtrVectorClear(vDest);

	if (pSrc == NULL || nSrcSize <= 0 
		|| pSpliter == NULL || nSpliterSize <= 0) return 0;

	//复制原buffer
	int nBufSize = nSrcSize + 1;
	char * pBuf = new char [nBufSize];
	memcpy(pBuf, pSrc, nSrcSize);
	pBuf[nSrcSize] = pSpliter[0]; //给整个buffer结尾处，增加一个分割标志，用于匹配最后一个数据

	DATABUF * pDataBuf = NULL;
	int nLeft = 0;
	int nRight = 0;
	while (nRight < nBufSize)
	{
		for (int i=0; i<nSpliterSize; i++)
		{
			if (pBuf[nRight] == pSpliter[i])
			{
				pDataBuf = new DATABUF();
				pDataBuf->nCount = nRight - nLeft;
				pDataBuf->pData = new char[pDataBuf->nCount + 1]; //每个数据段后边，增加一个 NULL 字符
				memcpy(pDataBuf->pData, &pBuf[nLeft], pDataBuf->nCount);
				pDataBuf->pData[pDataBuf->nCount] = NULL; //每个数据段后边，增加一个 NULL 字符
				
				vDest.push_back(pDataBuf);

				nLeft = nRight + 1;
				break;
			}
		}
		nRight++;
	}

	delete[] pBuf;
	pBuf = NULL;

	return (int)vDest.size();
}

int CString2VectorWithEmpty(vector<CString>& vDest, LPCTSTR lpszSrc, LPCTSTR lpszSpliter)
{
	vDest.clear();

	CString strSrc = lpszSrc == NULL ? "" : lpszSrc;
	if (strSrc.IsEmpty()) return 0;

	CString strSpliter = lpszSpliter == NULL ? "" : lpszSpliter;
	if (strSpliter.IsEmpty()) return 0;

	vector<DATABUF *> vDataBuf;
	Buffer2Vector(vDataBuf, (char *)(LPCTSTR)strSrc, strSrc.GetLength()
		, (char *)(LPCTSTR)strSpliter, strSpliter.GetLength());

	for (int i=0; i<(int)vDataBuf.size(); i++)
	{
		vDataBuf[i]->pData == NULL ? vDest.push_back("") : vDest.push_back(vDataBuf[i]->pData);
	}

	PtrVectorClear(vDataBuf);
	return (int)vDest.size();
}



//将BSTR类型的数据转换为CString类型
BOOL ConvertBSTR2CString(CString& strResult, const BSTR& pSrc)
{
	strResult = "";

	try
	{
		if (NULL == pSrc) return FALSE;
		char * pTemp = NULL;
		pTemp = _com_util::ConvertBSTRToString(pSrc);
		if (NULL == pTemp) return FALSE;
		strResult = pTemp;
		delete[] pTemp;
		pTemp = NULL;
		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}

}


CComVariant CString2Variant(LPCSTR lpszValue)
{
	CComVariant variant((LPCSTR)lpszValue);
	return variant;
}

CComVariant CStringW2Variant(LPCWSTR lpszValue)
{
	CComVariant variant((LPCOLESTR)lpszValue);
	return variant;
}


vector<CStringW> CStringW2Vector(LPCWSTR lpszSrc, LPCWSTR lpszSplit)
{
	vector<CStringW> vData;
	CStringW strSrc = lpszSrc == NULL ? L"" : lpszSrc;
	CStringW strSplit = lpszSplit == NULL ? L"" : lpszSplit;
	CStringW strToken;
	int nPos = 0;
	strToken = strSrc.Tokenize((LPCWSTR)strSplit, nPos);
	while (strToken != "")
	{
		vData.push_back(strToken);
		strToken = strSrc.Tokenize((LPCWSTR)strSplit, nPos);
	}
	return vData;
}

//
// CStringA转CStringW
//
CStringW CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;

	return cstrDestW;
}

//
// CStringW转CStringA
//
CStringA CStrW2CStrA(const CStringW &cstrSrcW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len);
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	delete[] str;

	return cstrDestA;
}

#include <comdef.h>
std::string ws2s_1(const std::wstring& ws)
{
	_bstr_t t = ws.c_str();
	char* pchar = (char*)t;
	std::string result = pchar;
	return result;
}

std::wstring s2ws_1(const std::string& s)
{
	_bstr_t t = s.c_str();
	wchar_t* pwchar = (wchar_t*)t;
	std::wstring result = pwchar;
	return result;
}

CString Variant2CString( const _variant_t& var )
{
	CString str;
	if ( var.vt == VT_NULL || var.vt == VT_EMPTY ) return "";
	str = (LPCTSTR)(_bstr_t)var;
	return str;
}