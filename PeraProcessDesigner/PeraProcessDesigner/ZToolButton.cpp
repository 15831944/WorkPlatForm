#include "StdAfx.h"
#include "ZToolButton.h"


IMPLEMENT_DYNAMIC(CZToolButton, CBCGButton)

CZToolButton::CZToolButton(void)
{
}


CZToolButton::~CZToolButton(void)
{
}

BEGIN_MESSAGE_MAP(CZToolButton, CZButtonDraw)
	//ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CZToolButton::DrawImg(CCxStaticRes* pImage, CDC* pDC)
{
	//CDC *pDC = GetWindowDC();  

	Graphics g(pDC->GetSafeHdc());

	CRect rect;
	GetClientRect(rect);
	CPoint pointCenter(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2);

	UINT nState = GetState();

	if(nState & BST_PUSHED)
	{
		//pointCenter.Offset(1, 1);
	}

	int x = pImage->m_pImgNormal->GetWidth();
	int y = pImage->m_pImgNormal->GetHeight();

	if ( pImage != NULL && pImage->m_pImgNormal)
		g.DrawImage(pImage->m_pImgNormal, pointCenter.x - x / 2, pointCenter.y - y / 2, x, y);

	//g.ReleaseHDC(pDC->GetSafeHdc() );
	//ReleaseDC(pDC);
}

void CZToolButton::DrawButton(CDC *pDC)
{

	//���ư�ť����
	CBrush Brush;
	Brush.CreateSolidBrush( m_BackColor );     //����ˢ
	CBrush* pOldBrush = pDC->SelectObject( &Brush );
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, m_BackColor );
	CPen* pOldPen = pDC->SelectObject( &Pen );
	pDC->FillSolidRect(m_ButRect, m_BackColor);    //��Բ�Ǿ���
	//pDC->RoundRect(&m_ButRect,CPoint(5,5));    //��Բ�Ǿ���
	//���ư�ť����ʱ�ı߿�

	UINT nState = GetState();
	if(nState & BST_PUSHED)
	{//����
		DrawImg(m_pImgBgDown, pDC);
	}
	else if(GetCapture() == this)
	{//����
		DrawImg(m_pImgBgHover, pDC);
	}


	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}  

void CZToolButton::SetImageBgHover( CCxStaticRes * pImg )
{
	m_pImgBgHover = pImg;
}

void CZToolButton::SetImageBgDown( CCxStaticRes * pImg )
{
	m_pImgBgDown = pImg;
}

void CZToolButton::Reset()
{
	m_Style = 0;
	Invalidate();
}
