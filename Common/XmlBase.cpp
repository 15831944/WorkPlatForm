#include "stdafx.h"
#include "XmlBase.h"
#include "atlenc.h"
#include "FileFunc.h"
#include <winuser.h>

static CString g_sAppTitle;

void SetCXmlBaseAppTitle(LPCTSTR lpszTitle)
{
	g_sAppTitle = lpszTitle == NULL ? "" : lpszTitle;
}

CXmlBase::CXmlBase(void)
{
	CoInitialize(NULL);
}

CXmlBase::~CXmlBase(void)
{
	CoUninitialize();
}

////取得应用程序当前目录
//CString CXmlBase::GetExecDir(void)
//{
//	CString strRet = _T("");
//	TCHAR exeFullPath[1024] = {0};
//	GetModuleFileName(NULL, exeFullPath, 1024);
//	strRet = exeFullPath;
//	int nPos = strRet.ReverseFind('\\');
//	strRet = strRet.Left(nPos);
//	return strRet;
//}
//
////文件路径是否存在
//BOOL CXmlBase::IsAccessibleDir(LPCTSTR strPath)
//{
//	WIN32_FIND_DATA fd;
//	HANDLE hFind;
//	BOOL bRet = FALSE;
//
//	if (strlen((char *)strPath) == 3)
//	{
//		char *p = (char *)strPath;
//		if (((*p > 'A' && *p < 'Z') || (*p > 'a' && *p < 'z'))
//			&& *(p+1) == ':'
//			&& *(p+2) == '\\')
//			return TRUE;
//		else
//			return FALSE;
//	}
//
//	hFind = FindFirstFile(strPath, &fd);
//	if ((hFind != INVALID_HANDLE_VALUE) && (FILE_ATTRIBUTE_DIRECTORY == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
//	{
//		bRet = TRUE;
//	}
//	FindClose(hFind);
//	return bRet;
//}
//
////文件是否存在
//BOOL CXmlBase::IsAccessibleFile(LPCTSTR lpszFile)
//{
//	BOOL bExist = TRUE;
//	//检查文件是否存在
//	WIN32_FIND_DATA wfd;
//	HANDLE hFile = NULL;
//	hFile = FindFirstFile( lpszFile, &wfd );
//	if( hFile == INVALID_HANDLE_VALUE )
//	{
//		bExist = FALSE;
//	}
//	FindClose( hFile );
//	return bExist;
//}
//
////创建子目录
//CString CXmlBase::CreateSubDir(LPCTSTR szParentDir, LPCTSTR szChildFolder)
//{
//	if (NULL == szParentDir || NULL == szChildFolder) return _T("");
//	CString str;
//	str.Format(_T("%s\\%s"), szParentDir, szChildFolder);
//	if (!IsAccessibleDir(str))
//	{
//		if(!CreateDirectory(str, NULL)) return _T("");
//		else if (!IsAccessibleDir(str)) return _T("");
//	}
//	return str;
//}
//
////取得缓存xml文件的目录
//CString CXmlBase::GetXmlDir(void)
//{
//	CString str = GetExecDir();
//	str += _T("\\_temp");
//	if (!IsAccessibleDir(str))
//	{
//		if(!CreateDirectory(str, NULL)) return _T("");
//		else if (!IsAccessibleDir(str)) return _T("");
//	}
//	return str;
//}

//xml文件中的标识符替换为特殊字符
void CXmlBase::XmlFlag2Symbol(CString &str)
{
	str.Replace(_T("&amp;"), _T("&"));
	str.Replace(_T("&lt;"), _T("<"));
	str.Replace(_T("&gt;"), _T(">"));
	str.Replace(_T("&quot;"), _T("\""));
	str.Replace(_T("&apos;"), _T("'"));
}

//xml文件中的特殊字符替换为标识符
void CXmlBase::Symbol2XmlFlag(CString &str)
{
	str.Replace(_T("&"), _T("&amp;"));
	str.Replace(_T("<"), _T("&lt;"));
	str.Replace(_T(">"), _T("&gt;"));
	str.Replace(_T("\""), _T("&quot;"));
	str.Replace(_T("'"), _T("&apos;"));
}

