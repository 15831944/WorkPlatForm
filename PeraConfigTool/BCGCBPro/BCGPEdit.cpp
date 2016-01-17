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
// BCGPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "bcgcbpro.h"
#include "bcgglobals.h"

#ifndef _BCGSUITE_
#include "BCGPToolBarImages.h"
#include "BCGPShellManager.h"
#endif

#include "BCGPVisualManager.h"
#include "BCGProRes.h"
#include "BCGPLocalResource.h"
#include "BCGPCalculator.h"
#include "BCGPEdit.h"
#include "BCGPDlgImpl.h"
#include "BCGPDrawManager.h"
#include "BCGPGlobalUtils.h"

#ifndef _BCGSUITE_
	#define visualManagerMFC	CBCGPVisualManager::GetInstance ()
#else
	#define visualManagerMFC	CMFCVisualManager::GetInstance ()
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPEdit

IMPLEMENT_DYNAMIC(CBCGPEdit, CEdit)

CBCGPEdit::CBCGPEdit()
{
	m_rectBtn.SetRectEmpty ();
	m_bIsButtonPressed = FALSE;
	m_bIsButtonHighlighted = FALSE;
	m_bIsButtonCaptured = FALSE;
	m_Mode = BrowseMode_None;
	m_sizeImage = CSize (0, 0);
	m_pCalcPopup = NULL;
	m_nBrowseButtonWidth = 20;
	m_bDefaultImage = TRUE;
	m_bVisualManagerStyle = FALSE;
	m_bOnGlass = FALSE;
	m_bSearchMode = FALSE;
	m_bTextIsEmpty = TRUE;
}

CBCGPEdit::~CBCGPEdit()
{
}

BEGIN_MESSAGE_MAP(CBCGPEdit, CEdit)
	//{{AFX_MSG_MAP(CBCGPEdit)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCMOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChange)
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPEdit message handlers

void CBCGPEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bIsButtonCaptured)
	{
		ReleaseCapture ();

		m_bIsButtonPressed = FALSE;
		m_bIsButtonCaptured = FALSE;
		m_bIsButtonHighlighted = FALSE;

		RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);

		if (m_rectBtn.PtInRect (point))
		{
			OnBrowse ();
		}

		return;
	}
	
	CEdit::OnLButtonUp(nFlags, point);
}
//*************************************************************************************
void CBCGPEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bIsButtonCaptured)
	{
		BOOL bIsButtonPressed = m_rectBtn.PtInRect (point);
		if (bIsButtonPressed != m_bIsButtonPressed)
		{
			m_bIsButtonPressed = bIsButtonPressed;
			RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		}

		return;
	}
	
	if (m_bIsButtonHighlighted)
	{
		if (!m_rectBtn.PtInRect (point))
		{
			m_bIsButtonHighlighted = FALSE;
			ReleaseCapture ();

			RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		}
	}
	
	CEdit::OnMouseMove(nFlags, point);
}
//*************************************************************************************
void CBCGPEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);

	if (m_Mode != BrowseMode_None)
	{
		lpncsp->rgrc [0].right -= m_nBrowseButtonWidth;
	}
}
//*************************************************************************************
void CBCGPEdit::OnNcPaint() 
{
	const BOOL bHasScrollBars = ((GetStyle () & WS_VSCROLL) == WS_VSCROLL) || ((GetStyle () & WS_HSCROLL) == WS_HSCROLL);

	if (bHasScrollBars || (!m_bVisualManagerStyle && !m_bOnGlass))
	{
		CEdit::OnNcPaint ();
	}
	else
	{
		CBCGPDrawOnGlass dog (m_bOnGlass);
		visualManagerMFC->OnDrawControlBorder (this);
	}

	if (GetStyle () & WS_BORDER)
	{
		visualManagerMFC->OnDrawControlBorder (this);
	}

	if (m_Mode == BrowseMode_None)
	{
		return;
	}

	CWindowDC dc (this);

	CRect rectWindow;
	GetWindowRect (rectWindow);

	m_rectBtn = rectWindow;
	m_rectBtn.left = m_rectBtn.right -  m_nBrowseButtonWidth;

	CRect rectClient;
	GetClientRect (rectClient);
	ClientToScreen (&rectClient);

	m_rectBtn.OffsetRect (rectClient.right + m_nBrowseButtonWidth - rectWindow.right, 0);
	m_rectBtn.top += rectClient.top - rectWindow.top;
	m_rectBtn.bottom -= rectWindow.bottom - rectClient.bottom;

	CRect rect = m_rectBtn;
	rect.OffsetRect (-rectWindow.left, -rectWindow.top);

	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect (&rect);

	dc.SelectClipRgn (&rgnClip);

	OnDrawBrowseButton (&dc, rect, m_bIsButtonPressed, m_bIsButtonHighlighted);

	dc.SelectClipRgn (NULL);

	ScreenToClient (&m_rectBtn);
}
//********************************************************************************
BCGNcHitTestType CBCGPEdit::OnNcHitTest(CPoint point) 
{
	CPoint ptClient = point;
	ScreenToClient (&ptClient);

	if (m_Mode != BrowseMode_None && m_rectBtn.PtInRect (ptClient))
	{
		return HTBORDER;
	}
	
	return CEdit::OnNcHitTest(point);
}
//********************************************************************************
void CBCGPEdit::OnDrawBrowseButton (CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bHighlight)
{
	ASSERT (m_Mode != BrowseMode_None);
	ASSERT_VALID (pDC);

	if (m_bSearchMode && m_ImageSearch.IsValid() && m_bTextIsEmpty)
	{
		if (m_bOnGlass)
		{
			CBCGPDrawManager dm(*pDC);
			dm.DrawRect(rect, IsWindowEnabled() ? globalData.clrWindow : globalData.clrBtnFace, (COLORREF)-1);
		}
		else
		{
			pDC->FillRect(rect, IsWindowEnabled() ? &globalData.brWindow : &globalData.brBtnFace);
		}

		if (IsWindowEnabled())
		{
			m_ImageSearch.DrawEx(pDC, rect, 0, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		}
		else
		{
			CBCGPDrawState ds;

			m_ImageSearch.PrepareDrawImage (ds);

			CSize sizeImage = m_ImageSearch.GetImageSize();

			m_ImageSearch.Draw (pDC, 
				rect.left + max(0, (rect.Width() - sizeImage.cx) / 2), 
				rect.top + max(0, (rect.Height() - sizeImage.cy) / 2), 
				0, FALSE, TRUE);
			m_ImageSearch.EndDrawImage (ds);
		}

		return;
	}

	CBCGPVisualManager::BCGBUTTON_STATE state = CBCGPVisualManager::ButtonsIsRegular;

	if (bIsButtonPressed)
	{
		state = CBCGPVisualManager::ButtonsIsPressed;
	}
	else if (bHighlight)
	{
		state = CBCGPVisualManager::ButtonsIsHighlighted;
	}

	COLORREF clrText = globalData.clrBtnText;

	if (!CBCGPVisualManager::GetInstance ()->OnDrawBrowseButton (pDC, rect, this, state, clrText))
	{
		return;
	}

	if (m_bSearchMode && m_ImageClear.IsValid() && !m_bTextIsEmpty)
	{
		CRect rectImage = rect;

		if (bIsButtonPressed && visualManagerMFC->IsOffsetPressedButton ())
		{
			rectImage.left++;
			rectImage.top++;
		}

		m_ImageClear.DrawEx(pDC, rectImage, 0, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		return;
		
	}

	int iImage = 0;

	if (m_ImageBrowse.IsValid())
	{
		if (m_bDefaultImage)
		{
			switch (m_Mode)
			{
			case BrowseMode_Folder:
				iImage = 0;
				break;

			case BrowseMode_File:
				iImage = 1;
				break;

			case BrowseMode_Calculator:
				iImage = 2;
				break;
			}
		}

		CRect rectImage = rect;

		if (bIsButtonPressed && visualManagerMFC->IsOffsetPressedButton ())
		{
			rectImage.left++;
			rectImage.top++;
		}

#ifndef _BCGSUITE_
		if (m_bOnGlass)
		{
			m_ImageBrowse.ConvertTo32Bits();
		}
#endif
		if (IsWindowEnabled())
		{
			m_ImageBrowse.DrawEx (pDC, rectImage, iImage, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		}
		else
		{
			CBCGPDrawState ds;

			m_ImageBrowse.PrepareDrawImage (ds);

			CSize sizeImage = m_ImageBrowse.GetImageSize();

			m_ImageBrowse.Draw (pDC, 
				rectImage.left + max(0, (rectImage.Width() - sizeImage.cx) / 2), 
				rectImage.top + max(0, (rectImage.Height() - sizeImage.cy) / 2), 
				iImage, FALSE, TRUE);
			m_ImageBrowse.EndDrawImage (ds);
		}
	}
	else if (!m_strLabel.IsEmpty ())
	{
		CFont* pFont = (CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT);

		CRect rectText = rect;
		rectText.DeflateRect (1, 2);
		rectText.OffsetRect (0, -2);

		if (bIsButtonPressed)
		{
			rectText.OffsetRect (1, 1);
		}

		DWORD dwDTFlags = DT_SINGLELINE | DT_CENTER | DT_VCENTER;

		if (m_bOnGlass)
		{
			CBCGPVisualManager::GetInstance ()->DrawTextOnGlass(pDC, m_strLabel, rectText, dwDTFlags, 0, clrText);
		}
		else
		{
			COLORREF clrTextOld = pDC->SetTextColor (clrText);
			int nTextMode = pDC->SetBkMode (TRANSPARENT);

			pDC->DrawText (m_strLabel, rectText, dwDTFlags);

			pDC->SetTextColor (clrTextOld);
			pDC->SetBkMode (nTextMode);
		}

		pDC->SelectObject (pFont);
	}
}
//********************************************************************************
void CBCGPEdit::EnableBrowseButton (BOOL bEnable/* = TRUE*/,
									LPCTSTR szLabel/* = _T("...")*/)
{
	ASSERT_VALID (this);
	ASSERT (GetSafeHwnd () != NULL);
	ASSERT (szLabel != NULL);

	m_Mode = bEnable ? BrowseMode_Default : BrowseMode_None;
	m_strLabel = szLabel;

	m_ImageBrowse.Clear ();
	m_sizeImage = CSize (0, 0);

	OnChangeLayout ();
}
//********************************************************************************
void CBCGPEdit::OnChangeLayout ()
{
	ASSERT_VALID (this);
	ASSERT (GetSafeHwnd () != NULL);

	m_nBrowseButtonWidth = max (20, m_sizeImage.cx + 8);

	SetWindowPos (NULL, 0, 0, 0, 0, 
		SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);

	if (m_Mode != BrowseMode_None)
	{
		GetWindowRect (m_rectBtn);
		m_rectBtn.left = m_rectBtn.right -  m_nBrowseButtonWidth;

		ScreenToClient (&m_rectBtn);
	}
	else
	{
		m_rectBtn.SetRectEmpty ();
	}
}
//********************************************************************************
void CBCGPEdit::OnNcLButtonDblClk(UINT /*nHitTest*/, CPoint /*point*/)
{
}
//********************************************************************************
void CBCGPEdit::OnBrowse ()
{
	ASSERT_VALID (this);
	ASSERT (GetSafeHwnd () != NULL);

	if (m_bSearchMode && !m_bTextIsEmpty)
	{
		SetWindowText(_T(""));
		m_bIsButtonHighlighted = FALSE;
		RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ERASE);
		return;
	}

	switch (m_Mode)
	{
	case BrowseMode_Folder:
#ifndef BCGP_EXCLUDE_SHELL
		{
#ifndef _BCGSUITE_
			CBCGPShellManager* pShellManager = g_pShellManager;
#else
			CShellManager* pShellManager = NULL;
			CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp ());
			if (pApp != NULL)
			{
				pShellManager = pApp->GetShellManager ();
			}
#endif
			if (pShellManager != NULL)
			{
				CString strFolder;
				GetWindowText (strFolder);

				CString strResult;
				if (pShellManager->BrowseForFolder (strResult, this, strFolder) &&
					(strResult != strFolder))
				{
					SetWindowText (strResult);
					SetModify (TRUE);
					OnAfterUpdate ();
				}
			}
			else
			{
				ASSERT (FALSE);
			}
		}
#endif
		break;

	case BrowseMode_File:
		{
			CString strEditorText;
			GetWindowText (strEditorText);

			if (strEditorText.FindOneOf (_T("*?<>|")) >= 0)
			{
				if (!OnIllegalFileName (strEditorText))
				{
					SetFocus ();
					return;
				}
			}

			CString strFileName;
			CString strInitialDir;

			if (!strEditorText.IsEmpty ())
			{
				DWORD dwAttrs = ::GetFileAttributes (strEditorText); // Check if strEditorText is a directory
				if ((dwAttrs != DWORD(-1)) && (0 != (dwAttrs & FILE_ATTRIBUTE_DIRECTORY)))
				{
					strInitialDir = strEditorText;
				}
				else
				{
					int iBackSlashPos = strEditorText.ReverseFind (_T('\\'));
					if (iBackSlashPos > 0)
					{
						strInitialDir = strEditorText.Left (iBackSlashPos);
						strFileName = strEditorText.Mid (iBackSlashPos + 1);
					}
					else // no backslash found
					{
						// use current directory
						strFileName = strEditorText;
					}
				}
			}

			CFileDialog dlg (TRUE, m_strDefFileExt, strFileName, 0, m_strFileFilter, GetParent ());
			
			// Setup initial directory if possible
			if (!strInitialDir.IsEmpty())
			{
				dlg.m_ofn.lpstrInitialDir = strInitialDir;
			}

			if (dlg.DoModal () == IDOK &&
				strEditorText != dlg.GetPathName ())
			{
				SetWindowText (dlg.GetPathName ());
				SetModify (TRUE);
				OnAfterUpdate ();
			}

			if (GetParent () != NULL)
			{
				GetParent ()->RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
			}
		}
		break;

	case BrowseMode_Calculator:
		{
			if (m_pCalcPopup != NULL)
			{
				m_pCalcPopup->SendMessage (WM_CLOSE);
				m_pCalcPopup = NULL;
				return;
			}

			CString strValue;
			GetWindowText (strValue);

			double dblValue = 0.;
			if (!strValue.IsEmpty ())
			{
				strValue.Replace (_T(','), _T('.'));
#if _MSC_VER < 1400
				_stscanf (strValue, _T("%lf"), &dblValue);
#else
				_stscanf_s (strValue, _T("%lf"), &dblValue);
#endif
			}

			m_pCalcPopup = new CBCGPCalculatorPopup (dblValue, 0, this);

			if (!m_lstCalcAdditionalCommands.IsEmpty ())
			{
				CBCGPCalculator* pCalc = DYNAMIC_DOWNCAST (CBCGPCalculator, 
					m_pCalcPopup->GetMenuBar());
				ASSERT_VALID (pCalc);

				pCalc->SetAdditionalCommands (m_lstCalcAdditionalCommands);
			}

			if (!m_lstCalcExtCommands.IsEmpty ())
			{
				CBCGPCalculator* pCalc = DYNAMIC_DOWNCAST (CBCGPCalculator, 
					m_pCalcPopup->GetMenuBar());
				ASSERT_VALID (pCalc);

				pCalc->SetExtendedCommands (m_lstCalcExtCommands);
			}

			CRect rectWindow;
			GetWindowRect (rectWindow);

			if (!m_pCalcPopup->Create (this, rectWindow.left, rectWindow.bottom, NULL, TRUE))
			{
				ASSERT (FALSE);
				m_pCalcPopup = NULL;
			}
			else
			{
				m_pCalcPopup->GetMenuBar()->SetFocus ();
				
				CRect rect;
				m_pCalcPopup->GetWindowRect (&rect);
				m_pCalcPopup->UpdateShadow (&rect);

				return;
			}
		}
		break;
	}

	SetFocus ();
}
//********************************************************************************
BOOL CBCGPEdit::OnIllegalFileName (CString& strFileName)
{
	CString strError;
	strError.LoadString (AFX_IDP_INVALID_FILENAME);

	BSTR bsError = strError.AllocSysString (); // Convert to unicode string
	BSTR bsFileName = strFileName.AllocSysString ();

	struct EDITBALLOONTIP_
	{
	    DWORD cbStruct;
		LPCWSTR pszTitle;
	    LPCWSTR pszText;
		INT ttiIcon;
	} tooltip;

	tooltip.cbStruct = sizeof (tooltip);
	tooltip.pszTitle = bsError;
	tooltip.pszText = bsFileName;
	tooltip.ttiIcon = 3; // TTI_ERROR constant

	if (!SendMessage(0x1503, 0, (LPARAM)&tooltip)) // Sending EM_SHOWBALLOONTIP message to edit control
	{
		CString strMessage;
		strMessage.Format (_T("%s\r\n%s"), strFileName, strError);
		MessageBox (strMessage, NULL, MB_OK | MB_ICONEXCLAMATION);
	}

	::SysFreeString (bsFileName);
	::SysFreeString (bsError);
	return FALSE;
}
//********************************************************************************
void CBCGPEdit::SetBrowseButtonImage (HICON hIcon, BOOL bAutoDestroy, BOOL bAlphaBlend)
{
	if (m_ImageBrowse.IsValid())
	{
		m_ImageBrowse.Clear ();
	}

	if (hIcon == NULL)
	{
		m_sizeImage = CSize (0, 0);
		return;
	}

	ICONINFO info;
	::GetIconInfo (hIcon, &info);

	BITMAP bmp;
	::GetObject (info.hbmColor, sizeof (BITMAP), (LPVOID) &bmp);

	m_sizeImage.cx = bmp.bmWidth;
	m_sizeImage.cy = bmp.bmHeight;

	::DeleteObject (info.hbmColor);
	::DeleteObject (info.hbmMask);

	m_ImageBrowse.SetImageSize(m_sizeImage);
	m_ImageBrowse.AddIcon(hIcon, bAlphaBlend);

	m_bDefaultImage = FALSE;

	if (bAutoDestroy)
	{
		::DestroyIcon (hIcon);
	}
}
//********************************************************************************
void CBCGPEdit::SetBrowseButtonImage (HBITMAP hBitmap, BOOL bAutoDestroy)
{
	if (m_ImageBrowse.IsValid())
	{
		m_ImageBrowse.Clear ();
	}

	if (hBitmap == NULL)
	{
		m_sizeImage = CSize (0, 0);
		return;
	}

	BITMAP bmp;
	::GetObject (hBitmap, sizeof (BITMAP), (LPVOID) &bmp);

	m_sizeImage.cx = bmp.bmWidth;
	m_sizeImage.cy = bmp.bmHeight;

	m_ImageBrowse.SetImageSize(m_sizeImage);
	m_ImageBrowse.AddImage(hBitmap, TRUE);

	m_bDefaultImage = FALSE;

	if (bAutoDestroy)
	{
		::DeleteObject (hBitmap);
	}
}
//********************************************************************************
void CBCGPEdit::SetBrowseButtonImage (UINT uiBmpResId)
{
	if (m_ImageBrowse.IsValid())
	{
		m_ImageBrowse.Clear ();
	}

	if (uiBmpResId == 0)
	{
		m_sizeImage = CSize (0, 0);
		return;
	}

	if (!m_ImageBrowse.Load (uiBmpResId))
	{
		ASSERT (FALSE);
		return;
	}

	m_bDefaultImage = FALSE;
}
//*********************************************************************************
void CBCGPEdit::EnableFileBrowseButton (LPCTSTR lpszDefExt/* = NULL*/, LPCTSTR lpszFilter/* = NULL*/)
{
	ASSERT_VALID (this);
	ASSERT (GetSafeHwnd () != NULL);

	m_strDefFileExt = lpszDefExt == NULL ? _T("") : lpszDefExt;
	m_strFileFilter = lpszFilter == NULL ? _T("") : lpszFilter;

	m_Mode = BrowseMode_File;
	SetIntenalImage ();

	globalUtils.EnableEditCtrlAutoComplete (GetSafeHwnd (), FALSE);

	OnChangeLayout ();
}
//*********************************************************************************
void CBCGPEdit::EnableFolderBrowseButton ()
{
#ifdef BCGP_EXCLUDE_SHELL
	ASSERT (FALSE);
#else
	ASSERT_VALID (this);
	ASSERT (GetSafeHwnd () != NULL);
#ifndef _BCGSUITE_
	ASSERT (g_pShellManager != NULL);	// You need to call CBCGPWorkspace::InitShellManager () first!
#endif

	m_Mode = BrowseMode_Folder;
	SetIntenalImage ();

	globalUtils.EnableEditCtrlAutoComplete (GetSafeHwnd (), TRUE);

	OnChangeLayout ();
#endif
}
//*********************************************************************************
void CBCGPEdit::EnableCalculatorButton (const CStringList* plstAdditionalCommands,
										const CList<UINT, UINT>* plstExtCommands)
{
	m_Mode = BrowseMode_Calculator;
	m_lstCalcAdditionalCommands.RemoveAll ();
	m_lstCalcExtCommands.RemoveAll();

	if (plstAdditionalCommands != NULL)
	{
		m_lstCalcAdditionalCommands.AddTail ((CStringList*) plstAdditionalCommands);
	}

	if (plstExtCommands != NULL)
	{
		m_lstCalcExtCommands.AddTail ((CList<UINT, UINT>*) plstExtCommands);
	}

#ifdef _BCGSUITE_
	m_strLabel = _T("c");
#else
	SetIntenalImage ();
#endif
	OnChangeLayout ();
}
//********************************************************************************
void CBCGPEdit::SetIntenalImage ()
{
	if (m_ImageBrowse.IsValid())
	{
		m_ImageBrowse.Clear ();
	}

	CBCGPLocalResource	lr;

	UINT uiImageListResID = globalData.Is32BitIcons () ? IDB_BCGBARRES_BROWSE32 : IDB_BCGBARRES_BROWSE;

	m_sizeImage = CSize (16, 15);
	m_ImageBrowse.SetImageSize(m_sizeImage);
	m_ImageBrowse.SetTransparentColor(RGB (255, 0, 255));
	m_ImageBrowse.Load(uiImageListResID);

	m_bDefaultImage = TRUE;
}
//********************************************************************************
void CBCGPEdit::OnAfterUpdate ()
{
	if (GetOwner () == NULL)
	{
		return;
	}

	GetOwner ()->PostMessage (EN_CHANGE, GetDlgCtrlID (), (LPARAM) GetSafeHwnd ());
	GetOwner ()->PostMessage (EN_UPDATE, GetDlgCtrlID (), (LPARAM) GetSafeHwnd ());
}
//**********************************************************************************
void CBCGPEdit::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	if (!m_bIsButtonCaptured)
	{
		CPoint ptClient = point;
		ScreenToClient (&ptClient);

		if (m_rectBtn.PtInRect (ptClient))
		{
			SetCapture ();
			m_bIsButtonHighlighted = TRUE;

			RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		}
	}
	
	CEdit::OnNcMouseMove(nHitTest, point);
}
//*********************************************************************************
void CBCGPEdit::OnCancelMode() 
{
	CEdit::OnCancelMode();
	
	if (IsWindowEnabled ())
	{
		ReleaseCapture ();
	}

	m_bIsButtonPressed = FALSE;
	m_bIsButtonCaptured = FALSE;
	m_bIsButtonHighlighted = FALSE;

	RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
}
//********************************************************************************
void CBCGPEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_Mode != BrowseMode_None && m_rectBtn.PtInRect (point))
	{
		SetFocus ();

		m_bIsButtonPressed = TRUE;
		m_bIsButtonCaptured = TRUE;

		SetCapture ();

		RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		return;
	}
	
	CEdit::OnLButtonDown(nFlags, point);
}
//********************************************************************************
BOOL CBCGPEdit::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_SYSKEYDOWN:
		if (m_Mode != BrowseMode_None && 
			(pMsg->wParam == VK_DOWN || pMsg->wParam == VK_RIGHT))
		{
			OnBrowse ();
			return TRUE;
		}
		break;

	case WM_CHAR:
		if (m_Mode == BrowseMode_Calculator && !FilterCalcKey ((int) pMsg->wParam))
		{
			return TRUE;
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}
//*********************************************************************************
BOOL CBCGPEdit::FilterCalcKey (int nChar)
{
	CString str;
	GetWindowText (str);

	if (isdigit (nChar))
	{
		return TRUE;
	}

	switch (nChar)
	{
	case _T('-'):
		{
			int nStartPos, nEndPos;
			GetSel (nStartPos, nEndPos);

			return str.Find (_T('-')) == -1 && nStartPos == 0;
		}

	case _T('.'):
	case _T(','):
		return str.FindOneOf (_T(".,")) == -1;
	}

	return !isprint (nChar);
}
//***********************************************************************************
void CBCGPEdit::OnCalculatorUserCommand (CBCGPCalculator* /*pCalculator*/, 
										 UINT /*uiCmd*/)
{
	ASSERT (FALSE);	// Must be implemented in derived class
}
//************************************************************************************
LRESULT CBCGPEdit::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	return 0;
}
//*************************************************************************************
LRESULT CBCGPEdit::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
//*************************************************************************************
void CBCGPEdit::OnPaint() 
{
	BOOL bDrawPrompt = m_bSearchMode && m_bTextIsEmpty && !m_strSearchPrompt.IsEmpty();

	if (!m_bOnGlass && !bDrawPrompt)
	{
		Default ();
		return;
	}

	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC (dc, this, 255 /* Opaque */);
	CDC* pDC = &memDC.GetDC ();
	
	SendMessage (WM_PRINTCLIENT, (WPARAM) pDC->GetSafeHdc (), (LPARAM) PRF_CLIENT);

	if (bDrawPrompt)
	{
		pDC->SetTextColor (globalData.clrGrayedText);
		pDC->SetBkMode(TRANSPARENT);

		CFont* pOldFont = pDC->SelectObject (GetFont());

		CRect rectText;
		GetClientRect(rectText);

		DWORD lRes = GetMargins ();
		rectText.left += LOWORD(lRes);

		if ((GetStyle () & WS_BORDER) != 0 || (GetExStyle () & WS_EX_CLIENTEDGE) != 0)
		{
			rectText.DeflateRect (1, 1);
		}

		pDC->DrawText(m_strSearchPrompt, rectText, DT_LEFT | DT_SINGLELINE);

		pDC->SelectObject (pOldFont);
	}
}
//**********************************************************************************************************
BOOL CBCGPEdit::OnChange() 
{
	if (m_bOnGlass)
	{
		InvalidateRect (NULL, FALSE);
		UpdateWindow ();
	}

	if (m_bSearchMode)
	{
		BOOL bTextIsEmpty = m_bTextIsEmpty;

		CString str;
		GetWindowText (str);

		m_bTextIsEmpty = str.IsEmpty();

		if (bTextIsEmpty != m_bTextIsEmpty)
		{
			RedrawWindow (NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
		}
	}

	return FALSE;
}
//**********************************************************************************************************
LRESULT CBCGPEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CPoint ptCaret (-1, -1);

	BOOL bCheckSel = FALSE;

	if (m_bOnGlass)
	{
		if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) ||
			(message >= WM_KEYFIRST && message <= WM_KEYLAST))
		{
			ptCaret = GetCaretPos ();
			bCheckSel = TRUE;
		}
	}

	LRESULT lres = CEdit::WindowProc(message, wParam, lParam);

	if (bCheckSel)
	{
		if (GetSel () != 0 || ptCaret != GetCaretPos ())
		{
			InvalidateRect (NULL, FALSE);
			UpdateWindow ();
		}
	}

	if (message == EM_SETSEL && m_bOnGlass)
	{
		InvalidateRect (NULL, FALSE);
		UpdateWindow ();
	}

	return lres;
}
//**********************************************************************************************************
HBRUSH CBCGPEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	if ((GetStyle() & ES_READONLY) == ES_READONLY && m_bVisualManagerStyle)
	{
		CBrush& br = CBCGPVisualManager::GetInstance ()->GetDlgBackBrush (GetParent());

		LOGBRUSH lbr;
		br.GetLogBrush(&lbr);
		
		pDC->SetBkColor(lbr.lbColor);
		pDC->SetTextColor (globalData.clrBarText);
		return (HBRUSH)br.GetSafeHandle();
	}

	return NULL;
}
//**************************************************************************
void CBCGPEdit::EnableSearchMode(BOOL bEnable, LPCTSTR lpszPrompt)
{
	ASSERT_VALID (this);

	m_bSearchMode = bEnable;

	if (m_bSearchMode)
	{
		ASSERT(lpszPrompt != NULL);
		m_strSearchPrompt = lpszPrompt;

		if (!m_ImageSearch.IsValid())
		{
			CBCGPLocalResource locaRes;
			m_ImageSearch.Load(globalData.Is32BitIcons () ?
				IDB_BCGBARRES_SEARCH32 : IDB_BCGBARRES_SEARCH);
			m_ImageSearch.SetSingleImage();
			m_ImageSearch.SetTransparentColor(globalData.clrBtnFace);
		}

		if (!m_ImageClear.IsValid())
		{
			CBCGPLocalResource locaRes;
			m_ImageClear.Load(globalData.Is32BitIcons () ?
				IDB_BCGBARRES_CLEAR32 : IDB_BCGBARRES_CLEAR);
			m_ImageClear.SetSingleImage();
			m_ImageClear.SetTransparentColor(globalData.clrBtnFace);
		}
	}
	else
	{
		if (m_ImageClear.IsValid())
		{
			m_ImageClear.Clear();
		}

		if (m_ImageSearch.IsValid())
		{
			m_ImageSearch.Clear();
		}
	}

	EnableBrowseButton(bEnable);
}
