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

	BOOL		m_bIsArray;			//是否 数组
	CString		m_sDataType;		//数据类型
	CString		m_sDirection;		//传递方向
	CString		m_sValue;			//值
	CString		m_sUnit;			//单位

	//------------以下为2014-7-25添加----------------------//
	CString		m_sArrayUpperIndex; //数组维数
	CString		m_sArrayLowerIndex; //数组下标
	CString		m_sFormat;
	CString		m_sState;
	CString		m_sGroup;
	BOOL		m_bMultifiles;
	BOOL		m_bIsSaveInModel;
	BOOL		m_bIsDynamicArray;
	BOOL		m_bReadOnly; //现在是指该参数被映射，不可修改
	CString		m_sListName;
	CString		m_sOutValue; //对于InOut类型的参数，In存放在m_sValue中，out类型存放在此处，临时存放，不会更新进xpdl
	CString		m_sArrayUpperIndex_Out; //数组维数
	CString		m_sArrayLowerIndex_Out; //数组下标 对应m_sOutValue，用于临时存放，不更新到xpdl



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