//将字节流转换为文件
BOOL CXmlBase::ConvertBSTR2File(const CString& sPathfilename, const BSTR& pSrc)
{
	char* pTemp;
	if (NULL == pSrc) return FALSE;
	pTemp = _com_util::ConvertBSTRToString(pSrc);
	if (NULL == pTemp) return FALSE;

	int nCodedLen = SysStringByteLen(pSrc); //编码的buffer长度
	if (nCodedLen <= 0) return FALSE;

	nCodedLen = Base64DecodeGetRequiredLength(nCodedLen);
	CString strSrc = pTemp;
	//if (!ConvertBSTR2CString(strSrc, pSrc)) return FALSE;

	int nDecodeLen = nCodedLen; //保存解码后的buffer
	char * pDecode = new char[nDecodeLen];
	memset(pDecode, NULL, nDecodeLen);
	
	BOOL bDecode = ATL::Base64Decode(strSrc, strSrc.GetLength(), (BYTE*)pDecode, &nDecodeLen);
	//WRITELOG("解码%s，解码后的数据长度为[%d]个字节", (bDecode ? _T("成功") : _T("失败")), nDecodeLen);

	delete[] pTemp;
	pTemp = NULL;

	DWORD nTotalWritten = 0;
//	DWORD nWriteSize = nDecodeLen;
	if (bDecode)
	{
		//写文件
		CFile file;
		if (file.Open((LPCTSTR)sPathfilename, CFile::modeCreate | CFile::modeWrite))
		{
			if (file.m_hFile == CFile::hFileNull) return FALSE;

//			DWORD nNumberOfBytesWritten = 0;
			//char *p = pDecode;
			file.SeekToBegin();
			file.Write(pDecode, nDecodeLen);
			file.Flush();
			nTotalWritten = nDecodeLen;

			//nWriteSize = 4096;
			//if (0 == nDecodeLen / 4096)
			//{
			//	nWriteSize = nDecodeLen % 4096;
			//}

			//while (nWriteSize > 0 
			//	&& WriteFile(file.m_hFile, p, nWriteSize, &nNumberOfBytesWritten, NULL) && nNumberOfBytesWritten > 0)
			//{
			//	p += nNumberOfBytesWritten;
			//	nTotalWritten += nNumberOfBytesWritten;
			//	if (0 == (nDecodeLen - nTotalWritten) / 4096)
			//	{
			//		nWriteSize = (nDecodeLen - nTotalWritten) % 4096;
			//	}
			//}
			file.Close();
		}
	}

	delete[] pDecode;
	pDecode = NULL;

	if (nTotalWritten != (DWORD)nDecodeLen || (!bDecode))
	{
		return FALSE;
	}
	return TRUE;
}

//将指定的文件内容转换为字节流，保存在 nDest 中
BOOL CXmlBase::ConvertFile2Variant(CComVariant& nDest, const CString& strPathfilename)
{
	nDest = _T("");
	//
	if (strPathfilename.IsEmpty()) return FALSE;
	
	//打开文件
	CFile file;
	if (!file.Open((LPCTSTR)strPathfilename
		, CFile::modeRead | CFile::shareDenyNone)) return FALSE;
	if (file.m_hFile == CFile::hFileNull) return FALSE;

	UINT nSize = (UINT)file.GetLength();
	char *pBuf = new char[nSize + 2];
	memset(pBuf, NULL, nSize + 2);
	file.SeekToBegin();
	char *p = pBuf;
	DWORD nNumberOfBytesRead = 0;
	UINT nTotalRead = 0;
	while (ReadFile(file.m_hFile, p, 1024 * 4, &nNumberOfBytesRead, NULL) && nNumberOfBytesRead > 0)
	{
		p += nNumberOfBytesRead;
		nTotalRead += nNumberOfBytesRead;
	}
	file.Close();

	if (nTotalRead == 0 || p == NULL) return FALSE;

	//base64
	int nEncodeLen = nTotalRead * 2;
	char * pEecode = new char[nEncodeLen];
	memset(pEecode, NULL, nEncodeLen);
	BOOL bEocode = ATL::Base64Encode((BYTE*)pBuf, nTotalRead, pEecode, &nEncodeLen);
	if (bEocode)
	{
		nDest = pEecode;
	}

	delete[] pEecode;
	delete[] pBuf;
	return TRUE;
}

//将BSTR类型的数据转换为CString类型
BOOL CXmlBase::ConvertBSTR2CString(CString& nDest, const BSTR& pSrc, BOOL bFlag)
{
	char* pTemp;
	if (NULL == pSrc) return FALSE;
	pTemp = _com_util::ConvertBSTRToString(pSrc);
	SysFreeString(pSrc);//release memory
	if (NULL == pTemp) return FALSE;
	nDest = pTemp;
	delete[] pTemp;
	pTemp = NULL;
	if (bFlag) XmlFlag2Symbol(nDest);
	return TRUE;
}

//将BSTR类型的数据转换为int类型
BOOL CXmlBase::ConvertBSTR2Int(int& nDest, const BSTR& pSrc)
{
	char* pTemp;
	if (NULL == pSrc) return FALSE;
	pTemp = _com_util::ConvertBSTRToString(pSrc);
	if (NULL == pTemp) return FALSE;
	nDest = atoi(pTemp);
	delete[] pTemp;
	pTemp = NULL;
	return TRUE;
}

