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
// BCGPVisualInfo.cpp: implementation of the CBCGPVisualInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPVisualInfo.h"
#include "BCGPTagManager.h"
#include "BCGPMath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const DWORD c_dwVersion = MAKELONG (1, 0);

static LPCTSTR s_szTag_Body                   = _T("BCGP_CONTAINER");

static LPCTSTR s_szTag_ToolTip                = _T("TOOLTIP");
static LPCTSTR s_szTag_Description            = _T("DESCRIPTION");
static LPCTSTR s_szTag_Rect                   = _T("RECT");
static LPCTSTR s_szTag_FrameSize              = _T("FRAME_SIZE");
static LPCTSTR s_szTag_LabelFormat            = _T("LABEL_FORMAT");
static LPCTSTR s_szTag_Visible                = _T("VISIBLE");
static LPCTSTR s_szTag_AutoDestroy            = _T("AUTO_DESTROY");
static LPCTSTR s_szTag_InteractiveMode        = _T("INTERACTIVE");

static LPCTSTR s_szTag_Scales                 = _T("SCALES");
static LPCTSTR s_szTag_Scale                  = _T("SCALE");
static LPCTSTR s_szTag_Scale_Id               = _T("SCALE_ID");
static LPCTSTR s_szTag_Pointers               = _T("POINTERS");
static LPCTSTR s_szTag_Pointer                = _T("POINTER");
static LPCTSTR s_szTag_Ranges                 = _T("RANGES");
static LPCTSTR s_szTag_Range                  = _T("RANGE");
static LPCTSTR s_szTag_Start                  = _T("START");
static LPCTSTR s_szTag_Finish                 = _T("FINISH");
static LPCTSTR s_szTag_Start_Angle            = _T("START_ANGLE");
static LPCTSTR s_szTag_Finish_Angle           = _T("FINISH_ANGLE");
static LPCTSTR s_szTag_Start_Value            = _T("START_VALUE");
static LPCTSTR s_szTag_Finish_Value           = _T("FINISH_VALUE");
static LPCTSTR s_szTag_Start_Width            = _T("START_WIDTH");
static LPCTSTR s_szTag_Finish_Width           = _T("FINISH_WIDTH");
static LPCTSTR s_szTag_Step                   = _T("STEP");
static LPCTSTR s_szTag_DataValue              = _T("DATA_VALUE");
static LPCTSTR s_szTag_Size                   = _T("SIZE");
static LPCTSTR s_szTag_Width                  = _T("WIDTH");
static LPCTSTR s_szTag_ExtraLen               = _T("EXTRALEN");
static LPCTSTR s_szTag_PointerStyle           = _T("STYLE");
static LPCTSTR s_szTag_Pos                    = _T("POS");
static LPCTSTR s_szTag_Offset                 = _T("OFFSET");
static LPCTSTR s_szTag_Offset_Center          = _T("OFFSET_CENTER");

static LPCTSTR s_szTag_Container              = _T("CONTAINER");
static LPCTSTR s_szTag_Element                = _T("ELEMENT");
static LPCTSTR s_szTag_Elements               = _T("ELEMENTS");

static LPCTSTR s_szTag_Colors                 = _T("COLORS");
static LPCTSTR s_szTag_Frame_Outline          = _T("FRAME_OUTLINE");
static LPCTSTR s_szTag_Frame_Brush            = _T("FRAME_BRUSH");
static LPCTSTR s_szTag_Frame_BrushInv         = _T("FRAME_BRUSH_INV");
static LPCTSTR s_szTag_Scale_Outline          = _T("SCALE_OUTLINE");
static LPCTSTR s_szTag_Scale_Brush            = _T("SCALE_BRUSH");
static LPCTSTR s_szTag_Fill_Outline           = _T("FILL_OUTLINE");
static LPCTSTR s_szTag_Fill_Brush             = _T("FILL_BRUSH");
static LPCTSTR s_szTag_Pointer_Outline        = _T("POINTER_OUTLINE");
static LPCTSTR s_szTag_Pointer_Brush          = _T("POINTER_BRUSH");
static LPCTSTR s_szTag_Cap_Outline            = _T("CAP_OUTLINE");
static LPCTSTR s_szTag_Cap_Brush              = _T("CAP_BRUSH");
static LPCTSTR s_szTag_Text_Brush             = _T("TEXT_BRUSH");
static LPCTSTR s_szTag_TickMark_Outline       = _T("TICKMARK_OUTLINE");
static LPCTSTR s_szTag_TickMark_Brush         = _T("TICKMARK_BRUSH");
static LPCTSTR s_szTag_TickMarkMinor_Brush    = _T("TICKMARK_MINOR_BRUSH");
static LPCTSTR s_szTag_TickMarkMajor_Brush    = _T("TICKMARK_MAJOR_BRUSH");
static LPCTSTR s_szTag_Separator_Brush        = _T("SEPARATOR_BRUSH");
static LPCTSTR s_szTag_Digit_Brush            = _T("DIGIT_BRUSH");
static LPCTSTR s_szTag_Decimal_Brush          = _T("DECIMAL_BRUSH");
static LPCTSTR s_szTag_Sign_Brush             = _T("SIGN_BRUSH");
static LPCTSTR s_szTag_Dot_Brush              = _T("DOT_BRUSH");

static LPCTSTR s_szTag_MajorTickMarkStep      = _T("MAJOR_TICKMARK_STEP");
static LPCTSTR s_szTag_MinorTickMarkSize      = _T("MINOR_TICKMARK_SIZE");
static LPCTSTR s_szTag_MajorTickMarkSize      = _T("MAJOR_TICKMARK_SIZE");
static LPCTSTR s_szTag_MinorTickMarkStyle     = _T("MINOR_TICKMARK_STYLE");
static LPCTSTR s_szTag_MajorTickMarkStyle     = _T("MAJOR_TICKMARK_STYLE");
static LPCTSTR s_szTag_CapSize                = _T("CAP_SIZE");
static LPCTSTR s_szTag_Shape_Area             = _T("SHAPE_AREA");
static LPCTSTR s_szTag_Offset_Frame           = _T("OFFSET_FRAME");
static LPCTSTR s_szTag_Closed                 = _T("CLOSED");
static LPCTSTR s_szTag_RotateLabels           = _T("ROTATE_LABELS");
static LPCTSTR s_szTag_DrawLastTickMark       = _T("DRAW_LAST_TICKMARK");
static LPCTSTR s_szTag_AnimationThroughStart  = _T("ANIMATION_THROUGH_START");
static LPCTSTR s_szTag_Vertical               = _T("VERTICAL");

