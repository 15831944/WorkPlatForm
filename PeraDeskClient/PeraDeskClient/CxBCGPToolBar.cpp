// CxBCGPToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "CxBCGPToolBar.h"
#include "CxBCGPToolbarButton.h"
#include "PtrVectorHeader.h"
#include "UserDefine.h"
//#include <..\src\mfc\afximpl.h>
#include <MainFrm.h>
#include <BCGPDockBar.h>

// CCxBCGPToolBar

BEGIN_MESSAGE_MAP(CCxBCGPToolBar, CBCGPToolBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CCxBCGPToolBar, CBCGPToolBar, VERSIONABLE_SCHEMA | 1)

CCxBCGPToolBar::CCxBCGPToolBar()
{
	//m_cyTopBorder = m_cyBottomBorder = 1;

	//m_sizeCurButtonLocked = CSize (23, 23); // m_bLargeIconsAreEnbaled == TRUE
	//m_sizeCurImageLocked = CSize (16, 16);
	//m_sizeButtonLocked = CSize (23, 23); // m_bLargeIconsAreEnbaled == FALSE
	//m_sizeImageLocked = CSize (16, 16);
	
	m_pMiniFrameRTC = RUNTIME_CLASS (CBCGPMiniFrameWnd);
	m_bUserOP = TRUE;
	SetPermament(); //bar浮动出来后，没有关闭按钮
	m_bLBDownForFloat = FALSE;
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
	pHotImages->SetTransparentColor ( g_crToolbarBg );

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
		"ToolbarLogout.png"
		,"ToolbarRefresh.png"
		,"ToolbarMessage.png"
		,"ToolbarPortal.png"
		, "ToolbarFlexware.png"
		, "ToolbarSetting.png"
		, "ToolbarAbout.png"
	};

	CString strDir = GetExecDir() + "\\Res\\MainToolbar\\";
	Bitmap * pBitmap = NULL;
	HBITMAP hbmp = NULL;

	//ResetAll();

	ClearData();

	m_BCGToolbarImages.SetImageSize( CSize(30, 30) );

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

	SetSizes( CSize(30, 30), CSize(30, 30) );
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

	//LoadButton( IDM_TOOLBAR_BUTTON_FIRST, IMG_TOOLBAR_MESSAGE, m_BCGToolbarImages.GetCount() );

	
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
		//switch (i)
		//{
		//case IMG_RUN_PROCESS:
		//case IMG_HISTORY_MONITOR:
		//case IMG_HELP:
		//	InsertSeparator( m_Buttons.GetCount() );
		//	break;
		//default:
		//	break;
		//}
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
	case IMG_TOOLBAR_LOGOUT:
		strTTText = "注销"; break;
	case IMG_TOOLBAR_MESSAGE:
		strTTText = "消息"; break;
	case IMG_TOOLBAR_REFRESH:
		strTTText = "刷新"; break;
	case IMG_TOOLBAR_FLEXWARE:
		strTTText = "过程建模"; break;
	case IMG_TOOLBAR_PORTAL:
		strTTText = "主页"; break;
	case IMG_TOOLBAR_SETTING:
		strTTText = "系统设置"; break;
	case IMG_TOOLBAR_ABOUT:
		strTTText = "关于"; break;
	//case IMG_SAVEAS:
	//	strTTText = "另存为"; break;
	//case IMG_INFO:
	//	strTTText = "属性"; break;
	//case IMG_PUBLISH:
	//	strTTText = "发布"; break;
	//case IMG_DATALINKER:
	//	strTTText = "数据映射"; break;
	//case IMG_RUN_PROCESS + 1:
	//	strTTText = "运行设计过程"; break;
	////case IMG_HISTORY_MONITOR + 2:
	////	strTTText = g_lpszViewTitleHistoryMonitor; break;
	//case IMG_PREVIEW + 2:
	//	strTTText = "分布式资源管理"; break;
	//case IMG_HELP + 3:
	//	strTTText = "帮助"; break;
