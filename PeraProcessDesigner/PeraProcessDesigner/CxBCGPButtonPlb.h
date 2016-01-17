#pragma once
#include "cxbcgpbutton.h"
#include "ResLibImageSet.h"

class CCxBCGPControlRendererButtonPlb : public CBCGPControlRenderer
{
	DECLARE_DYNCREATE(CCxBCGPControlRendererButtonPlb)

public:
	CCxBCGPControlRendererButtonPlb();
	virtual ~CCxBCGPControlRendererButtonPlb();

	// Operations:
public:
	BOOL CreateCustom( const CBCGPControlRendererParams& params, CString sPngPath, BOOL bFlipvert = FALSE);
};

class CCxBCGPButtonPlb :
	public CCxBCGPButton
{
	DECLARE_DYNAMIC(CCxBCGPButtonPlb)
public:
	CCxBCGPButtonPlb(void);
	~CCxBCGPButtonPlb(void);

	virtual void DoDrawItem( CDC* pDCPaint, CRect rectClient, UINT itemState );
	BOOL OnDrawPushButton (CDC* pDC, CRect rect, CBCGPButton* pButton, COLORREF& clrText);
	void SetImage(CCxStaticRes * pImg, CCxStaticRes * pImgDisabled);
	CCxBCGPControlRendererButtonPlb m_renderBtn;

	void Reset();
};