static LPCTSTR s_szTag_Style                  = _T("STYLE");
static LPCTSTR s_szTag_Cells                  = _T("CELLS");
static LPCTSTR s_szTag_Decimals               = _T("DECIMALS");
static LPCTSTR s_szTag_SeparatorWidth         = _T("SEPARATOR_WIDTH");
static LPCTSTR s_szTag_DrawSign               = _T("SIGN");
static LPCTSTR s_szTag_DrawDecimalPoint       = _T("DECIMAL_POINT");
static LPCTSTR s_szTag_DrawLeadingZeros       = _T("LEADING_ZEROS");

static LPCTSTR s_szTag_TextFormat             = _T("TEXTFORMAT");
static LPCTSTR s_szTag_Stretched              = _T("STRETCHED");

LPCTSTR CBCGPVisualInfo::s_szCircularGauge    = _T("CircularGauge");
LPCTSTR CBCGPVisualInfo::s_szKnob             = _T("Knob");
LPCTSTR CBCGPVisualInfo::s_szAnalogClock      = _T("AnalogClock");
LPCTSTR CBCGPVisualInfo::s_szLinearGauge      = _T("LinearGauge");
LPCTSTR CBCGPVisualInfo::s_szNumericInd       = _T("NumericIndicator");
LPCTSTR CBCGPVisualInfo::s_szColorInd         = _T("ColorIndicator");
LPCTSTR CBCGPVisualInfo::s_szTextInd          = _T("TextIndicator");
LPCTSTR CBCGPVisualInfo::s_szContainer        = _T("Container");


CBCGPVisualInfo::XData::XData ()
	: m_dblValue(0.0)
{
}

CBCGPVisualInfo::XData::~XData ()
{
}

BOOL CBCGPVisualInfo::XData::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

 	tm.ReadDouble (s_szTag_DataValue, m_dblValue);

	return TRUE;
}

void CBCGPVisualInfo::XData::ToTag (CString& strTag) const
{
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_DataValue, m_dblValue, 0.0));
}

CBCGPVisualInfo::XElement::XElement(const CString& strElementName)
	: CBCGPBaseInfo::XBase (strElementName)
	, m_Rect               (0, 0, 0, 0)
	, m_bIsVisible         (TRUE)
	, m_bIsAutoDestroy     (TRUE)
{
}

CBCGPVisualInfo::XElement::~XElement()
{
}

BOOL CBCGPVisualInfo::XElement::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strID;
	if (tm.ExcludeTag (s_szTag_ID, strID))
	{
		m_ID.FromTag (strID);
	}

	tm.ReadRect (s_szTag_Rect, m_Rect);
	tm.ReadBool (s_szTag_Visible, m_bIsVisible);
	tm.ReadBool (s_szTag_AutoDestroy, m_bIsAutoDestroy);

	return TRUE;
}

void CBCGPVisualInfo::XElement::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CString strID;
	m_ID.ToTag (strID);
	CBCGPTagManager::WriteItem (strTag, s_szTag_ID, strID);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteRect (s_szTag_Rect, m_Rect, CRect (0, 0, 0, 0)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Visible, m_bIsVisible, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_AutoDestroy, m_bIsAutoDestroy, TRUE));
}

CBCGPVisualInfo::XContainer::XContainer()
	: CBCGPBaseInfo::XBase (CBCGPVisualInfo::s_szContainer)
	, m_Rect               (0, 0, 0, 0)
{
}

CBCGPVisualInfo::XContainer::~XContainer()
{
	for (int i = 0; i < m_arElements.GetSize (); i++)
	{
		if (m_arElements[i] != NULL)
		{
			delete m_arElements[i];
		}
	}
	m_arElements.RemoveAll ();
}

BOOL CBCGPVisualInfo::XContainer::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPVisualInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arElements.Add (pElement);
			}
		}
	}

	tm.ReadRect (s_szTag_Rect, m_Rect);
	tm.ReadBrush (s_szTag_Fill_Brush, m_brFill);
	tm.ReadBrush (s_szTag_Fill_Outline, m_brOutline);

	return TRUE;
}

void CBCGPVisualInfo::XContainer::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteRect (s_szTag_Rect, m_Rect, CRect (0, 0, 0, 0)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_brFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Outline, m_brOutline));

	CString strElements;
	for (int i = 0; i < m_arElements.GetSize (); i++)
	{
		CString strElement;
		m_arElements[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);
}

CBCGPBaseInfo::XBase* CBCGPVisualInfo::CreateBaseFromName (const CString& name)
{
	CBCGPVisualInfo::XElement* element = CBCGPVisualInfo::XGaugeElement::CreateFromName(name);

	return element;
}

CBCGPBaseInfo::XBase* CBCGPVisualInfo::CreateBaseFromTag (const CString& tag)
{
	return CBCGPBaseInfo::CreateBaseFromTag (tag, &CBCGPVisualInfo::CreateBaseFromName);
}

CBCGPVisualInfo::XGaugeElement* CBCGPVisualInfo::XGaugeElement::CreateFromName (const CString& name)
{
	CBCGPVisualInfo::XGaugeElement* element = NULL;

	if (name.Compare (CBCGPVisualInfo::s_szCircularGauge) == 0)
	{
		element = new CBCGPVisualInfo::XElementCircular;
	}
	else if (name.Compare (CBCGPVisualInfo::s_szKnob) == 0)
	{
		element = new CBCGPVisualInfo::XElementKnob;
	}
	else if (name.Compare (CBCGPVisualInfo::s_szAnalogClock) == 0)
	{
		element = new CBCGPVisualInfo::XElementAnalogClock;
	}
	else if (name.Compare (CBCGPVisualInfo::s_szLinearGauge) == 0)
	{
		element = new CBCGPVisualInfo::XElementLinear;
	}
	else if (name.Compare (CBCGPVisualInfo::s_szNumericInd) == 0)
	{
		element = new CBCGPVisualInfo::XElementNumeric;
	}
	else if (name.Compare (CBCGPVisualInfo::s_szColorInd) == 0)
	{
		element = new CBCGPVisualInfo::XElementColor;
	}
	else if (name.Compare (CBCGPVisualInfo::s_szTextInd) == 0)
	{
		element = new CBCGPVisualInfo::XElementText;
	}

	return element;
}

