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
	//�������Ӱ���ϵ
	void ClearDataLinker(void);

	//����һ���ڵ㣬���������ӽڵ�
	CCxStructTreeNode * ParseNode( LPCTSTR lpszData );
	//�����ڵ�ID
	CString ParseNodeId( LPCTSTR lpszData );
	//����Tooltip����
	BOOL ParseTooltip( LPCTSTR lpszData );

	//���ϲ���һ���ڵ�
	CCxStructTreeNode * Find( const CString& strId );
	//������������Ӱ��
	BOOL ParseDataLinker( LPCTSTR lpszData );
	//���������ӳ�䣬ȡ����Ӱ���ϵ��Ԫ�����ꡢ��Ӧ��ʽ���Լ��Ƿ�Ϊ����ӳ�䡣
	BOOL GetDataLinker( vector< CCxStructTreeDataLinker* >& vLinker
		, CCxStructTreeNode * pFrom, CCxStructTreeNode * pTo );
	//
	//BOOL RunDataLinkerFormula( vector< CCxStructTreeDataLinker* >& vLinker
	//	, BOOL bModified, BOOL bPass, BOOL bDynArray );
	//
	BOOL AddNode( CCxStructTreeNode * pNewNode, CCxStructTreeNode * pParent, int nIndex = -1 );
	//
	BOOL DeleteNode( CCxStructTreeNode *& pNode );
	//�ƶ��ڵ�
	BOOL MoveNode( CCxStructTreeMoveEventData * pMoveEventData );
	//���µĽڵ��滻�ɵĽڵ�
	//BOOL ReplaceNode( CCxStructTreeNode *& pNewNode, CCxStructTreeNode *& pOldNode );
	//�ؽ���������ڵ�
//	CCxStructTreeNode * ReBuildArrayNode( CCxStructTreeNode* pOldNode );

	//���õ�ǰѡ�нڵ�
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
	//�����롢���������ӳ��From��To���У��Ƴ��ڵ�ָ��
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
	//ȡ������ӳ�乫ʽ���ʽ
	CString GetDataLinkerFormula( CCxStructTreeNode* pFromNode
		, CCxStructTreeNode* pToNode );
	//���½ṹ���ڵ�����
	BOOL UpdateNodeProp( CCxStructTreeNode * pNode );
	//
	//BOOL CheckNodeUpdateStatusInquire( CCxStructTreeNode * pNode, BOOL bAdd = FALSE );
	//
	//BOOL CheckNodeUpdateStatus( CCxStructTreeNode * pNode, BOOL bAdd = FALSE );

	//��������������̵����б�־
	//��������б�־������������̣�����޸ı�־
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
	//key������ڵ㣨��Ӱ��Ľڵ㣩
	STRUCTTREEMAP_REF m_mapRefOut;
	//key������ڵ㣨����Ӱ��Ľڵ㣩
	STRUCTTREEMAP_REF m_mapRefIn;
	//key����ӳ��Ľڵ�
	STRUCTTREEMAP_REF m_mapLinkerTo;
	//key������ӳ��Ľڵ�
	STRUCTTREEMAP_REF m_mapLinkerFrom;

	//
	STRUCTTREEMAP_NODE m_mapParseNodeCache;
	BOOL m_bNodeParseing; //��Ϊ��ʱ���� m_mapParseNodeCache

	//���ýڵ�ֵʱ�������Ѿ�������Ľڵ㡣
	//�÷�˵����ÿ�εݹ����ýڵ�ֵ֮ǰ������Ƿ��Ѿ��������ǰ�ڵ㣬������ڣ��򲻴���
	//�����һ��ֵ�󣬽�����洢������ṹ�С�
	//vector< CCxStructTreeNode* > m_vCacheSetValue;
	//���½ڵ�״̬ʱ�������Ѿ�������Ľڵ㡣
	//�÷�˵����ÿ�εݹ���½ڵ�״̬֮ǰ����鵱ǰ�ڵ��Ƿ��Ѿ���������������У�
	//������ڣ��򲻴������򣬷��Լ�����˻����С���������״̬��
	//vector< CCxStructTreeNode* > m_vCacheUpdateStatus;
	//vector< CCxStructTreeNode* > m_vCacheUpdateStatusInquire;
	//
	//vector< CCxStructTreeNode* > m_vCacheOnSetInvalidName;
	//
	//map<CString, CString> m_mapCacheReParseDynArray;
};

