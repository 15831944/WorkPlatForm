#pragma once
#include "resource.h"
#include "..\PeraProcessDesigner\Resource.h"
#include <afxwin.h>
#include "shoshtreectrl.h"
class MagicDB;
class CReadRobotDB;
class CResLibData;
// CComponentDlg �Ի���

class CDlgResLibEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgResLibEditor)

public:
	CDlgResLibEditor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgResLibEditor();

// �Ի�������
	enum { IDD = IDD_RESLIB_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	// Edit�ؼ��ִ�����
	enum STRATEGY  //���ģʽ
	{
		STRATEGY_NODE,  //�ڵ�
		STRATEGY_DIR,   //Ŀ¼
		STRATEGY_NAME, //����
		STRATEGY_VERSION, //�汾
		STRATEGY_DEFAULT //Ĭ��
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
