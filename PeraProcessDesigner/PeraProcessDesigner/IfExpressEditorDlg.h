#pragma once
#include "CxBCGPDialog.h"
#include "afxwin.h"
#include "MethodForDataMapIfExpress.h"
#include "PeraAutoCheckEdit.h"
#include "TreeDropDownDlg.h"
// CIfExpressEditorDlg 对话框
class CIfExpressEditorDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CIfExpressEditorDlg)
public:
	typedef enum
	{
		TP_NONE  = -1,
		TP_PARAM = 0,
		TP_CONST = 1
	}ComboParamType;
public:
	CIfExpressEditorDlg(const BranchExpress &Express,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIfExpressEditorDlg();

// 对话框数据
	enum { IDD = IDD_DLG_IF_EXPRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void  AdjustRightEditSize();
	void  AdjustLeftEditSize();
	void  DeleteExpress();
	void  UpdateComboBoxSymbol();
	BOOL CheckMapArry(CCxStructTreeNode * pNodeParam ,BOOL bIsLeft,CString &strErrMsg);
	CCxStructTreeNode * FindArryElementNode(const CString & strArrID,const CString & strArryIndex);
	BOOL CheckGetConst(BOOL bSlient);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL InitDlgCtrl();
	void   InitTree();
	void GetCheckDataType(CCxStructTreeNode *pNode);
private:
	int m_iCheckType;
	CString  m_strLastRightEditParam; // 记住上次的参数
	CString  m_strLastRightEditConst; //记住上次的常量
	ComboParamType      m_iLastComboTypeSel;
	int     m_iFocusEidtPCtrlID;
	CCxStructTreeNode *m_pNodeLeft; //临时指针
	CCxStructTreeNode *m_pNodeRight; //临时指针
	BOOL  m_bDeleted;
public:
	CPeraAutoCheckEdit m_EditLeft;
	CPeraAutoCheckEdit m_EditLeftArry;
	CComboBox m_ComboSymbol;
	CComboBox m_ComboRightType;
	CPeraAutoCheckEdit m_EditRight;
	CPeraAutoCheckEdit m_EditRightArry;
	BOOL m_bIfNotSubFlow;
	//树显示父窗口
	CTreeDropDownDlg m_wndTreeDropDown;
	BranchExpress m_Express;
	CPoint m_PointTreeLeaveFocus;
	DWORD    m_iTickCountTreeLeaveFocus;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeComboType();
	afx_msg LRESULT OnCheckEidtInput(WPARAM,LPARAM);
	afx_msg void OnBnClickedBtnDelIfExpress();
	afx_msg void OnBnClickedOk();
	afx_msg HRESULT OnMsgStructSelectParam(WPARAM,LPARAM);
	afx_msg HRESULT OnMsgStructLeaveFocus(WPARAM,LPARAM);
};
