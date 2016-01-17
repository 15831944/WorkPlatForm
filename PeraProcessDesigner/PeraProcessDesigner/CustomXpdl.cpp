#include "StdAfx.h"
#include "CustomXpdl.h"
#include <Strsafe.h>
#include "SyncDownUploadFunc.h"

CCustomXpdl::CCustomXpdl(void)
{
}

CCustomXpdl::~CCustomXpdl(void)
{
}

static CString UploadFileToDMS(CString sPath)
{
	CString sFileId = UploadHprose(sPath);
	ZTools::WriteZToolsFormatLog("发起设计过程中，上传文件到DMS，源：%s，结果：%s", sPath, sFileId);
	return sFileId;
}

static CString UploadParamFileToDMS(CString sPath)
{
	CString sFileId = UploadHprose(sPath);
	ZTools::WriteZToolsFormatLog("发起设计过程中，上传文件到DMS，源：%s，结果：%s", sPath, sFileId);

	if (sFileId.IsEmpty())
	{
		return "";
	}

	std::string strTemp = (LPCTSTR)sPath;
	std::replace(strTemp.begin(), strTemp.end(), '/', '\\');
	std::string strFileName = strTemp.substr(strTemp.rfind('\\') + 1, strTemp.length() - strTemp.rfind('\\') - 1);

	return ZTools::FormatString("%s*%s", sFileId, strFileName.c_str()).c_str();
}

BOOL CCustomXpdl::UploadFileAndReplacePath()
{
	for ( map< CString, CXpdlNode* >::iterator Iter = m_mapNode.begin(); Iter != m_mapNode.end(); Iter++ )
	{
		if ( Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_COMPONENTNODE || Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_SUBFLOWNODE || Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_ROOT )
		{
			CXpdlNode* pNode = (CXpdlNode *)Iter->second;

			if ( !pNode->m_sPaths.IsEmpty() )
			{
				pNode->m_sPaths = UploadFileToDMS(pNode->m_sPaths);
			}
			if ( !pNode->m_sPrePaths.IsEmpty() )
			{
				pNode->m_sPrePaths = UploadFileToDMS(pNode->m_sPrePaths);
			}
			if ( !pNode->m_sPostPaths.IsEmpty() )
			{
				pNode->m_sPostPaths = UploadFileToDMS(pNode->m_sPostPaths);
			}
			if ( pNode->m_dwNodeType == CXpdl::XPDL_NODE_COMPONENTNODE )
			{
				CXpdlNodeComponent* pComponent = (CXpdlNodeComponent*)pNode;
				CString sManifest = pComponent->m_cManifestPath;
				if ( !sManifest.IsEmpty() )
				{
					sManifest = UploadFileToDMS(sManifest);
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
			pNode->m_sValue = UploadParamFileToDMS(pNode->m_sValue);

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}
	return TRUE;
}