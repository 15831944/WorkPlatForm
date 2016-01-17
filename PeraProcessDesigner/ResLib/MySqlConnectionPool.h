#ifndef CONN_POOL_H
#define CONN_POOL_H
#pragma once
#include "StdAfx.h"
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <list>
#include "thread_env.h"

using namespace std;

class ConnPool{
    private:
        list<sql::Connection *> conns;//���Ӷ���
        int curSize;//��ǰ������·������Ŀ
        int maxSize;//���������Ŀ
        sql::ConnectOptionsMap connectionProperties;
        pthread_mutex_t lock;//���Ӷ��л�����
        static ConnPool * connPool;
        sql::Driver * driver;//mysql connector C++ driver
        sql::Connection * CreateConnection();//����һ������
        void TerminateConnection(sql::Connection * conn);//��ֹһ������
        void Init(int initialSize);//��ʼ�����ӳ�
        void Destroy();//�������ӳ�
		static bool ReadCfg();
		bool WriteCfg();
		static char* GetAppPath();
		char* itoa(int nNumber);
    protected:
        ConnPool(string host,string user,string password,int maxSize);
    public:
        ~ConnPool();
        sql::Connection * GetConnection();//��ȡһ������
        void ReleaseConnection(sql::Connection * conn);//�ͷ�һ������
        sql::Connection * GetConnectionTry(int maxNum);//GetConnection�ļ�ǿ�棬maxNum�������Դ���
        static ConnPool * GetInstance();//��ȡһ��ConnPool����ʵ��
    public:
		struct DBinfo
		{
			string host;
			string user;
			string pwd;
			string dbname;
			int maxsize;
		};
};

#endif