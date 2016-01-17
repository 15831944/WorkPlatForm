// CxBCGPToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxRobotBar.h"
#include "CxBCGPToolbarButton.h"
#include "PtrVectorHeader.h"
#include "ResLibImageSet.h"
#include "MainFrm.h"
#include "ResLibBaseComponentData.h"
#include "UserDefine.h"
//#include <..\src\mfc\afximpl.h>


// CCxBCGPToolBar

BEGIN_MESSAGE_MAP(CCxRobotBar, CBCGPToolBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CCxRobotBar, CBCGPToolBar, VERSIONABLE_SCHEMA | 1)

CCxRobotBar::CCxRobotBar()
{
	m_bLButtonDown = FALSE;
	m_bDragMe = TRUE;
	//m_cyTopBorder = m_cyBottomBorder = 1;

	//m_sizeCurButtonLocked = CSize (23, 23); // m_bLargeIconsAreEnbaled == TRUE
	//m_sizeCurImageLocked = CSize (16, 16);
	//m_sizeButtonLocked = CSize (23, 23); // m_bLargeIconsAreEnbaled == FALSE
	//m_sizeImageLocked = CSize (16, 16);
}

CCxRobotBar::~CCxRobotBar()
{
}

void CCxRobotBar::DoPaint( CDC* pDC )
{
	CBCGPToolBar::DoPaint(pDC);
}


//******************************************************************************************
BOOL CCxRobotBar::CreateEx (CWnd* pParentWnd, 
							DWORD dwCtrlStyle,
							DWORD dwStyle,
							CRect rcBorders,
							UINT nID)
{
	dwStyle |= CBRS_GRIPPER; 

	if (pParentWnd != NULL)
	{
		ASSERT_VALID(pParentWnd);   // must have a parent
	}

	if (rcBorders.left < 1)
	{
		rcBorders.left = 1;	// Otherwise, I have a problem with a "double" grippers
	}

	if (rcBorders.top < 1)
	{
		//rcBorders.top = 1;	// Otherwise, I have a problem with a "double" grippers
	}

	SetBorders (rcBorders);

	//----------------
	// Save the style:
	//----------------
	m_dwStyle = (dwStyle & CBRS_ALL);

	SetBarAlignment (m_dwStyle);
	if (nID == AFX_IDW_TOOLBAR)
	{
		m_dwStyle |= CBRS_HIDE_INPLACE;
	}

	BOOL bFixed = FALSE;

	dwStyle &= ~CBRS_ALL;

	if (dwStyle & CBRS_SIZE_FIXED)
	{
		bFixed = TRUE;
	}

	dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
	dwStyle |= dwCtrlStyle;

	if (!bFixed)
	{
		dwStyle &= ~CBRS_SIZE_FIXED;
	}

	//----------------------------
	// Initialize common controls:
	//----------------------------
	VERIFY (AfxDeferRegisterClass (AFX_WNDCOMMCTLS_REG));

	//-----------------
	// Create the HWND:
	//-----------------
	CRect rect;
	rect.SetRectEmpty();

	if (!CBCGPBaseToolBar::Create (
		globalData.RegisterWindowClass (_T("BCGPToolBar")), dwStyle, rect, pParentWnd, nID, 0))
	{
		return FALSE;
	}

	return TRUE;
}

