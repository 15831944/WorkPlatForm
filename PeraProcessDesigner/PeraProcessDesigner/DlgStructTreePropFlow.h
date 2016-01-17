#pragma once
//#include "StructTreeNodeData.h"
#include "CxStructTreeNodeFlow.h"
//class CCxStructTreeNodeFlow;

// CDlgStructTreePropFlow �Ի���

class CDlgStructTreePropFlow : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStructTreePropFlow)

public:
	CDlgStructTreePropFlow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgStructTreePropFlow();

// �Ի�������
	enum { IDD = IDD_STRUCTTREE_PROP_FLOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStpfOk();
	afx_msg void OnBnClickedStpfCancel();
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
	CEdit m_wndName;
	CEdit m_wndAuthor;
	CEdit m_wndCreated;
	CEdit m_wndVersion;
	CEdit m_wndDescrp;
	CButton m_wndOk;
	CButton m_wndCancel;

	CCxStructTreeNodeFlow * m_pNode;
};
