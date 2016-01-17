#pragma once
#include <afxdhtml.h>
#include "HtmlWindowEventCallback.h"
#include "htmlwindoweventsink.h"
#include "StlHeader.h"
#include "WebDataDefine.h"

extern UINT WM_WEBPAGE_PROCESS_ATTACHED;
extern UINT WM_WEBPAGE_LOADWEB_FINISHED;

class CBaseHtmlDialog : public CDHtmlDialog, public IHtmlWindowEventCallback
{
	DECLARE_DYNAMIC(CBaseHtmlDialog)

public:
	CBaseHtmlDialog(UINT nID, CWnd* pParent = NULL);
	virtual ~CBaseHtmlDialog();

	//����url
	void SetUrl(LPCTSTR lpszUrl = "about:blank")
	{
		m_strUrl = lpszUrl == NULL ? "" : lpszUrl;
		if (m_strUrl.IsEmpty())
			m_strUrl = "about:blank";
	}

	//����ҳ��� javascript ����
	BOOL InvokeJsMethod( CWebMethodData * pMethodData
		, CComPtr<IHTMLWindow2> pHtmlWindow = NULL );



public:
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_EVENTSINK_MAP()

protected:
	//ҳ�����exe����
	CComVariant WebCallback(int nCode, LPVARIANT lpData);
	//
	virtual BOOL OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
		, CComVariant& varResult );
	//
	int ParseWebCallbackParams(vector<CComVariant>& vParams
		, LPDISPATCH lpParamObj);

	int ParseWebCallbackParams(WEBCALLBACK_METHOD_PARAM_MAP& mapParams
		, LPDISPATCH lpParamObj);

	//
	//int ParseWebCallbackParams(vector<WEBPAGE_METHOD_PARAM_DATA *>& vParams
	//	, LPDISPATCH lpParamObj);

	//
	CComPtr<IHTMLWindow2> GetIHTMLWindow2(void);
	//
	LPSTREAM CreateHtmlWindowStream( CComPtr<IHTMLWindow2>& pHtmlWindow );
	//
	CComPtr<IHTMLWindow2> ReleaseAndGetHtmlWindow( LPSTREAM& lps );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	//���ˢ���¼�
	void DownloadBeginExplorer1();
	void setWindowHandler();
	bool m_normalPageLoad;
	CHtmlWindowEventSink *m_htmlWindowEventSink;
	void onLoadOccured();
	void onUnloadOccured();
	void releasePointer();

	virtual void OnBeforeNavigate( LPDISPATCH pDisp, LPCTSTR szUrl );
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	//virtual BOOL CanAccessExternal();
	virtual BOOL IsExternalDispatchSafe();
	
	//
	HRESULT SetProperty(LPDISPATCH pObj, LPOLESTR pName, VARIANT *pValue);
	HRESULT GetProperty(LPDISPATCH pObj, LPOLESTR pName, VARIANT *pValue);
	HRESULT InvokeMethod(LPDISPATCH pObj, LPOLESTR pName, VARIANT *pVarResult, VARIANT *p, int cArgs);
	DISPID FindId(LPDISPATCH pObj, LPOLESTR pName);
	//��ֹ���ļ�������
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);

public:
	HWND m_hwndRecvMsg;
protected:
	CString m_strUrl;
	CString m_strLog;
	LPSTREAM m_lpsHtmlWindow;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//���ֽڹ�����Ӣ��������ʱ�����URL�������ģ����������
	void Navigate(LPCTSTR lpszURL, DWORD dwFlags= 0, LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
};


