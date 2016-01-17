#include "stdafx.h"
#include "UpdateDownloadBotWsFile.h"
#include "FileTool.h"
#include "ZipTool.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "Tools.h"

using namespace rapidxml;

#define  MB_TO_UTF8(strMB)    ZTools::MBToUTF8(string(strMB)).c_str()
#define  UTF8_TO_MB(strUTF8)  ZTools::UTF8ToMB(string(strUTF8)).c_str()

// ���´���CDATA��ʶ�Ľڵ�ֵ
void UpdateCDataNode(xml_document<> & XmlDoc,xml_node<>* pNode,const char *pszNodeName,const char *pszNodeValue,BOOL & bbChanged)
{
	xml_node<>* pNodeOldName = pNode->first_node(MB_TO_UTF8(pszNodeName));
	if (pNodeOldName==NULL)
		return;

	xml_node<>* pNodeOldValue = pNodeOldName->first_node();
	if (pNodeOldValue==NULL)
		return;

	string strValue =pNodeOldValue->value();
	strValue  = UTF8_TO_MB(strValue);
	if (strValue != pszNodeValue)
	{
		pNode->remove_node(pNodeOldName); // ɾ���ڵ�

		xml_node<>* pNodeNewName  = XmlDoc.allocate_node(node_element,XmlDoc.allocate_string(MB_TO_UTF8(pszNodeName))); 
		xml_node<>* pNodeNewValue = XmlDoc.allocate_node(node_cdata,"",XmlDoc.allocate_string(MB_TO_UTF8(pszNodeValue))); 
		pNodeNewName->append_node(pNodeNewValue);
		pNode->append_node(pNodeNewName); // �����½ڵ�
		bbChanged = TRUE;
	}
}

// ���½ڵ������ֵ
void UpdateNodeAttribtute(xml_document<> & XmlDoc,xml_node<>* pNode,const char *pszAttName,const char *pszNewAttValue,BOOL & bbChanged)
{
	rapidxml::xml_attribute<char> * attr = pNode->first_attribute(MB_TO_UTF8(pszAttName));
	string strValue  = UTF8_TO_MB(attr->value());
	if (strValue != pszNewAttValue)
	{
		pNode->remove_attribute(attr); // ɾ������
		attr = XmlDoc.allocate_attribute(XmlDoc.allocate_string(MB_TO_UTF8(pszAttName)),XmlDoc.allocate_string(MB_TO_UTF8(pszNewAttValue)));
		if (attr!=NULL)
			pNode->append_attribute(attr); // ������Ӹ�����

		bbChanged = TRUE;
	}
}

