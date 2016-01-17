#pragma once
#include "CxBCGPDialog.h"
#include "ArrayGrid.h"


// CDlgParamEditArray �Ի���

class CDlgParamEditArray : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamEditArray)

public:
	CDlgParamEditArray(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParamEditArray();

// �Ի�������
	enum { IDD = IDD_PARAM_EDIT_ARRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEdit();

	static const int m_maxDim = 3; //���ά��

	static const int m_maxNum = 10000; //���Ԫ������

	std::vector<int> m_vDim; //ά��

	enum DataType
	{
		TypeInteger, //����
		TypeString, //�ַ���
		TypeDouble, //������
		TypeBool //������
	};

	static LPCTSTR m_strDefaultDatas[TypeBool + 1];

	enum {
		COL_NAME = 0,
		COL_VALUE,
	};


	DataType m_dataType; //��������

	BOOL m_bEdited; //�Ƿ�༭��
	BOOL IsEdited() const { return m_bEdited; }
	void SetEdited(BOOL val) { m_bEdited = val; }

	CArrayGrid m_arrayGrid; //������

	std::vector<std::string> m_vArray; //��������

	std::string m_strArray; //�����ַ���

	std::string m_strName; //��������

	BOOL StringToArray(); //�ַ���ת����

	void ArrayToString(); //����ת�ַ���
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void InitGrid();
	BOOL LoadArrayList( void ); //������������
	void UpdateArrayFromGrid(); //�����������ݴӱ��
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	void OnChangeEdit(); //EDIT�ı�
	void RefreshEdit();

	CString m_StrEdit1Last;
	CString m_StrEdit2Last;
	CString m_StrEdit3Last;

	BOOL m_bInit; //��ʼ����ɱ�־
};
