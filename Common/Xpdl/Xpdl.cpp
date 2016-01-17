#include "StdAfx.h"
#include "Xpdl.h"
#pragma comment(lib,"shlwapi.lib")

#include "stdlib.h"
#include <iostream>
#include <string>
#include "FileFunc.h"
#include "FileOperate.h"
#include <afxwin.h>
#include <Strsafe.h>
#include <Dbghelp.h>

#pragma warning( disable : 4995 )

#define ASC_UTF8_CONVERT	1

CXpdl::CXpdl(void)
{
	m_pRootNode = NULL;
	m_pBuffer = NULL;
}

CXpdl::~CXpdl(void)
{
	Clear();
}

void CXpdl::Clear()
{
	for( map< CString, CXpdlNode* >::iterator it = m_mapNode.begin(); it != m_mapNode.end(); )
	{
		CXpdlNode* pNode = it->second;

		it = m_mapNode.erase( it );

		delete pNode;

		pNode = NULL;
	}

	for ( map<CString, CXpdlNode* >::iterator iterMap = m_mapParam.begin(); iterMap != m_mapParam.end(); )
	{
		CXpdlNode* pNode = iterMap->second;

		iterMap = m_mapParam.erase( iterMap );

		delete pNode;

		pNode = NULL;
	}
	if ( m_pBuffer != NULL )
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}

BOOL CXpdl::Load( LPCTSTR lpszFilePath )
{
	if ( lpszFilePath == NULL )
	{
		return FALSE;
	}
	if ( !PathFileExists( lpszFilePath ) )
	{
		ZTools::WriteZToolsFormatLog("------Xpdl Load[%s]---------------\n", lpszFilePath );

		AfxMessageBox( "传入的xpdl文件不存在" ) ;

		return FALSE;
	}

	m_sFilePath = lpszFilePath;

	FILE *fp=NULL;

	fp=fopen( m_sFilePath ,"rb");

	if ( fp == NULL )
	{
		return FALSE;
	}
	fseek( fp, 0, SEEK_END );

	DWORD dwLength = ftell( fp );

	fseek( fp, 0, SEEK_SET );

	dwLength += 64;

	m_pBuffer = new char[ dwLength ];

	ZeroMemory( m_pBuffer, dwLength );

	fread( m_pBuffer, dwLength, 1, fp );

	fclose(fp);

	m_doc.parse< parse_full >( m_pBuffer );

	return TRUE;
}

// &lt; < 小于 
// &gt; > 大于 
// &amp; & 和号 
// &apos; ' 省略号 
// &quot; " 引号 

