// TreePopDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "TreeDropDownDlg.h"
#include "afxdialogex.h"


// CTreePopDlg 对话框

IMPLEMENT_DYNAMIC(CTreeDropDownDlg, CDialogEx)
CTreeDropDownDlg::CTreeDropDownDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTreeDropDownDlg::IDD, pParent)
{
}

CTreeDropDownDlg::~CTreeDropDownDlg()
{
}

void CTreeDropDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTreeDropDownDlg, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CTreePopDlg 消息处理程序


int CTreeDropDownDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}


BOOL CTreeDropDownDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW); 
	SetWindowPos(NULL,0,0,276,300,SWP_NOZORDER|SWP_NOMOVE);
	InitTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void  CTreeDropDownDlg::InitTree()
{
	CRect rectGrid;
	GetClientRect(&rectGrid);
	rectGrid.DeflateRect(1, 1, 1, 1);
	m_IfEditTree.InitTree(rectGrid, this);
	m_IfEditTree.EnableHeader(FALSE);
	m_IfEditTree.SetSingleSel(TRUE);
	m_IfEditTree.SetColumnVisible(CZGridCtrl::COL_TYPE, TRUE);
	m_IfEditTree.SetColumnWidth(CZGridCtrl::COL_NAME,180);
	m_IfEditTree.SetColumnWidth(CZGridCtrl::COL_TYPE,120);

	if (1)
	{
		//查找当前分支节点所属的黑盒节点
		CCxStructTreeNode* pNodeCur = theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData();
		CCxStructTreeNode* pNodeParent = NULL;
		pNodeParent = pNodeCur;

		while (pNodeParent->m_pParent)
		{
			if (pNodeParent->m_pParent->m_NodeType == CCxStructTreeNode::NT_ROOT
				|| pNodeParent->m_pParent->m_NodeType == CCxStructTreeNode::NT_SUBFLOW)
			{
				pNodeParent = pNodeParent->m_pParent;
				break;
			}
			else
			{
				pNodeParent = pNodeParent->m_pParent;
			}
		}
		m_IfEditTree.AddNodeSubFlow( pNodeParent, NULL );
	}
	else
	{
		if(TRUE)
		{//根节点
			m_IfEditTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot(),NULL );
		}
		else
		{//当前节点
			m_IfEditTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData(),NULL );
		}
	}
	m_IfEditTree.AdjustLayout();
}