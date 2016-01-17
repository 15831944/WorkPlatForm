#include "StdAfx.h"
#include <stdexcept>
#include "MySqlConnectionPool.h"
#include "LogMgr.h"
#include <tinyxml.h>

using namespace std;


ConnPool * ConnPool::connPool = NULL;
ConnPool::DBinfo m_dbinfo;

ConnPool::ConnPool(string host,string user,string password,int maxSize){
	//re ClogMgr::WriteLog("ConnPool���캯����ʼ!",FILENAME,LINENUM);
	connectionProperties["hostName"] = host;
	connectionProperties["userName"] = user;
	connectionProperties["password"] = password;
	connectionProperties["OPT_CONNECT_TIMEOUT"] = 600;
	connectionProperties["OPT_RECONNECT"] = true;

	this->maxSize = maxSize;
	this->curSize = 0;
	//��ʼ��driver
	try{
		this->driver = sql::mysql::get_driver_instance();  //���ﲻ���̰߳�ȫ��
	}
	catch(sql::SQLException &e){
		char temp[64];
		sprintf(temp, "%d", e.getErrorCode());
		string s(temp);
		string errorMsg = string("ConnPool���캯��--SQLException: ") + e.what() + string(" MySQL error code: ") + s + string(" SQLState ") +  e.getSQLState();
	//	ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
	}
	catch(std::runtime_error &e){
		string errorMsg = string("ConnPool���캯��--runtime_error: ") + e.what(); 
		//ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
	}

	PTHREAD_INIT;
	pthread_mutex_init(&lock,NULL);
	//��ʼ�����ӳ�
	this->Init(maxSize/2);
	////re ClogMgr::WriteLog("ConnPool���캯������!",FILENAME,LINENUM);

}

char* ConnPool::GetAppPath()
{
	TCHAR szFilePath[MAX_PATH + 1]; 
	GetModuleFileName(NULL, szFilePath, MAX_PATH); 
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	CString strExePath =  szFilePath;
	return strExePath.GetBuffer(0);
}
char* ConnPool::itoa(int nNumber)
{
	char *strRtn = (char*)malloc(10);
	if (strRtn) sprintf(strRtn,"%d",nNumber);
	return strRtn;
}

bool ConnPool::ReadCfg()
{
	////re ClogMgr::WriteLog("ConnPool::ReadCfg������ʼ!",FILENAME,LINENUM);
	//WIN32_FIND_DATA findFileData;

	char szFileName [MAX_PATH];

	::GetModuleFileName(NULL,szFileName,MAX_PATH);

// 	CString strXmlPath;
// 
// 	strXmlPath.Format("%s",szFileName);
// 
// 	strXmlPath = strXmlPath.Left(strXmlPath.ReverseFind('\\') + 1);
// 	
// 	strXmlPath += _T("Config.xml");
// 
// 	HANDLE hHandle = FindFirstFile(strXmlPath, &findFileData); 
// 
// 	if (hHandle == INVALID_HANDLE_VALUE)
// 	{
// 		return false;
// 	}
// 
// 	TiXmlDocument m_XmlDoc;
// 	m_XmlDoc.LoadFile(strXmlPath);
// 	m_XmlDoc.Parse(strXmlPath);
// 	TiXmlElement* rootElement = m_XmlDoc.RootElement(); //Config
// 	TiXmlNode* nodeConfig =  rootElement->FirstChild("DBManager");
// 	TiXmlElement* DBManagerElement = nodeConfig->ToElement(); 
// 	TiXmlElement* DBHostElement = DBManagerElement->FirstChildElement();
// 
// 	while ( DBHostElement ) {
// 		TiXmlAttribute* attributeOfLogHost = DBHostElement->FirstAttribute(); //���LogLevel������
// 		while ( attributeOfLogHost ) {
// 			attributeOfLogHost = attributeOfLogHost->Next();
// 		}
// 
// 		m_dbinfo.host = DBHostElement->GetText();
// 
// 		TiXmlElement* DBUserElement  = DBHostElement->NextSiblingElement();
// 		m_dbinfo.user = DBUserElement->GetText();
// 
// 		TiXmlElement* DBDBPwdElement  = DBUserElement->NextSiblingElement();
// 		m_dbinfo.pwd = DBDBPwdElement->GetText();
// 
// 		TiXmlElement* DBNameElement  = DBDBPwdElement->NextSiblingElement();
// 		m_dbinfo.dbname = DBNameElement->GetText();
// 
// 		TiXmlElement* DBPoolSzieElement  = DBNameElement->NextSiblingElement();
// 		m_dbinfo.maxsize = atoi(DBPoolSzieElement->GetText());
// 
// 		DBHostElement = DBPoolSzieElement->NextSiblingElement();
// 	}
	m_dbinfo.host = "tcp://127.0.0.1:3307";
		m_dbinfo.user = "root";
		m_dbinfo.pwd = "123789";
		m_dbinfo.dbname = "library";

		m_dbinfo.maxsize = 1;

	////re ClogMgr::WriteLog("ConnPool::ReadCfg��������!",FILENAME,LINENUM);
	return true;
}
bool ConnPool::WriteCfg()
{
	return true;
}

ConnPool::~ConnPool(){
	detach_ptw32();
	this->Destroy();
	
}

