//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************

#include "stdafx.h"
#include "BCGPChartAxis.h"
#include "BCGPChartVisualObject.h"

IMPLEMENT_DYNCREATE(CBCGPChartAxis, CObject)
IMPLEMENT_DYNCREATE(CBCGPChartAxisX, CBCGPChartAxis)
IMPLEMENT_DYNCREATE(CBCGPChartAxisY, CBCGPChartAxis)
IMPLEMENT_DYNCREATE(CBCGPChartAxisZ, CBCGPChartAxis)
IMPLEMENT_DYNCREATE(CBCGPChartAxisPolarY, CBCGPChartAxisY)
IMPLEMENT_DYNCREATE(CBCGPChartAxisPolarX, CBCGPChartAxisX)
IMPLEMENT_DYNCREATE(CBCGPChartTernaryAxis, CBCGPChartAxis)

//----------------------------------------------------------------------------//
// Common axis implementation
//----------------------------------------------------------------------------//
CBCGPChartAxis::CBCGPChartAxis()
{
	m_pChart = NULL;
	m_nAxisID = -1;
	m_axisDefaultPosition = (CBCGPChartAxis::AxisDefaultPosition)-1;
}

CBCGPChartAxis::CBCGPChartAxis(int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl)
{
	m_pChart = pChartCtrl;
	m_nAxisID = nAxisID;
	m_axisDefaultPosition = (CBCGPChartAxis::AxisDefaultPosition)-1;

	SetAxisDefaultPosition(position);
}

CBCGPChartAxis::~CBCGPChartAxis(void)
{
}

double CBCGPChartAxis::CalcLog(double dblVal) const
{
	if (dblVal <= 0)
	{
		dblVal = 1.;
	}

	return log(dblVal) / log(m_dblMajorUnit);
}

double CBCGPChartAxis::GetDoubleCorrection() const
{
	return m_dblMajorUnit > DBL_EPSILON ? 3 * DBL_EPSILON : m_dblMinorUnit;
}

void CBCGPChartAxis::SetAxisDefaultPosition(CBCGPChartAxis::AxisDefaultPosition defaultPos)
{
	if (m_axisDefaultPosition == -1)
	{
		m_axisDefaultPosition = defaultPos;
		m_bIsSecondaryAxis = m_axisDefaultPosition == CBCGPChartAxis::ADP_RIGHT || 
							m_axisDefaultPosition == CBCGPChartAxis::ADP_TOP || 
							m_axisDefaultPosition == CBCGPChartAxis::ADP_DEPTH_TOP;

		if (m_bIsSecondaryAxis)
		{
			m_bShowMajorGridLines = FALSE;
			m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE;
		}
		else
		{
			m_bShowMajorGridLines = TRUE;
			m_crossType = CBCGPChartAxis::CT_AUTO;
		}

		if (m_axisDefaultPosition == CBCGPChartAxis::ADP_DEPTH_BOTTOM)
		{
			m_crossType = CBCGPChartAxis::CT_MINIMUM_AXIS_VALUE;
		}
	}
}

