#pragma once
#include "BaseHtmlDialog.h"
#include "Resource.h"
#include "CxResLibImageToolTipCtrl.h"
#include "CxStructTreeNode.h"
#include "CxStructTreeNodeComponent.h"

// CDlgSvg 对话框

class CDlgSvg : public CBaseHtmlDialog
{
	DECLARE_DYNAMIC(CDlgSvg)

public:
	CDlgSvg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSvg();

// 对话框数据
	enum { IDD = IDD_SVG };

	DECLARE_DISPATCH_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//
	virtual BOOL OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
		, CComVariant& varResult );

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CCxResLibImageToolTipCtrl  m_ToolTip;

	BOOL JsMethod_parseSubProcess(LPCTSTR lpszData);
	CString JsMethod_runDataMappingExpr( LPCTSTR lpszData );
	BOOL JsMethod_paramUpdate( CCxStructTreeNodeParam * pParamNode );
	CString JsMethod_runComponent( CCxStructTreeNodeComponent * pComponentNode );
	void JsMethod_xpdlNodeSelect( CCxStructTreeNode * pNode );
	CString JsMethod_ConvertFlowId( CString& strFlowIdRemote );
	void JsMethod_updateRunFlag( LPCTSTR lpszData );
	void JsMethod_updateParamStatus( LPCTSTR lpszData );
};
