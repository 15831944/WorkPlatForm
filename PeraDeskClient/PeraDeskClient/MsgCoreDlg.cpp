// MsgCoreDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "MsgCoreDlg.h"
#include "ConfigFunc.h"
#include "UtilityFunc.h"
// CMsgCoreDlg 对话框

IMPLEMENT_DYNCREATE(CMsgCoreDlg, CBaseHtmlDialog)

CMsgCoreDlg::CMsgCoreDlg(CWnd* pParent /*=NULL*/)
	: CBaseHtmlDialog(CMsgCoreDlg::IDD,/*CMsgCoreDlg::IDH*/pParent)
{

}

CMsgCoreDlg::~CMsgCoreDlg()
{
}

void CMsgCoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseHtmlDialog::DoDataExchange(pDX);
}

BOOL CMsgCoreDlg::OnInitDialog()
{
	CBaseHtmlDialog::OnInitDialog();
	OpeMsgUrl();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CMsgCoreDlg, CBaseHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CMsgCoreDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CMsgCoreDlg 消息处理程序

HRESULT CMsgCoreDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	//OnOK();
	return S_OK;
}

HRESULT CMsgCoreDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	//OnCancel();
	return S_OK;
}
void CMsgCoreDlg::OpeMsgUrl()
{
	CString strMsgUrl = GetConfig("PeraDeskClient","MsgUrl");
	if (strMsgUrl.IsEmpty()|| theApp.m_LoginData.m_strTicket.IsEmpty())
	{
		strMsgUrl = "about:blank";
	}
	else
	{
		AttachTicket(strMsgUrl, theApp.m_LoginData.m_strTicket);
	}

	Navigate(strMsgUrl);
}
// 取消右键菜单
STDMETHODIMP CMsgCoreDlg::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return S_OK; //屏蔽右键菜单
}