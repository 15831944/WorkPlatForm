#pragma once
#include "StlHeader.h"
#include "BCGPGridCtrl.h"
#include "CxArrayND.h"

#include "value.h"

class CCxStructTree;
class CCxStructTreeRowT;

//////////////////////////////////////////////////////////////////////////
//结构树节点
class CCxStructTreeNode
{
	friend class CCxStructTree;
	friend class CCxStructTreeUpdateParamsData;
	friend class CCxStructTreeNodeParam;
public:
	//节点类型
	enum {
		NT_NULL = 0, //未指定
		NT_EMPTY, //空节点（结构树上不显示，应该也不需要传递过来）
		NT_ROOT, //根，设计过程（结构树上只能有一个根节点）
		NT_COMPONENT, //组件
		NT_PARAM, //参数
		NT_PARAM_ARRAY, //数组参数
		//前后表单
		NT_FRONT_FORM,
		NT_REAR_FORM,
		//subflow,子流程
		NT_PARALLEL, //并行
		NT_SEQUENCE, //串行
		NT_IF, //条件
		NT_FOR, //循环
		NT_GROUP, //分组
		NT_SUBFLOW //子流程（黑盒节点，里面参数不允许向外映射）
	};

	//状态///////////////////////////////
	//参数值无效，置灰状态
	//1、当前节点为【输出】；
	//2、当前节点被数据映射；
	static const DWORD NS_VALUE_INVALID = 0x00000001;
	//参数名称、图标无效，打叉状态
	//1、如果当前节点是输出节点，同级输入节点有修改标志；
	//2、如果有被数据映射，From节点有修改标志；
	static const DWORD NS_NAME_INVALID =  0x00000002;
	//参数值可编辑，设为蓝色、可编辑状态
	//1、如果是输入节点，且没有被数据映射
	static const DWORD NS_VALUE_CANEDIT = 0x00000004;
	//只读
	//输出
	static const DWORD NS_VALUE_READONLY = 0x00000008;
	//有修改标记
	//static const DWORD NS_VALUE_MODIFIED = 0x00000010;
	//有数据映射
	static const DWORD NS_DATA_LINKED = 0x00000020;

	//如果当前节点的上游节点设置了无效，之后节点都设置无效
	//static const DWORD NS_BRANCH_NAME_INVALID = 0x00010000;

public:
	CCxStructTreeNode( CCxStructTree * pTree );
	virtual ~CCxStructTreeNode(void);
	virtual void Clear(void);
	CCxStructTreeNode * GetCopied(void);
	BOOL Copy( CCxStructTreeNode * pSrc );
protected:
	virtual void OnGetCopied( CCxStructTreeNode * pNewNode );
public:
	CCxStructTreeNode * GetChild( LPCTSTR lpszId );
public:
	static int ConvertNodeType( LPCTSTR lpszType );
	static CString ConvertNodeType( int nType );
	static int ConvertParamDataType( LPCTSTR lpszType );
	static CString ConvertParamDataType( int nType );
	static int ConvertParamDir( LPCTSTR lpszDir );
	static CString ConvertParamDir( int nDir );

	//根据节点类型，创建一个新的节点对象
	static CCxStructTreeNode * GetNewNodeByType( int nNodeType, CCxStructTree * pTree );
	//以自身节点为根，查找子节点
	//virtual CCxStructTreeNode * Find( LPCTSTR lpszId );
	//按照指定的索引号，重新排序
	BOOL ResetChildrenOrder(void);
	//取得同级节点中的序号
	int GetSiblingPos( void );
	virtual CString GetTooltipText(void);
	//
	CCxStructTreeNode * GetCanSelectedNode( void );
	//
	virtual CString GetUpdatePropJsonStr( void );
	//
	BOOL IsComponentNode(void);
	BOOL IsGroupNode(void);
	BOOL IsRootNode(void);
	BOOL IsFlowNode(void);
	BOOL IsParamNode(void);
	BOOL IsSubFlowNode(void);
	//节点数据是否与行数据匹配
	BOOL IsRowMatched( void );

	//void SetModified( BOOL bModified = TRUE );

	//virtual void UpdateStatus( BOOL bForward = FALSE );

protected:
	//解析当前节点
	virtual BOOL Parse( Json::Value& _jsonObj, CCxStructTreeNode * pParseRoot
		, int nNodeType, CCxStructTreeNode * pParent );
	//解析自身属性
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	//所有子节点解析完成后，的后续处理
	virtual BOOL OnParsed( CCxStructTreeNode * pParseRoot );
	//
	void AddToNodeMap(void);

public:
	//删除Grid上的数据后，清空对应的Row指针
	void ClearRowData( void );

public:
	virtual BOOL UpdateNode( LPCTSTR lpszData );

public:
	CString m_strID; //节点唯一标识
	CString m_strParentID; //节点父ID
	CCxStructTreeNode * m_pParent;
	int m_NodeType; //节点类型
	CString m_strOriName; //节点内部名称
	CString m_strText; //在结构树上显示的名称，默认等于 m_strOriName
	//BOOL m_bModified; //修改标志，TRUE:表示节点内容被修改过。
	vector < CCxStructTreeNode * > m_vChildren; //子节点类表
	int m_nImageID;
	CCxStructTreeRowT * m_pRow; //结构树上对应的行指针
	int m_nIndex; //指定索引号（如果不指定，为 -1，按解析顺序排序）
	CCxStructTree * m_pTree;
	DWORD m_dwStatus; //节点状态
};

//保存除了数组以外的节点，用于查找
typedef map< CString, CCxStructTreeNode * > STRUCTTREEMAP_NODE;
//输入-输出关系
typedef map< CCxStructTreeNode *, CCxStructTreeNode * > STRUCTTREEMAP_REFPAIR;
//输出-输入关系
typedef map< CCxStructTreeNode *, STRUCTTREEMAP_REFPAIR > STRUCTTREEMAP_REF;

