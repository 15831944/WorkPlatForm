// DlgSvg.cpp : ʵ���ļ�
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


// CDlgSvg �Ի���

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
	//�� Enter ���رնԻ���
}

void CDlgSvg::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//�� ESC ���رնԻ���
	//ע�⣺����Ϣ���Զ����� WM_CLOSE ��Ϣ
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
		//���½ṹ��
	case WCB_SVGEVENT:
		{
			if (mapParams.size() != 2)
			{
				//varResult = "������������ȷ";
				//lc,���ʻ���������
				varResult = _bstr_t("������������ȷ").GetBSTR();
				return FALSE;
			}

			CString strJsonData, strOperatorFlag;
			itrParam = mapParams.begin();
			//strJsonData = itrParam->second.bstrVal;
			//lc,���ʻ���������
			strJsonData = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);
			itrParam++;
			//strOperatorFlag = itrParam->second.bstrVal;
			//lc,���ʻ���������
			strOperatorFlag = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);
			ZTools::WriteZToolsFormatLog("UpdateStructTree Flag=%s, JsonData=\r\n%s\r\n"
				, strOperatorFlag
				, strJsonData);

			theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.SvgEvent(strOperatorFlag, strJsonData);
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		//��ȡ��Ϣ
	case WCB_GET_INFO:
		{
			if (mapParams.size() != 1)
			{
				//varResult = "������������ȷ";
				//lc,���ʻ���������
				varResult = _bstr_t("������������ȷ").GetBSTR();
				return FALSE;
			}

			CString strJsonData;
			itrParam = mapParams.begin();
			//strJsonData = itrParam->second.bstrVal;
			//lc,���ʻ���������
			strJsonData = (LPCTSTR)_bstr_t(itrParam->second.bstrVal);
			CString strResult = WebGetInfo( strJsonData );
			//varResult = (LPCTSTR)strResult;
			//lc,���ʻ���������
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
	//��1������
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
	//��1������
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
	//��������
	wmd.m_sMethodName = "paramUpdate";
	//��1������
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
	//��1������
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
	//��������
	wmd.m_sMethodName = "xpdlNodeSelect";
	//��1������
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
	//��1������
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
	//��1������
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
	//��1������
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


