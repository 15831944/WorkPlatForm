#include "StdAfx.h"
#include "DataInterface.h"
#include <json/reader.h>
#include <json/writer.h>
CDataInvokeApp::CDataInvokeApp(void)
{
	m_pToolObj = NULL;
}


CDataInvokeApp::~CDataInvokeApp(void)
{
	Clear();
}
void CDataInvokeApp::Clear()
{
	if (m_pToolObj)
	{
		delete m_pToolObj;
		m_pToolObj = NULL;
	}
}
BOOL CDataInvokeApp::Parse(LPCTSTR lpsIn)
{
	Clear();

	CString strIn;
	if (lpsIn == NULL) lpsIn = "";
	strIn = lpsIn;
	if (strIn.IsEmpty())
		return FALSE;

	Json::Reader reader;
	Json::Value obj;
	CString str;

	try
	{
		if (!reader.parse((char*)(LPCTSTR)strIn, obj, false)) return FALSE;
		if (!obj.isObject()) return FALSE;

		m_pToolObj = new ToolObject;
		m_pToolObj->m_strID = obj["softwareId"].isString() ? obj["softwareId"].asCString():"";
		m_pToolObj->m_strName = obj["softwareName"].isString() ? obj["softwareName"].asCString():"";
		m_pToolObj->m_strDescription = obj["softwareDesc"].isString() ? obj["softwareDesc"].asCString():"";
		m_pToolObj->m_strDefaultKey = obj["regRootKey"].isString() ? obj["regRootKey"].asCString():"";
		m_pToolObj->m_strReverseKey = obj["regRootKey1"].isString() ? obj["regRootKey1"].asCString():"";
		m_pToolObj->m_strKeyName = obj["regKeyName"].isString() ? obj["regKeyName"].asCString():"";
		m_pToolObj->m_strKeyValueName = obj["regValueName"].isString() ? obj["regValueName"].asCString():"";
		m_pToolObj->m_strRunPath = obj["appRelativeDir"].isString() ? obj["appRelativeDir"].asCString():"";
		m_pToolObj->m_strRunName = obj["appName"].isString() ? obj["appName"].asCString():"";
		m_pToolObj->m_strAbsPath = obj["appAbsolutePath"].isString() ? obj["appAbsolutePath"].asCString():"";
		m_pToolObj->m_strRunParam = obj["appParams"].isString() ? obj["appParams"].asCString():"";

		m_pToolObj->m_strPathConfigType = obj["pathConfig"].isString() ? obj["pathConfig"].asCString():"";
		m_pToolObj->m_strURL = obj["urlAddress"].isString() ? obj["urlAddress"].asCString():"";
	}
	catch(...)
	{
		Clear();
		return FALSE;
	}

	return TRUE;
}