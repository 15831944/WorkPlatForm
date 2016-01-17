#include "StdAfx.h"
#include "XmlResolution.h"
#include "tinyxml.h"
#include "ParameterItem.h"
//#include "ComponentInfo.h"
#include "ResLibRobotData.h"
#include "ResLibProcessData.h"
#include "Tools.h"

CXmlResolution::CXmlResolution(void)
{
	//TiXmlDocument* doc =new TiXmlDocument;
}


CXmlResolution::~CXmlResolution(void)
{
}
bool CXmlResolution::GetManifestParametersLists(const char* pXmlData,int DataLen,map<string,CParameterItem*>& ParameterItemsMap)
{
	TiXmlDocument doc;
	const char* RetParse = doc.Parse(pXmlData,0,TIXML_ENCODING_UTF8);
	
	TiXmlElement* XmlManifest = doc.FirstChildElement("Manifest");
	if (XmlManifest == NULL)
	{
		return false;
	}
	TiXmlElement* XmlComponent = XmlManifest->FirstChildElement("Component");
	if (XmlComponent == NULL)
	{
		return false;
	}
	TiXmlElement* pPropertiesElement = XmlComponent->FirstChildElement("Parameters");
	if (pPropertiesElement == NULL)
	{
		return false;
	}
	TiXmlElement* pParameterItemElement = pPropertiesElement->FirstChildElement("ParameterItem");
	while (pParameterItemElement)
	{
		CParameterItem* NewParametTmp = new CParameterItem();
		TiXmlAttribute* pAb;
		std::string strValue("");
		std::string strName;
		pAb = pParameterItemElement->FirstAttribute();

		while (pAb)
		{
			
			strName = pAb->NameTStr().c_str();
			strValue = pAb->Value();
			strName = ZTools::UTF8ToMB(strName);
			strValue = ZTools::UTF8ToMB(strValue);
			NewParametTmp->m_IdValueMap.insert(make_pair(strName,strValue));
			pAb = pAb->Next();
		}
		string sIdVaule = "";
		for (map<string,string>::iterator it = NewParametTmp->m_IdValueMap.begin();it != NewParametTmp->m_IdValueMap.end();it++)
		{
			if ((it->first) == "id")
			{
				ParameterItemsMap.insert(make_pair(it->second,NewParametTmp));
			}
		}
		
		pParameterItemElement = pParameterItemElement->NextSiblingElement("ParameterItem");
	}
	
	
	doc.Clear();
// 	if (doc != NULL)
// 	{
// 		delete doc;
// 		doc = NULL;
// 	}

	return true;
}

bool CXmlResolution::GetManifestPropertiesInfo(const char* pXmlData,int DataLen,CResLibProcessData* ProcessBase)
{
	TiXmlDocument doc;
	const char* RetParse = doc.Parse(pXmlData,0,TIXML_ENCODING_UTF8);
	/////////////////////////////////Package/////////////////////////////////////////
	TiXmlElement* XmlPackage = doc.FirstChildElement("Package");
	if (XmlPackage == NULL)
		return false;
	
	TiXmlElement* XmlWorkflowProcesses = XmlPackage->FirstChildElement("WorkflowProcesses");
	if (XmlWorkflowProcesses == NULL)
		return false;

	TiXmlElement* XmlWorkflowProcess = XmlWorkflowProcesses->FirstChildElement("WorkflowProcess");
	if (XmlWorkflowProcess == NULL)
		return false;

	const char* pStr =  XmlWorkflowProcess->Attribute("Id");
	if (pStr != NULL)
	{
		ProcessBase->m_InstanceID = pStr;
		ProcessBase->m_InstanceID = ZTools::UTF8ToMB(ProcessBase->m_InstanceID);
	}
	pStr = NULL;
	pStr =  XmlPackage->Attribute("Name");
	if (pStr != NULL)
	{
		ProcessBase->m_sName = pStr;
		ProcessBase->m_sName = ZTools::UTF8ToMB(ProcessBase->m_sName);
	}
	//////////////////////////////////////////////////////////////////////////
	TiXmlElement* XmlPackageHeader = XmlWorkflowProcess->FirstChildElement("ProcessHeader");
	
	if (XmlPackageHeader == NULL)
	{
		return false;
	}

	TiXmlElement* XmlXPDLVersion= XmlPackageHeader->FirstChildElement("Version"); //Version
	if (XmlXPDLVersion == NULL)
		return false;

	TiXmlNode* NodeVersion =	XmlXPDLVersion->FirstChild();
	if (NodeVersion == NULL)
		return false;
	const char* sVersions = NodeVersion->Value();
	if (sVersions != NULL)
	{
		ProcessBase->m_sVersions = sVersions;
		ProcessBase->m_sVersions = ZTools::UTF8ToMB(ProcessBase->m_sVersions);
	}

	TiXmlElement* XmlAuthor= XmlPackageHeader->FirstChildElement("Author"); //Author
	if (XmlAuthor == NULL)
		return false;
	TiXmlNode* NodeVendor =	XmlAuthor->FirstChild();
	if (NodeVendor == NULL)
		return false;
	const char* sVendor = NodeVendor->Value();
	if (sVendor != NULL)
	{
		ProcessBase->m_sAuthor = sVendor;
		ProcessBase->m_sAuthor = ZTools::UTF8ToMB(ProcessBase->m_sAuthor);
	}
	

	TiXmlElement* XmlDescription= XmlPackageHeader->FirstChildElement("Description"); //Description
	if (XmlDescription == NULL)
		return false;

	TiXmlNode* NodeDescription =	XmlDescription->FirstChild();
	if (NodeDescription == NULL)
		return false;
	const char* sDescription = NodeDescription->Value();
	if (sDescription != NULL)
	{
		ProcessBase->m_sDsec = sDescription;
		ProcessBase->m_sDsec = ZTools::UTF8ToMB(ProcessBase->m_sDsec);
	}

	TiXmlElement* XmlCreated= XmlPackageHeader->FirstChildElement("Created"); //Created
	if (XmlCreated == NULL)
		return false;

	TiXmlNode* NodeCreated =	XmlCreated->FirstChild();
	if (NodeCreated == NULL)
		return false;
	const char* sCreatedTime = NodeCreated->Value();
	if (sCreatedTime != NULL)
	{
		ProcessBase->m_sCreatTime = sCreatedTime;
		ProcessBase->m_sCreatTime = ZTools::UTF8ToMB(ProcessBase->m_sCreatTime);
	}
	return true;
}

