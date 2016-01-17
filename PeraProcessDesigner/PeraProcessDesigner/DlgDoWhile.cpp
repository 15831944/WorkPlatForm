// DlgDoWhile.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgDoWhile.h"
#include "afxdialogex.h"


// CDlgDoWhile 对话框

IMPLEMENT_DYNAMIC(CDlgDoWhile, CDlgWhile)

CDlgDoWhile::CDlgDoWhile(CWnd* pParent /*=NULL*/)
	: CDlgWhile(pParent)
{

}

CDlgDoWhile::~CDlgDoWhile()
{
}

void CDlgDoWhile::DoDataExchange(CDataExchange* pDX)
{
	CDlgWhile::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDoWhile, CDlgWhile)
END_MESSAGE_MAP()


// CDlgDoWhile 消息处理程序