// 	case IMG_ABOUT + 3:
// 		strTTText = "关于"; break;
	default:
		return FALSE;
	}
	return TRUE;
}

void CCxBCGPToolBar::MoveToRight()
{
	if(theApp.m_bCreate == FALSE) return;
	CRect rectLogin;
	CMainFrame* pMain = (CMainFrame*)theApp.GetMainWnd();
	if(
		!IsWindow(pMain->GetSafeHwnd())
		|| !IsWindow(pMain->m_wndLoginBar.GetSafeHwnd())
		) return;

	pMain->m_wndLoginBar.GetWindowRect(rectLogin);
	if(
		m_pParentDockBar == NULL
		|| !IsLeftOf( rectLogin )
		|| pMain->m_wndLoginBar.GetDockBarRow() != GetDockBarRow()
		) return;
	
	SetUserOP(FALSE);
	CRect rectBar;
	m_pParentDockBar->GetWindowRect(rectBar);
	CRect rectThis;
	GetWindowRect(rectThis);
	int iDisp = rectBar.Width() - rectLogin.Width() - rectThis.Width() - 20;
	m_pParentDockBar->MoveControlBar(this, 0, CPoint(iDisp, 0));
	m_pDockBarRow->FixupVirtualRects (false);
	SetUserOP(TRUE);
	
}

void CCxBCGPToolBar::OnAfterDock (CBCGPBaseControlBar* pBar, LPCRECT lpRect, BCGP_DOCK_METHOD dockMethod) 
{
	m_bFloating = FALSE;
	CBCGPBaseToolBar::OnAfterDock  (pBar, lpRect, dockMethod);

	if(m_pParentDockBar && dockMethod == BCGP_DM_DBL_CLICK || dockMethod == BCGP_DM_RECT)
	{
		MoveToRight();
	}

	if (GetParent() != NULL)
	{
		CRect rect;
		GetWindowRect (rect);
		GetParent()->ScreenToClient (rect);

		GetParent()->RedrawWindow (rect);
	}
}

