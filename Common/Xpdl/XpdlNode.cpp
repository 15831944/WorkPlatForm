#include "StdAfx.h"
#include "XpdlNode.h"


CXpdlNode::CXpdlNode(void)
{
	m_pParentNode = NULL;

	m_dwNodeType = 0;
}


CXpdlNode::~CXpdlNode(void)
{
}

BOOL CXpdlNode::Copy( CXpdlNode* pSrcNode )
{
	return TRUE;
}

CString CXpdlNode::GetTooltipText(void)
{
	return "";
}