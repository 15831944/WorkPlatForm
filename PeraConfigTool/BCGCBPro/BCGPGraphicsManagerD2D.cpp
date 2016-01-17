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
// BCGPGraphicsManagerD2D.cpp: implementation of the CBCGPGraphicsManagerD2D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPGraphicsManagerD2D.h"
#include "BCGPDrawManager.h"
#include "BCGPD2DDefs.h"
#include "BCGPMath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef __wincodec_h__

GUID CLSID_WICImagingFactory = { 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa };
GUID GUID_WICPixelFormat32bppPBGRA = { 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10 };

#endif

#ifndef IID_PPV_ARGS

extern "C++"
{
    template<typename T> void** IID_PPV_ARGS_Helper(T** pp) 
    {
        static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
        return reinterpret_cast<void**>(pp);
    }
}

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)

#endif

template<class Interface>
AFX_INLINE void SafeRelease(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
}

IMPLEMENT_DYNAMIC(CBCGPGraphicsManagerD2D, CBCGPGraphicsManager)

BOOL CBCGPGraphicsManagerD2D::m_bD2DInitialized;
HINSTANCE CBCGPGraphicsManagerD2D::m_hinstD2DDLL = NULL;
HINSTANCE CBCGPGraphicsManagerD2D::m_hinstDWriteDLL = NULL;

ID2D1Factory* CBCGPGraphicsManagerD2D::m_pDirect2dFactory = NULL;
IDWriteFactory* CBCGPGraphicsManagerD2D::m_pWriteFactory = NULL;
IWICImagingFactory* CBCGPGraphicsManagerD2D::m_pWicFactory = NULL;

BOOL CBCGPGraphicsManagerD2D::InitD2D(BCGP_D2D1_FACTORY_TYPE d2dFactoryType, BCGP_DWRITE_FACTORY_TYPE writeFactoryType)
{
	if (m_bD2DInitialized)
	{
		return TRUE;
	}

	HRESULT hr = S_OK;

#ifndef _BCGSUITE_
	if (!globalData.m_bComInitialized)
	{
		hr = CoInitialize(NULL);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	globalData.m_bComInitialized = TRUE;
#endif

	if ((m_hinstD2DDLL = ::LoadLibrary (_T("D2D1.dll"))) == NULL)
	{
		return FALSE;
	}

	typedef HRESULT (WINAPI * D2D1CREATEFACTORY)(BCGP_D2D1_FACTORY_TYPE factoryType, REFIID riid, CONST D2D1_FACTORY_OPTIONS *pFactoryOptions, void **ppIFactory);
	typedef HRESULT (WINAPI * DWRITECREATEFACTORY)(BCGP_DWRITE_FACTORY_TYPE factoryType, REFIID riid, IUnknown **factory);

	D2D1CREATEFACTORY pfD2D1CreateFactory = (D2D1CREATEFACTORY)::GetProcAddress(m_hinstD2DDLL, "D2D1CreateFactory");
	if (pfD2D1CreateFactory != NULL)
	{
		hr = (*pfD2D1CreateFactory)(d2dFactoryType, __uuidof(ID2D1Factory),
			NULL, reinterpret_cast<void **>(&m_pDirect2dFactory));
		if (FAILED(hr))
		{
			m_pDirect2dFactory = NULL;
			return FALSE;
		}
	}

	m_hinstDWriteDLL = ::LoadLibrary (_T("DWrite.dll"));
	if (m_hinstDWriteDLL != NULL)
	{
		DWRITECREATEFACTORY pfD2D1CreateFactory = (DWRITECREATEFACTORY)::GetProcAddress(m_hinstDWriteDLL, "DWriteCreateFactory");
		if (pfD2D1CreateFactory != NULL)
		{
			hr = (*pfD2D1CreateFactory)(writeFactoryType, __uuidof(IDWriteFactory), (IUnknown**)&m_pWriteFactory);
		}
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWicFactory));

	m_bD2DInitialized = TRUE;
	return TRUE;
}

void CBCGPGraphicsManagerD2D::ReleaseD2DRefs()
{
	if (!m_bD2DInitialized)
	{
		return;
	}

	if (m_pDirect2dFactory != NULL)
	{
		m_pDirect2dFactory->Release();
		m_pDirect2dFactory = NULL;
	}

	if (m_pWriteFactory != NULL)
	{
		m_pWriteFactory->Release();
		m_pWriteFactory = NULL;
	}

	if (m_pWicFactory != NULL)
	{
		m_pWicFactory->Release();
		m_pWicFactory = NULL;
	}

	if (m_hinstD2DDLL != NULL)
	{
		::FreeLibrary(m_hinstD2DDLL);
	}

	if (m_hinstDWriteDLL != NULL)
	{
		::FreeLibrary(m_hinstDWriteDLL);
	}

	m_bD2DInitialized = FALSE;
}

BOOL CBCGPGraphicsManagerD2D::IsValid() const
{
	return InitD2D();
}

static D2D1_COLOR_F D2DColor(const CBCGPColor& color)
{
	D2D1_COLOR_F colorD2D;
	
	colorD2D.r = (float)color.r;
	colorD2D.g = (float)color.g;
	colorD2D.b = (float)color.b;
	colorD2D.a = (float)color.a;

	return colorD2D;
}

static D2D1_POINT_2F D2DPoint(const CBCGPPoint& point, BOOL bPixelOffset = FALSE)
{
	D2D1_POINT_2F pointD2D;

	pointD2D.x = (float)point.x;
	pointD2D.y = (float)point.y;

	if (bPixelOffset)
	{
		pointD2D.x = (int)pointD2D.x + .5f;
		pointD2D.y = (int)pointD2D.y + .5f;
	}

	return pointD2D;
}

static D2D1_RECT_F D2DRect(const CBCGPRect& rect, BOOL bPixelOffset = FALSE)
{
	D2D1_RECT_F rectD2D;

	rectD2D.left = (float)rect.left;
	rectD2D.right = (float)rect.right;
	rectD2D.top = (float)rect.top;
	rectD2D.bottom = (float)rect.bottom;

	if (bPixelOffset)
	{
		rectD2D.left = (int)rectD2D.left + .5f;
		rectD2D.right = (int)rectD2D.right + .5f;
		rectD2D.top = (int)rectD2D.top + .5f;
		rectD2D.bottom = (int)rectD2D.bottom + .5f;
	}

	return rectD2D;
}

static D2D1_RECT_U D2DRectU(const CBCGPRect& rect)
{
	D2D1_RECT_U rectD2D;

	rectD2D.left = (UINT)rect.left;
	rectD2D.right = (UINT)rect.right;
	rectD2D.top = (UINT)rect.top;
	rectD2D.bottom = (UINT)rect.bottom;

	return rectD2D;
}

static D2D1_SIZE_F D2DSize(const CBCGPSize& size)
{
	D2D1_SIZE_F sizeD2D;

	sizeD2D.width = (float)size.Width();
	sizeD2D.height = (float)size.Height();

	return sizeD2D;
}

static D2D1_SIZE_U D2DSizeU(const CBCGPSize& size)
{
	D2D1_SIZE_U sizeD2D;

	sizeD2D.width = (UINT)size.Width();
	sizeD2D.height = (UINT)size.Height();

	return sizeD2D;
}

static D2D1_ELLIPSE D2DEllipse(const CBCGPEllipse& ellipse, BOOL bPixelOffset = FALSE)
{
	D2D1_ELLIPSE ellipseD2D;

	ellipseD2D.point = D2DPoint(ellipse.point, bPixelOffset);
	ellipseD2D.radiusX = (float)ellipse.radiusX;
	ellipseD2D.radiusY = (float)ellipse.radiusY;

	if (bPixelOffset)
	{
		ellipseD2D.radiusX = (int)ellipseD2D.radiusX + .5f;
		ellipseD2D.radiusY = (int)ellipseD2D.radiusY + .5f;
	}

	return ellipseD2D;
}

static D2D1_ROUNDED_RECT D2DRoundedRect(const CBCGPRoundedRect& rect, BOOL bPixelOffset = FALSE)
{
	D2D1_ROUNDED_RECT rectD2D;

	rectD2D.rect = D2DRect(rect.rect, bPixelOffset);
	rectD2D.radiusX = (float)rect.radiusX;
	rectD2D.radiusY = (float)rect.radiusY;

	if (bPixelOffset)
	{
		rectD2D.radiusX = (int)rectD2D.radiusX + .5f;
		rectD2D.radiusY = (int)rectD2D.radiusY + .5f;
	}

	return rectD2D;
}

void CBCGPGraphicsManagerD2D::PrepareGradientPoints(ID2D1Brush* pD2DBrush, const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo,
								  CBCGPBrush::BCGP_GRADIENT_TYPE gradientType)
{
	switch (gradientType)
	{
	case CBCGPBrush::BCGP_NO_GRADIENT:
		return;

	case CBCGPBrush::BCGP_GRADIENT_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_BEVEL:
		{
			ID2D1LinearGradientBrush* linearGradientBrush = (ID2D1LinearGradientBrush*)pD2DBrush;

			linearGradientBrush->SetStartPoint(D2DPoint(ptFrom));
			linearGradientBrush->SetEndPoint(D2DPoint(ptTo));
		}
		break;

	default:
		return;
	}
}

