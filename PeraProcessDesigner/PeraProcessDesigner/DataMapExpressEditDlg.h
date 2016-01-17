#pragma once
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "afxwin.h"
#include "PeraAutoCheckEdit.h"
#include "MethodForDataMapIfExpress.h"
#include "CxBCGPDialog.h"
// CDataMapExpressEditDlg �Ի���
typedef struct
{
	SParam m_LeftParam;  // ����ӳ��=������ݽṹ��
	SParam m_RightParam; // ����ӳ��=�ұ����ݽṹ��
	CString m_strOtherParam; // ���ӱ��ʽ
	CString m_strOtherSymbol; // ���ӱ��ʽ
	void Clear()
	{
		m_LeftParam.Clear();
		m_RightParam.Clear();
		m_strOtherSymbol.Empty();
		m_strOtherParam.Empty();
	}

}DataMapData;

class CDataMapExpressEditDlg : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDataMapExpressEditDlg)

public:
	CDataMapExpressEditDlg(CString strExp,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDataMapExpressEditDlg();

// �Ի�������
	enum { IDD = IDD_MAP_EXPREE_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	BOOL ParseInPut();
	void InitUI();
	void InitComBox();
	bool IsValidArryMapParam(const CString & strText,vector<int> vArryDim,CString &strErrMsg,BOOL bIsLeft);
	BOOL CheckMapArry(CCxStructTreeNode * pNodeParam ,BOOL bIsLeft,CString &strErrMsg);
	BOOL CheckOtherParam(CString & strErrMsg);
	BOOL ProcessUpdate( const char* str );
	BOOL ParseResultJson(const char* strJson);
	void GetCheckDataType();
	 //bErrType = 0 �޴��� ��bErrType = 1 ������,bErrType = 2 �Ҳ����,3���Ҷ�����
	BOOL CheckArryMapOK(BOOL bSlient,CString & strErrMsgLeft,CString & strErrMsgRight,BOOL bOnOk,int & bErrType);
	BOOL  IsArry(CCxStructTreeNode *pNode);
private:
	CString m_strOldExp;
	CString m_strInJsonStr;
	DataMapData m_MapData;
	Json::Value m_Jv;
	int m_iCheckType;
	int m_iLeftStarSymbolCount;
	int m_iRightStarSymbolCount;
	BOOL m_bIsArryFullMap; // ���±�ʱ����ӳ��ı��
	DECLARE_MESSAGE_MAP()
public:
	// + - * /
	CComboBox m_ComboSymbol;
	CEdit m_LeftEdit;
	CEdit m_RightEdit;
	CPeraAutoCheckEdit m_OtherEdit;
	CPeraAutoCheckEdit m_EditLeftArry;
	CPeraAutoCheckEdit m_EditRightArry;
	CCxStructTreeNode *m_pNodeLeftParam;
	CCxStructTreeNode *m_pNodeRightParam;
	CToolTipCtrl m_ToolTipCtrl;
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboMapExpreessCondition();
	afx_msg HRESULT OnCheckEidtInput(WPARAM,LPARAM);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void   InitToolTipCtrl();
};
