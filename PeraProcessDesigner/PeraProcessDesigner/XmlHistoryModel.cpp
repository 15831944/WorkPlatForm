
#include "StdAfx.h"
#pragma warning(disable:4244)

#include "XmlHistoryModel.h"
#include "PtrVectorHeader.h"
#include "Tools.h"

#define  MB_TO_UTF8(strMB)    ZTools::MBToUTF8(string(strMB)).c_str()
#define  UTF8_TO_MB(strUTF8)  ZTools::UTF8ToMB(string(strUTF8)).c_str()

CHistoryModelData::CHistoryModelData()
{
	m_strName = "";
	m_strPath = "";
}

CHistoryModelData::~CHistoryModelData()
{
	Clear();
}

void CHistoryModelData::Clear( void )
{
	m_strName = "";
	m_strPath = "";
}


//////////////////////////////////////////////////////////////////////////


CXmlHistoryModel::CXmlHistoryModel(void)
{
	m_pszXmlData = NULL;
	m_pXmlDoc = NULL;
}

CXmlHistoryModel::~CXmlHistoryModel(void)
{
	if (m_pszXmlData)
	{
		free(m_pszXmlData);
		m_pszXmlData = NULL;
	}
	if (m_pXmlDoc)
	{
		delete m_pXmlDoc;
		m_pXmlDoc = NULL;
	}
	Clear();
}

void CXmlHistoryModel::Clear( void )
{
	PtrVectorClear( m_vDatas );
}

BOOL CXmlHistoryModel::Load( void )
{
	if (m_pXmlDoc)
	{
		delete m_pXmlDoc;
		m_pXmlDoc = NULL;
	}

	if (m_pszXmlData)
	{
		free(m_pszXmlData);
		m_pszXmlData = NULL;
	}

	Clear();
	CString strHistoryPath = GetExecDir() + "\\HistoryModel.xml";
	if ( !IsAccessibleFile( strHistoryPath ) )
	{
		return FALSE;
	}

	

	//取得xml文档对象
	file<> fdoc((LPCTSTR)strHistoryPath);
	m_pszXmlData = _strdup(fdoc.data());
	m_pXmlDoc = new xml_document<>;
	 rapidxml::xml_node<>* rot = m_pXmlDoc->allocate_node(
		 rapidxml::node_pi,m_pXmlDoc->allocate_string("xml version='1.0' encoding='utf-8'"));

	m_pXmlDoc->parse<0>(m_pszXmlData);
	m_pXmlDoc->insert_node(m_pXmlDoc->first_node(),rot);

	xml_node<>* pNodeHistory = m_pXmlDoc->first_node("historymodel");
	
	if (pNodeHistory)
	{
		xml_node<>* pNodeModel = pNodeHistory->first_node("model");
		CHistoryModelData * pData = NULL;
		while(pNodeModel)
		{
			pData = new CHistoryModelData();
			rapidxml::xml_attribute<char> * attr = pNodeModel->first_attribute("name");
			if (attr!=NULL) pData->m_strName = UTF8_TO_MB(attr->value());

			attr = pNodeModel->first_attribute("path");
			if (attr!=NULL) pData->m_strPath = UTF8_TO_MB(attr->value());

			m_vDatas.push_back(pData);
			pNodeModel =  pNodeModel->next_sibling();
		}
	}

	return TRUE;
}

BOOL CXmlHistoryModel::Insert( CHistoryModelData *& pData )
{
	if ( m_pXmlDoc == NULL ) goto Label_CXmlHistoryModel_Insert_Failed;
	if ( pData == NULL ) goto Label_CXmlHistoryModel_Insert_Failed;
	if ( pData->m_strName.IsEmpty() ) goto Label_CXmlHistoryModel_Insert_Failed;
	if ( !IsAccessibleFile( pData->m_strPath ) ) goto Label_CXmlHistoryModel_Insert_Failed;

	for ( int i=0; i<(int)m_vDatas.size(); i++ )
	{
		if ( m_vDatas[i]->m_strPath == pData->m_strPath ) goto Label_CXmlHistoryModel_Insert_Failed;
	}

	xml_node<>* pNodeHistory = m_pXmlDoc->first_node("historymodel");
	if (pNodeHistory)
	{
		//创建节点
		xml_node<>* pNodeModel = m_pXmlDoc->allocate_node(node_element,"model"); 
		rapidxml::xml_attribute<char> * attr = NULL;

		// 增加属性
		attr = m_pXmlDoc->allocate_attribute("name",m_pXmlDoc->allocate_string( MB_TO_UTF8(pData->m_strName)));
		if (attr!=NULL) pNodeModel->append_attribute(attr);

		attr = m_pXmlDoc->allocate_attribute("path",m_pXmlDoc->allocate_string(MB_TO_UTF8(pData->m_strPath)));
		if (attr!=NULL) pNodeModel->append_attribute(attr);

		pNodeHistory->append_node(pNodeModel); // 增加节点
		m_vDatas.push_back( pData );
		return TRUE;
	}

Label_CXmlHistoryModel_Insert_Failed:
	delete pData;
	pData = NULL;
	return FALSE;
}

