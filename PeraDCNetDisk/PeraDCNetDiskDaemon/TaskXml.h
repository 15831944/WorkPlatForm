#pragma once

#include <iostream> 
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include <vector>
#include <atlstr.h>
using namespace std;
using namespace rapidxml;

class CInterfaceData_DownloadFile;

class CTaskData
{
public:
	CTaskData();
	~CTaskData();
	void Clear(void);
	BOOL Copy( CTaskData* pSrc );
	CTaskData* GetCopied(void);
	BOOL CheckValue(void);
	BOOL From( CInterfaceData_DownloadFile* pDownFile );
	BOOL SetUrl( CString sUrl );
	BOOL SetStatus( DWORD dwStatus );
	BOOL SetDelFlag( BOOL bDel );
public:
	CString m_sUserName; //�û���
	CString m_sFlowId; //����ID
	//Ψһ��ʶ
	//�����ϵ����·��
	CString m_sIdPath;
	//CString m_sPath; //���·��
	//CString m_sFileName; //�ļ���
	//�ļ�ID
	//url��ַ�Ĳ�����Ψһ
	CString m_sFileId;
	CString m_sFileMd5; //md5
	CString m_sTotalSize; //�ļ��ܳߴ�
	
	//������ļ��У�����2��������Ч������Ϊ��
	//CString m_sSrcDir;
	//CString m_SrcIdDir;

	CString m_sDataId; //�ϴ�ʱ�õ�
	CString m_sNetDiskPath; //����·��
	CString m_sLocalPath; //����·�����ӿ����ݣ�
	CString m_sRealLocalPath; //��ʵ����·�����п�����������

	//���ؾ���·��������ͬ���ģ��Զ��������󣬵�·��
	//�ļ�������ʱ�����ؾ���·��Ϊ dstDir + path��
	//�ļ�����ʱ�����ؾ���·��Ϊ dstDir + filename��
	//CString m_sLocalPath; //
	CString m_sUrl; //
	DWORD	m_dwStatus; //״ֵ̬�����ء��ϴ��������еĺ���������в�ͬ
	CString m_sOperatorFlag; //copy��upload��download
	BOOL	m_bDelete; //�Ƿ��db��ɾ��������
};


class CTaskXml/* : public CXmlBase*/
{
	friend class CTaskDB;
	friend class CPeraDataPersist;

public:
	CTaskXml(void);
	virtual ~CTaskXml(void);
	void Clear(void);
	
	BOOL Open( LPCTSTR lpszXmlPath );
	BOOL Save(void);
	BOOL Add( CTaskData* pData );
	BOOL Update( CTaskData* pData );
	BOOL Delete( CTaskData* pData );

protected:
	BOOL ParseXml( void );
	BOOL UpdateNode(xml_node<>* pNode, const CTaskData* pTaskData );
	BOOL UpdateTaskData(const xml_node<>* pNode, CTaskData*& pTaskData );

	vector< CTaskData* > m_vTasks;
	CString m_sXmlPath;
private:
	xml_document<>   * m_pDoc;
	char *  m_pszXmlData;

	xml_node<>* m_pTasksNode;
	xml_node<>* m_pDownLoadTasksNode;
	xml_node<>* m_pUpLoadTasksNode;
	xml_node<>* m_pCopyTasksNode;
};

