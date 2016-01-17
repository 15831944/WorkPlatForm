#include "StdAfx.h"
#include "ZGridCtrl.h"
#include "UtilityFunc.h"
#include "PtrVectorHeader.h"
#include "CxBCGPVisualManager2007.h"
#include "ResLibImageSet.h"
#include "DlgIf.h"
#include "CxStructTreeNodeArrayElement.h"
#include "CxStructTreeNodeParam.h"


//控制行高
#define TEXT_VMARGIN	(2+2)

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

IMPLEMENT_SERIAL(CZGridItem, CBCGPGridItem, VERSIONABLE_SCHEMA | 1)

CZGridItem::CZGridItem ()
{
}
//****************************************************************************************
CZGridItem::CZGridItem(const _variant_t& varValue, DWORD_PTR dwData,
		LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
		LPCTSTR lpszValidChars) : CBCGPGridItem(varValue, dwData, lpszEditMask, lpszEditTemplate
		, lpszValidChars)
{
}

CZGridItem::~CZGridItem()
{

}
//*******************************************************************************************
CString CZGridItem::GetValueTooltip ()
{
	return __super::GetValueTooltip();

}

//****************************************************************************************
BOOL CZGridItem::OnClickValue (UINT uiMsg, CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	CZGridCtrl* pWndList = (CZGridCtrl*)((CZGridRow*)m_pGridRow)->m_pWndList;
	ASSERT_VALID (pWndList);

	if (m_pWndInPlace == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (m_pWndInPlace);

	if (m_pWndSpin != NULL)
	{
		ASSERT_VALID (m_pWndSpin);
		ASSERT (m_pWndSpin->GetSafeHwnd () != NULL);

		CRect rectSpin;
		m_pWndSpin->GetClientRect (rectSpin);
		m_pWndSpin->MapWindowPoints (pWndList, rectSpin);

		if (rectSpin.PtInRect (point))
		{
			pWndList->MapWindowPoints (m_pWndSpin, &point, 1); 

			m_pWndSpin->SendMessage (uiMsg, 0, MAKELPARAM (point.x, point.y));
			return TRUE;
		}
	}

	CPoint ptEdit = point;
	::MapWindowPoints (	pWndList->GetSafeHwnd (), 
						m_pWndInPlace->GetSafeHwnd (), &ptEdit, 1);

	//m_pWndInPlace->SendMessage (uiMsg, 0, MAKELPARAM (ptEdit.x, ptEdit.y));
	((CEdit*)m_pWndInPlace)->SetSel(0, -1);
	return TRUE;
}

//*****************************************************************************
BOOL CZGridItem::OnEdit (LPPOINT)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pGridRow);

	if (!m_bAllowEdit)
	{
		return FALSE;
	}
	CZGridCtrl* pWndList = (CZGridCtrl*)((CZGridRow*)m_pGridRow)->m_pWndList;
	ASSERT_VALID (pWndList);

	m_pWndInPlace = NULL;

	OnBeginInplaceEdit ();

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect (rectEdit, rectSpin);

	BOOL bDefaultFormat = FALSE;
	m_pWndInPlace = CreateInPlaceEdit (rectEdit, bDefaultFormat);
	((CEdit*)m_pWndInPlace)->SetLimitText(50);

	pWndList->OnAfterInplaceEditCreated (this, m_pWndInPlace);

	if (m_pWndInPlace != NULL)
	{
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
		//((CEdit*)m_pWndInPlace)->SetSel(0, -1);



		if (!m_bAllowEdit)
		{
			m_pWndInPlace->HideCaret ();
		}

		m_bInPlaceEdit = TRUE;
		return TRUE;
	}

	return FALSE;
}

