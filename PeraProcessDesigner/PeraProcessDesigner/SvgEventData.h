#pragma once
#include <CxStructTreeNode.h>
#include "CxStructTreeNodeComponent.h"
#include "CxStructTreeNodeFlow.h"
#include "CxStructTreeNodeGroup.h"

class CCxStructTree;

//////////////////////////////////////////////////////////////////////////
//�ṹ��-�ƶ��ڵ��¼� ���ݽṹ
class CCxStructTreeMoveEventData
{
public:
	CCxStructTreeMoveEventData();
	~CCxStructTreeMoveEventData();
	void Clear();
	BOOL Parse( LPCTSTR lpszData, CCxStructTree* pTree );
	BOOL OnParsed( CCxStructTree* pTree );
	BOOL CanMove( void );
public:
	CString m_strSrcId;
	CString m_strDestParentId;
	CString m_strNameNew;

	int m_nSrcIndex;
	int m_nDestIndex;
	CCxStructTreeNode * m_pDestParent;
	CCxStructTreeNode * m_pSrcParent;
	CCxStructTreeNode * m_pSrcNode;
};

//////////////////////////////////////////////////////////////////////////
//�ṹ��-Tooltip�¼� ���ݽṹ
class CCxStructTreeTooltipData
{
public:
	CCxStructTreeTooltipData();
	~CCxStructTreeTooltipData();
	void Clear();
	BOOL Parse( LPCTSTR lpszData, CCxStructTree * pTree );
public:
	CString m_strId;
	BOOL m_bShow;
	CCxStructTreeNode * m_pNode;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CCxStructTreeRunResultParam
{
public:
	CCxStructTreeRunResultParam();
	~CCxStructTreeRunResultParam();
	void Clear(void);
	CString m_strId;
	CString m_strValue;
	CCxStructTreeNodeParam * m_pNode;
};
class CCxStructTreeRunResultComponent
{
public:
	CCxStructTreeRunResultComponent();
	~CCxStructTreeRunResultComponent();
	void Clear(void);
	CString m_strId;
	CString m_strStatus;
	CCxStructTreeNodeComponent * m_pNode;
};
class CCxStructTreeRunResultFlow
{
public:
	CCxStructTreeRunResultFlow();
	~CCxStructTreeRunResultFlow();
	void Clear(void);
	CString m_strId;
	CString m_strStatus;
	CCxStructTreeNodeFlow * m_pNode;
};

//����ID-����ֵ
typedef map< CString, CCxStructTreeRunResultParam * > STRUCTTREEMAP_PARAM_VALUE;

//////////////////////////////////////////////////////////////////////////
//���н��
class CCxStructTreeRunResult
{
public:
	CCxStructTreeRunResult();
	~CCxStructTreeRunResult();
	void Clear(void);
	BOOL Parse( LPCTSTR lpszData, CCxStructTree * pTree );
	BOOL UpdateFileParam(void);
	CString GetSvgResultJsonStr(void);
	//CString ConvertSyncSvgData( CCxStructTree * pTree );
public:
	STRUCTTREEMAP_PARAM_VALUE m_mapRunReturn;
	CString m_strStatus;
	vector<CCxStructTreeRunResultFlow*> m_vFlows;
	vector<CCxStructTreeRunResultComponent*> m_vComponents;
	
};

//////////////////////////////////////////////////////////////////////////
//�������̲���

class CCxStructTreeUpdateParamsOperatorData
{
public:
	CCxStructTreeUpdateParamsOperatorData();
	~CCxStructTreeUpdateParamsOperatorData();
	void Clear();
public:
	CString m_strFlag; // modify, delete, add
	CString m_strId;
	CCxStructTreeNodeParam* m_pParamNode;
};

class CCxStructTreeUpdateParamsData
{
public:
	CCxStructTreeUpdateParamsData();
	~CCxStructTreeUpdateParamsData();
	void Clear();
	BOOL Parse( LPCTSTR lpszData, CCxStructTree * pTree );
	CCxStructTreeNode * FindParam( LPCTSTR lpszId );
public:
	//int m_nStartIndex;
	//CString m_strFlowNodeId;
	vector< CCxStructTreeNodeGroup* > m_vGroups;
	vector< CCxStructTreeNodeParam* > m_vParams;
	vector< CCxStructTreeUpdateParamsOperatorData* > m_vOperators;
	STRUCTTREEMAP_NODE m_mapParamNode;
	STRUCTTREEMAP_NODE m_mapGroupNode;
};

//////////////////////////////////////////////////////////////////////////
//�����Ҽ��˵�
class CCxStructTreePopMenuData
{
public:
	CCxStructTreePopMenuData();
	~CCxStructTreePopMenuData();
	void Clear();
	BOOL Parse( LPCTSTR lpszData, CCxStructTree * pTree );
public:
	CString m_strMenuType; 
	CString m_strId;
};