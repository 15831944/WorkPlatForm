#pragma once
#include <CxStructTreeNode.h>
#include <CxStructTreeNodeArrayElement.h>
#include "CxStructTreeDataLinker.h"

//////////////////////////////////////////////////////////////////////////
//结构树节点-参数
class CCxStructTreeNodeParam : public CCxStructTreeNode
{
	friend class CCxStructTree;
	friend class CCxStructTreeGridT;
public:
	CCxStructTreeNodeParam( CCxStructTree * pTree );
	virtual ~CCxStructTreeNodeParam();
	virtual void Clear(void);
protected:
	virtual void OnGetCopied( CCxStructTreeNode * pNewNode );
public:
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	virtual BOOL OnParsed( CCxStructTreeNode * pParseRoot );

	//清除 m_lpValueData 对象
	void ClearValueData(void);
	//解析 m_strValue 为可用的对象，m_lpValueData
	BOOL ParseValue( BOOL bRebuild = TRUE );
	//
	virtual CString GetTooltipText(void);
	//是1维数组
	BOOL IsArray1D(void);
	//是2维数组
	BOOL IsArray2D(void);
	//是3维数组
	BOOL IsArray3D(void);
	//是数组参数
	BOOL IsArray(void);
	//是文件参数
	BOOL IsFile(void);
	//是整形参数
	BOOL IsInt(void);
	//是浮点型参数
	BOOL IsDouble(void);
	//是布尔型参数
	BOOL IsBoolean(void);
	//是字符串型参数
	BOOL IsString(void);

	//取得数组元素类型（基本类型）
	int GetElementDataType(void);
	//
	CString GetUpdateParamJsonStr( void );
	//
	CCxStructTreeNodeArrayElement * GetElementNode( vector<int> vPos );
	//参数方向
	BOOL IsOut(void) { return (m_nDir == PARAM_DIR_OUT); }
	BOOL IsIn(void) { return (m_nDir == PARAM_DIR_IN); }
	BOOL IsInOut(void) { return (m_nDir == PARAM_DIR_INOUT); }
	//
	CString GetDataTypeForDisplay(void);
	//
	CString GetDataDirForDisplay(void);

	//
	BOOL DeleteArrayData(void);
	//
	BOOL ReBuildArray( LPCTSTR lpszArrayData );

	CCxStructTreeNode* GetParentWithoutGroup(void);

public:
	//
	DWORD GetStatus(void);
	//
	BOOL SetStatus( BOOL bValid, BOOL bForce = FALSE );
	//
	virtual BOOL UpdateNode( LPCTSTR lpszData );
	//
	BOOL UpdateValue( LPCTSTR lpszNewValue ,BOOL bForce = FALSE );
	//
	CString GetDefualtValue( int nDataType );
	//设置当前节点的父节点（流程或组件）的运行标志。
	BOOL SetRunFlag( BOOL bRan = TRUE );
	//检测当前节点的父节点（流程或组件）的运行标志。
	BOOL CheckRunFlag( void );
	//取得当前节点的所有同级参数节点
	int GetSiblingParam( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//取得当前节点的所有同级输出节点
	int GetSiblingOut( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//取得当前节点的所有映射To节点，不递归
	int GetSiblingLinkerTo( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//取得当前节点的所有映射To节点，递归
	int GetLinkerToNodes( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//取得当前节点的所有映射From节点，不递归
	int GetSiblingLinkerFrom( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//取得当前节点的所有映射From节点，递归
	int GetLinkerFromNodes( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//编辑值时，取得下游所有参数节点
	int GetNodeForEditValue( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//判断是否能运行时，取得上游所有参数节点
	BOOL CanRun( void );
	//
	int GetNodeForNewLinker( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//
	void OnAfterNewLinker( void );
	//
	void OnAfterEditValue();
	//
	void OnAfterRunCompleted( void );
	//
	BOOL RunDataLinker( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//
	BOOL OnRunDataLinker( CCxStructTreeNodeParam* pFrom, CCxStructTreeNodeParam* pTo
		, CCxStructTreeDataLinker* pLinker, LPCTSTR lpszNewValue, vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//当前参数存在交叉映射
	BOOL IsLoopLinker(void);
	//上游参数节点无效
	BOOL IsInvalidFrom(void);
	//当前节点无效
	BOOL IsInvalid( void );

	//
	void ParamNodePushVector( vector< CCxStructTreeNodeParam* >& vSpecNodes, CCxStructTreeNodeParam*& pParam );

protected:
	//
	BOOL ParseArray1D( CxArrayND * pArray, BOOL bRebuild = TRUE );
	//
	BOOL ParseArray2D( CxArrayND * pArray, BOOL bRebuild = TRUE );
	//
	BOOL ParseArray3D( CxArrayND * pArray, BOOL bRebuild = TRUE );
	//
	void SetGridRowFormat( void );
public:
	int m_nDataType; //数据类型
	int m_nDir; //方向（in、out、inout）
	CString m_strValue; //参数数据值
	CString m_strUnits; //数据的单位，如：米，厘米等
	BOOL m_bIsDynamicArray; //是否为动态数组
	LPVOID m_lpValueData; //指向 m_strValue 解析后的数据
	CString m_strCnName; //别名，如：（name：x，cnname：横坐标）
	CString m_strDescrip; //描述
	CString m_strFormat; //格式
	CString m_strGroup;
	BOOL m_bIsSaveInModel;
};
