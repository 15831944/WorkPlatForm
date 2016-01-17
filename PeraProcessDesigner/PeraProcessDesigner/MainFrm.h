// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "CxBCGPMenuBar.h"
#include "CxBCGPToolBar.h"
#include "CxRobotBar.h"
#include <ResLibData.h>
#include "CxLoginBar.h"
#include "ZToolTipEvent.h"
#include "XmlHistoryModel.h"

class CMainFrame : public CBCGPMDIFrameWnd, public ZToolTipEvent
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes


// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CCxBCGPMenuBar			m_wndMenuBar;
	CCxBCGPToolBar			m_wndToolBar;
	CCxRobotBar			    m_wndRobotBar;
	CCxLoginBar             m_wndLoginBar;

	CBCGPToolBarImages	m_UserImages;

	CXmlHistoryModel m_xhm;

	HICON m_hIcon ;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg LRESULT OnWebLoaded(WPARAM,LPARAM);
	void OnToolsViewUserToolbar (UINT id);
	void OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI);
	afx_msg void OnWindowManager();
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	//virtual BOOL OnShowMDITabContextMenu (CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual CBCGPMDIChildWnd* CreateDocumentWindow (LPCTSTR lpcszDocName, CObject* /*pObj*/);

	UINT	m_nAppLook;
public:
	afx_msg void OnMenuModuleOpenjianmo();
	afx_msg void OnMenuModuleActivejianmo();
	afx_msg void OnMenuModuleClosejianmo();
	afx_msg void OnMenuTaskNew();
	afx_msg void OnMenuTaskOpen();
	afx_msg void OnMenuTaskRun();
	afx_msg void OnMenuTaskSaveAs();
	afx_msg void OnMenuJobMgr();
	afx_msg void OnToolbarClicked( UINT nID );
	afx_msg void OnRobotbarClicked( UINT nID );
	afx_msg LRESULT OnUpdateDataMapping(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateComponentByLinker(WPARAM, LPARAM);
	afx_msg LRESULT OnPopMenuForSvg(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnStructTreeUpdateParamArray(WPARAM, LPARAM);
	afx_msg LRESULT OnStructTreeRunComponent (WPARAM /*wParam*/, LPARAM /*lParam*/);
	afx_msg LRESULT OnStructTreeUpdateProp (WPARAM wParam, LPARAM /*lParam*/);
	afx_msg LRESULT OnStructTreeShowToolTip (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStructTreeShowDataMapping(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateMainTitle(WPARAM, LPARAM);
	afx_msg LRESULT OnStructTreeRefresh(WPARAM, LPARAM);

	void LoadToolbar();
	void SaveXpdl(void);
	void ShowDataMapping(void);
	void LoadRobotbar();

	void UpdateRobotbar( list< CResLibData* > BaseComponentList );

	virtual BOOL GetToolbarButtonToolTipText (CBCGPToolbarButton* pButton, CString& strTTText);

	void ShowJobMgr(void);

	void ShowDistribution( void );

	afx_msg LRESULT OnShowProgress(WPARAM wParam, LPARAM);
	
protected:
	afx_msg LRESULT OnStructtreeEditIf(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStructtreeEditRootParams(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStructtreeEditFor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateResLibTree(WPARAM wParam, LPARAM lParam);
	void LoadLoginbar();
	afx_msg void OnLoginbarClicked( UINT nID );
	afx_msg LRESULT OnShowSvgDragLayer(WPARAM, LPARAM);
	afx_msg LRESULT OnStructTreeRunProcess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStructTreeRunSubProcess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStructTreeSaveAsSubProcess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStructtreeEditDataMappingExp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowRunFlowDialog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewRunFlowWebPage(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnViewToolbarz();
	afx_msg void OnViewComponent();
	afx_msg void OnViewLogin();
	afx_msg void OnUpdateViewComponent(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewLogin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewToolbarz(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTaskInfo();
	afx_msg void OnTaskExit();
	afx_msg void OnTaskCloseall();
	afx_msg void OnTaskSave();
	afx_msg void OnToolFlexware();
	afx_msg void OnDistribution();

	afx_msg void OnHelpAbout();
	afx_msg void OnSysLogin();
	afx_msg void OnSysLogout();
	afx_msg void OnUpdateSysLogin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSysLogout(CCmdUI *pCmdUI);
	afx_msg void OnTaskRecent();
	afx_msg void OnUpdateTaskRecent(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuHistoryRecent(UINT nID);
	afx_msg void OnMenuViewChange(UINT nID);
	afx_msg void OnHelp();
	afx_msg void OnTaskPublish();
	afx_msg void OnBarTree();
	afx_msg void OnUpdateBarTree(CCmdUI *pCmdUI);
	afx_msg void OnRes();
	afx_msg void OnUpdateRes(CCmdUI *pCmdUI);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT OnAwProcessAttaching( WPARAM wParam, LPARAM lParam );
	//向指定窗口提交附加进程消息
	BOOL OnCpMsgAwProcessAttaching( LPCTSTR lpszData );
	afx_msg LRESULT OnWebEventAsync( WPARAM wParam, LPARAM lParam );
public:
	//void OnWebPageEvent( LPVOID lpData );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnThemeChanged();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnChangeUIState(UINT nAction, UINT nUIElement);
	afx_msg LRESULT OnLoginbarClickedLogout( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


