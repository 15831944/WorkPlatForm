#include "StdAfx.h"
#include "ResLibRobotData.h"
#include "SqlCommand.h"
#include "FileTool.h"

#include "ParameterItem.h"
#include "reader.h"
#include "writer.h"
#include <string>
#include "EncodedTool.h"
#include "Tools.h"
#include "FlexRobotWithForm.h"
#include "GetGlobalDataFromReg.h"
#include "UpdateDownloadBotWsFile.h"

CResLibRobotData::CResLibRobotData(void)
{
	m_nResType = CResLibData::RT_ROBOT;
}


CResLibRobotData::~CResLibRobotData(void)
{
}

bool CResLibRobotData::SetPK_ID(string sPK_ID)
{
	if (m_sPkID != sPK_ID)
	{
		m_sPkID = sPK_ID;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"PK_ID",sPK_ID));		
		return true;
	}
	return false;
}
bool CResLibRobotData::SetPK_PID(string  sPK_PID)
{
	if (m_sFkpID != sPK_PID)
	{
		m_sFkpID = sPK_PID;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"PK_PID",sPK_PID));		
		return true;
	}
	return false;
}				
bool CResLibRobotData::SetComponentName(string sComonentName)
{
	if (m_sName != sComonentName)
	{
		m_sName = sComonentName;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_NAME",sComonentName));		
		return true;
	}
	return false;
}
bool CResLibRobotData::SetComponentParametJson(string sParametJson)
{
	if (sParametJson == "")
	{
		m_RobotParametersJson = sParametJson;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_PARAMET_JSON","[]"));	
		return true;
	}
	if (m_RobotParametersJson != sParametJson)
	{
		m_RobotParametersJson = sParametJson;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_PARAMET_JSON",sParametJson));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentOutVar(string stComoinentOutVar)
{
	if (m_RobotOutnVar != stComoinentOutVar)
	{
		m_RobotOutnVar = stComoinentOutVar;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_OUT_VAR",stComoinentOutVar));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentType(string stComponentType)
{
	if (m_sEntType != stComponentType)
	{
		m_sEntType = stComponentType;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_TYPE",stComponentType));		
		return true;
	}
	return false;
}
bool CResLibRobotData::SetComponentFilePath(string sFilePath)
{
	if (m_RobotFilePath != sFilePath)
	{
		m_RobotFilePath = sFilePath;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_FILE_PATH",sFilePath));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentInPath(string sInPath)
{
	if (m_RobotInPath != sInPath)
	{
		m_RobotInPath = sInPath;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_IN_PATH",sInPath));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentOutPath(string  sOutPath)
{
	if (m_RobotOutPath != sOutPath)
	{
		m_RobotOutPath = sOutPath;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_OUT_PATH",sOutPath));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentDesc(string sDesc)
{
	if (m_sDsec != sDesc)
	{
		m_sDsec = sDesc;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_DESC",sDesc));		
		return true;
	}
	return false;
}
bool CResLibRobotData::SetComponentVersion(string sVersion)
{
	if (m_sVersions != sVersion)
	{
		m_sVersions = sVersion;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_VERSION",sVersion));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentAuthor(string sAuthor)
{
	if (m_sAuthor != sAuthor)
	{
		m_sAuthor = sAuthor;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_AUTHOR",sAuthor));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentOpt	(string sOpt)
{
	if (m_sOpt != sOpt)
	{
		m_sOpt = sOpt;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_OPT",sOpt));		
		return true;
	}
	return false;
}
bool CResLibRobotData::SetRobotType	(string sTag)
{
	if (m_RobotType != sTag)
	{
		m_RobotType = sTag;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_TAG",m_RobotType));		
		return true;
	}
	return false;
}
bool CResLibRobotData::SetComponentCreatTime(string sCreatTime)
{
	if (m_sCreatTime != sCreatTime)
	{
		m_sCreatTime = sCreatTime;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_CREAT_TIME",sCreatTime));	
		return true;	
	}
	return false;
}
bool CResLibRobotData::SetComponentPublishTime(string sPublishTime)
{
	if (m_sPublishTime != sPublishTime)
	{
		m_sPublishTime = sPublishTime;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_PUBLISH_TIME",sPublishTime));
		return true;		
	}
	return false;
}
bool CResLibRobotData::SetComponentType_SRC(string sTypeSrc)
{
	if (m_sSrc != sTypeSrc)
	{
		sTypeSrc = sTypeSrc;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"COMPONENT_TYPE_SRC",sTypeSrc));
		return true;		
	}
	return false;
}
bool CResLibRobotData::SetInstanceID(string sInstanceID)
{
	if (m_InstanceID != sInstanceID)
	{
		m_InstanceID = sInstanceID;
		m_SqlList.push_back(CSqlCommand::GetUpdate("componentlib","PK_ID",m_sPkID,"PROCESS_VERSION",sInstanceID));
		return true;
	}
	return false;
}
bool CResLibRobotData::DownloadFile(string stOutFilePath)
{
	char stNewRobotoPath[255];
	string stNewFilePath = m_RobotFilePath;
	memset(stNewRobotoPath,0,255);	

	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	memcpy(stNewRobotoPath,strTmp.GetBuffer(),strTmp.GetLength());

	Myreplace(stNewFilePath,"//","\\");
	strcat(stNewRobotoPath, stNewFilePath.c_str());
	if (!UpdateDownloadBot(this,stNewRobotoPath,stOutFilePath.c_str()))
	{
		MessageBox(NULL,"下载组件失败!","提示",MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}

bool CResLibRobotData::PublishFile()
{
	if (m_RobotFilePath == "")
	{
		return false;
	}
	char stNewRobotoPath[255];
	char sOldRobotoPath[255];
	string filename;
	string sfilePath;
	int pathPosition = 0;
	memset(stNewRobotoPath,0,255);
	memset(sOldRobotoPath,0,255);

	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	memcpy(stNewRobotoPath,strTmp.GetBuffer(),strTmp.GetLength());
	memcpy(sOldRobotoPath,m_RobotFilePath.c_str(),m_RobotFilePath.size());

	sfilePath.append( "//Library//RobotLib//");
	strcat(stNewRobotoPath, "\\Library\\RobotLib\\");
	sfilePath.append( m_sPkID);
	strcat(stNewRobotoPath, m_sPkID.c_str());	
	strcat(stNewRobotoPath, "\\");
	pathPosition=m_RobotFilePath.find_last_of('\\');
	pathPosition++;   //add 1,
	filename.assign(&m_RobotFilePath[pathPosition],m_RobotFilePath.length()-pathPosition);
	sfilePath.append("//");
	sfilePath.append(filename);
	strcat(stNewRobotoPath, filename.c_str());
	if (CFileTool::CopyFile(stNewRobotoPath,sOldRobotoPath))
	{
		m_RobotFilePath = sfilePath;
		return true;
	} 	
	return false;
}
bool CResLibRobotData::DelPublishFile()
{
	if (m_RobotFilePath == "")
	{
		return false;
	}
	char stNewRobotoPath[255];

	memset(stNewRobotoPath,0,255);

	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	strcat(stNewRobotoPath,strTmp.GetBuffer());
	strcat(stNewRobotoPath, "\\Library\\RobotLib\\");
	strcat(stNewRobotoPath,m_sPkID.c_str());
	if (CFileTool::DeleteFile(stNewRobotoPath))
	{
		m_RobotFilePath = "";
		return true;
	} 	
	return false;
}
string CResLibRobotData::GetSvgJsonStr()
{
	Json::Value jsonOut;
	std::string strJsonParams("[]");
	jsonOut["InstanceID"] = Json::Value(m_InstanceID);
	jsonOut["RobotName"] = Json::Value(m_sName);
	jsonOut["RobotType"] = Json::Value(m_RobotType);

	jsonOut["Author"] = Json::Value(m_sAuthor);
	jsonOut["Created"] = Json::Value(m_sCreatTime);
	jsonOut["Version"] = Json::Value(m_sVersions);
	jsonOut["Description"] = Json::Value(m_sDsec);

	string sPathTemp = "";
	
	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	sPathTemp =strTmp.GetBuffer() +  m_RobotFilePath;
	CEncodedTool::StrReplace(sPathTemp,"//","\\");
	jsonOut["RobotPath"] = Json::Value(sPathTemp);	

	Json::Reader reader;
	Json::Value jsonParamet;
	if (reader.parse(m_RobotParametersJson,jsonParamet))
	{
		jsonOut["Parameters"] =jsonParamet;
	}

	if (ZTools::CompareExtName(sPathTemp, ".botw"))
	{
		CFlexRobotWithForm objBotWithForm;
		objBotWithForm.m_strLocalPath = sPathTemp.c_str();
		objBotWithForm.GetRobotCaption();
		jsonOut["componentPathInBotw"] = objBotWithForm.m_strCompPath;
		jsonOut["formPathInBotw"] = objBotWithForm.m_strFormPath;
		jsonOut["formInstanceID"] = objBotWithForm.m_strFormInstanceId;
	}

	Json::FastWriter writer;
	strJsonParams = writer.write(jsonOut);
	return strJsonParams;
}