// DlgAddGroup.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgAddGroup.h"



// CDlgAddGroup 对话框

IMPLEMENT_DYNAMIC(CDlgAddGroup, CCxBCGPDialog)

CDlgAddGroup::CDlgAddGroup(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgAddGroup::IDD, pParent)
	, m_strGroup(_T(""))
{

}

CDlgAddGroup::CDlgAddGroup(MAPSTR_JV JvStrMap,CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgAddGroup::IDD, pParent)
	, m_strGroup(_T(""))
{
	m_JvStrMap = JvStrMap;
	m_nChanged = 0;
}


CDlgAddGroup::~CDlgAddGroup()
{
}

void CDlgAddGroup::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strGroup);
}


BEGIN_MESSAGE_MAP(CDlgAddGroup, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgAddGroup::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAddGroup 消息处理程序

int CDlgAddGroup::IsMapListChanged()
{
	return m_nChanged;
}
CString CDlgAddGroup::GetAddGroupName()
{
	return m_strGroup.Trim();
}

void CDlgAddGroup::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_strGroup.Trim();
	if (m_strGroup.IsEmpty())
	{
		AfxMessageBox("组名不能为空!");
		return;
	}
	else
	{
		m_nChanged = AddGroup();
		if (m_nChanged)
			return CCxBCGPDialog::OnOK();
	}
	
	return ;
}



int CDlgAddGroup::AddGroup() 
{
	{
		ZTreeParam param;
		memset((void*)&param, 0,sizeof(ZTreeParam));
		UpdateData(TRUE);
		m_strGroup.Trim();
		if(CStringChecker::CheckParamName(m_strGroup) == FALSE)
		{
			MessageBox( "组名称" + CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
			return 0;
		}
		if (ZTreeParamSet::CheckSameGroup(m_JvStrMap,m_strGroup))
		{
			MessageBox( "该组名称已经存在！", g_lpszAppTitle, MB_ICONWARNING);
			return 0;
		}

		Json::Value jstmp;
		JS_NODE*pJsNode = new JS_NODE;
		pJsNode->strGroupName = m_strGroup;
		param.m_strName = m_strGroup;
		param.m_bIsGroup = TRUE;
		pJsNode->jsGroup = CDlgParamSetBase::ParamToJsonValue(param);
		m_JvStrMap.insert(pair<CString, JS_NODE*>(m_strGroup, pJsNode));
	}
	return 1;
}

MAPSTR_JV CDlgAddGroup::GetGroupList()
{
	return m_JvStrMap;
}