//******************************************************************************************
COLORREF CZGridItem::OnFillBackground (CDC* pDC, CRect rect)
{
	CZGridRow * pRow = (CZGridRow *)m_pGridRow;
	CZGridCtrl* pWndList = (CZGridCtrl*)pRow->m_pWndList;
	ASSERT_VALID (pWndList);

	COLORREF clrText = m_clrText != (COLORREF)-1 ? m_clrText : pWndList->m_ColorData.m_clrText;

	CZGridItem* pCurSelItem = (CZGridItem*)pWndList->GetCurSelItem ();

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



//////////////////////////////////////////////////////////////////////////


IMPLEMENT_SERIAL(CZGridRow, CBCGPGridRow, VERSIONABLE_SCHEMA | 1)

CZGridRow::CZGridRow (const CString& strGroupName
	, DWORD_PTR dwData, int nBlockSize)
{
	CBCGPGridRow::CBCGPGridRow( strGroupName, dwData, nBlockSize );
	m_bExpanded = FALSE;
}
//******************************************************************************************
CZGridRow::CZGridRow (int nColumnsNum, DWORD_PTR dwData, int nBlockSize)
{
	CBCGPGridRow::CBCGPGridRow( nColumnsNum, dwData, nBlockSize );
	m_bExpanded = FALSE;
}

//******************************************************************************************
void CZGridRow::OnDrawItems (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	CZGridCtrl * pList = ( CZGridCtrl * ) m_pWndList;
	CCxStructTreeNode * pNode = reinterpret_cast<CCxStructTreeNode *> ( GetData() );

	rect.NormalizeRect ();
	const int nFreezeOffset = pList->m_rectList.left + pList->GetColumnsInfo ().GetFreezeOffset ();

	for (int i = 0; i < m_arrRowItems.GetSize (); i++)
	{
		CZGridItem* pItem = (CZGridItem*)m_arrRowItems [i];
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

BOOL CZGridRow::AddSubItem( CBCGPGridRow* pItem, int nPos /*= -1*/, BOOL bRedraw /*= TRUE*/ )
{
	CCxStructTreeNode * pNode = reinterpret_cast< CCxStructTreeNode * > ( GetData() );
	if ( pNode == NULL ) return FALSE;

	if ( nPos == -1 ) return CBCGPGridRow::AddSubItem( pItem, bRedraw );

	CZGridRow * pRow = ( CZGridRow * )pItem;

	ASSERT_VALID (this);
	ASSERT_VALID (pRow);

	if (!IsGroup ())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pRow->m_pParent = this;

	int nPosParent = m_nIdRow;
	//int nSubItemsCount = 0;

	//for (int i=0; i<nPos; i++)
	//{
	//	CZGridRow * pChildRow = GetNodeRow( pNode->m_vChildren[i]->m_strID );
	//	if ( pChildRow == NULL ) return FALSE;
	//	nSubItemsCount ++;
	//	nSubItemsCount += pChildRow->GetSubItemsCount(TRUE);
	//}

	//int nSubItemsCount = GetSubItemsCount (TRUE);
	int nPosInsertAfter = nPosParent + nPos;
	m_pWndList->InsertRowAfter (nPosInsertAfter, pRow, bRedraw);

	m_lstSubItems.AddTail (pRow);
	pRow->m_pWndList = m_pWndList;

	return TRUE;
}

void CZGridRow::OnDrawExpandBox (CDC* pDC, CRect rect)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (m_pWndList);
	ASSERT (IsGroup ());

	BOOL bIsOpened = m_bExpanded && !m_lstSubItems.IsEmpty ();

	CZGridCtrl * pList = ( CZGridCtrl * ) m_pWndList;
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
CBCGPGridItem* CZGridRow::CreateItem (int nRow, int nColumn)
{
	if (m_pWndList == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	ASSERT_VALID (m_pWndList);
	return m_pWndList->CreateItem (nRow, nColumn);
}

BEGIN_MESSAGE_MAP(CZGridCtrl, CBCGPGridCtrl)
	ON_WM_LBUTTONDBLCLK()
	//ON_WM_CHAR()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CZGridCtrl, CBCGPGridCtrl)

CZGridCtrl::CZGridCtrl(void)
{		
	m_IsShowRoot = FALSE;
}


CZGridCtrl::~CZGridCtrl(void)
{
	RemoveAll();
}

//******************************************************************************************
void CZGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnChar(nChar, nRepCnt, nFlags);

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting || nChar == 3)
	{
		return;
	}

	if (!CanBeginInplaceEditOnChar (nChar, nRepCnt, nFlags))
	{
		return;
	}

	CBCGPGridRow* pSel = GetCurSel ();

	if (pSel == NULL || !pSel->IsEnabled())
	{
		return;
	}

	ASSERT_VALID (pSel);
	EnsureVisible (pSel, TRUE);

	SetBeginEditReason (BeginEdit_Char);
	if (!EditItem (pSel))
	{
		return;
	}

	DoInplaceEditSetSel (OnInplaceEditSetSel (GetCurSelItem (pSel), BeginEdit_Char));

	pSel->PushChar (nChar);
}

BOOL CZGridCtrl::InitTree( CRect& rc, CWnd * pParent )
{
	LPCTSTR arrColName[] = {
		"名称"
		, "值"
		, "类别"
		, "方向"
		, "单位"
	};

	int arrColWidth[] = {
		200
		, 100
		, 96
		, 64
		, 64
	};

	if ( IsWindow(m_hWnd) ) return FALSE;

	Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP|WS_GROUP /*| WS_BORDER*/, rc, pParent, (UINT)-1);

	EnableColumnAutoSize (TRUE);
	//SetSingleSel (TRUE);
	EnableGroupByBox (FALSE);
	//SetReadOnly (FALSE);
	SetWholeRowSel (FALSE);
	EnableInvertSelOnCtrl(TRUE);
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

	colors.m_LeftOffsetColors.m_clrBackground = globalData.clrWindow;;	// Left offset colors

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
		//InsertColumn (i, arrColName[i], arrColWidth[i]);
		//单元格中的数据对齐方式
		switch (i)
		{
		case COL_NAME:
			InsertColumn (i, arrColName[i], rc.Width() - 20);
			SetColumnAlign (COL_NAME, HDF_LEFT); 
			break;
		default:
			InsertColumn (i, arrColName[i], arrColWidth[i]);
			SetColumnAlign (COL_NAME, HDF_LEFT);
			break;
		}
	}
	SetColumnVisible(COL_VALUE, FALSE);
	SetColumnVisible(COL_TYPE, FALSE);
	SetColumnVisible(COL_DIR, FALSE);
	SetColumnVisible(COL_UNITS, FALSE);

	return TRUE;
}

