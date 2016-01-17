#pragma once

#include "StlHeader.h"

//#import "msxml4.dll"

#include "XpdlNode.h"
#include "XpdlNodeParallel.h"
#include "XpdlNodeSequence.h"
#include "XpdlNodeSubflow.h"
#include "XpdlNodeIf.h"
#include "XpdlNodeComponent.h"
#include "XpdlNodeParam.h"
#include "XpdlNodePrimary.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "Tools.h"
#include "ZipTool.h"

using namespace rapidxml;

class CXpdl
{
	friend class CRunCache;
	friend class CDlgParamPanel;
	friend class CDlgRunPanel;
public:

	CXpdl(void);

	~CXpdl(void);

public:

	BOOL			Load( LPCTSTR lpszFilePath );

	//因为杨波XPDL中参数值在CDATA中也是转义后存储的，不符合标准，所以有以下函数
	//字符串转义
	CString StringEscape(CString sSrc);
	//字符串反转义
	CString StringUnEscape(CString sSrc);
	//解析XPDL后需反转义得到真实参数值
	BOOL ConvertFromYangboCdata();
	//保存XPDL时需转义按杨波的格式写入
	BOOL ConvertToYangboCdata();

	BOOL			LoadStream( LPCTSTR lpszXpdlStream, LPCTSTR lpszXpdlPath );

	BOOL			Parse( void );

	BOOL			Add( LPCTSTR, CXpdlNode* );//未实现

	BOOL			SetNode( const CXpdlNode* pSrcNode );

	BOOL			Save();

	CXpdlNode*		Find( LPCTSTR lpId );

	CXpdlNode*		FindParamNodeByGuid( LPCTSTR lpParentId, LPCTSTR lpGuid );

	CString			GetFilePath( LPCTSTR lpszNodeId, int nType );

	BOOL			ConvertToAbsolute();

	BOOL			ConvertToRelative();

	void			GetXpdlStream( string& sFileStream );

	BOOL			GetSimpleXpdlStream( string & sXpdlStream );

	BOOL			CreateCache( LPCTSTR lpszDir );

protected:

	CXpdlNode*						GetNewXpdlNode( CString sNodeId, DWORD dwNodeType );

	CXpdlNode*						GetNewXpdlParam( CString sParamId );

	BOOL							GetFlowNodeAttr(  xml_node<> * pNode  );

	BOOL							GetNodeTypeInfo(  DWORD & dwNodeType, xml_node<> * pNode );

	BOOL							GetNodeParam( xml_node<>* pNode );

	void							Clear();

	std::wstring					Utf82Unicode(const std::string& utf8string);

	std::string						UTF_82ASCII(std::string& strUtf8Code);

	std::string						WideByte2Acsi(std::wstring& wstrcode); 

	std::string						ASCII2UTF_8(string& strAsciiCode);

	std::string						Unicode2Utf8(const std::wstring& widestring);

	wstring							Acsi2WideByte(string& strascii);

//public:

	CString							RelativeToAbsolute( CString sCurDir, CString sPathIn );

	CString							AbsoluteToRelative( CString sFrom, CString sTo );

	BOOL							SetParamReadOnly();

protected:

	CString							m_sFilePath;

	map< CString, CXpdlNode* >		m_mapNode;

	map< CString, CXpdlNode* >		m_mapParam;

	CXpdlNode*						m_pRootNode;

	list< CString >					m_lstDataMapping;

private:

	rapidxml::xml_document<>		m_doc;

	DWORD							m_dwCapacity; //存储

	char*							m_pBuffer;

public:

	enum
	{
		XPDL_NODE_NULL = 0,
		XPDL_NODE_ROOT,
		XPDL_NODE_PARALLELNODE,
		XPDL_NODE_SEQUENCENODE,
		XPDL_NODE_IFNODE,
		XPDL_NODE_SUBFLOWNODE,
		XPDL_NODE_COMPONENTNODE,
		XPDL_NODE_PARAM
	};

	enum
	{
		XPDL_FILEPATH_TYPE_NULL = 0,
		XPDL_FILEPATH_TYPE_PRECOMPONENT,
		XPDL_FILEPATH_TYPE_POSTCOMPONENT,
		XPDL_FILEPATH_TYPE_COMPONENT,
		XPDL_FILEPATH_TYPE_PARAM
	};
};

