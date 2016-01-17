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
���ܣ�
	����ָ���ķָ���ţ��ָ��ڴ����ݡ�ע�⣬�ָ����Ҫ�ͷ� vDest �е�����
������
	vDest���ָ������ݡ�
	pSrc��Ҫ�ָ���ڴ����ݡ�
	nSrcSize��Ҫ�ָ���ڴ������ֽڸ�����
	pSpliter���ָ��־���ַ����飩��֧�������ַ������� NULL ��
	nSpliterSize���ָ��־������
���أ�
	�ָ������ݸ�����
	vDest[i]->pData ָ��ָ�õ�ÿ�����ݡ�
	vDest[i]->nCount �ָ�õ�ÿ�������ֽڸ�����

�������ӣ�

	char buf[100];
	memset(buf, NULL, 100);
	������//ԭʼ���ݸ�ֵ

	char spliter[3];
	spliter[0] = ','; //����������ָ�
	spliter[1] = '?'; //�����ʺ���ָ�
	spliter[2] = ':'; //����ð����ָ�

	vector<DATABUF *> vDest;
	Buffer2Vector(vDest, buf, 100, spliter, 3);
	������//����ֺöε�����
	PtrVectorClear(vDest);
*/
int Buffer2Vector(vector<DATABUF *>& vDest, char * pSrc, int nSrcSize
	, char * pSpliter, int nSpliterSize)
{
	PtrVectorClear(vDest);

	if (pSrc == NULL || nSrcSize <= 0 
		|| pSpliter == NULL || nSpliterSize <= 0) return 0;

	//����ԭbuffer
	int nBufSize = nSrcSize + 1;
	char * pBuf = new char [nBufSize];
	memcpy(pBuf, pSrc, nSrcSize);
	pBuf[nSrcSize] = pSpliter[0]; //������buffer��β��������һ���ָ��־������ƥ�����һ������

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
				pDataBuf->pData = new char[pDataBuf->nCount + 1]; //ÿ�����ݶκ�ߣ�����һ�� NULL �ַ�
				memcpy(pDataBuf->pData, &pBuf[nLeft], pDataBuf->nCount);
				pDataBuf->pData[pDataBuf->nCount] = NULL; //ÿ�����ݶκ�ߣ�����һ�� NULL �ַ�
				
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



//��BSTR���͵�����ת��ΪCString����
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
// CStringAתCStringW
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
// CStringWתCStringA
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