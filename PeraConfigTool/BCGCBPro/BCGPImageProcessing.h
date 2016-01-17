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
// BCGPGraphicsManagerD2D.h: interface for the CBCGPGraphicsManagerD2D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPIMAGEPROCESSING_H__847B8B25_AE4A_44E1_B1E8_CFE353B89B48__INCLUDED_)
#define AFX_BCGPIMAGEPROCESSING_H__847B8B25_AE4A_44E1_B1E8_CFE353B89B48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _BCGSUITE_
#include "BCGPToolBarImages.h"
#endif

#include "BCGPMath.h"

class CBCGPScanliner
{
public:
	CBCGPScanliner();
	CBCGPScanliner(LPBYTE data, const CSize& size, size_t height = 0, size_t pitch = 0,
					BYTE channels = 1, BOOL invert = FALSE);
	CBCGPScanliner(LPBYTE data, const CRect& rect, size_t height = 0, size_t pitch = 0, 
					BYTE channels = 1, BOOL invert = FALSE);
	~CBCGPScanliner();

	void Attach
	(
		LPBYTE data,
		const CSize& size,
		size_t height = 0,
		size_t pitch  = 0,
		BYTE channels = 1,
		BOOL invert   = FALSE
	);

	void Attach
	(
		LPBYTE data,
		const CRect& rect,
		size_t height = 0,
		size_t pitch  = 0,
		BYTE channels = 1,
		BOOL invert   = FALSE
	);

	LPBYTE Begin()
	{
		m_line = m_line_begin;
		return m_line;
	}
	LPBYTE End()
	{
		m_line = m_line_end;
		return m_line;
	}

	LPBYTE operator[](int index)
	{
		return m_line_begin + m_offset * index;
	}
	const LPBYTE operator[](int index) const
	{
		return m_line_begin + m_offset * index;
	}

	LPBYTE Get()
	{
		return m_line;
	}
	const LPBYTE Get() const
	{
		return m_line;
	}

	DWORD pitch() const
	{
		return m_pitch;
	}
	DWORD GetRows() const
	{
		return m_rows;
	}
	DWORD GetColumns() const
	{
		return m_cols;
	}
	BYTE GetChannels() const
	{
		return m_channels;
	}

	const CBCGPScanliner& operator += (DWORD line)
	{
		m_line += m_offset * line;
		return *this;
	}

	const CBCGPScanliner& operator -= (DWORD line)
	{
		m_line -= m_offset * line;
		return *this;
	}

	const CBCGPScanliner& operator ++ ()
	{
		m_line += m_offset;
		return *this;
	}

	const CBCGPScanliner& operator ++ (int)
	{
		m_line += m_offset;
		return *this;
	}

	const CBCGPScanliner& operator -- ()
	{
		m_line -= m_offset;
		return *this;
	}

	const CBCGPScanliner& operator -- (int)
	{
		m_line -= m_offset;
		return *this;
	}

protected:
	void Empty();

	LPBYTE _begin(LPBYTE data) const;
	LPBYTE _end(LPBYTE data) const;

private:
	LPBYTE			m_line;
	LPBYTE			m_line_begin;
	LPBYTE			m_line_end;
	DWORD			m_pitch;
	DWORD			m_start_row;
	DWORD			m_start_col;
	DWORD			m_rows;
	DWORD			m_cols;
	int				m_offset;
	BYTE			m_channels;
	DWORD			m_height;
};


class CBCGPScanlinerBitmap: public CBCGPScanliner
{
public:
	CBCGPScanlinerBitmap();
	~CBCGPScanlinerBitmap();

	void Attach(HBITMAP bitmap, const CPoint& ptBegin = CPoint(0, 0));
};


class CBCGPTransferFunction
{
protected:
    struct XPoint
    {
        double m_Point;
        double m_Value;

        XPoint()
            : m_Point(0.0)
            , m_Value(0.0)
        {
        }

        XPoint(double point, double value)
            : m_Point(point)
            , m_Value(value)
        {
        }
    };

public:
    CBCGPTransferFunction();
    ~CBCGPTransferFunction();

public:
	void SetInput(double valueMin, double valueMax, double step = 1.0);
	void SetOutput(double valueMin, double valueMax);

	int	IsEmpty() const
	{
		return m_Points.GetSize() == 0;
	}

	void AddPoint(double point, double value);

	void Clear();

	void Calculate(CArray<double, double>& output) const;

protected:
	int GetIndex(double point) const
	{
		return (int)fabs((point - m_InputValueMin) / m_InputValueStep);
	}

private:
	double					m_InputValueMin;
	double					m_InputValueMax;
	double					m_InputValueStep;
	double					m_OutputValueMin;
	double					m_OutputValueMax;
	CArray<XPoint, XPoint>	m_Points;
};

class CBCGPColorTableFunction
{
public:
    CBCGPColorTableFunction(int size = 256);
	~CBCGPColorTableFunction();

