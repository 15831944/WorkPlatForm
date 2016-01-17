#include "StdAfx.h"
#include "WebCommunicationHost.h"
#include "Tools.h"
#include "PeraProcessDesigner.h"
#include "WebEventData.h"
#include "DlgRunFlow.h"
#include "UtilityFunc.h"


CWebCommunicationHost::CWebCommunicationHost(void)
{
}


CWebCommunicationHost::~CWebCommunicationHost(void)
{
}

BOOL CWebCommunicationHost::Send( HWND hWnd, CWebEventData* pData )
{
	ZTools::WriteZToolsLog( "CWebCommunicationHost::Send begin" );
	if ( !IsWindow( hWnd ) )
	{
		ZTools::WriteZToolsLog( "CWebCommunicationHost::Send �����Ч" );
		return FALSE;
	}
	if ( pData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommunicationHost::Send ������Ч" );
		return FALSE;
	}
	CString sData = pData->GetJsonStr();
	ZTools::WriteZToolsFormatLog( "CWebCommunicationHost ������Ϣ [%s]", sData );
	SendCopyData( hWnd, CPMSG_WEBEVENT, (LPVOID)(LPCTSTR)sData, sData.GetLength() );
	ZTools::WriteZToolsLog( "CWebCommunicationHost Send end" );
	return TRUE;
}

BOOL CWebCommunicationHost::Receive( LPCTSTR lpData )
{
	ZTools::WriteZToolsLog( "CWebCommunicationHost::Receive begin" );
	if( lpData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommunicationHost::Receive lpData == NULL" );
		return FALSE;
	}
	CWebEventData* pWebEventData = new CWebEventData();
	if ( !pWebEventData->Parse( lpData ) )
	{
		ZTools::WriteZToolsFormatLog( "CWebCommunicationHost::Receive ����ʧ�� [%s]", lpData );
		delete pWebEventData; pWebEventData = NULL;
		return FALSE;
	}

	switch( pWebEventData->m_dwWebType )
	{
	case WEBTYPE_RUNFLOW:
		{
			switch( pWebEventData->m_dwWebEvent )
			{
			case WE_RUNFLOW_COMPLETED:
			case WE_RUNFLOW_SUBFLOWID:
				{
					//ͬ������
					Handle( pWebEventData );
					delete pWebEventData; pWebEventData = NULL;
					ZTools::WriteZToolsLog( "CWebCommunicationHost::Receive ͬ������ end" );
				}
				return TRUE;
			default:
				break;
			}
		}
		break;
	default: 
		break;
	}

	//�����¼��첽����
	::PostMessageA( GetMainWnd(), WM_WEBEVENT_ASYNC, (WPARAM)pWebEventData, 0 );

	ZTools::WriteZToolsLog( "CWebCommunicationHost::Receive end" );
	return TRUE;
}

BOOL CWebCommunicationHost::Handle( CWebEventData* pData )
{
	ZTools::WriteZToolsLog( " CWebCommunicationHost::Handle() begin" );
	if ( pData == NULL )
	{
		ZTools::WriteZToolsLog( " CWebCommunicationHost::Handle() ����Ϊ��" );
		return FALSE;
	}
	switch( pData->m_dwWebType )
	{
	case WEBTYPE_RUNFLOW:
		{
			switch( pData->m_dwWebEvent )
			{
			case WE_RUNFLOW_COMPLETED:
				{
					CDlgRunFlow * pDlgRunFlow = (CDlgRunFlow *)CWnd::FromHandle( theApp.m_hwndRunFlow );
					pDlgRunFlow->m_bFinished = TRUE;
					pDlgRunFlow->m_wndCancel.EnableWindow(TRUE);
					pDlgRunFlow->m_wndOk.EnableWindow(TRUE);
				}
				break;
			case WE_RUNFLOW_SUBFLOWID:
				{
					CWERunFlowSubFlowId subFlowId;
					subFlowId.CopyEventHeaderFrom( pData );
					if( !subFlowId.ParseData( pData->m_sData ) )
					{
						ZTools::WriteZToolsLog( "CWebCommunicationHost::Handle WE_RUNFLOW_SUBFLOWID ����ʧ��" );
						//return FALSE;
					}
					theApp.m_processMgr.m_sFlowId = subFlowId.m_sSubFlowId;
					theApp.m_bGetClientSubFlowId = TRUE;
				}
			default:
				ZTools::WriteZToolsFormatLog( "CWebCommunicationHost::Handle WebEvent Error[%d]", pData->m_dwWebEvent );
				return FALSE;
			}
		}
		break;
	default:
		ZTools::WriteZToolsFormatLog( "CWebCommunicationHost::Handle WebType Error[%d]", pData->m_dwWebType );
		return FALSE;
	}
	ZTools::WriteZToolsLog( " CWebCommunicationHost::Handle() end" );
	return TRUE;
}

CWebCommunicationHost theCommHost;