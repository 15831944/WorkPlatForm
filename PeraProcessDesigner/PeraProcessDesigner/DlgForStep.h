#pragma once
#include "DlgForBase.h"

//ForStepѭ���������ݽṹ
struct ForStepData
{
	CString strParam; //����
	CString strFirstValue; //��ֵ
	CString strFirstValueConst; //��ֵ����
	CString strEndValue; //��ֵ
	CString strEndValueConst; //��ֵ����
	CString strStep; //����
	CString strStepConst; //��������
};

// CDlgForStep �Ի���

class CDlgForStep : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForStep)

public:
	CDlgForStep(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgForStep();

// �Ի�������
	enum { IDD = IDD_FOR_STEP };

	// ��Ӳ���
	virtual void AddParam(LPCTSTR str);

	// ɾ������
	virtual void DeleteParam(LPCTSTR str);

	// ����������
	virtual void RenameParam(LPCTSTR strOld, LPCTSTR strNew);

	//�������ͻ��߷����Ѹı�
	virtual void ParamChanged(ZTreeParam* pParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboFirstValue();
	afx_msg void OnCbnSelchangeComboEndValue();
	afx_msg void OnCbnSelchangeComboStep();
	afx_msg void OnEnKillfocusEditFirstValue();
	afx_msg void OnEnKillfocusEditEndValue();
	afx_msg void OnCbnKillfocusComboStep();
	BOOL CheckEditValue(int nID);
	virtual BOOL OnInitDialog();
	ForStepData GetData(); //��ȡ���ݽṹ
};