void CBCGPChartAxis::CommonInit()
{
	m_bShowMinorGridLines = FALSE;

	m_bReverseOrder = FALSE;

	m_majorTickMarkType = CBCGPChartAxis::TMT_OUTSIDE;
	m_minorTickMarkType = CBCGPChartAxis::TMT_NO_TICKS;
	m_axisLabelType = CBCGPChartAxis::ALT_NEXT_TO_AXIS;

	m_dblCrossOtherAxisValue = 0.;

	m_dblMinimum = 0.;
	m_dblMaximum = 0.;
	m_dblMajorUnit = 0.;
	m_dblMinorUnit = 0.;
	m_bFixedRange = FALSE;
	m_bFixedMaximum = FALSE;
	m_bFixedMinimum = FALSE;

	m_nMinorUnitCount = 5;

	m_strAxisName = _T("Axis Title");

	m_axisNameFormat.m_textFormat.Create(_T("Calibri"), 20);

	m_dblMaxDisplayedValue = 0.;
	m_dblMinDisplayedValue = 0.;
	m_nDisplayedLines = 0;
	m_bEnableRotateFont = TRUE;

	m_bInitialized = FALSE;
	m_bVisible = TRUE;
	m_bAlwaysVisible = FALSE;

	m_bLogScale = FALSE;
	m_dblLogScaleBase = 10;

	m_bDisplayDataBetweenTickMarks = FALSE;

	m_nMajorTickMarkLen = 6;
	m_nMinorTickMarkLen = 3;

	m_nLabelDistanceFromAxis = 0;
	
	m_dblLabelDistance = 2.; 
	m_axisLabelsFormat.SetContentPadding(CBCGPSize(2, 2)); 

	m_szMaxLabelSize.SetSize(0, 0);
	m_rectAxisLabels.SetRectEmpty();
	m_rectAxisName.SetRectEmpty();
	m_bDisplayAxisName = FALSE;

	m_bEmptySeries = FALSE;

	m_ptAxisStart.SetPoint(0, 0);
	m_ptAxisEnd.SetPoint(0, 0);

	m_nColumnOverlapPercent = -1;
	m_nColumnDistancePercent = 150;

	m_bFillMajorUnitInterval = FALSE;
	m_bFormatAsDate = FALSE;

	m_nUnitFillStep = 2;
	m_nFirstIntervalToFill = 0;

	m_pPerpendicularAxis = NULL;
	m_bUsePerpAxisForZoning = TRUE;

	m_nResizeBandSize = 3;

	m_bRoundMinMax = TRUE;

	m_dblBottomOffset = 0;
	m_dblTopOffset = 0;
	m_nAxisGap = 10;

	m_pSplitTop = NULL;
	m_pSplitBottom = NULL;

	BOOL bVert = (m_axisDefaultPosition == CBCGPChartAxis::ADP_LEFT || m_axisDefaultPosition == CBCGPChartAxis::ADP_RIGHT);
	SetVertical(bVert);

	m_bFixedMajorUnit = FALSE;
	m_dblMinAllowedMajorUnit = 0;

	m_bIsFixedIntervalWidth = FALSE;
	m_nFixedIntervalWidth = 10;
	m_nValuesPerInterval = 1;	

	m_bIsFxedUnitCount = FALSE;
	m_nFixedUnitCount = 10;

	m_bPrevFixedRange = FALSE;
	m_bPrevFixedMinimum = FALSE;
	m_bPrevFixedMaximum = FALSE;
	m_dblPrevMinimum = 0.;
	m_dblPrevMaximum = 0.;
	m_dblPrevMinDisplayedValue = 0.;
	m_dblPrevMaxDisplayedValue = 0.;
	m_nPrevFixedIntervalWidth = m_nFixedIntervalWidth;
	m_nPrevValuesPerInterval = m_nValuesPerInterval;

	m_bEnableZoom = TRUE;
	m_bEnableScroll = TRUE;
	m_bZoomed = FALSE;
	m_dblMaxZoomInFactor = 1000;

	m_dblMinScrollValue = -DBL_MAX;
	m_dblMaxScrollValue = DBL_MAX;

	m_dblScrollBarSize = 8;
	m_dblMinThumbSize = 16;

	m_rectScrollBar.SetRectEmpty();
	m_bShowScrollBar = FALSE;
	m_bTickMarksTopRight = FALSE;

	m_dblMinDataPointDistance = DBL_MAX;

	m_pIndexedSeries = NULL;

	m_bUseApproximation = TRUE;
	m_bAlwaysShowScrollBar = FALSE;

	m_bEnableInterlaceLabels = TRUE;

	// 3D properties
	m_dblAxisSize3D = 0;

	m_bEnableInterlaceNearWall = TRUE;
	m_bEnableInterlaceFarWall = TRUE;
	m_bEnableGridLineNearWall = TRUE;
	m_bEnableGridLineFarWall = TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::Reset()
{
	ASSERT_VALID(this);

	m_dblMinimum = 0.;
	m_dblMaximum = 0.;
	m_dblMajorUnit = 0.;
	m_dblMinorUnit = 0.;
	m_bFixedRange = FALSE;
	m_bFixedMinimum = FALSE;
	m_bFixedMaximum = FALSE;
	m_bFormatAsDate = FALSE;
	m_bFixedMajorUnit = FALSE;
	
	m_dblMaxDisplayedValue = 0.;
	m_dblMinDisplayedValue = 0.;
	m_nDisplayedLines = 0;

	m_dblMinAllowedMajorUnit = 0;

	m_nColumnOverlapPercent = 0;
	m_nColumnDistancePercent = 150;

	m_strDataFormat.Empty();

	m_nFirstIntervalToFill = 0;
	m_nUnitFillStep = 2;

	m_bEmptySeries = FALSE;
	m_bInitialized = FALSE;
	m_bDisplayDataBetweenTickMarks = FALSE;

	m_bFillMajorUnitInterval = FALSE;

	if (m_bIsSecondaryAxis)
	{
		m_crossType = CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE;
	}
	else
	{
		m_crossType = CBCGPChartAxis::CT_AUTO;
	}

	m_bIsFixedIntervalWidth = FALSE;
	m_nFixedIntervalWidth = 10;
	m_nValuesPerInterval = 1;

	m_bIsFxedUnitCount = FALSE;
	m_nFixedUnitCount = 10;
	
	m_pIndexedSeries = NULL;

	m_bPrevFixedRange = FALSE;
	m_bPrevFixedMinimum = FALSE;
	m_bPrevFixedMaximum = FALSE;
	m_dblPrevMinimum = 0.;
	m_dblPrevMaximum = 0.;
	m_dblPrevMinDisplayedValue = 0.;
	m_dblPrevMaxDisplayedValue = 0.;
	m_nPrevFixedIntervalWidth = m_nFixedIntervalWidth;
	m_nPrevValuesPerInterval = m_nValuesPerInterval;

	m_dblMinScrollValue = -DBL_MAX;
	m_dblMaxScrollValue = DBL_MAX;

	m_dblMinDataPointDistance = DBL_MAX;

	m_dblMaxZoomInFactor = 1000;

	m_bZoomed = FALSE;

	m_bUseApproximation = TRUE;
	m_bAlwaysShowScrollBar = FALSE;

	m_bEnableInterlaceLabels = TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetFixedMajorUnit(double dblMajorUnit, BOOL bSet)
{
	if (bSet && IsLogScale())
	{
		TRACE0("Fixed major unit mode can't be enabled for logarithmic scales.");
		return;
	}

	if (dblMajorUnit > 0. && bSet)
	{
		m_dblMajorUnit = dblMajorUnit;
		m_dblMinorUnit = dblMajorUnit / m_nMinorUnitCount;
	}

	m_bFixedMajorUnit = bSet;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetMinAllowedMajorUnit(double dblMinAllowedMajorUnit)
{
	m_dblMinAllowedMajorUnit = dblMinAllowedMajorUnit;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetIndexedSeries(CBCGPChartSeries* pSeries)
{
	ASSERT_VALID(this);

	if (pSeries != NULL)
	{
		ASSERT_VALID(pSeries);
	}

	m_pIndexedSeries = pSeries;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetFixedIntervalWidth(int nWidth, int nValuesPerInterval)
{
	m_nFixedIntervalWidth = nWidth;

	if (m_nFixedIntervalWidth < 2)
	{
		m_nFixedIntervalWidth = 2;
	}

	m_nValuesPerInterval = nValuesPerInterval;

	if (m_nValuesPerInterval < 1)
	{
		m_nValuesPerInterval = 1;
	}

	if (m_nValuesPerInterval > m_nFixedIntervalWidth)
	{
		m_nValuesPerInterval = m_nFixedIntervalWidth;
	}

	m_bIsFixedIntervalWidth = TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetFixedUnitCount(int nCount, BOOL bSet)
{
	m_bIsFxedUnitCount = bSet;
	m_nFixedUnitCount = bcg_clamp(nCount, 2, 5000);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::IsAxisVisible() const
{
	ASSERT_VALID(this);

	if (!m_bVisible || m_pChart == NULL)
	{
		return FALSE;
	}

	if (m_bAlwaysVisible)
	{
		return TRUE;
	}

	return HasSeries();
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::HasSeries() const
{
	for (int i = 0; i < m_pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries (i, FALSE);
		if (pSeries != NULL && pSeries->m_bVisible && 
			(pSeries->GetDataPointCount() > 0 || pSeries->IsFormulaSeries() || pSeries->IsVirtualMode()))
		{
			if (pSeries->IsShownOnAxis(this) && pSeries->GetChartImpl() != NULL && 
				pSeries->GetChartImpl()->GetAxisType() != CBCGPBaseChartImpl::AT_NO_AXIS)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SwapDirection(BOOL bAdjustGradientAngles)
{
	ASSERT_VALID(this);

	UNREFERENCED_PARAMETER(bAdjustGradientAngles);
	SetVertical(!IsVertical());
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetVertical(BOOL bVert)
{
	ASSERT_VALID(this);

	if (GetComponentIndex() == CBCGPChartData::CI_Z)
	{
		return;
	}

	m_bIsVertical = bVert;

	if (IsVertical())
	{
		m_axisNameFormat.m_textFormat.SetDrawingAngle(90);
	}
	else
	{
		m_axisNameFormat.m_textFormat.SetDrawingAngle(0);
	}

	if (IsDiagram3D() && GetComponentIndex() == CBCGPChartData::CI_X)
	{
		if (m_pChart != NULL)
		{
			CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

			if (pDiagram3D != NULL)
			{
				pDiagram3D->SetXHorizontal(!IsVertical());
			}
		}
		
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetLogScale(BOOL bSet, double dblLogScaleBase)
{
	if (m_bFixedMajorUnit && bSet)
	{
		TRACE0("Logarithmic scale can't be enabled for an axis in fixed major unit mode.");
		return;
	}

	if (m_bZoomed)
	{
		TRACE0("Logarithmic scale can't be enabled for zoomed axis.");
		return;
	}

	EnableScroll(!bSet);
	EnableZoom(!bSet);

	m_bLogScale = bSet;
	m_dblLogScaleBase = bcg_clamp(dblLogScaleBase, 2., 1000.);
}
//----------------------------------------------------------------------------//
CBCGPRect CBCGPChartAxis::GetBoundingRect() const
{
	ASSERT_VALID(this);

	CBCGPRect rect;

	CBCGPChartAxis* pPerpAxis = GetPerpendecularAxis();
	if (pPerpAxis == NULL)
	{
		return rect;
	}

	ASSERT_VALID(pPerpAxis);

	CBCGPPoint ptAxisStartThis;
	CBCGPPoint ptAxisEndThis;

	CBCGPPoint ptAxisStartPerp;
	CBCGPPoint ptAxisEndPerp;

	GetAxisPos(ptAxisStartThis, ptAxisEndThis);
	pPerpAxis->GetAxisPos(ptAxisStartPerp, ptAxisEndPerp);

	CBCGPRect rectDiagram;
	m_pChart->OnGetPlotAreaRect(rectDiagram);

	CBCGPRect rectThis;
	CBCGPRect rectPerp;

	if (IsVertical())
	{
		rectThis.SetRect(rectDiagram.left, ptAxisStartThis.y, rectDiagram.right, ptAxisEndThis.y);
		rectPerp.SetRect(ptAxisStartPerp.x, rectDiagram.top, ptAxisEndPerp.x, rectDiagram.bottom);
	}
	else
	{
		rectThis.SetRect(ptAxisStartThis.x, rectDiagram.top, ptAxisEndThis.x, rectDiagram.bottom);
		rectPerp.SetRect(rectDiagram.left, ptAxisStartPerp.y, rectDiagram.right, ptAxisEndPerp.y);
	}

	rectThis.Normalize();
	rectPerp.Normalize();

	rect.IntersectRect(rectThis, rectPerp);

	return rect;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::ShowMajorGridLines(BOOL bShow)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pChart);
	m_bShowMajorGridLines = bShow;

	if (m_pChart != NULL)
	{
		m_pChart->SetDirty();
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::ShowMinorGridLines(BOOL bShow)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pChart);
	m_bShowMinorGridLines = bShow;

	if (m_pChart != NULL)
	{
		m_pChart->SetDirty();
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetMinorUnitCount(int nCount)
{
	if (nCount < 1)
	{
		nCount = 1;
	}

	if (nCount > 10)
	{
		nCount = 10;
	}

	m_nMinorUnitCount = nCount;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetAutoDisplayRange()
{
	m_bFixedRange = FALSE;
	m_bFixedMinimum = FALSE;
	m_bFixedMaximum = FALSE;

	RestoreRanges();
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetFixedDisplayRange(double dblMinimum, double dblMaximum, double dblMajorUnit)
{
	OnBeforeSetFixedDisplayRange(dblMinimum, dblMaximum);

	if (dblMinimum == dblMaximum)
	{
		dblMinimum = m_dblMinorUnit > 0. ? dblMaximum - m_dblMinorUnit : dblMaximum - 0.1;
	}

	m_dblMinimum = dblMinimum;
	m_dblMaximum = dblMaximum;

	NormalizeDisplayRange();

	if (IsScrollEnabled())
	{
		if (m_dblMinimum < m_dblMinScrollValue)
		{
			m_dblMinimum = m_dblMinScrollValue;
		}

		if (m_dblMaximum > m_dblMaxScrollValue)
		{
			m_dblMaximum = m_dblMaxScrollValue;
		}
	}

	m_dblMinDisplayedValue = m_dblMinimum;
	m_dblMaxDisplayedValue = m_dblMaximum;

	m_bFixedRange = TRUE;
	m_bFixedMinimum = TRUE;
	m_bFixedMaximum = TRUE;

	if (dblMajorUnit > 0)
	{
		m_dblMajorUnit = dblMajorUnit;
		m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;
	
		m_bFixedMajorUnit = TRUE;
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetFixedMinimumDisplayValue(double dblMinimum, BOOL bByZoom)
{
	if (bByZoom)
	{
		SaveRanges();
	}

	if (IsScrollEnabled() && dblMinimum < m_dblMinScrollValue)
	{
		if (bByZoom)
		{
			dblMinimum = m_dblMinScrollValue;
		}
		else
		{
			m_dblMinScrollValue = dblMinimum;		
		}
		
	}

	m_dblMinimum = dblMinimum;
	m_dblMinDisplayedValue = m_dblMinimum;
	m_bFixedMinimum = TRUE;

	if (IsFixedMaximumDisplayValue())
	{
		m_bFixedRange = TRUE;
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetFixedMaximumDisplayValue(double dblMaximum, BOOL bByZoom)
{
	if (bByZoom)
	{
		SaveRanges();
	}

	if (IsScrollEnabled() && dblMaximum > m_dblMaxScrollValue)
	{
		if (bByZoom)
		{
			dblMaximum = m_dblMaxScrollValue;
		}
		else
		{
			m_dblMaxScrollValue = dblMaximum;
		}
		
	}

	m_dblMaximum = dblMaximum;
	m_dblMaxDisplayedValue = m_dblMaximum;
	m_bFixedMaximum = TRUE;

	if (IsFixedMinimumDisplayValue())
	{
		m_bFixedRange = TRUE;
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::NormalizeDisplayRange()
{
	if (m_dblMaximum < m_dblMinimum)
	{
		m_dblMinDisplayedValue = m_dblMaximum;
		m_dblMaxDisplayedValue = m_dblMinimum;

		m_dblMaximum = m_dblMaxDisplayedValue;
		m_dblMinimum = m_dblMinDisplayedValue;
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::ShowScrollBar(BOOL bShow, BOOL bRedraw)
{
	m_bShowScrollBar = bShow;

	if (bShow)
	{
		EnableScroll();
	}

	if (m_pChart != NULL)
	{
		m_pChart->SetDirty(bShow, bRedraw);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetAlwaysShowScrollBar(BOOL bShow, BOOL bRedraw)
{
	m_bAlwaysShowScrollBar = bShow;

	if (bShow)
	{
		EnableScroll();
	}

	if (m_pChart != NULL)
	{
		m_pChart->SetDirty(bShow, bRedraw);
	}
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::Scroll(double dblUnitsToScroll, BOOL bUnitsInPixels)
{
	if (!IsScrollEnabled())
	{
		return FALSE;
	}
	
	SaveRanges();

	double dblScrollValue = bUnitsInPixels ? dblUnitsToScroll : m_dblMajorUnit * dblUnitsToScroll;

	double dblScrollLow = GetMinDisplayedValue() + dblScrollValue;
	double dblScrollHigh = GetMaxDisplayedValue() + dblScrollValue;
	
	double dblScrollLowNew = -DBL_MAX;
	double dblScrollHighNew = DBL_MAX;

	if (dblScrollLow < m_dblMinScrollValue)
	{
		dblScrollLowNew = m_dblMinScrollValue;
	}
	
	if (dblScrollHigh > m_dblMaxScrollValue)
	{
		dblScrollHighNew = m_dblMaxScrollValue;
	}
	
	double dblLowDiff = fabs(GetMinDisplayedValue() - m_dblMinScrollValue);
	double dblHighDiff = fabs(dblScrollHighNew - GetMaxDisplayedValue());

	double dblScrollValueNew = dblScrollValue < 0 ? dblLowDiff : dblHighDiff;

	if (dblScrollValueNew < fabs(dblScrollValue))
	{
		dblScrollValue = dblScrollValue < 0 ? -dblScrollValueNew : dblScrollValueNew;

		dblScrollLow = GetMinDisplayedValue() + dblScrollValue;
		dblScrollHigh = GetMaxDisplayedValue() + dblScrollValue;
	}

	// check if it was really scrolled
	if (dblScrollLow != GetMinDisplayedValue() || dblScrollHigh != GetMaxDisplayedValue())
	{
		SetFixedDisplayRange(dblScrollLow, dblScrollHigh);
		return TRUE;
	}
	
	return FALSE;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::ScrollTo(double dblValue)
{
	if (!IsScrollEnabled())
	{
		return FALSE;
	}
	
	SaveRanges();

	double dblRange = GetMaxDisplayedValue() - GetMinDisplayedValue();

	double dblMinDisplayedValue = dblValue - dblRange / 2;
	double dblMaxDisplayedValue = dblValue + dblRange / 2;

	if (dblMinDisplayedValue < m_dblMinScrollValue)
	{
		dblMinDisplayedValue = m_dblMinScrollValue;
		dblMaxDisplayedValue = dblMinDisplayedValue + dblRange;
	}

	if (dblMaxDisplayedValue > m_dblMaxScrollValue)
	{
		dblMaxDisplayedValue = m_dblMaxScrollValue;
		dblMinDisplayedValue = dblMaxDisplayedValue - dblRange;

		if (dblMinDisplayedValue < m_dblMinScrollValue)
		{
			dblMinDisplayedValue = m_dblMinScrollValue;
		}
	}

	// check if it was really scrolled
	if (dblMinDisplayedValue != GetMinDisplayedValue() || dblMaxDisplayedValue != GetMaxDisplayedValue())
	{
		SetFixedDisplayRange(dblMinDisplayedValue, dblMaxDisplayedValue);
		return TRUE;
	}
	
	return FALSE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetScrollRange(double dblMinValue, double dblMaxValue)
{
	if (IsZoomed())
	{
		return;
	}

	if (dblMinValue < dblMaxValue)
	{
		m_dblMinScrollValue = dblMinValue;
		m_dblMaxScrollValue = dblMaxValue;
	}
	else
	{
		m_dblMaxScrollValue = dblMinValue;
		m_dblMinScrollValue = dblMaxValue;
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SaveRanges()
{
	if (!m_bZoomed)
	{
		m_bPrevFixedRange = IsFixedDisplayRange();
		m_bPrevFixedMinimum = IsFixedMinimumDisplayValue();
		m_bPrevFixedMaximum = IsFixedMaximumDisplayValue();
		m_dblPrevMinimum = m_dblMinimum;
		m_dblPrevMaximum = m_dblMaximum;
		m_dblPrevMinDisplayedValue = m_dblMinDisplayedValue;
		m_dblPrevMaxDisplayedValue = m_dblMaxDisplayedValue;
		m_nPrevFixedIntervalWidth = m_nFixedIntervalWidth;
		m_nPrevValuesPerInterval = m_nValuesPerInterval;
		m_bZoomed = TRUE;
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::RestoreRanges()
{
	if (m_bZoomed)
	{
		m_bFixedRange = m_bPrevFixedRange;
		m_bFixedMinimum = m_bPrevFixedMinimum;
		m_bFixedMaximum = m_bPrevFixedMaximum;
		m_dblMinimum = m_dblPrevMinimum;
		m_dblMaximum = m_dblPrevMaximum;
		m_dblMinDisplayedValue = m_dblPrevMinDisplayedValue;
		m_dblMaxDisplayedValue = m_dblPrevMaxDisplayedValue;
		m_nFixedIntervalWidth = m_nPrevFixedIntervalWidth;
		m_nValuesPerInterval = m_nPrevValuesPerInterval;
		m_bZoomed = FALSE;
	}
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::SetDisplayDataBetweenTickMarks(BOOL bSet, BOOL bRedraw)
{
	ASSERT_VALID(this);

	if (IsZoomed())
	{
		return FALSE;
	}

	BOOL bIsDisplayDataBetweenTickMarks = IsDisplayDataBetweenTickMarks();
	BOOL bResult = FALSE;

	if (CanDisplayDataBetweenTickMarks() && bSet)
	{
		m_bDisplayDataBetweenTickMarks = bSet;
		bResult = TRUE;
	}
	else
	{
		m_bDisplayDataBetweenTickMarks = FALSE;
	}

	if (bIsDisplayDataBetweenTickMarks != CanDisplayDataBetweenTickMarks() && m_pChart != NULL)
	{
		m_pChart->RecalcMinMaxValues();
		m_pChart->SetDirty(TRUE, bRedraw);
	}

	return bResult;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::HitTest(const CBCGPPoint& pt, BCGPChartHitInfo* pHitInfo, DWORD dwHitInfoFlags)
{
	ASSERT_VALID(this);
	ASSERT(pHitInfo != NULL);

	if (!IsAxisVisible())
	{
		return FALSE;
	}

	if (m_bDisplayAxisName && m_rectAxisName.NormalizedRect().PtInRect(pt) && 
		(dwHitInfoFlags & BCGPChartHitInfo::HIT_AXIS_NAME) != 0)
	{
		pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_NAME;
		pHitInfo->m_nIndex1 = m_nAxisID;
		return TRUE;
	}

	if ((dwHitInfoFlags & BCGPChartHitInfo::HIT_AXIS) != 0 && HitTestAxisLables(pt))
	{
		pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS;
		pHitInfo->m_nIndex1 = m_nAxisID;
		return TRUE;
	}

	if (CanShowScrollBar())
	{
		if (m_rectThumb.PtInRect(pt) && (dwHitInfoFlags & BCGPChartHitInfo::HIT_AXIS_THUMB) != 0)
		{
			pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_THUMB;
			pHitInfo->m_nIndex1 = m_nAxisID;
			return TRUE;
		}

		if (m_rectScrollBar.PtInRect(pt) && (dwHitInfoFlags & BCGPChartHitInfo::HIT_AXIS_SCROLL_BAR) != 0)
		{
			pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_SCROLL_BAR;
			pHitInfo->m_nIndex1 = m_nAxisID;
			return TRUE;
		}
	}

	if ((dwHitInfoFlags & BCGPChartHitInfo::HIT_AXIS) != 0 && HitTestAxisShape(pt))
	{
		pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS;
		pHitInfo->m_nIndex1 = m_nAxisID;
		return TRUE;
	}

	if ((m_dblTopOffset != 0 || m_dblBottomOffset != 0) && m_pChart->IsResizeAxesEnabled())
	{
		CBCGPRect rectBounds = GetBoundingRect();
		CBCGPRect rectBoundsSave = rectBounds;
		if (m_dblTopOffset != 0)
		{
			if (IsVertical())
			{
				rectBounds.bottom = rectBounds.top;
				rectBounds.top -= m_nResizeBandSize;  
			}
			else
			{
				rectBounds.left = rectBounds.right;
				rectBounds.right += m_nResizeBandSize;	
			}

			if (rectBounds.PtInRect(pt))
			{
				pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_RESIZE_BAND;
				pHitInfo->m_nIndex1 = m_nAxisID;
				pHitInfo->m_nIndex2 = 1;
				return TRUE;
			}
		}

		if (m_dblBottomOffset != 0)
		{
			rectBounds = rectBoundsSave;

			if (IsVertical())  
			{
				rectBounds.top = rectBounds.bottom;
				rectBounds.bottom += m_nResizeBandSize;
			}
			else
			{
				rectBounds.right = rectBounds.left;
				rectBounds.left -= m_nResizeBandSize;	
			}

			if (rectBounds.PtInRect(pt))
			{
				pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_RESIZE_BAND;
				pHitInfo->m_nIndex1 = m_nAxisID;
				pHitInfo->m_nIndex2 = 0;
				return TRUE;
			}
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::HitGridLinesTest(const CBCGPPoint& pt, BCGPChartHitInfo* pHitInfo)
{
	ASSERT_VALID(this);
	if (!m_bShowMajorGridLines && !m_bShowMinorGridLines)
	{
		return FALSE;
	}

	int i = 0;
	for (; i < m_arMajorGridLines.GetSize(); i++)
	{
		CBCGPRect rect = m_arMajorGridLines[i];
		if (rect.PtInRect(pt))
		{
			pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_GRIDLINE;
			pHitInfo->m_nIndex1 = m_nAxisID;
			pHitInfo->m_nIndex2 = 1;
			return TRUE;
		}
	}

	for (i = 0; i < m_arMinorGridLines.GetSize(); i++)
	{
		CBCGPRect rect = m_arMinorGridLines[i];
		if (rect.PtInRect(pt))
		{
			pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS_GRIDLINE;
			pHitInfo->m_nIndex1 = m_nAxisID;
			pHitInfo->m_nIndex2 = 0;
			return TRUE;
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::HitTestAxisShape(const CBCGPPoint& pt) const
{
	if (IsDiagram3D() && (fabs(m_ptAxisEnd.x - m_ptAxisStart.x) > 4 * m_nMajorTickMarkLen))
	{
		double dblACoef = (m_ptAxisEnd.y - m_ptAxisStart.y) / (m_ptAxisEnd.x - m_ptAxisStart.x);
		double dblBCoef = m_ptAxisEnd.y - dblACoef * m_ptAxisEnd.x;

		double dblY = dblACoef * pt.x + dblBCoef;
		double dblX = (pt.y - dblBCoef) / dblACoef;
		return (pt.y > (dblY - m_nMajorTickMarkLen) && pt.y < (dblY + m_nMajorTickMarkLen)) ||
			(pt.x > (dblX - m_nMajorTickMarkLen) && pt.x < (dblX + m_nMajorTickMarkLen));
	}

	CBCGPRect rectAxis = GetAxisRect(FALSE, TRUE, FALSE);
	return rectAxis.PtInRect(pt);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::HitTestAxisLables(const CBCGPPoint& pt) const
{
	return (m_axisLabelType != CBCGPChartAxis::ALT_NO_LABELS && m_rectAxisLabels.NormalizedRect().PtInRect(pt));
}
//----------------------------------------------------------------------------//
CBCGPRect CBCGPChartAxis::GetAxisRect(BOOL bCombineWithLabels, BOOL bCombineWithTickMarks, BOOL bCombineWithScrollBar) const
{
	CBCGPPoint ptStart;
	CBCGPPoint ptEnd;

	GetAxisPos(ptStart, ptEnd);

	CBCGPRect rectAxis;
	rectAxis.SetRect(ptStart, ptEnd);
	rectAxis.Normalize();

	if (bCombineWithTickMarks)
	{
		if (IsVertical())
		{
			rectAxis.InflateRect(GetMajorTickMarkLen(TRUE), 0);
		}
		else
		{
			rectAxis.InflateRect(0, GetMajorTickMarkLen(TRUE));
		}
	}

	if (bCombineWithLabels && m_axisLabelType != CBCGPChartAxis::ALT_NO_LABELS)
	{
		rectAxis.UnionRect(m_rectAxisLabels);
	}

	if (bCombineWithScrollBar && !m_rectScrollBar.IsRectEmpty())
	{
		rectAxis.UnionRect(m_rectScrollBar);
	}

	return rectAxis;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcNameRect(CBCGPGraphicsManager* pGM, CBCGPRect& rectPlotArea, const CRect& rectChartArea, BOOL bReposOnly)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pChart);

	if (!bReposOnly)
	{
		m_rectAxisName.SetRectEmpty();
	}

	CBCGPRect rectAxisName; // ignore in adjust layout for custom rects
	BOOL bCustomAxisNameRect = m_pChart->OnGetAxisNameAreaRect(this, rectAxisName);

	if (m_bDisplayAxisName && !bCustomAxisNameRect && IsAxisVisible())
	{
		CBCGPSize szAxisNameSize = m_pChart->OnCalcAxisNameSize(pGM, this);
		double dblResize = 0;

		if (IsDiagram3D())
		{
			if (IsVertical())
			{
				rectPlotArea.left += szAxisNameSize.cx;

				m_rectAxisName.SetRect(rectPlotArea.left - szAxisNameSize.cx, 
											rectPlotArea.CenterPoint().y - szAxisNameSize.cy / 2, 
											rectPlotArea.left - 1, 
											rectPlotArea.CenterPoint().y + szAxisNameSize.cy / 2);

			}
			else
			{
				rectPlotArea.bottom -= szAxisNameSize.cy;
				m_rectAxisName.SetRect(rectPlotArea.CenterPoint().x - szAxisNameSize.cx / 2, 
											rectPlotArea.bottom + 1, 
											rectPlotArea.CenterPoint().x + szAxisNameSize.cx / 2, 
											rectPlotArea.bottom + szAxisNameSize.cy + 1);
			}
			return;
		}

		if (IsVertical())				
		{	
			if (m_bIsSecondaryAxis && rectChartArea.right - rectPlotArea.right < szAxisNameSize.cx)
			{
				dblResize = szAxisNameSize.cx - (rectChartArea.right - rectPlotArea.right);
			}
			else if (!m_bIsSecondaryAxis && rectPlotArea.left - rectChartArea.left < szAxisNameSize.cx)
			{
				dblResize = szAxisNameSize.cx - (rectPlotArea.left - rectChartArea.left);
			}
		}
		else
		{
			if (m_bIsSecondaryAxis && rectPlotArea.top - rectChartArea.top < szAxisNameSize.cy)
			{
				dblResize = szAxisNameSize.cy - (rectPlotArea.top - rectChartArea.top);
			}
			else if (!m_bIsSecondaryAxis && rectChartArea.bottom - rectPlotArea.bottom < szAxisNameSize.cy)
			{
				dblResize = szAxisNameSize.cy - (rectChartArea.bottom - rectPlotArea.bottom);
			}
		}

		if (!bReposOnly)
		{
			if (m_bIsSecondaryAxis)
			{
				IsVertical() ? rectPlotArea.right -= dblResize : rectPlotArea.top += dblResize;
			}
			else
			{
				IsVertical() ? rectPlotArea.left += dblResize : rectPlotArea.bottom -= dblResize;
			}

			if (m_bIsSecondaryAxis)
			{
				IsVertical() ? 
					m_rectAxisName.SetRect(rectPlotArea.right + 1, rectPlotArea.CenterPoint().y - szAxisNameSize.cy / 2, 
							rectPlotArea.right + szAxisNameSize.cx + 1, rectPlotArea.CenterPoint().y + szAxisNameSize.cy / 2) :
					m_rectAxisName.SetRect(rectPlotArea.CenterPoint().x - szAxisNameSize.cx / 2, rectPlotArea.top - szAxisNameSize.cy + 1, 
							rectPlotArea.CenterPoint().x + szAxisNameSize.cx / 2, rectPlotArea.top + 1);
				
			}
			else
			{
				IsVertical() ? 
					m_rectAxisName.SetRect(rectPlotArea.left - szAxisNameSize.cx, rectPlotArea.CenterPoint().y - szAxisNameSize.cy / 2, 
							rectPlotArea.left - 1, rectPlotArea.CenterPoint().y + szAxisNameSize.cy / 2) :
					m_rectAxisName.SetRect(rectPlotArea.CenterPoint().x - szAxisNameSize.cx / 2, rectPlotArea.bottom + 1, 
							rectPlotArea.CenterPoint().x + szAxisNameSize.cx / 2, rectPlotArea.bottom + szAxisNameSize.cy + 1);
			}
		}
		else
		{
			double dblAxisSize = GetAxisSize();

			if (IsVertical())
			{
				
				m_rectAxisName.top = m_ptAxisStart.y - dblAxisSize / 2 - szAxisNameSize.cy / 2;
				m_rectAxisName.bottom = m_rectAxisName.top + szAxisNameSize.cy;
			}
			else
			{
				m_rectAxisName.left = m_ptAxisStart.x + dblAxisSize / 2 - szAxisNameSize.cx / 2;
				m_rectAxisName.right = m_rectAxisName.left + szAxisNameSize.cx;
			}
		}

		m_rectAxisName.Normalize();
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnScaleRatioChanged(const CBCGPSize& sizeScaleRatioNew, const CBCGPSize& sizeScaleRatioOld)
{
	ASSERT_VALID(this);

	m_axisNameFormat.OnScaleRatioChanged(sizeScaleRatioNew, sizeScaleRatioOld);
	m_axisLabelsFormat.OnScaleRatioChanged(sizeScaleRatioNew, sizeScaleRatioOld);

	m_axisLineFormat.SetScaleRatio(sizeScaleRatioNew);
	m_majorGridLineFormat.SetScaleRatio(sizeScaleRatioNew);
	m_minorGridLineFormat.SetScaleRatio(sizeScaleRatioNew);
}	
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetMajorTickMarkLen(int nLen) 
{
	m_nMajorTickMarkLen = nLen;

	if (m_pChart != NULL && IsDiagram3D())
	{
		m_pChart->GetDiagram3D()->m_bLayoutChanged = TRUE;
		m_pChart->SetDirty(TRUE);
	}
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetMajorTickMarkLen(BOOL bScaled) const
{
	ASSERT_VALID(this);

	if (!bScaled || m_pChart == NULL)
	{
		return m_nMajorTickMarkLen;
	}

	ASSERT_VALID(m_pChart);

	return IsVertical() ? m_pChart->GetScaleRatio().cx * m_nMajorTickMarkLen : m_pChart->GetScaleRatio().cy * m_nMajorTickMarkLen;

}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetMinorTickMarkLen(BOOL bScaled) const
{
	ASSERT_VALID(this);

	if (!bScaled || m_pChart == NULL)
	{
		return m_nMinorTickMarkLen;
	}

	ASSERT_VALID(m_pChart);

	return IsVertical() ? m_pChart->GetScaleRatio().cx * m_nMinorTickMarkLen : m_pChart->GetScaleRatio().cy * m_nMinorTickMarkLen;
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetLabelDistance() const
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return m_dblLabelDistance + m_nMajorTickMarkLen;
	}

	ASSERT_VALID(m_pChart);

	if (m_pChart->GetDiagram3D()->IsThickWallsAndFloor())
	{
		return m_dblLabelDistance * (IsVertical() ? m_pChart->GetScaleRatio().cx : m_pChart->GetScaleRatio().cy) + 6;
	}

	return m_dblLabelDistance * (IsVertical() ? m_pChart->GetScaleRatio().cx : m_pChart->GetScaleRatio().cy) + GetMajorTickMarkLen(TRUE);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetScrollBarSize(double dblSize) 
{
	m_dblScrollBarSize = bcg_clamp(dblSize, 4., 100.);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::CanShowScrollBar() const 
{
	if (GetMinDisplayedValue() <= GetMinScrollValue() && GetMaxDisplayedValue() >= GetMaxScrollValue() && 
		!IsAlwaysShowScrollBar() || IsDiagram3D())
	{
		return FALSE;
	}

	return (m_bShowScrollBar || IsAlwaysShowScrollBar()) && IsScrollEnabled() && !IsLogScale();
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetScrollBarSize(BOOL bScaled) const
{
	ASSERT_VALID(this);

	if (!bScaled || m_pChart == NULL)
	{
		return m_dblScrollBarSize;
	}

	ASSERT_VALID(m_pChart);

	return IsVertical() ? m_pChart->GetScaleRatio().cx * m_dblScrollBarSize : m_pChart->GetScaleRatio().cy * m_dblScrollBarSize;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetMinThumbSize(double dblSize)
{
	m_dblMinThumbSize = bcg_clamp(dblSize, 4., 1000.);
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetMinThumbSize(BOOL bScaled) const
{
	ASSERT_VALID(this);

	if (!bScaled || m_pChart == NULL)
	{
		return m_dblMinThumbSize;
	}

	ASSERT_VALID(m_pChart);

	return IsVertical() ? m_pChart->GetScaleRatio().cy * m_dblMinThumbSize : m_pChart->GetScaleRatio().cx * m_dblMinThumbSize;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::AdjustDiagramArea(CBCGPRect& rectDiagramArea, const CBCGPRect& rectPlotArea)
{
	ASSERT_VALID(this);

	BOOL bShowLabels = m_axisLabelType != CBCGPChartAxis::ALT_NO_LABELS;

	if (!IsAxisVisible() || !bShowLabels && !CanShowScrollBar() || GetPerpendecularAxis() == NULL)
	{
		return;
	}


	CBCGPSize szAxisLabels = m_szMaxLabelSize; 
	CBCGPChartAxis::CrossType crossType = IsDiagram3D () ? CBCGPChartAxis::CT_IGNORE : m_crossType;

	double dblWidth = bShowLabels ? szAxisLabels.cx + GetLabelDistance() : 0;
	double dblHeight = bShowLabels ? szAxisLabels.cy + GetLabelDistance() : 0;

	double dblScrollBarSize = GetScrollBarSize(TRUE);

	if (CanShowScrollBar())
	{
		IsVertical() ? dblWidth += dblScrollBarSize : dblHeight += dblScrollBarSize;
	}

	BOOL bReverseOrder = GetPerpendecularAxis()->m_bReverseOrder;

	switch(m_axisLabelType)
	{
	case CBCGPChartAxis::ALT_NO_LABELS:
	case CBCGPChartAxis::ALT_NEXT_TO_AXIS:
		if (m_crossType == CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE && !bReverseOrder || 
				bReverseOrder && !m_bIsSecondaryAxis)
		{
			if (IsVertical() && rectPlotArea.right - rectDiagramArea.right < dblWidth)
			{
				rectDiagramArea.right -= dblWidth - (rectPlotArea.right - rectDiagramArea.right);
			}
			else if (!IsVertical() && rectDiagramArea.top - rectPlotArea.top < dblHeight)
			{
				rectDiagramArea.top += dblHeight - (rectDiagramArea.top - rectPlotArea.top);
			}
		}
		else if (crossType == CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE && bReverseOrder && 
				 m_bIsSecondaryAxis)
		{
			if (IsVertical() && rectDiagramArea.left - rectPlotArea.left < dblWidth)
			{
				rectDiagramArea.left += dblWidth - (rectDiagramArea.left - rectPlotArea.left);
			}
			else if (!IsVertical() &&  rectPlotArea.bottom - rectDiagramArea.bottom < dblHeight)
			{
				rectDiagramArea.bottom -= dblHeight - (rectPlotArea.bottom - rectDiagramArea.bottom);
			}
		}
		else if (crossType == CBCGPChartAxis::CT_AXIS_VALUE || crossType == CBCGPChartAxis::CT_AUTO ||
				 crossType == CBCGPChartAxis::CT_MINIMUM_AXIS_VALUE || 
				 crossType == CBCGPChartAxis::CT_IGNORE)
		{
			if (m_axisDefaultPosition == CBCGPChartAxis::ADP_LEFT)
			{
				if (IsVertical() && rectDiagramArea.left - rectPlotArea.left < dblWidth)
				{
					rectDiagramArea.left += dblWidth - (rectDiagramArea.left - rectPlotArea.left);
				}
				else if (!IsVertical() &&  rectPlotArea.bottom - rectDiagramArea.bottom < dblHeight)
				{
					rectDiagramArea.bottom -= dblHeight - (rectPlotArea.bottom - rectDiagramArea.bottom);
				}
			}
			else if (m_axisDefaultPosition == CBCGPChartAxis::ADP_RIGHT && 
				rectPlotArea.right - rectDiagramArea.right < dblWidth)
			{
				if (IsVertical() && rectPlotArea.right - rectDiagramArea.right < dblWidth)
				{
					rectDiagramArea.right -= dblWidth - (rectPlotArea.right - rectDiagramArea.right);
				}
				else if (!IsVertical() && rectDiagramArea.top - rectPlotArea.top < dblHeight)
				{
					rectDiagramArea.top += dblHeight - (rectDiagramArea.top - rectPlotArea.top);
				}
				
			}
			else if (m_axisDefaultPosition == CBCGPChartAxis::ADP_TOP)
			{
				if (!IsVertical() && rectDiagramArea.top - rectPlotArea.top < dblHeight)
				{
					rectDiagramArea.top += dblHeight - (rectDiagramArea.top - rectPlotArea.top);
				}
				else if (IsVertical() && rectPlotArea.right - rectDiagramArea.right < dblWidth)
				{
					rectDiagramArea.right -= dblWidth - (rectPlotArea.right - rectDiagramArea.right);
				}
				
			}
			else if (m_axisDefaultPosition == CBCGPChartAxis::ADP_BOTTOM)
			{
				if (!IsVertical() && rectPlotArea.bottom - rectDiagramArea.bottom < dblHeight)
				{
					rectDiagramArea.bottom -= dblHeight - (rectPlotArea.bottom - rectDiagramArea.bottom);
				}
				else if (IsVertical() && rectDiagramArea.left - rectPlotArea.left < dblWidth)
				{
					rectDiagramArea.left += dblWidth - (rectDiagramArea.left - rectPlotArea.left);
				}
			}
		}
		break;

	case CBCGPChartAxis::ALT_LOW:
		if (IsVertical() && rectDiagramArea.left - rectPlotArea.left < dblWidth)
		{
			rectDiagramArea.left += dblWidth - (rectDiagramArea.left - rectPlotArea.left);
		}
		else if (!IsVertical() && rectPlotArea.bottom - rectDiagramArea.bottom < dblHeight)
		{
			rectDiagramArea.bottom -= dblHeight - (rectPlotArea.bottom - rectDiagramArea.bottom);
		}
		break;

	case CBCGPChartAxis::ALT_HIGH:
		if (IsVertical() && rectPlotArea.right - rectDiagramArea.right < dblWidth)
		{
			rectDiagramArea.right -= dblWidth - (rectPlotArea.right - rectDiagramArea.right);
		}
		else if (!IsVertical() && rectDiagramArea.top - rectPlotArea.top < dblHeight)
		{
			rectDiagramArea.top += dblHeight - (rectDiagramArea.top - rectPlotArea.top);
		}
		break;
	}
}

void CBCGPChartAxis::UpdateAxisPos(const CBCGPRect& rectDiagramArea)
{
	ASSERT_VALID(this);

	if (m_bIsSecondaryAxis)
	{
		CBCGPPoint ptStart;
		CBCGPPoint ptEnd;

		CBCGPChartAxis* pOpposite = GetOppositeAxis();
		pOpposite->GetAxisPos(ptStart, ptEnd);

		if (IsVertical())
		{
			m_ptAxisStart.y = ptStart.y;
			m_ptAxisEnd.y = ptEnd.y;
		}
		else
		{
			m_ptAxisStart.x = ptStart.x;
			m_ptAxisEnd.x = ptEnd.x;
		}
		return;
	}

	CBCGPRect rectUpdated = rectDiagramArea;

	int nTopGap = m_dblTopOffset != 0 ? m_nAxisGap : 0;
	int nBottomGap = m_dblBottomOffset != 0 ? m_nAxisGap : 0;

	if (IsVertical())
	{
		rectUpdated.bottom -= rectDiagramArea.Height() * m_dblBottomOffset / 100 + nBottomGap;
		rectUpdated.top += rectDiagramArea.Height() * m_dblTopOffset / 100 + nTopGap;
		
		m_ptAxisStart.y = m_bReverseOrder ? rectUpdated.top : rectUpdated.bottom;
		m_ptAxisEnd.y = m_bReverseOrder ? rectUpdated.bottom : rectUpdated.top;
	}
	else
	{
		rectUpdated.left += rectDiagramArea.Width() * m_dblBottomOffset / 100 + nBottomGap;
		rectUpdated.right -= rectDiagramArea.Width() * m_dblTopOffset / 100 + nTopGap;

		m_ptAxisStart.x = m_bReverseOrder ? rectUpdated.right : rectUpdated.left;
		m_ptAxisEnd.x = m_bReverseOrder ? rectUpdated.left : rectUpdated.right;
	}

	if (m_crossType == CT_FIXED_DEFAULT_POS)
	{
		switch (m_axisDefaultPosition)
		{
		case ADP_LEFT:
			m_ptAxisStart.x = rectUpdated.left;
			m_ptAxisEnd.x = rectUpdated.left;
			break;

		case ADP_BOTTOM:
			m_ptAxisStart.y = rectUpdated.bottom;
			m_ptAxisEnd.y = rectUpdated.bottom;
			break;

		case ADP_RIGHT:
			m_ptAxisStart.x = rectUpdated.right;
			m_ptAxisEnd.x = rectUpdated.right;
			break;

		case ADP_TOP:
			m_ptAxisStart.y = rectUpdated.top;
			m_ptAxisEnd.y = rectUpdated.top;
			break;
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcLabelsRect(CBCGPRect& rectDiagramArea)
{
	ASSERT_VALID(this);

	CalcAxisPos(rectDiagramArea);

	if (!IsAxisVisible() || m_axisLabelType == CBCGPChartAxis::ALT_NO_LABELS || GetPerpendecularAxis() == NULL)
	{
		m_rectAxisLabels.SetRectEmpty();
		return;
	}

	double nLabelRectSize = IsVertical() ? m_szMaxLabelSize.cx : m_szMaxLabelSize.cy; 

	double dblDistance = GetLabelDistance();

	if (CanShowScrollBar())
	{
		dblDistance += GetScrollBarSize(TRUE);
	}

	CBCGPPoint ptAxisStart = m_ptAxisStart;
	CBCGPPoint ptAxisEnd = m_ptAxisEnd;

	m_rectAxisLabels.SetRect(m_ptAxisStart, m_ptAxisEnd);

	CBCGPChartAxis* pOppositeAxis = GetOppositeAxis();

	BOOL bOppositeAxisVisible = pOppositeAxis != NULL && 
								pOppositeAxis->IsAxisVisible() && 
								!pOppositeAxis->m_rectAxisLabels.IsRectEmpty();

	CBCGPChartAxis::AxisLabelsType labelStyle = m_axisLabelType;

	BOOL bReverseOrder = GetPerpendecularAxis()->m_bReverseOrder;

	BOOL bRectSet = FALSE;

	switch(labelStyle)
	{
	case CBCGPChartAxis::ALT_NEXT_TO_AXIS:
		if (IsVertical())
		{
			if (m_crossType == CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE && !bReverseOrder || 
				bReverseOrder && !m_bIsSecondaryAxis)
			{
				m_rectAxisLabels.left = m_ptAxisStart.x + dblDistance;
			}
			else if (m_crossType != CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE ||
				m_crossType == CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE && bReverseOrder && m_bIsSecondaryAxis)
			{
				m_rectAxisLabels.left = m_ptAxisStart.x - dblDistance - nLabelRectSize;
			}

			m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
		}
		else
		{
			if (m_crossType == CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE && !bReverseOrder || 
				bReverseOrder && !m_bIsSecondaryAxis)
			{
				m_rectAxisLabels.top = m_ptAxisStart.y - dblDistance - nLabelRectSize;		
			}
			else if (m_crossType != CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE ||
				m_crossType == CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE && bReverseOrder && m_bIsSecondaryAxis)
			{
				m_rectAxisLabels.top = m_ptAxisStart.y + dblDistance;
			}
			
			m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
		}
		break;

	case CBCGPChartAxis::ALT_LOW:
		if (m_bIsSecondaryAxis && bOppositeAxisVisible)
		{
			if (IsVertical())
			{
				if (pOppositeAxis->m_rectAxisLabels.left < rectDiagramArea.left)
				{
					m_rectAxisLabels.left = pOppositeAxis->m_rectAxisLabels.left - dblDistance * 2 - nLabelRectSize;
					m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
					bRectSet = TRUE;
				}
			}
			else
			{
				if (pOppositeAxis->m_rectAxisLabels.bottom > rectDiagramArea.bottom)
				{
					m_rectAxisLabels.top = pOppositeAxis->m_rectAxisLabels.bottom + dblDistance * 2;
					m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
					bRectSet = TRUE;
				}
			}
		}

		if (!bRectSet)
		{
			if (IsVertical())
			{
				m_rectAxisLabels.left = rectDiagramArea.left - dblDistance - nLabelRectSize;
				m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
			}
			else
			{
				m_rectAxisLabels.top = rectDiagramArea.bottom + dblDistance;
				m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
			}
		}
		break;

	case CBCGPChartAxis::ALT_HIGH:			
		
		if (m_bIsSecondaryAxis && bOppositeAxisVisible)
		{
			if (IsVertical())
			{
				if (pOppositeAxis->m_rectAxisLabels.right > rectDiagramArea.right)
				{
					m_rectAxisLabels.left = pOppositeAxis->m_rectAxisLabels.right + dblDistance * 2;
					m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
					bRectSet = TRUE;
				}
			}
			else
			{
				if (pOppositeAxis->m_rectAxisLabels.top < rectDiagramArea.top)
				{
					m_rectAxisLabels.top = pOppositeAxis->m_rectAxisLabels.top - dblDistance * 2 - nLabelRectSize;
					m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
					bRectSet = TRUE;
				}
			}
		}

		if (!bRectSet)
		{
			if (IsVertical())
			{
				m_rectAxisLabels.left = rectDiagramArea.right + dblDistance;
				m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
			}
			else
			{
				m_rectAxisLabels.top = rectDiagramArea.top - dblDistance - nLabelRectSize;
				m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
			}
		}
		break;
	}

	OnCalcDefaultTextAlignment(0, m_rectAxisLabels, rectDiagramArea, m_axisLabelsFormat.m_textFormat);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcAxisPos(const CBCGPRect& rectDiagram, BOOL bStoreAxisPos)
{
	ASSERT_VALID(this);

	if (!IsAxisVisible() || IsDiagram3D())
	{
		return;
	}

	CBCGPPoint ptStart = CBCGPPoint(rectDiagram.left, rectDiagram.bottom);
	CBCGPPoint ptEnd = CBCGPPoint(IsVertical() ? rectDiagram.left : rectDiagram.right, 
		IsVertical() ? rectDiagram.top : rectDiagram.bottom);

	if (m_pChart == NULL)
	{
		m_ptAxisStart = ptStart;
		m_ptAxisEnd = ptEnd;
		return;
	}

	double dblCrossValue = GetCrossValuePos(TRUE);

	if (IsVertical())
	{
		ptStart.x = ptEnd.x = dblCrossValue;
	}
	else
	{
		ptStart.y = ptEnd.y = dblCrossValue;
	}

	if (m_bIsSecondaryAxis)
	{
		CBCGPChartAxis* pPrimaryAxis = GetOppositeAxis();
		if (m_axisLabelType == CBCGPChartAxis::ALT_HIGH && pPrimaryAxis->m_axisLabelType == 
			CBCGPChartAxis::ALT_HIGH && pPrimaryAxis->IsAxisVisible())  
		{
			if (IsVertical())
			{
				ptStart.x = ptEnd.x = pPrimaryAxis->m_rectAxisLabels.right + GetMajorTickMarkLen(TRUE);
			}
			else
			{
				ptStart.y = ptEnd.y = pPrimaryAxis->m_rectAxisLabels.top - GetMajorTickMarkLen(TRUE);
			}
		}
		else if (m_axisLabelType == CBCGPChartAxis::ALT_LOW && pPrimaryAxis->m_axisLabelType == 
			CBCGPChartAxis::ALT_LOW && pPrimaryAxis->IsAxisVisible())
		{
			if (IsVertical())
			{
				ptStart.x = ptEnd.x = pPrimaryAxis->m_rectAxisLabels.left - GetMajorTickMarkLen(TRUE);
			}
			else
			{
				ptStart.y = ptEnd.y = pPrimaryAxis->m_rectAxisLabels.bottom + GetMajorTickMarkLen(TRUE);
			}
		}
	}

	if (bStoreAxisPos)
	{
		m_ptPrevAxisStart = m_ptAxisStart;
		m_ptPrevAxisEnd = m_ptAxisEnd;
	}
	

	m_ptAxisStart = ptStart;
	m_ptAxisEnd = ptEnd;

	UpdateAxisPos(rectDiagram);

	if (!bStoreAxisPos)
	{
		if ((m_ptPrevAxisStart.y != m_ptAxisEnd.y && IsVertical() || m_ptPrevAxisEnd.x != m_ptAxisEnd.x && !IsVertical()) && m_bIsFixedIntervalWidth && m_bFixedRange && m_bZoomed)
		{
			m_bFixedRange = FALSE;
			m_bFixedMaximum = TRUE;
		}
	}

	m_rectScrollBar.SetRectEmpty();
	m_bTickMarksTopRight = FALSE;

	if (CanShowScrollBar())
	{
		if (IsVertical())
		{
			if (m_ptAxisStart.x >= rectDiagram.right)
			{
				m_rectScrollBar.SetRect(m_ptAxisEnd.x, m_ptAxisEnd.y, 
					m_ptAxisStart.x + GetScrollBarSize(TRUE), m_ptAxisStart.y);
				m_bTickMarksTopRight = TRUE;
			}
			else
			{
				m_rectScrollBar.SetRect(m_ptAxisEnd.x, m_ptAxisEnd.y, 
					m_ptAxisStart.x - GetScrollBarSize(TRUE), m_ptAxisStart.y);
			}
		}
		else
		{
			if (m_ptAxisStart.y <= rectDiagram.top)
			{
				m_rectScrollBar.SetRect(m_ptAxisStart.x, m_ptAxisStart.y - GetScrollBarSize(TRUE), 
					m_ptAxisEnd.x, m_ptAxisEnd.y);
				m_bTickMarksTopRight = TRUE;
			}
			else
			{
				m_rectScrollBar.SetRect(m_ptAxisStart.x, m_ptAxisStart.y, 
					m_ptAxisEnd.x, m_ptAxisEnd.y + GetScrollBarSize(TRUE));
			}
		}

		m_rectScrollBar.Normalize();
	}
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetCrossValuePos(BOOL bMapToDiagramArea)
{
	ASSERT_VALID(this);

	CBCGPChartAxis* pPerpendecularAxis = GetPerpendecularAxis();
	double dblValue = 0.;

	if (pPerpendecularAxis == NULL)
	{
		return dblValue;
	}

	if (m_crossType == CBCGPChartAxis::CT_IGNORE || IsDiagram3D())
	{
		if (m_axisDefaultPosition == CBCGPChartAxis::ADP_LEFT || m_axisDefaultPosition == CBCGPChartAxis::ADP_BOTTOM ||
			m_axisDefaultPosition == CBCGPChartAxis::ADP_DEPTH_BOTTOM)
		{
			dblValue = pPerpendecularAxis->GetMinDisplayedValue();
		}
		else if (m_axisDefaultPosition == CBCGPChartAxis::ADP_RIGHT || m_axisDefaultPosition == CBCGPChartAxis::ADP_TOP ||
			m_axisDefaultPosition == CBCGPChartAxis::ADP_DEPTH_TOP)
		{
 			CBCGPPoint ptAxisStart;
 			CBCGPPoint ptAxisEnd;
 			pPerpendecularAxis->GetAxisPos(ptAxisStart, ptAxisEnd);
			dblValue = pPerpendecularAxis->ValueFromPoint(ptAxisEnd);
		}
	}
	else
	{
		dblValue = GetNonIgnoredCrossValue();
	}

	if (bMapToDiagramArea)
	{
		dblValue = pPerpendecularAxis->PointFromValue(dblValue, TRUE);
	}

	return dblValue;
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetNonIgnoredCrossValue() const
{
	ASSERT_VALID(this);

	CBCGPChartAxis* pPerpendecularAxis = GetPerpendecularAxis();
	double dblValue = 0.;

	if (pPerpendecularAxis == NULL)
	{
		return dblValue;
	}

	switch (m_crossType)
	{
	case CBCGPChartAxis::CT_AUTO:
		if (pPerpendecularAxis->GetMinDisplayedValue() < 0. && pPerpendecularAxis->GetMaxDisplayedValue() > 0.)
		{
			dblValue = pPerpendecularAxis->IsComponentXSet() ? 0. : 1.;
		}
		else if (pPerpendecularAxis->GetMinDisplayedValue() >= 0.)
		{
			dblValue = pPerpendecularAxis->GetMinDisplayedValue();
		}
		else if (pPerpendecularAxis->GetMaxDisplayedValue() <= 0.)
		{
			dblValue = pPerpendecularAxis->GetMaxDisplayedValue();
		}
		break;
	case CBCGPChartAxis::CT_AXIS_VALUE:
		dblValue = m_dblCrossOtherAxisValue;
		break;
	case CBCGPChartAxis::CT_MAXIMUM_AXIS_VALUE:
		dblValue = pPerpendecularAxis->GetMaxDisplayedValue();
		break;

	case CBCGPChartAxis::CT_MINIMUM_AXIS_VALUE:
		dblValue = pPerpendecularAxis->GetMinDisplayedValue();
		break;
	}

	return dblValue;
}
//----------------------------------------------------------------------------//
int CBCGPChartAxis::GetNumberOfUnitsToDisplay(UINT nUnitsToDisplay)
{
	ASSERT_VALID(this);

	if (m_bEmptySeries)
	{
		CBCGPChartAxis* pOppositeAxis = GetOppositeAxis();
		if (pOppositeAxis != NULL && !pOppositeAxis->m_bEmptySeries)
		{
			return pOppositeAxis->GetNumberOfUnitsToDisplay(nUnitsToDisplay);
		}

		return 0;
	}

	if (m_bIsFxedUnitCount)
	{
		return m_nFixedUnitCount;
	}

	if (m_szMaxLabelSize.cx == 0)
	{
		m_szMaxLabelSize.cx = 16;
		m_szMaxLabelSize.cy = 16;
	}

	double dblDiagramSize = fabs(GetAxisSize());

	if (m_bIsFixedIntervalWidth)
	{
		return (int) (dblDiagramSize / m_nFixedIntervalWidth) * m_nValuesPerInterval;
	}

	double dblMajorUnitSize = IsVertical() ? m_szMaxLabelSize.cy * 1.1 : m_szMaxLabelSize.cx * 1.1;
	int nDisplayedUnits = nUnitsToDisplay == -1 ?((int)(dblDiagramSize / dblMajorUnitSize)) : nUnitsToDisplay;

	if (nDisplayedUnits > (int) m_nMaxTotalLines)
	{
		nDisplayedUnits = m_nMaxTotalLines;
	}

	if (nDisplayedUnits < 1)
	{
		nDisplayedUnits = 1;
	}

	return nDisplayedUnits;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcMajorMinorUnits(CBCGPGraphicsManager* pGM, UINT nUnitsToDisplay)
{
	ASSERT_VALID(this);

	double dblAxisSize = fabs(GetAxisSize());

	if (dblAxisSize == 0 || m_dblMaximum == m_dblMinimum && m_dblMaximum == 0.)
	{
		return;
	}

	m_nDisplayedLines = GetNumberOfUnitsToDisplay(nUnitsToDisplay); 

	if (m_nDisplayedLines == 0)
	{
		return;
	}

	if (m_bFixedMajorUnit)
	{
		if (m_dblMajorUnit == 0)
		{
			m_dblMajorUnit = fabs(m_dblMaximum - m_dblMinimum) / m_nDisplayedLines;
		}
		CalcMinMaxDisplayedValues(TRUE);
		m_bInitialized = TRUE;

		SetScrollRange(GetMinDisplayedValue(), GetMaxDisplayedValue());
		return;
	}

	if (IsLogScale())
	{
		m_dblMajorUnit = m_dblLogScaleBase;
		m_dblMinorUnit = m_dblLogScaleBase;
		m_nMinorUnitCount = (int)(m_dblLogScaleBase - 1);

		CalcMinMaxDisplayedValues(TRUE);
		CalcMaxLabelSize(pGM);

		double dblLabelSpacing = IsVertical() ? m_szMaxLabelSize.cy * 1.2 : m_szMaxLabelSize.cx * 1.2;
		if (dblLabelSpacing > 0)
		{
			double dblRequiredDiagramSize = IsVertical() ? dblAxisSize - m_szMaxLabelSize.cy : dblAxisSize - m_szMaxLabelSize.cx;

			if (dblRequiredDiagramSize < 0)
			{
				dblRequiredDiagramSize = 1.;
			}

			double dblRange = CalcLog(GetMaxDisplayedValue()) - CalcLog(GetMinDisplayedValue());
			double dblTimes = (dblLabelSpacing * dblRange / dblRequiredDiagramSize);

			m_dblMinorUnit = m_dblMajorUnit = pow(m_dblMajorUnit, ceil(dblTimes));
		}
		
		m_bInitialized = TRUE;

		return;
	}

	double dblMajorUnit = 0.;
	double dblRange = 0.;
	BOOL bCalcMinDisplayedValue = FALSE;
	BOOL bMinMaxOnTheSameSide = (m_dblMaximum > 0 && m_dblMinimum >= 0 || m_dblMaximum <= 0 && m_dblMinimum < 0);

	if (bMinMaxOnTheSameSide && !IsFixedDisplayRange())
	{
		// both maximums are located on the same side from the zero line
		dblRange = m_dblMaximum > 0. ? m_dblMaximum : fabs(m_dblMinimum);
		dblMajorUnit = CalcMajorUnit(10, dblRange);
		if (dblMajorUnit >= fabs(m_dblMaximum - m_dblMinimum) && fabs(m_dblMaximum - m_dblMinimum) > 0.)
		{
			dblRange = fabs(m_dblMaximum - m_dblMinimum);
			bCalcMinDisplayedValue = TRUE;
		}
	}
	else
	{
		dblRange = fabs(m_dblMaximum - m_dblMinimum);
		bCalcMinDisplayedValue = TRUE;
	}

	m_dblMajorUnit = CalcMajorUnit(m_nDisplayedLines, dblRange);

	if (m_dblMajorUnit > dblRange && IsFixedDisplayRange() && m_nDisplayedLines <= 2)
	{
		m_dblMajorUnit = dblRange;
	}

	if (!m_bIsFixedIntervalWidth)
	{
		int nLines = m_nDisplayedLines;

		if (m_nDisplayedLines > 2)
		{
			double dblUnit = m_dblMajorUnit;

			do 
			{
				nLines--;
				if (nLines < 0)
				{
					break;
				}
				dblUnit = CalcMajorUnit(nLines, dblRange);
			}
			while (dblUnit == m_dblMajorUnit);

			nLines++;
		}

		m_nDisplayedLines = nLines;
	}

	if (m_dblMajorUnit < m_dblMinAllowedMajorUnit)
	{
		m_dblMajorUnit = m_dblMinAllowedMajorUnit;
	}

	if (m_dblMajorUnit < DBL_EPSILON && !IsLogScale())
	{
		m_dblMajorUnit = DBL_EPSILON;
	}

	m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;

	if (IsFixedMinimumDisplayValue())
	{
		bCalcMinDisplayedValue = FALSE;
	}

	CalcMinMaxDisplayedValues(bCalcMinDisplayedValue);
	CalcMaxLabelSize(pGM);

	// enter recursion only for the first time
	if (m_nDisplayedLines > 3 && nUnitsToDisplay == -1 && !m_bIsFixedIntervalWidth)
	{
		double nLabelSpacing = IsVertical() ? m_szMaxLabelSize.cy * 1.2 : m_szMaxLabelSize.cx * 1.2;
		
		double nRequiredDiagramSize = IsVertical() ? dblAxisSize - m_szMaxLabelSize.cy : dblAxisSize - m_szMaxLabelSize.cx;
		
		if (nLabelSpacing * m_nDisplayedLines - 1 > nRequiredDiagramSize)
		{
			int nPrevDisplayedLines = m_nDisplayedLines - 1;
			do 
			{
				nPrevDisplayedLines = m_nDisplayedLines;
				CalcMajorMinorUnits(pGM, m_nDisplayedLines - 1);

				nLabelSpacing = IsVertical() ? m_szMaxLabelSize.cy * 1.2 : m_szMaxLabelSize.cx * 1.2;
				nRequiredDiagramSize = IsVertical() ? dblAxisSize - m_szMaxLabelSize.cy : dblAxisSize - m_szMaxLabelSize.cx;

				if (nLabelSpacing * m_nDisplayedLines < nRequiredDiagramSize)
					break;

			}while (nPrevDisplayedLines != m_nDisplayedLines);
		}
	}

	m_bInitialized = TRUE;
	
	SetScrollRange(GetMinDisplayedValue(), GetMaxDisplayedValue());
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcMinMaxDisplayedValues(BOOL bCalcMinDisplayedValue, double dblApproximation)
{
	ASSERT_VALID(this);

	// bCalcMinDisplayedValue is FALSE when the chart shows zero line, or min and max are displayed
	// on different sides of zero. Otherwise the chart displays interval [0, max], or [min, 0]

	if (IsFixedDisplayRange())
	{
		m_dblMinDisplayedValue = m_dblMinimum;
		m_dblMaxDisplayedValue = m_dblMaximum;
		return;
	}

	if (IsFixedMaximumDisplayValue())
	{
		m_dblMaxDisplayedValue = m_dblMaximum;
		m_dblMinDisplayedValue = m_dblMaxDisplayedValue - m_nDisplayedLines * m_dblMajorUnit;
		return;
	}

	if (IsFixedMinimumDisplayValue())
	{
		m_dblMinDisplayedValue = m_dblMinimum;
		m_dblMaxDisplayedValue = m_dblMinDisplayedValue + m_nDisplayedLines * m_dblMajorUnit;
		return;
	}

	if (IsLogScale())
	{
		double dblGrade = ceil(CalcLog(m_dblMaximum));
		m_dblMaxDisplayedValue = pow(m_dblMajorUnit, dblGrade);

		if (m_dblMinimum <= 0)
		{
			m_dblMinDisplayedValue = 1.;
		}
		else
		{
			dblGrade = floor(CalcLog(m_dblMinimum));
			dblGrade = dblGrade - 1;
			m_dblMinDisplayedValue = pow(m_dblMajorUnit, dblGrade);
		}
		return;
	}

	if (m_nDisplayedLines < 4)
	{
		dblApproximation = 1.01;
	}

	if (bCalcMinDisplayedValue)
	{
		if (m_bFixedMajorUnit)
		{
			double dblMiddleValue = m_dblMinimum + (m_dblMaximum - m_dblMinimum) / 2;
			m_dblMinDisplayedValue = dblMiddleValue - (m_nDisplayedLines / 2) * m_dblMajorUnit;
			m_dblMaxDisplayedValue = dblMiddleValue + (m_nDisplayedLines / 2) * m_dblMajorUnit;
			return;
		}

		if (m_dblMaximum > 0 && m_dblMinimum < 0)
		{
			double dblMaxDisplayedVal = ApproximateValue(m_dblMaximum, m_dblMajorUnit, dblApproximation);

			if (!IsFixedMaximumDisplayValue())
			{
				m_dblMaxDisplayedValue = dblMaxDisplayedVal;
			}
			m_dblMinDisplayedValue = m_dblMaxDisplayedValue - (m_nDisplayedLines + 1) * m_dblMajorUnit;

			if (m_dblMinDisplayedValue + m_dblMajorUnit < m_dblMinimum * dblApproximation)
			{
				m_dblMinDisplayedValue = m_dblMinDisplayedValue + m_dblMajorUnit;
			}
		}
		else
		{
			double dblMax = m_dblMaximum > 0. ? m_dblMaximum : fabs(m_dblMinimum);
			double dblMaxDisplayedVal = ApproximateValue(dblMax, m_dblMajorUnit, dblApproximation); 
			int nNumLines = m_nDisplayedLines + 2;

			if (m_nDisplayedLines < 3)
			{
				nNumLines--;
			}

			if (m_dblMaximum > 0)
			{
				if (!IsFixedMaximumDisplayValue())
				{
					m_dblMaxDisplayedValue = dblMaxDisplayedVal;
				}
				m_dblMinDisplayedValue = m_dblMaxDisplayedValue - nNumLines * m_dblMajorUnit;
			}
			else
			{
				m_dblMinDisplayedValue = -dblMaxDisplayedVal;
				if (!IsFixedMaximumDisplayValue())
				{
					m_dblMaxDisplayedValue = m_dblMinDisplayedValue + nNumLines * m_dblMajorUnit;
				}
			}
		}
	}
	else
	{
		double dblMaxValue = m_dblMaximum > 0.  ? m_dblMaximum : fabs(m_dblMinimum);

		double dblMaxDisplayedVal = ApproximateValue(dblMaxValue, m_dblMajorUnit, dblApproximation);

		if (m_dblMaximum > 0)
		{
			if (!IsFixedMaximumDisplayValue())
			{
				m_dblMaxDisplayedValue = dblMaxDisplayedVal;
			}
			if (!IsFixedMinimumDisplayValue())
			{
				m_dblMinDisplayedValue = 0;
			}
		}
		else
		{	
			if (!IsFixedMaximumDisplayValue())
			{
				m_dblMaxDisplayedValue = 0;
			}
			if (!IsFixedMinimumDisplayValue())
			{
				m_dblMinDisplayedValue = -dblMaxDisplayedVal;
			}
		}
	}
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::ApproximateValue(double dblValue, double dblUnit, double dblApproximation)
{
	ASSERT_VALID(this);

	if (IsLogScale())
	{
		return dblValue;
	}

	double dblDecimalPartUnit = dblUnit - floor(dblUnit);
	if (dblDecimalPartUnit != 0)
	{
		double dblDecimalDigits = fabs(log10(dblDecimalPartUnit)) + 1;
		double dblGrade = pow(10.0, (int) dblDecimalDigits);

		__int64 dblGradedUnit = (__int64) (dblUnit * dblGrade);
		__int64 dblGradedValue = (__int64) (dblValue * dblGrade);

		__int64 nStart = (__int64)((dblGradedValue / dblGradedUnit) * dblGradedUnit);

		while (nStart <= dblGradedValue)
		{
			nStart += dblGradedUnit;
		}

		if (nStart - dblGradedValue * dblApproximation  < 0)
		{
			nStart += dblGradedUnit;
		}

		return nStart / dblGrade;
	}

	if (log10(fabs(dblValue)) > 19 || log10(fabs(dblUnit)) > 19)
	{
		return dblValue >= 0 ? dblValue + dblUnit : dblValue - dblUnit;
	}
	
	__int64 nStart = (__int64)((__int64)(dblValue / dblUnit) * dblUnit);
	while (nStart <= (__int64)dblValue)
	{
		nStart += (__int64)dblUnit;
	}

	if (nStart - (__int64)(dblValue * dblApproximation + 0.5)  < 0)
	{
		nStart += (__int64)dblUnit;
	}

	return (double)nStart;
	
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::CanBeScrolled() const
{
	return IsScrollEnabled() && 
		(m_dblMinDisplayedValue > m_dblMinScrollValue || m_dblMaxDisplayedValue < m_dblMaxScrollValue);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetMaxZoomInFactor(double dblValue)
{
	m_dblMaxZoomInFactor = bcg_clamp(dblValue, 1., 100000.);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnCalcThumbRect()
{
	if (m_rectScrollBar.IsRectEmpty())
	{
		return;
	}

	double dblScrollRange = m_dblMaxScrollValue - m_dblMinScrollValue;

	if (dblScrollRange == 0)
	{
		m_rectThumb.SetRectEmpty();
		return;
	}

	double dblMinPercent = (GetMinDisplayedValue() - m_dblMinScrollValue) / dblScrollRange;
	double dblMaxPercent = (m_dblMaxScrollValue - GetMaxDisplayedValue()) / dblScrollRange;

	m_rectThumb = m_rectScrollBar;

	double dblMinThumbSize = GetMinThumbSize(TRUE);
	double dblAxisSize = fabs(GetAxisSize());

	if (IsVertical())
	{
		if (m_bReverseOrder)
		{
			m_rectThumb.bottom = m_ptAxisEnd.y - dblAxisSize * dblMaxPercent;
			m_rectThumb.top = m_ptAxisStart.y + dblAxisSize * dblMinPercent;
		}
		else
		{
			m_rectThumb.bottom = m_ptAxisStart.y - dblAxisSize * dblMinPercent;
			m_rectThumb.top =  m_ptAxisEnd.y + dblAxisSize * dblMaxPercent;
		}

		if (m_rectThumb.Height() < dblMinThumbSize)
		{
			m_rectThumb.bottom = m_rectThumb.CenterPoint().y + dblMinThumbSize / 2;
			m_rectThumb.top = m_rectThumb.bottom - dblMinThumbSize;
			
			if (m_rectThumb.bottom > m_rectScrollBar.bottom)
			{
				m_rectThumb.bottom = m_rectScrollBar.bottom;
				m_rectThumb.top = m_rectThumb.bottom - dblMinThumbSize;
			}
			else if (m_rectThumb.top < m_rectScrollBar.top)
			{
				m_rectThumb.top = m_rectScrollBar.top;
				m_rectThumb.bottom = m_rectThumb.top + dblMinThumbSize;
			}
		}
	}
	else
	{
		if (m_bReverseOrder)
		{
			m_rectThumb.left = m_ptAxisEnd.x + dblAxisSize * dblMaxPercent;
			m_rectThumb.right = m_ptAxisStart.x - dblAxisSize * dblMinPercent;
		}
		else
		{
			m_rectThumb.left = m_ptAxisStart.x + dblAxisSize * dblMinPercent;
			m_rectThumb.right = m_ptAxisEnd.x - dblAxisSize * dblMaxPercent;
		}
		
		if (m_rectThumb.Width() < dblMinThumbSize)
		{
			m_rectThumb.left = m_rectThumb.CenterPoint().x - dblMinThumbSize / 2;
			m_rectThumb.right = m_rectThumb.left + dblMinThumbSize;

			if (m_rectThumb.left < m_rectScrollBar.left)
			{
				m_rectThumb.left = m_rectScrollBar.left;
				m_rectThumb.right = m_rectThumb.left + dblMinThumbSize;
			}
			else if (m_rectThumb.right > m_rectScrollBar.right)
			{
				m_rectThumb.right = m_rectScrollBar.right;
				m_rectThumb.left = m_rectThumb.right - dblMinThumbSize;
			}
		}
	}

	m_rectThumb.Normalize();
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::Zoom(int nMagnifier, const CBCGPPoint& ptZoomCenter)
{
	if (!IsZoomEnabled())
	{
		return FALSE;
	}

 	if (m_bIsFixedIntervalWidth)
 	{
		m_nValuesPerInterval -= nMagnifier;
		if (m_nValuesPerInterval <= 0)
		{
			m_nValuesPerInterval = 1;
		}

		if (m_nValuesPerInterval > m_nFixedIntervalWidth)
		{
			m_nValuesPerInterval = m_nFixedIntervalWidth;
		}

		if (IsFixedDisplayRange())
		{
			m_bFixedMinimum = FALSE;
			m_bFixedRange = FALSE;
		}
	}
	else 
	{
		double dblZoomValue = nMagnifier > 0 ? nMagnifier : 1. / nMagnifier;

		if (dblZoomValue == 1)
		{
			dblZoomValue = 2;
		}

		if (dblZoomValue == -1)
		{
			dblZoomValue = 0.5;
		}

		if (dblZoomValue == 0)
		{
			return FALSE;
		}

		double dblRange = GetMaxDisplayedValue() - GetMinDisplayedValue();

		if (!IsComponentXSet() && dblRange <= 1 && nMagnifier > 0)
		{
			return FALSE;
		}

		double dblCenterValue = GetMinDisplayedValue() + dblRange / 2;
		double dblOffset = 0;

		double dblMin = dblCenterValue - dblRange / 2 / fabs(dblZoomValue);

		if (ptZoomCenter.x != 0 && ptZoomCenter.y != 0)
		{
			dblCenterValue = ValueFromPoint(ptZoomCenter);
			dblOffset = fabs(dblCenterValue - GetMinDisplayedValue()) / fabs(dblZoomValue);

			dblMin = dblCenterValue - dblOffset;
		}

		double dblMax = dblMin + dblRange / fabs(dblZoomValue);

		double dblNewRange = dblMax - dblMin;
		double dblScrollRange = m_dblMaxScrollValue - m_dblMinScrollValue;

		if (dblNewRange < dblScrollRange && dblScrollRange / dblNewRange > m_dblMaxZoomInFactor)
		{
			return FALSE;
		}

		if (m_bFixedMajorUnit || !IsComponentXSet())
		{
			double dblMaxAllowedRange = fabs(GetAxisSize()) * m_dblMajorUnit / m_nMinorUnitCount;

			if (dblNewRange > dblMaxAllowedRange)
			{
				dblMin = dblCenterValue - (dblMaxAllowedRange / 2);
				dblMax = dblCenterValue + (dblMaxAllowedRange / 2);
			}
		}

		if (!IsComponentXSet() && dblMin < 0)
		{
			dblMax += fabs(dblMin);
			dblMin = 0.;
		}

		if (dblMin < m_dblMinScrollValue)
		{
			dblMin = m_dblMinScrollValue;
		}

		if (dblMax > m_dblMaxScrollValue)
		{
			dblMax = m_dblMaxScrollValue;
		}

		if (dblMax <= dblMin)
		{
			if (dblMin < m_dblMinScrollValue + m_dblMajorUnit)
			{
				dblMax = dblMin + m_dblMajorUnit;
			}
			else
			{
				dblMin = dblMax - m_dblMajorUnit;
			}
		}

		SaveRanges();			
		SetFixedDisplayRange(dblMin, dblMax);
	}


	return TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::UnZoom()
{
	RestoreRanges();
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::CalcMajorUnit(int nMaxLines, double dblRange, double dblOffset)
{
	ASSERT_VALID(this);

	if (dblOffset == 0. || dblRange <= 0)
	{
		return 1.;
	}

	double dblValue = dblRange;
	double dblNormal = log10(dblValue);
	double dblStep = 0.0;

	// calculate standard range for m_nLines
	// calculate values for step 1
	// the upper and bottom values should not exceed 1.05 part of the total range
	double dblTopRange_1 = nMaxLines / dblOffset;
	double dblBottomRange_1 = (nMaxLines / 2.) / dblOffset;

	// calculate bottom value for range with step 0.1
	double dblBottomRange_01 = (nMaxLines / 10.) / dblOffset;

	// calculate bottom value for range with step 0.5
	double dblBottomRange_05 = (nMaxLines / 5.) / dblOffset;

	// calculate upper value for range with step 2
	double dblTopRange_2 = (nMaxLines * 2.)/dblOffset;

	// calculate upper value for range with step 5
	double dblTopRange_5 = (nMaxLines * 5.)/dblOffset;

	// incoming values should be normalized with log10, so the basic ranges should be normalized too
	double dblLogBottom_01 = log10(dblBottomRange_01);
	double dblLogBottom_05 = log10(dblBottomRange_05);
	double dblLogBottom_1 = log10(dblBottomRange_1);
	double dblLogUpper_1 = log10(dblTopRange_1); 
	double dblLogUpper_2 = log10(dblTopRange_2); 
	double dblLogUpper_5 = log10(dblTopRange_5); 

	// find grade
	double dblDecimalPart = 0.;
	double dblGrade = 0.;

	dblDecimalPart = dblNormal - floor(dblNormal);

	double dblIntPart = dblNormal - dblDecimalPart;
	dblGrade = pow (10.0, dblIntPart);

	if (dblDecimalPart <= dblLogBottom_01)
	{
		dblStep = 0.1 * dblGrade;
	}
	else if (dblDecimalPart > dblLogBottom_01 && dblDecimalPart <= dblLogBottom_05)
	{
		dblStep = 0.2 * dblGrade;
	}
	else if (dblDecimalPart > dblLogBottom_05 && dblDecimalPart <= dblLogBottom_1)
	{
		dblStep = 0.5 * dblGrade;
	}
	else if (dblDecimalPart > dblLogBottom_1 && dblDecimalPart <= dblLogUpper_1)
	{
		dblStep = 1. * dblGrade; 
	}
	else if (dblDecimalPart > dblLogUpper_1 && dblDecimalPart <= dblLogUpper_2)
	{
		dblStep = 2. * dblGrade; 
	}
	else if (dblDecimalPart > dblLogUpper_2 && dblDecimalPart <= dblLogUpper_5)
	{
		dblStep = 5. * dblGrade; 
	}
	else
	{
		dblStep = 10. * dblGrade; 
	}

	return dblStep;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcMaxLabelSize(CBCGPGraphicsManager* pGM)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pGM);
	ASSERT(m_pChart != NULL);

	m_szMaxLabelSize.SetSize(0, 0);

	if (m_pChart == NULL || !IsAxisVisible() || m_axisLabelType == CBCGPChartAxis::ALT_NO_LABELS ||
		m_pChart->OnGetMaxLabelSize(this, m_szMaxLabelSize) || m_dblMajorUnit == 0.)
	{
		return;
	}

	CBCGPSize sizeText(0,0); 
	CBCGPSize sizeTextMax(0,0);
	CString strLabel;

	double dblMinValue = GetMinDisplayedValue();
	double dblMaxValue = GetMaxDisplayedValue();

	if (IsLogScale())
	{
		double dblUnitStep = CalcLog(GetMinDisplayedValue());

		double dblCurrValue = dblMinValue;

		while (dblCurrValue <= dblMaxValue)
		{
			GetDisplayedLabel(dblCurrValue, strLabel);
			sizeText = m_pChart->OnCalcAxisLabelSize(pGM, this, dblCurrValue, strLabel, m_axisLabelsFormat.m_textFormat);

			sizeTextMax.cx = max(sizeText.cx, sizeTextMax.cx);
			sizeTextMax.cy = max(sizeText.cy, sizeTextMax.cy);

			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
	}
	else
	{
		BOOL bVariableAngle = IsVariableLabelAngle();
		double dblDrawingAngle = m_axisLabelsFormat.m_textFormat.GetDrawingAngle();

		dblMinValue = ApproximateValue(dblMinValue, m_dblMajorUnit, 1.);

		for (double dblVal = dblMinValue; dblVal <= dblMaxValue; dblVal += m_dblMajorUnit)
		{
			GetDisplayedLabel(dblVal, strLabel);
			if (!strLabel.IsEmpty())
			{
				if (bVariableAngle)
				{
					m_axisLabelsFormat.m_textFormat.SetDrawingAngle(GetVariableLabelAngle(dblVal));
				}

				sizeText = m_pChart->OnCalcAxisLabelSize(pGM, this, dblVal, strLabel, m_axisLabelsFormat.m_textFormat);
				sizeTextMax.cx = max(sizeText.cx, sizeTextMax.cx);
				sizeTextMax.cy = max(sizeText.cy, sizeTextMax.cy);
				
				if (bVariableAngle)
				{
					m_axisLabelsFormat.m_textFormat.SetDrawingAngle(dblDrawingAngle);
				}
			}
		}
	}

	CBCGPSize szPadding = m_axisLabelsFormat.GetContentPadding(TRUE);

	m_szMaxLabelSize.cx = sizeTextMax.cx + szPadding.cx * 2;
	m_szMaxLabelSize.cy = sizeTextMax.cy + szPadding.cy * 2;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::GetFormattedValue(double dblValue, CString& strValue)
{
	ASSERT_VALID(this);

	CString strFormat;
	GetFormatString(strFormat, dblValue);
	
	strValue.Format(strFormat, dblValue);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::GetDisplayedLabel(double dblValue, CString& strLabel)
{
	ASSERT_VALID(this);

	if (m_pIndexedSeries != NULL && m_pIndexedSeries->IsIndexMode() && m_pIndexedSeries->IsXComponentSet())
	{
		dblValue = m_pIndexedSeries->GetDataPointValue((int)dblValue, CBCGPChartData::CI_X).GetValue();
	}

	if (m_bFormatAsDate)
	{
		if (dblValue == 0.)
		{
			strLabel.Empty();
			return;
		}

		COleDateTime dt(dblValue);

		if (m_strDataFormat.IsEmpty())
		{
			strLabel = dt.Format(LOCALE_NOUSEROVERRIDE, LANG_USER_DEFAULT);
		}
		else
		{	
			strLabel = dt.Format(m_strDataFormat);
		}
	}
	else
	{
		if (fabs(dblValue) < GetDoubleCorrection() && m_dblMajorUnit > 0.)
		{
			if (IsLogScale())
			{
				strLabel.Format(_T("%.2G"), dblValue);
				return;
			}
			else if (log10(m_dblMajorUnit) > log10(fabs(dblValue)))
			{
				dblValue = 0.;
			}
		}
		GetFormattedValue(dblValue, strLabel);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::GetFormatString(CString& strFormat, double dblValue)
{
	if (m_strDataFormat.IsEmpty())
	{
		if (!m_bInitialized || m_dblMajorUnit == 0)
		{
			strFormat = _T("%.f");
			return;
		}

		double dblNormalMajor = fabs(log10(m_dblMajorUnit));
		double dblNormal = dblValue != 0 ? fabs(log10(fabs(dblValue))) : 0;

		if (dblNormal >= 8 && fabs(dblNormal - dblNormalMajor) < 6 && dblNormal != 0)
		{
			strFormat = _T("%.3G");
		}
		else if (dblNormal == 0)
		{
			strFormat = _T("%.f");
		}
		else
		{
			strFormat = _T("%.9G");
		}
	}
	else
	{
		strFormat = m_strDataFormat;
	}
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::ValueFromPointByScrollRange(const CBCGPPoint& pt, CBCGPChartAxis::RoundType roundType)
{
	ASSERT_VALID(this);

	if (GetMinScrollValue() == -DBL_MAX)
	{
		return ValueFromPoint(pt, roundType);
	}

	double dblMinRange = GetMinScrollValue();
	double dblMaxRange = GetMaxScrollValue();

	double dblRange = dblMaxRange - dblMinRange;

	if (dblRange == 0)
	{
		return 0;
	}

	double dblAxisSize = fabs(GetAxisSize());
	double dblPointOffset = IsVertical() ? 
		(m_bReverseOrder ? pt.y - m_ptAxisStart.y : m_ptAxisStart.y - pt.y) : 
		(m_bReverseOrder ? m_ptAxisStart.x - pt.x : pt.x - m_ptAxisStart.x);

	double dblVal = dblMinRange + dblRange / dblAxisSize * dblPointOffset;

	if (IsDisplayDataBetweenTickMarks())
	{
		dblVal -= GetBaseUnitSize() / 2;
	}

	if (roundType == CBCGPChartAxis::RT_EXACT)
	{
		return dblVal;
	}

	if (roundType == CBCGPChartAxis::RT_FLOOR)
	{
		dblVal = dblMinRange + (int)((dblVal - dblMinRange) / m_dblMajorUnit) * m_dblMajorUnit;
	}
	else
	{
		dblVal = dblMinRange + ((int)((dblVal - dblMinRange) / m_dblMajorUnit) + 1) * m_dblMajorUnit;
	}

	return dblVal;
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::ValueFromPoint(const CBCGPPoint& pt, CBCGPChartAxis::RoundType roundType)
{
	ASSERT_VALID(this);

	double dblRange = GetMaxDisplayedValue() - GetMinDisplayedValue();

	if (dblRange == 0)
	{
		return 0;
	}

	double dblAxisSize = fabs(GetAxisSize());
	double dblPointOffset = IsVertical() ? 
		(m_bReverseOrder ? pt.y - m_ptAxisStart.y : m_ptAxisStart.y - pt.y) : 
		(m_bReverseOrder ? m_ptAxisStart.x - pt.x : pt.x - m_ptAxisStart.x);

	double dblVal = GetMinDisplayedValue() + dblRange / dblAxisSize * dblPointOffset;

	if (IsDisplayDataBetweenTickMarks())
	{
		dblVal -= GetBaseUnitSize() / 2;
	}

	if (roundType == CBCGPChartAxis::RT_EXACT)
	{
		return dblVal;
	}

	if (roundType == CBCGPChartAxis::RT_FLOOR)
	{
		dblVal = GetMinDisplayedValue() + (int)((dblVal - GetMinDisplayedValue()) / m_dblMajorUnit) * m_dblMajorUnit;
	}
	else
	{
		dblVal = GetMinDisplayedValue() + ((int)((dblVal - GetMinDisplayedValue()) / m_dblMajorUnit) + 1) * m_dblMajorUnit;
	}

	return dblVal;
}
//----------------------------------------------------------------------------//
CBCGPPoint CBCGPChartAxis::PointFromValue3D(double dblValue, BOOL bForceValueOnThickMark, 
											BOOL bForceValueOnWall, BOOL bLogValue)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return CBCGPPoint();
	}

	double dblVal = CBCGPChartAxis::PointFromValue(dblValue, bForceValueOnThickMark, bLogValue);

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

	if (!IsDiagram3D() || pDiagram3D == NULL)
	{
		CBCGPPoint ptRet;
		
		if (IsVertical())
		{
			ptRet.x = m_ptAxisStart.x;
			ptRet.y = dblVal;
		}
		else
		{
			ptRet.y = m_ptAxisStart.y;
			ptRet.x = dblVal;
		}

		return ptRet;
	}

	CBCGPPoint pt = GetPointOnAxisPlane3D(dblVal, bForceValueOnWall);
	return pDiagram3D->TransformPoint(pt, pDiagram3D->GetDiagramRect().CenterPoint());
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::CalcNormalValue(double dblValue) const
{
	ASSERT_VALID(this);

	double dblMinValue = GetMinDisplayedValue();
	double dblMaxValue = GetMaxDisplayedValue();

	double dblRange = dblMaxValue - dblMinValue;

	if (dblMinValue == 0 && IsLogScale())
	{
		dblMinValue = 1.;
	}

	if (dblRange == 0)
	{
		return 0;
	}

	if (IsLogScale() && dblValue > 0)
	{
		dblValue = CalcLog(dblValue);
		dblRange = CalcLog(dblMaxValue) - CalcLog(dblMinValue);
		dblMinValue = CalcLog(dblMinValue);
	}

	double dblAxisSize = fabs(GetAxisSize(TRUE));

	return ((dblValue - dblMinValue) * dblAxisSize / dblRange);
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::PointFromValueOptimized3D(double dblValue, const CBCGPPoint& ptAxisStart,
											const CBCGPPoint& /*ptAxisEnd*/, double dblAxisSize, 
											BOOL bIsDisplayDataBetweenTickMarks, CBCGPChartData::ComponentIndex ci, 
											BOOL bIsLogScale)
{
	ASSERT_VALID(this);

	BOOL bLogValue = TRUE;

	double dblMinValue = GetMinDisplayedValue();
	double dblMaxValue = GetMaxDisplayedValue();

	if (dblMinValue == 0 && IsLogScale())
	{
		dblMinValue = 1.;
	}

	double dblRange = dblMaxValue - dblMinValue;

	if (dblRange == 0)
	{
		return IsVertical() ? ptAxisStart.y : ptAxisStart.x;
	}

	double dblPoint = 0.;

	if (bIsDisplayDataBetweenTickMarks)
	{
		dblValue += GetBaseUnitSize() / 2;
	}

	if (bIsLogScale && (dblValue > 0 || !bLogValue))
	{
		if (bLogValue)
		{
			dblValue = CalcLog(dblValue);
		}

		dblRange = CalcLog(dblMaxValue) - CalcLog(dblMinValue);
		dblMinValue = CalcLog(dblMinValue);
	}

	double dblStart = 0;

	switch (ci)
	{
	case CBCGPChartData::CI_X:
	case CBCGPChartData::CI_Y:
		dblStart = IsVertical() ? m_ptAxisStart3D.y : m_ptAxisStart3D.x;
		break;

	case CBCGPChartData::CI_Z:
		dblStart = m_ptAxisStart3D.z;
		break;
	}

	dblPoint = dblStart + ((dblValue - dblMinValue) * dblAxisSize / dblRange);
	
	return dblPoint;
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::PointFromValue(double dblValue, BOOL bForceValueOnThickMark, BOOL bLogValue)
{
	ASSERT_VALID(this);

	double dblMinValue = GetMinDisplayedValue();
	double dblMaxValue = GetMaxDisplayedValue();

	if (dblMinValue == 0 && IsLogScale())
	{
		dblMinValue = 1.;
	}

	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	GetAxisPos(ptAxisStart, ptAxisEnd);

	double dblRange = dblMaxValue - dblMinValue;

	if (dblRange == 0)
	{
		return IsVertical() ? ptAxisStart.y : ptAxisStart.x;
	}

	double dblPoint = 0.;

 	if (IsDisplayDataBetweenTickMarks() && !bForceValueOnThickMark)
 	{
 		dblValue += GetBaseUnitSize() / 2;
 	}

	if (IsLogScale() && (dblValue > 0 || !bLogValue))
	{
		if (bLogValue)
		{
			dblValue = CalcLog(dblValue);
		}
		
		dblRange = CalcLog(dblMaxValue) - CalcLog(dblMinValue);
		dblMinValue = CalcLog(dblMinValue);
	}

	double dblAxisSize = GetAxisSize(TRUE);

	if (IsDiagram3D())
	{
		CBCGPChartData::ComponentIndex ci = GetComponentIndex();
		double dblStart = 0;
		
		switch (ci)
		{
		case CBCGPChartData::CI_X:
		case CBCGPChartData::CI_Y:
			if (m_bReverseOrder)
			{
				dblStart = IsVertical() ? m_ptAxisEnd3D.y : m_ptAxisEnd3D.x;
			}
			else
			{
				dblStart = IsVertical() ? m_ptAxisStart3D.y : m_ptAxisStart3D.x;
			}
			break;
		case CBCGPChartData::CI_Z:
			dblStart = m_bReverseOrder ? m_ptAxisEnd3D.z : m_ptAxisStart3D.z;
			break;
		}

		if (m_bReverseOrder)
			dblPoint = dblStart - ((dblValue - dblMinValue) * dblAxisSize / dblRange);
		else
			dblPoint = dblStart + ((dblValue - dblMinValue) * dblAxisSize / dblRange);
	}
	else
	{
		if (IsVertical())
		{
			dblPoint = m_ptAxisStart.y - ((dblValue - dblMinValue) * dblAxisSize / dblRange);
		}
		else
		{
			dblPoint = m_ptAxisStart.x  + ((dblValue - dblMinValue) * dblAxisSize / dblRange);
		}
	}

	return dblPoint;
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::ScaleValue(double dblValue)
{
	ASSERT_VALID(this);

	double dblMinValue = GetMinDisplayedValue();
	double dblMaxValue = GetMaxDisplayedValue();

	if (dblMinValue == 0 && IsLogScale())
	{
		dblMinValue = 1.;
	}

	double dblRange = dblMaxValue - dblMinValue;

	if (dblRange <= 0)
	{
		return 0;
	}

	if (IsLogScale() && dblValue > 0)
	{
		dblRange = CalcLog(dblMaxValue) - CalcLog(dblMinValue);
		dblMinValue = CalcLog(dblMinValue);
	}

	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	GetAxisPos(ptAxisStart, ptAxisEnd);

	
	return IsVertical() ? ptAxisStart.y - PointFromValue3D(fabs(dblValue) + dblMinValue, TRUE, FALSE).y :
		PointFromValue3D(fabs(dblValue) + dblMinValue, TRUE, FALSE).x - ptAxisStart.x;
}
//----------------------------------------------------------------------------//
double CBCGPChartAxis::GetAxisUnitSize()
{
	ASSERT_VALID(this);

	if (m_nDisplayedLines == 0)
	{
		return 0;
	}

	double nLen = fabs(GetAxisSize(TRUE)); 
	if (m_nDisplayedLines == 1)
	{
		return nLen / m_dblMajorUnit;
	}

	int nDisplayedLines = m_nDisplayedLines;

	if (m_bFormatAsDate)
	{
		return (nLen / (nDisplayedLines)) / m_dblMajorUnit;
	}

	return (nLen / (nDisplayedLines));
}

BOOL CBCGPChartAxis::IsDiagram3D() const
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pChart);

	return m_pChart->IsChart3D();
}

double CBCGPChartAxis::GetAxisSize(BOOL bLogical) const
{
	ASSERT_VALID(this);

	if (IsDiagram3D())
	{
		if (bLogical)
		{
			CBCGPPoint ptDist = m_ptAxisEnd3D - m_ptAxisStart3D;
			CBCGPChartData::ComponentIndex ci = GetComponentIndex();

			switch (ci)
			{
			case CBCGPChartData::CI_X:
			case CBCGPChartData::CI_Y:
				return IsVertical() ? ptDist.y : ptDist.x;
			case CBCGPChartData::CI_Z:
				return ptDist.z;
			}
		}

		return m_dblAxisSize3D;
	}

	return IsVertical() ? m_ptAxisStart.y - m_ptAxisEnd.y : m_ptAxisEnd.x - m_ptAxisStart.x;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::IsPointOnAxis(const CBCGPPoint& pt)
{
	double dblCorrection = 0.01;

	CBCGPPoint ptStart;
	CBCGPPoint ptEnd;

	GetAxisPos(ptStart, ptEnd);

	if (IsVertical() && ptStart.y < ptEnd.y)
	{
		double tmp = ptStart.y;
		ptStart.y = ptEnd.y;
		ptEnd.y = tmp;
	}
	else if (!IsVertical() && ptStart.x > ptEnd.x)
	{
		double tmp = ptStart.x;
		ptStart.x = ptEnd.x;
		ptEnd.x = tmp;
	}

	return (IsVertical() && (pt.y <= ptStart.y + dblCorrection && pt.y >= ptEnd.y - dblCorrection) || 
 			!IsVertical() && (pt.x >= ptStart.x - dblCorrection && pt.x <= ptEnd.x + dblCorrection));
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::IsValueOnAxis3D(double dblVal)
{
	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	GetAxisPos3D(ptAxisStart, ptAxisEnd);

	CBCGPChartData::ComponentIndex ci = GetComponentIndex();

	if (IsVertical())
	{
		if (ptAxisStart.y > ptAxisEnd.y)
		{
			double tmp = ptAxisEnd.y;
			ptAxisEnd.y = ptAxisStart.y;
			ptAxisStart.y = tmp;
		}

		return (dblVal >= ptAxisStart.y && dblVal <= ptAxisEnd.y);
	}

	if (!IsVertical() && ci != CBCGPChartData::CI_Z)
	{
		if (ptAxisStart.x > ptAxisEnd.x)
		{
			double tmp = ptAxisEnd.x;
			ptAxisEnd.x = ptAxisStart.x;
			ptAxisStart.x = tmp;
		}

		return (dblVal >= ptAxisStart.x && dblVal <= ptAxisEnd.x);
	}
	
	if (ptAxisStart.z > ptAxisEnd.z)
	{
		double tmp = ptAxisEnd.z;
		ptAxisEnd.z = ptAxisStart.z;
		ptAxisStart.z = tmp;
	}

	return dblVal >= ptAxisStart.z && dblVal <= ptAxisEnd.z;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawAxisOnThickWall(CBCGPEngine3D* pEngine, CBCGPChartSide3D* pSide)
{
	if (m_pChart == NULL || !IsDiagram3D() || !IsAxisVisible() || pSide == NULL)
	{
		return;
	}

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();
	CBCGPChartShape3DWall* pWall = DYNAMIC_DOWNCAST(CBCGPChartShape3DWall, pSide->GetShape());

	if (pWall == NULL)
	{
		return;
	}

	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	GetAxisPos3D(ptAxisStart, ptAxisEnd);

	OnDrawAxisLine(pEngine->GetDefaultManager());

	if (m_majorTickMarkType == CBCGPChartAxis::TMT_NO_TICKS && m_minorTickMarkType == CBCGPChartAxis::TMT_NO_TICKS)
	{
		return;
	}

	CBCGPPoint ptInterval(m_nFixedIntervalWidth, m_nFixedIntervalWidth);

	if (IsVertical())
	{
		m_bReverseOrder ? ptInterval.y = ptAxisStart.y + ptInterval.y : ptInterval.y = m_ptAxisStart.y - ptInterval.y;
	}
	else
	{
		m_bReverseOrder ? ptInterval.x = ptAxisStart.x - ptInterval.x : ptInterval.x += ptAxisStart.x;
	}

	double dblUnitsInInterval = m_bIsFixedIntervalWidth ? (int)(ValueFromPoint(ptInterval) - GetMinDisplayedValue()) : m_dblMajorUnit;

	if (dblUnitsInInterval <= 0)
	{
		dblUnitsInInterval = 1.;
	}

	double dblMinorUnit = dblUnitsInInterval / m_nMinorUnitCount;
	double dblCurrValue = GetMinDisplayedValue();

	if (m_bUseApproximation && !IsLogScale())
	{
		dblCurrValue = ApproximateValue(dblCurrValue, m_dblMajorUnit, 1.) - m_dblMajorUnit;
	}

	double dblMinorTickMarkLen = GetMinorTickMarkLen(TRUE);

	CBCGPPoint ptScale(dblMinorTickMarkLen, 0);
	ptScale = pDiagram3D->TranslateDistance(ptScale);
	dblMinorTickMarkLen = ptScale.x;

	double dblUnitStep = CalcLog(GetMinDisplayedValue());

	int nWallPos = pWall->GetWallPosition();
	int nSideIndex = pSide->GetSideIndex();
	BOOL bIsFrontOrBack = nSideIndex == CBCGPChartShape3DCube::CUBE_SIDE_FRONT || 
							nSideIndex == CBCGPChartShape3DCube::CUBE_SIDE_BACK;

	double dblMaxValue = GetMaxDisplayedValue();

	while (dblCurrValue <= dblMaxValue)
	{
		double dblVal = PointFromValue(dblCurrValue, TRUE, FALSE);
		CBCGPPoint ptStartTick = GetPointOnAxisPlane3D(dblVal, TRUE);
		CBCGPPoint ptEndTick = ptStartTick;

		BOOL bDrawMajorTicks = IsValueOnAxis3D(dblVal);

		if (IsVertical())
		{
			if (bIsFrontOrBack)
			{
				ptEndTick.x = ptAxisStart.x;
			}
			else
			{
				ptEndTick.z = ptAxisStart.z;
			}
		}
		else
		{
			ptEndTick.y = ptAxisStart.y;
		}

		ptStartTick = pDiagram3D->TransformPoint(ptStartTick);
		ptEndTick = pDiagram3D->TransformPoint(ptEndTick);

		if (m_majorTickMarkType != TMT_NO_TICKS && bDrawMajorTicks)
		{
			m_pChart->OnDrawTickMark(pEngine->GetDefaultManager(), ptStartTick, ptEndTick, m_axisLineFormat, dblCurrValue, TRUE);
		}

		if ((m_minorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS) && 
			dblCurrValue < dblMaxValue && 
			dblCurrValue >= GetMinDisplayedValue())
		{
			double dblMinorValue = IsComponentXSet() ? dblCurrValue : dblCurrValue + dblMinorUnit;
			int nMaxCount = IsComponentXSet() ? m_nMinorUnitCount : m_nMinorUnitCount - 1;
			double dblLogStep = (pow(m_dblMajorUnit, dblUnitStep + 1) - dblCurrValue) / m_nMinorUnitCount;

			if (dblMinorValue > GetMaxDisplayedValue() + GetDoubleCorrection())
			{
				break;
			}

			for (int i = 0; i < nMaxCount; i++)
			{
				double dblVal = PointFromValue(dblMinorValue, TRUE, FALSE);
				CBCGPPoint ptStartTick = GetPointOnAxisPlane3D(dblVal, TRUE);
				CBCGPPoint ptEndTick = ptStartTick;

				if (!IsValueOnAxis3D(dblVal))
				{
					if (IsLogScale())
					{
						dblMinorValue += dblLogStep;
					}
					else
					{
						dblMinorValue += dblMinorUnit;
					}
					continue;
				}

				if (IsVertical())
				{
					if (bIsFrontOrBack)
					{
						ptEndTick.x = ptAxisStart.x;
						ptStartTick.x -= dblMinorTickMarkLen;
					}
					else
					{
						if (nSideIndex == CBCGPChartShape3DCube::CUBE_SIDE_LEFT && 
							nWallPos == CBCGPChartShape3DWall::WALL_POS_FRONT)
						{
							ptStartTick.z -= dblMinorTickMarkLen;
						}
						else
						{
							ptStartTick.z += dblMinorTickMarkLen;
						}
						ptEndTick.z = ptAxisStart.z;
					}
				}
				else
				{
					ptStartTick.y = ptAxisStart.y + dblMinorTickMarkLen;
					ptEndTick.y = ptAxisStart.y;
				}

				ptStartTick = pDiagram3D->TransformPoint(ptStartTick);
				ptEndTick = pDiagram3D->TransformPoint(ptEndTick);

				m_pChart->OnDrawTickMark(pEngine->GetDefaultManager(), ptStartTick, ptEndTick, m_axisLineFormat, dblMinorValue, FALSE);
				
				if (IsLogScale())
				{
					dblMinorValue += dblLogStep;
				}
				else
				{
					dblMinorValue += dblMinorUnit;
				}
			}
		}

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblUnitsInInterval;
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawInterlaceOnThickWall(CBCGPEngine3D* pEngine, CBCGPChartSide3D* /*pSide*/, BOOL bIsNearWall)
{
	if (pEngine == NULL)
	{
		return;
	}

	BOOL bSavedNear = m_bEnableInterlaceNearWall;
	BOOL bSavedFar = m_bEnableInterlaceFarWall;

	m_bEnableInterlaceNearWall = bIsNearWall && bSavedNear;
	m_bEnableInterlaceFarWall = !bIsNearWall && bSavedFar;

	if (m_bEnableInterlaceNearWall || m_bEnableInterlaceFarWall)
	{
		OnFillUnitIntervals(pEngine->GetDefaultManager(), CBCGPRect());
	}

	m_bEnableInterlaceNearWall = bSavedNear;
	m_bEnableInterlaceFarWall = bSavedFar;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawGridLinesOnThickWall(CBCGPEngine3D* pEngine, CBCGPChartSide3D* /*pSide*/, BOOL bIsNearWall)
{
	if (pEngine == NULL)
	{
		return;
	}

	BOOL bSavedNear = m_bEnableGridLineNearWall;
	BOOL bSavedFar = m_bEnableGridLineFarWall;

	m_bEnableGridLineNearWall = bIsNearWall && bSavedNear;
	m_bEnableGridLineFarWall = !bIsNearWall && bSavedFar;

	if (m_bEnableGridLineNearWall || m_bEnableGridLineFarWall)
	{
		OnDrawMajorGridLines(pEngine->GetDefaultManager(), CBCGPRect());
		OnDrawMinorGridLines(pEngine->GetDefaultManager(), CBCGPRect());
	}

	m_bEnableGridLineNearWall = bSavedNear;
	m_bEnableGridLineFarWall = bSavedFar;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);

	BOOL bIsAxisVisible = IsAxisVisible();

	if (m_pChart == NULL || !bIsAxisVisible || m_dblMajorUnit == 0.)
	{
		return;
	}

	if (m_nDisplayedLines == 0)
	{
		OnDrawScrollBar(pGM, m_rectScrollBar);
		OnDrawAxisLine(pGM);
	}

	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	GetAxisPos(ptAxisStart, ptAxisEnd);

	CBCGPPoint ptPrevGridLineStart = ptAxisStart;

	CBCGPChartAxis* pPerpAxis = GetPerpendecularAxis();
	
	CBCGPPoint ptInterval(m_nFixedIntervalWidth, m_nFixedIntervalWidth);

	if (IsVertical())
	{
		m_bReverseOrder ? ptInterval.y = ptAxisStart.y + ptInterval.y : ptInterval.y = m_ptAxisStart.y - ptInterval.y;
	}
	else
	{
		m_bReverseOrder ? ptInterval.x = ptAxisStart.x - ptInterval.x : ptInterval.x += ptAxisStart.x;
	}

	double dblUnitsInInterval = m_bIsFixedIntervalWidth ? (int)(ValueFromPoint(ptInterval) - GetMinDisplayedValue()) : m_dblMajorUnit;

	if (dblUnitsInInterval <= 0)
	{
		dblUnitsInInterval = 1.;
	}

	double dblScrollBarSize = CanShowScrollBar() ? GetScrollBarSize(TRUE) - 1 : 0;

	double dblMinorUnit = dblUnitsInInterval / m_nMinorUnitCount;
	double dblCurrValue = GetMinDisplayedValue();

	if (m_bUseApproximation && !IsLogScale())
	{
		dblCurrValue = ApproximateValue(dblCurrValue, m_dblMajorUnit, 1.) - m_dblMajorUnit;
	}

	CBCGPChartAxis::TickMarkType tmtMajor = m_majorTickMarkType;
	CBCGPChartAxis::TickMarkType tmtMinor = m_minorTickMarkType;

	BOOL bReverseTicks = pPerpAxis->m_bReverseOrder && !IsDiagram3D();

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

	if (IsDiagram3D() && IsVertical() && pDiagram3D != NULL)
	{
		double dblXRotation = pDiagram3D->GetXRotation();

		if (dblXRotation >= 90 && dblXRotation < 270)
		{
			bReverseTicks = TRUE;
		}
	}

	if (bReverseTicks)
	{
		if (tmtMajor == CBCGPChartAxis::TMT_INSIDE)
		{
			tmtMajor = CBCGPChartAxis::TMT_OUTSIDE;
		}
		else if (tmtMajor == CBCGPChartAxis::TMT_OUTSIDE)
		{
			tmtMajor = CBCGPChartAxis::TMT_INSIDE;
		}

		if (tmtMinor == CBCGPChartAxis::TMT_INSIDE)
		{
			tmtMinor = CBCGPChartAxis::TMT_OUTSIDE;
		}
		else if (tmtMinor == CBCGPChartAxis::TMT_OUTSIDE)
		{
			tmtMinor = CBCGPChartAxis::TMT_INSIDE;
		}
	}

	double dblMajorTickMarkLen = GetMajorTickMarkLen(TRUE);
	double dblMinorTickMarkLen = GetMinorTickMarkLen(TRUE);

	double dblUnitStep = CalcLog(GetMinDisplayedValue());
	double dblMaxValue = GetMaxDisplayedValue() + GetDoubleCorrection();

	while (dblCurrValue <= dblMaxValue)
	{
		ptAxisStart = PointFromValue3D(dblCurrValue, TRUE, FALSE);
		
		BOOL bDrawTick = TRUE;

		if (!IsPointOnAxis(ptAxisStart))
		{
			bDrawTick = FALSE;
		}
		
		CBCGPPoint ptStartTick = ptAxisStart;
		CBCGPPoint ptEndTick = ptAxisStart; 

		if (m_majorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS && bDrawTick)
		{
			if (!m_rectScrollBar.IsRectEmpty())
			{
				if (IsVertical())
				{
					ptStartTick.x += (m_bTickMarksTopRight ? dblScrollBarSize : -dblScrollBarSize);
					ptEndTick.x += (m_bTickMarksTopRight ? dblScrollBarSize : -dblScrollBarSize);
				}
				else
				{
					ptStartTick.y += (m_bTickMarksTopRight ? -dblScrollBarSize : dblScrollBarSize);
					ptEndTick.y += (m_bTickMarksTopRight ? -dblScrollBarSize : dblScrollBarSize);
				}
			}
			switch (tmtMajor)
			{
			case CBCGPChartAxis::TMT_INSIDE:
				if (IsVertical())
				{
					m_bIsSecondaryAxis ? ptStartTick.x -=  dblMajorTickMarkLen: 
						ptStartTick.x += dblMajorTickMarkLen;
				}
				else
				{
					m_bIsSecondaryAxis ? ptStartTick.y += dblMajorTickMarkLen :
						ptStartTick.y -= dblMajorTickMarkLen;					
				}
				break;
			case CBCGPChartAxis::TMT_OUTSIDE:
				if (IsVertical())
				{
					m_bIsSecondaryAxis ? ptStartTick.x += dblMajorTickMarkLen :
							ptStartTick.x -= dblMajorTickMarkLen;
				}
				else
				{
					m_bIsSecondaryAxis ? ptStartTick.y -= dblMajorTickMarkLen: 
						ptStartTick.y += dblMajorTickMarkLen;
				}
				break;
			case CBCGPChartAxis::TMT_CROSS:
				if (IsVertical())
				{
					ptStartTick.x -= dblMajorTickMarkLen;
					ptEndTick.x += dblMajorTickMarkLen;
				}
				else
				{
					ptStartTick.y -= dblMajorTickMarkLen;
					ptEndTick.y += dblMajorTickMarkLen;
				}
				break;
			}

			m_pChart->OnDrawTickMark(pGM, ptStartTick, ptEndTick, m_axisLineFormat, dblCurrValue, TRUE);
		}

		if ((m_minorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS) && dblCurrValue < GetMaxDisplayedValue() + GetDoubleCorrection())
		{
			double dblMinorValue = IsComponentXSet() ? dblCurrValue : dblCurrValue + dblMinorUnit;
			int nMaxCount = IsComponentXSet() ? m_nMinorUnitCount : m_nMinorUnitCount - 1;
			double dblLogStep = (pow(m_dblMajorUnit, dblUnitStep + 1) - dblCurrValue) / m_nMinorUnitCount;

			for (int i = 0; i < nMaxCount; i++)
			{
				ptAxisStart= PointFromValue3D(dblMinorValue, TRUE, FALSE);

				bDrawTick = IsPointOnAxis(ptAxisStart);

				CBCGPPoint ptStartTick = ptAxisStart;
				CBCGPPoint ptEndTick = ptAxisStart; 

				if (IsVertical())
				{
					ptStartTick.x += (m_bTickMarksTopRight ? dblScrollBarSize : -dblScrollBarSize);
					ptEndTick.x += (m_bTickMarksTopRight ? dblScrollBarSize : -dblScrollBarSize);
				}
				else
				{
					ptStartTick.y += (m_bTickMarksTopRight ? -dblScrollBarSize : dblScrollBarSize);
					ptEndTick.y += (m_bTickMarksTopRight ? -dblScrollBarSize : dblScrollBarSize);
				}

				switch (tmtMinor)
				{
				case CBCGPChartAxis::TMT_INSIDE:
					if (IsVertical())
					{
						m_bIsSecondaryAxis ? ptStartTick.x -= dblMinorTickMarkLen :
							ptStartTick.x += dblMinorTickMarkLen;
					}
					else
					{
						m_bIsSecondaryAxis ? ptStartTick.y += dblMinorTickMarkLen :
							ptStartTick.y -= dblMinorTickMarkLen;
					}
					break;
				case CBCGPChartAxis::TMT_OUTSIDE:
					if (IsVertical())
					{
						m_bIsSecondaryAxis ? ptStartTick.x += dblMinorTickMarkLen:
							ptStartTick.x -= dblMinorTickMarkLen;
					}
					else
					{
						m_bIsSecondaryAxis ? ptStartTick.y -= dblMinorTickMarkLen :
							ptStartTick.y += dblMinorTickMarkLen;
					}
					break;
				case CBCGPChartAxis::TMT_CROSS:
					if (IsVertical())
					{
						ptStartTick.x -= dblMinorTickMarkLen;
						ptEndTick.x += dblMinorTickMarkLen;
					}
					else
					{
						ptStartTick.y -= dblMinorTickMarkLen;
						ptEndTick.y += dblMinorTickMarkLen;
					}
					break;
				}

				if (bDrawTick)
				{
					m_pChart->OnDrawTickMark(pGM, ptStartTick, ptEndTick, m_axisLineFormat, dblMinorValue, FALSE);
				}

				if (IsLogScale())
				{
					dblMinorValue += dblLogStep;
				}
				else
				{
					dblMinorValue += dblMinorUnit;
				}
			}
		}

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblUnitsInInterval;
		}
	}

	if (bIsAxisVisible)
	{
		OnDrawAxisLine(pGM);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawScrollBar(CBCGPGraphicsManager* pGM)
{
	ASSERT_VALID(this);

	if (IsAxisVisible())
	{
		OnDrawScrollBar(pGM, m_rectScrollBar);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawAxisLine(CBCGPGraphicsManager* /*pGM*/)
{
	ASSERT_VALID(this);

	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	GetAxisPos(ptAxisStart, ptAxisEnd);

	const CBCGPBrush& brLineColor = m_axisLineFormat.m_brLineColor.IsEmpty() ? 
		m_pChart->GetColors().m_brAxisLineColor : m_axisLineFormat.m_brLineColor;

	
	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();
	if (pDiagram3D != NULL)
	{
		CBCGPEngine3D* pEngine3D = pDiagram3D->GetEngine3D();

		if (pEngine3D != NULL)
		{
			ASSERT_VALID(pEngine3D);

			pEngine3D->DrawLine(ptAxisStart, ptAxisEnd, brLineColor, m_axisLineFormat.GetLineWidth(TRUE), 
								&m_axisLineFormat.m_strokeStyle);
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawMajorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	BOOL bIsAxisVisible = IsAxisVisible();

	if (m_nDisplayedLines == 0 || m_pChart == NULL || !bIsAxisVisible || !m_bShowMajorGridLines)
	{
		return;
	}

	CBCGPPoint ptStart = m_ptAxisStart;
	CBCGPPoint ptEnd = m_ptAxisEnd;
	CBCGPPoint ptPrevGridLineStart = ptStart;

	CBCGPChartAxis* pPerpAxis = GetPerpendecularAxis();
	CBCGPPoint ptGridStart;
	CBCGPPoint ptGridEnd;

	if (pPerpAxis == NULL)
	{
		return;
	}

	ASSERT_VALID(pPerpAxis);

	CArray<CBCGPRect, CBCGPRect> arBoundingRects;
	GetBoundingRects(arBoundingRects);

	if (m_bUsePerpAxisForZoning)
	{
		pPerpAxis->GetAxisPos(ptGridStart, ptGridEnd);
	}
	else
	{
		IsVertical() ? ptGridStart.SetPoint(rectDiagram.left, m_ptAxisStart.y) : ptGridStart.SetPoint(m_ptAxisStart.x, rectDiagram.bottom);
		IsVertical() ? ptGridEnd.SetPoint(rectDiagram.right, m_ptAxisEnd.y) : ptGridEnd.SetPoint(m_ptAxisEnd.x, rectDiagram.top);
	}
	
	m_arMajorGridLines.RemoveAll();

	CBCGPPoint ptInterval(m_nFixedIntervalWidth, m_nFixedIntervalWidth);

	if (IsVertical())
	{
		m_bReverseOrder ? ptInterval.y = m_ptAxisStart.y + ptInterval.y : ptInterval.y = m_ptAxisStart.y - ptInterval.y;
	}
	else
	{
		m_bReverseOrder ? ptInterval.x = m_ptAxisStart.x - ptInterval.x : ptInterval.x += m_ptAxisStart.x;
	}

	double dblUnitsInInterval = m_bIsFixedIntervalWidth ? (int)(ValueFromPoint(ptInterval) - GetMinDisplayedValue()) : m_dblMajorUnit;

	if (dblUnitsInInterval <= 0)
	{
		dblUnitsInInterval = 1.;
	}

	double dblCurrValue = GetMinDisplayedValue();

	if (m_bUseApproximation && !IsLogScale())
	{
		dblCurrValue = ApproximateValue(dblCurrValue, m_dblMajorUnit, 1.) - m_dblMajorUnit;
	}

	double dblUnitStep = CalcLog(GetMinDisplayedValue());

	CBCGPPointsArray arAxesPos;

	CBCGPEngine3D* pEngine3D = m_pChart->GetEngine3D();

	BOOL bIsSoftRendering = pEngine3D != NULL && pEngine3D->IsSoftwareRendering();
	BOOL bIsDiagram3D = IsDiagram3D();
	BOOL bIsThickFloor = m_pChart->GetDiagram3D() == NULL || !IsDiagram3D() ? FALSE : m_pChart->GetDiagram3D()->IsThickWallsAndFloor();

	if (bIsDiagram3D && !bIsSoftRendering)
	{
		arAxesPos.SetSize(6);
		if (m_pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS)->IsAxisVisible())
			m_pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS)->GetAxisPos(arAxesPos[0], arAxesPos[1]);
		if (m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS)->IsAxisVisible())
			m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS)->GetAxisPos(arAxesPos[2], arAxesPos[3]);
		if (m_pChart->GetChartAxis(BCGP_CHART_Z_PRIMARY_AXIS)->IsAxisVisible())
			m_pChart->GetChartAxis(BCGP_CHART_Z_PRIMARY_AXIS)->GetAxisPos(arAxesPos[4], arAxesPos[5]);
	}

	double dblMaxValue = bIsThickFloor ? GetMaxDisplayedValue() : GetMaxDisplayedValue() + GetDoubleCorrection();

	while (dblCurrValue <= dblMaxValue)
	{
		ptStart = PointFromValue3D(dblCurrValue, TRUE, TRUE);
		BOOL bIsPointOnAxis = IsPointOnAxis(ptStart);

		if (bIsThickFloor && bIsDiagram3D)
		{
			double dblVal = PointFromValue(dblCurrValue, TRUE, FALSE);
			bIsPointOnAxis = IsValueOnAxis3D(dblVal);
		}

		if (!bIsPointOnAxis || 
			bIsDiagram3D && (dblCurrValue == GetMinDisplayedValue() || dblCurrValue == GetMaxDisplayedValue()))
		{
			if (IsLogScale())
			{
				dblUnitStep += 1.;
				dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
			}
			else
			{
				dblCurrValue += dblUnitsInInterval;
			}
			continue;
		}
		
		if ((IsVertical() && ptStart.y < m_ptAxisEnd.y || !IsVertical() && ptStart.x > m_ptAxisEnd.x) && 
			dblCurrValue > GetMaxDisplayedValue() + GetDoubleCorrection())
		{
			break;
		}

		if (bIsDiagram3D)
		{
			CBCGPPoint ptEndFloor;
			CBCGPPoint ptEndWall;

			OnCalcFloorAndWallGridLinePoints(dblCurrValue, ptEndFloor, ptEndWall);

			BOOL bDrawLine1 = TRUE && m_bEnableGridLineNearWall;
			BOOL bDrawLine2 = pPerpAxis != NULL && pPerpAxis->IsAxisVisible() && m_bEnableGridLineFarWall;

			if (!bIsSoftRendering)
			{
				static double dblPrecision = 4 * FLT_EPSILON;
				for (int i = 0; i < 6; i += 2)
				{
					if (arAxesPos[i].x != 0 && arAxesPos[i + 1].x != 0)
					{
						if (bDrawLine1)
						{
							if (bcg_pointInLine(arAxesPos[i], arAxesPos[i + 1], ptStart, dblPrecision) != 0 && 
								bcg_pointInLine(arAxesPos[i], arAxesPos[i + 1], ptEndFloor, dblPrecision) != 0)
							{
								bDrawLine1 = FALSE;
							}
						}

						if (bDrawLine2)
						{
							if (bcg_pointInLine(arAxesPos[i], arAxesPos[i + 1], ptEndFloor, dblPrecision) != 0 && 
								bcg_pointInLine(arAxesPos[i], arAxesPos[i + 1], ptEndWall, dblPrecision) != 0)
							{
								bDrawLine2 = FALSE;
							}
						}
					}
				}
			}
			
			if (bDrawLine1)
			{
				m_pChart->OnDrawGridLine(pGM, ptStart, ptEndFloor, this, dblCurrValue, 
														m_majorGridLineFormat, TRUE);
			}

			if (bDrawLine2)
			{
				m_pChart->OnDrawGridLine(pGM, ptEndFloor, ptEndWall, this, dblCurrValue, 
					m_majorGridLineFormat, TRUE);
			}
		}
		else
		{
			CBCGPPoint ptMajorGridLineStart(0, 0);
			CBCGPPoint ptMajorGridLineEnd(0, 0);
			
			if (IsVertical())
			{
				ptMajorGridLineStart.SetPoint(ptGridStart.x, ptStart.y);
				ptMajorGridLineEnd.SetPoint(ptGridEnd.x, ptStart.y);
			}
			else
			{
				ptMajorGridLineStart.SetPoint(ptStart.x, ptGridStart.y);
				ptMajorGridLineEnd.SetPoint(ptStart.x, ptGridEnd.y);
			}

			for (int j = 0; j < arBoundingRects.GetSize(); j++)
			{
				CBCGPRect rectBounds = arBoundingRects[j];

				CBCGPPoint ptStartLine = ptMajorGridLineStart;
				CBCGPPoint ptEndLine = ptMajorGridLineEnd;
				
				if (IsVertical())
				{
					ptStartLine.x = rectBounds.left;
					ptEndLine.x = rectBounds.right;
				}
				else
				{
					ptStartLine.y = rectBounds.top;
					ptEndLine.y = rectBounds.bottom;
				}

				m_pChart->OnDrawGridLine(pGM, ptStartLine, ptEndLine, this, dblCurrValue, 
												m_majorGridLineFormat, TRUE);
			}

			CBCGPRect rectMajorGL(ptMajorGridLineStart, ptMajorGridLineEnd);
			IsVertical() ? rectMajorGL.InflateRect(0, 4) : rectMajorGL.InflateRect(4, 0);

			m_arMajorGridLines.Add(rectMajorGL);
		}

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblUnitsInInterval;
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawMinorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	BOOL bIsAxisVisible = IsAxisVisible();

	if (m_nDisplayedLines == 0 || m_pChart == NULL || !bIsAxisVisible || !m_bShowMinorGridLines)
	{
		return;
	}

	CBCGPPoint ptStart = m_ptAxisStart;
	CBCGPPoint ptEnd = m_ptAxisEnd;
	CBCGPPoint ptPrevGridLineStart = ptStart;

	CBCGPChartAxis* pPerpAxis = GetPerpendecularAxis();
	CBCGPPoint ptGridStart;
	CBCGPPoint ptGridEnd;

	if (m_bUsePerpAxisForZoning)
	{
		pPerpAxis->GetAxisPos(ptGridStart, ptGridEnd);
	}
	else
	{
		IsVertical() ? ptGridStart.SetPoint(rectDiagram.left, m_ptAxisStart.y) : ptGridStart.SetPoint(m_ptAxisStart.x, rectDiagram.bottom);
		IsVertical() ? ptGridEnd.SetPoint(rectDiagram.right, m_ptAxisEnd.y) : ptGridEnd.SetPoint(m_ptAxisEnd.x, rectDiagram.top);
	}
	
	m_arMinorGridLines.RemoveAll();

	CBCGPPoint ptInterval(m_nFixedIntervalWidth, m_nFixedIntervalWidth);

	if (IsVertical())
	{
		m_bReverseOrder ? ptInterval.y = m_ptAxisStart.y + ptInterval.y : ptInterval.y = m_ptAxisStart.y - ptInterval.y;
	}
	else
	{
		m_bReverseOrder ? ptInterval.x = m_ptAxisStart.x - ptInterval.x : ptInterval.x += m_ptAxisStart.x;
	}

	double dblUnitsInInterval = m_bIsFixedIntervalWidth ? (int)(ValueFromPoint(ptInterval) - GetMinDisplayedValue()) : m_dblMajorUnit;

	if (dblUnitsInInterval <= 0)
	{
		dblUnitsInInterval = 1.;
	}

	double dblMinorUnit = dblUnitsInInterval / m_nMinorUnitCount;
	double dblCurrValue = GetMinDisplayedValue();

	if (m_bUseApproximation && !IsLogScale())
	{
		dblCurrValue = ApproximateValue(dblCurrValue, m_dblMajorUnit, 1.) - m_dblMajorUnit;
	}

	double dblUnitStep = CalcLog(GetMinDisplayedValue());

	BOOL bIsDiagram3D = IsDiagram3D();
	BOOL bIsThickFloor = m_pChart->GetDiagram3D() == NULL || !bIsDiagram3D ? FALSE : 
																m_pChart->GetDiagram3D()->IsThickWallsAndFloor();

	double dblMaxValue = bIsThickFloor ? GetMaxDisplayedValue() : GetMaxDisplayedValue() + GetDoubleCorrection();

	while (dblCurrValue <= dblMaxValue)
	{
		ptStart = PointFromValue3D(dblCurrValue, TRUE, TRUE);
		BOOL bIsPointOnAxis = IsPointOnAxis(ptStart);

		if (bIsThickFloor && bIsDiagram3D)
		{
			double dblVal = PointFromValue(dblCurrValue, TRUE, FALSE);
			bIsPointOnAxis = IsValueOnAxis3D(dblVal);
		}

		if (!bIsPointOnAxis || 
			bIsDiagram3D && (dblCurrValue == GetMinDisplayedValue() || dblCurrValue == GetMaxDisplayedValue()))
		{
			if (IsLogScale())
			{
				dblUnitStep += 1.;
				dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
			}
			else
			{
				dblCurrValue += dblMinorUnit;
			}
			continue;
		}
		
		if ((IsVertical() && ptStart.y < m_ptAxisEnd.y || !IsVertical() && ptStart.x > m_ptAxisEnd.x) && 
			dblCurrValue > GetMaxDisplayedValue() + GetDoubleCorrection())
		{
			break;
		}

		double dblMinorValue = dblCurrValue;
		double dblLogStep = (pow(m_dblMajorUnit, dblUnitStep + 1) - dblCurrValue) / m_nMinorUnitCount;
		int nMaxCount = IsLogScale() ? m_nMinorUnitCount : 1;

		for (int i = 0; i < nMaxCount; i++)
		{
			if (i != 0)
			{
				ptStart = PointFromValue3D(dblMinorValue, TRUE, TRUE);
				
				if ((IsVertical() && ptStart.y < m_ptAxisEnd.y || !IsVertical() && ptStart.x > m_ptAxisEnd.x) && 
					dblMinorValue > dblMaxValue || !bIsPointOnAxis)
				{
					break;
				}
			}
			
			if (bIsDiagram3D)
			{
				CBCGPPoint ptEndFloor;
				CBCGPPoint ptEndWall;

				OnCalcFloorAndWallGridLinePoints(dblMinorValue, ptEndFloor, ptEndWall);

				if (m_bEnableGridLineNearWall)
				{
					m_pChart->OnDrawGridLine(pGM, ptStart, ptEndFloor, this, dblCurrValue, 
						m_minorGridLineFormat, TRUE);
				}
				

				if (pPerpAxis != NULL && pPerpAxis->IsAxisVisible() && m_bEnableGridLineFarWall)
				{
					m_pChart->OnDrawGridLine(pGM, ptEndFloor, ptEndWall, this, dblCurrValue, 
														m_minorGridLineFormat, TRUE);
				}
			}
			else
			{
				CBCGPPoint ptGridLineStart(0, 0);
				CBCGPPoint ptGridLineEnd(0, 0);

				if (IsVertical())
				{
					ptGridLineStart.SetPoint(ptGridStart.x, ptStart.y);
					ptGridLineEnd.SetPoint(ptGridEnd.x, ptStart.y);
				}
				else
				{
					ptGridLineStart.SetPoint(ptStart.x, ptGridStart.y);
					ptGridLineEnd.SetPoint(ptStart.x, ptGridEnd.y);
				}

				m_pChart->OnDrawGridLine(pGM, ptGridLineStart, ptGridLineEnd, this, dblCurrValue, 
					m_minorGridLineFormat, FALSE);

				CBCGPRect rectMinorGL(ptGridLineStart, ptGridLineEnd);
				if (IsVertical())
				{
					rectMinorGL.InflateRect(0, 4);
				}
				else
				{
					rectMinorGL.InflateRect(4, 0);
				}

				m_arMinorGridLines.Add(rectMinorGL);
			}

			dblMinorValue += dblLogStep;
		}

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblMinorUnit;			
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnFillUnitIntervals(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	if (m_nDisplayedLines == 0 || m_pChart == NULL || !IsAxisVisible() || !m_bFillMajorUnitInterval)
	{
		return;
	}

	double dblCurrValue = GetMinDisplayedValue();
	int nCount = -1;
	int nStep = m_nUnitFillStep - 1;

	CBCGPPoint	ptStart(0 , 0);
	CBCGPPoint	ptPrevGridLineStart(0, 0);
	CBCGPPoint	ptGridLineStart(0, 0);
	CBCGPPoint	ptGridLineEnd(0, 0);

	CBCGPChartAxis* pPerpAxis = GetPerpendecularAxis();
	CBCGPPoint ptGridStart;
	CBCGPPoint ptGridEnd;

	CArray<CBCGPRect, CBCGPRect> arBoundingRects;
	GetBoundingRects(arBoundingRects);

	if (m_bUsePerpAxisForZoning)
	{
		pPerpAxis->GetAxisPos(ptGridStart, ptGridEnd);
	}
	else
	{
		IsVertical() ? ptGridStart.SetPoint(rectDiagram.left, m_ptAxisStart.y) : ptGridStart.SetPoint(m_ptAxisStart.x, rectDiagram.bottom);
		IsVertical() ? ptGridEnd.SetPoint(rectDiagram.right, m_ptAxisEnd.y) : ptGridEnd.SetPoint(m_ptAxisEnd.x, rectDiagram.top);
	}

	CBCGPPoint ptInterval(m_nFixedIntervalWidth, m_nFixedIntervalWidth);

	if (IsVertical())
	{
		m_bReverseOrder ? ptInterval.y = m_ptAxisStart.y + ptInterval.y : ptInterval.y = m_ptAxisStart.y - ptInterval.y;
	}
	else
	{
		m_bReverseOrder ? ptInterval.x = m_ptAxisStart.x - ptInterval.x : ptInterval.x += m_ptAxisStart.x;
	}

	double dblUnitsInInterval = m_bIsFixedIntervalWidth ? (int) (ValueFromPoint(ptInterval) - GetMinDisplayedValue()) : m_dblMajorUnit;

	if (dblUnitsInInterval <= 0)
	{
		dblUnitsInInterval = 1.;
	}

	if (m_bUseApproximation && !IsLogScale())
	{
		dblCurrValue = ApproximateValue(dblCurrValue, m_dblMajorUnit, 1.) - m_dblMajorUnit;
	}

	double dblUnitStep = CalcLog(GetMinDisplayedValue());

	CBCGPPoint ptFloor3D;
	CBCGPPoint ptWall3D;

	CBCGPPoint ptPrevStart3D;
	CBCGPPoint ptPrevFloor3D;
	CBCGPPoint ptPrevWall3D;

	const CBCGPBrush& br3DChart = pGM->IsSupported(BCGP_GRAPHICS_MANAGER_COLOR_OPACITY) ?
		m_pChart->GetColors().m_brAxisInterlaceColor3D :
		m_pChart->GetColors().m_brAxisInterlaceColor3DGDI;

	const CBCGPBrush& brChart = pGM->IsSupported(BCGP_GRAPHICS_MANAGER_COLOR_OPACITY) ?
		m_pChart->GetColors().m_brAxisInterlaceColor :
		m_pChart->GetColors().m_brAxisInterlaceColorGDI;

	const CBCGPBrush& br3D = (m_brInterval.IsEmpty() ? br3DChart : m_brInterval3D); 
	const CBCGPBrush& br = (m_brInterval.IsEmpty() ? brChart : m_brInterval);

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();
	BOOL bIsDiagram3D = IsDiagram3D();
	BOOL bIsThickFloor = m_pChart->GetDiagram3D() == NULL ? FALSE : m_pChart->GetDiagram3D()->IsThickWallsAndFloor();

	while (dblCurrValue <= GetMaxDisplayedValue() + GetDoubleCorrection())
	{
		ptStart = PointFromValue3D(dblCurrValue, TRUE, TRUE);
		BOOL bIsPointOnAxis = IsPointOnAxis(ptStart);

		if (bIsThickFloor && bIsDiagram3D)
		{
			double dblVal = PointFromValue(dblCurrValue, TRUE, FALSE);
			bIsPointOnAxis = IsValueOnAxis3D(dblVal);
		}

		if (!bIsPointOnAxis)
		{
			if (IsLogScale())
			{
				dblUnitStep += 1.;
				dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
			}
			else
			{
				dblCurrValue += dblUnitsInInterval;
			}
			continue;
		}
		
		if ((IsVertical() && ptStart.y < m_ptAxisEnd.y || !IsVertical() && ptStart.x > m_ptAxisEnd.x) && 
			dblCurrValue > GetMaxDisplayedValue() + GetDoubleCorrection())
		{
			break;
		}

		if (IsDiagram3D())
		{
			OnCalcFloorAndWallGridLinePoints(dblCurrValue, ptFloor3D, ptWall3D);

			if (ptPrevStart3D.x != 0 && nCount >= m_nFirstIntervalToFill)
			{
				if (nStep == (m_nUnitFillStep - 1))
				{
					CBCGPVector4 vNormal;

					if (m_bEnableInterlaceNearWall)
					{
						CBCGPPointsArray arPoints1;
						arPoints1.Add(ptPrevStart3D);
						arPoints1.Add(ptPrevFloor3D);
						arPoints1.Add(ptFloor3D);
						arPoints1.Add(ptStart);

						if (pDiagram3D->IsCalculateNormals())
						{
							vNormal.CalcNormal(ptPrevStart3D, ptPrevFloor3D, ptFloor3D);
							pDiagram3D->GetEngine3D()->SetPolygonNormal(vNormal[0], vNormal[1], vNormal[2]);
						}

						CBCGPPolygonGeometry g1(arPoints1);
						m_pChart->OnFillAxisUnitInterval(pGM, g1, br3D);
					}

					if (pPerpAxis != NULL && pPerpAxis->IsAxisVisible() && m_bEnableInterlaceFarWall)
					{
						CBCGPPointsArray arPoints2;
						arPoints2.Add(ptPrevFloor3D);
						arPoints2.Add(ptPrevWall3D);
						arPoints2.Add(ptWall3D);
						arPoints2.Add(ptFloor3D);

						if (pDiagram3D->IsCalculateNormals())
						{
							vNormal.CalcNormal(ptPrevFloor3D, ptPrevWall3D, ptWall3D);
							pDiagram3D->GetEngine3D()->SetPolygonNormal(vNormal[0], vNormal[1], vNormal[2]);
						}

						CBCGPPolygonGeometry g2(arPoints2);
						m_pChart->OnFillAxisUnitInterval(pGM, g2, br);
					}

					nStep = 0;
				}
				else
				{
					nStep++;
				}
			}
	
			ptPrevStart3D = ptStart;
			ptPrevFloor3D = ptFloor3D;
			ptPrevWall3D = ptWall3D;
		}
		else
		{
			if (IsVertical())
			{
				ptGridLineStart.SetPoint(ptGridStart.x, ptStart.y);
				ptGridLineEnd.SetPoint(ptGridEnd.x, ptStart.y);
			}
			else
			{
				ptGridLineStart.SetPoint(ptStart.x, ptGridStart.y);
				ptGridLineEnd.SetPoint(ptStart.x, ptGridEnd.y);
			}

			if (ptPrevGridLineStart.x != 0 && nCount >= m_nFirstIntervalToFill)
			{
				if (nStep == (m_nUnitFillStep - 1))
				{
					CBCGPRect rectInterval(ptPrevGridLineStart, ptGridLineEnd);
					rectInterval.Normalize();

					for (int j = 0; j < arBoundingRects.GetSize(); j++)
					{
						CBCGPRect rectFill = arBoundingRects [j];
						if (rectFill.IntersectRect(rectInterval))
						{
							m_pChart->OnFillAxisUnitInterval(pGM, rectFill, br);
						}
					}
					nStep = 0;
				}
				else
				{
					nStep++;
				}
			}

			ptPrevGridLineStart = ptGridLineStart;
		}

		nCount++;
		

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblUnitsInInterval;
		}

	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawAxisLabels(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	if (m_nDisplayedLines == 0 || m_pChart == NULL || !IsAxisVisible() || 
		m_axisLabelType == CBCGPChartAxis::ALT_NO_LABELS)
	{
		return;
	}

	CBCGPPoint ptStart;
	CBCGPPoint ptEnd;

	GetAxisPos(ptStart, ptEnd);

	CBCGPPoint ptAxisStart = ptStart;
	CBCGPPoint ptAxisEnd = ptEnd;

	CBCGPPoint ptInterval(m_nFixedIntervalWidth, m_nFixedIntervalWidth);

	if (IsVertical())
	{
		m_bReverseOrder ? ptInterval.y = ptStart.y + ptInterval.y : ptInterval.y = ptStart.y - ptInterval.y;
	}
	else
	{
		m_bReverseOrder ? ptInterval.x = ptStart.x - ptInterval.x : ptInterval.x += ptStart.x;
	}

	double dblUnitsInInterval = m_bIsFixedIntervalWidth ? (int)(ValueFromPoint(ptInterval) - GetMinDisplayedValue()) : m_dblMajorUnit;

	if (dblUnitsInInterval <= 0)
	{
		dblUnitsInInterval = 1.;
	}

	double dblCurrValue = GetMinDisplayedValue();
	double dblMajorUnitInterval = fabs(ScaleValue(dblUnitsInInterval));

	CBCGPRect rectPrevLabel;
	
	BOOL bInterlaceLabels = (IsVertical() && dblMajorUnitInterval < m_szMaxLabelSize.cy  || 
								!IsVertical() && dblMajorUnitInterval < m_szMaxLabelSize.cx ) && IsLabelInterlacingEnabled();
	
	int nCount = 0;
	int nStep = (int)(IsVertical() ? (m_szMaxLabelSize.cy / dblMajorUnitInterval) : (m_szMaxLabelSize.cx / dblMajorUnitInterval)) + 1;

	if (m_bUseApproximation && !IsLogScale())
	{
		dblCurrValue = ApproximateValue(dblCurrValue, m_dblMajorUnit, 1.) - m_dblMajorUnit;
	}

	int nOffset = (int) (IsDisplayDataBetweenTickMarks() ? dblMajorUnitInterval / 2 : 0);

	if (m_bReverseOrder)
	{
		nOffset = -nOffset;
	}

	CBCGPPoint ptOffset;
	IsVertical() ? ptOffset.y = -nOffset : ptOffset.x = nOffset;

	BOOL bIs3D = IsDiagram3D();
	double dblUnitStep = CalcLog(GetMinDisplayedValue());
	BOOL bIsDiagram3D = IsDiagram3D();
	BOOL bIsThickFloor = m_pChart->GetDiagram3D() == NULL ? FALSE : m_pChart->GetDiagram3D()->IsThickWallsAndFloor();

	while (dblCurrValue <= GetMaxDisplayedValue() + GetDoubleCorrection())
	{
		if (bIs3D)
		{
			double dblVal = IsDisplayDataBetweenTickMarks() ? dblCurrValue + m_dblMajorUnit / 2 : dblCurrValue;
			ptStart = PointFromValue3D(dblVal, TRUE, FALSE);
		}
		else
		{
			ptStart = PointFromValue3D(dblCurrValue, TRUE, FALSE);
			ptStart += ptOffset;
		}

		BOOL bIsPointOnAxis = IsPointOnAxis(ptStart);

		if (bIsThickFloor && bIsDiagram3D)
		{
			double dblVal = PointFromValue(dblCurrValue, TRUE, FALSE);
			bIsPointOnAxis = IsValueOnAxis3D(dblVal);
		}

		if (!bIsPointOnAxis)
		{
			if (IsLogScale())
			{
				dblUnitStep += 1.;
				dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
			}
			else
			{
				dblCurrValue += dblUnitsInInterval;
			}
			continue;
		}

		if ((IsVertical() && ptStart.y < ptAxisEnd.y || !IsVertical() && ptStart.x > ptAxisEnd.x) && 
			dblCurrValue > GetMaxDisplayedValue() + GetDoubleCorrection() || 
			IsDisplayDataBetweenTickMarks() && dblCurrValue >= GetMaxDisplayedValue())
		{
			break;
		}

		CString strLabel; 
		GetDisplayedLabel(dblCurrValue, strLabel);

		CBCGPSize szLabel = m_pChart->OnCalcAxisLabelSize(pGM, this, dblCurrValue, strLabel, m_axisLabelsFormat.m_textFormat);

		CBCGPSize szPadding = m_axisLabelsFormat.GetContentPadding(TRUE);
		CBCGPRect rectLabel;

		if (bIs3D)
		{
			rectLabel = OnCalcAxisLabelRect3D(ptStart, szLabel); 
		}
		else if (IsVertical())
		{
			rectLabel.SetRect(m_rectAxisLabels.left, 
							ptStart.y - szLabel.cy / 2, 
							m_rectAxisLabels.right, 
							ptStart.y + szLabel.cy / 2);
			
			rectLabel.InflateRect(0, szPadding.cy);
		}
		else
		{
			rectLabel.SetRect(ptStart.x - szLabel.cx / 2, 
				m_rectAxisLabels.top, 
				ptStart.x + szLabel.cx / 2, 
				m_rectAxisLabels.bottom);

			rectLabel.InflateRect(szPadding.cx, 0);
		}

		if (bIs3D || bInterlaceLabels && nCount % nStep == 0 || !bInterlaceLabels || m_nDisplayedLines <= 3)
		{
			if (IsVertical() && m_dblTopOffset != 0)
			{
				if (m_bReverseOrder)
				{
					if (rectLabel.top < ptStart.y)
					{
						rectLabel.top += ptStart.y - rectLabel.top;
						rectLabel.bottom += ptStart.y - rectLabel.top;
					}
				}
				else
				{
					if (rectLabel.top < ptEnd.y)
					{
						rectLabel.top += ptEnd.y - rectLabel.top;
						rectLabel.bottom += ptEnd.y - rectLabel.top;
					}
				}
			}

			m_pChart->OnDrawAxisLabel(pGM, dblCurrValue, strLabel, this, rectLabel, m_rectAxisLabels, rectDiagram);
			rectPrevLabel = rectLabel;
		}

		nCount++;

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblUnitsInInterval;
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnDrawScrollBar(CBCGPGraphicsManager* pGM, const CBCGPRect& rectScrollBar)
{
	if (!CanShowScrollBar() || m_pChart == NULL)
	{
		return;
	}

	OnCalcThumbRect();

	const CBCGPBrush& brLineColor = m_axisLineFormat.m_brLineColor.IsEmpty() ? 
			m_pChart->GetColors().m_brAxisLineColor : m_axisLineFormat.m_brLineColor;

	pGM->FillRectangle(rectScrollBar, m_pChart->GetColors().m_brChartFillColor);
	pGM->DrawRectangle(rectScrollBar, brLineColor);

	CBCGPRect rectThumb = m_rectThumb;

	pGM->FillRectangle(rectThumb, IsVertical() ? m_pChart->GetColors().m_brScrollBarVert : m_pChart->GetColors().m_brScrollBarHorz);
	pGM->DrawRectangle(rectThumb, brLineColor);
	
}
//----------------------------------------------------------------------------//
CBCGPRect CBCGPChartAxis::GetBoundingRects(CArray<CBCGPRect, CBCGPRect>& arRects) const
{
	ASSERT_VALID(this);

	CBCGPRect rectAxisTotal;

	if (m_pChart == NULL)
	{
		return rectAxisTotal;
	}

	ASSERT_VALID(m_pChart);

	for (int i = 0; i < m_pChart->GetAxisCount(); i++)
	{
		CBCGPChartAxis* pAxis = m_pChart->GetChartAxis(i);

		if (pAxis == NULL || pAxis == this || pAxis->IsVertical() && IsVertical() || 
			!pAxis->IsVertical() && !IsVertical())
		{
			continue;
		}

		CBCGPChartAxis* pPerpAxis = pAxis->GetPerpendecularAxis();

		if (pPerpAxis == NULL)
		{
			continue;
		}

		if (pPerpAxis == this)
		{
			CBCGPRect r = pAxis->GetBoundingRect();
			r.Normalize();

			arRects.Add(r);
		}
	}

	if (arRects.GetSize() == 0)
	{
		CBCGPRect r = GetBoundingRect();
		r.Normalize();

		arRects.Add(r);
	}

	for (int j = 0; j < arRects.GetSize(); j++)
	{
		rectAxisTotal.UnionRect(arRects [j], rectAxisTotal);
	}

	return rectAxisTotal;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnCalcDefaultTextAlignment(double /*dblCurrValue*/, const CBCGPRect& /*rectLabel*/, const CBCGPRect& /*rectDiagram*/, 
											CBCGPTextFormat& /*textFormat*/)
{
	ASSERT_VALID(this);
	return;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::CalcMinMaxValues()
{
	ASSERT_VALID(this);

	double dblMin = 0.;
	double dblMax = 0.;

	BOOL bMinMaxSet = FALSE;
	int nSeriesCount = 0;

	CBCGPChartAxis* pAxis = GetPerpendecularAxis();

	if (m_pChart == NULL)
	{
		return FALSE;
	}

	int nNumSeries = m_pChart->GetVisibleSeriesCount();

	if (nNumSeries == 0 || pAxis == NULL)
	{
		return FALSE;
	}

	m_dblMinDataPointDistance = DBL_MAX;

	for (int i = 0 ; i < m_pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries(i);
		if (pSeries == NULL || !pSeries->m_bVisible)
		{
			continue;
		}

		if (pSeries->IsShownOnAxis(this))
		{
			CBCGPChartValue valCurrMin = pSeries->GetMinValue(GetComponentIndex());
			CBCGPChartValue valCurrMax = pSeries->GetMaxValue(GetComponentIndex());

			nSeriesCount++;

			if (valCurrMin.IsEmpty() || valCurrMax.IsEmpty())
			{
				continue;
			}

			if (!bMinMaxSet)
			{
				dblMin = valCurrMin;
				dblMax = valCurrMax;
				bMinMaxSet = TRUE;
			}
			else
			{
				dblMin = min(dblMin, valCurrMin);
				dblMax = max(dblMax, valCurrMax);
			}

			m_dblMinDataPointDistance = min(m_dblMinDataPointDistance, pSeries->GetMinDataPointDistance());
		}
	}

	if (nSeriesCount > 0)
	{
		m_bEmptySeries = FALSE;
	}

	if (IsFixedDisplayRange() && nSeriesCount > 0)
	{
		if (pAxis->m_crossType == CBCGPChartAxis::CT_AXIS_VALUE)
		{
			m_dblMinimum = min(m_dblMinimum, pAxis->m_dblCrossOtherAxisValue);
			m_dblMaximum = max(m_dblMaximum, pAxis->m_dblCrossOtherAxisValue);
		}
		return TRUE;
	}

	if (m_pChart->OnGetMinMaxValues(this, dblMin, dblMax))
	{
		m_dblMinimum = dblMin;
		m_dblMaximum = dblMax;
		bMinMaxSet = TRUE;
		return TRUE;
	}

	if (bMinMaxSet)
	{
		if (pAxis->m_crossType == CBCGPChartAxis::CT_AXIS_VALUE && !IsDiagram3D())
		{
			m_dblMinimum = min(dblMin, pAxis->m_dblCrossOtherAxisValue);
			m_dblMaximum = max(dblMax, pAxis->m_dblCrossOtherAxisValue);
		}
		else
		{
			SetMinimumValue(dblMin);
			SetMaximumValue(dblMax);
		}
	}
	else if (nSeriesCount == 0)
	{
		CBCGPChartAxis* pAxis = GetOppositeAxis();
		if (pAxis != NULL)
		{
			ASSERT_VALID(pAxis);

			m_dblMinimum = pAxis->m_dblMinimum;
			m_dblMaximum = pAxis->m_dblMaximum;
			m_dblMinDisplayedValue = pAxis->GetMinDisplayedValue();
			m_dblMaxDisplayedValue = pAxis->GetMaxDisplayedValue();
			m_dblMajorUnit = pAxis->m_dblMajorUnit;
			m_dblMinorUnit = pAxis->m_dblMinorUnit;
			m_nMaxTotalLines = pAxis->m_nMaxTotalLines;
			m_nDisplayedLines = pAxis->GetDisplayedLinesCount();
			m_bDisplayDataBetweenTickMarks = pAxis->IsDisplayDataBetweenTickMarks();
			m_bFixedRange = pAxis->IsFixedDisplayRange();
			m_bFixedMinimum = pAxis->IsFixedMinimumDisplayValue();
			m_bFixedMaximum = pAxis->IsFixedMaximumDisplayValue();
			m_bFormatAsDate = pAxis->m_bFormatAsDate;
			m_strDataFormat = pAxis->m_strDataFormat;
			m_bEmptySeries = TRUE;
		}
	}

	return bMinMaxSet;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::SetAxisOffsets(double dblBottomOffset, double dblTopOffset, int nGapInPixels)
{
	m_dblBottomOffset = dblBottomOffset;
	m_dblTopOffset = dblTopOffset;
	m_nAxisGap = nGapInPixels;
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxis::Split(double dblPercent, int nGapInPixels, CRuntimeClass* pNewAxisRTC,
									  BOOL bCustomAxisAtTop)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(m_pChart);

	dblPercent = bcg_clamp(dblPercent, 1., 99.);
	m_nAxisGap = nGapInPixels;

	int nNextCustomID = m_pChart->GetNextCustomAxisID();

	CBCGPChartAxis* pAxis = pNewAxisRTC == NULL ? 
		DYNAMIC_DOWNCAST(CBCGPChartAxis, GetRuntimeClass()->CreateObject()) :
		DYNAMIC_DOWNCAST(CBCGPChartAxis, pNewAxisRTC->CreateObject());

	if (pAxis == NULL)
	{
		TRACE0("CBCGPChartAxis::Split. Dynamic Axis creation failed.");
		return NULL;
	}

	pAxis->m_pChart = m_pChart;
	pAxis->SetAxisDefaultPosition(m_axisDefaultPosition);
	pAxis->m_nAxisID = nNextCustomID;

	pAxis->CommonInit();
	m_pChart->AddAxis(pAxis);


	if (bCustomAxisAtTop)
	{
		pAxis->m_dblTopOffset = m_dblTopOffset;
		pAxis->m_nAxisGap = nGapInPixels;
		
		double dblCurrSize = 100. - (m_dblTopOffset + m_dblBottomOffset);
		double dblNewSize = dblCurrSize * (100. - dblPercent) / 100.;
		m_dblTopOffset = 100. - dblNewSize - m_dblBottomOffset;
		
		pAxis->m_dblBottomOffset = m_dblBottomOffset + dblNewSize;

		if (m_pSplitTop != NULL)
		{
			m_pSplitTop->m_pSplitBottom = pAxis;
			pAxis->m_pSplitTop = m_pSplitTop;
		}

		m_pSplitTop = pAxis;
		pAxis->m_pSplitBottom = this;
	}
	else
	{
		double dblCurrSize = 100. - (m_dblTopOffset + m_dblBottomOffset);
		double dblNewSize = dblCurrSize * (100. - dblPercent) / 100.;

		m_dblBottomOffset = m_dblBottomOffset + dblNewSize;
		pAxis->m_dblTopOffset = 100 - m_dblBottomOffset;
 		pAxis->m_nAxisGap = nGapInPixels;

		if (m_pSplitBottom != NULL)
		{
			m_pSplitBottom->m_pSplitTop = pAxis;
			pAxis->m_pSplitBottom = m_pSplitBottom;
		}

		m_pSplitBottom = pAxis;
		pAxis->m_pSplitTop = this;
	}
	

	return pAxis;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::RemoveCustomAxis()
{
	BOOL bHasTop = FALSE;
	if (m_pSplitTop != NULL)
	{
		m_pSplitTop->m_dblBottomOffset = m_dblBottomOffset;
		m_pSplitTop->m_pSplitBottom = m_pSplitBottom;
		bHasTop = TRUE;
	}

	if (m_pSplitBottom != NULL)
	{
		if (!bHasTop)
		{
			m_pSplitBottom->m_dblTopOffset = m_dblTopOffset;
		}
		
		m_pSplitBottom->m_pSplitTop = m_pSplitTop;
	}

	m_pSplitTop = NULL;
	m_pSplitBottom = NULL;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::UpdateAxisSizeByOffset(const CBCGPPoint& pt, BOOL bTop, const CBCGPRect& rectDiagram)
{
	double dblDistance = 0;
	

	if (pt.x == 0 && !IsVertical() || pt.y == 0 && IsVertical())
	{
		return FALSE;
	}

	double dblDiagramSize = IsVertical() ? rectDiagram.Height() : rectDiagram.Width();
	CBCGPRect rectAxis = GetAxisRect(FALSE, TRUE);
	rectAxis.Normalize();

	if (bTop)
	{
		if (IsVertical())
		{
			rectAxis.top += pt.y - m_nAxisGap;
			dblDistance = rectAxis.top - rectDiagram.top;
		}
		else
		{
			rectAxis.right += pt.x + m_nAxisGap;
			dblDistance = rectDiagram.right - rectAxis.right;
		}
	}
	else
	{
		if (IsVertical())
		{
			rectAxis.bottom += pt.y + m_nAxisGap;
			dblDistance = rectDiagram.bottom - rectAxis.bottom;
		}
		else
		{
			rectAxis.left += pt.x - m_nAxisGap;
			dblDistance = rectAxis.left  - rectDiagram.left;
		}
	}

	double dblPercent = (dblDistance * 100. / dblDiagramSize);
	
	bTop ? m_dblTopOffset = dblPercent : m_dblBottomOffset = dblPercent;

	return TRUE;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::CanUpdateAxisSizeByOffset(const CBCGPPoint& ptOffset, BOOL bTop) const
{
	double dblAxisSize = fabs(GetAxisSize());

	if (bTop)
	{
		return IsVertical() && (dblAxisSize - ptOffset.y > 10.) || 
			!IsVertical() && (dblAxisSize + ptOffset.x > 10.);
	}

	return IsVertical() && (dblAxisSize + ptOffset.y > 10.) || 
			!IsVertical() && (dblAxisSize - ptOffset.x > 10.);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxis::IsCustomOrResized() const 
{
	return m_nAxisID >= BCGP_CHART_FIRST_CUSTOM_ID || m_dblTopOffset != 0. || m_dblBottomOffset != 0;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::EnableLabelInterlacing(BOOL bEnable)
{
	ASSERT_VALID(this);

	m_bEnableInterlaceLabels = bEnable;

	if (m_pChart != NULL)
	{
		ASSERT_VALID(m_pChart);
		m_pChart->SetDirty(TRUE);
	}
}
//----------------------------------------------------------------------------//
// 3D interface
//----------------------------------------------------------------------------//
// void CBCGPChartAxis::SetAxisPos3D(const CBCGPPoint& ptStart, const CBCGPPoint& ptEnd)
// {
// 	m_ptAxisStart3D = ptStart;
// 	m_ptAxisEnd3D = ptEnd;
// 
// 	m_dblAxisSize3D = bcg_distance(ptStart, ptEnd);
// }
//----------------------------------------------------------------------------//
void CBCGPChartAxis::CalcAxisPos3D(const CBCGPRect& /*rectDiagram*/, BOOL bUpdateCrossValue)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return;
	}

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

	if (!IsDiagram3D() || pDiagram3D == NULL)
	{
		return;
	}

	if (bUpdateCrossValue)
	{
		double dblCrossValue = GetCrossValuePos(TRUE);

		if (IsVertical())
		{
			m_ptAxisStart3D.x = dblCrossValue;
			m_ptAxisEnd3D.x = dblCrossValue;
		}
		else
		{
			m_ptAxisStart3D.y = dblCrossValue;
			m_ptAxisEnd3D.y = dblCrossValue;
		}
	}
	else
	{
		pDiagram3D->GetNormalAxisCoordinates(m_nAxisID, m_ptAxisStart3D, m_ptAxisEnd3D);
	}

	CBCGPPoint ptCenter = pDiagram3D->GetDiagramRect().CenterPoint();

	m_ptAxisStart = pDiagram3D->TransformPoint(m_ptAxisStart3D, ptCenter);
	m_ptAxisEnd = pDiagram3D->TransformPoint(m_ptAxisEnd3D, ptCenter);

	m_dblAxisSize3D = bcg_distance(m_ptAxisStart, m_ptAxisEnd);
}
//----------------------------------------------------------------------------//
CBCGPRect CBCGPChartAxis::OnCalcAxisLabelRect3D(const CBCGPPoint& ptValue, const CBCGPSize& szLabel)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return CBCGPRect();
	}

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

	if (pDiagram3D == NULL)
	{
		return CBCGPRect();
	}

	CBCGPRect rectLabel;

	CBCGPSize szPadding = m_axisLabelsFormat.GetContentPadding(TRUE);
	double dblDistance = GetLabelDistance();
	double dblXRotation = pDiagram3D->GetXRotation();
	CBCGPPoint ptDistance;

	if (IsVertical())
	{
		if (dblXRotation >= 90 && dblXRotation < 270)
		{
			rectLabel.SetRect(ptValue.x, ptValue.y - szLabel.cy / 2, 
				ptValue.x + szLabel.cx, ptValue.y + szLabel.cy / 2); 
			ptDistance.x = dblDistance;
		}
		else
		{
			rectLabel.SetRect(ptValue.x - szLabel.cx, ptValue.y - szLabel.cy / 2, 
				ptValue.x, ptValue.y + szLabel.cy / 2); 
			ptDistance.x = -dblDistance;
		}
		
		rectLabel.OffsetRect(ptDistance);
		rectLabel.InflateRect(0, szPadding.cy);
	}
	else
	{
		double dblCatetX = m_ptAxisStart.x - m_ptAxisEnd.x;
		double dblCatetY = m_ptAxisStart.y - m_ptAxisEnd.y;

		double dblAngle = dblCatetX == 0 ? 90 : bcg_rad2deg(atan((dblCatetY / dblCatetX)));

		if (dblCatetX == 0 && dblCatetY < 0)
		{
			dblAngle = -90;
		}

		CBCGPPoint ptDiagramCenter = pDiagram3D->GetDiagramRect().CenterPoint() + pDiagram3D->GetScrollOffset();

		BOOL bIsAxisOnLeftSide = m_ptAxisStart.x < ptDiagramCenter.x && m_ptAxisEnd.x < ptDiagramCenter.x;
		BOOL bIsAxisOnRightSide = m_ptAxisStart.x > ptDiagramCenter.x && m_ptAxisEnd.x > ptDiagramCenter.x;

		if (dblAngle >= 55 && dblAngle <= 90 && !bIsAxisOnRightSide || bIsAxisOnLeftSide)
		{
			rectLabel.SetRect(ptValue.x - szLabel.cx, ptValue.y - szLabel.cy / 2, 
				ptValue.x, ptValue.y + szLabel.cy / 2); 
			ptDistance.x = -dblDistance;
			ptDistance.y = dblDistance * cos(bcg_deg2rad(dblAngle));
		}
		else if (dblAngle < -55 && dblAngle >= -90 && !bIsAxisOnLeftSide || bIsAxisOnRightSide)
		{
			rectLabel.SetRect(ptValue.x, ptValue.y - szLabel.cy / 2, 
				ptValue.x + szLabel.cx, ptValue.y + szLabel.cy / 2); 
			ptDistance.x = dblDistance;
			ptDistance.y = dblDistance * cos(fabs(bcg_deg2rad(dblAngle)));
		}
		else
		{
			rectLabel.SetRect(ptValue.x - szLabel.cx / 2, ptValue.y, 
					ptValue.x + szLabel.cx / 2, ptValue.y + szLabel.cy); 
			ptDistance.y = dblDistance;
		}

		rectLabel.OffsetRect(ptDistance);
		rectLabel.InflateRect(szPadding.cx, 0);
	}
	
	return rectLabel;
}
//----------------------------------------------------------------------------//
CBCGPPoint CBCGPChartAxis::GetPointOnAxisPlane3D(double dblValue, BOOL bValueOnWall) const
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return CBCGPPoint();
	}

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

	if (pDiagram3D == NULL)
	{
		return CBCGPPoint();
	}

	CBCGPChartData::ComponentIndex ci = GetComponentIndex();

	if (bValueOnWall)
	{
		CBCGPPoint ptPlane = pDiagram3D->GetAxisPoint(m_nAxisID, CBCGPChartDiagram3D::APT_PLANE);

		switch (ci)
		{
		case CBCGPChartData::CI_X:
		case CBCGPChartData::CI_Y:
			if (IsVertical())
			{
				ptPlane.y = dblValue;
			}
			else
			{
				ptPlane.x = dblValue;
			}
			return ptPlane;

		case CBCGPChartData::CI_Z:
			ptPlane.z = dblValue;
			return ptPlane;
		}
	}
 	else
	{
		switch (ci)
		{
		case CBCGPChartData::CI_X:
		case CBCGPChartData::CI_Y:
			if (IsVertical())
			{
				return CBCGPPoint (m_ptAxisStart3D.x, dblValue, m_ptAxisStart3D.z);
			}

			return CBCGPPoint (dblValue, m_ptAxisStart3D.y, m_ptAxisStart3D.z);

		case CBCGPChartData::CI_Z:
			return CBCGPPoint (m_ptAxisStart3D.x, m_ptAxisStart3D.y, dblValue);
		}
	}

	return CBCGPPoint();
	
}
//----------------------------------------------------------------------------//
void CBCGPChartAxis::OnCalcFloorAndWallGridLinePoints(double dblCurrValue, CBCGPPoint& ptFloor, CBCGPPoint& ptWall)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return;
	}

	CBCGPChartDiagram3D* pDiagram3D = m_pChart->GetDiagram3D();

	if (!IsDiagram3D() || pDiagram3D == NULL)
	{
		return;
	}

	double dblNormalValue = PointFromValue(dblCurrValue, TRUE);

	CBCGPPoint ptFloor3D = pDiagram3D->GetAxisPoint(m_nAxisID, CBCGPChartDiagram3D::APT_FLOOR);
	CBCGPPoint ptWall3D = pDiagram3D->GetAxisPoint(m_nAxisID, CBCGPChartDiagram3D::APT_WALL);

	CBCGPChartData::ComponentIndex ci = GetComponentIndex();

	switch (ci)
	{
	case CBCGPChartData::CI_X:
	case CBCGPChartData::CI_Y:
		if (IsVertical())
		{
			ptFloor3D.y = dblNormalValue;
			ptWall3D.y = dblNormalValue;
		}
		else
		{
			ptFloor3D.x = dblNormalValue;
			ptWall3D.x = dblNormalValue;
		}
		break;

	case CBCGPChartData::CI_Z:
		ptFloor3D.z = dblNormalValue;
		ptWall3D.z = dblNormalValue;
		break;
	}

	CBCGPPoint ptCenter = pDiagram3D->GetDiagramRect().CenterPoint();

	ptFloor = pDiagram3D->TransformPoint(ptFloor3D, ptCenter);
	ptWall = pDiagram3D->TransformPoint(ptWall3D, ptCenter);
}
//----------------------------------------------------------------------------//
// Y-axis implementation
//----------------------------------------------------------------------------//
CBCGPChartAxisY::CBCGPChartAxisY()
{
	
}

CBCGPChartAxisY::CBCGPChartAxisY(int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl) : 
	  CBCGPChartAxis(nAxisID, position, pChartCtrl)
{
	CommonInit();
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisY::CommonInit()
{
	CBCGPChartAxis::CommonInit();
	m_nMinorUnitCount = 5;
	m_nMaxTotalLines = 9;
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxisY::GetOppositeAxis()
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(m_pChart);

	if (m_axisDefaultPosition == CBCGPChartAxis::ADP_LEFT)
	{
		return m_pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
	}

	return m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxisY::GetPerpendecularAxis() const
{
	ASSERT_VALID(this);

	if ( m_pPerpendicularAxis != NULL)
	{
		return m_pPerpendicularAxis;
	}

	if (m_pChart == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(m_pChart);

	if (m_axisDefaultPosition == CBCGPChartAxis::ADP_RIGHT)
	{
		return m_pChart->GetChartAxis(BCGP_CHART_X_SECONDARY_AXIS);
	}

	return m_pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisY::GetPerpendecularAxes(CBCGPChartAxis*& pPrimary, CBCGPChartAxis*& pSecondary)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return;
	}

	ASSERT_VALID(m_pChart);

	pPrimary = m_pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
	pSecondary = m_pChart->GetChartAxis(BCGP_CHART_X_SECONDARY_AXIS);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisY::GetDisplayedLabel(double dblValue, CString& strLabel)
{
	ASSERT_VALID(this);

	strLabel.Empty();

	if (m_pChart != NULL && m_pChart->OnGetDisplayedLabel(this, dblValue, strLabel))
	{
		return;
	}

	CBCGPChartAxis::GetDisplayedLabel(dblValue, strLabel);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxisY::CalcMinMaxValues()
{
	BOOL bHasFullStackedSeries = FALSE;

	for (int i = 0; i < m_pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries(i);

		if (pSeries != NULL && pSeries->m_bVisible && pSeries->GetChartType() == BCGP_CT_100STACKED)
		{
			bHasFullStackedSeries = TRUE;
			break;
		}
	}
	
	if (!bHasFullStackedSeries)
	{
		return CBCGPChartAxis::CalcMinMaxValues();
	}

	SetAutoDisplayRange();

	BOOL bMinMaxSet = CBCGPChartAxis::CalcMinMaxValues();

	if (bMinMaxSet)
	{
		if (m_dblMaximum > 80.)
		{
			SetFixedMaximumDisplayValue(100., FALSE);
		}

		if (m_dblMinimum < -80.)
		{
			SetFixedMinimumDisplayValue(-100., FALSE);
		}
		
		if (m_dblMinimum == 0)
		{
			SetFixedMinimumDisplayValue(0., FALSE);
		}
	}

	return bMinMaxSet;
}
//----------------------------------------------------------------------------//
// X-axis implementation
//----------------------------------------------------------------------------//
CBCGPChartAxisX::CBCGPChartAxisX()
{

}

CBCGPChartAxisX::CBCGPChartAxisX (int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl) : 
	CBCGPChartAxis(nAxisID, position, pChartCtrl)
{
	CommonInit();
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::CommonInit()
{
	CBCGPChartAxis::CommonInit();

	m_nLabelDistanceFromAxis = 100;
	m_nMinorUnitCount = 2;
	m_nMaxTotalLines = 1;
	m_bFixedRange = FALSE;
	m_nMaxDataPointCount = 0;
	m_bDisplayDataBetweenTickMarks = TRUE;
	m_bIsAxisComponentSet = FALSE; 
	m_bOnlyFormula = FALSE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::Reset()
{
	ASSERT_VALID(this);

	CBCGPChartAxis::Reset();
	m_bFixedRange = FALSE;
	m_arCategories.RemoveAll();
	m_nMaxTotalLines = 1;
	m_nMaxDataPointCount = 0;
	m_bIsAxisComponentSet = FALSE;
	m_bOnlyFormula = FALSE;
	m_bDisplayDataBetweenTickMarks = TRUE;
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxisX::GetOppositeAxis()
{
	ASSERT_VALID(this);

	if (m_axisDefaultPosition == CBCGPChartAxis::ADP_BOTTOM)
	{
		return m_pChart->GetChartAxis(BCGP_CHART_X_SECONDARY_AXIS);
	}

	return m_pChart->GetChartAxis(BCGP_CHART_X_PRIMARY_AXIS);
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxisX::GetPerpendecularAxis() const
{
	ASSERT_VALID(this);

	if (m_pPerpendicularAxis != NULL)
	{
		return m_pPerpendicularAxis;
	}

	if (m_axisDefaultPosition == CBCGPChartAxis::ADP_TOP)
	{
		return m_pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
	}

	return m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::GetPerpendecularAxes(CBCGPChartAxis*& pPrimary, CBCGPChartAxis*& pSecondary)
{
	ASSERT_VALID(this);

	pPrimary = m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	pSecondary = m_pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxisX::Scroll(double dblUnitsToScroll, BOOL bUnitsInPixels)
{
	return CBCGPChartAxis::Scroll(dblUnitsToScroll, bUnitsInPixels);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::CheckAxisComponent()
{
	m_bIsAxisComponentSet = FALSE;
	m_bOnlyFormula = TRUE;

	for (int i = 0 ; i < m_pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries(i);
		if (pSeries == NULL || !pSeries->m_bVisible || !pSeries->IsShownOnAxis(this))
		{
			continue;
		}

		if (!pSeries->IsFormulaSeries() && !pSeries->IsVirtualMode())
		{
			m_bOnlyFormula = FALSE;
		}

		if (pSeries->IsComponentSet(GetComponentIndex()) && !pSeries->IsFormulaSeries() && !pSeries->IsVirtualMode())
		{
			m_bIsAxisComponentSet = TRUE;
			break;
		}
	}
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxisX::CalcMinMaxValues()
{
	ASSERT_VALID(this);

	CheckAxisComponent();

	if (!IsFixedDisplayRange())
	{
		if (!IsComponentXSet())
		{
			SetMinimumValue(1);
			SetMaximumValue(1);
		}
		else
		{
			SetMinimumValue(0);
			SetMaximumValue(0);
		}
		
	}

	BOOL bSet = CBCGPChartAxis::CalcMinMaxValues();

	// CalcMinMaxValues values can take the minimum from a formula and ovveride the 1.
	if (bSet && !IsFixedDisplayRange() && !IsComponentXSet())
	{
		m_dblMinimum = min(m_dblMinimum, 1.);
	}

	m_nMaxDataPointCount = 0;
	BOOL bSeriesCount = 0;

	for (int i = 0 ; i < m_pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries(i);
		if (pSeries == NULL || !pSeries->m_bVisible || !pSeries->IsShownOnAxis(this))
		{
			continue;
		}

		bSeriesCount++;

		if (pSeries->IsVirtualMode())
		{
			continue;
		}

		if (pSeries->IsIndexMode())
		{
			break;
		}

		m_nMaxDataPointCount = max(m_nMaxDataPointCount, pSeries->GetDataPointCount());

		if (!IsFixedDisplayRange() && !m_bIsAxisComponentSet)
		{
			SetMaximumValue(max(m_dblMaximum, pSeries->GetDataPointCount()));				
			m_nMaxTotalLines = max(m_nMaxTotalLines, (UINT)m_nMaxDataPointCount);
		}
		else if (m_nMaxTotalLines == 1)
		{
			m_nMaxTotalLines = 0xFFFE;
		}
	}

	if (m_bOnlyFormula && m_nMaxTotalLines == 1)
	{
		m_nMaxTotalLines = 0xFFFE;
	}

	if (bSeriesCount == 0)
	{
		CBCGPChartAxisX* pOpposAxis = DYNAMIC_DOWNCAST(CBCGPChartAxisX, GetOppositeAxis());

		if (pOpposAxis != NULL)
		{
			m_bIsAxisComponentSet = pOpposAxis->IsComponentXSet();
		}
		
		return FALSE;
	}

	if (m_bIsAxisComponentSet && m_nMaxTotalLines == 1)
	{
		m_nMaxTotalLines = 0xFFFE;
	}

	CBCGPChartAxis* pAxis = GetPerpendecularAxis();

	if (pAxis != NULL)
	{
		if (pAxis->m_crossType == CBCGPChartAxis::CT_AXIS_VALUE && !IsDiagram3D())
		{
			m_dblMinimum = min(pAxis->m_dblCrossOtherAxisValue, m_dblMinimum);
			m_dblMaximum = max(pAxis->m_dblCrossOtherAxisValue, m_dblMaximum);
		}
	}

	if (IsDisplayDataBetweenTickMarks())
	{
		m_nMaxTotalLines++;
		SetMaximumValue(max(m_dblMaximum, m_nMaxDataPointCount + 1));
		
	}
	else if (!IsComponentXSet())
	{
		SetMaximumValue(max(m_dblMaximum, m_nMaxDataPointCount));
	}

	return TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::OnBeforeSetFixedDisplayRange(double& /*dblMin*/, double& /*dblMax*/)
{

}
//----------------------------------------------------------------------------//
int CBCGPChartAxisX::GetNumberOfUnitsToDisplay(UINT nUnitsToDisplay)
{
	ASSERT_VALID(this);

	if (m_bIsFxedUnitCount)
	{
		return m_nFixedUnitCount;
	}

	int nUnits = CBCGPChartAxis::GetNumberOfUnitsToDisplay(nUnitsToDisplay);
	if ((nUnits < m_nMaxDataPointCount && m_bFormatAsDate || !m_bIsAxisComponentSet) && !m_bEmptySeries && !m_bFixedMajorUnit && !m_bZoomed && !m_bOnlyFormula)
	{
		nUnits = m_nMaxDataPointCount;
		if (IsDisplayDataBetweenTickMarks())
		{
			nUnits++;
		}
	}
	return nUnits;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::CalcMajorMinorUnits(CBCGPGraphicsManager* pGM, UINT nUnitsToDisplay)
{
	ASSERT_VALID(this);

	if (m_bFormatAsDate && !m_bFixedMajorUnit)
	{
		CBCGPChartAxis::CalcMajorMinorUnits(pGM, nUnitsToDisplay);

		if (m_dblMinAllowedMajorUnit != 0)
		{
			return;
		}

		if (m_dblMinDataPointDistance != DBL_MAX)
		{
			m_dblMajorUnit = m_dblMinDataPointDistance;
			m_nDisplayedLines = GetNumberOfUnitsToDisplay(nUnitsToDisplay);
			m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;
		}
		
		if (!IsFixedMinimumDisplayValue())
		{
			m_dblMinDisplayedValue = m_dblMinimum - m_dblMajorUnit;
		}

		if (!IsFixedMaximumDisplayValue())
		{
			m_dblMaxDisplayedValue = m_dblMaximum + m_dblMajorUnit;
		}

		SetScrollRange(GetMinDisplayedValue(), GetMaxDisplayedValue());
		
		return;
	}

	if (m_bIsAxisComponentSet || m_bOnlyFormula || m_bFormatAsDate || m_bFixedMajorUnit)
	{
		CBCGPChartAxis::CalcMajorMinorUnits(pGM, nUnitsToDisplay);
		return;
	}

	CalcMaxLabelSize(pGM);
	m_dblMajorUnit = 1.;
	m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;

	CBCGPChartAxis* pAxis = GetPerpendecularAxis();

	m_dblMinDisplayedValue = m_dblMinimum;
	m_dblMaxDisplayedValue = m_dblMaximum;
	
	m_nDisplayedLines = (int) m_dblMaximum - (int) m_dblMinimum;

	if (pAxis->m_crossType == CBCGPChartAxis::CT_AXIS_VALUE && !IsDiagram3D())
	{
		m_dblMinDisplayedValue = min(pAxis->m_dblCrossOtherAxisValue, m_dblMinDisplayedValue);
		m_dblMaxDisplayedValue = max(pAxis->m_dblCrossOtherAxisValue, m_dblMaxDisplayedValue);
		m_nDisplayedLines = (int)(m_dblMaxDisplayedValue - m_dblMinDisplayedValue);
	}	

	SetScrollRange(GetMinDisplayedValue(), GetMaxDisplayedValue());

	m_bInitialized = TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::GetDisplayedLabel(double dblValue, CString& strLabel)
{
	ASSERT_VALID(this);

	strLabel.Empty();

	if (m_pChart != NULL && m_pChart->OnGetDisplayedLabel(this, dblValue, strLabel))
	{
		return;
	}

	int nCategoryIndex = (int)(dblValue - 1.);

	if (m_arCategories.GetSize() > 0 && !m_bIsAxisComponentSet)
	{
		if (nCategoryIndex < m_arCategories.GetSize() && nCategoryIndex >= 0)
		{
			strLabel = m_arCategories[nCategoryIndex];
			if (strLabel.IsEmpty())
			{
				CBCGPChartAxis::GetDisplayedLabel(nCategoryIndex + 1, strLabel);
			}
			return;
		}
	}

	CBCGPChartAxis::GetDisplayedLabel(dblValue, strLabel);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisX::CalcMaxLabelSize(CBCGPGraphicsManager* pGM)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pGM);

	CBCGPSize sizeText(0,0);
	CBCGPSize sz(0, 0); 

	if (m_arCategories.GetSize() > 0 && !m_bIsAxisComponentSet)
	{
		for (int i = 0; i < m_arCategories.GetSize(); i++)
		{
			CString strCategory = m_arCategories[i];

			sizeText = m_pChart->OnCalcAxisLabelSize(pGM, this, i, strCategory, m_axisLabelsFormat.m_textFormat);
			m_szMaxLabelSize.cx = max(sz.cx, sizeText.cx);
			m_szMaxLabelSize.cy = max(sz.cy, sizeText.cy);
		}
	}
	else
	{
		CBCGPChartAxis::CalcMaxLabelSize(pGM);
	}
}
//----------------------------------------------------------------------------//
// Z-axis implementation
//----------------------------------------------------------------------------//
CBCGPChartAxisZ::CBCGPChartAxisZ()
{

}
//----------------------------------------------------------------------------//
CBCGPChartAxisZ::CBCGPChartAxisZ(int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl) : 
	CBCGPChartAxisX(nAxisID, position, pChartCtrl)
{
	CommonInit();
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisZ::CommonInit()
{
	m_crossType = CBCGPChartAxis::CT_IGNORE;
	m_bDisplaySeriesNameAsLabel = TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisZ::Reset()
{

}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxisZ::IsAxisVisible() const
{
	BOOL bVisible = CBCGPChartAxisX::IsAxisVisible();

	if (!bVisible || !IsDiagram3D())
	{
		return FALSE;
	}

	return !m_pChart->IsChart3DGrouped();
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxisZ::GetOppositeAxis()
{
	ASSERT_VALID(this);

	return NULL;
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartAxisZ::GetPerpendecularAxis() const
{
	ASSERT_VALID(this);

	if (m_pPerpendicularAxis != NULL)
	{
		return m_pPerpendicularAxis;
	}

	if (m_axisDefaultPosition == CBCGPChartAxis::ADP_DEPTH_TOP)
	{
		return m_pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
	}

	return m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisZ::GetPerpendecularAxes(CBCGPChartAxis*& pPrimary, CBCGPChartAxis*& pSecondary)
{
	ASSERT_VALID(this);

	pPrimary = m_pChart->GetChartAxis(BCGP_CHART_Y_PRIMARY_AXIS);
	pSecondary = m_pChart->GetChartAxis(BCGP_CHART_Y_SECONDARY_AXIS);
}
//----------------------------------------------------------------------------//
void CBCGPChartAxisZ::GetDisplayedLabel(double dblValue, CString& strLabel)
{
	ASSERT_VALID(this);

	strLabel.Empty();

	if (m_pChart != NULL && m_pChart->OnGetDisplayedLabel(this, dblValue, strLabel))
	{
		return;
	}

	int nCategoryIndex = (int)(dblValue - 1.);

	if (!m_bIsAxisComponentSet && m_bDisplaySeriesNameAsLabel)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries(nCategoryIndex, FALSE);
		
		if (pSeries != NULL)
		{
			strLabel = pSeries->m_strSeriesName;
			return;
		}
	}

	CBCGPChartAxis::GetDisplayedLabel(dblValue, strLabel);
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartAxisZ::CalcMinMaxValues()
{
	ASSERT_VALID(this);

	if (m_pChart->IsChart3DGrouped())
	{
		SetMinimumValue(0);
		SetMaximumValue(1);
		m_nMaxTotalLines = 1;
		return TRUE;
	}

	CheckAxisComponent();

	if (!IsFixedDisplayRange())
	{
		if (!IsComponentXSet())
		{
			SetMinimumValue(1);
			SetMaximumValue(1);
		}
		else
		{
			SetMinimumValue(0);
			SetMaximumValue(0);
		}
	}

	BOOL bSet = CBCGPChartAxis::CalcMinMaxValues();

	// CalcMinMaxValues values can take the minimum from a formula and override the 1.
	if (bSet && !IsFixedDisplayRange() && !IsAxisComponentSet())
	{
		m_dblMinimum = min(m_dblMinimum, 1.);
	}

	int nSeriesCount = 0;
	int nMaxGroupID = 0;
	BOOL bIsStacked = FALSE;
	for (int i = 0 ; i < m_pChart->GetSeriesCount(); i++)
	{
		CBCGPChartSeries* pSeries = m_pChart->GetSeries(i);
		if (pSeries == NULL || !pSeries->m_bVisible || !pSeries->IsShownOnAxis(this))
		{
			continue;
		}

		nSeriesCount++;
		nMaxGroupID = max(nMaxGroupID, pSeries->GetGroupID());

		if (!bIsStacked && pSeries->IsStakedSeries())
		{
			bIsStacked = TRUE;
		}
	}

	if (!m_pChart->IsChart3DGrouped() && bIsStacked && !IsAxisComponentSet())
	{
		nSeriesCount = nMaxGroupID + 1;
	}

	if (!IsFixedDisplayRange() && !IsAxisComponentSet())
	{
		SetMaximumValue(max(m_dblMaximum, nSeriesCount));				
		m_nMaxTotalLines = max(m_nMaxTotalLines, (UINT)nSeriesCount);
	}

	if (m_bIsAxisComponentSet && m_nMaxTotalLines == 1)
	{
		m_nMaxTotalLines = 0xFFFE;
	}

	CBCGPChartAxis* pAxis = GetPerpendecularAxis();

	if (pAxis != NULL)
	{
		if (pAxis->m_crossType == CBCGPChartAxis::CT_AXIS_VALUE && !IsDiagram3D())
		{
			m_dblMinimum = min(pAxis->m_dblCrossOtherAxisValue, m_dblMinimum);
			m_dblMaximum = max(pAxis->m_dblCrossOtherAxisValue, m_dblMaximum);
		}
	}

	if (IsDisplayDataBetweenTickMarks())
	{
		m_nMaxTotalLines++;
		SetMaximumValue(max(m_dblMaximum, nSeriesCount + 1));
	}
	else if (!IsAxisComponentSet())
	{
		SetMaximumValue(max(m_dblMaximum, nSeriesCount));
	}

	return TRUE;
}
//----------------------------------------------------------------------------//
// POLAR Axis implementation - Y axis
//----------------------------------------------------------------------------//
CBCGPChartAxisPolarY::CBCGPChartAxisPolarY()
{

}

CBCGPChartAxisPolarY::CBCGPChartAxisPolarY(int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl) :
	CBCGPChartAxisY(nAxisID, position, pChartCtrl)
{
	CommonInit();
}

void CBCGPChartAxisPolarY::CommonInit()
{
	CBCGPChartAxisY::CommonInit();
	m_bRadialGridLines = TRUE;
	m_bUseApproximation = FALSE;
}

CBCGPChartAxis* CBCGPChartAxisPolarY::GetPerpendecularAxis() const
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(m_pChart);

	return m_pChart->GetChartAxis(BCGP_CHART_X_POLAR_AXIS);
}

void CBCGPChartAxisPolarY::CalcNameRect(CBCGPGraphicsManager* /*pGM*/, CBCGPRect& /*rectPlotArea*/, const CRect& /*rectChartArea*/, BOOL /*bReposOnly*/)
{
}

void CBCGPChartAxisPolarY::AdjustDiagramArea(CBCGPRect& /*rectDiagramArea*/, const CBCGPRect& /*rectPlotArea*/)
{
}

void CBCGPChartAxisPolarY::UpdateAxisPos(const CBCGPRect& rectDiagramArea)
{
	ASSERT_VALID(this);

	CalcAxisPos(rectDiagramArea, FALSE);
}

void CBCGPChartAxisPolarY::CalcAxisPos(const CBCGPRect& rectDiagram, BOOL /*bStoreAxisPos*/)
{
	ASSERT_VALID(this);

	CBCGPChartAxis* pXAxis = GetPerpendecularAxis();

	if (pXAxis == NULL)
	{
		return;
	}
	
	double dblPadding = max(pXAxis->m_szMaxLabelSize.cy, pXAxis->m_szMaxLabelSize.cx) * 2;

	if (dblPadding == 0)
	{
		dblPadding = (pXAxis->GetMajorTickMarkLen(TRUE) + 1) * 2;
	}

	m_ptAxisStart = rectDiagram.CenterPoint();
	if (IsVertical())
	{
		m_ptAxisEnd.x = m_ptAxisStart.x;
		m_ptAxisEnd.y = m_ptAxisStart.y - min(rectDiagram.Height() / 2, rectDiagram.Width() / 2);
		m_ptAxisEnd.y += dblPadding;
	}
	else
	{
		m_ptAxisEnd.y = m_ptAxisStart.y;
		m_ptAxisEnd.x = m_ptAxisStart.x + min(rectDiagram.Height() / 2, rectDiagram.Width() / 2);
		m_ptAxisEnd.x -= dblPadding;
	}
}

void CBCGPChartAxisPolarY::CalcLabelsRect(CBCGPRect& rectDiagramArea)
{
	ASSERT_VALID(this);

	CalcAxisPos(rectDiagramArea);

	if (!IsAxisVisible() || m_axisLabelType == CBCGPChartAxis::ALT_NO_LABELS)
	{
		m_rectAxisLabels.SetRectEmpty();
		return;
	}

	double nLabelRectSize = IsVertical() ? m_szMaxLabelSize.cx : m_szMaxLabelSize.cy; 
	double dblDistance = GetLabelDistance();

	CBCGPPoint ptAxisStart = m_ptAxisStart;
	CBCGPPoint ptAxisEnd = m_ptAxisEnd;

	m_rectAxisLabels.SetRect(m_ptAxisStart, m_ptAxisEnd);

	switch(m_axisLabelType)
	{
	case CBCGPChartAxis::ALT_LOW:
	case CBCGPChartAxis::ALT_NEXT_TO_AXIS:
		if (IsVertical())
		{
			m_rectAxisLabels.left = m_ptAxisStart.x - dblDistance - nLabelRectSize;
			m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
		}
		else
		{
			m_rectAxisLabels.top = m_ptAxisStart.y + dblDistance;
			m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
		}
		break;
		
	case CBCGPChartAxis::ALT_HIGH:			
		if (IsVertical())
		{
			m_rectAxisLabels.left = rectDiagramArea.right + dblDistance;
			m_rectAxisLabels.right = m_rectAxisLabels.left + nLabelRectSize;
		}
		else
		{
			m_rectAxisLabels.top = rectDiagramArea.top - dblDistance - nLabelRectSize;
			m_rectAxisLabels.bottom = m_rectAxisLabels.top + nLabelRectSize;
		}
		break;
	}
}

void CBCGPChartAxisPolarY::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	if (!IsAxisVisible() || m_pChart == NULL)
	{
		return;
	}

	CBCGPChartAxisY::OnDraw(pGM, rectDiagram);
}

void CBCGPChartAxisPolarY::OnFillUnitIntervals(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);

	BOOL bIsAxisVisible = IsAxisVisible();
	CBCGPChartAxisPolarX* pXAxis = DYNAMIC_DOWNCAST(CBCGPChartAxisPolarX, GetPerpendecularAxis());

	if (m_nDisplayedLines == 0 || !m_bFillMajorUnitInterval || m_pChart == NULL || pXAxis == NULL || !bIsAxisVisible || !m_bShowMajorGridLines || 
		IsLogScale())
	{
		return;
	}

	ASSERT_VALID(pXAxis);

	double dblCurrValue = GetMinDisplayedValue() + m_nFirstIntervalToFill * m_dblMajorUnit;

	const CBCGPBrush& brChart = pGM->IsSupported(BCGP_GRAPHICS_MANAGER_COLOR_OPACITY) ?
		m_pChart->GetColors().m_brAxisInterlaceColor :
		m_pChart->GetColors().m_brAxisInterlaceColorGDI;

	const CBCGPBrush& br = m_brInterval.IsEmpty() ? brChart : m_brInterval;

	while (dblCurrValue <= GetMaxDisplayedValue() + GetDoubleCorrection() - m_dblMajorUnit)
	{
		double dblRadiusCurr = IsVertical() ? m_ptAxisStart.y - PointFromValue(dblCurrValue, TRUE) : 
			PointFromValue(dblCurrValue, TRUE) - m_ptAxisStart.x;
		double dblRadiusNext = IsVertical() ? m_ptAxisStart.y - PointFromValue(dblCurrValue + m_dblMajorUnit, TRUE) : 
			PointFromValue(dblCurrValue + m_dblMajorUnit, TRUE) - m_ptAxisStart.x;

		if (m_bRadialGridLines)
		{
			CBCGPEllipseGeometry gCurrEllipse(CBCGPEllipse(m_ptAxisStart, dblRadiusCurr, dblRadiusCurr));
			CBCGPEllipseGeometry gNextEllipse(CBCGPEllipse(m_ptAxisStart, dblRadiusNext, dblRadiusNext));

			pGM->CombineGeometry(gNextEllipse, gNextEllipse, gCurrEllipse, RGN_DIFF);
			m_pChart->OnFillAxisUnitInterval(pGM, gNextEllipse, br);
		}
		else
		{
			CBCGPPointsArray arCurr;
			CBCGPPointsArray arNext;

			int nIndex = 0;
			int nMaxValue = (int) pXAxis->GetMaxDisplayedValue();

			if (!pXAxis->IsComponentXSet())
			{
				nMaxValue++;
			}

			for (int i = (int)pXAxis->GetMinDisplayedValue(); i < nMaxValue; i += (int)pXAxis->GetMajorUnit(), nIndex++)
			{
				double dblAnglePoint = bcg_deg2rad(pXAxis->GetAngleFromIndex(nIndex));

				CBCGPPoint ptCurr = m_ptAxisStart;
				CBCGPPoint ptNext = m_ptAxisStart;

				ptCurr.x += dblRadiusCurr * sin(dblAnglePoint);
				ptCurr.y -= dblRadiusCurr * cos(dblAnglePoint);

				ptNext.x += dblRadiusNext * sin(dblAnglePoint);
				ptNext.y -= dblRadiusNext * cos(dblAnglePoint);

				arCurr.Add(ptCurr);
				arNext.Add(ptNext);
			}

			CBCGPPolygonGeometry gCurrPolygon(arCurr);
			CBCGPPolygonGeometry gNextPolygon(arNext);

			pGM->CombineGeometry(gNextPolygon, gNextPolygon, gCurrPolygon, RGN_DIFF);
			m_pChart->OnFillAxisUnitInterval(pGM, gNextPolygon, br);
		}

		dblCurrValue += m_dblMajorUnit * m_nUnitFillStep;
	}

}

void CBCGPChartAxisPolarY::OnDrawMajorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);
	DrawGridLines(pGM, TRUE);
}

void CBCGPChartAxisPolarY::OnDrawMinorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);
	if (!IsLogScale())
	{
		DrawGridLines(pGM, FALSE);
	}
}

void CBCGPChartAxisPolarY::DrawGridLines(CBCGPGraphicsManager* pGM, BOOL bMajor)
{
	ASSERT_VALID(this);

	BOOL bIsAxisVisible = IsAxisVisible();
	CBCGPChartAxisPolarX* pXAxis = DYNAMIC_DOWNCAST(CBCGPChartAxisPolarX, GetPerpendecularAxis());

	if (m_nDisplayedLines == 0 || m_pChart == NULL || pXAxis == NULL || !bIsAxisVisible || !m_bShowMajorGridLines && bMajor ||
		!m_bShowMinorGridLines && !bMajor)
	{
		return;
	}

	ASSERT_VALID(pXAxis);

	double dblUnit = bMajor ? m_dblMajorUnit : m_dblMinorUnit;
	const BCGPChartFormatLine& formatGridLine = bMajor ? m_majorGridLineFormat : m_minorGridLineFormat;

	BOOL bRadialLines = m_bRadialGridLines;

	double dblRadius = 0.;
	double dblUnitStep = CalcLog(GetMinDisplayedValue());

	double dblCurrValue = IsLogScale() ? GetMinDisplayedValue() : GetMinDisplayedValue() + dblUnit;

	while (dblCurrValue <= GetMaxDisplayedValue() + GetDoubleCorrection())
	{
		dblRadius = IsVertical() ? m_ptAxisStart.y - PointFromValue(dblCurrValue, TRUE) : 
			PointFromValue(dblCurrValue, TRUE) - m_ptAxisStart.x;

		CBCGPEllipse e(m_ptAxisStart, dblRadius, dblRadius);

		if (bRadialLines)
		{
			m_pChart->OnDrawGridEllipse(pGM, e, this, dblCurrValue, formatGridLine, bMajor);
		}
		else
		{
			int nIndex = 0;
			int nMaxValue = (int) pXAxis->GetMaxDisplayedValue();

			if (!pXAxis->IsComponentXSet())
			{
				nMaxValue++;
			}

			for (int i = (int)pXAxis->GetMinDisplayedValue(); i < nMaxValue; i += (int)pXAxis->GetMajorUnit(), nIndex++)
			{
				double dblAngleStart = bcg_deg2rad(pXAxis->GetAngleFromIndex(nIndex));
				double dblAngleEnd = bcg_deg2rad(pXAxis->GetAngleFromIndex(nIndex + 1));

				CBCGPPoint ptStart = m_ptAxisStart;
				CBCGPPoint ptEnd = m_ptAxisStart;

				ptStart.x += dblRadius * sin(dblAngleStart);
				ptStart.y -= dblRadius * cos(dblAngleStart);

				ptEnd.x += dblRadius * sin(dblAngleEnd);
				ptEnd.y -= dblRadius * cos(dblAngleEnd);

				m_pChart->OnDrawGridLine(pGM, ptStart, ptEnd, this, dblCurrValue, formatGridLine, bMajor);
			}
		}

		if (IsLogScale())
		{
			dblUnitStep += 1.;
			dblCurrValue = pow(m_dblMajorUnit, dblUnitStep);
		}
		else
		{
			dblCurrValue += dblUnit;
		}
	}
}

CBCGPRect CBCGPChartAxisPolarY::GetBoundingRect() const
{
	ASSERT_VALID(this);

	CBCGPRect rect(m_bReverseOrder ? m_ptAxisEnd : m_ptAxisStart, m_bReverseOrder ? m_ptAxisEnd : m_ptAxisStart);
	double dblSize = fabs(GetAxisSize());
	rect.InflateRect(dblSize, dblSize);

	return rect;
}

CBCGPRect CBCGPChartAxisPolarY::GetBoundingRects(CArray<CBCGPRect, CBCGPRect>& arRects) const
{
	ASSERT_VALID(this);

	CBCGPRect r = GetBoundingRect();
	arRects.Add(r);

	return r;
}
//----------------------------------------------------------------------------//
// POLAR Axis implementation - X axis
//----------------------------------------------------------------------------//
CBCGPChartAxisPolarX::CBCGPChartAxisPolarX()
{

}

CBCGPChartAxisPolarX::CBCGPChartAxisPolarX(int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl) :
		CBCGPChartAxisX(nAxisID, position, pChartCtrl)
{
	CommonInit();
}

void CBCGPChartAxisPolarX::CommonInit()
{
	CBCGPChartAxisX::CommonInit();

	m_nMinorUnitCount = 5;
	m_dblMajorUnit = 15.;
	m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;
	m_dblMinDisplayedValue = 0.;
	m_dblMaxDisplayedValue = 360.;

	m_bRotateLabels = FALSE;
	m_bUseGridLineForAxisLine = FALSE;
}

void CBCGPChartAxisPolarX::CalcMajorMinorUnits(CBCGPGraphicsManager* /*pGM*/, UINT /*nUnitsToDisplay*/)
{
	CheckAxisComponent();

	if (!IsComponentXSet())
	{
		m_dblMajorUnit = 1.;
		m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;
		m_dblMinDisplayedValue = 0;
		m_dblMaxDisplayedValue = m_nMaxDataPointCount - 1;
	}
	else
	{
		m_dblMajorUnit = 15.;
		m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;
		m_dblMinDisplayedValue = 0.;
		m_dblMaxDisplayedValue = 360.;
	}
}

void CBCGPChartAxisPolarX::SetFixedMajorUnit(double dblMajorUnit, BOOL bSet)
{
	CheckAxisComponent();

	if (IsComponentXSet())
	{
		if (bSet)
		{
			m_dblMajorUnit = bcg_clamp(dblMajorUnit, 1., 360.);
		}
		else
		{
			m_dblMajorUnit = 15;
		}

		m_dblMinorUnit = m_dblMajorUnit / m_nMinorUnitCount;
	}
}

void CBCGPChartAxisPolarX::CalcMinMaxDisplayedValues(BOOL /*bCalcMinDisplayedValue*/, double /*dblApproximation*/)
{
	ASSERT_VALID(this);

	CheckAxisComponent();

	if (!IsComponentXSet())
	{
		m_dblMinDisplayedValue = 0.;
		m_dblMaxDisplayedValue = 360.;
	}
	else
	{
		m_dblMinDisplayedValue = 0;
		m_dblMaxDisplayedValue = m_nMaxDataPointCount - 1;
	}
}

CBCGPChartAxis* CBCGPChartAxisPolarX::GetPerpendecularAxis() const
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return NULL;
	}

	ASSERT_VALID(m_pChart);

	return m_pChart->GetChartAxis(BCGP_CHART_Y_POLAR_AXIS);
}

BOOL CBCGPChartAxisPolarX::HitTest(const CBCGPPoint& pt, BCGPChartHitInfo* pHitInfo, DWORD dwHitInfoFlags)
{
	ASSERT_VALID(this);

	if (pHitInfo == NULL || !IsAxisVisible() || (dwHitInfoFlags & BCGPChartHitInfo::HIT_AXIS) == 0)
	{
		return FALSE;
	}

	CBCGPPoint ptCenter;
	double dblRadius = GetRadius(ptCenter);

	double dblDistance = bcg_distance(pt, ptCenter);

	if (fabs(dblDistance - dblRadius) < GetMajorTickMarkLen(TRUE))
	{
		pHitInfo->m_hitInfo = BCGPChartHitInfo::HIT_AXIS;
		pHitInfo->m_nIndex1 = m_nAxisID;
		return TRUE;
	}

	return FALSE;
}

void CBCGPChartAxisPolarX::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL || !IsAxisVisible() || !IsComponentXSet())
	{
		return;
	}

	CBCGPChartAxisPolarY* pAxisY = DYNAMIC_DOWNCAST(CBCGPChartAxisPolarY, GetPerpendecularAxis());
	if (pAxisY == NULL || !pAxisY->m_bRadialGridLines || m_dblMajorUnit == 0)  
	{
		return;
	}

	CBCGPPoint ptCenter;
	double dblRadius = GetRadius(ptCenter);

	double dblCurrValue = GetMinDisplayedValue();

	while (dblCurrValue < GetMaxDisplayedValue())
	{
		if (m_majorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS ||
			m_majorTickMarkType == CBCGPChartAxis::TMT_NO_TICKS && 
			m_minorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS)
		{
			// if major tick marks are not visible, draw minor in place of them 
			DrawTickMark(pGM, dblCurrValue, ptCenter, dblRadius, 
				m_majorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS);
		}

		if ((m_minorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS) && dblCurrValue + m_dblMinorUnit <= GetMaxDisplayedValue())
		{
			double dblMinorValue = dblCurrValue + m_dblMinorUnit;
			for (int i = 0; i < m_nMinorUnitCount - 1; i++)
			{
				DrawTickMark(pGM, dblMinorValue, ptCenter, dblRadius, FALSE);
				dblMinorValue += m_dblMinorUnit;
			}
		}

		dblCurrValue += m_dblMajorUnit;
	}


	CBCGPEllipse ellipse(ptCenter, dblRadius, dblRadius);


	if (m_bUseGridLineForAxisLine)
	{
		const CBCGPBrush& brLine = m_majorGridLineFormat.m_brLineColor.IsEmpty() ? 
			m_pChart->GetColors().m_brAxisMajorGridLineColor : m_majorGridLineFormat.m_brLineColor;

		pGM->DrawEllipse(ellipse, brLine, m_majorGridLineFormat.GetLineWidth(TRUE), 
							&m_majorGridLineFormat.m_strokeStyle);
	}
	else
	{
		const CBCGPBrush& brLineColor = m_axisLineFormat.m_brLineColor.IsEmpty() ? 
						m_pChart->GetColors().m_brAxisLineColor : m_axisLineFormat.m_brLineColor;

		pGM->DrawEllipse(ellipse, brLineColor, m_axisLineFormat.GetLineWidth(TRUE), &m_axisLineFormat.m_strokeStyle);
	}
}

void CBCGPChartAxisPolarX::DrawTickMark(CBCGPGraphicsManager* pGM, double dblValue, CBCGPPoint ptPolarCenter, 
										double dblRadius, BOOL bMajor)
{
	ASSERT_VALID(this);

	CBCGPChartAxis::TickMarkType tickMarkType = bMajor ? m_majorTickMarkType : m_minorTickMarkType;
	double dblTickMarkeLen = bMajor ? GetMajorTickMarkLen(TRUE) : GetMinorTickMarkLen(TRUE);

	double dblAngle = bcg_deg2rad(dblValue);

	ptPolarCenter.x += dblRadius * sin(dblAngle); 
	ptPolarCenter.y -= dblRadius * cos(dblAngle);

	CBCGPPoint ptStartTick = ptPolarCenter; 
	CBCGPPoint ptEndTick = ptPolarCenter;

	switch (tickMarkType)
	{
	case CBCGPChartAxis::TMT_INSIDE:
		ptStartTick.x -= dblTickMarkeLen * sin(dblAngle); 
		ptStartTick.y += dblTickMarkeLen * cos(dblAngle); 
		break;

	case CBCGPChartAxis::TMT_OUTSIDE:
		ptStartTick.x += dblTickMarkeLen * sin(dblAngle); 
		ptStartTick.y -= dblTickMarkeLen * cos(dblAngle); 
		break;
	case CBCGPChartAxis::TMT_CROSS:
		ptStartTick.x -= dblTickMarkeLen * sin(dblAngle); 
		ptStartTick.y += dblTickMarkeLen * cos(dblAngle); 
		ptEndTick.x += dblTickMarkeLen * sin(dblAngle); 
		ptEndTick.y -= dblTickMarkeLen * cos(dblAngle); 
		break;
	}

	if (m_bUseGridLineForAxisLine)
	{
		BCGPChartFormatLine lineFormat = m_majorGridLineFormat;
		lineFormat.m_brLineColor = m_majorGridLineFormat.m_brLineColor.IsEmpty() ? 
			m_pChart->GetColors().m_brAxisMajorGridLineColor : m_majorGridLineFormat.m_brLineColor;

		m_pChart->OnDrawTickMark(pGM, ptStartTick, ptEndTick, lineFormat, dblValue, bMajor);
	}
	else
	{
		m_pChart->OnDrawTickMark(pGM, ptStartTick, ptEndTick, m_axisLineFormat, dblValue, bMajor);
	}
}

void CBCGPChartAxisPolarX::OnDrawMajorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);
	DrawGridLines(pGM, TRUE);
}

void CBCGPChartAxisPolarX::OnDrawMinorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);
	DrawGridLines(pGM, FALSE);
}

void CBCGPChartAxisPolarX::DrawGridLines(CBCGPGraphicsManager* pGM, BOOL bMajor)
{
	ASSERT_VALID(this);

	CBCGPPoint ptCenter;
	double dblRadius = GetRadius(ptCenter);

	if (m_pChart == NULL || dblRadius == 0 || !m_bShowMajorGridLines && bMajor ||
		!m_bShowMinorGridLines && !bMajor || !IsAxisVisible() ||
		!m_bIsAxisComponentSet && m_nMaxDataPointCount == 0  || IsComponentXSet() && m_dblMajorUnit == 0)
	{
		return;
	}

	if (!IsComponentXSet() && !bMajor)
	{
		return;
	}

	double dblUnit = bMajor ? m_dblMajorUnit : m_dblMinorUnit;
	const BCGPChartFormatLine& formatGridLine = bMajor ? m_majorGridLineFormat : m_minorGridLineFormat;

	ASSERT_VALID(m_pChart);
	double dblCurrValue = GetMinDisplayedValue();
	double dblMaxValue = IsComponentXSet() ? GetMaxDisplayedValue() : 360.;
	double dblAngleStep = IsComponentXSet() ? dblUnit : 360. / m_nMaxDataPointCount;

	while (dblCurrValue < dblMaxValue)
	{
		CBCGPPoint ptGridLineEnd = ptCenter;
		double dblAngle = bcg_deg2rad(dblCurrValue);

		ptGridLineEnd.x += dblRadius * sin(dblAngle); 
		ptGridLineEnd.y -= dblRadius * cos(dblAngle); 

		m_pChart->OnDrawGridLine(pGM, ptCenter, ptGridLineEnd, this, dblCurrValue, formatGridLine, bMajor);
		dblCurrValue += dblAngleStep;
	}
}

void CBCGPChartAxisPolarX::OnFillUnitIntervals(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	CBCGPPoint ptCenter;
	double dblRadius = GetRadius(ptCenter);

	if (m_pChart == NULL || dblRadius == 0 || !m_bShowMajorGridLines || !IsAxisVisible())
	{
		return;
	}

	CBCGPChartAxisPolarY* pAxisY = DYNAMIC_DOWNCAST(CBCGPChartAxisPolarY, GetPerpendecularAxis());
	if (pAxisY == NULL  || !IsComponentXSet() && m_nMaxDataPointCount == 0  ||
		IsComponentXSet() && m_dblMajorUnit == 0)
	{
		return;
	}
	
	ASSERT_VALID(pAxisY);

	const CBCGPBrush& brChart = pGM->IsSupported(BCGP_GRAPHICS_MANAGER_COLOR_OPACITY) ?
		m_pChart->GetColors().m_brAxisInterlaceColor :
		m_pChart->GetColors().m_brAxisInterlaceColorGDI;

	const CBCGPBrush& br = m_brInterval.IsEmpty() ? brChart : m_brInterval;

	double dblMaxValue = m_bIsAxisComponentSet ? GetMaxDisplayedValue() : 360.;
	double dblAngleStep = m_bIsAxisComponentSet ? m_dblMajorUnit : 360. / m_nMaxDataPointCount;

	double dblCurrValue = GetMinDisplayedValue() + dblAngleStep * m_nFirstIntervalToFill;

	while (dblCurrValue < dblMaxValue)
	{
		CBCGPPoint ptGridLineCurr = ptCenter;
		CBCGPPoint ptGridLineNext = ptCenter;

		double dblAngleCurr = bcg_deg2rad(dblCurrValue);
		double dblAngleNext = bcg_deg2rad(dblCurrValue + dblAngleStep);

		ptGridLineCurr.x += dblRadius * sin(dblAngleCurr); 
		ptGridLineCurr.y -= dblRadius * cos(dblAngleCurr); 

		ptGridLineNext.x += dblRadius * sin(dblAngleNext); 
		ptGridLineNext.y -= dblRadius * cos(dblAngleNext); 

		dblCurrValue += dblAngleStep * m_nUnitFillStep;

		CBCGPComplexGeometry gFill(ptCenter);
		gFill.AddLine(ptGridLineCurr);

		if (pAxisY->m_bRadialGridLines)
		{
			gFill.AddArc(ptGridLineNext, CBCGPSize(dblRadius, dblRadius), TRUE, FALSE, 0);
		}
		else
		{
			gFill.AddLine(ptGridLineNext);
		}
		
		m_pChart->OnFillAxisUnitInterval(pGM, gFill, br);
	}
}

void CBCGPChartAxisPolarX::OnDrawAxisLabels(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	CBCGPPoint ptCenter;
	double dblRadius = GetRadius(ptCenter);

	if (m_pChart == NULL || !IsAxisVisible() || dblRadius == 0. || 
		m_axisLabelType == CBCGPChartAxis::ALT_NO_LABELS || !IsComponentXSet() && m_nMaxDataPointCount == 0 ||
		IsComponentXSet() && m_dblMajorUnit == 0)
	{
		return;
	}

	ASSERT_VALID(m_pChart);
	double dblCurrValue = GetMinDisplayedValue();

	double dblMaxValue = 360.;
	double dblAngleStep = IsComponentXSet() ? m_dblMajorUnit : 360. / m_nMaxDataPointCount;

	CBCGPSize szContentPadding = m_axisLabelsFormat.GetContentPadding(TRUE);

	double dblDistanceX = dblRadius + m_szMaxLabelSize.cx - szContentPadding.cx + m_dblLabelDistance;
	double dblDistanceY = dblRadius + m_szMaxLabelSize.cy - szContentPadding.cy + m_dblLabelDistance;

	int nCount = 0;

	while (dblCurrValue < dblMaxValue)
	{
		CBCGPPoint ptLabelCenter = ptCenter;
		double dblAngle = bcg_deg2rad(dblCurrValue);

		ptLabelCenter.x += dblDistanceX * sin(dblAngle); 
		ptLabelCenter.y -= dblDistanceY * cos(dblAngle); 

		CBCGPRect rectLabel(ptLabelCenter, ptLabelCenter);
		rectLabel.InflateRect(m_szMaxLabelSize.cx / 2, m_szMaxLabelSize.cy / 2);

		double dblDisplayedValue = IsComponentXSet() ? dblCurrValue : nCount;

		CString strLabel;
		GetDisplayedLabel(dblDisplayedValue, strLabel);

		double dblDrawingAngle = m_axisLabelsFormat.m_textFormat.GetDrawingAngle();

		if (m_bRotateLabels)
		{
			m_axisLabelsFormat.m_textFormat.SetDrawingAngle(GetVariableLabelAngle(dblCurrValue));
		}

		m_pChart->OnDrawAxisLabel(pGM, dblDisplayedValue, strLabel, this, rectLabel, m_rectAxisLabels, rectDiagram);		

		if (m_bRotateLabels)
		{
			m_axisLabelsFormat.m_textFormat.SetDrawingAngle(dblDrawingAngle);
		}

		dblCurrValue += dblAngleStep;
		nCount++;
	}
}

void CBCGPChartAxisPolarX::SwapDirection(BOOL /*bAdjustGradientAngles*/)
{
	ASSERT_VALID(this);
	m_bIsVertical = !IsVertical();
}

double CBCGPChartAxisPolarX::GetVariableLabelAngle (double dblValue)
{
	return 90 - dblValue;
}

double CBCGPChartAxisPolarX::GetAngleFromIndex(int nIndex) const
{
	if (IsComponentXSet())
	{
		return m_dblMajorUnit * nIndex;
	}

	if (m_nMaxDataPointCount == 0)
	{
		return 0;
	}

	return 360. / m_nMaxDataPointCount * nIndex;
}

CBCGPRect CBCGPChartAxisPolarX::GetBoundingRect() const
{
	ASSERT_VALID(this);

	CBCGPChartAxis* pAxis = GetPerpendecularAxis();
	if (pAxis != NULL)
	{
		return pAxis->GetBoundingRect();
	}

	if (m_pChart != NULL)
	{
		return m_pChart->GetDiagramArea();
	}

	return CBCGPRect();
}

CBCGPRect CBCGPChartAxisPolarX::GetBoundingRects(CArray<CBCGPRect, CBCGPRect>& arRects) const
{
	ASSERT_VALID(this);

	CBCGPRect r = GetBoundingRect();
	arRects.Add(r);

	return r;
}

double CBCGPChartAxisPolarX::GetRadius(CBCGPPoint& ptCenter) const
{
	ASSERT_VALID(this);

	CBCGPChartAxis* pAxisY = GetPerpendecularAxis();
	if (pAxisY == NULL)
	{
		return 0;
	}
	
	ASSERT_VALID(pAxisY);

	CBCGPPoint ptAxisStart;
	CBCGPPoint ptAxisEnd;

	pAxisY->GetAxisPos(ptAxisStart, ptAxisEnd);
	ptCenter = ptAxisStart;

	return fabs(pAxisY->GetAxisSize());
}

double CBCGPChartAxisPolarX::GetAxisSize() const
{
	CBCGPPoint ptCenter;
	return 2 * M_PI * GetRadius(ptCenter);
}
//----------------------------------------------------------------------------//
// Ternary axis implementation
//----------------------------------------------------------------------------//
CBCGPChartTernaryAxis::CBCGPChartTernaryAxis()
{

}
//----------------------------------------------------------------------------//
CBCGPChartTernaryAxis::CBCGPChartTernaryAxis(int nAxisID, CBCGPChartAxis::AxisDefaultPosition position, CBCGPChartVisualObject* pChartCtrl) :
	CBCGPChartAxis(nAxisID, position, pChartCtrl)
{
	CommonInit();
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::CommonInit()
{
	CBCGPChartAxis::CommonInit();
	m_nMaxTotalLines = 100;
	m_bUsePerpAxisForZoning = FALSE;

	switch (m_axisDefaultPosition)
	{
	case CBCGPChartAxis::ADP_RIGHT:
		m_strAxisName = _T("A Component");
		m_dblTickAngle = bcg_deg2rad(60);
		break;
	case CBCGPChartAxis::ADP_LEFT:
		m_strAxisName = _T("B Component");
		m_dblTickAngle = bcg_deg2rad(-60);
		break;
	case CBCGPChartAxis::ADP_BOTTOM:
		m_strAxisName = _T("C Component");
		m_dblTickAngle = bcg_deg2rad(180);
		break;
	}

	m_axisNameFormat.m_textFormat.SetDrawingAngle(0);

	m_bDisplayAxisName = TRUE;
	m_bShowMajorGridLines = TRUE;

	m_minorTickMarkType = CBCGPChartAxis::TMT_OUTSIDE;
	m_nMinorUnitCount = 5;

	SetFixedMajorUnit(25, TRUE);

	m_dblACoef = 0;
	m_dblBCoef = 0;

	m_labelMode = CBCGPChartTernaryAxis::LM_NORMAL;
}
//----------------------------------------------------------------------------//
CBCGPChartAxis* CBCGPChartTernaryAxis::GetPerpendecularAxis() const
{
	CBCGPChartAxis* pAxis = NULL;

	switch (m_axisDefaultPosition)
	{
	case CBCGPChartAxis::ADP_RIGHT:
		pAxis = m_pChart->GetChartAxis(BCGP_CHART_B_TERNARY_AXIS);
		break;

	case CBCGPChartAxis::ADP_LEFT:
		pAxis = m_pChart->GetChartAxis(BCGP_CHART_C_TERNARY_AXIS);
		break;

	case CBCGPChartAxis::ADP_BOTTOM:
		pAxis = m_pChart->GetChartAxis(BCGP_CHART_A_TERNARY_AXIS);
		break;
	}

	return pAxis;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartTernaryAxis::CalcMinMaxValues()
{
	ASSERT_VALID(this);

	m_dblMinimum = 0;
	m_dblMaximum = 100;

	return TRUE;
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::CalcMinMaxDisplayedValues(BOOL /*bCalcMinDisplayedValue*/, double /*dblApproximation*/)
{
	ASSERT_VALID(this);

	m_dblMinDisplayedValue = 0;
	m_dblMaxDisplayedValue = 100;
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::AdjustDiagramArea(CBCGPRect& /*rectDiagramArea*/, const CBCGPRect& /*rectPlotArea*/)
{

}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::UpdateAxisPos(const CBCGPRect& /*rectDiagramArea*/)
{

}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::CalcNameRect(CBCGPGraphicsManager* pGM, CBCGPRect& /*rectPlotArea*/, const CRect& /*rectChartArea*/, BOOL bReposOnly)
{
	ASSERT_VALID(this);

	if (!bReposOnly)
	{
		m_rectAxisName.SetRectEmpty();
	}

	if (!m_bDisplayAxisName || !IsAxisVisible())
	{
		return;
	}

	if (!bReposOnly)
	{
		// called for the first time, axis position has not been calculated, therefore
		// we need to set the size of name only;
		// this size will be used later to calculate axis position
		CBCGPSize szAxisNameSize = m_pChart->OnCalcAxisNameSize(pGM, this);
		m_rectAxisName.SetRect(CBCGPPoint(0, 0), szAxisNameSize);
	}
	else
	{
		// axis position has been calculated;
		// we can set the actual name rect
		CBCGPSize szName = m_rectAxisName.Size();

		switch (m_axisDefaultPosition)
		{
		case CBCGPChartAxis::ADP_RIGHT:
			m_rectAxisName.left = m_rectBounds.CenterPoint().x - szName.cx / 2;
			m_rectAxisName.top = m_rectBounds.top - szName.cy - m_szMaxLabelSize.cy;
			break;

		case CBCGPChartAxis::ADP_LEFT:
			m_rectAxisName.left = m_rectBounds.left - szName.cx / 2;
			m_rectAxisName.top = m_rectBounds.bottom + m_pChart->GetChartAxis(BCGP_CHART_C_TERNARY_AXIS)->m_szMaxLabelSize.cy +
				m_pChart->GetChartAxis(BCGP_CHART_C_TERNARY_AXIS)->GetLabelDistance();
			break;

		case CBCGPChartAxis::ADP_BOTTOM:
			m_rectAxisName.left = m_rectBounds.right - szName.cx / 2;
			m_rectAxisName.top = m_rectBounds.bottom + m_szMaxLabelSize.cy + GetLabelDistance();
			break;
		}

		m_rectAxisName.SetSize(szName);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::CalcAxisPos(const CBCGPRect& rectDiagram, BOOL /*bStoreAxisPos*/)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return;
	}
	CBCGPRect rcDiagram = rectDiagram;

	CBCGPChartAxis* pRightAxis = m_pChart->GetChartAxis(BCGP_CHART_A_TERNARY_AXIS);
	CBCGPChartAxis* pLeftAxis = m_pChart->GetChartAxis(BCGP_CHART_B_TERNARY_AXIS);
	CBCGPChartAxis* pBottomAxis = m_pChart->GetChartAxis(BCGP_CHART_C_TERNARY_AXIS);

	rcDiagram.top += pRightAxis->m_rectAxisName.Height();
	rcDiagram.top += max(pRightAxis->m_szMaxLabelSize.cy + pRightAxis->GetLabelDistance(), pLeftAxis->m_szMaxLabelSize.cy + pLeftAxis->GetLabelDistance());
	rcDiagram.bottom -= max(pBottomAxis->m_rectAxisName.Height(), pLeftAxis->m_rectAxisName.Height());
	rcDiagram.bottom -= pBottomAxis->m_szMaxLabelSize.cy + pBottomAxis->GetLabelDistance();
	rcDiagram.left += max(pBottomAxis->m_rectAxisName.Width(), pLeftAxis->m_rectAxisName.Width()) / 2;
	rcDiagram.right -= max(pBottomAxis->m_rectAxisName.Width(), pLeftAxis->m_rectAxisName.Width()) / 2;

	CBCGPPoint ptCenter = rcDiagram.CenterPoint();

	double dblSize = rcDiagram.Width() > rcDiagram.Height() ? rcDiagram.Height() : rcDiagram.Width();
	m_rectBounds.SetRect(ptCenter, ptCenter);

	m_rectBounds.InflateRect(dblSize / 2, dblSize / 2);
	double dblNewTop = m_rectBounds.bottom - m_rectBounds.Width() * sin(bcg_deg2rad(60));
	double dblOffset = (dblNewTop - m_rectBounds.top) / 2;
	m_rectBounds.top = dblNewTop;
	m_rectBounds.OffsetRect(0, -dblOffset);

	switch (m_axisDefaultPosition)
	{
	case CBCGPChartAxis::ADP_RIGHT:
		m_ptAxisStart = m_rectBounds.BottomRight();
		m_ptAxisEnd.x = m_rectBounds.CenterPoint().x;
		m_ptAxisEnd.y = m_rectBounds.top;
		break;

	case CBCGPChartAxis::ADP_LEFT:
		m_ptAxisStart.x = m_rectBounds.CenterPoint().x;
		m_ptAxisStart.y = m_rectBounds.top;
		m_ptAxisEnd = m_rectBounds.BottomLeft();
		break;

	case CBCGPChartAxis::ADP_BOTTOM:
		m_ptAxisStart = m_rectBounds.BottomLeft();
		m_ptAxisEnd = m_rectBounds.BottomRight();
		break;
	}

	m_dblACoef = (m_ptAxisEnd.y - m_ptAxisStart.y) / (m_ptAxisEnd.x - m_ptAxisStart.x);
	m_dblBCoef = m_ptAxisEnd.y - m_dblACoef * m_ptAxisEnd.x;
}
//----------------------------------------------------------------------------//
double CBCGPChartTernaryAxis::GetAxisSize() const
{
	return m_rectBounds.Width();
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::CalcLabelsRect(CBCGPRect& /*rectDiagramArea*/)
{
	
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartTernaryAxis::HitTestAxisShape(const CBCGPPoint& pt) const
{
	double dblY = m_dblACoef * pt.x + m_dblBCoef;

	return pt.y > dblY - m_nMajorTickMarkLen && pt.y < dblY + m_nMajorTickMarkLen;
}
//----------------------------------------------------------------------------//
BOOL CBCGPChartTernaryAxis::HitTestAxisLables(const CBCGPPoint& /*pt*/) const
{
	return FALSE;
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::GetDisplayedLabel(double dblValue, CString& strLabel)
{
	switch (m_labelMode)
	{
	case CBCGPChartTernaryAxis::LM_NORMAL:
		CBCGPChartAxis::GetDisplayedLabel(dblValue, strLabel);
		break;

	case CBCGPChartTernaryAxis::LM_2080:
		strLabel.Format(_T("%.3G/%.3G"), dblValue, 100. - dblValue);
		break;

	case CBCGPChartTernaryAxis::LM_BASE_1:
		strLabel.Format(_T("%.3G"), dblValue / 100);
		break;
	}	
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::SetLabelMode(CBCGPChartTernaryAxis::LabelMode lm, BOOL bRedraw)
{
	if (lm != m_labelMode)
	{
		m_labelMode = lm;

		if (m_pChart != NULL)
		{
			m_pChart->SetDirty(TRUE, bRedraw);
		}
	}
}
//----------------------------------------------------------------------------//
double CBCGPChartTernaryAxis::PointFromValue(double /*dblValue*/, BOOL /*bForceValueOnThickMark*/, BOOL /*bLogValue*/)
{
	ASSERT(FALSE);
	TRACE0("CBCGPChartTernaryAxis::PointFromValue is not implemented. Call PointFromChartData instead");

	return 0;
}
//----------------------------------------------------------------------------//
CBCGPPoint CBCGPChartTernaryAxis::PointFromChartData(const CBCGPChartData& data)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL)
	{
		return CBCGPPoint();
	}

	double dblRange = 100.;

	double dblA = fabs(data.GetValue(CBCGPChartData::CI_Y));
	double dblB = fabs(data.GetValue(CBCGPChartData::CI_Y1));
	double dblC = fabs(data.GetValue(CBCGPChartData::CI_Y2));

	double dblSum = dblA + dblB + dblC;

	if (dblSum == 0)
	{
		return CBCGPPoint();
	}

	double dblAPerc = dblA / dblSum * dblRange;
	double dblBPerc = dblB / dblSum * dblRange;
	
	double dblY = m_rectBounds.bottom - dblAPerc * m_rectBounds.Height() / dblRange;
	double dblX_B = dblBPerc * m_rectBounds.Width() / dblRange;

	CBCGPChartTernaryAxis* pAxisA = DYNAMIC_DOWNCAST(CBCGPChartTernaryAxis, m_pChart->GetChartAxis(BCGP_CHART_A_TERNARY_AXIS));
	ASSERT_VALID(pAxisA);
	
	double dblX = (dblY - pAxisA->m_dblBCoef - dblX_B * tan(bcg_deg2rad(60))) / pAxisA->m_dblACoef;

	return CBCGPPoint(dblX, dblY);
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL || !IsAxisVisible() || m_dblMajorUnit == 0.)
	{
		return;
	}
	
	if (m_majorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS || m_minorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS)
	{
		CBCGPChartData data;
		data.SetValue(0., CBCGPChartData::CI_Y);
		data.SetValue(0., CBCGPChartData::CI_Y1);
		data.SetValue(0., CBCGPChartData::CI_Y2);

		int nStep = 0;

		for (double dblCurValue = 0; dblCurValue <= m_dblMaximum;)
		{
			switch (m_axisDefaultPosition)
			{
			case CBCGPChartAxis::ADP_RIGHT:
				data.SetValue(dblCurValue, CBCGPChartData::CI_Y);
				data.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y2);
				break;
			case CBCGPChartAxis::ADP_LEFT:
				data.SetValue(dblCurValue, CBCGPChartData::CI_Y1);
				data.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y);
				break;
			case CBCGPChartAxis::ADP_BOTTOM:
				data.SetValue(dblCurValue, CBCGPChartData::CI_Y2);
				data.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y1);
				break;
			}

			CBCGPPoint pt = PointFromChartData(data);
			
			int nTickMarkLen = (nStep == 0 && m_majorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS) ? m_nMajorTickMarkLen : m_nMinorTickMarkLen;
			TickMarkType tmt = (nStep == 0 && m_majorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS) ? m_majorTickMarkType : m_minorTickMarkType;
	
			CBCGPPoint ptStart = pt;
			CBCGPPoint ptEnd = pt;

			switch (tmt)
			{
			case CBCGPChartAxis::TMT_INSIDE:
				ptStart.x -= nTickMarkLen * sin(m_dblTickAngle); 
				ptStart.y += nTickMarkLen * cos(m_dblTickAngle); 
				break;
			case CBCGPChartAxis::TMT_OUTSIDE:
				ptStart.x += nTickMarkLen * sin(m_dblTickAngle); 
				ptStart.y -= nTickMarkLen * cos(m_dblTickAngle); 
				break;
			case CBCGPChartAxis::TMT_CROSS:
				ptStart.x -= nTickMarkLen * sin(m_dblTickAngle); 
				ptStart.y += nTickMarkLen * cos(m_dblTickAngle); 
				ptEnd.x += nTickMarkLen * sin(m_dblTickAngle); 
				ptEnd.y -= nTickMarkLen * cos(m_dblTickAngle); 
				break;
			}

			m_pChart->OnDrawTickMark(pGM, ptStart, ptEnd, m_axisLineFormat, dblCurValue, nTickMarkLen == m_nMajorTickMarkLen);

			nStep++;
			dblCurValue += m_minorTickMarkType != CBCGPChartAxis::TMT_NO_TICKS ? m_dblMinorUnit : m_dblMajorUnit;

			if (nStep == m_nMinorUnitCount || m_minorTickMarkType == CBCGPChartAxis::TMT_NO_TICKS)
			{
				nStep = 0;
			}
		}
	}

	OnDrawAxisLine(pGM);
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::OnDrawMajorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL || !IsAxisVisible() || !m_bShowMajorGridLines || m_dblMajorUnit == 0.)
	{
		return;
	}

	for (double dblCurValue = 0; dblCurValue <= m_dblMaximum; dblCurValue += m_dblMajorUnit)
	{
		DrawGridLine(pGM, dblCurValue, TRUE);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::OnDrawMinorGridLines(CBCGPGraphicsManager* pGM, const CBCGPRect& /*rectDiagram*/)
{
	ASSERT_VALID(this);

	if (m_pChart == NULL || !IsAxisVisible() || !m_bShowMinorGridLines || m_dblMajorUnit == 0.)
	{
		return;
	}

	int nStep = 0;

	for (double dblCurValue = 0; dblCurValue <= m_dblMaximum; dblCurValue += m_dblMinorUnit)
	{
		if (nStep != 0 || !m_bShowMajorGridLines)
		{
			// always draw minor grid lines
			DrawGridLine(pGM, dblCurValue, FALSE);
		}

		nStep++;

		if (nStep == m_nMinorUnitCount || m_minorTickMarkType == CBCGPChartAxis::TMT_NO_TICKS)
		{
			nStep = 0;
		}
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::DrawGridLine(CBCGPGraphicsManager* pGM, double dblCurValue, BOOL bIsMajor)
{
	CBCGPChartData dataStart;
	dataStart.SetValue(0., CBCGPChartData::CI_Y);
	dataStart.SetValue(0., CBCGPChartData::CI_Y1);
	dataStart.SetValue(0., CBCGPChartData::CI_Y2);

	CBCGPChartData dataEnd;
	dataEnd.SetValue(0., CBCGPChartData::CI_Y);
	dataEnd.SetValue(0., CBCGPChartData::CI_Y1);
	dataEnd.SetValue(0., CBCGPChartData::CI_Y2);

	switch (m_axisDefaultPosition)
	{
	case CBCGPChartAxis::ADP_RIGHT:
		dataStart.SetValue(dblCurValue, CBCGPChartData::CI_Y);
		dataStart.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y2);

		dataEnd.SetValue(dblCurValue, CBCGPChartData::CI_Y);
		dataEnd.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y1);
		break;

	case CBCGPChartAxis::ADP_LEFT:
		dataStart.SetValue(dblCurValue, CBCGPChartData::CI_Y1);
		dataStart.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y);

		dataEnd.SetValue(dblCurValue, CBCGPChartData::CI_Y1);
		dataEnd.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y2);
		break;

	case CBCGPChartAxis::ADP_BOTTOM:
		dataStart.SetValue(dblCurValue, CBCGPChartData::CI_Y2);
		dataStart.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y1);

		dataEnd.SetValue(dblCurValue, CBCGPChartData::CI_Y2);
		dataEnd.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y);
		break;
	}

	CBCGPPoint ptStart = PointFromChartData(dataStart);
	CBCGPPoint ptEnd = PointFromChartData(dataEnd);

	m_pChart->OnDrawGridLine(pGM, ptStart, ptEnd, this, dblCurValue, bIsMajor ? m_majorGridLineFormat : m_minorGridLineFormat, bIsMajor);
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::OnDrawAxisLabels(CBCGPGraphicsManager* pGM, const CBCGPRect& rectDiagram)
{
	ASSERT_VALID(this);

	m_rectAxisLabels.SetRectEmpty();

	if (m_pChart == NULL || m_axisLabelType == CBCGPChartAxis::ALT_NO_LABELS || !IsAxisVisible() || m_dblMajorUnit == 0.)
	{
		return;
	}

	CBCGPChartData data;
	data.SetValue(0., CBCGPChartData::CI_Y);
	data.SetValue(0., CBCGPChartData::CI_Y1);
	data.SetValue(0., CBCGPChartData::CI_Y2);

	double dblDistance = GetLabelDistance();

	for (double dblCurValue = 0; dblCurValue <= m_dblMaximum; dblCurValue += m_dblMajorUnit)
	{
		switch (m_axisDefaultPosition)
		{
		case CBCGPChartAxis::ADP_RIGHT:
			data.SetValue(dblCurValue, CBCGPChartData::CI_Y);
			data.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y2);
			break;
		case CBCGPChartAxis::ADP_LEFT:
			data.SetValue(dblCurValue, CBCGPChartData::CI_Y1);
			data.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y);
			break;
		case CBCGPChartAxis::ADP_BOTTOM:
			data.SetValue(dblCurValue, CBCGPChartData::CI_Y2);
			data.SetValue(m_dblMaximum - dblCurValue, CBCGPChartData::CI_Y1);
			break;
		}

		CString strLabel;
		GetDisplayedLabel(dblCurValue, strLabel);

		CBCGPPoint pt = PointFromChartData(data);


		pt.x += (dblDistance + m_szMaxLabelSize.cx / 2) * sin(m_dblTickAngle);
		pt.y -= (dblDistance + m_szMaxLabelSize.cy / 2) * cos(m_dblTickAngle);

		CBCGPRect rectLabel(pt, pt);
		rectLabel.InflateRect(m_szMaxLabelSize.cx / 2, m_szMaxLabelSize.cy / 2);
		
		m_pChart->OnDrawAxisLabel(pGM, dblCurValue, strLabel, this, rectLabel, m_rectAxisLabels, rectDiagram);

		m_rectAxisLabels.UnionRect(m_rectAxisLabels, rectLabel);
	}
}
//----------------------------------------------------------------------------//
void CBCGPChartTernaryAxis::OnFillUnitIntervals(CBCGPGraphicsManager* /*pGM*/, const CBCGPRect& /*rectDiagram*/)
{

}