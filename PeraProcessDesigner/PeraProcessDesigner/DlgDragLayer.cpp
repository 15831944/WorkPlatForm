// DlgDragLayer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgDragLayer.h"
#include "afxdialogex.h"


// CDlgDragLayer �Ի���

BEGIN_MESSAGE_MAP(CDlgDragLayer, CSvgDragLayer)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDlgDragLayer, CSvgDragLayer)

CDlgDragLayer::CDlgDragLayer(CWnd* pParent /*=NULL*/)
	: CSvgDragLayer(CDlgDragLayer::IDD, pParent)
{

}

CDlgDragLayer::~CDlgDragLayer()
{
}

void CDlgDragLayer::DoDataExchange(CDataExchange* pDX)
{
	CSvgDragLayer::DoDataExchange(pDX);
}



