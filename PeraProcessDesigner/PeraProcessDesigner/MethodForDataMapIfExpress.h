#pragma once 
#include "stdafx.h"

class CCxStructTreeNode;
/************************************************************************/
/* ����ӳ����������ʽJS�����OldExp��������洢�ṹ��                   */
/************************************************************************/
typedef struct
{
	CString strID;   // ����ID
	CString strArry; // ����ӳ��ʱ�������±�
	CString strConst; // ��Ϊ�������������ֵ�ǿ�
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
/* �������ʽ�����ṹ��                                                      */
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
* ���ݽڵ�ID��ȡ�ڵ������ȫ·��������������ڵ��ָ��
*/
CString GetDataFullPathByID(const CString & strID,CCxStructTreeNode **ppNodeOut = NULL);
/*
* ������ӳ��ID���ʽ��������������
*/
bool ParseDataMapParam(CString strIn,SParam & spLeft,SParam &spRight,CString &strOterSymbol,CString &strOtherParam);
/*
* ��ID�������ʽ��������������
*/
bool ParseIfParam(CString strIn,SParam & spLeft,SParam &spRight,CString &strSymbol);