#pragma once
#include "zbuttondraw.h"
class CZToolButton :
	public CZButtonDraw
{
	DECLARE_DYNAMIC(CZToolButton)
protected:
	DECLARE_MESSAGE_MAP()
public:
	CZToolButton(void);
	virtual ~CZToolButton(void);

	virtual void DrawImg(CCxStaticRes* pImage, CDC* pDC);
	virtual void DrawButton(CDC *pDC);

	void SetImageBgHover( CCxStaticRes * pImg);
	void SetImageBgDown( CCxStaticRes * pImg);

	void Reset(); //��λ

	CCxStaticRes* m_pImgBgHover; //����
	CCxStaticRes* m_pImgBgDown; //����
};

