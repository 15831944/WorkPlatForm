#include "StdAfx.h"
#include "perapdweb.h"
#include "PeraPDWebContainer.h"
#include "Tools.h"
#include "UserDefine.h"
#include "AttachWindowClient.h"
#include "WebEventData.h"
#include "WebCommnunicationClient.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

BEGIN_MESSAGE_MAP(CPeraPDWeb, CBaseHtmlDialog)
	ON_BN_CLICKED(IDOK, &CPeraPDWeb::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPeraPDWeb::OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPeraPDWeb, CBaseHtmlDialog)
END_DISPATCH_MAP()

IMPLEMENT_DYNAMIC(CPeraPDWeb, CBaseHtmlDialog)

CPeraPDWeb::CPeraPDWeb(CWnd* pParent /*=NULL*/)
	: CBaseHtmlDialog(CPeraPDWeb::IDD, pParent)
{
}

CPeraPDWeb::~CPeraPDWeb(void)
{
}

void CPeraPDWeb::OnBnClickedOk()
{
	//�� Enter ���رնԻ���
}

void CPeraPDWeb::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//�� ESC ���رնԻ���
	//ע�⣺����Ϣ���Զ����� WM_CLOSE ��Ϣ
}

void CPeraPDWeb::DoDataExchange(CDataExchange* pDX)
{
	CBaseHtmlDialog::DoDataExchange(pDX);
}

BOOL CPeraPDWeb::OnInitDialog()
{
	CBaseHtmlDialog::OnInitDialog();
	return TRUE;
}

BOOL CPeraPDWeb::OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
	, CComVariant& varResult )
{
	varResult.Clear();

	CString str;
	WEBCALLBACK_METHOD_PARAM_MAP::iterator itrParam = mapParams.begin();

	switch (nCode)
	{
		//////////////////////////////////////////////////////////////////////////
		//���½ṹ��
	case WCB_RUNFLOW_COMPLETED:
		{
			if (mapParams.size() != 1)
			{
				//varResult = "������������ȷ";
				//lc,���ʻ���������
				varResult = _bstr_t("������������ȷ").GetBSTR();
				return FALSE;
			}

			itrParam = mapParams.begin();
			//str = itrParam->second.bstrVal;
			//lc,���ʻ���������
			str = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);

			ZTools::WriteZToolsFormatLog("CPeraPDWeb::OnWebCallback svg֪ͨ���н��� %s\r\n", str );

			if ( g_aw.m_nExeFlag == WEBTYPE_RUNFLOW )
			{
				CWERunFlowCompleted runFlowCompleted;
				if( !runFlowCompleted.ParseData( str ) )
				{
					ZTools::WriteZToolsLog( "CPeraPDWeb::OnWebCallback svg WCB_RUNFLOW_COMPLETED ����ʧ��" );
				}
				runFlowCompleted.m_dwWebType = WEBTYPE_RUNFLOW;
				runFlowCompleted.m_dwWebEvent = WE_RUNFLOW_COMPLETED;
				runFlowCompleted.m_sWebId = g_aw.m_sId;
				theCommClient.Send( g_aw.m_hHostWnd, &runFlowCompleted );
				ZTools::WriteZToolsLog( "CPeraPDWeb::OnWebCallback svg WCB_RUNFLOW_COMPLETED �������" );
			}
			else
			{
				ZTools::WriteZToolsLog( "CPeraPDWeb::OnWebCallback svg WCB_RUNFLOW_COMPLETED webtype ��ƥ��" );
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	case WCB_WEB_LOADED:
		{
			ZTools::WriteZToolsFormatLog("WebPage CPeraPDWeb::OnWebCallback %s\r\n", "WCB_WEB_LOADED");

			if ( IsWindow( g_aw.m_hHostWnd )  )
			{
				if ( g_aw.m_nExeFlag == WEBTYPE_RUNPROCESS || g_aw.m_nExeFlag == WEBTYPE_RUNFLOW )
				{
					::PostMessage( g_aw.m_hHostWnd, WM_WEB_LOADED, 0, 0 );
				}
				else
				{
					ZTools::WriteZToolsLog( "WebPage CPeraPDWeb::OnWebCallback WCB_WEB_LOADED webtype ��ƥ��" );
				}
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CPeraPDWeb::CancelRunFlow( void )
{
	ZTools::WriteZToolsFormatLog("CPeraPDWeb::Cancel begin >>>>> %s"
		, "\r\n" );

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "cancelFlowProcess";
	//
	if ( !InvokeJsMethod( &wmd ) )
	{
		ZTools::WriteZToolsFormatLog( "���� js ���� cancelFlowProcess ʧ�� %s" , wmd.m_sInfo );

		return FALSE;
	}
	if ( !wmd.m_sResult.IsEmpty() )
	{
		AfxMessageBox( wmd.m_sResult );
		return FALSE;
	}

	ZTools::WriteZToolsFormatLog( "CPeraPDWeb::Cancel end <<<<<< %s" , "\r\n" );

	return TRUE;
}

CString CPeraPDWeb::getClientSubFlowId( void )
{
	ZTools::WriteZToolsFormatLog("CPeraPDWeb::getClientSubFlowId begin >>>>> %s"
		, "\r\n" );

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "getClientSubFlowId";
	//
	if ( !InvokeJsMethod( &wmd ) )
	{
		ZTools::WriteZToolsFormatLog("���� js ���� getClientSubFlowId ʧ�� %s"
			, "\r\n" );

		return "";
	}

	ZTools::WriteZToolsFormatLog("���� js ���� getClientSubFlowId ���� %s\r\n"
		, wmd.m_sResult );

	Json::Reader reader;
	Json::Value obj;
	try
	{
		if (!reader.parse((char*)(LPCTSTR) wmd.m_sResult, obj, false)) return "";
		if (!obj.isObject()) return "";

		CString sFlowId = ( obj["flowId"].isString() ) ? obj["flowId"].asCString() : _T("");
		CString sOptMsg = ( obj["optMsg "].isString() ) ? obj["optMsg "].asCString() : _T("");
		if ( !sOptMsg.IsEmpty() || sFlowId.IsEmpty() ) return "";
		return sFlowId;
	}
	catch (...)
	{
		return "";
	}

	ZTools::WriteZToolsFormatLog("CPeraPDWeb::getClientSubFlowId end <<<<<< %s"
		, "\r\n" );
	return "";
}

BOOL CPeraPDWeb::PreTranslateMessage( MSG* pMsg )
{
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	if ( g_aw.m_hHostWnd != NULL && IsWindow( g_aw.m_hHostWnd ) )
	{
		if ( WebHotKeyHandle( pMsg, g_aw.m_hHostWnd ) )
		{
			return TRUE;
		}
	}

	return CBaseHtmlDialog::PreTranslateMessage(pMsg);
}
