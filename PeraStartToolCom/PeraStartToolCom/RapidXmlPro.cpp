#include "StdAfx.h"
#include "ToolsProUnity.h"
#include "Tools.h"

#if (!USE_TINY_XML)
#include <iostream> 
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
using namespace rapidxml;
#define  MB_TO_UTF8(strMB)    ZTools::MBToUTF8(string(strMB)).c_str()
#define  UTF8_TO_MB(strUTF8)  ZTools::UTF8ToMB(string(strUTF8)).c_str()

BOOL CToolsProUnity::WriteToolConfigXml(const ToolObject & Tool)
{
	xml_document<>   doc; 
	xml_node<>* pNodeTools = NULL;
	char * pszXmlData = NULL;
	if (PathFileExists(g_strConfigXml))
	{
		file<> fdoc((LPCTSTR)g_strConfigXml);
		pszXmlData = _strdup(fdoc.data());
		doc.parse<0>(pszXmlData);
		xml_node<>* pNodeRoot = doc.first_node("Root");
		if (pNodeRoot==NULL)
			pNodeRoot = doc.allocate_node(node_element,"Root");

		pNodeTools = pNodeRoot->first_node("Tools"); 
		if (pNodeTools==NULL)
			 doc.allocate_node(node_element,"Tools");
	}
	else
	{
		xml_node<>* rot = doc.allocate_node(rapidxml::node_pi,doc.allocate_string( "xml version='1.0' encoding='UTF-8'" ));  
		doc.append_node(rot);  
		xml_node<>* pNodeRoot = doc.allocate_node(node_element,"Root");    
		pNodeTools =   doc.allocate_node(node_element,"Tools");    
		doc.append_node(pNodeRoot);  
		pNodeRoot->append_node(pNodeTools); 
	}

	xml_node<>* pNodeTool = pNodeTools->first_node("Tool"); 
	while(pNodeTool)
	{
		rapidxml::xml_attribute<char> * attr = pNodeTool->first_attribute("softwareId");
		ASSERT(attr);
		CString strToolId = UTF8_TO_MB(attr->value());
		if (strToolId.CompareNoCase(Tool.m_strID)==0)
			break;

		pNodeTool =  pNodeTool->next_sibling();
	}

	if (pNodeTool) 
	{
		pNodeTools->remove_node(pNodeTool);
		pNodeTool = NULL;
	}

	//创建节点
	pNodeTool = doc.allocate_node(node_element,"Tool"); 
	rapidxml::xml_attribute<char> * attr = NULL;

	// 增加属性
	attr = doc.allocate_attribute("softwareId",doc.allocate_string( MB_TO_UTF8(Tool.m_strID)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("softwareName",doc.allocate_string(MB_TO_UTF8(Tool.m_strName)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("softwareDesc",doc.allocate_string(MB_TO_UTF8(Tool.m_strDescription)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("regRootKey",doc.allocate_string(MB_TO_UTF8(Tool.m_strDefaultKey)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("regRootKey1",doc.allocate_string(MB_TO_UTF8(Tool.m_strReverseKey)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("regKeyName",doc.allocate_string(MB_TO_UTF8(Tool.m_strKeyName)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("regValueName",doc.allocate_string(MB_TO_UTF8(Tool.m_strKeyValueName)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("appRelativeDir",doc.allocate_string(MB_TO_UTF8(Tool.m_strRunPath)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("appName",doc.allocate_string(MB_TO_UTF8(Tool.m_strRunName)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("appAbsolutePath",doc.allocate_string(MB_TO_UTF8(Tool.m_strAbsPath)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("appParams",doc.allocate_string(MB_TO_UTF8(Tool.m_strRunParam)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("localAppAbsolutePath",doc.allocate_string(MB_TO_UTF8(Tool.m_strLocalPath)));
	pNodeTool->append_attribute(attr);
	attr = doc.allocate_attribute("localAppParams",doc.allocate_string(MB_TO_UTF8(Tool.m_strLocalRunParam)));
	pNodeTool->append_attribute(attr);

	pNodeTools->append_node(pNodeTool); // 增加节点

	std::ofstream out(g_strConfigXml);  
	out<<doc; // 保存xml
	out.close();

	if (pszXmlData) free(pszXmlData);
	
	return TRUE;
}
void CToolsProUnity:: DeleteToolConfigFromXml(const CString & strToolId)
{
	xml_document<>   doc; 
	xml_node<>* pNodeTools = NULL;
	if (!PathFileExists(g_strConfigXml))
		return;

	file<> fdoc((LPCTSTR)g_strConfigXml);
	doc.parse<0>(fdoc.data());
	xml_node<>* pNodeRoot = doc.first_node("Root");
	if (pNodeRoot==NULL) return;

	pNodeTools = pNodeRoot->first_node("Tools"); 
	if (pNodeTools==NULL) return;

	xml_node<>* pNodeTool = pNodeTools->first_node("Tool"); 
	rapidxml::xml_attribute<char> * attr = NULL;
	while(pNodeTool)
	{
		attr = pNodeTool->first_attribute("softwareId");
		ASSERT(attr);

		CString strToolIdThis = UTF8_TO_MB(attr->value());
		if (strToolIdThis.CompareNoCase(strToolId)==0)
			break;

		pNodeTool = pNodeTool->next_sibling();
	}
	if (pNodeTool==NULL) return;
	pNodeTools->remove_node(pNodeTool);

	std::ofstream out(g_strConfigXml);  
	out<<doc; // 保存xml
	out.close();
}
void CToolsProUnity:: DeleteAllConfigFromXml()
{
	xml_document<>   doc; 
	xml_node<>* pNodeTools = NULL;
	if (!PathFileExists(g_strConfigXml))
		return;

	file<> fdoc((LPCTSTR)g_strConfigXml);
	doc.parse<0>(fdoc.data());
	xml_node<>* pNodeRoot = doc.first_node("Root");
	if (pNodeRoot==NULL) return;

	pNodeTools = pNodeRoot->first_node("Tools"); 
	if (pNodeTools==NULL) return;

	pNodeTools->remove_all_nodes(); // 删除所有子节点

	std::ofstream out(g_strConfigXml);  
	out<<doc; // 保存xml
	out.close();
}
BOOL CToolsProUnity::GetToolFormXml(const CString & strId,ToolObject & toolLocal)
{
	xml_document<>   doc; 
	xml_node<>* pNodeTools = NULL;
	toolLocal.Clear();
	if (!PathFileExists(g_strConfigXml))
		return FALSE;

	file<> fdoc((LPCTSTR)g_strConfigXml);
	doc.parse<0>(fdoc.data());
	xml_node<>* pNodeRoot = doc.first_node("Root");
	if (pNodeRoot==NULL) return FALSE;

	pNodeTools = pNodeRoot->first_node("Tools"); 
	if (pNodeTools==NULL) return FALSE;

	xml_node<>* pNodeTool = pNodeTools->first_node("Tool"); 
	rapidxml::xml_attribute<char> * attr = NULL;
	while(pNodeTool)
	{
		attr = pNodeTool->first_attribute("softwareId");
		ASSERT(attr);

		CString strToolIdThis = UTF8_TO_MB(attr->value());
		if (strToolIdThis.CompareNoCase(strId)==0)
			break;

		pNodeTool = pNodeTool->next_sibling();
	}
	if (pNodeTool==NULL) return FALSE;

	toolLocal.m_strID = UTF8_TO_MB(pNodeTool->first_attribute("softwareId")->value());
	toolLocal.m_strName = UTF8_TO_MB(pNodeTool->first_attribute("softwareName")->value());
	toolLocal.m_strDesrciption = UTF8_TO_MB(pNodeTool->first_attribute("softwareDesc")->value());
	toolLocal.m_strDefaultKey = UTF8_TO_MB(pNodeTool->first_attribute("regRootKey")->value());
	toolLocal.m_strReverseKey = UTF8_TO_MB(pNodeTool->first_attribute("regRootKey1")->value());
	toolLocal.m_strKeyName = UTF8_TO_MB(pNodeTool->first_attribute("regKeyName")->value());
	toolLocal.m_strKeyValueName = UTF8_TO_MB(pNodeTool->first_attribute("regValueName")->value());
	toolLocal.m_strRunPath = UTF8_TO_MB(pNodeTool->first_attribute("appRelativeDir")->value());
	toolLocal.m_strRunName = UTF8_TO_MB(pNodeTool->first_attribute("appName")->value());
	toolLocal.m_strAbsPath = UTF8_TO_MB(pNodeTool->first_attribute("appAbsolutePath")->value());
	toolLocal.m_strRunParam = UTF8_TO_MB(pNodeTool->first_attribute("appParams")->value());
	toolLocal.m_strLocalPath = UTF8_TO_MB(pNodeTool->first_attribute("localAppAbsolutePath")->value());
	toolLocal.m_strLocalRunParam = UTF8_TO_MB(pNodeTool->first_attribute("localAppParams")->value());

	return TRUE;
}
BOOL CToolsProUnity::LoadAllLocalTool(map<CString,ToolObject> & mapTools)
{
	xml_document<>   doc; 
	xml_node<>* pNodeTools = NULL;
	mapTools.clear();
	if (!PathFileExists(g_strConfigXml))
		return FALSE;

	file<> fdoc((LPCTSTR)g_strConfigXml);
	doc.parse<0>(fdoc.data());
	xml_node<>* pNodeRoot = doc.first_node("Root");
	if (pNodeRoot==NULL) return FALSE;

	pNodeTools = pNodeRoot->first_node("Tools"); 
	if (pNodeTools==NULL) return FALSE;

	xml_node<>* pNodeTool = pNodeTools->first_node("Tool"); 
	rapidxml::xml_attribute<char> * attr = NULL;

	try
	{
		while(pNodeTool)
		{
			attr = pNodeTool->first_attribute("softwareId");
			ToolObject toolLocal;
			toolLocal.m_strID = UTF8_TO_MB(pNodeTool->first_attribute("softwareId")->value());
			toolLocal.m_strName = UTF8_TO_MB(pNodeTool->first_attribute("softwareName")->value());
			toolLocal.m_strDesrciption = UTF8_TO_MB(pNodeTool->first_attribute("softwareDesc")->value());
			toolLocal.m_strDefaultKey = UTF8_TO_MB(pNodeTool->first_attribute("regRootKey")->value());
			toolLocal.m_strReverseKey = UTF8_TO_MB(pNodeTool->first_attribute("regRootKey1")->value());
			toolLocal.m_strKeyName = UTF8_TO_MB(pNodeTool->first_attribute("regKeyName")->value());
			toolLocal.m_strKeyValueName = UTF8_TO_MB(pNodeTool->first_attribute("regValueName")->value());
			toolLocal.m_strRunPath = UTF8_TO_MB(pNodeTool->first_attribute("appRelativeDir")->value());
			toolLocal.m_strRunName = UTF8_TO_MB(pNodeTool->first_attribute("appName")->value());
			toolLocal.m_strAbsPath = UTF8_TO_MB(pNodeTool->first_attribute("appAbsolutePath")->value());
			toolLocal.m_strRunParam = UTF8_TO_MB(pNodeTool->first_attribute("appParams")->value());
			toolLocal.m_strLocalPath = UTF8_TO_MB(pNodeTool->first_attribute("localAppAbsolutePath")->value());
			toolLocal.m_strLocalRunParam = UTF8_TO_MB(pNodeTool->first_attribute("localAppParams")->value());

			mapTools.insert(make_pair(toolLocal.m_strID,toolLocal));
			pNodeTool = pNodeTool->next_sibling();
		}
	}
	catch (...)
	{
		mapTools.clear();
		ZTools::WriteZToolsFormatLog("RapidXML ----LoadAllLocalTool加载全部配置记录出现异常....");
		return FALSE;
	}
	return TRUE;
}

#endif