void CBCGPGraphicsManagerD2D::PrepareGradientPoints(ID2D1Brush* pD2DBrush, 
								  const CBCGPRect& rectIn,
								  CBCGPBrush::BCGP_GRADIENT_TYPE gradientType)
{
	CBCGPRect rect = !m_rectCurrGradient.IsRectEmpty() ? m_rectCurrGradient : rectIn;

	CBCGPPoint ptFrom;
	CBCGPPoint ptTo;

	switch (gradientType)
	{
	case CBCGPBrush::BCGP_NO_GRADIENT:
	case CBCGPBrush::BCGP_GRADIENT_BEVEL:
		return;

	case CBCGPBrush::BCGP_GRADIENT_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL:
		ptFrom.x = ptTo.x = rect.CenterPoint().x;
		ptFrom.y = rect.top;
		ptTo.y = rect.bottom;
		break;

	case CBCGPBrush::BCGP_GRADIENT_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL:
		ptFrom.y = ptTo.y = rect.CenterPoint().y;
		ptFrom.x = rect.right;
		ptTo.x = rect.left;
		break;

	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT:
		ptFrom.x = rect.left;
		ptFrom.y = rect.top;
		ptTo.x = rect.right;
		ptTo.y = rect.bottom;
		break;

	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT:
		ptFrom.x = rect.right;
		ptFrom.y = rect.top;
		ptTo.x = rect.left;
		ptTo.y = rect.bottom;
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_CENTER:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_RIGHT:
		{
			ID2D1RadialGradientBrush* radialGradientBrush = (ID2D1RadialGradientBrush*)pD2DBrush;

			radialGradientBrush->SetCenter(D2DPoint(rect.CenterPoint()));
			radialGradientBrush->SetRadiusX((float)rect.Width() * 2 / 3);
			radialGradientBrush->SetRadiusY((float)rect.Height() * 2 / 3);

			double xOffset = 0.0;
			double yOffset = 0.0;

			const double dx = rect.Width() / 4.;
			const double dy = rect.Height() / 4.;

			if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP)
			{
				yOffset = -dy;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM)
			{
				yOffset = dy;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT)
			{
				xOffset = -dx;
				yOffset = -dy;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_RIGHT)
			{
				xOffset = dx;
				yOffset = -dy;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_LEFT)
			{
				xOffset = -dx;
				yOffset = dy;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_RIGHT)
			{
				xOffset = dx;
				yOffset = dy;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_LEFT)
			{
				xOffset = -dx;
			}
			else if (gradientType == CBCGPBrush::BCGP_GRADIENT_RADIAL_RIGHT)
			{
				xOffset = dx;
			}

			radialGradientBrush->SetGradientOriginOffset(D2DPoint(CBCGPPoint(xOffset, yOffset)));
		}
		return;

	default:
		return;
	}

	PrepareGradientPoints(pD2DBrush, ptFrom, ptTo, gradientType);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPGraphicsManagerD2D::CBCGPGraphicsManagerD2D(CDC* pDC, BOOL bDoubleBuffering, CBCGPGraphicsManagerParams* pParams)
{
	m_Type = BCGP_GRAPHICS_MANAGER_D2D;
	m_pCurrlayer = NULL;
	m_dcRenderTarget = NULL;
	m_pBitmapRenderTarget = NULL;
	m_pRenderTarget = NULL;
	m_bIsBindError = FALSE;

	if (InitD2D())
	{
		m_nSupportedFeatures =	BCGP_GRAPHICS_MANAGER_COLOR_OPACITY |
								BCGP_GRAPHICS_MANAGER_CAP_STYLE	|
								BCGP_GRAPHICS_MANAGER_LINE_JOIN |
								BCGP_GRAPHICS_MANAGER_ANTIALIAS;

		D2D1_RENDER_TARGET_PROPERTIES props;
		
		props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		props.dpiX = props.dpiY = 0;
		props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
		props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

		if (pParams != NULL)
		{
			if (pParams->bAlphaModePremultiplied)
			{
				props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
			}

			if (pParams->bUseSoftwareRendering)
			{
				props.type = D2D1_RENDER_TARGET_TYPE_SOFTWARE;
			}
			
			props.dpiX = pParams->dpiX;
			props.dpiY = pParams->dpiY;
		}

		m_pDirect2dFactory->CreateDCRenderTarget(&props, &m_dcRenderTarget);

		BindDC(pDC, bDoubleBuffering);

		if (m_pDC != NULL && m_dcRenderTarget != NULL)
		{
			BeginDraw();
		}
	}
}

CBCGPGraphicsManagerD2D::CBCGPGraphicsManagerD2D(const CBCGPRect& rectDest, CBCGPImage* pImageDest) :
	CBCGPGraphicsManager(rectDest, pImageDest)
{
	m_Type = BCGP_GRAPHICS_MANAGER_D2D;
	m_pCurrlayer = NULL;
	m_dcRenderTarget = NULL;
	m_pBitmapRenderTarget = NULL;
	m_pRenderTarget = NULL;
	m_bIsBindError = FALSE;
}

CBCGPGraphicsManagerD2D::~CBCGPGraphicsManagerD2D()
{
	EndDraw();

	if (m_pOriginal == NULL)
	{
		CleanResources();
	}
	else
	{
		m_pOriginal->Detach(&m_CurrGeometry);
	}

	if (m_pDirect2dFactory != NULL)
	{
		if (m_dcRenderTarget != NULL)
		{
			m_dcRenderTarget->Release();
		}

		if (m_pBitmapRenderTarget != NULL)
		{
			m_pBitmapRenderTarget->Release();
		}
	}
}

void CBCGPGraphicsManagerD2D::BindDC(CDC* pDC, BOOL /*bDoubleBuffering*/)
{
	m_bIsBindError = FALSE;

	m_pDC = pDC;

	if (pDC == NULL)
	{
		return;
	}

	CRect rectWindow;

	CWnd* pWnd = m_pDC->GetWindow();
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetClientRect(rectWindow);
	}
	else
	{
		m_bIsBindError = TRUE;

		TRACE0("You need to call CBCGPGraphicsManagerD2D::BindDC(CDC* pDC, const CBCGPRect& rect)\n");
		ASSERT(FALSE);
		return;
	}

	BindDC(pDC, rectWindow);
}

void CBCGPGraphicsManagerD2D::BindDC(CDC* pDC, const CRect& rect)
{
	m_bIsBindError = FALSE;

	if (m_pBitmapRenderTarget != NULL)
	{
		ASSERT(FALSE);
		return;
	}

	m_pDC = pDC;

	if (m_dcRenderTarget == NULL || pDC == NULL)
	{
		return;
	}

	HRESULT hr = m_dcRenderTarget->BindDC(m_pDC->GetSafeHdc(), rect);
	if (FAILED(hr))
	{
		TRACE1("CBCGPGraphicsManagerD2D::BindDC() failed. %x\r\n", hr);
		m_bIsBindError = TRUE;
	}
}

CBCGPGraphicsManager* CBCGPGraphicsManagerD2D::CreateOffScreenManager(const CBCGPRect& rect, CBCGPImage* pImageDest)
{
	if (m_dcRenderTarget == NULL)
	{
		return NULL;
	}

	if (rect.IsRectEmpty())
	{
		return NULL;
	}

	CBCGPGraphicsManagerD2D* pGM = new CBCGPGraphicsManagerD2D(rect, pImageDest);

	pGM->m_nSupportedFeatures = m_nSupportedFeatures;
	pGM->m_pOriginal = this;

	pGM->BeginDraw();

	return pGM;
}

BOOL CBCGPGraphicsManagerD2D::BeginDraw()
{
	if (m_bIsBindError)
	{
		return FALSE;
	}

	if (m_pImageDest != NULL && m_pOriginal != NULL)
	{
		CBCGPGraphicsManagerD2D* pGMOrig = (CBCGPGraphicsManagerD2D*)m_pOriginal;

		if (pGMOrig->m_dcRenderTarget == NULL)
		{
			return FALSE;
		}

		const CBCGPSize sizeDest = m_rectDest.Size();

		HRESULT hr = pGMOrig->m_dcRenderTarget->CreateCompatibleRenderTarget(D2DSize(sizeDest), &m_pBitmapRenderTarget);

		if (FAILED(hr))
		{
			TRACE1("CBCGPGraphicsManagerD2D::BeginDraw(): CreateRadialGradientBrush Failed. %x\r\n", hr);
			return FALSE;
		}

		if (m_pBitmapRenderTarget == NULL)
		{
			return FALSE;
		}

		FillRectangle(CBCGPRect(0, 0, sizeDest.cx, sizeDest.cy), CBCGPBrush(CBCGPColor()));
		m_pRenderTarget = m_pBitmapRenderTarget;
	}
	else
	{
		m_pRenderTarget = m_dcRenderTarget;
	}

	if (m_pRenderTarget == NULL)
	{
		return FALSE;
	}

	m_pRenderTarget->BeginDraw();
	return TRUE;
}

void CBCGPGraphicsManagerD2D::EndDraw()
{
	if (m_pRenderTarget == NULL)
	{
		return;
	}

	ReleaseClipArea();
	
	HRESULT hr = m_pRenderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
	{
		TRACE0("CBCGPGraphicsManagerD2D::EndDraw: D2DERR_RECREATE_TARGET\r\n");

		for (POSITION pos = m_lstRes.GetHeadPosition(); pos != NULL;)
		{
			CBCGPGraphicsResource* pRes = m_lstRes.GetNext(pos);
			ASSERT_VALID(pRes);

			pRes->Destroy(FALSE);
		}
	}
	else if (FAILED(hr))
	{
		TRACE1("CBCGPGraphicsManagerD2D::EndDraw: Failed. %x\r\n", hr);
	}

	if (m_pBitmapRenderTarget != NULL && m_pImageDest != NULL)
	{
		ASSERT_VALID(m_pImageDest);

		DestroyImage(*m_pImageDest);

		ID2D1Bitmap* pBitmap = NULL;

		m_pBitmapRenderTarget->GetBitmap(&pBitmap);

		if (pBitmap != NULL)
		{
			m_pImageDest->Set(this, (LPVOID)pBitmap);
		}

		m_pBitmapRenderTarget->Release();
		m_pBitmapRenderTarget = NULL;
	}

	m_pRenderTarget = NULL;
}

