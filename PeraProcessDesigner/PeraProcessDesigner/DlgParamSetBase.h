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
// CDlgParamSetBase �Ի���
//add by zhaoyw/////////////////////////////////////////////
//typedef vector<Json::Value> VECTOR_JV;
//typedef vector<Json::Value>::iterator ITER_JSVECTOR;
//
//typedef struct JvNode 
//{
//	VECTOR_JV   vecJvNode;			 //������Ա
//	CString     strGroupName;		//�������
//	Json::Value jsGroup;			//���Ӧ������
//}JS_NODE,*pJS_NODE;
//
//typedef map<CString, JS_NODE*> MAPSTR_JV;
//typedef map<CString, JS_NODE*>::iterator ITER_STR_JV;
////////////////////////////////////////////////////////


class CDlgParamSetBase : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamSetBase)

public:
	CDlgParamSetBase(LPCTSTR str, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParamSetBase();

	// �Ի�������
	enum { IDD = IDD_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	
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
	BOOL m_bParamsChanged;		//�����ı��־

	// ���
	CCxBCGPButtonPlb m_btnAdd;
	CxImage m_imageBarParam; //����
	CDlgParamEdit m_dlgParamEdit;
	std::string m_strJson; //�����ַ���
	Json::Value m_jv;

	// ɾ��IDC_BTN_ADDGROUP
	CCxBCGPButtonPlb m_btnDelete;
	CCxBCGPButtonPlb m_btnAddGroup;
	CCxBCGPButtonPlb m_btnMoveTo;

public:

	void AddNode(CString strParam, CString strGroupName);
	void UpdateTreeGrid(MAPSTR_JV MapStrJv);
	void UpdateJsonFromTree();
	void UpdateTreeFromJson();
	BOOL RemoveParam(ZTreeParam* pParam); //������ɾ������

public:	
	static Json::Value ParamToJsonValue(ZTreeParam& param);
	static std::string GetJsonStrFromSelNode(CCxStructTreeNode* pNode); //��ĳ���̽ڵ�ת��JsonString
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
