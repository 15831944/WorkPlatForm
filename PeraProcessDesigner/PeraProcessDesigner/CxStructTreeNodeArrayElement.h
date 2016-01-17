#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//结构树节点-数组元素
class CCxStructTreeNodeArrayElement : public CCxStructTreeNode
{
public:
	CCxStructTreeNodeArrayElement( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeArrayElement();
	virtual void Clear(void);
	//
	virtual CString GetTooltipText(void);
	//
	BOOL GetPos( vector<int>& vPos, BOOL& bElem );
	//
	CCxStructTreeNodeParam * GetParamNode(void);
public:
	CString m_strValue; //元素数据值
};

