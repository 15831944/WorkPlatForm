#pragma once
#include <BCGPGridCtrl.h>
class CCxBCGPGrid : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(CCxBCGPGrid)
public:
	CCxBCGPGrid(void);
	CCxBCGPGrid(int nMemBlockSize);
	virtual ~CCxBCGPGrid(void);

	HWND m_hwndRecvMsg;
protected:
	afx_msg void OnButtonClicked (UINT uID);
	DECLARE_MESSAGE_MAP()

	virtual void SetRowHeight();

	virtual int GetLeftMarginWidth() const;

	virtual void InitConstructor();

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