//字符串转义
CString CXpdl::StringEscape(CString sSrc)
{
	if (sSrc.IsEmpty())
	{
		return sSrc;
	}

	char* pSrc = (LPTSTR)(LPCTSTR)sSrc;
	char* pSrcEnd = pSrc + sSrc.GetLength();

	int nAddChars = 0;
	for (; pSrc < pSrcEnd; pSrc++)
	{
		switch (*pSrc)
		{
		case '<':
			nAddChars += 3;
			break;
		case '>': 
			nAddChars += 3;
			break;
		case '\'': 
			nAddChars += 5;
			break;
		case '"': 
			nAddChars += 5;
			break;
		case '&': 
			nAddChars += 4;
			break;
		default:	
			break;
		}
	}
	pSrc = (LPTSTR)(LPCTSTR)sSrc;
	std::vector<char> vDest(sSrc.GetLength() + nAddChars + 1, '\0');

	for (int i = 0; pSrc < pSrcEnd;)
	{
		switch (*pSrc)
		{
		case '<':
			vDest[i++] = '&'; vDest[i++] ='l'; vDest[i++] ='t'; vDest[i++] =';';
			break;
		case '>': 
			vDest[i++] ='&'; vDest[i++] ='g'; vDest[i++] ='t'; vDest[i++] =';';
			break;
		case '\'': 
			vDest[i++] ='&'; vDest[i++] ='a'; vDest[i++] ='p'; vDest[i++] ='o'; vDest[i++] ='s'; vDest[i++] =';';
			break;
		case '"': 
			vDest[i++] ='&'; vDest[i++] ='q'; vDest[i++] ='u'; vDest[i++] ='o'; vDest[i++] ='t'; vDest[i++] =';';
			break;
		case '&': 
			vDest[i++] ='&'; vDest[i++] ='a'; vDest[i++] ='m'; vDest[i++] ='p'; vDest[i++] =';'; 
			break;
		default:
			vDest[i++] = *pSrc;			
		}
		pSrc++;
	}
	sSrc = &vDest[0];
	return sSrc;
}
//字符串反转义
CString CXpdl::StringUnEscape(CString sSrc)
{
	if (sSrc.IsEmpty())
	{
		return sSrc;
	}

	char* pSrc = (LPTSTR)(LPCTSTR)sSrc;
	char* pSrcEnd = pSrc + sSrc.GetLength();
	std::vector<char> vDest(sSrc.GetLength()+1, '\0');

	for (int i = 0; pSrc < pSrcEnd;)
	{
		if (*pSrc == '&')
		{
			if (pSrc+4 < pSrcEnd && *(pSrc+1) == 'a' && *(pSrc+2) == 'm' && *(pSrc+3) == 'p' && *(pSrc+4) == ';')
			{
				vDest[i++] = '&';
				pSrc += 5;
				continue;
			}
			else if (pSrc+5 < pSrcEnd && *(pSrc+1) == 'a' && *(pSrc+2) == 'p' && *(pSrc+3) == 'o' && *(pSrc+4) == 's' && *(pSrc+5) == ';')
			{
				vDest[i++] = '\'';
				pSrc += 6;
				continue;
			}
			else if (pSrc+5 < pSrcEnd && *(pSrc+1) == 'q' && *(pSrc+2) == 'u' && *(pSrc+3) == 'o' && *(pSrc+4) == 't' && *(pSrc+5) == ';')
			{
				vDest[i++] = '"';
				pSrc += 6;
				continue;
			}
			else if (pSrc+3 < pSrcEnd && *(pSrc+1) == 'g' && *(pSrc+2) == 't' && *(pSrc+3) == ';')
			{
				vDest[i++] = '>';
				pSrc += 4;
				continue;
			}
			else if (pSrc+3 < pSrcEnd && *(pSrc+1) == 'l' && *(pSrc+2) == 't' && *(pSrc+3) == ';')
			{
				vDest[i++] = '<';
				pSrc += 4;
				continue;
			}
			else
			{
				vDest[i++] = *(pSrc++);
			}
		}
		else
		{
			vDest[i++] = *(pSrc++);
		}
	}
	sSrc = &vDest[0];

	return sSrc;
}
//解析XPDL后需反转义得到真实参数值
BOOL CXpdl::ConvertFromYangboCdata()
{
	for ( map< CString, CXpdlNode* >::iterator It = m_mapParam.begin(); It != m_mapParam.end(); It++ )
	{
		CXpdlNodeParam* pNode = ( CXpdlNodeParam* )It->second;

		if ( pNode->m_sDataType == "STRING" && !pNode->m_sValue.IsEmpty() )
		{
			pNode->m_sValue = StringUnEscape(pNode->m_sValue);

			if ( !pNode->m_sOutValue.IsEmpty() )
			{
				pNode->m_sOutValue = StringUnEscape( pNode->m_sOutValue );
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}
	return TRUE;
}
//保存XPDL时需转义按杨波的格式写入
BOOL CXpdl::ConvertToYangboCdata()
{
	for ( map< CString, CXpdlNode* >::iterator It = m_mapParam.begin(); It != m_mapParam.end(); It++ )
	{
		CXpdlNodeParam* pNode = ( CXpdlNodeParam* )It->second;

		if ( pNode->m_sDataType == "STRING" && !pNode->m_sValue.IsEmpty() )
		{
			pNode->m_sValue = StringEscape(pNode->m_sValue);

			if ( !pNode->m_sOutValue.IsEmpty() )
			{
				pNode->m_sOutValue = StringEscape( pNode->m_sOutValue );
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}
	return TRUE;
}

BOOL CXpdl::Parse()
{
	xml_node<>* pNodePackage = m_doc.first_node( "Package" );
	if ( pNodePackage == NULL )
	{
		AfxMessageBox( "解析xpdl，Parse获取Package新节点错误" );
		return FALSE;
	}
	xml_node<>* pNodeWorkflows = pNodePackage->first_node( "WorkflowProcesses" );
	if ( pNodeWorkflows == NULL )
	{
		AfxMessageBox( "解析xpdl，Parse获取WorkflowProcesses新节点错误" );
		return FALSE;
	}
	xml_node<> *root = pNodeWorkflows->first_node( "WorkflowProcess" );

	while( root != NULL ) //首先找到root节点去解析
	{
		DWORD dwNodeType = XPDL_NODE_NULL;

		if ( GetNodeTypeInfo( dwNodeType, root ) == XPDL_NODE_ROOT )
		{
			xml_attribute<>* xmlId = root->first_attribute( "Id" );

			if ( xmlId == NULL )
			{
				AfxMessageBox( "解析xpdl，Parse获取Id新节点错误" );
				return FALSE;
			}
			std::string sid = xmlId->value();

			CXpdlNode* pXpdlNode = GetNewXpdlNode( UTF_82ASCII( sid).c_str(), dwNodeType );

			if ( pXpdlNode == NULL )
			{
				return FALSE;
			}
			
			m_pRootNode = pXpdlNode;

			if( !GetNodeParam( root ) ) return FALSE;

			break;
		}
		root = root->next_sibling();
	}

	root = m_doc.first_node( "Package" )->first_node( "WorkflowProcesses" )->first_node( "WorkflowProcess" );

	while( root != NULL ) //遍历所有的
	{
		if ( !GetFlowNodeAttr( root ) ) return FALSE;

		root = root->next_sibling();
	}
	
	//解析XPDL后需反转义得到真实参数值
	if (!ConvertFromYangboCdata()) return FALSE;

	if( !SetParamReadOnly() ) return FALSE;

	return TRUE;
}

CXpdlNode*	CXpdl::Find( LPCTSTR lpId )
{
	if ( lpId == NULL )
	{
		return NULL;
	}

	map< CString, CXpdlNode* >::iterator it = m_mapNode.find( lpId );

	if ( it != m_mapNode.end() )
	{
		return it->second;
	}

	it = m_mapParam.find( lpId );

	if ( it == m_mapParam.end() )
	{
		return NULL;
	}

	return it->second;
}

BOOL CXpdl::GetFlowNodeAttr( xml_node<> * pNode )		
{
	if ( pNode == NULL )
	{
		AfxMessageBox("解析xpdl,GetFlowNodeAttr参数节点错误" );
		return FALSE;
	}

	DWORD dwNodeType = XPDL_NODE_NULL;

	if ( !GetNodeTypeInfo( dwNodeType, pNode ) )
	{
		AfxMessageBox( "解析xpdl获取节点类型错误" );
		return FALSE;
	}

	xml_attribute< char >*  pXmlValue = pNode->first_attribute( "Id" );

	if ( pXmlValue == NULL )
	{
		AfxMessageBox( "解析xpdl获取节点id错误" );
		return FALSE;
	}

	std::string sId = pXmlValue->value();

	CXpdlNode* pXpdlNode = GetNewXpdlNode( UTF_82ASCII( sId ).c_str(), dwNodeType );

	if ( pXpdlNode == NULL )
	{
		AfxMessageBox("解析xpdl创建新节点错误" );
		return FALSE;
	}

	pXmlValue = pNode->first_attribute( "Name" );

	if ( pXmlValue == NULL )
	{
		AfxMessageBox( "解析xpdl获取节点Name错误" );
		return FALSE;
	}

	sId = pXmlValue->value();

	pXpdlNode->m_sName = UTF_82ASCII( sId ).c_str();

	xml_node<>* pChildNode = pNode->first_node( "ProcessHeader" );

	if ( pChildNode == NULL )
	{
		AfxMessageBox( "解析xpdl获取ProcessHeader节点错误" );
		return FALSE;
	}

	xml_node<>* pInfoNode = NULL;

	if ( pChildNode->first_node( "Author" ) != NULL && pChildNode->first_node( "Author" )->first_node() != NULL )
	{
		sId = pChildNode->first_node( "Author" )->first_node()->value();

		pXpdlNode->m_sAuthor = UTF_82ASCII( sId ).c_str();
	}
	else
	{
		AfxMessageBox("Author节点没有CDATA节点" );

		return FALSE;
	}

	if ( pChildNode->first_node( "Created" ) != NULL && pChildNode->first_node( "Created" )->first_node() != NULL )
	{
		sId = pChildNode->first_node( "Created" )->first_node()->value();

		pXpdlNode->m_sCreated = UTF_82ASCII( sId ).c_str();
	}
	else
	{
		AfxMessageBox("Created节点没有CDATA节点" );

		return FALSE;
	}

	if ( pChildNode->first_node( "Description" ) != NULL && pChildNode->first_node( "Description" )->first_node() != NULL )
	{
		sId = pChildNode->first_node( "Description" )->first_node()->value();

		pXpdlNode->m_sDescription = UTF_82ASCII( sId ).c_str();
	}
	else
	{
		AfxMessageBox("Description节点没有CDATA节点" );

		return FALSE;
	}

	if ( pChildNode->first_node( "Version" ) != NULL && pChildNode->first_node( "Version" )->first_node() != NULL )
	{
		sId = pChildNode->first_node( "Version" )->first_node()->value();

		pXpdlNode->m_sVersion = UTF_82ASCII( sId ).c_str();
	}
	else
	{
		AfxMessageBox("Version节点没有CDATA节点" );

		return FALSE;
	}

	if ( pNode->first_node( "Activities" ) == NULL )
	{
		AfxMessageBox("解析xpdl获取Activities节点错误" );
		return FALSE;
	}

	pChildNode = pNode->first_node( "Activities" )->first_node( "Activity" );

	if ( pChildNode == NULL )
	{
		AfxMessageBox("解析xpdl获取Activity节点错误" );
		return FALSE;
	}

	std::string sValue;

	CXpdlNode* pParamNode = NULL;

	while( pChildNode )
	{
		if ( !GetNodeTypeInfo( dwNodeType, pChildNode ) )
		{
			AfxMessageBox("解析xpdl获取节点类型错误" );
			return FALSE;
		}

		CXpdlNode* pNewNode = NULL;

		sId = "";

		if ( dwNodeType == XPDL_NODE_COMPONENTNODE )
		{
			pXmlValue = pChildNode->first_attribute( "Id" );

			if ( pXmlValue == NULL )
			{
				pChildNode = pChildNode->next_sibling();
				continue;
			}

			sId = pXmlValue->value();

			pNewNode = NULL;

			pNewNode = GetNewXpdlNode( UTF_82ASCII( sId ).c_str(), dwNodeType );

			if ( pNewNode == NULL )
			{
				AfxMessageBox("解析xpdl创建新节点错误" );
				return FALSE;
			}

			pNewNode->m_pParentNode = pXpdlNode;

			pXmlValue = pChildNode->first_attribute( "Name" );

			if ( pXmlValue == NULL )
			{
				pChildNode = pChildNode->next_sibling();
				continue;
			}

			sValue = pXmlValue->value();

			pNewNode->m_sName = UTF_82ASCII( sValue ).c_str();

			if ( pChildNode->first_node( "Author" ) != NULL && pChildNode->first_node( "Author" )->first_node() != NULL )
			{
				sValue = pChildNode->first_node( "Author" )->first_node()->value();

				pNewNode->m_sAuthor = UTF_82ASCII( sValue ).c_str();
			}
			else
			{
				AfxMessageBox("Author节点没有CDATA节点" );

				return FALSE;
			}
			
			if ( pChildNode->first_node( "Created" ) != NULL && pChildNode->first_node( "Created" )->first_node() != NULL )
			{
				sValue = pChildNode->first_node( "Created" )->first_node()->value();

				pNewNode->m_sCreated = UTF_82ASCII( sValue ).c_str();
			}
			else
			{
				AfxMessageBox("Created节点没有CDATA节点" );

				return FALSE;
			}
			
			if ( pChildNode->first_node( "Description" ) != NULL && pChildNode->first_node( "Description" )->first_node() != NULL )
			{
				sValue = pChildNode->first_node( "Description" )->first_node()->value();

				pNewNode->m_sDescription = UTF_82ASCII( sValue ).c_str();
			}
			else
			{
				AfxMessageBox("Description节点没有CDATA节点" );

				return FALSE;
			}
			
			if ( pChildNode->first_node( "Version" ) != NULL && pChildNode->first_node( "Version" )->first_node() != NULL )
			{
				sValue = pChildNode->first_node( "Version" )->first_node()->value();

				pNewNode->m_sVersion = UTF_82ASCII( sValue ).c_str();
			}
			else
			{
				AfxMessageBox("Version节点没有CDATA节点" );

				return FALSE;
			}

			if ( pChildNode->first_node( "ExtendedAttributes" ) != NULL )
			{
				pInfoNode = pChildNode->first_node( "ExtendedAttributes" )->first_node( "ExtendedAttribute" );

				bool bIn = false, bOut = false,bExistChildNode = false;

				while ( pInfoNode )
				{
					bIn = false; bOut =false;bExistChildNode = false;

					pXmlValue = pInfoNode->first_attribute( "Name" );

					if ( pXmlValue == NULL )
					{
						pInfoNode = pInfoNode->next_sibling();
						continue;
					}

					sValue = pXmlValue->value();

					char cValue[MAX_PATH] = {0}, cId[MAX_PATH] = {0};

					strcpy( cValue, UTF_82ASCII( sValue).c_str() );

					if ( stricmp( cValue, "VariableToProcess_IN" ) == 0 )
					{
						bIn = true;
					}
					else if ( stricmp( cValue, "VariableToProcess_OUT" ) == 0 )
					{
						bOut = true;
					}
					else if ( stricmp( cValue, "ComponentFileStorePaths" ) == 0 )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue != NULL )
						{
							std::string sStr = pXmlValue->value();

							( ( CXpdlNodeComponent * )pNewNode )->m_sPaths = UTF_82ASCII( sStr ).c_str();
						}
					}
					else if ( stricmp( cValue, "PreComponentFileStorePaths" ) == 0 )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue != NULL )
						{
							std::string sStr = pXmlValue->value();

							( ( CXpdlNodeComponent * )pNewNode )->m_sPrePaths = UTF_82ASCII( sStr ).c_str();
						}
					}
					else if ( stricmp( cValue, "PostComponentFileStorePaths" ) == 0 )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue != NULL )
						{
							std::string sStr = pXmlValue->value();

							( ( CXpdlNodeComponent * )pNewNode )->m_sPostPaths = UTF_82ASCII( sStr ).c_str();
						}
					}
					else if ( stricmp( cValue, "ComponentType" ) == 0 )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue != NULL )
						{
							std::string sStr = pXmlValue->value();

							strcpy( ( ( CXpdlNodeComponent * )pNewNode )->m_cRobotType, UTF_82ASCII( sStr ).c_str() );
						}
					}
					else if ( stricmp( cValue, "Manifest" ) == 0 )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue != NULL )
						{
							std::string sStr = pXmlValue->value();

							strcpy( ( ( CXpdlNodeComponent * )pNewNode )->m_cManifestPath, UTF_82ASCII( sStr ).c_str() );
						}
					}
					else if ( stricmp( cValue, "DataMapping" ) == 0 )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue != NULL )
						{
							std::string sStr = pXmlValue->value();

							sStr = UTF_82ASCII( sStr );

							m_lstDataMapping.push_back( sStr.c_str() );
						}
					}

					if ( bIn || bOut )
					{
						pXmlValue = pInfoNode->first_attribute( "Value" );

						if ( pXmlValue == NULL )
						{
							AfxMessageBox("解析xpdl获取Value节点错误" );
							return FALSE;
						}
						sValue = pXmlValue->value();

						strcpy( cId, UTF_82ASCII( sValue).c_str() );

						CXpdlNode* pNode = GetNewXpdlParam( cId );

						if ( pNode == NULL )
						{
							AfxMessageBox("解析xpdl创建新参数节点错误" );
							return FALSE;
						}

						if ( bIn )
						{
							if ( ((CXpdlNodeParam*)pNode )->m_sDirection.IsEmpty() )
							{
								((CXpdlNodeParam*)pNode )->m_sDirection = "in";
							}
							else if ( ((CXpdlNodeParam*)pNode )->m_sDirection.CompareNoCase( "out" ) == 0 )
							{
								((CXpdlNodeParam*)pNode )->m_sDirection = "inout";
							}
							else if ( ((CXpdlNodeParam*)pNode )->m_sDirection.CompareNoCase( "in" ) != 0 &&
								((CXpdlNodeParam*)pNode )->m_sDirection.CompareNoCase( "inout" ) != 0)
							{
								((CXpdlNodeParam*)pNode )->m_sDirection = "in";
							}
						}
						else if ( bOut )
						{
							if ( ((CXpdlNodeParam*)pNode )->m_sDirection.IsEmpty() )
							{
								((CXpdlNodeParam*)pNode )->m_sDirection = "out";
							}
							else if ( ((CXpdlNodeParam*)pNode )->m_sDirection.CompareNoCase( "in" ) == 0 )
							{
								((CXpdlNodeParam*)pNode )->m_sDirection = "inout";
							}
							else if ( ((CXpdlNodeParam*)pNode )->m_sDirection.CompareNoCase( "out" ) != 0 &&
								((CXpdlNodeParam*)pNode )->m_sDirection.CompareNoCase( "inout" ) != 0)
							{
								((CXpdlNodeParam*)pNode )->m_sDirection = "out";
							}
						}

						map< CString, CString >::iterator iterChild;

						iterChild = ((CXpdlNodeComponent *)pNewNode)->m_mapChildNode.find( pNode->m_sID );

						if ( iterChild == ((CXpdlNodeComponent *)pNewNode)->m_mapChildNode.end() )
						{
							pNewNode->m_vecChildNode.push_back( pNode );

							((CXpdlNodeComponent *)pNewNode)->m_mapChildNode.insert( make_pair( pNode->m_sID, pNode->m_sID ));
						}

						pNode->m_pParentNode = pNewNode;
					}

					pInfoNode = pInfoNode->next_sibling();
				}
			}
			

			pXpdlNode->m_vecChildNode.push_back( pNewNode );
		}
		else
		{
			if ( pChildNode->first_node( "Implementation" ) != NULL )
			{
				pInfoNode = pChildNode->first_node( "Implementation" )->first_node( "SubFlow" );

				if ( pInfoNode != NULL )
				{
					pXmlValue = pInfoNode->first_attribute( "Id" );

					if ( pXmlValue == NULL )
					{
						AfxMessageBox("解析xpdl获取SubFlow_id节点错误" );
						return FALSE;
					}

					sId = pXmlValue->value();

					pNewNode = GetNewXpdlNode( UTF_82ASCII( sId ).c_str(), dwNodeType );

					pNewNode->m_pParentNode = pXpdlNode;

					pXpdlNode->m_vecChildNode.push_back( pNewNode );

					xml_attribute<>* AttrId = pChildNode->first_attribute( "Id" );

					if ( AttrId != NULL )
					{
						string sIdOther = AttrId->value();

						sIdOther =  UTF_82ASCII( sIdOther ).c_str();

						pNewNode->m_sIdOther.Format( "%s", sIdOther.c_str() );
					}
				}
			}

		}

		m_mapNode.insert( make_pair( UTF_82ASCII( sId ).c_str(), pNewNode ) );

		pChildNode = pChildNode->next_sibling();
	}


	if ( pXpdlNode->m_dwNodeType == XPDL_NODE_SUBFLOWNODE || pXpdlNode->m_dwNodeType == XPDL_NODE_ROOT )
	{
		xml_node<>* pAttrNode = pNode->first_node( "ExtendedAttributes" )->first_node( "ExtendedAttribute" );

		bool bIn = false, bOut = false,bExistChildNode = false;

		while( pAttrNode)
		{
			bIn = false; bOut =false;bExistChildNode = false;

			sValue = pAttrNode->first_attribute( "Name" )->value();

			char cValue[MAX_PATH] = {0}, cId[MAX_PATH] = {0};

			strcpy( cValue, UTF_82ASCII( sValue).c_str() );

			if ( stricmp( cValue, "VariableToProcess_IN" ) == 0 )
			{
				bIn = true;
			}
			else if ( stricmp( cValue, "VariableToProcess_OUT" ) == 0 )
			{
				bOut = true;
			}
			else if ( stricmp( cValue, "PostComponentFileStorePaths" ) == 0 )
			{
				xml_attribute<>* pAttrValue = pAttrNode->first_attribute( "Value" );
				string sPostComonent = pAttrValue->value();
				sPostComonent = UTF_82ASCII( sPostComonent );
				pXpdlNode->m_sPostPaths = sPostComonent.c_str();
			}
			else if ( stricmp( cValue, "PreComponentFileStorePaths" ) == 0 )
			{
				xml_attribute<>* pAttrValue = pAttrNode->first_attribute( "Value" );
				string sPreComonent = pAttrValue->value();
				sPreComonent = UTF_82ASCII( sPreComonent );
				pXpdlNode->m_sPrePaths = sPreComonent.c_str();
			}
			else if ( stricmp( cValue, "DataMapping" ) == 0 )
			{
				xml_attribute<>* pAttrValue = pAttrNode->first_attribute( "Value" );
				if ( pAttrValue != NULL )
				{
					string sDataMapping = pAttrValue->value();
					sDataMapping = UTF_82ASCII( sDataMapping );
					m_lstDataMapping.push_back( sDataMapping.c_str() );
				}
			}

			if ( bIn || bOut )
			{
				sValue = pAttrNode->first_attribute( "Value" )->value();

				strcpy( cId, UTF_82ASCII( sValue).c_str() );

				CXpdlNode* pNodeParam = GetNewXpdlParam( cId );

				if ( pNode == NULL )
				{
					return FALSE;
				}

				if ( bIn )
				{
					if ( ((CXpdlNodeParam*)pNodeParam )->m_sDirection.IsEmpty() )
					{
						((CXpdlNodeParam*)pNodeParam )->m_sDirection = "in";
					}
					else if ( ((CXpdlNodeParam*)pNodeParam )->m_sDirection.CompareNoCase( "out" ) == 0 )
					{
						((CXpdlNodeParam*)pNodeParam )->m_sDirection = "inout";
					}
					else if ( ((CXpdlNodeParam*)pNodeParam )->m_sDirection.CompareNoCase( "in" ) != 0 &&
						((CXpdlNodeParam*)pNodeParam )->m_sDirection.CompareNoCase( "inout" ) != 0)
					{
						((CXpdlNodeParam*)pNodeParam )->m_sDirection = "in";
					}
				}
				else if ( bOut )
				{
					if ( ((CXpdlNodeParam*)pNodeParam )->m_sDirection.IsEmpty() )
					{
						((CXpdlNodeParam*)pNodeParam )->m_sDirection = "out";
					}
					else if ( ((CXpdlNodeParam*)pNodeParam )->m_sDirection.CompareNoCase( "in" ) == 0 )
					{
						((CXpdlNodeParam*)pNodeParam )->m_sDirection = "inout";
					}
					else if ( ((CXpdlNodeParam*)pNodeParam )->m_sDirection.CompareNoCase( "out" ) != 0 &&
						((CXpdlNodeParam*)pNodeParam )->m_sDirection.CompareNoCase( "inout" ) != 0)
					{
						((CXpdlNodeParam*)pNodeParam )->m_sDirection = "out";
					}
				}

				map< CString, CString >::iterator iterChild;

				iterChild = ((CXpdlNodeSubflow *)pXpdlNode)->m_mapChildNode.find( pNodeParam->m_sID );

				if ( iterChild == ((CXpdlNodeSubflow *)pXpdlNode)->m_mapChildNode.end() )
				{
					pXpdlNode->m_vecChildNode.push_back( pNodeParam );

					((CXpdlNodeSubflow *)pXpdlNode)->m_mapChildNode.insert( make_pair( pNodeParam->m_sID, pNodeParam->m_sID ));
				}

				pNodeParam->m_pParentNode = pXpdlNode;
			}

			pAttrNode = pAttrNode->next_sibling();
		}
	}

	return TRUE;
}

