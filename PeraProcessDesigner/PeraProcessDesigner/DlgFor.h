#pragma once
#include "dlgparamsetbase.h"
#include "BCGPStatic.h"
#include "DlgForBase.h"
#include "DlgForLimit.h"

#define IDC_FOR_TYPE 1 

//所有循环类型共享数据结构
struct SharedData
{
	CString strType; //选择的循环类型
	BOOL bContinue; //循环执行失败是否继续
	int nConut; //最大循环次数
};

class CDlgFor :
	public CDlgParamSetBase
{
	DECLARE_DYNAMIC(CDlgFor)
public:

	enum ForType //循环种类
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
	void CreateForFrame(); //创建边框

	CBCGPStatic m_staSel; //循环类型
	CBCGPStatic m_staForFrame; //循环框架
	CBCGPStatic m_staLimitText; 
	CBCGPStatic m_staLimit; 

	CBCGPStatic m_staSelText; //提示文本
	CBCGPStatic m_staLimitStr; //提示文本
	CBCGPComboBox m_boxFor; //循环选择框
	CDlgForLimit m_dlgForLimit; //限制子窗口

	CRect m_rectSel; //循环类型选择区域
	CRect m_rectForFrame; //循环子窗口区域
	CRect m_rectLimitText; //循环限制子窗口标题区域
	CRect m_rectLimit; //循环限制子窗口区域

	ForType m_curForType; //当前循环类型
	CDlgFor::ForType GetCurForType() const { return m_curForType; }
	void SetCurForType(CDlgFor::ForType val) { m_curForType = val; }

	CDlgForBase* m_pDlgFor[Count]; //循环子窗口指针

	void ActivateDlgForByIndex(ForType type); //激活子窗口

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeComboFor();


	afx_msg void OnBnClickedOk1();

	SharedData GetData(); //获取共享数据
};

