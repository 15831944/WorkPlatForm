#include "StdAfx.h"
#include "CxBCGPButtonPlb.h"
#include "Resource.h"

static const COLORREF clrDefault = (COLORREF) -1;

IMPLEMENT_DYNAMIC(CCxBCGPButtonPlb, CCxBCGPButton)

CCxBCGPButtonPlb::CCxBCGPButtonPlb(void)
{
	//__super::SetImage(IDB_PNG1, IDB_PNG1, IDB_PNG1);
	//SetImage(GetDlgParamSetResImageData( "DlgParamSet_Btn_Delete" ), GetDlgParamSetResImageData( "DlgParamSet_Btn_DeleteGray" ));

	CBCGPControlRendererParams params (UINT(0), CRect (0, 0, 110, 22), CRect (3, 4, 3, 3));
	m_renderBtn.CreateCustom(params, GetExecDir() + "\\Res\\DlgParamSet\\push.png");
}


CCxBCGPButtonPlb::~CCxBCGPButtonPlb(void)
{
}


//****************************************************************************
static HBITMAP ButtonLoadPNG (LPCSTR strPath)
{
	if (strPath == 0)
	{
		return NULL;
	}


	HBITMAP hbmp = NULL;

	//-----------------------------
	// Try to load PNG image first:
	//-----------------------------
	CBCGPPngImage pngImage;
	if (pngImage.LoadFromFile(strPath))
	{
		hbmp = (HBITMAP) pngImage.Detach ();
	}

	return hbmp;
}


void CCxBCGPButtonPlb::SetImage(CCxStaticRes * pImg, CCxStaticRes * pImgDisabled)
{

	HBITMAP hbmp = ButtonLoadPNG (pImg->m_strPath);
	HBITMAP hbmpHot = ButtonLoadPNG (pImg->m_strPath);
	HBITMAP hbmpDisabled = ButtonLoadPNG (pImgDisabled->m_strPath);


	CCxBCGPButton::SetImage(hbmp, 1, hbmpHot, 0, hbmpDisabled);

}

