#include "stdafx.h"
#include "TaskXml.h"
#include "PtrVectorHeader.h"
#include "FileFunc.h"
#include "InterfaceData.h"

#include "Tools.h"

#define  MB_TO_UTF8(strMB)    ZTools::MBToUTF8(string(strMB)).c_str()
#define  UTF8_TO_MB(strUTF8)  ZTools::UTF8ToMB(string(strUTF8)).c_str()

CTaskData::CTaskData()
{
	Clear();
}

CTaskData::~CTaskData()
{
	Clear();
}

void CTaskData::Clear( void )
{
	m_sUserName.Empty();
	m_sFlowId.Empty();
	m_sIdPath.Empty();
	m_sFileId.Empty();
	m_sFileMd5.Empty();
	m_sTotalSize.Empty();
	m_sLocalPath.Empty();
	m_sUrl.Empty();
	m_dwStatus = 0;
	m_sOperatorFlag.Empty();
	m_bDelete = FALSE;
	m_sDataId.Empty();
	m_sNetDiskPath.Empty();
	m_sRealLocalPath.Empty();
}

BOOL CTaskData::Copy( CTaskData* pSrc )
{
	if ( pSrc == NULL ) return FALSE;

	m_sUserName.Format("%s", pSrc->m_sUserName );
	m_sFlowId.Format("%s", pSrc->m_sFlowId );
	m_sIdPath.Format("%s", pSrc->m_sIdPath );
	m_sFileId.Format("%s", pSrc->m_sFileId );
	m_sFileMd5.Format("%s", pSrc->m_sFileMd5 );
	m_sTotalSize.Format("%s", pSrc->m_sTotalSize );
	m_sLocalPath.Format("%s", pSrc->m_sLocalPath );
	m_sUrl.Format("%s", pSrc->m_sUrl );
	m_dwStatus		= pSrc->m_dwStatus;
	m_sOperatorFlag.Format("%s", pSrc->m_sOperatorFlag );
	m_bDelete		= pSrc->m_bDelete;
	m_sDataId.Format("%s", pSrc->m_sDataId );
	m_sNetDiskPath.Format("%s", pSrc->m_sNetDiskPath );
	m_sRealLocalPath.Format("%s", pSrc->m_sRealLocalPath );

	return TRUE;
}

CTaskData* CTaskData::GetCopied(void)
{
	CTaskData* pNew = new CTaskData();
	pNew->Copy( this );
	return pNew;
}

BOOL CTaskData::CheckValue(void)
{
	return TRUE;

	if ( m_sUserName.IsEmpty() ) return FALSE;
	if ( m_sFlowId.IsEmpty() ) return FALSE;
	if ( m_sIdPath.IsEmpty() ) return FALSE;
	if ( m_sFileId.IsEmpty() ) return FALSE;
	if ( m_sFileMd5.IsEmpty() ) return FALSE;
	if ( m_sTotalSize.IsEmpty() ) return FALSE;
	if ( m_sLocalPath.IsEmpty() ) return FALSE;
	//if ( m_sUrl.IsEmpty() ) return FALSE;
	if ( m_sOperatorFlag.IsEmpty() ) return FALSE;
	return TRUE;
}

BOOL CTaskData::From( CInterfaceData_DownloadFile* pDownFile )
{
	this->Clear();
	this->m_sUserName = pDownFile->m_sUserName;
	this->m_sTotalSize = pDownFile->m_sFileSize;
	this->m_sLocalPath = pDownFile->m_sLocalPath;
	this->m_sNetDiskPath = pDownFile->m_sNetDiskPath;
	this->m_sIdPath = pDownFile->m_sIdPath;
	this->m_sFlowId = pDownFile->m_sFlowId;
	this->m_sFileMd5 = pDownFile->m_sFileMd5;
	this->m_sFileId = pDownFile->m_sFileId;
	this->m_bDelete = FALSE;
	this->m_sOperatorFlag = "DownLoad";
	return TRUE;
}

BOOL CTaskData::SetUrl( CString sUrl )
{
	m_sUrl = sUrl;
	return TRUE;
}

BOOL CTaskData::SetStatus( DWORD dwStatus )
{
	m_dwStatus = dwStatus;
	return TRUE;
}

