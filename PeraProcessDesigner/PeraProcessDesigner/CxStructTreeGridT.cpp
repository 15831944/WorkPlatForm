// CxBCGPGridCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxStructTreeGridT.h"
#include "UtilityFunc.h"
#include "Resource.h"
#include "PtrVectorHeader.h"
#include "ConvertData.h"
#include "ResLibImageSet.h"
#include "DlgSvg.h"
#include "WebDataDefine.h"
#include "CxBCGPVisualManager2007.h"
#include "PeraProcessDesignerUtility.h"
#include "DlgStructTreePropComponent.h"
#include <ShoshTreeCtrl.h>
#include "TrackMouse.h"
#include "DlgIf.h"
#include "CStringChecker.h"
#include "CxStructTreeNodeParam.h"
#include "CxStructTreeNodeFlow.h"
#include "CxStructTreeNodeSubFlow.h"
#include "CxStructTreeNodeForm.h"
#include <Tools.h>
#include <DbgHelp.h>
#include "FlexRobotWithForm.h"

static LPCTSTR arrColName[] = {
	"名称"
	, "值"
	, "类型"
	, "方向"
	, "单位"
};

static int arrColWidth[] = {
	200
	, 100
	, 96
	, 64
	, 64
};

//控制行高
#define TEXT_VMARGIN	(2+2)

#define STRETCH_DELTA	2

#ifndef _BCGPGRID_STANDALONE
#ifndef _BCGSUITE_
#define visualManagerMFC	CCxBCGPVisualManager2007::GetInstance ()
#else
#define visualManagerMFC	CMFCVisualManager::GetInstance ()
#endif

#define visualManager		CCxBCGPVisualManager2007::GetInstance ()
#else
#define visualManager		CBCGPGridVisualManager::GetInstance ()
#define visualManagerMFC	CBCGPGridVisualManager::GetInstance ()
#endif

//用于恢复上次编辑的值
static CString g_strLastValue;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCxStructTreeEditT, CEdit)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCxStructTreeEditT, CEdit)

CCxStructTreeEditT::CCxStructTreeEditT()
{
	m_pItem = NULL;
}

CCxStructTreeEditT::~CCxStructTreeEditT()
{
}

BOOL CCxStructTreeEditT::PreTranslateMessage(MSG* pMsg)
{
	//判断是否为键盘消息
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		UINT nRepCnt = LOWORD(pMsg->lParam);
		UINT nFlags = HIWORD(pMsg->lParam);
		//是否持续按键（上次按住后，一直没有抬起来）
		BOOL bHolding = ((nFlags & 0x4000) == 0x4000);
		BOOL bCtrl = GetAsyncKeyState(VK_CONTROL);
		BOOL bShift = GetAsyncKeyState(VK_SHIFT);

		if ( pMsg->wParam == VK_SPACE )
		{
			if ( m_pItem != NULL )
			{
				CCxStructTreeRowT * pRow = (CCxStructTreeRowT*)m_pItem->GetParentRow();
				if ( pRow != NULL )
				{
					CCxStructTreeNode * pNode = reinterpret_cast< CCxStructTreeNode * >( pRow->GetData() );
					if ( pNode != NULL )
					{
						if ( pNode->IsParamNode() )
						{
							CCxStructTreeNodeParam * pParamNode = (CCxStructTreeNodeParam*)pNode;
							if ( pParamNode->IsInt() || pParamNode->IsDouble() )
							{
								return TRUE;
							}
						}
					}
				}
			}
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CCxStructTreeItemT, CBCGPGridItem, VERSIONABLE_SCHEMA | 1)

CCxStructTreeItemT::CCxStructTreeItemT ()
{
}
//****************************************************************************************
CCxStructTreeItemT::CCxStructTreeItemT(const _variant_t& varValue, DWORD_PTR dwData,
		LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
		LPCTSTR lpszValidChars) : CBCGPGridItem(varValue, dwData, lpszEditMask, lpszEditTemplate
		, lpszValidChars)
{
}

CCxStructTreeItemT::~CCxStructTreeItemT()
{

}
//*******************************************************************************************
CString CCxStructTreeItemT::GetValueTooltip ()
{
	ASSERT_VALID (this);
	//if (m_bValueIsTrancated) return _T("");
	CCxStructTreeRowT * pRow = (CCxStructTreeRowT*)GetParentRow();
	if ( pRow == NULL ) return "";
	CCxStructTreeNode * pNode = reinterpret_cast<CCxStructTreeNode*>(pRow->GetData());
	if ( pNode == NULL ) return "";
	return pNode->GetTooltipText();
}
//******************************************************************************************
COLORREF CCxStructTreeItemT::OnFillBackground (CDC* pDC, CRect rect)
{
	CCxStructTreeRowT * pRow = (CCxStructTreeRowT *)m_pGridRow;
	CCxStructTreeGridT* pWndList = (CCxStructTreeGridT*)pRow->m_pWndList;
	ASSERT_VALID (pWndList);

	COLORREF clrText = m_clrText != (COLORREF)-1 ? m_clrText : pWndList->m_ColorData.m_clrText;

	CCxStructTreeItemT* pCurSelItem = (CCxStructTreeItemT*)pWndList->GetCurSelItem ();

	BOOL bNameCol = FALSE;
	if ( pCurSelItem != NULL )
		bNameCol = ( pCurSelItem->GetColumnId() == CCxStructTreeGridT::COL_NAME );

	//BOOL bActiveItem =	pCurSelItem == this && 
	//					pWndList->AllowInPlaceEdit ();
	BOOL bActiveItem =	FALSE;

	if (GetMergedCells () != NULL && pCurSelItem != NULL)
	{
		if (pCurSelItem->GetMergedCells () == GetMergedCells () &&
			pWndList->AllowInPlaceEdit ())
		{
			bActiveItem = TRUE;
		}
	}

	BOOL bNoHighlightActiveItem = !pWndList->IsHighlightActiveItem ();// || 
		//pWndList->IsSelectionBorderEnabled (); // BUG
	BOOL bSelectionBorderActiveItem = pWndList->IsSelectionBorderForActiveItem () && 
		!pWndList->IsSelectionBorderEnabled ();

	CRect rectFill = rect;
	rectFill.top++;

	BOOL bSelected = 
		(pWndList->m_bSingleSel && pWndList->m_bWholeRowSel) ?
			m_pGridRow->IsSelected () :
			IsSelected ();

	if (bSelected && 
		pRow->HasValueField () && 
		!(bActiveItem && bNoHighlightActiveItem || m_bInPlaceEdit))
	{
		if (pWndList->m_bFocused && pWndList->IsWholeRowSel ())
		{
			rectFill.right++;
		}
		clrText = pWndList->OnFillSelItem (pDC, rectFill, this);
	}
	else if (bActiveItem && bNoHighlightActiveItem)
	{
		if (!m_rectButton.IsRectEmpty ())
		{
			rectFill.right = m_rectButton.left;
		}

		if (bSelectionBorderActiveItem || pWndList->IsSelectionBorderEnabled () &&
			(pWndList->m_lstSel.GetCount () > 1 || pWndList->IsGrouping ()))
		{
			pDC->FillRect (rectFill, &globalData.brBlack);

			rectFill.DeflateRect (1, 1);
		}

		if (pWndList->m_brBackground.GetSafeHandle () != NULL)
		{
			pDC->FillRect (rectFill, &pWndList->m_brBackground);
		}
		else
		{
			COLORREF clr = visualManager->OnFillGridItem (
				pWndList, pDC, rectFill, bSelected, bActiveItem, FALSE);
			if (clrText == (COLORREF)-1)
			{
				clrText = clr;
			}
		}
	}
	else 
	{
		CBCGPGridItemID id = pWndList->GetGridItemID (this);
		BOOL bCustomColors = FALSE;

		// Item has own color - first priority
		if (m_clrBackground != (COLORREF)-1)
		{
			CBrush br (m_clrBackground);
			pDC->FillRect (rectFill, &br);
			bCustomColors = TRUE;
		}

		// Use m_ColorData to get colors
		else if (!id.IsNull ())
		{
			if (pWndList->OnAlternateColor (id))
			{
				bCustomColors = pWndList->m_ColorData.m_EvenColors.Draw (pDC, rectFill);
				if (m_clrText == (COLORREF)-1 &&
					pWndList->m_ColorData.m_EvenColors.m_clrText != (COLORREF)-1)
				{
					clrText = pWndList->m_ColorData.m_EvenColors.m_clrText;
				}
			}
			else
			{
				bCustomColors = pWndList->m_ColorData.m_OddColors.Draw (pDC, rectFill);
				if (m_clrText == (COLORREF)-1 &&
					pWndList->m_ColorData.m_OddColors.m_clrText != (COLORREF)-1)
				{
					clrText = pWndList->m_ColorData.m_OddColors.m_clrText;
				}
			}
		}

		if (!bCustomColors)
		{
			// If the column of this item is sorted
			BOOL bSortedColumn = (pWndList->m_bMarkSortedColumn &&
				!id.IsNull () &&
				(pWndList->GetColumnsInfo ().GetColumnState (id.m_nColumn) != 0));

			COLORREF clr = visualManager->OnFillGridItem (
				pWndList, pDC, rectFill, bSelected, bActiveItem && bNoHighlightActiveItem, bSortedColumn);
			if (clrText == (COLORREF)-1)
			{
				clrText = clr;
			}
		}
	}

	return clrText;
}
//******************************************************************************************
void CCxStructTreeItemT::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	CCxStructTreeGridT* pWndList = (CCxStructTreeGridT*)((CCxStructTreeRowT*)m_pGridRow)->m_pWndList;
	ASSERT_VALID (pWndList);

	COLORREF clrTextOld = (COLORREF)-1;
	COLORREF clrText = OnFillBackground (pDC, rect);

	if (clrText != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor (clrText);
	}

	//-----------
	// Draw icon:
	//-----------
	OnDrawIcon (pDC, rect);

	// -----------
	// Draw value:
	// -----------
	rect.DeflateRect (TEXT_MARGIN, TEXT_VMARGIN);

	// If merged, refer to main item
	CBCGPGridMergedCells* pMerged = GetMergedCells ();
	const int nColumn = (pMerged != NULL) ? pMerged->GetMainItemID ().m_nColumn : GetGridItemID ().m_nColumn;

	int nTextAlign = pWndList->GetColumnAlign (nColumn);

	// If merged, refer to main item
	CBCGPGridItem* pMergedItem = GetMergedMainItem ();
	const CString& strText = (pMergedItem != NULL) ? pMergedItem->GetLabel () : GetLabel ();
	const DWORD dwFlags = (pMergedItem != NULL) ? pMergedItem->GetFlags () : GetFlags ();

	UINT uiTextFlags = DT_NOPREFIX | DT_END_ELLIPSIS;

	if (nTextAlign & HDF_CENTER)
	{
		uiTextFlags |= DT_CENTER;
	}
	else if (nTextAlign & HDF_RIGHT)
	{
		uiTextFlags |= DT_RIGHT;
	}
	else // nTextAlign & HDF_LEFT
	{
		uiTextFlags |= DT_LEFT;
	}

	if (dwFlags & BCGP_GRID_ITEM_VTOP)
	{
		uiTextFlags |= DT_TOP;
	}
	else if (dwFlags & BCGP_GRID_ITEM_VBOTTOM)
	{
		uiTextFlags |= DT_BOTTOM;
	}
	else // dwFlags & BCGP_GRID_ITEM_VCENTER
	{
		uiTextFlags |= DT_VCENTER;
	}

	if (!(dwFlags & BCGP_GRID_ITEM_MULTILINE))
	{
		uiTextFlags |= DT_SINGLELINE;
	}
	else
	{
		if (dwFlags & BCGP_GRID_ITEM_WORDWRAP)
		{
			uiTextFlags |= DT_WORDBREAK;
		}
	}

	m_bValueIsTrancated = pWndList->DoDrawText (pDC, strText, rect, uiTextFlags);

	if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor (clrTextOld);
	}
}

//*****************************************************************************
CEdit* CCxStructTreeItemT::NewInPlaceEdit ()
{
	CEdit * pEdit = new CCxStructTreeEditT;
	((CCxStructTreeEditT*)pEdit)->m_pItem = this;
	return pEdit;
}

void CCxStructTreeItemT::FormatDoubleValue( CString& strValue )
{
	CCxStructTreeRowT* pRow = (CCxStructTreeRowT*)GetParentRow();
	if ( pRow == NULL ) return;
	CCxStructTreeNode* pNode = (CCxStructTreeNode*)pRow->GetData();
	if ( pNode == NULL ) return;
	if ( !pNode->IsParamNode() ) return;
	CCxStructTreeNodeParam* pParamNode = (CCxStructTreeNodeParam*)pNode;
	if ( !pParamNode->IsDouble() ) return;

	CString str = CStringChecker::FormatDouble( (LPCTSTR)strValue );
	strValue = str;

}

//******************************************************************************************
BOOL CCxStructTreeItemT::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	CString strText;
	m_pWndInPlace->GetWindowText (strText);
	FormatDoubleValue( strText );

	// If merged, refer to main item
	CBCGPGridItem* pMergedItem = GetMergedMainItem ();
	CBCGPGridItem* pUpdateItem = (pMergedItem != NULL) ? pMergedItem : this;
	ASSERT_VALID (pUpdateItem);

	BOOL bRes = TRUE;
	BOOL bIsChanged = FormatItem () != strText;
	BOOL bUpdateData = bIsChanged && pUpdateItem->CanUpdateData ();

	if (bUpdateData)
	{
		bRes = pUpdateItem->TextToVar (strText);
	}

	if (bRes && bUpdateData)
	{
		pUpdateItem->SetItemChanged ();
		CCxStructTreeGridT * pGridT = dynamic_cast<CCxStructTreeGridT*>(pUpdateItem->GetOwnerList());
		if (pGridT!=NULL)
		{
			pGridT->m_bModified = TRUE;
		}
	}

	return bRes;
}


//////////////////////////////////////////////////////////////////////////


IMPLEMENT_SERIAL(CCxStructTreeRowT, CBCGPGridRow, VERSIONABLE_SCHEMA | 1)

CCxStructTreeRowT::CCxStructTreeRowT (const CString& strGroupName
	, DWORD_PTR dwData, int nBlockSize)
{
	CBCGPGridRow::CBCGPGridRow( strGroupName, dwData, nBlockSize );
	m_bExpanded = FALSE;
}
//******************************************************************************************
CCxStructTreeRowT::CCxStructTreeRowT (int nColumnsNum, DWORD_PTR dwData, int nBlockSize)
{
	CBCGPGridRow::CBCGPGridRow( nColumnsNum, dwData, nBlockSize );
	//m_bCustomExpandFlag = FALSE;
	m_bExpanded = FALSE;
}

//******************************************************************************************
void CCxStructTreeRowT::OnDrawItems (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	CCxStructTreeGridT * pList = ( CCxStructTreeGridT * ) m_pWndList;
	CCxStructTreeNode * pNode = reinterpret_cast<CCxStructTreeNode *> ( GetData() );

	rect.NormalizeRect ();
	const int nFreezeOffset = pList->m_rectList.left + pList->GetColumnsInfo ().GetFreezeOffset ();

	for (int i = 0; i < m_arrRowItems.GetSize (); i++)
	{
		CCxStructTreeItemT* pItem = (CCxStructTreeItemT*)m_arrRowItems [i];
		ASSERT_VALID (pItem);

		CRect rectValue = pItem->GetRect ();
		if (!rectValue.IsRectEmpty ())
		{
			if (pItem->GetMergedCells () != NULL)
			{
				// Item is a part of merged item - Draw item later.
				pList->RedrawMergedItem (pItem);
				continue;
			}

			rectValue.NormalizeRect ();

			CRect rectClipItem = rectValue;

			// frozen columns:
			if (pList->GetColumnsInfo ().IsFreezeColumnsEnabled () &&
				!pList->GetColumnsInfo ().IsColumnFrozen (i))
			{
				// Do not allow unfrozen columns to draw inside the frozen area
				rectClipItem.left = max (nFreezeOffset, rectClipItem.left);
				if (rectClipItem.left >= rectClipItem.right)
				{
					continue;
				}
			}

			rectClipItem.IntersectRect (rectClipItem, rect);

			CRect rectBordersSize (0, 0, 0, 0);
			if (pList->m_bGridItemBorders)
			{
				pItem->OnGetBorders (rectBordersSize);
				rectClipItem.InflateRect (rectBordersSize);
			}

			pList->m_rgnClipItem.CreateRectRgnIndirect (&rectClipItem);
			pDC->SelectClipRgn (&pList->m_rgnClipItem);

			pItem->OnDrawValue (pDC, rectValue);

			if (pList->m_bGridItemBorders)
			{
				pItem->OnDrawBorders (pDC, rectValue);
			}

			//if (!pItem->m_rectButton.IsRectEmpty ())
			//{
			//	pItem->OnDrawButton (pDC, pItem->m_rectButton);
			//}

			pDC->SelectClipRgn (&pList->m_rgnClipRow);
			pList->m_rgnClipItem.DeleteObject ();
		}
	}
}

BOOL CCxStructTreeRowT::AddSubItem( CBCGPGridRow* pItem, int nPos /*= -1*/, BOOL bRedraw /*= TRUE*/ )
{
	if ( nPos == -1 ) return CBCGPGridRow::AddSubItem( pItem, bRedraw );

	CCxStructTreeRowT * pRow = ( CCxStructTreeRowT * )pItem;

	ASSERT_VALID (this);
	ASSERT_VALID (pRow);

	//CCxStructTreeNode * pNode = reinterpret_cast< CCxStructTreeNode * > ( pRow->GetData() );
	//if ( pNode == NULL ) return FALSE;

	if (!IsGroup ())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pRow->m_pParent = this;

	int nPosParent = m_nIdRow;
	int nSubItemsCount = 0;

	for (int i=0; i<nPos; i++)
	{
		POSITION pos = m_lstSubItems.FindIndex( i );
		CCxStructTreeRowT * pChildRow = ( CCxStructTreeRowT * )m_lstSubItems.GetAt(pos);
		//CCxStructTreeRowT * pChildRow = GetNodeRow( pNode->m_vChildren[i]->m_strID );
		if ( pChildRow == NULL ) return FALSE;
		CCxStructTreeNode * pNode = reinterpret_cast< CCxStructTreeNode* > ( pChildRow->GetData() );
		nSubItemsCount += pChildRow->GetSubItemsCount(TRUE) + 1;
	}

	//int nSubItemsCount = GetSubItemsCount (TRUE);
	int nPosInsertAfter = nPosParent + nSubItemsCount;
	m_pWndList->InsertRowAfter (nPosInsertAfter, pRow, bRedraw);

	if ( nPos == 0 )
	{
		m_lstSubItems.AddHead ( pRow );
	}
	else
	{
		POSITION posAfter = m_lstSubItems.FindIndex( nPos - 1 );
		m_lstSubItems.InsertAfter(posAfter, pRow);
	}

	//m_lstSubItems.AddTail (pRow);
	pRow->m_pWndList = m_pWndList;

	return TRUE;
}

void CCxStructTreeRowT::OnDrawExpandBox (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (m_pWndList);
	ASSERT (IsGroup ());

	BOOL bIsOpened = m_bExpanded && !m_lstSubItems.IsEmpty ();

	CCxStructTreeGridT * pList = ( CCxStructTreeGridT * ) m_pWndList;
	if (pList->m_bIsPrinting)
	{
		// map to printer metrics
		ASSERT_VALID (pList->m_pPrintDC);
		CSize szOne = pList->m_PrintParams.m_pageInfo.m_szOne;

		CPoint ptCenter = rect.CenterPoint ();
		// align to integral logical point
		ptCenter.Offset (-(ptCenter.x % szOne.cx), -(ptCenter.y % szOne.cy));

		int nMinSize = (szOne.cx != 0 && szOne.cy != 0) ?
			min (rect.Width () / szOne.cx, rect.Height () / szOne.cy) : 0;
		int nBoxSize = min (9, nMinSize); // in logical units

		rect = CRect (ptCenter, szOne);
		rect.InflateRect ((nBoxSize / 2) * szOne.cx, (nBoxSize / 2) * szOne.cy);

		CPen penLine (PS_SOLID, 1, pList->m_clrPrintText);
		CPen* pOldPen = pDC->SelectObject (&penLine);

		pDC->MoveTo (rect.TopLeft ());
		pDC->LineTo (rect.right, rect.top);
		pDC->LineTo (rect.BottomRight ());
		pDC->LineTo (rect.left, rect.bottom);
		pDC->LineTo (rect.TopLeft ());

		rect.DeflateRect (2 * szOne.cx, 2 * szOne.cy);

		pDC->MoveTo (rect.left, ptCenter.y + szOne.cy / 2);
		pDC->LineTo (rect.right, ptCenter.y + szOne.cy / 2);

		if (!bIsOpened)
		{
			pDC->MoveTo (ptCenter.x + szOne.cx / 2, rect.top);
			pDC->LineTo (ptCenter.x + szOne.cx / 2, rect.bottom);
		}

		pDC->SelectObject (pOldPen);
	}
	else
	{
		CPoint ptCenter = rect.CenterPoint ();
		ptCenter.Offset(0, -2);

		int nBoxSize = min (9, rect.Width ());

		rect = CRect (ptCenter, CSize (1, 1));
		rect.InflateRect (nBoxSize / 2, nBoxSize / 2);

		COLORREF clrText = pList->m_bControlBarColors ? 
			globalData.clrBarText : globalData.clrBtnText;

		if (pList->m_bControlBarColors)
		{
			visualManager->OnDrawGridExpandingBox (pDC, rect, 
				bIsOpened, clrText);
		}
		else
		{
			visualManagerMFC->OnDrawExpandingBox (pDC, rect,
				bIsOpened, clrText);
		}
	}
}

//******************************************************************************************
CBCGPGridItem* CCxStructTreeRowT::CreateItem (int nRow, int nColumn)
{
	if (m_pWndList == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	ASSERT_VALID (m_pWndList);
	return m_pWndList->CreateItem (nRow, nColumn);
}

void CCxStructTreeRowT::OnExpand (BOOL bExpand)
{
	//m_bCustomExpandFlag = bExpand;
}

// CCxBCGPGridCtrl

BEGIN_MESSAGE_MAP(CCxStructTreeGridT, CBCGPGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_INITMENUPOPUP()
	//根节点
	ON_COMMAND(IDM_JIANMO_TREE_PROCESS_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeProcessEdit)
	ON_COMMAND(IDM_JIANMO_TREE_PROCESS_EDIT_FOR, &CCxStructTreeGridT::OnMenuJianmoTreeProcessEditFor)
	ON_COMMAND(IDM_JIANMO_TREE_PROCESS_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeProcessRun)
	ON_COMMAND(IDM_JIANMO_TREE_PROCESS_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishProcess)
	ON_COMMAND(IDM_JIANMO_TREE_PROCESS_DATAMAPPING, &CCxStructTreeGridT::OnMenuStructTreeProcessDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_PROCESS_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeProcessProp)
	//子流程节点
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeFlowRun)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishFlow)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SAVEAS, &CCxStructTreeGridT::OnMenuStructTreeFlowSaveAs)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_DATAMAPPING, &CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeFlowProp)
	//子流程节点for
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeFlowRun)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishFlow)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_SAVEAS, &CCxStructTreeGridT::OnMenuStructTreeFlowSaveAs)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_DATAMAPPING, &CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_FOR_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeFlowProp)
	//子流程节点if
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_EDITIF, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEditIf)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeFlowRun)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishFlow)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_SAVEAS, &CCxStructTreeGridT::OnMenuStructTreeFlowSaveAs)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_DATAMAPPING, &CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_IF_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeFlowProp)
	//子流程节点parallel
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeFlowRun)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishFlow)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_SAVEAS, &CCxStructTreeGridT::OnMenuStructTreeFlowSaveAs)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_DATAMAPPING, &CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_PARALLEL_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeFlowProp)
	//子流程节点sequence
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeFlowRun)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishFlow)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_SAVEAS, &CCxStructTreeGridT::OnMenuStructTreeFlowSaveAs)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_DATAMAPPING, &CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SEQUENCE_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeFlowProp)
	//子流程节点subflow(黑盒)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SUBFLOW_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SUBFLOW_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeFlowRun)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SUBFLOW_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SUBFLOW_DATAMAPING, &CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_FLOW_SUBFLOW_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeFlowProp)
	//组件节点
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeComponentEdit)
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_RUN, &CCxStructTreeGridT::OnMenuJianmoTreeComponentRun)
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeComponentDelete)
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_PUBLISH, &CCxStructTreeGridT::OnMenuStructTreePublishComponent)
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_SAVEAS, &CCxStructTreeGridT::OnMenuStructTreeComponentSaveAs)
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_DATAMAPPING, &CCxStructTreeGridT::OnMenuJianmoTreeComponentDataMapping)
	ON_COMMAND(IDM_JIANMO_TREE_COMPONENT_PROP, &CCxStructTreeGridT::OnMenuJianmoTreeComponentProp)
	//表单节点
	ON_COMMAND(IDM_JIANMO_TREE_FORM_EDIT, &CCxStructTreeGridT::OnMenuJianmoTreeFormEdit)
	ON_COMMAND(IDM_JIANMO_TREE_FORM_DELETE, &CCxStructTreeGridT::OnMenuJianmoTreeFormDelete)
	//列表头
	ON_COMMAND(IDM_JIANMO_TREE_HEADER_VALUE, &CCxStructTreeGridT::OnMenuJianmoTreeHeaderValue)
	ON_COMMAND(IDM_JIANMO_TREE_HEADER_TYPE, &CCxStructTreeGridT::OnMenuJianmoTreeHeaderType)
	ON_COMMAND(IDM_JIANMO_TREE_HEADER_DIRECTION, &CCxStructTreeGridT::OnMenuJianmoTreeHeaderDirection)
	ON_COMMAND(IDM_JIANMO_TREE_HEADER_UNITS, &CCxStructTreeGridT::OnMenuJianmoTreeHeaderUnits)
	//
	ON_UPDATE_COMMAND_UI(IDM_JIANMO_TREE_HEADER_VALUE, &CCxStructTreeGridT::OnUpdateHeaderValueStatus)
	ON_UPDATE_COMMAND_UI(IDM_JIANMO_TREE_HEADER_TYPE, &CCxStructTreeGridT::OnUpdateHeaderTypeStatus)
	ON_UPDATE_COMMAND_UI(IDM_JIANMO_TREE_HEADER_DIRECTION, &CCxStructTreeGridT::OnUpdateHeaderDirectionStatus)
	ON_UPDATE_COMMAND_UI(IDM_JIANMO_TREE_HEADER_UNITS, &CCxStructTreeGridT::OnUpdateHeaderUnitsStatus)
	ON_COMMAND(IDM_JIANMO_TREE_FORM_POPFORM, &CCxStructTreeGridT::OnJianmoTreeFormPopform)
	ON_COMMAND(IDM_JIANMO_TREE_FORM_NOTPOPFORM, &CCxStructTreeGridT::OnJianmoTreeFormNotpopform)
	ON_UPDATE_COMMAND_UI(IDM_JIANMO_TREE_FORM_POPFORM, &CCxStructTreeGridT::OnUpdateJianmoTreeFormPopform)
	ON_UPDATE_COMMAND_UI(IDM_JIANMO_TREE_FORM_NOTPOPFORM, &CCxStructTreeGridT::OnUpdateJianmoTreeFormNotpopform)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCxStructTreeGridT, CBCGPGridCtrl)

	//******************************************************************************************
