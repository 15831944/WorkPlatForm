#pragma once
#include "DlgForBase.h"
#include <ForEachGrid.h>
#include <DlgForEachListFrame.h>
#include <DlgForEachList.h>

#define COMBO_WIDTH 215 //宽
#define COMBO_HEIGHT 30 //高

// CDlgForEach 对话框

class CDlgForEach : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForEach)

public:
	CDlgForEach(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForEach();

// 对话框数据
	enum { IDD = IDD_FOR_EACH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void InitGrid();

	CForEachGrid m_forEachGrid; //添加元素的表格

	CDlgForEachListFrame m_dlgForEachListFrame; //列表框架

	CDlgForEachList m_dlgForEachList; //列表

	vector<CBCGPComboBox*> m_vpComboBox; //ComboBox集合

	int m_nIndexSel; //上次选择的BOX序号

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnCbnFocusCombo();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();

	void SwapBox(int nID1, int nID2); //交换两个BOX
	afx_msg void OnBnClickedButtonDown();
	void CheckButtonCanEnable();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void AddParam( LPCTSTR str );
	void ParamChanged( ZTreeParam* pParam );
	void DeleteParam( LPCTSTR str );
	void RenameParam( LPCTSTR strOld, LPCTSTR strNew );
};
