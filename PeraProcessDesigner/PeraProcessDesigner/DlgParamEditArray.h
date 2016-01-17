#pragma once
#include "CxBCGPDialog.h"
#include "ArrayGrid.h"


// CDlgParamEditArray 对话框

class CDlgParamEditArray : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamEditArray)

public:
	CDlgParamEditArray(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamEditArray();

// 对话框数据
	enum { IDD = IDD_PARAM_EDIT_ARRAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEdit();

	static const int m_maxDim = 3; //最大维度

	static const int m_maxNum = 10000; //最大元素数量

	std::vector<int> m_vDim; //维度

	enum DataType
	{
		TypeInteger, //整型
		TypeString, //字符型
		TypeDouble, //浮点型
		TypeBool //布尔型
	};

	static LPCTSTR m_strDefaultDatas[TypeBool + 1];

	enum {
		COL_NAME = 0,
		COL_VALUE,
	};


	DataType m_dataType; //数据类型

	BOOL m_bEdited; //是否编辑过
	BOOL IsEdited() const { return m_bEdited; }
	void SetEdited(BOOL val) { m_bEdited = val; }

	CArrayGrid m_arrayGrid; //数组表格

	std::vector<std::string> m_vArray; //数组数据

	std::string m_strArray; //数组字符串

	std::string m_strName; //数组名称

	BOOL StringToArray(); //字符串转数组

	void ArrayToString(); //数组转字符串
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void InitGrid();
	BOOL LoadArrayList( void ); //载入数组数据
	void UpdateArrayFromGrid(); //更新数组数据从表格
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	void OnChangeEdit(); //EDIT改变
	void RefreshEdit();

	CString m_StrEdit1Last;
	CString m_StrEdit2Last;
	CString m_StrEdit3Last;

	BOOL m_bInit; //初始化完成标志
};