void UpdateExtentExtendedAttributeNode(xml_document<> & XmlDoc,xml_node<>* pNodeParent,const char * pszNodeName,const char *pszAttName,
	const char *pszNewAttValue,BOOL & bbChanged)
{
	xml_node<>* pNodeOld = pNodeParent->first_node();
	string strValueName;
	string strValue;
	while(pNodeOld)
	{
		strValueName = UTF8_TO_MB(pNodeOld->first_attribute("Name")->value());
		if (strValueName == pszAttName)
		{
			UpdateNodeAttribtute(XmlDoc,pNodeOld,"Value",pszNewAttValue,bbChanged);
			break;
		}
		pNodeOld = pNodeOld->next_sibling();
	}
}
bool UpdateDownloadProcess(CResLibData * pProcessData,const char * pszSrcWs,const char * pszOutWsFile)
{
	if (pProcessData==NULL || pszSrcWs==NULL || pszOutWsFile==NULL)
		return false;

	// ��ws�ļ���ȡmain.xpdl�ļ�����
	string strContent = ZipTool::GetContentFromZip(pszSrcWs,"main.xpdl");

	xml_document<> XmlDoc;
	XmlDoc.parse<parse_full>((char*)strContent.c_str());
	xml_node<>* pNodePackage = XmlDoc.first_node("Package");
	if (pNodePackage==NULL)
	{
		ZTools::WriteZToolsFormatLog("main.xpdl��δ�ҵ�Package�ڵ�");
		return false;
	}

	BOOL bChanged = FALSE;
	UpdateNodeAttribtute(XmlDoc,pNodePackage,"Name",pProcessData->m_sName.c_str(),bChanged);
	xml_node<>* pNodeHeader = NULL;
	xml_node<>* pNodeExtendedAttributesNode = NULL;
	
	xml_node<>* pNode = pNodePackage->first_node("WorkflowProcesses");
	if (pNode!=NULL)
	{
		pNode = pNode->first_node("WorkflowProcess");
		if (pNode!=NULL)
		{
			UpdateNodeAttribtute(XmlDoc,pNode,"Name",pProcessData->m_sName.c_str(),bChanged);
			pNodeHeader = pNode->first_node("ProcessHeader");
			pNodeExtendedAttributesNode = pNode->first_node("ExtendedAttributes");
		}
	}

	if (pNodeHeader)
	{
		UpdateCDataNode(XmlDoc,pNodeHeader,"Author",pProcessData->m_sAuthor.c_str(),bChanged);
		UpdateCDataNode(XmlDoc,pNodeHeader,"Created",pProcessData->m_sCreatTime.c_str(),bChanged);
		UpdateCDataNode(XmlDoc,pNodeHeader,"Description",pProcessData->m_sDsec.c_str(),bChanged);
		UpdateCDataNode(XmlDoc,pNodeHeader,"Version",pProcessData->m_sVersions.c_str(),bChanged);
	}
	if (pNodeExtendedAttributesNode)
	{
		UpdateExtentExtendedAttributeNode(XmlDoc,pNodeExtendedAttributesNode,"ExtendedAttribute","Author",
			pProcessData->m_sAuthor.c_str(),bChanged);
		UpdateExtentExtendedAttributeNode(XmlDoc,pNodeExtendedAttributesNode,"ExtendedAttribute","Created",
			pProcessData->m_sCreatTime.c_str(),bChanged);
		UpdateExtentExtendedAttributeNode(XmlDoc,pNodeExtendedAttributesNode,"ExtendedAttribute","Version",
			pProcessData->m_sVersions.c_str(),bChanged);
	}
	if (bChanged)
	{
		std::string strNewContent;  
		rapidxml::print(std::back_inserter(strNewContent),XmlDoc,/*rapidxml::print_no_indenting*/0);
		ZTools::WriteZToolsFormatLog("��ʼ�滻zip�е��ļ�����,����zip��%s�������zip��%s��",pszSrcWs,pszOutWsFile);
		return ZipTool::ReplaceZipFile(pszSrcWs,pszOutWsFile,"main.xpdl",strNewContent,8);
	}
	else
	{
		// ������Ϣδ�����仯ʱ��ֱ�ӿ�����Ŀ�ĵ�
		if (CFileTool::CopyFile((char*)pszOutWsFile,(char*)pszSrcWs))
			return true;

		return false;
	}
}


void UpdateNodeProperty(xml_document<> & XmlDoc,xml_node<>* pNodeParent,const char * pszPopName,const char * pszNewPopValue,BOOL & bbChanged)
{
	xml_node<>* pNodeOld = pNodeParent->first_node();
	string strValueName;
	string strValue;
	while(pNodeOld)
	{
		strValueName = UTF8_TO_MB(pNodeOld->first_attribute("name")->value());
		strValue = UTF8_TO_MB(pNodeOld->value());

		//�ҵ���Ӧ�����Խڵ��ˣ�������ֵ�����˱仯
		if (strValueName == pszPopName && strValue!=pszNewPopValue )
		{
			bbChanged = TRUE;
			xml_node<>* pNodeWhereInsert = pNodeOld->next_sibling();
			pNodeParent->remove_node(pNodeOld);
			
			// �����ڵ�ʱ������Ĭ��ֵ
			xml_node<>* pNodeNew  = XmlDoc.allocate_node(node_element,"Property",XmlDoc.allocate_string(MB_TO_UTF8(pszNewPopValue))); 
			rapidxml::xml_attribute<char> * attr = NULL;
			attr = XmlDoc.allocate_attribute("name",XmlDoc.allocate_string(MB_TO_UTF8(pszPopName)));
			if (attr!=NULL)
				pNodeNew->append_attribute(attr); // ��Ӹ�����

			pNodeParent->insert_node(pNodeWhereInsert,pNodeNew);
			break;
		}
		pNodeOld = pNodeOld->next_sibling();
	}
}

