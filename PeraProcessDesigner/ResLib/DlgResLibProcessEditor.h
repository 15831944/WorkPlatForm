#include "afxwin.h"
#include "../PeraProcessDesigner/resource.h"
#pragma once
#include "ShoshTreeCtrl.h"

class MagicDB;
class CReadRobotDB;
class CResLibData;
// CProcessDlg 对话框

class CDlgResLibProcessEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgResLibProcessEditor)

public:
	CDlgResLibProcessEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgResLibProcessEditor();

// 对话框数据
	enum { IDD = IDD_RESLIB_PROCESS_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
	CString m_OldName;

	CString m_PK_ID;
	CString m_FK_PID;
	CString m_PROCESS_NAME;
	CString m_PROCESS_PIC_16;
	CString m_PROCESS_FILE_PATH;
	CString m_PROCESS_PIC_42;
	CString m_PROCESS_OPT;
	CString m_PROCESS_CREAT_TIME;
	CString m_PROCESS_CREATE_TIME;
	CString m_PROCESS_TYPE;
	CString m_PROCESS_SRC;
	CString m_PROCESS_ID;
	BOOL SelectProcessFile();
	BOOL SelectProcessFile(PublishInfo* ResLibDataInfo);
	void SetpResLibDataBase(CResLibData* pResLibDataBase);
	CResLibData* GetpResLibDataBase(){return m_pResLibDataBase;}
	void SetSelectTree(BOOL bSelectTree);
	afx_msg void OnBnClickedOk();

	CString m_PROCESS_DESC;
	CString m_PROCESS_VERSION;
	CString m_PROCESS_AUTHOR;
	CShoshTreeCtrl m_Tree;
	BOOL m_bSelectTree;
	CResLibData* m_pResLibDataBase;
	virtual BOOL OnInitDialog();
};