BOOL CCxStructTreeGridT::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
#ifndef _BCGPGRID_STANDALONE
	if (CBCGPPopupMenu::GetActiveMenu () != NULL)
	{
		return TRUE;
	}
#endif

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return FALSE;
	}

	if (m_nVertScrollTotal <= m_nVertScrollPage)
	{
		return FALSE;
	}

	if ((m_fScrollRemainder > 0) != (zDelta > 0))
	{
		m_fScrollRemainder = 0;
	}

	UINT nLinesToScrollUserSetting;
	if (!::SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nLinesToScrollUserSetting, 0))
	{
		nLinesToScrollUserSetting = 1;
	}

	if(nLinesToScrollUserSetting == WHEEL_PAGESCROLL) // scroll one page at a time
	{
		OnVScroll (zDelta < 0 ? SB_PAGEDOWN : SB_PAGEUP, 0, NULL);
		return TRUE;
	}

	float fTotalLinesToScroll = ((float)zDelta / WHEEL_DELTA) * nLinesToScrollUserSetting + m_fScrollRemainder;

	int nSteps = abs((int)fTotalLinesToScroll);
	m_fScrollRemainder = fTotalLinesToScroll - (int)fTotalLinesToScroll;

	for (int i = 0; i < nSteps; i++)
	{
		OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
	}

	return TRUE;
}

//******************************************************************************************
void CCxStructTreeGridT::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return;
	}

	CBCGPGridRow* pSel = GetCurSel ();

// 	if (pSel && pSel->IsInPlaceEditing())
// 	{
// 		return;
// 	}

	if (pSel != NULL && 
		pScrollBar->GetSafeHwnd () != NULL)
	{
		CSpinButtonCtrl* pWndSpin = pSel->GetSpinWnd ();
		if (pWndSpin != NULL && 
			pWndSpin->GetSafeHwnd () == pScrollBar->GetSafeHwnd ())
		{
			return;
		}
	}

	SetEndEditReason (EndEdit_Cancel | EndEdit_Layout);
	EndEditItem (TRUE);

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_TRACKPOS;

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
	{
		GetScrollInfo(SB_VERT, &info);
		nPos = info.nTrackPos;
	}

	int nPrevOffset = m_nVertScrollOffset;

	int nVertScrollPage = m_nVertScrollPage;
	if (m_bVirtualMode)
	{
		int nItemsCount = max (1, m_nVertScrollPage / m_nRowHeight);
		nVertScrollPage = nItemsCount * m_nRowHeight;
	}

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nVertScrollOffset -= m_nRowHeight;
		break;

	case SB_LINEDOWN:
		m_nVertScrollOffset += m_nRowHeight;
		break;

	case SB_TOP:
		m_nVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nVertScrollOffset = m_nVertScrollTotal;
		break;

	case SB_PAGEUP:
		m_nVertScrollOffset -= nVertScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nVertScrollOffset += nVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	m_nVertScrollOffset = max (0, min (m_nVertScrollOffset, 
		m_nVertScrollTotal - nVertScrollPage + 1));
	if (m_bVirtualMode)
	{
		// row alignment
		int nResidue = m_nVertScrollOffset % m_nRowHeight;
		if (nResidue > 0)
		{
			m_nVertScrollOffset -= nResidue;
			if (m_nVertScrollOffset + m_nVertScrollPage < m_nVertScrollTotal)
			{
				m_nVertScrollOffset += m_nRowHeight;
			}
		}
	}

	if (m_nVertScrollOffset == nPrevOffset)
	{
		return;
	}

	OnUpdateVScrollPos (m_nVertScrollOffset, nPrevOffset);

	SetScrollPos (SB_VERT, m_nVertScrollOffset);

	int dy = nPrevOffset - m_nVertScrollOffset;

	ShiftItems (0, dy);
	OnPosSizeChanged ();
	m_rectTrackSel = OnGetSelectionRect ();
	ScrollWindow (0, dy, m_rectList, m_rectList);
	ScrollWindow (0, dy, m_rectRowHeader, m_rectRowHeader);

	pSel = GetCurSel ();
	if (pSel != NULL)
	{
		pSel->AdjustButtonRect ();
	}

	OnAfterVScroll (m_nVertScrollOffset, nPrevOffset);
	
	UpdateWindow ();
}

CCxStructTreeGridT::CCxStructTreeGridT()
{
	m_hbmp = NULL;
	m_bUseSystemFont = FALSE;
	m_bModified = FALSE;
	m_bMouseHover = FALSE;
	m_bTrackMouseHover = FALSE;
	m_bSetCapture = FALSE;
	//m_bTreeClicked = FALSE;
}

CCxStructTreeGridT::CCxStructTreeGridT( int nMemBlockSize )
	: CBCGPGridCtrl(nMemBlockSize)
{
	m_hbmp = NULL;
	//m_bTreeClicked = FALSE;
}

CCxStructTreeGridT::~CCxStructTreeGridT()
{
}

void CCxStructTreeGridT::UpdateSelStatus(CCxStructTreeRowT *& pCurRow)
{
	pCurRow = ( CCxStructTreeRowT * )GetCurSel();
	if (pCurRow == NULL) return;
	
	m_pSelItem = pCurRow->GetItem( COL_NAME );

	//SelectRow(pCurRow->GetRowId(), TRUE);

	CBCGPGridRange* pSelRange = 
		new CBCGPGridRange (COL_NAME, 0, COL_UNITS, GetRowCount());
	CList<CBCGPGridRange*, CBCGPGridRange*>	lstSel;
	lstSel.AddTail (pSelRange);
	while (!lstSel.IsEmpty ())
	{
		CBCGPGridRange* pRange = lstSel.RemoveTail ();
		//ASSERT (pRange != NULL);

		SelectRange (*pRange, FALSE, TRUE);
		delete pRange;
	}

	pSelRange = 
		new CBCGPGridRange (COL_NAME, pCurRow->GetRowId(), COL_NAME, pCurRow->GetRowId());
	lstSel.AddTail (pSelRange);
	while (!lstSel.IsEmpty ())
	{
		CBCGPGridRange* pRange = lstSel.RemoveTail ();
		//ASSERT (pRange != NULL);

		SelectRange (*pRange, TRUE, TRUE);
		delete pRange;
		//pCurRow->Redraw();
	}
}


void CCxStructTreeGridT::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CWnd::OnLButtonDown(nFlags, point);

	OnFilterBarSetFocus (-1);
	SetFocus ();

	//--------------------
	// Group-by-box click:
	//--------------------
	CRect rectColumn;
	int nGroupHit = HitTestGroupByBox (point, rectColumn);

	if (nGroupHit >= 0)
	{
		m_ptStartDrag = point;
		StartDragColumn (nGroupHit, rectColumn, TRUE, FALSE);
		return;
	}

	// -------------
	// Track header:
	// -------------
	CPoint ptHeader = point;
	if (OnTrackHeader () && m_rectList.PtInRect (point))
	{
		ptHeader.y = m_rectHeader.top;
	}
	int nColumnHit = GetColumnsInfo ().HitTestColumn (ptHeader, TRUE, STRETCH_DELTA);
	if (nColumnHit != -1)
	{
		int nLButton = GetSystemMetrics (SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
		if ((GetAsyncKeyState (nLButton) & 0x8000) == 0)
		{
			return;
		}

		if (!GetColumnsInfo ().CanChangeWidth (nColumnHit))
		{
			// column can't be resized
			return;
		}

		CRect rectHeaderColumn;
		GetColumnsInfo ().GetColumnRect (nColumnHit, rectHeaderColumn);

		SetCapture ();
		m_nTrackColumn = nColumnHit;
		TrackHeader (rectHeaderColumn.right);
		m_bTracking = TRUE;
		return;
	}

	if (IsDragSelectionBorderEnabled () && HitTestSelectionBorder (point))
	{
		if (StartDragItems (point))
		{
			return;
		}
	}

	//--------------
	// Header click:
	//--------------
	CBCGPGridColumnsInfo::ClickArea clickAreaHeader;
	nColumnHit = GetColumnsInfo ().HitTestColumn (point, FALSE, STRETCH_DELTA, &clickAreaHeader);

	if (nColumnHit >= 0)
	{
		DoColumnHeaderClick (nColumnHit, point, clickAreaHeader);
		return;
	}

	//-------------------------
	// "Select all" area click:
	//-------------------------
	if (m_rectSelectAllArea.PtInRect (point))
	{
		OnSelectAllClick ();
		return;
	}

	//------------------
	// Row header click:
	//------------------
	CRect rectRowHeader;
	CBCGPGridRow* pHitHeaderRow = HitTestRowHeader (point, rectRowHeader);
	if (pHitHeaderRow != NULL)
	{
		DoRowHeaderClick (pHitHeaderRow, point, rectRowHeader);
		return;
	}

	// ---------------------------
	// Set selection (first click):
	// ---------------------------
	CBCGPGridRow::ClickArea clickArea;
	CBCGPGridItemID id;
	CBCGPGridItem* pHitItem = NULL;
	CCxStructTreeRowT* pHitRow = (CCxStructTreeRowT*)HitTest (point, id, pHitItem, &clickArea);
	//保存真正选择的Item，控制操作
	m_idRealSelItem = id;
	//强制只能选择第一列，控制绘制状态
	if ( id.m_nColumn >=0 && id.m_nRow >= 0 )
	{
		id.m_nColumn = COL_NAME;
	}

	BOOL bSelChanged = id != m_idLastSel;
	BOOL bIsButtonClick = pHitItem != NULL && ((CCxStructTreeItemT*)pHitItem)->m_rectButton.PtInRect (point);

	const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;
	const BOOL bCtrl = (::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0 && !m_bIgnoreCtrlBtn;

	if (IsDragSelectionEnabled () && IsItemSelected (id) && !(bCtrl && m_bInvertSelOnCtrl))
	{
		if (StartDragItems (point))
		{
			return;
		}
	}

	DWORD dwSelMode = SM_NONE;
	if (!id.IsNull ())
	{
		dwSelMode = SM_FIRST_CLICK |
			(m_bSingleSel ? SM_SINGE_SEL_GROUP :
			(bCtrl ? SM_ADD_SEL_GROUP :
			(bShift ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP)));

		if (bCtrl && m_bInvertSelOnCtrl)
		{
			dwSelMode |= SM_INVERT_SEL;
		}

		if (pHitRow != NULL && id.IsRow ())
		{
			dwSelMode |= SM_ROW;
		}
		if (id.IsColumn ())
		{
			dwSelMode |= SM_COLUMN;
		}
	}

	m_bNoUpdateWindow = TRUE; // prevent flickering
	m_pSetSelItem = m_bVirtualMode ? NULL : pHitItem;

	//m_bTreeClicked = TRUE;
	SetCurSel (m_idRealSelItem, dwSelMode);
	//m_bTreeClicked = FALSE;

	m_pSetSelItem = NULL;
	m_bNoUpdateWindow = FALSE;

	if (id.IsNull () || pHitRow == NULL)
	{
		return;
	}

	ASSERT_VALID (pHitRow);
	//EnsureVisible (pHitRow);

	CCxStructTreeRowT* pCurSel = (CCxStructTreeRowT*)GetCurSel ();
	CBCGPGridItem* pItem = GetCurSelItem (pCurSel);
	if (id != m_idActive || pCurSel == NULL || (pItem == NULL && clickArea == CBCGPGridRow::ClickValue))
	{
		// The hitten item is not active - do not translate a click to the grid item.
		// Translate a click for single item only.

		//如果点击了加号，则展开或者折叠
		if ( clickArea == CBCGPGridRow::ClickExpandBox )
		{
			BOOL bIsExpand = pCurSel->IsExpanded();
			if ( !bIsExpand )
			{
				CCxStructTreeNode* pNode = (CCxStructTreeNode*)pCurSel->GetData();
				if ( pNode != NULL )
				{
					int nChildSize = pNode->m_vChildren.size();
					if ( nChildSize > 0 )
					{
						for ( int i = 0; i < nChildSize; i++ )
						{
							CCxStructTreeNode* pNodeTemp = pNode->m_vChildren[i];
							if ( pNodeTemp != NULL )
							{
								if ( pNodeTemp->m_pRow == NULL )
								{
									AddNode( pNodeTemp, pCurSel, -1, FALSE );
									if ( pNodeTemp->IsParamNode() )
									{
										((CCxStructTreeNodeParam*)pNodeTemp)->SetGridRowFormat();
									}
								}
								else
								{
									//该行已经存在
								}
							}
							else
							{
								ZTools::WriteZToolsLog( "表格添加子节点时，子节点内存为空" );
								return;//错误
							}
						}
					}
					else
					{
						//没有子节点，不处理
					}
				}
				else
				{
					ZTools::WriteZToolsLog( "表格展开或者折叠时，节点内存为空" );
					return;//错误
				}
			}
			pCurSel->Expand ( !bIsExpand );
			return;
		}

	}

	// ------------------------------
	// Translate a click to the item:
	// ------------------------------
	ASSERT_VALID (pCurSel); // pCurSel - hitten row

	switch (clickArea)
	{
	case CBCGPGridRow::ClickName:
		pCurSel->OnClickName (point);

		// Start selecting range of items:
		StartSelectItems ();
		break;

	case CBCGPGridRow::ClickValue:
		ASSERT_VALID (pItem);	// pItem - hitten active item
		if (pCurSel->m_bEnabled && pItem->IsEnabled ())
		{
			if (bIsButtonClick || m_bSingleSel)
			{
				//m_bTreeClicked = TRUE;
				DoClickValue (pItem, WM_LBUTTONDOWN, point, bSelChanged, bIsButtonClick);
				//m_bTreeClicked = FALSE;
			}
			else
			{
				// Defer the item click:
				SetTimer (GRID_CLICKVALUE_TIMER_ID, GRID_CLICKVALUE_TIMER_INTERVAL, NULL);
				m_bClickTimer = TRUE;
				m_ptClickOnce = point;
				m_bIsFirstClick = bSelChanged;
				m_bIsButtonClick = bIsButtonClick;
				m_bHeaderRowSelecting = FALSE;
				m_bHeaderColSelecting = FALSE;

				StartSelectItems ();
				return;
			}
		}
		break;

	default:
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);
	
	CCxStructTreeRowT * pCurRow = NULL;
	UpdateSelStatus(pCurRow);
	if ( pCurRow == NULL ) return;

	//theApp.m_pBarJianmoTree->OnGridTreeSelChanged( 1, (LPARAM)this );
}

void CCxStructTreeGridT::OnLButtonUp(UINT nFlags, CPoint point)
{
	CBCGPGridCtrl::OnLButtonUp(nFlags, point);
}

void CCxStructTreeGridT::OnRButtonDown(UINT nFlags, CPoint point)
{
	//////////////////////////////////////////////////////////////////////////

	CWnd::OnRButtonDown(nFlags, point);

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return;
	}

	SetFocus ();

	// -------------
	// Track header:
	// -------------
	if (OnTrackHeader () && point.y >= m_rectList.top)
	{
		CPoint ptHeader = point;
		ptHeader.y = m_rectHeader.top;

		if (GetColumnsInfo ().HitTestColumn (ptHeader, TRUE, STRETCH_DELTA) != -1)
		{
			return;
		}
	}

	CBCGPGridRow::ClickArea clickArea;
	CBCGPGridItemID id;
	CBCGPGridItem* pHitItem = NULL;
	CBCGPGridRow* pHitRow = HitTest (point, id, pHitItem, &clickArea);

	m_idRealSelItem = id;
	//强制只能选择第一列，控制绘制状态
	if ( id.m_nColumn >=0 && id.m_nRow >= 0 )
	{
		id.m_nColumn = COL_NAME;
	}

	BOOL bSaveSelection = pHitRow != NULL &&
		(pHitItem != NULL ? pHitItem->IsSelected () : pHitRow->IsSelected () );

	BOOL bSelChanged = id != m_idLastSel;

	const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;
	const BOOL bCtrl = (::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0 && !m_bIgnoreCtrlBtn;

	DWORD dwSelMode = SM_NONE;
	if (!id.IsNull ())
	{
		dwSelMode = SM_FIRST_CLICK |
			(m_bSingleSel ? SM_SINGE_SEL_GROUP :
			(bCtrl ? SM_ADD_SEL_GROUP :
			(bShift ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP)));

		if (bCtrl && m_bInvertSelOnCtrl)
		{
			dwSelMode |= SM_INVERT_SEL;
		}
	}

	if (!bSaveSelection)
	{
		m_pSetSelItem = m_bVirtualMode ? NULL : pHitItem;

		//m_bTreeClicked = TRUE;
		SetCurSel (m_idRealSelItem, dwSelMode);
		//m_bTreeClicked = FALSE;

		m_pSetSelItem = NULL;
	}

	if (pHitRow != NULL)
	{
		ASSERT_VALID (pHitRow);
		//EnsureVisible (pHitRow);
	}

	CCxStructTreeRowT* pCurSel = (CCxStructTreeRowT*)GetCurSel ();
	CCxStructTreeItemT* pItem = (CCxStructTreeItemT*)GetCurSelItem (pCurSel);
	if (id != m_idActive || pCurSel == NULL || pItem == NULL)
	{
		// The hitten item is not active - do not translate a click to the grid item.
		// Translate a click for single item only.
		return;
	}

	ASSERT_VALID (pCurSel); // pCurSel - hitten row
	ASSERT_VALID (pItem);	// pItem - hitten active item

	switch (clickArea)
	{
	case CBCGPGridRow::ClickExpandBox:
		break;

	case CBCGPGridRow::ClickName:
		pCurSel->OnRClickName (point);
		break;

	case CBCGPGridRow::ClickValue:
		pCurSel->OnRClickValue (point, bSelChanged);

		if (pCurSel->m_bEnabled && !bSelChanged && !bSaveSelection)
		{
			SetBeginEditReason (BeginEdit_MouseClick, bShift, bCtrl);
			if (EditItem (pCurSel, &point) && pCurSel->m_bInPlaceEdit)
			{
				if (pItem->m_rectButton.PtInRect (point))
				{
					return;
				}

				pItem->OnClickValue (WM_RBUTTONDOWN, point);
			}
		}
		break;

	default:
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//CBCGPGridCtrl::OnRButtonDown(nFlags, point);

	TrackToolTip2(FALSE);

	CCxStructTreeRowT * pCurRow = NULL;
	UpdateSelStatus(pCurRow);
	if (pCurRow == NULL) return;
	//theApp.m_pBarJianmoTree->OnGridTreeSelChanged( 1, (LPARAM)this );
//	OnItemChanged( GetCurSelItem(), pCurRow->GetRowId(), pItem->GetColumnId() );
}


void CCxStructTreeGridT::OnRButtonUp(UINT nFlags, CPoint point)
{
	//CBCGPGridItem * pItem = GetCurSelItem();
	//CBCGPGridItemID id = GetCurSelItemID();
	CRect rcHeader = GetHeaderRect();
	if (rcHeader.PtInRect(point))
	{
		ClientToScreen(&point);
		//列表头
		CMenu * pPopup = m_HeaderMenu.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		return;
	}

	//CBCGPGridRow * pRow = GetCurSel();
	CCxStructTreeNode * pData = reinterpret_cast< CCxStructTreeNode * > ( GetCurSelNodeData() );
	if ( pData != NULL )
	{
		//CCxStructTreeNode * pData = reinterpret_cast< CCxStructTreeNode * > ( pRow->GetData() );
		CBCGPGridItem *pItem = NULL;
		CBCGPGridItemID id;
		HitTest(point,id,pItem,NULL,FALSE);
		if (pItem==NULL || id.m_nColumn!= COL_NAME)
			return; // 不在名称列上，不显示菜单（PERAS-4079 ）

		if ( pData != NULL && m_idRealSelItem.m_nColumn == COL_NAME )
		{
			switch ( pData->m_NodeType )
			{
			case CCxStructTreeNode::NT_COMPONENT:
				ShowPopMenu(IDR_JIANMO_TREE_COMPONENT_POP, this);
				return;
			case CCxStructTreeNode::NT_ROOT:
				ShowPopMenu(IDR_JIANMO_TREE_PROCESS_POP, this);
				return;
			case CCxStructTreeNode::NT_FRONT_FORM:
			case CCxStructTreeNode::NT_REAR_FORM:
				ShowPopMenu(IDR_JIANMO_TREE_FORM_POP, this);
				return;
			case CCxStructTreeNode::NT_FOR:
				ShowPopMenu(IDR_JIANMO_TREE_FLOW_FOR_POP, this);
				return;
			case CCxStructTreeNode::NT_IF:
				ShowPopMenu(IDR_JIANMO_TREE_FLOW_IF_POP, this);
				return;
			case CCxStructTreeNode::NT_PARALLEL:
				ShowPopMenu(IDR_JIANMO_TREE_FLOW_PARALLEL_POP, this);
				return;
			case CCxStructTreeNode::NT_SEQUENCE:
				ShowPopMenu(IDR_JIANMO_TREE_FLOW_SEQUENCE_POP, this);
				return;
			case CCxStructTreeNode::NT_SUBFLOW:
				ShowPopMenu(IDR_JIANMO_TREE_FLOW_SUBFLOW_POP, this);
				return;
			default:
				break;
			}
		}
	}

	return;

	CBCGPGridCtrl::OnRButtonUp(nFlags, point);
}


void CCxStructTreeGridT::OnMenuJianmoTreeHeaderValue()
{
	static BOOL bShow = TRUE;
	bShow = !bShow;
	SetColumnVisible(COL_VALUE, bShow);
	AdjustLayout ();
}


void CCxStructTreeGridT::OnMenuJianmoTreeHeaderType()
{
	static BOOL bShow = FALSE;
	bShow = !bShow;
	SetColumnVisible(COL_TYPE, bShow);
	AdjustLayout ();
}


void CCxStructTreeGridT::OnMenuJianmoTreeHeaderDirection()
{
	static BOOL bShow = FALSE;
	bShow = !bShow;
	SetColumnVisible(COL_DIR, bShow);
	AdjustLayout ();
}

void CCxStructTreeGridT::OnMenuJianmoTreeHeaderUnits()
{
	static BOOL bShow = FALSE;
	bShow = !bShow;
	SetColumnVisible(COL_UNITS, bShow);
	AdjustLayout ();
}

BOOL CCxStructTreeGridT::InitTree( CRect& rc, CWnd * pParent )
{
	if ( IsWindow(m_hWnd) ) return FALSE;

	Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, pParent, (UINT)-1);

	m_StructTree.m_pGrid = this;

	EnableColumnAutoSize (FALSE);
	SetSingleSel (TRUE);
	EnableGroupByBox (FALSE);
	//SetReadOnly (FALSE);
	SetWholeRowSel (FALSE);
	EnableHeader (TRUE, 0);
//	SetSelectionBorder(FALSE);

	//m_bGridItemBorders = TRUE;
	//m_bHighlightActiveItem = FALSE;
	//m_bDrawFocusRect = FALSE;
	//m_bSelectionBorder = FALSE;

	SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//CBitmap bmp;
	//bmp.LoadBitmap (IDB_ICONS);
	//m_Images.Create (16, 16, ILC_COLOR24 | ILC_MASK, 0, 0);
	//m_Images.Add (&bmp, RGB (255, 0, 255));
	//SetImageList (&m_Images);
	LoadImage();

	CBCGPGridColors colors;

	colors.m_clrBackground = g_crStructTreeClientBg;	// Grid background color
	//colors.m_clrText;			// Grid foreground color
	//colors.m_clrHeader;		// H foreground color

	//colors.m_EvenColors;		// Even rows color
	//colors.m_OddColors;		// Odd rows color

	//colors.m_GroupColors;		// Group colors
	colors.m_GroupSelColors.m_clrBackground = g_crStructTreeSelItemBg;	// Selected group colors

	colors.m_SelColors.m_clrBackground = g_crStructTreeSelItemBg;		// Selected rows color

	colors.m_HeaderColors.m_clrBackground = g_crStructTreeHeaderBg;		// Header item color
	//colors.m_HeaderSelColors;	// Selected header item color

	colors.m_LeftOffsetColors.m_clrBackground = g_crStructTreeClientBg;	// Left offset colors

	colors.m_clrHorzLine = g_crStructTreeClientBg;		// Grid horizontal line text
	colors.m_clrVertLine = g_crStructTreeClientBg;		// Grid vertical line text

	//colors.m_clrPreviewText;	// Description text color

	SetColorTheme (colors);

	SetSelectionBorderForActiveItem(FALSE);
	SetSelectionBorder(FALSE);
	//SetSelectionBorderForActiveItem(FALSE);

	int nColCnt = sizeof(arrColName) / sizeof(LPCTSTR);
	for ( int i=0; i<nColCnt; i++ )
	{
		InsertColumn (i, arrColName[i], arrColWidth[i]);
		//单元格中的数据对齐方式
		switch (i)
		{
		case COL_NAME:
			SetColumnAlign (COL_NAME, HDF_LEFT); 
			break;
		default:
			SetColumnAlign (COL_NAME, HDF_LEFT);
			break;
		}
	}

	SetColumnVisible(COL_TYPE, FALSE);
	SetColumnVisible(COL_DIR, FALSE);
	SetColumnVisible(COL_UNITS, FALSE);

	m_ToolTip2.Create (this);
	m_ToolTip.Deactivate();
	m_ToolTip.DestroyWindow();

	m_HeaderMenu.LoadMenu(IDR_JIANMO_TREE_HEADER_POP);
	CMenu * pPopup = m_HeaderMenu.GetSubMenu(0);
	/*pPopup->CheckMenuItem(IDM_JIANMO_TREE_HEADER_VALUE, MF_CHECKED | MF_BYCOMMAND);
	pPopup->CheckMenuItem(IDM_JIANMO_TREE_HEADER_TYPE, MF_UNCHECKED | MF_BYCOMMAND);
	pPopup->CheckMenuItem(IDM_JIANMO_TREE_HEADER_DIRECTION, MF_UNCHECKED | MF_BYCOMMAND);
	pPopup->CheckMenuItem(IDM_JIANMO_TREE_HEADER_UNITS, MF_UNCHECKED | MF_BYCOMMAND);*/
	return TRUE;
}

CBCGPGridRow * CCxStructTreeGridT::CreateRow ()
{
	return new CCxStructTreeRowT (0, 0, GetRowBlockSize ());
}

CCxStructTreeRowT * CCxStructTreeGridT::CreateNewRow()
{
	CBCGPGridRow* pRow = CBCGPGridCtrl::CreateRow ( GetColumnCount () );

	for (int i = 0; i < GetColumnCount ();i++)
	{
		pRow->GetItem (i)->AllowEdit (FALSE);
	}

	return ( CCxStructTreeRowT * )pRow;
}

CCxStructTreeRowT * CCxStructTreeGridT::GetNodeRow( LPCTSTR lpszId )
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if (strId.IsEmpty()) return ( CCxStructTreeRowT * )GetRow(0);

	CBCGPGridRow * pRow = NULL;
	CCxStructTreeNode * pNode = NULL;
	for (int i=0; i<GetRowCount(); i++)
	{
		pRow = GetRow(i);
		if ( pRow == NULL ) return NULL;
		pNode = reinterpret_cast< CCxStructTreeNode * >( pRow->GetData() );
		if ( pNode == NULL ) continue;
		if (pNode->m_strID == strId) return (CCxStructTreeRowT *)pRow;
	}
	return NULL;
}



