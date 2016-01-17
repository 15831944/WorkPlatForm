#include "StdAfx.h"
#include "CxNDArrayDimGrid.h"
#include <CxBCGPVisualManager2007.h>
#include "ConvertData.h"
#include "CxBCGPSpinButtonCtrl.h"
#include <CStringChecker.h>

static const LPCTSTR arrDimColCaption[] = {
	"维度",
	"分类",
	"位置"
};

static const int arrDimColWidth[] = {
	48,
	56,
	47
};

//控制行高
#define TEXT_VMARGIN	(2+2)

#define STRETCH_DELTA	2

#define	BCGPGRIDCTRL_ID_INPLACE	1

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

BEGIN_MESSAGE_MAP(CCxNDArrayDimEditT, CEdit)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCxNDArrayDimEditT, CEdit)

CCxNDArrayDimEditT::CCxNDArrayDimEditT()
{
	m_pItem = NULL;
}

CCxNDArrayDimEditT::~CCxNDArrayDimEditT()
{
}

BOOL CCxNDArrayDimEditT::PreTranslateMessage(MSG* pMsg)
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

		if ( m_pItem != NULL )
		{
			CString strValue, strNewChar;
			if ( pMsg->wParam >= 48 && pMsg->wParam <= 48 + 9 )
			{
				strNewChar.Format( "%d", pMsg->wParam - 48 );
			}
			else if ( pMsg->wParam >= 96 && pMsg->wParam <= 96 + 9 )
			{
				strNewChar.Format( "%d", pMsg->wParam - 96 );
			}

			if ( !strNewChar.IsEmpty() )
			{
				//strValue = Variant2CString( m_pItem->GetValue() );
				GetWindowText( strValue );

				int nStart, nEnd;
				nStart = nEnd = -1;
				GetSel(nStart, nEnd);
				if ( nEnd == nStart )
				{
					//插入字符
					strValue.Insert( nStart, strNewChar );
				}
				else if ( nEnd - nStart > 0 )
				{
					//有选中字符
					strValue.Delete( nStart, nEnd - nStart );
					strValue.Insert( nStart, strNewChar );
				}

				if ( !CStringChecker::CheckInt( strValue ) )
				{
					return TRUE;
				}
				int nValue = strtol( (char*)(LPCTSTR)strValue, NULL, 10 );
				if ( m_pItem->m_nMaxValue < nValue || nValue < m_pItem->m_nMinValue )
				{
					return TRUE;
				}
			}
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CCxNDArrayDimEditT::IsSelectedAll( void )
{
	int nStart, nEnd, nLen;
	GetSel(nStart, nEnd);
	CString str;
	GetWindowText( str );
	nLen = str.GetLength();
	if ( nLen == nEnd - nStart && nLen != 0 ) return TRUE;
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCxNDArrayDimGrid, CBCGPGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CCxNDArrayDimItem, CBCGPGridItem, VERSIONABLE_SCHEMA | 1)

CCxNDArrayDimItem::CCxNDArrayDimItem ()
{
}
//*******************************************************************************************
CCxNDArrayDimItem::CCxNDArrayDimItem(const _variant_t& varValue, DWORD_PTR dwData,
		LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
		LPCTSTR lpszValidChars) : CBCGPGridItem(varValue, dwData, lpszEditMask, lpszEditTemplate
		, lpszValidChars)
{
}
//*******************************************************************************************
CCxNDArrayDimItem::~CCxNDArrayDimItem()
{
}
//******************************************************************************************
COLORREF CCxNDArrayDimItem::OnFillBackground (CDC* pDC, CRect rect)
{
	return CBCGPGridItem::OnFillBackground(pDC, rect);
}
//*****************************************************************************
CEdit* CCxNDArrayDimItem::NewInPlaceEdit ()
{
	CCxNDArrayDimEditT* pEdit = new CCxNDArrayDimEditT;
	pEdit->m_pItem = this;
	return pEdit;
}
//*****************************************************************************
BOOL CCxNDArrayDimItem::OnEdit (LPPOINT)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	if (!m_bAllowEdit)
	{
		return FALSE;
	}

	CCxNDArrayDimGrid* pWndList = (CCxNDArrayDimGrid*)((CCxNDArrayDimRow*)m_pGridRow)->m_pWndList;
	ASSERT_VALID (pWndList);

	m_pWndInPlace = NULL;

	OnBeginInplaceEdit ();

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect (rectEdit, rectSpin);

	BOOL bDefaultFormat = FALSE;
	m_pWndInPlace = CreateInPlaceEdit (rectEdit, bDefaultFormat);

	pWndList->OnAfterInplaceEditCreated (this, m_pWndInPlace);

	if (m_pWndInPlace != NULL)
	{
		//通过隐藏CEdit控件，实现Combo单元格只读效果
		if ( m_nIdColumn == CCxNDArrayDimGrid::COL_DIM_TYPE )
		{
			m_pWndInPlace->ShowWindow(SW_HIDE);
		}

		if (bDefaultFormat)
		{
			m_pWndInPlace->SetWindowText (FormatItem ());
		}

		if (m_dwFlags & BCGP_GRID_ITEM_HAS_LIST)
		{
			CRect rectCombo = m_Rect;
			rectCombo.left = rectEdit.left - 4;

			m_pWndCombo = CreateCombo (pWndList, rectCombo);
			ASSERT_VALID (m_pWndCombo);

			SetComboFont ();

			//-------------------------------------------------------------------
			// Synchronize bottom edge of the combobox with the item bottom edge:
			//-------------------------------------------------------------------
			m_pWndCombo->GetWindowRect (rectCombo);
			pWndList->ScreenToClient (&rectCombo);

			int dy = rectCombo.Height () - m_Rect.Height ();

			m_pWndCombo->SetWindowPos (NULL, rectCombo.left,
				rectCombo.top - dy + 1, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			if (m_varValue.vt == VT_BOOL)
			{
				m_lstOptions.AddTail (pWndList->m_strTrue);
				m_lstOptions.AddTail (pWndList->m_strFalse);
			}

			for (POSITION pos = m_lstOptions.GetHeadPosition (); pos != NULL;)
			{
				m_pWndCombo->AddString (m_lstOptions.GetNext (pos));
			}
		}

		if (m_dwFlags & BCGP_GRID_ITEM_HAS_SPIN)
		{
			m_pWndSpin = CreateSpinControl (rectSpin);
		}

		SetInPlaceEditFont ();
		m_pWndInPlace->SetFocus ();

		if ( !m_bAllowEdit )
		{
			m_pWndInPlace->HideCaret ();
		}

		m_bInPlaceEdit = TRUE;
		return TRUE;
	}

	return FALSE;
}
//*****************************************************************************
CSpinButtonCtrl* CCxNDArrayDimItem::CreateSpinControl (CRect rectSpin)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	CCxNDArrayDimGrid* pWndList = (CCxNDArrayDimGrid*)((CCxNDArrayDimRow*)m_pGridRow)->m_pWndList;
	ASSERT_VALID (pWndList);

	CSpinButtonCtrl* pWndSpin = new CCxBCGPSpinButtonCtrl;

	if (!pWndSpin->Create (
		WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS,
		rectSpin, pWndList, BCGPGRIDCTRL_ID_INPLACE))
	{
		return NULL;
	}

	pWndSpin->SetBuddy (m_pWndInPlace);

	//if (m_nMinValue != 0 || m_nMaxValue != 0)
	{
		pWndSpin->SetRange32 (m_nMinValue, m_nMaxValue);
	}

	return pWndSpin;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CCxNDArrayDimRow, CBCGPGridRow, VERSIONABLE_SCHEMA | 1)

