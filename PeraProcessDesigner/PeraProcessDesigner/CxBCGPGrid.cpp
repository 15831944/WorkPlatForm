#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "CxBCGPGrid.h"
#include "DlgStart.h"
#include "CxBCGPGridItem.h"

BEGIN_MESSAGE_MAP(CCxBCGPGrid, CBCGPGridCtrl)
	ON_COMMAND_RANGE(ID_GRID_BUTTON_FIRST, ID_GRID_BUTTON_FIRST, OnButtonClicked)
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


void CCxBCGPGrid::OnButtonClicked (UINT uID)
{
	if ( !IsWindow( m_hwndRecvMsg ) ) return;

	this->StopSelectItems();
	::SendMessage ( m_hwndRecvMsg, WM_COMMAND, MAKEWPARAM (uID, BN_CLICKED), 0 );
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
	CBCGPGridRow* pRow = GetCurSel();
	if(!pRow)return;
	CCxGridButtonItem* pItem =  (CCxGridButtonItem*)pRow->GetItem(CDlgStart::COL_OP);
	pItem->OnClickValue(0, CPoint());
	CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);
}