CBCGPVisualInfo::XGaugeScale::XGaugeScale ()
	: m_dblStart            (0.0)
	, m_dblFinish           (100.0)
	, m_dblStep             (5.0)
	, m_dblOffsetFromFrame  (5.0)
	, m_strLabelFormat      (_T("%.0f"))
	, m_dblMajorTickMarkStep(2.0)
	, m_dblMinorTickMarkSize(5.0)
	, m_dblMajorTickMarkSize(10.0)
	, m_MinorTickMarkStyle  (CBCGPCircularGaugeScale::BCGP_TICKMARK_LINE)
	, m_MajorTickMarkStyle  (CBCGPCircularGaugeScale::BCGP_TICKMARK_LINE)
{
}

CBCGPVisualInfo::XGaugeScale::~XGaugeScale ()
{
}

BOOL CBCGPVisualInfo::XGaugeScale::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadDouble (s_szTag_Start, m_dblStart);
	tm.ReadDouble (s_szTag_Finish, m_dblFinish);
	tm.ReadDouble (s_szTag_Step, m_dblStep);
	tm.ReadDouble (s_szTag_Offset_Frame, m_dblOffsetFromFrame);
	tm.ReadString (s_szTag_LabelFormat, m_strLabelFormat);
	CBCGPTagManager::Entity_FromTag (m_strLabelFormat);
	tm.ReadDouble (s_szTag_MajorTickMarkStep, m_dblMajorTickMarkStep);
	tm.ReadDouble (s_szTag_MinorTickMarkSize, m_dblMinorTickMarkSize);
	tm.ReadDouble (s_szTag_MajorTickMarkSize, m_dblMajorTickMarkSize);

	int style = (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_LINE;
	tm.ReadInt (s_szTag_MinorTickMarkStyle, style);
	m_MinorTickMarkStyle = (CBCGPGaugeScaleObject::BCGP_TICKMARK_STYLE)bcg_clamp(style, (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_LINE, (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_CIRCLE);

	style = (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_LINE;
	tm.ReadInt (s_szTag_MajorTickMarkStyle, style);
	m_MajorTickMarkStyle = (CBCGPGaugeScaleObject::BCGP_TICKMARK_STYLE)bcg_clamp(style, (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_LINE, (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_CIRCLE);

	tm.ReadBrush (s_szTag_Fill_Brush, m_brFill);
	tm.ReadBrush (s_szTag_Fill_Outline, m_brOutline);
	tm.ReadBrush (s_szTag_Text_Brush, m_brText);
	tm.ReadBrush (s_szTag_TickMarkMinor_Brush, m_brTickMarkMinor);
	tm.ReadBrush (s_szTag_TickMarkMajor_Brush, m_brTickMarkMajor);

	return TRUE;
}

void CBCGPVisualInfo::XGaugeScale::ToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Start, m_dblStart, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Finish, m_dblFinish, 100.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Step, m_dblStep, 5.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Offset_Frame, m_dblOffsetFromFrame, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_LabelFormat, CBCGPTagManager::Entity_ToTag (m_strLabelFormat)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_MajorTickMarkStep, m_dblMajorTickMarkStep, 2.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_MinorTickMarkSize, m_dblMinorTickMarkSize, 5.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_MajorTickMarkSize, m_dblMajorTickMarkSize, 10.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_MinorTickMarkStyle, (int)m_MinorTickMarkStyle, (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_LINE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_MajorTickMarkStyle, (int)m_MajorTickMarkStyle, (int)CBCGPGaugeScaleObject::BCGP_TICKMARK_LINE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_brFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Outline, m_brOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Text_Brush, m_brText));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMarkMinor_Brush, m_brTickMarkMinor));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMarkMajor_Brush, m_brTickMarkMajor));
}


CBCGPVisualInfo::XGaugeData::XGaugeData ()
	: CBCGPVisualInfo::XData()
	, m_nScale              (0)
{
}

CBCGPVisualInfo::XGaugeData::~XGaugeData ()
{
}

BOOL CBCGPVisualInfo::XGaugeData::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadInt (s_szTag_Scale_Id, m_nScale);
 	tm.ReadBrush (s_szTag_Fill_Brush, m_brFill);
 	tm.ReadBrush (s_szTag_Fill_Outline, m_brOutline);

	return XData::FromTag(strTag);
}

void CBCGPVisualInfo::XGaugeData::ToTag (CString& strTag) const
{
	XData::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Scale_Id, m_nScale, m_nScale + 1));
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_brFill));
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Outline, m_brOutline));
}


CBCGPVisualInfo::XGaugeColoredRange::XGaugeColoredRange()
	: m_nScale            (0)
	, m_dblStartValue     (0.0)
	, m_dblFinishValue    (0.0)
	, m_dblStartWidth     (0.0)
	, m_dblFinishWidth    (0.0)
	, m_dblOffsetFromFrame(0.0)
{
}

CBCGPVisualInfo::XGaugeColoredRange::~XGaugeColoredRange()
{
}

BOOL CBCGPVisualInfo::XGaugeColoredRange::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadInt (s_szTag_Scale_Id, m_nScale);
 	tm.ReadDouble (s_szTag_Start_Value, m_dblStartValue);
	tm.ReadDouble (s_szTag_Finish_Value, m_dblFinishValue);
 	tm.ReadDouble (s_szTag_Start_Width, m_dblStartWidth);
	tm.ReadDouble (s_szTag_Finish_Width, m_dblFinishWidth);
	tm.ReadDouble (s_szTag_Offset_Frame, m_dblOffsetFromFrame);
 	tm.ReadBrush (s_szTag_Fill_Brush, m_brFill);
 	tm.ReadBrush (s_szTag_Fill_Outline, m_brOutline);
	tm.ReadBrush (s_szTag_TickMark_Brush, m_brTickMarkFill);
	tm.ReadBrush (s_szTag_TickMark_Outline, m_brTickMarkOutline);
	tm.ReadBrush (s_szTag_Text_Brush, m_brTextLabel);

	return TRUE;
}

void CBCGPVisualInfo::XGaugeColoredRange::ToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Scale_Id, m_nScale, m_nScale + 1));
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Start_Value, m_dblStartValue, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Finish_Value, m_dblFinishValue, 0.0));
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Start_Width, m_dblStartWidth, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Finish_Width, m_dblFinishWidth, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Offset_Frame, m_dblOffsetFromFrame, 0.0));
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_brFill));
 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Outline, m_brOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMark_Brush, m_brTickMarkFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMark_Outline, m_brTickMarkOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Text_Brush, m_brTextLabel));
}


CBCGPVisualInfo::XGaugeElement::XGaugeElement(const CString& strElementName)
	: CBCGPVisualInfo::XElement(strElementName)
	, m_nFrameSize             (6)
	, m_Pos                    (CBCGPGaugeImpl::BCGP_SUB_GAUGE_NONE)
	, m_ptOffset               (0, 0)
	, m_bIsInteractiveMode     (FALSE)
{
}