BOOL CXpdl::GetNodeParam( xml_node<>* pNode )
{
	if ( pNode == NULL )
	{
		AfxMessageBox( "解析xpdl，GetNodeParam参数节点错误" );
		return FALSE;
	}
	if ( pNode->first_node( "DataFields" ) == NULL )
	{
		AfxMessageBox( "解析xpdl获取DataFields节点错误" );
		return FALSE;
	}
	xml_node<>* pXmlNode = pNode->first_node( "DataFields" )->first_node( "DataField" );
	if ( pXmlNode == NULL )
	{
		return TRUE;
	}

	std::string sValue;

	xml_attribute<>* pAttrNode;

	char cValue[MAX_PATH] = {0};

	while ( pXmlNode )
	{
		ZeroMemory( cValue, MAX_PATH );

		pAttrNode = pXmlNode->first_attribute( "Id" );
		if ( pAttrNode == NULL )
		{
			AfxMessageBox( "解析xpdl，GetNodeParam获取id节点错误" );
			return FALSE;
		}

		sValue = pAttrNode->value();

		CXpdlNode* pXpdlNode = GetNewXpdlParam( UTF_82ASCII( sValue ).c_str() );

		if ( pXpdlNode == NULL )
		{
			AfxMessageBox( "解析xpdl，GetNodeParam创建新节点错误" );
			return FALSE;
		}

		pAttrNode = pXmlNode->first_attribute( "Name" );

		if ( pAttrNode == NULL ) 
		{
			AfxMessageBox( "解析xpdl，GetNodeParam获取Name新节点错误" );
			return FALSE;
		}

		sValue = pAttrNode->value();

		pXpdlNode->m_sName = UTF_82ASCII( sValue ).c_str();

		xml_attribute<>* pAttrIsArray = pXmlNode->first_attribute( "IsArray" );

		if ( pAttrIsArray == NULL )
		{
			AfxMessageBox( "解析xpdl，GetNodeParam获取IsArray新节点错误" );
			return FALSE;
		}

		sValue = pAttrIsArray->value();

		strcpy( cValue, UTF_82ASCII( sValue ).c_str() );

		xml_node<>* pAttributesNode;

		if ( stricmp( cValue, "true") == 0 )
		{
			((CXpdlNodeParam *)pXpdlNode)->m_bIsArray = TRUE;

			xml_node<>* pNodeDataType = pXmlNode->first_node( "DataType" );
			if ( pNodeDataType == NULL )
			{
				AfxMessageBox( "解析xpdl，GetNodeParam获取DataType新节点错误" );
				return FALSE;
			}
			xml_node<>* pNodeArrayType = pNodeDataType->first_node( "ArrayType" );
			if ( pNodeArrayType == NULL )
			{
				AfxMessageBox( "解析xpdl，GetNodeParam获取ArrayType新节点错误" );
				return FALSE;
			}
			xml_attribute<>* pAttrArrayUpperIndex = pNodeArrayType->first_attribute( "UpperIndex" );
			if ( pAttrArrayUpperIndex != NULL )
			{
				sValue = pAttrArrayUpperIndex->value();
				((CXpdlNodeParam *)pXpdlNode)->m_sArrayUpperIndex = UTF_82ASCII( sValue ).c_str();
				((CXpdlNodeParam *)pXpdlNode)->m_sArrayUpperIndex_Out = ((CXpdlNodeParam *)pXpdlNode)->m_sArrayUpperIndex;
			}
			xml_attribute<>* pAttrArrayLowerIndex = pNodeArrayType->first_attribute( "LowerIndex" );
			if ( pAttrArrayLowerIndex != NULL )
			{
				sValue = pAttrArrayLowerIndex->value();
				((CXpdlNodeParam *)pXpdlNode)->m_sArrayLowerIndex = UTF_82ASCII( sValue ).c_str();
				((CXpdlNodeParam *)pXpdlNode)->m_sArrayLowerIndex_Out = ((CXpdlNodeParam *)pXpdlNode)->m_sArrayLowerIndex;
			}

			pAttributesNode = pNodeArrayType->first_node( "BasicType" );
			if ( pAttributesNode == NULL )
			{
				AfxMessageBox( "解析xpdl，GetNodeParam获取BasicType新节点错误" );
				return FALSE;
			}

			pAttrNode = pAttributesNode->first_attribute( "Type" );
			if ( pAttrNode == NULL )
			{
				AfxMessageBox( "解析xpdl，GetNodeParam获取Type新节点错误" );
				return FALSE;
			}

			sValue = pAttrNode->value();

			((CXpdlNodeParam *)pXpdlNode)->m_sDataType = UTF_82ASCII( sValue ).c_str();
		}
		else
		{
			((CXpdlNodeParam *)pXpdlNode)->m_bIsArray = FALSE;

			xml_node<>* pNodeDataType = pXmlNode->first_node( "DataType" );

			if ( pNodeDataType == NULL )
			{
				AfxMessageBox( "解析xpdl，GetNodeParam获取DataType新节点错误" );
				return FALSE;
			}

			pAttributesNode = pNodeDataType->first_node( "BasicType" );

			if ( pAttributesNode == NULL )
			{
				AfxMessageBox( "解析xpdl，GetNodeParam获取BasicType新节点错误" );
				return FALSE;
			}

			pAttrNode = pAttributesNode->first_attribute( "Type" );

			if ( pAttrNode != NULL )
			{
				sValue = pAttrNode->value();

				((CXpdlNodeParam *)pXpdlNode)->m_sDataType = UTF_82ASCII( sValue ).c_str();
			}
		}
		xml_node<>* pNodeAttrExtends = pXmlNode->first_node( "ExtendedAttributes" );

		if ( pNodeAttrExtends != NULL )
		{
			pAttributesNode = pNodeAttrExtends->first_node( "ExtendedAttribute" );

			while( pAttributesNode ) 
			{
				pAttrNode = pAttributesNode->first_attribute( "Name" );

				if ( pAttrNode == NULL )
				{
					pAttributesNode = pAttributesNode->next_sibling();
					continue;
				}

				sValue = pAttrNode->value();

				ZeroMemory( cValue, MAX_PATH );

				strcpy( cValue, UTF_82ASCII( sValue ).c_str() );

				if ( stricmp( cValue, "units") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						((CXpdlNodeParam *)pXpdlNode)->m_sUnit = UTF_82ASCII( sValue ).c_str();
					}
				}
				else if ( stricmp( cValue, "desc") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						pXpdlNode->m_sDescription = UTF_82ASCII( sValue ).c_str();
					}
				}
				else if ( stricmp( cValue, "cnname") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						((CXpdlNodeParam *)pXpdlNode)->m_sText = UTF_82ASCII( sValue ).c_str();
					}
				}
				else if ( stricmp( cValue, "format") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						((CXpdlNodeParam *)pXpdlNode)->m_sFormat = UTF_82ASCII( sValue ).c_str();
					}
				}
				else if ( stricmp( cValue, "status") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						((CXpdlNodeParam *)pXpdlNode)->m_sState = UTF_82ASCII( sValue ).c_str();
					}
				}
				else if ( stricmp( cValue, "group") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						((CXpdlNodeParam *)pXpdlNode)->m_sGroup = UTF_82ASCII( sValue ).c_str();
					}
				}
				else if ( stricmp( cValue, "isDynamic") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						if ( stricmp( "true", sValue.c_str() ) == 0 )
						{
							((CXpdlNodeParam *)pXpdlNode)->m_bIsDynamicArray = TRUE;
						}
					}
				}
				else if ( stricmp( cValue, "issaveinmodel") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						if ( stricmp( "true", sValue.c_str() ) == 0 )
						{
							((CXpdlNodeParam *)pXpdlNode)->m_bIsSaveInModel = TRUE;
						}
					}
				}
				else if ( stricmp( cValue, "multifiles") == 0 )
				{
					xml_attribute<>* pNodeAttrValue = pAttributesNode->first_attribute( "Value" );

					if ( pNodeAttrValue != NULL )
					{
						sValue = pNodeAttrValue->value();

						if ( stricmp( "true", sValue.c_str() ) == 0 )
						{
							((CXpdlNodeParam *)pXpdlNode)->m_bMultifiles = TRUE;
						}
					}
				}

				pAttributesNode = pAttributesNode->next_sibling();
			}
		}

		pAttributesNode = pXmlNode->first_node( "InitialValue" );

		if ( pAttributesNode != NULL )
		{
			if ( pAttributesNode->first_node() != NULL )
			{
				sValue = pAttributesNode->first_node()->value();

				((CXpdlNodeParam *)pXpdlNode)->m_sValue = UTF_82ASCII( sValue ).c_str();

				((CXpdlNodeParam *)pXpdlNode)->m_sOutValue = ((CXpdlNodeParam *)pXpdlNode)->m_sValue;
			}
		}

		pXmlNode = pXmlNode->next_sibling();
	}
	return TRUE;
}

