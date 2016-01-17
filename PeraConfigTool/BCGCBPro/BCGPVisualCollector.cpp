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
//
// BCGPVisualCollector.cpp: implementation of the CBCGPRibbonCollector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPVisualCollector.h"
#include "BCGPVisualContainer.h"
#include "BCGPTextGaugeImpl.h"
#include "BCGPAnalogClock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPVisualCollector::CBCGPVisualCollector(CBCGPVisualInfo& info)
	: m_Info (info)
{
}

CBCGPVisualCollector::~CBCGPVisualCollector()
{
}

void CBCGPVisualCollector::Collect(const CBCGPVisualContainer& container)
{
	CollectContainer (container, GetInfo ().GetContainer ());
}

void CBCGPVisualCollector::CollectContainer(const CBCGPVisualContainer& container, CBCGPVisualInfo::XContainer& info)
{
	info.m_Rect      = container.GetRect ();
	info.m_brFill    = container.GetFillBrush ();
	info.m_brOutline = container.GetOutlineBrush ();

	CPoint ptOffset (0, 0);

	if (!info.m_Rect.IsRectEmpty ())
	{
		ptOffset = info.m_Rect.TopLeft ();
	}

	for (int i = 0; i < container.GetCount (); i++)
	{
		const CBCGPBaseVisualObject* pElement = container[i];
		ASSERT_VALID (pElement);

		if (pElement != NULL)
		{
			CBCGPVisualInfo::XElement* pElementInfo = CollectElement (*pElement);
			if (pElementInfo != NULL)
			{
				pElementInfo->m_Rect.OffsetRect (-ptOffset);
				info.m_arElements.Add (pElementInfo);
			}
		}
	}
}

CBCGPVisualInfo::XElement* CBCGPVisualCollector::CollectElement(const CBCGPBaseVisualObject& element)
{
	CBCGPVisualInfo::XElement* info = NULL;

	if (element.IsKindOf (RUNTIME_CLASS (CBCGPGaugeImpl)))
	{
		info = CollectGaugeElement ((const CBCGPGaugeImpl&)element);
	}

	return info;
}

