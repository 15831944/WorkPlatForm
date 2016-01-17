// ZStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "ZStatic.h"
#include "ResLibImageSet.h"
#include "ximage.h"


// CZStatic

IMPLEMENT_DYNAMIC(CZStatic, CStatic)

CZStatic::CZStatic()
{
	CCxStaticRes* pImage = GetFlowIfResImageData( "FlowIfResId_ResLib_Arrow" );


	if ( pImage != NULL && pImage->m_pImgNormal)
	{
		m_cxImage.Load(pImage->m_strPath, CXIMAGE_FORMAT_PNG);
	}

}

CZStatic::~CZStatic()
{
}


BEGIN_MESSAGE_MAP(CZStatic, CStatic)
END_MESSAGE_MAP()



// CZStatic 消息处理程序




void CZStatic::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{

	// TODO:  添加您的代码以绘制指定项
// 	CDC *pDC = GetWindowDC();  
// 
// 	Graphics g(pDC->GetSafeHdc());
// 
// 	CCxStaticRes* pImage = GetFlowIfResImageData( "FlowIfResId_ResLib_Arrow" );
// 
// 	CRect rect;
// 	GetClientRect(rect);
// 	CPoint pointCenter(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2);
// 	int y = pImage->m_pImgNormal->GetHeight();
// 
// 	if ( pImage != NULL && pImage->m_pImgNormal)
// 		g.DrawImage(pImage->m_pImgNormal, 5, pointCenter.y - y / 2, 14, 14);
// 
// 	g.ReleaseHDC( pDC->GetSafeHdc() );
// 
// 	ReleaseDC(pDC);  

	//Cximage贴图
	CDC *pDC = GetWindowDC();  

	Graphics g(pDC->GetSafeHdc());

	CCxStaticRes* pImage = GetFlowIfResImageData( "FlowIfResId_ResLib_Arrow" );

	CRect rect;
	GetClientRect(rect);
	CPoint pointCenter(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2);

	int y = pImage->m_pImgNormal->GetHeight();

	if ( pImage != NULL && pImage->m_pImgNormal)
	{			
		m_cxImage.Draw(pDC->GetSafeHdc(), 5, pointCenter.y - y / 2);
	}

	g.ReleaseHDC( pDC->GetSafeHdc() );

	ReleaseDC(pDC);  

}