void CBCGPGraphicsManagerD2D::Clear(const CBCGPColor& color)
{
	if (m_pRenderTarget == NULL)
	{
		return;
	}

	m_pRenderTarget->Clear(
		color.IsNull() ? D2DColor(globalData.clrBarWindow) : D2DColor(color));
}

void CBCGPGraphicsManagerD2D::DrawLine(	
	const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	PrepareGradientPoints(pD2DBrush, ptFrom, ptTo, brush.GetGradientType());

	BOOL bPixelOffset = (fabs(ptFrom.x - ptTo.x) < .1 || fabs(ptFrom.y - ptTo.y) < .1);

	m_pRenderTarget->DrawLine(D2DPoint(ptFrom, bPixelOffset), D2DPoint(ptTo, bPixelOffset), pD2DBrush, 
		(float)lineWidth, CreateStrokeStyle(pStrokeStyle, lineWidth));
}

void CBCGPGraphicsManagerD2D::DrawLines(	
		const CBCGPPointsArray& arPoints, const CBCGPBrush& brush, 
		double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	const int nSize = (int)arPoints.GetSize();
	if (nSize < 2)
	{
		return;
	}

	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	ID2D1StrokeStyle* pD2DStrokeStyle = CreateStrokeStyle(pStrokeStyle, lineWidth);

	for (int i = 0; i < nSize - 1; i++)
	{
		m_pRenderTarget->DrawLine(D2DPoint(arPoints[i]), D2DPoint(arPoints[i + 1]), pD2DBrush, 
			(float)lineWidth, pD2DStrokeStyle);
	}
}

void CBCGPGraphicsManagerD2D::DrawRectangle(
	const CBCGPRect& rect, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	PrepareGradientPoints(pD2DBrush, rect, brush.GetGradientType());

	m_pRenderTarget->DrawRectangle(D2DRect(rect, TRUE), pD2DBrush, (float) lineWidth,
		CreateStrokeStyle(pStrokeStyle, lineWidth));
}

void CBCGPGraphicsManagerD2D::FillRectangle(
	const CBCGPRect& rect, const CBCGPBrush& brush)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		DrawBeveledRectangle(rect, brush);
	}
	else
	{
		PrepareGradientPoints(pD2DBrush, rect, brush.GetGradientType());

		m_pRenderTarget->FillRectangle(D2DRect(rect), pD2DBrush);
	}
}

void CBCGPGraphicsManagerD2D::DrawRoundedRectangle(
	const CBCGPRoundedRect& rect, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	m_pRenderTarget->DrawRoundedRectangle(D2DRoundedRect(rect), pD2DBrush, (float)lineWidth,
		CreateStrokeStyle(pStrokeStyle, lineWidth));
}

void CBCGPGraphicsManagerD2D::FillRoundedRectangle(
	const CBCGPRoundedRect& rect, const CBCGPBrush& brush)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	PrepareGradientPoints(pD2DBrush, rect.rect, brush.GetGradientType());
	m_pRenderTarget->FillRoundedRectangle(D2DRoundedRect(rect), pD2DBrush);

	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		double nDepth = 3.;

		CBCGPRoundedRect rectInternal = rect;
		rectInternal.rect.DeflateRect(nDepth, nDepth);
		rectInternal.radiusX -= nDepth / 2.;
		rectInternal.radiusY -= nDepth / 2.;

		ID2D1Brush* pD2DBrush2 = (ID2D1Brush*)brush.GetHandle2();

		PrepareGradientPoints(pD2DBrush2, rectInternal.rect);
		m_pRenderTarget->FillRoundedRectangle(D2DRoundedRect(rectInternal), pD2DBrush2);
	}
}

void CBCGPGraphicsManagerD2D::DrawEllipse(
	const CBCGPEllipse& ellipse, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	if (ellipse.radiusX <= 0 || ellipse.radiusY <= 0)
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	PrepareGradientPoints(pD2DBrush, ellipse, brush.GetGradientType());

	m_pRenderTarget->DrawEllipse(D2DEllipse(ellipse), pD2DBrush, (float) lineWidth,
		CreateStrokeStyle(pStrokeStyle, lineWidth));
}

void CBCGPGraphicsManagerD2D::FillEllipse(
	const CBCGPEllipse& ellipse, const CBCGPBrush& brush)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	if (ellipse.radiusX <= 0 || ellipse.radiusY <= 0)
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	if (brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_BEVEL)
	{
		ID2D1Brush* pD2DBrush2 = (ID2D1Brush*)brush.GetHandle2();

		PrepareGradientPoints(pD2DBrush2, ellipse);
		m_pRenderTarget->FillEllipse(D2DEllipse(ellipse), pD2DBrush2);

		double nDepth = min(ellipse.radiusX, ellipse.radiusY) * .05;

		CBCGPEllipse ellipseInternal = ellipse;
		ellipseInternal.radiusX -= nDepth;
		ellipseInternal.radiusY -= nDepth;

		PrepareGradientPoints(pD2DBrush, ellipseInternal);
		m_pRenderTarget->FillEllipse(D2DEllipse(ellipseInternal), pD2DBrush);
	}
	else
	{
		PrepareGradientPoints(pD2DBrush, ellipse, brush.GetGradientType());
		m_pRenderTarget->FillEllipse(D2DEllipse(ellipse), pD2DBrush);
	}
}

void CBCGPGraphicsManagerD2D::DrawArc(
	const CBCGPEllipse& ellipse, double dblStartAngle, double dblFinishAngle, BOOL bIsClockwise,
	const CBCGPBrush& brush, double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	if (ellipse.IsNull ())
	{
		return;
	}

	if (ellipse.radiusX <= 0 || ellipse.radiusY <= 0)
	{
		return;
	}

	dblStartAngle = bcg_normalize_deg(dblStartAngle);
	dblFinishAngle = bcg_normalize_deg(dblFinishAngle);

	BOOL bIsLargeArc = FALSE;

	if (dblStartAngle < dblFinishAngle)
	{
		if (bIsClockwise)
		{
			bIsLargeArc = 360 - dblFinishAngle + dblStartAngle > 180.;
		}
		else
		{
			bIsLargeArc = dblFinishAngle - dblStartAngle > 180.;
		}
	}
	else
	{
		if (bIsClockwise)
		{
			bIsLargeArc = 360 + dblFinishAngle - dblStartAngle < 180.;
		}
		else
		{
			bIsLargeArc = dblStartAngle - dblFinishAngle < 180.;
		}
	}

	double dblAngle = bcg_deg2rad(dblStartAngle);
	double cosA = cos(dblAngle);
	double sinA = sin(dblAngle);
	double r = (ellipse.radiusX * ellipse.radiusY) / bcg_distance(ellipse.radiusY * cosA, ellipse.radiusX * sinA);
	double x1 = ellipse.point.x + cosA * r;
	double y1 = ellipse.point.y - sinA * r;

	dblAngle = bcg_deg2rad(dblFinishAngle);
	cosA = cos(dblAngle);
	sinA = sin(dblAngle);
	r = (ellipse.radiusX * ellipse.radiusY) / bcg_distance(ellipse.radiusY * cosA, ellipse.radiusX * sinA);
	double x2 = ellipse.point.x + cosA * r;
	double y2 = ellipse.point.y - sinA * r;

	ID2D1PathGeometry* pathGeometry = NULL;
	m_pDirect2dFactory->CreatePathGeometry(&pathGeometry);

	ID2D1GeometrySink* geometrySink = NULL;

	pathGeometry->Open(&geometrySink);

	geometrySink->BeginFigure(
		D2DPoint(CBCGPPoint(x1, y1), FALSE), 
		D2D1_FIGURE_BEGIN_FILLED);

	D2D1_ARC_SEGMENT arc;

	arc.point = D2DPoint(CBCGPPoint(x2, y2), FALSE);
	arc.size = D2DSize(CBCGPSize(ellipse.radiusX, ellipse.radiusY));
	arc.rotationAngle = 0.;
	arc.sweepDirection = bIsClockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
	arc.arcSize = bIsLargeArc ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;

	geometrySink->AddArc(arc);

	geometrySink->EndFigure(D2D1_FIGURE_END_OPEN);

	geometrySink->Close();
	geometrySink->Release();

	CBCGPGeometry geometry;
	geometry.Set(NULL, (LPVOID)pathGeometry);

	DrawGeometry(geometry, brush, lineWidth, pStrokeStyle);

	pathGeometry->Release();
}

void CBCGPGraphicsManagerD2D::DrawArc(
	const CBCGPPoint& ptFrom, const CBCGPPoint& ptTo, const CBCGPSize sizeRadius, 
	BOOL bIsClockwise, BOOL bIsLargeArc,
	const CBCGPBrush& brush, double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1PathGeometry* pathGeometry = NULL;
	m_pDirect2dFactory->CreatePathGeometry(&pathGeometry);

	ID2D1GeometrySink* geometrySink = NULL;

	pathGeometry->Open(&geometrySink);

	geometrySink->BeginFigure(
		D2DPoint(ptFrom, FALSE), 
		D2D1_FIGURE_BEGIN_FILLED);

	D2D1_ARC_SEGMENT arc;

	arc.point = D2DPoint(ptTo, FALSE);
	arc.size = D2DSize(sizeRadius);
	arc.rotationAngle = 0.;
	arc.sweepDirection = bIsClockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
	arc.arcSize = bIsLargeArc ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;

	geometrySink->AddArc(arc);

	geometrySink->EndFigure(D2D1_FIGURE_END_OPEN);

	geometrySink->Close();
	geometrySink->Release();

	CBCGPGeometry geometry;
	geometry.Set(NULL, (LPVOID)pathGeometry);

	DrawGeometry(geometry, brush, lineWidth, pStrokeStyle);

	pathGeometry->Release();
}

