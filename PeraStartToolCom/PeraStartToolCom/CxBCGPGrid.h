#pragma once
#include <BCGPGridCtrl.h>

#define  WM_MSG_GRID_DBCLK   WM_USER + 1 //用户双击表格项消息

class CCxBCGPGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(CCxBCGPGrid)
public:
	CCxBCGPGrid(void);
	CCxBCGPGrid(int nMemBlockSize);
	virtual ~CCxBCGPGrid(void);

	HWND m_hwndRecvMsg;
	DECLARE_MESSAGE_MAP()
protected:
	virtual void SetRowHeight();

	virtual int GetLeftMarginWidth() const;

	virtual void InitConstructor();

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

