#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//结构树节点-流程
class CCxStructTreeNodeGroup : public CCxStructTreeNode
{
public:
	CCxStructTreeNodeGroup( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeGroup();
	virtual void Clear(void);
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	virtual CString GetTooltipText(void);
	int GetParamNodes( vector< CCxStructTreeNodeParam* >& vParams );

	BOOL CanRun( void );
public:
};