CBCGPVisualInfo::XGaugeElement::~XGaugeElement()
{
	int i = 0;
	for (i = 0; i < m_arScales.GetSize (); i++)
	{
		if (m_arScales[i] != NULL)
		{
			delete m_arScales[i];
		}
	}
	m_arScales.RemoveAll ();

	for (i = 0; i < m_arRanges.GetSize (); i++)
	{
		if (m_arRanges[i] != NULL)
		{
			delete m_arRanges[i];
		}
	}
	m_arRanges.RemoveAll ();
}

CBCGPVisualInfo::XCircularScale::XCircularScale()
	: CBCGPVisualInfo::XGaugeScale()
	, m_dblStartAngle             (225.0)
	, m_dblFinishAngle            (-45.0)
	, m_bRotateLabels             (FALSE)
	, m_bIsClosed                 (FALSE)
	, m_bDrawLastTickMark         (TRUE)
	, m_bAnimationThroughStart    (TRUE)
{
}

CBCGPVisualInfo::XCircularScale::~XCircularScale()
{
}

BOOL CBCGPVisualInfo::XCircularScale::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBool (s_szTag_Closed, m_bIsClosed);

	if (m_bIsClosed)
	{
		m_dblStartAngle = 90.0;
		m_dblFinishAngle = -360.0 + m_dblStartAngle;

		tm.ReadBool (s_szTag_DrawLastTickMark, m_bDrawLastTickMark);
		tm.ReadBool (s_szTag_AnimationThroughStart, m_bAnimationThroughStart);
	}

	tm.ReadDouble (s_szTag_Start_Angle, m_dblStartAngle);
	tm.ReadDouble (s_szTag_Finish_Angle, m_dblFinishAngle);
	tm.ReadBool (s_szTag_RotateLabels, m_bRotateLabels);

	return XGaugeScale::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XCircularScale::ToTag (CString& strTag) const
{
	XGaugeScale::ToTag(strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Closed, m_bIsClosed, FALSE));

	if (m_bIsClosed)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Start_Angle, m_dblStartAngle, 90.0));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DrawLastTickMark, m_bDrawLastTickMark, TRUE));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_AnimationThroughStart, m_bAnimationThroughStart, TRUE));
	}
	else
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Start_Angle, m_dblStartAngle, 225.0));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Finish_Angle, m_dblFinishAngle, -45.0));
	}

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_RotateLabels, m_bRotateLabels, FALSE));
}

CBCGPVisualInfo::XCircularPointer::XCircularPointer()
	: CBCGPVisualInfo::XGaugeData()
	, m_dblSize                  (0.0)
	, m_dblWidth                 (0.0)
	, m_bExtraLen                (FALSE)
	, m_Style                    (CBCGPCircularGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE)
{
}

CBCGPVisualInfo::XCircularPointer::~XCircularPointer()
{
}

BOOL CBCGPVisualInfo::XCircularPointer::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadDouble (s_szTag_Size, m_dblSize);
	tm.ReadDouble (s_szTag_Width, m_dblWidth);
	tm.ReadBool (s_szTag_ExtraLen, m_bExtraLen);

	int style = CBCGPCircularGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE;
	tm.ReadInt (s_szTag_PointerStyle, style);
	m_Style = (CBCGPCircularGaugePointer::BCGP_GAUGE_POINTER_STYLE)bcg_clamp(style, (int)CBCGPCircularGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE, (int)CBCGPCircularGaugePointer::BCGP_GAUGE_MARKER_DIAMOND);

	return XGaugeData::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XCircularPointer::ToTag (CString& strTag) const
{
	XGaugeData::ToTag(strTag);

 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Size, m_dblSize, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Width, m_dblWidth, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_ExtraLen, m_bExtraLen, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_PointerStyle, (int)m_Style, (int)CBCGPCircularGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE));
}

CBCGPVisualInfo::XCircularColoredRange::XCircularColoredRange()
	: CBCGPVisualInfo::XGaugeColoredRange ()
{
}

CBCGPVisualInfo::XCircularColoredRange::~XCircularColoredRange()
{
}

BOOL CBCGPVisualInfo::XCircularColoredRange::FromTag (const CString& strTag)
{
	return XGaugeColoredRange::FromTag (strTag);
}

void CBCGPVisualInfo::XCircularColoredRange::ToTag (CString& strTag) const
{
	XGaugeColoredRange::ToTag(strTag);
}

CBCGPVisualInfo::XElementCircular::XElementCircular(const CString& strElementName)
	: CBCGPVisualInfo::XGaugeElement (strElementName)
	, m_dblCapSize                   (15.0)
	, m_bShapeByTicksArea            (FALSE)
{
}

CBCGPVisualInfo::XElementCircular::XElementCircular()
	: CBCGPVisualInfo::XGaugeElement (CBCGPVisualInfo::s_szCircularGauge)
	, m_dblCapSize                   (15.0)
	, m_bShapeByTicksArea            (FALSE)
{
}

CBCGPVisualInfo::XElementCircular::~XElementCircular()
{
	int i = 0;
	for (i = 0; i < m_arPointers.GetSize (); i++)
	{
		if (m_arPointers[i] != NULL)
		{
			delete m_arPointers[i];
		}
	}
	m_arPointers.RemoveAll ();

	for (i = 0; i < m_arSubGauges.GetSize (); i++)
	{
		if (m_arSubGauges[i] != NULL)
		{
			delete m_arSubGauges[i];
		}
	}
	m_arSubGauges.RemoveAll ();
}

CBCGPVisualInfo::XKnobPointer::XKnobPointer()
	: CBCGPVisualInfo::XGaugeData()
	, m_dblOffsetFromCenter      (0.0)
	, m_dblWidth                 (0.0)
	, m_Style                    (CBCGPKnobPointer::BCGP_KNOB_POINTER_CIRCLE)
{
}

CBCGPVisualInfo::XKnobPointer::~XKnobPointer()
{
}

BOOL CBCGPVisualInfo::XKnobPointer::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadDouble (s_szTag_Offset_Center, m_dblOffsetFromCenter);
	tm.ReadDouble (s_szTag_Width, m_dblWidth);

	int style = CBCGPKnobPointer::BCGP_KNOB_POINTER_CIRCLE;
	tm.ReadInt (s_szTag_PointerStyle, style);
	m_Style = (CBCGPKnobPointer::BCGP_KNOB_POINTER_STYLE)bcg_clamp(style, (int)CBCGPKnobPointer::BCGP_KNOB_POINTER_CIRCLE, (int)CBCGPKnobPointer::BCGP_KNOB_POINTER_HANDLE);

	return XGaugeData::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XKnobPointer::ToTag (CString& strTag) const
{
	XGaugeData::ToTag(strTag);

 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Offset_Center, m_dblOffsetFromCenter, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Width, m_dblWidth, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_PointerStyle, (int)m_Style, (int)CBCGPKnobPointer::BCGP_KNOB_POINTER_CIRCLE));
}