ConnPool *ConnPool::GetInstance(){
	if(!ReadCfg())
	{
		return NULL;
	}
	if(connPool == NULL) {
		connPool = new ConnPool(m_dbinfo.host,m_dbinfo.user,m_dbinfo.pwd,m_dbinfo.maxsize);
	}

	return connPool;
}

void ConnPool::Init(int size){
	////re ClogMgr::WriteLog("ConnPool::Init������ʼ!",FILENAME,LINENUM);
	sql::Connection * conn ;
	pthread_mutex_lock(&lock);

	for(int i = 0; i < size ;){
		conn = this->CreateConnection();
		if(conn){
			i++;            
			conns.push_back(conn);
			++curSize;
		}
		else{
		//	//re ClogMgr::WriteLog("ConnPool::Init����--��ʼ�����ӳ�ʧ��!",FILENAME,LINENUM,ERROR_LOG_LEVEL);
			break;
		}
	}

	pthread_mutex_unlock(&lock); 
	//re ClogMgr::WriteLog("ConnPool::Init��������!",FILENAME,LINENUM);
}

void ConnPool::Destroy(){
	list<sql::Connection *>::iterator pos;

	pthread_mutex_lock(&lock);

	for(pos = conns.begin(); pos != conns.end();++pos){
		this->TerminateConnection(*pos);
	}

	curSize = 0;
	conns.clear();
	
	pthread_mutex_unlock(&lock); 
	pthread_mutex_destroy(&lock);  
}

sql::Connection * ConnPool::CreateConnection(){//���ﲻ����curSize������
	//re ClogMgr::WriteLog("ConnPool::CreateConnection������ʼ!",FILENAME,LINENUM);
	sql::Connection *conn;

	try{
		conn = driver->connect(connectionProperties);
	//	//re ClogMgr::WriteLog("ConnPool::CreateConnection����һ������!",FILENAME,LINENUM);
		////re ClogMgr::WriteLog("ConnPool::CreateConnection��������!",FILENAME,LINENUM);
		return conn;
	}
	catch(sql::SQLException &e){
		char temp[64];
		sprintf(temp, "%d", e.getErrorCode());
		string s(temp);
		string errorMsg = string("ConnPool���캯��--SQLException: ") + e.what() + string(" MySQL error code: ") + s + string(" SQLState ") +  e.getSQLState();
	//	ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
		return NULL;
	}
	catch(std::runtime_error &e){
		string errorMsg = string("runtime_error: ") + e.what(); 
	//	ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
		return NULL;
	}
	////re ClogMgr::WriteLog("ConnPool::CreateConnection��������!",FILENAME,LINENUM);
}

void ConnPool::TerminateConnection(sql::Connection * conn){
	////re ClogMgr::WriteLog("ConnPool::TerminateConnection������ʼ!",FILENAME,LINENUM);
	if(conn){
		try{
			conn->close();
		}
		catch(sql::SQLException &e){
			char temp[64];
			sprintf(temp, "%d", e.getErrorCode());
			string s(temp);
			string errorMsg = string("SQLException:") + e.what() + string(" MySQL error code: ") + s + string(" SQLState ") +  e.getSQLState();
		//	ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
		}
		catch(std::runtime_error &e){
			string errorMsg = string("runtime_error: ") + e.what(); 
			//ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
		}

		free (conn);
		conn = NULL;
	}

	////re ClogMgr::WriteLog("ConnPool::TerminateConnection��������!",FILENAME,LINENUM);
}

sql::Connection * ConnPool::GetConnection(){
//	//re ClogMgr::WriteLog("ConnPool::GetConnection������ʼ!",FILENAME,LINENUM);
	sql::Connection * conn;

	pthread_mutex_lock(&lock);

	if(conns.size() > 0){//�п�������,�򷵻�
		conn = conns.front();
		conns.pop_front();

		if(conn->isClosed()){ //������ӹر�,�����´�һ������
			////re ClogMgr::WriteLog("ConnPool::GetConnectionһ�������ѹر�!",FILENAME,LINENUM);
			delete conn;
			conn = NULL;
			conn = this->CreateConnection();
		}

		if(conn == NULL){ //�������Ӳ��ɹ�
			--curSize;
		}
		pthread_mutex_unlock(&lock); 
		////re ClogMgr::WriteLog("ConnPool::GetConnection��������!",FILENAME,LINENUM);
		return conn;
	}
	else{
		if(curSize < maxSize){//�����Դ����µ�����
			conn = this->CreateConnection();
			if(conn){
				++curSize;
				pthread_mutex_unlock(&lock); 
			//	//re ClogMgr::WriteLog("ConnPool::GetConnection��������!",FILENAME,LINENUM);
				return conn;
			}
			else{
				pthread_mutex_unlock(&lock); 
				return NULL;
			}
		}
		else{//���ӳ��Ѿ�����
			pthread_mutex_unlock(&lock); 
			return NULL;
		}
	}    
	////re ClogMgr::WriteLog("ConnPool::GetConnection��������!",FILENAME,LINENUM);
}

void ConnPool::ReleaseConnection(sql::Connection * conn){
	if(conn){
		pthread_mutex_lock(&lock);

		conns.push_back(conn);

		pthread_mutex_unlock(&lock); 
	}
}

sql::Connection * ConnPool::GetConnectionTry(int maxNum){
	sql::Connection * conn = NULL;

	for(int i = 0; i < maxNum; ++i){
		conn = this->GetConnection();
		if(conn){
			return conn;
		}
		else {
			Sleep(2);
		}
	}

	return NULL;
}