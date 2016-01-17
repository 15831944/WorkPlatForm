#pragma once

#include "BranchCondition.h"

//#define  (WM_USER+300)

struct ZTreeParam //�ṹ������
{
	void Init();
	~ZTreeParam()
	{
	}
	std::string m_strID;
	std::string m_strName;
	std::string m_strType;   //����
	std::string m_strValue;  //ֵ
	std::string m_strUnit;    //��λ
	std::string m_strDesrciption; //����
	std::string m_strFormat; //��ʽ��
	std::string m_strCnName; //�e��
	std::string m_strGroup;
	std::string m_strListName; 
	std::string m_strIsDynamic; //��̬�����־
	std::string m_strIsSaveInModel;	//�ļ��Ƿ�����ѹ������
	std::string m_strDirection; //����
	BOOL m_bIsGroup;			//�Ƿ�Ϊ��ڵ�
	vector<ZTreeParam*>m_parmList;
	bool operator==(const ZTreeParam& other)
	{
		return (m_strID == other.m_strID
			&& m_strName==other.m_strName
			&& m_strType == other.m_strType
			&& m_strValue == other.m_strValue
			&& m_strUnit == other.m_strUnit
			&& m_strDesrciption == other.m_strDesrciption
			&& m_strFormat == other.m_strFormat
			&& m_strCnName == other.m_strCnName
			&& m_strGroup == other.m_strGroup
			&& m_strListName == other.m_strListName
			&& m_strIsDynamic == other.m_strIsDynamic
			&& m_strIsSaveInModel == other.m_strIsSaveInModel
			&& m_strDirection == other.m_strDirection);
	}
};




// CDlgParamEdit �Ի���

class CDlgParamEdit : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamEdit)


public:
	CDlgParamEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParamEdit();

	enum EParamType //��������
	{
		TypeNULL = 0,
		TypeFile,
		TypeString,
		TypeStringArray,
		TypeInteger,
		TypeIntegerArray,
		TypeDouble,
		TypeDoubleArray,
		TypeBool,
		TypeBoolArray
	};

// �Ի�������
	enum { IDD = IDD_PARAM_EDIT };

	static const ZRectF m_conRectName; //λ�ðٷֱ�
	static const ZRectF m_conRectType; //λ�ðٷֱ�
	static const ZRectF m_conRectValue; //λ�ðٷֱ�
	static const ZRectF m_conRectUnits; //λ�ðٷֱ�
	static const ZRectF m_conRectDesc; //λ�ðٷֱ�
	static const ZRectF m_conRectDirection; //λ�ðٷֱ�
	static const ZRectF m_conRectDynamic; //λ�ðٷֱ�
	static const ZRectF m_conRectBtnValue; //��ť
	static const ZRectF m_conRectBtnDesc; //��ť
	static const ZRectF m_conRectInModel; //λ�ðٷֱ�

	void SetParamInfo(ZTreeParam& zTreeParam); //���²�����Ϣ

	ZTreeParam GetParamInfo(); //��ȡ������Ϣ

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual void OnCancel();
	void Enable(BOOL bEnable = TRUE); //�����༭

	ZTreeParam m_zTreeParam; //��ǰ�༭�Ĳ���

	BOOL m_bEnabled; //������־
	afx_msg void OnBnClickedButtonDescription();
	afx_msg void OnCbnSelchangeComboDirection();


	afx_msg void OnBnClickedButtonValue();
	std::string TypeString2ComboBoxString(LPCTSTR str);
	std::string ComboBoxString2TypeString(LPCTSTR str);
	EParamType StringToType(LPCTSTR str);
	int m_nDim; //ά��

	EParamType m_eParamType;
	afx_msg void OnCbnSelchangeComboUnit();
	void ChangeEditControl(EParamType oldType, EParamType newType); //�������͸ı�ؼ�״̬
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnEnChangeEditValue();
	afx_msg void OnEnKillfocusEditValue();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCheckDynamic();
	void AdjustLayout();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusEditDescription();
	afx_msg void OnEnKillfocusEditCnname();
	void RefreshLayout();
	void StretchX(int nX);
	void MoveItem(int nID, int nLeft, int nTop, int nRight, int nBottom);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	BOOL ParamEditIsEnable();
	void SetParamEditIsEnable(BOOL bEnable);
};