BOOL CTaskData::SetDelFlag( BOOL bDel )
{
	m_bDelete = bDel;
	return TRUE;
}

CTaskXml::CTaskXml(void)
{
	m_pTasksNode = NULL;
	m_pDownLoadTasksNode = NULL;
	m_pUpLoadTasksNode = NULL;
	m_pCopyTasksNode = NULL;
	m_pDoc = NULL;
}

CTaskXml::~CTaskXml( void )
{
	if (m_pDoc)
	{
		delete m_pDoc;
		m_pDoc = NULL;
	}

	if (m_pszXmlData)
	{
		free(m_pszXmlData);
		m_pszXmlData = NULL;
	}
	Clear();
}

void CTaskXml::Clear( void )
{
	m_sXmlPath = "";
	m_pTasksNode = NULL;
	m_pDownLoadTasksNode = NULL;
	m_pUpLoadTasksNode = NULL;
	m_pCopyTasksNode = NULL;
	PtrVectorClear( m_vTasks );
}

BOOL CTaskXml::Open( LPCTSTR lpszXmlPath )
{
	if (m_pDoc)
	{
		delete m_pDoc;
		m_pDoc = NULL;
	}

	if (m_pszXmlData)
	{
		free(m_pszXmlData);
		m_pszXmlData = NULL;
	}

	Clear();
	if (!IsAccessibleFile(lpszXmlPath))
	{
		return FALSE;
	}
	m_sXmlPath = lpszXmlPath;

	file<> fdoc((LPCTSTR)m_sXmlPath);
	m_pszXmlData = _strdup(fdoc.data());
	m_pDoc = new xml_document<>;
	rapidxml::xml_node<>* rot = m_pDoc->allocate_node(
		rapidxml::node_pi,m_pDoc->allocate_string("xml version='1.0' encoding='utf-8'"));

	m_pDoc->parse<0>(m_pszXmlData);
	m_pDoc->insert_node(m_pDoc->first_node(),rot);

	m_pTasksNode =  m_pDoc->first_node("tasks");
	if (m_pTasksNode)
		m_pDownLoadTasksNode = m_pTasksNode->first_node("download");

	if (m_pTasksNode)
		m_pUpLoadTasksNode = m_pTasksNode->first_node("upload");

	if (m_pTasksNode)
		m_pCopyTasksNode = m_pTasksNode->first_node("copy");

	if ( m_pTasksNode == NULL 
		|| m_pDownLoadTasksNode == NULL
		|| m_pUpLoadTasksNode == NULL
		|| m_pCopyTasksNode == NULL)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

BOOL CTaskXml::Save( void )
{
	if ( m_sXmlPath.IsEmpty() ) return FALSE;

	if ( m_pDoc == NULL
		|| m_pTasksNode == NULL 
		|| m_pDownLoadTasksNode == NULL
		|| m_pUpLoadTasksNode == NULL
		|| m_pCopyTasksNode == NULL)
	{
		return FALSE;
	}

	std::ofstream out(m_sXmlPath);  
	out<<*m_pDoc; // 保存xml
	out.close();

	return TRUE;
}

BOOL CTaskXml::ParseXml( void )
{
	PtrVectorClear( m_vTasks );
	if ( m_pDoc == NULL
		|| m_pTasksNode == NULL 
		|| m_pDownLoadTasksNode == NULL
		|| m_pUpLoadTasksNode == NULL
		|| m_pCopyTasksNode == NULL)
	{
		return FALSE;
	}

	CTaskData* pTaskData = NULL;

	xml_node<>* pTaskNode = m_pDownLoadTasksNode->first_node("task");
	while(pTaskNode != NULL)
	{
		pTaskData = new CTaskData();

		if ( !UpdateTaskData( pTaskNode, pTaskData ) )
		{
			delete pTaskData;
			pTaskData = NULL;
			continue;
		}
		m_vTasks.push_back(pTaskData);
	}

	pTaskNode = m_pUpLoadTasksNode->first_node("upload");
	while(pTaskNode != NULL)
	{
		pTaskData = new CTaskData();

		if ( !UpdateTaskData( pTaskNode, pTaskData ) )
		{
			delete pTaskData;
			pTaskData = NULL;
			continue;
		}
		m_vTasks.push_back(pTaskData);
	}

	pTaskNode = m_pCopyTasksNode->first_node("copy");
	while(pTaskNode != NULL)
	{
		pTaskData = new CTaskData();

		if ( !UpdateTaskData( pTaskNode, pTaskData ) )
		{
			delete pTaskData;
			pTaskData = NULL;
			continue;
		}
		m_vTasks.push_back(pTaskData);
	}
	return TRUE;
}

BOOL CTaskXml::UpdateNode(xml_node<>* pNode, const CTaskData* pTaskData )
{
	if ( pNode == NULL ) return FALSE;
	if ( pTaskData == NULL ) return FALSE;

	rapidxml::xml_attribute<char> * attr = NULL;

	// 增加属性
	attr = m_pDoc->allocate_attribute("userName",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sUserName)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("flowId",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sFlowId)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("idPath",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sIdPath)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("fileId",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sFileId)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("fileSize",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sTotalSize)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("fileMd5",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sFileMd5)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("localPath",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sLocalPath)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("url",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sUrl)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("realLocalPath",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sRealLocalPath)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("netDiskPath",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sNetDiskPath)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("dataId",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sDataId)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	attr = m_pDoc->allocate_attribute("operatorFlag",m_pDoc->allocate_string(MB_TO_UTF8(pTaskData->m_sOperatorFlag)));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	CString str;
	str.Format("%d",pTaskData->m_dwStatus);
	attr = m_pDoc->allocate_attribute("status",m_pDoc->allocate_string(str));
	if (attr==NULL) return FALSE;
	pNode->append_attribute(attr);

	return TRUE;
}


BOOL CTaskXml::UpdateTaskData(const xml_node<>*  pNode, CTaskData*& pTaskData )
{
	if ( pNode == NULL ) return FALSE;
	if ( pTaskData == NULL ) return FALSE;
	
	rapidxml::xml_attribute<char> * attr = pNode->first_attribute("userName");
	if (attr!=NULL) pTaskData->m_sUserName = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("flowId");
	if (attr!=NULL) pTaskData->m_sFlowId = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("idPath");
	if (attr!=NULL) pTaskData->m_sIdPath = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("fileId");
	if (attr!=NULL) pTaskData->m_sFileId = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("fileSize");
	if (attr!=NULL) pTaskData->m_sTotalSize = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("fileMd5");
	if (attr!=NULL) pTaskData->m_sFileMd5 = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("localPath");
	if (attr!=NULL) pTaskData->m_sLocalPath = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("url");
	if (attr!=NULL) pTaskData->m_sUrl = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("realLocalPath");
	if (attr!=NULL) pTaskData->m_sRealLocalPath = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("netDiskPath");
	if (attr!=NULL) pTaskData->m_sNetDiskPath = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("dataId");
	if (attr!=NULL) pTaskData->m_sDataId = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("operatorFlag");
	if (attr!=NULL) pTaskData->m_sOperatorFlag = UTF8_TO_MB(attr->value());

	attr = pNode->first_attribute("status");
	if (attr!=NULL) pTaskData->m_dwStatus = atoi(attr->value());
	
	return TRUE;
}

BOOL CTaskXml::Add( CTaskData* pData )
{
	if ( pData == NULL ) return FALSE;

	if ( m_pDoc == NULL
		|| m_pTasksNode == NULL 
		|| m_pDownLoadTasksNode == NULL
		|| m_pUpLoadTasksNode == NULL
		|| m_pCopyTasksNode == NULL)
	{
		return FALSE;
	}

	// 创建新节点
	xml_node<>* pTaskNode = m_pDoc->allocate_node(node_element,"task"); 
	if ( pTaskNode == NULL ) return FALSE;

	if ( !UpdateNode( pTaskNode, pData ) )
		return FALSE;

	// 把节点增加到对应子项下面
	if ( pData->m_sOperatorFlag.CompareNoCase("download") == 0 )
	{
		m_pDownLoadTasksNode->append_node(pTaskNode);
	}
	else if ( pData->m_sOperatorFlag.CompareNoCase("upload") == 0 )
	{
		m_pUpLoadTasksNode->append_node(pTaskNode);
	}
	else if ( pData->m_sOperatorFlag.CompareNoCase("copy") == 0 )
	{
		m_pCopyTasksNode->append_node(pTaskNode);
	}
	m_vTasks.push_back(pData);
	return TRUE;
}

BOOL CTaskXml::Update( CTaskData* pData )
{
	if ( pData == NULL ) return FALSE;
	if ( m_pDoc == NULL
		|| m_pTasksNode == NULL 
		|| m_pDownLoadTasksNode == NULL
		|| m_pUpLoadTasksNode == NULL
		|| m_pCopyTasksNode == NULL)
	{
		return FALSE;
	}

	xml_node<>* pTaskNode = NULL;
	xml_node<>* pTaskNodeParent = NULL;
	CString strAttVal;
	if ( pData->m_sOperatorFlag.CompareNoCase("download") == 0 )
	{
		pTaskNodeParent = m_pDownLoadTasksNode;
	}
	else if ( pData->m_sOperatorFlag.CompareNoCase("upload") == 0 )
	{
		pTaskNodeParent = m_pUpLoadTasksNode;
	}
	else if ( pData->m_sOperatorFlag.CompareNoCase("copy") == 0 )
	{
		pTaskNodeParent = m_pCopyTasksNode;
	}

	if (pTaskNodeParent)
	{
		pTaskNode   = pTaskNodeParent->first_node("task");
		while(pTaskNode)
		{
			rapidxml::xml_attribute<char> * attr = pTaskNode->first_attribute("idPath");
			if (attr!=NULL)
			{
				strAttVal = UTF8_TO_MB(attr->value());
				if (strAttVal.Compare(pData->m_sIdPath)==0)
					break;
			}
			pTaskNode = pTaskNode->next_sibling();
		}
	}

	if (pTaskNode!=NULL)
	{
		pTaskNodeParent->remove_node(pTaskNode); // rapidxml 更新节点时先删除已经有的该节点，重新添加
		pTaskNode = NULL;
	}
	else
		return FALSE;

	pTaskNode = m_pDoc->allocate_node(node_element,"task"); 
	if ( !UpdateNode( pTaskNode, pData ) )
	{
		return FALSE;
	}

	pTaskNodeParent->insert_node(pTaskNodeParent->first_node("task"),pTaskNode);
	return TRUE;
}

BOOL CTaskXml::Delete( CTaskData* pData )
{
	if ( pData == NULL ) return FALSE;
	if ( m_pDoc == NULL
		|| m_pTasksNode == NULL 
		|| m_pDownLoadTasksNode == NULL
		|| m_pUpLoadTasksNode == NULL
		|| m_pCopyTasksNode == NULL)
	{
		return FALSE;
	}

	xml_node<>* pTaskNode = NULL;
	xml_node<>* pTaskNodeParent = NULL;
	CString strAttVal;
	if ( pData->m_sOperatorFlag.CompareNoCase("download") == 0 )
	{
		pTaskNodeParent = m_pDownLoadTasksNode;
	}
	else if ( pData->m_sOperatorFlag.CompareNoCase("upload") == 0 )
	{
		pTaskNodeParent = m_pUpLoadTasksNode;
	}
	else if ( pData->m_sOperatorFlag.CompareNoCase("copy") == 0 )
	{
		pTaskNodeParent = m_pCopyTasksNode;
	}

	if (pTaskNodeParent)
	{
		pTaskNode   = pTaskNodeParent->first_node("task");
		while(pTaskNode)
		{
			rapidxml::xml_attribute<char> * attr = pTaskNode->first_attribute("idPath");
			if (attr!=NULL)
			{
				strAttVal = UTF8_TO_MB(attr->value());
				if (strAttVal.Compare(pData->m_sIdPath)==0)
					break;
			}
			pTaskNode = pTaskNode->next_sibling();
		}
	}

	if (pTaskNode!=NULL)
	{
		pTaskNodeParent->remove_node(pTaskNode); // rapidxml 删除已经有的该节点
		pTaskNode = NULL;
		PtrVectorRemove( m_vTasks, pData );
		return TRUE;
	}
	else
		return FALSE;

}