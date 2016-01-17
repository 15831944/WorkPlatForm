#pragma once
#include "dlgparamsetbase.h"
#include "BCGPStatic.h"
#include "DlgForBase.h"
#include "DlgForLimit.h"

#define IDC_FOR_TYPE 1 

//����ѭ�����͹������ݽṹ
struct SharedData
{
	CString strType; //ѡ���ѭ������
	BOOL bContinue; //ѭ��ִ��ʧ���Ƿ����
	int nConut; //���ѭ������
};

class CDlgFor :
	public CDlgParamSetBase
{
	DECLARE_DYNAMIC(CDlgFor)
public:

	enum ForType //ѭ������
	{
		ForStep = 0,
		ForArray,
		ForEach,
		While,
		DoWhile,
		Count
	};

	static LPCTSTR m_strForType[Count];

	CDlgFor(LPCTSTR str, CWnd* pParent =NULL);
	virtual ~CDlgFor(void);
	virtual BOOL OnInitDialog();
	void HideUselessControl();
	void CreateForFrame(); //�����߿�

	CBCGPStatic m_staSel; //ѭ������
	CBCGPStatic m_staForFrame; //ѭ�����
	CBCGPStatic m_staLimitText; 
	CBCGPStatic m_staLimit; 

	CBCGPStatic m_staSelText; //��ʾ�ı�
	CBCGPStatic m_staLimitStr; //��ʾ�ı�
	CBCGPComboBox m_boxFor; //ѭ��ѡ���
	CDlgForLimit m_dlgForLimit; //�����Ӵ���

	CRect m_rectSel; //ѭ������ѡ������
	CRect m_rectForFrame; //ѭ���Ӵ�������
	CRect m_rectLimitText; //ѭ�������Ӵ��ڱ�������
	CRect m_rectLimit; //ѭ�������Ӵ�������

	ForType m_curForType; //��ǰѭ������
	CDlgFor::ForType GetCurForType() const { return m_curForType; }
	void SetCurForType(CDlgFor::ForType val) { m_curForType = val; }

	CDlgForBase* m_pDlgFor[Count]; //ѭ���Ӵ���ָ��

	void ActivateDlgForByIndex(ForType type); //�����Ӵ���

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeComboFor();


	afx_msg void OnBnClickedOk1();

	SharedData GetData(); //��ȡ��������
};