BOOL CZGridCtrl::LoadImage()
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
	m_Images.Add (&bmp, RGB (255, 255, 255));
	SetImageList (&m_Images);

	return TRUE;
}

void CZGridCtrl::ClearImage( void )
{
	for (int i = m_Images.GetImageCount() -1; i>=0; i--)
	{
		m_Images.Remove(i);
	}
	DeleteObject(m_hbmp); m_hbmp = NULL;
	PtrVectorClear(m_vImg);
}

void CZGridCtrl::SetRowImage( CCxStructTreeNode * pNode , CZGridRow * pRow )
{
	switch ( pNode->m_NodeType )
	{
	case CCxStructTreeNode::NT_ROOT:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_ROOT, FALSE );
		break;
	case CCxStructTreeNode::NT_PARALLEL:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_PARALLEL, FALSE );
		break;
	case CCxStructTreeNode::NT_SEQUENCE:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_SEQUENCE, FALSE );
		break;
	case CCxStructTreeNode::NT_SUBFLOW:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_SUBFLOW, FALSE );
		break;
	case CCxStructTreeNode::NT_IF:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_IF, FALSE );
		break;
	case CCxStructTreeNode::NT_FOR:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_FOR, FALSE );
		break;
	case CCxStructTreeNode::NT_FRONT_FORM:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_FRONTFORM, FALSE );
		break;
	case CCxStructTreeNode::NT_REAR_FORM:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_REARFORM, FALSE );
		break;
	case CCxStructTreeNode::NT_COMPONENT:
		SetComponentImage( (CCxStructTreeNodeComponent*)pNode, pRow);
		break;
	case CCxStructTreeNode::NT_GROUP:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_GROUP, FALSE );
		break;
	case CCxStructTreeNode::NT_PARAM:
		SetParamImage( (CCxStructTreeNodeParam*)pNode, pRow);
		break;
	default:
		return;
	}
}

