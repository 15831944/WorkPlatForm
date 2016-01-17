#pragma once
#include <string>
#include <map>

class CParameterItem;
class CComponentInfo;
class CResLibRobotData;
class CResLibProcessData;

using namespace std;
class CXmlResolution
{
public:
	CXmlResolution(void);
	virtual ~CXmlResolution(void);

public:
	static bool GetManifestParametersLists(const char* pXmlData,int DataLen,map<string,CParameterItem*>& ParameterItemsMap);
	static bool GetManifestPropertiesInfo(const char* pXmlData,int DataLen,CResLibRobotData* RobotBase);
	static bool GetManifestPropertiesInfo(const char* pXmlData,int DataLen,CResLibProcessData* ProcessBase);
	
};

