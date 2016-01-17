#pragma once
#include <string>

#include "ResLibData.h"
#include <list>

class CParameterItem;
using namespace std;

class CResLibRobotData :
	public CResLibData
{
public:
	CResLibRobotData(void);
	virtual ~CResLibRobotData(void);


	string m_RobotParametersJson;
	string m_RobotOutnVar;
	string m_RobotFilePath;
	string m_RobotInPath;
	string m_RobotOutPath;
	string m_RobotType;
	

	bool SetPK_ID(string sPK_ID);
	bool SetPK_PID(string  sPK_PID);					
	bool SetComponentName(string sName);
	bool SetComponentParametJson(string sParametJson);
	bool SetComponentOutVar(string sOutVar);
	bool SetComponentType(string sType);
	bool SetComponentFilePath(string sFilePath);
	bool SetComponentInPath(string sInPath);
	bool SetComponentOutPath(string  sOutPath);
	bool SetComponentDesc(string sDesc);
	bool SetComponentVersion(string sVersion);
	bool SetComponentAuthor(string sAuthor);
	bool SetComponentOpt	(string sOpt);
	bool SetRobotType	(string sTag);
	bool SetComponentCreatTime(string sCreatTime);
	bool SetComponentPublishTime(string sPublishTime);
	bool SetComponentType_SRC(string sTypeSrc);
	bool SetInstanceID(string sInstanceID);
	virtual string GetSvgJsonStr();
	virtual bool PublishFile();
	virtual bool DelPublishFile();
	bool DownloadFile(string stOutFilePath);
};

