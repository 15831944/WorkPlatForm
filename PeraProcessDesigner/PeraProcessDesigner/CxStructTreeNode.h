#pragma once
#include "StlHeader.h"
#include "BCGPGridCtrl.h"
#include "CxArrayND.h"

#include "value.h"

class CCxStructTree;
class CCxStructTreeRowT;

//////////////////////////////////////////////////////////////////////////
//�ṹ���ڵ�
class CCxStructTreeNode
{
	friend class CCxStructTree;
	friend class CCxStructTreeUpdateParamsData;
	friend class CCxStructTreeNodeParam;
public:
	//�ڵ�����
	enum {
		NT_NULL = 0, //δָ��
		NT_EMPTY, //�սڵ㣨�ṹ���ϲ���ʾ��Ӧ��Ҳ����Ҫ���ݹ�����
		NT_ROOT, //������ƹ��̣��ṹ����ֻ����һ�����ڵ㣩
		NT_COMPONENT, //���
		NT_PARAM, //����
		NT_PARAM_ARRAY, //�������
		//ǰ���
		NT_FRONT_FORM,
		NT_REAR_FORM,
		//subflow,������
		NT_PARALLEL, //����
		NT_SEQUENCE, //����
		NT_IF, //����
		NT_FOR, //ѭ��
		NT_GROUP, //����
		NT_SUBFLOW //�����̣��ںнڵ㣬�����������������ӳ�䣩
	};

	//״̬///////////////////////////////
	//����ֵ��Ч���û�״̬
	//1����ǰ�ڵ�Ϊ���������
	//2����ǰ�ڵ㱻����ӳ�䣻
	static const DWORD NS_VALUE_INVALID = 0x00000001;
	//�������ơ�ͼ����Ч�����״̬
	//1�������ǰ�ڵ�������ڵ㣬ͬ������ڵ����޸ı�־��
	//2������б�����ӳ�䣬From�ڵ����޸ı�־��
	static const DWORD NS_NAME_INVALID =  0x00000002;
	//����ֵ�ɱ༭����Ϊ��ɫ���ɱ༭״̬
	//1�����������ڵ㣬��û�б�����ӳ��
	static const DWORD NS_VALUE_CANEDIT = 0x00000004;
	//ֻ��
	//���
	static const DWORD NS_VALUE_READONLY = 0x00000008;
	//���޸ı��
	//static const DWORD NS_VALUE_MODIFIED = 0x00000010;
	//������ӳ��
	static const DWORD NS_DATA_LINKED = 0x00000020;

	//�����ǰ�ڵ�����νڵ���������Ч��֮��ڵ㶼������Ч
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

	//���ݽڵ����ͣ�����һ���µĽڵ����
	static CCxStructTreeNode * GetNewNodeByType( int nNodeType, CCxStructTree * pTree );
	//������ڵ�Ϊ���������ӽڵ�
	//virtual CCxStructTreeNode * Find( LPCTSTR lpszId );
	//����ָ���������ţ���������
	BOOL ResetChildrenOrder(void);
	//ȡ��ͬ���ڵ��е����
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
	//�ڵ������Ƿ���������ƥ��
	BOOL IsRowMatched( void );

	//void SetModified( BOOL bModified = TRUE );

	//virtual void UpdateStatus( BOOL bForward = FALSE );

protected:
	//������ǰ�ڵ�
	virtual BOOL Parse( Json::Value& _jsonObj, CCxStructTreeNode * pParseRoot
		, int nNodeType, CCxStructTreeNode * pParent );
	//������������
	virtual BOOL OnParsing( Json::Value& _jsonObj );
	//�����ӽڵ������ɺ󣬵ĺ�������
	virtual BOOL OnParsed( CCxStructTreeNode * pParseRoot );
	//
	void AddToNodeMap(void);

public:
	//ɾ��Grid�ϵ����ݺ���ն�Ӧ��Rowָ��
	void ClearRowData( void );

public:
	virtual BOOL UpdateNode( LPCTSTR lpszData );

public:
	CString m_strID; //�ڵ�Ψһ��ʶ
	CString m_strParentID; //�ڵ㸸ID
	CCxStructTreeNode * m_pParent;
	int m_NodeType; //�ڵ�����
	CString m_strOriName; //�ڵ��ڲ�����
	CString m_strText; //�ڽṹ������ʾ�����ƣ�Ĭ�ϵ��� m_strOriName
	//BOOL m_bModified; //�޸ı�־��TRUE:��ʾ�ڵ����ݱ��޸Ĺ���
	vector < CCxStructTreeNode * > m_vChildren; //�ӽڵ����
	int m_nImageID;
	CCxStructTreeRowT * m_pRow; //�ṹ���϶�Ӧ����ָ��
	int m_nIndex; //ָ�������ţ������ָ����Ϊ -1��������˳������
	CCxStructTree * m_pTree;
	DWORD m_dwStatus; //�ڵ�״̬
};

//���������������Ľڵ㣬���ڲ���
typedef map< CString, CCxStructTreeNode * > STRUCTTREEMAP_NODE;
//����-�����ϵ
typedef map< CCxStructTreeNode *, CCxStructTreeNode * > STRUCTTREEMAP_REFPAIR;
//���-�����ϵ
typedef map< CCxStructTreeNode *, STRUCTTREEMAP_REFPAIR > STRUCTTREEMAP_REF;

