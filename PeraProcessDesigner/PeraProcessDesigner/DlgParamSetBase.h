#pragma once
#include "ZTreeParamSet.h"
#include "CxBCGPDialog.h"
#include "DlgParamEdit.h"
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "zbuttondraw.h"
#include "ZToolButton.h"
#include "CxBCGPButtonPlb.h"
#include "DlgAddParam.h"
#include "DlgAddGroup.h"
#include "DlgMovePara.h"
#include "DlgParamEdit.h"
// CDlgParamSetBase 对话框
//add by zhaoyw/////////////////////////////////////////////
//typedef vector<Json::Value> VECTOR_JV;
//typedef vector<Json::Value>::iterator ITER_JSVECTOR;
//
//typedef struct JvNode 
//{
//	VECTOR_JV   vecJvNode;			 //存放组成员
//	CString     strGroupName;		//组的名字
//	Json::Value jsGroup;			//组对应的名字
//}JS_NODE,*pJS_NODE;
//
//typedef map<CString, JS_NODE*> MAPSTR_JV;
//typedef map<CString, JS_NODE*>::iterator ITER_STR_JV;
////////////////////////////////////////////////////////


class CDlgParamSetBase : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamSetBase)

public:
	CDlgParamSetBase(LPCTSTR str, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamSetBase();

	// 对话框数据
	enum { IDD = IDD_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	
	DECLARE_MESSAGE_MAP()
public:

	enum {
		GROUP_TO_OUT = 0,
		GROUP_TO_GROUP,
		OUT_TO_GROUP
	};

	enum
	{
		NONE_MOVE_NODE=0,
		SINGLE_MOVE_NODE,
		MULTI_MOVE_NODE
	};

	afx_msg LRESULT OnUserSortmsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	
	afx_msg void OnBnClickedBtnMove();
	afx_msg void OnBnClickedBtnAddGroup();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedBtnAddParam();
	virtual void DeleteRowItem();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	ZTreeParamSet m_wndGridTree;
	BOOL m_bParamsChanged;		//参数改变标志

	// 添加
	CCxBCGPButtonPlb m_btnAdd;
	CxImage m_imageBarParam; //长条
	CDlgParamEdit m_dlgParamEdit;
	std::string m_strJson; //传入字符串
	Json::Value m_jv;

	// 删除IDC_BTN_ADDGROUP
	CCxBCGPButtonPlb m_btnDelete;
	CCxBCGPButtonPlb m_btnAddGroup;
	CCxBCGPButtonPlb m_btnMoveTo;

public:

	void AddNode(CString strParam, CString strGroupName);
	void UpdateTreeGrid(MAPSTR_JV MapStrJv);
	void UpdateJsonFromTree();
	void UpdateTreeFromJson();
	BOOL RemoveParam(ZTreeParam* pParam); //从容器删除参数

public:	
	static Json::Value ParamToJsonValue(ZTreeParam& param);
	static std::string GetJsonStrFromSelNode(CCxStructTreeNode* pNode); //从某流程节点转化JsonString
	static void AddParamFromNode(Json::Value& jv, CCxStructTreeNode* pNode);

public:
	void SetParamChanged(BOOL bChanged);
	BOOL GetParamChanged();

public:
	void SelectCurRow(ZTreeParam *pParam);
	void MoveSelNodes(vector<ZTreeParam*>  &vecParams,string strGroupName);
	int GetMoveParamlst(vector<ZTreeParam*> &vecParams ,CString strGroup);	
	UINT MoveSingleNode(ZTreeParam *pParam, string strGroupName);;
	int GetSelMoveParamCount();
//	afx_msg LRESULT  OnHotKey(WPARAM  wParam,LPARAM  lParam); 
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
