#pragma once
#include "stdafx.h"
#include <stdexcept>
#include "magicdb.h"
#include "LogMgr.h"

using namespace std;

MagicDB::MagicDB(){
    this->connPool = ConnPool::GetInstance();
}

string MagicDB::Get_client_info()
{
	 sql::Connection *conn = NULL;
	 try{
		 conn = connPool->GetConnection();  //�����ӳ��л�ȡ����
		 if(conn == NULL){
			 conn = connPool->GetConnectionTry(2);
			 if(conn == NULL){
				 //re ClogMgr::WriteLog("MagicDB::Get_client_info����ִ�д���!",FILENAME,LINENUM,ERROR_LOG_LEVEL);
				 return NULL;
			 }
		 }
	 }
	 catch(sql::SQLException &e){
		 string errorMsg = string("SQLException:") + e.what() + string(" MySQL error code: ") + string(itoa(e.getErrorCode())) + string(" SQLState ") +  e.getSQLState();
		 ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);

		 connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
		 return NULL;
	 }
	return conn->getClientInfo();
}

sql::ResultSet * MagicDB::Query(const string &database,const string &sql){
	//re ClogMgr::WriteLog("MagicDB::Query������ʼ!",FILENAME,LINENUM);
    sql::Connection *conn = NULL;
    sql::Statement *stmt = NULL;
    sql::ResultSet *res = NULL;
    
    try{
        conn = connPool->GetConnection();  //�����ӳ��л�ȡ����
        if(conn == NULL){
            conn = connPool->GetConnectionTry(2);
            if(conn == NULL){
				//re ClogMgr::WriteLog("MagicDB::Query����--��ѯ���Ӵ���!",FILENAME,LINENUM,ERROR_LOG_LEVEL);
                
                return NULL;
            }
        }

        conn->setSchema(database);
        stmt = conn->createStatement();
        res = stmt->executeQuery(sql.c_str());
        
        connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
        if(stmt!=NULL)
		{
        delete stmt;
		stmt = NULL;
		}
		//re ClogMgr::WriteLog("MagicDB::Query��������!",FILENAME,LINENUM);
        return res;
    }
    catch(sql::SQLException &e){
		string errorMsg = string("SQLException:") + e.what() + string(" MySQL error code: ") + string(itoa(e.getErrorCode())) + string(" SQLState ") +  e.getSQLState();
		ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
        
        connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
        
		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
		if(res!=NULL)
		{
        delete res;
		res = NULL;
		}
        return NULL;
    }
    catch(std::runtime_error &e){
		string errorMsg = string("runtime_error: ") + e.what(); 
		ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
        
        connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
        
		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
		if(res!=NULL)
		{
			delete res;
			res = NULL;
		}
        return NULL;
    }
}

int MagicDB::Execute(const string &database,const string &sql){
    sql::Statement * stmt = NULL;
    sql::Connection *conn = NULL;
    
    try{
        conn = connPool->GetConnection();  //�����ӳ��л�ȡ����
		
        if(conn == NULL){
            conn = connPool->GetConnectionTry(2);
            if(conn == NULL){
              //re ClogMgr::WriteLog("MagicDB::Execute����--��ѯ���Ӵ���!",FILENAME,LINENUM,ERROR_LOG_LEVEL);
                return -3;
            }
        }
	
        conn->setSchema(database);
	
        stmt = conn->createStatement();
		conn->setAutoCommit(0);  

        stmt->execute(sql);
        conn->commit();
        connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
        stmt->close();
		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
        return 0;
    }
    catch(sql::SQLException &e){
		string errorMsg = string("SQLException:") + e.what() + string(" MySQL error code: ") + string(itoa(e.getErrorCode())) + string(" SQLState ") +  e.getSQLState();
		ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
        
        connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
        
		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
        return -1;
    }
    catch(std::runtime_error &e){
		string errorMsg = string("runtime_error: ") + e.what(); 
		ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);
        
        connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
        
		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
        return -2;
    }
}

int MagicDB::PrepareStatementExecute(const string &database,const string &sql)
{
	sql::PreparedStatement * stmt = NULL;
	sql::Connection *conn = NULL;

	try{
		conn = connPool->GetConnection();  //�����ӳ��л�ȡ����

		if(conn == NULL){
			conn = connPool->GetConnectionTry(2);
			if(conn == NULL){
				return -3;
			}
		}

		conn->setSchema(database);

		stmt = conn->prepareStatement(sql);
		conn->setAutoCommit(0);
		stmt->execute();
		conn->commit();
		stmt->close();
		connPool->ReleaseConnection(conn);//�����ӷ������ӳ���
		
		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
		return 0;
	}
	catch(sql::SQLException &e){
		string errorMsg = string("SQLException:") + e.what() + string(" MySQL error code: ") + string(itoa(e.getErrorCode())) + string(" SQLState ") +  e.getSQLState();
		ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);

		connPool->ReleaseConnection(conn);//�����ӷ������ӳ���

		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
		return -1;
	}
	catch(std::runtime_error &e){
		string errorMsg = string("runtime_error: ") + e.what(); 
		ClogMgr::WriteLog(errorMsg.c_str(),FILENAME,LINENUM,ERROR_LOG_LEVEL);

		connPool->ReleaseConnection(conn);//�����ӷ������ӳ���

		if(stmt!=NULL)
		{
			delete stmt;
			stmt = NULL;
		}
		return -2;
	}

}


LPCTSTR MagicDB::GetAppPath()
{
	TCHAR szFilePath[MAX_PATH + 1]; 
	GetModuleFileName(NULL, szFilePath, MAX_PATH); 
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	CString strExePath =  szFilePath;
	return strExePath;
}
char* MagicDB::itoa(int nNumber)
{
	char *strRtn = (char*)malloc(10);
	if (strRtn) sprintf(strRtn,"%d",nNumber);
	return strRtn;
}

MagicDB::~MagicDB(){
	if(connPool!=NULL)
	{
		delete connPool;
		connPool = NULL;
	}
    
}