BOOL CZGridCtrl::UpdateRowByNode( CZGridRow * pRow, CCxStructTreeNode * pNode )
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
		//pRow->GetItem (COL_NAME)->AllowEdit ( TRUE );
		//pNode->m_pRow = (CCxStructTreeRowT*)pRow;
		SetRowImage( pNode, pRow );
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
			else
			{
				pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)pNodePrarm->m_strValue, FALSE );
				if ( pNodePrarm->m_nDir == PARAM_DIR_IN || pNodePrarm->m_nDir == PARAM_DIR_INOUT )
					pRow->GetItem (COL_VALUE)->AllowEdit ( TRUE );
			}

			pRow->GetItem (COL_TYPE)->SetValue ( (LPCTSTR) ConvertDataTypeName( pNodePrarm->m_nDataType ), FALSE );
			pRow->GetItem (COL_DIR)->SetValue ( (LPCTSTR) ConvertParamDirName( pNodePrarm->m_nDir ), FALSE );
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

BOOL CZGridCtrl::AddNode( CCxStructTreeNode * pSpecNode, CZGridRow * pSpecParentRow
	, int nIndex, BOOL bRecursion )
{
	if ( pSpecNode == NULL )
	{
		return FALSE;
	}

	CZGridRow * pRow = NULL;

	try
	{
		if ( pSpecParentRow == NULL )
		{
			if (m_IsShowRoot && pSpecNode->m_NodeType != CCxStructTreeNode::NT_ROOT ) return FALSE;
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
			if ( !m_IsShowRoot && pSpecNode->m_NodeType != CCxStructTreeNode::NT_PARAM && pSpecNode->m_NodeType != CCxStructTreeNode::NT_PARAM_ARRAY && pSpecNode->m_NodeType != CCxStructTreeNode::NT_GROUP) return FALSE;
			pRow = CreateNewRow ();
			if ( pRow == NULL ) return FALSE;

			if ( !UpdateRowByNode( pRow, pSpecNode ) )
			{
				return FALSE;
			}

			pSpecParentRow->AddSubItem( pRow, nIndex, FALSE );
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
	RebuildIndexes();

	//CCxStructTreeRowT * p =  GetNodeRow( pNode->m_strID );

	//	UpdateParamValueBg();
	return TRUE;
}

//分支不做为子流程，意味着分支节点没有参数，表达式设置时可以看到其所属黑盒的参数20140321需求明确要求
//使用此方法，在向下遍历时，到子流程节点即停止
BOOL CZGridCtrl::AddNodeSubFlow( CCxStructTreeNode * pSpecNode, CZGridRow * pSpecParentRow
	, int nIndex, BOOL bRecursion )
{
	if ( pSpecNode == NULL )
	{
		return FALSE;
	}

	CZGridRow * pRow = NULL;

	try
	{
		if ( pSpecParentRow == NULL )
		{
			if (pSpecNode->m_NodeType != CCxStructTreeNode::NT_ROOT && pSpecNode->m_NodeType != CCxStructTreeNode::NT_SUBFLOW) return FALSE;
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
			pRow = CreateNewRow ();
			if ( pRow == NULL ) return FALSE;

			if ( !UpdateRowByNode( pRow, pSpecNode ) )
			{
				return FALSE;
			}

			pSpecParentRow->AddSubItem( pRow, nIndex, FALSE );
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
			if (pSpecNode->m_NodeType == CCxStructTreeNode::NT_SUBFLOW
				&& pSpecNode->m_vChildren[i]->m_NodeType != CCxStructTreeNode::NT_GROUP
				&& pSpecParentRow != NULL)
			{
				continue;
			}
			if ( !AddNodeSubFlow( pSpecNode->m_vChildren[i], pRow, -1, bRecursion ) )
			{
				return FALSE;
			}
		}
	}
	RebuildIndexes();

	//CCxStructTreeRowT * p =  GetNodeRow( pNode->m_strID );

	//	UpdateParamValueBg();
	return TRUE;
}

CZGridRow * CZGridCtrl::GetNodeRow( LPCTSTR lpszId )
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if (strId.IsEmpty()) return ( CZGridRow * )GetRow(0);

	CBCGPGridRow * pRow = NULL;
	CCxStructTreeNode * pNode = NULL;
	for (int i=0; i<GetRowCount(); i++)
	{
		pRow = GetRow(i);
		if ( pRow == NULL ) return NULL;
		pNode = reinterpret_cast< CCxStructTreeNode * >( pRow->GetData() );
		if ( pNode == NULL ) continue;
		if (pNode->m_strID == strId) return (CZGridRow *)pRow;
	}
	return NULL;
}

CBCGPGridRow * CZGridCtrl::CreateRow ()
{
	return new CZGridRow (0, 0, GetRowBlockSize ());
}

CZGridRow * CZGridCtrl::CreateNewRow()
{
	CBCGPGridRow* pRow = CBCGPGridCtrl::CreateRow ( GetColumnCount () );

	for (int i = 0; i < GetColumnCount ();i++)
	{
		pRow->GetItem (i)->AllowEdit (FALSE);
	}

	return ( CZGridRow * )pRow;
}

void CZGridCtrl::SetComponentImage( CCxStructTreeNodeComponent * pNode , CZGridRow * pRow )
{
	CCxStaticRes * pResImage = GetRobotImageData( pNode->m_strRobotType, 16 );
	if ( pResImage == NULL ) return;

	switch ( pResImage->m_nCode )
	{
	case CCxStaticRes::RT_SCRIPT_DRIVER:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_SCRIPT_DRIVER ); break;
	case CCxStaticRes::RT_DATA_PARSER:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_DATA_PARSER ); break;
	case CCxStaticRes::RT_EXCEL:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_EXCEL ); break;
	case CCxStaticRes::RT_TOOL_INTEGRATION:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_TOOL_INTEGRATION ); break;
	case CCxStaticRes::RT_WORD:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_WORD ); break;
	case CCxStaticRes::RT_FORMULA:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_FORMULA ); break;
	case CCxStaticRes::RT_FORM:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_FORM ); break;
	case CCxStaticRes::RT_DOCUMENT:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_DOCUMENT ); break;
	case CCxStaticRes::RT_PATRAN:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_PATRAN ); break;
	case CCxStaticRes::RT_WORKBENCH:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_WORKBENCH ); break;
	case CCxStaticRes::RT_CATIA:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_CATIA ); break;
	case CCxStaticRes::RT_SCRIPT:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_SCRIPT ); break;

