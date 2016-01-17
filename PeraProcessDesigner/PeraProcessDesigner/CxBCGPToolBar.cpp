// CxBCGPToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "CxBCGPToolBar.h"
#include "CxBCGPToolbarButton.h"
#include "PtrVectorHeader.h"
#include "UserDefine.h"
//#include <..\src\mfc\afximpl.h>


// CCxBCGPToolBar

BEGIN_MESSAGE_MAP(CCxBCGPToolBar, CBCGPToolBar)
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CCxBCGPToolBar, CBCGPToolBar, VERSIONABLE_SCHEMA | 1)

CCxBCGPToolBar::CCxBCGPToolBar()
{
	//m_cyTopBorder = m_cyBottomBorder = 1;

	//m_sizeCurButtonLocked = CSize (23, 23); // m_bLargeIconsAreEnbaled == TRUE
	//m_sizeCurImageLocked = CSize (16, 16);
	//m_sizeButtonLocked = CSize (23, 23); // m_bLargeIconsAreEnbaled == FALSE
	//m_sizeImageLocked = CSize (16, 16);
}

CCxBCGPToolBar::~CCxBCGPToolBar()
{
}

void CCxBCGPToolBar::DoPaint( CDC* pDCPaint )
{
	CBCGPToolBar::DoPaint(pDCPaint);
	return;

	ASSERT_VALID(this);
	ASSERT_VALID(pDCPaint);

	CBCGPDrawOnGlass dog (m_bOnGlass);

	CRect rectClip;
	pDCPaint->GetClipBox (rectClip);

	BOOL bHorz = GetCurrentAlignment () & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	CRect rectClient;
	GetClientRect (rectClient);

	CBCGPMemDC memDC (*pDCPaint, this);
	CDC* pDC = &memDC.GetDC ();

	if ((GetStyle () & TBSTYLE_TRANSPARENT) == 0)
	{
		CBCGPVisualManager::GetInstance ()->OnFillBarBackground (pDC, this,
			rectClient, rectClip);
	}
	else
	{
		//dunl--- m_Impl.GetBackgroundFromParent (pDC);
	}

	OnFillBackground (pDC);

//	pDC->SetTextColor (globalData.clrBtnText);
	pDC->SetTextColor ( g_crToolbarBg );
	pDC->SetBkMode (TRANSPARENT);

	CRect rect;
	GetClientRect(rect);

	//-----------------------------------
	// Force the full size of the button:
	//-----------------------------------
	if (bHorz)
	{
		rect.bottom = rect.top + GetRowHeight ();
	}
	else
	{
		rect.right = rect.left + GetColumnWidth ();
	}

	BOOL bDontScaleImages = m_bLocked ? m_bDontScaleLocked : m_bDontScaleImages;
	const double dblImageScale = bDontScaleImages ? 1.0 : globalData.GetRibbonImageScale ();

	CBCGPToolBarImages* pImages = 
		GetImageList (m_Images, m_ImagesLocked, m_LargeImages, m_LargeImagesLocked);
	CBCGPToolBarImages* pHotImages = pImages;
	CBCGPToolBarImages* pColdImages = 
		GetImageList (m_ColdImages, m_ColdImagesLocked, m_LargeColdImages, m_LargeColdImagesLocked);
	CBCGPToolBarImages* pDisabledImages = 
		GetImageList (m_DisabledImages, m_DisabledImagesLocked, m_LargeDisabledImages, m_LargeDisabledImagesLocked);
	CBCGPToolBarImages* pMenuImages = !m_bLocked ? 
		&m_MenuImages : &m_MenuImagesLocked;

	CBCGPToolBarImages* pDisabledMenuImages = !m_bLocked ?
		&m_DisabledMenuImages : &m_DisabledMenuImagesLocked;

	BOOL bDrawImages = pImages->IsValid ();

	//pHotImages->SetTransparentColor (globalData.clrBtnFace);
	//pHotImages->SetTransparentColor ( g_crToolbarBg );

	BOOL bFadeInactiveImages = CBCGPVisualManager::GetInstance ()->IsFadeInactiveImage ();

	CSize sizeImageDest = m_bMenuMode ? m_sizeMenuImage : GetImageSize ();
	if (dblImageScale != 1.)
	{
		if (m_bMenuMode && sizeImageDest == CSize (-1, -1))
		{
			sizeImageDest = GetImageSize ();

			if (dblImageScale > 1. && m_bLargeIconsAreEnbaled)
			{
				sizeImageDest = m_sizeImage;
			}
		}

		sizeImageDest = CSize ((int)(.5 + sizeImageDest.cx * dblImageScale), (int)(.5 + sizeImageDest.cy * dblImageScale));
	}

	CBCGPDrawState ds;
	if (bDrawImages)
	{
		if (dblImageScale != 1.0 && pHotImages->GetScale () == 1.0)
		{
			pHotImages->SmoothResize (dblImageScale);
		}

		if (!pHotImages->PrepareDrawImage (ds, sizeImageDest, bFadeInactiveImages))
		{
			return;     // something went wrong
		}
	}

	CFont* pOldFont;
	if (bHorz)
	{
		pOldFont = SelectDefaultFont (pDC);
	}
	else
	{
		pOldFont = (CFont*) pDC->SelectObject (&globalData.fontVert);
	}

	if (pColdImages->GetCount () > 0)
	{
		//------------------------------------------
		// Disable fade effect for inactive buttons:
		//------------------------------------------
		CBCGPVisualManager::GetInstance ()->SetFadeInactiveImage (FALSE);
	}

	//--------------
	// Draw buttons:
	//--------------
	int iButton = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL; iButton ++)
	{
		CBCGPToolbarButton* pButton = (CBCGPToolbarButton*) m_Buttons.GetNext (pos);
		if (pButton == NULL)
		{
			break;
		}

		ASSERT_VALID (pButton);

		rect = pButton->Rect ();
		CRect rectInter;

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			BOOL bHorzSeparator = bHorz;
			CRect rectSeparator; rectSeparator.SetRectEmpty ();

			OnCalcSeparatorRect (pButton, rectSeparator, bHorz);

			if (pButton->m_bWrap && bHorz)
			{
				bHorzSeparator = FALSE;
			}

			if (rectInter.IntersectRect (rectSeparator, rectClip) && !pButton->IsHidden())
			{
				DrawSeparator (pDC, rectSeparator, bHorzSeparator);
			}

			continue;
		}

		if (!rectInter.IntersectRect (rect, rectClip))
		{
			continue;
		}

		BOOL bHighlighted = IsButtonHighlighted (iButton);
		BOOL bDisabled = (pButton->m_nStyle & TBBS_DISABLED) && !IsCustomizeMode ();

		if (pDC->RectVisible(&rect))
		{
			BOOL bDrawDisabledImages = FALSE;

			if (bDrawImages)
			{
				CBCGPToolBarImages* pNewImages = NULL;

				if (pButton->m_bUserButton)
				{
					if (pButton->GetImage () >= 0)
					{
						pNewImages = m_pUserImages;
					}
				}
				else
				{
					if (m_bMenuMode)
					{
						if (bDisabled && pDisabledMenuImages->GetCount () > 0)
						{
							bDrawDisabledImages = TRUE;
							pNewImages = pDisabledMenuImages;
						}
						else if (pMenuImages->GetCount () > 0)
						{
							pNewImages = pMenuImages;
						}
						else
						{
							bDrawDisabledImages = 
								(bDisabled && pDisabledImages->GetCount () > 0);

							pNewImages =  bDrawDisabledImages ? 
pDisabledImages : pHotImages;
						}
					}
					else	// Toolbar mode
					{
						bDrawDisabledImages = 
							(bDisabled && pDisabledImages->GetCount () > 0);

						pNewImages =  bDrawDisabledImages ? 
pDisabledImages : pHotImages;

						if (!bHighlighted && !bDrawDisabledImages &&
							(pButton->m_nStyle & TBBS_PRESSED) == 0 &&
							pColdImages->GetCount () > 0 &&
							!pButton->IsDroppedDown ())
						{
							pNewImages = pColdImages;
						}
					}
				}

				if (bDrawImages && pNewImages != pImages && pNewImages != NULL)
				{
					pImages->EndDrawImage (ds);

					//pNewImages->SetTransparentColor (globalData.clrBtnFace);
					pNewImages->SetTransparentColor ( g_crToolbarBg );

					if (dblImageScale != 1.0 && pNewImages->GetScale () == 1.0)
					{
						pNewImages->SmoothResize (dblImageScale);
					}

					pNewImages->PrepareDrawImage (ds, sizeImageDest, bFadeInactiveImages);

					pImages = pNewImages;
				}
			}

			DrawButton (pDC, pButton, bDrawImages ? pImages : NULL, 
				bHighlighted, bDrawDisabledImages);
		}
	}

	//-------------------------------------------------------------
	// Highlight selected button in the toolbar customization mode:
	//-------------------------------------------------------------
	if (m_iSelected >= m_Buttons.GetCount ())
	{
		m_iSelected = -1;
	}

	if ((IsCustomizeMode () || m_bAltCustomizeMode) && m_iSelected >= 0 && !m_bLocked && 
		m_pSelToolbar == this)
	{
		CBCGPToolbarButton* pSelButton = GetButton (m_iSelected);
		ASSERT (pSelButton != NULL);

		if (pSelButton != NULL && pSelButton->CanBeStored ())
		{
			CRect rectDrag1 = pSelButton->Rect ();

			pDC->Draw3dRect(&rectDrag1, globalData.clrBtnText, globalData.clrBtnText);
			rectDrag1.DeflateRect (1, 1);
			pDC->Draw3dRect(&rectDrag1, globalData.clrBtnText, globalData.clrBtnText);
		}
	}

	if (IsCustomizeMode () && m_iDragIndex >= 0 && !m_bLocked)
	{
		DrawDragMarker (pDC);
	}

	pDC->SelectObject (pOldFont);

	if (bDrawImages)
	{
		pImages->EndDrawImage (ds);
	}

	CBCGPVisualManager::GetInstance ()->SetFadeInactiveImage (bFadeInactiveImages);

}