//将BSTR类型的数据转换为BOOL类型
BOOL CXmlBase::ConvertBSTR2BOOL(BOOL& nDest, const BSTR& pSrc)
{
	BOOL bRet = TRUE;
	char* pTemp;
	if (NULL == pSrc) return FALSE;
	pTemp = _com_util::ConvertBSTRToString(pSrc);
	if (NULL == pTemp) return FALSE;
	
	if (0 == strcmp("false", pTemp))
		nDest = FALSE;
	else if (0 == strcmp("true", pTemp))
		nDest = TRUE;
	else
		bRet = FALSE;

	delete[] pTemp;
	pTemp = NULL;

	return bRet;
}
//
//CString CXmlBase::GetServerConfigData(const CString strFilePath, CString strAtrributeName)
//{
//	BSTR bstrFileName;
//	MSXML2::IXMLDOMDocument2Ptr pXMLPtr;
//	VARIANT_BOOL varRet = VARIANT_FALSE;
//	HRESULT hr = NULL;
//	CString strServerInfo;
//	_bstr_t bsValue;
//
//	// 创建接口
//	hr = pXMLPtr.CreateInstance(__uuidof(MSXML2::DOMDocument40));
//	pXMLPtr->async = VARIANT_FALSE;
//	bstrFileName = strFilePath.AllocSysString();
//
//	varRet = pXMLPtr->load(bstrFileName);
//	if (varRet)
//	{
//		_bstr_t bsAttr;
//		bsAttr = _com_util::ConvertStringToBSTR(strAtrributeName);
//		_bstr_t xPath = _T("/clientSetting/" + bsAttr + "/@url");
//		MSXML2::IXMLDOMNodePtr nodePtr = pXMLPtr->selectSingleNode(xPath);
//		if(nodePtr)
//		{
//			bsValue = nodePtr->Gettext();
//		}
//
//		SysFreeString(bsAttr);
//	}
//
//	char* pTemp = _com_util::ConvertBSTRToString(bsValue);
//	strServerInfo = pTemp;
//	delete pTemp;
//	pTemp = NULL;
//
//	SysFreeString(bsValue);
//	SysFreeString(bstrFileName);
//	pXMLPtr.Release();
//
//	return strServerInfo;
//}

vector<CString> CXmlBase::ConvertCStringToVector(CString str, LPCTSTR lpszSpliters)
{
	vector<CString> v;
	int nPos = 0;
	BOOL bLoop = TRUE;
	//CString sElement;

	if (lpszSpliters == NULL 
		|| str.IsEmpty()) return v;

	while (bLoop)
	{
		nPos = str.FindOneOf(lpszSpliters);
		switch (nPos)
		{
		case -1:
			v.push_back(str);
			bLoop = FALSE;
			break;
		default:
			v.push_back(str.Left(nPos));
			str = str.Right(str.GetLength() - nPos - 1);
			break;
		}

		//if (-1 != nPos)
		//{
		//	sElement = str.Left(nPos);
		//	v.push_back(sElement);
		//	str = str.Right(str.GetLength() - nPos - 1);
		//}
		//else
		//{
		//	v.push_back(str);
		//	str = _T("");
		//}
	}
	return v;
}

MSXML2::IXMLDOMDocument2Ptr CXmlBase::GetXmlDocPtrByFile(LPCTSTR lpszXmlPathFile
	, LPCTSTR lpszNamespace)
{
	CComBSTR bstrXml;
	CString str(_T(""));
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocument2Ptr pDoc;

	//载入到xml文档结构中
	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
	if (FAILED(hr))
	{
//		WRITELOG("END, FAILED TO INVOKE pDoc.CreateInstance");
		return NULL;
	}

	pDoc->async = VARIANT_TRUE; //加载方式为“同步”
	pDoc->validateOnParse = VARIANT_FALSE; //文档结构有错误时，停止解析。

	if (VARIANT_FALSE == pDoc->load(lpszXmlPathFile))
	{
		//WRITELOG("END, FAILED TO INVOKE pDoc->load(%s)", strXmlPathFile);
		return NULL;
	}

	if (lpszNamespace != NULL)
	{
		hr = pDoc->setProperty("SelectionLanguage", "XPath");
		if (FAILED(hr)) return NULL;
		hr = pDoc->setProperty("SelectionNamespaces", lpszNamespace);
		if (FAILED(hr)) return NULL;
	}

	return pDoc;
}

