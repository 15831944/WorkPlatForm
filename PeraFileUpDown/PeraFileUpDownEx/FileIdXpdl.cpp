#include "StdAfx.h"
#include "FileIdXpdl.h"
#include <Strsafe.h>

CFileIdXpdl::CFileIdXpdl(void)
{
}


CFileIdXpdl::~CFileIdXpdl(void)
{
}

static std::string GetFileId(Json::Value jsonResult, std::string strPath)
{
	Json::Value jsonAllFiles = jsonResult["allFiles"];
	std::string strReturn = jsonAllFiles[strPath].asString();
	return strReturn;
}

static std::string GetParamFileId(Json::Value jsonResult, std::string strPath)
{
	Json::Value jsonAllFiles = jsonResult["allFiles"];
	std::string strReturn = jsonAllFiles[strPath].asString();
	if (strReturn.empty())
	{
		return "";
	}	

	std::string strTemp(strPath);
	std::replace(strTemp.begin(), strTemp.end(), '/', '\\');
	std::string strFileName = strTemp.substr(strTemp.rfind('\\') + 1, strTemp.length() - strTemp.rfind('\\') - 1);

	return ZTools::FormatString("%s*%s", strReturn.c_str(), strFileName.c_str());
}

BOOL CFileIdXpdl::ReplacePathToFileId(Json::Value jsonResult)
{
	for ( map< CString, CXpdlNode* >::iterator Iter = m_mapNode.begin(); Iter != m_mapNode.end(); Iter++ )
	{
		if ( Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_COMPONENTNODE || Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_SUBFLOWNODE || Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_ROOT )
		{
			CXpdlNode* pNode = (CXpdlNode *)Iter->second;

			if ( !pNode->m_sPaths.IsEmpty() )
			{
				pNode->m_sPaths = GetFileId(jsonResult, (LPCTSTR)pNode->m_sPaths).c_str();
			}
			if ( !pNode->m_sPrePaths.IsEmpty() )
			{
				pNode->m_sPrePaths = GetFileId(jsonResult, (LPCTSTR)pNode->m_sPrePaths).c_str();
			}
			if ( !pNode->m_sPostPaths.IsEmpty() )
			{
				pNode->m_sPostPaths = GetFileId(jsonResult, (LPCTSTR)pNode->m_sPostPaths).c_str();
			}
			if ( pNode->m_dwNodeType == CXpdl::XPDL_NODE_COMPONENTNODE )
			{
				CXpdlNodeComponent* pComponent = (CXpdlNodeComponent*)pNode;
				CString sManifest = pComponent->m_cManifestPath;
				if ( !sManifest.IsEmpty() )
				{
					sManifest = GetFileId(jsonResult, (LPCTSTR)sManifest).c_str();
					StringCbCopy( pComponent->m_cManifestPath, MAX_PATH*2, sManifest );
				}
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}

	for ( map< CString, CXpdlNode* >::iterator It = m_mapParam.begin(); It != m_mapParam.end(); It++ )
	{
		CXpdlNodeParam* pNode = ( CXpdlNodeParam* )It->second;

		if ( pNode->m_sDataType == "REFERENCE" && !pNode->m_sValue.IsEmpty() )
		{
			pNode->m_sValue = GetParamFileId(jsonResult, (LPCTSTR)pNode->m_sValue).c_str();

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}
	return TRUE;
}