#include "StdAfx.h"
#include "WebCommnunicationClient.h"
#include "Tools.h"
#include "UserDefine.h"
#include "UtilityFunc.h"
#include "PeraPDWebContainer.h"
#include "DlgMain.h"
#include "AttachWindowClient.h"
#include "WebEventData.h"
#include "PeraPDWeb.h"


CWebCommnunicationClient::CWebCommnunicationClient(void)
{
}


CWebCommnunicationClient::~CWebCommnunicationClient(void)
{
}

BOOL CWebCommnunicationClient::Send( HWND hWnd, CWebEventData* pData )
{
	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Send begin" );
	if ( !IsWindow( hWnd ) )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Send �����Ч" );
		return FALSE;
	}
	if ( pData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Send ������Ч" );
		return FALSE;
	}
	CString sData = pData->GetJsonStr();
	ZTools::WriteZToolsFormatLog( "CWebCommnunicationClient ������Ϣ [%s]", sData );
	SendCopyData( hWnd, CPMSG_WEBEVENT, (LPVOID)(LPCTSTR)sData, sData.GetLength() );
	ZTools::WriteZToolsLog( "CWebCommnunicationClient ������Ϣ ���" );
	return TRUE;
}

BOOL CWebCommnunicationClient::Receive( LPCTSTR lpData )
{
	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Receive begin" );
	if( lpData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Receive lpData == NULL" );
		return FALSE;
	}
	CWebEventData* pWebEventData = new CWebEventData();
	if ( !pWebEventData->Parse( lpData ) )
	{
		delete pWebEventData; pWebEventData = NULL;
		ZTools::WriteZToolsFormatLog( "CWebCommnunicationClient::Receive ����ʧ�� [%s]", lpData );
		return FALSE;
	}
	switch ( pWebEventData->m_dwWebType )
	{
	case WEBTYPE_RUNFLOW:
		{
			switch ( pWebEventData->m_dwWebEvent )
			{
			case WE_RUNFLOW_CANCEL:
			case WE_RUNFLOW_SUBFLOWID:
				//ͬ������
				Handle( pWebEventData );
				delete pWebEventData;
				pWebEventData = NULL;

				ZTools::WriteZToolsLog( "CWebCommnunicationClient::Receive ͬ������ end" );
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

	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Receive end" );
	return TRUE;
}

BOOL CWebCommnunicationClient::Handle( CWebEventData* pData )
{
	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() begin" );
	if ( pData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() ����Ϊ��" );
		return FALSE;
	}
	CDlgMain* pDlg = (CDlgMain*)theApp.m_pMainWnd;
	if ( pDlg == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() theApp.m_pMainWnd Ϊ��" );
		return FALSE;
	}
	if ( !IsWindow( pDlg->GetSafeHwnd() ) )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() dlgmain ��Ϊ����" );
		return FALSE;
	}
	if ( pDlg->m_pWeb == NULL || !IsWindow( pDlg->m_pWeb->GetSafeHwnd() ) )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() m_pWeb ��Ϊ����" );
		return FALSE;
	}
	switch( pData->m_dwWebType )
	{
	case WEBTYPE_RUNFLOW:
		{
			switch( pData->m_dwWebEvent )
			{
			case WE_RUNFLOW_CANCEL:
				{
					if( !( (CPeraPDWeb*)pDlg->m_pWeb )->CancelRunFlow() )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() m_pWeb Cancel ����ʧ��" );
						return FALSE;
					}
				}
				break;
			case WE_RUNFLOW_SUBFLOWID:
				{
					CString sFlowId = ( (CPeraPDWeb*)pDlg->m_pWeb )->getClientSubFlowId();
					if ( sFlowId.IsEmpty() )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() m_pWeb getClientSubFlowId ����ʧ��" );
						return FALSE;
					}
					if ( !IsWindow( g_aw.m_hHostWnd ) )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() �����ھ����Ч" );
						return FALSE;
					}

					CWERunFlowSubFlowId subFlowId;
					subFlowId.m_dwWebEvent = WE_RUNFLOW_SUBFLOWID;
					subFlowId.m_dwWebType = WEBTYPE_RUNFLOW;
					subFlowId.m_sWebId = g_aw.m_sId;
					subFlowId.m_sSubFlowId = sFlowId;
					if( !this->Send( g_aw.m_hHostWnd, &subFlowId ) )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() ����ʧ��" );
						return FALSE;
					}
				}
				break;
			default:
				ZTools::WriteZToolsFormatLog( "CWebCommnunicationClient::Handle WebEvent Error[%d]", pData->m_dwWebEvent );
				return FALSE;
			}
		}
		break;
	default:
		ZTools::WriteZToolsFormatLog( "CWebCommnunicationClient::Handle WebType Error[%d]", pData->m_dwWebType );
		return FALSE;
	}
	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle end" );
	return TRUE;
}

CWebCommnunicationClient theCommClient;