MSXML2::IXMLDOMDocument2Ptr CXmlBase::GetXmlDocPtrByStream(CComBSTR bstrXml
	, CString strXmlPathFile)
{
	CString str(_T(""));
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocument2Ptr pDoc;

	//载入到xml文档结构中
	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
	if (FAILED(hr))
	{
		//WRITELOG("END, FAILED TO INVOKE pDoc.CreateInstance");
		return NULL;
	}

	pDoc->async = VARIANT_FALSE; //加载方式为“同步”
	pDoc->validateOnParse = VARIANT_FALSE; //文档结构有错误时，停止解析。

	if (VARIANT_FALSE == pDoc->loadXML(bstrXml.m_str))
	{
		//WRITELOG("END, FAILED TO INVOKE pDoc->loadXML");
		return NULL;
	}

	if (!strXmlPathFile.IsEmpty())
	{
		if (S_OK != pDoc->save((LPCTSTR)strXmlPathFile))
		{
			//WRITELOG("END, FAILED TO INVOKE pDoc->save");
		}
	}
	return pDoc;
}

MSXML2::IXMLDOMNodePtr CXmlBase::GetRootNodePtr(MSXML2::IXMLDOMDocument2Ptr pDoc)
{
	MSXML2::IXMLDOMElementPtr pRoot;
	MSXML2::IXMLDOMNodePtr pNode;
	HRESULT hr = NULL;
	
	if (pDoc == NULL)
	{
		//WRITELOG("END, pDoc为空"); return NULL;
	}

	hr = pDoc->get_documentElement(&pRoot);
	if (FAILED(hr) || NULL == pRoot)
	{
//		WRITELOG("END, pDoc取得根节点失败");
		return NULL;
	}
	pNode = pRoot;
	return pNode;
}


//取得指定xpath的节点
MSXML2::IXMLDOMNodePtr CXmlBase::GetNodePtr(LPCTSTR lpszXPath 
	, MSXML2::IXMLDOMDocument2Ptr pDoc)
{
	if (pDoc == NULL || lpszXPath == NULL) return NULL;
	CComBSTR bstrText = _bstr_t(lpszXPath).GetBSTR();
	return pDoc->selectSingleNode(bstrText.m_str);
}


//
MSXML2::IXMLDOMNodeListPtr CXmlBase::GetNodeListPtr(MSXML2::IXMLDOMNodePtr pParentNode)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList;
	HRESULT hr = NULL;
	
	if (pParentNode == NULL)
	{
		//WRITELOG("END, pParentNode为空"); 
		return NULL;
	}
	hr = pParentNode->get_childNodes(&pNodeList);
	if (FAILED(hr) || NULL == pNodeList)
	{
		return NULL;
	}
	return pNodeList;
}

MSXML2::IXMLDOMNodeListPtr CXmlBase::GetNodeListPtr(LPCTSTR lpszXPath
	, MSXML2::IXMLDOMDocument2Ptr pDoc)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList;
	CComBSTR bstrText;
	
	if (lpszXPath == NULL)
	{
		return NULL;
	}
	if (pDoc == NULL)
	{
		return NULL;
	}
	bstrText = _bstr_t(lpszXPath).GetBSTR();
	pNodeList = pDoc->selectNodes(bstrText.m_str);
	return pNodeList;
}

int CXmlBase::GetAttrsOfNode(map<CString, CComBSTR>& mapAttrs, MSXML2::IXMLDOMNodePtr pNode)
{
	MSXML2::IXMLDOMNamedNodeMapPtr pAttrs;
	MSXML2::IXMLDOMNodePtr pAttr;
	HRESULT hr;
	CComBSTR bstrText;
	CString str;
	long lAttrCount = 0;

	mapAttrs.clear();
	
    if (NULL == pNode)
	{
		//WRITELOG("END, pNode为空");
		return 0;
	}

	pAttrs = pNode->Getattributes();
	if (pAttrs == NULL)
	{
		//WRITELOG("END, 取得节点属性集合失败");
		return 0;
	}

	lAttrCount = pAttrs->Getlength();
	for (int i=0; i<lAttrCount; i++)
	{
		pAttr = pAttrs->Getitem(i);
		if (pAttr == NULL)
		{
			//WRITELOG("取得第[%d]个属性为空", i); 
			continue;
		}

		hr = pAttr->get_nodeName(&bstrText);
		if (FAILED(hr))
		{
			//WRITELOG("取得第[%d]个属性名为空", i); 
			continue;
		}
		if (!ConvertBSTR2CString(str, bstrText.m_str))
		{
			//WRITELOG("转换第[%d]个属性名 失败", i); 
			continue;
		}
		hr = pAttr->get_text(&bstrText);
		if (FAILED(hr))
		{
			//WRITELOG("取得第[%d]个属性值 失败，属性[%s]", i, str); 
			continue;
		}
		mapAttrs.insert(make_pair(str, bstrText));
	}
	
	return (int)mapAttrs.size();
}


