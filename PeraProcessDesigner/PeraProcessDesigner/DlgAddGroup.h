#pragma once

#include "CxBCGPDialog.h"
// CDlgAddGroup �Ի���
#include "DlgParamSetBase.h"
#include "ZTreeParamSet.h"
class CDlgAddGroup : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAddGroup)

public:
	CDlgAddGroup(CWnd* pParent = NULL);   // ��׼���캯��
	CDlgAddGroup(MAPSTR_JV JvStrMap, CWnd* pParent =NULL);
	virtual ~CDlgAddGroup();

// �Ի�������
	enum { IDD = IDD_ADD_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

public:
	CString m_strGroup;
	int AddGroup();
	MAPSTR_JV GetGroupList();
	int m_nChanged;
	int IsMapListChanged();
	CString GetAddGroupName();
private:
	MAPSTR_JV m_JvStrMap;
};
