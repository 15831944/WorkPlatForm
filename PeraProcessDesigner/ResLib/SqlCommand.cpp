#include "StdAfx.h"
#include "SqlCommand.h"
#include "EncodedTool.h"


CSqlCommand::CSqlCommand(void)
{
}


CSqlCommand::~CSqlCommand(void)
{
}
string CSqlCommand::GetProcessSelectID(string stFK_PID)
{
	string stSql = "SELECT PK_ID,FK_PID,PROCESS_NAME,PROCESS_VERSION,PROCESS_AUTHOR,PROCESS_DESC,PROCESS_FILE_PATH,PROCESS_OPT,PROCESS_CREAT_TIME,PROCESS_PUBLISH_TIME,PROCESS_TYPE,PROCESS_SRC,INSTANCE_PK_ID FROM processlib WHERE (FK_PID IN ('";
	stSql = stSql + stFK_PID + "'))";
	return stSql;
}
string CSqlCommand::GetRootSelectID(string F_ID)
{
	string stSql = "SELECT PK_ID,ROOT_NAME FROM configlib WHERE (FK_PID IN ('";
	stSql = stSql + F_ID + "'))";
	return stSql;
}

string CSqlCommand::GetComponentSelectID(string stPK_PID)
{
	string stSql = "SELECT PK_ID,PK_PID,COMPONENT_NAME,COMPONENT_PARAMET_JSON,COMPONENT_OUT_VAR,COMPONENT_TYPE,COMPONENT_FILE_PATH,COMPONENT_IN_PATH,COMPONENT_OUT_PATH,COMPONENT_DESC,COMPONENT_VERSION,COMPONENT_AUTHOR,COMPONENT_OPT,COMPONENT_TAG,COMPONENT_CREAT_TIME,COMPONENT_PUBLISH_TIME,COMPONENT_TYPE_SRC,INSTANCE_PK_ID FROM componentlib WHERE (PK_PID IN ('";
	stSql = stSql + stPK_PID + "'))";
	return stSql;
}
string CSqlCommand::GetBaseComponentSelectID(string stPK_PID)
{
	string stSql = "SELECT PK_ID,PK_PID,COMPONENT_NAME,COMPONENT_PARAMET_JSON,COMPONENT_TYPE,COMPONENT_DESC,COMPONENT_VERSION,COMPONENT_AUTHOR,COMPONENT_OPT,COMPONENT_TAG,COMPONENT_CREAT_TIME,COMPONENT_PUBLISH_TIME,COMPONENT_TYPE_SRC FROM baselib WHERE (PK_PID IN ('";
	stSql = stSql + stPK_PID + "'))";
	return stSql;
}
//INSERT INTO 表名称 VALUES (值1, 值2,....)
string CSqlCommand::RPLChar(string content,string find,string rel)
{
	size_t found=0;
	while(found < content.length())
	{
		found=content.find(find.c_str(),found,find.length());
		if(found < content.length())
		{
			content = content.replace(found,find.length(),rel.c_str());
			found = found + rel.length();
		}
		else break;
	}
	return content;
}

string CSqlCommand::InsetrEnt(string sTableName, list<string> ValueList)
{
	string stSqlValues="";
	for (list<string>::iterator it = ValueList.begin();it != ValueList.end();it++)
	{
		string stDataTest = RPLChar(CEncodedTool::MBToUTF8(*it),"\\","\\\\");
		stDataTest = RPLChar(stDataTest,"\'","\\'");
		if (stSqlValues == "")
		{
			stSqlValues = "'" + stDataTest + "'";
		} 
		else
		{
			stSqlValues = stSqlValues +  ",'" + stDataTest  + "'";
		}
		
	}
	string stSql = "INSERT INTO "  +	sTableName + " VALUES (" + stSqlValues  + ")";
	return stSql;
}
//DELETE FROM 表名称 WHERE 列名称 = 值
string CSqlCommand::DeleteEntID(string sTableName, string stKeyID,string stKeyValue)
{
	string stSql = "DELETE FROM "  +	sTableName + " WHERE " + stKeyID + "=" + "'" + stKeyValue + "'";
	return stSql;
}
string CSqlCommand::GetUpdate(string sTableName ,string stKeyID,string stKeyValue, string stUpdataName,string stUpdataValue)
{
	string stSql = "UPDATE "  +	sTableName + " SET " + stUpdataName + "=" + "'" + CEncodedTool::MBToUTF8(stUpdataValue) + "'" + " WHERE ("+ stKeyID +  " IN ('" +  stKeyValue  + "'))";
	return stSql;
}
string CSqlCommand::FindComponentFkId(string sId)
{
	string stSql = "SELECT COMPONENT_NAME FROM componentlib WHERE (PK_PID IN ('";
	stSql = stSql + CEncodedTool::MBToUTF8(sId) + "'))";
	return stSql;
}
string CSqlCommand::FindComponentName(string sName)
{
	string stSql = "SELECT COMPONENT_NAME FROM componentlib WHERE (COMPONENT_NAME IN ('";
	stSql = stSql + CEncodedTool::MBToUTF8(sName) + "'))";
	return stSql;
}
string CSqlCommand::FindProcessName(string sName)
{
	string stSql = "SELECT PROCESS_NAME FROM processlib WHERE (PROCESS_NAME IN ('";
	stSql = stSql + CEncodedTool::MBToUTF8(sName) + "'))";
	return stSql;
}
string CSqlCommand::FindProcessFkId(string sId)
{
	string stSql = "SELECT PROCESS_NAME FROM processlib WHERE (FK_PID IN ('";
	stSql = stSql + CEncodedTool::MBToUTF8(sId) + "'))";
	return stSql;
}