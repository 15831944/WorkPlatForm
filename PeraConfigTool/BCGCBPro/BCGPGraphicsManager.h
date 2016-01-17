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
// BCGPGraphicsManager.h: interface for the CBCGPGraphicsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPGRAPHICSMANAGER_H__979A73FE_851A_4E39_B412_4F2C22047E28__INCLUDED_)
#define AFX_BCGPGRAPHICSMANAGER_H__979A73FE_851A_4E39_B412_4F2C22047E28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"
#include "BCGGlobals.h"

#if (!defined _BCGSUITE_) && (!defined _BCGSUITE_INC_)
#include "BCGPToolBarImages.h"
#endif

// Graphics Manager supported features:

#define BCGP_GRAPHICS_MANAGER_COLOR_OPACITY	0x0001
#define BCGP_GRAPHICS_MANAGER_CAP_STYLE		0x0002
#define BCGP_GRAPHICS_MANAGER_LINE_JOIN		0x0004
#define BCGP_GRAPHICS_MANAGER_ANTIALIAS		0x0008

////////////////////////////////////////////////////////////////////////////////////
// CBCGPPoint

class BCGCBPRODLLEXPORT AFX_NOVTABLE CBCGPPoint
{
// constructors & destructors
public:
	CBCGPPoint()
		: x(0.)
		, y(0.)
		, z(0.)
	{
	}

	CBCGPPoint(double X, double Y, double Z = 0)
		: x(X)
		, y(Y)
		, z(Z)
	{
	}

	CBCGPPoint(const CBCGPPoint& pt)
		: x(pt.x)
		, y(pt.y)
		, z(pt.z)
	{
	}

	CBCGPPoint(const CPoint& pt)
		: x((double)pt.x)
		, y((double)pt.y)
		, z(0)
	{
	}

// member functions
public:
	void SetPoint(double X, double Y, double Z = 0)
	{
		x = X;
		y = Y;
		z = Z;
	}

	void SetPoint(const CBCGPPoint& pt)
	{
		SetPoint (pt.x, pt.y, pt.z);
	}

	void Offset(double X, double Y, double Z = 0)
	{
		x += X;
		y += Y;
		z += Z;
	}

	void Offset(const CBCGPPoint& pt)
	{
		Offset (pt.x, pt.y, pt.z);
	}

	void Swap()
	{
		double tmp = x;
		x = y;
		y = tmp;
	}

	CBCGPPoint Ortho() const
	{
		return CBCGPPoint(-y, x);
	}

// operators
public:
	CBCGPPoint& operator = (const CBCGPPoint& pt)
	{
		x = pt.x;
		y = pt.y;
		z = pt.z;
		return *this;
	}

	const CBCGPPoint& operator += (const CBCGPPoint& pt)
	{
		Offset (pt.x, pt.y, pt.z);
		return *this;
	}

	const CBCGPPoint& operator -= (const CBCGPPoint& pt)
	{
		Offset (-pt.x, -pt.y, -pt.z);
		return *this;
	}

