#pragma once
#include <list>
using namespace std;
class CSqlCommand
{
public:
	CSqlCommand(void);
	virtual ~CSqlCommand(void);

public:
	static string GetProcessSelectID(string stFK_PID);
	static string GetRootSelectID(string F_ID);
	static string GetComponentSelectID(string stPK_PID);
	static string GetBaseComponentSelectID(string stPK_PID);
	static string FindComponentName(string sName);
	static string FindComponentFkId(string sId);
	static string FindProcessName(string sName);
	static string FindProcessFkId(string sId);
	static string InsetrEnt(string sTableName, list<string> ValueList);

	static string DeleteEntID(string sTableName, string stKeyID,string stKeyValue);
	static string GetUpdate(string sTableName ,string stKeyID,string stKeyValue, string stUpdataName,string stUpdataValue);
	static string RPLChar(string content,string find,string rel);
};

