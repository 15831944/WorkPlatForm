#pragma once
#include <CxStructTreeNode.h>
#include <CxStructTreeNodeArrayElement.h>
#include "CxStructTreeDataLinker.h"

//////////////////////////////////////////////////////////////////////////
//�ṹ���ڵ�-����
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

	//��� m_lpValueData ����
	void ClearValueData(void);
	//���� m_strValue Ϊ���õĶ���m_lpValueData
	BOOL ParseValue( BOOL bRebuild = TRUE );
	//
	virtual CString GetTooltipText(void);
	//��1ά����
	BOOL IsArray1D(void);
	//��2ά����
	BOOL IsArray2D(void);
	//��3ά����
	BOOL IsArray3D(void);
	//���������
	BOOL IsArray(void);
	//���ļ�����
	BOOL IsFile(void);
	//�����β���
	BOOL IsInt(void);
	//�Ǹ����Ͳ���
	BOOL IsDouble(void);
	//�ǲ����Ͳ���
	BOOL IsBoolean(void);
	//���ַ����Ͳ���
	BOOL IsString(void);

	//ȡ������Ԫ�����ͣ��������ͣ�
	int GetElementDataType(void);
	//
	CString GetUpdateParamJsonStr( void );
	//
	CCxStructTreeNodeArrayElement * GetElementNode( vector<int> vPos );
	//��������
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
	//���õ�ǰ�ڵ�ĸ��ڵ㣨���̻�����������б�־��
	BOOL SetRunFlag( BOOL bRan = TRUE );
	//��⵱ǰ�ڵ�ĸ��ڵ㣨���̻�����������б�־��
	BOOL CheckRunFlag( void );
	//ȡ�õ�ǰ�ڵ������ͬ�������ڵ�
	int GetSiblingParam( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//ȡ�õ�ǰ�ڵ������ͬ������ڵ�
	int GetSiblingOut( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//ȡ�õ�ǰ�ڵ������ӳ��To�ڵ㣬���ݹ�
	int GetSiblingLinkerTo( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//ȡ�õ�ǰ�ڵ������ӳ��To�ڵ㣬�ݹ�
	int GetLinkerToNodes( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//ȡ�õ�ǰ�ڵ������ӳ��From�ڵ㣬���ݹ�
	int GetSiblingLinkerFrom( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//ȡ�õ�ǰ�ڵ������ӳ��From�ڵ㣬�ݹ�
	int GetLinkerFromNodes( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//�༭ֵʱ��ȡ���������в����ڵ�
	int GetNodeForEditValue( vector< CCxStructTreeNodeParam* >& vSpecNodes );
	//�ж��Ƿ�������ʱ��ȡ���������в����ڵ�
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
	//��ǰ�������ڽ���ӳ��
	BOOL IsLoopLinker(void);
	//���β����ڵ���Ч
	BOOL IsInvalidFrom(void);
	//��ǰ�ڵ���Ч
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
	int m_nDataType; //��������
	int m_nDir; //����in��out��inout��
	CString m_strValue; //��������ֵ
	CString m_strUnits; //���ݵĵ�λ���磺�ף����׵�
	BOOL m_bIsDynamicArray; //�Ƿ�Ϊ��̬����
	LPVOID m_lpValueData; //ָ�� m_strValue �����������
	CString m_strCnName; //�������磺��name��x��cnname�������꣩
	CString m_strDescrip; //����
	CString m_strFormat; //��ʽ
	CString m_strGroup;
	BOOL m_bIsSaveInModel;
};
