#include "StdAfx.h"
#include "FlexRobotData.h"
#include "zlib.h"
#include "XmlResolution.h"
#include <map>
#include <string>
#include "ParameterItem.h"
//#include "ComponentInfo.h"
#include "ResLibRobotData.h"
#include "reader.h"
#include "writer.h"
#include "Tools.h"

#include "ZipTool.h"

CFlexRobotData::CFlexRobotData(void)
{

	m_pBase = new CResLibRobotData();
}


CFlexRobotData::~CFlexRobotData(void)
{

	if (m_pBase != NULL)
	{
		delete m_pBase;
		m_pBase = NULL;
	}

	for (map<string,CParameterItem*>::iterator it = m_ParameterItemsMap.begin();it != m_ParameterItemsMap.end();it++)
	{
		if ((it->second) != NULL)
		{
			delete (it->second);
			(it->second) = NULL;
		}
	}
	m_ParameterItemsMap.clear();
}

static LPTSTR LoadFromMem(BYTE* pByte, DWORD dwSize)
{
	if (dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF) 
	{
		pByte += 3; dwSize -= 3;
	}

	DWORD nWide = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0);

	LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1)*sizeof(WCHAR)));
	::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, w_str, nWide );
	w_str[nWide] = L'\0';

	DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);

	LPTSTR pstrXML = static_cast<LPTSTR>(malloc((wide + 1)*sizeof(TCHAR)));
	::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, pstrXML, wide, NULL, NULL);
	pstrXML[wide] = _T('\0');

	free(w_str);

	return pstrXML;
}

bool CFlexRobotData::GetRobotCaption(void)
{
	std::string strXml = ZipTool::GetContentFromZip((LPCTSTR)m_strLocalPath,"Manifest.xml");
	if (strXml.empty())
		return false;

	for (map<string,CParameterItem*>::iterator it = m_ParameterItemsMap.begin();it != m_ParameterItemsMap.end();it++)
	{
		if ((it->second) != NULL)
		{
			delete (it->second);
			(it->second) = NULL;
		}
	}
	m_ParameterItemsMap.clear();
	
	//strXml = strXml.c_str() + 3;
	CXmlResolution::GetManifestParametersLists(strXml.c_str(), strXml.length(), m_ParameterItemsMap);
	CXmlResolution::GetManifestPropertiesInfo(strXml.c_str(), strXml.length(), (CResLibRobotData*)m_pBase);
	
	return true;
}
bool CFlexRobotData::GetProcessCaption(void)
{
	std::string strXml = ZipTool::GetContentFromZip((LPCTSTR)m_strLocalPath,"main.xpdl");
	if (strXml.empty())
		return false;

	CXmlResolution::GetManifestPropertiesInfo(strXml.c_str(),strXml.length(),(CResLibProcessData*)m_pBase);
	return true;
}

//lc,20140415,组件入库时参数名按方向分组后再按中文排序////////////////////////////////////////////////////////////////////////
static std::string ConvertToDirectionName(const Json::Value& j)
{
	std::string strName = j["name"].asString();
	std::string strDirection = j["direction"].asString();
	ZTools::LowerString(strDirection);
	if (strDirection.compare("in") == 0)
	{
		strDirection = "1";
	}
	else if (strDirection.compare("out") == 0)
	{
		strDirection = "2";
	}
	else if (strDirection.compare("inout") == 0)
	{
		strDirection = "3";
	}
	else
	{
		strDirection = "0";
	}

	std::string strReturn = strDirection + strName;
	return strReturn;
}
//lc,20140326,组件入库时参数名按中文排序////////////////////////////////////////////////////////////////////////
static bool localeLessThan (const Json::Value& j1, const Json::Value& j2) 
{ 
	std::string s1 = ConvertToDirectionName(j1);
	std::string s2 = ConvertToDirectionName(j2);
	if (strcoll(s1.c_str(), s2.c_str()) <0)
		return true;
	else
		return false;
} 

static void SortParamJson(Json::Value& jsonParams)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs"); 

	vector<Json::Value> vJson;
	int nCount = jsonParams.size();
	for (int i = 0; i < nCount; i++)
	{
		vJson.push_back(jsonParams[Json::Value::ArrayIndex(i)]);
	}

// 	for (UINT i = 0; i < vJson.size(); ++i)
// 	{
// 		ZTools::WriteZToolsLog(vJson[i]["name"].asString());
// 	}

	std::sort(vJson.begin(), vJson.end(), localeLessThan);

	jsonParams.clear();
	for (UINT i = 0; i < vJson.size(); ++i)
	{
		//ZTools::WriteZToolsLog(vJson[i]["name"].asString());
		jsonParams.append(vJson[i]);
	}

	setlocale(LC_ALL, curLocale.c_str());
}
//////////////////////////////////////////////////////////////////////////
string CFlexRobotData::GetParametJson(void)
{
	std::string strJsonParams("[]");
	Json::Value jsonParams(Json::arrayValue);
	for (map<string,CParameterItem*>::iterator it = m_ParameterItemsMap.begin();it != m_ParameterItemsMap.end();it++)
	{
		Json::Value jsonParameItem;
		for (map<string,string>::iterator itValue = (it->second)->m_IdValueMap.begin();itValue != (it->second)->m_IdValueMap.end();itValue++)
		{
			jsonParameItem[itValue->first] = Json::Value(itValue->second);
		}
		jsonParams.append(jsonParameItem);
	}
	//lc,20140326,组件入库时参数名按中文排序////////////////////////////////////////////////////////////////////////
	SortParamJson(jsonParams);
	//////////////////////////////////////////////////////////////////////////
	Json::FastWriter writer;
	strJsonParams = writer.write(jsonParams);
	return strJsonParams;
}