bool CXmlResolution::GetManifestPropertiesInfo(const char* pXmlData,int DataLen,CResLibRobotData* RobotBase)
{
	TiXmlDocument doc;
	const char* RetParse = doc.Parse(pXmlData,0,TIXML_ENCODING_UTF8);

	TiXmlElement* XmlManifest = doc.FirstChildElement("Manifest");
	if (XmlManifest == NULL)
	{
		return false;
	}

	TiXmlElement* XmlComponent = XmlManifest->FirstChildElement("Component");
	if (XmlComponent == NULL)
	{
		return false;
	}
	
	const char* pStr =  XmlComponent->Attribute("RobotType");		
	if (pStr != NULL)
	{
		RobotBase->m_RobotType = pStr;
		RobotBase->m_RobotType = ZTools::UTF8ToMB(RobotBase->m_RobotType);
	}
	pStr = NULL;
	pStr =  XmlComponent->Attribute("RobotName");		
	if (pStr != NULL)
	{
		RobotBase->m_sName = pStr;
		RobotBase->m_sName = ZTools::UTF8ToMB(RobotBase->m_sName);
	}
	


	TiXmlElement* XmlProperties = XmlComponent->FirstChildElement("Properties");
	if (XmlProperties == NULL)
	{
		return false;
	}

	TiXmlElement* Property= XmlProperties->FirstChildElement(); //BaseLib

	while(Property){  

		TiXmlAttribute *att=Property->FirstAttribute();  
		
		
		while(att){//遍历循环属性  
			const char* stIDTmp = att->Value();

			if (strcmp(stIDTmp,"ID") == 0)
			{
				TiXmlNode* Value= Property->FirstChild();
				if (Value != NULL)
				{
					RobotBase->m_InstanceID = Value->Value();
					RobotBase->m_InstanceID = ZTools::UTF8ToMB(RobotBase->m_InstanceID);
				}
			}
			if (strcmp(stIDTmp,"Version") == 0)
			{
				TiXmlNode* Value= Property->FirstChild();
				if (Value != NULL)
				{
					RobotBase->m_sVersions = Value->Value();
					RobotBase->m_sVersions = ZTools::UTF8ToMB(RobotBase->m_sVersions);
				}
			}
			
			if (strcmp(stIDTmp,"CreateTime") == 0)
			{
				TiXmlNode* Value= Property->FirstChild();
				if (Value != NULL)
				{
					RobotBase->m_sCreatTime = Value->Value();
					RobotBase->m_sCreatTime = ZTools::UTF8ToMB(RobotBase->m_sCreatTime);
				}
			}

			if (strcmp(stIDTmp,"ModifyTime") == 0)
			{
				TiXmlNode* Value= Property->FirstChild();
				if (Value != NULL)
				{
					RobotBase->m_sPublishTime = Value->Value();
					RobotBase->m_sPublishTime = ZTools::UTF8ToMB(RobotBase->m_sPublishTime);
				}
			}
				
			if (strcmp(stIDTmp,"Author") == 0)
			{
				TiXmlNode* Value= Property->FirstChild();
				if (Value != NULL)
				{
					RobotBase->m_sAuthor = Value->Value();
					RobotBase->m_sAuthor = ZTools::UTF8ToMB(RobotBase->m_sAuthor);
				}
			}

			if (strcmp(stIDTmp,"Description") == 0)
			{
				TiXmlNode* Value= Property->FirstChild();
				if (Value != NULL)
				{
					RobotBase->m_sDsec = Value->Value();
					RobotBase->m_sDsec = ZTools::UTF8ToMB(RobotBase->m_sDsec);
				}
			}
			att=att->Next();  
			
		}  
		Property=Property->NextSiblingElement();  
		//cout<<"指针： "<<propertyNN<<endl; //当指针propertyNN遍历完整个节点，地址就为0000000，跳出while(propertyNN)循环  
	}  

	return true;
}