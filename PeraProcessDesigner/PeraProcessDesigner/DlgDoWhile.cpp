// DlgDoWhile.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgDoWhile.h"
#include "afxdialogex.h"


// CDlgDoWhile �Ի���

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


// CDlgDoWhile ��Ϣ�������
