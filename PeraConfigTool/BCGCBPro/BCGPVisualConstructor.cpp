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
// BCGPVisualConstructor.cpp: implementation of the CBCGPRibbonConstrucor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPVisualConstructor.h"
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

CBCGPVisualConstructor::CBCGPVisualConstructor(const CBCGPVisualInfo& info)
	: m_Info(info)
{
}

CBCGPVisualConstructor::~CBCGPVisualConstructor()
{
}

void CBCGPVisualConstructor::Construct (CBCGPVisualContainer& container) const
{
	const CBCGPVisualInfo::XContainer& infoContainer = GetInfo ().GetContainer ();

	for (int i = 0; i < infoContainer.m_arElements.GetSize (); i++)
	{
		CBCGPBaseVisualObject* pElement = CreateElement ((const CBCGPVisualInfo::XElement&)*infoContainer.m_arElements[i]);
		if (pElement != NULL)
		{
			ASSERT_VALID (pElement);

			container.Add(pElement);
		}
	}	

	if (!infoContainer.m_Rect.IsRectEmpty ())
	{
		container.SetRect (infoContainer.m_Rect, TRUE, FALSE);
	}

	container.SetFillBrush (infoContainer.m_brFill);
	container.SetOutlineBrush (infoContainer.m_brOutline);
}

CBCGPBaseVisualObject* CBCGPVisualConstructor::CreateElement (const CBCGPVisualInfo::XElement& info) const
{
	CBCGPBaseVisualObject* pElement = NULL;

	pElement = CreateGaugeElement((const CBCGPVisualInfo::XGaugeElement&)info);

	return pElement;
}