//**********************************************************************************
CSize CCxRobotBar::CalcSize (BOOL bVertDock)
{
	if (m_Buttons.IsEmpty ())
	{
		return GetButtonSize ();
	}

	CClientDC dc (this);
	CFont* pOldFont = NULL;
	
	if (!bVertDock)
	{
		pOldFont = SelectDefaultFont (&dc);
	}
	else
	{
		pOldFont = dc.SelectObject (&globalData.fontVert);
	}

	ASSERT (pOldFont != NULL);

	CSize sizeGrid (GetColumnWidth (), GetRowHeight ());
	CSize sizeResult = sizeGrid;

	CRect rect; rect.SetRectEmpty();
	CalcInsideRect (rect, !bVertDock);

	int iStartX = bVertDock ? 0 : 4;
	int iStartY = bVertDock ? 1 : 0;

	CPoint cur (iStartX, iStartY);

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGPToolbarButton* pButton = (CBCGPToolbarButton*) m_Buttons.GetNext (pos);
		if (pButton == NULL)
		{
			break;
		}

		ASSERT_VALID (pButton);

		if (pos == NULL && m_pCustomizeBtn != NULL && IsFloating ())
		{
// 			ASSERT_VALID (m_pCustomizeBtn);
// 			ASSERT (m_pCustomizeBtn == pButton);	// Should be last
			break;
		}

		CSize sizeDefault (sizeGrid.cx, 
				m_bDrawTextLabels ? GetButtonSize ().cy : sizeGrid.cy);
		CSize sizeButton = pButton->OnCalculateSize (&dc, sizeDefault, !bVertDock);

		if (m_bDrawTextLabels)
		{
			sizeButton.cy = m_nMaxBtnHeight;
		}

		if (!bVertDock)
		{
			if ((cur.x == iStartX || pButton->m_bWrap)
				&& pButton->m_nStyle & TBBS_SEPARATOR)
			{
				sizeButton = CSize (0, 0);
			}

			sizeResult.cx = max (cur.x + sizeButton.cx, sizeResult.cx);
			sizeResult.cy = max (cur.y + sizeButton.cy, sizeResult.cy);

			cur.x += sizeButton.cx + 2;

			if (pButton->m_bWrap)
			{
				cur.x = iStartX;
				cur.y += sizeGrid.cy + LINE_OFFSET;
			}
		}
		else
		{
			sizeResult.cx = max (cur.x + sizeButton.cx, sizeResult.cx);
			sizeResult.cy = max (cur.y + sizeButton.cy, sizeResult.cy);

			cur.x = iStartX;
			cur.y += sizeButton.cy;
		}
	}

	dc.SelectObject (pOldFont);
	return sizeResult;
}

//**********************************************************************************
void CCxRobotBar::AdjustLocations ()
{
	ASSERT_VALID(this);

	if (m_Buttons.IsEmpty () || GetSafeHwnd () == NULL)
	{
		return;
	}

	BOOL bHorz = GetCurrentAlignment () & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	CRect rectClient;
	GetClientRect (rectClient);

	int xRight = rectClient.right;

	CClientDC dc (this);
	CFont* pOldFont;
	if (bHorz)
	{
		pOldFont = SelectDefaultFont (&dc);
	}
	else
	{
		pOldFont = (CFont*) dc.SelectObject (&globalData.fontVert);
	}
	
	ASSERT (pOldFont != NULL);

	int iStartOffset;
	if (bHorz)
	{
		iStartOffset = rectClient.left + 1;
	}
	else
	{
		iStartOffset = rectClient.top + 1;
	}

	iStartOffset += 3;
	int iOffset = iStartOffset;
	int y = rectClient.top;

	CSize sizeGrid (GetColumnWidth (), GetRowHeight ());

	CSize sizeCustButton (0, 0);

	BOOL bHideChevronInCustomizeMode = IsCustomizeMode() && DYNAMIC_DOWNCAST(CBCGPReBar, GetParent()) != NULL;

	if (m_pCustomizeBtn != NULL && !IsFloating () && !bHideChevronInCustomizeMode)
	{
// 		ASSERT_VALID (m_pCustomizeBtn);
// 		ASSERT (m_pCustomizeBtn == m_Buttons.GetTail ());	// Should be last

// 		m_pCustomizeBtn->m_lstInvisibleButtons.RemoveAll ();
// 
// 		BOOL bIsEmpty = m_pCustomizeBtn->m_bIsEmpty;
// 		m_pCustomizeBtn->m_bIsEmpty = FALSE;
// 
// 		sizeCustButton = m_pCustomizeBtn->OnCalculateSize (&dc,
// 			CSize (	bHorz ? sizeGrid.cx : rectClient.Width (), 
// 			bHorz ? rectClient.Height () : sizeGrid.cy), bHorz);
// 
// 		m_pCustomizeBtn->m_bIsEmpty = bIsEmpty;
	}

	BOOL bPrevWasSeparator = FALSE;
	int nRowActualWidth = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		POSITION posSave = pos;

		CBCGPToolbarButton* pButton = (CBCGPToolbarButton*) m_Buttons.GetNext (pos);
		if (pButton == NULL)
		{
			break;
		}

		ASSERT_VALID (pButton);

		BOOL bVisible = TRUE;

		CSize sizeButton = pButton->OnCalculateSize (&dc, sizeGrid, bHorz);
		if (pButton->m_bTextBelow && bHorz)
		{
			sizeButton.cy =  sizeGrid.cy;
		}

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			if (iOffset == iStartOffset || bPrevWasSeparator)
			{
				sizeButton = CSize (0, 0);
				bVisible = FALSE;
			}
			else
			{
				bPrevWasSeparator = TRUE;
			}
		}

		int iOffsetPrev = iOffset;

		CRect rectButton;
		if (bHorz)
		{
			rectButton.left = iOffset;
			rectButton.right = rectButton.left + sizeButton.cx;
			rectButton.top = y;
			rectButton.bottom = rectButton.top + sizeButton.cy;
			
			iOffset += sizeButton.cx + 2;
			nRowActualWidth += sizeButton.cx;
		}
		else
		{
			rectButton.left = rectClient.left;
			rectButton.right = rectClient.left + sizeButton.cx;
			rectButton.top = iOffset;
			rectButton.bottom = iOffset + sizeButton.cy;

			iOffset += sizeButton.cy;
		}

