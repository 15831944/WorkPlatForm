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
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Send 句柄无效" );
		return FALSE;
	}
	if ( pData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Send 数据无效" );
		return FALSE;
	}
	CString sData = pData->GetJsonStr();
	ZTools::WriteZToolsFormatLog( "CWebCommnunicationClient 发送消息 [%s]", sData );
	SendCopyData( hWnd, CPMSG_WEBEVENT, (LPVOID)(LPCTSTR)sData, sData.GetLength() );
	ZTools::WriteZToolsLog( "CWebCommnunicationClient 发送消息 完成" );
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
		ZTools::WriteZToolsFormatLog( "CWebCommnunicationClient::Receive 解析失败 [%s]", lpData );
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
				//同步处理
				Handle( pWebEventData );
				delete pWebEventData;
				pWebEventData = NULL;

				ZTools::WriteZToolsLog( "CWebCommnunicationClient::Receive 同步处理 end" );
				return TRUE;
			default:
				break;
			}
		}
		break;
	
	default:
		break;
	}

	//其他事件异步处理
	::PostMessageA( GetMainWnd(), WM_WEBEVENT_ASYNC, (WPARAM)pWebEventData, 0 );

	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Receive end" );
	return TRUE;
}

BOOL CWebCommnunicationClient::Handle( CWebEventData* pData )
{
	ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() begin" );
	if ( pData == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() 参数为空" );
		return FALSE;
	}
	CDlgMain* pDlg = (CDlgMain*)theApp.m_pMainWnd;
	if ( pDlg == NULL )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() theApp.m_pMainWnd 为空" );
		return FALSE;
	}
	if ( !IsWindow( pDlg->GetSafeHwnd() ) )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() dlgmain 不为窗口" );
		return FALSE;
	}
	if ( pDlg->m_pWeb == NULL || !IsWindow( pDlg->m_pWeb->GetSafeHwnd() ) )
	{
		ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() m_pWeb 不为窗口" );
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
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() m_pWeb Cancel 调用失败" );
						return FALSE;
					}
				}
				break;
			case WE_RUNFLOW_SUBFLOWID:
				{
					CString sFlowId = ( (CPeraPDWeb*)pDlg->m_pWeb )->getClientSubFlowId();
					if ( sFlowId.IsEmpty() )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() m_pWeb getClientSubFlowId 调用失败" );
						return FALSE;
					}
					if ( !IsWindow( g_aw.m_hHostWnd ) )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() 主窗口句柄无效" );
						return FALSE;
					}

					CWERunFlowSubFlowId subFlowId;
					subFlowId.m_dwWebEvent = WE_RUNFLOW_SUBFLOWID;
					subFlowId.m_dwWebType = WEBTYPE_RUNFLOW;
					subFlowId.m_sWebId = g_aw.m_sId;
					subFlowId.m_sSubFlowId = sFlowId;
					if( !this->Send( g_aw.m_hHostWnd, &subFlowId ) )
					{
						ZTools::WriteZToolsLog( "CWebCommnunicationClient::Handle() 发送失败" );
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
