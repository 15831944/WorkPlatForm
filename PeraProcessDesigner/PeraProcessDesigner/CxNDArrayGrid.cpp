// CxNDArrayGrid.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxNDArrayGrid.h"
#include "UserDefine.h"
#include "CxBCGPVisualManager2007.h"
#include "CxArrayND.h"
#include "CxStructTreeNodeParam.h"
#include <CStringChecker.h>

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


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCxNDArrayEditT, CEdit)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCxNDArrayEditT, CEdit)

	CCxNDArrayEditT::CCxNDArrayEditT()
{
	m_pArrayND = NULL;
}

CCxNDArrayEditT::~CCxNDArrayEditT()
{
}

BOOL CCxNDArrayEditT::PreTranslateMessage(MSG* pMsg)
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
			if ( m_pArrayND != NULL )
			{
				int nDataType = m_pArrayND->GetRealDataType();
				if ( nDataType == DT_DOUBLE || nDataType == DT_INT )
				{
					return TRUE;
				}
			}
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CCxNDArrayGrid, CBCGPGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CCxNDArrayItem, CBCGPGridItem, VERSIONABLE_SCHEMA | 1)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCxNDArrayItem::CCxNDArrayItem ()
{
}
//*******************************************************************************************
CCxNDArrayItem::CCxNDArrayItem(const _variant_t& varValue, DWORD_PTR dwData,
		LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
		LPCTSTR lpszValidChars) : CBCGPGridItem(varValue, dwData, lpszEditMask, lpszEditTemplate
		, lpszValidChars)
{
}
//*******************************************************************************************
CCxNDArrayItem::~CCxNDArrayItem()
{
}
//******************************************************************************************
COLORREF CCxNDArrayItem::OnFillBackground (CDC* pDC, CRect rect)
{
	return CBCGPGridItem::OnFillBackground(pDC, rect);
}
//******************************************************************************************
void CCxNDArrayItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	CCxNDArrayGrid* pWndList = (CCxNDArrayGrid*)((CCxNDArrayRow*)m_pGridRow)->m_pWndList;
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
//******************************************************************************************
void CCxNDArrayItem::OnDrawBorders (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CCxNDArrayGrid* pWndList = (CCxNDArrayGrid*)GetOwnerList ();
	if (pWndList->GetSafeHwnd () == NULL)
	{
		return;
	}

	GRID_BORDERS borders;
	OnGetBorders (borders);

	CPen* pOldPen = NULL;

	if (borders.top != GRID_BORDERSTYLE_EMPTY)
	{
	}

	if (borders.left != GRID_BORDERSTYLE_EMPTY)
	{
	}

	if (borders.bottom != GRID_BORDERSTYLE_EMPTY)
	{
		CPen* pPen = pDC->SelectObject (&pWndList->m_penHLine);
		if (pOldPen == NULL)
		{
			pOldPen = pPen;
		}

		pDC->MoveTo (rect.left, rect.bottom);
		pDC->LineTo (rect.right + 1, rect.bottom);
	}

	if (borders.right != GRID_BORDERSTYLE_EMPTY)
	{
		CPen* pPen = pDC->SelectObject (&pWndList->m_penVLine);
		if (pOldPen == NULL)
		{
			pOldPen = pPen;
		}

		pDC->MoveTo (rect.right, rect.top);
		pDC->LineTo (rect.right, rect.bottom);
	}

	if (pOldPen != NULL)
	{
		pDC->SelectObject (pOldPen);
	}
}

//*****************************************************************************
CEdit* CCxNDArrayItem::NewInPlaceEdit ()
{
	CEdit * pEdit = new CCxNDArrayEditT;
	CCxNDArrayRow * pRow = (CCxNDArrayRow *)GetParentRow();
	if ( pRow != NULL )
	{
		CCxNDArrayGrid * pGrid = (CCxNDArrayGrid*)pRow->m_pWndList;
		if ( pGrid != NULL )
		{
			((CCxNDArrayEditT*)pEdit)->m_pArrayND = pGrid->m_pArrayData;
		}
	}
	return pEdit;
}


void CCxNDArrayItem::FormatDoubleValue( CString& strValue )
{
	CCxNDArrayRow* pRow = (CCxNDArrayRow*)GetParentRow();
	if ( pRow == NULL ) return;
	CCxNDArrayGrid* pGrid = (CCxNDArrayGrid*)pRow->GetOwnerList();
	if ( pGrid == NULL ) return;
	if ( pGrid->m_pArrayData == NULL ) return;
	if ( pGrid->m_pArrayData->GetRealDataType() != DT_DOUBLE ) return;

	CString str = CStringChecker::FormatDouble( (LPCTSTR)strValue );
	strValue = str;
}


