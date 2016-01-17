#include "StdAfx.h"
#include "FlexRobotWithForm.h"
#include "ZipTool.h"
#include <Tools.h>
#include <tinyxml.h>
#include <dbghelp.h>


CFlexRobotWithForm::CFlexRobotWithForm(void)
{
}

CFlexRobotWithForm::~CFlexRobotWithForm(void)
{
}

static string GetStringAttributeFromXmlElement(TiXmlElement* xmlElement, string strKey)
{
	string strValue;
#ifdef TIXML_USE_STL
	xmlElement->QueryStringAttribute(strKey.c_str(), &strValue);
#else
	const char* pStrValue = xmlElement->Attribute(strKey.c_str());
	if (pStrValue != NULL)
	{
		strValue = std::string(pStrValue);
	}
#endif
	return strValue;
}

BOOL CFlexRobotWithForm::ZipToBotw(string strBotwFullPath, string strCompFullPath, string strFormFullPath
	, string strName, string strVersion, string strAuth, string strTime, string strDesc)
{
#pragma region 读组件的instanceid和参数
	string strCompManifest = ZipTool::GetContentFromZip(strCompFullPath, "manifest.xml");

	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.Parse(strCompManifest.c_str());

		if (doc.Error())
		{
			ZTools::WriteZToolsFormatLog("解析manifest出错");
			return FALSE;
		}
		TiXmlElement* rootElement = doc.RootElement();//Manifest
		if (rootElement)
		{
			TiXmlElement* pComponentElement = rootElement->FirstChildElement("Component");
			if (pComponentElement)
			{
				//读取组件类型
				m_strCompType = GetStringAttributeFromXmlElement(pComponentElement, "RobotType");
				ZTools::UTF8ToMB(m_strCompType);
				m_strCompName = GetStringAttributeFromXmlElement(pComponentElement, "RobotName");
				ZTools::UTF8ToMB(m_strCompName);
				//读id作为instanceid
				TiXmlElement* pPropertiesElement = pComponentElement->FirstChildElement("Properties");
				if (pPropertiesElement)
				{
					TiXmlElement* pPropertyElement = pPropertiesElement->FirstChildElement("Property");
					while (pPropertyElement)
					{
						string strName = GetStringAttributeFromXmlElement(pPropertyElement, "name");

						if (strName == "ID")
						{
							const char* sID = pPropertyElement->GetText();
							if (sID)
							{
								//取组件的instanceid
								m_strCompInstanceId = sID;
								ZTools::UTF8ToMB(m_strCompInstanceId);
							}
						}
						else if (strName == "Version")
						{
							const char* sValue = pPropertyElement->GetText();
							if (sValue)
							{
								m_strVersion = sValue;
								ZTools::UTF8ToMB(m_strVersion);
							}
						}
						else if (strName == "CreateTime")
						{
							const char* sValue = pPropertyElement->GetText();
							if (sValue)
							{
								m_strTime = sValue;
								ZTools::UTF8ToMB(m_strTime);
							}
						}
						else if (strName == "Author")
						{
							const char* sValue = pPropertyElement->GetText();
							if (sValue)
							{
								m_strAuth = sValue;
								ZTools::UTF8ToMB(m_strAuth);
							}
						}
						else if (strName == "Description")
						{
							const char* sValue = pPropertyElement->GetText();
							if (sValue)
							{
								m_strDesc = sValue;
								ZTools::UTF8ToMB(m_strDesc);
							}
						}

						pPropertyElement = pPropertyElement->NextSiblingElement("Property");
					}
				}

				//取组件的参数
				if (TRUE)
				{
					TiXmlElement* pParametersElement = pComponentElement->FirstChildElement("Parameters");
					if (pParametersElement)
					{
						TiXmlElement* pParameterItemElement = pParametersElement->FirstChildElement("ParameterItem");
						while (pParameterItemElement)
						{
							CParameterItem* NewParametTmp = new CParameterItem();
							TiXmlAttribute* pAb;
							std::string strValue;
							std::string strName;
							pAb = pParameterItemElement->FirstAttribute();

							while (pAb)
							{
								strName = pAb->Name();
								strValue = pAb->Value();
								ZTools::UTF8ToMB(strName);
								ZTools::UTF8ToMB(strValue);
								NewParametTmp->m_IdValueMap.insert(make_pair(strName,strValue));
								pAb = pAb->Next();
							}

							m_ParameterItemsMap[NewParametTmp->m_IdValueMap["id"]] = NewParametTmp;

							pParameterItemElement = pParameterItemElement->NextSiblingElement("ParameterItem");
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region 读表单的instanceid
	string strFormManifest = ZipTool::GetContentFromZip(strFormFullPath, "manifest.xml");

	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.Parse(strFormManifest.c_str());

		if (doc.Error())
		{
			ZTools::WriteZToolsFormatLog("解析manifest出错");
			return FALSE;
		}
		TiXmlElement* rootElement = doc.RootElement();//Manifest
		if (rootElement)
		{
			TiXmlElement* pComponentElement = rootElement->FirstChildElement("Component");
			if (pComponentElement)
			{
				//读id作为instanceid
				TiXmlElement* pPropertiesElement = pComponentElement->FirstChildElement("Properties");
				if (pPropertiesElement)
				{
					TiXmlElement* pPropertyElement = pPropertiesElement->FirstChildElement("Property");
					while (pPropertyElement)
					{
						string strName = GetStringAttributeFromXmlElement(pPropertyElement, "name");

						if (strName == "ID")
						{
							const char* sID = pPropertyElement->GetText();
							if (sID)
							{
								//取表单的instanceid
								m_strFormInstanceId = sID;
								ZTools::UTF8ToMB(m_strFormInstanceId);
							}
							break;
						}

						pPropertyElement = pPropertyElement->NextSiblingElement("Property");
					}
				}
			}
		}
	}
#pragma endregion

#pragma region 生成botw的描述文件main.xml
	string strMainxmlPath;
	string strCompFilename;
	string strFormFilename;
	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;

		TiXmlDeclaration * pDeclEle = new TiXmlDeclaration("1.0", "UTF-8", "");  
		doc.LinkEndChild(pDeclEle); 

		TiXmlElement* pRootEle = new TiXmlElement("mainxml");
		doc.LinkEndChild(pRootEle);

		TiXmlElement* pBotwEle = new TiXmlElement("botw");

		//修改为以BOT内部属性来初始化BOTW内部描述中的属性，不以传入参数为准，20141105，lc
// 		pBotwEle->SetAttribute("Version", ZTools::MBToUTF8(string(strVersion)).c_str());
// 		pBotwEle->SetAttribute("Author", ZTools::MBToUTF8(string(strAuth)).c_str());
// 		pBotwEle->SetAttribute("Created", ZTools::MBToUTF8(string(strTime)).c_str());
// 		pBotwEle->SetAttribute("Description", ZTools::MBToUTF8(string(strDesc)).c_str());
// 		pBotwEle->SetAttribute("Name", ZTools::MBToUTF8(string(strName)).c_str());

		pBotwEle->SetAttribute("Version", ZTools::MBToUTF8(string(m_strVersion)).c_str());
		pBotwEle->SetAttribute("Author", ZTools::MBToUTF8(string(m_strAuth)).c_str());
		pBotwEle->SetAttribute("Created", ZTools::MBToUTF8(string(m_strTime)).c_str());
		pBotwEle->SetAttribute("Description", ZTools::MBToUTF8(string(m_strDesc)).c_str());
		pBotwEle->SetAttribute("Name", ZTools::MBToUTF8(string(m_strCompName)).c_str());

		pBotwEle->SetAttribute("Type", ZTools::MBToUTF8(string(m_strCompType)).c_str());
		pRootEle->LinkEndChild(pBotwEle);

		TiXmlElement* pComponentEle = new TiXmlElement("component");
		pComponentEle->SetAttribute("InstanceID", ZTools::MBToUTF8(string(m_strCompInstanceId)).c_str());
		strCompFilename = strCompFullPath;
		strCompFilename = strCompFilename.substr(strCompFilename.find_last_of("/\\") + 1, strCompFilename.length() - strCompFilename.find_last_of("/\\") - 1);
		pComponentEle->SetAttribute("Path", ZTools::MBToUTF8(string(strCompFilename)).c_str());
		pRootEle->LinkEndChild(pComponentEle);

		TiXmlElement* pFormEle = new TiXmlElement("form");
		pFormEle->SetAttribute("InstanceID", ZTools::MBToUTF8(string(m_strFormInstanceId)).c_str());
		strFormFilename = strFormFullPath;
		strFormFilename = strFormFilename.substr(strFormFilename.find_last_of("/\\") + 1, strFormFilename.length() - strFormFilename.find_last_of("/\\") - 1);
		pFormEle->SetAttribute("Path", ZTools::MBToUTF8(string(strFormFilename)).c_str());
		pRootEle->LinkEndChild(pFormEle);

		TiXmlElement* pParamsEle = new TiXmlElement("Parameters");
		pRootEle->LinkEndChild(pParamsEle);

		TiXmlElement* pParamEle = NULL;
		for (map<string,CParameterItem*>::iterator it = m_ParameterItemsMap.begin();it != m_ParameterItemsMap.end();it++)
		{
			pParamEle = new TiXmlElement("ParameterItem");
			for (map<string,string>::iterator itValue = (it->second)->m_IdValueMap.begin();itValue != (it->second)->m_IdValueMap.end();itValue++)
			{
				pParamEle->SetAttribute(ZTools::MBToUTF8(string(itValue->first)).c_str(), ZTools::MBToUTF8(string(itValue->second)).c_str());
			}
			pParamsEle->LinkEndChild(pParamEle);
		}

		strMainxmlPath = strBotwFullPath;
		strMainxmlPath = strMainxmlPath.substr(0, strMainxmlPath.find_last_of("/\\"));
		strMainxmlPath += "\\main.xml";
		MakeSureDirectoryPathExists(strMainxmlPath.c_str());
		doc.SaveFile(strMainxmlPath.c_str());
	}
#pragma endregion

#pragma region 打包botw文件
	Json::Value jsonFileArray(Json::arrayValue);

	Json::Value jsonMainXmlFile;
	jsonMainXmlFile["src"] = Json::Value(strMainxmlPath);
	jsonMainXmlFile["dst"] = Json::Value("main.xml");
	jsonFileArray.append(jsonMainXmlFile);

	Json::Value jsonComponentFile;
	jsonComponentFile["src"] = Json::Value(strCompFullPath);
	jsonComponentFile["dst"] = Json::Value(strCompFilename);
	jsonFileArray.append(jsonComponentFile);

	Json::Value jsonFormFile;
	jsonFormFile["src"] = Json::Value(strFormFullPath);
	jsonFormFile["dst"] = Json::Value(strFormFilename);
	jsonFileArray.append(jsonFormFile);

	if (!ZipTool::ZipFromJson(strBotwFullPath, jsonFileArray, ZipTool::STORE))
	{
		return FALSE;
	}
#pragma endregion

	return TRUE;
}

bool CFlexRobotWithForm::GetRobotCaption(void)
{
	string strMainXml = ZipTool::GetContentFromZip((LPCTSTR)m_strLocalPath, "main.xml");

	{
		TiXmlBase::SetCondenseWhiteSpace(false);
		TiXmlDocument doc;
		doc.Parse(strMainXml.c_str());

		if (doc.Error())
		{
			ZTools::WriteZToolsFormatLog("解析mainxml出错");
			return false;
		}
		TiXmlElement* pRootEle = doc.RootElement();//mainxml
		if (pRootEle)
		{
			//取BOTW信息
			TiXmlElement* pBotwEle = pRootEle->FirstChildElement("botw");
			if (pBotwEle)
			{
				m_strVersion = GetStringAttributeFromXmlElement(pBotwEle, "Version");
				ZTools::UTF8ToMB(m_strVersion);
				m_strAuth = GetStringAttributeFromXmlElement(pBotwEle, "Author");
				ZTools::UTF8ToMB(m_strAuth);
				m_strTime = GetStringAttributeFromXmlElement(pBotwEle, "Created");
				ZTools::UTF8ToMB(m_strTime);
				m_strDesc = GetStringAttributeFromXmlElement(pBotwEle, "Description");
				ZTools::UTF8ToMB(m_strDesc);
				m_strCompName = GetStringAttributeFromXmlElement(pBotwEle, "Name");
				ZTools::UTF8ToMB(m_strCompName);
				m_strCompType = GetStringAttributeFromXmlElement(pBotwEle, "Type");
				ZTools::UTF8ToMB(m_strCompType);
			}

			//取组件信息
			TiXmlElement* pComponentEle = pRootEle->FirstChildElement("component");
			if (pComponentEle)
			{
				m_strCompInstanceId = GetStringAttributeFromXmlElement(pComponentEle, "InstanceID");
				ZTools::UTF8ToMB(m_strCompInstanceId);
				m_strCompPath = GetStringAttributeFromXmlElement(pComponentEle, "Path");
				ZTools::UTF8ToMB(m_strCompPath);
			}

			//取表单信息
			TiXmlElement* pFormEle = pRootEle->FirstChildElement("form");
			if (pFormEle)
			{
				m_strFormInstanceId = GetStringAttributeFromXmlElement(pFormEle, "InstanceID");
				ZTools::UTF8ToMB(m_strFormInstanceId);
				m_strFormPath = GetStringAttributeFromXmlElement(pFormEle, "Path");
				ZTools::UTF8ToMB(m_strFormPath);
			}

			//取参数信息
			TiXmlElement* pParamsEle = pRootEle->FirstChildElement("Parameters");
			if (pParamsEle)
			{
				TiXmlElement* pParamItemEle = pParamsEle->FirstChildElement("ParameterItem");
				while (pParamItemEle)
				{
					CParameterItem* NewParametTmp = new CParameterItem();
					TiXmlAttribute* pAb;
					std::string strValue;
					std::string strName;
					pAb = pParamItemEle->FirstAttribute();

					while (pAb)
					{
						strName = pAb->Name();
						strValue = pAb->Value();
						ZTools::UTF8ToMB(strName);
						ZTools::UTF8ToMB(strValue);
						NewParametTmp->m_IdValueMap.insert(make_pair(strName,strValue));
						pAb = pAb->Next();
					}

					m_ParameterItemsMap[NewParametTmp->m_IdValueMap["id"]] = NewParametTmp;

					pParamItemEle = pParamItemEle->NextSiblingElement("ParameterItem");
				}
			}
		}
	}
	return true;
}
