#pragma once
#include "ZTreeParamSet.h"
#include "CxBCGPDialog.h"
#include "DlgParamEdit.h"
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "zbuttondraw.h"
#include "ZToolButton.h"
#include "CxBCGPButtonPlb.h"
#include "DlgParamSetBase.h"


// CDlgParamSet 对话框

class CDlgParamSet : public CDlgParamSetBase
{
	DECLARE_DYNAMIC(CDlgParamSet)

public:
	CDlgParamSet(LPCTSTR str, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamSet();
	
// 对话框数据
	enum { IDD = IDD_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOk1();

	afx_msg void OnBnClickedButtonDelete();
	CZGridRow* GetNextRowAfterDel( CZGridRow*pCurItem);
public:
	static Json::Value ParamToJsonValue(ZTreeParam& param);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void DeleteRowItem();
	int GetDelParamlst(vector<ZTreeParam*> &vecParams,int &nGroupCount);
	int DelSelNodes(vector<ZTreeParam*> &vecParams);
};
