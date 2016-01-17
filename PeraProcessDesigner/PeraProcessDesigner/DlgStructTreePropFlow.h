#pragma once
//#include "StructTreeNodeData.h"
#include "CxStructTreeNodeFlow.h"
//class CCxStructTreeNodeFlow;

// CDlgStructTreePropFlow 对话框

class CDlgStructTreePropFlow : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStructTreePropFlow)

public:
	CDlgStructTreePropFlow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStructTreePropFlow();

// 对话框数据
	enum { IDD = IDD_STRUCTTREE_PROP_FLOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStpfOk();
	afx_msg void OnBnClickedStpfCancel();
	DECLARE_MESSAGE_MAP()
	// Edit控件字串过滤
	enum STRATEGY  //检测模式
	{
		STRATEGY_NODE,  //节点
		STRATEGY_DIR,   //目录
		STRATEGY_NAME, //名字
		STRATEGY_VERSION, //版本
		STRATEGY_DEFAULT //默认
	};	
	bool CheckEdit( CEdit& edit, STRATEGY strategy = STRATEGY_DEFAULT , CString cName = "",int nLenLimit = 20);
public:
	CEdit m_wndName;
	CEdit m_wndAuthor;
	CEdit m_wndCreated;
	CEdit m_wndVersion;
	CEdit m_wndDescrp;
	CButton m_wndOk;
	CButton m_wndCancel;

	CCxStructTreeNodeFlow * m_pNode;
};
