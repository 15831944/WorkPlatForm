// DlgSvg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgSvg.h"
#include "afxdialogex.h"
#include "WebDataDefine.h"
#include "WebUtility.h"
#include "Tools.h"
#include "PeraProcessDesignerUtility.h"
#include "CxStructTreeNodeParam.h"


// CDlgSvg 对话框

BEGIN_MESSAGE_MAP(CDlgSvg, CBaseHtmlDialog)
	ON_BN_CLICKED(IDOK, &CDlgSvg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSvg::OnBnClickedCancel)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDlgSvg, CBaseHtmlDialog)
END_DISPATCH_MAP()

IMPLEMENT_DYNAMIC(CDlgSvg, CBaseHtmlDialog)

CDlgSvg::CDlgSvg(CWnd* pParent /*=NULL*/)
	: CBaseHtmlDialog(CDlgSvg::IDD, pParent)
{

}

CDlgSvg::~CDlgSvg()
{
}

void CDlgSvg::DoDataExchange(CDataExchange* pDX)
{
	CBaseHtmlDialog::DoDataExchange(pDX);
}


BOOL CDlgSvg::OnInitDialog()
{
	CBaseHtmlDialog::OnInitDialog();
	m_ToolTip.Create (this);
	CString strPage = GetSvgDefaultPage();
	if ( !IsAccessibleFile(strPage) ) strPage = "about:blank";
	Navigate( strPage );
	//if ( theGlobalConfig.m_strSvgUrl.IsEmpty() ) theGlobalConfig.m_strSvgUrl = "about:blank";
	//this->Navigate( theGlobalConfig.m_strSvgUrl );
	return TRUE;
}


void CDlgSvg::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}

void CDlgSvg::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//按 ESC 不关闭对话框
	//注意：此消息将自动屏蔽 WM_CLOSE 消息
}

BOOL CDlgSvg::OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
	, CComVariant& varResult )
{
	varResult.Clear();
	CString str;
	WEBCALLBACK_METHOD_PARAM_MAP::iterator itrParam = mapParams.begin();
//	CStructTreeNodeData * pNode = NULL;

	switch (nCode)
	{
		//////////////////////////////////////////////////////////////////////////
		//更新结构树
	case WCB_SVGEVENT:
		{
			if (mapParams.size() != 2)
			{
				//varResult = "参数个数不正确";
				//lc,国际化乱码问题
				varResult = _bstr_t("参数个数不正确").GetBSTR();
				return FALSE;
			}

			CString strJsonData, strOperatorFlag;
			itrParam = mapParams.begin();
			//strJsonData = itrParam->second.bstrVal;
			//lc,国际化乱码问题
			strJsonData = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);
			itrParam++;
			//strOperatorFlag = itrParam->second.bstrVal;
			//lc,国际化乱码问题
			strOperatorFlag = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);
			ZTools::WriteZToolsFormatLog("UpdateStructTree Flag=%s, JsonData=\r\n%s\r\n"
				, strOperatorFlag
				, strJsonData);

			theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent(strOperatorFlag, strJsonData);
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//获取信息
	case WCB_GET_INFO:
		{
			if (mapParams.size() != 1)
			{
				//varResult = "参数个数不正确";
				//lc,国际化乱码问题
				varResult = _bstr_t("参数个数不正确").GetBSTR();
				return FALSE;
			}

			CString strJsonData;
			itrParam = mapParams.begin();
			//strJsonData = itrParam->second.bstrVal;
			//lc,国际化乱码问题
			strJsonData = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);
			CString strResult = WebGetInfo( strJsonData );
			//varResult = (LPCTSTR)strResult;
			//lc,国际化乱码问题
			varResult = _bstr_t(strResult).GetBSTR();
		}
		break;

		//////////////////////////////////////////////////////////////////////////
	case WCB_WEB_LOADED:
		{
			ZTools::WriteZToolsFormatLog("WebPage CDlgSvg::OnWebCallback %s\r\n"
				, "WCB_WEB_LOADED");
		}
		break;

		//////////////////////////////////////////////////////////////////////////
	default:
		return FALSE;
	}
	return TRUE;
}

//****************************************************************************************
void CDlgSvg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CBaseHtmlDialog::OnMouseMove(nFlags, point);
}

BOOL CDlgSvg::JsMethod_parseSubProcess( LPCTSTR lpszData )
{
	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "parseSubProcess";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonStr";
	pParam->m_sValue = lpszData;
	wmd.m_vParams.push_back(pParam);
	//
	if ( !InvokeJsMethod( &wmd ) )
	{
		return FALSE;
	}
	return TRUE;
}

CString CDlgSvg::JsMethod_runDataMappingExpr( LPCTSTR lpszData )
{
	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "runDataMappingExpr";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "formula";
	pParam->m_sValue = lpszData;
	wmd.m_vParams.push_back(pParam);
	//
	if ( !InvokeJsMethod( &wmd ) )
	{
		return "";
	}
	return wmd.m_sResult;
}

BOOL CDlgSvg::JsMethod_paramUpdate( CCxStructTreeNodeParam * pParamNode )
{
	if ( pParamNode == NULL ) return FALSE;

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//方法名称
	wmd.m_sMethodName = "paramUpdate";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonObj";
	pParam->m_sValue = pParamNode->GetUpdateParamJsonStr();
	wmd.m_vParams.push_back(pParam);
	if ( !InvokeJsMethod( &wmd ) ) return FALSE;
	//pParamNode->m_bModified = TRUE;
	//pParamNode->UpdateStatus( TRUE );
	return TRUE;
}

CString CDlgSvg::JsMethod_runComponent( CCxStructTreeNodeComponent * pComponentNode )
{
	if ( pComponentNode == NULL ) return "";

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//
	wmd.m_sMethodName = "runComponent";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = pComponentNode->m_strID;
	wmd.m_vParams.push_back(pParam);
	//
	if ( !InvokeJsMethod( &wmd ) ) return "";
	return wmd.m_sResult;
}

void CDlgSvg::JsMethod_xpdlNodeSelect( CCxStructTreeNode * pNode )
{
	if ( pNode == NULL ) return;

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//方法名称
	wmd.m_sMethodName = "xpdlNodeSelect";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = pNode->m_strID;
	wmd.m_vParams.push_back(pParam);
	InvokeJsMethod( &wmd );
}

CString CDlgSvg::JsMethod_ConvertFlowId( CString& strFlowIdRemote )
{
	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//
	wmd.m_sMethodName = "convertSub2Activtity";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = strFlowIdRemote;
	wmd.m_vParams.push_back(pParam);
	//
	if ( !InvokeJsMethod( &wmd ) ) return "";
	return wmd.m_sResult;
}

void CDlgSvg::JsMethod_updateRunFlag( LPCTSTR lpszData )
{
	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//
	wmd.m_sMethodName = "updateRunFlag";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonArr";
	pParam->m_sValue = lpszData == NULL ? "" : lpszData;
	wmd.m_vParams.push_back(pParam);
	//
	if ( !InvokeJsMethod( &wmd ) ) return;
}

void CDlgSvg::JsMethod_updateParamStatus( LPCTSTR lpszData )
{
	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//
	wmd.m_sMethodName = "updateParamStatus";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonArr";
	pParam->m_sValue = lpszData == NULL ? "" : lpszData;
	wmd.m_vParams.push_back(pParam);
	//
	if ( !InvokeJsMethod( &wmd ) ) return;
}

BOOL CDlgSvg::PreTranslateMessage( MSG* pMsg )
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