	void AddRPoint(BYTE point, BYTE value)
    {
        m_R.AddPoint(point, value);
    }
	void AddGPoint(BYTE point, BYTE value)
    {
        m_G.AddPoint(point, value);
    }
	void AddBPoint(BYTE point, BYTE value)
    {
        m_B.AddPoint(point, value);
    }

	void Calculate();

	int GetSize() const
	{
		return m_Size;
	}

	const RGBQUAD* GetColors() const
    {
        return m_Colors;
    }

private:
	int						m_Size;
	CBCGPTransferFunction	m_R;
	CBCGPTransferFunction	m_G;
	CBCGPTransferFunction	m_B;
	RGBQUAD*				m_Colors;
};


class CBCGPZoomKernel
{
public:
	typedef double XFilterProc(double dValue);
	typedef XFilterProc* XLPFilterProc;

	struct XKernel
	{
		int    pixel;
		double weight;
	};

	struct XKernelList
	{
		DWORD		  count;
		XKernel* stat;
	};

	enum XZoomType
	{
		e_ZoomTypeFirst 	= 0,
		e_ZoomTypeStretch	= e_ZoomTypeFirst,
		e_ZoomTypeFitImage	= 1,
		e_ZoomTypeFitWidth	= 2,
		e_ZoomTypeFitHeight = 3,
		e_ZoomTypeLast		= e_ZoomTypeFitHeight
	};

	enum XFilterType
	{
		e_FilterTypeFirst	 = 0,
		e_FilterTypeBox 	 = e_FilterTypeFirst,
		e_FilterTypeBilinear = 1,
		e_FilterTypeBicubic  = 2,
		e_FilterTypeBell	 = 3,
		e_FilterTypeBSpline  = 4,
		e_FilterTypeLanczos3 = 5,
		e_FilterTypeMitchell = 6,
		e_FilterTypeLast	 = e_FilterTypeMitchell
	};

public:
	CBCGPZoomKernel();
	virtual ~CBCGPZoomKernel();

	void Create(int sizeSrc, int sizeDst, int originSrc, int widthSrc, XFilterType ft);
	void Create(int sizeSrc, int sizeDst, XFilterType ft);
	void Empty();

	XKernelList& operator [](int index)
	{
		return m_List[index];
	}
	const XKernelList& operator [](int index) const
	{
		return m_List[index];
	}

	static double FilterWidth(XFilterType ft);
	static XLPFilterProc FilterProc(XFilterType ft);
	static double Filter(XFilterType ft, double value);

	static void CorrectZoomSize(const CSize& sizeSrc, CSize& sizeDst, XZoomType zt);

private:
	DWORD			m_Size;
	XKernelList*	m_List;
};


void BCGPResizeImage32
(
    CBCGPScanliner& md,       // destination matrix
    int nXOriginDst,          // x-coord of destination upper-left corner
    int nYOriginDst,          // y-coord of destination upper-left corner
    int nWidthDst,            // width of destination rectangle
    int nHeightDst,           // height of destination rectangle
    const CBCGPScanliner& ms, // source matrix
    const CBCGPZoomKernel& KernelX,
    const CBCGPZoomKernel& KernelY
);

void BCGPResizeImage32
(
    CBCGPScanliner& md,       // destination matrix
    int nXOriginDst,          // x-coord of destination upper-left corner
    int nYOriginDst,          // y-coord of destination upper-left corner
    int nWidthDst,            // width of destination rectangle
    int nHeightDst,           // height of destination rectangle
    const CBCGPScanliner& ms, // source matrix
    int nXOriginSrc,          // x-coord of source upper-left corner
    int nYOriginSrc,          // y-coord of source upper-left corner
    int nWidthSrc,            // width of source rectangle
    int nHeightSrc,           // height of source rectangle
    CBCGPZoomKernel::XFilterType ft
);

void BCGPResizeImage32
(
    CBCGPScanliner& md,       // destination matrix
    int nWidthDst,            // width of destination rectangle
    int nHeightDst,           // height of destination rectangle
    const CBCGPScanliner& ms, // source matrix
    int nWidthSrc,            // width of source rectangle
    int nHeightSrc,           // height of source rectangle
    CBCGPZoomKernel::XFilterType ft
);

void BCGPResizeImage32
(
#ifndef _BCGSUITE_
	CBCGPToolBarImages& imageSrc,
	CBCGPToolBarImages& imageDst,
#else
	CMFCToolBarImages& imageSrc,
	CMFCToolBarImages& imageDst,
#endif
	int index,
#ifndef _BCGSUITE_
	CBCGPToolBarImages::ImageAlignHorz horz,
	CBCGPToolBarImages::ImageAlignVert vert,
#else
	CMFCToolBarImages::ImageAlignHorz horz,
	CMFCToolBarImages::ImageAlignVert vert,
#endif
	CBCGPZoomKernel::XFilterType ft
);

HBITMAP BCGPIconToBitmap32 (HICON hIcon);

#endif // !defined(AFX_BCGPIMAGEPROCESSING_H__847B8B25_AE4A_44E1_B1E8_CFE353B89B48__INCLUDED_)
