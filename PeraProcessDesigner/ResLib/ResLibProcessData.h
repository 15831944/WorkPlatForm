#pragma once
#include <string>
#include <list>
#include "ResLibData.h"
using namespace std;

class CResLibProcessData :
	public CResLibData
{
public:
	CResLibProcessData(void);
	virtual ~CResLibProcessData(void);
	
	string m_ProcessFilePath;

public:
	bool SetPK_ID(string stID);
	bool SetFK_PID(string stPID);
	bool SetProcessName(string sName);
	bool SetProcessVersion(string sVersion);
	bool SetProcessAuthor(string sAuthor);
	bool SetProcessDesc(string sDesc);
	bool SetProcessFilePaht(string sFilePath);
	bool SetProcessOpt(string sOpt);
	bool SetProcessCreatTime(string sCreatTime);
	bool SetProcessPublishTime(string sPublishTime);
	bool SetProcessEntType(string sEntType);
	bool SetProcessSrc(string sSrc);
	virtual string GetSvgJsonStr();
	virtual bool PublishFile();
	virtual bool DelPublishFile();
	bool DownloadFile(string stOutFilePath);
};

