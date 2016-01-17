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
// BCGPVisualContainer.cpp: implementation of the CBCGPVisualContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPMath.h"
#include "BCGPVisualContainer.h"
#include "BCGPVisualCtrl.h"

#if (!defined _BCGSUITE_) && (!defined _BCGPCHART_STANDALONE)
#include "BCGPVisualCollector.h"
#include "BCGPVisualConstructor.h"
#include "BCGPPngImage.h"
#endif

#include "BCGPDrawManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT BCGM_POSTREDRAW = ::RegisterWindowMessage (_T("BCGM_POSTREDRAW"));

IMPLEMENT_DYNCREATE(CBCGPVisualContainer, CObject)
IMPLEMENT_DYNAMIC(CBCGPVisualDataObject, CObject)

CMap<UINT,UINT,CBCGPBaseVisualObject*,CBCGPBaseVisualObject*> CBCGPBaseVisualObject::m_mapAnimations;
CCriticalSection CBCGPBaseVisualObject::g_cs;

IMPLEMENT_DYNAMIC(CBCGPBaseVisualObject, CObject)

#define SEL_MARKER_SIZE 4

#ifdef _BCGSUITE_
class CBCGPToolBarImagesForSave : public CBCGPToolBarImages
{
public:
	CBCGPToolBarImagesForSave()
	{
		m_bUserImagesList = TRUE;
	}
};
#endif

static BOOL SaveBitmapToFile(HBITMAP hbmp, const CString& strFilePath)
{
	if (hbmp == NULL)
	{
		return FALSE;
	}

	TCHAR ext[_MAX_EXT];

#if _MSC_VER < 1400
	_tsplitpath (strFilePath, NULL, NULL, NULL, ext);
#else
	_tsplitpath_s (strFilePath, NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT);
#endif

	CString strExt = ext;
	strExt.MakeUpper();

#if (!defined _BCGSUITE_) && (!defined _BCGPCHART_STANDALONE)
	if (strExt == _T(".PNG"))
	{
		CBCGPPngImage pngImage;
		pngImage.Attach(hbmp);

		return pngImage.SaveToFile(strFilePath);
	}
	else
#endif
	{
#ifdef _BCGSUITE_
		CBCGPToolBarImagesForSave img;
#else
		CBCGPToolBarImages img;
#endif
		img.AddImage(hbmp, TRUE);
		img.SetSingleImage();

		return img.Save(strFilePath);
	}
}