//***********************************************************************************
BCGP_CS_STATUS CCxBCGPToolBar::IsChangeState (int nOffset, 
											 CBCGPBaseControlBar** ppTargetBar) const
{
	if(!IsUserOP()) return BCGP_CS_NOTHING;

	ASSERT_VALID (this);
	ASSERT (ppTargetBar != NULL);
	

	CPoint	ptMousePos;
	CRect	rectBarWnd;
	CRect	rectDockBarWnd;
	CRect	rectIntersect;

	CRect	rectVirtual;

	CPoint  ptDelta;

	GetCursorPos (&ptMousePos);

	GetWindowRect  (&rectBarWnd);
	GetVirtualRect (rectVirtual);

	// check whether the mouse is around a dock bar
	CBCGPBaseControlBar* pBaseBar = ControlBarFromPoint (ptMousePos, nOffset, FALSE,
											RUNTIME_CLASS (CBCGPDockBar)); 

	*ppTargetBar = DYNAMIC_DOWNCAST (CBCGPDockBar, pBaseBar);

	if (m_pParentDockBar != NULL)
	{
		// the mouse is around the dock bar, check the virtual rect
		m_pParentDockBar->GetWindowRect (&rectDockBarWnd);
		if (!rectIntersect.IntersectRect (rectDockBarWnd, rectVirtual))
		{
			return BCGP_CS_DOCK_IMMEDIATELY;
		}

		// there is some intersection of the virtual rectangle an the dock bar. 
		// special processing when horizontal bar is about to float in horizontal direction
		bool bTreatMouse = false; 
		if (m_pParentDockBar->IsHorizontal ())
		{

			if (rectVirtual.left < rectDockBarWnd.left && 
				rectDockBarWnd.left - rectVirtual.left > nOffset * 2 ||
				rectVirtual.right > rectDockBarWnd.right && 
				rectVirtual.right - rectDockBarWnd.right > nOffset * 2)
			{
				bTreatMouse = true;	
			}
		}
		else
		{
			if (rectVirtual.top < rectDockBarWnd.top && 
				rectDockBarWnd.top - rectVirtual.top > nOffset * 2 ||
				rectVirtual.bottom > rectDockBarWnd.bottom && 
				rectVirtual.bottom - rectDockBarWnd.bottom > nOffset * 2)
			{
				bTreatMouse = true;	
			}
			
		}

		if (bTreatMouse && !rectDockBarWnd.PtInRect (ptMousePos))
		{
			return BCGP_CS_DOCK_IMMEDIATELY;
		}
	}
	else
	{
		if (*ppTargetBar == NULL)
		{
			// the mouse is out of dock bar in either direction - keep the bar floating
			return BCGP_CS_NOTHING;
		}

		if (!CanBeDocked (*ppTargetBar))
		{
			// bar's style does not allow to dock the bar to this dock bar
			return BCGP_CS_NOTHING;
		}
		// the mouse is getting closer to a dock bar
		(*ppTargetBar)->GetWindowRect (&rectDockBarWnd);

		if (rectDockBarWnd.PtInRect (ptMousePos))
		{
			// the mouse is over the dock bar, the bar must be docked
			return BCGP_CS_DOCK_IMMEDIATELY;
		}

		// check on which side the mouse is relatively to the dock bar
		bool bMouseLeft = ptMousePos.x < rectDockBarWnd.left;
		bool bMouseRight = ptMousePos.x > rectDockBarWnd.right;
		bool bMouseTop  = ptMousePos.y < rectDockBarWnd.top;
		bool bMouseBottom = ptMousePos.y > rectDockBarWnd.bottom;

		double	dPixelsOnDock = nOffset;
		int		nMouseOffset  = 0;
		if (bMouseLeft)
		{
			dPixelsOnDock = ((rectBarWnd.right - ptMousePos.x) * 100. / 
								rectBarWnd.Width ()) / 100. * nOffset;
			nMouseOffset = rectDockBarWnd.left - ptMousePos.x;
			
		}
		else if (bMouseRight)
		{
			dPixelsOnDock = ((ptMousePos.x - rectBarWnd.left) * 100. / 
								rectBarWnd.Width ()) / 100. * nOffset;
			nMouseOffset = ptMousePos.x - rectDockBarWnd.right;
		}
		else if (bMouseTop)
		{
			dPixelsOnDock = ((rectBarWnd.bottom - ptMousePos.y) * 100. / 
								rectBarWnd.Height ()) / 100. * nOffset;
			nMouseOffset = rectDockBarWnd.top - ptMousePos.y;
		}
		else if (bMouseBottom)
		{
			dPixelsOnDock = ((ptMousePos.y - rectBarWnd.top) * 100. / 
								rectBarWnd.Height ()) / 100. * nOffset;
			nMouseOffset = ptMousePos.y - rectDockBarWnd.bottom;
		}

		if (nMouseOffset <= dPixelsOnDock)
		{
			return BCGP_CS_DOCK_IMMEDIATELY;
		}
	}

	return BCGP_CS_NOTHING;
}

BOOL CCxBCGPToolBar::CanFloat () const 
{
	if ( m_bLBDownForFloat )
		return FALSE;
	else return CBCGPToolBar::CanFloat();
}

void CCxBCGPToolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLBDownForFloat = TRUE;
	CBCGPToolBar::OnLButtonDown( nFlags, point );

}


void CCxBCGPToolBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bLBDownForFloat = FALSE;
	CBCGPToolBar::OnLButtonUp( nFlags, point );
}

void CCxBCGPToolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CBCGPToolBar::OnMouseMove( nFlags, point );
}

void CCxBCGPToolBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_bLBDownForFloat = TRUE;
	CBCGPToolBar::OnLButtonDblClk( nFlags, point );
	m_bLBDownForFloat = FALSE;
}



BOOL CCxBCGPToolBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	 ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	 return TRUE;

	//return CBCGPToolBar::OnSetCursor(pWnd, nHitTest, message);
}
