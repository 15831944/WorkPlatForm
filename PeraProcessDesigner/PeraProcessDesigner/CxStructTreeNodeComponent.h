#pragma once
#include <CxStructTreeNode.h>


//////////////////////////////////////////////////////////////////////////
//结构树节点-组件
class CCxStructTreeNodeComponent : public CCxStructTreeNode
{
public:
	CCxStructTreeNodeComponent( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeComponent();
	virtual void Clear(void);
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	virtual CString GetTooltipText(void);
	virtual CString GetUpdatePropJsonStr(void);
	//virtual void ClearStatus(void);
	//virtual void ClearModifiedFlag(void);
	//void SetRunFlag( BOOL bRan = TRUE );

	BOOL CanRun( void );

	//
	BOOL IsLoopLinker(void);

	void OnAfterRunCompleted( void );

public:
	CString m_strAuthor; //作者
	CString m_strDescription; //描述
	CString m_strCreated; //创建日期
	CString m_strVersion; //版本

	CString m_strRobotType; //对应 CCxResLibImage::m_strGuid
	CString m_strLocalPath; //如果是组件，bot全路径。
	BOOL m_bRunFlag;
	BOOL m_bIsPopForm; // 是否弹出表单
};

