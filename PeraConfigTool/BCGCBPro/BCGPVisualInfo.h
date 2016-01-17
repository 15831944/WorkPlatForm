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
// BCGPVisualInfo.h: interface for the CBCGPCircularGaugeImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPVISUALINFO_H__38DF9C07_6F92_433D_B7D0_2CBB09EDD9D6__INCLUDED_)
#define AFX_BCGPVISUALINFO_H__38DF9C07_6F92_433D_B7D0_2CBB09EDD9D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bcgcbpro.h"
#include "BCGPBaseInfo.h"

#include "BCGPCircularGaugeImpl.h"
#include "BCGPKnob.h"
#include "BCGPLinearGaugeImpl.h"
#include "BCGPNumericIndicatorImpl.h"
#include "BCGPColorIndicatorImpl.h"

class CBCGPVisualInfo: public CBCGPBaseInfo  
{
public:
	static CBCGPBaseInfo::XBase* CreateBaseFromName (const CString& name);
	static CBCGPBaseInfo::XBase* CreateBaseFromTag (const CString& tag);

	class XData
	{
	public:
		XData ();
		virtual ~XData ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		double		m_dblValue;
	};
	typedef CArray<XData*, XData*> XArrayData;

	class XElement: public CBCGPBaseInfo::XBase
	{
	protected:
		XElement(const CString& strElementName);

	public:
		virtual ~XElement ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		XID			m_ID;
		CRect		m_Rect;
		BOOL		m_bIsVisible;
		BOOL		m_bIsAutoDestroy;
	};
	typedef CArray<XElement*, XElement*> XArrayElement;

	class XContainer: public CBCGPBaseInfo::XBase
	{
	public:
		XContainer();
		virtual ~XContainer();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		CRect			m_Rect;
		CBCGPBrush		m_brFill;
		CBCGPBrush		m_brOutline;

		XArrayElement	m_arElements;
	};

	class XGaugeScale
	{
	public:
		XGaugeScale ();
		virtual ~XGaugeScale ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		double			m_dblStart;
		double			m_dblFinish;
		double			m_dblStep;
		double			m_dblOffsetFromFrame;
		CString			m_strLabelFormat;
		double			m_dblMajorTickMarkStep;
		double			m_dblMinorTickMarkSize;
		double			m_dblMajorTickMarkSize;
		CBCGPGaugeScaleObject::BCGP_TICKMARK_STYLE
						m_MinorTickMarkStyle;
		CBCGPGaugeScaleObject::BCGP_TICKMARK_STYLE
						m_MajorTickMarkStyle;
		CBCGPBrush		m_brFill;
		CBCGPBrush		m_brOutline;
		CBCGPBrush		m_brText;
		CBCGPBrush		m_brTickMarkMajor;
		CBCGPBrush		m_brTickMarkMinor;
	};
	typedef CArray<XGaugeScale*, XGaugeScale*> XArrayGaugeScale;

	class XGaugeData : public XData
	{
	public:
		XGaugeData ();
		virtual ~XGaugeData ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		int			m_nScale;
		CBCGPBrush	m_brFill;
		CBCGPBrush	m_brOutline;
	};

	class XGaugeColoredRange
	{
	public:
		XGaugeColoredRange ();
		virtual ~XGaugeColoredRange ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		int			m_nScale;
		double		m_dblStartValue;
		double		m_dblFinishValue;
		double		m_dblStartWidth;
		double		m_dblFinishWidth;
		double		m_dblOffsetFromFrame;
		CBCGPBrush	m_brFill;
		CBCGPBrush	m_brOutline;
		CBCGPBrush	m_brTickMarkFill;
		CBCGPBrush	m_brTickMarkOutline;
		CBCGPBrush	m_brTextLabel;
	};
	typedef CArray<XGaugeColoredRange*, XGaugeColoredRange*> XArrayGaugeColoredRange;

	class XGaugeElement: public XElement
	{
	protected:
		XGaugeElement(const CString& strElementName);

	public:
		virtual ~XGaugeElement();

		static XGaugeElement* CreateFromName (const CString& name);

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	protected:
		virtual BOOL ColorsFromTag (const CString& strTag) = 0;
		virtual void ColorsToTag (CString& strTag) const = 0;

	public:
		int						m_nFrameSize;
		CBCGPGaugeImpl::BCGP_SUB_GAUGE_POS
								m_Pos;
		CPoint					m_ptOffset;
		XArrayGaugeScale		m_arScales;
		XArrayGaugeColoredRange	m_arRanges;

		BOOL					m_bIsInteractiveMode;
		CString					m_strToolTip;
		CString					m_strDescription;
	};
	typedef CArray<XGaugeElement*, XGaugeElement*> XArrayGaugeElement;

	class XCircularScale: public XGaugeScale
	{
	public:
		XCircularScale ();
		virtual ~XCircularScale ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		double			m_dblStartAngle;
		double			m_dblFinishAngle;
		BOOL			m_bRotateLabels;
		BOOL			m_bIsClosed;
		BOOL			m_bDrawLastTickMark;
		BOOL			m_bAnimationThroughStart;
	};

	class XCircularPointer: public XGaugeData
	{
	public:
		XCircularPointer ();
		virtual ~XCircularPointer ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		double		m_dblSize;
		double		m_dblWidth;
		BOOL		m_bExtraLen;
		CBCGPCircularGaugePointer::BCGP_GAUGE_POINTER_STYLE
					m_Style;
	};

	class XCircularColoredRange: public XGaugeColoredRange
	{
	public:
		XCircularColoredRange ();
		virtual ~XCircularColoredRange ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;
	};

	class XElementCircular: public XGaugeElement
	{
	protected:
		XElementCircular(const CString& strElementName);