int CXmlBase::GetAttrsOfNode(map<CString, CString>& mapAttrs, MSXML2::IXMLDOMNodePtr pNode)
{
	MSXML2::IXMLDOMNamedNodeMapPtr pAttrs;
	MSXML2::IXMLDOMNodePtr pAttr;
	HRESULT hr;
	CComBSTR bstrText;
	CString str, strName, strValue;
	long lAttrCount = 0;

	mapAttrs.clear();

	if (NULL == pNode)
	{
		//WRITELOG("END, pNode为空");
		return 0;
	}

	pAttrs = pNode->Getattributes();
	if (pAttrs == NULL)
	{
		//WRITELOG("END, 取得节点属性集合失败");
		return 0;
	}

	lAttrCount = pAttrs->Getlength();
	for (int i=0; i<lAttrCount; i++)
	{
		pAttr = pAttrs->Getitem(i);
		if (pAttr == NULL)
		{
			//WRITELOG("取得第[%d]个属性为空", i); 
			continue;
		}

		hr = pAttr->get_nodeName(&bstrText);
		if (FAILED(hr))
		{
			//WRITELOG("取得第[%d]个属性名为空", i); 
			continue;
		}
		if (!ConvertBSTR2CString(strName, bstrText.m_str))
		{
			//WRITELOG("转换第[%d]个属性名 失败", i); 
			continue;
		}
		hr = pAttr->get_text(&bstrText);
		if (FAILED(hr))
		{
			//WRITELOG("取得第[%d]个属性值 失败，属性[%s]", i, str); 
			continue;
		}
		if (!ConvertBSTR2CString(strValue, bstrText.m_str))
		{
			//WRITELOG("转换第[%d]个属性名 失败", i); 
			continue;
		}
		mapAttrs.insert(make_pair(strName, strValue));
	}

	return (int)mapAttrs.size();
}

//遍历指定节点列表的数据
BOOL CXmlBase::FetchNodeList2(MSXML2::IXMLDOMNodePtr pParentNode, DWORD dwFlag)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	long lNodeCount = 0;
	HRESULT hr = S_OK;

	if (NULL == pParentNode)
	{
		return FALSE;
	}
	pNodeList = GetNodeListPtr(pParentNode);
	if (pNodeList == NULL)
	{
		return FALSE;
	}

	lNodeCount = pNodeList->Getlength();
	for (long i=0; i<lNodeCount; i++)
	{
		hr = pNodeList->get_item(i, &pNode);
		if (FAILED(hr) || NULL == pNode)
		{
			return FALSE;
		}
		if (!OnFetchNodeList2(pNode, dwFlag))
		{
			return FALSE;
		}
	}
	return TRUE;
}

//遍历指定节点列表的数据
BOOL CXmlBase::OnFetchNodeList2(MSXML2::IXMLDOMNodePtr pNode, DWORD dwFlag)
{
	return TRUE;
}

//遍历指定节点列表的数据
BOOL CXmlBase::FetchNodeList(LPCTSTR lpszXmlPathFile, LPCTSTR lpszNamespace
	, LPCTSTR lpszNodeListXPath, DWORD dwFlag)
{
	MSXML2::IXMLDOMDocument2Ptr pDoc = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	long lNodeCount = 0;
	HRESULT hr = S_OK;

	if (!IsAccessibleFile(lpszXmlPathFile)) return FALSE;

	pDoc = GetXmlDocPtrByFile(lpszXmlPathFile, lpszNamespace);
	if (pDoc == NULL) return FALSE;

	pNodeList = GetNodeListPtr(lpszNodeListXPath, pDoc);
	if (pNodeList == NULL) return FALSE;

	lNodeCount = pNodeList->Getlength();
	for (int i=0; i<lNodeCount; i++)
	{
		hr = pNodeList->get_item(i, &pNode);
		if (FAILED(hr) || NULL == pNode) return FALSE;
		if (!OnFetchNodeList(pNode, dwFlag)) return FALSE;
	}
	return TRUE;//(lNodeCount != 0);
}

//遍历指定节点列表的数据
BOOL CXmlBase::OnFetchNodeList(MSXML2::IXMLDOMNodePtr pNode, DWORD dwFlag)
{
	return TRUE;
}

BOOL CXmlBase::SetNameSpace(MSXML2::IXMLDOMDocument2Ptr& pDoc, LPCTSTR lpszNameSpace)
{
	HRESULT hr = S_OK;
	if (lpszNameSpace == NULL 
		|| pDoc == NULL) return FALSE;
	
	hr = pDoc->setProperty("SelectionLanguage", "XPath");
	if (FAILED(hr)) return FALSE;
	hr = pDoc->setProperty("SelectionNamespaces", lpszNameSpace);
	if (FAILED(hr)) return FALSE;
	return TRUE;
}

