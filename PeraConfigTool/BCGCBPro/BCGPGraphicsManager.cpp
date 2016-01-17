//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPGraphicsManager.cpp: implementation of the CBCGPGraphicsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPMath.h"
#include "BCGPGraphicsManager.h"
#include "BCGPGraphicsManagerGDI.h"
#include "BCGPGraphicsManagerD2D.h"
#include "BCGPDrawManager.h"
#if (!defined _BCGSUITE_) && (!defined _BCGPCHART_STANDALONE)
#include "BCGPColorBar.h"
#endif
#include <float.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CBCGPGraphicsResource, CObject)
IMPLEMENT_DYNAMIC(CBCGPGeometry, CBCGPGraphicsResource)
IMPLEMENT_DYNAMIC(CBCGPRectangleGeometry, CBCGPGeometry)
IMPLEMENT_DYNAMIC(CBCGPRoundedRectangleGeometry, CBCGPGeometry)
IMPLEMENT_DYNAMIC(CBCGPEllipseGeometry, CBCGPGeometry)
IMPLEMENT_DYNAMIC(CBCGPPolygonGeometry, CBCGPGeometry)
IMPLEMENT_DYNAMIC(CBCGPSplineGeometry, CBCGPGeometry)
IMPLEMENT_DYNAMIC(CBCGPComplexGeometry, CBCGPGeometry)
IMPLEMENT_DYNAMIC(CBCGPGraphicsManager, CObject)
IMPLEMENT_DYNAMIC(CBCGPLineSegment, CObject)
IMPLEMENT_DYNAMIC(CBCGPBezierSegment, CObject)
IMPLEMENT_DYNAMIC(CBCGPArcSegment, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPGraphicsManager::CBCGPGraphicsManager()
{
	m_Type = (BCGP_GRAPHICS_MANAGER)-1;
	m_pOriginal = NULL;
	m_pImageDest = NULL;
	m_nSupportedFeatures = 0;
	m_bIsTransparentGradient = TRUE;
}

CBCGPGraphicsManager::~CBCGPGraphicsManager()
{
}

CBCGPGraphicsManager::CBCGPGraphicsManager(const CBCGPRect& rectDest, CBCGPImage* pImageDest) :
	m_pImageDest(pImageDest),
	m_rectDest(rectDest)
{
	m_Type = (BCGP_GRAPHICS_MANAGER)-1;
	m_pOriginal = NULL;
	m_bIsTransparentGradient = TRUE;
	m_nSupportedFeatures = 0;
}

CBCGPGraphicsManager* CBCGPGraphicsManager::CreateInstance(BCGP_GRAPHICS_MANAGER manager, BOOL bFallback, CBCGPGraphicsManagerParams* pParams)
{
	if (manager == BCGP_GRAPHICS_MANAGER_DEFAULT)
	{
		manager = BCGP_GRAPHICS_MANAGER_D2D;
	}

	CBCGPGraphicsManager* pGM = NULL;

	switch (manager)
	{
	case BCGP_GRAPHICS_MANAGER_GDI:
		pGM = new CBCGPGraphicsManagerGDI(NULL, TRUE, pParams);
		break;

	case BCGP_GRAPHICS_MANAGER_D2D:
		pGM = new CBCGPGraphicsManagerD2D(NULL, TRUE, pParams);

		if (!pGM->IsValid())
		{
			delete pGM;
			pGM = NULL;

			if (bFallback)
			{
				pGM = new CBCGPGraphicsManagerGDI(NULL, TRUE, pParams);;
			}
		}
		break;
	}

	return pGM;
}

void CBCGPGraphicsManager::CleanResources()
{
	for (POSITION pos = m_lstRes.GetHeadPosition(); pos != NULL;)
	{
		CBCGPGraphicsResource* pRes = m_lstRes.GetNext(pos);
		ASSERT_VALID(pRes);

		pRes->Destroy(FALSE);
	}

	m_lstRes.RemoveAll();
}

void CBCGPGraphicsManager::Detach(CBCGPGraphicsResource* pRes)
{
	ASSERT_VALID(pRes);

	if (pRes->IsTemporary())
	{
		return;
	}

	POSITION pos = m_lstRes.Find(pRes);
	if (pos != NULL)
	{
		m_lstRes.RemoveAt(pos);
	}
}

double CBCGPGraphicsManager::PrepareBevelColors(const CBCGPColor& color, CBCGPColor& colorLight, CBCGPColor& colorDark)
{
	colorDark = color;
	colorDark.MakeDarker(.1);

	colorLight = color;
	colorLight.MakePale();

	return 3.;
}

void CBCGPGraphicsManager::DrawPie(
	const CBCGPEllipse& ellipseSrc, 
	double dblStartAngle, double dblFinishAngle, 
	const CBCGPBrush& brFill, const CBCGPBrush& brLine,
	double dblOffsetFromCenter)
{
	DrawDoughnut(ellipseSrc, 0., dblStartAngle, dblFinishAngle, brFill, brLine, dblOffsetFromCenter);
}

void CBCGPGraphicsManager::DrawDoughnut(
	const CBCGPEllipse& ellipseSrc, 
	double dblHolePerc,	/* 0 - 1 */
	double dblStartAngle, double dblFinishAngle, 
	const CBCGPBrush& brFill, const CBCGPBrush& brLine,
	double dblOffsetFromCenter)
{
	if (ellipseSrc.radiusX < 1. || ellipseSrc.radiusY < 1.)
	{
		return;
	}

	const CBCGPSize sizeHole(ellipseSrc.radiusX * dblHolePerc, ellipseSrc.radiusY * dblHolePerc);

	const BOOL bIsFullEllipse = fabs(bcg_normalize_deg(dblStartAngle) - bcg_normalize_deg(dblFinishAngle)) < .1f;

	if (bIsFullEllipse)
	{
		if (!sizeHole.IsEmpty())
		{
			CBCGPEllipseGeometry g1(ellipseSrc);

			CBCGPEllipse ellipseHole = ellipseSrc;
			ellipseHole.radiusX = sizeHole.cx;
			ellipseHole.radiusY = sizeHole.cy;

			CBCGPEllipseGeometry g2(ellipseHole);

			CBCGPGeometry geometry;
			CombineGeometry(geometry, g1, g2, RGN_XOR);

			if (brFill.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
			{
				const double dblBevelRatio = .95;

				CBCGPEllipse ellipseBevel = ellipseSrc;
				ellipseBevel.radiusX *= dblBevelRatio;
				ellipseBevel.radiusY *= dblBevelRatio;

				CBCGPEllipseGeometry geometryBevel(ellipseBevel);

				CBCGPGeometry geometryInternal;
				CombineGeometry(geometryInternal, geometry, geometryBevel, RGN_AND);

				OnFillGeometryBevel(brFill, geometry, geometryInternal);
			}
			else
			{
				FillGeometry(geometry, brFill);
			}

			DrawEllipse(ellipseHole, brLine);
		}
		else
		{
			FillEllipse(ellipseSrc, brFill);
		}

		DrawEllipse(ellipseSrc, brLine);
		return;
	}

	double angleCos1 = cos(bcg_deg2rad(dblStartAngle));
	double angleSin1 = sin(bcg_deg2rad(dblStartAngle));

	double angleCos2 = cos(bcg_deg2rad(dblFinishAngle));
	double angleSin2 = sin(bcg_deg2rad(dblFinishAngle));

	BOOL bIsLargeArc = fabs(dblFinishAngle - dblStartAngle) >= 180.;
	BOOL bIsClockwise = dblStartAngle > dblFinishAngle;

	CBCGPEllipse ellipse = ellipseSrc;

	if (dblOffsetFromCenter != 0. && !bIsFullEllipse)
	{
		double angleMid = .5 * (bcg_deg2rad(dblStartAngle) + bcg_deg2rad(dblFinishAngle));

		ellipse.point.x += dblOffsetFromCenter * cos(angleMid);
		ellipse.point.y -= dblOffsetFromCenter * sin(angleMid);
	}

	CBCGPComplexGeometry shape;

	CBCGPPoint ptStart = CBCGPPoint(
		ellipse.point.x + angleCos1 * sizeHole.cx, 
		ellipse.point.y - angleSin1 * sizeHole.cy);

	shape.SetStart(ptStart);
	
	shape.AddLine(CBCGPPoint(
		ellipse.point.x + angleCos1 * ellipse.radiusX, 
		ellipse.point.y - angleSin1 * ellipse.radiusY));

	shape.AddArc(
		CBCGPPoint(ellipse.point.x + angleCos2 * ellipse.radiusX, ellipse.point.y - angleSin2 * ellipse.radiusY),
		CBCGPSize(ellipse.radiusX, ellipse.radiusY), bIsClockwise, bIsLargeArc);

	if (!sizeHole.IsEmpty())
	{
		shape.AddLine(CBCGPPoint(
			ellipse.point.x + angleCos2 * sizeHole.cx, 
			ellipse.point.y - angleSin2 * sizeHole.cy));

		shape.AddArc(ptStart, sizeHole, !bIsClockwise, bIsLargeArc);
	}

	m_rectCurrGradient = ellipseSrc;

	if (brFill.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		CBCGPComplexGeometry shapeInternal;
		double nDepth = min(ellipse.radiusX, ellipse.radiusY) * .07;

		ptStart = CBCGPPoint(
			ellipse.point.x + angleCos1 * sizeHole.cx, 
			ellipse.point.y - angleSin1 * sizeHole.cy);

		shapeInternal.SetStart(ptStart);

		double rX = ellipse.radiusX - nDepth;
		double rY = ellipse.radiusY - nDepth;
		
		shapeInternal.AddLine(CBCGPPoint(
			ellipse.point.x + angleCos1 * rX, 
			ellipse.point.y - angleSin1 * rY));

		shapeInternal.AddArc(
			CBCGPPoint(ellipse.point.x + angleCos2 * rX, ellipse.point.y - angleSin2 * rY),
			CBCGPSize(rX, rY), bIsClockwise, bIsLargeArc);

		if (!sizeHole.IsEmpty())
		{
			shapeInternal.AddLine(CBCGPPoint(
				ellipse.point.x + angleCos2 * sizeHole.cx, 
				ellipse.point.y - angleSin2 * sizeHole.cy));

			shapeInternal.AddArc(ptStart, sizeHole, !bIsClockwise, bIsLargeArc);
		}

		OnFillGeometryBevel(brFill, shape, shapeInternal);
	}
	else
	{
		FillGeometry(shape, brFill);
	}

	DrawGeometry(shape, brLine);
	m_rectCurrGradient.SetRectEmpty();
}

void CBCGPGraphicsManager::OnFillGeometryBevel(const CBCGPBrush& brFill, const CBCGPGeometry& shape, const CBCGPGeometry& shapeInternal)
{
	CBCGPColor colorLight, colorDark;
	PrepareBevelColors(brFill.GetColor(), colorLight, colorDark);

	FillGeometry(shape, 
		CBCGPBrush(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, brFill.GetOpacity()));

	FillGeometry(shapeInternal, 
		CBCGPBrush(brFill.GetColor(), colorLight, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, brFill.GetOpacity()));
}

static void Create3DDoughnutSideShape(CBCGPComplexGeometry& shape,
								 const CBCGPPoint& pt,
								 double dblStartAngle,
								 double dblFinishAngle,
								 const CBCGPSize& szRadiusIn,
								 const CBCGPPoint& ptCenter,
								 double dblHeight)
{
	if (pt.y < ptCenter.y)
	{
		return;
	}

	CBCGPSize szRadius = szRadiusIn;
	szRadius.cx = max(0, szRadius.cx);
	szRadius.cy = max(0, szRadius.cy);

	CBCGPPoint ptSide = ptCenter;

	BOOL bIsClockWise = FALSE;

	if (dblStartAngle > dblFinishAngle)
	{
		ptSide.x += szRadius.cx;
		bIsClockWise = TRUE;
	}
	else
	{
		ptSide.x -= szRadius.cx;
		bIsClockWise = FALSE;
	}

	shape.SetStart(CBCGPPoint(ptSide.x, ptSide.y + dblHeight));

	shape.AddArc(CBCGPPoint(pt.x, pt.y + dblHeight), szRadius, bIsClockWise, FALSE);
	shape.AddLine(pt);
	shape.AddArc(ptSide, szRadius, !bIsClockWise, FALSE);
}

void CBCGPGraphicsManager::DrawFillPolygon(const CBCGPPointsArray& pts, const CBCGPBrush& brFill, const CBCGPBrush& brLine)
{
	int nSize = (int)pts.GetSize();
	if (nSize < 3)
	{
		return;
	}

	CBCGPComplexGeometry shape;
	shape.SetStart(pts[0]);

	int i = 0;

	for (i = 1; i < nSize; i++)
	{
		shape.AddLine(pts[i]);
	}

	if (!brFill.IsEmpty())
	{
		FillGeometry(shape, brFill);
	}

	if (!brLine.IsEmpty())
	{
		for (i = 0; i < nSize; i++)
		{
			DrawLine(pts[i], pts[i == nSize - 1 ? 0 : i + 1], brLine);
		}
	}
}

void CBCGPGraphicsManager::DrawBeveledRectangle(const CBCGPRect& rect, const CBCGPBrush& brush, 
												int bevel_size, BOOL bDrawBorder)
{
	if (brush.IsEmpty())
	{
		return;
	}

	CBCGPRect rectShape((CRect)rect);
	int bevel = bevel_size;
	
	if (bDrawBorder)
	{
		rectShape.DeflateRect (1.0, 1.0);
		bevel--;
	}
	
	COLORREF color = brush.GetColor();
	double opacity = brush.GetOpacity();
	
	double percent = 0.23;

	CBCGPColor colorLight(color);
	colorLight.MakeLighter (percent);

	CBCGPColor colorDark(color);
	colorDark.MakeDarker (percent);
	
	// draw internal part
	{
		CBCGPRect rectInternal(rectShape);
		rectInternal.DeflateRect (bevel, bevel);
		
		CBCGPBrush brFill(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, opacity);
		FillRectangle (rectInternal, brFill);
	}

	// draw bevel
	{
		CBCGPRect rectBevel(rectShape);
		rectBevel.DeflateRect (bevel, bevel);
		
		CBCGPRect rectBevelHT(rectBevel);
		rectBevelHT.bottom = rectBevelHT.top + 1.0;
		
		CBCGPRect rectBevelHB(rectBevel);
		rectBevelHB.top = rectBevelHB.bottom - 1.0;
		
		CBCGPRect rectBevelVL(rectBevel);
		rectBevelVL.top++;
		rectBevelVL.bottom--;
		rectBevelVL.right = rectBevelVL.left + 1.0;
		
		CBCGPRect rectBevelVR(rectBevel);
		rectBevelVR.top++;
		rectBevelVR.bottom--;
		rectBevelVR.left = rectBevelVR.right - 1.0;
		
		CBCGPColor color1(colorLight);
		color1.MakeLighter (percent);
		CBCGPBrush br1(color1, opacity);

		CBCGPColor color2(colorDark);
		color2.MakeDarker (percent);
		CBCGPBrush br2(color2, opacity);

		double percent2 = percent / 3.0;

		color1 = colorLight;
		color1.MakeDarker (percent2);
		color2 = colorDark;
		color2.MakeDarker (percent2);

		CBCGPBrush br3(color2, color1, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, opacity);

		color1 = colorLight;
		color1.MakeDarker (percent);
		color2 = colorDark;
		color2.MakeDarker (percent);

		CBCGPBrush br4(color2, color1, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, opacity);

		for (int i = 0; i < bevel; i++)
		{
			rectBevelHT.InflateRect (1.0, 0.0);
			rectBevelHT.OffsetRect (0.0, -1.0);
			rectBevelHB.InflateRect (1.0, 0.0);
			rectBevelHB.OffsetRect (0.0, 1.0);

			FillRectangle (rectBevelHT, br1);
			FillRectangle (rectBevelHB, br2);
			
			rectBevelVL.InflateRect (0.0, 1.0);
			rectBevelVL.OffsetRect (-1.0, 0.0);

			FillRectangle (rectBevelVL, br3);
			
			rectBevelVR.InflateRect (0.0, 1.0);
			rectBevelVR.OffsetRect (1.0, 0.0);

			FillRectangle (rectBevelVR, br4);
		}
	}
	
	// draw border
	if (bDrawBorder)
	{
		rectShape = rect;
		if (GetType () == CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_D2D)
		{
			rectShape.right--;
			rectShape.bottom--;
		}
		
		colorDark.MakeDarker (0.23);
		DrawRectangle (rectShape, CBCGPBrush(colorDark, opacity));
	}
}

void CBCGPGraphicsManager::Draw3DPie(
	const CBCGPEllipse& ellipseSrc, 
	const double dblHeightSrc,
	double dblStartAngle, double dblFinishAngle, 
	const CBCGPBrush& brFill, const CBCGPBrush& brSideFill, const CBCGPBrush& brLine,
	double dblOffsetFromCenter,
	int nDrawingFlags)
{
	Draw3DDoughnut(ellipseSrc, 0., dblHeightSrc, dblStartAngle, dblFinishAngle, brFill, brSideFill, brLine, dblOffsetFromCenter, nDrawingFlags);
}

void CBCGPGraphicsManager::Draw3DDoughnut(
	const CBCGPEllipse& ellipseSrc, 
	const double dblHolePerc,
	const double dblHeightSrc,
	double dblStartAngle, double dblFinishAngle, 
	const CBCGPBrush& brFill, const CBCGPBrush& brSideFillSrc, const CBCGPBrush& brLine,
	double dblOffsetFromCenter,
	int nDrawingFlags)
{
	CBCGPBrush brBevel1;
	CBCGPBrush brBevel2;
	CBCGPBrush brSideFillBevel;

	const double dblBevelRatio = .95;

	const BOOL bIsBevel = (brFill.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL);

	if (bIsBevel)
	{
		CBCGPColor colorLight, colorDark;
		PrepareBevelColors(brFill.GetColor(), colorLight, colorDark);

		brBevel1.SetColors(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, brFill.GetOpacity());
		brBevel2.SetColors(brFill.GetColor(), colorLight, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, brFill.GetOpacity());

		PrepareBevelColors(brSideFillSrc.GetColor(), colorLight, colorDark);
		brSideFillBevel.SetColors(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, brFill.GetOpacity());
	}

	const CBCGPBrush& brSideFill = bIsBevel ? brSideFillBevel : brSideFillSrc;

	if (ellipseSrc.radiusX < 1. || ellipseSrc.radiusY < 1.)
	{
		return;
	}

	const CBCGPSize sizeHole(ellipseSrc.radiusX * dblHolePerc, ellipseSrc.radiusY * dblHolePerc);

	const BOOL bIsFullEllipse = fabs(bcg_normalize_deg(dblStartAngle) - bcg_normalize_deg(dblFinishAngle)) < .1f;

	double dblHeight = dblHeightSrc;
	dblHeight += .5;

	if (dblHeight <= 2.)
	{
		DrawDoughnut(ellipseSrc, dblHolePerc, dblStartAngle, dblFinishAngle, brFill, brLine, dblOffsetFromCenter);
		return;
	}

	CBCGPEllipse ellipse = ellipseSrc;

	if (dblOffsetFromCenter != 0.)
	{
		double angleMid = .5 * (bcg_deg2rad(dblStartAngle) + bcg_deg2rad(dblFinishAngle));

		ellipse.point.x += dblOffsetFromCenter * cos(angleMid);
		ellipse.point.y -= dblOffsetFromCenter * sin(angleMid);
	}

	double angleCos1 = cos(bcg_deg2rad(dblStartAngle));
	double angleSin1 = sin(bcg_deg2rad(dblStartAngle));

	double angleCos2 = cos(bcg_deg2rad(dblFinishAngle));
	double angleSin2 = sin(bcg_deg2rad(dblFinishAngle));

	double dblDelta = fabs(dblFinishAngle - dblStartAngle);

	BOOL bIsLargeArc = dblDelta >= 180.;

	CBCGPPoint ptCenter = ellipse.point;

	CBCGPPoint pt1(ptCenter.x + angleCos1 * ellipse.radiusX, ptCenter.y - angleSin1 * ellipse.radiusY);
	CBCGPPoint pt2(ptCenter.x + angleCos2 * ellipse.radiusX, ptCenter.y - angleSin2 * ellipse.radiusY);

	CBCGPSize szRadius(ellipse.radiusX, ellipse.radiusY);
	
	CBCGPComplexGeometry shapeTop;
	CBCGPComplexGeometry shapeTopInternal;

	if (!bIsFullEllipse)
	{
		CBCGPPoint ptStart = CBCGPPoint(
			ptCenter.x + angleCos1 * sizeHole.cx, 
			ptCenter.y - angleSin1 * sizeHole.cy);

		shapeTop.SetStart(ptStart);

		shapeTop.AddLine(pt1);
		shapeTop.AddArc(pt2, szRadius, dblStartAngle > dblFinishAngle, bIsLargeArc);

		if (!sizeHole.IsEmpty())
		{
			shapeTop.AddLine(CBCGPPoint(
				ptCenter.x + angleCos2 * sizeHole.cx, 
				ptCenter.y - angleSin2 * sizeHole.cy));

			shapeTop.AddArc(ptStart, sizeHole, dblStartAngle <= dblFinishAngle, bIsLargeArc);
		}

		if (bIsBevel)
		{
			shapeTopInternal.SetStart(ptStart);
		
			double rX = ellipse.radiusX * dblBevelRatio;
			double rY = ellipse.radiusY * dblBevelRatio;
			
			shapeTopInternal.AddLine(CBCGPPoint(
				ellipse.point.x + angleCos1 * rX, 
				ellipse.point.y - angleSin1 * rY));

			shapeTopInternal.AddArc(
				CBCGPPoint(ellipse.point.x + angleCos2 * rX, ellipse.point.y - angleSin2 * rY),
				CBCGPSize(rX, rY), dblStartAngle > dblFinishAngle, bIsLargeArc);

			if (!sizeHole.IsEmpty())
			{
				shapeTopInternal.AddLine(CBCGPPoint(
					ellipse.point.x + angleCos2 * sizeHole.cx, 
					ellipse.point.y - angleSin2 * sizeHole.cy));

				shapeTopInternal.AddArc(ptStart, sizeHole, dblStartAngle <= dblFinishAngle, bIsLargeArc);
			}
		}
	}

	CBCGPPoint pt1Bottom = pt1;
	pt1Bottom.y += dblHeight;

	CBCGPPoint pt2Bottom = pt2;
	pt2Bottom.y += dblHeight;

	CBCGPPoint ptCenterBottom = ptCenter;
	ptCenterBottom.y += dblHeight;

	CBCGPComplexGeometry shapeSide;
	CBCGPComplexGeometry shapeSide2;

	if (nDrawingFlags & (BCGP_3D_DRAW_SIDE1 | BCGP_3D_DRAW_SIDE2))
	{
		if (pt1.y >= ptCenter.y && pt2.y >= ptCenter.y && !bIsLargeArc && !bIsFullEllipse)
		{
			if (nDrawingFlags & BCGP_3D_DRAW_SIDE1)
			{
				shapeSide.SetStart(pt1Bottom);

				shapeSide.AddArc(pt2Bottom, szRadius, dblStartAngle > dblFinishAngle, bIsLargeArc);
				shapeSide.AddLine(pt2);
				shapeSide.AddArc(pt1, szRadius, dblStartAngle < dblFinishAngle, bIsLargeArc);
			}
		}
		else if (bIsFullEllipse || (pt1.y < ptCenter.y && pt2.y < ptCenter.y && bIsLargeArc))
		{
			if (nDrawingFlags & BCGP_3D_DRAW_SIDE1)
			{
				CBCGPPoint ptLeft(ptCenter.x - szRadius.cx, ptCenter.y);
				CBCGPPoint ptRight(ptCenter.x + szRadius.cx, ptCenter.y);

				CBCGPPoint ptLeftBottom(ptCenter.x - szRadius.cx, ptCenter.y + dblHeight);
				CBCGPPoint ptRightBottom(ptCenter.x + szRadius.cx, ptCenter.y + dblHeight);

				shapeSide.SetStart(ptLeft);

				shapeSide.AddArc(ptRight, szRadius, FALSE, bIsLargeArc);
				shapeSide.AddLine(ptRightBottom);
				shapeSide.AddArc(ptLeftBottom, szRadius, TRUE, bIsLargeArc);
			}
		}
		else
		{
			if (nDrawingFlags & BCGP_3D_DRAW_SIDE1)
			{
				Create3DDoughnutSideShape(shapeSide, pt1, dblFinishAngle, dblStartAngle, szRadius, ptCenter, dblHeight);
			}

			if (nDrawingFlags & BCGP_3D_DRAW_SIDE2)
			{
				Create3DDoughnutSideShape(shapeSide2, pt2, dblStartAngle, dblFinishAngle, szRadius, ptCenter, dblHeight);
			}
		}
	}

	CBCGPPoint ptOffsetHole1(angleCos1 * sizeHole.cx, -angleSin1 * sizeHole.cy);
	CBCGPPoint ptOffsetHole2(angleCos2 * sizeHole.cx, -angleSin2 * sizeHole.cy);

	CBCGPPoint ptOffsetHole1Int = (ptOffsetHole1.y > 0 && ptOffsetHole2.y > 0) || bIsFullEllipse ? CBCGPPoint(sizeHole.cx, 0) : ptOffsetHole1;
	CBCGPPoint ptOffsetHole2Int = (ptOffsetHole1.y> 0  && ptOffsetHole2.y > 0) || bIsFullEllipse ? CBCGPPoint(-sizeHole.cx, 0) : ptOffsetHole2;

	CBCGPComplexGeometry shapeInternal;

	BOOL bDrawInternalShape = !sizeHole.IsEmpty() && (ptOffsetHole1.y < 0 || ptOffsetHole2.y < 0 || bIsLargeArc) &&
		(nDrawingFlags & BCGP_3D_DRAW_INTERNAL_SIDE);

	if (bDrawInternalShape)
	{
		shapeInternal.SetStart(ptCenter + ptOffsetHole1Int);

		shapeInternal.AddArc(ptCenter + ptOffsetHole2Int, sizeHole, FALSE, bIsLargeArc);
		shapeInternal.AddLine(ptCenterBottom + ptOffsetHole2Int);
		shapeInternal.AddArc(ptCenterBottom + ptOffsetHole1Int, sizeHole, TRUE, bIsLargeArc);
	}

	CBCGPPointsArray ptsEdge1;
	CBCGPPointsArray ptsEdge2;

	if (nDrawingFlags & (BCGP_3D_DRAW_EDGE1 | BCGP_3D_DRAW_EDGE2))
	{
		if (fabs(pt1.x - ptCenter.x) > 2.0 && (nDrawingFlags & BCGP_3D_DRAW_EDGE1))
		{
			ptsEdge1.Add(ptCenter + ptOffsetHole1);
			ptsEdge1.Add(pt1);
			ptsEdge1.Add(pt1Bottom);
			ptsEdge1.Add(ptCenterBottom + ptOffsetHole1);
		}

		if (fabs(pt2.x - ptCenter.x) > 2.0 && (nDrawingFlags & BCGP_3D_DRAW_EDGE2))
		{
			ptsEdge2.Add(ptCenter + ptOffsetHole2);
			ptsEdge2.Add(pt2);
			ptsEdge2.Add(pt2Bottom);
			ptsEdge2.Add(ptCenterBottom + ptOffsetHole2);
		}

		if (pt2.y > pt1.y)
		{
			DrawFillPolygon(ptsEdge1, brSideFill, brLine);
			DrawFillPolygon(ptsEdge2, brSideFill, brLine);
		}
		else
		{
			DrawFillPolygon(ptsEdge2, brSideFill, brLine);
			DrawFillPolygon(ptsEdge1, brSideFill, brLine);
		}
	}

	if (bDrawInternalShape)
	{
		m_rectCurrGradient = ellipseSrc;

		FillGeometry(shapeInternal, brSideFill);

		DrawArc(ptCenterBottom + ptOffsetHole2Int, ptCenterBottom + ptOffsetHole1Int, 
			sizeHole, TRUE, bIsLargeArc, brLine);

		m_rectCurrGradient.SetRectEmpty();

		if (ptsEdge1.GetSize() > 0 && pt1.y >= ptCenter.y)
		{
			DrawFillPolygon(ptsEdge1, brSideFill, brLine);
		}

		if (ptsEdge2.GetSize() > 0 && pt2.y >= ptCenter.y)
		{
			DrawFillPolygon(ptsEdge2, brSideFill, brLine);
		}
	}

	if (!shapeSide.IsNull() || !shapeSide2.IsNull())
	{
		m_rectCurrGradient = ellipseSrc;

		m_rectCurrGradient.top = m_rectCurrGradient.CenterPoint().y;
		m_rectCurrGradient.bottom = m_rectCurrGradient.top + dblHeight;

		if (!shapeSide.IsNull())
		{
			FillGeometry(shapeSide, brSideFill);
			DrawGeometry(shapeSide, brLine);
		}

		if (!shapeSide2.IsNull())
		{
			FillGeometry(shapeSide2, brSideFill);
			DrawGeometry(shapeSide2, brLine);
		}

		m_rectCurrGradient.SetRectEmpty();
	}

	if (nDrawingFlags & BCGP_3D_DRAW_TOP)
	{
		if (bIsFullEllipse)
		{
			if (!sizeHole.IsEmpty())
			{
				CBCGPEllipseGeometry g1(ellipseSrc);

				CBCGPEllipse ellipseHole = ellipseSrc;
				ellipseHole.radiusX = sizeHole.cx;
				ellipseHole.radiusY = sizeHole.cy;

				CBCGPEllipseGeometry g2(ellipseHole);

				CBCGPGeometry geometry;
				CombineGeometry(geometry, g1, g2, RGN_XOR);

				if (bIsBevel)
				{
					CBCGPEllipse ellipseBevel = ellipseSrc;
					ellipseBevel.radiusX *= dblBevelRatio;
					ellipseBevel.radiusY *= dblBevelRatio;

					CBCGPEllipseGeometry geometryBevel(ellipseBevel);

					CBCGPGeometry geometryInternal;
					CombineGeometry(geometryInternal, geometry, geometryBevel, RGN_AND);

					OnFillGeometryBevel(brFill, geometry, geometryInternal);
				}
				else
				{
					FillGeometry(geometry, brFill);
				}

				DrawEllipse(ellipseHole, brLine);
			}
			else
			{
				if (bIsBevel)
				{
					FillEllipse(ellipseSrc, brBevel1);

					CBCGPEllipse ellipseInternal = ellipseSrc;
					ellipseInternal.radiusX *= dblBevelRatio;
					ellipseInternal.radiusY *= dblBevelRatio;

					FillEllipse(ellipseInternal, brBevel2);
				}
				else
				{
					FillEllipse(ellipseSrc, brFill);
				}
			}

			DrawEllipse(ellipseSrc, brLine);
		}
		else
		{
			m_rectCurrGradient = ellipseSrc;

			if (bIsBevel)
			{
				OnFillGeometryBevel(brFill, shapeTop, shapeTopInternal);
			}
			else
			{
				FillGeometry(shapeTop, brFill);
			}

			DrawGeometry(shapeTop, brLine);
		}
	}

	m_rectCurrGradient.SetRectEmpty();
}

struct CBCGPTorusArc
{
	CBCGPPoint ptFrom;
	CBCGPPoint ptTo;
	CBCGPSize sizeRadius;
	BOOL bIsClockwise;
	BOOL bIsLargeArc;

	CBCGPTorusArc()
	{
		bIsClockwise = bIsLargeArc = FALSE;
	}
};

static void PrepareTorusShapes(const CBCGPEllipse& ellipseExternal,
							   const CBCGPEllipse& ellipseInternal,
							   double dblHeight,
							   double dblStartAngle, double dblFinishAngle,
							   CBCGPEllipse& ellipseEdge1, CBCGPEllipse& ellipseEdge2,
							   CBCGPComplexGeometry& shape,
							   CBCGPTorusArc* pArc1 = NULL,
							   CBCGPTorusArc* pArc2 = NULL,
							   BOOL bBuildShape = TRUE)
{
	CBCGPPoint ptCenter = ellipseExternal.point;


	double angleCos1 = cos(bcg_deg2rad(dblStartAngle));
	double angleSin1 = sin(bcg_deg2rad(dblStartAngle));

	double angleCos2 = cos(bcg_deg2rad(dblFinishAngle));
	double angleSin2 = sin(bcg_deg2rad(dblFinishAngle));

	CBCGPPoint pt1External(ellipseExternal.point.x + angleCos1 * ellipseExternal.radiusX, ellipseExternal.point.y - angleSin1 * ellipseExternal.radiusY);
	CBCGPPoint pt2External(ellipseExternal.point.x + angleCos2 * ellipseExternal.radiusX, ellipseExternal.point.y - angleSin2 * ellipseExternal.radiusY);

	CBCGPPoint pt1Internal(ellipseInternal.point.x + angleCos1 * ellipseInternal.radiusX, ellipseInternal.point.y - angleSin1 * ellipseInternal.radiusY);
	CBCGPPoint pt2Internal(ellipseInternal.point.x + angleCos2 * ellipseInternal.radiusX, ellipseInternal.point.y - angleSin2 * ellipseInternal.radiusY);

	CBCGPPoint ptCenter1(pt1Internal.x + dblHeight * angleCos1 / 2, (pt1Internal.y + pt1External.y) / 2);
	CBCGPPoint ptCenter2(pt2Internal.x + dblHeight * angleCos2 / 2, (pt2Internal.y + pt2External.y) / 2);

	CBCGPSize szRadiusSide1(dblHeight * fabs(angleCos1), dblHeight - dblHeight * fabs(angleSin1) / 4);
	CBCGPSize szRadiusSide2(dblHeight * fabs(angleCos2), dblHeight - dblHeight * fabs(angleSin2) / 4);

	CBCGPPoint pt1Ext(
		ptCenter1.x + angleCos1 * (szRadiusSide1.cx / 2 + 1),
		ptCenter1.y - angleSin1 * (szRadiusSide1.cy / 2 + 1));

	CBCGPPoint pt1Int(
		ptCenter1.x - angleCos1 * (szRadiusSide1.cx / 2 + 1),
		ptCenter1.y + angleSin1 * (szRadiusSide1.cy / 2 + 1));

	CBCGPPoint pt2Ext(
		ptCenter2.x + angleCos2 * (szRadiusSide2.cx / 2 + 1),
		ptCenter2.y - angleSin2 * (szRadiusSide2.cy / 2 + 1));

	CBCGPPoint pt2Int(
		ptCenter2.x - angleCos2 * (szRadiusSide2.cx / 2 + 1),
		ptCenter2.y + angleSin2 * (szRadiusSide2.cy / 2 + 1));

	if (ptCenter1.x > ptCenter.x + 2.0)
	{
		ellipseEdge1 = CBCGPEllipse(ptCenter1, szRadiusSide1);
	}

	if (ptCenter2.x < ptCenter.x - 2.0)
	{
		ellipseEdge2 = CBCGPEllipse(ptCenter2, szRadiusSide2);
	}

	BOOL bIsClockwise1 = pt1Int.x > ptCenter.x;
	BOOL bIsClockwise2 = pt2Int.x > ptCenter.x;

	if (bBuildShape)
	{
		double angleStep = .01;

		double angleRad1 = min(bcg_deg2rad(dblStartAngle), bcg_deg2rad(dblFinishAngle));
		double angleRad2 = max(bcg_deg2rad(dblStartAngle), bcg_deg2rad(dblFinishAngle));

		double angle = 0.;

		for (angle = angleRad1; angle <= angleRad2; angle += angleStep)
		{
			double angleCos = cos(angle);
			double angleSin = sin(angle);

			CBCGPPoint ptExternal(ellipseExternal.point.x + angleCos * ellipseExternal.radiusX, ellipseExternal.point.y - angleSin * ellipseExternal.radiusY);
			CBCGPPoint ptInternal(ellipseInternal.point.x + angleCos * ellipseInternal.radiusX, ellipseInternal.point.y - angleSin * ellipseInternal.radiusY);

			CBCGPPoint ptCenterCurr(ptInternal.x + dblHeight * angleCos / 2, (ptInternal.y + ptExternal.y) / 2);
			CBCGPSize szRadiusSide(dblHeight * fabs(angleCos), dblHeight - dblHeight * fabs(angleSin) / 4);

			CBCGPPoint pt(
				ptCenterCurr.x + angleCos * (szRadiusSide.cx / 2 + 1),
				ptCenterCurr.y - angleSin * (szRadiusSide.cy / 2 + 1));

			if (angle == angleRad1)
			{
				shape.SetStart(pt);
			}
			else
			{
				shape.AddLine(pt);
			}
		}

		if (szRadiusSide2.cx > 2)
		{
			shape.AddArc(pt2Int, 
				CBCGPSize(szRadiusSide2.cx / 2, szRadiusSide2.cy / 2), !bIsClockwise2, FALSE);
		}

		for (angle = angleRad2; angle >= angleRad1; angle -= angleStep)
		{
			double angleCos = cos(angle);
			double angleSin = sin(angle);

			CBCGPPoint ptExternal(ellipseExternal.point.x + angleCos * ellipseExternal.radiusX, ellipseExternal.point.y - angleSin * ellipseExternal.radiusY);
			CBCGPPoint ptInternal(ellipseInternal.point.x + angleCos * ellipseInternal.radiusX, ellipseInternal.point.y - angleSin * ellipseInternal.radiusY);

			CBCGPPoint ptCenterCurr(ptInternal.x + dblHeight * angleCos / 2, (ptInternal.y + ptExternal.y) / 2);
			CBCGPSize szRadiusSide(dblHeight * fabs(angleCos), dblHeight - dblHeight * fabs(angleSin) / 4);

			CBCGPPoint pt(
				ptCenterCurr.x - angleCos * (szRadiusSide.cx / 2 + 1),
				ptCenterCurr.y + angleSin * (szRadiusSide.cy / 2 + 1));

			shape.AddLine(pt);
		}

		if (szRadiusSide1.cx > 2)
		{
			shape.AddArc(pt1Ext, 
				CBCGPSize(szRadiusSide1.cx / 2, szRadiusSide1.cy / 2), bIsClockwise1, FALSE);
		}
	}

	if (pArc1 != NULL)
	{
		pArc1->ptFrom = pt1Int;
		pArc1->ptTo = pt1Ext;
		pArc1->sizeRadius = CBCGPSize(szRadiusSide1.cx / 2, szRadiusSide1.cy / 2);
		pArc1->bIsClockwise = bIsClockwise1;
		pArc1->bIsLargeArc = FALSE;
	}

	if (pArc2 != NULL)
	{
		pArc2->ptFrom = pt2Ext;
		pArc2->ptTo = pt2Int;
		pArc2->sizeRadius = CBCGPSize(szRadiusSide2.cx / 2, szRadiusSide2.cy / 2);
		pArc2->bIsClockwise = !bIsClockwise2;
		pArc2->bIsLargeArc = FALSE;
	}
}

void CBCGPGraphicsManager::Draw3DTorus(
	const CBCGPEllipse& ellipseSrc, 
	const double dblHeightSrc,
	double dblStartAngle, double dblFinishAngle, 
	const CBCGPBrush& brFillSrc, const CBCGPBrush& brSideFillSrc, const CBCGPBrush& brLineSrc,
	double dblOffsetFromCenter,
	int nDrawingFlags)
{
	if (ellipseSrc.radiusX < 1. || ellipseSrc.radiusY < 1.)
	{
		return;
	}

	if (dblHeightSrc <= 2.)
	{
		DrawDoughnut(ellipseSrc, .5, dblStartAngle, dblFinishAngle, brFillSrc, brLineSrc, dblOffsetFromCenter);
		return;
	}

	double dblGradientStep = IsSupported(BCGP_GRAPHICS_MANAGER_ANTIALIAS) ? 1. : 20;

	CBCGPBrush brFill = brFillSrc;
	CBCGPBrush brSideFill = brSideFillSrc;
	CBCGPBrush brLine = brLineSrc;

	CBCGPColor clrDark = brFill.GetColor();
	clrDark.MakeDarker();

	CBCGPColor clrLight = brFill.GetGradientColor();
	clrLight.MakePale();

	brFill.SetColors(clrDark, clrLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT);

	clrLight = brLine.GetColor();
	clrLight.MakePale(.7);

	brLine.SetColor(clrLight);

	clrDark = brSideFill.GetColor();
	clrDark.MakeDarker();

	clrLight = brSideFill.GetGradientColor();
	clrLight.MakePale();

	brSideFill.SetColors(clrDark, clrLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT);

	CBCGPEllipse ellipse = ellipseSrc;
	ellipse.radiusY = min(ellipse.radiusY, ellipse.radiusX * 3 / 4);

	double dblHeight = min(dblHeightSrc, min(ellipse.radiusX / 2, ellipse.radiusY * 3 / 4));

	const CBCGPSize sizeHole(ellipse.radiusX * (dblHeight / ellipse.radiusY), dblHeight);

	const BOOL bIsFullEllipse = fabs(bcg_normalize_deg(dblStartAngle) - bcg_normalize_deg(dblFinishAngle)) < .1f;

	ellipse.radiusX -= (ellipse.radiusX - sizeHole.cx) / 3;
	ellipse.radiusY -= (ellipse.radiusY - sizeHole.cy) / 2;

	if (dblOffsetFromCenter != 0.)
	{
		double angleMid = .5 * (bcg_deg2rad(dblStartAngle) + bcg_deg2rad(dblFinishAngle));

		ellipse.point.x += dblOffsetFromCenter * cos(angleMid);
		ellipse.point.y -= dblOffsetFromCenter * sin(angleMid);
	}

	CBCGPPoint ptCenter = ellipse.point;

	CBCGPEllipse ellipseInternal = ellipse;
	ellipseInternal.radiusX -= dblHeight * ellipse.radiusY / ellipse.radiusX;
	ellipseInternal.radiusY -= dblHeight / 2;

	CBCGPEllipse ellipseExternal = ellipse;
	ellipseExternal.radiusX += dblHeight * ellipse.radiusY / ellipse.radiusX;
	ellipseExternal.radiusY += dblHeight / 2;

	CBCGPEllipse ellipseEdge1;
	CBCGPEllipse ellipseEdge2;

	if (bIsFullEllipse)
	{
		if (!IsSupported(BCGP_GRAPHICS_MANAGER_ANTIALIAS))
		{
			CBCGPEllipseGeometry g1(ellipseExternal);
			CBCGPEllipseGeometry g2(ellipseInternal);

			CBCGPGeometry geometry;
			CombineGeometry(geometry, g1, g2, RGN_XOR);

			m_rectCurrGradient = ellipseSrc;
			m_rectCurrGradient.top -= m_rectCurrGradient.Height() / 2;

			FillGeometry(geometry, brFill);

			DrawEllipse(ellipseExternal, brLine);
			DrawEllipse(ellipseInternal, brLine);

			m_rectCurrGradient.SetRectEmpty();
		}
		else
		{
			for (double dblAngle = 0; dblAngle <= 360; dblAngle += dblGradientStep)
			{
				CBCGPComplexGeometry shape;

				PrepareTorusShapes(ellipseExternal, ellipseInternal, dblHeight,
								   dblAngle, dblAngle + dblGradientStep + 2.,
								   ellipseEdge1, ellipseEdge2, shape);
				FillGeometry(shape, brFill);
			}
		}

		return;
	}

	CBCGPComplexGeometry shape;

	CBCGPTorusArc arc1;
	CBCGPTorusArc arc2;

	BOOL bSmouthFill = nDrawingFlags & (BCGP_3D_DRAW_SIDE1 | BCGP_3D_DRAW_SIDE2);

	PrepareTorusShapes(ellipseExternal, ellipseInternal, dblHeight,
					   dblStartAngle, dblFinishAngle,
					   ellipseEdge1, ellipseEdge2, shape, &arc1, &arc2, bSmouthFill);

	if (bSmouthFill)
	{
		if (IsSupported(BCGP_GRAPHICS_MANAGER_ANTIALIAS))
		{
			for (double dblAngle = dblStartAngle; dblAngle < dblFinishAngle; dblAngle += dblGradientStep)
			{
				double dblAngle2 = min(dblAngle + dblGradientStep + 1., dblFinishAngle);

				if (dblAngle < dblAngle2)
				{
					CBCGPEllipse ellipseEdge11;
					CBCGPEllipse ellipseEdge21;

					CBCGPComplexGeometry shape1;

					PrepareTorusShapes(ellipseExternal, ellipseInternal, dblHeight,
									   dblAngle, dblAngle2,
									   ellipseEdge11, ellipseEdge21, shape1);
					FillGeometry(shape1, brFill);
				}
			}
		}
		else
		{
			FillGeometry(shape, brFill);
		}
	}

	if (nDrawingFlags & BCGP_3D_DRAW_EDGE1)
	{
		FillEllipse(ellipseEdge1, brSideFill);
		DrawEllipse(ellipseEdge1, brLineSrc);
	}

	DrawArc(arc1.ptFrom, arc1.ptTo, arc1.sizeRadius, arc1.bIsClockwise, arc1.bIsLargeArc, brLine);

	if (nDrawingFlags & BCGP_3D_DRAW_EDGE2)
	{
		FillEllipse(ellipseEdge2, brSideFill);
		DrawEllipse(ellipseEdge2, brLineSrc);
	}

	DrawArc(arc2.ptFrom, arc2.ptTo, arc2.sizeRadius, arc2.bIsClockwise, arc2.bIsLargeArc, brLine);
}

void CBCGPGraphicsManager::DrawPyramid(const CBCGPRect& rect,
		const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double Y1, double Y2)
{
	Draw3DPyramid(rect, 0., brFill, CBCGPBrush(), brLine, 0., Y1, Y2);
}

void CBCGPGraphicsManager::Draw3DPyramid(const CBCGPRect& rectPyramid, double dblDepth,
		const CBCGPBrush& brSideFillLeft, const CBCGPBrush& brSideFillRight, const CBCGPBrush& brLine,
		double dblXCenterOffset, double Y1, double Y2, BOOL bIsCircularBase, const CBCGPBrush& brTopFill)
{
	if (rectPyramid.IsRectEmpty())
	{
		return;
	}

	const BOOL bIsTransparent = brSideFillLeft.GetOpacity() < 1. && dblDepth > 0.;

	CBCGPPointsArray		ptsLeft;
	CBCGPPointsArray		ptsRight;
	CBCGPPointsArray		ptsTop;
	CBCGPPointsArray		ptsHidden;

	CBCGPComplexGeometry	shapeSide;
	CBCGPEllipse			ellipseTop;
	CBCGPEllipse			ellipseBottom;

	dblDepth = min(dblDepth, rectPyramid.Height() / 2 - 1);
	dblDepth = min(dblDepth, rectPyramid.Width() / 2 - 1);

	if (dblDepth <= 4.)
	{
		dblDepth = 0.;
	}

	CBCGPRect rect = rectPyramid;
	rect.DeflateRect(dblDepth, 0, dblDepth, dblDepth);

	double dblMaxXOffset = rect.Width() / 2;

	if (fabs(dblXCenterOffset) > dblMaxXOffset)
	{
		if (dblXCenterOffset < 0)
		{
			dblXCenterOffset = -dblMaxXOffset; 
		}
		else
		{
			dblXCenterOffset = dblMaxXOffset;
		}
	}

	double xCenter = rect.CenterPoint().x;

	if (dblXCenterOffset != 0 && !bIsCircularBase)
	{
		double dblHalfWidth = rect.Width() / 2;
		dblDepth *= (dblHalfWidth - fabs(dblXCenterOffset)) / dblHalfWidth;
	}

	if (Y1 <= 0.)
	{
		double bottom = Y2 < 0. ? rect.bottom : rect.top + Y2;

		double dblRatio = (rect.bottom - bottom) / rect.Height();
		
		dblDepth *= (1. - dblRatio);
		dblXCenterOffset *= (1. - dblRatio);

		double dx = .5 * dblRatio * rect.Width();

		CBCGPPoint ptTop(xCenter, rect.top);
		CBCGPPoint ptBottom(xCenter + dblXCenterOffset, bottom);

		CBCGPPoint ptBottomLeft(rect.left + dx, bottom - .5 * dblDepth);
		CBCGPPoint ptBottomRight(rect.right - dx, bottom - .5 * dblDepth);

		if (bIsCircularBase)
		{
			shapeSide.SetStart(ptTop);
			shapeSide.AddLine(ptBottomLeft);

			if (dblDepth > 0.)
			{
				shapeSide.AddArc(ptBottomRight, CBCGPSize(fabs(xCenter - ptBottomLeft.x), dblDepth / 2), FALSE);
			}
			else
			{
				shapeSide.AddLine(ptBottomRight);
			}

			if (bIsTransparent)
			{
				ellipseBottom.point = CBCGPPoint(xCenter, ptBottomLeft.y + .5);
				ellipseBottom.radiusX = fabs(xCenter - ptBottomLeft.x) - .5;
				ellipseBottom.radiusY = dblDepth / 2 - .5;
			}
		}
		else if (brSideFillRight.IsEmpty())
		{
			ptsLeft.Add(ptTop);
			ptsLeft.Add(ptBottomLeft);
			ptsLeft.Add(ptBottomRight);
		}
		else
		{
			ptsLeft.Add(ptTop);
			ptsLeft.Add(ptBottomLeft);
			ptsLeft.Add(ptBottom);

			ptsRight.Add(ptTop);
			ptsRight.Add(ptBottomRight);
			ptsRight.Add(ptBottom);
		}

		if (bIsTransparent && !bIsCircularBase)
		{
			ptsHidden.Add(ptBottomLeft);
			ptsHidden.Add(CBCGPPoint(ptTop.x, bottom - dblDepth));
			ptsHidden.Add(ptTop);
			ptsHidden.Add(ptBottomRight);
		}
	}
	else
	{
		Y1 = rect.top + Y1;
		Y2 = (Y2 < 0.) ? rect.bottom : rect.top + Y2;

		double dblRatioTop = (rect.bottom - Y1) / rect.Height();
		double dblDepthTop = dblDepth * (1. - dblRatioTop);

		double dblRatioBottom = (rect.bottom - Y2) / rect.Height();
		double dblDepthBottom = dblDepth * (1. - dblRatioBottom);

		double dblXCenterTop = xCenter + dblXCenterOffset * (1. - dblRatioTop);
		double dblXCenterBottom = xCenter + dblXCenterOffset * (1. - dblRatioBottom);

		double dxTop = .5 * dblRatioTop * rect.Width();
		double dxBottom = .5 * dblRatioBottom * rect.Width();

		CBCGPPoint ptTopCenter1(xCenter - dblXCenterOffset * (1. - dblRatioTop), Y1 - dblDepthTop);
		CBCGPPoint ptTopCenter2(dblXCenterTop, Y1);

		CBCGPPoint ptTopLeft(rect.left + dxTop, Y1 - .5 * dblDepthTop);
		CBCGPPoint ptTopRight(rect.right - dxTop, Y1 - .5 * dblDepthTop);

		CBCGPPoint ptBottomCenter(dblXCenterBottom, Y2);

		CBCGPPoint ptBottomLeft(rect.left + dxBottom, Y2 - .5 * dblDepthBottom);
		CBCGPPoint ptBottomRight(rect.right - dxBottom, Y2 - .5 * dblDepthBottom);

		if (dblDepth > 0.)
		{
			if (bIsCircularBase)
			{
				ellipseTop.point = CBCGPPoint(xCenter, ptTopLeft.y + .5);
				ellipseTop.radiusX = fabs(xCenter - ptTopLeft.x) - .5;
				ellipseTop.radiusY = dblDepthTop / 2 - .5;

				if (bIsTransparent)
				{
					ellipseBottom.point = CBCGPPoint(xCenter, ptBottomLeft.y + .5);
					ellipseBottom.radiusX = fabs(xCenter - ptBottomLeft.x) - .5;
					ellipseBottom.radiusY = dblDepthBottom / 2 - .5;
				}
			}
			else
			{
				ptsTop.Add(ptTopCenter1);
				ptsTop.Add(ptTopLeft);
				ptsTop.Add(ptTopCenter2);
				ptsTop.Add(ptTopRight);
			}
		}

		if (bIsCircularBase)
		{
			double dblDelta = (m_Type == BCGP_GRAPHICS_MANAGER_GDI) ? 1. : 0.;

			shapeSide.SetStart(ptBottomLeft);

			if (dblDepth > 0.)
			{
				shapeSide.AddArc(ptBottomRight, CBCGPSize(fabs(xCenter - ptBottomLeft.x), dblDepthBottom / 2 + dblDelta), FALSE);
				shapeSide.AddLine(ptTopRight);
				shapeSide.AddArc(ptTopLeft, CBCGPSize(fabs(xCenter - ptTopLeft.x), dblDepthTop / 2 - dblDelta), TRUE);
			}
			else
			{
				shapeSide.AddLine(ptBottomRight);
				shapeSide.AddLine(ptTopRight);
				shapeSide.AddLine(ptTopLeft);
			}
		}
		else if (brSideFillRight.IsEmpty())
		{
			ptsLeft.Add(ptTopLeft);
			ptsLeft.Add(ptBottomLeft);
			ptsLeft.Add(ptBottomRight);
			ptsLeft.Add(ptTopRight);
		}
		else
		{
			ptsLeft.Add(ptTopCenter2);
			ptsLeft.Add(ptTopLeft);
			ptsLeft.Add(ptBottomLeft);
			ptsLeft.Add(ptBottomCenter);

			ptsRight.Add(ptTopCenter2);
			ptsRight.Add(ptTopRight);
			ptsRight.Add(ptBottomRight);
			ptsRight.Add(ptBottomCenter);
		}

		if (bIsTransparent && !bIsCircularBase)
		{
			ptsHidden.Add(ptBottomLeft);
			ptsHidden.Add(CBCGPPoint(xCenter - dblXCenterOffset * (1. - dblRatioBottom), Y2 - dblDepthBottom));
			ptsHidden.Add(ptTopCenter1);
			ptsHidden.Add(ptBottomRight);
		}
	}

	if (!shapeSide.IsEmpty())
	{
		FillGeometry(shapeSide, brSideFillLeft);
		DrawGeometry(shapeSide, brLine);

		if (!ellipseTop.IsNull())
		{
			FillEllipse(ellipseTop, brTopFill.IsEmpty() ? brSideFillLeft : brTopFill);
			DrawEllipse(ellipseTop, brLine);
		}

		if (!ellipseBottom.IsNull())
		{
			DrawEllipse(ellipseBottom, brLine);
		}
	}
	else
	{
		DrawFillPolygon(ptsLeft, brSideFillLeft, brLine);
		DrawFillPolygon(ptsRight, brSideFillRight, brLine);

		DrawFillPolygon(ptsTop, brTopFill.IsEmpty() ? brSideFillLeft : brTopFill, brLine);
	}

	if (ptsHidden.GetSize() == 4)
	{
		CBCGPBrush brHidden;
		brHidden.SetColor(brLine.GetColor(), brLine.GetOpacity() / 5);

		DrawLine(ptsHidden[0], ptsHidden[1], brHidden);
		DrawLine(ptsHidden[1], ptsHidden[2], brHidden);
		DrawLine(ptsHidden[1], ptsHidden[3], brHidden);
	}
}

void CBCGPGraphicsManager::DrawFunnel(const CBCGPRect& rectFunnel,
	double dblNeckHeight, double dblNeckWidth,
	const CBCGPBrush& brFill, const CBCGPBrush& brLine,
	double Y1, double Y2)
{
	Draw3DFunnel(rectFunnel, 0., dblNeckHeight, dblNeckWidth, brFill, brLine, Y1, Y2);
}

void CBCGPGraphicsManager::Draw3DFunnel(const CBCGPRect& rectFunnel, double dblDepth,
	double dblNeckHeight, double dblNeckWidth,
	const CBCGPBrush& brFill, const CBCGPBrush& brLine,
	double Y1, double Y2, BOOL /*bIsCircularBase*/, const CBCGPBrush& brTopFill)
{
	if (rectFunnel.IsRectEmpty())
	{
		return;
	}

	CBCGPComplexGeometry shapeSide;
	CBCGPEllipse ellipseTop;

	CBCGPRect rect = rectFunnel;

	dblNeckWidth = min(dblNeckWidth, rect.Width());
	dblNeckHeight = min(dblNeckHeight, rect.Height());

	double dblRadiusY1 = 5.0;
	double dblRadiusY2 = 5.0;

	if (dblDepth >= 1.)
	{
		double dblDepth2 = dblDepth / 2.0;
		double nHeight = rect.Height();

		rect.DeflateRect(0.0, dblDepth2);

		double dblRatio = rect.Height() / nHeight;
		dblNeckHeight *= dblRatio;
		Y1 *= dblRatio;
		Y2 *= dblRatio;

		nHeight = rect.Height();

		dblRadiusY1 = max(dblRadiusY1, dblDepth2 * Y1 / nHeight);
		dblRadiusY2 = max(dblRadiusY2, dblDepth2 * Y2 / nHeight);
	}

	Y1 += rect.top;
	Y2 += rect.top;

	double xCenter = rect.CenterPoint().x;

	rect.bottom -= dblNeckHeight;

	if (Y1 >= rect.bottom)
	{
		// Neck part:
		double dblXRadius = dblNeckWidth / 2;

		if (dblDepth < 1.)
		{
			shapeSide.SetStart(CBCGPPoint(xCenter - dblXRadius, Y1));
			shapeSide.AddLine(CBCGPPoint(xCenter + dblXRadius, Y1));
			shapeSide.AddLine(CBCGPPoint(xCenter + dblXRadius, Y2));
			shapeSide.AddLine(CBCGPPoint(xCenter - dblXRadius, Y2));
		}
		else
		{
			CBCGPSize szRadiusTop(dblXRadius, dblRadiusY1);
			CBCGPSize szRadiusBottom(dblXRadius, dblRadiusY2);

			double dblDelta = m_Type == BCGP_GRAPHICS_MANAGER_GDI ? 0 : 2;

			shapeSide.SetStart(CBCGPPoint(xCenter - dblXRadius, Y1 + dblDelta));
			shapeSide.AddArc(CBCGPPoint(xCenter + dblXRadius, Y1), szRadiusTop, FALSE);
			shapeSide.AddLine(CBCGPPoint(xCenter + dblXRadius, Y2));
			shapeSide.AddArc(CBCGPPoint(xCenter - dblXRadius, Y2), szRadiusBottom, TRUE);

			ellipseTop.point = CBCGPPoint(xCenter, Y1 + 1);
			ellipseTop.radiusX = szRadiusTop.cx;
			ellipseTop.radiusY = szRadiusTop.cy;
		}
	}
	else
	{
		double dblNeckWidth2 = dblNeckWidth / 2;
		double dblAngle = bcg_angle(rect.Width() / 2 - dblNeckWidth2, rect.Height());

		double yBottom = Y2 > rect.bottom ? rect.bottom : Y2;

		double dy       = (yBottom - Y1);
		double dx       = dy / tan(dblAngle);
		double dyBottom = Y2 > rect.bottom ? 0 : rect.bottom - Y2;
		double dxBottom = dblNeckWidth2 + (dx * dyBottom / dy);
		double dxTop    = dxBottom + dx;

		CBCGPPoint ptTopLeft(xCenter + dxTop, Y1);
		CBCGPPoint ptTopRight(xCenter - dxTop, Y1);

		CBCGPPoint ptBottomLeft(xCenter + dxBottom, yBottom);
		CBCGPPoint ptBottomRight(xCenter - dxBottom, yBottom);

		if (Y2 > rect.bottom)
		{
			CBCGPPoint ptBottomLeftNeck(xCenter + dxBottom, Y2);
			CBCGPPoint ptBottomRightNeck(xCenter - dxBottom, Y2);

			if (dblDepth < 1.)
			{
				shapeSide.SetStart(ptTopLeft);
				shapeSide.AddLine(ptBottomLeft);
				shapeSide.AddLine(ptBottomLeftNeck);
				shapeSide.AddLine(ptBottomRightNeck);
				shapeSide.AddLine(ptBottomRight);
				shapeSide.AddLine(ptTopRight);
			}
			else
			{
				shapeSide.SetStart(ptBottomLeftNeck);
				shapeSide.AddArc(ptBottomRightNeck, CBCGPSize(fabs(xCenter - ptBottomLeft.x), dblRadiusY2), TRUE);
				shapeSide.AddLine(ptBottomRight);
				shapeSide.AddLine(ptTopRight);
				shapeSide.AddArc(ptTopLeft, CBCGPSize(fabs(xCenter - ptTopLeft.x), dblRadiusY1), FALSE);
				shapeSide.AddLine(ptBottomLeft);

				ellipseTop.point = CBCGPPoint(xCenter, ptTopLeft.y + 1);
				ellipseTop.radiusX = fabs(xCenter - ptTopLeft.x);
				ellipseTop.radiusY = dblRadiusY1;
			}
		}
		else
		{
			if (dblDepth < 1.)
			{
				shapeSide.SetStart(ptTopLeft);
				shapeSide.AddLine(ptBottomLeft);
				shapeSide.AddLine(ptBottomRight);
				shapeSide.AddLine(ptTopRight);
			}
			else
			{
				shapeSide.SetStart(ptBottomLeft);
				shapeSide.AddArc(ptBottomRight, CBCGPSize(fabs(xCenter - ptBottomLeft.x), dblRadiusY2), TRUE);
				shapeSide.AddLine(ptTopRight);
				shapeSide.AddArc(ptTopLeft, CBCGPSize(fabs(xCenter - ptTopLeft.x), dblRadiusY1), FALSE);

				ellipseTop.point = CBCGPPoint(xCenter, ptTopLeft.y + 1);
				ellipseTop.radiusX = fabs(xCenter - ptTopLeft.x);
				ellipseTop.radiusY = dblRadiusY1;
			}
		}
	}

	FillGeometry(shapeSide, brFill);
	DrawGeometry(shapeSide, brLine);

	if (!ellipseTop.IsNull())
	{
		FillEllipse(ellipseTop, brTopFill.IsEmpty() ? brFill : brTopFill);
		DrawEllipse(ellipseTop, brLine);
	}
}

void CBCGPGraphicsManager::DrawArrow3(const CBCGPPoint& pt1, const CBCGPPoint& pt2, double length, 
	const CBCGPBrush& brFill, const CBCGPBrush& brLine, double arrowAngle, DWORD dwStyle)
{
    const double angle = bcg_angle (CBCGPPoint(pt1), CBCGPPoint(pt2));
    double delta = arrowAngle / 180.0;
    const double len2  = length / 2.0;

	CBCGPPoint ptStart = pt2;

    if (dwStyle == 1)
    {
		ptStart.x += cos(angle) * len2;
		ptStart.y += sin(angle) * len2;
    }
    else if (dwStyle == 2)
    {
		ptStart.x += cos(angle) * length;
		ptStart.y += sin(angle) * length;
    }

	CBCGPComplexGeometry shapeArrow;
	shapeArrow.SetStart(ptStart);

    length = length / sin(delta);
    delta *= M_PI;

    shapeArrow.AddLine(CBCGPPoint(
		cos (angle - delta) * length + ptStart.x,
		sin (angle - delta) * length + ptStart.y));

    shapeArrow.AddLine(CBCGPPoint(
		cos (angle + delta) * length + ptStart.x,
		sin (angle + delta) * length + ptStart.y));

	FillGeometry(shapeArrow, brFill);
	DrawGeometry(shapeArrow, brLine);
}

void CBCGPGraphicsManager::DrawArrow4(const CBCGPPoint& pt1, const CBCGPPoint& pt2, double length, 
	const CBCGPBrush& brFill, const CBCGPBrush& brLine, double arrowAngle, DWORD dwStyle)
{
    const double angle = bcg_angle(CBCGPPoint(pt1), CBCGPPoint(pt2));
    double delta = arrowAngle / 180.0;
    const double len2  = length / 2.0;

	CBCGPPoint ptStart = pt2;

    if (dwStyle == 1)
    {
        ptStart.x += cos (angle) * len2;
        ptStart.y += sin (angle) * len2;
    }
    else if (dwStyle == 2)
    {
        ptStart.x += cos (angle) * length;
        ptStart.y += sin (angle) * length;
    }

	CBCGPComplexGeometry shapeArrow;
	shapeArrow.SetStart(ptStart);

    length = length / sin(delta);
    delta *= M_PI;

	shapeArrow.AddLine(CBCGPPoint(
		cos (angle - delta) * length + ptStart.x,
		sin (angle - delta) * length + ptStart.y));

	shapeArrow.AddLine(CBCGPPoint(
		ptStart.x - cos (angle) * len2 * 1.5,
		ptStart.y - sin (angle) * len2 * 1.5));

	shapeArrow.AddLine(CBCGPPoint(
		cos (angle + delta) * length + ptStart.x,
		sin (angle + delta) * length + ptStart.y));

	FillGeometry(shapeArrow, brFill);
	DrawGeometry(shapeArrow, brLine);
}

void CBCGPGraphicsManager::DrawSide(
		const CBCGPPoint& pt1, const CBCGPPoint& pt2, const CBCGPPoint& pt3, const CBCGPPoint& pt4,
		const CBCGPBrush& brFill, const CBCGPBrush& brLine, double lineWidth)
{
	if (!brFill.IsEmpty())
	{
		CBCGPComplexGeometry shape;
		shape.SetStart(pt1);

		shape.AddLine(pt2);
		shape.AddLine(pt3);
		shape.AddLine(pt4);

		FillGeometry(shape, brFill);
	}

	if (!brLine.IsEmpty())
	{
		DrawLine(pt1, pt2, brLine, lineWidth);
		DrawLine(pt2, pt3, brLine, lineWidth);
		DrawLine(pt3, pt4, brLine, lineWidth);
		DrawLine(pt4, pt1, brLine, lineWidth);
	}
}

void CBCGPGraphicsManager::SetClipRect(const CBCGPRect& rectClip, int nFlags)
{
	CBCGPRectangleGeometry geometry(rectClip);
	SetClipArea(geometry, nFlags);
}

void CBCGPGraphicsManager::SetClipEllipse(const CBCGPEllipse& ellipseClip, int nFlags)
{
	CBCGPEllipseGeometry geometry(ellipseClip);
	SetClipArea(geometry, nFlags);
}

void CBCGPGraphicsManager::SetClipRoundedRect(const CBCGPRoundedRect& rectRounded, int nFlags)
{
	CBCGPRoundedRectangleGeometry geometry(rectRounded);
	SetClipArea(geometry, nFlags);
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPGraphicsResource

void CBCGPGraphicsResource::Set(CBCGPGraphicsManager* pGM, LPVOID lpHandle, LPVOID lpHandle1, LPVOID lpHandle2)
{
	ASSERT(m_lpHandle == NULL || lpHandle == NULL);

	if (pGM != NULL && pGM->m_pOriginal != NULL)
	{
		pGM = pGM->m_pOriginal;
	}

	m_pGM = pGM;
	m_lpHandle = lpHandle;
	m_lpHandle1 = lpHandle1;
	m_lpHandle2 = lpHandle2;

	if (m_pGM != NULL && !m_bIsTemporary)
	{
		ASSERT_VALID(m_pGM);

		if (m_pGM->m_lstRes.Find(this) == NULL)
		{
			m_pGM->m_lstRes.AddTail(this);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPPointsArray

CBCGPRect CBCGPPointsArray::GetBoundsRect() const
{
	if (!m_rectBounds.IsRectNull())
	{
		return m_rectBounds;
	}

	CBCGPRect rt(0, 0, 0, 0);

	int nCount = (int)GetSize();

	if (nCount == 0)
	{
		return rt;
	}

	rt.left   = (*this)[0].x;
	rt.top    = (*this)[0].y;
	rt.right  = rt.left;
	rt.bottom = rt.top;

	for(int i = 1; i < nCount; i++)
	{
		const CBCGPPoint& pt = (*this)[i];

		if(rt.left > pt.x)
		{
			rt.left = pt.x;
		}

		if(rt.top > pt.y)
		{
			rt.top = pt.y;
		}

		if(rt.right < pt.x)
		{
			rt.right = pt.x;
		}

		if(rt.bottom < pt.y)
		{
			rt.bottom = pt.y;
		}
	}

	return rt;
}

void CBCGPPointsArray::Rotate(const CBCGPPoint& pt, double angle)
{
	for(int i = 0; i < GetSize(); i++)
	{
		CBCGPPoint& xpt = (*this)[i];

		double pt_distance = bcg_distance(pt, xpt);
		double pt_angle    = bcg_angle(pt, xpt, TRUE);

		xpt.x += pt_distance * cos(angle + pt_angle);
		xpt.y -= pt_distance * sin(angle + pt_angle);
	}
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPColor

CArray<COLORREF, COLORREF> CBCGPColor::m_arColors;

CBCGPColor::operator COLORREF() const
{
	return RGB(bcg_clamp(bcg_round(r * 255.0), 0, 255), 
		bcg_clamp(bcg_round(g * 255.0), 0, 255), bcg_clamp(bcg_round(b * 255.0), 0, 255));
}

void CBCGPColor::MakeLighter(double dblRatio)
{
	if (IsNull())
	{
		return;
	}

	if (dblRatio < 0)
	{
		MakeDarker(-dblRatio);
		return;
	}

	double H;
	double S;
	double L;

	COLORREF clr = (COLORREF)*this;

	if (clr == 0)
	{
		int nVal = bcg_round(dblRatio * 255.0);
		clr = RGB(nVal, nVal, nVal);
	}
	else
	{
		dblRatio += 1.;

		CBCGPDrawManager::RGBtoHSL (clr, &H, &S, &L);
		
		clr = CBCGPDrawManager::HLStoRGB_ONE (
			H,
			min (1., L * dblRatio),
			min (1., S * dblRatio));
	}

	*this = CBCGPColor(clr);
}

void CBCGPColor::MakeDarker(double dblRatio)
{
	if (IsNull())
	{
		return;
	}

	if (dblRatio < 0)
	{
		MakeLighter(-dblRatio);
		return;
	}

	double H;
	double S;
	double L;

	dblRatio = max(0., 1. - dblRatio);

	COLORREF clr = (COLORREF)*this;

	CBCGPDrawManager::RGBtoHSL (clr, &H, &S, &L);
	
	clr = CBCGPDrawManager::HLStoRGB_ONE (
		H,
		min (1., L * dblRatio),
		min (1., S * dblRatio));

	*this = CBCGPColor(clr);
}

void CBCGPColor::MakePale(double dblLum)
{
	if (IsNull())
	{
		return;
	}

	double H;
	double S;
	double L;

	COLORREF clr = (COLORREF)*this;

	CBCGPDrawManager::RGBtoHSL (clr, &H, &S, &L);
	
	clr = CBCGPDrawManager::HLStoRGB_ONE(H, dblLum, S);

	*this = CBCGPColor(clr);
}

#ifndef _BCGPCHART_STANDALONE

void CBCGPColor::InsertColorSorted(BCGP_COLOR colorName, const CString& strColorLabel)
{
	COLORREF color = (COLORREF)CBCGPColor(colorName);

	if (!strColorLabel.IsEmpty())
	{
		CString str = strColorLabel;

		for (int i = 1; i < str.GetLength(); i++)
		{
			TCHAR tc = str[i];

			if (tc == (TCHAR)_totupper(tc))
			{
				str.Insert(i, _T(" "));
				i++;
			}
		}

		CBCGPColorBar::SetColorName(color, str);
	}

	double H;
	double S;
	double L;
	double H1;
	double S1;
	double L1;

	CBCGPDrawManager::RGBtoHSL (color, &H, &S, &L);

	for (int i = 0; i < (int)m_arColors.GetSize(); i++)
	{
		CBCGPDrawManager::RGBtoHSL (m_arColors[i], &H1, &S1, &L1);

		BOOL bInsert = FALSE;

		if (L == L1)
		{
			if (S == S1)
			{
				bInsert = H < H1;
			}
			else
			{
				bInsert = S < S1;
			}
		}
		else
		{
			bInsert = L < L1;
		}

		if (bInsert)
		{
			m_arColors.InsertAt(i, color);
			return;
		}
	}

	m_arColors.Add(color);
}

#define BCGP_InsertColorSorted(color) InsertColorSorted(color, _T(#color))

const CArray<COLORREF, COLORREF>& CBCGPColor::GetRGBArray()
{
	if (m_arColors.GetSize() > 0)
	{
		return m_arColors;
	}

	BCGP_InsertColorSorted(AliceBlue);
	BCGP_InsertColorSorted(AntiqueWhite);
	BCGP_InsertColorSorted(Aqua);
	BCGP_InsertColorSorted(Aquamarine);
	BCGP_InsertColorSorted(Azure);
	BCGP_InsertColorSorted(Beige);
	BCGP_InsertColorSorted(Bisque);
	BCGP_InsertColorSorted(Black);
	BCGP_InsertColorSorted(BlanchedAlmond);
	BCGP_InsertColorSorted(Blue);
	BCGP_InsertColorSorted(BlueViolet);
	BCGP_InsertColorSorted(Brown);
	BCGP_InsertColorSorted(BurlyWood);
	BCGP_InsertColorSorted(CadetBlue);
	BCGP_InsertColorSorted(Chartreuse);
	BCGP_InsertColorSorted(Chocolate);
	BCGP_InsertColorSorted(Coral);
	BCGP_InsertColorSorted(CornflowerBlue);
	BCGP_InsertColorSorted(Cornsilk);
	BCGP_InsertColorSorted(Crimson);
	BCGP_InsertColorSorted(Cyan);
	BCGP_InsertColorSorted(DarkBlue);
	BCGP_InsertColorSorted(DarkCyan);
	BCGP_InsertColorSorted(DarkGoldenrod);
	BCGP_InsertColorSorted(DarkGray);
	BCGP_InsertColorSorted(DarkGreen);
	BCGP_InsertColorSorted(DarkKhaki);
	BCGP_InsertColorSorted(DarkMagenta);
	BCGP_InsertColorSorted(DarkOliveGreen);
	BCGP_InsertColorSorted(DarkOrange);
	BCGP_InsertColorSorted(DarkOrchid);
	BCGP_InsertColorSorted(DarkRed);
	BCGP_InsertColorSorted(DarkSalmon);
	BCGP_InsertColorSorted(DarkSeaGreen);
	BCGP_InsertColorSorted(DarkSlateBlue);
	BCGP_InsertColorSorted(DarkSlateGray);
	BCGP_InsertColorSorted(DarkTurquoise);
	BCGP_InsertColorSorted(DarkViolet);
	BCGP_InsertColorSorted(DeepPink);
	BCGP_InsertColorSorted(DeepSkyBlue);
	BCGP_InsertColorSorted(DimGray);
	BCGP_InsertColorSorted(DodgerBlue);
	BCGP_InsertColorSorted(Firebrick);
	BCGP_InsertColorSorted(FloralWhite);
	BCGP_InsertColorSorted(ForestGreen);
	BCGP_InsertColorSorted(Fuchsia);
	BCGP_InsertColorSorted(Gainsboro);
	BCGP_InsertColorSorted(GhostWhite);
	BCGP_InsertColorSorted(Gold);
	BCGP_InsertColorSorted(Goldenrod);
	BCGP_InsertColorSorted(Gray);
	BCGP_InsertColorSorted(Green);
	BCGP_InsertColorSorted(GreenYellow);
	BCGP_InsertColorSorted(Honeydew);
	BCGP_InsertColorSorted(HotPink);
	BCGP_InsertColorSorted(IndianRed);
	BCGP_InsertColorSorted(Indigo);
	BCGP_InsertColorSorted(Ivory);
	BCGP_InsertColorSorted(Khaki);
	BCGP_InsertColorSorted(Lavender);
	BCGP_InsertColorSorted(LavenderBlush);
	BCGP_InsertColorSorted(LawnGreen);
	BCGP_InsertColorSorted(LemonChiffon);
	BCGP_InsertColorSorted(LightBlue);
	BCGP_InsertColorSorted(LightCoral);
	BCGP_InsertColorSorted(LightCyan);
	BCGP_InsertColorSorted(LightGoldenrodYellow);
	BCGP_InsertColorSorted(LightGreen);
	BCGP_InsertColorSorted(LightGray);
	BCGP_InsertColorSorted(LightPink);
	BCGP_InsertColorSorted(LightSalmon);
	BCGP_InsertColorSorted(LightSeaGreen);
	BCGP_InsertColorSorted(LightSkyBlue);
	BCGP_InsertColorSorted(LightSlateGray);
	BCGP_InsertColorSorted(LightSteelBlue);
	BCGP_InsertColorSorted(LightYellow);
	BCGP_InsertColorSorted(Lime);
	BCGP_InsertColorSorted(LimeGreen);
	BCGP_InsertColorSorted(Linen);
	BCGP_InsertColorSorted(Magenta);
	BCGP_InsertColorSorted(Maroon);
	BCGP_InsertColorSorted(MediumAquamarine);
	BCGP_InsertColorSorted(MediumBlue);
	BCGP_InsertColorSorted(MediumOrchid);
	BCGP_InsertColorSorted(MediumPurple);
	BCGP_InsertColorSorted(MediumSeaGreen);
	BCGP_InsertColorSorted(MediumSlateBlue);
	BCGP_InsertColorSorted(MediumSpringGreen);
	BCGP_InsertColorSorted(MediumTurquoise);
	BCGP_InsertColorSorted(MediumVioletRed);
	BCGP_InsertColorSorted(MidnightBlue);
	BCGP_InsertColorSorted(MintCream);
	BCGP_InsertColorSorted(MistyRose);
	BCGP_InsertColorSorted(Moccasin);
	BCGP_InsertColorSorted(NavajoWhite);
	BCGP_InsertColorSorted(Navy);
	BCGP_InsertColorSorted(OldLace);
	BCGP_InsertColorSorted(Olive);
	BCGP_InsertColorSorted(OliveDrab);
	BCGP_InsertColorSorted(Orange);
	BCGP_InsertColorSorted(OrangeRed);
	BCGP_InsertColorSorted(Orchid);
	BCGP_InsertColorSorted(PaleGoldenrod);
	BCGP_InsertColorSorted(PaleGreen);
	BCGP_InsertColorSorted(PaleTurquoise);
	BCGP_InsertColorSorted(PaleVioletRed);
	BCGP_InsertColorSorted(PapayaWhip);
	BCGP_InsertColorSorted(PeachPuff);
	BCGP_InsertColorSorted(Peru);
	BCGP_InsertColorSorted(Pink);
	BCGP_InsertColorSorted(Plum);
	BCGP_InsertColorSorted(PowderBlue);
	BCGP_InsertColorSorted(Purple);
	BCGP_InsertColorSorted(Red);
	BCGP_InsertColorSorted(RosyBrown);
	BCGP_InsertColorSorted(RoyalBlue);
	BCGP_InsertColorSorted(SaddleBrown);
	BCGP_InsertColorSorted(Salmon);
	BCGP_InsertColorSorted(SandyBrown);
	BCGP_InsertColorSorted(SeaGreen);
	BCGP_InsertColorSorted(SeaShell);
	BCGP_InsertColorSorted(Sienna);
	BCGP_InsertColorSorted(Silver);
	BCGP_InsertColorSorted(SkyBlue);
	BCGP_InsertColorSorted(SlateBlue);
	BCGP_InsertColorSorted(SlateGray);
	BCGP_InsertColorSorted(Snow);
	BCGP_InsertColorSorted(SpringGreen);
	BCGP_InsertColorSorted(SteelBlue);
	BCGP_InsertColorSorted(Tan);
	BCGP_InsertColorSorted(Teal);
	BCGP_InsertColorSorted(Thistle);
	BCGP_InsertColorSorted(Tomato);
	BCGP_InsertColorSorted(Turquoise);
	BCGP_InsertColorSorted(Violet);
	BCGP_InsertColorSorted(Wheat);
	BCGP_InsertColorSorted(White);
	BCGP_InsertColorSorted(WhiteSmoke);
	BCGP_InsertColorSorted(Yellow);
	BCGP_InsertColorSorted(YellowGreen);

	return m_arColors;
}

#endif

////////////////////////////////////////////////////////////////////////////////////
// CBCGPBrush

CBCGPBrush::CBCGPBrush()
{
	CommonInit();
}

CBCGPBrush::CBCGPBrush(const CBCGPColor& color, double opacity)
{
	CommonInit();

	m_opacity = opacity;
	m_color = color;
}

CBCGPBrush::CBCGPBrush(const CBCGPColor& color, const CBCGPColor& colorGradient, 
	BCGP_GRADIENT_TYPE gradientType, double opacity)
{
	CommonInit();

	m_gradientType = gradientType;
	m_opacity = opacity;
	m_color = color;
	m_colorGradient = colorGradient;
}

CBCGPBrush::CBCGPBrush(const CBCGPColor& color, 
	BCGP_GRADIENT_TYPE gradientType, double opacity)
{
	CommonInit();

	m_gradientType = gradientType;
	m_opacity = opacity;
	m_color = color;
	m_colorGradient = CBCGPColor(1., 1., 1.);
}

CBCGPBrush::CBCGPBrush(const CBCGPBrush& brush)
{
	m_opacity        = brush.m_opacity;
	m_color          = brush.m_color;
	m_colorGradient  = brush.m_colorGradient;
	m_gradientType   = brush.m_gradientType;
	m_nPenWidth		 = brush.m_nPenWidth;
	m_nPenStyle		 = brush.m_nPenStyle;
}

void CBCGPBrush::CommonInit()
{
	m_gradientType = BCGP_NO_GRADIENT;
	m_opacity = 1.;
	m_nPenWidth = 0;
	m_nPenStyle = PS_SOLID;
}

void CBCGPBrush::Destroy(BOOL bDetachFromGM)
{
	if (m_pGM != NULL)
	{
		if (bDetachFromGM)
		{
			m_pGM->Detach(this);
		}

		m_pGM->DestroyBrush(*this);
	}
}

void CBCGPBrush::SetColor(const CBCGPColor& color, double opacity)
{
	SetColors(color, CBCGPColor(), BCGP_NO_GRADIENT, opacity);
}

void CBCGPBrush::SetColors(const CBCGPColor& color, const CBCGPColor& colorGradient, 
		BCGP_GRADIENT_TYPE gradientType, double opacity)
{
	if (m_gradientType == gradientType && m_opacity == opacity && 
		m_color == color && m_colorGradient == colorGradient)
	{
		return;
	}

	if (m_pGM != NULL)
	{
		ASSERT_VALID(m_pGM);

		CBCGPGraphicsManager* pGM = m_pGM;
		m_pGM->DestroyBrush(*this);

		m_pGM = pGM;
	}

	m_gradientType = gradientType;
	m_opacity = opacity;
	m_color = color;
	m_colorGradient = colorGradient;
}

void CBCGPBrush::SetPenAttributes(int nWidth, int nStyle)
{
	m_nPenWidth = nWidth;
	m_nPenStyle = nStyle;
}

void CBCGPBrush::MakeLighter(double dblRatio)
{
	m_color.MakeLighter(dblRatio);
	m_colorGradient.MakeLighter(dblRatio);
}

void CBCGPBrush::MakeDarker(double dblRatio)
{
	m_color.MakeDarker(dblRatio);
	m_colorGradient.MakeDarker(dblRatio);
}

void CBCGPBrush::MakePale(double dblLum)
{
	m_color.MakePale(dblLum);
	m_colorGradient.MakePale(dblLum);
}

void CBCGPBrush::CopyFrom(const CBCGPBrush& src)
{
	if (m_gradientType == src.m_gradientType && 
		m_opacity == src.m_opacity && 
		m_color == src.m_color && 
		m_colorGradient == src.m_colorGradient &&
		m_nPenWidth == src.m_nPenWidth &&
		m_nPenStyle == src.m_nPenStyle)
	{
		return;
	}

	Destroy();

	m_gradientType = src.m_gradientType;
	m_opacity = src.m_opacity;
	m_color = src.m_color;
	m_colorGradient = src.m_colorGradient;
	m_nPenWidth = src.m_nPenWidth;
	m_nPenStyle = src.m_nPenStyle;
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPGeometry

void CBCGPGeometry::Destroy(BOOL bDetachFromGM)
{
	if (m_pGM != NULL)
	{
		if (bDetachFromGM)
		{
			m_pGM->Detach(this);
		}

		m_pGM->DestroyGeometry(*this);
	}
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPArcSegment

CBCGPPoint CBCGPArcSegment::GetArcCenter(const CBCGPPoint& ptFrom, BOOL& bIsLargeArc, double& rX, double& rY) const
{
	const double x1 = ptFrom.x;
	const double y1 = ptFrom.y;

	const double x2 = m_Point.x;
	const double y2 = fabs(m_Point.y - y1) < (double)FLT_EPSILON ? y1 : m_Point.y;

	rX = m_Radius.cx;
	rY = m_Radius.cy;

	CBCGPPoint ptCenter;

	if (rX <= 0.0 || rY <= 0.0)
	{
		rX = 0.0;
		rY = 0.0;
		return ptCenter;
	}

	const double x12  = x1 * x1;
	const double x22  = x2 * x2;
	const double y12  = y1 * y1;
	const double y22  = y2 * y2;

	const double rX2  = rX * rX;
	const double rY2  = rY * rY;
	const double rYX2 = rY2 / rX2;

	const double dY = y1 == y2 ? 0.01 : (y1 - y2);
	const double K = rYX2 * (x2 - x1) / dY;
	const double M = (rYX2 * (x12 - x22) + y12 - y22) / (2.0 * dY);
	const double A = rY2 + rX2 * K * K;
	const double B = 2.0 * (rX2 * K * (M - y1) - rY2 * x1);
	const double C = rY2 * x12 + rX2 * ((M - 2.0 * y1) * M + y12) - rX2 * rY2;
	double D = B * B - 4.0 * A * C;

	bIsLargeArc = m_bIsLargeArc;

	if (D >= 0)
	{
		if (D > 0)
		{
			D = sqrt(D);

			if (y2 <= y1)
			{
				D = -D;
			}

			if ((!m_bIsClockwise || !m_bIsLargeArc) && (m_bIsClockwise || m_bIsLargeArc))
			{
				D = -D;
			}
		}

		ptCenter.x = (-B + D) / (2.0 * A);
		ptCenter.y = K * ptCenter.x + M;
	}
	else
	{
		ptCenter.x = (x2 + x1) / 2.0;
		ptCenter.y = (y2 + y1) / 2.0;

		D = rY / rX;
		rX = bcg_distance(x1 - ptCenter.x, (y1 - ptCenter.y) / D);
		rY = D * rX;

		bIsLargeArc = FALSE;
	}

	return ptCenter;
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPStrokeStyle

CBCGPStrokeStyle::CBCGPStrokeStyle(
	BCGP_CAP_STYLE startCap,
	BCGP_CAP_STYLE endCap,
	BCGP_CAP_STYLE dashCap,
	BCGP_LINE_JOIN lineJoin,
	FLOAT miterLimit,
	BCGP_DASH_STYLE dashStyle,
	FLOAT dashOffset)
{
	m_startCap = startCap;
	m_endCap = endCap;
	m_dashCap = dashCap;
	m_lineJoin = lineJoin;
	m_miterLimit = miterLimit;
	m_dashStyle = dashStyle;
	m_dashOffset = dashOffset;

	CommonInit();
}

CBCGPStrokeStyle::CBCGPStrokeStyle(
	const CArray<FLOAT, FLOAT>& arDashes,
	BCGP_CAP_STYLE startCap,
	BCGP_CAP_STYLE endCap,
	BCGP_CAP_STYLE dashCap,
	BCGP_LINE_JOIN lineJoin,
	FLOAT miterLimit,
	FLOAT dashOffset)
{
	m_arDashes.Append(arDashes);

	m_startCap = startCap;
	m_endCap = endCap;
	m_dashCap = dashCap;
	m_lineJoin = lineJoin;
	m_miterLimit = miterLimit;
	m_dashStyle = BCGP_DASH_STYLE_CUSTOM;
	m_dashOffset = dashOffset;

	CommonInit();
}

CBCGPStrokeStyle::CBCGPStrokeStyle(const CBCGPStrokeStyle& strokeStyle)
{
	CopyFrom(strokeStyle);
}

void CBCGPStrokeStyle::CommonInit()
{
	if (m_dashStyle == BCGP_DASH_STYLE_DOT && m_dashCap == BCGP_CAP_STYLE_FLAT)
	{
		m_dashCap = BCGP_CAP_STYLE_SQUARE;
	}
}

void CBCGPStrokeStyle::CopyFrom(const CBCGPStrokeStyle& strokeStyle)
{
	if (this != &strokeStyle)
	{
		Destroy();

		m_arDashes.RemoveAll();
		m_arDashes.Append(strokeStyle.m_arDashes);

		m_startCap = strokeStyle.m_startCap;
		m_endCap = strokeStyle.m_endCap;
		m_dashCap = strokeStyle.m_dashCap;
		m_lineJoin = strokeStyle.m_lineJoin;
		m_miterLimit = strokeStyle.m_miterLimit;
		m_dashStyle = strokeStyle.m_dashStyle;
		m_dashOffset = strokeStyle.m_dashOffset;
	}
}

BOOL CBCGPStrokeStyle::CompareWith(const CBCGPStrokeStyle& strokeStyle) const
{
	return	m_startCap == strokeStyle.m_startCap &&
			m_endCap == strokeStyle.m_endCap &&
			m_dashCap == strokeStyle.m_dashCap &&
			m_lineJoin == strokeStyle.m_lineJoin &&
			m_miterLimit == strokeStyle.m_miterLimit &&
			m_dashStyle == strokeStyle.m_dashStyle &&
			m_dashOffset == strokeStyle.m_dashOffset;
}

void CBCGPStrokeStyle::Destroy(BOOL bDetachFromGM)
{
	if (m_pGM != NULL)
	{
		if (bDetachFromGM)
		{
			m_pGM->Detach(this);
		}

		m_pGM->DestroyStrokeStyle(*this);
	}
}

void CBCGPStrokeStyle::SetDashStyle(BCGP_DASH_STYLE dashStyle, FLOAT dashOffset)
{
	if (dashStyle == m_dashStyle && dashOffset == m_dashOffset)
	{
		return;
	}

	Destroy();

	m_dashStyle = dashStyle;
	m_dashOffset = dashOffset;
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPTextFormat

CBCGPTextFormat::CBCGPTextFormat()
{
	CommonInit();
}

CBCGPTextFormat::CBCGPTextFormat(
		const CString& strFontFamily,
		float fFontSize,
		long lFontWeight,
		BCGP_FONT_STYLE fontStyle,
		LPCTSTR lpszFontLocale)
{
	CommonInit();
	Create(strFontFamily, fFontSize, lFontWeight, fontStyle, lpszFontLocale);
}

CBCGPTextFormat::CBCGPTextFormat(
		const LOGFONT& lf)
{
	CommonInit();
	CreateFromLogFont(lf);
}

CBCGPTextFormat::CBCGPTextFormat(
		const CBCGPTextFormat& tf)
{
	CopyFrom(tf);
	Create(m_strFontFamily, m_fFontSize, m_lFontWeight, m_fontStyle, m_strFontLocale);
}

BOOL CBCGPTextFormat::Create(
	const CString& strFontFamily,
	float fFontSize,
	long lFontWeight,
	BCGP_FONT_STYLE fontStyle,
	LPCTSTR lpszFontLocale)
{
	if (m_pGM != NULL && GetHandle() != NULL)
	{
		ASSERT_VALID(m_pGM);

		CBCGPGraphicsManager* pGM = m_pGM;
		m_pGM->DestroyTextFormat(*this);

		m_pGM = pGM;
	}

	m_strFontFamily = strFontFamily;
	m_fFontSize = fFontSize;
	m_lFontWeight = lFontWeight;
	m_fontStyle = fontStyle;
	m_strFontLocale = lpszFontLocale == NULL ? _T("") : lpszFontLocale;

	return TRUE;
}

BOOL CBCGPTextFormat::CreateFromLogFont(const LOGFONT& lf)
{
	m_bFromLogFont = TRUE;

	return Create(lf.lfFaceName, (float)lf.lfHeight, lf.lfWeight,
		lf.lfItalic ? BCGP_FONT_STYLE_ITALIC : BCGP_FONT_STYLE_NORMAL);
}

void CBCGPTextFormat::CopyFrom(const CBCGPTextFormat& from)
{
	if (this != &from)
	{
		Destroy();

		m_strFontFamily =			from.m_strFontFamily;   
		m_fFontSize =				from.m_fFontSize;       
		m_lFontWeight =				from.m_lFontWeight;     
		m_fontStyle =				from.m_fontStyle;       
		m_strFontLocale =			from.m_strFontLocale;   
		m_textAlignment =			from.m_textAlignment;   
		m_textAlignmentVert =		from.m_textAlignmentVert;
		m_bWordWrap =				from.m_bWordWrap;       
		m_bClipText	=				from.m_bClipText;
		m_bFromLogFont =			from.m_bFromLogFont;
		m_dblDrawingAngle =			from.m_dblDrawingAngle;
		m_bDrawingAngleWasChanged = from.m_bDrawingAngleWasChanged; 
	}
}

void CBCGPTextFormat::Destroy(BOOL bDetachFromGM)
{
	if (m_pGM != NULL)
	{
		if (bDetachFromGM)
		{
			m_pGM->Detach(this);
		}

		m_pGM->DestroyTextFormat(*this);
	}
}

void CBCGPTextFormat::CommonInit()
{
	m_fFontSize = 0.;
	m_lFontWeight = 0;
	m_fontStyle = BCGP_FONT_STYLE_NORMAL;
	m_textAlignment = BCGP_TEXT_ALIGNMENT_LEADING;
	m_textAlignmentVert = BCGP_TEXT_ALIGNMENT_LEADING;
	m_bWordWrap = FALSE;
	m_bClipText = FALSE;
	m_bFromLogFont = FALSE;
	m_dblDrawingAngle = 0.;
	m_bDrawingAngleWasChanged = FALSE;
}

void CBCGPTextFormat::SetFontSize(float fFontSize)
{
	if (m_fFontSize != fFontSize)
	{
		Destroy();
		m_fFontSize = fFontSize;
		Create(m_strFontFamily, m_fFontSize, m_lFontWeight, m_fontStyle, m_strFontLocale);
	}
}

void CBCGPTextFormat::AdjustTextSize(double dblAngle, CBCGPSize& sz)
{
	if (dblAngle == 0.)
	{
		return;
	}

	dblAngle = bcg_deg2rad(dblAngle);

	CBCGPPoint pt1(sz.cx* cos(dblAngle), sz.cx* sin(dblAngle));
	CBCGPPoint pt2(sz.cy* cos(-M_PI_2 + dblAngle), sz.cy* sin(-M_PI_2 + dblAngle));

	double d = bcg_distance(sz.cx, sz.cy);
	double a = bcg_angle(sz.cx, -sz.cy) + dblAngle;
	CBCGPPoint pt3(d* cos(a), d* sin(a));

	sz.cx = fabs(max(max(max(pt1.x, pt2.x), pt3.x), 0) - min(0, min(min(pt1.x, pt2.x), pt3.x)));
	sz.cy = fabs(max(max(max(pt1.y, pt2.y), pt3.y), 0) - min(0, min(min(pt1.y, pt2.y), pt3.y)));
}


////////////////////////////////////////////////////////////////////////////////////
// CBCGPImage

CBCGPImage::CBCGPImage(UINT uiResID, LPCTSTR lpszType)
{
	CommonInit();

	m_uiResID = uiResID;
	m_szType = lpszType;
}

CBCGPImage::CBCGPImage(const CString& strPath)
{
	CommonInit();
	m_strPath = strPath;
}

CBCGPImage::CBCGPImage(HICON hIcon, CSize sizeIcon, BOOL bIsAlpha)
{
	CommonInit();

	m_hIcon = hIcon;
	m_sizeIcon = sizeIcon;
	m_bIsAlphaIcon = bIsAlpha;
}

CBCGPImage::CBCGPImage(HBITMAP hbmp, BOOL bIsIgnoreAlpha)
{
	CommonInit();

	m_hBitmap = hbmp;
	m_bIsIgnoreAlphaBitmap = bIsIgnoreAlpha;
}

CBCGPImage::CBCGPImage(const CBCGPImage& srcImage)
{
	CopyFrom(srcImage);
}

void CBCGPImage::CommonInit()
{
	m_uiResID = 0;
	m_szType = NULL;
	m_hIcon = NULL;
	m_sizeIcon = CSize(0, 0);
	m_bIsAlphaIcon = FALSE;
	m_hBitmap = NULL;
	m_bIsIgnoreAlphaBitmap = FALSE;
}

void CBCGPImage::Load(UINT uiResID, LPCTSTR lpszType)
{
	Destroy(FALSE);

	m_uiResID = uiResID;
	m_szType = lpszType;
}

void CBCGPImage::Load(const CString& strPath)
{
	m_strPath = strPath;
}

void CBCGPImage::Destroy(BOOL bDetachFromGM)
{
	if (m_pGM != NULL)
	{
		if (bDetachFromGM)
		{
			m_pGM->Detach(this);
		}

		m_pGM->DestroyImage(*this);
	}
}

void CBCGPImage::CopyFrom(const CBCGPImage& srcImage)
{
	if (m_pGM != NULL && m_lpHandle != NULL)
	{
		m_pGM->DestroyImage(*this);
	}

	m_uiResID = srcImage.m_uiResID;
	m_szType = srcImage.m_szType;
	m_strPath = srcImage.m_strPath;
	m_hIcon = srcImage.m_hIcon;
	m_bIsAlphaIcon = srcImage.m_bIsAlphaIcon;
	m_hBitmap = srcImage.m_hBitmap;
	m_bIsIgnoreAlphaBitmap = srcImage.m_bIsIgnoreAlphaBitmap;

	if (srcImage.m_lpHandle != NULL)
	{
		if (m_pGM == NULL)
		{
			m_pGM = srcImage.m_pGM;
		}

		if (m_pGM == NULL)
		{
			return;
		}

		ASSERT_VALID(m_pGM);
		m_pGM->CopyImage((CBCGPImage&)srcImage, (CBCGPImage&)*this, CBCGPRect());
	}
	else
	{
		m_lpHandle = NULL;
	}
}

CBCGPImage& CBCGPImage::operator = (const CBCGPImage& image)
{
	CopyFrom (image);
	return *this;
}

CBCGPSize CBCGPImage::GetSize(CBCGPGraphicsManager* pGM) const
{
	if (pGM == NULL)
	{
		pGM = m_pGM;
	}

	if (pGM == NULL)
	{
		return CBCGPSize();
	}

	return pGM->GetImageSize((CBCGPImage&)*this);
}

BOOL CBCGPImage::CopyTo(CBCGPImage& imageDest, const CBCGPRect& rectSrc) const
{
	if (m_pGM == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pGM);

	return m_pGM->CopyImage((CBCGPImage&)*this, imageDest, rectSrc);
}

HBITMAP CBCGPImage::ExportToBitmap()
{
	if (m_pGM == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(m_pGM);

	return m_pGM->ExportImageToBitmap((CBCGPImage&)*this);
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPSplineGeometry

void CBCGPSplineGeometry::SetPoints(const CBCGPPointsArray& arPoints, BCGP_SPLINE_TYPE type, 
									BOOL bIsClosed, double tension, double bias, double continuity)
{
	Clear();

	m_bIsClosed = bIsClosed;

	int nCount = (int)arPoints.GetSize ();

	if (nCount < 3)
	{
		return;
	}

	m_arPoints.SetSize((nCount - 1) * 3 + 1, 0);

	CBCGPPoint* pDstPoints = m_arPoints.GetData ();
	*pDstPoints++ = arPoints[0];

	if (type == BCGP_SPLINE_TYPE_KB)
	{
		tension    = bcg_clamp(tension, -1.0, 1.0);
		bias       = bcg_clamp(bias, -1.0, 1.0);
		continuity = bcg_clamp(continuity, -1.0, 1.0);

		for (int i = 0; i < nCount - 1; i++)
		{
			const CBCGPPoint& pt0 = arPoints[i == 0 ? (m_bIsClosed ? nCount - 2 : 0) : i - 1];
			const CBCGPPoint& pt1 = arPoints[i];
			const CBCGPPoint& pt2 = arPoints[i + 1];
			const CBCGPPoint& pt3 = arPoints[i == nCount - 2 ? (m_bIsClosed ? 1 : nCount - 1) : i + 2];

			*pDstPoints++ = pt1 + ((pt2 - pt1) * (1.0 - bias) * (1.0 - continuity) +
						(pt1 - pt0) * (1.0 + bias) * (1.0 + continuity)) * (1.0 - tension) / 6.0;
			*pDstPoints++ = pt2 - ((pt3 - pt2) * (1.0 - bias) * (1.0 + continuity) +
						(pt2 - pt1) * (1.0 + bias) * (1.0 - continuity)) * (1.0 - tension) / 6.0;
			*pDstPoints++ = pt2;
		}
	}
	else
	{
		for (int i = 0; i < nCount - 1; i++)
		{
			const CBCGPPoint& pt0 = arPoints[i == 0 ? (m_bIsClosed ? nCount - 2 : 0) : i - 1];
			const CBCGPPoint& pt1 = arPoints[i];
			const CBCGPPoint& pt2 = arPoints[i + 1];
			const CBCGPPoint& pt3 = arPoints[i == nCount - 2 ? (m_bIsClosed ? 1 : nCount - 1) : i + 2];
	
			double x   = bcg_distance(pt2, pt1);
			double x01 = bcg_distance(pt1, pt0);
			double x10 = bcg_distance(pt3, pt2);

			if (x <= FLT_EPSILON)
			{
				x = 1.0;
			}
			if (x01 <= FLT_EPSILON)
			{
				x01 = 1.0;
			}
			if (x10 <= FLT_EPSILON)
			{
				x10 = 1.0;
			}

			*pDstPoints++ = pt1 + ((pt2 - pt1) + (pt1 - pt0) * x / x01) / 6.0;
			*pDstPoints++ = pt2 - ((pt3 - pt2) * x / x10 + (pt2 - pt1)) / 6.0;
			*pDstPoints++ = pt2;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// CBCGPComplexGeometry

void CBCGPComplexGeometry::AddLine(const CBCGPPoint& pt)
{
	if (m_ptStart == CBCGPPoint(-1., -1.))
	{
		return;
	}

	m_arSegments.Add(new CBCGPLineSegment(pt));
}

void CBCGPComplexGeometry::AddBezier(const CBCGPPoint& pt1, const CBCGPPoint& pt2, const CBCGPPoint& pt3)
{
	if (m_ptStart == CBCGPPoint(-1., -1.))
	{
		ASSERT(FALSE);
		return;
	}

	m_arSegments.Add(new CBCGPBezierSegment(pt1, pt2, pt3));
}

void CBCGPComplexGeometry::AddArc(const CBCGPPoint& pt, const CBCGPSize szRadiusIn, 
	BOOL bIsClockwise, BOOL bIsLargeArc, double dblRotationAngle)
{
	if (m_ptStart == CBCGPPoint(-1., -1.))
	{
		ASSERT(FALSE);
		return;
	}

	CBCGPSize szRadius = szRadiusIn;
	szRadius.cx = max(0, szRadius.cx);
	szRadius.cy = max(0, szRadius.cy);

	m_arSegments.Add(new CBCGPArcSegment(pt, szRadius, bIsClockwise, bIsLargeArc, dblRotationAngle));
}

void CBCGPComplexGeometry::AddPoints(const CBCGPPointsArray& arPoints, CBCGPPolygonGeometry::BCGP_CURVE_TYPE curveType)
{
	BOOL bSetStart = m_ptStart == CBCGPPoint(-1., -1.);

	int nCount = (int)arPoints.GetSize();
	if (nCount < 1)
	{
		return;
	}

	if (bSetStart)
	{
		SetStart (arPoints[0]);
	}

	if (curveType == CBCGPPolygonGeometry::BCGP_CURVE_TYPE_LINE)
	{
		for (int i = (bSetStart ? 1 : 0); i < nCount; i++)
		{
			m_arSegments.Add(new CBCGPLineSegment(arPoints[i]));
		}
	}
	else
	{
		if (!bSetStart)
		{
			m_arSegments.Add(new CBCGPLineSegment(arPoints[0]));
		}

		if (nCount == 2)
		{
			m_arSegments.Add(new CBCGPLineSegment(arPoints[1]));
			return;
		}

		for (int i = 1; i < nCount; i++)
		{
			const CBCGPPoint& pt = arPoints[i];
			CBCGPBezierSegment* pSegment = new CBCGPBezierSegment(pt, pt, pt);

			if (i < nCount - 1)
			{
				i++;
			}

			pSegment->m_Point2 = arPoints[i];
			if (i < nCount - 1)
			{
				i++;
			}

			pSegment->m_Point3 = arPoints[i];

			m_arSegments.Add(pSegment);
		}
	}
}

void CBCGPComplexGeometry::Clear()
{
	m_ptStart = CBCGPPoint(-1., -1.);
	m_bIsClosed = TRUE;

	for (int i = 0; i < m_arSegments.GetSize(); i++)
	{
		delete m_arSegments[i];
	}

	m_arSegments.RemoveAll();

	if (m_pGM != NULL)
	{
		ASSERT_VALID(m_pGM);

		CBCGPGraphicsManager* pGM = m_pGM;
		m_pGM->DestroyGeometry(*this);

		m_pGM = pGM;
	}
}

void CBCGPPolygonGeometry::Clear()
{
	m_bIsClosed = TRUE;
	m_curveType = BCGP_CURVE_TYPE_LINE;

	m_arPoints.RemoveAll();

	if (m_pGM != NULL)
	{
		ASSERT_VALID(m_pGM);

		CBCGPGraphicsManager* pGM = m_pGM;
		m_pGM->DestroyGeometry(*this);

		m_pGM = pGM;
	}
}

void CBCGPSplineGeometry::Clear()
{
	m_bIsClosed = TRUE;
	m_arPoints.RemoveAll();

	if (m_pGM != NULL)
	{
		ASSERT_VALID(m_pGM);

		CBCGPGraphicsManager* pGM = m_pGM;
		m_pGM->DestroyGeometry(*this);

		m_pGM = pGM;
	}
}