CXpdlNode* CXpdl::GetNewXpdlNode( CString sNodeId, DWORD dwNodeType )
{
	CXpdlNode* pNode = NULL;

	map< CString, CXpdlNode* >::iterator iterMap = m_mapNode.find( sNodeId );

	if ( iterMap != m_mapNode.end() )
	{
		return iterMap->second;
	}

	switch( dwNodeType )
	{
	case XPDL_NODE_ROOT:
		{
			pNode = new CXpdlNodePrimary();
		}break;
	case XPDL_NODE_PARALLELNODE:
		{
			pNode = new CXpdlNodeParallel();
		}break;
	case XPDL_NODE_SEQUENCENODE:
		{
			pNode = new CXpdlNodeSequence();
		}break;
	case XPDL_NODE_IFNODE:
		{
			pNode = new CXpdlNodeIf();
		}break;
	case XPDL_NODE_SUBFLOWNODE:
		{
			pNode = new CXpdlNodeSubflow();
		}break;
	case XPDL_NODE_COMPONENTNODE:
		{
			pNode = new CXpdlNodeComponent();
		}break;
	default:
		{
			pNode = new CXpdlNode();
		}break;
	}

	if ( pNode != NULL )
	{
		m_mapNode.insert( make_pair( sNodeId, pNode ) );
	}

	pNode->m_sID = sNodeId;

	pNode->m_dwNodeType = dwNodeType;

	return pNode;
}

CXpdlNode* CXpdl::GetNewXpdlParam( CString sParamId )
{
	if ( sParamId.IsEmpty() )
	{
		return NULL;
	}

	map< CString, CXpdlNode* >::iterator iterParam = m_mapParam.find( sParamId );

	if ( iterParam != m_mapParam.end() )
	{
		return iterParam->second;
	}

	CXpdlNode* pNode = (CXpdlNode *)new CXpdlNodeParam();

	if ( pNode == NULL )
	{
		return NULL;
	}

	pNode->m_sID = sParamId;

	pNode->m_dwNodeType = XPDL_NODE_PARAM;

	m_mapParam.insert( make_pair( sParamId, pNode ) );

	return pNode;
}

BOOL CXpdl::GetNodeTypeInfo(  DWORD & dwNodeType, xml_node<> * pNode )
{
	if ( !pNode )
	{
		return FALSE;
	}
	xml_node<>* pNodeExtends = pNode->first_node( "ExtendedAttributes" );

	if ( pNodeExtends == NULL )
	{
		AfxMessageBox( "解析xpdl，GetNodeTypeInfo获取ExtendedAttributes新节点错误" );
		return FALSE;
	}

	xml_node<> * RootNodeType = pNodeExtends->first_node( "ExtendedAttribute" );

	while( RootNodeType )
	{
		xml_attribute< char >* NodeTypeName = RootNodeType->first_attribute( "Name" );

		if ( NodeTypeName == NULL )
		{
			RootNodeType = RootNodeType->next_sibling();
			continue;
		}

		std::string sValue = NodeTypeName->value();

		if ( stricmp( sValue.c_str(), "nodeType" ) == 0 )
		{
			xml_attribute< char >* NodeTypeType = RootNodeType->first_attribute( "Value" );
			
			if ( NodeTypeType == NULL )
			{
				RootNodeType = RootNodeType->next_sibling();
				continue;
			}

			std::string sType = NodeTypeType->value();

			if ( stricmp( sType.c_str(), "root" ) == 0 )
			{
				dwNodeType = XPDL_NODE_ROOT;
			}
			else if ( stricmp( sType.c_str(), "PARALLELNODE" ) == 0 )
			{
				dwNodeType = XPDL_NODE_PARALLELNODE;
			}
			else if ( stricmp( sType.c_str(), "SEQUENCENODE" ) == 0 )
			{
				dwNodeType = XPDL_NODE_SEQUENCENODE;
			}
			else if ( stricmp( sType.c_str(), "IFNODE" ) == 0 )
			{
				dwNodeType = XPDL_NODE_IFNODE;
			}
			else if ( stricmp( sType.c_str(), "SUBFLOWNODE" ) == 0 )
			{
				dwNodeType = XPDL_NODE_SUBFLOWNODE;
			}
			else if ( stricmp( sType.c_str(), "COMPONENTNODE" ) == 0 )
			{
				dwNodeType = XPDL_NODE_COMPONENTNODE;
			}

			break;
		}

		RootNodeType = RootNodeType->next_sibling();
	}

	return TRUE;
}