//设置指定xpath的节点值
BOOL CXmlBase::SetNodeValue(LPCTSTR lpszValue, LPCTSTR lpszXPath 
	, MSXML2::IXMLDOMDocument2Ptr pDoc)
{
	if (pDoc == NULL || lpszXPath == NULL) return FALSE;
	MSXML2::IXMLDOMNodeListPtr pNodeList = pDoc->selectNodes(lpszXPath);
	long lCnt = pNodeList->Getlength();
	if (lCnt == 0) return FALSE;
	MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(0);
	return SetNodeValue(lpszValue, pNode);
}

//设置指定xpath的节点值
BOOL CXmlBase::SetNodeValue(LPCTSTR lpszValue, MSXML2::IXMLDOMNodePtr pNode)
{
	if (pNode == NULL) return FALSE;
	if (lpszValue == NULL) return FALSE;
	MSXML2::IXMLDOMElementPtr pElem = pNode;
	pElem->Puttext(lpszValue);
	return TRUE;
}

//设置指定xpath的节点属性
BOOL CXmlBase::SetAttrValue(LPCTSTR lpszValue, LPCTSTR lpszAttrName, LPCTSTR lpszXPath 
	, MSXML2::IXMLDOMDocument2Ptr pDoc)
{
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	long lCnt = 0;
	if (pDoc == NULL || lpszXPath == NULL) return FALSE;
	try
	{
		pNodeList = pDoc->selectNodes(lpszXPath);
		lCnt = pNodeList->Getlength();
		if (lCnt == 0) return FALSE;
		pNode = pNodeList->Getitem(0);
	}
	catch (_com_error e)
	{
		::MessageBox(NULL, e.ErrorMessage(), NULL, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	return SetAttrValue(lpszValue, lpszAttrName, pNode);
}

//设置指定xpath的节点属性
BOOL CXmlBase::SetAttrValue(LPCTSTR lpszValue, LPCTSTR lpszAttrName, MSXML2::IXMLDOMNodePtr pNode)
{
	if (lpszValue == NULL 
		|| lpszAttrName == NULL 
		|| pNode == NULL) return FALSE;
	MSXML2::IXMLDOMElementPtr pElem = pNode;
	//lc,国际化乱码问题
// 	CComVariant varText(_bstr_t(lpszValue).GetBSTR());
// 	HRESULT hr = pElem->setAttribute(_bstr_t(lpszAttrName).GetBSTR(), varText);
	BSTR bstr = SysAllocString(_bstr_t(lpszValue).GetBSTR());   //modify memory leak
	CComVariant varText(bstr);
	BSTR bstrName =SysAllocString(_bstr_t(lpszAttrName).GetBSTR());
	HRESULT hr = pElem->setAttribute(bstrName, varText);
	SysFreeString(bstr);
	bstr = NULL;
	SysFreeString(bstrName);
	bstrName = NULL;
	if (FAILED(hr)) return FALSE;
	return TRUE;
}


//取得节点
MSXML2::IXMLDOMNodePtr CXmlBase::GetNodePtr(LPCTSTR lpszAttrName, LPCTSTR lpszAttrValue
	, MSXML2::IXMLDOMNodePtr pParentNode)
{
	if (lpszAttrName == NULL 
		|| lpszAttrValue == NULL 
		|| pParentNode == NULL) return NULL;

	CComBSTR strCompare = _bstr_t(lpszAttrValue).GetBSTR();
	map<CString, CComBSTR> mapAttrs;
	MSXML2::IXMLDOMNodePtr pNode;
	for (pNode = pParentNode->GetfirstChild(); 
		pNode != NULL; 
		pNode = pNode->GetnextSibling())
	{
		GetAttrsOfNode(mapAttrs, pNode);
		if (mapAttrs[lpszAttrName] != strCompare) continue;
		return pNode;
	}
	return NULL;
}

//取得节点
MSXML2::IXMLDOMNodePtr CXmlBase::GetFirstChildNodeWithoutComment(MSXML2::IXMLDOMNodePtr pParentNode)
{
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	static const _bstr_t bstr_commentstr("#comment");
	if (pParentNode == NULL) return NULL;
	try
	{
		pNode = pParentNode->GetfirstChild();
		while (pNode != NULL)
		{
			if (pNode->GetnodeName() == bstr_commentstr)
			{
				pNode = pNode->GetnextSibling();
				continue;
			}
			break;
		}
	}
	catch (...)
	{
		return NULL;
	}
	return pNode;
}


MSXML2::IXMLDOMDocument2Ptr CXmlBase::CreateXmlDocPtr(void)
{
	::CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pDoc = NULL;
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument40));
	if (FAILED(hr))
	{
		return NULL;
	}

	pDoc->async = VARIANT_FALSE; //加载方式为“同步”
	pDoc->validateOnParse = VARIANT_FALSE; //文档结构有错误时，停止解析。
	//pDoc->preserveWhiteSpace = VARIANT_TRUE;
	return pDoc;
}