// 		if (m_pCustomizeBtn != NULL && pButton != m_pCustomizeBtn &&
// 			!IsFloating () && !IsCustomizeMode ())
// 		{
// 			CSize fakeSizeCustButton (sizeCustButton);
// 			class CCustomizeButton;
			//-------------------------------------------------------------------------
			// I assume, that the customize button is at the tail position at any time.
			//-------------------------------------------------------------------------
// 			if ((int) m_pCustomizeBtn->m_uiCustomizeCmdId <= 0 &&
// 				(pos != NULL && m_Buttons.GetAt (pos) == m_pCustomizeBtn) && 
// 				m_pCustomizeBtn->m_lstInvisibleButtons.IsEmpty ())
// 			{
// 				fakeSizeCustButton = CSize (0,0);
// 			}
// 
// 			if ((bHorz && rectButton.right > xRight - fakeSizeCustButton.cx) ||
// 				(!bHorz && rectButton.bottom > rectClient.bottom - fakeSizeCustButton.cy))
// 			{
// 				bVisible = FALSE;
// 				iOffset = iOffsetPrev;
// 				
// 				m_pCustomizeBtn->m_lstInvisibleButtons.AddTail (pButton);
// 			}
//		}

		pButton->Show (bVisible);
		pButton->SetRect (rectButton);

		if (bVisible)
		{
			bPrevWasSeparator = (pButton->m_nStyle & TBBS_SEPARATOR);
		}

		if ((pButton->m_bWrap || pos == NULL) && bHorz)
		{
			//-----------------------
			// Center buttons in row:
			//-----------------------
			int nShift = (xRight - nRowActualWidth - iStartOffset) / 2;
			if (IsFloating () && nShift > 0 && m_bTextLabels)
			{
				for (POSITION posRow = posSave; posRow != NULL;)
				{
					BOOL bThis = (posRow == posSave);

					CBCGPToolbarButton* pButtonRow = (CBCGPToolbarButton*) m_Buttons.GetPrev (posRow);
					ASSERT (pButtonRow != NULL);

					if (pButtonRow->m_bWrap && !bThis)
					{
						break;
					}

					CRect rect = pButtonRow->Rect ();
					rect.OffsetRect (nShift, 0);
					pButtonRow->SetRect (rect);
				}
			}

			iOffset = iStartOffset;
			nRowActualWidth = 0;
			y += sizeGrid.cy + LINE_OFFSET;
		}
	}

	if (m_pCustomizeBtn != NULL)
	{
		CRect rectButton = rectClient;

// 		if ((int) m_pCustomizeBtn->m_uiCustomizeCmdId <= 0 &&
// 			m_pCustomizeBtn->m_lstInvisibleButtons.IsEmpty () || IsFloating () || bHideChevronInCustomizeMode)
// 		{
// 			// Hide customize button:
// 			m_pCustomizeBtn->SetRect (CRect (0, 0, 0, 0));
// 			m_pCustomizeBtn->Show (FALSE);
// 		}
// 		else
		{
			if (bHorz)
			{
				rectButton.right = xRight - 1;
				rectButton.left = rectButton.right - sizeCustButton.cx + 1;
			}
			else
			{
				rectButton.bottom --;
				rectButton.top = rectButton.bottom - sizeCustButton.cy;
			}

// 			m_pCustomizeBtn->SetRect (rectButton);
// 			m_pCustomizeBtn->Show (TRUE);
		}
	}

	dc.SelectObject (pOldFont);
	UpdateTooltips ();
	RedrawCustomizeButton ();
}

