#pragma once
#include "DlgForBase.h"

//ForStep循环类型数据结构
struct ForStepData
{
	CString strParam; //参数
	CString strFirstValue; //初值
	CString strFirstValueConst; //初值常量
	CString strEndValue; //终值
	CString strEndValueConst; //终值常量
	CString strStep; //步长
	CString strStepConst; //步长常量
};

// CDlgForStep 对话框

class CDlgForStep : public CDlgForBase
{
	DECLARE_DYNAMIC(CDlgForStep)

public:
	CDlgForStep(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgForStep();

// 对话框数据
	enum { IDD = IDD_FOR_STEP };

	// 添加参数
	virtual void AddParam(LPCTSTR str);

	// 删除参数
	virtual void DeleteParam(LPCTSTR str);

	// 重命名参数
	virtual void RenameParam(LPCTSTR strOld, LPCTSTR strNew);

	//参数类型或者方向已改变
	virtual void ParamChanged(ZTreeParam* pParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
	ForStepData GetData(); //获取数据结构
};
