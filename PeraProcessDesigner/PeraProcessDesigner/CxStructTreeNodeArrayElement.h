#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//�ṹ���ڵ�-����Ԫ��
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
	CString m_strValue; //Ԫ������ֵ
};