BOOL CXpdl::SetNode( const CXpdlNode* pSrcNode )
{
	if ( pSrcNode->m_dwNodeType == XPDL_NODE_PARAM )
	{
		xml_node<>* pNodePackage = m_doc.first_node( "Package" );
		
		if ( pNodePackage == NULL )
		{
			AfxMessageBox( "保存xpdl，获取Package节点错误");
			return FALSE;
		}

		xml_node<>* pNodeWorkflows = pNodePackage->first_node( "WorkflowProcesses" );

		if ( pNodeWorkflows == NULL )
		{
			AfxMessageBox( "保存xpdl，获取WorkflowProcesses节点错误");
			return FALSE;
		}
		xml_node<>* pXmlNode = pNodeWorkflows->first_node( "WorkflowProcess" );

		if ( pXmlNode == NULL )
		{
			AfxMessageBox( "保存xpdl，获取WorkflowProcess节点错误");
			return FALSE;
		}

		DWORD dwNodeType = XPDL_NODE_NULL;

		xml_node<>* pParamNode;

		xml_node<>* pOperationNode;

		xml_node<>* pActivityNode;

		xml_node<>* pReplaceNode;

		xml_attribute<>* pAttrNode;
		
		std::string sValue;

		std::string sStr;

		std::string sCstr;

		bool bExistParam = false, bExistParent = false;

		while( pXmlNode ) //确保是root节点
		{
			if ( !GetNodeTypeInfo( dwNodeType, pXmlNode ) ) return FALSE;

			if ( dwNodeType == XPDL_NODE_ROOT )
			{
				pParamNode = pXmlNode->first_node( "DataFields" )->first_node( "DataField" );

				while( pParamNode )
				{
					sValue =  pParamNode->first_attribute( "Id" )->value() ;

					if ( pSrcNode->m_sID.CompareNoCase( UTF_82ASCII(sValue).c_str() ) == 0 )  //Id确定需要修改的参数节点
					{
						sValue = pParamNode->first_attribute( "Name" )->value();

						if ( pSrcNode->m_sName.CompareNoCase(  UTF_82ASCII(sValue).c_str()  ) != 0 )//Name属性已经被修改
						{
							pParamNode->remove_attribute( pParamNode->first_attribute( "Name" ) );

							pAttrNode = pParamNode->first_attribute( "IsArray" );

							sCstr = pSrcNode->m_sName;

							pParamNode->insert_attribute( pAttrNode, m_doc.allocate_attribute( m_doc.allocate_string( "Name" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) ); 
						}

						sValue = pParamNode->first_attribute( "IsArray" )->value();

						if ( stricmp( "true", UTF_82ASCII(sValue).c_str() ) == 0 )
						{
							pOperationNode = pParamNode->first_node( "DataType" )->first_node( "ArrayType" )->first_node( "BasicType" );

							sStr = pOperationNode->first_attribute( "Type" )->value();

							sCstr = ( (CXpdlNodeParam *)pSrcNode )->m_sDataType;

							if ( sCstr.compare( UTF_82ASCII(sStr) ) != 0 )
							{
								pOperationNode->remove_all_attributes();

								pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Type" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
							if ( ( (CXpdlNodeParam*)pSrcNode )->m_bIsDynamicArray )
							{
								xml_node<>* pArrayType = pParamNode->first_node( "DataType" )->first_node( "ArrayType" );

								if ( pArrayType )
								{
									sCstr = ( (CXpdlNodeParam *)pSrcNode )->m_sArrayUpperIndex;

									pArrayType->remove_all_attributes();

									pArrayType->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "UpperIndex" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );

									sCstr = ( (CXpdlNodeParam *)pSrcNode )->m_sArrayLowerIndex;

									pArrayType->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "LowerIndex" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
						}
						else
						{
							pOperationNode = pParamNode->first_node( "DataType" )->first_node( "BasicType" );

							sStr = pOperationNode->first_attribute( "Type" )->value();

							sCstr = ( (CXpdlNodeParam *)pSrcNode )->m_sDataType;

							if ( sCstr.compare( UTF_82ASCII(sStr) ) != 0 )
							{
								pOperationNode->remove_all_attributes();

								pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Type" ), m_doc.allocate_string( UTF_82ASCII(sCstr).c_str() ) )  );
							}
						}

						if ( ( stricmp( "false", UTF_82ASCII(sValue).c_str() ) == 0 &&  ((CXpdlNodeParam *)pSrcNode)->m_bIsArray )  || 
							( !((CXpdlNodeParam *)pSrcNode)->m_bIsArray &&  stricmp( "true", UTF_82ASCII(sValue).c_str() ) == 0 ) )
						{
							pParamNode->remove_attribute( pParamNode->first_attribute( "IsArray" ) );

							pAttrNode = pParamNode->first_attribute( "Name" );

							pOperationNode = pParamNode->first_node( "DataType" );

							pOperationNode->remove_all_nodes();

							sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sDataType;

							if ( ((CXpdlNodeParam *)pSrcNode)->m_bIsArray )
							{
								pParamNode->insert_attribute( pAttrNode, m_doc.allocate_attribute( "IsArray", "TRUE" ) );

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_element, m_doc.allocate_string( "ArrayType" ) ) );

								xml_node<>* pArrayNode = pOperationNode->first_node( "ArrayType" );

								pArrayNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "UpperIndex" ), m_doc.allocate_string( ((CXpdlNodeParam *)pSrcNode)->m_sArrayUpperIndex )  ) );

								pArrayNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "LowerIndex" ),m_doc.allocate_string( ((CXpdlNodeParam *)pSrcNode)->m_sArrayLowerIndex )  ) );

								pArrayNode->append_node( m_doc.allocate_node( rapidxml::node_element, m_doc.allocate_string( "BasicType" ) ) );

								xml_node<>* pBasicNode = pArrayNode->first_node( "BasicType" );

								pBasicNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Type" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
							else
							{
								pParamNode->insert_attribute( pAttrNode, m_doc.allocate_attribute( m_doc.allocate_string( "IsArray" ), m_doc.allocate_string( "FALSE" ) ) );

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_element, m_doc.allocate_string( "BasicType" ) ) );

								xml_node<>* pBasicNode = pOperationNode->first_node( "BasicType" );

								pBasicNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Type" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}

						pOperationNode = pParamNode->first_node( "ExtendedAttributes" )->first_node( "ExtendedAttribute" );

						while ( pOperationNode )
						{
							sValue = pOperationNode->first_attribute( "Name" )->value();

							if ( stricmp( "units", UTF_82ASCII(sValue).c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sUnit.CompareNoCase( UTF_82ASCII(sStr).c_str() ) != 0 )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sUnit;

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "desc", UTF_82ASCII(sValue).c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								sStr = UTF_82ASCII( sStr );

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sDescription.CompareNoCase( sStr.c_str() ) != 0 )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sDescription;

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "cnname", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sText.CompareNoCase( UTF_82ASCII(sStr).c_str() ) != 0 )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sText;

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "status", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sState.CompareNoCase( UTF_82ASCII(sStr).c_str() ) != 0 )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sState;

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "format", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sFormat.CompareNoCase( UTF_82ASCII(sStr).c_str() ) != 0 )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sFormat;

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "group", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sGroup.CompareNoCase( UTF_82ASCII(sStr).c_str() ) != 0 )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sGroup;

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "multifiles", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								BOOL bTemp = stricmp( sStr.c_str(), "true" )==0 ? TRUE : FALSE;

								if ( ((CXpdlNodeParam *)pSrcNode)->m_bMultifiles != bTemp )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_bMultifiles ? "false" : "true";

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "issaveinmodel", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								BOOL bTemp = stricmp( sStr.c_str(), "true" )==0 ? TRUE : FALSE;

								if ( ((CXpdlNodeParam *)pSrcNode)->m_bIsSaveInModel != bTemp )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_bIsSaveInModel ? "false" : "true";

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else if ( stricmp( "isDynamic", sValue.c_str() ) == 0 )
							{
								sStr = pOperationNode->first_attribute( "Value" )->value();

								BOOL bTemp = stricmp( sStr.c_str(), "true" )==0 ? TRUE : FALSE;

								if ( ((CXpdlNodeParam *)pSrcNode)->m_bIsDynamicArray != bTemp )
								{
									pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_bIsDynamicArray ? "false" : "true";

									pOperationNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}

							pOperationNode = pOperationNode->next_sibling();
						}

						pOperationNode = pParamNode->first_node( "InitialValue" );

						if ( pOperationNode )
						{
							if ( pOperationNode->first_node() )
							{
								sStr = pOperationNode->first_node()->value();

								if ( ((CXpdlNodeParam *)pSrcNode)->m_sValue.CompareNoCase( UTF_82ASCII(sStr).c_str() ) != 0 )
								{
									pOperationNode->remove_all_nodes();

									sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sValue;

									pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ),m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
								}
							}
							else
							{
								pOperationNode->remove_all_nodes();

								sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sValue;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ),m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );

							}
						}
						else if ( !pOperationNode )
						{
							pOperationNode = m_doc.allocate_node( rapidxml::node_element, m_doc.allocate_string( "InitialValue" ) );

							pParamNode->append_node( pOperationNode );

							sCstr = ((CXpdlNodeParam *)pSrcNode)->m_sValue;

							pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ),m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );

						}

						break;
					}

					pParamNode = pParamNode->next_sibling();
				}

				bExistParam = true;
			}

			sValue = pXmlNode->first_attribute( "Id" )->value();

			sValue = UTF_82ASCII(sValue);

			if ( ( pSrcNode->m_pParentNode && pSrcNode->m_pParentNode->m_sID.CompareNoCase( sValue.c_str() ) == 0) || 
				( pSrcNode->m_pParentNode && pSrcNode->m_pParentNode->m_pParentNode && pSrcNode->m_pParentNode->m_pParentNode->m_sID.CompareNoCase( sValue.c_str() ) == 0 ) )
			{
				bExistParent = true;

				bool bIn = false, bOut = false;

				if ( ( (CXpdlNodeParam *)pSrcNode )->m_sDirection.CompareNoCase( "inout" ) == 0 )
				{
					bIn = true; bOut = true;
				}
				else if ( ( (CXpdlNodeParam *)pSrcNode )->m_sDirection.CompareNoCase( "in" ) == 0 )
				{
					bIn = true;
				}
				else if ( ( (CXpdlNodeParam *)pSrcNode )->m_sDirection.CompareNoCase( "out" ) == 0 )
				{
					bOut = true;
				}

				if ( pSrcNode->m_pParentNode->m_dwNodeType == XPDL_NODE_COMPONENTNODE )
				{
					pActivityNode = pXmlNode->first_node( "Activities" )->first_node( "Activity" );
				}
				else if (  pSrcNode->m_pParentNode->m_dwNodeType == XPDL_NODE_SUBFLOWNODE ||
					pSrcNode->m_pParentNode->m_dwNodeType == XPDL_NODE_ROOT )
				{
					pActivityNode = pXmlNode;
				}

				while ( pActivityNode )
				{
					sValue = pActivityNode->first_attribute( "Id" )->value();

					sValue = UTF_82ASCII( sValue );

					if ( pSrcNode->m_pParentNode->m_sID.CompareNoCase( sValue.c_str() ) == 0 )
					{
						pOperationNode = pActivityNode->first_node( "ExtendedAttributes" )->first_node( "ExtendedAttribute" );

						while ( pOperationNode && ( bIn || bOut ) )
						{
							sValue = pOperationNode->first_attribute( "Value" )->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sID.CompareNoCase( sValue.c_str() ) == 0 )
							{
								sValue = pOperationNode->first_attribute( "Name" )->value();

								sValue = UTF_82ASCII( sValue );

								if (  stricmp( "VariableToProcess_IN", sValue.c_str() ) == 0)
								{
									if ( !bIn )
									{
										pReplaceNode = pOperationNode->next_sibling();

										pActivityNode->first_node( "ExtendedAttributes" )->remove_node( pOperationNode );

										pOperationNode = pReplaceNode;
									}

									bIn = false;
								}
								else if ( stricmp( "VariableToProcess_OUT", sValue.c_str() ) == 0 )
								{
									if ( !bOut )
									{
										pReplaceNode = pOperationNode->next_sibling();

										pActivityNode->first_node( "ExtendedAttributes" )->remove_node( pOperationNode );

										pOperationNode = pReplaceNode;
									}

									bOut = false;
								}
							}

							pOperationNode = pOperationNode->next_sibling();
						}

						if ( bIn )
						{
							xml_node<>* pChildNode = m_doc.allocate_node( rapidxml::node_element, "ExtendedAttribute" );

							pChildNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Name" ), m_doc.allocate_string( "VariableToProcess_IN" ) ) );

							sCstr = pSrcNode->m_sID;

							pChildNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );

							pActivityNode->first_node( "ExtendedAttributes" )->append_node( pChildNode );
						}
						if ( bOut )
						{
							xml_node<>* pChildNode = m_doc.allocate_node( rapidxml::node_element, "ExtendedAttribute" );

							pChildNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Name" ), m_doc.allocate_string( "VariableToProcess_OUT" ) ) );

							sCstr = pSrcNode->m_sID;

							pChildNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Value" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );

							pActivityNode->first_node( "ExtendedAttributes" )->append_node( pChildNode );
						}
						break;
					}
					pActivityNode = pActivityNode->next_sibling();
				}

			}

			if ( bExistParent && bExistParam )
			{
				break;
			}

			pXmlNode = pXmlNode->next_sibling();
		}
	}
	else if(  pSrcNode->m_dwNodeType == XPDL_NODE_COMPONENTNODE  )
	{
		if ( !pSrcNode->m_pParentNode )
		{
			AfxMessageBox( "组件节点没有父节点，错误" );

			return FALSE;
		}
		/*
		未考虑修改节点类型 和 修改父子关系
		*/
		xml_node<>* pParentNode = m_doc.first_node( "Package" )->first_node( "WorkflowProcesses" )->first_node( "WorkflowProcess" );

		xml_node<>* pComponentNode;

		xml_node<>* pOperationNode;

		std::string sValue;

		std::string sCstr;

		while ( pParentNode )
		{
			sValue = pParentNode->first_attribute( "Id" )->value();

			if ( pSrcNode->m_pParentNode->m_sID.CompareNoCase( UTF_82ASCII(sValue).c_str() ) == 0 )
			{
				pComponentNode = pParentNode->first_node( "Activities" )->first_node( "Activity" );

				while ( pComponentNode )
				{
					sValue = pComponentNode->first_attribute( "Id" )->value();

					sValue = UTF_82ASCII( sValue );

					if ( pSrcNode->m_sID.CompareNoCase( sValue.c_str() ) == 0 )
					{
						sValue = pComponentNode->first_attribute( "Name" )->value();

						sValue = UTF_82ASCII( sValue );

						if ( pSrcNode->m_sName.CompareNoCase( sValue.c_str() ) != 0 )
						{
							pComponentNode->remove_attribute( pComponentNode->first_attribute( "Name" ) );

							sCstr = pSrcNode->m_sName;

							pComponentNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Name" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
						}

						pOperationNode = pComponentNode->first_node( "Author" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sAuthor.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sAuthor;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Author节点没有CDATA节点" );

							return FALSE;
						}

						pOperationNode = pComponentNode->first_node( "Created" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sCreated.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sCreated;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata,m_doc.allocate_string( "" ) , m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Created节点没有CDATA节点" );

							return FALSE;
						}


						pOperationNode = pComponentNode->first_node( "Description" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sDescription.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sDescription;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Description节点没有CDATA节点" );

							return FALSE;
						}
						
						pOperationNode = pComponentNode->first_node( "Version" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sVersion.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sVersion;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ) , m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Version节点没有CDATA节点" );

							return FALSE;
						}

						pOperationNode = pComponentNode->first_node( "ExtendedAttributes" )->first_node( "ExtendedAttribute" );

						while ( pOperationNode )
						{
							std::string sName = pOperationNode->first_attribute( "Name" )->value();

							sName = UTF_82ASCII( sName );

							if ( stricmp( "ComponentFileStorePaths", sName.c_str() ) == 0 )
							{
								pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

								sCstr = ( ( CXpdlNodeComponent * )pSrcNode )->m_sPaths;

								pOperationNode->append_attribute( m_doc.allocate_attribute( "Value", m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() )));
							}
							else if ( stricmp( "PreComponentFileStorePaths", sName.c_str() ) == 0 )
							{
								pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

								sCstr = ( ( CXpdlNodeComponent * )pSrcNode )->m_sPrePaths;

								pOperationNode->append_attribute( m_doc.allocate_attribute( "Value", m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() )));
							}
							else if ( stricmp( "PostComponentFileStorePaths", sName.c_str() ) == 0 )
							{
								pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

								sCstr = ( ( CXpdlNodeComponent * )pSrcNode )->m_sPostPaths;

								pOperationNode->append_attribute( m_doc.allocate_attribute( "Value", m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() )));
							}
							else if ( stricmp( "Manifest", sName.c_str() ) == 0 )
							{
								pOperationNode->remove_attribute( pOperationNode->first_attribute( "Value" ) );

								sCstr = ( ( CXpdlNodeComponent * )pSrcNode )->m_cManifestPath;

								pOperationNode->append_attribute( m_doc.allocate_attribute( "Value", m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() )));
							}

							pOperationNode = pOperationNode->next_sibling();
						}

						break;
					}

					pComponentNode = pComponentNode->next_sibling();
				}

				break;
			}

			pParentNode = pParentNode->next_sibling();
		}
	}
	else
	{
		xml_node<>* pOperationNode;

		xml_node<>* pActivityNode;

		std::string sValue;

		std::string sStr;

		std::string sCstr;

		bool bExistNode = false, bExistParentNode = false;

		xml_node<>* pFlowNode = m_doc.first_node( "Package" )->first_node( "WorkflowProcesses" )->first_node( "WorkflowProcess" );

		while( pFlowNode )
		{
			sValue = pFlowNode->first_attribute( "Id" )->value();

			sValue = UTF_82ASCII( sValue );

			if ( pSrcNode->m_sID.CompareNoCase( sValue.c_str() ) == 0 )
			{
				bExistNode = true;

				pOperationNode = pFlowNode->first_node( "ProcessHeader" )->first_node( "Author" );

				if ( pOperationNode->first_node() )
				{
					sStr = pOperationNode->first_node()->value();

					sStr = UTF_82ASCII( sStr );

					if ( pSrcNode->m_sAuthor.CompareNoCase( sStr.c_str() ) != 0 )
					{
						pOperationNode->remove_all_nodes();

						sCstr = pSrcNode->m_sAuthor;

						pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
					}
				}
				else
				{
					AfxMessageBox( "Author节点没有cdata节点" );

					return FALSE;
				}

				pOperationNode = pFlowNode->first_node( "ProcessHeader" )->first_node( "Created" );

				if ( pOperationNode->first_node() )
				{
					sStr = pOperationNode->first_node()->value();

					sStr = UTF_82ASCII( sStr );

					if ( pSrcNode->m_sCreated.CompareNoCase( sStr.c_str() ) != 0 )
					{
						pOperationNode->remove_all_nodes();

						sCstr = pSrcNode->m_sCreated;

						pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
					}
				}
				else
				{
					AfxMessageBox( "Created节点没有cdata节点" );

					return FALSE;
				}

				pOperationNode = pFlowNode->first_node( "ProcessHeader" )->first_node( "Description" );

				if ( pOperationNode->first_node() )
				{
					sStr = pOperationNode->first_node()->value();

					sStr = UTF_82ASCII( sStr );

					if ( pSrcNode->m_sDescription.CompareNoCase( sStr.c_str() ) != 0 )
					{
						pOperationNode->remove_all_nodes();

						sCstr = pSrcNode->m_sDescription;

						pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
					}
				}
				else
				{
					AfxMessageBox( "Description节点没有cdata节点" );

					return FALSE;
				}

				pOperationNode = pFlowNode->first_node( "ProcessHeader" )->first_node( "Version" );

				if ( pOperationNode->first_node() )
				{
					sStr = pOperationNode->first_node()->value();

					sStr = UTF_82ASCII( sStr );

					if ( pSrcNode->m_sVersion.CompareNoCase( sStr.c_str() ) != 0 )
					{
						pOperationNode->remove_all_nodes();

						sCstr = pSrcNode->m_sVersion;

						pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
					}
				}

				sStr = pFlowNode->first_attribute( "Name" )->value();

				sStr = UTF_82ASCII( sStr );

				if ( pSrcNode->m_sName.CompareNoCase( sStr.c_str()) != 0 )
				{
					pFlowNode->remove_attribute( pFlowNode->first_attribute( "Name" ) );

					sCstr = pSrcNode->m_sName;

					pFlowNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string( "Name" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
				}

				xml_node<>* pNodeAttrEx = pFlowNode->first_node( "ExtendedAttributes" );
				if ( pNodeAttrEx != NULL )
				{
					xml_node<>* pNodeAttr = pNodeAttrEx->first_node( "ExtendedAttribute" );
					while( pNodeAttr )
					{
						xml_attribute<>* pAttr = pNodeAttr->first_attribute( "Name" );
						if ( pAttr != NULL )
						{
							string sAttrValue = pAttr->value();
							sAttrValue = UTF_82ASCII( sAttrValue );
							if ( stricmp( "PreComponentFileStorePaths" , sAttrValue.c_str() ) == 0 ) //这里处理了前表单、后表单路径不一致的处理，未处理新增路径
							{
								xml_attribute<>* pAttrValue = pNodeAttr->first_attribute( "Value" );
								if ( pAttrValue != NULL )
								{
									sAttrValue = pAttrValue->value();
									sAttrValue = UTF_82ASCII( sAttrValue );
									if ( pSrcNode->m_sPrePaths.CompareNoCase( sAttrValue.c_str() ) != 0 )
									{
										pNodeAttr->remove_attribute( pAttrValue );
										sAttrValue = pSrcNode->m_sPrePaths;
										pNodeAttr->append_attribute( m_doc.allocate_attribute( "Value", m_doc.allocate_string( ASCII2UTF_8( sAttrValue ).c_str() ) ) );
									}
								}
							}
							else if ( stricmp( "PostComponentFileStorePaths" , sAttrValue.c_str() ) == 0 )
							{
								xml_attribute<>* pAttrValue = pNodeAttr->first_attribute( "Value" );
								if ( pAttrValue != NULL )
								{
									sAttrValue = pAttrValue->value();
									sAttrValue = UTF_82ASCII( sAttrValue );
									if ( pSrcNode->m_sPostPaths.CompareNoCase( sAttrValue.c_str() ) != 0 )
									{
										pNodeAttr->remove_attribute( pAttrValue );
										sAttrValue = pSrcNode->m_sPostPaths;
										pNodeAttr->append_attribute( m_doc.allocate_attribute( "Value", m_doc.allocate_string( ASCII2UTF_8( sAttrValue ).c_str() ) ) );
									}
								}
							}
						}
						pNodeAttr = pNodeAttr->next_sibling();
					}
				}
			}

			if ( pSrcNode->m_pParentNode == NULL )
			{
				bExistParentNode = true;
			}
			else
			{
				bExistParentNode = false;
			}

			if ( !bExistParentNode && pSrcNode->m_pParentNode->m_sID.CompareNoCase( sValue.c_str() ) == 0 )
			{
				bExistParentNode = true;

				pActivityNode = pFlowNode->first_node( "Activities" )->first_node( "Activity" );

				while ( pActivityNode )
				{
					sStr.clear();
					xml_node<>* pNodeImplementation = pActivityNode->first_node( "Implementation" );
					if ( pNodeImplementation != NULL )
					{
						xml_node<>* pNodeSubFlow = pNodeImplementation->first_node( "SubFlow" );
						if ( pNodeSubFlow != NULL )
						{
							xml_attribute<>* pAttrSubId = pNodeSubFlow->first_attribute( "Id" );
							if ( pAttrSubId != NULL )
							{
								sStr = pAttrSubId->value();
							}
						}
					}

					sStr = UTF_82ASCII( sStr );

					if ( pSrcNode->m_sID.CompareNoCase( sStr.c_str() ) == 0 )
					{
						pOperationNode = pActivityNode->first_node( "Author" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sAuthor.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sAuthor;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Author节点没有cdata节点" );

							return FALSE;
						}
						
						pOperationNode = pActivityNode->first_node( "Created" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sCreated.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sCreated;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Created节点没有cdata节点" );

							return FALSE;
						}

						pOperationNode = pActivityNode->first_node( "Description" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sDescription.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sDescription;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Description节点没有cdata节点" );

							return FALSE;
						}

						pOperationNode = pActivityNode->first_node( "Version" );

						if ( pOperationNode->first_node() )
						{
							sValue = pOperationNode->first_node()->value();

							sValue = UTF_82ASCII( sValue );

							if ( pSrcNode->m_sVersion.CompareNoCase( sValue.c_str() ) != 0 )
							{
								pOperationNode->remove_all_nodes();

								sCstr = pSrcNode->m_sVersion;

								pOperationNode->append_node( m_doc.allocate_node( rapidxml::node_cdata, m_doc.allocate_string( "" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
							}
						}
						else
						{
							AfxMessageBox( "Version节点没有cdata节点" );

							return FALSE;
						}

						sValue = pActivityNode->first_attribute( "Name" )->value();

						sValue = UTF_82ASCII( sValue );

						if ( pSrcNode->m_sName.CompareNoCase( sValue.c_str() ) != 0 )
						{
							pActivityNode->remove_attribute( pActivityNode->first_attribute( "Name" ) );

							sCstr = pSrcNode->m_sName;

							pActivityNode->append_attribute( m_doc.allocate_attribute( m_doc.allocate_string(  "Name" ), m_doc.allocate_string( ASCII2UTF_8(sCstr).c_str() ) ) );
						}
					}

					pActivityNode = pActivityNode->next_sibling();
				}
			}

			if ( bExistParentNode && bExistNode )
			{
				break;
			}
			pFlowNode = pFlowNode->next_sibling();
		}
	}

	return TRUE;
}

BOOL CXpdl::Save()
{
#if 0

	xml_node<>* pNode = m_doc.first_node(); //添加节点示例

	if ( pNode->type() != rapidxml::node_pi )
	{
		xml_node<>* rot = m_doc.allocate_node( rapidxml::node_pi, m_doc.allocate_string( "xml version='1.0' encoding='utf-8'" ) );  

		m_doc.insert_node( m_doc.first_node(), rot );
	}

#endif

	ConvertToRelative();

	std::ofstream xml_file( m_sFilePath );

	//保存XPDL时需转义按杨波的格式写入
	ConvertToYangboCdata();
	xml_file<<m_doc;
	//写完后反转义还原
	ConvertFromYangboCdata();

	xml_file.close();

	ConvertToAbsolute();
	
	return TRUE;
}

CString CXpdl::GetFilePath( LPCTSTR lpszNodeId , int nType )
{
	if ( lpszNodeId == NULL )
	{
		return "";
	}
	if ( nType == XPDL_FILEPATH_TYPE_PARAM )
	{
		xml_node<>* NodeDataFields = m_doc.first_node( "DataFields" );
		if ( NodeDataFields == NULL )
		{
			return "";
		}
		xml_node<>* NodeData = NodeDataFields->first_node( "DataField" );
		CString sNodeId;
		while( NodeData != NULL )
		{
			xml_attribute<>* AttrTemp = NodeData->first_attribute( "Id" );
			if ( AttrTemp == NULL )
			{
				NodeData = NodeData->next_sibling();
				continue;
			}
			sNodeId = AttrTemp->value();
			if ( sNodeId.CompareNoCase( lpszNodeId ) == 0 )
			{
				xml_node<>* NodeValue = NodeData->first_node( "InitialValue" );
				if ( NodeValue != NULL && NodeValue->first_node() != NULL )
				{
					return NodeValue->first_node()->value();
				}
			}
			NodeData = NodeData->next_sibling();
		}
		return "";
	}
	if ( nType < XPDL_FILEPATH_TYPE_NULL || nType > XPDL_FILEPATH_TYPE_COMPONENT )
	{
		return "";
	}
	xml_node<>* NodePackage = m_doc.first_node( "Package" );
	if ( NodePackage == NULL )
	{
		return "";
	}
	xml_node<>* NodeWorkflowProcesses = NodePackage->first_node( "WorkflowProcesses" );
	if ( NodeWorkflowProcesses == NULL )
	{
		return "";
	}
	xml_node<>* NodeWorkflowProcess = NodeWorkflowProcesses->first_node( "WorkflowProcess" );
	while( NodeWorkflowProcess != NULL )
	{
		xml_attribute<>* AttrWorkflowProcessId = NodeWorkflowProcess->first_attribute( "Id" );
		if ( AttrWorkflowProcessId != NULL )
		{
			string sValue = AttrWorkflowProcessId->value();
			sValue = UTF_82ASCII( sValue );
			if ( stricmp( sValue.c_str(), lpszNodeId ) == 0 )
			{
				xml_node<>* NodeWorkflowAttrExs = NodeWorkflowProcess->first_node( "ExtendedAttributes" );
				if ( NodeWorkflowAttrExs == NULL )
				{
					return "";
				}
				xml_node<>* NodeWorkflowAttr = NodeWorkflowAttrExs->first_node( "ExtendedAttribute" );
				while( NodeWorkflowAttr != NULL )
				{
					xml_attribute<>* AttrWorkflowAttr = NodeWorkflowAttr->first_attribute( "Name" );
					if ( AttrWorkflowAttr != NULL )
					{
						sValue = AttrWorkflowAttr->value();
						sValue = UTF_82ASCII( sValue );
						if ( nType == XPDL_FILEPATH_TYPE_PRECOMPONENT && stricmp( sValue.c_str(), "PreComponentFileStorePaths" ) == 0 )
						{
							xml_attribute<>* AttrWorkflowAttrValue = NodeWorkflowAttr->first_attribute( "Value" );
							if ( AttrWorkflowAttrValue == NULL )
							{
								return "";
							}
							return AttrWorkflowAttrValue->value();
						}
						else if ( nType == XPDL_FILEPATH_TYPE_POSTCOMPONENT &&  stricmp( sValue.c_str(), "PostComponentFileStorePaths" ) == 0 )
						{
							xml_attribute<>* AttrWorkflowAttrValue = NodeWorkflowAttr->first_attribute( "Value" );
							if ( AttrWorkflowAttrValue == NULL )
							{
								return "";
							}
							return AttrWorkflowAttrValue->value();
						}
					}
					NodeWorkflowAttr = NodeWorkflowAttr->next_sibling();
				}
				return "";
			}
		}
		xml_node<>* NodeActivities = NodeWorkflowProcess->first_node( "Activities" );
		if ( NodeActivities == NULL )
		{
			return "";
		}

		xml_node<>* NodeActivity = NodeActivities->first_node( "Activity" );
		while( NodeActivity != NULL )
		{
			xml_attribute<>* AttributeTemp = NodeActivity->first_attribute( "Id" );
			if ( AttributeTemp == NULL )
			{
				NodeActivity = NodeActivity->next_sibling();
				continue;
			}
			CString sNodeId = AttributeTemp->value();
			if ( sNodeId.CompareNoCase( lpszNodeId ) == 0 )
			{
				xml_node<>* NodeAttributes = NodeActivity->first_node( "ExtendedAttributes" );
				if ( NodeAttributes != NULL )
				{
					xml_node<>* NodeAttr = NodeAttributes->first_node( "ExtendedAttribute" );
					while( NodeAttr != NULL )
					{
						AttributeTemp = NodeAttr->first_attribute( "Name" );
						if ( AttributeTemp != NULL )
						{
							CString sAttrName = AttributeTemp->value();
							if ( nType == XPDL_FILEPATH_TYPE_PRECOMPONENT && sAttrName.CompareNoCase( "PreComponentFileStorePaths" ) == 0 )
							{
								AttributeTemp = NodeAttr->first_attribute( "Value" );
								if ( AttributeTemp != NULL )
								{
									return AttributeTemp->value();
								}
							}
							else if ( nType == XPDL_FILEPATH_TYPE_POSTCOMPONENT && sAttrName.CompareNoCase( "PostComponentFileStorePaths" ) == 0 )
							{
								AttributeTemp = NodeAttr->first_attribute( "Value" );
								if ( AttributeTemp != NULL )
								{
									return AttributeTemp->value();
								}
							}
							else if ( nType == XPDL_FILEPATH_TYPE_COMPONENT && sAttrName.CompareNoCase( "ComponentFileStorePaths" ) == 0 )
							{
								AttributeTemp = NodeAttr->first_attribute( "Value" );
								if ( AttributeTemp != NULL )
								{
									return AttributeTemp->value();
								}
							}
						}
						NodeAttr = NodeAttr->next_sibling();
					}
				}
				break;
			}
			NodeActivity = NodeActivity->next_sibling();
		}
		NodeWorkflowProcess = NodeWorkflowProcess->next_sibling();
	}
	return  "";
}

std::wstring CXpdl::Utf82Unicode(const std::string& utf8string)  
{  
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);  
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)  
	{  
		throw std::exception("Invalid UTF-8 sequence.");  
	}  
	if (widesize == 0)  
	{  
		throw std::exception("Error in conversion.");  
	}  

	std::vector<wchar_t> resultstring(widesize);  

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);  

	if (convresult != widesize)  
	{  
		throw std::exception("La falla!");  
	}  

	return std::wstring(&resultstring[0]);  
}

