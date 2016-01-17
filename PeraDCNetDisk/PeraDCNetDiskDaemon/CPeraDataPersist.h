#include "stdafx.h"
#include "TaskDB.h"

extern CPeraDataPersist thePersistMrg;

#ifndef __PERADATAPERSIST_H__
#define __PERADATAPERSIST_H__


typedef vector< CTaskData* >	PERSIST_DATA;

class CPeraDataPersist
{
public:
	CPeraDataPersist();
	~CPeraDataPersist();
	BOOL	Init();
	BOOL	Release();
	BOOL	DoOperation( CTaskData* pData );										//Add Delete Update 操作
	BOOL	Save();
	/*
	提供用来下载模块或上传模块启动时查询数据，用来导入任务的接口。
	实际获取的数据时某一时刻的XML文件的一个快照。
	列表中的元素是个指针类型，内部创建快照时做的一个副本，需要每次调用该接口时，释放每一个元素的空间。
	*/
	BOOL	GetDownLoadData( PERSIST_DATA & vecData );
	/*
	同上说明
	*/
	BOOL	GetUpLoadData( PERSIST_DATA & vecData );
	BOOL    GetFileCopyData( PERSIST_DATA & vecData );
	CTaskData* GetTaskDataCopied( LPCTSTR lpszIdPath );

protected:
	PERSIST_DATA 				m_vecData;											//任务队列
	static DWORD WINAPI 		_WorksThread(LPVOID lpParameter);					//工作线程
	CString						GetXmlPath();
private:
	HANDLE						m_hWorkThread;
	DWORD						m_dwThreadID;
	CRITICAL_SECTION			m_criLock;											//锁: 任务队列 和 XML对象
	BOOL						m_bStopFlag;										//线程执行标志
	CTaskDB						m_XmlDataObj;
	HANDLE						m_hEventInit;
};

#endif