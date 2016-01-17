#include "StdAfx.h"
#include "CxBCGPGrid.h"

BEGIN_MESSAGE_MAP(CCxBCGPGrid, CBCGPGridCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCxBCGPGrid, CBCGPGridCtrl)

CCxBCGPGrid::CCxBCGPGrid(void)
{
	m_hwndRecvMsg = NULL;
}


CCxBCGPGrid::CCxBCGPGrid( int nMemBlockSize )
	: CBCGPGridCtrl(nMemBlockSize)
{
	m_hwndRecvMsg = NULL;
}

CCxBCGPGrid::~CCxBCGPGrid(void)
{
	m_hwndRecvMsg = NULL;
}
void CCxBCGPGrid::SetRowHeight()
{
	m_nRowHeight = 25;//tm.tmHeight + 2 * 2;
	m_nLargeRowHeight = m_nRowHeight;
	m_nBaseHeight = 25;
}

int CCxBCGPGrid::GetLeftMarginWidth() const
{
	return 0;
}

void CCxBCGPGrid::InitConstructor()
{
	CBCGPGridCtrl::InitConstructor();
}

void CCxBCGPGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_hwndRecvMsg==NULL) return;
	CBCGPGridRow* pRow = GetCurSel(); if(!pRow)return;
	int iRow = pRow->GetRowId();

	::PostMessage(m_hwndRecvMsg,WM_MSG_GRID_DBCLK,iRow,NULL);

	CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);
}