	public:
		XElementCircular();
		virtual ~XElementCircular();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	protected:
		virtual BOOL ColorsFromTag (const CString& strTag);
		virtual void ColorsToTag (CString& strTag) const;

	public:
		CBCGPCircularGaugeColors	m_Colors;
		CBCGPTextFormat				m_fmtText;
		double						m_dblCapSize;
		BOOL						m_bShapeByTicksArea;

		XArrayData					m_arPointers;
		XArrayGaugeElement			m_arSubGauges;
	};

	class XKnobPointer: public XGaugeData
	{
	public:
		XKnobPointer ();
		virtual ~XKnobPointer ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		double		m_dblOffsetFromCenter;
		double		m_dblWidth;
		CBCGPKnobPointer::BCGP_KNOB_POINTER_STYLE
					m_Style;
	};

	class XElementKnob: public XElementCircular
	{
	public:
		XElementKnob ();
		virtual ~XElementKnob ();
	};

	class XElementAnalogClock: public XElementCircular
	{
	public:
		XElementAnalogClock ();
		virtual ~XElementAnalogClock ();
	};

	class XLinearScale: public XGaugeScale
	{
	public:
		XLinearScale ();
		virtual ~XLinearScale ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;
	};

	class XLinearPointer: public XGaugeData
	{
	public:
		XLinearPointer ();
		virtual ~XLinearPointer ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	public:
		double		m_dblSize;
		double		m_dblWidth;
		CBCGPLinearGaugePointer::BCGP_GAUGE_POINTER_STYLE
					m_Style;
	};

	class XLinearColoredRange: public XGaugeColoredRange
	{
	public:
		XLinearColoredRange ();
		virtual ~XLinearColoredRange ();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;
	};

	class XElementLinear: public XGaugeElement
	{
	public:
		XElementLinear();
		virtual ~XElementLinear();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	protected:
		virtual BOOL ColorsFromTag (const CString& strTag);
		virtual void ColorsToTag (CString& strTag) const;

	public:
		CBCGPLinearGaugeColors		m_Colors;
		CBCGPTextFormat				m_fmtText;
		BOOL						m_bIsVertical;

		XArrayData					m_arPointers;
	};

	class XElementNumeric: public XGaugeElement
	{
	public:
		XElementNumeric();
		virtual ~XElementNumeric();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	protected:
		virtual BOOL ColorsFromTag (const CString& strTag);
		virtual void ColorsToTag (CString& strTag) const;

	public:
		CBCGPNumericIndicatorColors	m_Colors;
		CBCGPTextFormat				m_fmtText;
		CBCGPNumericIndicatorImpl::BCGPNumericIndicatorStyle
									m_Style;
		int							m_nCells;
		int							m_nDecimals;
		int							m_nSeparatorWidth;
		BOOL						m_bDrawSign;
		BOOL						m_bDrawDecimalPoint;
		BOOL						m_bDrawLeadingZeros;
		double						m_dblValue;
	};

	class XElementColor: public XGaugeElement
	{
	public:
		XElementColor();
		virtual ~XElementColor();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	protected:
		virtual BOOL ColorsFromTag (const CString& strTag);
		virtual void ColorsToTag (CString& strTag) const;

	public:
		CBCGPColorIndicatorColors	m_Colors;
		CBCGPColorIndicatorImpl::BCGPColorIndicatorStyle
									m_Style;
		BOOL						m_bStretched;
	};

	class XElementText: public XGaugeElement
	{
	public:
		XElementText();
		virtual ~XElementText();

		virtual BOOL FromTag (const CString& strTag);
		virtual void ToTag (CString& strTag) const;

	protected:
		virtual BOOL ColorsFromTag (const CString& strTag);
		virtual void ColorsToTag (CString& strTag) const;

	public:
		CBCGPColor		m_clrText;
		CBCGPTextFormat	m_fmtText;
		CString			m_strText;
	};

public:
	CBCGPVisualInfo();
	virtual ~CBCGPVisualInfo();

	virtual BOOL FromTag (const CString& strTag);
	virtual void ToTag (CString& strTag) const;

	inline XContainer& GetContainer ()
	{
		return m_Container;
	}
	inline const XContainer& GetContainer () const
	{
		return m_Container;
	}

public:
	static LPCTSTR s_szCircularGauge;
	static LPCTSTR s_szKnob;
	static LPCTSTR s_szAnalogClock;
	static LPCTSTR s_szLinearGauge;
	static LPCTSTR s_szNumericInd;
	static LPCTSTR s_szColorInd;
	static LPCTSTR s_szTextInd;
	static LPCTSTR s_szContainer;

private:
	XContainer	m_Container;
};

class CBCGPVisualInfoLoader: public CBCGPBaseInfoLoader
{
public:
	CBCGPVisualInfoLoader (CBCGPVisualInfo& info);
	virtual ~CBCGPVisualInfoLoader();

protected:
	CBCGPVisualInfo& GetVisualInfo () const
	{
		return (CBCGPVisualInfo&)GetInfo ();
	}
};

class CBCGPVisualInfoWriter: public CBCGPBaseInfoWriter
{
public:
	CBCGPVisualInfoWriter(CBCGPVisualInfo& info);
	virtual ~CBCGPVisualInfoWriter();

	virtual BOOL Save (const CString& strFileName);

protected:
	CBCGPVisualInfo& GetVisualInfo ()
	{
		return (CBCGPVisualInfo&)GetInfo ();
	}
};

#endif // !defined(AFX_BCGPVISUALINFO_H__38DF9C07_6F92_433D_B7D0_2CBB09EDD9D6__INCLUDED_)