CCxNDArrayDimRow::CCxNDArrayDimRow (const CString& strGroupName
	, DWORD_PTR dwData, int nBlockSize)
	: CBCGPGridRow( strGroupName, dwData, nBlockSize )
{
}
//******************************************************************************************
CCxNDArrayDimRow::CCxNDArrayDimRow (int nColumnsNum, DWORD_PTR dwData, int nBlockSize)
	: CBCGPGridRow( nColumnsNum, dwData, nBlockSize )
{
}
//******************************************************************************************
CBCGPGridItem* CCxNDArrayDimRow::CreateItem (int nRow, int nColumn)
{
	if (m_pWndList == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}
	ASSERT_VALID (m_pWndList);
	CCxNDArrayDimGrid * pGrid = ( CCxNDArrayDimGrid * )m_pWndList;
	return pGrid->CreateItem (nRow, nColumn);
}
//******************************************************************************************
void CCxNDArrayDimRow::OnDrawItems (CDC* pDC, CRect rect)
{
	CBCGPGridRow::OnDrawItems (pDC, rect); return;
	
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	CCxNDArrayDimGrid * pGrid = ( CCxNDArrayDimGrid * ) m_pWndList;
	
	rect.NormalizeRect ();
	const int nFreezeOffset = pGrid->m_rectList.left + pGrid->GetColumnsInfo ().GetFreezeOffset ();

	for (int i = 0; i < m_arrRowItems.GetSize (); i++)
	{
		CCxNDArrayDimItem* pItem = (CCxNDArrayDimItem*)m_arrRowItems [i];
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

			if (!pItem->m_rectButton.IsRectEmpty ())
			{
				pItem->OnDrawButton (pDC, pItem->m_rectButton);
			}

			pDC->SelectClipRgn (&pGrid->m_rgnClipRow);
			pGrid->m_rgnClipItem.DeleteObject ();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCxNDArrayDimGrid, CBCGPGridCtrl)

	//******************************************************************************************
BOOL CCxNDArrayDimGrid::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
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
void CCxNDArrayDimGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

CCxNDArrayDimGrid::CCxNDArrayDimGrid()
{
	m_pArrayData = NULL;
	m_bModified = FALSE;
}
//*********************************************************************
CCxNDArrayDimGrid::CCxNDArrayDimGrid( int nMemBlockSize )
	: CBCGPGridCtrl(nMemBlockSize)
{
}
//*********************************************************************
CCxNDArrayDimGrid::~CCxNDArrayDimGrid()
{
}
//*********************************************************************
BOOL CCxNDArrayDimGrid::Init( CRect& rc, CWnd * pParent )
{
	if ( IsWindow(m_hWnd) ) return FALSE;

	Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rc, pParent, ID_NAE_ARRAY_DIM);

	EnableColumnAutoSize (FALSE);
	SetSingleSel (TRUE);
	EnableGroupByBox (FALSE);
	SetReadOnly (FALSE);
	SetWholeRowSel (FALSE);
	EnableHeader (TRUE, 0);
	m_nBaseHeight = 20;

	// Set grid tab order (first):
	SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
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

	colors.m_LeftOffsetColors.m_clrBackground = g_crStructTreeClientBg;	// Left offset colors

	//colors.m_clrHorzLine = g_crStructTreeClientBg;		// Grid horizontal line text
	//colors.m_clrVertLine = g_crStructTreeClientBg;		// Grid vertical line text

	//colors.m_clrPreviewText;	// Description text color

	SetColorTheme (colors);

	//SetSelectionBorderForActiveItem(FALSE);
	//SetSelectionBorder(FALSE);


	//////////////////////////////////////////////////////////////////////////

	CString str;
	int nColCnt = 0;
	int nRowCnt = 0;
	CCxNDArrayDimItem* pItem = NULL;
	CCxNDArrayDimRow * pRow = NULL;

	nColCnt = sizeof( arrDimColCaption ) / sizeof( LPCTSTR );
	for (int i=0; i<nColCnt; i++)
	{
		InsertColumn (i, arrDimColCaption[i], arrDimColWidth[i]);
	}
	if ( m_pArrayData != NULL )
	{
		nRowCnt = m_pArrayData->GetDimCount();
		for ( int i=0; i<nRowCnt; i++ )
		{
			//if ( nRowCnt <= 2 )
			//{
			//	break;
			//}

			pRow = CreateNewRow ();
			pRow->AllowSubItems (FALSE);
			for (int col = 0; col < nColCnt; col++)
			{
				switch (col)
				{
				case COL_DIM_INDEX:
				case COL_DIM_TYPE:
				case COL_DIM_VALUE:
					pRow->GetItem(col)->AllowEdit(FALSE);
					break;
				default:
					break;
				}
			}

			str.Format("%d", i);
			pRow->GetItem (COL_DIM_INDEX)->SetValue( (LPCTSTR)str );

			CString strSel;
			switch (i)
			{
			case 0:
				strSel = "Row"; break;
			case 1:
				strSel = "Column"; break;
			//case 2:
			//	strSel = "Column"; break;
			default:
				strSel = "Index"; break;
			}

			pItem = new CCxNDArrayDimItem ( (LPCTSTR)strSel );
			if ( nRowCnt > 0 ) 
				pItem->AddOption ( "Row", 1 );
			if ( nRowCnt > 1 )
				pItem->AddOption ( "Column", 1 );
			if ( nRowCnt > 2 )
				pItem->AddOption ( "Index", 1 );
			pRow->ReplaceItem (COL_DIM_TYPE, pItem);

			if ( nRowCnt <= 2 ) 
			{
				pRow->GetItem( COL_DIM_TYPE )->Enable(FALSE);
				pRow->GetItem( COL_DIM_TYPE )->AllowEdit(FALSE);
			}

			pItem = new CCxNDArrayDimItem ( 0 ); 
			pItem->EnableSpinControl(TRUE, 0, m_pArrayData->GetDataCount( nRowCnt-i-1 ) - 1 );
			pRow->ReplaceItem (COL_DIM_VALUE, pItem);

			switch (i)
			{
			case 2: //row col
			case 1:
				pRow->GetItem( COL_DIM_VALUE )->Enable( FALSE ); break;
			default:
				break;
			}

			AddRow (pRow, FALSE);
		}

		//if ( nRowCnt == 2 )
		//{
		//	GetRow(0)->GetItem(COL_DIM_TYPE)->Enable(FALSE);
		//	GetRow(0)->GetItem(COL_DIM_VALUE)->Enable(FALSE);
		//}

		//if ( nRowCnt == 1 )
		//{
		//	GetRow(0)->GetItem(COL_DIM_TYPE)->Enable(FALSE);
		//	GetRow(0)->GetItem(COL_DIM_VALUE)->Enable(FALSE);
		//	GetRow(1)->GetItem(COL_DIM_TYPE)->Enable(FALSE);
		//	GetRow(1)->GetItem(COL_DIM_VALUE)->Enable(FALSE);
		//	GetRow(2)->GetItem(COL_DIM_TYPE)->Enable(FALSE);
		//	GetRow(2)->GetItem(COL_DIM_VALUE)->Enable(FALSE);
		//}
	}
	m_nButtonWidth = 20;
	AdjustLayout();
	return TRUE;
}
//*********************************************************************
CBCGPGridRow * CCxNDArrayDimGrid::CreateRow ()
{
	return new CCxNDArrayDimRow (0, 0, GetRowBlockSize ());
}
//*********************************************************************
CCxNDArrayDimRow * CCxNDArrayDimGrid::CreateNewRow()
{
	CCxNDArrayDimRow* pRow = (CCxNDArrayDimRow*)CBCGPGridCtrl::CreateRow ( GetColumnCount () );

	for (int i = 0; i < GetColumnCount ();i++)
	{
		pRow->GetItem (i)->AllowEdit ( !m_bReadOnly );
	}

	return ( CCxNDArrayDimRow * )pRow;
}
//*********************************************************************
CBCGPGridItem* CCxNDArrayDimGrid::CreateItem (int nRow, int nColumn)
{
	CCxNDArrayDimItem* pItem = NULL;

	CRuntimeClass* pItemRTC = NULL;
	if (m_mapItemsRTC.Lookup (nColumn, pItemRTC))
	{
		ASSERT (pItemRTC != NULL);

		pItem = DYNAMIC_DOWNCAST (CCxNDArrayDimItem, pItemRTC->CreateObject ());
	}
	else if (m_pDefaultItemRTC != NULL)
	{
		pItem = DYNAMIC_DOWNCAST (CCxNDArrayDimItem, m_pDefaultItemRTC->CreateObject ());
	}
	else
	{
		pItem = new CCxNDArrayDimItem ();
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
void CCxNDArrayDimGrid::SetRowHeight ()
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
void CCxNDArrayDimGrid::OnDraw(CDC* pDCPaint)
{
	return CBCGPGridCtrl::OnDraw(pDCPaint);
}
//*********************************************************************
BOOL CCxNDArrayDimGrid::OnDrawItem (CDC* pDC, CBCGPGridRow* pItem)
{
	return CBCGPGridCtrl::OnDrawItem(pDC, pItem);
}

void CCxNDArrayDimGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CBCGPGridCtrl::OnLButtonDown(nFlags, point);

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
	CCxNDArrayDimRow* pHitHeaderRow = (CCxNDArrayDimRow*)HitTestRowHeader (point, rectRowHeader);
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
	CBCGPGridRow* pHitRow = HitTest (point, id, pHitItem, &clickArea);

	BOOL bSelChanged = id != m_idLastSel;
	BOOL bIsButtonClick = pHitItem != NULL && ((CCxNDArrayDimItem*)pHitItem)->m_rectButton.PtInRect (point);

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

	SetCurSel (id, dwSelMode);

	m_pSetSelItem = NULL;
	m_bNoUpdateWindow = FALSE;

	if (id.IsNull () || pHitRow == NULL)
	{
		return;
	}

	ASSERT_VALID (pHitRow);
	EnsureVisible (pHitRow);

	CCxNDArrayDimRow* pCurSel = (CCxNDArrayDimRow*)GetCurSel ();
	CCxNDArrayDimItem* pItem = ( CCxNDArrayDimItem* )GetCurSelItem (pCurSel);
	if (id != m_idActive || pCurSel == NULL || (pItem == NULL && clickArea == CBCGPGridRow::ClickValue))
	{
		// The hitten item is not active - do not translate a click to the grid item.
		// Translate a click for single item only.
		return;
	}

	// ------------------------------
	// Translate a click to the item:
	// ------------------------------
	ASSERT_VALID (pCurSel); // pCurSel - hitten row

	switch (clickArea)
	{
	case CBCGPGridRow::ClickExpandBox:
		pCurSel->Expand (!pCurSel->IsExpanded ());
		break;

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
				DoClickValue (pItem, WM_LBUTTONDOWN, point, bSelChanged, bIsButtonClick);
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
}
//******************************************************************************************
BOOL CCxNDArrayDimGrid::EditItem (CBCGPGridRow* pItem, LPPOINT lptClick)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pItem);

	ClearEndEditReason ();

	if (!EndEditItem ())
	{
		return FALSE;
	}

	CCxNDArrayDimRow * pCxRow = (CCxNDArrayDimRow*)pItem;

	if (pCxRow->IsGroup () && !pCxRow->HasValueField ())
	{
		return FALSE;
	}

	ASSERT (pCxRow == GetCurSel ());

	CCxNDArrayDimItem * pCxItem = (CCxNDArrayDimItem *)pCxRow->GetItem(COL_DIM_TYPE);
	if ( pCxItem == NULL ) return FALSE;

	CString str;
	ConvertBSTR2CString(str, pCxItem->GetValue().bstrVal);
	if ( str.CompareNoCase("Row")==0 || str.CompareNoCase("Column")==0 )
	{
		if ( GetCurSelItemID().m_nColumn == COL_DIM_VALUE )
		{
			return FALSE;
		}
	}

	if (pCxRow->OnEdit (lptClick))
	{
		pCxRow->Redraw ();
		//SetCurSel (m_idActive, SM_SET_ACTIVE_ITEM);
		SetCapture ();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
//****************************************************************************************
void CCxNDArrayDimGrid::DrawHeaderPart (CDC* pDC, const CBCGPGridHeaderParams& params)
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
			}
			break;
		}
	}
}
//****************************************************************************************
void CCxNDArrayDimGrid::FillHeaderPartBackground (CDC* pDC, const CBCGPGridHeaderParams& params)
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