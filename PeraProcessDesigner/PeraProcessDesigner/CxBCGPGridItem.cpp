#include "StdAfx.h"
#include "CxBCGPGridItem.h"

//////////////////////////////////////////////////////////////////////////

CCxGridButtonItem::CCxGridButtonItem (LPCTSTR lpszText, UINT id) :
	CBCGPGridItem (lpszText)
{
	m_id = id;
}

void CCxGridButtonItem::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (pDC);

	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	if ( pGridCtrl == NULL ) return;

	OnFillBackground (pDC, rect);

	rect.DeflateRect (1, 1);
	
	//if ( pGridCtrl->IsSelectionBorder() )
	//{
	//	pDC->FillRect (rect, &globalData.brLight);
	//}
	//else
	//{
	//	pDC->FillRect (rect, &globalData.brBarFace);
	//}
	
	pDC->FillRect (rect, &globalData.brBarFace);

	pDC->Draw3dRect (rect, globalData.clrBarLight, globalData.clrBarDkShadow);
	rect.DeflateRect (1, 1);
	pDC->Draw3dRect (rect, globalData.clrBarHilite, globalData.clrBarShadow);

	COLORREF clrText = pDC->SetTextColor (globalData.clrBarText);

	CString strText = (LPCTSTR)(_bstr_t) m_varValue;

	pDC->DrawText (strText, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	pDC->SetTextColor (clrText);
}

BOOL CCxGridButtonItem::OnClickValue (UINT uiMsg, CPoint point)
{
	CBCGPGridCtrl* pGridCtrl = GetOwnerList();
	if ( pGridCtrl == NULL ) return TRUE;

	pGridCtrl->SendMessage (WM_COMMAND, MAKEWPARAM (m_id, BN_CLICKED));
	return TRUE;
}