MSXML2::IXMLDOMNodePtr CXmlBase::CreateXmlTitle(MSXML2::IXMLDOMDocument2Ptr pDoc)
{
	MSXML2::IXMLDOMProcessingInstructionPtr pi = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;

	if (pDoc == NULL) return NULL;
	try
	{
		pi = pDoc->createProcessingInstruction(_T("xml")
			, _T("version=\"1.0\" encoding=\"UTF-8\""));
		if (pi == NULL) return NULL;
		
		pNode = pi;
		if (NULL == (pNode = pDoc->appendChild(pNode)))
		{
			pi.Release();
			pNode.Release();
			return NULL;
		}
		pi.Release();
		//pNode.Release();
	}
	catch(_com_error &e)
	{
		CString str;
		str.Format(_T("创建xml头发生异常。\r\nComError:\r\n%s")
			, e.ErrorMessage());
		::MessageBox( NULL,str, g_sAppTitle, MB_OK|MB_ICONERROR);
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pNode;
}


MSXML2::IXMLDOMNodePtr CXmlBase::CreateRootNode(MSXML2::IXMLDOMDocument2Ptr pDoc
	, LPCTSTR lpszRootText)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	CComBSTR bstrRootText = _bstr_t("root").GetBSTR();

	if (pDoc == NULL) return NULL;

	if (lpszRootText != NULL)
		bstrRootText = lpszRootText;

	if (bstrRootText.Length() == 0) return NULL;

	try
	{
		//hr = pDoc->raw_createElement(bstrRootText.m_str, &pElement);
		//if (FAILED(hr) || pElement == NULL) return NULL;
		//pNode = pElement;
		pNode = pDoc->createNode(NODE_ELEMENT, bstrRootText.m_str, _T(""));
		if (pNode == NULL) return NULL;
		//hr = pDoc->raw_appendChild(pNode, NULL);
		//if (FAILED(hr)) return NULL;
		pNode = pDoc->appendChild(pNode);
	}
	catch(_com_error &e)
	{
		CString str;
		str.Format(_T("创建xml根节点发生异常。\r\nComError:\r\n%s")
			, e.ErrorMessage());
		::MessageBox( NULL, str, g_sAppTitle, MB_OK|MB_ICONERROR);
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pNode;
}


MSXML2::IXMLDOMNodePtr CXmlBase::CreateNode(MSXML2::IXMLDOMNodePtr pParentNode
	, LPCTSTR lpszNodeText)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocument2Ptr pDoc = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	CComBSTR bstrNodeText = _bstr_t("node").GetBSTR();

	if (pParentNode == NULL 
		|| lpszNodeText == NULL) return NULL;

	bstrNodeText = lpszNodeText;
	if (bstrNodeText.Length() == 0) return NULL;

	try
	{
		pDoc = pParentNode->GetownerDocument();
		if (pDoc == NULL) return NULL;
		//hr = pDoc->raw_createElement(bstrNodeText.m_str, &pElement);
		//if (FAILED(hr) || pElement == NULL) return NULL;
		pNode = pDoc->createNode(NODE_ELEMENT, bstrNodeText.m_str, _T(""));
		if (pNode == NULL) return NULL;
		pNode = pParentNode->appendChild(pNode);
	}
	catch(_com_error &e)
	{
		CString str;
		str.Format(_T("创建xml节点发生异常。\r\nComError:\r\n%s")
			, e.ErrorMessage());
		::MessageBox(NULL,str, g_sAppTitle, MB_OK|MB_ICONERROR);
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pNode;
}

MSXML2::IXMLDOMNodePtr CXmlBase::InsertNode( MSXML2::IXMLDOMNodePtr pParentNode , MSXML2::IXMLDOMNodePtr pNodeBack , LPCTSTR lpszNodeText )
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocument2Ptr pDoc = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	CComBSTR bstrNodeText = _bstr_t("node").GetBSTR();

	if (pParentNode == NULL 
		|| lpszNodeText == NULL) return NULL;

	bstrNodeText = lpszNodeText;
	if (bstrNodeText.Length() == 0) return NULL;

	try
	{
		pDoc = pParentNode->GetownerDocument();
		if (pDoc == NULL) return NULL;
		//hr = pDoc->raw_createElement(bstrNodeText.m_str, &pElement);
		//if (FAILED(hr) || pElement == NULL) return NULL;
		pNode = pDoc->createNode(NODE_ELEMENT, bstrNodeText.m_str, _T(""));
		if (pNode == NULL) return NULL;
		CComVariant varNode = pNodeBack.GetInterfacePtr();
		pNode = pParentNode->insertBefore(pNode, varNode);
	}
	catch(_com_error &e)
	{
		CString str;
		str.Format(_T("插入xml节点发生异常。\r\nComError:\r\n%s")
			, e.ErrorMessage());
		::MessageBox(NULL,str, g_sAppTitle, MB_OK|MB_ICONERROR);
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pNode;
}

MSXML2::IXMLDOMNodePtr CXmlBase::InsertEnterAndTabs(MSXML2::IXMLDOMNodePtr pParentNode, int nLevel)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocument2Ptr pDoc = NULL;
	MSXML2::IXMLDOMTextPtr pTabsText = NULL;
	MSXML2::IXMLDOMNodePtr pTabsNode = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	CString strText(_T("")), strTabs(_T(""));
	CComBSTR bstrText;

	if (nLevel < 0 
		|| pParentNode == NULL) return NULL;

	for (int i=0; i<nLevel; i++)
	{
		strTabs += _T("\t");
	}
	strText.Format(_T("%s%s"), _T("\r\n"), (LPCTSTR)strTabs);
	bstrText = _bstr_t(strText).GetBSTR();

	try
	{
		pDoc = pParentNode->GetownerDocument();
		if (pDoc == NULL) return NULL;

		pTabsText = pDoc->createTextNode((LPCTSTR)strText);
		if (pTabsText == NULL) return NULL;
		pTabsNode = pParentNode->appendChild(pTabsText);

		
		//hr = pDoc->raw_createTextNode(bstrText.m_str, &pTabsText);
		//if (FAILED(hr) || pTabsText == NULL) return NULL;
		//pElement = pTabsText;
		//pTabsNode = pParentNode->appendChild(pElement);

		//pTabsNode = pDoc->createTextNode((LPCTSTR)strText);  
		//if (pTabsNode == NULL) return NULL;
		//pNode = pNode->appendChild(pTabsNode);
	}
	catch(_com_error &e)
	{
		CString str;
		str.Format(_T("创建xml节点发生异常。\r\nComError:\r\n%s")
			, e.ErrorMessage());
		::MessageBox(NULL,str, g_sAppTitle, MB_OK|MB_ICONERROR);
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pTabsNode;
}


MSXML2::IXMLDOMNodePtr CXmlBase::InsertEnterAndTabs(MSXML2::IXMLDOMNodePtr pParentNode, MSXML2::IXMLDOMNodePtr pNode, int nLevel)
{
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMDocument2Ptr pDoc = NULL;
	MSXML2::IXMLDOMTextPtr pTabsText = NULL;
	MSXML2::IXMLDOMNodePtr pTabsNode = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	CString strText(_T("")), strTabs(_T(""));
	CComBSTR bstrText;

	if (nLevel < 0 
		|| pParentNode == NULL) return NULL;

	if(pNode == NULL)nLevel = 0;

	for (int i=0; i<nLevel; i++)
	{
		strTabs += _T("\t");
	}
	strText.Format(_T("%s%s"), _T("\r\n"), (LPCTSTR)strTabs);
	bstrText = _bstr_t(strText).GetBSTR();

	try
	{
		pDoc = pParentNode->GetownerDocument();
		if (pDoc == NULL) return NULL;

		pTabsText = pDoc->createTextNode((LPCTSTR)strText);
		if (pTabsText == NULL) return NULL;
		CComVariant varNode = pNode.GetInterfacePtr();
		pTabsNode = pParentNode->insertBefore(pTabsText, varNode);


		//hr = pDoc->raw_createTextNode(bstrText.m_str, &pTabsText);
		//if (FAILED(hr) || pTabsText == NULL) return NULL;
		//pElement = pTabsText;
		//pTabsNode = pParentNode->appendChild(pElement);

		//pTabsNode = pDoc->createTextNode((LPCTSTR)strText);  
		//if (pTabsNode == NULL) return NULL;
		//pNode = pNode->appendChild(pTabsNode);
	}
	catch(_com_error &e)
	{
		CString str;
		str.Format(_T("创建xml节点发生异常。\r\nComError:\r\n%s")
			, e.ErrorMessage());
		::MessageBox(NULL,str, g_sAppTitle, MB_OK|MB_ICONERROR);
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}
	return pTabsNode;
}

