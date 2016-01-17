#pragma once
#include "afxwin.h"
#include "ZTreeExpEditor.h"
#include "CxBCGPDialog.h"


// CDlgExpEditor 对话框

class CDlgExpEditor : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgExpEditor)

public:
	CDlgExpEditor(int nIndex, const char* strExp, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgExpEditor();

// 对话框数据
	enum { IDD = IDD_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedOk();

	static const int m_nBtnCount = 17;
	CString m_strExp; //表达式
	int m_nIndex; //索引号
	static const char* const m_str[m_nBtnCount];

	virtual BOOL OnInitDialog();
	// 编辑框
	CBCGPEdit m_editExp;
	afx_msg void OnEnSetfocusEditExp();
	afx_msg void OnEnKillfocusEditExp();
	void InsertText(const char* str);
	void InsertTextByID(int nID);
	int m_nEditPosStart; //光标开始
	int m_nEditPosEnd; //光标结束

	ZTreeExpEditor m_wndGridTree;  //树
	virtual void OnOK();
	afx_msg void OnEnUpdateEditExp();
	int MoveItemUp(CWnd* wndItem, CWnd* wndDst, int d);
	BOOL m_IsShowRoot; //显示根节点
	BOOL m_IsInitTree; //显示左侧树
};
