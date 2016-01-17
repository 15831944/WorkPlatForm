/************************************************************************
ʹ��˵����
1�����ͷ�ļ�·����
	..\CXImageD\CxImage
2�����ͷ�ļ�·�����߶�Ӧ��ͷ�ļ���
	PtrVectorHeader.h
	CommonFunctionFile.h
3����ӿ�Ŀ¼��
	..\LibD
4����ӿ⣺
	cximage.lib Jpeg.lib png.lib Tiff.lib zlib.lib jbig.lib jasper.lib j2k.lib
5��ȷ��ϵͳ���Ѿ���װ��΢��� Xml�����⣺msxml.msi

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

	//����һ���µ�xml�ĵ�����һ�����������µ�xml�ļ�
	MSXML2::IXMLDOMDocument2Ptr CreateXmlDocPtr(void);
	//ȡ��xml�ĵ�����ָ�루ͨ���ֽ�����
	MSXML2::IXMLDOMDocument2Ptr GetXmlDocPtrByStream(CComBSTR bstrXml
		, CString strXmlPathFile = _T(""));
	//ȡ��xml�ĵ�����ָ�루ͨ���ĵ���
	MSXML2::IXMLDOMDocument2Ptr GetXmlDocPtrByFile(LPCTSTR lpszXmlPathFile
		, LPCTSTR lpszNamespace = NULL);

	//ȡ�ø��ڵ�
	MSXML2::IXMLDOMNodePtr GetRootNodePtr(MSXML2::IXMLDOMDocument2Ptr pDoc);

	//ȡ��ָ��xpath�Ľڵ�
	MSXML2::IXMLDOMNodePtr GetNodePtr(LPCTSTR lpszXPath 
		, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//����ָ��xpath�Ľڵ�ֵ
	BOOL SetNodeValue(LPCTSTR lpszValue, LPCTSTR lpszXPath 
		, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//���ýڵ�ֵ
	BOOL SetNodeValue(LPCTSTR lpszValue, MSXML2::IXMLDOMNodePtr pNode);

	//����ָ��xpath�Ľڵ�����
	BOOL SetAttrValue(LPCTSTR lpszValue, LPCTSTR lpszAttrName, LPCTSTR lpszXPath 
		, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//���ýڵ�����
	BOOL SetAttrValue(LPCTSTR lpszValue, LPCTSTR lpszAttrName, MSXML2::IXMLDOMNodePtr pNode);
	
	//ȡ��ָ���ڵ��µ������ӽڵ㼯��
	MSXML2::IXMLDOMNodeListPtr GetNodeListPtr(MSXML2::IXMLDOMNodePtr pParentNode);
	//ȡ��ָ��·���µ������ӽڵ㼯��
	MSXML2::IXMLDOMNodeListPtr GetNodeListPtr(LPCTSTR lpszXPath
	, MSXML2::IXMLDOMDocument2Ptr pDoc);

	//
	int GetAttrsOfNode(map<CString, CComBSTR>& mapAttrs
		, MSXML2::IXMLDOMNodePtr pNode);
	int GetAttrsOfNode(map<CString, CString>& mapAttrs, MSXML2::IXMLDOMNodePtr pNode);
	//����ָ���ڵ��б������
	BOOL FetchNodeList2(MSXML2::IXMLDOMNodePtr pParentNode, DWORD dwFlag);
	//����ָ���ڵ��б������
	virtual BOOL OnFetchNodeList2(MSXML2::IXMLDOMNodePtr pNode, DWORD dwFlag);
	//����ָ���ڵ��б������
	BOOL FetchNodeList(LPCTSTR lpszXmlPathFile, LPCTSTR lpszNamespace
		, LPCTSTR lpszNodeListXPath, DWORD dwFlag);
	//����ָ���ڵ��б������
	virtual BOOL OnFetchNodeList(MSXML2::IXMLDOMNodePtr pNode, DWORD dwFlag);

	BOOL SetNameSpace(MSXML2::IXMLDOMDocument2Ptr& pDoc, LPCTSTR lpszNameSpace);

	//ȡ�ýڵ�
	MSXML2::IXMLDOMNodePtr GetNodePtr(LPCTSTR lpszAttrName, LPCTSTR lpszAttrValue
		, MSXML2::IXMLDOMNodePtr pParentNode);

	//ȡ�õ�һ���ӽڵ㣬���˵�ע����Ϣ
	MSXML2::IXMLDOMNodePtr GetFirstChildNodeWithoutComment(MSXML2::IXMLDOMNodePtr pParentNode);

	//����xmlͷ
	MSXML2::IXMLDOMNodePtr CreateXmlTitle(MSXML2::IXMLDOMDocument2Ptr pDoc);
	//�������ڵ�
	MSXML2::IXMLDOMNodePtr CreateRootNode(MSXML2::IXMLDOMDocument2Ptr pDoc
		, LPCTSTR lpszRootText = NULL);
	//�����ڵ�
	MSXML2::IXMLDOMNodePtr CreateNode(MSXML2::IXMLDOMNodePtr pParentNode
	, LPCTSTR lpszNodeText);
	//�� pNode �ڵ�֮�󣬴��� �س������С�Tab�ڵ�
	MSXML2::IXMLDOMNodePtr InsertEnterAndTabs(MSXML2::IXMLDOMNodePtr pParentNode, int nLevel = 0);
	//�� pNode �ڵ�֮�󣬴��� �س������С�Tab�ڵ�
	MSXML2::IXMLDOMNodePtr InsertEnterAndTabs(MSXML2::IXMLDOMNodePtr pParentNode, MSXML2::IXMLDOMNodePtr pNode, int nLevel = 0);
	//����ڵ�
	MSXML2::IXMLDOMNodePtr InsertNode(MSXML2::IXMLDOMNodePtr pParentNode , MSXML2::IXMLDOMNodePtr pNodeBack , LPCTSTR lpszNodeText);
};