BOOL CCxStructTreeGridT::LoadImage()
{
	static const LPCTSTR arrImgName[] = {
		"root.png", "empty.png"
		//subflow
		, "if.png", "for.png", "parallel.png", "sequence.png", "subflow.png"
		, "frontform.png", "rearform.png", "group.png"
		//param
		, "param_in.png", "param_in_invalid.png"
		, "param_out.png", "param_out_invalid.png"
		, "param_inout.png", "param_inout_invalid.png"
	};

	static const LPCTSTR arrImgRobot[] = {
		//robot
		"ScriptDriver_16.png", 
		"DataParser_16.png", 
		"Excel_16.png", 
		"ToolIntegration_16.png", 
		"Word_16.png", 
		"Formula_16.png", 
		"Form_16.png", 
		"Document_16.png", 
		"Patran_16.png", 
		"Workbench_16.png", 
		"Catia_16.png", 
		"ScriptCom_16.png",
		#ifdef PROJECT_203
			_T("UGCom_16.png"),
		#endif
		"CustomRobot_16.png"
	};
	int nCx = 16;
	int nCy = 16;

	Bitmap * pBitmap = NULL;
	CString strDir = GetExecDir() + "\\Res\\StructTree\\";
	int nCount = sizeof( arrImgName ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pBitmap = GetNewBitmap( strDir + arrImgName[i] );
		if ( pBitmap == NULL )
		{
			CString strLog;
			strLog.Format( "图片资源丢失，%s", arrImgName[i] );
			MessageBox( strLog, g_lpszAppTitle );
		}
		m_vImg.push_back( pBitmap );
	}
	strDir = GetExecDir() + "\\Res\\RobotImage\\";
	nCount = sizeof( arrImgRobot ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pBitmap = GetNewBitmap( strDir + arrImgRobot[i] );
		if ( pBitmap == NULL )
		{
			CString strLog;
			strLog.Format( "图片资源丢失，%s", arrImgRobot[i] );
			MessageBox( strLog, g_lpszAppTitle );
		}
		m_vImg.push_back( pBitmap );
	}

	vector<HBITMAP> arrhbmps;
	for (int i=0; i<(int)m_vImg.size(); i++)
	{
		HBITMAP hbitmap = NULL;
		m_vImg[i]->GetHBITMAP( g_crStructTreeImgBg, &hbitmap );
		HBITMAP hbitmap1 = CBCGPDrawManager::CreateBitmap_32 (hbitmap, g_crStructTreeImgBg);
		if (hbitmap1)
		{
			AfxDeleteObject((HGDIOBJ*)&hbitmap);
			hbitmap = NULL;
			arrhbmps.push_back(hbitmap1);
		}
		else
		{
			arrhbmps.push_back(hbitmap);
		}
	}

	m_hbmp = CombineBitmap(arrhbmps);

	CBitmap bmp;
	bmp.Attach(m_hbmp);
	m_Images.Create (nCx, nCy, ILC_COLOR32 | ILC_MASK, 0, 0);
	m_Images.Add (&bmp, g_crStructTreeImgBg );
	SetImageList (&m_Images);

	return TRUE;
}

void CCxStructTreeGridT::ClearImage( void )
{
	for (int i = m_Images.GetImageCount() -1; i>=0; i--)
	{
		m_Images.Remove(i);
	}
	DeleteObject(m_hbmp); m_hbmp = NULL;
	PtrVectorClear(m_vImg);
}


/*
用一个新的节点替换一个指定的节点
*/
//void CCxStructTreeGridT::AddReplaceNode( CStructTreeReplaceData & ReplaceData )
//{
//	if (
//		ReplaceData.m_nSrcIndex == -1
//		|| ReplaceData.m_pSrcNode == NULL
//		|| ReplaceData.m_pSrcParent == NULL
//		) return ;
//
//	CCxStructTreeRowT * pOldRow = GetNodeRow( ReplaceData.m_pSrcNode->m_strID );
//	if ( pOldRow == NULL ) return;
//
//	CCxStructTreeRowT * pOldParentRow = GetNodeRow( ReplaceData.m_pSrcParent->m_strID );
//	if ( pOldParentRow == NULL ) return;
//
//	this->RemoveRow(pOldRow->GetRowId(), FALSE);
//	delete ReplaceData.m_pSrcNode;
//	RebuildIndexes();
//	AddStructTreeNode( ReplaceData.m_pDestNode );
//	RebuildIndexes();
//	UpdateParamValueBg();
//	AdjustLayout ();
//	m_bModified = TRUE;
//}

/*
用已存在的节点替换一个指定的节点
*/
//void CCxStructTreeGridT::MoveReplaceNode( CStructTreeReplaceData & ReplaceData )
//{
//	if (
//		ReplaceData.m_nSrcIndex == -1
//		|| ReplaceData.m_nDestIndex == -1
//		|| ReplaceData.m_pSrcNode == NULL
//		|| ReplaceData.m_pSrcParent == NULL
//		|| ReplaceData.m_pDestNode == NULL
//		|| ReplaceData.m_pDestParent == NULL
//		) return ;
//
//	CCxStructTreeRowT * pOldRow = GetNodeRow( ReplaceData.m_pSrcNode->m_strID );
//	if ( pOldRow == NULL ) return;
//
//	CCxStructTreeRowT * pOldParentRow = GetNodeRow( ReplaceData.m_pSrcParent->m_strID );
//	if ( pOldParentRow == NULL ) return;
//
//	CCxStructTreeRowT * pNewRow = GetNodeRow( ReplaceData.m_pDestNode->m_strID );
//	if ( pNewRow == NULL ) return;
//
//	CCxStructTreeRowT * pNewParentRow = GetNodeRow( ReplaceData.m_pDestParent->m_strID );
//	if ( pNewParentRow == NULL ) return;
//
//	if ( pOldRow->GetRowId() > pNewRow->GetRowId() )
//	{
//		this->RemoveRow(pOldRow->GetRowId(), FALSE);
//		this->RemoveRow(pNewRow->GetRowId(), FALSE);
//	}
//	else
//	{
//		this->RemoveRow(pNewRow->GetRowId(), FALSE);
//		this->RemoveRow(pOldRow->GetRowId(), FALSE);
//	}
//	RebuildIndexes();
//	AddStructTreeNode( ReplaceData.m_pSrcNode );
//	delete ReplaceData.m_pDestNode;
//	RebuildIndexes();
//	UpdateParamValueBg();
//	AdjustLayout ();
//	m_bModified = TRUE;
//}






int CCxStructTreeGridT::GetInsertRowPos( CCxStructTreeRowT * pParentRow
	, CCxStructTreeRowT * pCurRow, int nChildIndex )
{
	CCxStructTreeNode * pParentNode = reinterpret_cast< CCxStructTreeNode * > ( pParentRow->GetData() );
	if ( pParentNode == NULL ) return -1;

	if ( nChildIndex == -1 ) return -1;

	int nSubItemsCount = 0;

	for (int i=0; i<nChildIndex; i++)
	{
		CCxStructTreeRowT * pChildRow = GetNodeRow( pParentNode->m_vChildren[i]->m_strID );
		if ( pChildRow == NULL ) return -1;
		nSubItemsCount ++;
		nSubItemsCount += pChildRow->GetSubItemsCount(TRUE);
	}
	return nSubItemsCount;
}


BOOL CCxStructTreeGridT::UpdateParamValue( LPVOID lpNewValue, CCxStructTreeRowT * pCurRow )
{
	if ( lpNewValue == NULL ) return FALSE;
	if ( pCurRow == NULL ) return FALSE;

	CCxStructTreeNodeParam * pCurNode = NULL;

	pCurNode = reinterpret_cast < CCxStructTreeNodeParam * > ( pCurRow->GetData() );
	if ( pCurNode == NULL ) return FALSE;

	switch ( pCurNode->m_NodeType )
	{
	case CCxStructTreeNode::NT_PARAM:
		switch ( pCurNode->m_nDataType )
		{
		case DT_INT:
		case DT_DOUBLE:
		case DT_STRING:
		case DT_BOOLEAN:
			pCurNode->m_strValue = reinterpret_cast< LPCTSTR > ( lpNewValue );
			break;

		case DT_ARRAY1D_BOOLEAN:
		case DT_ARRAY2D_BOOLEAN:
		case DT_ARRAY3D_BOOLEAN:
		case DT_ARRAY4D_BOOLEAN:
		case DT_ARRAY1D_INT:
		case DT_ARRAY2D_INT:
		case DT_ARRAY3D_INT:
		case DT_ARRAY4D_INT:
		case DT_ARRAY1D_DOUBLE:
		case DT_ARRAY2D_DOUBLE:
		case DT_ARRAY3D_DOUBLE:
		case DT_ARRAY4D_DOUBLE:
		case DT_ARRAY1D_STRING:
		case DT_ARRAY2D_STRING:
		case DT_ARRAY3D_STRING:
		case DT_ARRAY4D_STRING:
		case DT_FILE:
			break;

		default:
			return FALSE;
		}
		break;

	//case CCxStructTreeNode::NT_PARAM_ARRAY:
	//	break;

	default:
		return FALSE;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return FALSE;

	if ( !IsWindow( pSvg->GetSafeHwnd() ) ) return FALSE;

	pSvg->JsMethod_paramUpdate( pCurNode );

	//CWebMethodData wmd;
	//CWebMethodParamData * pParam = NULL;
	////方法名称
	//wmd.m_strMethodName = "paramUpdate";
	////第1个参数
	//pParam = new CWebMethodParamData();
	//pParam->m_strName = "jsonObj";
	//pParam->m_strValue = pCurNode->GetUpdateParamJsonStr();
	//wmd.m_vParams.push_back(pParam);
	//pSvg->InvokeJsMethod( &wmd );
	m_bModified = TRUE;
	
	return TRUE;
}

void CCxStructTreeGridT::SetRowHeight ()
{
	if (m_bIsPrinting)
	{
		ASSERT_VALID (m_pPrintDC);

		// map to printer metrics
		HDC hDCFrom = ::GetDC(NULL);
		int nYMul = m_pPrintDC->GetDeviceCaps(LOGPIXELSY);	// pixels in print dc
		int nYDiv = ::GetDeviceCaps(hDCFrom, LOGPIXELSY);	// pixels in screen dc
		::ReleaseDC(NULL, hDCFrom);

		TEXTMETRIC tm;
		m_pPrintDC->GetTextMetrics (&tm);
		m_PrintParams.m_nRowHeight = tm.tmHeight + ::MulDiv (2 * TEXT_VMARGIN, nYMul, nYDiv);
		m_PrintParams.m_nLargeRowHeight = m_PrintParams.m_nRowHeight;
	}
	else
	{
		CClientDC dc (this);
		HFONT hfontOld = SetCurrFont (&dc);

		TEXTMETRIC tm;
		dc.GetTextMetrics (&tm);
		m_nRowHeight = tm.tmHeight + 2 * TEXT_VMARGIN;
		m_nLargeRowHeight = m_nRowHeight;

		::SelectObject (dc.GetSafeHdc (), hfontOld);
	}
}

//******************************************************************************************
BOOL CCxStructTreeGridT::OnDrawItem (CDC* pDC, CBCGPGridRow* pItem)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pItem);

	CCxStructTreeRowT * pRow = ( CCxStructTreeRowT * )pItem;

	CCxStructTreeNode * pNode = reinterpret_cast< CCxStructTreeNode * > ( pRow->GetData() );

	if (!pRow->m_Rect.IsRectEmpty ())
	{
		if (pRow->m_Rect.top >= m_rectClip.bottom)
		{
			return FALSE;
		}

		if (pRow->m_Rect.bottom >= m_rectClip.top)
		{
			int dx = IsSortingMode () && !IsGrouping () ? 0 : pRow->GetHierarchyLevel () * m_nRowHeight;
			
			BOOL bIsAutoGroup = pItem->IsGroup () && (pRow->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0;
			BOOL bGroupUnderline = !m_bHighlightGroups && !pRow->HasValueField ();

			COLORREF clrTextOld = (COLORREF)-1;

			BOOL bNoScrollCol0 = !pRow->IsGroup () && m_nHorzScrollOffset > 0 && GetColumnsInfo ().GetFrozenColumnCount () > 0;

			// ----------------
			// Draw left margin
			// ----------------
			CRect rectLeft = pRow->m_Rect;
			rectLeft.top++;
			if (bNoScrollCol0)
			{	// do not scroll left margin of frozen columns
				rectLeft.OffsetRect (m_nHorzScrollOffset, 0);
			}
			if (!m_bHighlightGroups || !pRow->IsGroup ())
			{
				rectLeft.right = rectLeft.left + GetExtraHierarchyOffset () + dx;
			}
			rectLeft.bottom++;

			CRgn rgnClipLeft;
			CRect rectLeftClip = rectLeft;
			rectLeftClip.left = max (rectLeftClip.left, m_rectList.left);
			rectLeftClip.bottom = min (rectLeftClip.bottom + 1, m_rectList.bottom);

			if (GetColumnsInfo ().GetColumnCount (TRUE) > 0)
			{
				int nCol0Idx = GetColumnsInfo ().OrderToIndex (0);
				if (nCol0Idx != -1)
				{
					int nCol0Width = GetColumnsInfo ().GetColumnWidth (nCol0Idx);
					nCol0Width += GetHierarchyOffset () + GetExtraHierarchyOffset ();
					if (nCol0Width > 0)
					{
						rectLeftClip.right = min (rectLeftClip.right, pRow->m_Rect.left + nCol0Width + (bNoScrollCol0? m_nHorzScrollOffset : 0));
					}
				}
			}

			if (rectLeftClip.left < rectLeftClip.right)
			{
				rgnClipLeft.CreateRectRgnIndirect (&rectLeftClip);
				pDC->SelectClipRgn (&rgnClipLeft);

				OnFillLeftOffset (pDC, rectLeft, pRow, 
					pRow->HasValueField () && !bIsAutoGroup);

				pDC->SelectClipRgn (&m_rgnClip);
			}

			if (!pRow->IsEnabled ())
			{
				clrTextOld = pDC->SetTextColor (globalData.clrGrayedText);
			}
			
			CRect rectName = pRow->m_Rect;

			if (!pRow->HasValueField () || bIsAutoGroup)
			{
				// fill group background
				CRect rectFill = rectName;
				rectFill.top++;
				rectFill.DeflateRect (dx, 0, 0, 0);

				if (!bGroupUnderline)
				{
					rectFill.DeflateRect (GetExtraHierarchyOffset (), 0, 0, 0);
				}

				if (!m_bHighlightGroups)
				{
					if (IsKindOf (RUNTIME_CLASS (CBCGPReportCtrl)) ||
						!m_ColorData.m_GroupColors.Draw (pDC, rectFill))
					{
						if (m_brGroupBackground.GetSafeHandle () != NULL ||
							m_brBackground.GetSafeHandle () != NULL)
						{
							CBrush& br = (m_brGroupBackground.GetSafeHandle () != NULL) ? 
								m_brGroupBackground : m_brBackground;
							pDC->FillRect (rectFill, &br);
						}
						else
						{
							visualManager->OnFillGridGroupBackground (this, pDC, rectFill);
						}
					}
				}

				// draw group underline
				if (bGroupUnderline && pRow->IsGroup ())
				{
					rectFill.top = rectFill.bottom;
					rectFill.InflateRect (0, 1);

					visualManager->OnDrawGridGroupUnderline (this, pDC, rectFill);
				}
			}

			DrawTreeLine(pDC, pRow, rectLeft);

			int nMaxWidth = pRow->GetItem(CCxStructTreeGridT::COL_NAME)->GetRect().Width();
			nMaxWidth += rectLeft.Width();

			// ---------------
			// draw expandbox:
			// ---------------
			if (pRow->IsGroup () && (!IsSortingMode () || IsGrouping ()) &&
				/*!pRow->m_lstSubItems.IsEmpty ())*/
				pNode->m_vChildren.size() > 0 )
			{
				CRect rectExpand = rectName;
				rectName.left += m_nRowHeight + dx;
				rectExpand.right = rectName.left;
				rectExpand.DeflateRect (dx, 0, 0, 0);

				CRgn rgnClipExpand;
				CRect rectExpandClip = rectExpand;
				rectExpandClip.bottom = min (rectExpandClip.bottom, m_rectList.bottom);

				if ( rectExpand.right < nMaxWidth )
				{
					rgnClipExpand.CreateRectRgnIndirect (&rectExpandClip);
					pDC->SelectClipRgn (&rgnClipExpand);

					pRow->OnDrawExpandBox (pDC, rectExpand);

					pDC->SelectClipRgn (&m_rgnClip);
				}
			}

			// ----------------
			// Draw row marker:
			// ----------------
			BOOL bActiveItem = (GetCurSel () == pRow);
			if (m_bRowMarker && !IsRowMarkerOnRowHeader () && bActiveItem && 
				!pRow->IsGroup() && GetExtraHierarchyOffset () > 0)
			{
				CRect rectRowMarker = rectLeft;
				rectRowMarker.left = max (
					rectRowMarker.right - GetExtraHierarchyOffset (), 
					rectRowMarker.left);

				CRgn rgnClipMarker;
				CRect rectMarkerClip = rectRowMarker;
				rectMarkerClip.left = min (rectMarkerClip.left, m_rectList.left);
				rectMarkerClip.bottom = min (rectMarkerClip.bottom, m_rectList.bottom);

				rgnClipMarker.CreateRectRgnIndirect (&rectMarkerClip);
				pDC->SelectClipRgn (&rgnClipMarker);

				pRow->OnDrawRowMarker (pDC, rectRowMarker);

				pDC->SelectClipRgn (&m_rgnClip);
			}

			// ----------
			// draw name:
			// ----------
			if (rectName.right > rectName.left)
			{
				CRgn rgnClipName;
				CRect rectNameClip = rectName;
				rectNameClip.bottom = min (rectNameClip.bottom, m_rectList.bottom);

				rgnClipName.CreateRectRgnIndirect (&rectNameClip);
				pDC->SelectClipRgn (&rgnClipName);

				HFONT hOldFont = NULL;
				if (pRow->IsGroup ())
				{
					hOldFont = (HFONT) ::SelectObject (pDC->GetSafeHdc (), m_fontBold.GetSafeHandle ());
				}

				pRow->OnDrawName (pDC, rectName);

				if (hOldFont != NULL)
				{
					::SelectObject (pDC->GetSafeHdc (), hOldFont);
				}

				pDC->SelectClipRgn (&m_rgnClip);
			}

			// ------------
			// draw values:
			// ------------
			if (pRow->HasValueField () && !bIsAutoGroup)
			{
				CRect rectItems = pRow->m_Rect;

				CRect rectValClip = rectItems;
				rectValClip.bottom = min (rectValClip.bottom, m_rectList.bottom);
				rectValClip.bottom++;

				m_rgnClipRow.CreateRectRgnIndirect (&rectValClip);
				pDC->SelectClipRgn (&m_rgnClipRow);

				pRow->OnDrawItems (pDC, rectItems);

				if (!m_bGridItemBorders)
				{
					// Draw default horizontal grid lines
					pDC->MoveTo (m_rectList.left + GetExtraHierarchyOffset () + dx - m_nHorzScrollOffset, pRow->m_Rect.bottom);
					pDC->LineTo (m_rectList.right, pRow->m_Rect.bottom);
				}

				pDC->SelectClipRgn (NULL);
				m_rgnClipRow.DeleteObject ();
			}
			else if (!m_bHighlightGroups && !bGroupUnderline)
			{
				pDC->SelectClipRgn (NULL);
				
				pDC->MoveTo (m_rectList.left + GetExtraHierarchyOffset () + dx - m_nHorzScrollOffset, pRow->m_Rect.bottom);
				pDC->LineTo (m_rectList.right, pRow->m_Rect.bottom);
			}

			pDC->SelectClipRgn (&m_rgnClip);

			if (m_bDrawFocusRect &&
				IsFocused () && IsWholeRowSel () && GetCurSel () == pRow)
			{
				CRect rect = pRow->m_Rect;
				rect.top++;
				rect.DeflateRect (dx, 0, 0, 0);
				pDC->DrawFocusRect (rect);
			}

			if (clrTextOld != (COLORREF)-1)
			{
				pDC->SetTextColor (clrTextOld);
			}
		}
	}

	return TRUE;
}