std::string CXpdl::UTF_82ASCII(std::string& strUtf8Code)  
{  
#if !ASC_UTF8_CONVERT

	return strUtf8Code;

#else

	std::string strRet("");  
	//先把 utf8 转为 unicode   
	std::wstring wstr = Utf82Unicode(strUtf8Code);  
	//最后把 unicode 转为 ascii   
	strRet = WideByte2Acsi(wstr);  

	return strRet; 

#endif
} 

std::string CXpdl::WideByte2Acsi(std::wstring& wstrcode)  
{  
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);  
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)  
	{  
		throw std::exception("Invalid UTF-8 sequence.");  
	}  
	if (asciisize == 0)  
	{  
		throw std::exception("Error in conversion.");  
	}  
	std::vector<char> resultstring(asciisize);  
	int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);  

	if (convresult != asciisize)  
	{  
		throw std::exception("La falla!");  
	}  

	return std::string(&resultstring[0]);  
}  

string CXpdl::ASCII2UTF_8(string& strAsciiCode)
{
#if !ASC_UTF8_CONVERT

	return strAsciiCode;

#else

	string strRet("");

	wstring wstr = Acsi2WideByte(strAsciiCode);

	strRet = Unicode2Utf8(wstr);

	return strRet;

#endif
}

std::string  CXpdl::Unicode2Utf8(const std::wstring& widestring)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);

	if (utf8size == 0)
	{
		return "";
	}

	std::vector<char> resultstring(utf8size);

	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

	if (convresult != utf8size)
	{
		return "";
	}

	return std::string(&resultstring[0]);
}

