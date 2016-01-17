#pragma once
#include "CxStructTreeNode.h"
#include "SvgEventData.h"
#include "CxStructTreeDataLinker.h"
#include "StlHeader.h"

class CCxStructTreeGridT;
class CCxStructTreeItemT;

class CCxStructTree
{
	friend class CCxStructTreeGridT;
	friend class CCxStructTreeNode;
	friend class CCxStructTreeNodeParam;
	friend class CCxStructTreeUpdateParamsData;
	friend class CCxStructTreeNodeComponent;
	friend class CCxStructTreeNodeFlow;
	friend class CCxStructTreeNodeRoot;

public:
	CCxStructTree(void);
	virtual ~CCxStructTree(void);
	void Clear(void);
	//清除数据影射关系
	void ClearDataLinker(void);

	//解析一个节点，包含所有子节点
	CCxStructTreeNode * ParseNode( LPCTSTR lpszData );
	//解析节点ID
	CString ParseNodeId( LPCTSTR lpszData );
	//解析Tooltip数据
	BOOL ParseTooltip( LPCTSTR lpszData );

	//树上查找一个节点
	CCxStructTreeNode * Find( const CString& strId );
	//解析所有数据影射
	BOOL ParseDataLinker( LPCTSTR lpszData );
	//如果是数组映射，取得有影射关系的元素坐标、对应公式，以及是否为整体映射。
	BOOL GetDataLinker( vector< CCxStructTreeDataLinker* >& vLinker
		, CCxStructTreeNode * pFrom, CCxStructTreeNode * pTo );
	//
	//BOOL RunDataLinkerFormula( vector< CCxStructTreeDataLinker* >& vLinker
	//	, BOOL bModified, BOOL bPass, BOOL bDynArray );
	//
	BOOL AddNode( CCxStructTreeNode * pNewNode, CCxStructTreeNode * pParent, int nIndex = -1 );
	//
	BOOL DeleteNode( CCxStructTreeNode *& pNode );
	//移动节点
	BOOL MoveNode( CCxStructTreeMoveEventData * pMoveEventData );
	//用新的节点替换旧的节点
	//BOOL ReplaceNode( CCxStructTreeNode *& pNewNode, CCxStructTreeNode *& pOldNode );
	//重建数组参数节点
//	CCxStructTreeNode * ReBuildArrayNode( CCxStructTreeNode* pOldNode );

	//设置当前选中节点
	BOOL SetSelNode( CCxStructTreeNode * pSelNode );
	//
	BOOL UpdateParams( LPCTSTR lpszData );
	//
	BOOL SvgEvent( CString& strFlag, CString& strData );
	//
	CCxStructTreeNode * GetRoot(void);
	//
	void OnNodeSelected( BOOL bNotifySvg = FALSE );
	//
	BOOL OnUpdateParamValue( CCxStructTreeNodeParam * pParamNode, LPCTSTR lpszNewValue );
	//
	BOOL RemoveNodeFromMap( CCxStructTreeNode * pNode );
	BOOL RemoveNodeFromMap( CString& strId );
	//从输入、输出、数据映射From、To表中，移除节点指针
	void RemoveNodeFromRefMap( CCxStructTreeNode * pNode );
	void RemoveNodeFromRefMap( STRUCTTREEMAP_REF& mapRef, CCxStructTreeNode * pNode );
	//
	void ReplaceNodeInRefMap( CCxStructTreeNode * pNewNode, CCxStructTreeNode * pOldNode );
	void ReplaceNodeInRefMap( STRUCTTREEMAP_REF& mapRef, CCxStructTreeNode * pNewNode, CCxStructTreeNode * pOldNode );
	//
	BOOL AddNodeToMap( CCxStructTreeNode * pNode );
	//
	BOOL ReloadRefMap( void );
	//
	STRUCTTREEMAP_REF::iterator InsertRefMap( STRUCTTREEMAP_REF& mapRef, CCxStructTreeNode * pNode );

	//
	void UpdateGridAllRowStatus(void);
	//
	//BOOL CheckInvalidStatusByRef( CCxStructTreeNode * pNode );
	//BOOL CheckInvalidStatusByLinker( CCxStructTreeNode * pNode );
	//
	BOOL SetRoot( CCxStructTreeNode * pRoot );
	//
	BOOL UpdateArrayParamNode( CCxStructTreeNode * pParamNode );
	//
	BOOL RetriveDataMapping(void);
	//取得数据映射公式表达式
	CString GetDataLinkerFormula( CCxStructTreeNode* pFromNode
		, CCxStructTreeNode* pToNode );
	//更新结构树节点属性
	BOOL UpdateNodeProp( CCxStructTreeNode * pNode );
	//
	//BOOL CheckNodeUpdateStatusInquire( CCxStructTreeNode * pNode, BOOL bAdd = FALSE );
	//
	//BOOL CheckNodeUpdateStatus( CCxStructTreeNode * pNode, BOOL bAdd = FALSE );

	//设置组件或子流程的运行标志
	//如果有运行标志的组件或子流程，清除修改标志
	//void BeforeRunning( CCxStructTreeNode * pNode );

	BOOL GetParamNodesWithLinker( vector< CCxStructTreeNodeParam* >& vParamNodes );
	void ResetRunFlag();
	
public:
	CString m_strCurSelId;

protected:
	CCxStructTreeNode * m_pRoot;
	vector< CCxStructTreeDataLinker * > m_vDataLinker;
	//STRUCTTREE_DATAMAPPING m_vDataMapping;
	CCxStructTreeGridT * m_pGrid;

	STRUCTTREEMAP_NODE m_mapNode;
	//key：输出节点（被影响的节点）
	STRUCTTREEMAP_REF m_mapRefOut;
	//key：输入节点（产生影响的节点）
	STRUCTTREEMAP_REF m_mapRefIn;
	//key：被映射的节点
	STRUCTTREEMAP_REF m_mapLinkerTo;
	//key：发起映射的节点
	STRUCTTREEMAP_REF m_mapLinkerFrom;

	//
	STRUCTTREEMAP_NODE m_mapParseNodeCache;
	BOOL m_bNodeParseing; //当为真时，用 m_mapParseNodeCache

	//设置节点值时，缓存已经处理过的节点。
	//用法说明：每次递归设置节点值之前，检查是否已经处理过当前节点，如果存在，则不处理。
	//处理过一个值后，将自身存储在这个结构中。
	//vector< CCxStructTreeNode* > m_vCacheSetValue;
	//更新节点状态时，缓存已经处理过的节点。
	//用法说明：每次递归更新节点状态之前，检查当前节点是否已经存在于这个缓存中，
	//如果存在，则不处理。否则，放自己放入此缓存中。更新自身状态。
	//vector< CCxStructTreeNode* > m_vCacheUpdateStatus;
	//vector< CCxStructTreeNode* > m_vCacheUpdateStatusInquire;
	//
	//vector< CCxStructTreeNode* > m_vCacheOnSetInvalidName;
	//
	//map<CString, CString> m_mapCacheReParseDynArray;
};