//*******************************************************************************************
CBCGPGridItem* CCxStructTreeGridT::CreateItem (int nRow, int nColumn)
{
	CCxStructTreeItemT* pItem = NULL;

	CRuntimeClass* pItemRTC = NULL;
	if (m_mapItemsRTC.Lookup (nColumn, pItemRTC))
	{
		ASSERT (pItemRTC != NULL);

		pItem = DYNAMIC_DOWNCAST (CCxStructTreeItemT, pItemRTC->CreateObject ());
	}
	else if (m_pDefaultItemRTC != NULL)
	{
		pItem = DYNAMIC_DOWNCAST (CCxStructTreeItemT, m_pDefaultItemRTC->CreateObject ());
	}
	else
	{
		pItem = new CCxStructTreeItemT ();
	}

	if (pItem == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	ASSERT_VALID (pItem);
	
	if (!pItem->OnCreate (nRow, nColumn))
	{
		delete pItem;
		return NULL;
	}

	return pItem;
}

void CCxStructTreeGridT::DrawTreeLine( CDC* pDC, CCxStructTreeRowT* pCurRow, CRect rcLeftOffset )
{
	if ( pCurRow == NULL ) return;
	if ( rcLeftOffset.IsRectEmpty() || rcLeftOffset.IsRectNull() ) return;
	CCxStructTreeNode * pCurNode = reinterpret_cast<CCxStructTreeNode*>(pCurRow->GetData());
	if ( pCurNode == NULL ) return;

	CRect rc = rcLeftOffset;
	rc.right = rc.left + rc.Height();
	int nCount = rcLeftOffset.Width() / rc.Width();
	CPoint ptCenter;

	int nMaxWidth = pCurRow->GetItem(CCxStructTreeGridT::COL_NAME)->GetRect().Width();
	nMaxWidth += rcLeftOffset.Width();

	//CPen penLine (PS_DASHDOT, 1, globalData.clrBarText);

	CBrush Brush( g_crStructTreeTreeLine );
	LOGBRUSH lBrush = { 0 };
	Brush.GetLogBrush( &lBrush );
 
	DWORD Style[] = { 0, 2 };   
	int StyleLength = 2;   

	CPen penLine( PS_GEOMETRIC | PS_USERSTYLE, 1, &lBrush, StyleLength, Style );   

	CPen* pOldPen = pDC->SelectObject (&penLine);

	CCxStructTreeNode * pTempNode = NULL;
	int nPos = -1;
	vector <CCxStructTreeNode *>::iterator itrNode;

	rc.OffsetRect(-rc.Height(), 0);
	for (int i=0; i<nCount; i++)
	{
		rc.OffsetRect(rc.Height(), 0);
		ptCenter = rc.CenterPoint ();
		ptCenter.Offset(1, -2);

		if ( i + 1 == nCount ) //最后一个区域
		{
			//绘制一条 从中心点 到 右边界 的横线
			if ( ptCenter.x < nMaxWidth && rc.right < nMaxWidth )
			{
				pDC->MoveTo (ptCenter.x, ptCenter.y);
				pDC->LineTo (rc.right, ptCenter.y);
			}
			else
			{

			}


			if ( pCurNode->m_pParent != NULL )
			{
				//绘制一条 从中心点 到 上边界 的竖线
				if ( ptCenter.x < nMaxWidth )
				{
					pDC->MoveTo (ptCenter.x, ptCenter.y);
					pDC->LineTo (ptCenter.x, rc.top - 1);
				}


				nPos = -1;
				itrNode = pCurNode->m_pParent->m_vChildren.begin();
				while ( itrNode != pCurNode->m_pParent->m_vChildren.end() )
				{
					nPos++;
					if ( pCurNode == *itrNode )
					{
						break;
					}
					++itrNode;
				}
				if ( nPos > -1 && nPos + 1 < (int)pCurNode->m_pParent->m_vChildren.size() )
				{
					//绘制一条 从中心点 到 下边界 的竖线
					if ( ptCenter.x < nMaxWidth )
					{
						pDC->MoveTo (ptCenter.x, ptCenter.y);
						pDC->LineTo (ptCenter.x, rc.bottom);
					}
				}
			}
			else
			{
				//do nothing
			}
		}
		else //不是最后一个区域，之前的区域，绘制竖线。
		{
			if ( i == 0 ) continue;

			pTempNode = pCurNode;
			for (nPos = i+1; nPos < nCount; nPos++)
			{
				if ( pTempNode->m_pParent == NULL )
				{
					pTempNode = NULL;
					break;
				}
				pTempNode = pTempNode->m_pParent;
			}
			if ( pTempNode == NULL ) continue;
			if ( pTempNode->m_pParent != NULL )
			{
				nPos = -1;
				itrNode = pTempNode->m_pParent->m_vChildren.begin();
				while ( itrNode != pTempNode->m_pParent->m_vChildren.end() )
				{
					nPos++;
					if ( pTempNode == *itrNode )
					{
						break;
					}
					++itrNode;
				}
				if ( nPos > -1 && nPos + 1 < (int)pTempNode->m_pParent->m_vChildren.size() )
				{
					//绘制一条 从上边界 到 下边界 的竖线
					if ( ptCenter.x < nMaxWidth )
					{
						pDC->MoveTo (ptCenter.x, rc.top);
						pDC->LineTo (ptCenter.x, rc.bottom);
					}
				}
			}
		}

	}

	pDC->SelectObject (pOldPen);
}
//
//
//void CCxStructTreeGridT::UpdateParamValueBg( void )
//{
//	//CCxStructTreeRowT * pRow = NULL;
//	//CCxStructTreeNode * pNode = NULL;
//
//	////将所有修改过的参数设置为无效状态，其他设置为有效
//	//for (int i=0; i<GetRowCount(); i++)
//	//{
//	//	pRow = (CCxStructTreeRowT *)GetRow(i);
//	//	if ( pRow == NULL ) continue;
//	//	pNode = reinterpret_cast<CCxStructTreeNode *>( pRow->GetData() );
//	//	if ( pNode == NULL ) continue;
//	//	if ( pNode->m_NodeType != CCxStructTreeNode::NT_PARAM ) continue;
//
//	//	if ( ((CCxStructTreeNodeParam*)pNode)->m_nDir == PARAM_DIR_OUT 
//	//		|| ((CCxStructTreeNodeParam*)pNode)->m_vDataLinkerFrom.size() > 0 )
//	//	{
//	//		//不可修改状态
//	//		pRow->GetItem(COL_VALUE)->SetTextColor( g_crStructTreeTextInvalid );
//	//		pRow->GetItem(COL_VALUE)->Enable(FALSE);
//	//	}
//	//	else
//	//	{
//	//		//可以编辑状态
//	//		pRow->GetItem(COL_VALUE)->SetTextColor( g_crStructTreeTextCanEdit );
//	//		pRow->GetItem(COL_VALUE)->Enable(TRUE);
//	//	}
//
//	//	//	pItem = pNodeRef->m_pRow->GetItem ( CCxStructTreeGridT::COL_VALUE );
//	//	//	pItem->SetTextColor(RGB(123,123,123));
//	//	//	pItem->Redraw();	
//	//}
//
//	//AdjustLayout ();
//}
//

void CCxStructTreeGridT::UpdateDataMapping( void )
{
	m_StructTree.ReloadRefMap();
	m_StructTree.UpdateGridAllRowStatus();

	//if ( CCxStructTreeNode::m_pRoot == NULL ) return;
	//CCxStructTreeNode::m_pRoot->ClearParamStatus();

	//CCxStructTreeRowT * pRow = NULL;
	//CCxStructTreeNode * pNode = NULL;

	//将所有修改过的参数设置为无效状态，其他设置为有效
	//for (int i=0; i<GetRowCount(); i++)
	//{
	//	pRow = (CCxStructTreeRowT *)GetRow(i);
	//	if ( pRow == NULL ) continue;
	//	pNode = reinterpret_cast<CCxStructTreeNode *>( pRow->GetData() );
	//	if ( pNode == NULL ) continue;
	//	pNode->OnUpdateParamStatus( CCxStructTreeNode::m_pRoot );
	//}

	//for (int i=0; i<GetRowCount(); i++)
	//{
	//	pRow = (CCxStructTreeRowT *)GetRow(i);
	//	if ( pRow == NULL ) continue;
	//	pNode = reinterpret_cast<CCxStructTreeNode *>( pRow->GetData() );
	//	if ( pNode == NULL ) continue;
	//	SetParamImage(pNode, (CCxStructTreeRowT *)pNode->m_pRow);
	//}
	//UpdateParamValueBg();
	//AdjustLayout ();

	AdjustLayout ();
}










CCxStructTreeNode * CCxStructTreeGridT::GetCurSelNodeData( void )
{
	CCxStructTreeRowT * pRow = (CCxStructTreeRowT *)GetCurSel();
	CCxStructTreeNode * pNode = NULL;
	if ( pRow == NULL )
	{
		pNode = m_StructTree.Find( m_StructTree.m_strCurSelId );
	}
	else
	{
		pNode = reinterpret_cast< CCxStructTreeNode * >(pRow->GetData());
	}
	return pNode;
}
//*****************************************************************************************
void CCxStructTreeGridT::TrackToolTip (CPoint point)
{
	return;
}

//*****************************************************************************************

void CCxStructTreeGridT::TrackToolTip2 (BOOL bShow)
{
	static CRect g_rcLast = CRect(0,0,0,0);
	static CCxStructTreeRowT* g_pLastHitRow = NULL;

	if (!bShow)
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
		return;
	}

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return;
	}

#ifndef _BCGPGRID_STANDALONE
	if (CBCGPPopupMenu::GetActiveMenu () != NULL)
	{
		return;
	}
#endif

	CPoint ptScreen;
	::GetCursorPos(&ptScreen);
	ScreenToClient (&ptScreen);

	CRect rectTT;
	m_ToolTip2.GetLastRect (rectTT);

	//if (rectTT.PtInRect (ptScreen) && m_ToolTip.IsWindowVisible ())
	//{
	//	return;
	//}

	//if ( m_bMouseHover  )
	//{
	//	Sleep(0);
	//}

	if (::GetCapture () == GetSafeHwnd ())
	{
		ReleaseCapture ();
	}

	if (!m_bShowInPlaceToolTip)
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
		return;
	}

	if (m_rectColumnChooser.PtInRect (ptScreen))
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
		return;
	}

	CBCGPGridRow::ClickArea clickArea;
	CBCGPGridItemID id;
	CBCGPGridItem* pHitItemBase = NULL;
	CCxStructTreeItemT * pHitItem = NULL;
	CCxStructTreeRowT* pHitRow = (CCxStructTreeRowT*)HitTest (ptScreen, id, pHitItemBase, &clickArea);
	pHitItem = (CCxStructTreeItemT *)pHitItemBase;

	if (pHitRow == NULL || pHitItem == NULL)
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
		return;
	}

	if ( pHitItem->m_nIdColumn != COL_NAME )
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
		return;
	}

	if ( g_pLastHitRow != NULL && pHitRow != NULL && g_pLastHitRow != pHitRow )
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
		return;
	}

	if ( !g_rcLast.IsRectEmpty() && g_rcLast == rectTT )
	{
		return;
	}

	if ( g_pLastHitRow != NULL && g_pLastHitRow == pHitRow )
	{
		return;
	}

	ASSERT_VALID (pHitRow);

	CString strTipText;
	CRect rectToolTip;
	rectToolTip.SetRectEmpty ();

	if (clickArea == CBCGPGridRow::ClickName)
	{
		strTipText = pHitRow->GetNameTooltip ();
		rectToolTip = pHitRow->GetNameTooltipRect ();
		if (ptScreen.x < rectToolTip.left)
		{
			strTipText.Empty ();
		}
	}
	else if (clickArea == CBCGPGridRow::ClickValue)
	{
		BOOL bIsAutoGroup = pHitRow->IsGroup () && (pHitRow->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0;
		if (pHitRow->IsGroup () && 
			(!pHitRow->HasValueField () || bIsAutoGroup))
		{
			strTipText = pHitRow->GetValueTooltip ();
			rectToolTip = pHitRow->GetRect ();
		}
		else
		{
			if (pHitItem != NULL)
			{
				ASSERT_VALID (pHitItem);
				if (pHitItem->IsInPlaceEditing ())
				{
					return;
				}
				if ( pHitItem->m_nIdColumn != COL_NAME )
				{
					m_ToolTip2.Deactivate ();
					g_rcLast = CRect(0,0,0,0);
					g_pLastHitRow = NULL;
					m_bTrackMouseHover = FALSE;
					m_bMouseHover = FALSE;
					return;
				}
				CRect rc = pHitItem->GetRect();
				//ClientToScreen (&rc);
				//if (ptScreen.x < rc.left || ptScreen.x > rc.left + rc.Height() 
				//	|| ptScreen.y < rc.top || ptScreen.y > rc.bottom )
				//{
				//	m_ToolTip2.Deactivate ();
				//	return;
				//}

				strTipText = pHitItem->GetValueTooltip ();
				rectToolTip = pHitItem->GetTooltipRect ();
			}
		}
	}
	
	if (!strTipText.IsEmpty ())
	{
		if (  !m_bMouseHover  ) return;

		ClientToScreen (&rectToolTip);
		
		//if (rectTT.TopLeft () == rectToolTip.TopLeft ())
		//{
		//	// Tooltip on the same place, don't show it to prevent flashing
		//	return;
		//}
		ClientToScreen (&ptScreen);
		
		ptScreen.x += g_ToolTipOffset.x;
		ptScreen.y += g_ToolTipOffset.y;
		
		rectToolTip.MoveToXY(ptScreen);
		m_ToolTip2.SetTextMargin (TEXT_MARGIN + 2);
		m_ToolTip2.SetFont (GetFont ());
		m_ToolTip2.Track (rectToolTip, strTipText);
		m_ToolTip2.m_rectLast = rectToolTip;//
		g_rcLast = rectToolTip;
		g_pLastHitRow = pHitRow;
		SetCapture ();
	}
	else
	{
		m_ToolTip2.Deactivate ();
		g_rcLast = CRect(0,0,0,0);
		g_pLastHitRow = NULL;
		m_bTrackMouseHover = FALSE;
		m_bMouseHover = FALSE;
	}
}


//*******************************************************************************************
BOOL CCxStructTreeGridT::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_RBUTTONDOWN:
		break;
	}

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return CWnd::PreTranslateMessage(pMsg);
	}

   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		if (m_pToolTip->GetSafeHwnd () != NULL)
		{
			m_pToolTip->RelayEvent(pMsg);
		}
		break;
	}

   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
		if (m_ToolTip.GetSafeHwnd () != NULL &&
			m_ToolTip.IsWindowVisible ())
		{
			m_ToolTip.Hide ();

			if (::GetCapture () == GetSafeHwnd ())
			{
				ReleaseCapture ();
			}

			return CWnd::PreTranslateMessage(pMsg);
		}
		break;
		
	case WM_MOUSEMOVE:
		if (pMsg->wParam == 0)	// No buttons pressed
		{
			//CPoint ptCursor;
			//::GetCursorPos (&ptCursor);
			//ScreenToClient (&ptCursor);

			TrackToolTip2 ();
		}
		break;
	}

	CCxStructTreeRowT* pSel = (CCxStructTreeRowT*)GetCurSel ();

	if (pMsg->message == WM_SYSKEYDOWN && 
		(pMsg->wParam == VK_DOWN || pMsg->wParam == VK_RIGHT) && 
		pSel != NULL && pSel->m_bEnabled)
	{
		CCxStructTreeItemT* pItem = (CCxStructTreeItemT*)GetCurSelItem (pSel);

		DWORD dwEditReason = BeginEdit_ComboOpen;
		if (pMsg->wParam == VK_DOWN)
		{
			dwEditReason |= BeginEdit_Down;
		}
		if (pMsg->wParam == VK_RIGHT)
		{
			dwEditReason |= BeginEdit_Right;
		}
		SetBeginEditReason (dwEditReason);

  		if (pItem != NULL && 
			((pItem->m_dwFlags) & BCGP_GRID_ITEM_HAS_LIST) &&
			EditItem (pSel))
		{
			pItem->DoClickButton (CPoint (-1, -1));

			DoInplaceEditSetSel (OnInplaceEditSetSel (pItem, dwEditReason));
		}

		return TRUE;
	}

	if (pSel != NULL && pSel->m_bInPlaceEdit && pSel->m_bEnabled)
	{
		ASSERT_VALID (pSel);

		if (pMsg->message == WM_KEYDOWN)
		{
			return OnInplaceEditKeyDown (pSel, pMsg);
		}
		else if (pMsg->message == WM_RBUTTONDOWN)
		{
			CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			CWnd* pWnd = pSel->GetInPlaceWnd ();
			if (pWnd != NULL)
			{
				ASSERT_VALID (pWnd);
				ASSERT (pWnd->GetSafeHwnd () != NULL);

				CRect rc;
				pWnd->GetClientRect (rc);
				pWnd->MapWindowPoints (this, rc);

				if (!rc.PtInRect (ptCursor))
				{
					if (!EndEditItem ())
					{
						m_bNoUpdateWindow = FALSE;
						return TRUE;
					}
				}
			}
		}
		else if (pMsg->message >= WM_MOUSEFIRST &&
				 pMsg->message <= WM_MOUSELAST)
		{
			CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			CSpinButtonCtrl* pWndSpin = pSel->GetSpinWnd ();
			if (pWndSpin != NULL)
			{
				ASSERT_VALID (pWndSpin);
				ASSERT (pWndSpin->GetSafeHwnd () != NULL);

				CRect rectSpin;
				pWndSpin->GetClientRect (rectSpin);
				pWndSpin->MapWindowPoints (this, rectSpin);

				if (rectSpin.PtInRect (ptCursor))
				{
					MapWindowPoints (pWndSpin, &ptCursor, 1); 

					pWndSpin->SendMessage (pMsg->message, pMsg->wParam, 
						MAKELPARAM (ptCursor.x, ptCursor.y));
					return TRUE;
				}
			}

			CWnd* pWndInPlaceEdit = pSel->GetInPlaceWnd ();
			if (pWndInPlaceEdit == NULL)
			{
				return CWnd::PreTranslateMessage(pMsg);
			}

			ASSERT_VALID (pWndInPlaceEdit);

			if (!pSel->m_bAllowEdit)
			{
				pWndInPlaceEdit->HideCaret ();
			}

			CRect rectEdit;
			pWndInPlaceEdit->GetClientRect (rectEdit);
			pWndInPlaceEdit->MapWindowPoints (this, rectEdit);

			if (rectEdit.PtInRect (ptCursor) &&
				pMsg->message == WM_LBUTTONDBLCLK)
			{
				if (pSel->OnDblClick (ptCursor))
				{
					return TRUE;
				}
			}

			if (rectEdit.PtInRect (ptCursor) && 
				pMsg->message == WM_RBUTTONDOWN &&
				!pSel->m_bAllowEdit)
			{
				return TRUE;
			}

			if (!rectEdit.PtInRect (ptCursor) &&
				(pMsg->message == WM_LBUTTONDOWN ||
				pMsg->message == WM_NCLBUTTONDOWN))
			{
				CCxStructTreeItemT* pItem = (CCxStructTreeItemT*)GetCurSelItem (pSel);
				if (pItem!= NULL && pItem->m_rectButton.PtInRect (ptCursor))
				{
					pItem->DoClickButton (ptCursor);
					return TRUE;
				}

				SetEndEditReason (EndEdit_AutoApply | EndEdit_Selection);
				if (!EndEditItem ())
				{
					m_bNoUpdateWindow = FALSE;
					return TRUE;
				}
			}
			else
			{
				MapWindowPoints (pWndInPlaceEdit, &ptCursor, 1); 
				pWndInPlaceEdit->SendMessage (pMsg->message, pMsg->wParam, 
					MAKELPARAM (ptCursor.x, ptCursor.y));
				return TRUE;
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}



void CCxStructTreeGridT::PublishComponent( CCxStructTreeNodeComponent * pNode )
{
	if ( pNode == NULL ) return;
	if (!theApp.m_processMgr.GetComponentStateCanSave())
		return;

	BOOL bHaveForm = FALSE;
	CString sFormPath;

	for ( int i=0; i < (int)pNode->m_vChildren.size(); i++ )
	{		
		if ( pNode->m_vChildren[i]->m_NodeType == CCxStructTreeNode::NT_FRONT_FORM
			|| pNode->m_vChildren[i]->m_NodeType == CCxStructTreeNode::NT_REAR_FORM)
		{
			bHaveForm = TRUE;
			CCxStructTreeNodeForm* pFormNode = (CCxStructTreeNodeForm*)pNode->m_vChildren[i];
			if (pFormNode == NULL)
			{
				MessageBox("取表单指针失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
				return;
			}
			sFormPath = pFormNode->m_strLocalPath;
			break;
		}
	}
	
	PublishInfo pi;
	pi.sAuthor = (char *)(LPCTSTR)pNode->m_strAuthor;
	pi.sDesc = (char *)(LPCTSTR)pNode->m_strDescription;
	pi.sName = (char *)(LPCTSTR)pNode->m_strOriName;
	pi.sTime = (char *)(LPCTSTR)pNode->m_strCreated;
	pi.sVersion = (char *)(LPCTSTR)pNode->m_strVersion;

	if (bHaveForm == FALSE)
	{
		pi.sFilePath = (char *)(LPCTSTR)pNode->m_strLocalPath;
	}
	else
	{
		CString sBotwPath;
		sBotwPath.Format("%s\\publish\\%s\\%s.botw", theApp.m_processMgr.m_sWorkPath, ZTools::GetCurrentTimeString16().c_str(), pi.sName.c_str());
		MakeSureDirectoryPathExists(sBotwPath);

		CString sCompPath = pNode->m_strLocalPath;

		CFlexRobotWithForm objRobotWithForm;
		BOOL bZip = objRobotWithForm.ZipToBotw((LPCTSTR)sBotwPath, (LPCTSTR)sCompPath, (LPCTSTR)sFormPath
			, pi.sName, pi.sVersion, pi.sAuthor, pi.sTime, pi.sDesc);
		if (bZip == FALSE || PathFileExists(sBotwPath) == FALSE)
		{
			MessageBox("压botw失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return;
		}

		pi.sFilePath = (LPCTSTR)sBotwPath;
	}

	theApp.m_pDlgResLibTree->m_tree.SendRobotTreeDB(pi);
}

void CCxStructTreeGridT::OnMenuJianmoTreeProcessEdit()
{
	CCxStructTreeNode * pNode = this->GetCurSelNodeData();
	theApp.m_processMgr.OnEditSubProcess(pNode);
}

void CCxStructTreeGridT::OnMenuJianmoTreeProcessEditFor()
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_STRUCTTREE_EDIT_FOR, NULL, NULL);
}


void CCxStructTreeGridT::OnMenuJianmoTreeProcessRun()
{
	theApp.m_processMgr.OnRunModel();
}

void CCxStructTreeGridT::OnMenuStructTreePublishProcess()
{
	//设计过程发布操作调用设计过程管理类
	theApp.m_processMgr.Publish();
}

void CCxStructTreeGridT::OnMenuStructTreeProcessDataMapping()
{
	if (  theApp.m_pMainWnd != NULL && IsWindow(  theApp.m_pMainWnd->GetSafeHwnd() ) )
	{
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_SHOW_DATAMAPPING );
	}
}

void CCxStructTreeGridT::OnMenuJianmoTreeProcessProp()
{
	if ( theApp.m_pMainWnd == NULL ) return;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;
	CCxStructTreeNode * pNode = this->GetCurSelNodeData();
	if ( pNode == NULL ) return;

	theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_UPDATE_PROP
		, reinterpret_cast<WPARAM>(pNode) );
}

void CCxStructTreeGridT::OnMenuJianmoTreeFlowEdit()
{
	CCxStructTreeNode * pNode = this->GetCurSelNodeData();
	if (pNode == NULL)
	{
		return;
	}

	if (pNode->IsFlowNode() == FALSE)
	{
		return;
	}

	theApp.m_processMgr.OnEditSubProcess(pNode);
	return;
}

void CCxStructTreeGridT::OnMenuJianmoTreeFlowEditIf()
{
	CCxStructTreeNode * pNode = this->GetCurSelNodeData();
	if (pNode == NULL)
	{
		return;
	}

	if (pNode->m_NodeType != CCxStructTreeNode::NT_IF)
	{
		return;
	}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;

	wmd.m_sMethodName = "editIfProcess";

	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = pNode->m_strID;
	wmd.m_vParams.push_back(pParam);
	BOOL b = pSvg->InvokeJsMethod( &wmd );
	return;

	CString str = "\
				  {\
				  \"NodeId\": \"f1004\",\
				  \"IfFlag\": \"false\",\
				  \"ElseFlag\": \"true\",\
				  \"Exps\": [\
				  {\
				  \"NodeName\": \"分支1\",\
				  \"Exp\": \"组件A.参数a+组件B.参数b>1\",\
				  \"Error\": \"\"\
				  },\
				  {\
				  \"NodeName\": \"分支2\",\
				  \"Exp\": \"组件A.参数a<2\",\
				  \"Error\": \"\"\
				  },\
				  {\
				  \"NodeName\": \"分支3\",\
				  \"Exp\": \"组件A.参数a=1\",\
				  \"Error\": \"\"\
				  },\
				  {\
				  \"NodeName\": \"分支4\",\
				  \"Exp\": \"组件B.参数b=2\",\
				  \"Error\": \"\"\
				  },\
				  \
				  {\
				  \"NodeName\": \"分支5\",\
				  \"Exp\": \"组件A.参数a+组件B.参数b>1\",\
				  \"Error\": \"\"\
				  },\
				  {\
				  \"NodeName\": \"分支6\",\
				  \"Exp\": \"组件A.参数a<2\",\
				  \"Error\": \"\"\
				  },\
				  {\
				  \"NodeName\": \"分支7\",\
				  \"Exp\": \"组件A.参数a=1\",\
				  \"Error\": \"\"\
				  },\
				  {\
				  \"NodeName\": \"分支8\",\
				  \"Exp\": \"组件B.参数b=2\",\
				  \"Error\": \"\"\
				  }\
				  \
				  ]\
				  }\
				  ";
	//EditIf(str);
	CDlgIf dlg(str.GetBuffer());
	dlg.DoModal();
}

void CCxStructTreeGridT::OnMenuJianmoTreeFlowRun()
{
	//CCxStructTreeNode * pNode = GetCurSelNodeData();
	//if ( pNode == NULL ) return;
	//if ( !pNode->IsFlowNode() ) return;
	//if ( !((CCxStructTreeNodeFlow*)pNode)->CanRun() )
	//{
	//	::MessageBox(GetMainWnd(), "存在未成功运行的上游节点，无法运行", g_lpszAppTitle, MB_OK|MB_ICONERROR);
	//	return;
	//}
	theApp.m_processMgr.OnRunSubProcess();
}

void CCxStructTreeGridT::OnMenuJianmoTreeFlowDelete()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "deleteObj";
	//第1个参数
	//pParam = new CWebMethodParamData();
	//pParam->m_strName = "id";
	//pParam->m_strValue = pNode->m_strID;
	//wmd.m_vParams.push_back(pParam);
	//
	pSvg->InvokeJsMethod( &wmd );
	//UpdateParamValueBg();
	AdjustLayout();
	m_bModified = TRUE;
}

void CCxStructTreeGridT::OnMenuStructTreePublishFlow()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	theApp.m_processMgr.OnPublishSubProcess(pNode);
}

void CCxStructTreeGridT::OnMenuStructTreeFlowSaveAs()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	theApp.m_processMgr.OnSaveSubProcess(pNode);
}

void CCxStructTreeGridT::OnMenuStructTreeFlowDataMapping()
{
	if (  theApp.m_pMainWnd != NULL && IsWindow(  theApp.m_pMainWnd->GetSafeHwnd() ) )
	{
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_SHOW_DATAMAPPING );
	}
}

void CCxStructTreeGridT::OnMenuJianmoTreeFlowProp()
{
	if ( theApp.m_pMainWnd == NULL ) return;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;
	CCxStructTreeNode * pNode = this->GetCurSelNodeData();
	if ( pNode == NULL ) return;

	theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_UPDATE_PROP
		, reinterpret_cast<WPARAM>(pNode) );
}

void CCxStructTreeGridT::OnMenuJianmoTreeComponentEdit()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "editComponent";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = pNode->m_strID;
	wmd.m_vParams.push_back(pParam);

	pSvg->InvokeJsMethod( &wmd );
	AdjustLayout();
	m_bModified = TRUE;
}

void CCxStructTreeGridT::OnMenuJianmoTreeComponentRun()
{
	if ( theApp.m_pMainWnd == NULL ) return;
	if ( !::IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;

	//CCxStructTreeRowT * pRow = (CCxStructTreeRowT *)GetCurSel();
	//if (pRow == NULL) return;
	//CCxStructTreeNodeComponent * pNode = reinterpret_cast< CCxStructTreeNodeComponent * >( pRow->GetData() );
	CCxStructTreeNodeComponent * pNode = reinterpret_cast< CCxStructTreeNodeComponent * >( GetCurSelNodeData() );
	if ( pNode == NULL ) return;
	theApp.m_pMainWnd->PostMessage(WM_STRUCTTREE_RUN_COMPNENT);
}

void CCxStructTreeGridT::OnMenuJianmoTreeComponentDelete()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "deleteObj";
	//第1个参数
	//pParam = new CWebMethodParamData();
	//pParam->m_strName = "id";
	//pParam->m_strValue = pNode->m_strID;
	//wmd.m_vParams.push_back(pParam);
	//
	pSvg->InvokeJsMethod( &wmd );
//	UpdateParamValueBg();
	AdjustLayout();
	m_bModified = TRUE;
}

void CCxStructTreeGridT::OnMenuStructTreePublishComponent()
{
	CCxStructTreeNodeComponent * pNode = ( CCxStructTreeNodeComponent* )GetCurSelNodeData();
	PublishComponent(pNode);
}

