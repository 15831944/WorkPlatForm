#pragma once

#include "BaseHtmlDialog.h"

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CDHtmlDialog��"
#endif 

// CMsgCoreDlg �Ի���

class CMsgCoreDlg : public CBaseHtmlDialog
{
	DECLARE_DYNCREATE(CMsgCoreDlg)

public:
	CMsgCoreDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMsgCoreDlg();
	void OpeMsgUrl();
// ��д
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
// �Ի�������
	enum { IDD = IDD_DIALOG_MSG_CORE, IDH = IDR_HTML_MSGCOREDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	// ȡ���Ҽ��˵�
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