void CBCGPGraphicsManagerD2D::DrawGeometry(
	const CBCGPGeometry& geometry, const CBCGPBrush& brush, 
	double lineWidth, const CBCGPStrokeStyle* pStrokeStyle)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	ID2D1Geometry* pGeometry = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)geometry);
	ASSERT(pGeometry != NULL);

	CBCGPRect rectGradient = m_rectCurrGradient;
	if (rectGradient.IsRectEmpty())
	{
		D2D1_RECT_F rect;
		pGeometry->GetBounds(NULL, &rect);

		rectGradient = CBCGPRect(rect.left, rect.top, rect.right, rect.bottom);
	}

	PrepareGradientPoints(pD2DBrush, rectGradient, brush.GetGradientType());

	m_pRenderTarget->DrawGeometry(pGeometry, pD2DBrush, (float)lineWidth,
		CreateStrokeStyle(pStrokeStyle, lineWidth));
}

void CBCGPGraphicsManagerD2D::FillGeometry(
	const CBCGPGeometry& geometry, const CBCGPBrush& brush)
{
	if (m_pRenderTarget == NULL || brush.IsEmpty())
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	ID2D1Geometry* pGeometry = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)geometry);
	ASSERT(pGeometry != NULL);

	CBCGPRect rectGradient = m_rectCurrGradient;
	if (rectGradient.IsRectEmpty())
	{
		D2D1_RECT_F rect;
		pGeometry->GetBounds(NULL, &rect);

		rectGradient = CBCGPRect(rect.left, rect.top, rect.right, rect.bottom);
	}

	PrepareGradientPoints(pD2DBrush, rectGradient, brush.GetGradientType());

	m_pRenderTarget->FillGeometry(pGeometry, pD2DBrush);
}

void CBCGPGraphicsManagerD2D::DrawImage(
	const CBCGPImage& image, const CBCGPPoint& ptDest, const CBCGPSize& sizeDest,
	double opacity, CBCGPImage::BCGP_IMAGE_INTERPOLATION_MODE interpolationMode, const CBCGPRect& rectSrc)
{
	if (m_pRenderTarget == NULL)
	{
		return;
	}

	if (sizeDest.cx < 0. || sizeDest.cy < 0)
	{
		return;
	}

	ID2D1Bitmap* pBitmap = (ID2D1Bitmap*)CreateImage((CBCGPImage&)image);
	if (pBitmap == NULL)
	{
		return;
	}

	CBCGPSize size = sizeDest.IsEmpty() ? image.GetSize() : sizeDest;
	CBCGPRect rectImage(ptDest, size);

	D2D1_RECT_F rectSrcD2D = D2DRect(rectSrc);

	D2D1_BITMAP_INTERPOLATION_MODE interpolationModeD2D = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

	switch (interpolationMode)
	{
	case CBCGPImage::BCGP_IMAGE_INTERPOLATION_MODE_NEAREST_NEIGHBOR:
		interpolationModeD2D = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
		break;

	case CBCGPImage::BCGP_IMAGE_INTERPOLATION_MODE_LINEAR:
		interpolationModeD2D = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
		break;
	}

	m_pRenderTarget->DrawBitmap(pBitmap, D2DRect(rectImage), (FLOAT)opacity,
		interpolationModeD2D, rectSrc.IsRectEmpty() ? NULL : &rectSrcD2D);
}

static DWRITE_MATRIX MakeRotateTransform(
    float angle,    // angle in radians
    float x,        // x coordinate of the center of rotation
    float y         // y coordinate of the center of rotation
    )
{
    DWRITE_MATRIX matrix;

    // Initialize the first four members, which comprise a 2x2 matrix.
    float sinA = sinf(angle);
    float cosA = cosf(angle);

    // If the angle is axis aligned, we'll make it a clean matrix.
    if (fmod(angle, 90.0f) == 0)
    {
        sinA = floorf(sinA + .5f);
        cosA = floorf(cosA + .5f);
    }

    matrix.m11 = cosA;
    matrix.m12 = sinA;
    matrix.m21 = -sinA;
    matrix.m22 = cosA;

    // Transform the given point.
    float xT = (x * matrix.m11) + (y * matrix.m21);
    float yT = (x * matrix.m12) + (y * matrix.m22);

    // Initialize the displacement to compensate such that the given
    // point will be the center of rotation.
    matrix.dx = x - xT;
    matrix.dy = y - yT;

    return matrix;
}

