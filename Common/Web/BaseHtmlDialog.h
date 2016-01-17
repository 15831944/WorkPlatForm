#pragma once
#include <afxdhtml.h>
#include "HtmlWindowEventCallback.h"
#include "htmlwindoweventsink.h"
#include "StlHeader.h"
#include "WebDataDefine.h"

//extern UINT WM_WEBPAGE_PROCESS_ATTACHED;
//extern UINT WM_WEBPAGE_LOADWEB_FINISHED;

class CBaseHtmlDialog : public CDHtmlDialog, public IHtmlWindowEventCallback
{
	DECLARE_DYNAMIC(CBaseHtmlDialog)

public:
	CBaseHtmlDialog(UINT nID, CWnd* pParent = NULL);
	virtual ~CBaseHtmlDialog();

	//设置url
	void SetUrl(LPCTSTR lpszUrl = "about:blank")
	{
		m_strUrl = lpszUrl == NULL ? "" : lpszUrl;
		if (m_strUrl.IsEmpty())
			m_strUrl = "about:blank";
	}

	//调用页面的 javascript 方法
	BOOL InvokeJsMethod( CWebMethodData * pMethodData
		, CComPtr<IHTMLWindow2> pHtmlWindow = NULL );

	/*
	在 PreTranslateMessage 中调用。
	web页面中热键、鼠标事件的默认处理逻辑。
	参数：
		pMsg：PreTranslateMessage传过来的参数。
		hHostWnd：如果是内嵌Web，为自己主窗口句柄；
							如果是吸附进程中的web，为CMainframe窗口句柄。
	返回值：
		TRUE：在 PreTranslateMessage 中不需要再继续处理。
	*/
	BOOL WebHotKeyHandle( MSG* pMsg, HWND hHostWnd );

public:
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_EVENTSINK_MAP()

protected:
	//页面调用exe方法
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

	//检查刷新事件
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
	//禁止拖文件到容器
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);

//public:
	//HWND m_hwndRecvMsg; //web中转发消息的处理窗口，如CMainFrame窗口，或者CDlgMain窗口
protected:
	CString m_strUrl;
	CString m_strLog;
	LPSTREAM m_lpsHtmlWindow;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//多字节工程在英文区域下时，如果URL中有中文，会出现乱码
	void Navigate(LPCTSTR lpszURL, DWORD dwFlags= 0, LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
};