CBCGPGaugeImpl* CBCGPVisualConstructor::CreateGaugeElement (const CBCGPVisualInfo::XGaugeElement& info) const
{
	CBCGPGaugeImpl* pElement = NULL;

	if (info.GetElementName ().Compare (CBCGPVisualInfo::s_szCircularGauge) == 0 ||
		info.GetElementName ().Compare (CBCGPVisualInfo::s_szKnob) == 0 ||
		info.GetElementName ().Compare (CBCGPVisualInfo::s_szAnalogClock) == 0)
	{
		BOOL bKnob = info.GetElementName ().Compare (CBCGPVisualInfo::s_szKnob) == 0;
		BOOL bAnalogClock = info.GetElementName ().Compare (CBCGPVisualInfo::s_szAnalogClock) == 0;

		const CBCGPVisualInfo::XElementCircular& infoElement = 
			(const CBCGPVisualInfo::XElementCircular&)info;

		CBCGPCircularGaugeImpl* pNewElement = NULL;
		if (bKnob)
		{
			pNewElement = new CBCGPKnob;
		}
		else if (bAnalogClock)
		{
			pNewElement = new CBCGPAnalogClock;
		}
		else
		{
			pNewElement = new CBCGPCircularGaugeImpl;
		}

		pElement = pNewElement;

		ConstructGaugeElement (*pElement, info);

		pNewElement->SetColors (infoElement.m_Colors);

		if (!infoElement.m_fmtText.IsEmpty())
		{
			pNewElement->SetTextFormat (infoElement.m_fmtText);
		}

		pNewElement->SetCapSize (infoElement.m_dblCapSize);

		if (!bKnob && !bAnalogClock)
		{
			pNewElement->EnableShapeByTicksArea (infoElement.m_bShapeByTicksArea);
		}

		int i = 0;
		for (i = 0; i < infoElement.m_arScales.GetSize (); i++)
		{
			const CBCGPVisualInfo::XCircularScale* pScaleInfo = 
				(const CBCGPVisualInfo::XCircularScale*)infoElement.m_arScales[i];

			int index = 0;
			if (i != 0)
			{
				index = pNewElement->AddScale ();
			}

			if (pScaleInfo->m_bIsClosed)
			{
				pNewElement->SetClosedRange (pScaleInfo->m_dblStart, pScaleInfo->m_dblFinish, pScaleInfo->m_dblStartAngle, 
					pScaleInfo->m_bDrawLastTickMark, pScaleInfo->m_bAnimationThroughStart, 
					index);
			}
			else
			{
				pNewElement->SetRange (pScaleInfo->m_dblStart, pScaleInfo->m_dblFinish, index);
				pNewElement->SetTicksAreaAngles (pScaleInfo->m_dblStartAngle, pScaleInfo->m_dblFinishAngle, index);
			}
			pNewElement->SetStep (pScaleInfo->m_dblStep, index);
			pNewElement->SetTextLabelFormat (pScaleInfo->m_strLabelFormat, index);
			pNewElement->SetTickMarkStyle (pScaleInfo->m_MinorTickMarkStyle, FALSE, pScaleInfo->m_dblMinorTickMarkSize, index);
			pNewElement->SetTickMarkStyle (pScaleInfo->m_MajorTickMarkStyle, TRUE, pScaleInfo->m_dblMajorTickMarkSize, index);
			pNewElement->SetMajorTickMarkStep (pScaleInfo->m_dblMajorTickMarkStep, index);
			pNewElement->SetScaleFillBrush (pScaleInfo->m_brFill, index);
			pNewElement->SetScaleOutlineBrush (pScaleInfo->m_brOutline, index);
			pNewElement->SetScaleTextBrush (pScaleInfo->m_brText, index);
			pNewElement->SetScaleTickMarkBrush (pScaleInfo->m_brTickMarkMinor, FALSE, index);
			pNewElement->SetScaleTickMarkBrush (pScaleInfo->m_brTickMarkMajor, TRUE, index);
			pNewElement->SetScaleOffsetFromFrame (pScaleInfo->m_dblOffsetFromFrame, index);
			pNewElement->EnableLabelsRotation (pScaleInfo->m_bRotateLabels, index);
		}

		if (bKnob)
		{
			if (infoElement.m_arPointers.GetSize () > 0)
			{
				const CBCGPVisualInfo::XKnobPointer* pPointerInfo = 
					(const CBCGPVisualInfo::XKnobPointer*)infoElement.m_arPointers[0];

				CBCGPKnobPointer pointer(pPointerInfo->m_brFill, 
						pPointerInfo->m_brOutline, pPointerInfo->m_Style, pPointerInfo->m_dblOffsetFromCenter, 
						pPointerInfo->m_dblWidth);

				((CBCGPKnob*)pNewElement)->SetPointer (pointer, FALSE);
				pNewElement->SetValue (pPointerInfo->m_dblValue, 0, 0, FALSE);
			}
		}
		else
		{
			int nDefaultPointers = 1;
			if (bAnalogClock)
			{
				nDefaultPointers = (int)infoElement.m_arPointers.GetSize ();
				if (nDefaultPointers == 3)
				{
					((CBCGPAnalogClock*)pNewElement)->EnableSecondHand ();
				}
			}

			for (i = 0; i < infoElement.m_arPointers.GetSize (); i++)
			{
				const CBCGPVisualInfo::XCircularPointer* pPointerInfo = 
					(const CBCGPVisualInfo::XCircularPointer*)infoElement.m_arPointers[i];

				CBCGPCircularGaugePointer pointer(pPointerInfo->m_brFill, 
						pPointerInfo->m_brOutline, pPointerInfo->m_Style, pPointerInfo->m_dblSize, 
						pPointerInfo->m_dblWidth, pPointerInfo->m_bExtraLen);

				if (i >= nDefaultPointers)
				{
					pNewElement->AddPointer (pointer, pPointerInfo->m_nScale, FALSE);
				}
				else
				{
					pNewElement->ModifyPointer (i, pointer, FALSE);
				}

				if (!bAnalogClock)
				{
					pNewElement->SetValue (pPointerInfo->m_dblValue, i, 0, FALSE);
				}
			}
		}

		for (i = 0; i < infoElement.m_arRanges.GetSize (); i++)
		{
			const CBCGPVisualInfo::XCircularColoredRange* pRangeInfo = 
				(const CBCGPVisualInfo::XCircularColoredRange*)infoElement.m_arRanges[i];

			pNewElement->AddColoredRange (pRangeInfo->m_dblStartValue, pRangeInfo->m_dblFinishValue, pRangeInfo->m_brFill,
				pRangeInfo->m_brOutline, pRangeInfo->m_nScale, pRangeInfo->m_dblStartWidth, pRangeInfo->m_dblFinishWidth, 
				pRangeInfo->m_dblOffsetFromFrame, pRangeInfo->m_brTextLabel, pRangeInfo->m_brTickMarkOutline, pRangeInfo->m_brTickMarkFill, FALSE);
		}

		for (i = 0; i < infoElement.m_arSubGauges.GetSize (); i++)
		{
			CBCGPVisualInfo::XGaugeElement* pSubGaugeInfo = infoElement.m_arSubGauges[i];
			CBCGPGaugeImpl* pSubGauge = CreateGaugeElement (*pSubGaugeInfo);
			if (pSubGauge != NULL)
			{
				pNewElement->AddSubGauge (pSubGauge, pSubGaugeInfo->m_Pos, pSubGaugeInfo->m_Rect.Size (), pSubGaugeInfo->m_ptOffset);
			}
		}
	}
	else if (info.GetElementName ().Compare (CBCGPVisualInfo::s_szLinearGauge) == 0)
	{
		const CBCGPVisualInfo::XElementLinear& infoElement = 
			(const CBCGPVisualInfo::XElementLinear&)info;

		CBCGPLinearGaugeImpl* pNewElement = new CBCGPLinearGaugeImpl;

		pElement = pNewElement;

		ConstructGaugeElement (*pElement, info);

		pNewElement->SetColors (infoElement.m_Colors);
		if (!infoElement.m_fmtText.IsEmpty())
		{
			pNewElement->SetTextFormat (infoElement.m_fmtText);
		}
		pNewElement->SetVerticalOrientation (infoElement.m_bIsVertical);

		int i = 0;
		for (i = 0; i < infoElement.m_arScales.GetSize (); i++)
		{
			const CBCGPVisualInfo::XLinearScale* pScaleInfo = 
				(const CBCGPVisualInfo::XLinearScale*)infoElement.m_arScales[i];

			int index = 0;
			if (i != 0)
			{
				index = pNewElement->AddScale ();
			}

			pNewElement->SetRange (pScaleInfo->m_dblStart, pScaleInfo->m_dblFinish, index);
			pNewElement->SetStep (pScaleInfo->m_dblStep, index);
			pNewElement->SetTextLabelFormat (pScaleInfo->m_strLabelFormat, index);
			pNewElement->SetTickMarkStyle (pScaleInfo->m_MinorTickMarkStyle, FALSE, pScaleInfo->m_dblMinorTickMarkSize, index);
			pNewElement->SetTickMarkStyle (pScaleInfo->m_MajorTickMarkStyle, TRUE, pScaleInfo->m_dblMajorTickMarkSize, index);
			pNewElement->SetMajorTickMarkStep (pScaleInfo->m_dblMajorTickMarkStep, index);
			pNewElement->SetScaleFillBrush (pScaleInfo->m_brFill, index);
			pNewElement->SetScaleOutlineBrush (pScaleInfo->m_brOutline, index);
			pNewElement->SetScaleTextBrush (pScaleInfo->m_brText, index);
			pNewElement->SetScaleTickMarkBrush (pScaleInfo->m_brTickMarkMinor, FALSE, index);
			pNewElement->SetScaleTickMarkBrush (pScaleInfo->m_brTickMarkMajor, TRUE, index);
			pNewElement->SetScaleOffsetFromFrame (pScaleInfo->m_dblOffsetFromFrame, index);
		}

		for (i = 0; i < infoElement.m_arPointers.GetSize (); i++)
		{
			const CBCGPVisualInfo::XLinearPointer* pPointerInfo = 
				(const CBCGPVisualInfo::XLinearPointer*)infoElement.m_arPointers[i];

			CBCGPLinearGaugePointer pointer(pPointerInfo->m_brFill, 
					pPointerInfo->m_brOutline, pPointerInfo->m_Style, pPointerInfo->m_dblSize, 
					pPointerInfo->m_dblWidth);

			int index = pNewElement->AddPointer (pointer, pPointerInfo->m_nScale, FALSE);
			pNewElement->SetValue (pPointerInfo->m_dblValue, index, 0, FALSE);
		}

		for (i = 0; i < infoElement.m_arRanges.GetSize (); i++)
		{
			const CBCGPVisualInfo::XLinearColoredRange* pRangeInfo = 
				(const CBCGPVisualInfo::XLinearColoredRange*)infoElement.m_arRanges[i];

			pNewElement->AddColoredRange (pRangeInfo->m_dblStartValue, pRangeInfo->m_dblFinishValue, pRangeInfo->m_brFill,
				pRangeInfo->m_brOutline, pRangeInfo->m_nScale, pRangeInfo->m_dblStartWidth, pRangeInfo->m_dblFinishWidth, 
				pRangeInfo->m_dblOffsetFromFrame, pRangeInfo->m_brTextLabel, pRangeInfo->m_brTickMarkOutline, pRangeInfo->m_brTickMarkFill, FALSE);
		}
	}
	else if (info.GetElementName ().Compare (CBCGPVisualInfo::s_szNumericInd) == 0)
	{
		const CBCGPVisualInfo::XElementNumeric& infoElement = 
			(const CBCGPVisualInfo::XElementNumeric&)info;

		CBCGPNumericIndicatorImpl* pNewElement = new CBCGPNumericIndicatorImpl;

		pElement = pNewElement;

		ConstructGaugeElement (*pElement, info);

		pNewElement->SetStyle (infoElement.m_Style);
		pNewElement->SetColors (infoElement.m_Colors);
		if (!infoElement.m_fmtText.IsEmpty())
		{
			pNewElement->SetTextFormat (infoElement.m_fmtText);
		}
		pNewElement->SetCells (infoElement.m_nCells);
		pNewElement->SetDecimals (infoElement.m_nDecimals);
		pNewElement->SetSeparatorWidth (infoElement.m_nSeparatorWidth);
		pNewElement->SetDrawSign (infoElement.m_bDrawSign);
		pNewElement->SetDrawDecimalPoint (infoElement.m_bDrawDecimalPoint);
		pNewElement->SetDrawLeadingZeros (infoElement.m_bDrawLeadingZeros);
		pNewElement->SetValue (infoElement.m_dblValue);
	}
	else if (info.GetElementName ().Compare (CBCGPVisualInfo::s_szColorInd) == 0)
	{
		const CBCGPVisualInfo::XElementColor& infoElement = 
			(const CBCGPVisualInfo::XElementColor&)info;

		CBCGPColorIndicatorImpl* pNewElement = new CBCGPColorIndicatorImpl;

		pElement = pNewElement;

		ConstructGaugeElement (*pElement, info);

		pNewElement->SetColors (infoElement.m_Colors);
		pNewElement->SetStyle (infoElement.m_Style);
		pNewElement->SetStretched (infoElement.m_bStretched);
	}
	else if (info.GetElementName ().Compare (CBCGPVisualInfo::s_szTextInd) == 0)
	{
		const CBCGPVisualInfo::XElementText& infoElement = 
			(const CBCGPVisualInfo::XElementText&)info;

		CBCGPTextGaugeImpl* pNewElement = new CBCGPTextGaugeImpl;

		pElement = pNewElement;

		ConstructGaugeElement (*pElement, info);

		pNewElement->SetTextColor (infoElement.m_clrText);
		if (!infoElement.m_fmtText.IsEmpty())
		{
			pNewElement->SetTextFormat (infoElement.m_fmtText);
		}
		pNewElement->SetText (infoElement.m_strText, infoElement.m_clrText);
	}

	return pElement;
}

void CBCGPVisualConstructor::ConstructGaugeElement (CBCGPGaugeImpl& element, const CBCGPVisualInfo::XGaugeElement& info) const
{
	element.SetName (info.m_ID.m_Name);
	element.SetID (info.m_ID.m_Value);
	element.SetRect (info.m_Rect);
	element.SetVisible (info.m_bIsVisible);
	element.SetAutoDestroy(info.m_bIsAutoDestroy);
	element.SetInteractiveMode (info.m_bIsInteractiveMode);
	element.SetToolTip (info.m_strToolTip, info.m_strDescription);

	element.SetFrameSize (info.m_nFrameSize);
}