void CBCGPGraphicsManagerD2D::DrawText(
	const CString& strText, const CBCGPRect& rectText, const CBCGPTextFormat& textFormat,
	const CBCGPBrush& foregroundBrush)
{
	USES_CONVERSION;

	if (m_pRenderTarget == NULL)
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)foregroundBrush);
	if (pD2DBrush == NULL)
	{
		return;
	}

	IDWriteTextFormat* pWriteTextFormat = (IDWriteTextFormat*)CreateTextFormat((CBCGPTextFormat&)textFormat);
	if (pWriteTextFormat == NULL)
	{
		return;
	}

	double centerX = rectText.CenterPoint().x;
	double centerY = rectText.CenterPoint().y;

	if (textFormat.GetTextAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_TRAILING)
	{
		pWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		centerX = rectText.right;
	}
	else if (textFormat.GetTextAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
	{
		pWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}
	else if (textFormat.GetTextAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_LEADING)
	{
		pWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		centerX = rectText.left;
	}

	if (textFormat.GetTextVerticalAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_TRAILING)
	{
		pWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		centerY = rectText.bottom;
	}
	else if (textFormat.GetTextVerticalAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_CENTER)
	{
		pWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	else if (textFormat.GetTextVerticalAlignment() == CBCGPTextFormat::BCGP_TEXT_ALIGNMENT_LEADING)
	{
		pWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		centerY = rectText.top;
	}

	pWriteTextFormat->SetWordWrapping(textFormat.IsWordWrap() ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

	D2D1_RECT_F rectD2D = D2DRect(rectText);

	PrepareGradientPoints(pD2DBrush, rectText, foregroundBrush.GetGradientType());

	D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE;

	if (textFormat.IsClipText())
	{
		options = /*D2D1_DRAW_TEXT_OPTIONS_CLIP*/(D2D1_DRAW_TEXT_OPTIONS)0x00000002;
	}

	D2D1_MATRIX_3X2_F currMatrix;

	if (textFormat.GetDrawingAngle() != 0.)
	{
		ASSERT(m_pWriteFactory != NULL);

		DWRITE_MATRIX transform = MakeRotateTransform(
			float(bcg_deg2rad(-textFormat.GetDrawingAngle())), float(centerX), float(centerY));

		D2D1_MATRIX_3X2_F matrix;
		memcpy(&matrix, &transform, sizeof(matrix));

		m_pRenderTarget->GetTransform(&currMatrix);
		m_pRenderTarget->SetTransform(matrix);

		options = D2D1_DRAW_TEXT_OPTIONS_NONE;
	}

	m_pRenderTarget->DrawText(T2CW(strText), strText.GetLength(), pWriteTextFormat, &rectD2D, pD2DBrush, options);

	if (textFormat.GetDrawingAngle() != 0.)
	{
		m_pRenderTarget->SetTransform(currMatrix);
	}
}

CBCGPSize CBCGPGraphicsManagerD2D::GetTextSize(const CString& strText, const CBCGPTextFormat& textFormat, double dblWidth, BOOL bIgnoreTextRotation)
{
	USES_CONVERSION;

	ASSERT(m_pWriteFactory != NULL);

	CBCGPSize size;

	if (m_pWriteFactory == NULL || strText.IsEmpty())
	{
		return size;
	}

	if (textFormat.IsWordWrap() && dblWidth == 0.)
	{
		ASSERT(FALSE);
		return size;
	}

	IDWriteTextFormat* pWriteTextFormat = (IDWriteTextFormat*)CreateTextFormat((CBCGPTextFormat&)textFormat);
	if (pWriteTextFormat == NULL)
	{
		return size;
	}

	IDWriteTextLayout* pTextLayout = NULL;

	m_pWriteFactory->CreateTextLayout(T2CW(strText), strText.GetLength(), pWriteTextFormat, 
		textFormat.IsWordWrap() ? (float)dblWidth : D2D1::FloatMax(), D2D1::FloatMax(), &pTextLayout);

	DWRITE_TEXT_METRICS tm;
	memset(&tm, 0, sizeof(DWRITE_TEXT_METRICS));

	pTextLayout->GetMetrics(&tm);

	size.cx = tm.width;
	size.cy = tm.height;

	pTextLayout->Release();

	if (!bIgnoreTextRotation)
	{
		CBCGPTextFormat::AdjustTextSize(textFormat.GetDrawingAngle(), size);
	}

	return size;
}

LPVOID CBCGPGraphicsManagerD2D::CreateGeometry(CBCGPGeometry& geometry)
{
	if (m_pOriginal != NULL)
	{
		return (CBCGPGraphicsManagerD2D*)m_pOriginal->CreateGeometry(geometry);
	}

	if (geometry.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = geometry.GetGraphicsManager();

		if (!IsGraphicsManagerValid(pGM))
		{
			return NULL;
		}
	
		return geometry.GetHandle();
	}

	ID2D1Geometry* pIGeometry = NULL;

	if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPRectangleGeometry)))
	{
		CBCGPRectangleGeometry& rectGeometry = (CBCGPRectangleGeometry&)geometry;

		ID2D1RectangleGeometry* pRectangleGeometry = NULL;
		m_pDirect2dFactory->CreateRectangleGeometry(D2DRect(rectGeometry.GetRectangle()), &pRectangleGeometry);

		pIGeometry = pRectangleGeometry;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPEllipseGeometry)))
	{
		CBCGPEllipseGeometry& ellipseGeometry = (CBCGPEllipseGeometry&)geometry;

		ID2D1EllipseGeometry* pEllipseGeometry = NULL;
		m_pDirect2dFactory->CreateEllipseGeometry(D2DEllipse(ellipseGeometry.GetEllipse()), &pEllipseGeometry);

		pIGeometry = pEllipseGeometry;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPRoundedRectangleGeometry)))
	{
		CBCGPRoundedRectangleGeometry& roundedRectGeometry = (CBCGPRoundedRectangleGeometry&)geometry;

		D2D1_ROUNDED_RECT rect = D2DRoundedRect(roundedRectGeometry.GetRoundedRect());

		ID2D1RoundedRectangleGeometry* pRoundedRectGeometry = NULL;
		m_pDirect2dFactory->CreateRoundedRectangleGeometry(&rect, &pRoundedRectGeometry);

		pIGeometry = pRoundedRectGeometry;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPPolygonGeometry)))
	{
		CBCGPPolygonGeometry& polygonGeometry = (CBCGPPolygonGeometry&)geometry;
		const CBCGPPointsArray& arPoints = polygonGeometry.GetPoints();
		const int nCount = (int)arPoints.GetSize();

		ID2D1PathGeometry* pathGeometry = NULL;
		m_pDirect2dFactory->CreatePathGeometry(&pathGeometry);

		ID2D1GeometrySink* geometrySink = NULL;

		pathGeometry->Open(&geometrySink);

		geometrySink->SetFillMode(D2D1_FILL_MODE_WINDING);

		geometrySink->BeginFigure(
			arPoints.GetSize() > 0 ? D2DPoint(arPoints[0], TRUE) : D2DPoint(CBCGPPoint(0., 0.)), 
			D2D1_FIGURE_BEGIN_FILLED);

		if (polygonGeometry.GetCurveType() == CBCGPPolygonGeometry::BCGP_CURVE_TYPE_BEZIER)
		{
			for (int i = 1; i < nCount; i++)
			{
				D2D1_BEZIER_SEGMENT segment;
				
				segment.point1 = D2DPoint(arPoints[i]);
				if (i < nCount - 1)
				{
					i++;
				}

				segment.point2 = D2DPoint(arPoints[i]);
				if (i < nCount - 1)
				{
					i++;
				}

				segment.point3 = D2DPoint(arPoints[i]);

				geometrySink->AddBezier(segment);
			}
		}
		else
		{
			for (int i = 1; i < nCount; i++)
			{
				geometrySink->AddLine(D2DPoint(arPoints[i]));
			}
		}

		geometrySink->EndFigure(
			polygonGeometry.IsClosed() ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

		geometrySink->Close();
		geometrySink->Release();

		pIGeometry = pathGeometry;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPSplineGeometry)))
	{
		CBCGPSplineGeometry& splineGeometry = (CBCGPSplineGeometry&)geometry;
		const CBCGPPointsArray& arPoints = splineGeometry.GetPoints();
		int nCount = (int)arPoints.GetSize();
		if (nCount > 1)
		{
			nCount = (nCount - 1) / 3;
		}

		ID2D1PathGeometry* pathGeometry = NULL;
		m_pDirect2dFactory->CreatePathGeometry(&pathGeometry);

		ID2D1GeometrySink* geometrySink = NULL;

		pathGeometry->Open(&geometrySink);

		geometrySink->SetFillMode(D2D1_FILL_MODE_WINDING);

		geometrySink->BeginFigure(
			nCount > 0 ? D2DPoint(arPoints[0], TRUE) : D2DPoint(CBCGPPoint(0., 0.)), 
			D2D1_FIGURE_BEGIN_FILLED);

		int index = 1;
		for (int i = 0; i < nCount; i++)
		{
			D2D1_BEZIER_SEGMENT segment;
			
			segment.point1 = D2DPoint(arPoints[index]);
			segment.point2 = D2DPoint(arPoints[index + 1]);
			segment.point3 = D2DPoint(arPoints[index + 2]);
			index += 3;

			geometrySink->AddBezier(segment);
		}

		geometrySink->EndFigure(
			splineGeometry.IsClosed() ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

		geometrySink->Close();
		geometrySink->Release();

		pIGeometry = pathGeometry;
	}
	else if (geometry.IsKindOf(RUNTIME_CLASS(CBCGPComplexGeometry)))
	{
		CBCGPComplexGeometry& complexGeometry = (CBCGPComplexGeometry&)geometry;
		const CObArray& arSegments = complexGeometry.GetSegments();

		ID2D1PathGeometry* pathGeometry = NULL;
		m_pDirect2dFactory->CreatePathGeometry(&pathGeometry);

		ID2D1GeometrySink* geometrySink = NULL;

		pathGeometry->Open(&geometrySink);

		geometrySink->SetFillMode(D2D1_FILL_MODE_WINDING);

		geometrySink->BeginFigure(
			D2DPoint(complexGeometry.GetStartPoint()), 
			D2D1_FIGURE_BEGIN_FILLED);

		for (int i = 0; i < arSegments.GetSize(); i++)
		{
			CObject* pSegment = arSegments[i];
			ASSERT_VALID(pSegment);

			if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPLineSegment)))
			{
				CBCGPLineSegment* pLineSegment = (CBCGPLineSegment*)pSegment;
				geometrySink->AddLine(D2DPoint(pLineSegment->m_Point));
			}
			else if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPBezierSegment)))
			{
				CBCGPBezierSegment* pBezierSegment = (CBCGPBezierSegment*)pSegment;

				D2D1_BEZIER_SEGMENT segment;

				segment.point1 = D2DPoint(pBezierSegment->m_Point1);
				segment.point2 = D2DPoint(pBezierSegment->m_Point2);
				segment.point3 = D2DPoint(pBezierSegment->m_Point3);

				geometrySink->AddBezier(segment);
			}
			else if (pSegment->IsKindOf(RUNTIME_CLASS(CBCGPArcSegment)))
			{
				CBCGPArcSegment* pArcSegment = (CBCGPArcSegment*)pSegment;

				D2D1_ARC_SEGMENT arc;

				arc.point = D2DPoint(pArcSegment->m_Point);
				arc.size = D2DSize(pArcSegment->m_Radius);
				arc.rotationAngle = (float)pArcSegment->m_dblRotationAngle;
				arc.sweepDirection = 
					pArcSegment->m_bIsClockwise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
				arc.arcSize = pArcSegment->m_bIsLargeArc ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;

				geometrySink->AddArc(arc);
			}
		}

		geometrySink->EndFigure(
			complexGeometry.IsClosed() ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

		geometrySink->Close();
		geometrySink->Release();

		pIGeometry = pathGeometry;
	}

	if (pIGeometry != NULL)
	{
		geometry.Set(this, (LPVOID)pIGeometry);
	}

	return pIGeometry;
}

BOOL CBCGPGraphicsManagerD2D::DestroyGeometry(CBCGPGeometry& geometry)
{
	ID2D1Geometry* pIGeometry = (ID2D1Geometry*)geometry.GetHandle();
	if (pIGeometry != NULL && m_pDirect2dFactory != NULL)
	{
		pIGeometry->Release();
	}

	geometry.Set(NULL, NULL);
	return TRUE;
}

LPVOID CBCGPGraphicsManagerD2D::CreateTextFormat(CBCGPTextFormat& textFormat)
{
	if (m_pOriginal != NULL)
	{
		return (CBCGPGraphicsManagerD2D*)m_pOriginal->CreateTextFormat(textFormat);
	}

	USES_CONVERSION;

	ASSERT(m_pWriteFactory != NULL);

	if (textFormat.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = textFormat.GetGraphicsManager();

		if (!IsGraphicsManagerValid(pGM))
		{
			return NULL;
		}
	
		return textFormat.GetHandle();
	}

	DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;

	switch (textFormat.GetFontStyle())
	{
	case CBCGPTextFormat::BCGP_FONT_STYLE_NORMAL:
		fontStyle = DWRITE_FONT_STYLE_NORMAL;
		break;

	case CBCGPTextFormat::BCGP_FONT_STYLE_OBLIQUE:
		fontStyle = DWRITE_FONT_STYLE_OBLIQUE;
		break;

	case CBCGPTextFormat::BCGP_FONT_STYLE_ITALIC:
		fontStyle = DWRITE_FONT_STYLE_ITALIC;
		break;
	}

	if (textFormat.GetFontSize() == 0. && textFormat.GetFontFamily().IsEmpty())
	{
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));

		globalData.fontRegular.GetLogFont(&lf);

		textFormat.CreateFromLogFont(lf);
	}

	IDWriteTextFormat* pWriteTextFormat = NULL;
	m_pWriteFactory->CreateTextFormat(T2CW(textFormat.GetFontFamily()), NULL, 
			(DWRITE_FONT_WEIGHT)textFormat.GetFontWeight(), fontStyle, 
			DWRITE_FONT_STRETCH_NORMAL, (float)fabs(textFormat.GetFontSize()), 
			T2CW(textFormat.GetFontLocale()), &pWriteTextFormat);

	textFormat.Set(this, (LPVOID)pWriteTextFormat);

	return textFormat.GetHandle();
}

BOOL CBCGPGraphicsManagerD2D::DestroyTextFormat(CBCGPTextFormat& textFormat)
{
	if (textFormat.GetHandle() != NULL && m_pWriteFactory != NULL)
	{
		IDWriteTextFormat* pWriteTextFormat = (IDWriteTextFormat*)textFormat.GetHandle();
		pWriteTextFormat->Release();
	}

	textFormat.Set(NULL, NULL);
	return TRUE;
}