void CCxStructTreeGridT::OnMenuStructTreeComponentSaveAs()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "saveAs";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = pNode->m_strID;
	wmd.m_vParams.push_back(pParam);
	
	pSvg->InvokeJsMethod( &wmd );
}

void CCxStructTreeGridT::OnMenuJianmoTreeComponentDataMapping()
{
	if (  theApp.m_pMainWnd != NULL && IsWindow(  theApp.m_pMainWnd->GetSafeHwnd() ) )
	{
		theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_SHOW_DATAMAPPING );
	}
}

void CCxStructTreeGridT::OnMenuJianmoTreeComponentProp()
{
	if ( theApp.m_pMainWnd == NULL ) return;
	if ( !IsWindow( theApp.m_pMainWnd->GetSafeHwnd() ) ) return;
	CCxStructTreeNode * pNode = this->GetCurSelNodeData();
	if ( pNode == NULL ) return;

	theApp.m_pMainWnd->PostMessage( WM_STRUCTTREE_UPDATE_PROP
		, reinterpret_cast<WPARAM>(pNode) );
}

void CCxStructTreeGridT::OnMenuJianmoTreeFormEdit()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "editForm";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = pNode->m_strID;
	wmd.m_vParams.push_back(pParam);

	pSvg->InvokeJsMethod( &wmd );
	AdjustLayout();
	m_bModified = TRUE;
}


void CCxStructTreeGridT::OnMenuJianmoTreeFormDelete()
{
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "deleteObj";
	//第1个参数
	//pParam = new CWebMethodParamData();
	//pParam->m_strName = "id";
	//pParam->m_strValue = pNode->m_strID;
	//wmd.m_vParams.push_back(pParam);
	//
	pSvg->InvokeJsMethod( &wmd );
//	UpdateParamValueBg();
	AdjustLayout();
	m_bModified = TRUE;
}

//****************************************************************************************
void CCxStructTreeGridT::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	SetMouseTrack( m_bTrackMouseHover, GetSafeHwnd(), g_dwToolTipDelay );

	//if ( m_bMouseHover )
	//{
	//	m_bMouseHover = FALSE;
	//	SetMouseTrack( m_bMouseHover, GetSafeHwnd(), g_dwToolTipDelay );
	//}
	//m_bMouseHover = FALSE;

	const int nXDelta = 5;
	const int nYDelta = 5;

	if (m_nDraggedColumn >= 0)
	{
		CPoint pt = point;
		ClientToScreen (&pt);
		DragColumn (pt);
	}
	else if (m_bTracking)
	{
		TrackHeader (point.x);
	}
	else if (m_bSelecting && !m_bSingleSel)
	{
		if (abs (point.x - m_ptClickOnce.x) > nXDelta ||
			abs (point.y - m_ptClickOnce.y) > nYDelta)
		{
			KillTimer (GRID_CLICKVALUE_TIMER_ID);
			m_bClickTimer = FALSE;
			m_ptClickOnce = CPoint (0, 0);

			SetCursor (::LoadCursor (NULL, IDC_ARROW));

			SelectItems (point);
		}
	}
	else if (m_bClickDrag)
	{
		if (abs (point.x - m_ptClickOnce.x) > nXDelta ||
			abs (point.y - m_ptClickOnce.y) > nYDelta)
		{
			if (!DragItems (point))
			{
				StopDragItems ();
			}

			m_bClickDrag = FALSE;
			m_ptClickOnce = CPoint (0, 0);
		}
	}

	if (!(m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting))
	{
		//----------------------
		// Highlight header item
		//----------------------
		int nHighlightColumn = -1;
		int nHighlightColumnBtn = -1;

		CBCGPGridColumnsInfo::ClickArea clickAreaHeader;
		int nHitColumn = GetColumnsInfo ().HitTestColumn (point, FALSE, STRETCH_DELTA, &clickAreaHeader);
		if (nHitColumn != -1)
		{
			if (clickAreaHeader == CBCGPGridColumnsInfo::ClickHeader)
			{
				nHighlightColumn = nHitColumn;
			}
			else if (clickAreaHeader == CBCGPGridColumnsInfo::ClickHeaderButton)
			{
				nHighlightColumnBtn = nHitColumn;
			}
		}

		BOOL bChanged = (GetColumnsInfo ().GetHighlightColumn () != nHighlightColumn ||
			GetColumnsInfo ().GetHighlightColumnBtn () != nHighlightColumnBtn);
		if (bChanged)
		{
			GetColumnsInfo ().SetHighlightColumn (nHighlightColumn);
			GetColumnsInfo ().SetHighlightColumnBtn (nHighlightColumnBtn);

			if (m_pToolTip->GetSafeHwnd () != NULL)
			{
				m_pToolTip->Pop ();
			}
		}

		SetMouseTrack( m_bTrackMouseHover, GetSafeHwnd(), g_dwToolTipDelay );

		if (bChanged)
		{
			RedrawWindow ();
		}
	}
}

LRESULT CCxStructTreeGridT::OnMouseHover( WPARAM wp,LPARAM lp )
{
	m_bMouseHover = TRUE;

	TrackToolTip2 ();

	return 0;
}

void CCxStructTreeGridT::OnUpdateHeaderValueStatus(CCmdUI* pCmdUI) 
{
	BOOL bCheck = GetColumnVisible(COL_VALUE);
	pCmdUI->SetCheck(bCheck);
	//SwitchPopMenuCheck(m_HeaderMenu, IDM_JIANMO_TREE_HEADER_VALUE);
}

void CCxStructTreeGridT::OnUpdateHeaderTypeStatus(CCmdUI* pCmdUI) 
{
	BOOL bCheck = GetColumnVisible(COL_TYPE);
	pCmdUI->SetCheck(bCheck);
	//SwitchPopMenuCheck(m_HeaderMenu, IDM_JIANMO_TREE_HEADER_TYPE);
}

void CCxStructTreeGridT::OnUpdateHeaderDirectionStatus(CCmdUI* pCmdUI) 
{
	BOOL bCheck = GetColumnVisible(COL_DIR);
	pCmdUI->SetCheck(bCheck);
	//SwitchPopMenuCheck(m_HeaderMenu, IDM_JIANMO_TREE_HEADER_DIRECTION);
}

void CCxStructTreeGridT::OnUpdateHeaderUnitsStatus(CCmdUI* pCmdUI) 
{
	BOOL bCheck = GetColumnVisible(COL_UNITS);
	pCmdUI->SetCheck(bCheck);
	//SwitchPopMenuCheck(m_HeaderMenu, IDM_JIANMO_TREE_HEADER_UNITS);
}

//****************************************************************************************
void CCxStructTreeGridT::OnDrawHeaderItemBorder (CDC* pDC, CRect rect, int nCol)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	CBCGPGridHeaderParams params;
	params.m_nHeaderPart = CBCGPGridHeaderParams::HeaderTop;

	CRect rectInnerBorders;
	CRect rectOuterBorders;
	OnGetHeaderBorders (rectInnerBorders, rectOuterBorders, params);
	params.m_rectInnerBorders = rectInnerBorders;
	params.m_rectOuterBorders = rectOuterBorders;

	params.m_rect = rect;
	params.m_nColumn = nCol;
	params.m_nItemSelected = (IsColumnSelected (nCol)) ? CBCGPGridHeaderParams::Selected : CBCGPGridHeaderParams::NotSelected;

	if (nCol == m_nDraggedColumn && m_nDraggedColumn != -1 && !m_bDragGroupItem && !m_bDragFromChooser)
	{
		params.m_nItemState = CBCGPGridHeaderParams::Pressed;
	}
	else
	{
		if (nCol == GetColumnsInfo ().GetHighlightColumn () && nCol != -1)
		{
			params.m_nItemState = CBCGPGridHeaderParams::Hot;
		}
		else
		{
			params.m_nItemState = CBCGPGridHeaderParams::Normal;
		}
	}

	DrawHeaderPart (pDC, params);
}

//****************************************************************************************
void CCxStructTreeGridT::DrawHeaderPart (CDC* pDC, const CBCGPGridHeaderParams& params)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

//	if (!visualManager->DrawHeaderPart (pDC, params))
	{
		switch (params.m_nHeaderPart)
		{
		case CBCGPGridHeaderParams::HeaderTop:
			//if (params.m_nColumn != -1)
			{
				CBCGPGridColors::ColorData colorData = 
					params.IsSelected () && m_ColorData.m_HeaderSelColors.m_clrBackground != -1 ? 
						m_ColorData.m_HeaderSelColors :
						m_ColorData.m_HeaderColors;

				colorData.Draw (pDC, params.m_rect);
				//if (!colorData.Draw (pDC, params.m_rect))
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					GetColumnsInfo ().m_bInvertPressedColumn = 
						visualManager->OnDrawGridHeaderItemBorder (this, pDC, params.m_rect, params.IsPressed ());
				}
			}
			break;

		case CBCGPGridHeaderParams::HeaderLeft:
			{
				CBCGPGridColors::ColorData colorData = 
					params.IsSelected () && m_ColorData.m_HeaderSelColors.m_clrBackground != -1 ? 
						m_ColorData.m_HeaderSelColors :
						m_ColorData.m_HeaderColors;

				if (!colorData.Draw (pDC, params.m_rect))
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					GetColumnsInfo ().m_bInvertPressedColumn = 
						visualManager->OnDrawGridRowHeaderItemBorder (this, pDC, params.m_rect, params.IsPressed ());
				}

				if (m_bLineNumbers && params.m_pRow != NULL)
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					OnDrawLineNumber (pDC, params.m_pRow, params.m_rect, params.IsSelected (), params.IsPressed ());
				}
			}
			break;

		case CBCGPGridHeaderParams::HeaderTopLeft:
			//----------------------
			// Draw Select All area:
			//----------------------
			{
				CBCGPGridColors::ColorData colorData = 
					params.IsSelected () && m_ColorData.m_HeaderSelColors.m_clrBackground != -1 ? 
						m_ColorData.m_HeaderSelColors :
						m_ColorData.m_HeaderColors;

				if (!colorData.Draw (pDC, params.m_rect))
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					GetColumnsInfo ().m_bInvertPressedColumn = 
						visualManager->OnDrawGridSelectAllAreaBorder (this, pDC, params.m_rect, params.IsPressed ());
				}
			}
			break;
		}
	}
}

//****************************************************************************************
void CCxStructTreeGridT::DrawHeaderItem (CDC* pDC, CRect rect, CBCGPHeaderItem* pHeaderItem)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (pHeaderItem == NULL)
	{
		return;
	}

	ASSERT_VALID (pHeaderItem);

	//-------------
	// Draw border:
	//-------------
	if (pHeaderItem->m_bIsGroupBox)
	{
		visualManager->OnDrawGridGroupByBoxItemBorder (this, pDC, rect);
	}
	else
	{
		OnDrawHeaderItemBorder (pDC, rect, pHeaderItem->m_bNotPressed ? -1 : pHeaderItem->m_nColumn);
	}

	BOOL bValidColumn = TRUE;
	if (pHeaderItem->m_nColumn < 0 ||
		pHeaderItem->m_nColumn >= GetColumnsInfo ().GetColumnCount (FALSE))
	{
//		return; // last
		bValidColumn = FALSE;
	}

	if (pHeaderItem->m_nHierarchyOffset > 0 && !pHeaderItem->m_bIsGroupBox)
	{
		rect.DeflateRect (pHeaderItem->m_nHierarchyOffset, 0, 0, 0);
	}

	int nSortVal = bValidColumn ? GetColumnsInfo ().GetColumnState (pHeaderItem->m_nColumn) : 0;
	BOOL bDrawText = !pHeaderItem->m_strName.IsEmpty () &&
		(!pHeaderItem->m_bNoText || !bValidColumn || !GetColumnTextHidden (pHeaderItem->m_nColumn));

	//------------
	// Draw image:
	//------------
	if (pHeaderItem->m_iImage >= 0 && !pHeaderItem->m_bNoImage)
	{
		//---------------------------------------
		// The column has a image from imagelist:
		//---------------------------------------
		if (m_pImagesHeader != NULL)
		{			
			int cx = 0;
			int cy = 0;

			VERIFY (::ImageList_GetIconSize (*m_pImagesHeader, &cx, &cy));

			CPoint pt = rect.TopLeft ();
			pt.x ++;
			pt.y = (rect.top + rect.bottom - cy) / 2;

			int nArrow = 0; // width of the arrow or the button

			if (nSortVal != 0 && !pHeaderItem->m_bNoSortArrow)
			{
				if (rect.Width () >= GetRowHeight () + cx)
				{
					nArrow = GetRowHeight ();
				}
			}

			int nBtnWidth = bValidColumn ? GetHeaderMenuButtonRect (rect, pHeaderItem->m_nColumn).Width () : 0;
			if (nBtnWidth > 0 && !pHeaderItem->m_bNoButton)
			{
				nArrow = nBtnWidth;
			}

			if (rect.Width () > cx + nArrow)
			{
				if (!pHeaderItem->m_bIsHeaderItemDragWnd &&
					rect.Width () > cx)
				{
					if (bDrawText || pHeaderItem->m_bIsGroupBox)
					{
					}
					else if (pHeaderItem->m_nAlign & HDF_CENTER)
					{
						pt.x += (rect.Width () - nArrow - cx) / 2;
					}
					else if (pHeaderItem->m_nAlign & HDF_RIGHT)
					{
						pt.x = rect.right - nArrow - cx - 1;
					}
					
					rect.left = pt.x;
				}

				VERIFY (m_pImagesHeader->Draw (pDC, pHeaderItem->m_iImage, pt, ILD_NORMAL));

				rect.left += cx;
			}
		}
	}

	if (!pHeaderItem->m_bNoButton && bValidColumn && IsHeaderMenuButtonEnabled (pHeaderItem->m_nColumn))
	{
		CRect rectBtn = GetHeaderMenuButtonRect (rect, pHeaderItem->m_nColumn);
		OnDrawHeaderMenuButton (pDC, rectBtn, pHeaderItem->m_nColumn, !pHeaderItem->m_bNoSortArrow);
		rect.right = rectBtn.left - 1;
	}
	else if (nSortVal != 0 && !pHeaderItem->m_bNoSortArrow)
	{
		//-----------------
		// Draw sort arrow:
		//-----------------
		int nArrowHeight = GetRowHeight () - 2 * pHeaderItem->m_nArrowMargin;

		CRect rectArrow = rect;
		rectArrow.DeflateRect (pHeaderItem->m_nArrowMargin, pHeaderItem->m_nArrowMargin);
		if (rectArrow.Width () >= nArrowHeight)
		{
			rectArrow.left = rectArrow.right - nArrowHeight;
			rect.right = rectArrow.left - 1;

			int dy2 = (int) (.134 * rectArrow.Width ());
			rectArrow.DeflateRect (0, dy2);

			OnDrawSortArrow (pDC, rectArrow, nSortVal > 0);
		}
	}

	//-----------
	// Draw text:
	//-----------
	if (bDrawText)
	{
		COLORREF clrText = (IsColumnSelected (pHeaderItem->m_nColumn) && m_ColorData.m_HeaderSelColors.m_clrText != -1)
			? m_ColorData.m_HeaderSelColors.m_clrText
			: m_ColorData.m_HeaderColors.m_clrText;
		COLORREF clrTextOld = (COLORREF)-1;
		if (clrText != (COLORREF)-1 && !pHeaderItem->m_bIsGroupBox)
		{
			clrTextOld = pDC->SetTextColor (clrText);
		}

		CRect rectLabel = rect;
		rectLabel.DeflateRect (pHeaderItem->m_nTextMargin, 0);
		
		UINT uiTextFlags = DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;

		if (!pHeaderItem->m_bIsGroupBox)	// ignore align flags in groupbox
		{
			if (pHeaderItem->m_nAlign & HDF_CENTER)
			{
				uiTextFlags |= DT_CENTER;
			}
			else if (pHeaderItem->m_nAlign & HDF_RIGHT)
			{
				uiTextFlags |= DT_RIGHT;
			}
		}

		if (pHeaderItem->m_bMultiLine && !pHeaderItem->m_bIsGroupBox)
		{
			uiTextFlags |= DT_WORDBREAK;
		}
		else
		{
			uiTextFlags |= DT_SINGLELINE;
		}

		pHeaderItem->m_bNameIsTrancated = 
			DoDrawText (pDC, pHeaderItem->m_strName, rectLabel, uiTextFlags);

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor (clrTextOld);
		}
	}
}
//******************************************************************************************
void CCxStructTreeGridT::OnDraw(CDC* pDCPaint)
{
	pDCPaint->GetClipBox (m_rectClip);

	if (m_rectClip.IsRectEmpty ())
	{
		m_rectClip = m_rectList;
	}

	m_rectClip.top = max (m_rectClip.top, m_rectList.top);

	CRect rect;
	GetClientRect (rect);
	rect.right = m_rectList.right;
	rect.bottom = m_rectList.bottom + m_nGridFooterHeight;

	CRect rectGripper;
	GetClientRect (rectGripper);
	rectGripper.left = m_rectList.right;
	rectGripper.top = m_rectList.bottom;
	pDCPaint->FillRect (rectGripper, 
		m_bControlBarColors ? &globalData.brBarFace : &globalData.brBtnFace);

	if (rect.IsRectEmpty ())
	{
		return;
	}

	CBCGPMemDC memDC (*pDCPaint, rect);
	CDC* pDC = &memDC.GetDC ();

	m_clrGray = visualManager->GetGridLeftOffsetColor (this);

	CRect rectClient;
	GetClientRect (rectClient);

	CRect rectFill = rectClient;
	rectFill.top += m_rectHeader.Height () + m_nGroupByBoxHeight;
	rectFill.left += m_rectRowHeader.Width ();

	OnFillBackground (pDC, rectFill);
	OnFillRowHeaderBackground (pDC, m_rectRowHeader);

	if (m_bFilterBar)
	{
		OnFillFilterBar (pDC);
	}

	HFONT hfontOld = SetCurrFont (pDC);
	pDC->SetTextColor (GetTextColor ());
	pDC->SetBkMode (TRANSPARENT);

	CRect rectList = m_rectList;
	rectList.left = m_rectRowHeader.left;
	m_rectClip.NormalizeRect ();
	BOOL bDrawList = m_rectClip.IntersectRect (&rectList, &m_rectClip);

	m_rgnClip.CreateRectRgnIndirect (m_rectClip);
	pDC->SelectClipRgn (&m_rgnClip);

	if (bDrawList)
	{
		OnDrawList (pDC);
		OnDrawSelectionBorder (pDC);

		if (m_bDragDrop || m_bDragEnter)
		{
			OnDrawDragMarker (pDC);
			OnDrawDragFrame (pDC);
		}
	}

	pDC->SelectClipRgn (NULL);

	m_rectClip.SetRectEmpty ();
	m_rgnClip.DeleteObject ();

	OnDrawGridHeader (pDC);
	OnDrawGridFooter (pDC);

	CRect rectGroupByBox = rectClient;
	rectGroupByBox.bottom = min (rectGroupByBox.top + m_nGroupByBoxHeight, rectClient.bottom);
	OnDrawGroupByBox (pDC, rectGroupByBox);

	OnDrawSelectAllArea (pDC);
	OnDrawHeader (pDC);

	::SelectObject (pDC->GetSafeHdc (), hfontOld);

	if (m_pColumnChooser == NULL && m_bColumnChooserVisible)
	{
		ShowColumnsChooser ();
	}
}
//****************************************************************************************
void CCxStructTreeGridT::OnDrawHeader (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_rectHeader.IsRectEmpty ())
	{
		return;
	}

	OnFillHeaderBackground (pDC, m_rectHeader);

	const int nFreezeOffset = m_rectList.left + GetColumnsInfo ().GetFreezeOffset ();

	for (int i = 0; i <= GetColumnsInfo ().GetColumnCount(); i++)
	{
		if (i < GetColumnsInfo ().GetColumnCount() &&
			!GetColumnsInfo ().GetColumnVisible (i))
		{
			continue;
		}

		CRect rectColumn;
		GetColumnsInfo ().GetColumnRect (i, rectColumn);

		if (m_nHorzScrollOffset > 0)
		{
			CRect rectColumnClip = rectColumn;
			if (m_rectHeader.left > rectColumnClip.left)
			{
				rectColumnClip.left = min (rectColumnClip.right, m_rectHeader.left);
			}

			if (GetColumnsInfo ().IsFreezeColumnsEnabled () && !GetColumnsInfo ().IsColumnFrozen (i))
			{
				if (nFreezeOffset > rectColumnClip.left)
				{
					rectColumnClip.left = min (rectColumnClip.right, nFreezeOffset);
				}
			}

			CRgn rgnClipColumn;
			rgnClipColumn.CreateRectRgnIndirect (&rectColumnClip);
			pDC->SelectClipRgn (&rgnClipColumn);

			GetColumnsInfo ().DrawColumn (pDC, i, rectColumn);

			pDC->SelectClipRgn (NULL);
		}
		else
		{
			GetColumnsInfo ().DrawColumn (pDC, i, rectColumn);
		}
	}
}


//*******************************************************************************************
CRect CCxStructTreeGridT::OnGetHeaderRect (CDC* pDC, const CRect& rectDraw)
{
	CRect rect;
	if (m_bHeader)
	{
		int nYMul = 1;
		int nYDiv = 1;
		if (m_bIsPrinting && pDC != NULL)
		{
			ASSERT_VALID (pDC);

			// map to printer metrics
			HDC hDCFrom = ::GetDC(NULL);
			nYMul = pDC->GetDeviceCaps(LOGPIXELSY);			// pixels in print dc
			nYDiv = ::GetDeviceCaps(hDCFrom, LOGPIXELSY);	// pixels in screen dc
			::ReleaseDC(NULL, hDCFrom);
		}

		int nHeaderHeight = (m_bIsPrinting ? m_PrintParams.m_nRowHeight : m_nRowHeight) + ::MulDiv (4, nYMul, nYDiv);
		nHeaderHeight += 4;

		rect = rectDraw;
		rect.top += (m_bIsPrinting ? m_PrintParams.m_nGroupByBoxHeight : m_nGroupByBoxHeight);
		rect.bottom = rect.top + nHeaderHeight;

		if (m_bRowHeader)
		{
			rect.left += (m_bIsPrinting ? m_PrintParams.m_nRowHeaderWidth : m_nRowHeaderWidth);
		}
	}
	else
	{
		rect.SetRectEmpty ();
	}

	return rect;
}

