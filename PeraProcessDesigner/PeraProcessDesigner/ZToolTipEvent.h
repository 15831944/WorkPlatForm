#pragma once
#include <afxcmn.h>
#include <BCGPTooltipManager.h>

class ZToolTipEvent
{
public:
	ZToolTipEvent() {m_pToolTip = NULL; m_pLstSysButtons = NULL;}
	~ZToolTipEvent() {CBCGPTooltipManager::DeleteToolTip (m_pToolTip);}

	CToolTipCtrl*			m_pToolTip; //TIP控件
	const CObList*       m_pLstSysButtons; //系统按钮集合
};