LPVOID CBCGPGraphicsManagerD2D::CreateBrush(CBCGPBrush& brush)
{
	if (m_pOriginal != NULL)
	{
		return (CBCGPGraphicsManagerD2D*)m_pOriginal->CreateBrush(brush);
	}

	if (m_dcRenderTarget == NULL)
	{
		return NULL;
	}

	if (brush.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = brush.GetGraphicsManager();

		if (!IsGraphicsManagerValid(pGM))
		{
			return NULL;
		}
	
		return brush.GetHandle();
	}

	HRESULT hr = S_OK;

	D2D1_BRUSH_PROPERTIES brushProperties;
	brushProperties.opacity = (float)brush.GetOpacity();
	brushProperties.transform = D2D1::Matrix3x2F::Identity();

	switch (brush.GetGradientType())
	{
	case CBCGPBrush::BCGP_NO_GRADIENT:
		{
			ID2D1SolidColorBrush* solidColorBrush = NULL;

			hr = m_dcRenderTarget->CreateSolidColorBrush(
				D2DColor(brush.GetColor()), brushProperties, &solidColorBrush);

			if (FAILED(hr))
			{
				TRACE1("CBCGPGraphicsManagerD2D::CreateBrush: CreateSolidColorBrush Failed. %x\r\n", hr);
				return NULL;
			}

			brush.Set(this, (LPVOID)solidColorBrush);
		}
		break;

	case CBCGPBrush::BCGP_GRADIENT_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_DIAGONAL_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL:
	case CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL:
		{
			ID2D1LinearGradientBrush* linearGradientBrush = NULL;

			D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES linearGradientBrushProperties;
			linearGradientBrushProperties.startPoint = D2D1::Point2F();
			linearGradientBrushProperties.endPoint = D2D1::Point2F();

			ID2D1GradientStopCollection* gradientStopCollection = NULL;

			BOOL bIsCenter =	(brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_CENTER_HORIZONTAL) ||
								(brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_CENTER_VERTICAL);

			BOOL bIsPipe =		(brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_PIPE_HORIZONTAL) ||
								(brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL);

			const int nStops = (bIsCenter || bIsPipe) ? 3 : 2;

			D2D1_GRADIENT_STOP* stops = new D2D1_GRADIENT_STOP[nStops];

			if (bIsCenter)
			{
				stops[0].position = 0.;
				stops[0].color = D2DColor(brush.GetGradientColor());

				stops[1].position = .5;
				stops[1].color = D2DColor(brush.GetColor());

				stops[2].position = 1.;
				stops[2].color = D2DColor(brush.GetGradientColor());
			}
			else if (bIsPipe)
			{
				stops[0].position = 0.;
				stops[0].color = D2DColor(brush.GetColor());

				CBCGPColor clrLight = brush.GetGradientColor();
				if ((COLORREF)clrLight == CBCGPColor::White)
				{
					clrLight = brush.GetColor();
					clrLight.MakeLighter(.5);
				}

				stops[1].position = brush.GetGradientType() == CBCGPBrush::BCGP_GRADIENT_PIPE_VERTICAL ?
					0.7f : 0.3f;
				stops[1].color = D2DColor(clrLight);

				stops[2].position = 1.;
				stops[2].color = D2DColor(brush.GetColor());
			}
			else
			{
				stops[0].position = 0.;
				stops[0].color = D2DColor(brush.GetGradientColor());

				stops[1].position = 1.;
				stops[1].color = D2DColor(brush.GetColor());
			}

			hr = m_dcRenderTarget->CreateGradientStopCollection(
				stops,
				nStops,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&gradientStopCollection);

			delete[] stops;

			if (FAILED(hr))
			{
				TRACE1("CBCGPGraphicsManagerD2D::CreateBrush: CreateGradientStopCollection Failed. %x\r\n", hr);
				return NULL;
			}

			hr = m_dcRenderTarget->CreateLinearGradientBrush(
				&linearGradientBrushProperties, &brushProperties, 
				gradientStopCollection, &linearGradientBrush);

			if (FAILED(hr))
			{
				TRACE1("CBCGPGraphicsManagerD2D::CreateBrush: CreateLinearGradientBrush Failed. %x\r\n", hr);
				return NULL;
			}

			brush.Set(this, (LPVOID)linearGradientBrush);
			gradientStopCollection->Release();
		}
		break;

	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_CENTER:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_BOTTOM_RIGHT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_LEFT:
	case CBCGPBrush::BCGP_GRADIENT_RADIAL_RIGHT:
		{
			ID2D1RadialGradientBrush* radialGradientBrush = NULL;

			D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radialGradientBrushProperties;
			radialGradientBrushProperties.center = D2D1::Point2F();
			radialGradientBrushProperties.radiusX = radialGradientBrushProperties.radiusY = 10.;

			ID2D1GradientStopCollection* gradientStopCollection = NULL;

			D2D1_GRADIENT_STOP stops[2];

			stops[0].position = 0.;
			stops[0].color = D2DColor(brush.GetGradientColor());

			stops[1].position = 1.;
			stops[1].color = D2DColor(brush.GetColor());

			hr = m_dcRenderTarget->CreateGradientStopCollection(
				stops,
				2,
				D2D1_GAMMA_2_2,
				D2D1_EXTEND_MODE_CLAMP,
				&gradientStopCollection);

			if (FAILED(hr))
			{
				TRACE1("CBCGPGraphicsManagerD2D::CreateBrush: CreateGradientStopCollection Failed. %x\r\n", hr);
				return NULL;
			}

			hr = m_dcRenderTarget->CreateRadialGradientBrush(
				&radialGradientBrushProperties, &brushProperties, 
				gradientStopCollection, &radialGradientBrush);

			if (FAILED(hr))
			{
				TRACE1("CBCGPGraphicsManagerD2D::CreateBrush: CreateRadialGradientBrush Failed. %x\r\n", hr);
				return NULL;
			}

			brush.Set(this, (LPVOID)radialGradientBrush);
			gradientStopCollection->Release();
		}
		break;

	case CBCGPBrush::BCGP_GRADIENT_BEVEL:
		{
			CBCGPColor colorLight, colorDark;
			PrepareBevelColors(brush.GetColor(), colorLight, colorDark);

			CBCGPBrush br1(brush.GetColor(), CBCGPColor::White, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, brush.GetOpacity());
			CreateBrush(br1);

			CBCGPBrush br2(colorDark, brush.GetColor(), CBCGPBrush::BCGP_GRADIENT_HORIZONTAL, brush.GetOpacity());
			CreateBrush(br2);

			CBCGPBrush br3(colorDark, colorLight, CBCGPBrush::BCGP_GRADIENT_DIAGONAL_LEFT, brush.GetOpacity());
			CreateBrush(br3);

			brush.Set(this, br1.Detach(), br2.Detach(), br3.Detach());
		}
		break;
	}

	return brush.GetHandle();
}

BOOL CBCGPGraphicsManagerD2D::DestroyBrush(CBCGPBrush& brush)
{
	if (m_pDirect2dFactory != NULL)
	{
		if (brush.GetHandle() != NULL)
		{
			ID2D1Brush* pD2DBrush = (ID2D1Brush*)brush.GetHandle();
			pD2DBrush->Release();
		}

		if (brush.GetHandle1() != NULL)
		{
			ID2D1Brush* pD2DBrush = (ID2D1Brush*)brush.GetHandle1();
			pD2DBrush->Release();
		}

		if (brush.GetHandle2() != NULL)
		{
			ID2D1Brush* pD2DBrush = (ID2D1Brush*)brush.GetHandle2();
			pD2DBrush->Release();
		}
	}

	brush.Set(NULL, NULL);
	return TRUE;
}