//******************************************************************************************
BOOL CCxBCGPToolBar::CreateEx (CWnd* pParentWnd, 
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
CSize CCxBCGPToolBar::CalcSize (BOOL bVertDock)
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
void CCxBCGPToolBar::AdjustLocations ()
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

void CCxBCGPToolBar::ClearData()
{
	m_BCGToolbarImages.Clear();
	for (int i=0; i<(int)m_vHBitmaps.size(); i++)
	{
		DeleteObject ( m_vHBitmaps[i] );
	}
	m_vHBitmaps.clear();
	PtrVectorClear( m_vImages );
}

void CCxBCGPToolBar::LoadData()
{
// 	static const LPCTSTR arrImageName[] = {
// 		"NewProcess.png", "OpenProcess.png", "RunProcess.png",
// 		"Preview.png", "Save.png", "DataLinker.png", "Monitor.png", 
// 		"Help.png", "About.png" 
// 	};

	static const LPCTSTR arrImageName[] = {
		"NewProcess.png", "OpenProcess.png", "Save.png", "SaveAs.png", "Info.png", "Publish.png", "DataLinker.png", 
		"RunProcess.png",
		"Monitor.png", "Preview.png", 
		"Help.png"/*, "About.png" */
	};

	CString strDir = GetExecDir() + "\\Res\\MainToolbar\\";
	Bitmap * pBitmap = NULL;
	HBITMAP hbmp = NULL;

	//ResetAll();

	ClearData();

	m_BCGToolbarImages.SetImageSize( CSize(16, 16) );

	//Color crImgBg;
	//crImgBg.SetFromCOLORREF(g_crToolbarBg);
	//Color crImgBg = Color::White;
	//m_BCGToolbarImages.SetTransparentColor (g_crToolbarBg);

	int nCount = sizeof( arrImageName ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pBitmap = GetNewBitmap( strDir + arrImageName[i] );
		if ( pBitmap == NULL )
		{
			CString strLog;
			strLog.Format( "图片资源丢失，%s", arrImageName[i] );
			MessageBox( strLog, g_lpszAppTitle );

			ClearData();
			return;
		}
		m_vImages.push_back( pBitmap );
		pBitmap->GetHBITMAP( NULL, &hbmp);
		if ( hbmp == NULL )
		{
			ClearData();
			return;
		}
		m_vHBitmaps.push_back( hbmp );
		m_BCGToolbarImages.AddImage( hbmp );
	}

	if ( m_BCGToolbarImages.GetCount() == 0 ) return;

	SetSizes( CSize(32, 22), CSize(16, 16) );
	//SetUserImages( &m_BCGToolbarImages );

	int nImgPos = m_Images.GetCount() > 0 ? m_Images.GetCount() : 0;

	for (int i=0; i<m_BCGToolbarImages.GetCount(); i++)
	{
		m_Images.AddImage( m_vHBitmaps[i] );
		m_ImagesLocked.AddImage( m_vHBitmaps[i] );
		m_LargeImages.AddImage( m_vHBitmaps[i] );
		m_LargeImagesLocked.AddImage( m_vHBitmaps[i] );
	}

	//LoadButton( IDM_TOOLBAR_BUTTON_FIRST, IMG_NEW_PROCESS, m_BCGToolbarImages.GetCount() );

	m_BCGToolbarImages.ConvertTo32Bits();
	m_Images.ConvertTo32Bits();
	m_ImagesLocked.ConvertTo32Bits();
	m_LargeImages.ConvertTo32Bits();
	m_LargeImagesLocked.ConvertTo32Bits();
	
	LoadButton( IDM_TOOLBAR_BUTTON_FIRST
		, nImgPos
		, m_BCGToolbarImages.GetCount() );

}




void CCxBCGPToolBar::LoadButton( int nFirstCmd, int nFirstImg, int nCount )
{
	CCxBCGPToolbarButton * pButton = NULL;

	RemoveAllButtons ();

	while (!m_OrigButtons.IsEmpty ())
	{
		delete m_OrigButtons.RemoveHead ();
	}

	for (int i=0; i<nCount; i++)
	{
		switch (i)
		{
		case IMG_RUN_PROCESS:
		case IMG_JOBMGR:
		case IMG_HELP:
			InsertSeparator( m_Buttons.GetCount() );
			break;
		default:
			break;
		}
		m_DefaultImages.SetAt (IDM_TOOLBAR_BUTTON_FIRST + i, nFirstImg + i);

		m_OrigButtons.AddTail (new CBCGPToolbarButton (IDM_TOOLBAR_BUTTON_FIRST + i
			, nFirstImg + i, NULL, TRUE));

		InsertButton (CBCGPToolbarButton (IDM_TOOLBAR_BUTTON_FIRST + i
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


BOOL CCxBCGPToolBar::OnUserToolTip (CBCGPToolbarButton* pButton, CString& strTTText) const
{
	ASSERT_VALID (pButton);

	int iButton = ButtonToIndex(pButton);
	switch ( iButton )
	{
	case IMG_NEW_PROCESS:
		strTTText = "新建设计过程"; break;
	case IMG_OPEN_PROCESS:
		strTTText = "打开设计过程"; break;
	case IMG_SAVE:
		strTTText = "保存"; break;
	case IMG_SAVEAS:
		strTTText = "另存为"; break;
	case IMG_INFO:
		strTTText = "属性"; break;
	case IMG_PUBLISH:
		strTTText = "发布"; break;
	case IMG_DATALINKER:
		strTTText = "数据映射"; break;
	case IMG_RUN_PROCESS + 1:
		strTTText = "运行设计过程"; break;
	case IMG_JOBMGR + 2:
		strTTText = g_lpszViewTitleJobMgr; break;
	case IMG_PREVIEW + 2:
		strTTText = "分布式资源管理"; break;
	case IMG_HELP + 3:
		strTTText = "帮助"; break;
// 	case IMG_ABOUT + 3:
// 		strTTText = "关于"; break;
	default:
		return FALSE;
	}
	return TRUE;
}