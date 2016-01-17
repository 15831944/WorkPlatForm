#pragma once

#include "StlHeader.h"

class CXpdlNode
{
public:
	CXpdlNode(void);

	virtual ~CXpdlNode(void);

	virtual BOOL			Copy( CXpdlNode* pSrcNode );

	virtual CString			GetTooltipText(void);

public:

	DWORD					m_dwNodeType;

	CString					m_sAuthor;

	CString					m_sCreated;

	CString					m_sDescription;

	CString					m_sVersion;

	CString					m_sName;

	CString					m_sText;

	CString					m_sID;

	CString					m_sIdOther;
	
	CString					m_sPrePaths;

	CString					m_sPostPaths;

	CString					m_sPaths;

	CXpdlNode*				m_pParentNode;		//���ڵ�

	vector< CXpdlNode* >	m_vecChildNode;		//�ӽڵ�  �洢ID

	map< CString, CString > m_mapChildNode;
};