//*****************************************************************************************
void CCxStructTreeGridT::RebuildIndexes (int nStartFrom)
{
	if (m_bVirtualMode)
	{
		ASSERT (FALSE);
		return;
	}

	const CList<CBCGPGridRow*, CBCGPGridRow*>& lst = 
		(!IsSortingMode () && !IsGrouping ()) ? m_lstItems : m_lstTerminalItems;

	POSITION posStartFrom = NULL;
	int nRowIndex = 0;
	if (nStartFrom >= 0 && nStartFrom < lst.GetCount ())
	{
		posStartFrom = lst.FindIndex (nStartFrom);
		nRowIndex = nStartFrom;
	}

	if (posStartFrom == NULL)
	{
		posStartFrom = lst.GetHeadPosition ();
		nRowIndex = 0;
	}

	for (POSITION pos = posStartFrom; pos != NULL; )
	{
		CCxStructTreeRowT* pRow = (CCxStructTreeRowT*)lst.GetNext (pos);
		if ( pRow == NULL )
		{
			::MessageBox(GetMainWnd(), "重建行索引失败", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return;
		}
		CCxStructTreeNode* pNode = reinterpret_cast< CCxStructTreeNode* >( pRow->GetData() );

		//if (pGridItem->IsItemVisible ())
		{
			pRow->m_nIdRow = nRowIndex;
			nRowIndex++;
		}
	}
}
//******************************************************************************************
void CCxStructTreeGridT::OnFillLeftOffset (CDC* pDC, CRect rectFill, CBCGPGridRow* /*pRow*/,
	BOOL bDrawRightBorder)
{
	ASSERT_VALID (pDC);

	//const int nRightBorder = rectFill.left + GetHierarchyOffset () + GetExtraHierarchyOffset ();

	//// special drawing if custom gradient colors
	//if (m_ColorData.m_LeftOffsetColors.m_clrBackground != (COLORREF)-1 &&
	//	m_ColorData.m_LeftOffsetColors.m_clrGradient != (COLORREF)-1 &&
	//	m_ColorData.m_LeftOffsetColors.m_nGradientAngle == 0)
	//{
	//	CRect rectGradient = rectFill;
	//	rectGradient.right = nRightBorder;
	//	m_ColorData.m_LeftOffsetColors.Draw (pDC, rectGradient, TRUE);

	//	if (rectGradient.right < rectFill.right)
	//	{
	//		CRect rectOneColor = rectFill;
	//		rectOneColor.left = rectGradient.right;
	//		CBrush br (m_ColorData.m_LeftOffsetColors.m_clrGradient);
	//		pDC->FillRect (rectOneColor, &br);
	//	}
	//}

	//else if (!m_ColorData.m_LeftOffsetColors.Draw (pDC, rectFill, TRUE))
	//{
	//	CBrush br (m_clrGray);
	//	pDC->FillRect (rectFill, &br);
	//}

	//if (bDrawRightBorder &&
	//	rectFill.right <= nRightBorder &&
	//	m_ColorData.m_LeftOffsetColors.m_clrBorder != (COLORREF)-1)
	//{
	//	CPen pen (PS_SOLID, 1, m_ColorData.m_LeftOffsetColors.m_clrBorder);
	//	CPen* pOldPen = pDC->SelectObject (&pen);

	//	pDC->MoveTo (rectFill.right - 1, rectFill.top);
	//	pDC->LineTo (rectFill.right - 1, rectFill.bottom);

	//	pDC->SelectObject (pOldPen);
	//}

	{
		CBrush br (g_crStructTreeClientBg);
		pDC->FillRect (rectFill, &br);
	}
	//CBCGPDrawManager dm(*pDC);
	//dm.DrawRect( rectFill, g_crStructTreeClientBg, g_crStructTreeClientBg );
}
//****************************************************************************************
BOOL CCxStructTreeGridT::DoDrawText (CDC* pDC, CString strText, CRect rect, UINT uiDrawTextFlags, LPRECT lpRectClip, BOOL bNoCalcExtent)
{
	ASSERT_VALID (pDC);

	if (lpRectClip != NULL)
	{
		pDC->SaveDC();
		pDC->IntersectClipRect(lpRectClip);
	}

	BOOL bTextTrancatedHorz = FALSE;
	BOOL bTextTrancatedVert = FALSE;

	TEXTMETRIC tm;
	pDC->GetTextMetrics (&tm);
	ASSERT(tm.tmHeight >= 0);

	CRect rectText = rect;
	UINT uiFlags = uiDrawTextFlags;

	if ((uiFlags & DT_SINGLELINE) == 0 && !bNoCalcExtent) // multiline
	{
		uiFlags |= DT_WORDBREAK | DT_EDITCONTROL | DT_EXTERNALLEADING;

		CRect rectTextExtent = rect;
		pDC->DrawText (strText, rectTextExtent, DT_CALCRECT | uiFlags| DT_NOPREFIX);

		int nOffset = 0;
		int nDecreseBottom = 0;
		if (rect.Height () < rectTextExtent.Height ())
		{
			// trancate partialy visible lines
			nDecreseBottom = rect.Height () % (int)tm.tmHeight;
			rectTextExtent.bottom = rect.bottom - nDecreseBottom;
			bTextTrancatedVert = TRUE;
		}

		if (rect.Height () > rectTextExtent.Height ())
		{
			if (uiFlags & DT_TOP)
			{
				// do nothing
			}
			if (uiFlags & DT_VCENTER)
			{
				nOffset = (rect.Height () - rectTextExtent.Height ()) / 2;
				nDecreseBottom = max (0, nDecreseBottom - nOffset);
			}
			else if (uiFlags & DT_BOTTOM)
			{
				nOffset = rect.Height () - rectTextExtent.Height ();
				nDecreseBottom = 0;
			}
		}

		rectText.top += nOffset;
		rectText.bottom -= nDecreseBottom;

		if (rectText.Height () / tm.tmHeight <= 1)
		{
			uiFlags = uiDrawTextFlags | DT_SINGLELINE;
			rectText = rect;
		}
	}

	if (uiFlags & DT_SINGLELINE)
	{
		strText.TrimLeft ();
		strText.Replace (_T('\n'), _T(' '));

		if (!bNoCalcExtent)
		{
			bTextTrancatedHorz = pDC->GetTextExtent (strText).cx > rectText.Width ();
		}
	}

	if (bTextTrancatedHorz)
	{
		uiFlags &= ~(DT_CENTER | DT_RIGHT);
		uiFlags |= DT_LEFT;
	}

	if (bTextTrancatedVert && (uiFlags & DT_SINGLELINE) == 0)
	{
		int nLinesTotal = rectText.Height () / (int)tm.tmHeight;
		int nLine = 1;
		for (int iEOL = 0; iEOL < strText.GetLength (); iEOL++)
		{
			iEOL = strText.Find (_T('\n'), iEOL);
			if (iEOL < 0)
			{
				break;
			}

			if (nLine++ == nLinesTotal)
			{
				strText = strText.Left (iEOL);
				if (uiDrawTextFlags & (DT_PATH_ELLIPSIS | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS))
				{
					strText += _T("...");
				}
				break;
			}
		}
	}

	uiFlags |= DT_EXPANDTABS;
	pDC->DrawText (strText, rectText, uiFlags);

	if (lpRectClip != NULL)
	{
		pDC->RestoreDC(-1);
	}

	return bTextTrancatedHorz || bTextTrancatedVert;
}


BOOL CCxStructTreeGridT::Refresh( void )
{
	m_idRealSelItem.SetNull();
	RemoveAll();
	if ( m_StructTree.GetRoot() == NULL )
	{
		AdjustLayout(); return FALSE;
	}

	AddNode( m_StructTree.GetRoot(), NULL );

//	CCxStructTreeRowT * pRow = NULL;

	//try
	//{
	//	pRow = CreateNewRow ();
	//	if ( pRow == NULL ) return FALSE;

	//	if ( !UpdateRowByNode( pRow, m_StructTree.GetRoot() ) )
	//	{
	//		return FALSE;
	//	}

	//	AddRow( pRow, FALSE );
	//}
	//catch (...)
	//{
	//	return FALSE;
	//}



	//AddStructTreeNode( CCxStructTreeNode::m_pRoot );

	//////////////////////////////////////////////////////////////////////////
	//if ( !m_bPostAdjustLayout && GetSafeHwnd() != NULL)
	//{
	//	PostMessage(BCGM_GRID_ADJUST_LAYOUT);
	//	m_bPostAdjustLayout = TRUE;
	//}

//	UpdateParamValueBg();

	return TRUE;
}

BOOL CCxStructTreeGridT::AddNode( CCxStructTreeNode * pSpecNode, CCxStructTreeRowT * pSpecParentRow
	, int nIndex, BOOL bRecursion )
{
	if ( pSpecNode == NULL )
	{
		return FALSE;
	}

	CCxStructTreeRowT * pRow = NULL;

	try
	{
		if ( pSpecParentRow == NULL )
		{
			if ( pSpecNode->m_NodeType != CCxStructTreeNode::NT_ROOT ) return FALSE;
			if ( GetRowCount() != 0 ) return FALSE;
			//添加根节点
			pRow = CreateNewRow ();
			if ( pRow == NULL ) return FALSE;
			if ( !UpdateRowByNode( pRow, pSpecNode ) )
			{
				return FALSE;
			}

			AddRow( pRow, FALSE );
		}
		else
		{
			if ( nIndex > -1 )
			{
				/*
				修改结构树后当插入非0索引的行时，如果该索引同级别的行未创建出来则同时创建出来
				*/
				CCxStructTreeNode* pNode = (CCxStructTreeNode*)pSpecParentRow->GetData();
				if ( pNode != NULL )
				{
					BOOL bExistRow = FALSE;
					int nCount = pNode->m_vChildren.size();
					for ( int j = 0; j < nCount; j++ )
					{
						CCxStructTreeNode* pNodeTemp = pNode->m_vChildren[j];
						if ( pNodeTemp == NULL )
						{
							ZTools::WriteZToolsLog( "CCxStructTreeGridT::AddNode 获取子节点内存节点数据为空 错误" );
							return FALSE;
						}
						if ( pNodeTemp->m_pRow != NULL )
						{
							bExistRow = TRUE;
							break;
						}
					}
					if ( bExistRow )//存在兄弟行
					{
						pRow = CreateNewRow ();
						if ( pRow == NULL ) return FALSE;
						if ( !UpdateRowByNode( pRow, pSpecNode ) )
						{
							return FALSE;
						}

						pSpecParentRow->AddSubItem( pRow, nIndex, FALSE );
					}
					else
					{
						for ( int i = 0; i < nCount; i++ )
						{
							CCxStructTreeNode* pNodeTemp = pNode->m_vChildren[i];
							if ( pNodeTemp == NULL )
							{
								ZTools::WriteZToolsLog( "CCxStructTreeGridT::AddNode 获取子节点内存节点数据为空 错误" );
								return FALSE;
							}
							if ( pNodeTemp->m_pRow != NULL )
							{
								continue;
							}
							pRow = NULL;
							pRow = CreateNewRow ();
							if ( pRow == NULL ) return FALSE;
							if ( !UpdateRowByNode( pRow, pNodeTemp ) )
							{
								return FALSE;
							}

							pSpecParentRow->AddSubItem( pRow, -1, FALSE );
						}
					}
				}
				else
				{
					ZTools::WriteZToolsLog( "CCxStructTreeGridT::AddNode 获取父节点内存节点数据为空 错误" );
				}
			}
			else
			{
				pRow = CreateNewRow ();
				if ( pRow == NULL ) return FALSE;
				if ( !UpdateRowByNode( pRow, pSpecNode ) )
				{
					return FALSE;
				}

				pSpecParentRow->AddSubItem( pRow, nIndex, FALSE );
				//pSpecParentRow->Expand( pSpecParentRow->m_bCustomExpandFlag );
			}
		}
	}
	catch (...)
	{
		return FALSE;
	}

	if ( bRecursion )
	{
		for (int i=0; i<(int)pSpecNode->m_vChildren.size(); i++)
		{
			if ( !AddNode( pSpecNode->m_vChildren[i], pRow, -1, bRecursion ) )
			{
				return FALSE;
			}
		}
	}
	//RebuildIndexes();

	//CCxStructTreeRowT * p =  GetNodeRow( pNode->m_strID );

	//	UpdateParamValueBg();
	return TRUE;
}

BOOL CCxStructTreeGridT::DeleteNode( CCxStructTreeNode * pSpecNode )
{
	if ( pSpecNode == NULL ) return FALSE;
	if ( !pSpecNode->IsRowMatched() ) return FALSE;

	int nRowId = pSpecNode->m_pRow->GetRowId();
	this->RemoveRow(nRowId, FALSE);
	RebuildIndexes();
	pSpecNode->ClearRowData();
	m_bModified = TRUE;
	return TRUE;
}
//
//BOOL CCxStructTreeGridT::EditNode( CCxStructTreeNode * pSpecNode )
//{
//	if ( pSpecNode == NULL ) return FALSE;
//	CCxStructTreeRowT * pRow = (CCxStructTreeRowT *)pSpecNode->m_pRow;
//	if ( pRow == NULL ) return FALSE;
//
//	CCxStructTreeRowT * pParentRow = (CCxStructTreeRowT *)pRow->GetParent();
//	pRow->
//	//if ( pNode->m_pParent != NULL )
//	//{
//	//	for (int i=0; i<(int)pNode->m_pParent->m_vChildren.size(); i++)
//	//	{
//	//		if (pNode->m_pParent->m_vChildren[i] == pNode)
//	//		{
//	//			pNode->m_nIndex = i;
//	//			break;
//	//		}
//	//	}
//	//}
//
//	//if ( pNode->m_nIndex == -1 )
//	//{
//	//	pNode->m_nIndex = pNode->GetSiblingPos();
//	//}
//
//
//	//this->RemoveRow(pRow->GetRowId(), FALSE);
//	//RebuildIndexes();
//	////@@@AddStructTreeNode( pNode );
//	//if ( pParentRow != NULL )
//	//{
//	//	RebuildIndexes();
//	//}
//	//UpdateParamValueBg();
//
//	SelectNode( pSpecNode );
//
//	//AdjustLayout ();
//	m_bModified = TRUE;
//	return TRUE;
//}
//
//BOOL CCxStructTreeGridT::MoveNode( CCxStructTreeMoveEventData & MoveData )
//{
//	if ( !MoveData.CanMove() ) return FALSE;
//
//	CCxStructTreeRowT * pSrcRow = (CCxStructTreeRowT*)MoveData.m_pSrcNode->m_pRow;
//	if ( pSrcRow == NULL ) return FALSE;	
//	CCxStructTreeRowT * pSrcParentRow = (CCxStructTreeRowT*)MoveData.m_pSrcParent->m_pRow;
//	if ( pSrcParentRow == NULL ) return FALSE;
//	CCxStructTreeRowT * pDestParentRow = (CCxStructTreeRowT*)MoveData.m_pDestParent->m_pRow;
//	if ( pDestParentRow == NULL ) return FALSE;
//	//当前行id
//	//int nSrcRowId = pSrcRow->GetRowId();
//	//if ( nSrcRowId <= -1 || nSrcRowId >= m_lstItems.GetCount () ) return FALSE;
//	//当前行pos
//	//POSITION posSrcRow = m_lstItems.FindIndex ( nSrcRowId );
//	//if ( posSrcRow == NULL ) return FALSE;
//
//	this->RemoveRow( pSrcRow->GetRowId(), FALSE );
//	RebuildIndexes();
//
//	if ( !m_StructTree.MoveNode( &MoveData ) )
//	{
//		AfxMessageBox("成功删除要移动的行，但是更新结构树数据失败");
//		return FALSE;
//	}
//
//	this->AddNode( MoveData.m_pSrcNode, pNewParentRow, -1 );
//	//	AddStructTreeNode( MoveData.m_pSrcNode );
//	RebuildIndexes();
//	
//	UpdateParamValueBg();
//	AdjustLayout ();
//	m_bModified = TRUE;
//	return TRUE;
//}
//
//BOOL CCxStructTreeGridT::SelectNode( CCxStructTreeNode * pSpecNode )
//{
////	if ( pSpecNode == NULL ) return FALSE;
////	CCxStructTreeRowT * pCurRow = ( CCxStructTreeRowT * )pSpecNode->m_pRow;
////	if ( pCurRow == NULL ) return FALSE;
////	CCxStructTreeItemT * pCurItem = ( CCxStructTreeItemT * )pCurRow->GetItem(COL_NAME);
////	if ( pCurItem == NULL ) return FALSE;
////	
//////	Clear();
//////	pCurRow->GetItem(0)->Select(TRUE);
////
////	this->EnsureVisible( pCurRow, FALSE );
////
////	CBCGPGridItemID id = pCurItem->GetGridItemID();
////
////	DWORD dwSelMode = SM_NONE;
////	if (!id.IsNull ())
////	{
////		dwSelMode = SM_FIRST_CLICK |
////			(m_bSingleSel ? SM_SINGE_SEL_GROUP : 0 );
////
////		if ( id.IsRow ())
////		{
////			dwSelMode |= SM_ROW;
////		}
////		if (id.IsColumn ())
////		{
////			dwSelMode |= SM_COLUMN;
////		}
////	}
////
////	m_bNoUpdateWindow = TRUE; // prevent flickering
////	m_pSetSelItem = m_bVirtualMode ? NULL : pCurItem;
////
////	SetCurSel (id, dwSelMode);
////
////	m_pSelRow = pCurRow;
////	AdjustLayout ();
//
//
//
//
//	CCxStructTreeRowT* pSelRow = NULL;
//	CCxStructTreeItemT* pSelItem = NULL;
//	CBCGPGridItemID id = m_idRealSelItem;
//	CPoint point;
//	CRect rc;
//
//	m_idLastSel.SetNull();
//	m_idActive.SetNull();
//
//	//强制只能选择第一列，控制绘制状态
//	if ( id.m_nColumn >=0 && id.m_nRow >= 0 )
//	{
//		id.m_nColumn = COL_NAME;
//	}
//
//	BOOL bSelChanged = id != m_idLastSel;
//
//	const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;
//	const BOOL bCtrl = (::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0 && !m_bIgnoreCtrlBtn;
//
//	pSelRow = (CCxStructTreeRowT*)GetRow( id.m_nRow );
//	if ( pSelRow != NULL )
//	{
//		pSelItem = (CCxStructTreeItemT*)pSelRow->GetItem( id.m_nColumn );
//	}
//
//	DWORD dwSelMode = SM_NONE;
//	if (!id.IsNull ())
//	{
//		dwSelMode = SM_FIRST_CLICK |
//			(m_bSingleSel ? SM_SINGE_SEL_GROUP :
//			(bCtrl ? SM_ADD_SEL_GROUP :
//			(bShift ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP)));
//
//		if (bCtrl && m_bInvertSelOnCtrl)
//		{
//			dwSelMode |= SM_INVERT_SEL;
//		}
//
//		if (pSelRow != NULL && id.IsRow ())
//		{
//			dwSelMode |= SM_ROW;
//		}
//		if (id.IsColumn ())
//		{
//			dwSelMode |= SM_COLUMN;
//		}
//	}
//
//	m_bNoUpdateWindow = TRUE; // prevent flickering
//	m_pSetSelItem = m_bVirtualMode ? NULL : pSelItem;
//
//	//m_bTreeClicked = TRUE;
//	SetCurSel (m_idRealSelItem, dwSelMode);
//	//m_bTreeClicked = FALSE;
//
//	m_pSetSelItem = NULL;
//	m_bNoUpdateWindow = FALSE;
//
//	if (id.IsNull () || pSelRow == NULL)
//	{
//		return TRUE;
//	}
//
//	ASSERT_VALID (pSelRow);
//	EnsureVisible (pSelRow, FALSE);
//
//	CCxStructTreeRowT* pCurSel = (CCxStructTreeRowT*)GetCurSel ();
//	CBCGPGridItem* pItem = GetCurSelItem (pCurSel);
//	if (id != m_idActive || pCurSel == NULL || (pItem == NULL ))
//	{
//		// The hitten item is not active - do not translate a click to the grid item.
//		// Translate a click for single item only.
//		return TRUE;
//	}
//
//	rc = pSelItem->GetRect();
//	point.x = rc.left + 2; point.y = rc.top + 2;
//	ASSERT_VALID (pItem);	// pItem - hitten active item
//	if (pCurSel->m_bEnabled && pItem->IsEnabled ())
//	{
//		if (/*bIsButtonClick || */m_bSingleSel)
//		{
//			//m_bTreeClicked = TRUE;
//			DoClickValue (pItem, WM_LBUTTONDOWN, point, bSelChanged, FALSE/*bIsButtonClick*/);
//			//m_bTreeClicked = FALSE;
//		}
//		else
//		{
//			// Defer the item click:
//			SetTimer (GRID_CLICKVALUE_TIMER_ID, GRID_CLICKVALUE_TIMER_INTERVAL, NULL);
//			m_bClickTimer = TRUE;
//			m_ptClickOnce = point;
//			m_bIsFirstClick = bSelChanged;
//			m_bIsButtonClick = FALSE/*bIsButtonClick*/;
//			m_bHeaderRowSelecting = FALSE;
//			m_bHeaderColSelecting = FALSE;
//
//			StartSelectItems ();
//			return TRUE;
//		}
//	}
//
//	CCxStructTreeRowT * pCurRow = NULL;
//	UpdateSelStatus(pCurRow);
//	if ( pCurRow == NULL ) return TRUE;
//
//	return TRUE;
//}



CCxStructTreeRowT * CCxStructTreeGridT::FindRow( LPCTSTR lpszNodeId )
{
	CString strNodeId = lpszNodeId == NULL ? "" : lpszNodeId;
	if ( strNodeId.IsEmpty() ) return NULL;
	CCxStructTreeRowT * pRow = NULL;
	CCxStructTreeNode * pNode = NULL;

	for (POSITION pos = m_lstItems.GetHeadPosition (); pos != NULL; )
	{
		pRow = (CCxStructTreeRowT *)m_lstItems.GetNext (pos);
		if ( pRow == NULL )
		{
			return NULL;
		}

		pNode = ( CCxStructTreeNode* )pRow->GetData();
		if ( pNode == NULL )
		{
			return NULL;
		}
		if ( pNode->m_strID.CompareNoCase( lpszNodeId ) == 0 )
		{
			return pRow;
		}
	}
	return NULL;
}

BOOL CCxStructTreeGridT::SwitchRow( CCxStructTreeRowT * pRow1, CCxStructTreeRowT * pRow2 )
{
	if ( pRow1 == NULL || pRow2 == NULL ) return FALSE;
	//行id
	int nRowId1 = pRow1->GetRowId();
	int nRowId2 = pRow2->GetRowId();
	if ( nRowId1 <= -1 || nRowId1 >= m_lstItems.GetCount () 
		||  nRowId2 <= -1 ||  nRowId2 >= m_lstItems.GetCount () ) return FALSE;

	//行pos
	POSITION pos1 = m_lstItems.FindIndex (nRowId1);
	POSITION pos2 = m_lstItems.FindIndex (nRowId2);
	if ( pos1 == NULL || pos2 == NULL ) return FALSE;

	CCxStructTreeRowT * pTempRow1 = (CCxStructTreeRowT *)m_lstItems.GetAt( pos1 );
	CCxStructTreeRowT * pTempRow2 = (CCxStructTreeRowT *)m_lstItems.GetAt( pos2 );
	m_lstItems.SetAt( pos1, pTempRow2 );
	m_lstItems.SetAt( pos2, pTempRow1 );

	SetRebuildTerminalItems ();
	RebuildIndexes( min( nRowId1, nRowId2 ) );
	return TRUE;
}

BOOL CCxStructTreeGridT::UpdateRowByNode( CCxStructTreeRowT * pRow, CCxStructTreeNode * pNode )
{
	if ( pRow == NULL || pNode == NULL )
	{
		return FALSE;
	}

	CCxStructTreeNodeParam * pNodePrarm = NULL;
	CCxStructTreeNodeArrayElement * pNodeArrayElement = NULL;

	try
	{
		pRow->SetData( reinterpret_cast<DWORD_PTR>( pNode ) );
		pRow->GetItem (COL_NAME)->SetValue ( (LPCTSTR)pNode->m_strText, FALSE );
		pNode->m_pRow = pRow;
		SetRowImage( pNode );
		pRow->AllowSubItems ();

		switch ( pNode->m_NodeType )
		{
		case CCxStructTreeNode::NT_PARAM:
			pNodePrarm = (CCxStructTreeNodeParam *)pNode;
			if ( pNodePrarm->IsArray() || pNodePrarm->IsFile() )
			{
				if (pNodePrarm->m_nDir == PARAM_DIR_IN || pNodePrarm->m_nDir == PARAM_DIR_INOUT)
					pRow->GetItem (COL_VALUE)->SetValue ( "<Edit...>", FALSE );
				else if (pNodePrarm->m_nDir == PARAM_DIR_OUT)
					pRow->GetItem (COL_VALUE)->SetValue ( "<View...>", FALSE );
				pRow->GetItem (COL_VALUE)->AllowEdit ( FALSE );
			}
			else if ( pNodePrarm->GetElementDataType() == DT_BOOLEAN )
			{
				pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)pNodePrarm->m_strValue, FALSE );
				pRow->GetItem (COL_VALUE)->AllowEdit ( FALSE );
			}
			else
			{
				pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)pNodePrarm->m_strValue, FALSE );
				if ( pNodePrarm->m_nDir == PARAM_DIR_IN || pNodePrarm->m_nDir == PARAM_DIR_INOUT )
					pRow->GetItem (COL_VALUE)->AllowEdit ( TRUE );
			}

			pRow->GetItem (COL_TYPE)->SetValue ( (LPCTSTR) pNodePrarm->GetDataTypeForDisplay(), FALSE );
			pRow->GetItem (COL_DIR)->SetValue ( (LPCTSTR) pNodePrarm->GetDataDirForDisplay(), FALSE );
			pRow->GetItem (COL_UNITS)->SetValue ( (LPCTSTR)pNodePrarm->m_strUnits, FALSE );
			break;

		case CCxStructTreeNode::NT_PARAM_ARRAY:
			pNodeArrayElement = (CCxStructTreeNodeArrayElement *)pNode;
			pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)pNodeArrayElement->m_strValue, FALSE );
			break;

		default:
			break;
		}
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}



void CCxStructTreeGridT::SetComponentImage( CCxStructTreeNodeComponent * pNode
	/*, CCxStructTreeRowT * pRow*/ )
{
	if ( pNode == NULL ) return;
	if ( !pNode->IsRowMatched() ) return;

	CCxStaticRes * pResImage = GetRobotImageData( pNode->m_strRobotType, 16 );
	if ( pResImage == NULL ) return;

	switch ( pResImage->m_nCode )
	{
	case CCxStaticRes::RT_SCRIPT_DRIVER:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_SCRIPT_DRIVER ); break;
	case CCxStaticRes::RT_DATA_PARSER:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_DATA_PARSER ); break;
	case CCxStaticRes::RT_EXCEL:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_EXCEL ); break;
	case CCxStaticRes::RT_TOOL_INTEGRATION:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_TOOL_INTEGRATION ); break;
	case CCxStaticRes::RT_WORD:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_WORD ); break;
	case CCxStaticRes::RT_FORMULA:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_FORMULA ); break;
	case CCxStaticRes::RT_FORM:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_FORM ); break;
	case CCxStaticRes::RT_DOCUMENT:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_DOCUMENT ); break;
	case CCxStaticRes::RT_PATRAN:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PATRAN ); break;
	case CCxStaticRes::RT_WORKBENCH:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_WORKBENCH ); break;
	case CCxStaticRes::RT_CATIA:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_CATIA ); break;
	case CCxStaticRes::RT_SCRIPT:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_SCRIPT ); break;
#ifdef PROJECT_203
	case CCxStaticRes::RT_UG:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_UG ); break;
#endif
	default:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_CUSTOM ); break;
	}
}

void CCxStructTreeGridT::SetParamImage( CCxStructTreeNodeParam * pNode
	/*, CCxStructTreeRowT * pRow*/ )
{
	if ( pNode == NULL ) return;
	if ( !pNode->IsRowMatched() ) return;

	switch ( pNode->m_nDir )
	{
	case PARAM_DIR_IN:
		if ( 
			( pNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID ) == CCxStructTreeNode::NS_NAME_INVALID
//			|| pNode->m_bModified 
			)
			pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_IN_INVALID/*, FALSE*/ );
		else
			pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_IN/*, FALSE*/ );
		break;
	case PARAM_DIR_OUT:
		if ( 
			( pNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID ) == CCxStructTreeNode::NS_NAME_INVALID
//			|| pNode->m_bModified 
			)
			pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_OUT_INVALID/*, FALSE*/ );
		else
			pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_OUT/*, FALSE*/ );
		break;
	case PARAM_DIR_INOUT:
		if ( 
			( pNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID ) == CCxStructTreeNode::NS_NAME_INVALID
//			|| pNode->m_bModified 
			)
			pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_INOUT_INVALID/*, FALSE*/ );
		else
			pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_INOUT/*, FALSE*/ );
		break;
	default:
		break;
	}
}

void CCxStructTreeGridT::SetRowImage( CCxStructTreeNode * pNode /*, CCxStructTreeRowT * pRow*/ )
{
	if ( pNode == NULL ) return;
	if ( !pNode->IsRowMatched() ) return;

	switch ( pNode->m_NodeType )
	{
	case CCxStructTreeNode::NT_ROOT:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_ROOT, FALSE );
		break;
	case CCxStructTreeNode::NT_PARALLEL:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_PARALLEL, FALSE );
		break;
	case CCxStructTreeNode::NT_SEQUENCE:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_SEQUENCE, FALSE );
		break;
	case CCxStructTreeNode::NT_SUBFLOW:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_SUBFLOW, FALSE );
		break;
	case CCxStructTreeNode::NT_IF:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_IF, FALSE );
		break;
	case CCxStructTreeNode::NT_FOR:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_FOR, FALSE );
		break;
	case CCxStructTreeNode::NT_FRONT_FORM:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_FRONTFORM, FALSE );
		break;
	case CCxStructTreeNode::NT_REAR_FORM:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_REARFORM, FALSE );
		break;
	case CCxStructTreeNode::NT_GROUP:
		pNode->m_pRow->GetItem (COL_NAME)->SetImage ( IMG_GROUP, FALSE );
		break;
	case CCxStructTreeNode::NT_COMPONENT:
		SetComponentImage( (CCxStructTreeNodeComponent*)pNode );
		break;
	case CCxStructTreeNode::NT_PARAM:
		SetParamImage( (CCxStructTreeNodeParam*)pNode );
		break;
	default:
		return;
	}
}


