#pragma once
#include <list>
class CResLibData;

using namespace std;

class CResLibData
{
public:
	CResLibData(void);
	virtual ~CResLibData(void);

	enum {
		RT_ROBOT,
		RT_PROCESS,
		RT_BASECOMPONET
	};

	string m_sPkID;
	string m_sFkpID;
	string m_sName;
	string m_sVersions;
	string m_sAuthor;
	string m_sDsec;
	string m_sOpt;
	string m_sCreatTime;
	string m_sPublishTime;
	string m_sEntType;
	string m_sSrc;
	string m_sSvgJson;
	string m_InstanceID;
	int m_nResType;

	virtual string GetSvgJsonStr(){return "";}
	virtual bool PublishFile(){return true;};
	virtual bool DelPublishFile(){return true;};

protected:

	list<string> m_SqlList;
public:
	int GetSqlLists(list<string>& SqlList);
	list<CResLibData*> m_pBaseList;

	bool RemoveBaseList(CResLibData* pResLibDataBase);

	void Myreplace(string &str,string oldstr,string newstr);
	
};

