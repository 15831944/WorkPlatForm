#pragma once
#include "afxwin.h"
#include "DlgParamSetBase.h"
#include "ZTreeParamSet.h"
#include "UUID.h"
#include "CStringChecker.h"
#include "XTipComboBox.h"
// CDlgAddParam 对话框

class CDlgAddParam : public CCxBCGPDialog

{
	DECLARE_DYNAMIC(CDlgAddParam)

public:
	CDlgAddParam(CWnd* pParent = NULL);   // 标准构造函数
	CDlgAddParam(MAPSTR_JV mapStrJv, VECTOR_NODES vParams,CWnd* pParent =NULL);
	virtual ~CDlgAddParam();

// 对话框数据IDD_ADD_GROUP
	enum { IDD = IDD_ADD_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	CString m_strParamName;
	CString m_strGroupName;
	CXTipComboBox m_ctlComBox;
	MAPSTR_JV m_JvStrMap;		//组的MapList
	MAP_STRJSON m_MapPrams;		//参数的MapList

	virtual BOOL OnInitDialog();
	
	int m_nChanged;
	//BOOL m_bIsSameParam;
	VECTOR_NODES m_vParams;
	VECTOR_NODES GetParasVector();
	BOOL CheckSameName(ZTreeParam* pParamIn) ;
	void SetGroupName(BOOL bIsSame);
	int IsMapListChanged();
	void SetParasList(MAP_STRJSON paramList);
	MAP_STRJSON GetParasList();
public:
	void InitComBox() ;
	int AddParam2Group();
	MAPSTR_JV GetGroupList();
	afx_msg void OnBnClickedOk();
	int bin_search(VECTOR_JV key,int low, int high,int k);
	CString GetAddParaName();
	void SetGroupName(CString strName);
	CString GetGroupName();
//public:
//	ZTreeParamSet m_wndGridTree;

};
