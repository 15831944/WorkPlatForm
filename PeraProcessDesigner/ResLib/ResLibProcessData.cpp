#include "StdAfx.h"
#include "ResLibProcessData.h"
#include "SqlCommand.h"
#include "FileTool.h"
#include "reader.h"
#include "EncodedTool.h"
#include "writer.h"
#include "GetGlobalDataFromReg.h"
#include "UpdateDownloadBotWsFile.h"



CResLibProcessData::CResLibProcessData(void)
{
	m_nResType = RT_PROCESS;
}


CResLibProcessData::~CResLibProcessData(void)
{
}
bool CResLibProcessData::SetPK_ID(string sID)
{
	if (m_sPkID != sID)
	{
		m_sPkID = sID;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PK_ID",sID));
		return true;			
	}
	return false;
}
bool CResLibProcessData::SetFK_PID(string sPID)
{
	if (m_sFkpID != sPID)
	{
		m_sFkpID = sPID;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"FK_PID",sPID));
		return true;			
	}
	return false;
}
bool CResLibProcessData::SetProcessName(string sName)
{
	if (m_sName != sName)
	{
		m_sName = sName;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_NAME",sName));
		return true;			
	}
	return false;
}
bool CResLibProcessData::SetProcessVersion(string sVersion)
{
	if (m_sVersions != sVersion)
	{
		m_sVersions = sVersion;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_VERSION",sVersion));
		return true;			
	}
	return false;
}
bool CResLibProcessData::SetProcessAuthor(string sAuthor)
{
	if (m_sAuthor != sAuthor)
	{
		m_sAuthor = sAuthor;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_AUTHOR",sAuthor));
		return true;			
	}
	return false;
}
bool CResLibProcessData::SetProcessDesc(string sDesc)
{
	if (m_sDsec != sDesc)
	{
		m_sDsec = sDesc;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_DESC",sDesc));
		return true;		
	}
	return false;
}

bool CResLibProcessData::SetProcessFilePaht(string sFilePath)
{
	if (m_ProcessFilePath != sFilePath)
	{
		m_ProcessFilePath = sFilePath;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_FILE_PATH",sFilePath));
		return true;			
	}
	return false;
}

bool CResLibProcessData::SetProcessOpt(string stOPT)
{
	if (m_sOpt != stOPT)
	{
		m_sOpt = stOPT;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_OPT",stOPT));
		return true;			
	}
	return false;
}
bool CResLibProcessData::SetProcessCreatTime(string stCREAT_TIME)
{
	if (m_sCreatTime != stCREAT_TIME)
	{
		m_sCreatTime = stCREAT_TIME;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_CREAT_TIME",stCREAT_TIME));
		return true;		
	}
	return false;
}
bool CResLibProcessData::SetProcessPublishTime(string stPUBLISH_TIME)
{
	if (m_sPublishTime != stPUBLISH_TIME)
	{
		m_sPublishTime = stPUBLISH_TIME;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_PUBLISH_TIME",stPUBLISH_TIME));
		return true;	
	}
	return false;
}
bool CResLibProcessData::SetProcessEntType(string stTYPE)
{
	if (m_sEntType != stTYPE)
	{
		m_sEntType = stTYPE;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_TYPE",stTYPE));
		return 	true;	
	}
	return false;
}
bool CResLibProcessData::SetProcessSrc(string stPROCESS_SRC)
{
	if (m_sSrc != stPROCESS_SRC)
	{
		m_sSrc = stPROCESS_SRC;
		m_SqlList.push_back(CSqlCommand::GetUpdate("processlib","PK_ID",m_sPkID,"PROCESS_SRC",stPROCESS_SRC));
		return 	true;	
	}
	return false;
}

bool CResLibProcessData::DownloadFile(string stOutFilePath)
{
	char stNewRobotoPath[255];
	string stNewFilePath = m_ProcessFilePath;
	memset(stNewRobotoPath,0,255);	
	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	memcpy(stNewRobotoPath,strTmp.GetBuffer(),strTmp.GetLength());
	
	Myreplace(stNewFilePath,"//","\\");
	strcat(stNewRobotoPath, stNewFilePath.c_str());
	if (!UpdateDownloadProcess(this,stNewRobotoPath,stOutFilePath.c_str()))
	{
		MessageBox(NULL,"下载设计过程失败!","提示",MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}

bool CResLibProcessData::PublishFile()
{
	if (m_ProcessFilePath == "")
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
	// 修改组件发布目录为从注册表读取
	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	memcpy(stNewRobotoPath,strTmp.GetBuffer(),strTmp.GetLength());
	memcpy(sOldRobotoPath,m_ProcessFilePath.c_str(),m_ProcessFilePath.size());
	sfilePath.append( "//Library//ProcessLib//");
	strcat(stNewRobotoPath, "\\Library\\ProcessLib\\");
	sfilePath.append( m_sPkID);
	strcat(stNewRobotoPath, m_sPkID.c_str());	
	strcat(stNewRobotoPath, "\\");
	pathPosition=m_ProcessFilePath.find_last_of('\\');
	pathPosition++;   //add 1,
	filename.assign(&m_ProcessFilePath[pathPosition],m_ProcessFilePath.length()-pathPosition);
	sfilePath.append("//");
	sfilePath.append(filename);
	strcat(stNewRobotoPath, filename.c_str());
	if (CFileTool::CopyFile(stNewRobotoPath,sOldRobotoPath))
	{
		m_ProcessFilePath = sfilePath;
		return true;
	} 	
	return false;
}
bool CResLibProcessData::DelPublishFile()
{
	if (m_ProcessFilePath == "")
	{
		return false;
	}
	char stNewRobotoPath[255];

	memset(stNewRobotoPath,0,255);

	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	strcat(stNewRobotoPath,strTmp.GetBuffer());
	strcat(stNewRobotoPath, "\\Library\\ProcessLib\\");
	strcat(stNewRobotoPath,m_sPkID.c_str());
	if (CFileTool::DeleteFile(stNewRobotoPath))
	{
		m_ProcessFilePath = "";
		return true;
	} 	
	return false;
}

string CResLibProcessData::GetSvgJsonStr()
{
	Json::Value jsonOut;
	std::string strJsonParams("[]");	
	string sPathTemp = "";
	CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
	if(strTmp.IsEmpty())
		strTmp = GetExecDir();

	sPathTemp = strTmp.GetBuffer() +  m_ProcessFilePath;
	CEncodedTool::StrReplace(sPathTemp,"//","\\");
	jsonOut["Path"] = Json::Value(sPathTemp);	
	jsonOut["PkgID"] = Json::Value(m_InstanceID);
	jsonOut["Name"] = Json::Value(m_sName);


	jsonOut["Author"] = Json::Value(m_sAuthor);
	jsonOut["Created"] = Json::Value(m_sCreatTime);
	jsonOut["Version"] = Json::Value(m_sVersions);
	jsonOut["Description"] = Json::Value(m_sDsec);

	Json::FastWriter writer;
	strJsonParams = writer.write(jsonOut);
	return strJsonParams;
}