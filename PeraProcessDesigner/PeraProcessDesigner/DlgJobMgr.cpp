// DlgJobMgr.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgJobMgr.h"
#include "afxdialogex.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "UtilityFunc.h"
#include "ConfigFunc.h"


//////////////////////////////////////////////////////////////////////////
// CDlgJobMgr �Ի���

BEGIN_MESSAGE_MAP(CDlgJobMgr, CBaseHtmlDialog)
	ON_BN_CLICKED(IDOK, &CDlgJobMgr::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgJobMgr::OnBnClickedCancel)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDlgJobMgr, CBaseHtmlDialog)
END_DISPATCH_MAP()

void CDlgJobMgr::DoDataExchange(CDataExchange* pDX)
{
	CBaseHtmlDialog::DoDataExchange(pDX);
}

IMPLEMENT_DYNAMIC(CDlgJobMgr, CBaseHtmlDialog)

CDlgJobMgr::CDlgJobMgr(CWnd* pParent /*=NULL*/)
	: CBaseHtmlDialog(CDlgJobMgr::IDD, pParent)
{

}

CDlgJobMgr::~CDlgJobMgr()
{
}

void CDlgJobMgr::OnBnClickedOk()
{
	//�� Enter ���رնԻ���
}

void CDlgJobMgr::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//�� ESC ���رնԻ���
	//ע�⣺����Ϣ���Զ����� WM_CLOSE ��Ϣ
}

BOOL CDlgJobMgr::OnInitDialog()
{
	CBaseHtmlDialog::OnInitDialog();
	Navigate( "about:blank" );
	//if ( theGlobalConfig.m_strSvgUrl.IsEmpty() ) theGlobalConfig.m_strSvgUrl = "about:blank";
	//this->Navigate( theGlobalConfig.m_strSvgUrl );
	return TRUE;
}


BOOL CDlgJobMgr::OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
	, CComVariant& varResult )
{
	varResult.Clear();
	CString str;
	WEBCALLBACK_METHOD_PARAM_MAP::iterator itrParam = mapParams.begin();

	switch (nCode)
	{
	case WCB_SHOW_RUNFLOW_VIEW:
		{
			if (mapParams.size() != 1)
			{
				//lc,���ʻ���������
				varResult = _bstr_t("������������ȷ").GetBSTR();
				return FALSE;
			}

			CString * pJsonObjStr = new CString();
			itrParam = mapParams.begin();
			//*pJsonObjStr = itrParam->second.bstrVal;
			//lc,���ʻ���������
			*pJsonObjStr = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);

			::PostMessage( theApp.m_pMainWnd->GetSafeHwnd(), WM_SHOW_RUNFLOW_VIEW, (WPARAM)pJsonObjStr, 0 );
		}
		break;

	////////////////////////////////////////////////////////////////////////////
	default:
		FALSE;
	}
	return TRUE;
}

CString CDlgJobMgr::LoadUrl( void )
{
	CString strUrl;
	strUrl = GetConfig("PeraProcessDesigner", "historymonitor"); //��ȡ��ҵ����url
	if (
		strUrl.IsEmpty()
		|| theApp.m_LoginData.m_strTicket.IsEmpty()
		)
	{
		this->Navigate("about:blank");
		return "";
	}

	AttachTicket(strUrl, theApp.m_LoginData.m_strTicket);
	this->Navigate((LPCTSTR)strUrl);
	return strUrl;
}

void CDlgJobMgr::JsMethod_reloadGrid( void )
{
	ZTools::WriteZToolsFormatLog("CDlgJobMgr::JsMethod_reloadGrid %s\r\n"
		, "invoke");

	CWebMethodData wmd;
	wmd.m_sMethodName = "reloadGrid";
	if ( !InvokeJsMethod( &wmd ) )
	{
		//ʧ��
	}
}

BOOL CDlgJobMgr::PreTranslateMessage( MSG* pMsg )
{
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	if ( IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) )
	{
		if ( WebHotKeyHandle( pMsg, theApp.m_pMainWnd->GetSafeHwnd() ) )
		{
			return TRUE;
		}
	}

	return CBaseHtmlDialog::PreTranslateMessage(pMsg);
}