void CCxBCGPButtonPlb::DoDrawItem( CDC* pDCPaint, CRect rectClient, UINT itemState )
{
	CBCGPMemDC memDC (*pDCPaint, this);
	CDC* pDC = &memDC.GetDC ();

	m_clrText = clrDefault;

	BOOL bDefaultDraw = TRUE;
	BOOL bDefaultCheckRadio = (m_bCheckButton || m_bRadioButton) && (GetStyle () & BS_PUSHLIKE) == 0;

	if (((m_bVisualManagerStyle && !m_bDontSkin) || bDefaultCheckRadio) && !m_bTransparent)
	{
		if (bDefaultCheckRadio)
		{
			globalData.DrawParentBackground (this, pDC);

			//----------------
			// Draw check box:
			//----------------
			CRect rectCheck = rectClient;
			const CSize sizeCheck = CBCGPVisualManager::GetInstance ()->GetCheckRadioDefaultSize ();

			if (m_bIsLeftText)
			{
				rectCheck.left = rectCheck.right - sizeCheck.cx;
			}
			else
			{
				rectCheck.right = rectCheck.left + sizeCheck.cx;
			}

			BOOL bMultiLine = (GetStyle () & BS_MULTILINE) != 0;

			if (bMultiLine)
			{
				if ((GetStyle () & BS_VCENTER) == BS_VCENTER)
				{
					rectCheck.top = rectCheck.CenterPoint ().y - sizeCheck.cy / 2;
				}
				else if ((GetStyle () & BS_BOTTOM) == BS_BOTTOM)
				{
					rectCheck.top = rectCheck.bottom - sizeCheck.cy;
				}
				else if ((GetStyle () & BS_TOP) == BS_TOP)
				{
				}
				else
				{
					rectCheck.top = rectCheck.CenterPoint ().y - sizeCheck.cy / 2;
				}
			}
			else
			{
				rectCheck.top = rectCheck.CenterPoint ().y - sizeCheck.cy / 2;
			}

			rectCheck.bottom = rectCheck.top + sizeCheck.cy;

			const BOOL bIsHighlighted = IsHighlighted () && IsWindowEnabled ();

			if (m_bCheckButton)
			{
				int nCheck = IsChecked () ? 1 : 0;

				if (m_b3State && m_bIndeterminate)
				{
					nCheck = 2;
				}

				CBCGPVisualManager::GetInstance ()->OnDrawCheckBoxEx (pDC,
					rectCheck, nCheck,
					bIsHighlighted, IsPressed () && bIsHighlighted, IsWindowEnabled ());
			}
			else
			{
				CBCGPVisualManager::GetInstance ()->OnDrawRadioButton (pDC,
					rectCheck, IsChecked () || (IsPressed () && bIsHighlighted),
					bIsHighlighted, IsPressed () && bIsHighlighted, IsWindowEnabled ());
			}

			if (m_bIsLeftText)
			{
				rectClient.right = rectCheck.left;
				rectClient.left++;
			}
			else
			{
				rectClient.left = rectCheck.right + 1;
			}

			bDefaultDraw = FALSE;
		}
		else
		{
			if (OnDrawPushButton (pDC, rectClient, this, m_clrText)/*CBCGPVisualManager::GetInstance ()->OnDrawPushButton (pDC, rectClient, this, m_clrText)*/)
			{
				if (m_clrFace != -1 && globalData.m_nBitsPerPixel > 8)
				{
					CBCGPDrawManager dm(*pDC);

					CRect rectHighlight = rectClient;
					int nPercentage = 50;
					BOOL bIsFocused = (itemState & ODS_FOCUS);

					if (IsHighlighted() || IsPressed() || IsChecked() || bIsFocused)
					{
						rectHighlight.DeflateRect(2, 2);
						nPercentage = 20;
					}

					dm.HighlightRect(rectHighlight, nPercentage, (COLORREF)-1, 0, m_clrFace);
				}

				rectClient.DeflateRect (2, 2);
				bDefaultDraw = FALSE;
			}
		}
	}

	if (bDefaultDraw)
	{
		OnFillBackground (pDC, rectClient);
		OnDrawBorder (pDC, rectClient, itemState);
	}

	//---------------------
	// Draw button content:
	//---------------------
	OnDraw (pDC, rectClient, itemState);

	if ((itemState & ODS_FOCUS) && m_bDrawFocus)
	{
		OnDrawFocusRect (pDC, rectClient);
	}
}

BOOL CCxBCGPButtonPlb::OnDrawPushButton (CDC* pDC, CRect rect, CBCGPButton* pButton, COLORREF& clrText)
{
// 	if (!CanDrawImage ())
// 	{
// 		return CBCGPVisualManager2003::OnDrawPushButton (pDC, rect, pButton, clrText);
// 	}
// 
// 	ASSERT_VALID (pDC);
// 	ASSERT_VALID (pButton);

	int index = 0;

	BOOL bDisabled    = !pButton->IsWindowEnabled ();
	BOOL bFocused     = pButton->GetSafeHwnd () == ::GetFocus ();
	BOOL bDefault     = pButton->IsDefaultButton ();
	BOOL bPressed     = pButton->IsPressed ();
	BOOL bChecked     = pButton->IsChecked ();
	BOOL bHighlighted = pButton->IsHighlighted ();

	//bHighlighted |= bFocused;

	if (bDisabled)
	{
		index = 6;
	}
	else
	{
		if (bChecked)
		{
			index = 3;
		}
		else
		{
			if (bDefault && !bHighlighted)
			{
				index = 5;
			}
		}

		if (bPressed)
		{
			if (bHighlighted)
			{
				index = 2;
			}
		}
		else if (bHighlighted)
		{
			index++;
		}
	}

	globalData.DrawParentBackground (pButton, pDC);
	m_renderBtn.Draw (pDC, rect, index);

// 	clrText = globalData.clrBarText;
// 
// 	if (bDisabled)
// 	{
// 		clrText = m_clrRibbonBarBtnTextDisabled;
// 	}
// 	else if (bHighlighted)
// 	{
// 		clrText = m_clrRibbonBarBtnTextHighlighted;
// 	}

	return TRUE;
}

