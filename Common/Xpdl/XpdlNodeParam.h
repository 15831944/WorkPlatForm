#pragma once
#include "xpdlnode.h"

class CXpdlNodeParam : public CXpdlNode
{
	friend class CXpdl;
public:
	CXpdlNodeParam(void);

	virtual ~CXpdlNodeParam(void);

	virtual BOOL Copy( CXpdlNodeParam* pSrcNode );

public:

	BOOL		m_bIsArray;			//�Ƿ� ����
	CString		m_sDataType;		//��������
	CString		m_sDirection;		//���ݷ���
	CString		m_sValue;			//ֵ
	CString		m_sUnit;			//��λ

	//------------����Ϊ2014-7-25���----------------------//
	CString		m_sArrayUpperIndex; //����ά��
	CString		m_sArrayLowerIndex; //�����±�
	CString		m_sFormat;
	CString		m_sState;
	CString		m_sGroup;
	BOOL		m_bMultifiles;
	BOOL		m_bIsSaveInModel;
	BOOL		m_bIsDynamicArray;
	BOOL		m_bReadOnly; //������ָ�ò�����ӳ�䣬�����޸�
	CString		m_sListName;
	CString		m_sOutValue; //����InOut���͵Ĳ�����In�����m_sValue�У�out���ʹ���ڴ˴�����ʱ��ţ�������½�xpdl
	CString		m_sArrayUpperIndex_Out; //����ά��
	CString		m_sArrayLowerIndex_Out; //�����±� ��Ӧm_sOutValue��������ʱ��ţ������µ�xpdl



	enum
	{
		XPDL_PARAMTYPE_NULL = 0,
		XPDL_PARAMTYPE_INTEGER,
		XPDL_PARAMTYPE_FLOAT,
		XPDL_PARAMTYPE_REFERENCE,
		XPDL_PARAMTYPE_STRING,
		XPDL_PARAMTYPE_BOOLEAN
	};
};

