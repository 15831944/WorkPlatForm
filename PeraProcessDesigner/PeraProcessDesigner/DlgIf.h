#pragma once
#include "afxwin.h"
#include "ZEdit.h"
#include "ZGridCtrl.h"
#include "Resource.h"
#include "WndIf.h"
#include "DlgWndIf.h"
#include "zbuttondraw.h"
#include "CxBCGPDialog.h"

// CDlgIf 对话框

class CDlgIf : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIf)

public:
	CDlgIf(std::string str, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgIf();

// 对话框数据
	enum { IDD = IDD_IF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 显示Edit
	CZEdit m_EditShow;
	virtual BOOL OnInitDialog();
	// 名字
	CStatic m_StaticName;

	std::string m_strJsonSrc; //接收Json

	std::string m_strJsonRet; //返回Json

	CCxStructTreeNode * m_TreeRoot; //树根节点

	virtual void OnOK();

	CStatic m_staticName1;

	int m_radio;

	CZEdit m_editExp;  

	//CWndIf m_wndIf; //核心编辑窗口
	CDlgWndIf m_wndIf; //核心编辑窗口

	int m_nWidth; //窗口宽
	int m_nHeight;// 窗口高
	static const int m_nWidthMax;
	static const int m_nHeightMax;

	int m_nVisualManager; //风格

	//分支不做为子流程，意味着分支节点没有参数，表达式设置时可以看到其所属黑盒的参数20140321需求明确要求
	BOOL m_bIfNotSubFlow;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	void MoveOffset( CWnd& wnd, CRect& rect );
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedRadioExclusive();
	afx_msg void OnBnClickedRadioInclusive();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	void ProcessUpdate( const char* str );
	void ParseJson(const char* strJson);
	CZButtonDraw m_btnUp; //上按钮
	CZButtonDraw m_btnDown; //下按钮
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVisualManager();
};
