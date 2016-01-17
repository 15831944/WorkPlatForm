#pragma once
#include "CxBCGPDialog.h"
#include "ParamEventListener.h"
#include "DlgParamEdit.h"

// CDlgForBase 对话框

class CDlgForBase : public CCxBCGPDialog
	, public ParamEventListener
{
	DECLARE_DYNAMIC(CDlgForBase)

public:
	CDlgForBase(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForBase();

// 对话框数据
	enum { IDD = IDD_FOR_BASE };

	CBrush m_brush; //背景画刷

	ZTreeParam* GetParamByName(LPCTSTR strName); //根据参数名得到参数指针

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnOK();
	virtual void OnCancel();
	void AddStringToComboBox(int nID, LPCTSTR str);
	void AddStringToComboBox(CComboBox* pBox, LPCTSTR str);
	void DeleteStringFromComboBox(int nID, LPCTSTR str);
	void DeleteStringFromComboBox(CComboBox* pBox, LPCTSTR str);
	void SelectStringFromComboBox(int nID, LPCTSTR str);
	void SelectStringFromComboBox(CComboBox* pBox, LPCTSTR str);
	CString GetSelStrFromComboBox(int nID);
	CString GetSelStrFromComboBox(CComboBox* pBox);
};