CBCGPVisualInfo::XElementKnob::XElementKnob()
	: CBCGPVisualInfo::XElementCircular (CBCGPVisualInfo::s_szKnob)
{
}

CBCGPVisualInfo::XElementKnob::~XElementKnob()
{
}

CBCGPVisualInfo::XElementAnalogClock::XElementAnalogClock()
	: CBCGPVisualInfo::XElementCircular (CBCGPVisualInfo::s_szAnalogClock)
{
}

CBCGPVisualInfo::XElementAnalogClock::~XElementAnalogClock()
{
}

CBCGPVisualInfo::XLinearScale::XLinearScale()
	: CBCGPVisualInfo::XGaugeScale()
{
}

CBCGPVisualInfo::XLinearScale::~XLinearScale()
{
}

BOOL CBCGPVisualInfo::XLinearScale::FromTag (const CString& strTag)
{
	return XGaugeScale::FromTag (strTag);
}

void CBCGPVisualInfo::XLinearScale::ToTag (CString& strTag) const
{
	XGaugeScale::ToTag(strTag);
}

CBCGPVisualInfo::XLinearPointer::XLinearPointer()
	: CBCGPVisualInfo::XGaugeData()
	, m_dblSize                  (0.0)
	, m_dblWidth                 (0.0)
	, m_Style                    (CBCGPLinearGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE)
{
}

CBCGPVisualInfo::XLinearPointer::~XLinearPointer()
{
}

BOOL CBCGPVisualInfo::XLinearPointer::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadDouble (s_szTag_Size, m_dblSize);
	tm.ReadDouble (s_szTag_Width, m_dblWidth);

	int style = CBCGPLinearGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE;
	tm.ReadInt (s_szTag_PointerStyle, style);
	m_Style = (CBCGPLinearGaugePointer::BCGP_GAUGE_POINTER_STYLE)bcg_clamp(style, (int)CBCGPLinearGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE, (int)CBCGPLinearGaugePointer::BCGP_GAUGE_NEEDLE_DIAMOND);

	return XGaugeData::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XLinearPointer::ToTag (CString& strTag) const
{
	XGaugeData::ToTag(strTag);

 	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Size, m_dblSize, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_Width, m_dblWidth, 0.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_PointerStyle, (int)m_Style, (int)CBCGPLinearGaugePointer::BCGP_GAUGE_NEEDLE_TRIANGLE));
}

CBCGPVisualInfo::XLinearColoredRange::XLinearColoredRange()
	: CBCGPVisualInfo::XGaugeColoredRange ()
{
}

CBCGPVisualInfo::XLinearColoredRange::~XLinearColoredRange()
{
}

BOOL CBCGPVisualInfo::XLinearColoredRange::FromTag (const CString& strTag)
{
	return XGaugeColoredRange::FromTag (strTag);
}

void CBCGPVisualInfo::XLinearColoredRange::ToTag (CString& strTag) const
{
	XGaugeColoredRange::ToTag(strTag);
}

CBCGPVisualInfo::XElementLinear::XElementLinear()
	: CBCGPVisualInfo::XGaugeElement (CBCGPVisualInfo::s_szLinearGauge)
	, m_bIsVertical                  (FALSE)
{
}

CBCGPVisualInfo::XElementLinear::~XElementLinear()
{
	int i = 0;
	for (i = 0; i < m_arPointers.GetSize (); i++)
	{
		if (m_arPointers[i] != NULL)
		{
			delete m_arPointers[i];
		}
	}
	m_arPointers.RemoveAll ();
}

CBCGPVisualInfo::XElementNumeric::XElementNumeric()
	: CBCGPVisualInfo::XGaugeElement (CBCGPVisualInfo::s_szNumericInd)
	, m_Style                  (CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	, m_nCells                 (7)
	, m_nDecimals              (1)
	, m_nSeparatorWidth        (1)
	, m_bDrawSign              (FALSE)
	, m_bDrawDecimalPoint      (FALSE)
	, m_bDrawLeadingZeros      (TRUE)
	, m_dblValue               (0.0)
{
	m_nFrameSize = 1;
}

CBCGPVisualInfo::XElementNumeric::~XElementNumeric()
{
}

CBCGPVisualInfo::XElementColor::XElementColor()
	: CBCGPVisualInfo::XGaugeElement (CBCGPVisualInfo::s_szColorInd)
	, m_Style                  (CBCGPColorIndicatorImpl::BCGP_COLOR_INDICATOR_ELLIPSE)
	, m_bStretched             (FALSE)
{
}

CBCGPVisualInfo::XElementColor::~XElementColor()
{
}

CBCGPVisualInfo::XElementText::XElementText()
	: CBCGPVisualInfo::XGaugeElement (CBCGPVisualInfo::s_szTextInd)
	, m_clrText (CBCGPColor::Black)
{
}

CBCGPVisualInfo::XElementText::~XElementText()
{
}


BOOL CBCGPVisualInfo::XGaugeElement::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strColors;
	if (tm.ExcludeTag (s_szTag_Colors, strColors))
	{
		ColorsFromTag (strColors);
	}

	tm.ReadInt (s_szTag_FrameSize, m_nFrameSize);

	int pos = (int)CBCGPGaugeImpl::BCGP_SUB_GAUGE_NONE;
	tm.ReadInt (s_szTag_Pos, pos);
	m_Pos = (CBCGPGaugeImpl::BCGP_SUB_GAUGE_POS)bcg_clamp(pos, (int)CBCGPGaugeImpl::BCGP_SUB_GAUGE_NONE, (int)CBCGPGaugeImpl::BCGP_SUB_GAUGE_RIGHT);

	tm.ReadPoint (s_szTag_Offset, m_ptOffset);

	tm.ReadBool (s_szTag_InteractiveMode, m_bIsInteractiveMode);

	tm.ReadString (s_szTag_ToolTip, m_strToolTip);
	CBCGPTagManager::Entity_FromTag (m_strToolTip);
	tm.ReadString (s_szTag_Description, m_strDescription);
	CBCGPTagManager::Entity_FromTag (m_strDescription);

	return XElement::FromTag(strTag);
}

