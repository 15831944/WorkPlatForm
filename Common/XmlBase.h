/************************************************************************
使用说明：
1、添加头文件路径：
	..\CXImageD\CxImage
2、添加头文件路径或者对应的头文件：
	PtrVectorHeader.h
	CommonFunctionFile.h
3、添加库目录：
	..\LibD
4、添加库：
	cximage.lib Jpeg.lib png.lib Tiff.lib zlib.lib jbig.lib jasper.lib j2k.lib
5、确保系统中已经安装了微软的 Xml解析库：msxml.msi

************************************************************************/
#pragma once
#pragma warning(disable: 4278)
#include <vector>
#include <map>
using namespace std;

//#import "msxml4.dll"
#include "msxml4.tlh"

extern void SetCXmlBaseAppTitle(LPCTSTR lpszTitle);

class CXmlBase
{
public:
	CXmlBase(void);
public:
	virtual ~CXmlBase(void);
public:
	//CString GetExecDir(void);
	//BOOL IsAccessibleDir(LPCTSTR strPath);
	//BOOL IsAccessibleFile(LPCTSTR lpszFile);
	//CString GetXmlDir(void);
	//CString CreateSubDir(LPCTSTR szParentDir, LPCTSTR szChildFolder);

	void XmlFlag2Symbol(CString &str);
	void Symbol2XmlFlag(CString &str);

	BOOL ConvertBSTR2File(const CString& sPathfilename, const BSTR& pSrc);
	BOOL ConvertFile2Variant(CComVariant& nDest, const CString& strPathfilename);
	
	BOOL ConvertBSTR2CString(CString& nDest, const BSTR& pSrc, BOOL bFlag = TRUE);
	BOOL ConvertBSTR2Int(int& nDest, const BSTR& pSrc);
	BOOL ConvertBSTR2BOOL(BOOL& nDest, const BSTR& pSrc);

	//CString GetServerConfigData(const CString strFilePath, CString strAtrributeName);
	vector<CString> ConvertCStringToVector(CString str, LPCTSTR lpszSpliters);

	//创建一个新的xml文档对象，一般用于生成新的xml文件
	MSXML2::IXMLDOMDocument2Ptr CreateXmlDocPtr(void);
	//取得xml文档对象指针（通过字节流）
	MSXML2::IXMLDOMDocument2Ptr GetXmlDocPtrByStream(CComBSTR bstrXml
		, CString strXmlPathFile = _T(""));
	//取得xml文档对象指针（通过文档）
	MSXML2::IXMLDOMDocument2Ptr GetXmlDocPtrByFile(LPCTSTR lpszXmlPathFile
		, LPCTSTR lpszNamespace = NULL);

	//取得根节点
	MSXML2::IXMLDOMNodePtr GetRootNodePtr(MSXML2::IXMLDOMDocument2Ptr pDoc);

	//取得指定xpath的节点
	MSXML2::IXMLDOMNodePtr GetNodePtr(LPCTSTR lpszXPath 
		, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//设置指定xpath的节点值
	BOOL SetNodeValue(LPCTSTR lpszValue, LPCTSTR lpszXPath 
		, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//设置节点值
	BOOL SetNodeValue(LPCTSTR lpszValue, MSXML2::IXMLDOMNodePtr pNode);

	//设置指定xpath的节点属性
	BOOL SetAttrValue(LPCTSTR lpszValue, LPCTSTR lpszAttrName, LPCTSTR lpszXPath 
		, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//设置节点属性
	BOOL SetAttrValue(LPCTSTR lpszValue, LPCTSTR lpszAttrName, MSXML2::IXMLDOMNodePtr pNode);
	
	//取得指定节点下的所有子节点集合
	MSXML2::IXMLDOMNodeListPtr GetNodeListPtr(MSXML2::IXMLDOMNodePtr pParentNode);
	//取得指定路径下的所有子节点集合
	MSXML2::IXMLDOMNodeListPtr GetNodeListPtr(LPCTSTR lpszXPath
	, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//
	int GetAttrsOfNode(map<CString, CComBSTR>& mapAttrs
		, MSXML2::IXMLDOMNodePtr pNode);
	int GetAttrsOfNode(map<CString, CString>& mapAttrs, MSXML2::IXMLDOMNodePtr pNode);
	//遍历指定节点列表的数据
	BOOL FetchNodeList2(MSXML2::IXMLDOMNodePtr pParentNode, DWORD dwFlag);
	//遍历指定节点列表的数据
	virtual BOOL OnFetchNodeList2(MSXML2::IXMLDOMNodePtr pNode, DWORD dwFlag);
	//遍历指定节点列表的数据
	BOOL FetchNodeList(LPCTSTR lpszXmlPathFile, LPCTSTR lpszNamespace
		, LPCTSTR lpszNodeListXPath, DWORD dwFlag);
	//遍历指定节点列表的数据
	virtual BOOL OnFetchNodeList(MSXML2::IXMLDOMNodePtr pNode, DWORD dwFlag);

	BOOL SetNameSpace(MSXML2::IXMLDOMDocument2Ptr& pDoc, LPCTSTR lpszNameSpace);

	//取得节点
	MSXML2::IXMLDOMNodePtr GetNodePtr(LPCTSTR lpszAttrName, LPCTSTR lpszAttrValue
		, MSXML2::IXMLDOMNodePtr pParentNode);

	//取得第一个子节点，过滤掉注释信息
	MSXML2::IXMLDOMNodePtr GetFirstChildNodeWithoutComment(MSXML2::IXMLDOMNodePtr pParentNode);

	//创建xml头
	MSXML2::IXMLDOMNodePtr CreateXmlTitle(MSXML2::IXMLDOMDocument2Ptr pDoc);
	//创建根节点
	MSXML2::IXMLDOMNodePtr CreateRootNode(MSXML2::IXMLDOMDocument2Ptr pDoc
		, LPCTSTR lpszRootText = NULL);
	//创建节点
	MSXML2::IXMLDOMNodePtr CreateNode(MSXML2::IXMLDOMNodePtr pParentNode
	, LPCTSTR lpszNodeText);
	//在 pNode 节点之后，创建 回车、换行、Tab节点
	MSXML2::IXMLDOMNodePtr InsertEnterAndTabs(MSXML2::IXMLDOMNodePtr pParentNode, int nLevel = 0);
	//在 pNode 节点之后，创建 回车、换行、Tab节点
	MSXML2::IXMLDOMNodePtr InsertEnterAndTabs(MSXML2::IXMLDOMNodePtr pParentNode, MSXML2::IXMLDOMNodePtr pNode, int nLevel = 0);
	//插入节点
	MSXML2::IXMLDOMNodePtr InsertNode(MSXML2::IXMLDOMNodePtr pParentNode , MSXML2::IXMLDOMNodePtr pNodeBack , LPCTSTR lpszNodeText);
};
