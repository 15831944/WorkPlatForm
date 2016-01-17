// DlgForEachList.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForEachList.h"
#include "afxdialogex.h"
#include "DlgForEach.h"


// CDlgForEachList 对话框

IMPLEMENT_DYNAMIC(CDlgForEachList, CDlgForBase)

CDlgForEachList::CDlgForEachList(CWnd* pParent /*=NULL*/)
	: CDlgForBase(pParent)
{

}

CDlgForEachList::~CDlgForEachList()
{
}

void CDlgForEachList::DoDataExchange(CDataExchange* pDX)
{
	CDlgForBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgForEachList, CDlgForBase)
	ON_CBN_SETFOCUS(IDC_COMBO_ID, &CDlgForEachList::OnCbnFocusCombo)
END_MESSAGE_MAP()

void CDlgForEachList::OnCbnFocusCombo()
{
	//(GetParent()->GetParent())->SendMessage(CBN_SETFOCUS, IDC_COMBO_ID, IDC_COMBO_ID);
	((CDlgForEach*)(GetParent()->GetParent()))->OnCbnFocusCombo();
}


// CDlgForEachList 消息处理程序
