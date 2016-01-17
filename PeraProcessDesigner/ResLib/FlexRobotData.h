#pragma once
#include <string>

class CParameterItem;
class CComponentInfo;
class CResLibData;

using namespace std;
class CFlexRobotData
{
public:
	CFlexRobotData(void);
	virtual ~CFlexRobotData(void);

	CString m_strLocalPath;
	CString m_strCaption;

public:
	map<string,CParameterItem*> m_ParameterItemsMap;
	CResLibData* m_pBase;
public:
	virtual bool GetRobotCaption(void);
	bool GetProcessCaption(void);
	string  GetParametJson(void);
};