void CBCGPVisualInfo::XGaugeElement::ToTag (CString& strTag) const
{
	XElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_FrameSize, m_nFrameSize, 6));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Pos, (int)m_Pos, (int)CBCGPGaugeImpl::BCGP_SUB_GAUGE_NONE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WritePoint (s_szTag_Offset, m_ptOffset, CPoint(0, 0)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_InteractiveMode, m_bIsInteractiveMode, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_ToolTip, CBCGPTagManager::Entity_ToTag (m_strToolTip)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Description, CBCGPTagManager::Entity_ToTag (m_strDescription)));

	CString strColors;
	ColorsToTag (strColors);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Colors, strColors);
}

BOOL CBCGPVisualInfo::XElementCircular::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	BOOL bKnob = GetElementName().Compare (CBCGPVisualInfo::s_szKnob) == 0;

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			CString strElementName;
			{
				CBCGPTagManager tm (strElement);
				tm.ReadString (CBCGPBaseInfo::s_szTag_ElementName, strElementName);
			}

			if (!strElementName.IsEmpty ())
			{
				XGaugeElement* pElement = (XGaugeElement*)CBCGPVisualInfo::XGaugeElement::CreateFromName (strElementName);
				if (pElement != NULL)
				{
					pElement->FromTag (strElement);
					m_arSubGauges.Add (pElement);
				}
			}
		}
	}

	CString strScales;
	if (tm.ExcludeTag (s_szTag_Scales, strScales))
	{
		CBCGPTagManager tmScales (strScales);

		CString strScale;
		while (tmScales.ExcludeTag (s_szTag_Scale, strScale))
		{
			XCircularScale* pScale = new XCircularScale;
			if (pScale->FromTag (strScale))
			{
				m_arScales.Add (pScale);
			}
			else
			{
				delete pScale;
			}
		}
	}

	CString strPointers;
	if (tm.ExcludeTag (s_szTag_Pointers, strPointers))
	{
		CBCGPTagManager tmPointers (strPointers);

		CString strPointer;
		while (tmPointers.ExcludeTag (s_szTag_Pointer, strPointer))
		{
			XGaugeData* pPointer = NULL;
			if (!bKnob)
			{
				pPointer = new XCircularPointer;
			}
			else
			{
				pPointer = new XKnobPointer;
			}

			if (pPointer->FromTag (strPointer))
			{
				m_arPointers.Add (pPointer);
			}
			else
			{
				delete pPointer;
			}
		}
	}

	CString strRanges;
	if (tm.ExcludeTag (s_szTag_Ranges, strRanges))
	{
		CBCGPTagManager tmRanges (strRanges);

		CString strRange;
		while (tmRanges.ExcludeTag (s_szTag_Range, strRange))
		{
			XCircularColoredRange* pRange = new XCircularColoredRange;
			if (pRange->FromTag (strRange))
			{
				m_arRanges.Add (pRange);
			}
			else
			{
				delete pRange;
			}
		}
	}

	tm.ReadDouble (s_szTag_CapSize, m_dblCapSize);
	tm.ReadBool (s_szTag_Shape_Area, m_bShapeByTicksArea);
	tm.ReadTextFormat (s_szTag_TextFormat, m_fmtText);

	return XGaugeElement::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XElementCircular::ToTag (CString& strTag) const
{
	XGaugeElement::ToTag (strTag);

	int i = 0;

	CString strScales;
	for (i = 0; i < m_arScales.GetSize (); i++)
	{
		CString strScale;
		m_arScales[i]->ToTag (strScale);
		CBCGPTagManager::WriteItem (strScales, s_szTag_Scale, strScale, TRUE);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Scales, strScales);

	CString strPointers;
	for (i = 0; i < m_arPointers.GetSize (); i++)
	{
		CString strPointer;
		m_arPointers[i]->ToTag (strPointer);
		CBCGPTagManager::WriteItem (strPointers, s_szTag_Pointer, strPointer, TRUE);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Pointers, strPointers);

	CString strRanges;
	for (i = 0; i < m_arRanges.GetSize (); i++)
	{
		CString strRange;
		m_arRanges[i]->ToTag (strRange);
		CBCGPTagManager::WriteItem (strRanges, s_szTag_Range, strRange, TRUE);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Ranges, strRanges);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_CapSize, m_dblCapSize, 20.0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Shape_Area, m_bShapeByTicksArea, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteTextFormat (s_szTag_TextFormat, m_fmtText));

	CString strElements;
	for (i = 0; i < m_arSubGauges.GetSize (); i++)
	{
		CString strElement;
		m_arSubGauges[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);
}

BOOL CBCGPVisualInfo::XElementCircular::ColorsFromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBrush (s_szTag_Pointer_Brush, m_Colors.m_brPointerFill);
	tm.ReadBrush (s_szTag_Pointer_Outline, m_Colors.m_brPointerOutline);
	tm.ReadBrush (s_szTag_Scale_Brush, m_Colors.m_brScaleFill);
	tm.ReadBrush (s_szTag_Scale_Outline, m_Colors.m_brScaleOutline);
	tm.ReadBrush (s_szTag_Text_Brush, m_Colors.m_brText);
	tm.ReadBrush (s_szTag_Frame_Brush, m_Colors.m_brFrameFill);
	tm.ReadBrush (s_szTag_Frame_BrushInv, m_Colors.m_brFrameFillInv);
	tm.ReadBrush (s_szTag_Frame_Outline, m_Colors.m_brFrameOutline);
	tm.ReadBrush (s_szTag_Fill_Brush, m_Colors.m_brFill);
	tm.ReadBrush (s_szTag_Cap_Brush, m_Colors.m_brCapFill);
	tm.ReadBrush (s_szTag_Cap_Outline, m_Colors.m_brCapOutline);
	tm.ReadBrush (s_szTag_TickMark_Brush, m_Colors.m_brTickMarkFill);
	tm.ReadBrush (s_szTag_TickMark_Outline, m_Colors.m_brTickMarkOutline);

	return TRUE;
}

void CBCGPVisualInfo::XElementCircular::ColorsToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Pointer_Brush, m_Colors.m_brPointerFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Pointer_Outline, m_Colors.m_brPointerOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Scale_Brush, m_Colors.m_brScaleFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Scale_Outline, m_Colors.m_brScaleOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Text_Brush, m_Colors.m_brText));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Frame_Brush, m_Colors.m_brFrameFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Frame_BrushInv, m_Colors.m_brFrameFillInv));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Frame_Outline, m_Colors.m_brFrameOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_Colors.m_brFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Cap_Brush, m_Colors.m_brCapFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Cap_Outline, m_Colors.m_brCapOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMark_Brush, m_Colors.m_brTickMarkFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMark_Outline, m_Colors.m_brTickMarkOutline));
}

