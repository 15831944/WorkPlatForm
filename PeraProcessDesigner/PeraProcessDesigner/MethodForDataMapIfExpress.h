#pragma once 
#include "stdafx.h"

class CCxStructTreeNode;
/************************************************************************/
/* 数据映射和条件表达式JS传入的OldExp解析结果存储结构体                   */
/************************************************************************/
typedef struct
{
	CString strID;   // 参数ID
	CString strArry; // 数组映射时，数组下标
	CString strConst; // 若为常量参数，则该值非空
	CString strParamFullPath;
	void Clear()
	{
		strID.Empty();
		strArry.Empty();
		strConst.Empty();
		strParamFullPath.Empty();
	}
}SParam;
/************************************************************************/
/* 条件表达式参数结构体                                                      */
/************************************************************************/
typedef struct
{
	CString m_strIdExpress;
	CString m_strFullPathExpress;
	SParam  m_LeftParam;
	SParam  m_RightParam;
	CString m_strExpressSymbol;
	void Clear()
	{
		m_strIdExpress.Empty();
		m_strFullPathExpress.Empty();
		m_LeftParam.Clear();
		m_RightParam.Clear();
		m_strExpressSymbol.Empty();
	}
}BranchExpress;

/*
* 根据节点ID获取节点参数的全路径，并输出参数节点的指针
*/
CString GetDataFullPathByID(const CString & strID,CCxStructTreeNode **ppNodeOut = NULL);
/*
* 从数据映射ID表达式解析出各个部分
*/
bool ParseDataMapParam(CString strIn,SParam & spLeft,SParam &spRight,CString &strOterSymbol,CString &strOtherParam);
/*
* 从ID条件表达式解析出各个部分
*/
bool ParseIfParam(CString strIn,SParam & spLeft,SParam &spRight,CString &strSymbol);