#pragma once
#include <BCGPGridCtrl.h>

class CCxGridButtonItem : public CBCGPGridItem
{
	friend class CCxBCGPGrid;
public:
	CCxGridButtonItem (LPCTSTR lpszText, UINT id);

protected:
	virtual CWnd* CreateInPlaceEdit (CRect /*rectEdit*/, BOOL& /*bDefaultFormat*/)
	{
		return NULL;
	}

	virtual BOOL OnSetCursor () const
	{
		return FALSE; /* Use default */	
	}

	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);

	UINT	m_id;
};
