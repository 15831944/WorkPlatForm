#pragma once
#include "afxcmn.h"
#include "ToolsProUnity.h"
#include "XListCtrl.h"
#include "CxBCGPDialog.h"
// CToolManagerDlg 对话框

class CToolManagerDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CToolManagerDlg)

public:
	CToolManagerDlg(map<CString,ToolObject> toolmap,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolManagerDlg();

 friend int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// 对话框数据
	enum { IDD = IDD_DIALOG_TOOL_MANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void UpdateItem(int iRow,ToolObject *pTool);

	DECLARE_MESSAGE_MAP()
private:
	virtual BOOL OnInitDialog();
	CXListCtrl m_ListCtrl;
	map<CString,ToolObject> m_ToolMap;
	BOOL   m_bArrySortAscending[6];
	int    m_iSortColumn;

	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDelAllSel();
	afx_msg void OnDelAll();
	afx_msg void OnRunCur();
	afx_msg void OnConfigCur();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateRunCur(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConfigCur(CCmdUI *pCmdUI);
public:
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};
