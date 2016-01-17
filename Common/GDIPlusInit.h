#pragma once
#include <gdiplus.h>
using namespace Gdiplus;

class CGDIPlusInit
{
public:
	CGDIPlusInit(void);
	virtual ~CGDIPlusInit(void);

	BOOL InitGDIPlus(void);
	BOOL ReleaseGDIPlus(void);

	ULONG_PTR m_gdiplusToken;    // ULONG PTR 为int64 类型
};

//缩放图片
extern Bitmap * ScaleBitmap( Bitmap *& pSrc, UINT nCx, UINT nCy, BOOL bCreate = TRUE );
//设置高亮
extern Bitmap * HighlightBitmap( Bitmap * pSrc, float fGamma = 2.2f, BOOL bCreate = TRUE );

//设置高亮
extern void DrawHighlight( Image * pImage, Graphics * pGraphics, RectF rcDraw, float fGamma = 2.2f );

//
extern void DrawSudoku(Image * pImage, Graphics * pGraphics, RectF& rcDraw
	, UINT nLeft = 3, UINT nTop = 3, UINT nRight = 3, UINT nBottom = 3 );

extern Bitmap * GetNewSudokuBitmap(LPCTSTR lpszPath, int nCx, int nCy);


//
extern Image * LoadImage(LPCTSTR lpszPath);
//
extern Bitmap * GetNewBitmap(LPCTSTR lpszPath);

extern void PrepDrawByGdiPlus(CDC * pDC, Bitmap*& pCanvas, Graphics*& pGrap, RectF& rcClient, COLORREF crBg);
extern void DrawOnGDI( CDC * pDC, Bitmap*& pCanvas, Graphics*& pGrap );