#pragma once
#include <CxStructTreeNodeFlow.h>

//////////////////////////////////////////////////////////////////////////
//�ṹ���ڵ�-���ڵ�
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