void CCxRobotBar::ClearData()
{
	m_BCGToolbarImages.Clear();
	for (int i=0; i<(int)m_vHBitmaps.size(); i++)
	{
		DeleteObject ( m_vHBitmaps[i] );
	}
	m_vHBitmaps.clear();
	PtrVectorClear( m_vImages );
}

void CCxRobotBar::LoadData()
{
	Bitmap * pBitmap = NULL;
	HBITMAP hbmp = NULL;
	CCxStaticRes * pImgData = NULL;

	ResetAll();
	ClearData();

	m_BCGToolbarImages.SetImageSize( CSize(16, 16) );
	//m_BCGToolbarImages.SetTransparentColor (g_crToolbarBg);

	//Color crImgBg;
	//crImgBg.SetFromCOLORREF(g_crToolbarBg);

	int iCompentCount = (int)CCxStaticRes::RT_SCRIPT;
#ifdef PROJECT_203
	iCompentCount = (int)CCxStaticRes::RT_UG;
#endif

	for (int i = CCxStaticRes::RT_NULL + 1; 
		i <= iCompentCount; i++)
	{
		pImgData = GetRobotImageData(i, 16);
		if ( pImgData == NULL )
		{
			//ResetAll();
			ClearData();
			return;
		}

		pBitmap = GetNewBitmap( pImgData->m_strPath );
		if ( pBitmap == NULL )
		{
			CString strLog;
			strLog.Format( "图片资源丢失，%s", pImgData->m_strPath );
			MessageBox( strLog, g_lpszAppTitle );

			ClearData(); return;
		}
		//ScaleBitmap(pBitmap, 16, 16, FALSE);
		m_vImages.push_back( pBitmap );
		pBitmap->GetHBITMAP( NULL, &hbmp );
		if ( hbmp == NULL )
		{
			ClearData(); return;
		}
		m_vHBitmaps.push_back( hbmp );
		m_BCGToolbarImages.AddImage( hbmp );
	}

	if ( m_BCGToolbarImages.GetCount() == 0 ) return;

	SetSizes( CSize(24, 24), CSize(16, 16) );
	//m_sizeCurImage = CSize(12,12);
	//m_sizeImage = CSize(12,12);

	//SetUserImages( &m_BCGToolbarImages );

	int nImgPos = m_Images.GetCount() > 0 ? m_Images.GetCount() : 0;

	for (int i=0; i<m_BCGToolbarImages.GetCount(); i++)
	{
		m_Images.AddImage( m_vHBitmaps[i] );
		m_ImagesLocked.AddImage( m_vHBitmaps[i] );
		m_LargeImages.AddImage( m_vHBitmaps[i] );
		m_LargeImagesLocked.AddImage( m_vHBitmaps[i] );
	}

	m_BCGToolbarImages.ConvertTo32Bits();
	m_Images.ConvertTo32Bits();
	m_ImagesLocked.ConvertTo32Bits();
	m_LargeImages.ConvertTo32Bits();
	m_LargeImagesLocked.ConvertTo32Bits();

	LoadButton( IDM_ROBOTBAR_BUTTON_FIRST
		, nImgPos
		, m_BCGToolbarImages.GetCount() );


}




