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
	BOOL	DoOperation( CTaskData* pData );										//Add Delete Update ����
	BOOL	Save();
	/*
	�ṩ��������ģ����ϴ�ģ������ʱ��ѯ���ݣ�������������Ľӿڡ�
	ʵ�ʻ�ȡ������ʱĳһʱ�̵�XML�ļ���һ�����ա�
	�б��е�Ԫ���Ǹ�ָ�����ͣ��ڲ���������ʱ����һ����������Ҫÿ�ε��øýӿ�ʱ���ͷ�ÿһ��Ԫ�صĿռ䡣
	*/
	BOOL	GetDownLoadData( PERSIST_DATA & vecData );
	/*
	ͬ��˵��
	*/
	BOOL	GetUpLoadData( PERSIST_DATA & vecData );
	BOOL    GetFileCopyData( PERSIST_DATA & vecData );
	CTaskData* GetTaskDataCopied( LPCTSTR lpszIdPath );

protected:
	PERSIST_DATA 				m_vecData;											//�������
	static DWORD WINAPI 		_WorksThread(LPVOID lpParameter);					//�����߳�
	CString						GetXmlPath();
private:
	HANDLE						m_hWorkThread;
	DWORD						m_dwThreadID;
	CRITICAL_SECTION			m_criLock;											//��: ������� �� XML����
	BOOL						m_bStopFlag;										//�߳�ִ�б�־
	CTaskDB						m_XmlDataObj;
	HANDLE						m_hEventInit;
};

#endif