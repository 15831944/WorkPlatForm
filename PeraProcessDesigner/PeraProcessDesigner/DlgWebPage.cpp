// DlgWebPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgWebPage.h"
#include "afxdialogex.h"


// CDlgWebPage 对话框

IMPLEMENT_DYNAMIC(CDlgWebPage, CDialogEx)

CDlgWebPage::CDlgWebPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWebPage::IDD, pParent)
{

}

CDlgWebPage::~CDlgWebPage()
{
}

void CDlgWebPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWebPage, CDialogEx)
END_MESSAGE_MAP()


// CDlgWebPage 消息处理程序
