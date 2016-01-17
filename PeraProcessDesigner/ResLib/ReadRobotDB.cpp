#include "StdAfx.h"
#include "ReadRobotDB.h"
#include "Magicdb.h"
#include "EncodedTool.h"
#include "SqlCommand.h"
#include "ResLibProcessData.h"
#include "ResLibRobotData.h"
#include "ResLibData.h"
#include <ResLibBaseComponentData.h>
#include <PeraProcessDesigner.h>
#include "VorxThread.h"

extern CPeraProcessDesignerApp theApp;
extern ConnPool::DBinfo m_dbinfo;
CReadRobotDB::CReadRobotDB(void)
{
	m_pthread = new CVorxThread;
	m_nIsLoad = READ_DB_LOAD_OFF;
}


CReadRobotDB::~CReadRobotDB(void)
{
	if (m_PRoot != NULL)
	{	
		delete m_PRoot;
		m_PRoot = NULL;
	}
	if (m_DBmgr != NULL)
	{
		delete m_DBmgr;
		m_DBmgr = NULL;
	}
	m_nIsLoad = 0;
	if (m_pthread != NULL)
	{
		delete m_pthread;
		m_pthread = NULL;
	}
}
int  CReadRobotDB::ReadDBThread(LPVOID lParam)
{
	CReadRobotDB* pReadDB = (CReadRobotDB*)lParam;
	if (pReadDB->LoadingDB() == 0)
	{
		return 1;
	}
	theApp.m_pMainWnd->SendMessage(WM_UPDATE_RESLIB_TREE
		, NULL, NULL);
	return -1;
}
void CReadRobotDB::Load()
{
	if (m_nIsLoad == READ_DB_LOAD_NO)
	{
		Close();
	}
	m_pthread->SetParam((LPFUN_THREAD)ReadDBThread,this,100);
	m_pthread->SetPriority(3);
	m_pthread->StartThread();
}
void CReadRobotDB::Close()
{
	if (m_PRoot != NULL)
	{	
		delete m_PRoot;
		m_PRoot = NULL;
	}
	if (m_DBmgr != NULL)
	{
		delete m_DBmgr;
		m_DBmgr = NULL;
	}
	m_nIsLoad = READ_DB_LOAD_OFF;
}
int CReadRobotDB::LoadingDB()
{
	if (m_PRoot == NULL)
	{
		m_PRoot = new CResLibData();
	}	
	if (m_DBmgr == NULL)
	{
		m_DBmgr = new MagicDB();
	}
	if (m_nIsLoad == READ_DB_LOAD_NO)
	{
		return 1;
	}
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::GetRootSelectID("0"));	
	if (rProcesslibDb == NULL)
	{
		return 0;
	}

	while (rProcesslibDb->next())
	{
		m_stID = rProcesslibDb->getString("PK_ID");
		m_stID = CEncodedTool::UTF8ToMB(m_stID);
		m_stName = rProcesslibDb->getString("ROOT_NAME");
		m_stName = CEncodedTool::UTF8ToMB(m_stName);
	}
	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}

	RecursionDB(m_stID,(CResLibProcessData*)m_PRoot);
	RecursionDB(m_stID,(CResLibRobotData*)m_PRoot);
	RecursionDB(m_stID,(CResLibBaseComponentData*)m_PRoot);
	m_nIsLoad = READ_DB_LOAD_NO;
	return 1;
}
bool CReadRobotDB::AgreementDb(sql::ResultSet * rProcesslibDb,CResLibProcessData* ProcessEnt)
{
	try
	{
		ProcessEnt->m_sPkID								=		rProcesslibDb->getString("PK_ID");  
		ProcessEnt->m_sPkID = CEncodedTool::UTF8ToMB(ProcessEnt->m_sPkID);
		ProcessEnt->m_sFkpID                              =		rProcesslibDb->getString("FK_PID");  
		ProcessEnt->m_sFkpID = CEncodedTool::UTF8ToMB(ProcessEnt->m_sFkpID);
		ProcessEnt->m_sName                        =		rProcesslibDb->getString("PROCESS_NAME"); 
		ProcessEnt->m_sName = CEncodedTool::UTF8ToMB(ProcessEnt->m_sName);   
		ProcessEnt->m_sVersions                     =		rProcesslibDb->getString("PROCESS_VERSION");  
		ProcessEnt->m_sVersions = CEncodedTool::UTF8ToMB(ProcessEnt->m_sVersions);
		ProcessEnt->m_sAuthor                      =		rProcesslibDb->getString("PROCESS_AUTHOR"); 
		ProcessEnt->m_sAuthor = CEncodedTool::UTF8ToMB(ProcessEnt->m_sAuthor); 
		ProcessEnt->m_sDsec                        =		rProcesslibDb->getString("PROCESS_DESC");
		ProcessEnt->m_sDsec = CEncodedTool::UTF8ToMB(ProcessEnt->m_sDsec);    
		
		ProcessEnt->m_ProcessFilePath                   =		rProcesslibDb->getString("PROCESS_FILE_PATH");
		ProcessEnt->m_ProcessFilePath = CEncodedTool::UTF8ToMB(ProcessEnt->m_ProcessFilePath);
		ProcessEnt->m_sOpt                         =		rProcesslibDb->getString("PROCESS_OPT");
		ProcessEnt->m_sOpt = CEncodedTool::UTF8ToMB(ProcessEnt->m_sOpt);
		ProcessEnt->m_sCreatTime                  =		rProcesslibDb->getString("PROCESS_CREAT_TIME");
		ProcessEnt->m_sCreatTime = CEncodedTool::UTF8ToMB(ProcessEnt->m_sCreatTime);
		ProcessEnt->m_sPublishTime				=		rProcesslibDb->getString("PROCESS_PUBLISH_TIME");
		ProcessEnt->m_sPublishTime = CEncodedTool::UTF8ToMB(ProcessEnt->m_sPublishTime);
		ProcessEnt->m_sEntType                        =		rProcesslibDb->getString("PROCESS_TYPE");
		ProcessEnt->m_sEntType = CEncodedTool::UTF8ToMB(ProcessEnt->m_sEntType);
		ProcessEnt->m_sSrc                        =		rProcesslibDb->getString("PROCESS_SRC");
		ProcessEnt->m_sSrc = CEncodedTool::UTF8ToMB(ProcessEnt->m_sSrc);
		ProcessEnt->m_InstanceID                        =		rProcesslibDb->getString("INSTANCE_PK_ID");
		ProcessEnt->m_InstanceID = CEncodedTool::UTF8ToMB(ProcessEnt->m_InstanceID); 
		ProcessEnt->m_nResType = CResLibData::RT_PROCESS;
	}
	catch (...)
	{
		return false;
	}
	return true;
}
bool CReadRobotDB::DelDB(CResLibProcessData* ProcessEnt)
{
	m_DBmgr->Execute(m_dbinfo.dbname,CSqlCommand::DeleteEntID("processlib","PK_ID",ProcessEnt->m_sPkID));
	return true;
}
bool CReadRobotDB::AddDB(CResLibProcessData* ProcessEnt)
{

	list<string> ValueListtmp;	
	ValueListtmp.push_back(ProcessEnt->m_sPkID);												
	ValueListtmp.push_back(ProcessEnt->m_sFkpID);              
	ValueListtmp.push_back(ProcessEnt->m_sName);        
	ValueListtmp.push_back(ProcessEnt->m_sVersions);     
	ValueListtmp.push_back(ProcessEnt->m_sAuthor);      
	ValueListtmp.push_back(ProcessEnt->m_sDsec);   
	ValueListtmp.push_back("");
	ValueListtmp.push_back(ProcessEnt->m_ProcessFilePath); 
	ValueListtmp.push_back("");
	ValueListtmp.push_back(ProcessEnt->m_sOpt);         
	ValueListtmp.push_back(ProcessEnt->m_sCreatTime);  
	ValueListtmp.push_back(ProcessEnt->m_sPublishTime);
	ValueListtmp.push_back(ProcessEnt->m_sEntType);        
	ValueListtmp.push_back(ProcessEnt->m_sSrc); 
	ValueListtmp.push_back(ProcessEnt->m_InstanceID); 
	int nRet = m_DBmgr->Execute(m_dbinfo.dbname,CSqlCommand::InsetrEnt("processlib",ValueListtmp));	
	if (nRet == -1)
	{
		return false;
	}
	return true;
}
bool CReadRobotDB::EditDB(CResLibProcessData* ProcessEnt)
{
	list<string> sqlDateList;
	ProcessEnt->GetSqlLists(sqlDateList);
	for (list<string>::iterator it = sqlDateList.begin();it != sqlDateList.end();it++)
	{		
		try
		{
			m_DBmgr->Execute(m_dbinfo.dbname,(*it));
		}
		catch (...)
		{
			
		}		
	}
	sqlDateList.clear();
	return true;
}
void CReadRobotDB::RecursionDB(string stFK_PID,CResLibProcessData* ProcessBaseList)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::GetProcessSelectID(stFK_PID));	
	if (rProcesslibDb == NULL)
	{
		return ;
	}
	while (rProcesslibDb->next())
	{ 
		CEncodedTool entool;
		CResLibProcessData* ProcessEntTmp = new CResLibProcessData();
		if(!AgreementDb(rProcesslibDb,ProcessEntTmp))		//½âÎö
			return ;
		RecursionDB(ProcessEntTmp->m_sPkID,ProcessEntTmp);
		ProcessBaseList->m_pBaseList.push_back(ProcessEntTmp);
	}

	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
}
bool CReadRobotDB::AgreementDb(sql::ResultSet * rProcesslibDb,CResLibRobotData* RobotEnt)
{
	try
	{
		RobotEnt->m_sPkID                   =		rProcesslibDb->getString("PK_ID");
		RobotEnt->m_sFkpID                  =		rProcesslibDb->getString("PK_PID");
		RobotEnt->m_sName          =		rProcesslibDb->getString("COMPONENT_NAME");
		RobotEnt->m_RobotParametersJson        =		rProcesslibDb->getString("COMPONENT_PARAMET_JSON");
		RobotEnt->m_RobotOutnVar       =		rProcesslibDb->getString("COMPONENT_OUT_VAR");
		RobotEnt->m_sEntType          =		rProcesslibDb->getString("COMPONENT_TYPE");
		RobotEnt->m_RobotFilePath     =		rProcesslibDb->getString("COMPONENT_FILE_PATH");
		RobotEnt->m_RobotInPath       =		rProcesslibDb->getString("COMPONENT_IN_PATH");
		RobotEnt->m_RobotOutPath      =		rProcesslibDb->getString("COMPONENT_OUT_PATH");
		RobotEnt->m_sDsec          =		rProcesslibDb->getString("COMPONENT_DESC");
		RobotEnt->m_sVersions       =		rProcesslibDb->getString("COMPONENT_VERSION");
		RobotEnt->m_sAuthor        =		rProcesslibDb->getString("COMPONENT_AUTHOR");
		RobotEnt->m_sOpt           =		rProcesslibDb->getString("COMPONENT_OPT");
		RobotEnt->m_RobotType           =		rProcesslibDb->getString("COMPONENT_TAG");
		RobotEnt->m_sCreatTime    =		rProcesslibDb->getString("COMPONENT_CREAT_TIME");
		RobotEnt->m_sPublishTime  =		rProcesslibDb->getString("COMPONENT_PUBLISH_TIME");
		RobotEnt->m_sSrc      =		rProcesslibDb->getString("COMPONENT_TYPE_SRC");
		RobotEnt->m_InstanceID           =		rProcesslibDb->getString("INSTANCE_PK_ID");


		RobotEnt->m_sPkID                   =		  CEncodedTool::UTF8ToMB(RobotEnt->m_sPkID);
		RobotEnt->m_sFkpID                  =       CEncodedTool::UTF8ToMB(RobotEnt->m_sFkpID);
		RobotEnt->m_sName          =		  CEncodedTool::UTF8ToMB(RobotEnt->m_sName);
		RobotEnt->m_RobotParametersJson        =		  CEncodedTool::UTF8ToMB(RobotEnt->m_RobotParametersJson);
		RobotEnt->m_RobotOutnVar       =       CEncodedTool::UTF8ToMB(RobotEnt->m_RobotOutnVar);
		RobotEnt->m_sEntType          =       CEncodedTool::UTF8ToMB(RobotEnt->m_sEntType);
		RobotEnt->m_RobotFilePath     =       CEncodedTool::UTF8ToMB(RobotEnt->m_RobotFilePath);
		RobotEnt->m_RobotInPath       =       CEncodedTool::UTF8ToMB(RobotEnt->m_RobotInPath);
		RobotEnt->m_RobotOutPath      =       CEncodedTool::UTF8ToMB(RobotEnt->m_RobotOutPath);
		RobotEnt->m_sDsec          =       CEncodedTool::UTF8ToMB(RobotEnt->m_sDsec);
		RobotEnt->m_sVersions       =       CEncodedTool::UTF8ToMB(RobotEnt->m_sVersions);
		RobotEnt->m_sAuthor        =       CEncodedTool::UTF8ToMB(RobotEnt->m_sAuthor);
		RobotEnt->m_sOpt           =       CEncodedTool::UTF8ToMB(RobotEnt->m_sOpt);
		RobotEnt->m_RobotType           =       CEncodedTool::UTF8ToMB(RobotEnt->m_RobotType);
		RobotEnt->m_sCreatTime    =       CEncodedTool::UTF8ToMB(RobotEnt->m_sCreatTime);
		RobotEnt->m_sPublishTime  =       CEncodedTool::UTF8ToMB(RobotEnt->m_sPublishTime);
		RobotEnt->m_sSrc      =       CEncodedTool::UTF8ToMB(RobotEnt->m_sSrc);
		RobotEnt->m_InstanceID           =       CEncodedTool::UTF8ToMB(RobotEnt->m_InstanceID);
		RobotEnt->m_nResType = CResLibData::RT_ROBOT;
	}
	catch (...)
	{
		return false;
	}
	return true;
	
}
bool CReadRobotDB::AgreementDb(sql::ResultSet * rProcesslibDb,CResLibBaseComponentData* RobotEnt)
{
	try
	{
		RobotEnt->m_sPkID                   =		rProcesslibDb->getString("PK_ID");
		RobotEnt->m_sFkpID                  =		rProcesslibDb->getString("PK_PID");
		RobotEnt->m_sName          =		rProcesslibDb->getString("COMPONENT_NAME");
			
		RobotEnt->m_sEntType          =		rProcesslibDb->getString("COMPONENT_TYPE");		
		RobotEnt->m_sDsec          =		rProcesslibDb->getString("COMPONENT_DESC");
		RobotEnt->m_sVersions       =		rProcesslibDb->getString("COMPONENT_VERSION");
		RobotEnt->m_sAuthor        =		rProcesslibDb->getString("COMPONENT_AUTHOR");
		RobotEnt->m_sOpt           =		rProcesslibDb->getString("COMPONENT_OPT");
		RobotEnt->m_RobotType           =		rProcesslibDb->getString("COMPONENT_TAG");
		RobotEnt->m_sCreatTime    =		rProcesslibDb->getString("COMPONENT_CREAT_TIME");
		RobotEnt->m_sPublishTime  =		rProcesslibDb->getString("COMPONENT_PUBLISH_TIME");
		RobotEnt->m_sSrc      =		rProcesslibDb->getString("COMPONENT_TYPE_SRC");

		RobotEnt->m_sPkID                   =		  CEncodedTool::UTF8ToMB(RobotEnt->m_sPkID);
		RobotEnt->m_sFkpID                  =       CEncodedTool::UTF8ToMB(RobotEnt->m_sFkpID);
		RobotEnt->m_sName          =		  CEncodedTool::UTF8ToMB(RobotEnt->m_sName);
			
		RobotEnt->m_sEntType          =       CEncodedTool::UTF8ToMB(RobotEnt->m_sEntType);		
		RobotEnt->m_sDsec          =       CEncodedTool::UTF8ToMB(RobotEnt->m_sDsec);
		RobotEnt->m_sVersions       =       CEncodedTool::UTF8ToMB(RobotEnt->m_sVersions);
		RobotEnt->m_sAuthor        =       CEncodedTool::UTF8ToMB(RobotEnt->m_sAuthor);
		RobotEnt->m_sOpt           =       CEncodedTool::UTF8ToMB(RobotEnt->m_sOpt);
		RobotEnt->m_RobotType           =       CEncodedTool::UTF8ToMB(RobotEnt->m_RobotType);
		RobotEnt->m_sCreatTime    =       CEncodedTool::UTF8ToMB(RobotEnt->m_sCreatTime);
		RobotEnt->m_sPublishTime  =       CEncodedTool::UTF8ToMB(RobotEnt->m_sPublishTime);
		RobotEnt->m_sSrc      =       CEncodedTool::UTF8ToMB(RobotEnt->m_sSrc);
		
		RobotEnt->m_nResType = CResLibData::RT_BASECOMPONET;
	}
	catch (...)
	{
		return false;
	}
	return true;

}
bool CReadRobotDB::DelDB(CResLibRobotData* RobotEnt)
{
	m_DBmgr->Execute(m_dbinfo.dbname,CSqlCommand::DeleteEntID("componentlib","PK_ID",RobotEnt->m_sPkID));
	return true;
}
bool CReadRobotDB::AddDB(CResLibRobotData* RobotEnt)
{
	list<string> ValueListtmp;	
	ValueListtmp.push_back(RobotEnt->m_sPkID);
	ValueListtmp.push_back(RobotEnt->m_sFkpID);                
	ValueListtmp.push_back(RobotEnt->m_sName);
	ValueListtmp.push_back(RobotEnt->m_RobotParametersJson);
	ValueListtmp.push_back(RobotEnt->m_RobotOutnVar);
	ValueListtmp.push_back(RobotEnt->m_sEntType);
	ValueListtmp.push_back(RobotEnt->m_RobotFilePath);
	ValueListtmp.push_back(RobotEnt->m_RobotInPath);
	ValueListtmp.push_back(RobotEnt->m_RobotOutPath);
	ValueListtmp.push_back(RobotEnt->m_sDsec);
	ValueListtmp.push_back(RobotEnt->m_sVersions);
	ValueListtmp.push_back(RobotEnt->m_sAuthor);
	ValueListtmp.push_back(RobotEnt->m_sOpt);
	ValueListtmp.push_back(RobotEnt->m_RobotType);
	ValueListtmp.push_back(RobotEnt->m_sCreatTime);
	ValueListtmp.push_back(RobotEnt->m_sPublishTime);
	ValueListtmp.push_back(RobotEnt->m_sSrc);
	ValueListtmp.push_back(RobotEnt->m_InstanceID);

	m_DBmgr->Execute(m_dbinfo.dbname,CSqlCommand::InsetrEnt("componentlib",ValueListtmp));	
	return true;
}
bool CReadRobotDB::EditDB(CResLibRobotData* RobotEnt)
{
	list<string> sqlDateList;
	RobotEnt->GetSqlLists(sqlDateList);
	for (list<string>::iterator it = sqlDateList.begin();it != sqlDateList.end();it++)
	{		
		m_DBmgr->Execute(m_dbinfo.dbname,(*it));
	}
	sqlDateList.clear();
	return true;
}
void CReadRobotDB::RecursionDB(string stFK_PID,CResLibRobotData* pRobotBaseList)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::GetComponentSelectID(stFK_PID));	
	if (rProcesslibDb == NULL)
	{
		return ;
	}
	while (rProcesslibDb->next())
	{ 
		CEncodedTool entool;
		CResLibRobotData* RobotEntTmp = new CResLibRobotData();
		if(!AgreementDb(rProcesslibDb,RobotEntTmp))		//½âÎö
			return ;
		RecursionDB(RobotEntTmp->m_sPkID,RobotEntTmp);
		pRobotBaseList->m_pBaseList.push_back(RobotEntTmp);
	}

	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
}
void CReadRobotDB::RecursionDB(string stFK_PID,CResLibBaseComponentData* pBaseComponentList)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::GetBaseComponentSelectID(stFK_PID));	
	if (rProcesslibDb == NULL)
	{
		return ;
	}	
	while (rProcesslibDb->next())
	{ 
		CEncodedTool entool;
		CResLibBaseComponentData* RobotEntTmp = new CResLibBaseComponentData();
		if(!AgreementDb(rProcesslibDb,RobotEntTmp))		//½âÎö
			return ;
		RecursionDB(RobotEntTmp->m_sPkID,RobotEntTmp);
		pBaseComponentList->m_pBaseList.push_back(RobotEntTmp);
	}

	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
}
bool CReadRobotDB::FindProcessName(string ProcessFkId,string ProcessName)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::FindProcessFkId(ProcessFkId));	
	if (rProcesslibDb == NULL)
	{
		return false;
	}
	while (rProcesslibDb->next())
	{ 
		string sName;
		sName = rProcesslibDb->getString("PROCESS_NAME");
		CEncodedTool::UTF8ToMB(sName);
		if (sName == ProcessName)
		{
			return true;
		}
	}
	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
	return false;
}
bool CReadRobotDB::FindProcessName(string ProcessName)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::FindProcessName(ProcessName));	
	if (rProcesslibDb == NULL)
	{
		return false;
	}
	while (rProcesslibDb->next())
	{ 
		string sName;
		sName = rProcesslibDb->getString("PROCESS_NAME");
		CEncodedTool::UTF8ToMB(sName);
		if (sName == ProcessName)
		{
			return true;
		}
	}
	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
	return false;
}
bool CReadRobotDB::FindRobotName(string RobotFkId,string RobotName)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::FindComponentFkId(RobotFkId));	
	if (rProcesslibDb == NULL)
	{
		return false;
	}
	while (rProcesslibDb->next())
	{ 
		string sName;
		sName = rProcesslibDb->getString("COMPONENT_NAME");
		CEncodedTool::UTF8ToMB(sName);
		if (sName == RobotName)
		{
			return true;
		}
	}
	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
	return false;
}
bool CReadRobotDB::FindRobotName(string RobotName)
{
	sql::ResultSet * rProcesslibDb = m_DBmgr->Query(m_dbinfo.dbname,CSqlCommand::FindComponentName(RobotName));	
	if (rProcesslibDb == NULL)
	{
		return false;
	}
	while (rProcesslibDb->next())
	{ 
		string sName;
		sName = rProcesslibDb->getString("COMPONENT_NAME");
		CEncodedTool::UTF8ToMB(sName);
		if (sName == RobotName)
		{
			return true;
		}
	}
	rProcesslibDb->close();
	if (rProcesslibDb != NULL)
	{
		delete rProcesslibDb;
		rProcesslibDb = NULL;
	}
	return false;
}
