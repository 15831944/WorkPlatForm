#pragma once
#include <BCGPToolbarButton.h>

// CCxBCGPToolbarButton ÃüÁîÄ¿±ê

class CCxBCGPToolbarButton : public CBCGPToolbarButton
{
	DECLARE_SERIAL(CCxBCGPToolbarButton)
public:
	CCxBCGPToolbarButton();
	CCxBCGPToolbarButton (UINT uiID, int iImage, LPCTSTR lpszText = NULL, 
		BOOL bUserButton = FALSE, BOOL bLocked = FALSE);
	virtual ~CCxBCGPToolbarButton();

	CRect GetRect()
	{
		return m_rect;
	}
};


