#include "StdAfx.h"
#include "CxStructTreeNodeForm.h"

#include "value.h"
#include "reader.h"
#include "writer.h"
#include "PeraProcessDesigner.h"

//static BOOL ConvertJsonObj( Json::Value& obj, LPVOID lpJsonObj )
//{
//	Json::Value NewObj;
//	obj = NewObj;
//	Json::Value * pObj = reinterpret_cast< Json::Value * > ( lpJsonObj );
//	if ( pObj == NULL ) return FALSE;
//	obj = *pObj;
//	if ( !obj.isObject() ) return FALSE;
//	return TRUE;
//}

CCxStructTreeNodeForm::CCxStructTreeNodeForm( CCxStructTree * pTree )
	: CCxStructTreeNode( pTree )
{

}

CCxStructTreeNodeForm::~CCxStructTreeNodeForm()
{

}

void CCxStructTreeNodeForm::Clear( void )
{
	m_strAuthor = "";
	m_strDescription = "";
	m_strCreated = "";
	m_strVersion = "";
	CCxStructTreeNode::Clear();
}

BOOL CCxStructTreeNodeForm::OnParsing( Json::Value& _jsonObj )
{
	if ( !CCxStructTreeNode::OnParsing( _jsonObj ) ) return FALSE;

//	Json::Value obj;
//	if ( !ConvertJsonObj( obj, lpJsonObj ) ) return FALSE;
	CString str;

	try
	{
		m_strDescription = (_jsonObj["Description"].isString()) ? _jsonObj["Description"].asCString() : _T("");
		m_strAuthor = (_jsonObj["Author"].isString()) ? _jsonObj["Author"].asCString() : _T("");
		m_strCreated = (_jsonObj["Created"].isString()) ? _jsonObj["Created"].asCString() : _T("");
		m_strVersion = (_jsonObj["Version"].isString()) ? _jsonObj["Version"].asCString() : _T("");
		m_strLocalPath = (_jsonObj["localPath"].isString()) ? _jsonObj["localPath"].asCString() : _T("");
		if (m_strLocalPath.IsEmpty())
		{
			::MessageBox(GetMainWnd(), "取表单路径失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
		}
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}

CString CCxStructTreeNodeForm::GetTooltipText( void )
{
	CString strTip = m_strOriName;
	/*strTip.Format( "名称：%s\r\n作者：%s\r\n时间：%s\r\n版本：%s\r\n描述：%s"
	, m_strOriName
	, m_strAuthor
	, m_strCreated
	, m_strVersion
	, m_strDescription);*/
	return strTip;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
