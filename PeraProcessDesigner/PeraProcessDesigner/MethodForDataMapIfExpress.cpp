#include "stdafx.h"
#include "MethodForDataMapIfExpress.h"
#include "PeraProcessDesigner.h"
#include "CStringChecker.h"
#include <boost/regex.hpp>
#include <vector>
using namespace std;

CString GetDataFullPathByID(const CString & strID,CCxStructTreeNode **ppNodeOut)
{
	CString strFullpath = "";
	CCxStructTreeNode *pNodeParam = NULL;
	pNodeParam = theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.Find(strID);
	if (ppNodeOut!=NULL)
		*ppNodeOut = pNodeParam;

	if (NULL==pNodeParam)
	{
		return "";
	}

	CCxStructTreeNode *pCurNode = pNodeParam;
	while(pCurNode)
	{
		if (pCurNode->IsGroupNode())
		{
			pCurNode = pCurNode->m_pParent;
			continue;
		}

		CString strTextCur = pCurNode->m_strOriName;
		if (!pCurNode->IsParamNode())
			strFullpath = strTextCur + CString('.') + strFullpath;
		else
			strFullpath = strTextCur + strFullpath;

		pCurNode = pCurNode->m_pParent;
	}
	return strFullpath;
}
bool ParseDataMapParam(CString strIn,SParam & spLeft,SParam &spRight,CString &strOterSymbol,CString &strOtherParam)
{
	strOterSymbol = "";
	strOtherParam = "";
	spLeft.Clear();
	spRight.Clear();
	const char *szRegMine = "^(#\\{)(\\w{15}-\\w{4}-\\w{4}-\\w{4}-\\w{12})(\\[.*?\\])?(\\})(=)(#\\{)(\\w{15}-\\w{4}-\\w{4}-\\w{4}-\\w{12})(\\[.*?\\])?(\\})(.*?)?$";
	//strIn.Replace(" ","");
	boost::cmatch mat;
	boost::regex reg( szRegMine );
	bool r=boost::regex_match( (LPCTSTR)strIn, mat, reg);
	if (mat[2].matched)
		spLeft.strID = mat[2].str().c_str();

	if (mat[3].matched)
		spLeft.strArry = mat[3].str().c_str();

	if (mat[7].matched)
		spRight.strID = mat[7].str().c_str();

	if (mat[8].matched)
		spRight.strArry = mat[8].str().c_str();

	CString strOterPart;
	if (mat[10].matched)
		strOterPart = mat[10].str().c_str();

	strOterPart.Trim();
	if (!strOterPart.IsEmpty())
	{
		strOterSymbol = strOterPart.Left(1);
		strOtherParam = strOterPart.Mid(1);
		strOtherParam.Trim();
	}
	return true;
}
static bool MatcIdParam(CString strIn,SParam & parm)
{
	boost::cmatch matLeft;
	const char *szRegIdParam = "^(#\\{)(\\w{15}-\\w{4}-\\w{4}-\\w{4}-\\w{12})(\\[.*?\\])?(\\})";
	//strIn.Replace(" ","");
	parm.Clear();
	boost::regex regIdParam( szRegIdParam );
	bool r = boost::regex_match( (LPCTSTR)strIn, matLeft, regIdParam);
	if (!r)
	{
		if (CStringChecker::CheckDouble(strIn))
		{
			parm.strConst = strIn;
			return true;
		}
		else if (CStringChecker::CheckBool(strIn))
		{
			parm.strConst = strIn;
			return true;
		}
		else if (strIn.Left(1)=="\"" && strIn.Right(1)=="\"" )
		{
			parm.strConst = strIn;
			return true;
		}
		return false;
	}

	if (matLeft[2].matched)
		parm.strID = matLeft[2].str().c_str();

	if (matLeft[3].matched)
		parm.strArry = matLeft[3].str().c_str();

	if (parm.strID.IsEmpty()) return false;

	return true;
}
bool ParseIfParam(CString strIn,SParam & spLeft,SParam &spRight,CString &strSymbol)
{
	strSymbol = "";
	spLeft.Clear();
	spRight.Clear();
	const char *szRegMine = "^(.*?)(==|!=|>=|<=|>|<)(.*?)$";
	//strIn.Replace(" ","");
	boost::cmatch mat;
	boost::regex reg( szRegMine );
	bool r=boost::regex_match( (LPCTSTR)strIn, mat, reg);
	if (!mat[1].matched || !mat[2].matched || !mat[3].matched)
		return false;

	CString strLeft = mat[1].str().c_str();
	CString strRight = mat[3].str().c_str();
	strSymbol = mat[2].str().c_str();

	bool bOk = MatcIdParam(strLeft,spLeft);
	if (!bOk)
		return false;
	if (spLeft.strID.IsEmpty())
		return false;

	bOk = MatcIdParam(strRight,spRight);
	if (!bOk)
		return false;
	return true;
}