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
	CString m_sUserName; //用户名
	CString m_sFlowId; //流程ID
	//唯一标识
	//网盘上的相对路径
	CString m_sIdPath;
	//CString m_sPath; //相对路径
	//CString m_sFileName; //文件名
	//文件ID
	//url地址的参数，唯一
	CString m_sFileId;
	CString m_sFileMd5; //md5
	CString m_sTotalSize; //文件总尺寸
	
	//如果是文件夹，以下2个参数有效，否则为空
	//CString m_sSrcDir;
	//CString m_SrcIdDir;

	CString m_sDataId; //上传时用到
	CString m_sNetDiskPath; //网盘路径
	CString m_sLocalPath; //本地路径（接口数据）
	CString m_sRealLocalPath; //真实下载路径，有可能重命名。

	//本地绝对路径，如有同名的，自动重命名后，的路径
	//文件夹下载时，本地绝对路径为 dstDir + path。
	//文件下载时，本地绝对路径为 dstDir + filename。
	//CString m_sLocalPath; //
	CString m_sUrl; //
	DWORD	m_dwStatus; //状态值，下载、上传、拷贝中的含义可能略有不同
	CString m_sOperatorFlag; //copy、upload、download
	BOOL	m_bDelete; //是否从db中删除本任务
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

