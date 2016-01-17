#pragma once
#include "resource.h"
#include "..\PeraProcessDesigner\Resource.h"
#include <afxwin.h>
#include "shoshtreectrl.h"
class MagicDB;
class CReadRobotDB;
class CResLibData;
// CComponentDlg 对话框

class CDlgResLibEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgResLibEditor)

public:
	CDlgResLibEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgResLibEditor();

// 对话框数据
	enum { IDD = IDD_RESLIB_EDIT };

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
	CString m_COMPONENT_PK_ID;
	CString m_COMPONENT_FK_PID;
	CString m_COMPONENT_NAME;

	CString m_COMPONENT_PIC_16;
	CString m_COMPONENT_SRC;
	afx_msg void OnIDOK();
	CString m_FILE_PATH;
	CString m_INSTANCE_PK_ID;
	CString m_TAG;
	CString m_ParametsJson;
	CString m_csDesc;
	CString m_COMPONENT_VERSION;
	CString m_COMPONENT_AUTHOR;
	CString m_COMPONENT_CREATE_TIME;
	CShoshTreeCtrl m_Tree;
	CResLibData* m_pResLibDataBase;
	virtual BOOL OnInitDialog();
	BOOL m_bSelectTree;

	BOOL SelectRobotFile();
	BOOL SelectRobotFile(PublishInfo* ResLibDataInfo);
	void SetSelectTree(BOOL bSelectTree);
	void SetpResLibDataBase(CResLibData* pResLibDataBase);
	CResLibData* GetpResLibDataBase(){return m_pResLibDataBase;};
};