//////////////////////////////////////////////////////////////////////
// CBCGPBaseVisualObject

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPBaseVisualObject::CBCGPBaseVisualObject(CBCGPVisualContainer* pContainer)
{
	m_pParentContainer = NULL;

	if (pContainer != NULL)
	{
		ASSERT_VALID(pContainer);
		pContainer->Add(this);
	}
	else
	{
		m_pWndOwner = NULL;
	}

	m_nID = 0;
	m_bIsVisible = TRUE;
	m_rect.SetRectEmpty();
	m_bIsAutoDestroy = TRUE;
	m_bIsDirty = TRUE;
	m_bCacheImage = FALSE;
	m_bIsSelected = FALSE;
	m_uiEditFlags = 0;
	m_bIsTracked = FALSE;
	m_sizeScaleRatio = CBCGPSize(1., 1.);
}
//*******************************************************************************
CBCGPBaseVisualObject::~CBCGPBaseVisualObject()
{
	StopAllAnimations();

	int i = 0;

	for (i = 0; i < m_arData.GetSize(); i++)
	{
		CBCGPVisualDataObject* pData = m_arData[i];
		ASSERT_VALID(pData);

		delete pData;
	}

	m_arData.RemoveAll();
}
//*******************************************************************************
CWnd* CBCGPBaseVisualObject::SetOwner(CWnd* pWndOwner, BOOL bRedraw/* = TRUE*/)
{
	CWnd* pOldOwner = m_pWndOwner;

	SetDirty();

	if (m_pWndOwner != pWndOwner)
	{
		if (pWndOwner->GetSafeHwnd () == NULL)
		{
			bRedraw = FALSE;
		}

		m_pWndOwner = pWndOwner;

		StopAllAnimations(bRedraw);

		if (bRedraw)
		{
			Redraw ();
		}
	}

	return pOldOwner;
}
//*******************************************************************************
void CBCGPBaseVisualObject::SetRect(const CBCGPRect& rect, BOOL bRedraw)
{
	if (m_rect != rect)
	{
		m_rect = rect;
		SetDirty();
	}

	if (m_bIsSelected)
	{
		SetSelected();
	}

	if (bRedraw)
	{
		Redraw();
	}
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::SetValue(double dblVal, int nIndex, UINT uiAnimationTime, BOOL bRedraw)
{
	if (nIndex < 0 || nIndex >= m_arData.GetSize())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CBCGPVisualDataObject* pData = m_arData[nIndex];
	ASSERT_VALID(pData);

	const double dblRange = GetAnimationRange(nIndex);

	SetDirty();

	if (uiAnimationTime == 0 || dblRange == 0.)
	{
		pData->SetValue(dblVal);

		if (bRedraw)
		{
			Redraw();
		}

		return TRUE;
	}

	pData->SetAnimatedValue(pData->GetValue());
	pData->SetValue(dblVal);

	if (pData->GetAnimationID() > 0)
	{
		::KillTimer(NULL, pData->GetAnimationID());
	}

	pData->SetAnimationStep(fabs((pData->GetValue() - pData->GetAnimatedValue()) / 10));

	double dblSteps = fabs((dblRange) / (pData->GetValue() - pData->GetAnimatedValue()));

	pData->SetAnimationID ((UINT) ::SetTimer (NULL, 0, 
		(int)(.5 + (double)uiAnimationTime / dblSteps), AnimTimerProc));

	g_cs.Lock ();
	m_mapAnimations.SetAt (pData->GetAnimationID(), this);
	g_cs.Unlock ();

	return TRUE;
}
//*******************************************************************************
double CBCGPBaseVisualObject::GetValue(int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_arData.GetSize())
	{
		ASSERT(FALSE);
		return 0.;
	}

	return m_arData[nIndex]->GetValue();
}
//*******************************************************************************
VOID CALLBACK CBCGPBaseVisualObject::AnimTimerProc (HWND /*hwnd*/, UINT /*uMsg*/,
													   UINT_PTR idEvent, DWORD /*dwTime*/)
{
	CBCGPBaseVisualObject* pObject = NULL;

	g_cs.Lock ();

	if (!m_mapAnimations.Lookup ((UINT) idEvent, pObject))
	{
		g_cs.Unlock ();
		return;
	}

	ASSERT_VALID(pObject);

	g_cs.Unlock ();

	BOOL bStopAnimation = pObject->OnAnimation((UINT)idEvent);

	if (bStopAnimation)
	{
		pObject->StopAnimation((UINT)idEvent, TRUE);
	}
	else
	{
		pObject->Redraw();
	}
}
//*******************************************************************************
CBCGPVisualDataObject* CBCGPBaseVisualObject::GetAnimated(UINT uiID) const
{
	for (int i = 0; i < m_arData.GetSize(); i++)
	{
		CBCGPVisualDataObject* pData = m_arData[i];
		ASSERT_VALID(pData);

		if (pData->GetAnimationID() == uiID)
		{
			return pData;
		}
	}

	return NULL;
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::OnAnimation(UINT idEvent)
{
	CBCGPVisualDataObject* pData = GetAnimated(idEvent);
	if (pData == NULL)
	{
		ASSERT(FALSE);
		return TRUE;
	}

	BOOL bStopAnimation = FALSE;

	if (pData->GetAnimatedValue() < pData->GetValue())
	{
		pData->SetAnimatedValue(pData->GetAnimatedValue() + pData->GetAnimationStep());
		bStopAnimation = pData->GetAnimatedValue() >= pData->GetValue();
	}
	else
	{
		pData->SetAnimatedValue(pData->GetAnimatedValue() - pData->GetAnimationStep());
		bStopAnimation = pData->GetAnimatedValue() <= pData->GetValue();
	}

	return bStopAnimation;
}
//*******************************************************************************
void CBCGPBaseVisualObject::StopAnimation(UINT id, BOOL bRedraw)
{
	ASSERT_VALID (this);

	if (id == 0)
	{
		return;
	}

	::KillTimer (NULL, id);

	g_cs.Lock ();
	m_mapAnimations.RemoveKey (id);
	g_cs.Unlock ();

	CBCGPVisualDataObject* pData = GetAnimated(id);
	if (pData == NULL)
	{
		ASSERT(FALSE);
	}
	else
	{
		pData->SetAnimationID(0);
		pData->SetAnimatedValue(0.);
		pData->SetAnimationStep(0.);
	}

	if (bRedraw)
	{
		Redraw();
	}
}
//*******************************************************************************
void CBCGPBaseVisualObject::StopAllAnimations(BOOL bRedraw)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arData.GetSize(); i++)
	{
		CBCGPVisualDataObject* pData = m_arData[i];
		ASSERT_VALID(pData);

		StopAnimation(pData->GetAnimationID(), bRedraw);
	}
}
//*******************************************************************************
void CBCGPBaseVisualObject::Redraw()
{
	ASSERT_VALID (this);

	if (m_pWndOwner->GetSafeHwnd() != NULL && !m_rect.IsRectEmpty())
	{
		if (m_pWndOwner->GetExStyle() & WS_EX_LAYERED)
		{
			CBCGPBaseVisualCtrl* pCtrl = DYNAMIC_DOWNCAST(CBCGPBaseVisualCtrl, m_pWndOwner);
			if (pCtrl != NULL)
			{
				pCtrl->OnDrawLayeredPopup();
				return;
			}
		}

		m_pWndOwner->RedrawWindow();
	}
}
//*******************************************************************************
void CBCGPBaseVisualObject::PostRedraw(BOOL bSetDirty)
{
	ASSERT_VALID (this);

	if (m_pWndOwner->GetSafeHwnd() != NULL && !m_rect.IsRectEmpty())
	{
		m_pWndOwner->PostMessage(BCGM_POSTREDRAW, (WPARAM)bSetDirty);
	}
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::OnSetMouseCursor(const CBCGPPoint& pt)
{
	int nHitTest = HitTest(pt);

	switch (nHitTest)
	{
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_SIZENWSE));
		return TRUE;

	case HTTOP:
	case HTBOTTOM:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_SIZENS));
		return TRUE;

	case HTRIGHT:
	case HTLEFT:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_SIZEWE));
		return TRUE;

	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_SIZENESW));
		return TRUE;

	case HTCAPTION:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_SIZEALL));
		return TRUE;
	}

	return FALSE;
}
//*******************************************************************************
HBITMAP CBCGPBaseVisualObject::ExportToBitmap(CBCGPGraphicsManager* pGM)
{
	ASSERT_VALID(pGM);

	const CBCGPSize size = m_rect.Size();

	HBITMAP hmbpDib = CBCGPDrawManager::CreateBitmap_32(size, NULL);
	if (hmbpDib == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	CBCGPRect rect(CBCGPPoint(), size);

	CDC dcMem;
	dcMem.CreateCompatibleDC (NULL);

	HBITMAP hbmpOld = (HBITMAP) dcMem.SelectObject (hmbpDib);

	pGM->BindDC(&dcMem, rect);
	pGM->BeginDraw();

	CBCGPRect rectSaved = m_rect;

	SetRect(m_rect - rectSaved.TopLeft());
	SetDirty();

	BOOL bCacheImage = m_bCacheImage;
	m_bCacheImage = FALSE;

	pGM->FillRectangle(m_rect, CBCGPBrush(CBCGPColor::White));

	OnDraw(pGM, m_rect);

	pGM->EndDraw();

	dcMem.SelectObject (hbmpOld);

	CBCGPDrawManager::FillAlpha (rect, hmbpDib, 255);

	pGM->BindDC(NULL);

	SetRect(rectSaved);
	m_bCacheImage = bCacheImage;

	SetDirty();

	return hmbpDib;
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::ExportToImage(CBCGPGraphicsManager* pGMSrc, CBCGPImage& image)
{
	ASSERT_VALID(pGMSrc);

	CBCGPGraphicsManager* pGM = pGMSrc->CreateOffScreenManager(m_rect, &image);

	if (pGM == NULL)
	{
		return FALSE;
	}

	CBCGPRect rectSaved = m_rect;
	m_rect = m_rect - rectSaved.TopLeft();

	pGM->FillRectangle(m_rect, CBCGPBrush(CBCGPColor::White));

	SetDirty();
	OnDraw(pGM, CBCGPRect());

	m_rect = rectSaved;
	delete pGM;

	return TRUE;
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::CopyToClipboard(CBCGPGraphicsManager* pGM)
{
	if (pGM == NULL)
	{
		return FALSE;
	}

	HBITMAP hbmp = ExportToBitmap(pGM);

	CBCGPToolBarImages img;
	img.AddImage(hbmp, TRUE);
	img.SetSingleImage();

	return img.CopyImageToClipboard(0);
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::ExportToFile(const CString& strFilePath, CBCGPGraphicsManager* pGM)
{
	if (pGM == NULL)
	{
		return FALSE;
	}

	return SaveBitmapToFile(ExportToBitmap(pGM), strFilePath);
}
//*******************************************************************************
void CBCGPBaseVisualObject::EnableImageCache(BOOL bEnable)
{
	m_bCacheImage = bEnable;
}
//*******************************************************************************
CBCGPRect CBCGPBaseVisualObject::MakeTrackMarker(double x, double y)
{
	CBCGPRect rect(x, y, x, y);
	rect.InflateRect(SEL_MARKER_SIZE, SEL_MARKER_SIZE);

	return rect;
}
//*******************************************************************************
CBCGPRect CBCGPBaseVisualObject::MakeTrackMarker(const CBCGPPoint& pt)
{
	return MakeTrackMarker(pt.x, pt.y);
}
//*******************************************************************************
void CBCGPBaseVisualObject::SetSelected(BOOL bSet)
{
	if (!IsEditMode())
	{
		ASSERT(FALSE);
		return;
	}

	if (!bSet)
	{
		m_rectTrack.SetRectEmpty();
		m_bIsSelected = FALSE;
		m_mapTrackRects.RemoveAll();
		return;
	}

	SetTrackingRect(m_rect);
	m_bIsSelected = TRUE;
}
//*******************************************************************************
void CBCGPBaseVisualObject::SetTrackingRect(const CBCGPRect& rect)
{
	const CBCGPSize sizeMin = GetMinSize();

	if (!sizeMin.IsEmpty())
	{
		if (rect.Width() < sizeMin.cx || rect.Height() < sizeMin.cy)
		{
			return;
		}
	}

	m_rectTrack = rect;

	if ((m_uiEditFlags & BCGP_EDIT_NOSIZE) == BCGP_EDIT_NOSIZE)
	{
		return;
	}

	m_mapTrackRects[HTTOPLEFT] = MakeTrackMarker(m_rectTrack.TopLeft());
	m_mapTrackRects[HTBOTTOMRIGHT] = MakeTrackMarker(m_rectTrack.BottomRight());
	m_mapTrackRects[HTTOPRIGHT] = MakeTrackMarker(m_rectTrack.right, m_rectTrack.top);
	m_mapTrackRects[HTBOTTOMLEFT] = MakeTrackMarker(m_rectTrack.left, m_rectTrack.bottom);

	m_mapTrackRects[HTLEFT] = MakeTrackMarker(m_rectTrack.left, m_rectTrack.CenterPoint().y);
	m_mapTrackRects[HTRIGHT] = MakeTrackMarker(m_rectTrack.right, m_rectTrack.CenterPoint().y);
	m_mapTrackRects[HTTOP] = MakeTrackMarker(m_rectTrack.CenterPoint().x, m_rectTrack.top);
	m_mapTrackRects[HTBOTTOM] = MakeTrackMarker(m_rectTrack.CenterPoint().x, m_rectTrack.bottom);
}
//*******************************************************************************
BOOL CBCGPBaseVisualObject::IsEditMode() const
{
	return m_pParentContainer != NULL && m_pParentContainer->IsEditMode();
}
//*******************************************************************************
int CBCGPBaseVisualObject::HitTest(const CBCGPPoint& pt) const
{
	if (m_bIsSelected)
	{
		for (POSITION pos = m_mapTrackRects.GetStartPosition (); pos != NULL;)
		{
			UINT uiHitTest = 0;
			CBCGPRect rect;

			m_mapTrackRects.GetNextAssoc (pos, uiHitTest, rect);

			if (rect.PtInRect(pt))
			{
				return uiHitTest;
			}
		}
	}

	CBCGPRect rect = m_rect;
	rect.Normalize();

	if (!rect.PtInRect(pt))
	{
		return HTNOWHERE;
	}

	if (IsEditMode() && (m_uiEditFlags & BCGP_EDIT_NOMOVE) == 0)
	{
		return HTCAPTION;
	}

	return  HTCLIENT;
}
//*******************************************************************************
void CBCGPBaseVisualObject::DrawTrackingRect(CBCGPGraphicsManager* pGM,
											 const CBCGPBrush& brOutline, const CBCGPBrush& brFill)
{
	if (m_rectTrack.IsRectEmpty())
	{
		return;
	}

	pGM->DrawRectangle(m_rectTrack, brOutline);

	for (POSITION pos = m_mapTrackRects.GetStartPosition (); pos != NULL;)
	{
		UINT uiHitTest = 0;
		CBCGPRect rect;

		m_mapTrackRects.GetNextAssoc (pos, uiHitTest, rect);

		switch (uiHitTest)
		{
		case HTTOPLEFT:
		case HTBOTTOMRIGHT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
			pGM->FillEllipse(rect, brFill);
			pGM->DrawEllipse(rect, brOutline);
			break;

		default:
			pGM->FillRectangle(rect, brFill);
			pGM->DrawRectangle(rect, brOutline);
		}
	}
}

#ifndef _BCGPCHART_STANDALONE

//////////////////////////////////////////////////////////////////////
// CBCGPVisualLayout

void CBCGPVisualLayout::AdjustLayout ()
{
	const int count = (int)m_listWnd.GetCount ();
	if (count == 0)
	{
		return;
	}

	POSITION pos = m_listWnd.GetHeadPosition ();
	while (pos != NULL)
	{
		XWndItem& item = m_listWnd.GetNext (pos);

		int nIndex = m_container.FindIndex((const CBCGPBaseVisualObject*)item.m_Handle);
		if (nIndex >= 0)
		{
			CRect rectItem;
			UINT uiFlags = CalculateItem (item, rectItem);

			if ((uiFlags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE))
			{
				CBCGPBaseVisualObject* pObject = (CBCGPBaseVisualObject*)m_container[nIndex];
				pObject->SetRect(rectItem);
			}
		}
	}
}
//*******************************************************************************
CRect CBCGPVisualLayout::GetItemRect(XWndItem& item) const
{
	int nIndex = m_container.FindIndex((const CBCGPBaseVisualObject*)item.m_Handle);
	if (nIndex >= 0)
	{
		return m_container[nIndex]->GetRect();
	}

	return CRect (0, 0, 0, 0);
}
//********************************************************************************
BOOL CBCGPVisualLayout::AddAnchor(CBCGPBaseVisualObject* pObject, XMoveType typeMove, XSizeType typeSize, 
								  const CPoint& percMove, const CPoint& percSize)
{
	if (pObject == NULL || m_container.FindIndex(pObject) < 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	return CBCGPStaticLayout::AddAnchor ((LPVOID)pObject, typeMove, typeSize, percMove, percSize);
}
//********************************************************************************
void CBCGPVisualLayout::GetHostWndRect(CRect& rect) const
{
	rect = m_container.GetRect();
}

#endif

//////////////////////////////////////////////////////////////////////
// CBCGPVisualContainer

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPVisualContainer::CBCGPVisualContainer(CWnd* pWndOwner)
{
#ifndef _BCGPCHART_STANDALONE
	m_pLayout = NULL;
#endif

	m_bDrawDynamicObjectsOnTop = TRUE;
	m_pWndOwner = pWndOwner;
	m_rect.SetRectEmpty();
	m_bCacheImage = FALSE;
	m_bIsEditMode = FALSE;
	m_dwEditFlags = 0;
	m_nDragMode = HTNOWHERE;
	m_ptDragStart = m_ptDragFinish = CBCGPPoint(-1, -1);

	m_brSelFill = CBCGPBrush(CBCGPColor::LightBlue, .5);
	m_brSelOutline = CBCGPBrush(CBCGPColor::SteelBlue);
	m_brGrid = CBCGPBrush(CBCGPColor::WhiteSmoke);

	m_sizeGrid = CBCGPSize(10., 10.);

	m_pNewObject = NULL;
	m_bAddNewObjectMode = FALSE;

	m_GMType = (CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER)-1;
}
//*******************************************************************************
CBCGPVisualContainer::~CBCGPVisualContainer()
{
#ifndef _BCGPCHART_STANDALONE
	if (m_pLayout != NULL)
	{
		delete m_pLayout;
		m_pLayout = NULL;
	}
#endif

	RemoveAll();
}
//*******************************************************************************
CWnd* CBCGPVisualContainer::SetOwner(CWnd* pWndOwner, BOOL bRedraw/* = TRUE*/)
{
	CWnd* pOldOwner = m_pWndOwner;

	if (m_pWndOwner != pWndOwner)
	{
		m_pWndOwner = pWndOwner;

		for (int i = 0; i < m_arObjects.GetSize(); i++)
		{
			CBCGPBaseVisualObject* pObject = m_arObjects[i];
			ASSERT_VALID(pObject);

			pObject->SetOwner(m_pWndOwner, FALSE);
		}

		if (bRedraw && m_pWndOwner->GetSafeHwnd () != NULL)
		{
			Redraw ();
		}
	}

	return pOldOwner;
}
//*******************************************************************************
void CBCGPVisualContainer::EnableImageCache(BOOL bEnable)
{
	m_bCacheImage = bEnable;

	if (!m_bCacheImage)
	{
		m_ImageCache.Destroy();
	}
}
//*******************************************************************************
BOOL CBCGPVisualContainer::Add(CBCGPBaseVisualObject* pObject, BOOL bAutoDestroy)
{
	ASSERT_VALID(pObject);
	return InsertAt(pObject, -1, bAutoDestroy);
}
//*******************************************************************************
BOOL CBCGPVisualContainer::InsertAt(CBCGPBaseVisualObject* pObject, int nIndex, BOOL bAutoDestroy)
{
	ASSERT_VALID(pObject);
	ASSERT(pObject->m_pParentContainer == NULL);

	if (nIndex == -1)
	{
		nIndex = (int)m_arObjects.GetSize();
	}

	if (nIndex < 0 || nIndex > m_arObjects.GetSize())
	{
		return FALSE;
	}

	if (FindIndex(pObject) < 0)
	{
		ASSERT(pObject->m_pParentContainer == NULL);
		m_arObjects.InsertAt(nIndex, pObject);
	}

	pObject->m_pParentContainer = this;
	pObject->m_pWndOwner = m_pWndOwner;
	pObject->m_bIsAutoDestroy = bAutoDestroy;

	AdjustLayout();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::Remove(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arObjects.GetSize())
	{
		return FALSE;
	}

	CBCGPBaseVisualObject* pObject = m_arObjects[nIndex];
	ASSERT_VALID(pObject);

	POSITION pos = m_lstSel.Find(pObject);
	if (pos != NULL)
	{
		m_lstSel.RemoveAt(pos);
	}

	if (pObject->m_bIsAutoDestroy)
	{
		delete pObject;
	}

	m_arObjects.RemoveAt(nIndex);
	AdjustLayout();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::Remove(CBCGPBaseVisualObject* pObject)
{
	int nIndex = FindIndex(pObject);
	if (nIndex == -1)
	{
		return FALSE;
	}

	return Remove(nIndex);
}
//*******************************************************************************
void CBCGPVisualContainer::RemoveAll()
{
	m_lstSel.RemoveAll();

	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		if (pObject->m_bIsAutoDestroy)
		{
			delete pObject;
		}
		else
		{
			pObject->m_pParentContainer = NULL;
		}
	}

	m_arObjects.RemoveAll();
	AdjustLayout();
}
//*******************************************************************************
int CBCGPVisualContainer::GetCount() const
{
	return (int)m_arObjects.GetSize();
}
//*******************************************************************************
CBCGPBaseVisualObject* CBCGPVisualContainer::GetAt(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arObjects.GetSize())
	{
		return NULL;
	}

	CBCGPBaseVisualObject* pObject = m_arObjects.GetAt(nIndex);
	ASSERT_VALID(pObject);

	return pObject;
}
//*******************************************************************************
const CBCGPBaseVisualObject* CBCGPVisualContainer::operator[](int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_arObjects.GetSize())
	{
		return NULL;
	}

	const CBCGPBaseVisualObject* pObject = m_arObjects[nIndex];
	ASSERT_VALID(pObject);

	return pObject;
}
//*******************************************************************************
CBCGPBaseVisualObject* CBCGPVisualContainer::GetFromPoint(const CPoint& point)
{
	for (POSITION pos = m_lstSel.GetHeadPosition(); pos != NULL;)
	{
		CBCGPBaseVisualObject* pObject = m_lstSel.GetNext(pos);
		ASSERT_VALID(pObject);

		if (pObject->HitTest(point) != HTNOWHERE)
		{
			return pObject;
		}
	}

	int count = (int)m_arObjects.GetSize ();

	for (int i = 0; i < count; i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[count - i - 1];
		ASSERT_VALID(pObject);

		CBCGPRect rect = pObject->m_rect;
		rect.Normalize();

		if (rect.PtInRect (point))
		{
			return pObject;
		}
	}

	return NULL;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::MoveForward(CBCGPBaseVisualObject* pObject)
{
	ASSERT_VALID(pObject);

	int nIndex = FindIndex(pObject);
	if (nIndex < 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (nIndex == m_arObjects.GetSize() - 1)
	{
		return FALSE;
	}

	CBCGPBaseVisualObject* pObjectNext = m_arObjects[nIndex + 1];

	m_arObjects[nIndex + 1] = pObject;
	m_arObjects[nIndex] = pObjectNext;

	m_ImageCache.Destroy();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::MoveBackward(CBCGPBaseVisualObject* pObject)
{
	ASSERT_VALID(pObject);

	int nIndex = FindIndex(pObject);
	if (nIndex < 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (nIndex == 0)
	{
		return FALSE;
	}

	CBCGPBaseVisualObject* pObjectPrev = m_arObjects[nIndex - 1];

	m_arObjects[nIndex - 1] = pObject;
	m_arObjects[nIndex] = pObjectPrev;

	m_ImageCache.Destroy();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::MoveToFront(CBCGPBaseVisualObject* pObject)
{
	ASSERT_VALID(pObject);

	int nIndex = FindIndex(pObject);
	if (nIndex < 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (nIndex == m_arObjects.GetSize() - 1)
	{
		return FALSE;
	}

	for (int i = nIndex; i < m_arObjects.GetSize() - 1; i++)
	{
		m_arObjects[i] = m_arObjects[i + 1];
	}

	m_arObjects[m_arObjects.GetSize() - 1] = pObject;

	m_ImageCache.Destroy();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::MoveToBack(CBCGPBaseVisualObject* pObject)
{
	ASSERT_VALID(pObject);

	int nIndex = FindIndex(pObject);
	if (nIndex < 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (nIndex == 0)
	{
		return FALSE;
	}

	for (int i = nIndex; i > 0; i--)
	{
		m_arObjects[i] = m_arObjects[i - 1];
	}

	m_arObjects[0] = pObject;

	m_ImageCache.Destroy();
	return TRUE;
}
//*******************************************************************************
int CBCGPVisualContainer::FindIndex(const CBCGPBaseVisualObject* pObject) const
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pArObject = m_arObjects[i];
		ASSERT_VALID(pArObject);

		if (pArObject == pObject)
		{
			return i;
		}
	}

	return -1;
}
//*******************************************************************************
CBCGPRect CBCGPVisualContainer::GetBoundsRect(BOOL bOnlyVisible) const
{
	CBCGPRect rect(0, 0, 0, 0);

	if (bOnlyVisible)
	{
		for (int i = 0; i < m_arObjects.GetSize(); i++)
		{
			CBCGPBaseVisualObject* pObject = m_arObjects[i];
			ASSERT_VALID(pObject);

			if (pObject->IsVisible ())
			{
				rect.UnionRect(rect, pObject->m_rect);
			}
		}
	}
	else
	{
		for (int i = 0; i < m_arObjects.GetSize(); i++)
		{
			CBCGPBaseVisualObject* pObject = m_arObjects[i];
			ASSERT_VALID(pObject);

			rect.UnionRect(rect, pObject->m_rect);
		}
	}

	return rect;
}
//*******************************************************************************
void CBCGPVisualContainer::SetRect(const CBCGPRect& rect, BOOL bReposObjects, BOOL bRedraw)
{
	CBCGPRect rectOld = m_rect;

	m_rect = rect;

	if (m_rect == rectOld)
	{
		return;
	}

	if (bReposObjects)
	{
		double dx = m_rect.left;
		double dy = m_rect.top;

		if (!rectOld.IsRectEmpty())
		{
			dx -= rectOld.left;
			dy -= rectOld.top;
		}

		for (int i = 0; i < m_arObjects.GetSize(); i++)
		{
			CBCGPBaseVisualObject* pObject = m_arObjects[i];
			ASSERT_VALID(pObject);

			pObject->m_rect.OffsetRect(dx, dy);
		}
	}

	AdjustLayout();

	if (bRedraw)
	{
		Redraw();
	}
}
//*******************************************************************************
void CBCGPVisualContainer::EnableEditMode(BOOL bEnable, DWORD dwFlags)
{
	m_bIsEditMode = bEnable;
	m_dwEditFlags = dwFlags;
}
//*******************************************************************************
void CBCGPVisualContainer::SetDirty(BOOL bSet) const
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		pObject->SetDirty(bSet);
	}
}
//*******************************************************************************
BOOL CBCGPVisualContainer::IsDirty() const
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		if (pObject->IsDirty())
		{
			return TRUE;
		}
	}

	return FALSE;
}
//*******************************************************************************
void CBCGPVisualContainer::Redraw()
{
	ASSERT_VALID (this);

	if (m_pWndOwner->GetSafeHwnd() != NULL && !m_rect.IsRectEmpty())
	{
		m_pWndOwner->RedrawWindow((const CRect&)m_rect);
	}
}
//*******************************************************************************
void CBCGPVisualContainer::DrawObjects(CBCGPGraphicsManager* pGM, CBCGPRect rectClip, DWORD dwFlags, const CBCGPPoint& ptOffset)
{
	CBCGPRect rectInter;

	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		BOOL bCacheImage = pObject->m_bCacheImage;
		if (m_bCacheImage || !m_bDrawDynamicObjectsOnTop)
		{
			pObject->m_bCacheImage = FALSE;
		}

		CBCGPRect rectSaved = pObject->GetRect();

		if (ptOffset != CBCGPPoint(0, 0))
		{
			pObject->m_rect.OffsetRect(-ptOffset);
		}

		CBCGPRect rectObj = pObject->GetRect().NormalizedRect();

		if (!rectObj.IsRectEmpty() && pObject->IsVisible() &&
			rectInter.IntersectRect (rectObj, rectClip))
		{
			pObject->OnDraw(pGM, rectClip, dwFlags);
			pObject->SetDirty(FALSE);
		}

		pObject->m_bCacheImage = bCacheImage;

		if (ptOffset != CBCGPPoint(0, 0))
		{
			pObject->m_rect = rectSaved;
		}
	}
}
//*******************************************************************************
void CBCGPVisualContainer::DrawTrackingRects(CBCGPGraphicsManager* pGM)
{
	if (!m_bIsEditMode)
	{
		return;
	}

	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		pObject->DrawTrackingRect(pGM, m_brSelOutline, m_brSelFill);
	}
}
//*******************************************************************************
void CBCGPVisualContainer::OnDraw(CBCGPGraphicsManager* pGM, const CBCGPRect& rectClip)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pGM);

	if (m_bIsEditMode)
	{
		m_ImageCache.Destroy();
	}

	DWORD dwDrawFlags = m_bDrawDynamicObjectsOnTop ? BCGP_DRAW_STATIC : (BCGP_DRAW_STATIC | BCGP_DRAW_DYNAMIC);

	if (m_bCacheImage && !pGM->IsOffscreen() && !m_bIsEditMode)
	{	
		if (m_ImageCache.GetHandle() == NULL || IsDirty())
		{
			m_ImageCache.Destroy();

			CBCGPRect rectBounds = GetBoundsRect(TRUE);
			
			rectBounds.InflateRect(1., 1.);
			rectBounds.left = m_rect.left;
			rectBounds.top = m_rect.top;
			rectBounds.right = max(m_rect.right, rectBounds.right);
			rectBounds.bottom = max(m_rect.bottom, rectBounds.bottom);

			CBCGPGraphicsManager* pGMMem = pGM->CreateOffScreenManager(rectBounds, &m_ImageCache);
			if (pGMMem != NULL)
			{
				CBCGPRect rectSaved = m_rect;
				m_rect = m_rect - m_rect.TopLeft();

				OnFillBackground(pGMMem);
				DrawObjects(pGMMem, m_rect, dwDrawFlags, rectSaved.TopLeft());
				OnDrawBorder(pGMMem);

				delete pGMMem;

				m_rect = rectSaved;
			}
		}
	}

	if (m_ImageCache.GetHandle() != NULL)
	{
		pGM->DrawImage(m_ImageCache, m_rect.TopLeft());
	}
	else
	{
		CBCGPRect rectSel;
		if (m_nDragMode == HTNOWHERE && m_ptDragStart != CBCGPPoint(-1, -1) && m_pNewObject == NULL)
		{
			rectSel = CBCGPRect(m_ptDragStart, CBCGPSize(m_ptDragFinish.x - m_ptDragStart.x, m_ptDragFinish.y - m_ptDragStart.y));
		}

		BOOL bIsSemitransp = pGM->IsSupported(BCGP_GRAPHICS_MANAGER_COLOR_OPACITY);

		OnFillBackground(pGM);

		if (!bIsSemitransp && !rectSel.IsRectEmpty())
		{
			DrawSelectedArea(pGM, rectSel);
		}

		DrawObjects(pGM, rectClip, dwDrawFlags);
		DrawTrackingRects(pGM);

		if (bIsSemitransp && !rectSel.IsRectEmpty())
		{
			DrawSelectedArea(pGM, rectSel);
		}

		if (m_pNewObject != NULL)
		{
			ASSERT_VALID(m_pNewObject);
			m_pNewObject->OnDraw(pGM, rectClip);
		}

		OnDrawBorder(pGM);
	}

	if (m_bDrawDynamicObjectsOnTop)
	{
		DrawObjects(pGM, rectClip, BCGP_DRAW_DYNAMIC);
	}
}
//*******************************************************************************
void CBCGPVisualContainer::OnFillBackground(CBCGPGraphicsManager* pGM)
{
	if (!m_brFill.IsEmpty())
	{
		pGM->FillRectangle(m_rect, m_brFill);
	}

	if (m_bIsEditMode && (m_dwEditFlags & BCGP_CONTAINER_GRID) == BCGP_CONTAINER_GRID)
	{
		OnDrawGrid(pGM);
	}
}
//*******************************************************************************
void CBCGPVisualContainer::OnDrawBorder(CBCGPGraphicsManager* pGM)
{
	if (!m_brOutline.IsEmpty())
	{
		CBCGPRect rect = m_rect;
		rect.right--;
		rect.bottom--;

		pGM->DrawRectangle(rect, m_brOutline);
	}
}
//*******************************************************************************
void CBCGPVisualContainer::DrawSelectedArea(CBCGPGraphicsManager* pGM, const CBCGPRect& rectSel)
{
	pGM->FillRectangle(rectSel, m_brSelFill);
	pGM->DrawRectangle(rectSel, m_brSelOutline);
}
//*******************************************************************************
void CBCGPVisualContainer::OnDrawGrid(CBCGPGraphicsManager* pGM)
{
	if (m_brGrid.IsEmpty())
	{
		return;
	}

	for (double x = m_rect.left + m_sizeGrid.cx; x < m_rect.right; x += m_sizeGrid.cx)
	{
		pGM->DrawLine(x, m_rect.top, x, m_rect.bottom, m_brGrid);
	}

	for (double y = m_rect.top + m_sizeGrid.cy; y < m_rect.bottom; y += m_sizeGrid.cy)
	{
		pGM->DrawLine(m_rect.left, y, m_rect.right, y, m_brGrid);
	}
}
//*******************************************************************************
#if (!defined _BCGSUITE_) && (!defined _BCGPCHART_STANDALONE)

BOOL CBCGPVisualContainer::LoadFromXML (UINT uiXMLResID)
{
	ASSERT_VALID (this);
	return LoadFromXML (MAKEINTRESOURCE (uiXMLResID));
}
//*******************************************************************************
BOOL CBCGPVisualContainer::LoadFromXML (LPCTSTR lpszXMLResID)
{
	CBCGPVisualInfo info;
	CBCGPVisualInfoLoader loader (info);

	if (!loader.Load (lpszXMLResID))
	{
		TRACE0("Cannot load dashboard from buffer\n");
		return FALSE;
	}

	CBCGPVisualConstructor constr (info);
	constr.Construct (*this);

	m_ImageCache.Destroy();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::LoadFromBuffer (LPCTSTR lpszXMLBuffer)
{
	ASSERT_VALID (this);
	ASSERT (lpszXMLBuffer != NULL);

	CBCGPVisualInfo info;
	CBCGPVisualInfoLoader loader (info);

	if (!loader.LoadFromBuffer (lpszXMLBuffer))
	{
		TRACE0("Cannot load ribbon from buffer\n");
		return FALSE;
	}

	CBCGPVisualConstructor constr (info);
	constr.Construct (*this);

	m_ImageCache.Destroy();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::SaveToXML (const CString& strFileName)
{
	CBCGPVisualInfo info;

	CBCGPVisualCollector collector (info);
	collector.Collect (*this);

	CBCGPVisualInfoWriter writer (info);
	return writer.Save (strFileName);
}

#endif // _BCGSUITE_

#ifndef _BCGPCHART_STANDALONE

void CBCGPVisualContainer::EnableLayout(BOOL bEnable)
{
	if (m_pLayout != NULL)
	{
		delete m_pLayout;
		m_pLayout = NULL;
	}

	if (!bEnable)
	{
		return;
	}

	m_pLayout = new CBCGPVisualLayout(*this);
}

#endif

void CBCGPVisualContainer::AdjustLayout()
{
#ifndef _BCGPCHART_STANDALONE
	m_ImageCache.Destroy();

	if (m_pLayout == NULL)
	{
		return;
	}

	ASSERT_VALID(m_pLayout);
	m_pLayout->AdjustLayout();
#endif
}
//*******************************************************************************
void CBCGPVisualContainer::SetOutlineBrush(const CBCGPBrush& br)
{
	m_brOutline = br;
}
//*******************************************************************************
void CBCGPVisualContainer::SetFillBrush(const CBCGPBrush& br)
{
	m_brFill = br;
}
//*******************************************************************************
void CBCGPVisualContainer::SetGridBrush(const CBCGPBrush& br)
{
	m_brGrid = br;
}
//*******************************************************************************
void CBCGPVisualContainer::SetDrawDynamicObjectsOnTop(BOOL bSet)
{
	m_bDrawDynamicObjectsOnTop = bSet;
	SetDirty();
}
//*******************************************************************************
BOOL CBCGPVisualContainer::OnMouseDown(int nButton, const CBCGPPoint& pt)
{
	CBCGPBaseVisualObject* pObject = GetFromPoint(pt);
	if (pObject != NULL)
	{
		ASSERT_VALID(pObject);
		if (pObject->OnMouseDown(nButton, pt))
		{
			return TRUE;
		}
	}

	if (m_bIsEditMode)
	{
		if (nButton == 0)
		{
			m_nDragMode = HTNOWHERE;

			if (pObject != NULL && !m_bAddNewObjectMode)
			{
				const BOOL bCtrl = ::GetAsyncKeyState (VK_CONTROL) & 0x8000;

				Select(pObject, bCtrl);
				pObject->Redraw();

				m_nDragMode = pObject->HitTest(pt);
			}
			else
			{
				ClearSelection();

				if (IsSingleSel())
				{
					Redraw();
					return FALSE;
				}
			}

			m_ptDragStart = m_ptDragFinish = pt;
			
			if (m_bAddNewObjectMode)
			{
				m_pNewObject = OnStartAddNewObject(pt);

				if (m_pNewObject != NULL)
				{
					m_pNewObject->SetRect(CBCGPRect(pt, CBCGPSize(1., 1.)));
				}
			}

			Redraw();

			return TRUE;
		}
		else if (nButton == 1 && pObject == NULL)
		{
			ClearSelection();
		}
	}

	return FALSE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::OnGetToolTip(const CBCGPPoint& pt, CString& strToolTip, CString& strDescr)
{
	CBCGPBaseVisualObject* pObject = GetFromPoint(pt);
	if (pObject != NULL)
	{
		ASSERT_VALID(pObject);

		if (pObject->OnGetToolTip(pt, strToolTip, strDescr))
		{
			return TRUE;
		}
	}

	return FALSE;
}
//*******************************************************************************
void CBCGPVisualContainer::Select(CBCGPBaseVisualObject* pObject, BOOL bAddToSel)
{
	ASSERT_VALID(pObject);

	if (!pObject->IsSelected())
	{
		if (!bAddToSel || IsSingleSel())
		{
			ClearSelection();
		}

		pObject->SetSelected();
		m_lstSel.AddTail(pObject);
	}
}
//*******************************************************************************
void CBCGPVisualContainer::ClearSelection()
{
	if (!m_bIsEditMode)
	{
		return;
	}

	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		pObject->SetSelected(FALSE);
		pObject->Redraw();
	}

	m_lstSel.RemoveAll();
}
//*******************************************************************************
CBCGPBaseVisualObject* CBCGPVisualContainer::GetSel(int nIndex) const
{
	if (m_lstSel.IsEmpty())
	{
		return NULL;
	}

	POSITION pos = m_lstSel.FindIndex(nIndex);
	if (pos == NULL)
	{
		return NULL;
	}

	return m_lstSel.GetAt(pos);
}
//*******************************************************************************
void CBCGPVisualContainer::OnMouseUp(int nButton, const CBCGPPoint& pt)
{
	int i = 0;

	for (i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		if (pObject->IsCaptured())
		{
			pObject->OnMouseUp(nButton, pt);
			return;
		}
	}

	CBCGPBaseVisualObject* pObject = GetFromPoint(pt);
	if (pObject != NULL)
	{
		ASSERT_VALID(pObject);
		pObject->OnMouseUp(nButton, pt);
	}

	if (m_ptDragStart == CBCGPPoint(-1, -1))
	{
		return;	
	}

	if (m_nDragMode == HTNOWHERE)
	{
		CBCGPRect rectSel(m_ptDragStart, CBCGPSize(m_ptDragFinish.x - m_ptDragStart.x, m_ptDragFinish.y - m_ptDragStart.y));

		if (m_pNewObject != NULL)
		{
			if ((m_pNewObject->GetEditFlags() & BCGP_EDIT_NO_NORMALIZE_RECT) == 0)
			{
				rectSel.Normalize();
			}

			if (OnFinishAddNewObject(m_pNewObject, rectSel))
			{
				m_pNewObject->SetRect(rectSel);
				Add(m_pNewObject);

				Select(m_pNewObject);
			}
			else
			{
				delete m_pNewObject;
			}

			m_pNewObject = NULL;
			m_bAddNewObjectMode = FALSE;
		}
		else
		{
			rectSel.Normalize();

			for (i = 0; i < m_arObjects.GetSize(); i++)
			{
				CBCGPBaseVisualObject* pObject = m_arObjects[i];
				ASSERT_VALID(pObject);

				CBCGPRect rectInter;

				if (rectInter.IntersectRect(pObject->GetRect().NormalizedRect(), rectSel))
				{
					Select(pObject, TRUE);
				}
			}
		}
	}
	else
	{
		for (POSITION pos = m_lstSel.GetHeadPosition(); pos != NULL;)
		{
			CBCGPBaseVisualObject* pObject = m_lstSel.GetNext(pos);
			ASSERT_VALID(pObject);

			if (!pObject->m_rectTrack.IsRectEmpty())
			{
				CBCGPRect rect = pObject->m_rectTrack;

				if ((pObject->GetEditFlags() & BCGP_EDIT_NO_NORMALIZE_RECT) == 0)
				{
					rect.Normalize();
				}

				if (pObject->GetRect() != rect)
				{
					if (rect.bottom <= m_rect.top)
					{
						rect.OffsetRect(0, m_rect.top - rect.bottom + 2);
					}
					else if (rect.top >= m_rect.bottom)
					{
						rect.OffsetRect(0, -(rect.top - m_rect.bottom + 4));
					}

					if (rect.right <= m_rect.left)
					{
						rect.OffsetRect(m_rect.left - rect.right + 2, 0);
					}
					else if (rect.left >= m_rect.right)
					{
						rect.OffsetRect(-(rect.left - m_rect.right + 4), 0);
					}

					pObject->SetRect(rect);
				}
			}
		}
	}

	m_ptDragStart = m_ptDragFinish = CBCGPPoint(-1, -1);
	Redraw();
}
//*******************************************************************************
void CBCGPVisualContainer::OnMouseMove(const CBCGPPoint& pt)
{
	if (m_bAddNewObjectMode && m_ptDragStart == CBCGPPoint(-1, -1))
	{
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_CROSS));
		return;
	}

	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pArObject = m_arObjects[i];
		ASSERT_VALID(pArObject);

		if (pArObject->IsCaptured())
		{
			pArObject->OnMouseMove(pt);
			return;
		}

		if (pArObject->m_bIsTracked)
		{
			CBCGPRect rect = pArObject->m_rect;
			rect.Normalize();

			if (!rect.PtInRect(pt))
			{
				pArObject->m_bIsTracked = FALSE;
				pArObject->OnMouseLeave();
			}
		}
	}

	CBCGPBaseVisualObject* pObject = GetFromPoint(pt);
	if (pObject != NULL)
	{
		ASSERT_VALID(pObject);

		pObject->OnMouseMove(pt);
		pObject->m_bIsTracked = TRUE;
	}

	if (m_ptDragStart == CBCGPPoint(-1, -1))
	{
		return;	
	}

	if (m_nDragMode != HTNOWHERE)
	{
		CBCGPPoint ptOffset = pt - m_ptDragStart;
		m_ptDragStart = pt;

		for (POSITION pos = m_lstSel.GetHeadPosition(); pos != NULL;)
		{
			CBCGPBaseVisualObject* pObject = m_lstSel.GetNext(pos);
			ASSERT_VALID(pObject);

			CBCGPRect rect = pObject->m_rectTrack;

			BOOL bLockAspRatio = (pObject->GetEditFlags() & BCGP_EDIT_SIZE_LOCK_ASPECT_RATIO) == BCGP_EDIT_SIZE_LOCK_ASPECT_RATIO;

			if (bLockAspRatio && m_nDragMode != HTCAPTION)
			{
				if (m_nDragMode == HTTOPLEFT || m_nDragMode == HTBOTTOMRIGHT)
				{
					if (fabs(ptOffset.x) > fabs(ptOffset.y))
					{
						ptOffset.y = ptOffset.x;
					}
					else
					{
						ptOffset.x = ptOffset.y;
					}
				}
				else if (m_nDragMode == HTTOPRIGHT || m_nDragMode == HTBOTTOMLEFT)
				{
					if (fabs(ptOffset.x) < fabs(ptOffset.y))
					{
						ptOffset.y = -ptOffset.x;
					}
					else
					{
						ptOffset.x = -ptOffset.y;
					}
				}
			}

			switch (m_nDragMode)
			{
			case HTCAPTION:
				rect += ptOffset;
				break;

			case HTLEFT:
				rect.left += ptOffset.x;
				if (bLockAspRatio)
				{
					rect.bottom -= ptOffset.x;
				}
				break;

			case HTRIGHT:
				rect.right += ptOffset.x;
				if (bLockAspRatio)
				{
					rect.bottom += ptOffset.x;
				}
				break;

			case HTTOP:
				rect.top += ptOffset.y;
				if (bLockAspRatio)
				{
					rect.right -= ptOffset.y;
				}
				break;

			case HTBOTTOM:
				rect.bottom += ptOffset.y;
				if (bLockAspRatio)
				{
					rect.right += ptOffset.y;
				}
				break;

			case HTTOPLEFT:
				rect.left += ptOffset.x;
				rect.top += ptOffset.y;
				break;

			case HTBOTTOMLEFT:
				rect.left += ptOffset.x;
				rect.bottom += ptOffset.y;
				break;

			case HTTOPRIGHT:
				rect.right += ptOffset.x;
				rect.top += ptOffset.y;
				break;

			case HTBOTTOMRIGHT:
				rect.right += ptOffset.x;
				rect.bottom += ptOffset.y;
				break;
			}

			pObject->SetTrackingRect(rect);
		}
	}
	else if (m_pNewObject != NULL)
	{
		ASSERT_VALID(m_pNewObject);

		CBCGPRect rect(m_ptDragStart, CBCGPSize(m_ptDragFinish.x - m_ptDragStart.x, m_ptDragFinish.y - m_ptDragStart.y));
		if ((m_pNewObject->GetEditFlags() & BCGP_EDIT_NO_NORMALIZE_RECT) == 0)
		{
			rect.Normalize();
		}

		m_pNewObject->SetRect(rect);
	}

	m_ptDragFinish = pt;

	if (m_nDragMode != HTCAPTION)
	{
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_CROSS));
	}

	Redraw();
}
//*******************************************************************************
void CBCGPVisualContainer::OnMouseLeave()
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		if (pObject->m_bIsTracked)
		{
			pObject->m_bIsTracked = FALSE;
			pObject->OnMouseLeave();
		}
	}
}
//*******************************************************************************
void CBCGPVisualContainer::OnCancelMode()
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		pObject->OnCancelMode();
	}
}
//*******************************************************************************
BOOL CBCGPVisualContainer::OnSetMouseCursor(const CBCGPPoint& pt)
{
	if (m_bAddNewObjectMode)
	{
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_CROSS));
		return TRUE;
	}

	CBCGPBaseVisualObject* pObject = GetFromPoint(pt);
	if (pObject != NULL)
	{
		ASSERT_VALID(pObject);
		return pObject->OnSetMouseCursor(pt);
	}

	return FALSE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::OnMouseWheel(const CBCGPPoint& pt, short zDelta)
{
	CBCGPBaseVisualObject* pObject = GetFromPoint(pt);
	if (pObject != NULL)
	{
		ASSERT_VALID(pObject);
		return pObject->OnMouseWheel(pt, zDelta);
	}

	return FALSE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::OnKeyboardDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		if (pObject->OnKeyboardDown(nChar, nRepCnt, nFlags))
		{
			return TRUE;
		}
	}

	return FALSE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::OnKeyboardUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	for (int i = 0; i < m_arObjects.GetSize(); i++)
	{
		CBCGPBaseVisualObject* pObject = m_arObjects[i];
		ASSERT_VALID(pObject);

		if (pObject->OnKeyboardUp(nChar, nRepCnt, nFlags))
		{
			return TRUE;
		}
	}

	return FALSE;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::ExportToImage(CBCGPGraphicsManager* pGMSrc, CBCGPImage& image)
{
	ASSERT_VALID(pGMSrc);

	CBCGPGraphicsManager* pGM = pGMSrc->CreateOffScreenManager(m_rect, &image);

	if (pGM == NULL)
	{
		return FALSE;
	}

	CBCGPRect rectSaved = m_rect;
	m_rect = m_rect - rectSaved.TopLeft();

	pGM->FillRectangle(m_rect, CBCGPBrush(CBCGPColor::White));

	SetDirty();
	OnDraw(pGM, CBCGPRect());

	m_rect = rectSaved;
	delete pGM;

	return TRUE;
}
//*******************************************************************************
HBITMAP CBCGPVisualContainer::ExportToBitmap(CBCGPGraphicsManager* pGM)
{
	ASSERT_VALID(pGM);

	const CBCGPSize size = m_rect.Size();

	HBITMAP hmbpDib = CBCGPDrawManager::CreateBitmap_32(size, NULL);
	if (hmbpDib == NULL)
	{
		ASSERT (FALSE);
		return NULL;
	}

	CBCGPRect rect(CBCGPPoint(), size);

	CDC dcMem;
	dcMem.CreateCompatibleDC (NULL);

	HBITMAP hbmpOld = (HBITMAP) dcMem.SelectObject (hmbpDib);

	pGM->BindDC(&dcMem, rect);
	pGM->BeginDraw();

	CBCGPRect rectSaved = m_rect;

	SetRect(m_rect - rectSaved.TopLeft());
	SetDirty();

	BOOL bCacheImage = m_bCacheImage;
	m_bCacheImage = FALSE;

	if (m_brFill.IsEmpty())
	{
		pGM->FillRectangle(m_rect, CBCGPBrush(CBCGPColor::White));
	}

	OnDraw(pGM, m_rect);

	pGM->EndDraw();

	dcMem.SelectObject (hbmpOld);

	CBCGPDrawManager::FillAlpha (rect, hmbpDib, 255);

	pGM->BindDC(NULL);

	SetRect(rectSaved);
	m_bCacheImage = bCacheImage;

	SetDirty();

	return hmbpDib;
}
//*******************************************************************************
BOOL CBCGPVisualContainer::CopyToClipboard(CBCGPGraphicsManager* pGM)
{
	if (pGM == NULL)
	{
		return FALSE;
	}

	HBITMAP hbmp = ExportToBitmap(pGM);

	CBCGPToolBarImages img;
	img.AddImage(hbmp, TRUE);
	img.SetSingleImage();

	return img.CopyImageToClipboard(0);
}
//*******************************************************************************
BOOL CBCGPVisualContainer::ExportToFile(const CString& strFilePath, CBCGPGraphicsManager* pGM)
{
	if (pGM == NULL)
	{
		return FALSE;
	}

	return SaveBitmapToFile(ExportToBitmap(pGM), strFilePath);
}