wstring CXpdl::Acsi2WideByte(string& strascii)
{
	int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);

	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		return L"";
	}
	if (widesize == 0)
	{
		return L"";
	}
	std::vector<wchar_t> resultstring(widesize);

	int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);

	if (convresult != widesize)
	{
		return L"";
	}

	return std::wstring(&resultstring[0]);
}

CString CXpdl::RelativeToAbsolute( CString sCurDir, CString sPathIn )
{
	CString sValue;

	if ( sCurDir.GetLength() < 1 || sPathIn.GetLength() < 1 )
	{
		return sPathIn;
	}
	if ( sCurDir.GetAt( sCurDir.GetLength()-1 ) != '\\' )
	{
		sCurDir += "\\";
	}

	if ( sPathIn.GetAt( 1 ) == '\\' )
	{
		sPathIn = sPathIn.Right( sPathIn.GetLength() - 2 );
	}
	if ( sPathIn.GetAt( 0 ) == '\\' )
	{
		sPathIn = sPathIn.Right( sPathIn.GetLength() - 1 );
	}

	sValue = sCurDir + sPathIn;

	return sValue;
}

CString CXpdl::AbsoluteToRelative( CString sFrom, CString sTo )
{
	CString sValue;

	if ( sFrom.GetLength() < 1 || sTo.GetLength() < 1 )
	{
		return sTo;
	}
	if ( sFrom.GetAt( sFrom.GetLength()-1 ) == '\\' )
	{
		sFrom = sFrom.Left( sFrom.GetLength() - 1 );
	}

	sValue = sTo.Right( sTo.GetLength()-sFrom.GetLength() );

	return sValue;
}