void CCxRobotBar::LoadButton( int nFirstCmd, int nFirstImg, int nCount )
{
	CCxBCGPToolbarButton * pButton = NULL;

	RemoveAllButtons ();

	while (!m_OrigButtons.IsEmpty ())
	{
		delete m_OrigButtons.RemoveHead ();
	}

	for (int i=0; i<nCount; i++)
	{
		//switch (i)
		//{
		//case 2:
		//case 4:
		//case 7:
		//case 9:
		//	InsertSeparator( m_Buttons.GetCount() );
		//	break;
		//default:
		//	break;
		//}
		m_DefaultImages.SetAt (IDM_ROBOTBAR_BUTTON_FIRST + i, nFirstImg + i);

		m_OrigButtons.AddTail (new CCxBCGPToolbarButton (IDM_ROBOTBAR_BUTTON_FIRST + i
			, nFirstImg + i, NULL, TRUE));

		InsertButton ( new CCxBCGPToolbarButton (IDM_ROBOTBAR_BUTTON_FIRST + i
			, nFirstImg + i
			, NULL
			, FALSE
			, m_bLocked));
		

		//	pButton = new CCxBCGPToolbarButton();
		//	pButton->SetImage( IMG_NEW_COMPONENT + i );
		////	pButton->m_bUserButton = TRUE;
		//	pButton->m_nID = IDM_TOOLBAR_BUTTON_FIRST + i;
		//	InsertButton( pButton );
	}



	CWnd* pParentFrame = (m_pDockSite == NULL) ?
		GetParent () : m_pDockSite;
	if (pParentFrame != NULL)
	{
		pParentFrame->SendMessage (BCGM_RESETTOOLBAR, (WPARAM) m_uiOriginalResID);

		while (!m_OrigResetButtons.IsEmpty ())
		{
			delete m_OrigResetButtons.RemoveHead ();
		}

		//-------------------------------------------
		//	Store Buttons state after OnToolbarReset
		//-------------------------------------------
		int i = 0;
		for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i++)
		{
			CBCGPToolbarButton* pButton = (CBCGPToolbarButton*) m_Buttons.GetNext (pos);

			if(pButton != NULL && pButton->IsKindOf(RUNTIME_CLASS(CBCGPToolbarButton)))
			{
				CRuntimeClass* pRTC = pButton->GetRuntimeClass();
				CBCGPToolbarButton* pBtn = (CBCGPToolbarButton*)pRTC->CreateObject();
				pBtn->CopyFrom(*pButton);
				m_OrigResetButtons.AddTail(pBtn); 

			}
		}
	}
}


void CCxRobotBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	int iButton = HitTest(point);
	m_bDragMe = ( iButton < 0 );

	CBCGPToolBar::OnLButtonDown(nFlags, point);
}


void CCxRobotBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = FALSE;
	m_bDragMe = TRUE;

	CBCGPToolBar::OnLButtonUp(nFlags, point);

	ClearButtonStatus();
}


void CCxRobotBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bLButtonDown )
	{
		CCxBCGPToolbarButton* pButton = NULL;
		//int i = 0;
		//for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i++)
		//{
		//	pButton = (CCxBCGPToolbarButton*) m_Buttons.GetNext (pos);
		//	if(pButton != NULL && pButton->IsKindOf(RUNTIME_CLASS(CBCGPToolbarButton)))
		//	{
		//		if ( pButton->GetRect().PtInRect( point ) )
		//		{
		//			break;
		//		}
		//		pButton = NULL;
		//	}
		//	else
		//		pButton = NULL;
		//}

		int iButton = HitTest(point);

		if ( iButton >= 0 && iButton < m_Buttons.GetCount() && !m_bDragMe )
		{
			int i = 0;
			for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; i++)
			{
				pButton = (CCxBCGPToolbarButton*) m_Buttons.GetNext (pos);
				if (i==iButton)
				{
					break;
				}
				pButton = NULL;
			}

			if ( pButton != NULL )
			{
				m_bLButtonDown = FALSE;
				m_iHighlighted = -1;
				m_iButtonCapture = -1;
				m_iSelected = -1;
				pButton->m_nStyle &= (~TBBS_PRESSED & ~TBBS_CHECKED);
				RedrawWindow();
				//OnLButtonUp(nFlags, point);
				theApp.m_pMainWnd->PostMessage(WM_SHOW_SVG_DRAG_LAYER
					, (WPARAM)TRUE, (LPARAM)iButton);
			}

			

			

			//DragLayer(i);
		}
	}

	CBCGPToolBar::OnMouseMove(nFlags, point);
}



