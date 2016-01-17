#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//�ṹ���ڵ�-����
class CCxStructTreeNodeFlow : public CCxStructTreeNode
{
public:
	CCxStructTreeNodeFlow( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeFlow();
	virtual void Clear(void);
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	virtual CString GetTooltipText(void);
	virtual CString GetUpdatePropJsonStr(void);
	//virtual void ClearStatus( void );
	//virtual void ClearModifiedFlag(void);
	//void SetRunFlag( BOOL bRan = TRUE );
	BOOL CanRun( void );
	void OnAfterRunCompleted( void );

public:
	CString m_strAuthor; //����
	CString m_strDescription; //����
	CString m_strCreated; //��������
	CString m_strVersion; //�汾
	CString m_strPkgId;
	BOOL m_bRunFlag;
	BOOL m_bIsPopForm; // �Ƿ񵯳���
};
