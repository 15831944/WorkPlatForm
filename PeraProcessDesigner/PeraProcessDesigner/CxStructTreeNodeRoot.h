#pragma once
#include <CxStructTreeNodeFlow.h>

//////////////////////////////////////////////////////////////////////////
//结构树节点-根节点
class CCxStructTreeNodeRoot : public CCxStructTreeNodeFlow
{
public:
	CCxStructTreeNodeRoot( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeRoot();
	virtual void Clear(void);
	virtual BOOL OnParsing( Json::Value& _jsonObj );

public:
	CString m_strSelNodeId; //
};
