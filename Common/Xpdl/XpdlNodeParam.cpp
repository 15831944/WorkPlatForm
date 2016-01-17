#include "StdAfx.h"
#include "XpdlNodeParam.h"


CXpdlNodeParam::CXpdlNodeParam(void)
{
	m_bIsArray = FALSE;
	m_bIsSaveInModel = FALSE;
	m_bIsDynamicArray = FALSE;
	m_bMultifiles = FALSE;
	m_bReadOnly = FALSE;
}


CXpdlNodeParam::~CXpdlNodeParam(void)
{
}

BOOL CXpdlNodeParam::Copy( CXpdlNodeParam* pSrcNode )
{
	//m_bIsArray = pSrcNode->m_bIsArray;
	m_sDataType = pSrcNode->m_sDataType;
	//m_sDirection = pSrcNode->m_sDirection;
	m_sValue = pSrcNode->m_sValue;
	//m_sUnit = pSrcNode->m_sUnit;
	m_sArrayUpperIndex = pSrcNode->m_sArrayUpperIndex;
	m_sArrayLowerIndex = pSrcNode->m_sArrayLowerIndex;
	//m_sFormat = pSrcNode->m_sFormat;
	//m_sState = pSrcNode->m_sState;
	//m_sGroup = pSrcNode->m_sGroup;
	//m_bMultifiles = pSrcNode->m_bMultifiles;
	//m_bIsSaveInModel = pSrcNode->m_bIsSaveInModel;
	//m_bIsDynamicArray = pSrcNode->m_bIsDynamicArray;
	//m_sListName = pSrcNode->m_sListName;
	return TRUE;
}