void CBCGPGraphicsManagerD2D::SetClipRect(const CBCGPRect& rectClip, int nFlags)
{
	if (m_pRenderTarget == NULL)
	{
		return;
	}

	if (!m_rectClip.IsRectEmpty())
	{
		m_pRenderTarget->PopAxisAlignedClip();
		m_rectClip.SetRectEmpty();
	}

	if (nFlags != RGN_COPY)
	{
		SetClipArea(CBCGPRectangleGeometry(rectClip), nFlags);
		return;
	}

	ReleaseClipArea();

	m_pRenderTarget->PushAxisAlignedClip(D2DRect(rectClip), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	m_rectClip = rectClip;
}

void CBCGPGraphicsManagerD2D::SetClipArea(const CBCGPGeometry& geometry, int nFlags)
{
	HRESULT hr = S_OK;

	if (m_pRenderTarget == NULL)
	{
		return;
	}

	if (m_pCurrlayer != NULL)
	{
		m_pRenderTarget->PopLayer();

		m_pCurrlayer->Release();
		m_pCurrlayer = NULL;
	}

	if (!m_rectClip.IsRectEmpty())
	{
		m_pRenderTarget->PopAxisAlignedClip();
		m_rectClip.SetRectEmpty();
	}

	if (m_CurrGeometry.IsNull() || nFlags == RGN_COPY)
	{
		ReleaseClipArea();

		CBCGPPointsArray arPoints;
		m_CurrGeometry.SetPoints(arPoints);

		nFlags = RGN_OR;
	}

	CombineGeometry(m_CurrGeometry, geometry, m_CurrGeometry, nFlags);

	if (m_pCurrlayer == NULL)
	{
		hr = m_pRenderTarget->CreateLayer(&m_pCurrlayer);

		if (FAILED(hr))
		{
			TRACE1("CBCGPGraphicsManagerD2D::SetClipArea: CreateLayer Failed. %x\r\n", hr);
			return;
		}
	}

	D2D1_LAYER_PARAMETERS params;

	params.contentBounds = D2DRect(CBCGPRect(-D2D1::FloatMax(), -D2D1::FloatMax(), D2D1::FloatMax(),  D2D1::FloatMax()));
	params.geometricMask = (ID2D1Geometry*)m_CurrGeometry.GetHandle();
	params.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
	params.maskTransform = D2D1::Matrix3x2F::Identity();
	params.opacity = 1.0;
	params.opacityBrush = NULL;
	params.layerOptions = D2D1_LAYER_OPTIONS_NONE;
	
	m_pRenderTarget->PushLayer(&params, m_pCurrlayer);
}

void CBCGPGraphicsManagerD2D::ReleaseClipArea()
{
	if (m_pCurrlayer != NULL)
	{
		if (m_pRenderTarget != NULL)
		{
			m_pRenderTarget->PopLayer();
		}

		m_pCurrlayer->Release();
		m_pCurrlayer = NULL;
	}

	if (!m_rectClip.IsRectEmpty())
	{
		if (m_pRenderTarget != NULL)
		{
			m_pRenderTarget->PopAxisAlignedClip();
		}

		m_rectClip.SetRectEmpty();
	}

	DestroyGeometry(m_CurrGeometry);
}

void CBCGPGraphicsManagerD2D::CombineGeometry(CBCGPGeometry& geometryDest, const CBCGPGeometry& geometrySrc1, const CBCGPGeometry& geometrySrc2, int nFlags)
{
	if (m_pRenderTarget == NULL || m_pDirect2dFactory == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	ID2D1Geometry* pIGeometrySrc1 = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)geometrySrc1);
	ASSERT(pIGeometrySrc1 != NULL);

	ID2D1Geometry* pIGeometrySrc2 = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)geometrySrc2);
	ASSERT(pIGeometrySrc2 != NULL);

	ID2D1PathGeometry* pIGeometryDest = NULL;
	m_pDirect2dFactory->CreatePathGeometry(&pIGeometryDest);

	ID2D1GeometrySink* pGeometrySink = NULL;
	pIGeometryDest->Open(&pGeometrySink);

	D2D1_COMBINE_MODE mode = D2D1_COMBINE_MODE_UNION;

	switch (nFlags)
	{
	case RGN_OR:
		mode = D2D1_COMBINE_MODE_UNION;
		break;

	case RGN_AND:
		mode = D2D1_COMBINE_MODE_INTERSECT;
		break;

	case RGN_XOR:
		mode = D2D1_COMBINE_MODE_XOR;
		break;

	case RGN_DIFF:
		mode = D2D1_COMBINE_MODE_EXCLUDE;
		break;
	}

	pIGeometrySrc1->CombineWithGeometry(pIGeometrySrc2, mode, NULL, pGeometrySink);

	pGeometrySink->Close();
	pGeometrySink->Release();

	DestroyGeometry(geometryDest);
	geometryDest.Set(this, (LPVOID)pIGeometryDest);
}

void CBCGPGraphicsManagerD2D::GetGeometryBoundingRect(const CBCGPGeometry& geometry, CBCGPRect& rectOut)
{
	rectOut.SetRectEmpty();

	ID2D1Geometry* pGeometry = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)geometry);
	ASSERT(pGeometry != NULL);

	D2D1_RECT_F rect;
	pGeometry->GetBounds(NULL, &rect);

	rectOut = CBCGPRect(rect.left, rect.top, rect.right, rect.bottom);
}

LPVOID CBCGPGraphicsManagerD2D::CreateImage(CBCGPImage& image)
{
	if (m_pOriginal != NULL)
	{
		return (CBCGPGraphicsManagerD2D*)m_pOriginal->CreateImage(image);
	}

	USES_CONVERSION;

	if (image.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = image.GetGraphicsManager();

		if (!IsGraphicsManagerValid(pGM))
		{
			return NULL;
		}
	
		return image.GetHandle();
	}

	if (m_dcRenderTarget == NULL || m_pWicFactory == NULL)
	{
		ASSERT(FALSE);
		return NULL;
	}

	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICStream* pStream = NULL;
	IWICBitmapScaler* pScaler = NULL;
	IWICBitmap* pWICBitmap = NULL;
	ID2D1Bitmap* pBitmap = NULL;

	CString strPath = image.GetPath();
	UINT uiResID = image.GetResourceID();
	HICON hIcon = image.GetIcon();
	HBITMAP hBitmap = image.GetBitmap();

	HBITMAP hBmpSrc = NULL;

	HRESULT hr = S_OK;

	if (!strPath.IsEmpty())
	{
		hr = m_pWicFactory->CreateDecoderFromFilename(
			T2CW(strPath),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);

		if (FAILED(hr))
		{
			return NULL;
		}
	}
	else if (hIcon != NULL)
	{
		hr = m_pWicFactory->CreateBitmapFromHICON(hIcon, &pWICBitmap);

		if (FAILED(hr))
		{
			return NULL;
		}

		m_dcRenderTarget->CreateBitmapFromWicBitmap (pWICBitmap, 0, &pBitmap);
	}
	else if (hBitmap != NULL)
	{
		hr = m_pWicFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, image.IsIgnoreAlphaBitmap () ? WICBitmapIgnoreAlpha : WICBitmapUsePremultipliedAlpha, &pWICBitmap);

		if (FAILED(hr))
		{
			return NULL;
		}

		m_dcRenderTarget->CreateBitmapFromWicBitmap (pWICBitmap, 0, &pBitmap);
	}
	else if (uiResID != 0)
	{
		HRSRC imageResHandle = NULL;
		HGLOBAL imageResDataHandle = NULL;
		void *pImageFile = NULL;
		DWORD imageFileSize = 0;

		LPCTSTR lpszResourceName = MAKEINTRESOURCE(uiResID);
		ASSERT(lpszResourceName != NULL);

		// Locate the resource.
		LPCTSTR szResType = image.GetResourceType();

		HINSTANCE hInst = NULL;

		if (szResType == NULL)
		{
			// First, try PNG:
			szResType = _T("PNG");

			hInst = AfxFindResourceHandle(lpszResourceName, szResType);

			if (::FindResource(hInst, lpszResourceName, szResType) == NULL)
			{
				// Not found, try BMP:
				szResType = RT_BITMAP;
			}
		}

		if (hInst == NULL)
		{
			hInst = AfxFindResourceHandle(lpszResourceName, szResType);
		}

		if (szResType == RT_BITMAP)
		{
			hBmpSrc = (HBITMAP) ::LoadImage(hInst, lpszResourceName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			hr = hBmpSrc != NULL ? S_OK : E_FAIL;

			if (SUCCEEDED(hr))
			{
				hr = m_pWicFactory->CreateBitmapFromHBITMAP(hBmpSrc, NULL, WICBitmapUseAlpha, &pWICBitmap);

				if (SUCCEEDED(hr))
				{
					m_dcRenderTarget->CreateBitmapFromWicBitmap (pWICBitmap, 0, &pBitmap);
				}
			}
		}
		else		
		{
			if (hInst != NULL)
			{
				imageResHandle = FindResource(hInst, lpszResourceName, szResType);
			}

			hr = imageResHandle ? S_OK : E_FAIL;

			if (SUCCEEDED(hr))
			{
				// Load the resource.
				imageResDataHandle = LoadResource(hInst, imageResHandle);

				hr = imageResDataHandle ? S_OK : E_FAIL;
			}

			if (SUCCEEDED(hr))
			{
				// Lock it to get a system memory pointer.
				pImageFile = LockResource(imageResDataHandle);

				hr = pImageFile ? S_OK : E_FAIL;
			}
			if (SUCCEEDED(hr))
			{
				// Calculate the size.
				imageFileSize = SizeofResource(hInst, imageResHandle);

				hr = imageFileSize ? S_OK : E_FAIL;

			}
			if (SUCCEEDED(hr))
			{
				// Create a WIC stream to map onto the memory.
				hr = m_pWicFactory->CreateStream(&pStream);
			}
			if (SUCCEEDED(hr))
			{
				// Initialize the stream with the memory pointer and size.
				hr = pStream->InitializeFromMemory(
					reinterpret_cast<BYTE*>(pImageFile),
					imageFileSize
					);
			}
			if (SUCCEEDED(hr))
			{
				// Create a decoder for the stream.
				hr = m_pWicFactory->CreateDecoderFromStream(
					pStream,
					NULL,
					WICDecodeMetadataCacheOnLoad,
					&pDecoder
					);
			}
		}
	}

	if (pBitmap == NULL)
	{
		if (SUCCEEDED(hr) && pDecoder != NULL)
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
			pWICBitmap = (IWICBitmap*)pSource;
		}

		if (SUCCEEDED(hr))
		{
			// Convert the image format to 32bppPBGRA
			// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
			hr = m_pWicFactory->CreateFormatConverter(&pConverter);
		}

		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		UINT32 destinationWidth = 0/*m_sizeDest.width*/;
		UINT32 destinationHeight = 0/*m_sizeDest.height*/;

		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;

			hr = pWICBitmap->GetSize(&originalWidth, &originalHeight);

			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = m_pWicFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pWICBitmap,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
					if (SUCCEEDED(hr))
					{
						hr = pConverter->Initialize(
							pScaler,
							GUID_WICPixelFormat32bppPBGRA,
							WICBitmapDitherTypeNone,
							NULL,
							0.f,
							WICBitmapPaletteTypeMedianCut
							);
					}
				}
			}
		}
		else
		{
			if (SUCCEEDED(hr))
			{
				hr = pConverter->Initialize(pWICBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
					NULL, 0.f, WICBitmapPaletteTypeMedianCut);
			}
		}

		if (SUCCEEDED(hr))
		{
			// Create a Direct2D bitmap from the WIC bitmap.
			hr = m_dcRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, &pBitmap);
		}
	}

	if (pWICBitmap == (IWICBitmap*)pSource)
	{
		pWICBitmap = NULL;
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);
	SafeRelease(&pWICBitmap);

	image.Set(this, (LPVOID)pBitmap);

	return image.GetHandle();
}

