#pragma once
#include "TaskXml.h"

class CTaskDB
{
	friend class CPeraDataPersist;
public:
	CTaskDB(void);
	virtual ~CTaskDB(void);
	//初始化
	//打开xml
	//取得 pDoc、pTasksNode 指针
	BOOL Init( LPCTSTR lpszXmlPath );
	//释放
	void Release(void);
	//保存
	BOOL Save(void);
	//在末尾添加新的任务数据，如果存在相同的idPath，失败
	BOOL Add( CTaskData* pNewData );
	//在指定位置添加新任务数据
	BOOL InsertAfter( const CTaskData* pNewData );
	//删除一条任务
	BOOL Delete( LPCTSTR lpszIdPath );
	//更新相同 idpath 的任务
	BOOL Update( CTaskData* pNewData );
	//取得一条任务数据的副本
	CTaskData* GetTaskDataCopied( LPCTSTR lpszIdPath );
	//判断任务数据是否存在
	BOOL CheckExisted( LPCTSTR lpszIdPath );

protected:
	//查找任务数据
	CTaskData* Find( LPCTSTR lpszIdPath );

protected:
	CTaskXml m_Xml;
};

