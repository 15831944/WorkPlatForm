#pragma once

#include <iostream> 
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
using namespace rapidxml;

class CHistoryModelData
{
public:
	CHistoryModelData();
	~CHistoryModelData();
	void Clear(void);
public:
	CString m_strName;
	CString m_strPath;
};

class CXmlHistoryModel
{
public:
	CXmlHistoryModel(void);
	virtual ~CXmlHistoryModel(void);
	void Clear(void);
	BOOL Load( void );
	void Save( void );
	BOOL Insert( CHistoryModelData *& pData );
	BOOL InsertFront( CHistoryModelData *& pData ); //�ڿ�ͷ����Node
	BOOL ClearNode( CHistoryModelData *& pData ); //����ָ��Node
	BOOL ClearNode(); //���·�������ڵ�Node
	vector<CHistoryModelData *> m_vDatas;
private:
	xml_document<>   * m_pXmlDoc;
	char *  m_pszXmlData;
};
