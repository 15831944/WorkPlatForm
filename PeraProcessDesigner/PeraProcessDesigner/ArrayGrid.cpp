#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "ArrayGrid.h"
#include "DlgParamEditArray.h"
#include <ConvertData.h>
#include <CStringChecker.h>

IMPLEMENT_DYNAMIC(CArrayGrid, CBCGPGridCtrl)

CArrayGrid::CArrayGrid(void)
{
}


CArrayGrid::~CArrayGrid(void)
{
}
BEGIN_MESSAGE_MAP(CArrayGrid, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBCGPGridCtrl)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//******************************************************************************************
void CArrayGrid::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnChar(nChar, nRepCnt, nFlags);

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting || nChar == 3)
	{
		return;
	}

	if (!CanBeginInplaceEditOnChar (nChar, nRepCnt, nFlags))
	{
		return;
	}

	CBCGPGridRow* pSel = GetCurSel ();

	if (pSel == NULL || !pSel->IsEnabled())
	{
		return;
	}

	ASSERT_VALID (pSel);
	EnsureVisible (pSel, TRUE);

	SetBeginEditReason (BeginEdit_Char);
	if (!EditItem (pSel))
	{
		return;
	}

	DoInplaceEditSetSel (OnInplaceEditSetSel (GetCurSelItem (pSel), BeginEdit_Char));

	pSel->PushChar (nChar);
}

//******************************************************************************************
BOOL CArrayGrid::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
#ifndef _BCGPGRID_STANDALONE
	if (CBCGPPopupMenu::GetActiveMenu () != NULL)
	{
		return TRUE;
	}
#endif

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return FALSE;
	}

	if (m_nVertScrollTotal <= m_nVertScrollPage)
	{
		return FALSE;
	}

	if ((m_fScrollRemainder > 0) != (zDelta > 0))
	{
		m_fScrollRemainder = 0;
	}

	UINT nLinesToScrollUserSetting;
	if (!::SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nLinesToScrollUserSetting, 0))
	{
		nLinesToScrollUserSetting = 1;
	}

	if(nLinesToScrollUserSetting == WHEEL_PAGESCROLL) // scroll one page at a time
	{
		OnVScroll (zDelta < 0 ? SB_PAGEDOWN : SB_PAGEUP, 0, NULL);
		return TRUE;
	}

	float fTotalLinesToScroll = ((float)zDelta / WHEEL_DELTA) * nLinesToScrollUserSetting + m_fScrollRemainder;

	int nSteps = abs((int)fTotalLinesToScroll);
	m_fScrollRemainder = fTotalLinesToScroll - (int)fTotalLinesToScroll;

	for (int i = 0; i < nSteps; i++)
	{
		OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
	}

	return TRUE;
}

//******************************************************************************************
void CArrayGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return;
	}

	CBCGPGridRow* pSel = GetCurSel ();

// 	if (pSel && pSel->IsInPlaceEditing())
// 	{
// 		return;
// 	}

	if (pSel != NULL && 
		pScrollBar->GetSafeHwnd () != NULL)
	{
		CSpinButtonCtrl* pWndSpin = pSel->GetSpinWnd ();
		if (pWndSpin != NULL && 
			pWndSpin->GetSafeHwnd () == pScrollBar->GetSafeHwnd ())
		{
			return;
		}
	}

	SetEndEditReason (EndEdit_Cancel | EndEdit_Layout);
	EndEditItem (TRUE);

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_TRACKPOS;

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
	{
		GetScrollInfo(SB_VERT, &info);
		nPos = info.nTrackPos;
	}

	int nPrevOffset = m_nVertScrollOffset;

	int nVertScrollPage = m_nVertScrollPage;
	if (m_bVirtualMode)
	{
		int nItemsCount = max (1, m_nVertScrollPage / m_nRowHeight);
		nVertScrollPage = nItemsCount * m_nRowHeight;
	}

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nVertScrollOffset -= m_nRowHeight;
		break;

	case SB_LINEDOWN:
		m_nVertScrollOffset += m_nRowHeight;
		break;

	case SB_TOP:
		m_nVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nVertScrollOffset = m_nVertScrollTotal;
		break;

	case SB_PAGEUP:
		m_nVertScrollOffset -= nVertScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nVertScrollOffset += nVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	m_nVertScrollOffset = max (0, min (m_nVertScrollOffset, 
		m_nVertScrollTotal - nVertScrollPage + 1));
	if (m_bVirtualMode)
	{
		// row alignment
		int nResidue = m_nVertScrollOffset % m_nRowHeight;
		if (nResidue > 0)
		{
			m_nVertScrollOffset -= nResidue;
			if (m_nVertScrollOffset + m_nVertScrollPage < m_nVertScrollTotal)
			{
				m_nVertScrollOffset += m_nRowHeight;
			}
		}
	}

	if (m_nVertScrollOffset == nPrevOffset)
	{
		return;
	}

	OnUpdateVScrollPos (m_nVertScrollOffset, nPrevOffset);

	SetScrollPos (SB_VERT, m_nVertScrollOffset);

	int dy = nPrevOffset - m_nVertScrollOffset;

	ShiftItems (0, dy);
	OnPosSizeChanged ();
	m_rectTrackSel = OnGetSelectionRect ();
	ScrollWindow (0, dy, m_rectList, m_rectList);
	ScrollWindow (0, dy, m_rectRowHeader, m_rectRowHeader);

	pSel = GetCurSel ();
	if (pSel != NULL)
	{
		pSel->AdjustButtonRect ();
// 		if(pSel->IsInPlaceEditing())
// 		{
// 			CRect rect;
// 			pSel->GetInPlaceWnd()->GetWindowRect(rect);
// 			ScreenToClient(rect);
// 			rect.OffsetRect(0, - (m_nVertScrollOffset - nPrevOffset));
// 			pSel->GetInPlaceWnd()->MoveWindow(rect);
// 			CRect rectHeader = GetHeaderRect();
// // 			if(rect.top < rectHeader.bottom)pSel->GetInPlaceWnd()->ShowWindow(SW_HIDE);
// // 			else pSel->GetInPlaceWnd()->ShowWindow(SW_SHOW);
// 		}
	}

	OnAfterVScroll (m_nVertScrollOffset, nPrevOffset);
	
	UpdateWindow ();
}

