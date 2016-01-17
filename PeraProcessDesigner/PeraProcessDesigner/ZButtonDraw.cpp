// ZButtonDraw.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "ZButtonDraw.h"
#include "ResLibImageSet.h"
#include "CxBCGPVisualManager2007.h"


// CZButtonDraw

IMPLEMENT_DYNAMIC(CZButtonDraw, CBCGButton)

CZButtonDraw::CZButtonDraw()
{
	m_nState = 0;

	m_Style = 0;                //按钮形状风格
	b_InRect = false;           //鼠标进入标志
	m_strText = _T("");         //按钮文字（使用默认文字）
	m_ForeColor = RGB(0,0,0);             //文字颜色（黑色）
	//m_BackColor = RGB(243,243,243);       //背景色（灰白色）
	m_BackColor = GetSysColor(CTLCOLOR_DLG);
	m_BackColor = globalData.clrBarFace;//对话框背景颜色
	//m_BackColor = GetClassLong(GetParent()->GetSafeHwnd(), GCL_HBRBACKGROUND);
	m_LockForeColor = GetSysColor(COLOR_GRAYTEXT);     //锁定按钮的文字颜色
	p_Font = NULL;                        //字体指针

}

CZButtonDraw::~CZButtonDraw()
{
}


BEGIN_MESSAGE_MAP(CZButtonDraw, CBCGButton)
	//ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//ON_WM_MOUSEMOVE()
	//ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CZButtonDraw 消息处理程序
void CZButtonDraw::DrawImg(CDC* pDC)
{
	if(IsWindowEnabled())
	{			
		DrawImg(m_pImg, pDC);
	}
	else
	{		
		DrawImg(m_pImgDisabled, pDC);
	}
}

void CZButtonDraw::DrawImg(CCxStaticRes* pImage)
{
	CDC *pDC = GetWindowDC();  

	Graphics g(pDC->GetSafeHdc());

	CRect rect;
	GetClientRect(rect);
	CPoint pointCenter(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2);

	UINT nState = GetState();

	if(nState & BST_PUSHED)
	{
		pointCenter.Offset(1, 1);
	}

	int x = pImage->m_pImgNormal->GetWidth();
	int y = pImage->m_pImgNormal->GetHeight();

	if ( pImage != NULL && pImage->m_pImgNormal)
		g.DrawImage(pImage->m_pImgNormal, pointCenter.x - x / 2, pointCenter.y - y / 2, x, y);

	g.ReleaseHDC(pDC->GetSafeHdc() );
	ReleaseDC(pDC);
}

void CZButtonDraw::DrawImg(CCxStaticRes* pImage, CDC* pDC)
{
	//CDC *pDC = GetWindowDC();  

	Graphics g(pDC->GetSafeHdc());

	CRect rect;
	GetClientRect(rect);
	CPoint pointCenter(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2);

	UINT nState = GetState();

	if(nState & BST_PUSHED)
	{
		pointCenter.Offset(1, 1);
	}

	int x = pImage->m_pImgNormal->GetWidth();
	int y = pImage->m_pImgNormal->GetHeight();

	if ( pImage != NULL && pImage->m_pImgNormal)
		g.DrawImage(pImage->m_pImgNormal, pointCenter.x - x / 2, pointCenter.y - y / 2, x, y);

	//g.ReleaseHDC(pDC->GetSafeHdc() );
	//ReleaseDC(pDC);
}

void CZButtonDraw::OnPaint()
{
	CBCGButton::OnPaint();

// 	if(IsWindowEnabled())
// 	{			
// 		DrawImg(m_pImg);
// 	}
// 	else
// 	{		
// 		DrawImg(m_pImgDisabled);
// 	}

}


LRESULT CZButtonDraw::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	LRESULT res = CBCGButton::WindowProc(message, wParam, lParam);
	if (message == BM_SETSTATE)
	{
// 		Invalidate(FALSE);
// 		UpdateWindow();
	}
	return res;
}

void CZButtonDraw::SetImage(CCxStaticRes * pImg)
{
	m_pImg = pImg;
}

void CZButtonDraw::SetImageDisabled(CCxStaticRes * pImg)
{
	m_pImgDisabled = pImg;
}



void CZButtonDraw::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_nState = 1;

	m_Style = 2;
	//Invalidate();         //重绘按钮

	CBCGButton::OnLButtonDown(nFlags, point);
}


void CZButtonDraw::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_nState = 0;

	m_Style = 1;



	
	if(!m_ButRect.PtInRect(point))
	{
		ReleaseCapture();
		//Invalidate();         //重绘按钮
	}
	CBCGButton::OnLButtonUp(nFlags, point);
}

BOOL CZButtonDraw::EnableWindow(BOOL bEnable /* = TRUE */)
{
	BOOL b = CBCGButton::EnableWindow(bEnable);
// 	if(bEnable)
// 	{
// 		DrawImg(m_pImg);
// 	}
// 	else
// 	{
// 		DrawImg(m_pImgDisabled);
// 	}
	//OnPaint();
	return b;
}

void CZButtonDraw::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	ModifyStyle( 0, BS_OWNERDRAW );         //设置按钮属性为自画式

	CBCGButton::PreSubclassWindow();
}