BOOL CBCGPVisualInfo::XElementLinear::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strScales;
	if (tm.ExcludeTag (s_szTag_Scales, strScales))
	{
		CBCGPTagManager tmScales (strScales);

		CString strScale;
		while (tmScales.ExcludeTag (s_szTag_Scale, strScale))
		{
			XLinearScale* pScale = new XLinearScale;
			if (pScale->FromTag (strScale))
			{
				m_arScales.Add (pScale);
			}
			else
			{
				delete pScale;
			}
		}
	}

	CString strPointers;
	if (tm.ExcludeTag (s_szTag_Pointers, strPointers))
	{
		CBCGPTagManager tmPointers (strPointers);

		CString strPointer;
		while (tmPointers.ExcludeTag (s_szTag_Pointer, strPointer))
		{
			XLinearPointer* pPointer = new XLinearPointer;
			if (pPointer->FromTag (strPointer))
			{
				m_arPointers.Add (pPointer);
			}
			else
			{
				delete pPointer;
			}
		}
	}

	CString strRanges;
	if (tm.ExcludeTag (s_szTag_Ranges, strRanges))
	{
		CBCGPTagManager tmRanges (strRanges);

		CString strRange;
		while (tmRanges.ExcludeTag (s_szTag_Range, strRange))
		{
			XLinearColoredRange* pRange = new XLinearColoredRange;
			if (pRange->FromTag (strRange))
			{
				m_arRanges.Add (pRange);
			}
			else
			{
				delete pRange;
			}
		}
	}

	tm.ReadBool (s_szTag_Vertical, m_bIsVertical);
	tm.ReadTextFormat (s_szTag_TextFormat, m_fmtText);

	return XGaugeElement::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XElementLinear::ToTag (CString& strTag) const
{
	XGaugeElement::ToTag (strTag);

	int i = 0;

	CString strScales;
	for (i = 0; i < m_arScales.GetSize (); i++)
	{
		CString strScale;
		m_arScales[i]->ToTag (strScale);
		CBCGPTagManager::WriteItem (strScales, s_szTag_Scale, strScale, TRUE);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Scales, strScales);

	CString strPointers;
	for (i = 0; i < m_arPointers.GetSize (); i++)
	{
		CString strPointer;
		m_arPointers[i]->ToTag (strPointer);
		CBCGPTagManager::WriteItem (strPointers, s_szTag_Pointer, strPointer, TRUE);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Pointers, strPointers);

	CString strRanges;
	for (i = 0; i < m_arRanges.GetSize (); i++)
	{
		CString strRange;
		m_arRanges[i]->ToTag (strRange);
		CBCGPTagManager::WriteItem (strRanges, s_szTag_Range, strRange, TRUE);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Ranges, strRanges);
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Vertical, m_bIsVertical, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteTextFormat (s_szTag_TextFormat, m_fmtText));
}

BOOL CBCGPVisualInfo::XElementLinear::ColorsFromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBrush (s_szTag_Pointer_Brush, m_Colors.m_brPointerFill);
	tm.ReadBrush (s_szTag_Pointer_Outline, m_Colors.m_brPointerOutline);
	tm.ReadBrush (s_szTag_Scale_Brush, m_Colors.m_brScaleFill);
	tm.ReadBrush (s_szTag_Scale_Outline, m_Colors.m_brScaleOutline);
	tm.ReadBrush (s_szTag_Text_Brush, m_Colors.m_brText);
	tm.ReadBrush (s_szTag_Frame_Brush, m_Colors.m_brFrameFill);
	tm.ReadBrush (s_szTag_Frame_Outline, m_Colors.m_brFrameOutline);
	tm.ReadBrush (s_szTag_Fill_Brush, m_Colors.m_brFill);
	tm.ReadBrush (s_szTag_TickMark_Brush, m_Colors.m_brTickMarkFill);
	tm.ReadBrush (s_szTag_TickMark_Outline, m_Colors.m_brTickMarkOutline);

	return TRUE;
}

void CBCGPVisualInfo::XElementLinear::ColorsToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Pointer_Brush, m_Colors.m_brPointerFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Pointer_Outline, m_Colors.m_brPointerOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Scale_Brush, m_Colors.m_brScaleFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Scale_Outline, m_Colors.m_brScaleOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Text_Brush, m_Colors.m_brText));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Frame_Brush, m_Colors.m_brFrameFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Frame_Outline, m_Colors.m_brFrameOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_Colors.m_brFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMark_Brush, m_Colors.m_brTickMarkFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_TickMark_Outline, m_Colors.m_brTickMarkOutline));
}

BOOL CBCGPVisualInfo::XElementNumeric::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	int style = (int)CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC;
	tm.ReadInt (s_szTag_Style, style);
	m_Style = (CBCGPNumericIndicatorImpl::BCGPNumericIndicatorStyle)bcg_clamp(style, (int)CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC, (int)CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_DIGITAL);

	tm.ReadInt (s_szTag_Cells, m_nCells);
	tm.ReadInt (s_szTag_Decimals, m_nDecimals);
	if (m_Style == CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	{
		tm.ReadInt (s_szTag_SeparatorWidth, m_nSeparatorWidth);
	}
	else
	{
		m_nSeparatorWidth = 0;
	}
	tm.ReadBool (s_szTag_DrawSign, m_bDrawSign);
	tm.ReadBool (s_szTag_DrawDecimalPoint, m_bDrawDecimalPoint);
	tm.ReadBool (s_szTag_DrawLeadingZeros, m_bDrawLeadingZeros);
	tm.ReadDouble (s_szTag_DataValue, m_dblValue);

	if (m_Style == CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	{
		tm.ReadTextFormat (s_szTag_TextFormat, m_fmtText);
	}

	return XGaugeElement::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XElementNumeric::ToTag (CString& strTag) const
{
	XGaugeElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Style, (int)m_Style, (int)CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Cells, m_nCells, 7));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Decimals, m_nDecimals, 1));
	if (m_Style == CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_SeparatorWidth, m_nSeparatorWidth, 1));
	}
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DrawSign, m_bDrawSign, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DrawDecimalPoint, m_bDrawDecimalPoint, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DrawLeadingZeros, m_bDrawLeadingZeros, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteDouble (s_szTag_DataValue, m_dblValue, m_dblValue + 1.0));

	if (m_Style == CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteTextFormat (s_szTag_TextFormat, m_fmtText));
	}
}

