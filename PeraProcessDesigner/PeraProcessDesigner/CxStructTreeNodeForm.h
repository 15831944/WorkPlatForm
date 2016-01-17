#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//�ṹ���ڵ�-��
class CCxStructTreeNodeForm : public CCxStructTreeNode
{
public:
	CCxStructTreeNodeForm( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeForm();
	virtual void Clear(void);
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	virtual CString GetTooltipText(void);

public:
	CString m_strAuthor; //����
	CString m_strDescription; //����
	CString m_strCreated; //��������
	CString m_strVersion; //�汾

	CString m_strLocalPath; //����Ǳ���botȫ·����
};
