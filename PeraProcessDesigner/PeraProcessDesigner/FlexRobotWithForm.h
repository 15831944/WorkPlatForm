#pragma once
#include <string>
#include <vector>
#include <map>
#include "ParameterItem.h"
#include "FlexRobotData.h"
using namespace std;

class CFlexRobotWithForm : public CFlexRobotData
{
public:
	CFlexRobotWithForm(void);
	virtual ~CFlexRobotWithForm(void);

public:
	BOOL ZipToBotw(string strBotwFullPath, string strCompFullPath, string strFormFullPath, string strName,
		string strVersion, string strAuth, string strTime, string strDesc);

	virtual bool GetRobotCaption(void);

public:

	string m_strVersion;
	string m_strAuth;
	string m_strTime;
	string m_strDesc;
	string m_strCompName;
	string m_strCompType;

	string m_strCompPath;
	string m_strCompInstanceId;

	string m_strFormPath;
	string m_strFormInstanceId;
};