BOOL CXpdl::ConvertToAbsolute()
{
	CString sCurDir = m_sFilePath;

	sCurDir = sCurDir.Left( sCurDir.ReverseFind( '\\' ) );

	for ( map< CString, CXpdlNode* >::iterator Iter = m_mapNode.begin(); Iter != m_mapNode.end(); Iter++ )
	{
		if ( Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_COMPONENTNODE || Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_SUBFLOWNODE || Iter->second->m_dwNodeType >= CXpdl::XPDL_NODE_ROOT )
		{
			CXpdlNode* pNode = (CXpdlNode *)Iter->second;

			if ( !pNode->m_sPaths.IsEmpty() )
			{
				pNode->m_sPaths = RelativeToAbsolute( sCurDir, pNode->m_sPaths );
			}
			if ( !pNode->m_sPrePaths.IsEmpty() )
			{
				pNode->m_sPrePaths = RelativeToAbsolute( sCurDir, pNode->m_sPrePaths );
			}
			if ( !pNode->m_sPostPaths.IsEmpty() )
			{
				pNode->m_sPostPaths = RelativeToAbsolute( sCurDir, pNode->m_sPostPaths );
			}
			if ( pNode->m_dwNodeType == CXpdl::XPDL_NODE_COMPONENTNODE )
			{
				CXpdlNodeComponent* pComponent = (CXpdlNodeComponent*)pNode;
				CString sManifest = pComponent->m_cManifestPath;
				if ( !sManifest.IsEmpty() )
				{
					sManifest = RelativeToAbsolute( sCurDir, sManifest );
					StringCbCopy( pComponent->m_cManifestPath, MAX_PATH*2, sManifest );
				}
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}

	for ( map< CString, CXpdlNode* >::iterator It = m_mapParam.begin(); It != m_mapParam.end(); It++ )
	{
		CXpdlNodeParam* pNode = ( CXpdlNodeParam* )It->second;

		if ( pNode->m_sDataType == "REFERENCE" && !pNode->m_sValue.IsEmpty() && pNode->m_bIsSaveInModel )
		{
			pNode->m_sValue = RelativeToAbsolute( sCurDir, pNode->m_sValue );

			if ( !pNode->m_sOutValue.IsEmpty() )
			{
				pNode->m_sOutValue = RelativeToAbsolute( sCurDir, pNode->m_sOutValue );
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}
	return TRUE;
}

BOOL CXpdl::ConvertToRelative()
{
	CString sCurDir = m_sFilePath;

	sCurDir = sCurDir.Left( sCurDir.ReverseFind( '\\' ) );

	for ( map< CString, CXpdlNode* >::iterator Iter = m_mapNode.begin(); Iter != m_mapNode.end(); Iter++ )
	{
		if ( Iter->second->m_dwNodeType == CXpdl::XPDL_NODE_COMPONENTNODE ||  Iter->second->m_dwNodeType == CXpdl::XPDL_NODE_SUBFLOWNODE || Iter->second->m_dwNodeType == CXpdl::XPDL_NODE_ROOT )
		{
			CXpdlNode* pNode = (CXpdlNode*)Iter->second;

			if ( !pNode->m_sPaths.IsEmpty() )
			{
				pNode->m_sPaths = AbsoluteToRelative(  sCurDir, pNode->m_sPaths );
			}
			if ( !pNode->m_sPrePaths.IsEmpty() )
			{
				pNode->m_sPrePaths = AbsoluteToRelative( sCurDir,pNode->m_sPrePaths );
			}
			if ( !pNode->m_sPostPaths.IsEmpty() )
			{
				pNode->m_sPostPaths = AbsoluteToRelative( sCurDir,pNode->m_sPostPaths );
			}
			if ( pNode->m_dwNodeType == CXpdl::XPDL_NODE_COMPONENTNODE )
			{
				CXpdlNodeComponent* pComponent = (CXpdlNodeComponent*)pNode;
				if ( stricmp( pComponent->m_cManifestPath, "" ) != 0 )
				{
					CString sManifest = pComponent->m_cManifestPath;

					sManifest = AbsoluteToRelative( sCurDir,sManifest );

					StringCbCopy( pComponent->m_cManifestPath, MAX_PATH*2, sManifest );
				}
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}

	for ( map< CString, CXpdlNode* >::iterator It = m_mapParam.begin(); It != m_mapParam.end(); It++ )
	{
		CXpdlNodeParam* pNode = ( CXpdlNodeParam* )It->second;

		if ( pNode->m_sDataType == "REFERENCE" && !pNode->m_sValue.IsEmpty() && pNode->m_bIsSaveInModel )
		{
			{
				CString sFilePath = m_sFilePath.Left( m_sFilePath.ReverseFind( '\\' ) );
				sFilePath += "\\";
				sFilePath += pNode->m_sID;
				sFilePath += pNode->m_sValue.Right( pNode->m_sValue.GetLength()-pNode->m_sValue.ReverseFind( '\\' ) );
				if ( !PathFileExists( sFilePath ) )
				{
					if( !FileOperationFun( pNode->m_sValue, sFilePath, FO_COPY ) )
					{
						//continue;
					}
				}
				pNode->m_sValue = sFilePath;
			}
			pNode->m_sValue = AbsoluteToRelative( sCurDir,pNode->m_sValue );

			if ( !pNode->m_sOutValue.IsEmpty() )
			{
				pNode->m_sOutValue = AbsoluteToRelative( sCurDir,pNode->m_sOutValue );
			}

			if ( !SetNode( pNode ) ) return FALSE;
		}
	}

	return TRUE;
}

//CalcXpdlStreamBytes()


void CXpdl::GetXpdlStream( string& sFileStream )
{
	//保存XPDL时需转义按杨波的格式写入
	ConvertToYangboCdata();
	rapidxml::print( std::back_inserter(sFileStream), m_doc, rapidxml::print_no_indenting );
	//写完后反转义还原
	ConvertFromYangboCdata();
	sFileStream = ZTools::UTF8ToMB( sFileStream );
}

CXpdlNode* CXpdl::FindParamNodeByGuid( LPCTSTR lpParentId, LPCTSTR lpGuid )
{
	if ( lpParentId == NULL || lpGuid == NULL )
	{
		return NULL;
	}
	map< CString, CXpdlNode* >::iterator itParentNode = m_mapNode.find( lpParentId );
	if ( itParentNode != m_mapNode.end() )
	{
		CXpdlNode* pParentNode = itParentNode->second;
		if ( pParentNode == NULL )
		{
			return NULL;
		}
		for ( vector< CXpdlNode* >::iterator itNode = pParentNode->m_vecChildNode.begin(); itNode != pParentNode->m_vecChildNode.end(); itNode++ )
		{
			CXpdlNode* pNode = *itNode;
			if ( pNode == NULL )
			{
				return NULL;
			}
			if ( pNode->m_sID.Find( lpGuid ) >= 0 )
			{
				return pNode;
			}
		}
	}
	return NULL;
}

/*UTF-8*/
static LPCTSTR lpszNodeName[] = { "DataField", "VariableToProcess_IN", "VariableToProcess_OUT", "Author", "Created", "Description", "Version", "DataMapping", NULL };

static BOOL IsSkipNode( LPCTSTR lpszName )
{
	if ( lpszName == NULL )
	{
		return TRUE;
	}
	for ( int i = 0;;i++ )
	{
		if ( lpszNodeName[i] == NULL )
		{
			return FALSE;
		}
		if ( stricmp( (char*)lpszNodeName[i], (char*)lpszName ) == 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL CopyRapidXmlNode( xml_node<>* OrgNode, xml_node<>* NewNode, xml_document<>& doc )
{
	xml_attribute<>* AttrNode = OrgNode->first_attribute();
	while( AttrNode != NULL )
	{
		if ( IsSkipNode( AttrNode->name() ) )
		{
			AttrNode = AttrNode->next_attribute();
			continue;
		}
		xml_attribute<>* AttrTemp = doc.allocate_attribute( doc.allocate_string( AttrNode->name() ), doc.allocate_string( AttrNode->value() ) );
		NewNode->append_attribute( AttrTemp );
		AttrNode = AttrNode->next_attribute();
	}
	xml_node<>* ChildNode = OrgNode->first_node();
	while( ChildNode != NULL )
	{
		if ( IsSkipNode( ChildNode->name() ) )
		{
			ChildNode = ChildNode->next_sibling();
			continue;
		}
		{
			xml_attribute<>* NameAttrNode = ChildNode->first_attribute( "Name" );
			if ( NameAttrNode != NULL )
			{
				if ( IsSkipNode( NameAttrNode->value() ) )
				{
					ChildNode = ChildNode->next_sibling();
					continue;
				}
			}
		}
		xml_node<>* NewChildNode = doc.allocate_node( ChildNode->type(), doc.allocate_string( ChildNode->name() ), doc.allocate_string( ChildNode->value() ) );

		if( !CopyRapidXmlNode( ChildNode, NewChildNode, doc ) )
		{
			return FALSE;
		}
		NewNode->append_node( NewChildNode );
		ChildNode = ChildNode->next_sibling();
	}
	return TRUE;
}

BOOL CXpdl::GetSimpleXpdlStream( string & sXpdlStream )
{
	rapidxml::xml_document<> doc;
	xml_node<>* BaseNode = m_doc.first_node( "Package" );
	if ( BaseNode != NULL )
	{
		xml_node<>* FirstNode = doc.allocate_node( BaseNode->type(), doc.allocate_string( BaseNode->name() ), doc.allocate_string( BaseNode->value() ) );
		doc.append_node( FirstNode );
		if ( !CopyRapidXmlNode( BaseNode, FirstNode, doc ) )
		{
			return FALSE;
		}
	}
	rapidxml::print( std::back_inserter(sXpdlStream), doc, rapidxml::print_no_indenting );
	sXpdlStream = ZTools::UTF8ToMB( sXpdlStream );
	return TRUE;
}

BOOL CXpdl::CreateCache( LPCTSTR lpszDir )
{
	if ( lpszDir == NULL )
	{
		return FALSE;
	}
	CString sStoreDir = lpszDir;
	if ( sStoreDir.GetAt( sStoreDir.GetLength()-1 ) != '\\' )
	{
		sStoreDir += "\\";
	}
	if ( !MakeSureDirectoryPathExists( sStoreDir ) )
	{
		return FALSE;
	}
	CString sComponentManifest, sComponentBot, sPreComponent, sPostComponent, sParam;
	for ( map< CString, CXpdlNode* >::iterator itNode = m_mapNode.begin(); itNode != m_mapNode.end(); itNode++ )
	{
		if ( itNode->second->m_dwNodeType == XPDL_NODE_COMPONENTNODE )
		{
			CXpdlNodeComponent* pNode = (CXpdlNodeComponent*)itNode->second;

			{
				sComponentManifest = sStoreDir + pNode->m_sID;
				char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_cManifestPath, '\\' );
				if ( pTemp == NULL )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache manifest[%s] 查找文件名失败\n", pNode->m_cManifestPath );
					continue;
				}
				sComponentManifest += pTemp;
				if ( !FileOperationFun( pNode->m_cManifestPath, sComponentManifest, FO_COPY ) )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache manifest[%s][%s] 复制失败\n", pNode->m_cManifestPath, sComponentManifest );
					continue;
				}
				StringCbCopy( pNode->m_cManifestPath, MAX_PATH*2, sComponentManifest );
				if( !SetNode( pNode ) )
				{
					ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl节点失败 \n", 1 );
					continue;
				}
			}
			{
				sComponentBot = sStoreDir + pNode->m_sID;
				char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_sPaths, '\\' );
				if ( pTemp == NULL )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache bot[%s] 查找文件名失败\n", pNode->m_sPaths );
					continue;
				}
				sComponentBot += pTemp;
				if ( !FileOperationFun( pNode->m_sPaths, sComponentBot, FO_COPY ) )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache bot[%s][%s] 复制失败\n", pNode->m_sPaths, sComponentBot );
					continue;
				}
				pNode->m_sPaths = sComponentBot;
				if( !SetNode( pNode ) )
				{
					ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl bot节点失败 \n", 1 );
					continue;
				}
			}
			if ( !pNode->m_sPrePaths.IsEmpty() )
			{
				sPreComponent = sStoreDir + pNode->m_sID;
				char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_sPrePaths, '\\' );
				if ( pTemp == NULL )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Precomponent[%s] 查找文件名失败\n", pNode->m_sPrePaths );
					continue;
				}
				sPreComponent += pTemp;
				if ( !FileOperationFun( pNode->m_sPrePaths, sPreComponent, FO_COPY ) )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Precomponent[%s][%s] 复制失败\n", pNode->m_sPrePaths, sPreComponent );
					continue;
				}
				pNode->m_sPrePaths = sPreComponent;
				if( !SetNode( pNode ) )
				{
					ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl Precomponent节点失败 \n", 1 );
					continue;
				}
			}
			if ( !pNode->m_sPostPaths.IsEmpty() )
			{
				sPostComponent = sStoreDir + pNode->m_sID;
				char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_sPostPaths, '\\' );
				if ( pTemp == NULL )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Postcomponent[%s] 查找文件名失败\n", pNode->m_sPostPaths );
					continue;
				}
				sPostComponent += pTemp;
				if ( !FileOperationFun( pNode->m_sPostPaths, sPostComponent, FO_COPY ) )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Postcomponent[%s][%s] 复制失败\n", pNode->m_sPostPaths, sPostComponent );
					continue;
				}
				pNode->m_sPostPaths = sPostComponent;
				if( !SetNode( pNode ) )
				{
					ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl Postcomponent节点失败 \n", 1 );
					continue;
				}
			}
		}
		else
		{
			CXpdlNode* pNode = itNode->second;
			if ( !pNode->m_sPrePaths.IsEmpty() )
			{
				if ( pNode->m_dwNodeType != CXpdl::XPDL_NODE_ROOT )
				{
					if ( !pNode->m_sIdOther.IsEmpty() )
					{
						sPreComponent = sStoreDir + pNode->m_sIdOther;
					}
					else
					{
						sPreComponent = sStoreDir + pNode->m_sID;
					}
				}
				else
				{
					sPreComponent = sStoreDir;
				}
				char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_sPrePaths, '\\' );
				if ( pTemp == NULL )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Precomponent[%s] 查找文件名失败\n", pNode->m_sPrePaths );
					continue;
				}
				sPreComponent += pTemp;
				if ( !FileOperationFun( pNode->m_sPrePaths, sPreComponent, FO_COPY ) )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Precomponent[%s][%s] 复制失败\n", pNode->m_sPrePaths, sPreComponent );
					continue;
				}
				pNode->m_sPrePaths = sPreComponent;
				if( !SetNode( pNode ) )
				{
					ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl Precomponent节点失败 \n", 1 );
					continue;
				}
			}
			if ( !pNode->m_sPostPaths.IsEmpty() )
			{
				sPostComponent = sStoreDir + pNode->m_sID;
				char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_sPostPaths, '\\' );
				if ( pTemp == NULL )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Postcomponent[%s] 查找文件名失败\n", pNode->m_sPostPaths );
					continue;
				}
				sPostComponent += pTemp;
				if ( !FileOperationFun( pNode->m_sPostPaths, sPostComponent, FO_COPY ) )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache Postcomponent[%s][%s] 复制失败\n", pNode->m_sPostPaths, sPostComponent );
					continue;
				}
				pNode->m_sPostPaths = sPostComponent;
				if( !SetNode( pNode ) )
				{
					ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl Postcomponent节点失败 \n", 1 );
					continue;
				}
			}
		}
	}

	for ( map< CString, CXpdlNode* >::iterator itParam = m_mapParam.begin(); itParam != m_mapParam.end(); itParam++ )
	{
		CXpdlNodeParam* pNode = (CXpdlNodeParam*)itParam->second;
		if ( pNode->m_sDataType.CompareNoCase( "REFERENCE" ) == 0 && pNode->m_bIsSaveInModel )
		{
			if ( pNode->m_sValue.IsEmpty() )
			{
				ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache [%s] 文件路径为空\n",pNode->m_sID );
				continue;
			}
			sParam = sStoreDir + pNode->m_sID;
			char* pTemp = strrchr( (char*)(LPCTSTR)pNode->m_sValue, '\\' );
			if ( pTemp == NULL )
			{
				ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache param[%s] 查找文件名失败\n", pNode->m_sValue );
				continue;
			}
			sParam += pTemp;
			if ( !FileOperationFun( pNode->m_sValue, sParam, FO_COPY ) )
			{
				ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache param[%s][%s] 复制失败\n", pNode->m_sValue, sParam );
				//continue;
				//拷贝文件失败，继续后续处理
				if ( pNode->m_sDirection.Find( "out" ) > -1 )
				{
					ZTools::WriteZToolsFormatLog( "CXpdl::CreateCache param [%s]是输出类型文件\n", sParam );
				}
			}
			pNode->m_sValue = sParam;
			if( !SetNode( pNode ) )
			{
				ZTools::WriteZToolsLog( "CXpdl::CreateCache 保存xpdl Postcomponent节点失败 \n", 1 );
				continue;
			}
		}
	}

	m_sFilePath = sStoreDir + "main.xpdl";

	if ( !Save() )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CXpdl::LoadStream( LPCTSTR lpszXpdlStream, LPCTSTR lpszXpdlPath  )
{
	if ( lpszXpdlStream == NULL || lpszXpdlPath == NULL )
	{
		return FALSE;
	}
	m_sFilePath = lpszXpdlPath;

	DWORD dwLength = strlen( lpszXpdlStream );

	dwLength += 16;

	m_pBuffer = new char[ dwLength ];

	ZeroMemory( m_pBuffer, dwLength );

	StringCbCopy( m_pBuffer, dwLength, lpszXpdlStream );

	m_doc.parse< parse_full >( m_pBuffer );

	return TRUE;
}

BOOL CXpdl::SetParamReadOnly()
{
	char szSub = '=';
	char szArray = '[';
	CString sDataMapping;
	CString sMapped;
	int	nArrayIndex = -1;
	for ( list< CString >::iterator itDataMapping = m_lstDataMapping.begin(); itDataMapping != m_lstDataMapping.end(); itDataMapping++ )
	{
		sDataMapping = *itDataMapping;
		AfxExtractSubString( sMapped, sDataMapping, 0, szSub );
		if ( sMapped.IsEmpty() )
		{
			continue;
		}
		if ( sMapped.GetLength() <= 3 ) //#{nv9io4x2d773602-3269-4144-b138-50d79190e2f1}
		{
			continue;
		}
		sMapped = sMapped.Left( sMapped.GetLength()-1 );
		sMapped = sMapped.Right( sMapped.GetLength()-2 );
		nArrayIndex = sMapped.Find( szArray );
		if ( nArrayIndex >= 0 )
		{
			sMapped = sMapped.Left( nArrayIndex );
		}
		CXpdlNodeParam* pNode = (CXpdlNodeParam*)Find( sMapped );
		if ( pNode == NULL )
		{
			continue;
		}
		pNode->m_bReadOnly = TRUE;
	}
	return TRUE;
}
