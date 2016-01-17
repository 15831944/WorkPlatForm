#pragma once
#include "DlgForBase.h"
#include <ForEachGrid.h>
#include <DlgForEachListFrame.h>
#include <DlgForEachList.h>

#define COMBO_WIDTH 215 //��
#define COMBO_HEIGHT 30 //��

// CDlgForEach �Ի���

class CDlgForEach : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForEach)

public:
	CDlgForEach(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgForEach();

// �Ի�������
	enum { IDD = IDD_FOR_EACH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void InitGrid();

	CForEachGrid m_forEachGrid; //���Ԫ�صı��

	CDlgForEachListFrame m_dlgForEachListFrame; //�б���

	CDlgForEachList m_dlgForEachList; //�б�

	vector<CBCGPComboBox*> m_vpComboBox; //ComboBox����

	int m_nIndexSel; //�ϴ�ѡ���BOX���

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnCbnFocusCombo();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();

	void SwapBox(int nID1, int nID2); //��������BOX
	afx_msg void OnBnClickedButtonDown();
	void CheckButtonCanEnable();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void AddParam( LPCTSTR str );
	void ParamChanged( ZTreeParam* pParam );
	void DeleteParam( LPCTSTR str );
	void RenameParam( LPCTSTR strOld, LPCTSTR strNew );
};
