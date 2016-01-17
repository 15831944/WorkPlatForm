// CxFloatDlgBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "BarStructTree.h"
//#include "CxArray1D.h"
//#include "CxArray2D.h"
//#include "CxArray3D.h"
//#include "DlgArrayEditor.h"
#include "CxArrayND.h"
#include "DlgNDArrayEditor.h"
#include "DlgSvg.h"
#include "ConvertData.h"
#include "DlgStructTreeFileParamEditor.h"
#include "CStringChecker.h"

static int g_nCloseButtonMargin = 1;
static int g_nCaptionVertMargin = 2;
//static int g_nCaptionHorzMargin = 5;

// CCxFloatDlgBar

BEGIN_MESSAGE_MAP(CBarStructTree, CCxFloatBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, &CBarStructTree::OnGridTreeSelChanged)
	//ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, &CBarStructTree::OnGridTreeItemChanged)
	//ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_BEGININPLACEEDIT, &CBarStructTree::OnGridTreeBeginInplaceEdit)
	//ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_ENDINPLACEEDIT, &CBarStructTree::OnGridTreeEndInplaceEdit)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CBarStructTree, CCxFloatBar)

CBarStructTree::CBarStructTree()
{
}

CBarStructTree::~CBarStructTree()
{
}


int CBarStructTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCxFloatBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect(&rcClient);
	m_wndGridTree.InitTree(rcClient, this);

	CCxStructTreeNode * pNode = NULL;

	UpdateTree();

	return 0;
}


void CBarStructTree::OnSize(UINT nType, int cx, int cy)
{
	CCxFloatBar::OnSize(nType, cx, cy);
	//m_wndGridTree.SetWindowPos (NULL, -1, -1, cx, cy,
	//	SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndGridTree.SetWindowPos (NULL, 1, -1, cx - 2, cy 
		, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CBarStructTree::UpdateTree()
{
	m_wndGridTree.AdjustLayout ();
}

int CBarStructTree::GetCaptionHeight () const
{
	if (IsFloating () || IsMDITabbed () || m_cyGripper == 0)
	{
		return 0;
	}

	return m_cyGripper + CBCGPVisualManager::GetInstance ()->GetDockingBarCaptionExtraHeight ();
}

void CBarStructTree::OnEraseNCBackground (CDC* pDC, CRect rcBar)
{
	ASSERT_VALID (this);
	//theApp.m_nFlagFillBarBackground = CF_BAR_JIANMO_STRUCTTREE;
	CBCGPVisualManager::GetInstance ()->OnFillBarBackground (pDC, this, rcBar, rcBar, 
		TRUE /* NC area */);
	//theApp.m_nFlagFillBarBackground = 0;
}

void CBarStructTree::OnPaint() 
{
	ASSERT_VALID (this);
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(&rcClient);

	//CBCGPDrawManager dm( dc );
	//dm.DrawRect(rcClient, g_crStructTreeClientBg, g_crStructTreeClientBg);
}