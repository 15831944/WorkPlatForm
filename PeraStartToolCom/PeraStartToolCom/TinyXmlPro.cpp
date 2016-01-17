#include "StdAfx.h"
#include "ToolsProUnity.h"

#if USE_TINY_XML
	#include "tinyxml.h"
	#include "Tools.h"
	#define  MB_TO_UTF8(strMB)    ZTools::MBToUTF8(string(strMB)).c_str()
	#define  UTF8_TO_MB(strUTF8)  ZTools::UTF8ToMB(string(strUTF8)).c_str()

	BOOL CToolsProUnity::WriteToolConfigXml(const ToolObject & Tool)
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		CString strXml = g_strConfigXml;

		TiXmlDocument doc;
		if (PathFileExists(strXml))
		{
			if (!doc.LoadFile(strXml))
				return FALSE;
		}
		else
		{
			TiXmlDeclaration *pDeclaration=new TiXmlDeclaration("1.0","UTF-8","");   //创建xml文件头
			doc.LinkEndChild(pDeclaration);        //加入文件头
			doc.SaveFile(strXml);
			if (!doc.LoadFile(strXml))
				return FALSE;
		}

		TiXmlElement* rootElement = doc.RootElement(); 
		if (NULL==rootElement)
		{
			rootElement =  new TiXmlElement("Root");
			if (NULL==rootElement)
				return FALSE;
			// 设置节点的属性值，然后插入节点
			doc.LinkEndChild(rootElement);
		}

		TiXmlElement* pToolsElement = rootElement->FirstChildElement("Tools"); 
		if (NULL==pToolsElement)
		{
			pToolsElement = new TiXmlElement("Tools");
			if (NULL==pToolsElement)
				return FALSE;
			rootElement->LinkEndChild(pToolsElement);
		}

		TiXmlElement * pTool = pToolsElement->FirstChildElement("Tool");
		while(pTool)
		{
			//参数方向
			CString strToolId = UTF8_TO_MB(pTool->Attribute("softwareId"));
			if (strToolId.CompareNoCase(Tool.m_strID)==0)
				break;

			pTool = pTool->NextSiblingElement("Tool"); // 寻找下一个兄弟节点
		}

		BOOL bNewInsert = FALSE;
		if (pTool==NULL) // 没有设置过该节点
		{
			TiXmlElement *pNewToolNode = new TiXmlElement("Tool");
			if (NULL==pNewToolNode)
				return FALSE;

			bNewInsert = TRUE;
			pTool = pNewToolNode;
		}
		// 设置节点的属性值，然后插入节点
		pTool->SetAttribute("softwareId",MB_TO_UTF8(Tool.m_strID));
		pTool->SetAttribute("softwareName",MB_TO_UTF8(Tool.m_strName));
		pTool->SetAttribute("softwareDesc",MB_TO_UTF8(Tool.m_strDesrciption));
		pTool->SetAttribute("regRootKey",MB_TO_UTF8(Tool.m_strDefaultKey));
		pTool->SetAttribute("regRootKey1",MB_TO_UTF8(Tool.m_strReverseKey));
		pTool->SetAttribute("regKeyName",MB_TO_UTF8(Tool.m_strKeyName));
		pTool->SetAttribute("regValueName",MB_TO_UTF8(Tool.m_strKeyValueName));
		pTool->SetAttribute("appRelativeDir",MB_TO_UTF8(Tool.m_strRunPath));
		pTool->SetAttribute("appName",MB_TO_UTF8(Tool.m_strRunName));
		pTool->SetAttribute("appAbsolutePath",MB_TO_UTF8(Tool.m_strAbsPath));
		pTool->SetAttribute("appParams",MB_TO_UTF8(Tool.m_strRunParam));
		pTool->SetAttribute("localAppAbsolutePath",MB_TO_UTF8(Tool.m_strLocalPath));
		pTool->SetAttribute("localAppParams",MB_TO_UTF8(Tool.m_strLocalRunParam));
		if (bNewInsert)
			pToolsElement->LinkEndChild(pTool);

		doc.SaveFile(strXml);
		return TRUE;
	}
	void CToolsProUnity:: DeleteToolConfigFromXml(const CString & strToolId)
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		CString strXml = g_strConfigXml;
		TiXmlDocument doc;
		if (!PathFileExists(strXml) || !doc.LoadFile(strXml))
			return;

		TiXmlElement* rootElement = doc.RootElement();
		if (NULL==rootElement) return ;

		TiXmlElement* pToolsElement = rootElement->FirstChildElement("Tools");
		if (NULL==pToolsElement) return ;

		TiXmlElement * pTool = pToolsElement->FirstChildElement("Tool");
		while(pTool)
		{
			CString strToolIdThis = UTF8_TO_MB(pTool->Attribute("softwareId"));
			if (strToolIdThis.CompareNoCase(strToolId)==0)
				break;

			pTool = pTool->NextSiblingElement("Tool"); // 寻找下一个兄弟节点
		}

		if (pTool)
		{
			pToolsElement->RemoveChild(pTool);
			doc.SaveFile(strXml);
		}
	}
	void CToolsProUnity:: DeleteAllConfigFromXml()
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		CString strXml = g_strConfigXml;
		TiXmlDocument doc;
		if (!PathFileExists(strXml) || !doc.LoadFile(strXml))
			return;

		TiXmlElement* rootElement = doc.RootElement();
		if (NULL==rootElement) return ;

		TiXmlElement* pToolsElement = rootElement->FirstChildElement("Tools");
		if (NULL==pToolsElement) return ;

		TiXmlElement * pTool = pToolsElement->FirstChildElement("Tool");
		while(pTool)
		{
			TiXmlElement *pNext = pTool->NextSiblingElement("Tool"); // 寻找下一个兄弟节点
			pToolsElement->RemoveChild(pTool);
			pTool = pNext;
		}

		doc.SaveFile(strXml);
	}
	BOOL CToolsProUnity::GetToolFormXml(const CString & strId,ToolObject & toolLocal)
	{
		toolLocal.Clear();
		TiXmlBase::SetCondenseWhiteSpace(false);
		CString strXml = g_strConfigXml;
		TiXmlDocument doc;
		if (!PathFileExists(strXml) || !doc.LoadFile(strXml))
			return FALSE;

		TiXmlElement* rootElement = doc.RootElement();
		if (NULL==rootElement) return FALSE;

		TiXmlElement* pToolsElement = rootElement->FirstChildElement("Tools");
		if (NULL==pToolsElement) return FALSE;

		TiXmlElement * pTool = pToolsElement->FirstChildElement("Tool");
		while(pTool)
		{
			CString strToolIdThis = UTF8_TO_MB(pTool->Attribute("softwareId"));
			if (strToolIdThis.CompareNoCase(strId)==0)
				break;

			pTool = pTool->NextSiblingElement("Tool"); // 寻找下一个兄弟节点
		}
		if (pTool==NULL)
			return FALSE;

		toolLocal.m_strID = UTF8_TO_MB(pTool->Attribute("softwareId"));
		toolLocal.m_strName = UTF8_TO_MB(pTool->Attribute("softwareName"));
		toolLocal.m_strDesrciption = UTF8_TO_MB(pTool->Attribute("softwareDesc"));
		toolLocal.m_strDefaultKey = UTF8_TO_MB(pTool->Attribute("regRootKey"));
		toolLocal.m_strReverseKey = UTF8_TO_MB(pTool->Attribute("regRootKey1"));
		toolLocal.m_strKeyName = UTF8_TO_MB(pTool->Attribute("regKeyName"));
		toolLocal.m_strKeyValueName = UTF8_TO_MB(pTool->Attribute("regValueName"));
		toolLocal.m_strRunPath = UTF8_TO_MB(pTool->Attribute("appRelativeDir"));
		toolLocal.m_strRunName = UTF8_TO_MB(pTool->Attribute("appName"));
		toolLocal.m_strAbsPath = UTF8_TO_MB(pTool->Attribute("appAbsolutePath"));
		toolLocal.m_strRunParam = UTF8_TO_MB(pTool->Attribute("appParams"));
		toolLocal.m_strLocalPath = UTF8_TO_MB(pTool->Attribute("localAppAbsolutePath"));
		toolLocal.m_strLocalRunParam = UTF8_TO_MB(pTool->Attribute("localAppParams"));
		return TRUE;
	}
	BOOL CToolsProUnity::LoadAllLocalTool(map<CString,ToolObject> & mapTools)
	{
		mapTools.clear();
		TiXmlBase::SetCondenseWhiteSpace(false);
		CString strXml = g_strConfigXml;
		TiXmlDocument doc;
		if (!PathFileExists(strXml) || !doc.LoadFile(strXml))
			return FALSE;

		TiXmlElement* rootElement = doc.RootElement();
		if (NULL==rootElement) return FALSE;

		TiXmlElement* pToolsElement = rootElement->FirstChildElement("Tools");
		if (NULL==pToolsElement) return FALSE;

		TiXmlElement * pTool = pToolsElement->FirstChildElement("Tool");
		while(pTool)
		{
			ToolObject toolLocal;
			toolLocal.m_strID = UTF8_TO_MB(pTool->Attribute("softwareId"));
			toolLocal.m_strName = UTF8_TO_MB(pTool->Attribute("softwareName"));
			toolLocal.m_strDesrciption = UTF8_TO_MB(pTool->Attribute("softwareDesc"));
			toolLocal.m_strDefaultKey = UTF8_TO_MB(pTool->Attribute("regRootKey"));
			toolLocal.m_strReverseKey = UTF8_TO_MB(pTool->Attribute("regRootKey1"));
			toolLocal.m_strKeyName = UTF8_TO_MB(pTool->Attribute("regKeyName"));
			toolLocal.m_strKeyValueName = UTF8_TO_MB(pTool->Attribute("regValueName"));
			toolLocal.m_strRunPath = UTF8_TO_MB(pTool->Attribute("appRelativeDir"));
			toolLocal.m_strRunName = UTF8_TO_MB(pTool->Attribute("appName"));
			toolLocal.m_strAbsPath = UTF8_TO_MB(pTool->Attribute("appAbsolutePath"));
			toolLocal.m_strRunParam = UTF8_TO_MB(pTool->Attribute("appParams"));
			toolLocal.m_strLocalPath = UTF8_TO_MB(pTool->Attribute("localAppAbsolutePath"));
			toolLocal.m_strLocalRunParam = UTF8_TO_MB(pTool->Attribute("localAppParams"));
			mapTools[toolLocal.m_strID] = toolLocal;
			pTool = pTool->NextSiblingElement("Tool"); // 寻找下一个兄弟节点
		}
		return TRUE;
	}

#endif // USE_TINY_XML