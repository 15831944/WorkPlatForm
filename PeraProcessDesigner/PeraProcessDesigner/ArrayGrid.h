#pragma once
#include "bcgpgridctrl.h"
class CArrayGrid :
	public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(CArrayGrid)
protected:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual int GetLeftMarginWidth () const { return 0; }
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn);
	DECLARE_MESSAGE_MAP()

public:
	CArrayGrid(void);
	virtual ~CArrayGrid(void);
};