//******************************************************************************************
BOOL CCxNDArrayItem::OnUpdateValue ()
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
	}

	return bRes;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CCxNDArrayRow, CBCGPGridRow, VERSIONABLE_SCHEMA | 1)

CCxNDArrayRow::CCxNDArrayRow (const CString& strGroupName
	, DWORD_PTR dwData, int nBlockSize)
	: CBCGPGridRow( strGroupName, dwData, nBlockSize )
{
}
//******************************************************************************************
CCxNDArrayRow::CCxNDArrayRow (int nColumnsNum, DWORD_PTR dwData, int nBlockSize)
	: CBCGPGridRow( nColumnsNum, dwData, nBlockSize )
{
}
//******************************************************************************************
CBCGPGridItem* CCxNDArrayRow::CreateItem (int nRow, int nColumn)
{
	if (m_pWndList == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}
	ASSERT_VALID (m_pWndList);
	CCxNDArrayGrid * pGrid = ( CCxNDArrayGrid * )m_pWndList;
	return pGrid->CreateItem (nRow, nColumn);
}
//******************************************************************************************
void CCxNDArrayRow::OnDrawItems (CDC* pDC, CRect rect)
{
	//CBCGPGridRow::OnDrawItems (pDC, rect);
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	CCxNDArrayGrid * pGrid = ( CCxNDArrayGrid * ) m_pWndList;
	
	rect.NormalizeRect ();
	const int nFreezeOffset = pGrid->m_rectList.left + pGrid->GetColumnsInfo ().GetFreezeOffset ();

	for (int i = 0; i < m_arrRowItems.GetSize (); i++)
	{
		CCxNDArrayItem* pItem = (CCxNDArrayItem*)m_arrRowItems [i];
		ASSERT_VALID (pItem);

		CRect rectValue = pItem->GetRect ();
		if (!rectValue.IsRectEmpty ())
		{
			if (pItem->GetMergedCells () != NULL)
			{
				// Item is a part of merged item - Draw item later.
				pGrid->RedrawMergedItem (pItem);
				continue;
			}

			rectValue.NormalizeRect ();

			CRect rectClipItem = rectValue;

			// frozen columns:
			if (pGrid->GetColumnsInfo ().IsFreezeColumnsEnabled () &&
				!pGrid->GetColumnsInfo ().IsColumnFrozen (i))
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
			if (pGrid->m_bGridItemBorders)
			{
				pItem->OnGetBorders (rectBordersSize);
				rectClipItem.InflateRect (rectBordersSize);
			}

			pGrid->m_rgnClipItem.CreateRectRgnIndirect (&rectClipItem);
			pDC->SelectClipRgn (&pGrid->m_rgnClipItem);

			pItem->OnDrawValue (pDC, rectValue);

			if (pGrid->m_bGridItemBorders)
			{
				pItem->OnDrawBorders (pDC, rectValue);
			}

			//if (!pItem->m_rectButton.IsRectEmpty ())
			//{
			//	pItem->OnDrawButton (pDC, pItem->m_rectButton);
			//}

			pDC->SelectClipRgn (&pGrid->m_rgnClipRow);
			pGrid->m_rgnClipItem.DeleteObject ();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCxNDArrayGrid, CBCGPGridCtrl)

CCxNDArrayGrid::CCxNDArrayGrid()
{
	m_pArrayData = NULL;
	m_bModified = FALSE;
}
//*********************************************************************
CCxNDArrayGrid::CCxNDArrayGrid( int nMemBlockSize )
	: CBCGPGridCtrl(nMemBlockSize)
{
}
//*********************************************************************
CCxNDArrayGrid::~CCxNDArrayGrid()
{
}
//*********************************************************************
BOOL CCxNDArrayGrid::Init( CRect& rc, CWnd * pParent )
{
	if ( IsWindow(m_hWnd) ) return FALSE;

	Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, pParent, ID_NDARRAY_VIEW);

	EnableColumnAutoSize (FALSE);
	SetSingleSel (TRUE);
	EnableGroupByBox (FALSE);
	if ( m_pArrayData == NULL )
	{
		SetReadOnly (FALSE);
	}
	else
	{
		BOOL bReadOnly = (m_pArrayData->m_nDir == PARAM_DIR_OUT);

		if ( !bReadOnly )
		{
			bReadOnly = (m_pArrayData->m_pNode->m_dwStatus & CCxStructTreeNode::NS_VALUE_READONLY);
		}

		if ( !bReadOnly )
		{
			bReadOnly = ( m_pArrayData->GetRealDataType() == DT_BOOLEAN );
		}
		SetReadOnly ( bReadOnly );
	}
	SetWholeRowSel (FALSE);
	EnableHeader (TRUE, 0);
	EnableRowHeader (TRUE, 0, 30);
	m_bLineNumbers = TRUE;
	// Set grid tab order (first):
	SetWindowPos (&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	m_nBaseHeight = 20;
	//	SetSelectionBorder(FALSE);

	//m_bGridItemBorders = TRUE;
	//m_bHighlightActiveItem = FALSE;
	//m_bDrawFocusRect = FALSE;
	//m_bSelectionBorder = FALSE;

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
	//colors.m_HeaderColors.m_clrBorder = g_crNDArrayHeaderBorder;

	colors.m_LeftOffsetColors.m_clrBackground = g_crStructTreeClientBg;	// Left offset colors

	//colors.m_clrHorzLine = g_crStructTreeClientBg;		// Grid horizontal line text
	//colors.m_clrVertLine = g_crStructTreeClientBg;		// Grid vertical line text

	//colors.m_clrPreviewText;	// Description text color

	SetColorTheme (colors);

	//SetSelectionBorderForActiveItem(FALSE);
	//SetSelectionBorder(FALSE);

	AdjustLayout();

	return TRUE;
}
//*********************************************************************
CBCGPGridRow * CCxNDArrayGrid::CreateRow ()
{
	return new CCxNDArrayRow (0, 0, GetRowBlockSize ());
}
//*********************************************************************
CCxNDArrayRow * CCxNDArrayGrid::CreateNewRow()
{
	CCxNDArrayRow* pRow = (CCxNDArrayRow*)CBCGPGridCtrl::CreateRow ( GetColumnCount () );

	for (int i = 0; i < GetColumnCount ();i++)
	{
		pRow->GetItem (i)->AllowEdit ( !m_bReadOnly );
	}

	return ( CCxNDArrayRow * )pRow;
}
//*********************************************************************
CBCGPGridItem* CCxNDArrayGrid::CreateItem (int nRow, int nColumn)
{
	CCxNDArrayItem* pItem = NULL;

	CRuntimeClass* pItemRTC = NULL;
	if (m_mapItemsRTC.Lookup (nColumn, pItemRTC))
	{
		ASSERT (pItemRTC != NULL);

		pItem = DYNAMIC_DOWNCAST (CCxNDArrayItem, pItemRTC->CreateObject ());
	}
	else if (m_pDefaultItemRTC != NULL)
	{
		pItem = DYNAMIC_DOWNCAST (CCxNDArrayItem, m_pDefaultItemRTC->CreateObject ());
	}
	else
	{
		pItem = new CCxNDArrayItem ();
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
//*********************************************************************
void CCxNDArrayGrid::SetRowHeight ()
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
void CCxNDArrayGrid::OnDraw(CDC* pDCPaint)
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
//*********************************************************************
BOOL CCxNDArrayGrid::OnDrawItem (CDC* pDC, CBCGPGridRow* pItem)
{
	//return CBCGPGridCtrl::OnDrawItem(pDC, pItem);

	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pItem);

	CCxNDArrayRow * pRow = ( CCxNDArrayRow * )pItem;

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
							CBrush& br = (m_brGroupBackground.GetSafeHandle () != NULL) ? m_brGroupBackground : m_brBackground;
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

			// ---------------
			// draw expandbox:
			// ---------------
			if (pRow->IsGroup () && (!IsSortingMode () || IsGrouping ()) &&
				!pRow->m_lstSubItems.IsEmpty ())
			{
				CRect rectExpand = rectName;
				rectName.left += m_nRowHeight + dx;
				rectExpand.right = rectName.left;
				rectExpand.DeflateRect (dx, 0, 0, 0);

				CRgn rgnClipExpand;
				CRect rectExpandClip = rectExpand;
				rectExpandClip.bottom = min (rectExpandClip.bottom, m_rectList.bottom);

				rgnClipExpand.CreateRectRgnIndirect (&rectExpandClip);
				pDC->SelectClipRgn (&rgnClipExpand);

				pRow->OnDrawExpandBox (pDC, rectExpand);

				pDC->SelectClipRgn (&m_rgnClip);
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
//****************************************************************************************
void CCxNDArrayGrid::OnDrawLineNumber (CDC* pDC, CBCGPGridRow* pRow, CRect rect, BOOL bSelected, BOOL/* bPressed*/)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (this);

	if (pRow != NULL)
	{
		ASSERT_VALID (pRow);

		//-----------
		// Draw text:
		//-----------
		COLORREF clrText = (bSelected && m_ColorData.m_HeaderSelColors.m_clrText != -1)
			? m_ColorData.m_HeaderSelColors.m_clrText
			: m_ColorData.m_HeaderColors.m_clrText;
		COLORREF clrTextOld = (COLORREF)-1;
		if (clrText != (COLORREF)-1)
		{
			clrTextOld = pDC->SetTextColor (clrText);
		}

		CRect rectLabel = rect;
		rectLabel.DeflateRect (TEXT_MARGIN, 0);

		CString strLabel;
		if (pRow->GetRowId () >= 0)
		{
			strLabel.Format (_T("%d"), pRow->GetRowId () /*+ 1*/);
		}

		UINT uiTextFlags = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
		uiTextFlags |= DT_CENTER;

		pDC->DrawText (strLabel, rectLabel, uiTextFlags);

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor (clrTextOld);
		}
	}
}

	//******************************************************************************************
BOOL CCxNDArrayGrid::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
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
void CCxNDArrayGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CCxNDArrayGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridCtrl::OnLButtonDown(nFlags, point);

	if (m_pArrayData != NULL)
	{
		if (m_pArrayData->GetRealDataType() == DT_BOOLEAN)
		{
			BOOL bEditBoolean = TRUE;
			if ( m_pArrayData->m_pNode != NULL )
			{
				if (m_pArrayData->m_pNode->m_dwStatus & CCxStructTreeNodeParam::NS_VALUE_READONLY)
					bEditBoolean = FALSE;
				else if ( m_pArrayData->m_pNode->IsOut() )
					bEditBoolean = FALSE;
			}

			CCxNDArrayItem * pItem = (CCxNDArrayItem *) GetCurSelItem();
			if (pItem == NULL) bEditBoolean = FALSE;
			else
			{
				CRect rc = pItem->GetRect();
				if ( point.x < rc.left || point.y < rc.top 
					|| point.x > rc.right || point.y > rc.bottom )
					bEditBoolean = FALSE;
			}

			if ( bEditBoolean )
			{
				CString * pStr = reinterpret_cast< CString * > ( pItem->GetData() );
				if ( pStr != NULL )
				{
					if ( (*pStr).CompareNoCase("true") == 0 )
						(*pStr) = "false";
					else
						(*pStr) = "true";

					pItem->SetValue( (LPCTSTR)(*pStr) );
					m_bModified = TRUE;
				}
			}
		}
	}

	////SelectRow(pCurRow->GetRowId(), TRUE);

	//CBCGPGridRange* pSelRange = 
	//	new CBCGPGridRange (COL_NAME, 0, COL_DIR, GetRowCount());
	//CList<CBCGPGridRange*, CBCGPGridRange*>	lstSel;
	//lstSel.AddTail (pSelRange);
	//while (!lstSel.IsEmpty ())
	//{
	//	CBCGPGridRange* pRange = lstSel.RemoveTail ();
	//	//ASSERT (pRange != NULL);

	//	SelectRange (*pRange, FALSE, TRUE);
	//	delete pRange;
	//}

	//pSelRange = 
	//	new CBCGPGridRange (COL_NAME, pCurRow->GetRowId(), COL_NAME, pCurRow->GetRowId());
	//lstSel.AddTail (pSelRange);
	//while (!lstSel.IsEmpty ())
	//{
	//	CBCGPGridRange* pRange = lstSel.RemoveTail ();
	//	//ASSERT (pRange != NULL);

	//	SelectRange (*pRange, TRUE, TRUE);
	//	delete pRange;
	//	//pCurRow->Redraw();
	//}
}
//****************************************************************************************
BOOL CCxNDArrayGrid::DoDrawText (CDC* pDC, CString strText, CRect rect, UINT uiDrawTextFlags, LPRECT lpRectClip, BOOL bNoCalcExtent)
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
//****************************************************************************************
void CCxNDArrayGrid::DrawHeaderPart (CDC* pDC, const CBCGPGridHeaderParams& params)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CRect rcCell = params.m_rect;

//	if (!visualManager->DrawHeaderPart (pDC, params))
	{
		switch (params.m_nHeaderPart)
		{
			//表头区域
		case CBCGPGridHeaderParams::HeaderTop:
			//if (params.m_nColumn != -1)
			{
				CBCGPGridColors::ColorData colorData = 
					params.IsSelected () && m_ColorData.m_HeaderSelColors.m_clrBackground != -1 ? 
						m_ColorData.m_HeaderSelColors :
						m_ColorData.m_HeaderColors;

				if (!colorData.Draw (pDC, rcCell))
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					GetColumnsInfo ().m_bInvertPressedColumn = 
						visualManager->OnDrawGridHeaderItemBorder (this, pDC, rcCell, params.IsPressed ());
				}

				//rcCell.top -= 1;
				//rcCell.left -= 1;
				//pDC->Draw3dRect (rcCell, g_crNDArrayHeaderBorder, g_crNDArrayHeaderBorder);

				{
					CPen pen (PS_SOLID, 1, g_crNDArrayHeaderBorder);
					CPen* pOldPen = pDC->SelectObject (&pen);
					//下
					pDC->MoveTo ( rcCell.left, rcCell.bottom - 1 );
					pDC->LineTo ( rcCell.right, rcCell.bottom - 1 );
					//右
					pDC->MoveTo ( rcCell.right - 1, rcCell.top );
					pDC->LineTo ( rcCell.right - 1, rcCell.bottom );
					pDC->SelectObject (pOldPen);
				}
			}
			break;
			//左侧行编号区域
		case CBCGPGridHeaderParams::HeaderLeft:
			{
				CBCGPGridColors::ColorData colorData = 
					params.IsSelected () && m_ColorData.m_HeaderSelColors.m_clrBackground != -1 ? 
						m_ColorData.m_HeaderSelColors :
						m_ColorData.m_HeaderColors;
				
				rcCell.top += 1;
				if (!colorData.Draw (pDC, rcCell))
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					GetColumnsInfo ().m_bInvertPressedColumn = 
						visualManager->OnDrawGridRowHeaderItemBorder (this, pDC, rcCell, params.IsPressed ());
				}
				{
					CPen pen (PS_SOLID, 1, g_crNDArrayHeaderBorder);
					CPen* pOldPen = pDC->SelectObject (&pen);
					//下
					pDC->MoveTo ( rcCell.left, rcCell.bottom );
					pDC->LineTo ( rcCell.right, rcCell.bottom );
					//右
					pDC->MoveTo ( rcCell.right - 1, rcCell.top );
					pDC->LineTo ( rcCell.right - 1, rcCell.bottom );
					pDC->SelectObject (pOldPen);
				}

				if (m_bLineNumbers && params.m_pRow != NULL)
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					OnDrawLineNumber (pDC, params.m_pRow, rcCell, params.IsSelected (), params.IsPressed ());
				}
			}
			break;
			//行编号矩形区域、表头矩形区域 交叉的 矩形区域。
		case CBCGPGridHeaderParams::HeaderTopLeft:
			//----------------------
			// Draw Select All area:
			//----------------------
			{
				CBCGPGridColors::ColorData colorData = 
					params.IsSelected () && m_ColorData.m_HeaderSelColors.m_clrBackground != -1 ? 
						m_ColorData.m_HeaderSelColors :
						m_ColorData.m_HeaderColors;

				if (!colorData.Draw (pDC, rcCell))
				{
					m_bHeaderItemHovered = params.IsHighlighted ();
					GetColumnsInfo ().m_bInvertPressedColumn = 
						visualManager->OnDrawGridSelectAllAreaBorder (this, pDC, rcCell, params.IsPressed ());
				}

				rcCell.top -= 1;
				rcCell.left -= 1;
				pDC->Draw3dRect (rcCell, g_crNDArrayHeaderBorder, g_crNDArrayHeaderBorder);
			}
			break;
		}
	}
}
//****************************************************************************************
void CCxNDArrayGrid::FillHeaderPartBackground (CDC* pDC, const CBCGPGridHeaderParams& params)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	
	CRect rcCell = params.m_rect;

//	if (!visualManager->FillHeaderPartBackground (pDC, params))
	{
		switch (params.m_nHeaderPart)
		{
		case CBCGPGridHeaderParams::HeaderTop:
			break;

		case CBCGPGridHeaderParams::HeaderLeft:
			//---------------------------
			// Row header item background
			//---------------------------
			rcCell.top += 1;
			if (!m_ColorData.m_HeaderColors.Draw (pDC, rcCell))
			{
				visualManager->OnFillGridRowHeaderBackground (this, pDC, rcCell);
			}

			break;

		case CBCGPGridHeaderParams::HeaderTopLeft:
			if (!m_ColorData.m_HeaderColors.Draw (pDC, rcCell))
			{
				visualManager->OnFillGridSelectAllAreaBackground (this, pDC, rcCell, params.IsPressed ());
			}
			break;
		}
	}
}