void CArrayGrid::OnItemChanged( CBCGPGridItem* pItem, int nRow, int nColumn )
{
	CBCGPGridCtrl::OnItemChanged(pItem, nRow, nColumn);

	CString str;
	ConvertBSTR2CString( str, pItem->GetValue().bstrVal );

	CString strOld = ((CDlgParamEditArray*)GetParent())->m_vArray[nRow].c_str();

	CDlgParamEditArray::DataType type = ((CDlgParamEditArray*)GetParent())->m_dataType;

	switch(type)
	{
	case CDlgParamEditArray::TypeInteger:
		{
			str.Trim(' ');
			if(CStringChecker::CheckInt(str) == FALSE)
			{
				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
				pItem->SetValue(strOld.GetBuffer());
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//ËÉ¿ª×ó¼ü
				return;
			}
// 			if(CStringChecker::CheckLength(str, 10) == FALSE)
// 			{
// 				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				pItem->SetValue(strOld.GetBuffer());
// 				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//ËÉ¿ª×ó¼ü
// 				return;
// 			}
			break;
		}
	case CDlgParamEditArray::TypeDouble:
		{
			str.Trim(' ');
			if(CStringChecker::CheckDouble(str) == FALSE)
			{
				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
				pItem->SetValue(strOld.GetBuffer());
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//ËÉ¿ª×ó¼ü
				return;
			}
			str = CStringChecker::FormatDouble(str);
			pItem->TextToVar(str);
			//pItem->Redraw();
			//pItem->SetValue(str.GetBuffer());
			//if(GetAsyncKeyState(VK_LBUTTON))
			//{
				//POINT CurPoint;
				//GetCursorPos(&CurPoint);

				//mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			//}
// 			if(CStringChecker::CheckLength(str, 20) == FALSE)
// 			{
// 				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				pItem->SetValue(strOld.GetBuffer());
// 				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//ËÉ¿ª×ó¼ü
// 				return;
// 			}
			break;
		}
	case CDlgParamEditArray::TypeBool:
		{
			if(CStringChecker::CheckBool(str) == FALSE)
			{
				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
				pItem->SetValue(strOld.GetBuffer());
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//ËÉ¿ª×ó¼ü
				return;
			}
			break;
		}
	case CDlgParamEditArray::TypeString:
		{
// 			if(CStringChecker::CheckLength(str, 3000) == FALSE)
// 			{
// 				MessageBox( CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
// 				pItem->SetValue(strOld.GetBuffer());
// 				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);//ËÉ¿ª×ó¼ü
// 				return;
// 			}
			break;
		}
	}

	((CDlgParamEditArray*)GetParent())->m_vArray[nRow] = str;
	//pItem->SetValue(str.GetBuffer());

	return;
}
void CArrayGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBCGPGridCtrl::OnLButtonDown(nFlags, point);
	CDlgParamEditArray::DataType type = ((CDlgParamEditArray*)GetParent())->m_dataType;
	if (type!=CDlgParamEditArray::TypeBool)
		return;

	if (IsColumnSelected(0))
		return;

	CBCGPGridItem * pItem = GetCurSelItem();
	if (pItem==NULL)
		return;

	CString str;
	ConvertBSTR2CString( str, pItem->GetValue().bstrVal );
	if ( str.CompareNoCase("true") == 0 )
		str = "false";
	else
		str = "true";

	pItem->SetValue( (LPCTSTR)(str) );
	int iRow = pItem->GetGridItemID().m_nRow;
	((CDlgParamEditArray*)GetParent())->m_vArray[iRow] = str;
}