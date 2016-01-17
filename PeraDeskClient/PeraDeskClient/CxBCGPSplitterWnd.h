#pragma once
#include <BCGPSplitterWnd.h>


// CCxBCGPSplitterWnd

class CCxBCGPSplitterWnd : public CBCGPSplitterWnd
{
	DECLARE_DYNAMIC(CCxBCGPSplitterWnd)

public:
	CCxBCGPSplitterWnd();
	virtual ~CCxBCGPSplitterWnd();
	virtual void CCxBCGPSplitterWnd::RecalcLayout();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void StartTracking( int ht );

	virtual void StopTracking( BOOL bAccept );

	afx_msg	void OnMouseMove(UINT nFlags, CPoint pt);

	virtual void GetHitRect(int ht, CRect& rect);

};