BOOL CBCGPVisualInfo::XElementNumeric::ColorsFromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBrush (s_szTag_Fill_Brush, m_Colors.m_brFill);
	tm.ReadBrush (s_szTag_Fill_Outline, m_Colors.m_brOutline);
	tm.ReadBrush (s_szTag_Digit_Brush, m_Colors.m_brDigit);
	tm.ReadBrush (s_szTag_Decimal_Brush, m_Colors.m_brDecimal);
	tm.ReadBrush (s_szTag_Sign_Brush, m_Colors.m_brSign);
	tm.ReadBrush (s_szTag_Dot_Brush, m_Colors.m_brDot);

	if (m_Style == CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	{
		tm.ReadBrush (s_szTag_Separator_Brush, m_Colors.m_brSeparator);
	}
	else
	{
		m_Colors.m_brSeparator = m_Colors.m_brDigit;
	}

	return TRUE;
}

void CBCGPVisualInfo::XElementNumeric::ColorsToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_Colors.m_brFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Outline, m_Colors.m_brOutline));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Digit_Brush, m_Colors.m_brDigit));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Decimal_Brush, m_Colors.m_brDecimal));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Sign_Brush, m_Colors.m_brSign));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Dot_Brush, m_Colors.m_brDot));

	if (m_Style == CBCGPNumericIndicatorImpl::BCGP_NUMERIC_INDICATOR_CLASSIC)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Separator_Brush, m_Colors.m_brSeparator));
	}
}

BOOL CBCGPVisualInfo::XElementColor::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	int style = (int)CBCGPColorIndicatorImpl::BCGP_COLOR_INDICATOR_ELLIPSE;
	tm.ReadInt (s_szTag_Style, style);
	m_Style = (CBCGPColorIndicatorImpl::BCGPColorIndicatorStyle)bcg_clamp(style, (int)CBCGPColorIndicatorImpl::BCGP_COLOR_INDICATOR_ELLIPSE, (int)CBCGPColorIndicatorImpl::BCGP_COLOR_INDICATOR_RECTANGLE);
	tm.ReadBool (s_szTag_Stretched, m_bStretched);

	return XGaugeElement::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XElementColor::ToTag (CString& strTag) const
{
	XGaugeElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Style, (int)m_Style, (int)CBCGPColorIndicatorImpl::BCGP_COLOR_INDICATOR_ELLIPSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Stretched, m_bStretched, FALSE));
}

BOOL CBCGPVisualInfo::XElementColor::ColorsFromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBrush (s_szTag_Fill_Brush, m_Colors.m_brFill);
	tm.ReadBrush (s_szTag_Fill_Outline, m_Colors.m_brOutline);

	return TRUE;
}

void CBCGPVisualInfo::XElementColor::ColorsToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Brush, m_Colors.m_brFill));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBrush (s_szTag_Fill_Outline, m_Colors.m_brOutline));
}


BOOL CBCGPVisualInfo::XElementText::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadString (s_szTag_DataValue, m_strText);
	CBCGPTagManager::Entity_FromTag (m_strText);
	tm.ReadTextFormat (s_szTag_TextFormat, m_fmtText);

	return XGaugeElement::FromTag (tm.GetBuffer ());
}

void CBCGPVisualInfo::XElementText::ToTag (CString& strTag) const
{
	XGaugeElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_DataValue, CBCGPTagManager::Entity_ToTag (m_strText)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteTextFormat (s_szTag_TextFormat, m_fmtText));
}

BOOL CBCGPVisualInfo::XElementText::ColorsFromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadColor (s_szTag_Text_Brush, m_clrText);

	return TRUE;
}

void CBCGPVisualInfo::XElementText::ColorsToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteColor (s_szTag_Text_Brush, m_clrText, CBCGPColor(CBCGPColor::Black)));
}


CBCGPVisualInfo::CBCGPVisualInfo()
	: CBCGPBaseInfo(c_dwVersion, 0)
{
}

CBCGPVisualInfo::~CBCGPVisualInfo()
{
}

BOOL CBCGPVisualInfo::FromTag (const CString& strTag)
{
	CString strXML;
	{
		CBCGPTagManager tmContainer (strTag);

		if (!tmContainer.ExcludeTag (s_szTag_Body, strXML))
		{
			return FALSE;
		}
	}

	CBCGPTagManager tm (strXML);

	CString strHeader;
	if (tm.ExcludeTag (CBCGPBaseInfo::s_szTag_Header, strHeader))
	{
		CBCGPTagManager tmHeader (strHeader);

		int nValue = (int)GetVersion ();
		tmHeader.CBCGPTagManager::ReadInt (CBCGPBaseInfo::s_szTag_Version, nValue);
		SetVersion ((DWORD)nValue);
	}
	else
	{
		return FALSE;
	}

	CString strContainer;
	if (tm.ExcludeTag (s_szTag_Container, strContainer))
	{
		GetContainer().FromTag (strContainer);
	}

	return TRUE;
}

void CBCGPVisualInfo::ToTag (CString& strTag) const
{
	CString strData;

	CString strHeader;
	CBCGPTagManager::WriteTag (strHeader, CBCGPTagManager::WriteUInt (CBCGPBaseInfo::s_szTag_Version, GetVersion (), 0));
	CBCGPTagManager::WriteItem (strData, CBCGPBaseInfo::s_szTag_Header, strHeader);

	CString strContainer;
	GetContainer().ToTag (strContainer);
	CBCGPTagManager::WriteItem (strData, s_szTag_Container, strContainer);

	CBCGPTagManager::WriteItem (strTag, s_szTag_Body, strData);
}


CBCGPVisualInfoLoader::CBCGPVisualInfoLoader (CBCGPVisualInfo& info)
	: CBCGPBaseInfoLoader(info, _T("BCGP_CONTAINER_XML"), 0)
{
}

CBCGPVisualInfoLoader::~CBCGPVisualInfoLoader()
{
}

CBCGPVisualInfoWriter::CBCGPVisualInfoWriter(CBCGPVisualInfo& info)
	: CBCGPBaseInfoWriter(info)
{
}

CBCGPVisualInfoWriter::~CBCGPVisualInfoWriter()
{
}

BOOL CBCGPVisualInfoWriter::Save (const CString& strFileName)
{
	if (strFileName.IsEmpty ())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CStringArray sa;
	sa.Add (strFileName);

	if (!CheckFiles (sa))
	{
		return FALSE;
	}

	return SaveInfo (strFileName);
}
