#pragma once
#include "afxwin.h"
#include "ToolsProUnity.h"
#include "CxBCGPDialog.h"

// CToolSetDlg 对话框

class CToolSetDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CToolSetDlg)

public:
	CToolSetDlg(ToolObject *pTool,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolSetDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_TOOLSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedOk();
private:
	ToolObject *m_pTool;
public:
	// 软件ID
	CString m_strToolId;
	// 软件名称
	CString m_strToolName;
	// 绝对路径
	CString m_strAbsPath;
	// 默认主键
	CString m_strDefaultKey;
	// 备用主键
	CString m_strReverseKey;
	// 注册表项
	CString m_strKeyName;
	// 注册表键值项
	CString m_strKeyValueName;
	// 启动路径
	CString m_strRunPath;
	// 启动参数
	CString m_strRunParam;
	// 启动名称
	CString m_strRunName;

	// 本地配置绝对路径
	CString m_strLocalPath;
	// 本地配置启动参数
	CString m_strLocalRunParam;
};
