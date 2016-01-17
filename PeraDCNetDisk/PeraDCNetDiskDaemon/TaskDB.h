#pragma once
#include "TaskXml.h"

class CTaskDB
{
	friend class CPeraDataPersist;
public:
	CTaskDB(void);
	virtual ~CTaskDB(void);
	//��ʼ��
	//��xml
	//ȡ�� pDoc��pTasksNode ָ��
	BOOL Init( LPCTSTR lpszXmlPath );
	//�ͷ�
	void Release(void);
	//����
	BOOL Save(void);
	//��ĩβ����µ��������ݣ����������ͬ��idPath��ʧ��
	BOOL Add( CTaskData* pNewData );
	//��ָ��λ���������������
	BOOL InsertAfter( const CTaskData* pNewData );
	//ɾ��һ������
	BOOL Delete( LPCTSTR lpszIdPath );
	//������ͬ idpath ������
	BOOL Update( CTaskData* pNewData );
	//ȡ��һ���������ݵĸ���
	CTaskData* GetTaskDataCopied( LPCTSTR lpszIdPath );
	//�ж����������Ƿ����
	BOOL CheckExisted( LPCTSTR lpszIdPath );

protected:
	//������������
	CTaskData* Find( LPCTSTR lpszIdPath );

protected:
	CTaskXml m_Xml;
};

