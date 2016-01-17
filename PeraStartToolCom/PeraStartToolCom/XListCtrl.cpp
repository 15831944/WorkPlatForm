// XListCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XListCtrl.h"


// CXListCtrl

IMPLEMENT_DYNAMIC(CXListCtrl, CListCtrl)

CXListCtrl::CXListCtrl()
{

}

CXListCtrl::~CXListCtrl()
{
}


BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CXListCtrl ��Ϣ�������



void CXListCtrl::DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC); 

	LVITEM lvi = {0}; 
	lvi.mask = LVIF_STATE;//|LVIF_IMAGE; 
	lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED ; 
	lvi.iItem = lpDrawItemStruct->itemID; 
	BOOL bGet = GetItem(&lvi); 

	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)||((lvi.state & LVIS_SELECTED) && 
		((GetFocus() == this)|| (GetStyle() & LVS_SHOWSELALWAYS)))); 

	// ���ı����� 
	CRect rcBack = lpDrawItemStruct->rcItem; 
	pDC->SetBkMode(TRANSPARENT); 
	if( bHighlight ) //�����ѡ��
	{ 
		COLORREF ref=RGB(0,0,0);
		pDC->SetTextColor(ref); 
		pDC->SetBkColor(ref); 

		pDC->FillRect(rcBack, &CBrush(RGB( 0xff, 0xe4, 0x91 )/*0xEEDED4*/));
	} 
	else 
	{ 

		pDC->SetTextColor(0x000000/*0x0C0F80*/); 
		pDC->FillRect(rcBack, &CBrush(0xFFFFFF/*0xEEF1E3*/)); 

		//if (lpDrawItemStruct->itemID%2 ==0) 
		//{ 
		//	pDC->SetTextColor(0x000000/*0x0C0F80*/); 
		//	pDC->FillRect(rcBack, &CBrush(0xFFFFFF/*0xEEF1E3*/)); 
		//} 
		//else 
		//{ 
		//	pDC->SetTextColor(0x000000/*RGB(0,128,255)*/); 
		//	pDC->FillRect(rcBack, &CBrush(RGB(255,255,204)/*0xFFFFCC*/)); 
		//} 
	} 

	//����������ɫ 
	CString str; 

	//�õ������С 
	CRect rcFocus = lpDrawItemStruct->rcItem; 
	rcFocus.DeflateRect(1,1,1,1); 

	if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) 
	{ 
		//д�ı� 
		CString szText; 
		for (int i = 0; i < /*GetColumns()*/10; i++) 
		{ //ѭ���õ��ı� 
			CRect rcItem; 
			if ( !GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_LABEL, rcItem )) 
				continue; 
			szText = GetItemText( lpDrawItemStruct->itemID, i ); 

			rcItem.left += 5; rcItem.right -= 1; 
			pDC->DrawText(szText/*szText*/, lstrlen(szText), &rcItem, DT_NOPREFIX|DT_SINGLELINE); 
		} 
	} 
}