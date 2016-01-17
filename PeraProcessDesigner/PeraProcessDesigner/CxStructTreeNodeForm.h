#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//结构树节点-表单
class CCxStructTreeNodeForm : public CCxStructTreeNode
{
public:
	CCxStructTreeNodeForm( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeForm();
	virtual void Clear(void);
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	virtual CString GetTooltipText(void);

public:
	CString m_strAuthor; //作者
	CString m_strDescription; //描述
	CString m_strCreated; //创建日期
	CString m_strVersion; //版本

	CString m_strLocalPath; //如果是表单，bot全路径。
};