void CCxBCGPButtonPlb::Reset()
{
	//m_bHighlighted = FALSE;
	m_bCaptured = FALSE;
// 	Invalidate ();
// 	UpdateWindow ();
}

IMPLEMENT_DYNAMIC(CCxBCGPControlRendererButtonPlb, CBCGPControlRenderer)

CCxBCGPControlRendererButtonPlb::CCxBCGPControlRendererButtonPlb()
{

}

CCxBCGPControlRendererButtonPlb::~CCxBCGPControlRendererButtonPlb()
{

}


BOOL CCxBCGPControlRendererButtonPlb::CreateCustom( const CBCGPControlRendererParams& params, CString sPngPath, BOOL bFlipvert /*= FALSE*/ )
{
	CleanUp ();

	m_Params = params;

	m_Bitmap.SetImageSize (m_Params.m_rectImage.Size ());
	m_Bitmap.SetPreMultiplyAutoCheck (m_Params.m_bPreMultiplyCheck);
	m_Bitmap.SetMapTo3DColors (FALSE);
	//m_Bitmap.LoadStr (lpszResID);
	m_Bitmap.Load(sPngPath);

	if (bFlipvert)
	{
		m_Bitmap.MirrorVert ();
	}

	if (m_Params.m_clrTransparent != CLR_DEFAULT)
	{
		m_Bitmap.SetTransparentColor (m_Params.m_clrTransparent);
	}

	if (m_Params.m_clrBase != (COLORREF)-1 &&
		m_Params.m_clrTarget != (COLORREF)-1)
	{
		m_Bitmap.AddaptColors (m_Params.m_clrBase, m_Params.m_clrTarget);
	}

	if (CBCGPToolBarImages::IsRTL () && m_Bitmap.GetImageWell () != NULL &&
		m_Params.m_clrTransparent == CLR_DEFAULT)
	{
		BITMAP bmp;
		if (::GetObject (m_Bitmap.GetImageWell (), sizeof (BITMAP), &bmp) != 0)
		{
			if (bmp.bmBitsPixel == 32)
			{
				Mirror ();
			}
		}
	}

	if (m_Params.m_rectSides.IsRectNull ())
	{
		m_Params.m_rectSides = m_Params.m_rectCorners;
	}

	if (m_Params.m_rectInter.IsRectNull ())
	{
		m_Params.m_rectInter = CRect (CPoint (0, 0), m_Params.m_rectImage.Size ());
		m_Params.m_rectInter.left   += m_Params.m_rectCorners.left;
		m_Params.m_rectInter.top    += m_Params.m_rectCorners.top;
		m_Params.m_rectInter.right  -= m_Params.m_rectCorners.right;
		m_Params.m_rectInter.bottom -= m_Params.m_rectCorners.bottom;
	}

	if (bFlipvert)
	{
		long temp;
		temp = m_Params.m_rectCorners.top;
		m_Params.m_rectCorners.top = m_Params.m_rectCorners.bottom;
		m_Params.m_rectCorners.bottom = temp;

		temp = m_Params.m_rectSides.top;
		m_Params.m_rectSides.top = m_Params.m_rectSides.bottom;
		m_Params.m_rectSides.bottom = temp;

		long height = m_Params.m_rectImage.Height ();
		temp = m_Params.m_rectInter.top;
		m_Params.m_rectInter.top = height - m_Params.m_rectInter.bottom;
		m_Params.m_rectInter.bottom = height - temp;
	}

	return TRUE;
}
