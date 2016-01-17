#pragma once
#include <CxStructTreeNode.h>

//////////////////////////////////////////////////////////////////////////
//结构树节点-流程
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
	CString m_strAuthor; //作者
	CString m_strDescription; //描述
	CString m_strCreated; //创建日期
	CString m_strVersion; //版本
	CString m_strPkgId;
	BOOL m_bRunFlag;
	BOOL m_bIsPopForm; // 是否弹出表单
};