CBCGPVisualInfo::XGaugeElement* CBCGPVisualCollector::CollectGaugeElement(const CBCGPGaugeImpl& element)
{
	CBCGPVisualInfo::XGaugeElement* info = NULL;

	if (element.IsKindOf (RUNTIME_CLASS (CBCGPCircularGaugeImpl)))
	{
		BOOL bKnob = element.IsKindOf (RUNTIME_CLASS (CBCGPKnob));
		BOOL bAnalogClock = element.IsKindOf (RUNTIME_CLASS (CBCGPAnalogClock));

		CBCGPVisualInfo::XElementCircular* pNewInfo = NULL;
		if (bKnob)
		{
			pNewInfo = new CBCGPVisualInfo::XElementKnob;
		}
		else if (bAnalogClock)
		{
			pNewInfo = new CBCGPVisualInfo::XElementAnalogClock;
		}
		else
		{
			pNewInfo = new CBCGPVisualInfo::XElementCircular;
		}
		info = pNewInfo;

		CollectBaseGaugeElement (element, *info);

		CBCGPCircularGaugeImpl* pElement = (CBCGPCircularGaugeImpl*)&element;
		ASSERT_VALID (pElement);

		pNewInfo->m_Colors             = pElement->GetColors ();
		pNewInfo->m_fmtText            = pElement->GetTextFormat ();
		pNewInfo->m_dblCapSize         = pElement->GetCapSize ();

		if (!bKnob)
		{
			pNewInfo->m_bShapeByTicksArea  = pElement->IsShapeByTicksArea ();
		}

		int i = 0;
		for (i = 0; i < element.GetScalesCount (); i++)
		{
			CBCGPVisualInfo::XCircularScale* pScaleInfo = new CBCGPVisualInfo::XCircularScale;

			pScaleInfo->m_dblStart               = pElement->GetStart (i);
			pScaleInfo->m_dblFinish              = pElement->GetFinish (i);
			pScaleInfo->m_dblStep                = pElement->GetStep (i);
			pScaleInfo->m_strLabelFormat         = pElement->GetTextLabelFormat (i);
			pScaleInfo->m_dblMinorTickMarkSize   = pElement->GetTickMarkSize (FALSE, i);
			pScaleInfo->m_dblMajorTickMarkSize   = pElement->GetTickMarkSize (TRUE, i);
			pScaleInfo->m_dblMajorTickMarkStep   = pElement->GetMajorTickMarkStep (i);
			pScaleInfo->m_brFill                 = pElement->GetScaleFillBrush (i);
			pScaleInfo->m_brOutline              = pElement->GetScaleOutlineBrush (i);
			pScaleInfo->m_brText                 = pElement->GetScaleTextBrush (i);
			pScaleInfo->m_brTickMarkMinor        = pElement->GetScaleTickMarkBrush (FALSE, i);
			pScaleInfo->m_brTickMarkMajor        = pElement->GetScaleTickMarkBrush (TRUE, i);
			pScaleInfo->m_dblStartAngle          = pElement->GetTicksAreaStartAngle (i);
			pScaleInfo->m_dblFinishAngle         = pElement->GetTicksAreaFinishAngle (i);
			pScaleInfo->m_dblOffsetFromFrame     = pElement->GetScaleOffsetFromFrame (i);
			pScaleInfo->m_bRotateLabels          = pElement->IsLabelRotation (i);
			pScaleInfo->m_MinorTickMarkStyle     = pElement->GetTickMarkStyle (FALSE, i);
			pScaleInfo->m_MajorTickMarkStyle     = pElement->GetTickMarkStyle (TRUE, i);
			pScaleInfo->m_bIsClosed              = pElement->IsClosed (i);
			pScaleInfo->m_bDrawLastTickMark      = pElement->IsDrawLastTickMark (i);
			pScaleInfo->m_bAnimationThroughStart = pElement->IsAnimationThroughStart (i);

			pNewInfo->m_arScales.Add (pScaleInfo);
		}

		for (i = 0; i < pElement->GetDataCount (); i++)
		{
			const CBCGPCircularGaugePointer* pPointer = (const CBCGPCircularGaugePointer*)pElement->GetData (i);
			CBCGPVisualInfo::XCircularPointer* pPointerInfo = new CBCGPVisualInfo::XCircularPointer;

			pPointerInfo->m_nScale    = pPointer->GetScale ();
			if (!bAnalogClock)
			{
				pPointerInfo->m_dblValue  = pPointer->GetValue ();
			}
			pPointerInfo->m_brFill    = pPointer->GetFillBrush ();
			pPointerInfo->m_brOutline = pPointer->GetOutlineBrush ();
			pPointerInfo->m_dblSize   = pPointer->GetSize ();
			pPointerInfo->m_dblWidth  = pPointer->GetWidth ();
			pPointerInfo->m_bExtraLen = pPointer->IsExtraLen ();
			pPointerInfo->m_Style     = pPointer->GetStyle ();
			pNewInfo->m_arPointers.Add(pPointerInfo);
		}

		for (i = 0; i < pElement->GetColoredRangesCount (); i++)
		{
			CBCGPGaugeColoredRangeObject* pRange = (CBCGPGaugeColoredRangeObject*)pElement->GetColoredRange (i);
			CBCGPVisualInfo::XCircularColoredRange* pRangeInfo = new CBCGPVisualInfo::XCircularColoredRange;

			pRangeInfo->m_nScale              = pRange->GetScale ();
			pRangeInfo->m_dblStartValue       = pRange->GetStartValue ();
			pRangeInfo->m_dblFinishValue      = pRange->GetFinishValue ();
			pRangeInfo->m_brFill              = pRange->GetFillBrush ();
			pRangeInfo->m_brOutline           = pRange->GetOutlineBrush ();
			pRangeInfo->m_brTickMarkFill      = pRange->GetTickMarkFillBrush ();
			pRangeInfo->m_brTickMarkOutline   = pRange->GetTickMarkOutlineBrush ();
			pRangeInfo->m_brTextLabel         = pRange->GetTextLabelBrush ();
			pRangeInfo->m_dblStartWidth       = pRange->GetStartWidth ();
			pRangeInfo->m_dblFinishWidth      = pRange->GetFinishWidth ();
			pRangeInfo->m_dblOffsetFromFrame  = pRange->GetOffsetFromFrame ();
			pNewInfo->m_arRanges.Add(pRangeInfo);
		}

		const CArray<CBCGPGaugeImpl*, CBCGPGaugeImpl*>& arSubGauges = pElement->GetSubGauges ();
		for (i = 0; i < arSubGauges.GetSize (); i++)
		{
			CBCGPGaugeImpl* pSubGauge = arSubGauges[i];
			if (pSubGauge != NULL)
			{
				CBCGPVisualInfo::XGaugeElement* pSubGaugeInfo = CollectGaugeElement(*pSubGauge);
				if (pSubGaugeInfo != NULL)
				{
					pNewInfo->m_arSubGauges.Add (pSubGaugeInfo);
				}
			}
		}
	}
	else if (element.IsKindOf (RUNTIME_CLASS (CBCGPLinearGaugeImpl)))
	{
		CBCGPVisualInfo::XElementLinear* pNewInfo = new CBCGPVisualInfo::XElementLinear;
		info = pNewInfo;

		CollectBaseGaugeElement (element, *info);

		CBCGPLinearGaugeImpl* pElement = (CBCGPLinearGaugeImpl*)&element;
		ASSERT_VALID (pElement);

		pNewInfo->m_Colors      = pElement->GetColors ();
		pNewInfo->m_fmtText     = pElement->GetTextFormat ();
		pNewInfo->m_bIsVertical = pElement->IsVerticalOrientation ();

		int i = 0;
		for (i = 0; i < element.GetScalesCount (); i++)
		{
			CBCGPVisualInfo::XLinearScale* pScaleInfo = new CBCGPVisualInfo::XLinearScale;

			pScaleInfo->m_dblStart               = pElement->GetStart (i);
			pScaleInfo->m_dblFinish              = pElement->GetFinish (i);
			pScaleInfo->m_dblStep                = pElement->GetStep (i);
			pScaleInfo->m_strLabelFormat         = pElement->GetTextLabelFormat (i);
			pScaleInfo->m_dblMinorTickMarkSize   = pElement->GetTickMarkSize (FALSE, i);
			pScaleInfo->m_dblMajorTickMarkSize   = pElement->GetTickMarkSize (TRUE, i);
			pScaleInfo->m_dblMajorTickMarkStep   = pElement->GetMajorTickMarkStep (i);
			pScaleInfo->m_brFill                 = pElement->GetScaleFillBrush (i);
			pScaleInfo->m_brOutline              = pElement->GetScaleOutlineBrush (i);
			pScaleInfo->m_brText                 = pElement->GetScaleTextBrush (i);
			pScaleInfo->m_brTickMarkMinor        = pElement->GetScaleTickMarkBrush (FALSE, i);
			pScaleInfo->m_brTickMarkMajor        = pElement->GetScaleTickMarkBrush (TRUE, i);
			pScaleInfo->m_dblOffsetFromFrame     = pElement->GetScaleOffsetFromFrame (i);
			pScaleInfo->m_MinorTickMarkStyle     = pElement->GetTickMarkStyle (FALSE, i);
			pScaleInfo->m_MajorTickMarkStyle     = pElement->GetTickMarkStyle (TRUE, i);

			pNewInfo->m_arScales.Add (pScaleInfo);
		}

		for (i = 0; i < pElement->GetDataCount (); i++)
		{
			const CBCGPLinearGaugePointer* pPointer = (const CBCGPLinearGaugePointer*)pElement->GetData (i);
			CBCGPVisualInfo::XLinearPointer* pPointerInfo = new CBCGPVisualInfo::XLinearPointer;

			pPointerInfo->m_nScale    = pPointer->GetScale ();
			pPointerInfo->m_dblValue  = pPointer->GetValue ();
			pPointerInfo->m_brFill    = pPointer->GetFillBrush ();
			pPointerInfo->m_brOutline = pPointer->GetOutlineBrush ();
			pPointerInfo->m_dblSize   = pPointer->GetSize ();
			pPointerInfo->m_dblWidth  = pPointer->GetWidth ();
			pPointerInfo->m_Style     = pPointer->GetStyle ();
			pNewInfo->m_arPointers.Add(pPointerInfo);
		}

		for (i = 0; i < pElement->GetColoredRangesCount (); i++)
		{
			CBCGPGaugeColoredRangeObject* pRange = (CBCGPGaugeColoredRangeObject*)pElement->GetColoredRange (i);
			CBCGPVisualInfo::XLinearColoredRange* pRangeInfo = new CBCGPVisualInfo::XLinearColoredRange;

			pRangeInfo->m_nScale              = pRange->GetScale ();
			pRangeInfo->m_dblStartValue       = pRange->GetStartValue ();
			pRangeInfo->m_dblFinishValue      = pRange->GetFinishValue ();
			pRangeInfo->m_brFill              = pRange->GetFillBrush ();
			pRangeInfo->m_brOutline           = pRange->GetOutlineBrush ();
			pRangeInfo->m_brTickMarkFill      = pRange->GetTickMarkFillBrush ();
			pRangeInfo->m_brTickMarkOutline   = pRange->GetTickMarkOutlineBrush ();
			pRangeInfo->m_brTextLabel         = pRange->GetTextLabelBrush ();
			pRangeInfo->m_dblStartWidth       = pRange->GetStartWidth ();
			pRangeInfo->m_dblFinishWidth      = pRange->GetFinishWidth ();
			pRangeInfo->m_dblOffsetFromFrame  = pRange->GetOffsetFromFrame ();
			pNewInfo->m_arRanges.Add(pRangeInfo);
		}
	}
	else if (element.IsKindOf (RUNTIME_CLASS (CBCGPNumericIndicatorImpl)))
	{
		CBCGPVisualInfo::XElementNumeric* pNewInfo = new CBCGPVisualInfo::XElementNumeric;
		info = pNewInfo;

		CollectBaseGaugeElement (element, *info);

		const CBCGPNumericIndicatorImpl* pElement = (const CBCGPNumericIndicatorImpl*)&element;
		ASSERT_VALID (pElement);

		pNewInfo->m_Colors            = pElement->GetColors ();
		pNewInfo->m_fmtText           = pElement->GetTextFormat ();
		pNewInfo->m_Style             = pElement->GetStyle ();
		pNewInfo->m_nCells            = pElement->GetCells ();
		pNewInfo->m_nDecimals         = pElement->GetDecimals ();
		pNewInfo->m_nSeparatorWidth   = pElement->GetSeparatorWidth ();
		pNewInfo->m_bDrawSign         = pElement->IsDrawSign ();
		pNewInfo->m_bDrawDecimalPoint = pElement->IsDrawDecimalPoint ();
		pNewInfo->m_bDrawLeadingZeros = pElement->IsDrawLeadingZeros ();
		pNewInfo->m_dblValue          = pElement->GetValue ();
	}
	else if (element.IsKindOf (RUNTIME_CLASS (CBCGPColorIndicatorImpl)))
	{
		CBCGPVisualInfo::XElementColor* pNewInfo = new CBCGPVisualInfo::XElementColor;
		info = pNewInfo;

		CollectBaseGaugeElement (element, *info);

		const CBCGPColorIndicatorImpl* pElement = (const CBCGPColorIndicatorImpl*)&element;
		ASSERT_VALID (pElement);

		pNewInfo->m_Colors     = pElement->GetColors ();
		pNewInfo->m_Style      = pElement->GetStyle ();
		pNewInfo->m_bStretched = pElement->IsStretched ();
	}
	else if (element.IsKindOf (RUNTIME_CLASS (CBCGPTextGaugeImpl)))
	{
		CBCGPVisualInfo::XElementText* pNewInfo = new CBCGPVisualInfo::XElementText;
		info = pNewInfo;

		CollectBaseGaugeElement (element, *info);

		const CBCGPTextGaugeImpl* pElement = (const CBCGPTextGaugeImpl*)&element;
		ASSERT_VALID (pElement);

		pNewInfo->m_clrText = pElement->GetTextColor ();
		pNewInfo->m_fmtText = pElement->GetTextFormat ();
		pNewInfo->m_strText = pElement->GetText ();
	}

	return info;
}

void CBCGPVisualCollector::CollectBaseGaugeElement (const CBCGPGaugeImpl& element, CBCGPVisualInfo::XGaugeElement& info)
{
	info.m_ID.m_Name  = element.GetName ();
	info.m_ID.m_Value = element.GetID ();
	info.m_nFrameSize = element.GetFrameSize ();
	info.m_bIsVisible = element.IsVisible ();
	info.m_bIsAutoDestroy = element.IsAutoDestroy ();
	info.m_bIsInteractiveMode = element.IsInteractiveMode ();
	info.m_strToolTip = element.GetToolTip ();
	info.m_strDescription = element.GetDescription ();

	info.m_Pos        = element.GetPos ();
	info.m_ptOffset   = element.GetOffset ();
	info.m_Rect       = element.GetRect ();
}
