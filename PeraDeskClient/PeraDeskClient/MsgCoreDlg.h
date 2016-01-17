#pragma once

#include "BaseHtmlDialog.h"

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CDHtmlDialog。"
#endif 

// CMsgCoreDlg 对话框

class CMsgCoreDlg : public CBaseHtmlDialog
{
	DECLARE_DYNCREATE(CMsgCoreDlg)

public:
	CMsgCoreDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgCoreDlg();
	void OpeMsgUrl();
// 重写
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
// 对话框数据
	enum { IDD = IDD_DIALOG_MSG_CORE, IDH = IDR_HTML_MSGCOREDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	// 取消右键菜单
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