//*******************************************************************************************
BOOL CCxStructTreeGridT::SetCurSel (CBCGPGridItemID idItem,
	DWORD dwSelMode, BOOL bRedraw)
{
	ASSERT_VALID (this);

	CCxStructTreeRowT* pSel = ( CCxStructTreeRowT*)GetCurSel ();
	CCxStructTreeRowT* pOldSelectedItem = pSel;

	if ( m_idLastSel == idItem &&
		 !(dwSelMode == SM_NONE) &&
		 !( ((SM_ALL | SM_COLUMN | SM_ROW | SM_SINGLE_ITEM | 
			  SM_FIRST_CLICK | SM_SECOND_CLICK | SM_INVERT_SEL) & dwSelMode) != 0) )
	{
		return FALSE;
	}

	//设置每行不论点击那个item，都只选择首列
	//CBCGPGridItemID idbak = idItem;
	//if ( idItem.m_nColumn > 0 && idItem.m_nRow >= 0 )
	//{
	//	idItem.m_nColumn = 0;
	//}
	//////////////////////////////////////////////////////////////////////////


	if (pSel != NULL && pSel->m_bInPlaceEdit)
	{
		SetEndEditReason (EndEdit_Cancel | EndEdit_Selection);
		EndEditItem (FALSE);
	}

	BOOL	bExcelSel = TRUE;			// Excell behavior

	const int nFirstColumn = GetColumnsInfo ().GetFirstVisibleColumn ();
	const int nFirstRow = 0;
	const int nLastColumn = GetColumnsInfo ().GetLastVisibleColumn ();
	const int nLastRow = GetTotalItems () - 1;

	ASSERT (nLastColumn <= GetColumnsInfo ().GetColumnCount () - 1);

	if (nLastColumn < 0 || nLastRow < 0)
	{
		dwSelMode = SM_NONE;
	}

	// -----------------
	// Change selection:
	// -----------------
	if ((dwSelMode & SM_ALL) != 0 || idItem.IsAll ())
	{
		idItem.SetAll ();

		// Store previous active item, select all items
		if (m_idActive.IsNull ())
		{
			m_idActive = idItem;
		}
		m_idLastSel = idItem;

		while (!m_lstSel.IsEmpty ())
		{
			delete m_lstSel.RemoveTail ();
		}

		CBCGPGridRange* pSelRange = 
			new CBCGPGridRange (nFirstColumn, nFirstRow, nLastColumn, nLastRow);
		m_lstSel.AddTail (pSelRange);
	}
	else if ((SM_NONE == dwSelMode) ||
		idItem.IsNull () || 
		nLastColumn < 0 || nLastRow < 0)
	{
		// Remove selection
		m_idActive.SetNull ();
		m_idLastSel.SetNull ();

		while (!m_lstSel.IsEmpty ())
		{
			CBCGPGridRange* pRange = m_lstSel.RemoveTail ();
			ASSERT (pRange != NULL);

			SelectRange (*pRange, FALSE, bRedraw);
			delete pRange;
		}
	}
	else if ((dwSelMode & SM_SET_ACTIVE_ITEM) != 0)
	{
		// idItem should be within selection only
		// Store selection, change active item
		if (IsItemSelected (idItem))
		{
			m_idActive = idItem;
		}
	}

	else if ((dwSelMode & SM_SINGLE_ITEM) != 0 && !bExcelSel)
	{
	}
	else if ((dwSelMode & SM_FIRST_CLICK) != 0 ||
			(dwSelMode & SM_SINGLE_ITEM) != 0 ||
			(dwSelMode & SM_INVERT_SEL) != 0)
	{
		// Regarding nSelMode, modify selection or set new selection.
		// If not INVERT_SEL, the active item is always selected.

		if ((dwSelMode & SM_INVERT_SEL) != 0 && IsItemSelected (idItem))
		{
			DoInvertSelection (idItem, dwSelMode, bRedraw, 
							nFirstColumn, nFirstRow, nLastColumn, nLastRow);
		}
		else if ((dwSelMode & SM_SINGE_SEL_GROUP) != 0)
		{
			// Remove selection, set active item, add selected block
			while (!m_lstSel.IsEmpty ())
			{
				CBCGPGridRange* pRange = m_lstSel.RemoveTail ();
				ASSERT (pRange != NULL);

				SelectRange (*pRange, FALSE, bRedraw);
				delete pRange;
			}

            BOOL bAddSelGroup = TRUE;
			CBCGPGridItemID idbak = idItem;
			DWORD dwSelModeBak = dwSelMode;
			if (/*idItem.m_nColumn >=0 &&*/ idItem.m_nRow >= 0)
			{
				idItem.m_nColumn = 0;
				dwSelMode &= ~SM_ROW;
			}
            DoSetSelection (idItem, dwSelMode, bAddSelGroup, bRedraw, 
							nFirstColumn, nFirstRow, nLastColumn, nLastRow);
			idItem = idbak;
			dwSelMode = dwSelModeBak;
		}
		else if ((dwSelMode & SM_CONTINUE_SEL_GROUP) != 0)
		{
			// Store previous active item, modify selected block
            BOOL bAddSelGroup = FALSE;
            DoSetSelection (idItem, dwSelMode, bAddSelGroup, bRedraw,
							nFirstColumn, nFirstRow, nLastColumn, nLastRow);
		}
		else if ((dwSelMode & SM_ADD_SEL_GROUP) != 0)
		{
			// Store selection, set new active item, add new selected block
            BOOL bAddSelGroup = TRUE;
            DoSetSelection (idItem, dwSelMode, bAddSelGroup, bRedraw,
							nFirstColumn, nFirstRow, nLastColumn, nLastRow);
		}
		else
		{
			return FALSE;
		}
	}
	else if ((dwSelMode & SM_SECOND_CLICK) != 0)
	{
		// Store previous active item, modify selected block
        BOOL bAddSelGroup = FALSE;
        DoSetSelection (idItem, dwSelMode, bAddSelGroup, bRedraw,
						nFirstColumn, nFirstRow, nLastColumn, nLastRow);
	}
	else
	{
		return FALSE;
	}

	if (m_bWholeRowSel && !m_lstSel.IsEmpty ())
	{
		// Inflate last selected block to the whole row
		CBCGPGridRange* pRange = (CBCGPGridRange*) m_lstSel.GetTail ();
		ASSERT (pRange != NULL);

		pRange->m_nLeft = nFirstColumn;
		pRange->m_nRight = nLastColumn;
	}

	CCxStructTreeItemT* pPrevItem = (CCxStructTreeItemT*)GetCurSelItem ();
	if (m_bVirtualMode)
	{
		m_pSelRow = NULL;
		m_pSelItem = NULL;

		m_pLastSelRow = NULL;
		m_pLastSelItem = NULL;
	}
	else
	{
		m_pSelRow = (m_idActive.m_nRow != -1) ? GetRow (m_idActive.m_nRow) : NULL;
		m_pSelItem = (m_pSelRow != NULL) ? m_pSelRow->GetItem (m_idActive.m_nColumn) : NULL;

		m_pLastSelRow = (m_idLastSel.m_nRow != -1) ? GetRow (m_idLastSel.m_nRow) : NULL;
		m_pLastSelItem = (m_pLastSelRow != NULL) ? m_pLastSelRow->GetItem (m_idLastSel.m_nColumn) : NULL;
	}

	CRect rectTrackSelOld = m_rectTrackSel;
	m_rectTrackSel = OnGetSelectionRect ();
	if (bRedraw)
	{
		InvalidateRect (rectTrackSelOld);
		InvalidateRect (m_rectTrackSel);
	}

	// -------------------------
	// Notify control and items:
	// -------------------------
	if (m_bWholeRowSel && m_bSingleSel)
	{
		CCxStructTreeRowT* pSel = (CCxStructTreeRowT*)GetCurSel ();
		OnChangeSelection (pSel, pOldSelectedItem);

		if (pOldSelectedItem != NULL)
		{
			ASSERT_VALID (pOldSelectedItem);

			pOldSelectedItem->OnKillSelection (pSel);

			if (bRedraw)
			{
				CRect rectOld = pOldSelectedItem->m_Rect;
				InvalidateRect (rectOld);
			}
		}

		if (pSel != NULL)
		{
			ASSERT_VALID (pSel);

			pSel->OnSetSelection (pOldSelectedItem);

			if (bRedraw)
			{
				CRect rect = pSel->m_Rect;
				InvalidateRect (rect);
			}
		}
	}

	for (POSITION pos = m_lstSel.GetHeadPosition (); pos != NULL; )
	{
		CBCGPGridRange* pRange = (CBCGPGridRange*) m_lstSel.GetNext (pos);
		ASSERT (pRange != NULL);

		SelectRange (*pRange, TRUE, bRedraw);
	}

	if (pPrevItem != NULL)
	{
		CRect rectButton = pPrevItem->m_rectButton;
		pPrevItem->m_rectButton.SetRectEmpty ();

		InvalidateRect (rectButton);
	}

	CCxStructTreeItemT* pCurItem = (CCxStructTreeItemT*)GetCurSelItem ();
	if (pCurItem != NULL && pCurItem->HasButton ())
	{
		pCurItem->AdjustButtonRect ();

		InvalidateRect (pCurItem->m_rectButton);
	}

	if (m_bRowHeader)
	{
		InvalidateRect (m_rectSelectAllArea);
	}
	
	// ---------------
	// Update control:
	// ---------------
	if (bRedraw && !m_bNoUpdateWindow)
	{
		UpdateWindow ();
	}

/*
	//ListCtrl behavior
		// arrows
		// Remove selection, set active item, add selected block
		// Ctrl + arrows
		// Change current item. Active item can be not selected.
		// Ctrl + Space
		// Invert selection for the active item.
		// Ctrl + Shift + arrows
		// Invert selection for range from active item to current item.

	//Excell behavior
	// change selection:
	switch (nSelMode)
	case SM_INVERT_SEL:		// non-Excell behavior
		// Add items to m_arrSel at first time.
		// If items are already selected then remove them from m_arrSel .

	case SM_SINGE_SEL_GROUP:// click, arrows
		// Remove selection, set active item, add selected block
	case SM_CONTINUE_SEL_GROUP:// Shift + click, Shift + arrows
		// Store previous active item, modify selected block
	case SM_ADD_SEL_GROUP:	// Ctrl + click
		// Store selection, set new active item, add new selected block

	case SM_SET_ACTIVE_ITEM:// Tab, Enter
		// Should be within selection only
		// Store selection, change active item

	case SM_NONE:
		// Remove selection
	case SM_ALL:			// Ctrl + A
		// Store previous active item, select all items
	case SM_SINGLE_ROW:		// Shift + Space
		// Store previous active item, inflate selected block to the whole row
	case SM_SINGLE_COLUMN:	// Ctrl + Space
		// Store previous active item, inflate selected block to the whole column

	case SM_SINGLE_ITEM:	// arrows
	case SM_FIRST_CLICK:
		// Regarding nSelMode, set active item or set new selection.
		// If not INVERT_SEL, the active item is always selected.
	case SM_SECOND_CLICK:
		// Store previous active item, modify selected block
		m_arrSel.Add (pHitItem);
*/
	if (GetSafeHwnd () != NULL)
	{
		if ( idItem.m_nRow >= 0 )
			//pCurItem->m_nIdColumn = idbak.m_nColumn;
		m_StructTree.OnNodeSelected(  );
		CWnd* pOwnerWnd = GetOwner ();
		if (pOwnerWnd != NULL)
		{
			pOwnerWnd->SendMessage (BCGM_GRID_SEL_CHANGED, 0, (LPARAM) this);
		}
	}

	return TRUE;
}