void CCxRobotBar::DragLayer( int nPos )
{
	ClearButtonStatus();
	//int i= 0;
	//for (list< CResLibDataBase* >::iterator it = m_BaseComponentList.begin();
	//	it != m_BaseComponentList.end();it++)
	//{
	//}
	//m_BaseComponentList.

	
	if ( m_vResLibData.size() == 0 ) return;
	if ( nPos >= (int)m_vResLibData.size() ) return;
	if ( m_vResLibData[nPos] == NULL ) return;

	//int i= 0;
	//for (list< CResLibData* >::iterator it = m_BaseComponentList.begin();
	//	it != m_BaseComponentList.end(); ++it)
	//{
	//	if ( i == nPos )
	//	{
	//		pBase = *it;
	//		break;
	//	}
	//	++i ;
	//}
	//if ( pBase == NULL ) return;

	if (theApp.m_pDlgDragLayer == NULL)
	{
		CMainFrame * pMainFrame = reinterpret_cast< CMainFrame * > ( theApp.m_pMainWnd );
		theApp.m_pDlgDragLayer = new CDlgDragLayer();
		theApp.m_pDlgDragLayer->Create(CDlgDragLayer::IDD, pMainFrame);
	}

	theApp.m_pDlgDragLayer->StartDrag( m_vResLibData[nPos] );

}


void CCxRobotBar::UpdateResData( list< CResLibData* >& BaseComponentList )
{
	m_vResLibData.clear();
	m_vResLibData.resize( m_Buttons.GetCount() );

	//if ( m_Buttons.GetCount() < (int)BaseComponentList.size() )
	//{
	//	AfxMessageBox("基础组件个数不匹配。");
	//	return;
	//}

	int nCode = -1;
	for ( list< CResLibData * >::iterator itr = BaseComponentList.begin();
		itr != BaseComponentList.end(); ++itr )
	{
		nCode = ConvertRobotId2Code( ((CResLibBaseComponentData *)(*itr))->m_RobotType.c_str() );
		if ( nCode == CCxStaticRes::RT_NULL )
		{
			continue;
		}
		m_vResLibData[ nCode - (CCxStaticRes::RT_NULL + 1) ] = (*itr);
	}
}

void CCxRobotBar::ClearButtonStatus()
{
	CCxBCGPToolbarButton* pButton = NULL;

	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; )
	{
		pButton = (CCxBCGPToolbarButton*) m_Buttons.GetNext (pos);
		if ( pButton == NULL ) continue;

		if ( 
			(pButton->m_nStyle & TBBS_PRESSED) == TBBS_PRESSED 
			|| (pButton->m_nStyle & TBBS_CHECKED) == TBBS_CHECKED 
			|| m_iHighlighted != -1 
			|| m_iButtonCapture != -1 
			|| m_iSelected != -1 
			)
		{
			pButton->m_nStyle &= (~TBBS_PRESSED & ~TBBS_CHECKED);
		}
	}
	m_iHighlighted = -1;
	m_iButtonCapture = -1;
	m_iSelected = -1;

	CDC * pDC = this->GetDC();
	DoPaint( pDC );
	this->ReleaseDC( pDC );
}

