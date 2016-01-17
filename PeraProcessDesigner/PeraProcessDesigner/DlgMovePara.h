#pragma once
#include "CxBCGPDialog.h"
#include "DlgParamSetBase.h"
#include "ZTreeParamSet.h"
#include "afxwin.h"
#include <algorithm>
#include "XTipComboBox.h"
// CDlgMovePara 对话框

class CDlgMovePara : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgMovePara)

public:
	CDlgMovePara(CWnd* pParent = NULL);   // 标准构造函数
	CDlgMovePara(MAPSTR_JV JvStrMap, CWnd* pParent =NULL);
	virtual ~CDlgMovePara();

// 对话框数据
	enum { IDD = IDD_MOVE_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	
private:
	MAPSTR_JV m_JvStrMap;

public:
	void InitComBox() ;
	MAPSTR_JV GetParaList();
	void MoveParaNode(ZTreeParam*pParam);
	int MoveParaNode(Json::Value mvJson, CString strNewGroup);
//	int MoveParaNode(Json::Value mvJson);
	void SetDeletNode(Json::Value mvJson);
	int IsMapListChanged();
	void SetParasList(MAP_STRJSON paramList);
	MAP_STRJSON GetParasList();
	void RemoveParam(VECTOR_JV& vectorParams,Json::Value jvDel);
	CString GetMoveGroupName();
public:
	CXTipComboBox m_ctrlComboBox;
	Json::Value m_jvDel;
	int m_nChanged;
	MAP_STRJSON m_MapPrams;
	afx_msg void OnBnClickedOk();
	CString GetMoveParaName();
	CString m_strCurCombox;
	CString m_strParamName;
};
