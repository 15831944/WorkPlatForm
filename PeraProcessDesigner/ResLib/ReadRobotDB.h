#pragma once
#include <string>
#include <cppconn\prepared_statement.h>



class CVorxThread;
class CResLibData;
class MagicDB;
class CResLibProcessData;
class CResLibRobotData;
class CcomponentChain;
class CProcessChain;
class CResLibBaseComponentData;



using namespace std;
class CReadRobotDB
{
public:
	CReadRobotDB(void);
	virtual ~CReadRobotDB(void);

enum
{
	READ_DB_LOAD_OFF,
	READ_DB_LOAD_NO
};
public:
	void Load();
	void Close();
	int LoadingDB();//��ȡ���ݿ����

	
	bool AgreementDb(sql::ResultSet * rProcesslibDb,CResLibRobotData* RobotEnt);
	void RecursionDB(string stFK_PID,CResLibRobotData* pRobotBaseList);
	bool DelDB(CResLibRobotData* RobotEnt);
	bool AddDB(CResLibRobotData* RobotEnt);
	bool EditDB(CResLibRobotData* RobotEnt);
	bool FindRobotName(string RobotName);
	bool FindRobotName(string RobotFkId,string RobotName);

	bool AgreementDb(sql::ResultSet * rProcesslibDb,CResLibProcessData* ProcessEnt);
	void RecursionDB(string stFK_PID,CResLibProcessData* ProcessBaseList);
	bool DelDB(CResLibProcessData* ProcessEnt);
	bool AddDB(CResLibProcessData* ProcessEnt);
	bool EditDB(CResLibProcessData* ProcessEnt);
	bool FindProcessName(string ProcessName);
	bool FindProcessName(string ProcessFkId,string ProcessName);
	bool AgreementDb(sql::ResultSet * rProcesslibDb,CResLibBaseComponentData* RobotEnt);
	void RecursionDB(string stFK_PID,CResLibBaseComponentData* pBaseComponentList);

	CResLibData* m_PRoot;
	MagicDB* m_DBmgr;
	CVorxThread* m_pthread;
	int m_nIsLoad;
public:
	string m_stID;		//����Ԫ��ID
	string m_stName;	//������Դ��
public:
	static int ReadDBThread(LPVOID lParam);
};