#ifdef PROJECT_203
	case CCxStaticRes::RT_UG:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_UG ); break;
#endif
	default:
		pRow->GetItem (COL_NAME)->SetImage ( IMG_CUSTOM ); break;
	}
}

int CZGridCtrl::GetInsertRowPos( CZGridRow * pParentRow
	, CZGridRow * pCurRow, int nChildIndex )
{
	CCxStructTreeNode * pParentNode = reinterpret_cast< CCxStructTreeNode * > ( pParentRow->GetData() );
	if ( pParentNode == NULL ) return -1;

	if ( nChildIndex == -1 ) return -1;

	int nSubItemsCount = 0;

	for (int i=0; i<nChildIndex; i++)
	{
		CZGridRow * pChildRow = GetNodeRow( pParentNode->m_vChildren[i]->m_strID );
		if ( pChildRow == NULL ) return -1;
		nSubItemsCount ++;
		nSubItemsCount += pChildRow->GetSubItemsCount(TRUE);
	}
	return nSubItemsCount;
}


void CZGridCtrl::SetParamImage( CCxStructTreeNodeParam * pNode , CZGridRow * pRow )
{
	switch ( pNode->m_nDir )
	{
	case PARAM_DIR_IN:
		if ( pNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID )
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_IN_INVALID, FALSE );
		else
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_IN, FALSE );
		break;
	case PARAM_DIR_OUT:
		if ( pNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID  )
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_OUT_INVALID, FALSE );
		else
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_OUT, FALSE );
		break;
	case PARAM_DIR_INOUT:
		if ( pNode->m_dwStatus & CCxStructTreeNode::NS_NAME_INVALID  )
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_INOUT_INVALID, FALSE );
		else
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_INOUT, FALSE );
		break;
	default:
		break;
	}
}

void CZGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CBCGPGridCtrl::OnLButtonDblClk(nFlags, point);


	CBCGPGridRow * pRow = GetCurSel();
	if ( pRow != NULL )
	{
		CCxStructTreeNode * pData = reinterpret_cast< CCxStructTreeNode * > ( pRow->GetData() );
		if ( pData != NULL )
		{
			switch ( pData->m_NodeType )
			{
			case CCxStructTreeNode::NT_COMPONENT:
				return;
			case CCxStructTreeNode::NT_ROOT:
				return;
			case CCxStructTreeNode::NT_FOR:
				return;
			case CCxStructTreeNode::NT_IF:
				return;
			case CCxStructTreeNode::NT_PARALLEL:
				return;
			case CCxStructTreeNode::NT_SEQUENCE:
				return;
			case CCxStructTreeNode::NT_SUBFLOW:
				return;
			case CCxStructTreeNode::NT_PARAM:
			case CCxStructTreeNode::NT_PARAM_ARRAY:
				{
					{//是否静态数组
						CCxStructTreeNode * p;
						if(!pData->m_vChildren.empty()) return;//非最底层节点无效
						for(p = pData; p->m_NodeType != CCxStructTreeNode::NT_PARAM; p = p->m_pParent);
						if(((CCxStructTreeNodeParam*)p)->m_bIsDynamicArray || ((CCxStructTreeNodeParam*)p)->IsFile()) return;//属于动态数组或文件参数则无效
					}
					CString strName;
					CString strComponentName;
					{//参数名确定				
						if(pData->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY)
						{//数组参数
//							if(!pData->m_vChildren.empty()) return; //非最底层节点则无效
							strName = ((CCxStructTreeNodeParam*)pData)->m_strText;
						}
						else
						{
							strName = pData->m_strOriName;
						}
					}
					{//组件名确定
						CCxStructTreeNode * p;
						for(p = pData; p->m_NodeType == CCxStructTreeNode::NT_PARAM || p->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY || p->m_NodeType == CCxStructTreeNode::NT_GROUP; p = p->m_pParent);
						strComponentName = p->m_strOriName;
					}

					CDlgWndIf& wndIf = ((CDlgIf*)GetParent())->m_wndIf;
					int nIndex = wndIf.m_nBranchSelect;
					int nSize = wndIf.m_vBCs.size();
					if(nIndex < 0 || (nSize >= 2 && nIndex == nSize - 1 && wndIf.m_nCurSelect == 1)) return; //未选中或者下拉框只读

					CZEdit& edit = wndIf.m_vBCs[nIndex]->m_editExp;

					int nPos = edit.m_nPos; //光标
					int nPosEnd = edit.m_nPosEnd; //光标

					CString strAdd = strComponentName + '.' + strName; //要添加的
					//////////////////////////含有减号则为其添加单引号//////////////////////////////////
					//if(strAdd.Find('-') != -1) 
					//{
					//	strAdd = '\'' + strAdd + '\'';
					//}
					///////////////////////////////////////////////////
					CString strEdit;  //原始的
					//strEdit = edit.m_strText;
					edit.GetWindowText(strEdit);

					CStringW wstrEdit(strEdit);

					int nLen = wstrEdit.GetLength();

					CString strLeft(wstrEdit.Left(nPos));
					CString strRight(wstrEdit.Right(wstrEdit.GetLength() - nPosEnd));

					//if(nPos > 0) strAdd = " " + strAdd;
					//if(nPos < nLen) strAdd += " ";
					CString str = strLeft + strAdd + strRight;			

					edit.SetSel(edit.m_nPos, edit.m_nPosEnd);
					edit.ReplaceSel(strAdd);
					//edit.SetWindowText(str);
					//edit.m_strText = str; //保存

					CStringW wstrAdd(strAdd);
					edit.m_nPos += wstrAdd.GetLength();
					edit.m_nPosEnd = edit.m_nPos; //光标自增

					edit.SetFocus();
					return;
				}				
			default:
				break;
			}
		}
	}
}