BOOL CCxRobotBar::DrawButton(CDC* pDC, CBCGPToolbarButton* pButton,
							CBCGPToolBarImages* pImages,
							BOOL bHighlighted, BOOL bDrawDisabledImages)
{
	ASSERT_VALID (pDC);
	ASSERT_VALID (pButton);

	if (!pButton->IsVisible() || pButton->IsHidden () || !pDC->RectVisible (pButton->Rect ()))
	{
		return TRUE;
	}

	BOOL bHorz = GetCurrentAlignment () & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	//---------------------
	// Draw button context:
	//---------------------
	pButton->OnDraw (pDC, pButton->Rect (), pImages, bHorz, 
		IsCustomizeMode () && !m_bAltCustomizeMode && !m_bLocked, 
		bHighlighted, m_bShowHotBorder,
		m_bGrayDisabledButtons && !bDrawDisabledImages);
	return TRUE;
}
//
//INT_PTR CCxRobotBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
//{
//	ASSERT_VALID(this);
//
//	if (!m_bShowTooltips)
//	{
//		return -1;
//	}
//
//	int nIndex = -1;
//	CCxBCGPToolbarButton* pButton= NULL;
//
//	int iButton = 0;
//	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
//	{
//		pButton = (CCxBCGPToolbarButton*) m_Buttons.GetNext (pos);
//		//ASSERT (pButton != NULL);
//
//		if (pButton->Rect ().PtInRect (point) && !pButton->IsHidden ())
//		{
//			if (pButton->m_nStyle & TBBS_SEPARATOR) 
//				return -1;
//			nIndex = iButton;
//			break;
//		}
//	}
//
//	if ( nIndex == -1 ) return -1;
//	if ( (int)m_vResLibData.size() <= nIndex ) return -1;
//
//	//CCxBCGPToolbarButton* pButton = (CCxBCGPToolbarButton*)GetButton(nIndex);
//	//if ( pButton == NULL ) return -1;
//
//	CString strTipText = m_vResLibData[nIndex]->m_sName.c_str();
//	CString strTipDescr = "";
//
//	CBCGPTooltipManager::SetTooltipText (pTI,
//	m_pToolTip, BCGP_TOOLTIP_TYPE_TOOLBAR, strTipText, strTipDescr);
//
//	GetItemRect((int)nIndex, &pTI->rect);
//	pTI->uId = (pButton->m_nID == (UINT) -1) ? 0 : pButton->m_nID;
//	pTI->hwnd = m_hWnd;
//	
//	return nIndex;
//	//return nHit;
//}

BOOL CCxRobotBar::OnUserToolTip (CBCGPToolbarButton* pButton, CString& strTTText) const
{
	ASSERT_VALID (pButton);

	int iButton = ButtonToIndex(pButton);
	if ( iButton == -1 ) return FALSE;
	if ( (int)m_vResLibData.size() <= iButton ) return -1;
	if ( m_vResLibData[iButton] == NULL ) return -1;

	strTTText = m_vResLibData[iButton]->m_sName.c_str();
	return TRUE;

	//CFrameWnd* pTopFrame = BCGPGetParentFrame (this);
	//if (pTopFrame == NULL)
	//{
	//	return FALSE;
	//}

	//CBCGPDropDownFrame* pDropFrame = DYNAMIC_DOWNCAST (CBCGPDropDownFrame, pTopFrame);
	//if (pDropFrame != NULL)
	//{
	//	pTopFrame = BCGPGetParentFrame (pDropFrame);
	//	if (pTopFrame == NULL)
	//	{
	//		return FALSE;
	//	}
	//}

	//CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, pTopFrame);
	//if (pMainFrame != NULL)
	//{
	//	return pMainFrame->GetToolbarButtonToolTipText (pButton, strTTText);
	//}
	//else	// Maybe, SDI frame...
	//{
	//	CBCGPFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPFrameWnd, pTopFrame);
	//	if (pFrame != NULL)
	//	{
	//		return pFrame->GetToolbarButtonToolTipText (pButton, strTTText);
	//	}
	//	else // Maybe, MDIChild frame
	//	{
	//		 CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pTopFrame);

	//		 if(pMDIChild != NULL)
	//		 {
	//			 return pMDIChild->GetToolbarButtonToolTipText (pButton, strTTText);
	//		 }
	//		 else	// Maybe, OLE frame...
	//		 {
	//				CBCGPOleIPFrameWnd* pOleFrame = 
	//					DYNAMIC_DOWNCAST (CBCGPOleIPFrameWnd, pFrame);
	//				if (pOleFrame != NULL)
	//				{
	//					return pOleFrame->GetToolbarButtonToolTipText (pButton, strTTText);
	//				}
	//		 }
	//	}
	//}

	//return FALSE;
}