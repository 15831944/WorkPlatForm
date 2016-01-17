#pragma once
#include "BaseHtmlDialog.h"
#include "CxStructTree.h"
#include "afxwin.h"
#include "CxBCGPDialog.h"
#include "CxBCGPButton.h"


//////////////////////////////////////////////////////////////////////////

class CRunFlowGetResult
{
public:
	CRunFlowGetResult();
	~CRunFlowGetResult();
	void Clear();
	CString GetMethodParamStr(void);
	CString GetMethodName(void);
	BOOL ParseResult(LPCTSTR lpszData, CCxStructTree * pTree);
public:
	CString m_strFlowId;
	BOOL m_bParsed;
	STRUCTTREEMAP_PARAM_VALUE mapParamValue;
};

class CRunFlowExec
{
public:
	CRunFlowExec();
	~CRunFlowExec();
	void Clear();
	CString GetMethodParamStr(void);
	CString GetMethodName(void);
	BOOL ParseResult(LPCTSTR lpszData, CCxStructTree * pTree);
public:
	CString m_strFlowId;
};


// CDlgRunFlow 对话框

class CDlgRunFlow : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgRunFlow)

public:
	CDlgRunFlow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRunFlow();

// 对话框数据
	enum { IDD = IDD_RUNFLOW };
	//DECLARE_DISPATCH_MAP()

public:
	LPCTSTR m_lpszUrl;
	LPCTSTR m_lpszFlowId;
	BOOL m_bFinished;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	//void UpdateContent(void);

	//virtual BOOL OnWebCallback( int nCode, WEBCALLBACK_METHOD_PARAM_MAP& mapParams
	//	, CComVariant& varResult );

	//
	static DWORD WINAPI GetResultThread(LPVOID lpParameter);
	//
	BOOL GetResult(void);

	//
//	BOOL Cancel(void);

	afx_msg LRESULT OnRunFlowCompleted( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAwProcessAttaching( WPARAM wParam, LPARAM lParam );
	CRunFlowGetResult m_Result;
	CString m_strResultData;
	//CCxWebPageEvent * m_pStartWebPageEvent;

	CAttachWindow m_AttachWindow;
	//HWND m_hwndWebPage;
	//BOOL m_bValidData;
	BOOL m_bGetResult;

	DWORD m_dwExStyleWebPage;
	DWORD m_dwStyleWebPage;
	
	afx_msg void OnClose();
	CCxBCGPButton m_wndOk;
	CCxBCGPButton m_wndCancel;
	afx_msg void OnBnClickedRunflowOk();
	afx_msg void OnBnClickedRunflowCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