//*******************************************************************************************
CBCGPGridItem* CZGridCtrl::CreateItem (int nRow, int nColumn)
{
	CZGridItem* pItem = NULL;

	CRuntimeClass* pItemRTC = NULL;
	if (m_mapItemsRTC.Lookup (nColumn, pItemRTC))
	{
		ASSERT (pItemRTC != NULL);

		pItem = DYNAMIC_DOWNCAST (CZGridItem, pItemRTC->CreateObject ());
	}
	else if (m_pDefaultItemRTC != NULL)
	{
		pItem = DYNAMIC_DOWNCAST (CZGridItem, m_pDefaultItemRTC->CreateObject ());
	}
	else
	{
		pItem = new CZGridItem ();
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

BOOL CZGridCtrl::OnDrawItem( CDC* pDC, CBCGPGridRow* pItem )
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pItem);

	CZGridRow * pRow = ( CZGridRow * )pItem;

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

				//pRow->OnDrawRowMarker (pDC, rectRowMarker);

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

void CZGridCtrl::DrawTreeLine( CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset )
{
	if(!m_IsShowRoot) return;//显示选中节点时不画线
	if ( pCurRow == NULL ) return;
	if ( rcLeftOffset.IsRectEmpty() || rcLeftOffset.IsRectNull() ) return;
	CCxStructTreeNode * pCurNode = reinterpret_cast<CCxStructTreeNode*>(pCurRow->GetData());
	if ( pCurNode == NULL ) return;

	CRect rc = rcLeftOffset;
	rc.right = rc.left + rc.Height();
	int nCount = rcLeftOffset.Width() / rc.Width();
	CPoint ptCenter;

	//CPen penLine (PS_DASHDOT, 1, globalData.clrBarText);

	CBrush Brush(RGB(128, 128, 128));
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
			pDC->MoveTo (ptCenter.x, ptCenter.y);
			pDC->LineTo (rc.right, ptCenter.y);

			if ( pCurNode->m_pParent != NULL )
			{
				//绘制一条 从中心点 到 上边界 的竖线
				pDC->MoveTo (ptCenter.x, ptCenter.y);
				pDC->LineTo (ptCenter.x, rc.top - 1);

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
					pDC->MoveTo (ptCenter.x, ptCenter.y);
					pDC->LineTo (ptCenter.x, rc.bottom);
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
					pDC->MoveTo (ptCenter.x, rc.top);
					pDC->LineTo (ptCenter.x, rc.bottom);
				}
			}
		}

	}

	pDC->SelectObject (pOldPen);
}

void CZGridCtrl::SetRowHeight ()
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