string UpdateBotMainfestXml(string strInXmlStream,CResLibData * pProcessData,BOOL & bChanged)
{
	xml_document<> XmlDoc;
	char *pData = strdup(strInXmlStream.c_str());
	XmlDoc.parse<parse_full>(pData);

	xml_node<>* pNodeManifest = XmlDoc.first_node("Manifest");
	if (pNodeManifest==NULL)
	{
		ZTools::WriteZToolsFormatLog("Manifest.xml��δ�ҵ�Manifest�ڵ�");
		free(pData);
		return false;
	}

	xml_node<>* pNodeComponent = pNodeManifest->first_node("Component");
	if (pNodeComponent==NULL)
	{
		ZTools::WriteZToolsFormatLog("Manifest�ڵ���δ�ҵ�Component�ڵ�");
		free(pData);
		return false;
	}
	xml_node<>* pNodeProperties = pNodeComponent->first_node("Properties");
	if (pNodeProperties==NULL)
	{
		ZTools::WriteZToolsFormatLog("Component�ڵ���δ�ҵ�Properties�ڵ�");
		free(pData);
		return false;
	}

	UpdateNodeAttribtute(XmlDoc,pNodeComponent,"RobotName",pProcessData->m_sName.c_str(),bChanged);
	UpdateNodeProperty(XmlDoc,pNodeProperties,"Version",pProcessData->m_sVersions.c_str(),bChanged);
	UpdateNodeProperty(XmlDoc,pNodeProperties,"Author",pProcessData->m_sAuthor.c_str(),bChanged);
	UpdateNodeProperty(XmlDoc,pNodeProperties,"Description",pProcessData->m_sDsec.c_str(),bChanged);
	UpdateNodeProperty(XmlDoc,pNodeProperties,"CreateTime",pProcessData->m_sCreatTime.c_str(),bChanged);
	// ModifyTimeʹ�ú�CreateTime��ͬ��ʱ��
	UpdateNodeProperty(XmlDoc,pNodeProperties,"ModifyTime",pProcessData->m_sCreatTime.c_str(),bChanged);

	if (bChanged)
	{
		/*CTime tm = CTime::GetCurrentTime();
		CString strModifyTime =  tm.Format("%Y-%m-%d %H:%M:%S");
		UpdateNodeProperty(XmlDoc,pNodeProperties,"ModifyTime",(LPCTSTR)strModifyTime,bChanged);*/

		std::string strNewContent;  
		rapidxml::print(std::back_inserter(strNewContent),XmlDoc,/*rapidxml::print_no_indenting*/0);
		free(pData);
		return strNewContent;
	}
	else
	{
		free(pData);
		return strInXmlStream;
	}
}
bool UpdateDownloadBot(CResLibData * pProcessData,const char * pszSrcBot,const char * pszOutBotFile)
{
	if (pProcessData==NULL || pszSrcBot==NULL || pszOutBotFile==NULL)
		return false;

	CString strEnd = pszOutBotFile;
	strEnd = strEnd.Mid(strEnd.GetLength()- 4,4);
	if (strEnd.CompareNoCase("botw")==0)
	  return UpdateDownloadBotW(pProcessData,pszSrcBot,pszOutBotFile);

	// ��bot�ļ���ȡManifest.xml�ļ�����
	string strContent = ZipTool::GetContentFromZip(pszSrcBot,"Manifest.xml");
	BOOL bChanged = FALSE;
	string strNewContent = UpdateBotMainfestXml(strContent,pProcessData,bChanged);
	if (bChanged)
		return ZipTool::ReplaceZipFile(pszSrcBot,pszOutBotFile,"Manifest.xml",strNewContent,8);
	else
	{
		// ������Ϣδ�����仯ʱ��ֱ�ӿ�����Ŀ�ĵ�
		if (CFileTool::CopyFile((char*)pszOutBotFile,(char*)pszSrcBot))
			return true;

		return false;
	}

}
bool UpdateDownloadBotW(CResLibData * pProcessData,const char * pszSrcBotW,const char * pszOutBotFile)
{
	// ��botw�ļ���ȡmain.xml�ļ�����
	string strContent = ZipTool::GetContentFromZip(pszSrcBotW,"main.xml");
	xml_document<> XmlDoc;
	XmlDoc.parse<parse_full>((char*)strContent.c_str());

	xml_node<>* pNodeMainXml = XmlDoc.first_node("mainxml");
	if (pNodeMainXml==NULL)
	{
		ZTools::WriteZToolsFormatLog("main.xml��δ�ҵ�mainxml�ڵ�");
		return false;
	}
	xml_node<>* pNodeBotw = pNodeMainXml->first_node("botw");
	if (pNodeBotw==NULL)
	{
		ZTools::WriteZToolsFormatLog("mainxml�ڵ���δ�ҵ���botw�ڵ�");
		return false;
	}
	xml_node<>* pNodeCompent = pNodeMainXml->first_node("component");
	if (pNodeCompent==NULL)
	{
		ZTools::WriteZToolsFormatLog("mainxml�ڵ���δ�ҵ���component�ڵ�");
		return false;
	}
	rapidxml::xml_attribute<char> * attrPath = pNodeCompent->first_attribute("Path");
	if (attrPath==NULL)
	{
		ZTools::WriteZToolsFormatLog("component�ڵ���δ�ҵ���Path����ֵ");
		return false;
	}
	string strBotFileName = UTF8_TO_MB(attrPath->value());
	if (strBotFileName.empty())
	{
		ZTools::WriteZToolsFormatLog("component�ڵ��Path����ֵΪ��");
		return false;
	}

	BOOL bChanged = FALSE;
	UpdateNodeAttribtute(XmlDoc,pNodeBotw,"Author",pProcessData->m_sAuthor.c_str(),bChanged);
	UpdateNodeAttribtute(XmlDoc,pNodeBotw,"Created",pProcessData->m_sCreatTime.c_str(),bChanged);
	UpdateNodeAttribtute(XmlDoc,pNodeBotw,"Description",pProcessData->m_sDsec.c_str(),bChanged);
	UpdateNodeAttribtute(XmlDoc,pNodeBotw,"Name",pProcessData->m_sName.c_str(),bChanged);
	UpdateNodeAttribtute(XmlDoc,pNodeBotw,"Version",pProcessData->m_sVersions.c_str(),bChanged);

	string strNewBotFilePath = pProcessData->m_sName +".bot";
	UpdateNodeAttribtute(XmlDoc,pNodeCompent,"Path",strNewBotFilePath.c_str(),bChanged);

	std::string strNewContent;
	if (bChanged)
	{
		// ���main.xml���º������
		rapidxml::print(std::back_inserter(strNewContent),XmlDoc,/*rapidxml::print_no_indenting*/0);
		// �õ�botw�ļ��е�bot������ֽ���
		int iOutLenZip = 0;
		char * pszCompentBotStream = ZipTool::GetContentFromZip(pszSrcBotW,strBotFileName,iOutLenZip);
		int iOutLenMainfest = 0;
		char * pszOldMainfestXml = ZipTool::GetContentFormMemZip(pszCompentBotStream,iOutLenZip,"Manifest.xml",iOutLenMainfest);
	
		string strNewMainfestXml = UpdateBotMainfestXml(pszOldMainfestXml,pProcessData,bChanged);
		delete pszOldMainfestXml;
		
		char * pszNewBotZip = NULL;
		int iNewBotZipLen = 0;

		ZipTool::UpdateMemoryZipFile(pszCompentBotStream,iOutLenZip,pszNewBotZip,iNewBotZipLen,
			"Manifest.xml",strNewMainfestXml.c_str(),strNewMainfestXml.length());

		delete pszCompentBotStream;

		// ���ֽ�����ȡzip,�������µ�zip�ļ�
		ZTools::WriteZToolsFormatLog("��ʼ�滻botw�е��ļ�����,����botw��%s�������botw��%s��",pszSrcBotW,pszOutBotFile);
		vector<UpdateFileObj> vUpFileArry;
		UpdateFileObj ob1;
		ob1.m_strFileName = "main.xml";
		ob1.m_strNewName = "main.xml";
		ob1.m_pszFileContent = (char*)strNewContent.c_str();
		ob1.m_iFileLen = strNewContent.length();
		vUpFileArry.push_back(ob1);

		UpdateFileObj ob2;
		ob2.m_strFileName = strBotFileName; // botw�е�bot�ļ���
		ob2.m_strNewName = strNewBotFilePath; // botw�е��ļ�ʹ���µ�����
		ob2.m_pszFileContent = pszNewBotZip;
		ob2.m_iFileLen = iNewBotZipLen ;
		vUpFileArry.push_back(ob2);

		bool bok = ZipTool::ReplaceZipFile(pszSrcBotW,pszOutBotFile,vUpFileArry,8);
		delete pszNewBotZip;
		return bok;
	}
	else
	{
		// ������Ϣδ�����仯ʱ��ֱ�ӿ�����Ŀ�ĵ�
		if (CFileTool::CopyFile((char*)pszOutBotFile,(char*)pszSrcBotW))
			return true;

		return false;
	}
	

	return true;
}