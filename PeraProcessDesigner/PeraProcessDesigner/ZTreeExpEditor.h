#pragma once
#include "zgridctrl.h"
class ZTreeExpEditor :
	public CZGridCtrl
{
public:
	ZTreeExpEditor(void);
	virtual ~ZTreeExpEditor(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