	const CBCGPPoint& operator *= (double value)
	{
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	const CBCGPPoint& operator /= (double value)
	{
		_ASSERTE(value != 0.0);
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	bool operator == (const CBCGPPoint& pt) const
	{
		return x == pt.x && y == pt.y && z == pt.z;
	}

	bool operator != (const CBCGPPoint& pt) const
	{
		return x != pt.x || y != pt.y || z != pt.z;
	}

	CBCGPPoint operator + (const CBCGPPoint& pt) const
	{
		return CBCGPPoint(x + pt.x, y + pt.y, z + pt.z);
	}

	CBCGPPoint operator - (const CBCGPPoint& pt) const
	{
		return CBCGPPoint(x - pt.x, y - pt.y, z - pt.z);
	}

	CBCGPPoint operator - () const
	{
		return CBCGPPoint(-x, -y, -z);
	}

	CBCGPPoint operator * (double value) const
	{
		return CBCGPPoint(x * value, y * value, z * value);
	}

	CBCGPPoint operator / (double value) const
	{
		_ASSERTE(value != 0.0);
		return CBCGPPoint(x / value, y / value, z / value);
	}

	operator CPoint() const
	{
		return CPoint((long)x, (long)y);
	}

public:
	double x;
	double y;
	double z;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPSize

class BCGCBPRODLLEXPORT AFX_NOVTABLE CBCGPSize
{
// constructors & destructors
public:
	CBCGPSize()
		: cx(0.)
		, cy(0.)
	{
	}

	CBCGPSize(double w, double h)
		: cx(w)
		, cy(h)
	{
	}

	CBCGPSize(const CBCGPSize& sz)
		: cx(sz.cx)
		, cy(sz.cy)
	{
	}

	CBCGPSize(const CSize& sz)
		: cx((double)sz.cx)
		, cy((double)sz.cy)
	{
	}

// public member function
public:
	bool IsNull() const
	{
		return cx == 0.0 && cy == 0.0;
	}

	bool IsEmpty() const
	{
		return cx == 0.0 || cy == 0.0;
	}

	bool IsValid() const
	{
		return cx >= 0.0 && cy >= 0.0;
	}

	double Width() const
	{
		return cx;
	}

	double Height() const
	{
		return cy;
	}

	void SetSize(double w, double h)
	{
		cx = w;
		cy = h;
	}

	void SetSize(const CBCGPSize& sz)
	{
		SetSize (sz.cx, sz.cy);
	}

	void SetSizeEmpty()
	{
		SetSize(0.0, 0.0);
	}

	void Inflate(double w, double h)
	{
		cx += w;
		cy += h;
	}

	void Inflate(const CBCGPSize& sz)
	{
		Inflate(sz.cx, sz.cy);
	}

	void Deflate(double w, double h)
	{
		cx -= w;
		cy -= h;
	}

	void Deflate(const CBCGPSize& sz)
	{
		Deflate(sz.cx, sz.cy);
	}

	void Swap()
	{
		double tmp = cx;
		cx = cy;
		cy = tmp;
	}

// public operators
public:
	const CBCGPSize& operator = (const CBCGPSize& sz)
	{
		cx = sz.cx;
		cy = sz.cy;
		return *this;
	}

	bool operator == (const CBCGPSize& sz) const
	{
		return cx == sz.cx && cy == sz.cy;
	}

	bool operator != (const CBCGPSize& sz) const
	{
		return cx != sz.cx || cy != sz.cy;
	}

	const CBCGPSize& operator += (const CBCGPSize& sz)
	{
		Inflate(sz.cx, sz.cy);
		return *this;
	}

	const CBCGPSize& operator -= (const CBCGPSize& sz)
	{
		Deflate(sz.cx, sz.cy);
		return *this;
	}

	const CBCGPSize& operator *= (double value)
	{
		cx *= value;
		cy *= value;
		return *this;
	}

	const CBCGPSize& operator /= (double value)
	{
		_ASSERTE(value != 0.0);
		cx /= value;
		cy /= value;
		return *this;
	}

	CBCGPSize operator + (const CBCGPSize& sz) const
	{
		return CBCGPSize(cx + sz.cx, cy + sz.cy);
	}

	CBCGPSize operator - (const CBCGPSize& sz) const
	{
		return CBCGPSize(cx - sz.cx, cy - sz.cy);
	}

	CBCGPSize operator * (double value) const
	{
		return CBCGPSize(cx * value, cy * value);
	}

	CBCGPSize operator / (double value) const
	{
		_ASSERTE(value != 0.0);
		return CBCGPSize(cx / value, cy / value);
	}

	operator CSize() const
	{
		return CSize((long)cx, (long)cy);
	}

public:
	double	cx;
	double	cy;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPRect

class BCGCBPRODLLEXPORT AFX_NOVTABLE CBCGPRect
{
// constructors & destructors
public:
	CBCGPRect()
		: left  (0.)
		, top   (0.)
		, right (0.)
		, bottom(0.)
	{
	}

	CBCGPRect(double l, double t, double r, double b)
		: left  (l)
		, top   (t)
		, right (r)
		, bottom(b)
	{
	}

	CBCGPRect(const CBCGPPoint& ptLT, const CBCGPPoint& ptRB)
		: left  (ptLT.x)
		, top   (ptLT.y)
		, right (ptRB.x)
		, bottom(ptRB.y)
	{
	}

	CBCGPRect(const CBCGPPoint& pt, const CBCGPSize& sz)
		: left  (pt.x)
		, top   (pt.y)
		, right (pt.x + sz.cx)
		, bottom(pt.y + sz.cy)
	{
	}

	CBCGPRect(const CBCGPRect& rt)
		: left  (rt.left)
		, top   (rt.top)
		, right (rt.right)
		, bottom(rt.bottom)
	{
	}

	CBCGPRect(const CRect& rt)
		: left  ((double)rt.left)
		, top   ((double)rt.top)
		, right ((double)rt.right)
		, bottom((double)rt.bottom)
	{
	}

// public member function
public:
	bool IsRectNull() const
	{
		return left == 0.0 && top == 0.0 && right == 0.0 && bottom == 0.0;
	}

	bool IsRectEmpty() const
	{
		return left == right || top == bottom;
	}

	bool IsValid() const
	{
		return left < right && top < bottom;
	}

	bool PtInRect(double x, double y) const
	{
		return left <= x && right >= x && top <= y && bottom >= y;
	}

	bool PtInRect(const CBCGPPoint& pt) const
	{
		return PtInRect (pt.x, pt.y);
	}

	bool RtInRect(double l, double t, double r, double b) const
	{
		return left <= l && right >= r && top <= t && bottom >= b;
	}

	bool RtInRect(const CBCGPRect& rt) const
	{
		return RtInRect(rt.left, rt.top, rt.right, rt.bottom);
	}

	void Normalize()
	{
		if (right < left)
		{
			SwapLeftRight();
		}

		if (bottom < top)
		{
			SwapTopBottom();
		}
	}

	CBCGPRect NormalizedRect () const
	{
		CBCGPRect rect(*this);
		rect.Normalize ();
		return rect;
	}

	double Width() const
	{
		return right - left;
	}

	void SetWidth(double w)
	{
		right = left + w;
	}

	double Height() const
	{
		return bottom - top;
	}

	void SetHeight(double h)
	{
		bottom = top + h;
	}

	void OffsetRect(double x, double y)
	{
		left   += x;
		right  += x;
		top    += y;
		bottom += y;
	}

	void OffsetRect(const CBCGPPoint& pt)
	{
		OffsetRect (pt.x, pt.y);
	}

	void OffsetRect(const CBCGPSize& sz)
	{
		OffsetRect (sz.cx, sz.cy);
	}

	void SetPoint(double x, double y)
	{
		OffsetRect(x - left, y - top);
	}

	void SetPoint(const CBCGPPoint& pt)
	{
		SetPoint (pt.x, pt.y);
	}

	void SetSize(double w, double h)
	{
		right  = left + w;
		bottom = top  + h;
	}

	void SetSize(const CBCGPSize& sz)
	{
		SetSize (sz.cx, sz.cy);
	}

	CBCGPSize Size() const
	{
		return CBCGPSize(Width(), Height());
	}

	void SetRect(double l, double t, double r, double b)
	{
		left   = l;
		top    = t;
		right  = r;
		bottom = b;
	}

	void SetRect(const CBCGPPoint& ptLT, const CBCGPPoint& ptRB)
	{
		SetRect (ptLT.x, ptLT.y, ptRB.x, ptRB.y);
	}

	void SetRect(const CBCGPPoint& pt, const CBCGPSize& sz)
	{
		SetRect(pt.x, pt.y, pt.x + sz.cx, pt.y + sz.cy);
	}

	void SetRect(const CBCGPRect& rt)
	{
		SetRect(rt.left, rt.top, rt.right, rt.bottom);
	}

	CBCGPPoint CenterPoint() const
	{
		return CBCGPPoint((left + right) / 2.0, (top + bottom) / 2.0);
	}

	double Length() const
	{
		CBCGPSize sz(Size());
		return 2.0 * ((sz.cx < 0.0 ? -sz.cx : sz.cx) + (sz.cy < 0.0 ? -sz.cy : sz.cy));
	}

	double Area() const
	{
		double a = Width() * Height();
		if (a < 0.0)
		{
			a = -a;
		}

		return a;
	}

	BOOL IntersectRect(const CBCGPRect& rt)
	{
		*this &= rt;
		return !IsRectEmpty();
	}

	BOOL IntersectRect(const CBCGPRect& rt1, const CBCGPRect& rt2)
	{
		*this = rt1 & rt2;
		return !IsRectEmpty();
	}

	void UnionRect(const CBCGPRect& rt)
	{
		*this |= rt;
	}

	void UnionRect(const CBCGPRect& rt1, const CBCGPRect& rt2)
	{
		*this = rt1 | rt2;
	}

	CBCGPPoint TopLeft() const
	{
		return CBCGPPoint(left, top);
	}

	CBCGPPoint BottomRight() const
	{
		return CBCGPPoint(right, bottom);
	}

	CBCGPPoint TopRight() const
	{
		return CBCGPPoint(right, top);
	}

	CBCGPPoint BottomLeft() const
	{
		return CBCGPPoint(left, bottom);
	}

	void InflateRect(double l, double t, double r, double b)
	{
		left   -= l;
		top    -= t;
		right  += r;
		bottom += b;
	}

	void InflateRect(double x, double y)
	{
		InflateRect (x, y, x, y);
	}

	void InflateRect(const CBCGPPoint& pt)
	{
		InflateRect (pt.x, pt.y, pt.x, pt.y);
	}

	void InflateRect(const CBCGPSize& sz)
	{
		InflateRect (sz.cx, sz.cy, sz.cx, sz.cy);
	}

	void InflateRect(const CBCGPRect& rt)
	{
		InflateRect (rt.left, rt.top, rt.right, rt.bottom);
	}

	void DeflateRect(double l, double t, double r, double b)
	{
		left   += l;
		top    += t;
		right  -= r;
		bottom -= b;
	}

	void DeflateRect(double x, double y)
	{
		DeflateRect (x, y, x, y);
	}

	void DeflateRect(const CBCGPPoint& pt)
	{
		DeflateRect (pt.x, pt.y, pt.x, pt.y);
	}

	void DeflateRect(const CBCGPSize& sz)
	{
		DeflateRect (sz.cx, sz.cy, sz.cx, sz.cy);
	}

	void DeflateRect(const CBCGPRect& rt)
	{
		DeflateRect (rt.left, rt.top, rt.right, rt.bottom);
	}

	void SwapLeftRight()
	{
		double tmp = left;
		left = right;
		right = tmp;
	}

	void SwapTopBottom()
	{
		double tmp = top;
		top = bottom;
		bottom = tmp;
	}

	void Swap()
	{
		SwapLeftRight ();
		SwapTopBottom ();
	}

	void SetRectEmpty()
	{
		left = right = top = bottom = 0.;
	}

// public operators
public:
	const CBCGPRect& operator = (const CBCGPRect& rt)
	{
		left   = rt.left;
		top    = rt.top;
		right  = rt.right;
		bottom = rt.bottom;
		return *this;
	}

	bool operator == (const CBCGPRect& rt) const
	{
		return left == rt.left && top == rt.top && right == rt.right && bottom == rt.bottom;
	}

	bool operator != (const CBCGPRect& rt) const
	{
		return !(*this == rt);
	}

	const CBCGPRect& operator &= (const CBCGPRect& rt)
	{
		*this = *this & rt;
		return *this;
	}

	const CBCGPRect& operator |= (const CBCGPRect& rt)
	{
		*this = *this | rt;
		return *this;
	}

	const CBCGPRect& operator += (const CBCGPPoint& pt)
	{
		OffsetRect (pt.x, pt.y);
		return *this;
	}

	const CBCGPRect& operator -= (const CBCGPPoint& pt)
	{
		OffsetRect (-pt.x, -pt.y);
		return *this;
	}

	const CBCGPRect& operator += (const CBCGPSize& sz)
	{
		SetSize (Width() + sz.cx, Height() + sz.cy);
		return *this;
	}

	const CBCGPRect& operator -= (const CBCGPSize& sz)
	{
		SetSize (Width() - sz.cx, Height() - sz.cy);
		return *this;
	}

	CBCGPRect operator & (const CBCGPRect& rt) const
	{
		CBCGPRect tmp;
		tmp.left   = max(left  , rt.left  );
		tmp.right  = min(right , rt.right );
		tmp.top    = max(top   , rt.top   );
		tmp.bottom = min(bottom, rt.bottom);

		if (tmp.left >= tmp.right || tmp.top >= tmp.bottom)
		{
			tmp.SetRectEmpty();
		}

		return tmp;
	}

	CBCGPRect operator | (const CBCGPRect& rt) const
	{
		if (IsValid())
		{
			if (rt.IsValid())
			{
				CBCGPRect tmp;
				tmp.left   = min(left  , rt.left  );
				tmp.right  = max(right , rt.right );
				tmp.top    = min(top   , rt.top   );
				tmp.bottom = max(bottom, rt.bottom);
				return tmp;
			}
			else
			{
				return *this;
			}
		}
		else
		{
			CBCGPRect tmp(rt);
			return tmp;
		}
	}

	CBCGPRect operator + (const CBCGPPoint& pt) const
	{
		CBCGPRect tmp;
		tmp.left   = left   + pt.x;
		tmp.right  = right  + pt.x;
		tmp.top    = top    + pt.y;
		tmp.bottom = bottom + pt.y;
		return tmp;
	}

	CBCGPRect operator - (const CBCGPPoint& pt) const
	{
		CBCGPRect tmp;
		tmp.left   = left   - pt.x;
		tmp.right  = right  - pt.x;
		tmp.top    = top    - pt.y;
		tmp.bottom = bottom - pt.y;
		return tmp;
	}

	operator CRect() const
	{
		return CRect((long)left, (long)top, (long)right, (long)bottom);
	}

public:
	double	left;
	double	right;
	double	top;
	double	bottom;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPRoundedRect

class BCGCBPRODLLEXPORT AFX_NOVTABLE CBCGPRoundedRect
{
// constructors & destructors
public:
	CBCGPRoundedRect()
		: radiusX(0)
		, radiusY(0)
	{
	}

	CBCGPRoundedRect(const CBCGPRect& rectSrc, double radiusXSrc, double radiusYSrc)
		: rect   (rectSrc)
		, radiusX(radiusXSrc)
		, radiusY(radiusYSrc)
	{
	}

	CBCGPRoundedRect(double l, double t, double r, double b, double radiusXSrc, double radiusYSrc)
		: rect   (l, t, r, b)
		, radiusX(radiusXSrc)
		, radiusY(radiusYSrc)
	{
	}

public:
	BOOL IsEmpty() const
	{
		return rect.IsRectEmpty() || radiusX == 0. || radiusY == 0.;
	}

public:
	CBCGPRect	rect;
	double		radiusX;
	double		radiusY;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPEllipse

class BCGCBPRODLLEXPORT AFX_NOVTABLE CBCGPEllipse
{
// constructors & destructors
public:
	CBCGPEllipse()
		: radiusX(0)
		, radiusY(0)
	{
	}

	CBCGPEllipse(const CBCGPPoint& ptSrc, double radiusXSrc, double radiusYSrc)
		: point  (ptSrc)
		, radiusX(radiusXSrc)
		, radiusY(radiusYSrc)
	{
	}

	CBCGPEllipse(const CBCGPPoint& ptSrc, const CBCGPSize& szSrc)
		: point  (ptSrc)
		, radiusX(szSrc.cx * .5)
		, radiusY(szSrc.cy * .5)
	{
	}

	CBCGPEllipse(const CBCGPRect& rect)
		: point  (rect.CenterPoint())
		, radiusX(rect.Width() * .5)
		, radiusY(rect.Height() * .5)
	{
	}

public:
	BOOL IsNull() const
	{
		return radiusX <= 0. || radiusY <= 0.;
	}

	operator CBCGPRect() const { return CBCGPRect(point.x - radiusX, point.y - radiusY, point.x + radiusX, point.y + radiusY); }

public:
	CBCGPPoint	point;
	double		radiusX;
	double		radiusY;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPPointsArray

class BCGCBPRODLLEXPORT CBCGPPointsArray : public CArray<CBCGPPoint, const CBCGPPoint&>
{
public:
	CBCGPPointsArray() {}
	CBCGPPointsArray(int nNewSize, int nGrowBy = -1)
	{
		SetSize(nNewSize, nGrowBy);
	}

	~CBCGPPointsArray() {}

	CBCGPRect GetBoundsRect() const;
	CBCGPPoint GetBoundsCenter() const
	{
		return GetBoundsRect ().CenterPoint ();
	}

	void StoreBoundsRect(const CBCGPRect& rect) {m_rectBounds = rect;}

	void Multiply(double value)
	{
		Multiply(value, value, value);
	}

	void Multiply(double valueX, double valueY, double valueZ = 1.0)
	{
		for(int i = 0; i < GetSize(); i++)
		{
			CBCGPPoint& xpt = (*this)[i];
			xpt.x *= valueX;
			xpt.y *= valueY;
			xpt.z *= valueZ;
		}
	}

	void Offset(const CBCGPPoint& pt)
	{
		Offset(pt.x, pt.y, pt.z);
	}

	void Offset(double valueX, double valueY, double valueZ = 0.0)
	{
		for(int i = 0; i < GetSize(); i++)
		{
			CBCGPPoint& xpt = (*this)[i];
			xpt.x += valueX;
			xpt.y += valueY;
			xpt.z += valueZ;
		}
	}

	void Scale(double value)
	{
		Scale(value, value);
	}

	void Scale(double valueX, double valueY)
	{
		Scale(GetBoundsCenter(), valueX, valueY);
	}

	void Scale(const CBCGPPoint& pt, double valueX, double valueY)
	{
		// multiply & translate
		for(int i = 0; i < GetSize(); i++)
		{
			CBCGPPoint& xpt = (*this)[i];
			xpt.x = pt.x + (xpt.x - pt.x) * valueX;
			xpt.y = pt.y + (xpt.y - pt.y) * valueY;
		}
	}

	void Rotate(double angle)
	{
		Rotate(GetBoundsCenter(), angle);
	}

	void Rotate(const CBCGPPoint& pt, double angle);

	const CBCGPPointsArray& operator +=(const CBCGPPoint& pt)
	{
		Offset(pt.x, pt.y, pt.z);
		return *this;
	}

	const CBCGPPointsArray& operator -=(const CBCGPPoint& pt)
	{
		Offset(-pt.x, -pt.y, -pt.z);
		return *this;
	}

	const CBCGPPointsArray& operator *=(double value)
	{
		Multiply(value, value, value);
		return *this;
	}

	const CBCGPPointsArray& operator /=(double value)
	{
		_ASSERTE(value != 0.0);
		value = 1.0 / value;
		Multiply(value, value, value);
		return *this;
	}

	const CBCGPPointsArray& operator=(const CBCGPPointsArray& src)
	{
		Copy(src);
		return *this;
	}
protected:
	CBCGPRect m_rectBounds;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPGraphicsResource

class CBCGPGraphicsManager;

class BCGCBPRODLLEXPORT CBCGPGraphicsResource : public CObject
{
	DECLARE_DYNAMIC(CBCGPGraphicsResource)

protected:
	CBCGPGraphicsResource()
	{
		m_lpHandle1 = m_lpHandle2 = m_lpHandle = NULL;
		m_pGM = NULL;
		m_bIsTemporary = FALSE;
	}

	virtual ~CBCGPGraphicsResource()
	{
	}

public:
	virtual BOOL IsNull() const
	{
		return m_lpHandle == NULL;
	};

	LPVOID GetHandle() const
	{
		return m_lpHandle;
	}

	LPVOID GetHandle1() const
	{
		return m_lpHandle1;
	}

	LPVOID GetHandle2() const
	{
		return m_lpHandle2;
	}

	CBCGPGraphicsManager* GetGraphicsManager() const
	{
		return m_pGM;
	}

	void Set(CBCGPGraphicsManager* pGM, LPVOID lpHandle, LPVOID lpHandle1 = NULL, LPVOID lpHandle2 = NULL);

	virtual void Destroy(BOOL bDetachFromGM = TRUE) = 0;

	LPVOID Detach()
	{
		LPVOID lpHandle = m_lpHandle;
		m_lpHandle = NULL;
		return lpHandle;
	}

	void SetTemporary()
	{
		m_bIsTemporary = TRUE;
	}

	BOOL IsTemporary() const
	{
		return m_bIsTemporary;
	}

protected:
	LPVOID					m_lpHandle;
	CBCGPGraphicsManager*	m_pGM;
	LPVOID					m_lpHandle1;
	LPVOID					m_lpHandle2;
	BOOL					m_bIsTemporary;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPGeometry

class BCGCBPRODLLEXPORT CBCGPGeometry : public CBCGPGraphicsResource
{
	DECLARE_DYNAMIC(CBCGPGeometry)

public:
	CBCGPGeometry()
	{
	}

	virtual ~CBCGPGeometry()
	{
		Destroy();
	}

	virtual void Destroy(BOOL bDetachFromGM = TRUE);
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPRectangleGeometry

class BCGCBPRODLLEXPORT CBCGPRectangleGeometry : public CBCGPGeometry
{
	DECLARE_DYNAMIC(CBCGPRectangleGeometry)

public:
	CBCGPRectangleGeometry(const CBCGPRect& rect)
	{
		m_rect = rect;
	}

	const CBCGPRect& GetRectangle() const
	{
		return m_rect;
	}

	virtual BOOL IsNull() const
	{
		return m_rect.IsRectNull() && m_lpHandle == NULL;
	}

protected:
	CBCGPRect	m_rect;
};

////////////////////////////////////////////////////////////////////////////////////
//  CBCGPRoundedRectangleGeometry

class BCGCBPRODLLEXPORT CBCGPRoundedRectangleGeometry : public CBCGPGeometry
{
	DECLARE_DYNAMIC(CBCGPRoundedRectangleGeometry)

public:
	CBCGPRoundedRectangleGeometry(const CBCGPRoundedRect& rectRounded) :
		m_rectRounded(rectRounded)
	{
		m_rectRounded = rectRounded;
	}

	const CBCGPRoundedRect& GetRoundedRect() const
	{
		return m_rectRounded;
	}

	virtual BOOL IsNull() const
	{
		return m_rectRounded.IsEmpty();
	}

protected:
	CBCGPRoundedRect	m_rectRounded;
};

////////////////////////////////////////////////////////////////////////////////////
//  CBCGPEllipseGeometry

class BCGCBPRODLLEXPORT CBCGPEllipseGeometry : public CBCGPGeometry
{
	DECLARE_DYNAMIC(CBCGPEllipseGeometry)

public:
	CBCGPEllipseGeometry(const CBCGPEllipse& ellipse)
	{
		m_Ellipse = ellipse;
	}

	CBCGPEllipseGeometry(const CBCGPRect& rect) :
		m_Ellipse(rect.CenterPoint(), rect.Width() / 2., rect.Height() / 2.)
	{
	}

	const CBCGPEllipse& GetEllipse() const
	{
		return m_Ellipse;
	}

	virtual BOOL IsNull() const
	{
		return m_Ellipse.IsNull() && m_lpHandle == NULL;
	}

protected:
	CBCGPEllipse	m_Ellipse;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPPolygonGeometry

class BCGCBPRODLLEXPORT CBCGPPolygonGeometry : public CBCGPGeometry
{
	DECLARE_DYNAMIC(CBCGPPolygonGeometry)

public:
	enum BCGP_CURVE_TYPE
	{
		BCGP_CURVE_TYPE_LINE,
		BCGP_CURVE_TYPE_BEZIER,
	};

	CBCGPPolygonGeometry()
	{
		m_bIsClosed = TRUE;
		m_curveType = BCGP_CURVE_TYPE_LINE;
	}

	CBCGPPolygonGeometry(const CBCGPPointsArray& arPoints, BOOL bIsClosed = TRUE,
		BCGP_CURVE_TYPE curveType = BCGP_CURVE_TYPE_LINE)
	{
		SetPoints(arPoints, bIsClosed, curveType);
	}

	void SetPoints(const CBCGPPointsArray& arPoints, BOOL bIsClosed = TRUE, 
		BCGP_CURVE_TYPE curveType = BCGP_CURVE_TYPE_LINE)
	{
		Clear();

		m_arPoints.Copy(arPoints);

		m_bIsClosed = bIsClosed;
		m_curveType = curveType;
	}

	const CBCGPPointsArray& GetPoints() const
	{
		return m_arPoints;
	}

	void Clear();

	virtual BOOL IsNull() const
	{
		return m_arPoints.GetSize() == 0 && m_lpHandle == NULL;
	}

	BOOL IsClosed() const
	{
		return m_bIsClosed;
	}

	BCGP_CURVE_TYPE GetCurveType() const
	{
		return m_curveType;
	}
	
protected:
	CBCGPPointsArray	m_arPoints;
	BOOL				m_bIsClosed;
	BCGP_CURVE_TYPE		m_curveType;
};

class BCGCBPRODLLEXPORT CBCGPSplineGeometry : public CBCGPGeometry
{
	DECLARE_DYNAMIC(CBCGPSplineGeometry)

	enum BCGP_SPLINE_TYPE
	{
		BCGP_SPLINE_TYPE_KB,		// Kochanek-Bartels spline
		BCGP_SPLINE_TYPE_HERMITE,	// Hermite spline
	};

public:
	CBCGPSplineGeometry()
	{
		m_bIsClosed = TRUE;
	}

	CBCGPSplineGeometry(const CBCGPPointsArray& arPoints, BCGP_SPLINE_TYPE type, 
		BOOL bIsClosed = TRUE, double tension = 0.0, double bias = 0.0, double continuity = 0.0)
	{
		SetPoints(arPoints, type, bIsClosed, tension, bias, continuity);
	}

	void SetPoints(const CBCGPPointsArray& arPoints, BCGP_SPLINE_TYPE type, 
		BOOL bIsClosed = TRUE, double tension = 0.0, double bias = 0.0, double continuity = 0.0);

	void SetPoints(const CBCGPPointsArray& arPoints, BOOL bClosed = TRUE)
	{
		m_arPoints.RemoveAll();
		m_arPoints.Append(arPoints);
		m_bIsClosed = bClosed;
	}

	const CBCGPPointsArray& GetPoints() const
	{
		return m_arPoints;
	}

	void Clear();

	virtual BOOL IsNull() const
	{
		return m_arPoints.GetSize() == 0 && m_lpHandle == NULL;
	}

	BOOL IsClosed() const
	{
		return m_bIsClosed;
	}
	
protected:
	CBCGPPointsArray	m_arPoints;
	BOOL				m_bIsClosed;
};

class BCGCBPRODLLEXPORT CBCGPLineSegment : public CObject
{
	DECLARE_DYNAMIC(CBCGPLineSegment)

public:
	CBCGPLineSegment(const CBCGPPoint& point)
	{
		m_Point = point;
	}

	CBCGPPoint	m_Point;
};

class BCGCBPRODLLEXPORT CBCGPBezierSegment : public CObject
{
	DECLARE_DYNAMIC(CBCGPBezierSegment)

public:
	CBCGPBezierSegment(const CBCGPPoint& point1, const CBCGPPoint& point2, const CBCGPPoint& point3)
	{
		m_Point1 = point1;
		m_Point2 = point2;
		m_Point3 = point3;
	}

	CBCGPPoint	m_Point1;
	CBCGPPoint	m_Point2;
	CBCGPPoint	m_Point3;
};

class BCGCBPRODLLEXPORT CBCGPArcSegment : public CObject
{
	DECLARE_DYNAMIC(CBCGPArcSegment)

public:
	CBCGPArcSegment(const CBCGPPoint& point, const CBCGPSize& radius, BOOL bIsClockwise, BOOL bIsLargeArc, double dblRotationAngle)
	{
		m_Point = point;
		m_Radius = radius;
		m_bIsClockwise = bIsClockwise;
		m_bIsLargeArc = bIsLargeArc;
		m_dblRotationAngle = dblRotationAngle;
	}

	CBCGPPoint GetArcCenter(const CBCGPPoint& ptFrom, BOOL& bIsLargeArc, double& rX, double& rY) const;

	CBCGPPoint	m_Point;
	CBCGPSize	m_Radius;
	BOOL		m_bIsClockwise;
	BOOL		m_bIsLargeArc;
	double		m_dblRotationAngle;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPComplexGeometry

class BCGCBPRODLLEXPORT CBCGPComplexGeometry : public CBCGPGeometry
{
	DECLARE_DYNAMIC(CBCGPComplexGeometry)

public:
	CBCGPComplexGeometry(const CBCGPPoint& ptStart = CBCGPPoint(-1., -1.), BOOL bIsClosed = TRUE)
	{
		m_ptStart = ptStart;
		m_bIsClosed = bIsClosed;
	}

	virtual ~CBCGPComplexGeometry()
	{
		Clear();
	}

	void SetClosed(BOOL bIsClosed = TRUE)
	{
		m_bIsClosed = bIsClosed;
	}

	BOOL IsClosed() const
	{
		return m_bIsClosed;
	}

	void SetStart(const CBCGPPoint& ptStart)
	{
		m_ptStart = ptStart;
	}

	void AddLine(const CBCGPPoint& pt);
	void AddBezier(const CBCGPPoint& pt1, const CBCGPPoint& pt2, const CBCGPPoint& pt3);
	void AddArc(const CBCGPPoint& pt, const CBCGPSize szRadius, 
		BOOL bIsClockwise = TRUE, BOOL bIsLargeArc = FALSE, double dblRotationAngle = 0.);
	void AddPoints(const CBCGPPointsArray& arPoints, CBCGPPolygonGeometry::BCGP_CURVE_TYPE curveType);

	void Clear();

	const CBCGPPoint& GetStartPoint() const
	{
		return m_ptStart;
	}

	const CObArray& GetSegments() const
	{
		return m_arSegments;
	}

	virtual BOOL IsEmpty() const
	{
		return m_arSegments.GetSize() == 0;
	}

	virtual BOOL IsNull() const
	{
		return m_arSegments.GetSize() == 0 && m_lpHandle == NULL;
	}

protected:
	CBCGPPoint	m_ptStart;
	CObArray	m_arSegments;
	BOOL		m_bIsClosed;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPColor

class BCGCBPRODLLEXPORT CBCGPColor
{
public:
    enum BCGP_COLOR
    {
        AliceBlue = 0xF0F8FF,
        AntiqueWhite = 0xFAEBD7,
        Aqua = 0x00FFFF,
        Aquamarine = 0x7FFFD4,
        Azure = 0xF0FFFF,
        Beige = 0xF5F5DC,
        Bisque = 0xFFE4C4,
        Black = 0x000000,
        BlanchedAlmond = 0xFFEBCD,
        Blue = 0x0000FF,
        BlueViolet = 0x8A2BE2,
        Brown = 0xA52A2A,
        BurlyWood = 0xDEB887,
        CadetBlue = 0x5F9EA0,
        Chartreuse = 0x7FFF00,
        Chocolate = 0xD2691E,
        Coral = 0xFF7F50,
        CornflowerBlue = 0x6495ED,
        Cornsilk = 0xFFF8DC,
        Crimson = 0xDC143C,
        Cyan = 0x00FFFF,
        DarkBlue = 0x00008B,
        DarkCyan = 0x008B8B,
        DarkGoldenrod = 0xB8860B,
        DarkGray = 0xA9A9A9,
        DarkGreen = 0x006400,
        DarkKhaki = 0xBDB76B,
        DarkMagenta = 0x8B008B,
        DarkOliveGreen = 0x556B2F,
        DarkOrange = 0xFF8C00,
        DarkOrchid = 0x9932CC,
        DarkRed = 0x8B0000,
        DarkSalmon = 0xE9967A,
        DarkSeaGreen = 0x8FBC8F,
        DarkSlateBlue = 0x483D8B,
        DarkSlateGray = 0x2F4F4F,
        DarkTurquoise = 0x00CED1,
        DarkViolet = 0x9400D3,
        DeepPink = 0xFF1493,
        DeepSkyBlue = 0x00BFFF,
        DimGray = 0x696969,
        DodgerBlue = 0x1E90FF,
        Firebrick = 0xB22222,
        FloralWhite = 0xFFFAF0,
        ForestGreen = 0x228B22,
        Fuchsia = 0xFF00FF,
        Gainsboro = 0xDCDCDC,
        GhostWhite = 0xF8F8FF,
        Gold = 0xFFD700,
        Goldenrod = 0xDAA520,
        Gray = 0x808080,
        Green = 0x008000,
        GreenYellow = 0xADFF2F,
        Honeydew = 0xF0FFF0,
        HotPink = 0xFF69B4,
        IndianRed = 0xCD5C5C,
        Indigo = 0x4B0082,
        Ivory = 0xFFFFF0,
        Khaki = 0xF0E68C,
        Lavender = 0xE6E6FA,
        LavenderBlush = 0xFFF0F5,
        LawnGreen = 0x7CFC00,
        LemonChiffon = 0xFFFACD,
        LightBlue = 0xADD8E6,
        LightCoral = 0xF08080,
        LightCyan = 0xE0FFFF,
        LightGoldenrodYellow = 0xFAFAD2,
        LightGreen = 0x90EE90,
        LightGray = 0xD3D3D3,
        LightPink = 0xFFB6C1,
        LightSalmon = 0xFFA07A,
        LightSeaGreen = 0x20B2AA,
        LightSkyBlue = 0x87CEFA,
        LightSlateGray = 0x778899,
        LightSteelBlue = 0xB0C4DE,
        LightYellow = 0xFFFFE0,
        Lime = 0x00FF00,
        LimeGreen = 0x32CD32,
        Linen = 0xFAF0E6,
        Magenta = 0xFF00FF,
        Maroon = 0x800000,
        MediumAquamarine = 0x66CDAA,
        MediumBlue = 0x0000CD,
        MediumOrchid = 0xBA55D3,
        MediumPurple = 0x9370DB,
        MediumSeaGreen = 0x3CB371,
        MediumSlateBlue = 0x7B68EE,
        MediumSpringGreen = 0x00FA9A,
        MediumTurquoise = 0x48D1CC,
        MediumVioletRed = 0xC71585,
        MidnightBlue = 0x191970,
        MintCream = 0xF5FFFA,
        MistyRose = 0xFFE4E1,
        Moccasin = 0xFFE4B5,
        NavajoWhite = 0xFFDEAD,
        Navy = 0x000080,
        OldLace = 0xFDF5E6,
        Olive = 0x808000,
        OliveDrab = 0x6B8E23,
        Orange = 0xFFA500,
        OrangeRed = 0xFF4500,
        Orchid = 0xDA70D6,
        PaleGoldenrod = 0xEEE8AA,
        PaleGreen = 0x98FB98,
        PaleTurquoise = 0xAFEEEE,
        PaleVioletRed = 0xDB7093,
        PapayaWhip = 0xFFEFD5,
        PeachPuff = 0xFFDAB9,
        Peru = 0xCD853F,
        Pink = 0xFFC0CB,
        Plum = 0xDDA0DD,
        PowderBlue = 0xB0E0E6,
        Purple = 0x800080,
        Red = 0xFF0000,
        RosyBrown = 0xBC8F8F,
        RoyalBlue = 0x4169E1,
        SaddleBrown = 0x8B4513,
        Salmon = 0xFA8072,
        SandyBrown = 0xF4A460,
        SeaGreen = 0x2E8B57,
        SeaShell = 0xFFF5EE,
        Sienna = 0xA0522D,
        Silver = 0xC0C0C0,
        SkyBlue = 0x87CEEB,
        SlateBlue = 0x6A5ACD,
        SlateGray = 0x708090,
        Snow = 0xFFFAFA,
        SpringGreen = 0x00FF7F,
        SteelBlue = 0x4682B4,
        Tan = 0xD2B48C,
        Teal = 0x008080,
        Thistle = 0xD8BFD8,
        Tomato = 0xFF6347,
        Turquoise = 0x40E0D0,
        Violet = 0xEE82EE,
        Wheat = 0xF5DEB3,
        White = 0xFFFFFF,
        WhiteSmoke = 0xF5F5F5,
        Yellow = 0xFFFF00,
        YellowGreen = 0x9ACD32,
    };

	CBCGPColor()
	{
		r = g = b = 0.;
		a = 0.;
	}

	CBCGPColor(double rSrc, double gSrc, double bSrc, double aSrc = 1.)
	{
		ASSERT(rSrc >= 0. && rSrc <= 1.);
		ASSERT(gSrc >= 0. && gSrc <= 1.);
		ASSERT(bSrc >= 0. && bSrc <= 1.);
		ASSERT(aSrc >= 0. && aSrc <= 1.);

		r = rSrc;
		g = gSrc;
		b = bSrc;
		a = aSrc;
	}

	CBCGPColor(BCGP_COLOR color, double aSrc = 1.)
	{
		ASSERT(aSrc >= 0. && aSrc <= 1.);

		r = GetBValue((COLORREF)color) / 255.0f;
		g = GetGValue((COLORREF)color) / 255.0f;
		b = GetRValue((COLORREF)color) / 255.0f;
		a = aSrc;
	}

	CBCGPColor(COLORREF color, double aSrc = 1.)
	{
		SetRGB(color, aSrc);
	}

	BOOL operator == (const CBCGPColor& rOther) const
	{
		return r == rOther.r && g == rOther.g && b == rOther.b && a == rOther.a;
	}
	BOOL operator != (const CBCGPColor& rOther) const
	{
		return r != rOther.r || g != rOther.g || b != rOther.b || a != rOther.a;
	}

	operator COLORREF() const;

	BOOL IsNull() const
	{
		return r == 0. && g == 0. && b == 0. && a == 0.;
	}

	void SetRGB(COLORREF color, double aSrc = 1.)
	{
		ASSERT(aSrc >= 0. && aSrc <= 1.);

		r = GetRValue(color) / 255.0f;
		g = GetGValue(color) / 255.0f;
		b = GetBValue(color) / 255.0f;
		a = aSrc;
	}

	void MakeLighter(double dblRatio = .1);
	void MakeDarker(double dblRatio = .1);
	void MakePale(double dblLum = .97);

#ifndef _BCGPCHART_STANDALONE
	static const CArray<COLORREF, COLORREF>& GetRGBArray();
#endif

	double	r;
	double	g;
	double	b;
	double	a;

protected:

#ifndef _BCGPCHART_STANDALONE
	static void InsertColorSorted(BCGP_COLOR colorName, const CString& strColorLabel = _T(""));
#endif
	static CArray<COLORREF, COLORREF>	m_arColors;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPBrush

class BCGCBPRODLLEXPORT CBCGPBrush : public CBCGPGraphicsResource
{
public:
	enum BCGP_GRADIENT_TYPE
	{
		BCGP_NO_GRADIENT = 0,
		BCGP_GRADIENT_HORIZONTAL,
		BCGP_GRADIENT_VERTICAL,
		BCGP_GRADIENT_DIAGONAL_LEFT,
		BCGP_GRADIENT_DIAGONAL_RIGHT,
		BCGP_GRADIENT_CENTER_HORIZONTAL,
		BCGP_GRADIENT_CENTER_VERTICAL,
		BCGP_GRADIENT_RADIAL_TOP,
		BCGP_GRADIENT_RADIAL_CENTER,
		BCGP_GRADIENT_RADIAL_BOTTOM,
		BCGP_GRADIENT_RADIAL_LEFT,
		BCGP_GRADIENT_RADIAL_RIGHT,
		BCGP_GRADIENT_RADIAL_TOP_LEFT,
		BCGP_GRADIENT_RADIAL_TOP_RIGHT,
		BCGP_GRADIENT_RADIAL_BOTTOM_LEFT,
		BCGP_GRADIENT_RADIAL_BOTTOM_RIGHT,
		BCGP_GRADIENT_BEVEL,
		BCGP_GRADIENT_PIPE_VERTICAL,
		BCGP_GRADIENT_PIPE_HORIZONTAL,
	};

public:
	CBCGPBrush();
	CBCGPBrush(const CBCGPColor& color, double opacity = 1.);
	CBCGPBrush(const CBCGPColor& color, const CBCGPColor& colorGradient, 
		BCGP_GRADIENT_TYPE gradientType, double opacity = 1.);
	CBCGPBrush(const CBCGPColor& color, BCGP_GRADIENT_TYPE gradientType, double opacity = 1.);

	CBCGPBrush(const CBCGPBrush& brush);

	virtual ~CBCGPBrush()
	{
		Destroy();
	}

	virtual void Destroy(BOOL bDetachFromGM = TRUE);

	CBCGPBrush& operator = (const CBCGPBrush& src)
	{
		CopyFrom(src);
		return *this;
	}

public:
	const BOOL IsEmpty() const
	{
		return m_color.IsNull() && m_colorGradient.IsNull();
	}

	void Empty()
	{
		SetColor(CBCGPColor());
	}

	BOOL IsTransparent() const
	{
		return (m_opacity < 1.) || (m_color.a < 1.) || (m_gradientType != BCGP_NO_GRADIENT && m_colorGradient.a < 1.);
	}

	const CBCGPColor& GetColor() const
	{
		return m_color;
	}

	const CBCGPColor& GetGradientColor() const
	{
		return m_colorGradient;
	}

	BCGP_GRADIENT_TYPE GetGradientType() const
	{
		return m_gradientType;
	}

	double GetOpacity() const
	{
		return m_opacity;
	}

	int GetPenWidth() const
	{
		return m_nPenWidth;
	}

	int GetPenStyle() const
	{
		return m_nPenStyle;
	}

	void SetColor(const CBCGPColor& color, double opacity = 1.);
	void SetColors(const CBCGPColor& color, const CBCGPColor& colorGradient, 
		BCGP_GRADIENT_TYPE gradientType, double opacity = 1.);

	void MakeLighter(double dblRatio = .1);
	void MakeDarker(double dblRatio = .1);
	void MakePale(double dblLum = .97);

	void SetPenAttributes(int nWidth, int nStyle);

	CBCGPToolBarImages& GetImage()
	{
		return m_Image;
	}

	void CopyFrom(const CBCGPBrush& src);

protected:
	void CommonInit();

	double				m_opacity;
	CBCGPColor			m_color;
	CBCGPColor			m_colorGradient;
	BCGP_GRADIENT_TYPE	m_gradientType;
	int					m_nPenWidth;
	int					m_nPenStyle;
	CBCGPToolBarImages	m_Image;
};

class BCGCBPRODLLEXPORT CBCGPImage : public CBCGPGraphicsResource
{
public:
	enum BCGP_IMAGE_INTERPOLATION_MODE
	{
		BCGP_IMAGE_INTERPOLATION_MODE_NEAREST_NEIGHBOR = 0,
		BCGP_IMAGE_INTERPOLATION_MODE_LINEAR
	};

// Construction:
public:
	CBCGPImage(UINT uiResID = 0, LPCTSTR lpszType = NULL);
	CBCGPImage(const CString& strPath);
	CBCGPImage(const CBCGPImage& srcImage);
	CBCGPImage(HICON hIcon, CSize sizeIcon = CSize(16, 16), BOOL bIsAlpha = FALSE);
	CBCGPImage(HBITMAP hBmp, BOOL bIsIgnoreAlpha = FALSE);

	virtual ~CBCGPImage()
	{
		Destroy();
	}

protected:
	void CommonInit();

// Attributes:
public:
	UINT GetResourceID() const
	{
		return m_uiResID;
	}

	LPCTSTR GetResourceType() const
	{
		return m_szType;
	}

	LPCTSTR GetPath() const
	{
		return m_strPath;
	}

	CBCGPSize GetSize(CBCGPGraphicsManager* pGM = 0) const;

	HICON GetIcon() const
	{
		return m_hIcon;
	}

	CSize GetIconSize() const
	{
		return m_sizeIcon;
	}

	BOOL IsAlphaIcon() const
	{
		return m_bIsAlphaIcon;
	}

	HBITMAP GetBitmap() const
	{
		return m_hBitmap;
	}

	BOOL IsIgnoreAlphaBitmap() const
	{
		return m_bIsIgnoreAlphaBitmap;
	}

// Operations:
public:
	BOOL CopyTo(CBCGPImage& imageDest, const CBCGPRect& rectSrc = CBCGPRect()) const;
	virtual void Destroy(BOOL bDetachFromGM = TRUE);

	void CopyFrom(const CBCGPImage& srcImage);

	void Load(UINT uiResID = 0, LPCTSTR lpszType = NULL);
	void Load(const CString& strPath);

	CBCGPImage& operator = (const CBCGPImage& image);

	HBITMAP ExportToBitmap();

protected:
	UINT	m_uiResID;
	LPCTSTR	m_szType;
	CString	m_strPath;
	HICON	m_hIcon;
	CSize	m_sizeIcon;
	BOOL	m_bIsAlphaIcon;
	HBITMAP	m_hBitmap;
	BOOL	m_bIsIgnoreAlphaBitmap;
};

class BCGCBPRODLLEXPORT CBCGPStrokeStyle : public CBCGPGraphicsResource
{
public:
	enum BCGP_CAP_STYLE
	{
		BCGP_CAP_STYLE_FLAT = 0,
		BCGP_CAP_STYLE_SQUARE,
		BCGP_CAP_STYLE_ROUND,
		BCGP_CAP_STYLE_TRIANGLE,
	};

	enum BCGP_LINE_JOIN
	{
		BCGP_LINE_JOIN_MITER = 0,
		BCGP_LINE_JOIN_BEVEL,
		BCGP_LINE_JOIN_ROUND,
		BCGP_LINE_JOIN_MITER_OR_BEVEL,
	};

	enum BCGP_DASH_STYLE
	{
		BCGP_DASH_STYLE_SOLID = 0,
		BCGP_DASH_STYLE_DASH,
		BCGP_DASH_STYLE_DOT,
		BCGP_DASH_STYLE_DASH_DOT,
		BCGP_DASH_STYLE_DASH_DOT_DOT,
		BCGP_DASH_STYLE_CUSTOM,
	};

// Construction:
public:
	CBCGPStrokeStyle(
		BCGP_CAP_STYLE startCap = BCGP_CAP_STYLE_FLAT,
		BCGP_CAP_STYLE endCap = BCGP_CAP_STYLE_FLAT,
		BCGP_CAP_STYLE dashCap = BCGP_CAP_STYLE_FLAT,
		BCGP_LINE_JOIN lineJoin = BCGP_LINE_JOIN_MITER,
		FLOAT miterLimit = 10.0f,
		BCGP_DASH_STYLE dashStyle = BCGP_DASH_STYLE_SOLID,
		FLOAT dashOffset = 0.0f);

	CBCGPStrokeStyle(
		const CArray<FLOAT, FLOAT>& arDashes,
		BCGP_CAP_STYLE startCap = BCGP_CAP_STYLE_FLAT,
		BCGP_CAP_STYLE endCap = BCGP_CAP_STYLE_FLAT,
		BCGP_CAP_STYLE dashCap = BCGP_CAP_STYLE_FLAT,
		BCGP_LINE_JOIN lineJoin = BCGP_LINE_JOIN_MITER,
		FLOAT miterLimit = 10.0f,
		FLOAT dashOffset = 0.0f);

	CBCGPStrokeStyle(const CBCGPStrokeStyle& strokeStyle);

	virtual ~CBCGPStrokeStyle()
	{
		Destroy();
	}

	CBCGPStrokeStyle& operator = (const CBCGPStrokeStyle& src)
	{
		CopyFrom(src);
		return *this;
	}

	bool operator == (const CBCGPStrokeStyle& src)
	{
		return (CompareWith(src) == TRUE);
	}

	void CopyFrom(const CBCGPStrokeStyle& strokeStyle);

	BOOL CompareWith(const CBCGPStrokeStyle& strokeStyle) const;

// Attributes:
public:
	const BOOL IsEmpty() const
	{
		return CompareWith(CBCGPStrokeStyle());
	}

	void SetDashStyle(BCGP_DASH_STYLE dashStyle, FLOAT dashOffset = 0.0f);

	const CArray<FLOAT, FLOAT>& GetDashes() const
	{
		return m_arDashes;
	}

	BCGP_CAP_STYLE GetStartCap() const
	{
		return m_startCap;
	}

	BCGP_CAP_STYLE GetEndCap() const
	{
		return m_endCap;
	}

	BCGP_CAP_STYLE GetDashCap() const
	{
		return  m_dashCap;
	}

	BCGP_LINE_JOIN GetLineJoin() const
	{
		return m_lineJoin;
	}

	FLOAT GetMitterLimit() const
	{
		return m_miterLimit;
	}

	BCGP_DASH_STYLE GetDashStyle() const
	{
		return m_dashStyle;
	}

	FLOAT GetDashOffset() const
	{
		return m_dashOffset;
	}

// Operations:
	virtual void Destroy(BOOL bDetachFromGM = TRUE);

protected:
	void CommonInit();

	BCGP_CAP_STYLE			m_startCap;
	BCGP_CAP_STYLE			m_endCap;
	BCGP_CAP_STYLE			m_dashCap;
	BCGP_LINE_JOIN			m_lineJoin;
	FLOAT					m_miterLimit;
	BCGP_DASH_STYLE			m_dashStyle;
	FLOAT					m_dashOffset;
	CArray<FLOAT, FLOAT>	m_arDashes;
};

class BCGCBPRODLLEXPORT CBCGPTextFormat : public CBCGPGraphicsResource
{
	friend class CBCGPGraphicsManagerGDI;

public:
	enum BCGP_TEXT_ALIGNMENT
	{
		BCGP_TEXT_ALIGNMENT_LEADING,
		BCGP_TEXT_ALIGNMENT_TRAILING,
		BCGP_TEXT_ALIGNMENT_CENTER
	};

	enum BCGP_FONT_STYLE
	{
		BCGP_FONT_STYLE_NORMAL,
		BCGP_FONT_STYLE_OBLIQUE,
		BCGP_FONT_STYLE_ITALIC
	};

public:
	CBCGPTextFormat();

	CBCGPTextFormat(
		const CString& strFontFamily,
		float fFontSize,
		long lFontWeight = FW_REGULAR,
		BCGP_FONT_STYLE fontStyle = BCGP_FONT_STYLE_NORMAL,
		LPCTSTR lpszFontLocale = NULL);

	CBCGPTextFormat(const LOGFONT& lf);

	CBCGPTextFormat(const CBCGPTextFormat& textFormat);

	virtual ~CBCGPTextFormat()
	{
		Destroy();
	}

	BOOL Create(
		const CString& strFontFamily,
		float fFontSize,
		long lFontWeight = FW_REGULAR,
		BCGP_FONT_STYLE fontStyle = BCGP_FONT_STYLE_NORMAL,
		LPCTSTR lpszFontLocale = NULL);

	BOOL CreateFromLogFont(const LOGFONT& lf);

	virtual void Destroy(BOOL bDetachFromGM = TRUE);

	void CopyFrom(const CBCGPTextFormat& from);

	CBCGPTextFormat& operator = (const CBCGPTextFormat& src)
	{
		CopyFrom(src);
		return *this;
	}
	
	BOOL IsEmpty() const
	{
		return m_strFontFamily.IsEmpty() || m_fFontSize == 0.0f;
	}

	static void AdjustTextSize(double dblAngle, CBCGPSize& sz);

protected:
	void CommonInit();

// Attributes:
public:
	const CString& GetFontFamily() const
	{
		return m_strFontFamily;
	}

	float GetFontSize() const
	{
		return m_fFontSize;
	}

	long GetFontWeight() const
	{
		return m_lFontWeight;
	}

	BCGP_FONT_STYLE	GetFontStyle() const
	{
		return m_fontStyle;
	}

	CString	GetFontLocale() const
	{
		return m_strFontLocale;
	}

	BCGP_TEXT_ALIGNMENT	GetTextAlignment() const
	{
		return m_textAlignment;
	}

	BCGP_TEXT_ALIGNMENT	GetTextVerticalAlignment() const
	{
		return m_textAlignmentVert;
	}

	void SetWordWrap(BOOL bWordWrap = TRUE)
	{
		m_bWordWrap = bWordWrap;
	}

	BOOL IsWordWrap() const
	{
		return m_bWordWrap;
	}

	void SetTextAlignment(BCGP_TEXT_ALIGNMENT textAlignment)
	{
		m_textAlignment = textAlignment;
	}

	void SetTextVerticalAlignment(BCGP_TEXT_ALIGNMENT textAlignment)
	{
		m_textAlignmentVert = textAlignment;
	}

	void SetFontSize(float fFontSize);

	void SetClipText(BOOL bSet = TRUE)
	{
		m_bClipText = bSet;
	}

	BOOL IsClipText() const
	{
		return m_bClipText;
	}

	BOOL FromLogFont() const
	{
		return m_bFromLogFont;
	}

	double GetDrawingAngle() const
	{
		return m_dblDrawingAngle;
	}

	void SetDrawingAngle(double dblDrawingAngle)
	{
		if (m_dblDrawingAngle != dblDrawingAngle)
		{
			m_dblDrawingAngle = dblDrawingAngle;
			m_bDrawingAngleWasChanged = TRUE;
		}
	}

// Attributes:
protected:
	CString				m_strFontFamily;
	float				m_fFontSize;
	long				m_lFontWeight;
	BCGP_FONT_STYLE		m_fontStyle;
	CString				m_strFontLocale;
	BCGP_TEXT_ALIGNMENT	m_textAlignment;
	BCGP_TEXT_ALIGNMENT	m_textAlignmentVert;
	BOOL				m_bWordWrap;
	BOOL				m_bClipText;
	BOOL				m_bFromLogFont;
	double				m_dblDrawingAngle;
	BOOL				m_bDrawingAngleWasChanged;
};

////////////////////////////////////////////////////////////////////////////////////
// CBCGPGraphicsManager

// 3D Pie/Doughnut drawing flags:

#define BCGP_3D_DRAW_TOP			0x0001
#define BCGP_3D_DRAW_SIDE1			0x0002
#define BCGP_3D_DRAW_SIDE2			0x0004
#define BCGP_3D_DRAW_EDGE1			0x0008
#define BCGP_3D_DRAW_EDGE2			0x0010
#define BCGP_3D_DRAW_INTERNAL_SIDE	0x0020
#define BCGP_3D_DRAW_ALL			0xFFFF

struct BCGCBPRODLLEXPORT CBCGPGraphicsManagerParams
{
	CBCGPGraphicsManagerParams()
	{
		bUseSoftwareRendering = FALSE;
		bAlphaModePremultiplied = FALSE;
		dpiX = dpiY = 0.0;
	}

	BOOL bUseSoftwareRendering;
	BOOL bAlphaModePremultiplied;
    float dpiX;
    float dpiY;
};

class BCGCBPRODLLEXPORT CBCGPGraphicsManager : public CObject  
{
	friend class CBCGPGeometry;
	friend class CBCGPTextFormat;
	friend class CBCGPBrush;
	friend class CBCGPImage;
	friend class CBCGPStrokeStyle;
	friend class CBCGPGraphicsResource;

	DECLARE_DYNAMIC(CBCGPGraphicsManager)

public:
	enum BCGP_GRAPHICS_MANAGER
	{
		BCGP_GRAPHICS_MANAGER_DEFAULT = 0,
		BCGP_GRAPHICS_MANAGER_GDI,
		BCGP_GRAPHICS_MANAGER_D2D
	};

public:
	static CBCGPGraphicsManager* CreateInstance(
		BCGP_GRAPHICS_MANAGER manager = BCGP_GRAPHICS_MANAGER_DEFAULT, BOOL bFallback = TRUE,
		CBCGPGraphicsManagerParams* pParams = NULL);

	CBCGPGraphicsManager();
	virtual ~CBCGPGraphicsManager();

	virtual CBCGPGraphicsManager* CreateOffScreenManager(const CBCGPRect& rect, CBCGPImage* pImageDest) = 0;

	BOOL IsSupported(UINT nFeatureMask)
	{
		return (m_nSupportedFeatures & nFeatureMask) == nFeatureMask;
	}

	BCGP_GRAPHICS_MANAGER GetType() const
	{
		return m_Type;
	}

	BOOL IsOffscreen() const
	{
		return m_pImageDest != NULL;
	}

	void CleanResources();

protected:
	CBCGPGraphicsManager(const CBCGPRect& rectDest, CBCGPImage* pImageDest);

// Overrides:
public:
	virtual BOOL IsValid() const = 0;

	virtual void BindDC(CDC* pDC, BOOL bDoubleBuffering = TRUE) = 0;
	virtual void BindDC(CDC* pDC, const CRect& rect) = 0;

	virtual BOOL BeginDraw() = 0;
	virtual void EndDraw() = 0;

	virtual void Clear(const CBCGPColor& color = CBCGPColor()) = 0;

	virtual void DrawLine(	
		const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void DrawLine(	
		double x1, double y1, double x2, double y2, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL)
	{
		DrawLine(
			CBCGPPoint(x1, y1), CBCGPPoint(x2, y2), brush, lineWidth, pStrokeStyle);
	}

	virtual void DrawLines(	
		const CBCGPPointsArray& arPoints, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void DrawRectangle(
		const CBCGPRect& rect, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void FillRectangle(
		const CBCGPRect& rect, const CBCGPBrush& brush) = 0;

	virtual void DrawRoundedRectangle(
		const CBCGPRoundedRect& rect, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void FillRoundedRectangle(
		const CBCGPRoundedRect& rect, const CBCGPBrush& brush) = 0;

	virtual void DrawEllipse(
		const CBCGPEllipse& ellipse, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void FillEllipse(
		const CBCGPEllipse& ellipse, const CBCGPBrush& brush) = 0;

	virtual void DrawArc(
		const CBCGPEllipse& ellipse, double dblStartAngle, double dblFinishAngle, BOOL bIsClockwise,
		const CBCGPBrush& brush, double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void DrawArc(
		const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo, const CBCGPSize sizeRadius, 
		BOOL bIsClockwise, BOOL bIsLargeArc,
		const CBCGPBrush& brush, double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void DrawGeometry(
		const CBCGPGeometry& geometry, const CBCGPBrush& brush, 
		double lineWidth = 1., const CBCGPStrokeStyle* pStrokeStyle = NULL) = 0;

	virtual void FillGeometry(
		const CBCGPGeometry& geometry, const CBCGPBrush& brush) = 0;

	virtual void DrawImage(
		const CBCGPImage& image, const CBCGPPoint& ptDest, const CBCGPSize& sizeDest = CBCGPSize(),
		double opacity = 1., CBCGPImage::BCGP_IMAGE_INTERPOLATION_MODE interpolationMode = CBCGPImage::BCGP_IMAGE_INTERPOLATION_MODE_LINEAR, 
		const CBCGPRect& rectSrc = CBCGPRect()) = 0;

	virtual void DrawText(
		const CString& strText, const CBCGPRect& rectText, const CBCGPTextFormat& textFormat,
		const CBCGPBrush& foregroundBrush) = 0;

	virtual CBCGPSize GetTextSize(const CString& strText, const CBCGPTextFormat& textFormat, double dblWidth = 0., BOOL bIgnoreTextRotation = FALSE) = 0;

	virtual void DrawFillPolygon(const CBCGPPointsArray& pts, const CBCGPBrush& brFill, const CBCGPBrush& brLine);

	virtual void DrawBeveledRectangle(const CBCGPRect& rect, const CBCGPBrush& brush, 
									int bevel_size = 3, BOOL bDrawBorder = FALSE);

	virtual void DrawPie(
		const CBCGPEllipse& ellipse, 
		double dblStartAngle, double dblFinishAngle, 
		const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double dblOffsetFromCenter = 0.);

	virtual void Draw3DPie(
		const CBCGPEllipse& ellipse, 
		const double dblHeight,
		double dblStartAngle, double dblFinishAngle, 
		const CBCGPBrush& brFill, const CBCGPBrush& brSideFill, const CBCGPBrush& brLine,
		double dblOffsetFromCenter = 0.,
		int nDrawingFlags = BCGP_3D_DRAW_ALL);

	virtual void DrawDoughnut(
		const CBCGPEllipse& ellipse, 
		double dblHolePerc,	/* 0 - 1 */
		double dblStartAngle, double dblFinishAngle, 
		const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double dblOffsetFromCenter = 0.);

	virtual void Draw3DDoughnut(
		const CBCGPEllipse& ellipse, 
		const double dblHolePerc,	/* 0 - 1 */
		const double dblHeight,
		double dblStartAngle, double dblFinishAngle, 
		const CBCGPBrush& brFill, const CBCGPBrush& brSideFill, const CBCGPBrush& brLine,
		double dblOffsetFromCenter = 0.,
		int nDrawingFlags = BCGP_3D_DRAW_ALL);

	virtual void Draw3DTorus(
		const CBCGPEllipse& ellipse, 
		const double dblHeight,
		double dblStartAngle, double dblFinishAngle, 
		const CBCGPBrush& brFill, const CBCGPBrush& brSideFill, const CBCGPBrush& brLine,
		double dblOffsetFromCenter = 0.,
		int nDrawingFlags = BCGP_3D_DRAW_ALL);

	virtual void DrawFunnel(const CBCGPRect& rectFunnel,
		double dblNeckHeight, double dblNeckWidth,
		const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double Y1, double Y2);

	virtual void Draw3DFunnel(const CBCGPRect& rectFunnel, double dblDepth,
		double dblNeckHeight, double dblNeckWidth,
		const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double Y1, double Y2, BOOL bIsCircularBase = TRUE,
		const CBCGPBrush& brTopFill = CBCGPBrush());

	virtual void DrawPyramid(const CBCGPRect& rectPyramid,
		const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double Y1 = -1, double Y2 = -1);

	virtual void Draw3DPyramid(const CBCGPRect& rectPyramid, double dblDepth,
		const CBCGPBrush& brSideFillLeft, const CBCGPBrush& brSideFillRight, const CBCGPBrush& brLine,
		double dblXCenterOffset = 0., double Y1 = -1, double Y2 = -1, BOOL bIsCircularBase = FALSE, const CBCGPBrush& brTopFill = CBCGPBrush());

    virtual void DrawArrow3(
		const CBCGPPoint& ptLineStart, const CBCGPPoint& ptLineEnd, 
		double arrowLength, const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double arrowAngle = 170.0, DWORD dwArrowType = 0);

    virtual void DrawArrow4(
		const CBCGPPoint& ptLineStart, const CBCGPPoint& ptLineEnd, 
		double arrowLength, const CBCGPBrush& brFill, const CBCGPBrush& brLine,
		double arrowAngle = 165.0, DWORD dwArrowType = 0);

	virtual void DrawSide(
		const CBCGPPoint& pt1, const CBCGPPoint& pt2, const CBCGPPoint& pt3, const CBCGPPoint& pt4,
		const CBCGPBrush& brFill, const CBCGPBrush& brLine, double lineWidth = 1.);

	virtual void SetClipRect(const CBCGPRect& rectClip, int nFlags = RGN_COPY);
	virtual void SetClipEllipse(const CBCGPEllipse& ellipseClip, int nFlags = RGN_COPY);
	virtual void SetClipRoundedRect(const CBCGPRoundedRect& rectRounded, int nFlags = RGN_COPY);
	virtual void SetClipArea(const CBCGPGeometry& geometry, int nFlags = RGN_COPY) = 0;
	virtual void ReleaseClipArea() = 0;

	virtual void CombineGeometry(CBCGPGeometry& geometryDest, const CBCGPGeometry& geometrySrc1, const CBCGPGeometry& geometrySrc2, int nFlags) = 0;
	virtual void GetGeometryBoundingRect(const CBCGPGeometry& geometry, CBCGPRect& rect) = 0;

	void Detach(CBCGPGraphicsResource* pRes);

	virtual LPVOID CreateGeometry(CBCGPGeometry& geometry) = 0;
	virtual BOOL DestroyGeometry(CBCGPGeometry& geometry) = 0;

	virtual LPVOID CreateTextFormat(CBCGPTextFormat& textFormat) = 0;
	virtual BOOL DestroyTextFormat(CBCGPTextFormat& textFormat) = 0;

	virtual LPVOID CreateBrush(CBCGPBrush& brush) = 0;
	virtual BOOL DestroyBrush(CBCGPBrush& brush) = 0;

	virtual LPVOID CreateImage(CBCGPImage& image) = 0;
	virtual BOOL DestroyImage(CBCGPImage& image) = 0;
	virtual CBCGPSize GetImageSize(CBCGPImage& image) = 0;
	virtual BOOL CopyImage(CBCGPImage& imageSrc, CBCGPImage& imageDest, const CBCGPRect& rectSrc) = 0;
	virtual HBITMAP ExportImageToBitmap(CBCGPImage& image) = 0;

	virtual LPVOID CreateStrokeStyle(CBCGPStrokeStyle& style) = 0;
	virtual BOOL DestroyStrokeStyle(CBCGPStrokeStyle& style) = 0;

	CList<CBCGPGraphicsResource*, CBCGPGraphicsResource*>	m_lstRes;

	CBCGPGraphicsManager* GetOriginal() const
	{
		return m_pOriginal;
	}

	virtual void EnableAntialiasing(BOOL bEnable = TRUE) = 0;
	virtual BOOL IsAntialiasingEnabled() const = 0;

	void EnableTransparentGradient(BOOL bEnable = TRUE)
	{
		m_bIsTransparentGradient = bEnable;
	}

	BOOL IsTransparentGradient() const
	{
		return m_bIsTransparentGradient;
	}

	virtual BOOL IsBindDCFailed() const
	{
		return FALSE;
	}

protected:

	virtual void OnFillGeometryBevel(const CBCGPBrush& brFill, const CBCGPGeometry& shape, const CBCGPGeometry& shapeInternal);

	double PrepareBevelColors(const CBCGPColor& color, CBCGPColor& colorLight, CBCGPColor& colorDark);

	UINT					m_nSupportedFeatures;
	BCGP_GRAPHICS_MANAGER	m_Type;
	CBCGPRect				m_rectCurrGradient;
	CBCGPImage*				m_pImageDest;
	CBCGPRect				m_rectDest;
	CBCGPGraphicsManager*	m_pOriginal;
	BOOL					m_bIsTransparentGradient;
};

#endif // !defined(AFX_BCGPGRAPHICSMANAGER_H__979A73FE_851A_4E39_B412_4F2C22047E28__INCLUDED_)