void CZButtonDraw::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  添加您的代码以绘制指定项
	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );

	//m_BackColor = globalData.clrBarFace;//对话框背景颜色
	//m_BackColor = RGB(255,255,255);

	m_ButRect = lpDrawItemStruct->rcItem;     //获取按钮尺寸

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, m_ButRect.Width(), m_ButRect.Height());
	CBitmap* pOldBmp = dc.SelectObject(&bmp);

	if( m_strText.IsEmpty() )
		GetWindowText( m_strText );           //获取按钮文本

	int nSavedDC = pDC->SaveDC();
	VERIFY( pDC );
	//DrawButton( pDC );                 //绘制按钮
	//CBCGButton::OnEraseBkgnd(pDC);
	//DrawImg(pDC);  //背景绘制
// 	OnEraseBkgnd(&dc);
	DrawButton( &dc); 
 	DrawImg(&dc);
// 
 	pDC->BitBlt(0, 0, m_ButRect.Width(), m_ButRect.Height(), &dc, 0, 0, SRCCOPY);

	dc.SelectObject(pOldBmp);

	pDC->RestoreDC( nSavedDC );

}


void CZButtonDraw::DrawButton(CDC *pDC)
{
	//调整状态
	if( m_Style==3 ) m_Style = 0;
	if( GetStyle() & WS_DISABLED )
		m_Style = 3;     //禁止状态
	//根据状态调整边框颜色和文字颜色
	COLORREF bColor, fColor;     //bColor为边框颜色，fColor为文字颜色
	switch( m_Style )
	{
	case 0: bColor = RGB(192,192,192); fColor = m_ForeColor; break;   //正常按钮
	case 1: bColor = RGB(255,255,255); fColor = m_ForeColor; break;   //鼠标进入时按钮
	case 2: bColor = RGB(192,192,192); fColor = m_ForeColor; break;   //按下的按钮
	case 3: bColor = m_BackColor; fColor = m_LockForeColor; break;    //锁定的按钮
	}
	//绘制按钮背景
	CBrush Brush;
	Brush.CreateSolidBrush( m_BackColor );     //背景刷
	CBrush* pOldBrush = pDC->SelectObject( &Brush );
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, bColor );
	CPen* pOldPen = pDC->SelectObject( &Pen );
	pDC->FillSolidRect(m_ButRect, m_BackColor);    //画圆角矩形
	//pDC->RoundRect(&m_ButRect,CPoint(5,5));    //画圆角矩形
	//绘制按钮按下时的边框

	UINT nState = GetState();
	if(nState & BST_PUSHED)
	{
	}
	else
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+2, m_ButRect.top+1, m_ButRect.right, m_ButRect.bottom );
		pDC->DrawEdge( &Rect, BDR_RAISEDINNER, BF_RECT );     //画边框
	}

	if( m_Style!=2 )
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+2, m_ButRect.top+1, m_ButRect.right, m_ButRect.bottom );
		//pDC->DrawEdge( &Rect, BDR_RAISEDINNER, BF_RECT );     //画边框
	}
	//绘制按钮文字
	pDC->SetTextColor( fColor );         //画文字
	pDC->SetBkMode( TRANSPARENT );
	pDC->DrawText( m_strText, &m_ButRect, DT_SINGLELINE | DT_CENTER
		| DT_VCENTER | DT_END_ELLIPSIS);
	//绘制拥有焦点按钮的虚线框
	if( GetFocus()==this )
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+3, m_ButRect.top+2, m_ButRect.right-3, m_ButRect.bottom-2 );
		//pDC->DrawFocusRect( &Rect );     //画拥有焦点的虚线框
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}  



void CZButtonDraw::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if( !b_InRect || GetCapture()!=this )     //鼠标进入按钮
	{
		b_InRect = true;     //设置进入标志
		SetCapture();        //捕获鼠标
		m_Style = 1;         //设置按钮状态
		Invalidate();        //重绘按钮
	}
	else
	{
		if ( !m_ButRect.PtInRect(point) )     //鼠标离开按钮
		{
			b_InRect = false;    //清除进入标志
			ReleaseCapture();    //释放捕获的鼠标
			m_Style = 0;         //设置按钮状态
			Invalidate();        //重绘按钮
		}
	}

	CBCGButton::OnMouseMove(nFlags, point);
}

//设置按钮文本
void CZButtonDraw::SetText(CString str)
{
	m_strText = _T("");
	SetWindowText(str);
}  

//设置文本颜色
void CZButtonDraw::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
	Invalidate();
}  

//设置背景颜色
void CZButtonDraw::SetBkColor(COLORREF color)
{
	m_BackColor = color;
	Invalidate();
}  

//设置字体(字体高度、字体名)
void CZButtonDraw::SetTextFont(int FontHight,LPCTSTR FontName)
{
	if ( p_Font )     delete p_Font;     //删除旧字体
	p_Font = new CFont;
	p_Font->CreatePointFont( FontHight, FontName );     //创建新字体
	SetFont( p_Font );                 //设置字体
}  



void CZButtonDraw::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CBCGButton::OnLButtonDblClk(nFlags, point);
}


LRESULT CZButtonDraw::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (message == WM_LBUTTONDBLCLK)
	{
		message = WM_LBUTTONDOWN;
	}
	return CBCGButton::DefWindowProc(message, wParam, lParam);//很重要,消除双击没反应

}


BOOL CZButtonDraw::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CBCGButton::OnEraseBkgnd(pDC);
}