BOOL CBCGPGraphicsManagerD2D::DestroyImage(CBCGPImage& image)
{
	if (image.GetHandle() != NULL && m_pWicFactory != NULL)
	{
		ID2D1Bitmap* pBitmap = (ID2D1Bitmap*)image.GetHandle();
		pBitmap->Release();
	}

	image.Set(NULL, NULL);
	return TRUE;
}

CBCGPSize CBCGPGraphicsManagerD2D::GetImageSize(CBCGPImage& image)
{
	ID2D1Bitmap* pBitmap = (ID2D1Bitmap*)CreateImage(image);
	if (pBitmap != NULL)
	{
		D2D1_SIZE_F sizeD2D = pBitmap->GetSize();

		return CBCGPSize(sizeD2D.width, sizeD2D.height);
	}

	return CBCGPSize();
}

BOOL CBCGPGraphicsManagerD2D::CopyImage(CBCGPImage& imageSrc, CBCGPImage& imageDest, 
										const CBCGPRect& rectSrc1)
{
	if (m_dcRenderTarget == NULL)
	{
		return FALSE;
	}

	ID2D1Bitmap* pBitmapSrc = (ID2D1Bitmap*)CreateImage(imageSrc);
	if (pBitmapSrc == NULL)
	{
		return FALSE;
	}

	CBCGPRect rectSrc = rectSrc1;
	if (!rectSrc.IsRectEmpty())
	{
		rectSrc.right += rectSrc.left;
		rectSrc.bottom += rectSrc.top;
	}

	DestroyImage(imageDest);

	ID2D1Bitmap* pBitmapDest = NULL;

	D2D1_BITMAP_PROPERTIES bitmapProps;

	pBitmapSrc->GetDpi(&bitmapProps.dpiX, &bitmapProps.dpiY);
	bitmapProps.pixelFormat = pBitmapSrc->GetPixelFormat();

	HRESULT hr = m_dcRenderTarget->CreateBitmap(
		D2DSizeU(rectSrc.IsRectEmpty() ? imageSrc.GetSize() : rectSrc.Size()), 
		bitmapProps, &pBitmapDest);

	if (FAILED(hr))
	{
		TRACE1("CBCGPGraphicsManagerD2D::CopyImage: CreateBitmap Failed. %x\r\n", hr);
		return FALSE;
	}

	D2D1_RECT_U rectD2D = D2DRectU(rectSrc);

	D2D1_POINT_2U pt = { 0, 0 };

	hr = pBitmapDest->CopyFromBitmap(&pt, pBitmapSrc, rectSrc.IsRectEmpty() ? NULL : &rectD2D);

	if (FAILED(hr))
	{
		TRACE1("CBCGPGraphicsManagerD2D::CopyImage: CopyFromBitmap Failed. %x\r\n", hr);
	}

	imageDest.Set(this, (LPVOID)pBitmapDest);

	return SUCCEEDED(hr);
}

HBITMAP CBCGPGraphicsManagerD2D::ExportImageToBitmap(CBCGPImage& image)
{
	ID2D1Bitmap* pBitmap = (ID2D1Bitmap*)CreateImage(image);
	if (pBitmap == NULL)
	{
		return NULL;
	}

	const CBCGPSize size = image.GetSize();

	HBITMAP hmbpDib = CBCGPDrawManager::CreateBitmap_32(size, NULL);
	if (hmbpDib == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	CDC dcMem;
	dcMem.CreateCompatibleDC (NULL);

	HBITMAP hbmpOld = (HBITMAP) dcMem.SelectObject (hmbpDib);

	{
		CBCGPGraphicsManagerD2D gm;
		CBCGPRect rect(CBCGPPoint(), size);
		gm.BindDC(&dcMem, rect);
		gm.BeginDraw();
		gm.Clear();
		gm.DrawImage(image, CBCGPPoint(0., 0.));
	}

	dcMem.SelectObject (hbmpOld);
    return hmbpDib;
}

LPVOID CBCGPGraphicsManagerD2D::CreateStrokeStyle(CBCGPStrokeStyle& style)
{
	if (m_pOriginal != NULL)
	{
		return (CBCGPGraphicsManagerD2D*)m_pOriginal->CreateStrokeStyle(style);
	}

	if (m_pDirect2dFactory == NULL)
	{
		return NULL;
	}

	if (style.GetHandle() != NULL)
	{
		CBCGPGraphicsManager* pGM = style.GetGraphicsManager();

		if (!IsGraphicsManagerValid(pGM))
		{
			return NULL;
		}
	
		return style.GetHandle();
	}

	D2D1_STROKE_STYLE_PROPERTIES strokeStyleProperties;

	strokeStyleProperties.startCap = (D2D1_CAP_STYLE)style.GetStartCap();
	strokeStyleProperties.endCap = (D2D1_CAP_STYLE)style.GetEndCap();
	strokeStyleProperties.dashCap = (D2D1_CAP_STYLE)style.GetDashCap();
	strokeStyleProperties.lineJoin = (D2D1_LINE_JOIN)style.GetLineJoin();
	strokeStyleProperties.miterLimit = style.GetMitterLimit();
	strokeStyleProperties.dashStyle = (D2D1_DASH_STYLE)style.GetDashStyle();
	strokeStyleProperties.dashOffset = style.GetDashOffset();

	ID2D1StrokeStyle* pD2DStrokeStyle = NULL;

	m_pDirect2dFactory->CreateStrokeStyle(&strokeStyleProperties, 
		style.GetDashes().GetData(), (int)style.GetDashes().GetSize(), &pD2DStrokeStyle);

	style.Set(this, (LPVOID)pD2DStrokeStyle);

	return pD2DStrokeStyle;
}

ID2D1StrokeStyle* CBCGPGraphicsManagerD2D::CreateStrokeStyle(const CBCGPStrokeStyle* pStyle, double lineWidth)
{
	if (pStyle == NULL || pStyle->IsEmpty())
	{
		return NULL;
	}

	if (lineWidth <= 2.5 && pStyle->GetDashStyle() == CBCGPStrokeStyle::BCGP_DASH_STYLE_SOLID)
	{
		return NULL;
	}

	return (ID2D1StrokeStyle*)CreateStrokeStyle((CBCGPStrokeStyle&)*pStyle);
}

BOOL CBCGPGraphicsManagerD2D::DestroyStrokeStyle(CBCGPStrokeStyle& style)
{
	if (style.GetHandle() != NULL && m_pDirect2dFactory != NULL)
	{
		ID2D1StrokeStyle* pD2DStrokeStyle = (ID2D1StrokeStyle*)style.GetHandle();
		pD2DStrokeStyle->Release();
	}

	style.Set(NULL, NULL);
	return TRUE;
}

void CBCGPGraphicsManagerD2D::OnFillGeometryBevel(const CBCGPBrush& brFill, const CBCGPGeometry& shape, const CBCGPGeometry& shapeInternal)
{
	if (m_pRenderTarget == NULL)
	{
		return;
	}

	ID2D1Brush* pD2DBrush = (ID2D1Brush*)CreateBrush((CBCGPBrush&)brFill);
	if (pD2DBrush == NULL)
	{
		return;
	}

	ID2D1Brush* pD2DBrush2 = (ID2D1Brush*)brFill.GetHandle2();
	if (pD2DBrush2 == NULL)
	{
		return;
	}

	ID2D1Geometry* pGeometry = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)shape);
	ASSERT(pGeometry != NULL);

	D2D1_RECT_F rect;
	pGeometry->GetBounds(NULL, &rect);

	CBCGPRect rectGradient(rect.left, rect.top, rect.right, rect.bottom);
	PrepareGradientPoints(pD2DBrush2, rectGradient);

	m_pRenderTarget->FillGeometry(pGeometry, pD2DBrush2);

	ID2D1Geometry* pGeometryInternal = (ID2D1Geometry*)CreateGeometry((CBCGPGeometry&)shapeInternal);
	ASSERT(pGeometryInternal != NULL);

	pGeometryInternal->GetBounds(NULL, &rect);

	CBCGPRect rectGradientInternal(rect.left, rect.top, rect.right, rect.bottom);
	PrepareGradientPoints(pD2DBrush, rectGradientInternal);

	m_pRenderTarget->FillGeometry(pGeometryInternal, pD2DBrush);
}

BOOL CBCGPGraphicsManagerD2D::IsGraphicsManagerValid(CBCGPGraphicsManager* pGM)
{
	if (pGM == NULL)
	{
		return TRUE;
	}

	while (pGM->GetOriginal() != NULL)
	{
		pGM = pGM->GetOriginal();
	}

	CBCGPGraphicsManager* pThis = this;

	while (pThis->GetOriginal() != NULL)
	{
		pThis = pThis->GetOriginal();
	}

	if (pGM == pThis)
	{
		return TRUE;
	}

	TRACE0("You cannot use the same D2D resource with the different graphics managers!\n");
	ASSERT(FALSE);

	return FALSE;
}

void CBCGPGraphicsManagerD2D::EnableAntialiasing(BOOL bEnable)
{
	if (m_pRenderTarget == NULL)
	{
		return;
	}

	m_pRenderTarget->SetAntialiasMode(bEnable ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED);
}

BOOL CBCGPGraphicsManagerD2D::IsAntialiasingEnabled() const
{
	if (m_pRenderTarget == NULL)
	{
		return FALSE;
	}

	return m_pRenderTarget->GetAntialiasMode() == D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
}