//******************************************************************************************
void CCxStructTreeGridT::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE)
	{
		if (m_nDraggedColumn >= 0)
		{
			// Cancel dragging a column:
			CPoint pt(-1, -1);
			StopDragColumn (pt, FALSE);
		}
		if (m_bTracking)
		{
			// Cancel resizing a column:
			TrackHeader (-1);
			m_bTracking = FALSE;

			if (::GetCapture () == GetSafeHwnd ())
			{
				ReleaseCapture ();
			}
		}
	}

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	
	if (m_lstItems.GetCount() == 0 && !m_bVirtualMode)
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	CCxStructTreeRowT* pSel = (CCxStructTreeRowT*)GetCurSel ();

	switch (nChar)
	{
	case VK_F2:
		if (pSel != NULL && pSel->m_bEnabled)
		{
			EnsureVisible (pSel);
			SetBeginEditReason (BeginEdit_F2);
			if (EditItem (pSel))
			{
				DoInplaceEditSetSel (OnInplaceEditSetSel (GetCurSelItem (pSel), BeginEdit_F2));
			}
		}
		break;

	case VK_F4:
		if (pSel != NULL && pSel->m_bEnabled)
		{
			EnsureVisible (pSel);
			SetBeginEditReason (BeginEdit_F4);
			if (EditItem (pSel))
			{
				CCxStructTreeItemT* pItem = (CCxStructTreeItemT*)GetCurSelItem (pSel);
				if (pItem != NULL && 
					((pItem->m_dwFlags) & (BCGP_GRID_ITEM_HAS_BUTTON | BCGP_GRID_ITEM_HAS_LIST)))
				{
					pItem->DoClickButton (CPoint (-1, -1));
				}

				DoInplaceEditSetSel (OnInplaceEditSetSel (pItem, BeginEdit_F4 | BeginEdit_ComboOpen));
			}
			return;
		}
		break;

	case VK_SPACE:
		if (!m_bWholeRowSel && !m_bSingleSel)
		{
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;
			const BOOL bCtrl = (::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0 && !m_bIgnoreCtrlBtn;

			if (bShift && bCtrl) 
			{
				if (!m_idActive.IsNull ())
				{
					SetCurSel (m_idActive, SM_ALL);
				}
				return;
			}
			else if (bShift)
			{
				if (!m_idLastSel.IsNull ())
				{
					SetCurSel (m_idLastSel, SM_SINGLE_ITEM | SM_CONTINUE_SEL_GROUP | SM_ROW);
				}
				return;
			}
			else if (bCtrl)
			{
				if (!m_idLastSel.IsNull ())
				{
					SetCurSel (m_idLastSel, SM_SINGLE_ITEM | SM_CONTINUE_SEL_GROUP | SM_COLUMN);
				}
				return;
			}
		}
		break;

	case 0x41: // Ctrl+ A
		if (!m_bSingleSel && (::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0)
		{
			SetCurSel (m_idActive, SM_ALL);
			return;
		}
		break;

	case VK_LEFT:
		if ((::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0 && !m_bIgnoreCtrlBtn)
		{
			return;
		}
		else if (pSel != NULL && pSel->IsGroup () && pSel->IsExpanded () && 
			(!IsSortingMode () || (pSel->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0) &&
			!pSel->m_lstSubItems.IsEmpty ())
		{
			pSel->Expand (FALSE);
			return;
		}
		else if (!m_bWholeRowSel)
		{
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			// Go left to the nearest item:
			CCxStructTreeRowT* pRowCurSel = (bShift && !m_bSingleSel) ? (CCxStructTreeRowT*)m_pLastSelRow : (CCxStructTreeRowT*)m_pSelRow;
			CCxStructTreeItemT* pItemCurSel = (bShift && !m_bSingleSel) ? (CCxStructTreeItemT*)m_pLastSelItem : (CCxStructTreeItemT*)m_pSelItem;
			if (m_bVirtualMode)
			{
				CBCGPGridItemID idCurSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
				pRowCurSel = (CCxStructTreeRowT*)GetVirtualRow (idCurSel.m_nRow);
				pItemCurSel = (pRowCurSel != NULL) ? (CCxStructTreeItemT*)pRowCurSel->GetItem (idCurSel.m_nColumn) : NULL;
			}

			CBCGPGridItemID id;
			CBCGPGridItem* pItem = NULL;

			if (pItemCurSel != NULL)
			{
				if (m_bVirtualMode)
				{
					id = pItemCurSel->GetGridItemID ();
					//id.m_nColumn --;
					int nPos = GetColumnsInfo ().IndexToOrder (id.m_nColumn);

					// Get position order of the merged cell
					if (pItemCurSel->GetMergedCells () != NULL)
					{
						CBCGPGridRange range;
						if (pItemCurSel->GetMergedRange (range))
						{
							int nPosLeft = GetColumnsInfo ().IndexToOrder (range.m_nLeft);
							int nPosRight = GetColumnsInfo ().IndexToOrder (range.m_nRight);
							if (nPosLeft != -1 && nPosRight != -1)
							{
								nPos = max (nPosLeft, nPosRight);
							}
							else
							{
								nPos = -1;
							}
						}
					}

					if (nPos > 0)
					{
						id.m_nColumn = GetColumnsInfo ().OrderToIndex (--nPos);
					}
					else
					{
						id.m_nColumn = -1;
					}
				}
				else
				{
					//CRect rect = pItemCurSel->GetRect ();
					//if (pItemCurSel->GetMergedCells () != NULL)
					//{
					//	rect.left = pItemCurSel->GetMergedRect ().left;
					//}
					//CPoint point (rect.left - 2, rect.bottom - 1);
					//设定只能选择首列
					CRect rect = pItemCurSel->GetParentRow()->GetItem(COL_NAME)->GetRect();
					CPoint point (rect.left, rect.bottom - 1);

					int nColumn = -1;
					if (pItemCurSel->m_pGridRow->HitTest (point, nColumn, pItem) != NULL)
					{
						id = CBCGPGridItemID (pItemCurSel->m_pGridRow->GetRowId (), nColumn);
					}
				}
			}
				
			if (!id.IsNull () && pRowCurSel != NULL &&
				!(id.IsRow () && m_nHorzScrollOffset > 0)) // first scroll, then select thole row
			{
				if (id.IsRow () && (bShift && !m_bSingleSel))
				{
					// do not select thole row when "shift")
				}
				else
				{
					DWORD dwSelMode = SM_SINGLE_ITEM |
						((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);
					m_pSetSelItem = m_bVirtualMode ? NULL : pItem;

					SetCurSel (id, dwSelMode);

					m_pSetSelItem = NULL;

					EnsureVisible (pRowCurSel);
				}
			}
			else
			{
				OnHScroll (SB_TOP, 0, NULL);
			}
			return;
		}
		
		// else ==> act as VK_UP!

	case VK_UP:
		{
			if (pSel == NULL)
			{
				SetCurSel (CBCGPGridItemID (0, 0));
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			// Select prev. item:
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			CBCGPGridItemID idCurSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
			CBCGPGridRow* pRowCurSel = (bShift && !m_bSingleSel) ? m_pLastSelRow : m_pSelRow;
			CBCGPGridItem* pItemCurSel = (bShift && !m_bSingleSel) ? m_pLastSelItem : m_pSelItem;
			if (m_bVirtualMode)
			{
				pRowCurSel = GetVirtualRow (idCurSel.m_nRow);
				pItemCurSel = (pRowCurSel != NULL) ? pRowCurSel->GetItem (idCurSel.m_nColumn) : NULL;
			}
			if (pRowCurSel != NULL)
			{
				ASSERT_VALID (pRowCurSel);
				CRect rect = pRowCurSel->GetRect ();
				if (pItemCurSel != NULL && pItemCurSel->GetMergedCells () != NULL)
				{
					rect.top = pItemCurSel->GetMergedRect ().top;
				}
				CPoint point (rect.right - 1, rect.top - 2);

				CBCGPGridRow* pRowNewSel = HitTest (point, NULL, TRUE);

				if (pRowNewSel != NULL)
				{
					ASSERT_VALID (pRowNewSel);

					CBCGPGridItemID id (pRowNewSel->GetRowId (), idCurSel.m_nColumn);

					if (!id.IsNull ())
					{
						DWORD dwSelMode = SM_SINGLE_ITEM |
							((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);

						m_bNoUpdateWindow = TRUE; // prevent flickering
						SetCurSel (id, dwSelMode);
						m_bNoUpdateWindow = FALSE;

						EnsureVisible (pRowNewSel);
					}
				}
				else
				{
					OnVScroll (SB_TOP, 0, NULL);
				}
			}
			return;
		}

	case VK_RIGHT:
		if ((::GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0 && !m_bIgnoreCtrlBtn)
		{
			return;
		}
		else if (pSel != NULL && pSel->IsGroup () && !pSel->IsExpanded () && 
			(!IsSortingMode () || (pSel->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0))
		{
			pSel->Expand ();
			return;
		}
		else if (!m_bWholeRowSel)
		{
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			// Go right to the nearest item:
			CCxStructTreeRowT* pRowCurSel = (bShift && !m_bSingleSel) ? (CCxStructTreeRowT*)m_pLastSelRow : (CCxStructTreeRowT*)m_pSelRow;
			CCxStructTreeItemT* pItemCurSel = (bShift && !m_bSingleSel) ? (CCxStructTreeItemT*)m_pLastSelItem : (CCxStructTreeItemT*)m_pSelItem;
			if (m_bVirtualMode)
			{
				CBCGPGridItemID idCurSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
				pRowCurSel = (CCxStructTreeRowT*)GetVirtualRow (idCurSel.m_nRow);
				pItemCurSel = (pRowCurSel != NULL) ? (CCxStructTreeItemT*)pRowCurSel->GetItem (idCurSel.m_nColumn) : NULL;
			}

			CBCGPGridItemID id;
			CBCGPGridItem* pItem = NULL;

			if (pItemCurSel != NULL)
			{
				if (m_bVirtualMode)
				{
					id = pItemCurSel->GetGridItemID ();
					//id.m_nColumn ++;
					int nPos = GetColumnsInfo ().IndexToOrder (id.m_nColumn);

					// Get position order of the merged cell
					if (pItemCurSel->GetMergedCells () != NULL)
					{
						CBCGPGridRange range;
						if (pItemCurSel->GetMergedRange (range))
						{
							int nPosLeft = GetColumnsInfo ().IndexToOrder (range.m_nLeft);
							int nPosRight = GetColumnsInfo ().IndexToOrder (range.m_nRight);
							if (nPosLeft != -1 && nPosRight != -1)
							{
								nPos = max (nPosLeft, nPosRight);
							}
							else
							{
								nPos = -1;
							}
						}
					}

					if (nPos >= 0 && nPos < GetColumnsInfo ().GetColumnCount (TRUE) - 1)
					{
						id.m_nColumn = GetColumnsInfo ().OrderToIndex (++nPos);
					}
					else
					{
						id.m_nColumn = -1;
					}
				}
				else
				{
					//CRect rect = pItemCurSel->GetRect ();
					//if (pItemCurSel->GetMergedCells () != NULL)
					//{
					//	rect.right = pItemCurSel->GetMergedRect ().right;
					//}
					//CPoint point (rect.right + 2, rect.bottom - 1);
					//设定只能选择首列
					CRect rect = pItemCurSel->GetParentRow()->GetItem(COL_NAME)->GetRect();
					CPoint point (rect.left, rect.bottom - 1);

					int nColumn = -1;
					if (pItemCurSel->m_pGridRow->HitTest (point, nColumn, pItem) != NULL)
					{
						id = CBCGPGridItemID (pItemCurSel->m_pGridRow->GetRowId (), nColumn);
					}

				}
			}
			else if (pRowCurSel != NULL)
			{
				id = CBCGPGridItemID (pRowCurSel->GetRowId (), GetColumnsInfo ().GetFirstVisibleColumn ());
			}

			if (!id.IsNull () && pRowCurSel != NULL &&
				!id.IsRow ()) // scroll, never select thole row
			{
				DWORD dwSelMode = SM_SINGLE_ITEM |
					((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);
				m_pSetSelItem = m_bVirtualMode ? NULL : pItem;


				SetCurSel (id, dwSelMode);

				m_pSetSelItem = NULL;

				EnsureVisible (pRowCurSel);
			}
			else
			{
				OnHScroll (SB_BOTTOM, 0, NULL);
			}
			return;
		}
		
		// else ==> act as VK_DOWN!

	case VK_DOWN:
		{
			if (pSel == NULL)
			{
				SetCurSel (CBCGPGridItemID (0, 0));
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			if ((::GetAsyncKeyState (VK_MENU) & 0x8000) && nChar == VK_DOWN)
			{
				CBCGPGridItem* pItem = GetCurSelItem (pSel);
				if (pItem != NULL)
				{
					pItem->DoClickButton (CPoint (-1, -1));
				}

				return;
			}

			// Select next item:
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			CBCGPGridItemID idCurSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
			CBCGPGridRow* pRowCurSel = (bShift && !m_bSingleSel) ? m_pLastSelRow : m_pSelRow;
			CBCGPGridItem* pItemCurSel = (bShift && !m_bSingleSel) ? m_pLastSelItem : m_pSelItem;
			if (m_bVirtualMode)
			{
				pRowCurSel = GetVirtualRow (idCurSel.m_nRow);
				pItemCurSel = (pRowCurSel != NULL) ? pRowCurSel->GetItem (idCurSel.m_nColumn) : NULL;
			}
			if (pRowCurSel != NULL)
			{
				ASSERT_VALID (pRowCurSel);
				CRect rect = pRowCurSel->GetRect ();
				if (pItemCurSel != NULL && pItemCurSel->GetMergedCells () != NULL)
				{
					rect.bottom = pItemCurSel->GetMergedRect ().bottom;
				}
				CPoint point (rect.right - 1, rect.bottom + 2);

				CBCGPGridRow* pRowNewSel = HitTest (point, NULL, TRUE);

				if (pRowNewSel != NULL)
				{
					ASSERT_VALID (pRowNewSel);

					CBCGPGridItemID id (pRowNewSel->GetRowId (), idCurSel.m_nColumn);

					if (!id.IsNull ())
					{
						DWORD dwSelMode = SM_SINGLE_ITEM |
							((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);

						m_bNoUpdateWindow = TRUE; // prevent flickering
						SetCurSel (id, dwSelMode);
						m_bNoUpdateWindow = FALSE;

						EnsureVisible (pRowNewSel);
					}
				}
				else
				{
					//OnVScroll (SB_BOTTOM, 0, NULL);
				}
			}
			return;
		}

	case VK_NEXT:
		{
			if (pSel == NULL)
			{
				SetCurSel (CBCGPGridItemID (0, 0));
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			if (m_nVertScrollPage != 0)
			{
				const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

				CBCGPGridItemID idCurSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
				CBCGPGridRow* pRowCurSel = (bShift && !m_bSingleSel) ? m_pLastSelRow : m_pSelRow;
				if (m_bVirtualMode)
				{
					pRowCurSel = GetVirtualRow (idCurSel.m_nRow);
				}

				if (pRowCurSel != NULL)
				{
					//OnVScroll (SB_PAGEDOWN, 0, NULL);

					// Get row id:
					ASSERT_VALID (pRowCurSel);
					CRect rect = pRowCurSel->GetRect ();
					CPoint point (rect.right - 1,
								  max (rect.top + m_nVertScrollPage - m_nRowHeight,
									   rect.bottom + 1));

					CBCGPGridRow* pRowNewSel = HitTest (point, NULL, TRUE);

					CBCGPGridItemID id;
					if (pRowNewSel != NULL)
					{
						ASSERT_VALID (pRowNewSel);
						id = CBCGPGridItemID (pRowNewSel->GetRowId (), idCurSel.m_nColumn);
					}
					else
					{
						//---------------------
						// Calculate page size:
						//---------------------
						int nFirst = -1, nLast = -1;
						int nPageItems = GetPageItems (nFirst, nLast, m_nFirstVisibleItem);
						if (nPageItems <= 0)
						{
							nPageItems = 1;
						}

						const int nLastRow = GetTotalItems () - 1;
						const int nNextRow = OffsetVisibleRow (pRowCurSel->GetRowId (), nPageItems, TRUE);
						id = CBCGPGridItemID (min (nNextRow, nLastRow), idCurSel.m_nColumn);
					}

					if (!id.IsNull ())
					{
						// Change selection:
						DWORD dwSelMode = SM_SINGLE_ITEM |
							((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);
						SetCurSel (id, dwSelMode);
						
						pRowNewSel = (bShift && !m_bSingleSel) ? m_pLastSelRow : m_pSelRow;
						if (m_bVirtualMode)
						{
							CBCGPGridItemID idNewSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
							pRowNewSel = GetVirtualRow (idNewSel.m_nRow);
						}

						// Scroll:
						if (pRowNewSel != NULL)
						{
							EnsureVisible (pRowNewSel);
						}
						else
						{
							OnVScroll (SB_BOTTOM, 0, NULL);
						}
					}
				}
			}
			return;
		}

	case VK_END:
		{
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			const int nRowCount = GetTotalItems ();

			CBCGPGridItemID id;
			id.m_nRow = nRowCount - 1;
			id.m_nColumn = (bShift && !m_bSingleSel && !m_idLastSel.IsNull ()) ?
				m_idLastSel.m_nColumn :
				((!m_idActive.IsNull ()) ? m_idActive.m_nColumn : 0);

			CBCGPGridRow* pLastRow = GetRow (id.m_nRow);
			if (pLastRow != NULL)
			{
				ASSERT_VALID (pLastRow);
				if (!IsRowVisible (pLastRow))
				{
					id.m_nRow = OffsetVisibleRow (id.m_nRow, 1, FALSE);
				}
			}

			DWORD dwSelMode = SM_SINGLE_ITEM |
				((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);

			SetCurSel (id, dwSelMode);
			OnVScroll (SB_BOTTOM, 0, NULL);
		}
		return;

	case VK_PRIOR:
		{
			if (pSel == NULL)
			{
				SetCurSel (CBCGPGridItemID (0, 0));
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			if (m_nVertScrollPage != 0)
			{
				const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

				CBCGPGridItemID idCurSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
				CBCGPGridRow* pRowCurSel = (bShift && !m_bSingleSel) ? m_pLastSelRow : m_pSelRow;
				if (m_bVirtualMode)
				{
					pRowCurSel = GetVirtualRow (idCurSel.m_nRow);
				}
				
				if (pRowCurSel != NULL)
				{
					//OnVScroll (SB_PAGEUP, 0, NULL);
					
					// Get row id:
					ASSERT_VALID (pRowCurSel);
					CRect rect = pRowCurSel->GetRect ();
					CPoint point (rect.right - 1,
								  min (rect.bottom - m_nVertScrollPage + m_nRowHeight,
									   rect.top - 1));

					CBCGPGridRow* pRowNewSel = HitTest (point, NULL, TRUE);

					CBCGPGridItemID id;
					if (pRowNewSel != NULL)
					{
						ASSERT_VALID (pRowNewSel);
						id = CBCGPGridItemID (pRowNewSel->GetRowId (), idCurSel.m_nColumn);
					}
					else
					{
						//---------------------
						// Calculate page size:
						//---------------------
						int nFirst = -1, nLast = -1;
						int nPageItems = GetPageItems (nFirst, nLast, m_nFirstVisibleItem);
						if (nPageItems <= 0)
						{
							nPageItems = 1;
						}

						const int nFirstRow = 0;
						const int nPrevRow = OffsetVisibleRow (pRowCurSel->GetRowId (), nPageItems, FALSE);
						id = CBCGPGridItemID (max (nPrevRow, nFirstRow), idCurSel.m_nColumn);
					}
					
					if (!id.IsNull ())
					{
						// Change selection:
						DWORD dwSelMode = SM_SINGLE_ITEM |
							((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);
						SetCurSel (id, dwSelMode);
			
						pRowNewSel = (bShift && !m_bSingleSel) ? m_pLastSelRow : m_pSelRow;
						if (m_bVirtualMode)
						{
							CBCGPGridItemID idNewSel = (bShift && !m_bSingleSel) ? m_idLastSel: m_idActive;
							pRowNewSel = GetVirtualRow (idNewSel.m_nRow);
						}

						// Scroll:
						if (pRowNewSel != NULL)
						{
							EnsureVisible (pRowNewSel);
						}
						else
						{
							OnVScroll (SB_TOP, 0, NULL);
						}
					}
				}
			}
			return;
		}

	case VK_HOME:
		{
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			CBCGPGridItemID id;
			id.m_nRow = 0;
			id.m_nColumn = (bShift && !m_bSingleSel && !m_idLastSel.IsNull ()) ?
				m_idLastSel.m_nColumn :
				((!m_idActive.IsNull ()) ? m_idActive.m_nColumn : 0);

			CBCGPGridRow* pLastRow = GetRow (id.m_nRow);
			if (pLastRow != NULL)
			{
				ASSERT_VALID (pLastRow);
				if (!IsRowVisible (pLastRow))
				{
					id.m_nRow = OffsetVisibleRow (id.m_nRow, 1, TRUE);
				}
			}

			DWORD dwSelMode = SM_SINGLE_ITEM |
				((bShift && !m_bSingleSel) ? SM_CONTINUE_SEL_GROUP : SM_SINGE_SEL_GROUP);
			
			SetCurSel (id, dwSelMode);
			OnVScroll (SB_TOP, 0, NULL);
		}

		return;

	case VK_ADD:
		if (pSel != NULL && pSel->IsGroup () && !pSel->IsExpanded () &&
			!pSel->IsInPlaceEditing () &&
			(!IsSortingMode () || (pSel->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0))
		{
			pSel->Expand ();
		}
		return;

	case VK_SUBTRACT:
		if (pSel != NULL && pSel->IsGroup () && pSel->IsExpanded () && 
			!pSel->IsInPlaceEditing () &&
			(!IsSortingMode () || (pSel->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0))
		{
			pSel->Expand (FALSE);
		}
		return;

	case VK_RETURN:
		if (pSel != NULL && pSel->IsGroup () && !pSel->IsInPlaceEditing () &&
			(!IsSortingMode () || (pSel->m_dwFlags & BCGP_GRID_ITEM_AUTOGROUP) != 0))
		{
			pSel->Expand (!pSel->IsExpanded ());
		}
		return;

	case VK_TAB:
		{
			const BOOL bShift = (::GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0 && !m_bIgnoreShiftBtn;

			if (m_bFilterBar && m_nFocusedFilter != -1)
			{
				OnFilterBarTab (bShift);
			}
			else if (pSel != NULL && GetCurSelItem (pSel) == NULL)
			{
				GoToNextItem (!bShift ? NextRow : PrevRow );
			}
			else
			{
				GoToNextItem (!bShift ? 
					(NextColumn | Down): 
					(PrevColumn | Up) );
			}
		}
		return;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//*****************************************************************************************
BOOL CCxStructTreeGridT::DoRemoveRow (POSITION posDel, BOOL bRedraw)
{
	ASSERT_VALID (this);

	CCxStructTreeNode * pNode = NULL;

	if (posDel != NULL)
	{
		CCxStructTreeRowT* pItemDel = (CCxStructTreeRowT*)m_lstItems.GetAt (posDel);
		ASSERT_VALID (pItemDel);

		if (pItemDel->IsSelected ())
		{
			// Cleanup selection:
			m_idActive.SetNull ();
			m_idLastSel.SetNull ();
			SetCurSel (m_idActive, SM_NONE, FALSE);
		}

		// Remove parent-child links:
		CCxStructTreeRowT* pParent = (CCxStructTreeRowT*)pItemDel->GetParent ();
		if (pParent != NULL)
		{
			for (POSITION pos = pParent->m_lstSubItems.GetHeadPosition (); pos != NULL; )
			{
				POSITION posSave = pos;

				CCxStructTreeRowT* pChildRow = (CCxStructTreeRowT*)pParent->m_lstSubItems.GetNext (pos);
				ASSERT_VALID (pChildRow);

				if (pChildRow == pItemDel)
				{
					pParent->m_lstSubItems.RemoveAt (posSave);
					break;
				}
			}
		}

		for (POSITION pos = pItemDel->m_lstSubItems.GetHeadPosition (); pos != NULL; )
		{
			CCxStructTreeRowT* pChildRow = (CCxStructTreeRowT*)pItemDel->m_lstSubItems.GetNext (pos);
			ASSERT_VALID (pChildRow);

			pChildRow->m_pParent = NULL;
		}


		m_lstItems.RemoveAt (posDel);

		//删除行的同时，删除结构树中的航指针
		pNode = reinterpret_cast<CCxStructTreeNode*> ( pItemDel->GetData() );
		if ( pNode != NULL ) pNode->m_pRow = NULL;

		delete pItemDel;

		SetRebuildTerminalItems ();

		if (bRedraw && !m_bPostAdjustLayout && GetSafeHwnd() != NULL)
		{
			PostMessage(BCGM_GRID_ADJUST_LAYOUT);
			m_bPostAdjustLayout = TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

//*****************************************************************************
BOOL CCxStructTreeGridT::DoClickValue (CBCGPGridItem* pItem, UINT uiMsg, CPoint point, 
								  BOOL bFirstClick, BOOL bButtonClick)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pItem);

	CCxStructTreeRowT* pCurSel =(CCxStructTreeRowT* ) GetCurSel ();
	if (pCurSel == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (pCurSel);

	CCxStructTreeItemT * pSelItem = NULL;
	if ( m_idRealSelItem.m_nColumn >= 0 )
	{
		pSelItem = (CCxStructTreeItemT *)pCurSel->GetItem( m_idRealSelItem.m_nColumn );
	}
	// Start inplace editing:
	if (AllowInPlaceEdit () && pSelItem->IsAllowEdit ()
		&& (!bFirstClick || IsEditFirstClick ()))
	{
		SetBeginEditReason (BeginEdit_MouseClick);
		EditItem (pCurSel, &point);

		DoInplaceEditSetSel (OnInplaceEditSetSel (pSelItem, BeginEdit_MouseClick));
	}

	// Translate click:
	if (bButtonClick)
	{
		if (pCurSel->m_bInPlaceEdit)
		{
			if (::GetCapture () == GetSafeHwnd ())
			{
				ReleaseCapture ();
			}
		}

		pItem->DoClickButton (point);
	}
	else
	{
		return pSelItem->OnClickValue (uiMsg, point);
	}

	return TRUE;
}
//*******************************************************************************************
void CCxStructTreeGridT::OnBeginInplaceEdit (CBCGPGridItem* pItem)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pItem);

	CWnd* pOwner = GetOwner ();
	if (pOwner == NULL)
	{
		return;
	}

	CBCGPGridItemID id = pItem->GetGridItemID ();

	memset (&m_CurrentItemInfo, 0, sizeof (BCGPGRID_ITEM_INFO));

	m_CurrentItemInfo.pItem = pItem;
	m_CurrentItemInfo.nRow = id.m_nRow;
	m_CurrentItemInfo.nCol = id.m_nColumn;
	m_CurrentItemInfo.dwResultCode = m_dwBeginEditReason;

	ConvertBSTR2CString( g_strLastValue, pItem->GetValue().bstrVal );

	pOwner->SendMessage (BCGM_GRID_ITEM_BEGININPLACEEDIT, GetDlgCtrlID (), LPARAM (&m_CurrentItemInfo));
}
//****************************************************************************************
void CCxStructTreeGridT::OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pItem);

	CWnd* pOwner = GetOwner ();
	if (pOwner == NULL)
	{
		return;
	}

	BCGPGRID_ITEM_INFO ii;
	memset (&ii, 0, sizeof (BCGPGRID_ITEM_INFO));
	ii.pItem = pItem;
	ii.nRow = nRow;
	ii.nCol = nColumn;
	ii.dwResultCode = 0;

	pOwner->SendMessage (BCGM_GRID_ITEM_CHANGED, GetDlgCtrlID (), LPARAM (&ii));
	
	//////////////////////////////////////////////////////////////////////////

	CCxStructTreeRowT* pCurRow = (CCxStructTreeRowT *)pItem->GetParentRow();
	if (pCurRow == NULL) return;

	CCxStructTreeNode* pCurNode = reinterpret_cast < CCxStructTreeNode * > ( pCurRow->GetData() );
	if ( pCurNode == NULL ) return;

	CString strCurValue, strDoubleValue;
	ConvertBSTR2CString( strCurValue, pItem->GetValue().bstrVal );
	if ( pCurNode->IsParamNode() )
	{
		//if ( ((CCxStructTreeNodeParam*)pCurNode)->IsDouble() )
		//{
		//	strDoubleValue = ConvertDoubleValue( strCurValue );
		//	strCurValue = strDoubleValue;
		//	if ( !strCurValue.IsEmpty() )
		//	{
		//		pItem->SetValue( (LPCTSTR)strCurValue );
		//	}
		//}
	}
	if ( g_strLastValue == strCurValue ) return;

	//验证输入数据
	switch ( ((CCxStructTreeNodeParam*)pCurNode)->m_nDataType )
	{
	case DT_INT:
		if ( !CStringChecker::CheckInt( strCurValue ) )
		{
			::MessageBox(GetMainWnd(), "输入数据不是整型", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			pItem->SetValue( (LPCTSTR)g_strLastValue );
			return;
		}
		{
			strCurValue.Trim();
			int iVal = atoi(strCurValue);
			itoa(iVal,strCurValue.GetBuffer(50),10);
			strCurValue.ReleaseBuffer();
			pItem->SetValue((LPCTSTR)strCurValue);
		}
		break;
	case DT_DOUBLE:
		if ( !CStringChecker::CheckDouble( strCurValue ) )
		{
			::MessageBox(GetMainWnd(), "输入数据不是浮点型", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			pItem->SetValue( (LPCTSTR)g_strLastValue );
			return;
		}
		break;
	default:
		break;
	}

	//UpdateParamValue( (LPVOID)(LPCTSTR)strCurValue, (CCxStructTreeRowT*)pCurRow );
	//pCurNode->SetModified();

	if ( !pCurNode->IsParamNode() ) return;
	CCxStructTreeNodeParam* pParamNode = ( CCxStructTreeNodeParam* )pCurNode;
//	pParamNode->SetValue( (LPCTSTR)strCurValue, TRUE, FALSE, FALSE );
	pParamNode->UpdateValue( (LPCTSTR)strCurValue );
	pParamNode->OnAfterEditValue();

	//if ( pParamNode->IsArray )
	//switch ( pCurNode->m_NodeType )
	//{
	//case CCxStructTreeNode::NT_PARAM:
	//	switch ( pCurNode->m_nDataType )
	//	{
	//	case DT_INT:
	//	case DT_DOUBLE:
	//	case DT_STRING:
	//	case DT_BOOLEAN:
	//		pCurNode->m_strValue = reinterpret_cast< LPCTSTR > ( lpNewValue );
	//		break;

	//	case DT_ARRAY1D_BOOLEAN:
	//	case DT_ARRAY2D_BOOLEAN:
	//	case DT_ARRAY3D_BOOLEAN:
	//	case DT_ARRAY4D_BOOLEAN:
	//	case DT_ARRAY1D_INT:
	//	case DT_ARRAY2D_INT:
	//	case DT_ARRAY3D_INT:
	//	case DT_ARRAY4D_INT:
	//	case DT_ARRAY1D_DOUBLE:
	//	case DT_ARRAY2D_DOUBLE:
	//	case DT_ARRAY3D_DOUBLE:
	//	case DT_ARRAY4D_DOUBLE:
	//	case DT_ARRAY1D_STRING:
	//	case DT_ARRAY2D_STRING:
	//	case DT_ARRAY3D_STRING:
	//	case DT_ARRAY4D_STRING:
	//	case DT_FILE:
	//		break;

	//	default:
	//		return FALSE;
	//	}
	//	break;

	//case CCxStructTreeNode::NT_PARAM_ARRAY:
	//	break;

	//default:
	//	return FALSE;
	//}

	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;
	pSvg->JsMethod_paramUpdate( pParamNode );


}

void CCxStructTreeGridT::ShowPopMenuForSvg( CCxStructTreePopMenuData* popd )
{
	if (popd->m_strMenuType.CompareNoCase("component") == 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_COMPONENT_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("process")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_PROCESS_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("frontForm")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FORM_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("rearForm")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FORM_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("for")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FLOW_FOR_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("if")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FLOW_IF_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("parallel")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FLOW_PARALLEL_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("sequence")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FLOW_SEQUENCE_POP, this);
	}
	else if (popd->m_strMenuType.CompareNoCase("subflow")== 0)
	{
		ShowPopMenu(IDR_JIANMO_TREE_FLOW_SUBFLOW_POP, this);
	}

	else
		return;
}

void CCxStructTreeGridT::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child Windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CCxStructTreeGridT::OnJianmoTreeFormPopform()
{
	// TODO: 在此添加命令处理程序代码
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	ASSERT(pNode->m_pParent!=NULL);
	CCxStructTreeNodeComponent *pComponent = NULL;
	CCxStructTreeNodeFlow *pFlow = NULL;
	CCxStructTreeNodeSubFlow *pSubFlow = NULL;

	CString strId = pNode->m_pParent->m_strID;
	if (pNode->m_pParent->IsComponentNode())
	{
		pComponent = static_cast<CCxStructTreeNodeComponent*>(pNode->m_pParent);
		ASSERT(pComponent!=NULL);
		if (pComponent->m_bIsPopForm) return;
		pComponent->m_bIsPopForm = TRUE;
	}
	else if (pNode->m_pParent->m_NodeType == CCxStructTreeNode::NT_SUBFLOW)
	{
		pSubFlow = static_cast<CCxStructTreeNodeSubFlow*>(pNode->m_pParent);
		ASSERT(pSubFlow!=NULL);
		if (pSubFlow->m_bIsPopForm) return;
		pSubFlow->m_bIsPopForm = TRUE;
	}
	else if (pNode->m_pParent->IsFlowNode()|| pNode->m_pParent->IsRootNode())
	{
		pFlow = static_cast<CCxStructTreeNodeFlow*>(pNode->m_pParent);
		ASSERT(pFlow!=NULL);
		if (pFlow->m_bIsPopForm) return;
		pFlow->m_bIsPopForm = TRUE;
	}
	else
		return;

	TellSvgFormShowStateChange(strId,TRUE);
}
void CCxStructTreeGridT::OnJianmoTreeFormNotpopform()
{
	// TODO: 在此添加命令处理程序代码
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	ASSERT(pNode->m_pParent!=NULL);
	CCxStructTreeNodeComponent *pComponent = NULL;
	CCxStructTreeNodeFlow *pFlow = NULL;
	CCxStructTreeNodeSubFlow *pSubFlow = NULL;
	CString strId = pNode->m_pParent->m_strID;
	if (pNode->m_pParent->IsComponentNode())
	{
		pComponent = static_cast<CCxStructTreeNodeComponent*>(pNode->m_pParent);
		ASSERT(pComponent!=NULL);
		if (!pComponent->m_bIsPopForm) return;
		pComponent->m_bIsPopForm = FALSE;
	}
	else if (pNode->m_pParent->m_NodeType == CCxStructTreeNode::NT_SUBFLOW)
	{
		pSubFlow = static_cast<CCxStructTreeNodeSubFlow*>(pNode->m_pParent);
		ASSERT(pSubFlow!=NULL);
		if (!pSubFlow->m_bIsPopForm) return;
		pSubFlow->m_bIsPopForm = FALSE;
	}
	else if (pNode->m_pParent->IsFlowNode() || pNode->m_pParent->IsRootNode())
	{
		pFlow = static_cast<CCxStructTreeNodeFlow *>(pNode->m_pParent);
		ASSERT(pFlow!=NULL);
		if (!pFlow->m_bIsPopForm) return;
		pFlow->m_bIsPopForm = FALSE;
	}
	else
		return;

	TellSvgFormShowStateChange(strId,FALSE);
}
void CCxStructTreeGridT::TellSvgFormShowStateChange(CString strId,BOOL bShowForm)
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "setPopFormFlag";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = strId;
	wmd.m_vParams.push_back(pParam);

	//第2个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "flag";
	pParam->m_sValue = bShowForm ? "true":"false";
	wmd.m_vParams.push_back(pParam);

	pSvg->InvokeJsMethod( &wmd );
	AdjustLayout();
	m_bModified = TRUE;
}
void CCxStructTreeGridT::OnUpdateJianmoTreeFormPopform(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	CCxStructTreeNodeComponent *pComponent = NULL;
	CCxStructTreeNodeFlow *pFlow = NULL;
	CCxStructTreeNodeSubFlow *pSubFlow = NULL;
	BOOL bShowForm = TRUE;

	if (pNode->m_pParent->IsComponentNode())
	{
		pComponent = static_cast<CCxStructTreeNodeComponent *>(pNode->m_pParent);
		bShowForm = pComponent->m_bIsPopForm;
	}
	else if (pNode->m_pParent->m_NodeType == CCxStructTreeNode::NT_SUBFLOW)
	{
		pSubFlow = static_cast<CCxStructTreeNodeSubFlow*>(pNode->m_pParent);
		bShowForm = pSubFlow->m_bIsPopForm;
	}
	else if (pNode->m_pParent->IsFlowNode()|| pNode->m_pParent->IsRootNode())
	{
		pFlow = static_cast<CCxStructTreeNodeFlow *>(pNode->m_pParent);
		bShowForm = pFlow->m_bIsPopForm;
	}
	else
		return;

	pCmdUI->SetCheck(bShowForm);
}


void CCxStructTreeGridT::OnUpdateJianmoTreeFormNotpopform(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CCxStructTreeNode * pNode = GetCurSelNodeData();
	if ( pNode == NULL ) return;

	ASSERT(pNode->m_pParent);
	CCxStructTreeNodeComponent *pComponent = NULL;
	CCxStructTreeNodeFlow *pFlow = NULL;
	CCxStructTreeNodeSubFlow *pSubFlow = NULL;
	BOOL bShowForm = TRUE;

	if (pNode->m_pParent->IsComponentNode())
	{
		pComponent = static_cast<CCxStructTreeNodeComponent *>(pNode->m_pParent);
		bShowForm = pComponent->m_bIsPopForm;
	}
	else if (pNode->m_pParent->m_NodeType == CCxStructTreeNode::NT_SUBFLOW)
	{
		pSubFlow = static_cast<CCxStructTreeNodeSubFlow*>(pNode->m_pParent);
		bShowForm = pSubFlow->m_bIsPopForm;
	}
	else if (pNode->m_pParent->IsFlowNode()|| pNode->m_pParent->IsRootNode())
	{
		pFlow = static_cast<CCxStructTreeNodeFlow *>(pNode->m_pParent);
		bShowForm = pFlow->m_bIsPopForm;
	}
	else
		return;

	pCmdUI->SetCheck(!bShowForm);
}

BOOL CCxStructTreeGridT::CanAddNewRow( CCxStructTreeNode* pNode )
{
	//添加子行时需要判断：父行pRow不存在，或兄弟行pRow不存在，则不运行添加行；
	if ( pNode == NULL )
	{
		return FALSE;
	}
	CCxStructTreeNode* pRoot = m_StructTree.GetRoot();
	if ( pNode == pRoot )
	{
		return TRUE;
	}
	CCxStructTreeNode* pParentNode = pNode->m_pParent;
	if ( pParentNode == NULL )
	{
		return FALSE;
	}
	if ( pParentNode->m_pRow == NULL )
	{
		return FALSE;
	}
	int nChildSize = pParentNode->m_vChildren.size();
	if ( nChildSize <= 0 )
	{
		return FALSE;
	}
	else
	{
		for ( int i = 0; i < nChildSize; i++ )
		{
			CCxStructTreeNode* pBrotherNode = pParentNode->m_vChildren[i];
			if ( pBrotherNode != NULL )
			{
				if ( pBrotherNode != pNode && pBrotherNode->m_pRow == NULL )
				{
					return FALSE;
				}
			}
			else
			{
				ZTools::WriteZToolsLog( "CCxStructTreeGridT::CanAddNewRow 获取子节点内存为空" );
				return FALSE;
			}
		}
		return TRUE;
	}
}

BOOL CCxStructTreeGridT::CanRemoveRow( CCxStructTreeNode* pNode )
{
	if ( pNode == NULL )
	{
		return FALSE;
	}
	if ( pNode->m_pRow == NULL )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