BOOL CXmlHistoryModel::InsertFront( CHistoryModelData *& pData )
{
	if ( m_pXmlDoc == NULL ) goto Label_CXmlHistoryModel_InsertFront_Failed;
	if ( pData == NULL ) goto Label_CXmlHistoryModel_InsertFront_Failed;
	if ( pData->m_strName.IsEmpty() ) goto Label_CXmlHistoryModel_InsertFront_Failed;
	if ( !IsAccessibleFile( pData->m_strPath ) ) goto Label_CXmlHistoryModel_InsertFront_Failed;

	for ( int i=0; i<(int)m_vDatas.size(); i++ )
	{
		if ( m_vDatas[i]->m_strPath == pData->m_strPath ) goto Label_CXmlHistoryModel_InsertFront_Failed;
	}

	xml_node<>* pNodeHistory = m_pXmlDoc->first_node("historymodel");
	if (pNodeHistory == NULL) goto Label_CXmlHistoryModel_InsertFront_Failed;

	xml_node<>* pNodeModelFirst = pNodeHistory->first_node("model");
	//创建节点
	xml_node<>* pNodeModel = m_pXmlDoc->allocate_node(node_element,"model"); 
	rapidxml::xml_attribute<char> * attr = NULL;
	// 增加属性
	attr =  m_pXmlDoc->allocate_attribute("name", m_pXmlDoc->allocate_string( MB_TO_UTF8(pData->m_strName)));
	if (attr!=NULL) pNodeModel->append_attribute(attr);

	attr =  m_pXmlDoc->allocate_attribute("path", m_pXmlDoc->allocate_string(MB_TO_UTF8(pData->m_strPath)));
	if (attr!=NULL) pNodeModel->append_attribute(attr);

	pNodeHistory->insert_node(pNodeModelFirst,pNodeModel); // 增加节点

	m_vDatas.insert(m_vDatas.begin(), pData);

	{//数量限制为20
		int nCount = m_vDatas.size();
		if(nCount > 20)
		{
		
			CHistoryModelData* pHMD = m_vDatas[m_vDatas.size()-1];	
			delete pHMD;
			pHMD = NULL;
			m_vDatas.pop_back();
			pNodeHistory->remove_last_node();
		}
	}

	return TRUE;

Label_CXmlHistoryModel_InsertFront_Failed:
	delete pData;
	pData = NULL;
	return FALSE;
}

BOOL CXmlHistoryModel::ClearNode( CHistoryModelData *& pData )
{
	std::vector<CHistoryModelData *>::iterator itr = m_vDatas.begin();
	if (m_pXmlDoc==NULL) goto Label_CXmlHistoryModel_Clear_Failed;

	xml_node<>* pNodeHistory = m_pXmlDoc->first_node("historymodel");
	if (pNodeHistory == NULL) goto Label_CXmlHistoryModel_Clear_Failed;

	xml_node<>* pNodeModel = pNodeHistory->first_node("model");
	if (pNodeModel == NULL) return TRUE;

	
	for (itr;itr!= m_vDatas.end();)
	{
		if ((*itr)->m_strPath == pData->m_strPath ||  !IsAccessibleFile( (*itr)->m_strPath))
		{//路径相同或者文件失效
			
			xml_node<>* pNodeModelT = pNodeModel->next_sibling();
			pNodeHistory->remove_node(pNodeModel);
			pNodeModel = pNodeModelT;

			CHistoryModelData* pHMD = *itr;
			itr = m_vDatas.erase(itr);			
			delete pHMD;
			pHMD = NULL;
		}
		else
		{
			itr ++;
			pNodeModel = pNodeModel->next_sibling();
		}
	}

	return TRUE;

Label_CXmlHistoryModel_Clear_Failed:
	delete pData;
	pData = NULL;
	return FALSE;
}

BOOL CXmlHistoryModel::ClearNode()
{
	
	std::vector<CHistoryModelData *>::iterator itr = m_vDatas.begin();
	if (m_pXmlDoc==NULL) goto Label_CXmlHistoryModel_Clear2_Failed;

	xml_node<>* pNodeHistory = m_pXmlDoc->first_node("historymodel");
	if (pNodeHistory == NULL) goto Label_CXmlHistoryModel_Clear2_Failed;

	xml_node<>* pNodeModel = pNodeHistory->first_node("model");
	if (pNodeModel == NULL) goto Label_CXmlHistoryModel_Clear2_Failed;

	for (itr;itr != m_vDatas.end();)
	{
		if ( !IsAccessibleFile( (*itr)->m_strPath))
		{//文件失效

			xml_node<>* pNodeModelT = pNodeModel->next_sibling();
			pNodeHistory->remove_node(pNodeModel);
			pNodeModel = pNodeModelT;
			
			CHistoryModelData* pHMD = *itr;
			itr = m_vDatas.erase(itr);
			delete pHMD;
			pHMD = NULL;
		}
		else
		{
			itr ++;
			pNodeModel = pNodeModel->next_sibling();
		}
	}

	return TRUE;

Label_CXmlHistoryModel_Clear2_Failed:
	return FALSE;
}

void CXmlHistoryModel::Save( void )
{
	if (m_pXmlDoc==NULL)
		return;

	CString strHistoryPath = GetExecDir() + "\\HistoryModel.xml";
	std::ofstream out(strHistoryPath);  
	out<<*m_pXmlDoc; // 保存xml
	out.close();

	if (m_pszXmlData) 
	   free(m_pszXmlData);

	m_pszXmlData = NULL;
}
