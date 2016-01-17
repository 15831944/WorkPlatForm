#pragma once

#include "BranchCondition.h"

//#define  (WM_USER+300)

struct ZTreeParam //结构树参数
{
	void Init();
	~ZTreeParam()
	{
	}
	std::string m_strID;
	std::string m_strName;
	std::string m_strType;   //类型
	std::string m_strValue;  //值
	std::string m_strUnit;    //单位
	std::string m_strDesrciption; //描述
	std::string m_strFormat; //格式化
	std::string m_strCnName; //別名
	std::string m_strGroup;
	std::string m_strListName; 
	std::string m_strIsDynamic; //动态数组标志
	std::string m_strIsSaveInModel;	//文件是否打包至压缩包中
	std::string m_strDirection; //方向
	BOOL m_bIsGroup;			//是否为组节点
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




// CDlgParamEdit 对话框

class CDlgParamEdit : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamEdit)


public:
	CDlgParamEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamEdit();

	enum EParamType //参数类型
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

// 对话框数据
	enum { IDD = IDD_PARAM_EDIT };

	static const ZRectF m_conRectName; //位置百分比
	static const ZRectF m_conRectType; //位置百分比
	static const ZRectF m_conRectValue; //位置百分比
	static const ZRectF m_conRectUnits; //位置百分比
	static const ZRectF m_conRectDesc; //位置百分比
	static const ZRectF m_conRectDirection; //位置百分比
	static const ZRectF m_conRectDynamic; //位置百分比
	static const ZRectF m_conRectBtnValue; //按钮
	static const ZRectF m_conRectBtnDesc; //按钮
	static const ZRectF m_conRectInModel; //位置百分比

	void SetParamInfo(ZTreeParam& zTreeParam); //更新参数信息

	ZTreeParam GetParamInfo(); //获取参数信息

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual void OnCancel();
	void Enable(BOOL bEnable = TRUE); //开启编辑

	ZTreeParam m_zTreeParam; //当前编辑的参数

	BOOL m_bEnabled; //开启标志
	afx_msg void OnBnClickedButtonDescription();
	afx_msg void OnCbnSelchangeComboDirection();


	afx_msg void OnBnClickedButtonValue();
	std::string TypeString2ComboBoxString(LPCTSTR str);
	std::string ComboBoxString2TypeString(LPCTSTR str);
	EParamType StringToType(LPCTSTR str);
	int m_nDim; //维度

	EParamType m_eParamType;
	afx_msg void OnCbnSelchangeComboUnit();
	void ChangeEditControl(EParamType oldType, EParamType newType); //根据类型改变控件状态
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
