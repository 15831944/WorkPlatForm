#include "StdAfx.h"
#include "GDIPlusInit.h"
#pragma comment(lib, "gdiplus.lib")
#include "FileFunc.h"
#include <winnt.h>
#include <atlstr.h>
#include "ConvertData.h"

static BOOL g_bInited = FALSE;

CGDIPlusInit::CGDIPlusInit(void)
{
}


CGDIPlusInit::~CGDIPlusInit(void)
{
}

BOOL CGDIPlusInit::InitGDIPlus(void)
{
	if ( g_bInited ) return TRUE;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	g_bInited = TRUE;
	return TRUE;
}

BOOL CGDIPlusInit::ReleaseGDIPlus(void)
{
	if ( !g_bInited ) return TRUE;
	GdiplusShutdown(m_gdiplusToken);
	g_bInited = FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

Bitmap * ScaleBitmap( Bitmap *& pSrc, UINT nCx, UINT nCy, BOOL bCreate )
{
	if (pSrc == NULL) return NULL;
	if (nCx < 1 || nCy < 1) return NULL;

	Bitmap * pResult = NULL;

	if (pSrc->GetWidth() == nCx && pSrc->GetHeight() == nCy)
	{
		if (bCreate)
		{
			return pSrc->Clone( 0, 0, nCx, nCy, pSrc->GetPixelFormat() );
		}
		else
		{
			return pSrc;
		}
	}

	pResult = new Bitmap( nCx, nCy, pSrc->GetPixelFormat() );
	if( pResult == NULL ) return NULL;

	Graphics * g = Graphics::FromImage(pResult);
	if ( g == NULL)
	{
		delete pResult; pResult = NULL;
		return NULL;
	}

	// use the best interpolation mode
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	Status status = g->DrawImage(pSrc, 0, 0, nCx, nCy);
	
	delete g; g= NULL;
	
	if ( Ok != status )
	{
		delete pResult; pResult = NULL;
		return NULL;
	}

	if (!bCreate)
	{
		delete pSrc;
		pSrc = pResult;
	}

	return pResult;
}

/*
gamma 参数的典型值在 1.0 到 2.2 之间；但在某些情况下，0.1 到 5.0 范围内的值也很有用。
imageAttr.SetGamma 参数值越大，图像越暗，反之则越亮
*/
Bitmap * HighlightBitmap(Bitmap * pSrc, float fGamma, BOOL bCreate )
{
	if (pSrc == NULL) return NULL;
	if (fGamma <= 0.0f) return NULL;

	RectF rc(0.0f, 0.0f, (float)pSrc->GetWidth(), (float)pSrc->GetHeight());
	if ( rc.IsEmptyArea()) return NULL;

	Bitmap * pResult = !bCreate ? pSrc : new Bitmap( (int)rc.Width, (int)rc.Height, pSrc->GetPixelFormat() );
	if( pResult == NULL ) return NULL;

	Graphics * g = Graphics::FromImage(pResult);
	if ( g == NULL)
	{
		if (bCreate) { delete pResult; pResult = NULL; }
		return NULL;
	}
	
	ImageAttributes imageAttr;

	//////////////////////////////////////////////////////////////////////////
#if TRUE
	imageAttr.SetGamma( 1/fGamma );
#else
	Gdiplus:: ColorMatrix HotMat = 
	{1.05f, 0.00f, 0.00f, 0.00f,0.00f,
	0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
	0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
	0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
	0.05f, 0.05f, 0.05f, 0.00f, 1.00f};
	imageAttr.SetColorMatrix(&HotMat);
#endif
	//////////////////////////////////////////////////////////////////////////

	Status status = g->DrawImage(pSrc, rc, 0, 0, (float)pSrc->GetWidth(), (float)pSrc->GetHeight()
		,Gdiplus:: UnitPixel, &imageAttr);

	delete g; g = NULL;

	if ( Ok != status )
	{
		if (bCreate) { delete pResult; pResult = NULL; }
		return NULL;
	}

	return pResult;
}

Image * LoadImage(LPCTSTR lpszPath)
{
	CStringW strPathW;
	CString strPath = lpszPath == NULL ? "" : lpszPath;

	if (strPath.IsEmpty()) return NULL;
	if ( !IsAccessibleFile(strPath) ) return NULL;
	strPathW = strPath;

	Image * pImg = Image::FromFile( (WCHAR *)(LPCWCH)strPathW );
	if (pImg == NULL) return NULL;
	return pImg;
}



Bitmap * GetNewBitmap(LPCTSTR lpszPath)
{
	Image * pImg = LoadImage(lpszPath);
	if (pImg == NULL) return NULL;
	if (pImg->GetHeight() < 1 || pImg->GetWidth() < 1)
	{
		delete pImg; pImg = NULL;
		return NULL;
	}

	Bitmap * pResult = new Bitmap( pImg->GetWidth(), pImg->GetHeight(), PixelFormat32bppARGB );
	if (pResult == NULL)
	{
		delete pImg; pImg = NULL;
		return NULL;
	}
	Graphics g(pResult);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	Point ImgPoints[] = {
		Point(0, 0) //原点
		, Point(pImg->GetWidth(), 0)
		, Point(0, pImg->GetHeight())
	};
	g.DrawImage(pImg, ImgPoints, 3);

	delete pImg; pImg = NULL;
	return pResult;
}

void DrawHighlight( Image * pImage, Graphics * pGraphics, RectF rcDraw, float fGamma /*= 2.2f */ )
{
	if (pGraphics == NULL) return;
	if (pImage == NULL) return;

	ImageAttributes imageAttr;
	//////////////////////////////////////////////////////////////////////////
#if FALSE
	imageAttr.SetGamma( 1/fGamma );
#else
	Gdiplus:: ColorMatrix HotMat = 
	{1.05f, 0.00f, 0.00f, 0.00f,0.00f,
	0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
	0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
	0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
	0.05f, 0.05f, 0.05f, 0.00f, 1.00f};
	imageAttr.SetColorMatrix(&HotMat);
#endif
	//////////////////////////////////////////////////////////////////////////

	RectF rc (0.0f, 0.0f, (float)pImage->GetWidth(), (float)pImage->GetHeight());
	pGraphics->DrawImage( pImage, rcDraw, rc.GetLeft(), rc.GetTop(), rc.Width, rc.Height
		, Gdiplus:: UnitPixel, &imageAttr );
}

void DrawSudoku(Image * pImage, Graphics * pGraphics, RectF& rcDraw
	, UINT nLeft, UINT nTop, UINT nRight, UINT nBottom )
{
	if (pGraphics == NULL) return;
	if (pImage == NULL) return;

	UINT cx = pImage->GetWidth();
	UINT cy = pImage->GetHeight();

	if ( nLeft + nRight > cx )
	{
		nLeft = nRight = 0;
	}
	if ( nTop + nBottom > cy )
	{
		nTop = nBottom = 0;
	}

	//左上角
	if (nLeft > 0 && nTop > 0 && nTop < cy && nLeft < cx)
	{
		pGraphics->DrawImage( pImage
			, rcDraw.GetLeft(), rcDraw.GetTop()
			, 0.0f, 0.0f, (float)nLeft, (float)nTop
			, Gdiplus:: UnitPixel );
	}
	//右上角
	if (nRight > 0 && nTop > 0 && nTop < cy && nRight < cx)
	{
		pGraphics->DrawImage( pImage
			, rcDraw.GetRight() - (float)nRight, rcDraw.GetTop() 
			, (float)cx - (float)nRight, 0.0f, (float)nRight, (float)nTop
			, Gdiplus:: UnitPixel );
	}
	//右下角
	if (nRight > 0 && nBottom > 0 && nBottom < cy && nRight < cx)
	{
		pGraphics->DrawImage( pImage
			, rcDraw.GetRight() - (float)nRight, rcDraw.GetBottom() - (float)nBottom
			, (float)cx - (float)nRight, (float)cy - (float)nBottom, (float)nRight, (float)nBottom
			, Gdiplus:: UnitPixel );
	}
	//左下角
	if (nLeft > 0 && nBottom > 0 && nBottom < cy && nLeft < cx)
	{
		pGraphics->DrawImage( pImage
			, rcDraw.GetLeft(), rcDraw.GetBottom() - (float)nBottom
			, 0.0f, (float)cy - (float)nBottom , (float)nLeft, (float)nBottom
			, Gdiplus:: UnitPixel );
	}
	
	ImageAttributes ImgAtt;
	ImgAtt.SetWrapMode(WrapModeTileFlipXY);

	//左边
	if (nLeft > 0 && cy - nTop - nBottom > 0)
	{
		RectF rc(rcDraw.GetLeft(), rcDraw.GetTop() + (float)nTop
			, (float)nLeft, rcDraw.Height - (float)nTop - (float)nBottom );
		pGraphics->DrawImage( pImage
			, rc
			, 0.0f, (float)nTop, (float)nLeft, (float)cy - (float)nTop - (float)nBottom
			, UnitPixel, &ImgAtt );
	}
	//上边
	if (nTop > 0 && cx - nLeft - nRight > 0)
	{
		RectF rc(rcDraw.GetLeft() + (float)nLeft, rcDraw.GetTop()
			, rcDraw.Width - (float)nLeft - (float)nRight, (float)nTop );
		pGraphics->DrawImage( pImage
			, rc
			, (float)nLeft, 0.0f, (float)cx - (float)nLeft - (float)nRight, (float)nTop
			, UnitPixel, &ImgAtt );
	}
	//右边
	if (nRight > 0 && cy - nTop - nBottom > 0)
	{
		RectF rc(rcDraw.GetRight() - (float)nRight, rcDraw.GetTop() + (float)nTop
			, (float)nRight, rcDraw.Height - (float)nTop - (float)nBottom );
		pGraphics->DrawImage( pImage
			, rc
			, (float)cx - (float)nRight, (float)nTop, (float)nRight, (float)cy - (float)nTop - (float)nBottom
			, UnitPixel, &ImgAtt );
	}
	//下边
	if (nBottom > 0 && cx - nLeft - nRight > 0)
	{
		RectF rc(rcDraw.GetLeft() + (float)nLeft, rcDraw.GetBottom() - (float)nBottom
			, rcDraw.Width - (float)nLeft - (float)nRight, (float)nBottom );
		pGraphics->DrawImage( pImage
			, rc
			, (float)nLeft, (float)cy - (float)nBottom, (float)cx - (float)nLeft - (float)nRight, (float)nBottom
			, UnitPixel, &ImgAtt );

		CString str;
		str.Format("下边:%0.4f", rc.GetTop());
		OutputDebugString(str);
	}
	//中间
	{
		RectF rc = rcDraw;
		if (nLeft > 0) rc.X += (float)nLeft;
		if (nTop > 0) rc.Y += (float)nTop;
		if (nRight > 0) rc.Width -= (float)nRight;
		if (nBottom > 0) rc.Height -= (float)nBottom;
		rc.Width -= (float)nLeft;
		rc.Height -= (float)nTop;

		pGraphics->DrawImage( pImage
			, rc
			, (float)nLeft, (float)nTop, (float)(cx - nLeft - nRight), (float)(cy - nBottom - nTop)
			, UnitPixel, &ImgAtt );
	}
}

Bitmap * GetNewSudokuBitmap(LPCTSTR lpszPath, int nCx, int nCy)
{
	Image * pImg = LoadImage(lpszPath);
	if (pImg == NULL) return NULL;
	if (pImg->GetHeight() < 1 || pImg->GetWidth() < 1)
	{
		delete pImg; pImg = NULL;
		return NULL;
	}

	Bitmap * pResult = new Bitmap( nCx, nCy, PixelFormat32bppARGB );
	if (pResult == NULL)
	{
		delete pImg; pImg = NULL;
		return NULL;
	}
	Graphics g(pResult);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	RectF rc(0.0f, 0.0f, (float)nCx, (float)nCy);
	DrawSudoku(pImg, &g, rc);

	delete pImg; pImg = NULL;
	return pResult;
}


void PrepDrawByGdiPlus(CDC * pDC, Bitmap*& pCanvas, Graphics*& pGrap, RectF& rcClient, COLORREF crBg)
{
	//rcClient.X = rcClient.Y = rcClient.Width = rcClient.Height;
	pCanvas = NULL;
	pGrap = NULL;

	if ( pDC == NULL || pDC->GetSafeHdc() == NULL ) return;

	CRect rc;
	pDC->GetClipBox(&rc);
	//CWnd::FromHandle(hWnd)->GetClientRect(&rc);
	pCanvas = new Bitmap(rc.Width(), rc.Height(), PixelFormat32bppARGB);
	pGrap = new Graphics(pCanvas);
	//设定透明模式，和点阵模式，如果不设定会出现毛边，造成图形边界不是很平滑
	pGrap->SetSmoothingMode(SmoothingModeAntiAlias);
	pGrap->SetPixelOffsetMode(PixelOffsetModeHighQuality);
	pGrap->SetTextRenderingHint(TextRenderingHintAntiAlias);
	//擦除背景
	rcClient = RectF((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
	SolidBrush br(crBg);
	pGrap->FillRectangle(&br, rcClient);
};

void DrawOnGDI( CDC * pDC, Bitmap*& pCanvas, Graphics*& pGrap )
{
	Graphics g( pDC->GetSafeHdc() );
	CachedBitmap cachedBmp(pCanvas, &g);
	g.DrawCachedBitmap(&cachedBmp, 0, 0);
	g.ReleaseHDC( pDC->GetSafeHdc() );

	delete pGrap; pGrap = NULL;
	delete pCanvas; pCanvas = NULL;
}