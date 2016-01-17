#include "StdAfx.h"
#include "CxWebNavigate.h"
#include "PeraDCWebContainer.h"
#include "Tools.h"
#include "FileFunc.h"
#include "UserDefine.h"

BEGIN_MESSAGE_MAP(CCxWebNavigate, CBaseHtmlDialog)
	ON_BN_CLICKED(IDOK, &CCxWebNavigate::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCxWebNavigate::OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCxWebNavigate, CBaseHtmlDialog)
END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CCxWebNavigate, CBaseHtmlDialog)
	ON_EVENT(CCxWebNavigate, AFX_IDC_BROWSER, 271, NavigateErrorExplorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

IMPLEMENT_DYNAMIC(CCxWebNavigate, CBaseHtmlDialog)

CCxWebNavigate::CCxWebNavigate(CWnd* pParent /*=NULL*/)
	: CBaseHtmlDialog(CCxWebNavigate::IDD, pParent)
{
}

CCxWebNavigate::~CCxWebNavigate(void)
{
}

void CCxWebNavigate::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}

void CCxWebNavigate::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//按 ESC 不关闭对话框
	//注意：此消息将自动屏蔽 WM_CLOSE 消息
}

void CCxWebNavigate::DoDataExchange(CDataExchange* pDX)
{
	CBaseHtmlDialog::DoDataExchange(pDX);
}

BOOL CCxWebNavigate::OnInitDialog()
{
	CBaseHtmlDialog::OnInitDialog();
	return TRUE;
}

BOOL CCxWebNavigate::OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
	, CComVariant& varResult )
{
	varResult.Clear();

	CString str;
	WEBCALLBACK_METHOD_PARAM_MAP::iterator itrParam = mapParams.begin();

	switch (nCode)
	{
	//////////////////////////////////////////////////////////////////////////
	case WCB_LOADWEB_FINISHED:
		{
			ZTools::WriteZToolsFormatLog("WebPage CCxWebNavigate::OnWebCallback %s\r\n"
				, "WCB_SVG_LOADED");

			//
			if ( theApp.m_pStartEvent != NULL )
			{
				if ( IsWindow( theApp.m_pStartEvent->m_hWndMain ) )
				{
					switch ( theApp.m_pStartEvent->GetModuleFlag() )
					{
					case CPeraWebPageMgr::FW_TABVIEW_TASK_MODEL:
						::PostMessage( theApp.m_pStartEvent->m_hWndMain, WM_WEBPAGE_LOADWEB_FINISHED, 0, 0 );
						break;

					default:
						break;
					}
				}
			}
		}
		break;
	case WCB_SHOW_PROGRESS:
		{
			ZTools::WriteZToolsFormatLog("WebPage CCxWebNavigate::OnWebCallback %s\r\n"
				, "WCB_SHOW_PROGRESS,参数个数：%d",mapParams.size());
			
			BOOL bShow = FALSE;
			if (mapParams.size()==1)
			{
				CString strShow = itrParam->second;
				ZTools::WriteZToolsFormatLog("WCB_SHOW_PROGRESS 消息参数值=%s",strShow);
				if (strShow.CompareNoCase("true")==0)
					bShow = TRUE;
				else if (strShow.CompareNoCase("false")==0)
					bShow = FALSE;
				else
					bShow = FALSE;
			}
			else
			{
				ZTools::WriteZToolsFormatLog("WCB_SHOW_PROGRESS 消息参数个数正确！");
				return FALSE;
			}

			if ( theApp.m_pStartEvent != NULL )
			{
				if ( IsWindow( theApp.m_pStartEvent->m_hWndMain ) )
				{
					::PostMessage( theApp.m_pStartEvent->m_hWndMain, WM_PROGRESS_SHOW, bShow, 0 );
					break;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
	default:
		return FALSE;
	}

	return TRUE;
}

void CCxWebNavigate::OnWebPageEvent( CCxWebPageEvent * pEvent )
{
	if ( pEvent == NULL ) return;
	return;
}


void CCxWebNavigate::NavigateErrorExplorer1( LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel )
{
	CString sUrl(URL->bstrVal);	
	CString sUrlLower = sUrl;
	sUrlLower.MakeLower();
	if (sUrlLower.Compare("http:///") == 0 || sUrlLower.CompareNoCase("javascript:void(0)") == 0)
	{
		return;
	}
	else if (pDisp != m_pBrowserApp)
	{
		return;
	}
	else if (StatusCode->vt == VT_I4 && StatusCode->lVal == 200)
	{
		return;
	}
	else
	{
//#ifdef _DEBUG_LOG
//		g_pDlgIMClient->WriteMyLog(ZTools::FormatString("CDlgPopupIE::NavigateErrorExplorer1\n<br>%s\n<br>StatusCode: %d 0x%x", sUrl, StatusCode->intVal, StatusCode->intVal).c_str());
//#endif // _DEBUG_LOG
		CString strPath = GetExecDir();
		strPath += "\\Res\\html\\nopage.html";
		//sUrl.Format("%shtml/nopage.html", g_pDlgIMClient->m_sExePath);
		Navigate(strPath);
	}
}
STDMETHODIMP CCxWebNavigate::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return S_OK; //屏蔽右键菜单
}