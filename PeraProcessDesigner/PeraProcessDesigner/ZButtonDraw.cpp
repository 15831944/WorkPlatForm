// ZButtonDraw.cpp : ʵ���ļ�
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

	m_Style = 0;                //��ť��״���
	b_InRect = false;           //�������־
	m_strText = _T("");         //��ť���֣�ʹ��Ĭ�����֣�
	m_ForeColor = RGB(0,0,0);             //������ɫ����ɫ��
	//m_BackColor = RGB(243,243,243);       //����ɫ���Ұ�ɫ��
	m_BackColor = GetSysColor(CTLCOLOR_DLG);
	m_BackColor = globalData.clrBarFace;//�Ի��򱳾���ɫ
	//m_BackColor = GetClassLong(GetParent()->GetSafeHwnd(), GCL_HBRBACKGROUND);
	m_LockForeColor = GetSysColor(COLOR_GRAYTEXT);     //������ť��������ɫ
	p_Font = NULL;                        //����ָ��

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



// CZButtonDraw ��Ϣ�������
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
	// TODO: �ڴ����ר�ô����/����û���

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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_nState = 1;

	m_Style = 2;
	//Invalidate();         //�ػ水ť

	CBCGButton::OnLButtonDown(nFlags, point);
}


void CZButtonDraw::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_nState = 0;

	m_Style = 1;



	
	if(!m_ButRect.PtInRect(point))
	{
		ReleaseCapture();
		//Invalidate();         //�ػ水ť
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
	// TODO: �ڴ����ר�ô����/����û���
	ModifyStyle( 0, BS_OWNERDRAW );         //���ð�ť����Ϊ�Ի�ʽ

	CBCGButton::PreSubclassWindow();
}


void CZButtonDraw::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  ������Ĵ����Ի���ָ����
	CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );

	//m_BackColor = globalData.clrBarFace;//�Ի��򱳾���ɫ
	//m_BackColor = RGB(255,255,255);

	m_ButRect = lpDrawItemStruct->rcItem;     //��ȡ��ť�ߴ�

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, m_ButRect.Width(), m_ButRect.Height());
	CBitmap* pOldBmp = dc.SelectObject(&bmp);

	if( m_strText.IsEmpty() )
		GetWindowText( m_strText );           //��ȡ��ť�ı�

	int nSavedDC = pDC->SaveDC();
	VERIFY( pDC );
	//DrawButton( pDC );                 //���ư�ť
	//CBCGButton::OnEraseBkgnd(pDC);
	//DrawImg(pDC);  //��������
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
	//����״̬
	if( m_Style==3 ) m_Style = 0;
	if( GetStyle() & WS_DISABLED )
		m_Style = 3;     //��ֹ״̬
	//����״̬�����߿���ɫ��������ɫ
	COLORREF bColor, fColor;     //bColorΪ�߿���ɫ��fColorΪ������ɫ
	switch( m_Style )
	{
	case 0: bColor = RGB(192,192,192); fColor = m_ForeColor; break;   //������ť
	case 1: bColor = RGB(255,255,255); fColor = m_ForeColor; break;   //������ʱ��ť
	case 2: bColor = RGB(192,192,192); fColor = m_ForeColor; break;   //���µİ�ť
	case 3: bColor = m_BackColor; fColor = m_LockForeColor; break;    //�����İ�ť
	}
	//���ư�ť����
	CBrush Brush;
	Brush.CreateSolidBrush( m_BackColor );     //����ˢ
	CBrush* pOldBrush = pDC->SelectObject( &Brush );
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, bColor );
	CPen* pOldPen = pDC->SelectObject( &Pen );
	pDC->FillSolidRect(m_ButRect, m_BackColor);    //��Բ�Ǿ���
	//pDC->RoundRect(&m_ButRect,CPoint(5,5));    //��Բ�Ǿ���
	//���ư�ť����ʱ�ı߿�

	UINT nState = GetState();
	if(nState & BST_PUSHED)
	{
	}
	else
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+2, m_ButRect.top+1, m_ButRect.right, m_ButRect.bottom );
		pDC->DrawEdge( &Rect, BDR_RAISEDINNER, BF_RECT );     //���߿�
	}

	if( m_Style!=2 )
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+2, m_ButRect.top+1, m_ButRect.right, m_ButRect.bottom );
		//pDC->DrawEdge( &Rect, BDR_RAISEDINNER, BF_RECT );     //���߿�
	}
	//���ư�ť����
	pDC->SetTextColor( fColor );         //������
	pDC->SetBkMode( TRANSPARENT );
	pDC->DrawText( m_strText, &m_ButRect, DT_SINGLELINE | DT_CENTER
		| DT_VCENTER | DT_END_ELLIPSIS);
	//����ӵ�н��㰴ť�����߿�
	if( GetFocus()==this )
	{
		CRect Rect;
		Rect.SetRect( m_ButRect.left+3, m_ButRect.top+2, m_ButRect.right-3, m_ButRect.bottom-2 );
		//pDC->DrawFocusRect( &Rect );     //��ӵ�н�������߿�
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}  



void CZButtonDraw::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if( !b_InRect || GetCapture()!=this )     //�����밴ť
	{
		b_InRect = true;     //���ý����־
		SetCapture();        //�������
		m_Style = 1;         //���ð�ť״̬
		Invalidate();        //�ػ水ť
	}
	else
	{
		if ( !m_ButRect.PtInRect(point) )     //����뿪��ť
		{
			b_InRect = false;    //��������־
			ReleaseCapture();    //�ͷŲ�������
			m_Style = 0;         //���ð�ť״̬
			Invalidate();        //�ػ水ť
		}
	}

	CBCGButton::OnMouseMove(nFlags, point);
}

//���ð�ť�ı�
void CZButtonDraw::SetText(CString str)
{
	m_strText = _T("");
	SetWindowText(str);
}  

//�����ı���ɫ
void CZButtonDraw::SetForeColor(COLORREF color)
{
	m_ForeColor = color;
	Invalidate();
}  

//���ñ�����ɫ
void CZButtonDraw::SetBkColor(COLORREF color)
{
	m_BackColor = color;
	Invalidate();
}  

//��������(����߶ȡ�������)
void CZButtonDraw::SetTextFont(int FontHight,LPCTSTR FontName)
{
	if ( p_Font )     delete p_Font;     //ɾ��������
	p_Font = new CFont;
	p_Font->CreatePointFont( FontHight, FontName );     //����������
	SetFont( p_Font );                 //��������
}  



void CZButtonDraw::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CBCGButton::OnLButtonDblClk(nFlags, point);
}


LRESULT CZButtonDraw::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���

	if (message == WM_LBUTTONDBLCLK)
	{
		message = WM_LBUTTONDOWN;
	}
	return CBCGButton::DefWindowProc(message, wParam, lParam);//����Ҫ,����˫��û��Ӧ

}


BOOL CZButtonDraw::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	return CBCGButton::OnEraseBkgnd(pDC);
}
