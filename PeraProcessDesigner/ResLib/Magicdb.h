#pragma once
#ifndef MAGICDB_H
#define MAGICDB_H

#include "stdafx.h"
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "MySqlConnectionPool.h"

using namespace std;

class MagicDB {
    private:
        ConnPool *connPool;
    public:
        MagicDB();
        ~MagicDB();
		static LPCTSTR GetAppPath();
		char* itoa(int nNumber);
        sql::ResultSet *Query(const string &database,const string &sql);
        int Execute(const string &database,const string &sql);
		int PrepareStatementExecute(const string &database,const string &sql);
		string Get_client_info();  //{ return connPool->getClientInfo() ; };
};

#endif