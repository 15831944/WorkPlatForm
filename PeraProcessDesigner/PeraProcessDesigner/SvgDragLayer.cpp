// SvgDragLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "SvgDragLayer.h"
#include "MainFrm.h"
#include "DlgSvg.h"
#include "value.h"
#include "reader.h"

#include <ResLibProcessData.h>
#include <ResLibRobotData.h>
#include <ResLibBaseComponentData.h>
#include "ResLibImageSet.h"
#include "PeraProcessDesignerUtility.h"
#include "Tools.h"

//SVG区域中工具栏的宽度
#define TOOLBAR_IN_SVG_LEFT 32
#define TOOLBAR_IN_SVG_TOP 30
// CSvgDragLayer 对话框

IMPLEMENT_DYNAMIC(CSvgDragLayer, CDialog)

CSvgDragLayer::CSvgDragLayer(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
{
	m_pDstImage = NULL;
	m_pDstFormImage = NULL;
	m_setPointDstImage.clear();

	m_bPointInSvg = FALSE;

	m_pHighlightImage = NULL;
	m_pHighlightFormImage = NULL;
	m_rectHighlightImage.Clear();

	m_pDragToolImage = NULL;
	m_pointDragToolImage.x = -1;
	m_pointDragToolImage.y = -1;

	m_rcCanvasWnd.SetRectEmpty();
	m_rcModelingWnd.SetRectEmpty();

	m_pDataBase = NULL;
}

CSvgDragLayer::~CSvgDragLayer()
{
	if (m_pDragToolImage)
	{
		delete m_pDragToolImage;
		m_pDragToolImage = NULL;
	}
	if (m_pDstImage)
	{
		delete m_pDstImage;
		m_pDstImage = NULL;
	}
	if (m_pDstFormImage)
	{
		delete m_pDstFormImage;
		m_pDstFormImage = NULL;
	}
	if (m_pHighlightImage)
	{
		delete m_pHighlightImage;
		m_pHighlightImage = NULL;
	}
	if (m_pHighlightFormImage)
	{
		delete m_pHighlightFormImage;
		m_pHighlightFormImage = NULL;
	}
}

void CSvgDragLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSvgDragLayer, CDialog)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CSvgDragLayer 消息处理程序


BOOL CSvgDragLayer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化	
	ModifyStyle(
		0xFFFFFFFF
		, 
		WS_POPUP
		);

	ModifyStyleEx(
		0xFFFFFFFF
		, 0);

	SetCapture();
	Init();
	UpdateBk();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSvgDragLayer::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}


void CSvgDragLayer::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnCancel();
}


void CSvgDragLayer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	EndDrag(FALSE);
	CString s;
	if (m_rcModelingWnd.PtInRect(point))
	{
		//先只联调已有组件的情况
		CResLibProcessData * pProcess = NULL;
		CResLibRobotData * pRobot = NULL;
		CResLibBaseComponentData * pBaseComponent = NULL;
		CCxStaticRes * pRobotImage = NULL;
		std::string strJson("");
		switch (m_pDataBase->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			pRobot = dynamic_cast<CResLibRobotData *>(m_pDataBase);
			if ( pRobot != NULL )
			{
				strJson = pRobot->GetSvgJsonStr();
			}

			if (ZTools::CompareExtName(pRobot->m_RobotFilePath, ".bot"))
			{
				m_sDragType = "component";
			}
			else if (ZTools::CompareExtName(pRobot->m_RobotFilePath, ".botw"))
			{
				m_sDragType = "componentWithForm";
			}

			break;
		case CResLibData::RT_PROCESS:
			pProcess = dynamic_cast<CResLibProcessData *>(m_pDataBase);
			if ( pProcess != NULL )
			{
				strJson = pProcess->GetSvgJsonStr();
			}

			m_sDragType = "process";
			break;
		case CResLibData::RT_BASECOMPONET:
			pBaseComponent = dynamic_cast<CResLibBaseComponentData *>(m_pDataBase);
			if ( pBaseComponent != NULL )
			{
				strJson = pBaseComponent->GetSvgJsonStr();
			}

			if (pBaseComponent->m_RobotType == "E712BA3D-34AD-4286-AAC5-D800BAF60415")
			//if (pBaseComponent->m_RobotType == "E5FE966F-BD57-41D9-9C19-97F4337B5D51")
			{
				m_sDragType = "form";
			}
			else
			{
				m_sDragType = "baseComponent";
			}

			break;
		default:
			break;
		}

		PutJsonToSvg(m_rectHighlightImage.m_sId, m_sDragType, strJson.c_str());
	}
	m_pDataBase = NULL;
	CDialog::OnLButtonUp(nFlags, point);
}


void CSvgDragLayer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pointDragToolImage = point;

	//鼠标在建模页面中时才显示目的点
	if (m_rcModelingWnd.PtInRect(point))
	{
		m_bPointInSvg = TRUE;
		m_rectHighlightImage.Clear();

		CPoint pointInModeling;
		pointInModeling.x = point.x - m_rcModelingWnd.left;
		pointInModeling.y = point.y - m_rcModelingWnd.top;
		for (std::set<CDragRect, CDragRectSort>::iterator it = m_setPointDstImage.begin(); it != m_setPointDstImage.end(); ++it)
		{
			if (it->m_rect.Contains((Gdiplus::REAL)pointInModeling.x, (Gdiplus::REAL)pointInModeling.y))
			{
				m_rectHighlightImage = *it;
				break;
			}
		}
	}
	else
	{
		m_bPointInSvg = FALSE;
	}

	UpdateBk();

	CDialog::OnMouseMove(nFlags, point);
}

void CSvgDragLayer::ClearDstRects()
{
	m_setPointDstImage.clear();
}

void CSvgDragLayer::AddDstRect( CDragRect point )
{
	m_setPointDstImage.insert(point);
}

void CSvgDragLayer::SetDragToolImage( Gdiplus::Image* pToolImage )
{
	if (m_pDragToolImage)
	{
		delete m_pDragToolImage;
		m_pDragToolImage = NULL;
	}
	m_pDragToolImage = pToolImage->Clone();
}

BOOL CSvgDragLayer::Init()
{
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);

	//char drive[_MAX_DRIVE];
	//char dir[_MAX_DIR];
	//char fname[_MAX_FNAME];
	//char ext[_MAX_EXT];
	//char exeFullPath[MAX_PATH]; // MAX_PATH
	//GetModuleFileName(NULL,exeFullPath,MAX_PATH);//得到程序模块名称，全路径
	//_splitpath_s( exeFullPath, drive, dir, fname, ext );
	//CString sPicPath;
	//CStringW strW;

	//sPicPath.Format("%s%simg\\DstImage.png", drive, dir);
	//strW = sPicPath;
	//m_pDstImage = NULL;
	//m_pDstImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_DST)->m_pImgNormal;
	//ASSERT(m_pDstImage);

	//sPicPath.Format("%s%simg\\DstFormImage.png", drive, dir);
	//strW = sPicPath;
	//m_pDstFormImage = NULL;
	//m_pDstFormImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_DSTFORM)->m_pImgNormal;
	//ASSERT(m_pDstFormImage);

	//sPicPath.Format("%s%simg\\HighlightImage.png", drive, dir);
	//strW = sPicPath;
	//m_pHighlightImage = NULL;
	//m_pHighlightImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_HIGHLIGHT)->m_pImgNormal;
	//ASSERT(m_pHighlightImage);

	//sPicPath.Format("%s%simg\\HighlightFormImage.png", drive, dir);
	//strW = sPicPath;
	//m_pHighlightFormImage = NULL;
	//m_pHighlightFormImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_HIGHLIGHT_FROM)->m_pImgNormal;
	//ASSERT(m_pHighlightFormImage);

	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	return TRUE;
}

void CSvgDragLayer::DrawMyImage(Gdiplus::Graphics &graph,
	Gdiplus::Image* m_pImage,
	const Gdiplus::RectF &oDstRect
	)
{
	int nImageWidth = m_pImage->GetWidth();
	int nImageHeight = m_pImage->GetHeight();
	Gdiplus::REAL dl,dr,dt,db,sl,sr,st,sb;
	//左
	if (oDstRect.X < TOOLBAR_IN_SVG_LEFT)
	{
		dl = (Gdiplus::REAL)TOOLBAR_IN_SVG_LEFT;
		sl = ((Gdiplus::REAL)TOOLBAR_IN_SVG_LEFT - oDstRect.X) / oDstRect.Width * nImageWidth;
	}
	else
	{
		dl = oDstRect.X;
		sl = 0;
	}
	//上
	if (oDstRect.Y < TOOLBAR_IN_SVG_TOP)
	{
		dt = (Gdiplus::REAL)TOOLBAR_IN_SVG_TOP;
		st = ((Gdiplus::REAL)TOOLBAR_IN_SVG_TOP - oDstRect.Y) / oDstRect.Height * nImageHeight;
	}
	else
	{
		dt = oDstRect.Y;
		st = 0;
	}
	//右
	if (oDstRect.GetRight() > m_rcModelingWnd.Width())
	{
		dr = (Gdiplus::REAL)m_rcModelingWnd.Width();
		sr = nImageWidth - (oDstRect.GetRight() - m_rcModelingWnd.Width()) / oDstRect.Width * nImageWidth;
	}
	else
	{
		dr = oDstRect.GetRight();
		sr = (Gdiplus::REAL)nImageWidth;
	}
	//下
	if (oDstRect.GetBottom() > m_rcModelingWnd.Height())
	{
		db = (Gdiplus::REAL)m_rcModelingWnd.Height();
		sb = nImageHeight - (oDstRect.GetBottom() - m_rcModelingWnd.Height()) / oDstRect.Height * nImageHeight;
	}
	else
	{
		db = oDstRect.GetBottom();
		sb = (Gdiplus::REAL)nImageHeight;
	}
	Gdiplus::RectF sRc(sl, st, sr - sl, sb - st);
	Gdiplus::RectF dRc(dl, dt, dr - dl, db - dt);
	dRc.Offset((Gdiplus::REAL)m_rcModelingWnd.left, (Gdiplus::REAL)m_rcModelingWnd.top);
	graph.DrawImage(m_pImage, dRc, sl, st, sr - sl, sb - st, Gdiplus::UnitPixel);
}

BOOL CSvgDragLayer::UpdateBk()
{
	HDC hdcScreen=::GetDC(m_hWnd);
	HDC m_hdcMemory=CreateCompatibleDC(hdcScreen);
	//HBITMAP hBitMap=CreateCompatibleBitmap(hdcTemp, m_nBkWidth, m_nBkHeight);
	HBITMAP hBitMap=KCreateCompatibleBitmap(m_rcCanvasWnd.Width(), m_rcCanvasWnd.Height());
	HBITMAP hOldBitMap = (HBITMAP)SelectObject(m_hdcMemory,hBitMap);

	POINT ptWinPos={m_rcCanvasWnd.left, m_rcCanvasWnd.top};

	Gdiplus::Graphics graph(m_hdcMemory);

	if (m_bPointInSvg == TRUE)
	{
		for (std::set<CDragRect, CDragRectSort>::iterator it = m_setPointDstImage.begin(); it != m_setPointDstImage.end(); ++it)
		{
			//不在窗口区域中不画
			Gdiplus::RectF modelWndRectTemp = Gdiplus::RectF(
				(Gdiplus::REAL)(TOOLBAR_IN_SVG_LEFT) + 0.0f, 
				(Gdiplus::REAL)(TOOLBAR_IN_SVG_TOP) + 0.0f,
				(Gdiplus::REAL)m_rcModelingWnd.Width() - (Gdiplus::REAL)(TOOLBAR_IN_SVG_LEFT),
				(Gdiplus::REAL)m_rcModelingWnd.Height() - (Gdiplus::REAL)(TOOLBAR_IN_SVG_TOP));
			if (!modelWndRectTemp.Intersect(it->m_rect))
			{
				continue;
			}

			Gdiplus::RectF dstRc(
				it->m_rect.X + m_rcModelingWnd.left, 
				it->m_rect.Y + m_rcModelingWnd.top, 
				it->m_rect.Width, 
				it->m_rect.Height);
			if (m_sDragType.Compare("form") == 0)
			{
				DrawMyImage(graph, m_pDstFormImage, it->m_rect);
			}
			else
			{
				DrawMyImage(graph, m_pDstImage, it->m_rect);
			}
		}

		if (!m_rectHighlightImage.m_rect.IsEmptyArea())
		{
			//不在窗口区域中不画
			Gdiplus::RectF modelWndRectTemp = Gdiplus::RectF(
				(Gdiplus::REAL)(TOOLBAR_IN_SVG_LEFT) + 0.0f, 
				(Gdiplus::REAL)(TOOLBAR_IN_SVG_TOP) + 0.0f,
				(Gdiplus::REAL)m_rcModelingWnd.Width() - (Gdiplus::REAL)(TOOLBAR_IN_SVG_LEFT),
				(Gdiplus::REAL)m_rcModelingWnd.Height() - (Gdiplus::REAL)(TOOLBAR_IN_SVG_TOP));
			if (modelWndRectTemp.Intersect(m_rectHighlightImage.m_rect))
			{
				if (m_sDragType.Compare("form") == 0)
				{
					DrawMyImage(graph, m_pHighlightFormImage, m_rectHighlightImage.m_rect);
				}
				else
				{
					DrawMyImage(graph, m_pHighlightImage, m_rectHighlightImage.m_rect);
				}
			}
		}
	}

	if (m_pDragToolImage)
	{
		if (m_pointDragToolImage.x != -1 && m_pointDragToolImage.y != -1)
		{
			int nImageX = m_pointDragToolImage.x + 5;
			int nImageY = m_pointDragToolImage.y + 5;
			Gdiplus::Rect rc(nImageX, nImageY, m_pDragToolImage->GetWidth(), m_pDragToolImage->GetHeight());
			graph.DrawImage(m_pDragToolImage, rc, 0, 0, m_pDragToolImage->GetWidth(), m_pDragToolImage->GetHeight(), Gdiplus::UnitPixel);
		}
	}

	SIZE sizeWindow={m_rcCanvasWnd.Width(), m_rcCanvasWnd.Height()};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);

	BOOL bRet=FALSE;
	bRet= ::UpdateLayeredWindow( m_hWnd, hdcScreen, &ptWinPos,
		&sizeWindow, m_hdcMemory, &ptSrc, 0, &m_Blend, 2);
	graph.ReleaseHDC(m_hdcMemory);

	SelectObject(m_hdcMemory, hOldBitMap);
	DeleteObject(hBitMap);
	DeleteDC(m_hdcMemory);
	m_hdcMemory=NULL;
	::ReleaseDC(m_hWnd,hdcScreen);
	hdcScreen=NULL;
	return bRet;
}

HBITMAP CSvgDragLayer::KCreateCompatibleBitmap( int nWidth, int nHeight )
{
	BYTE                      * pBits ;   
	BITMAPINFOHEADER          bmih;   
	ZeroMemory( &bmih, sizeof(BITMAPINFOHEADER));   

	bmih.biSize                 = sizeof (BITMAPINFOHEADER) ;   
	bmih.biWidth                = nWidth;   
	bmih.biHeight               = nHeight;   
	bmih.biPlanes               = 1 ;   
	bmih.biBitCount             = 32;        //这里一定要是32   
	bmih.biCompression          = BI_RGB ;   
	bmih.biSizeImage            = 0 ;   
	bmih.biXPelsPerMeter        = 0 ;   
	bmih.biYPelsPerMeter        = 0 ;   
	bmih.biClrUsed              = 0 ;   
	bmih.biClrImportant          = 0 ;   

	HBITMAP hBitMap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, 0, (VOID**)&pBits, NULL, 0) ;  
	return hBitMap;
}

void CSvgDragLayer::InitSize( CRect rcCanvasWnd, CRect rcModelingWnd )
{
	m_rcCanvasWnd = rcCanvasWnd;
	m_rcModelingWnd = rcModelingWnd;
	m_rcModelingWnd.OffsetRect(0 - rcCanvasWnd.left, 0 - rcCanvasWnd.top);
}

void CSvgDragLayer::StartDrag( CResLibData* pResLibDataBase )
{
	ShowWindow(SW_SHOW);
	SetCapture();	
	CWnd* pCurrentWnd = theViewMgr.GetViewTab(g_lpszViewTitleModel)->GetActiveWnd();

	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
	CWnd* pJianmoWnd = (CWnd*)pView->m_pChildFrame;

	if (pCurrentWnd != pJianmoWnd)
	{
		EndDrag();
		return;
	}

	if (!InitDragData(pResLibDataBase))
	{
		EndDrag();
		return;
	}
}

void CSvgDragLayer::EndDrag( BOOL bClearSvgDraw /*= TRUE*/ )
{
	m_sDragType.Empty();
	if (m_pDragToolImage)
	{
		delete m_pDragToolImage;
		m_pDragToolImage = NULL;
	}
	ClearDstRects();
	UpdateBk();
	ShowWindow(SW_HIDE);
	ReleaseCapture();

	if (bClearSvgDraw == TRUE)
	{
		PutJsonToSvg("", "", "");
	}
}

void CSvgDragLayer::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	// TODO: 在此处添加消息处理程序代码
	if (nState == WA_INACTIVE)
	{
		EndDrag();
	}
}

BOOL CSvgDragLayer::InitDragData( CResLibData* pDataBase )
{
	m_pDataBase = pDataBase;

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CDlgSvg * pDlgSvg = GetSvgDilogPtr();
	if ( pDlgSvg == NULL )
	{
		return FALSE;
	}

	CRect rcMain;
	::GetWindowRect(pMainWnd->GetSafeHwnd(), &rcMain);
	CRect rc;
	::GetWindowRect(pDlgSvg->GetSafeHwnd(), &rc);
	InitSize(rcMain, rc);

	//先只联调已有组件的情况
	CResLibProcessData * pProcess = NULL;
	CResLibRobotData * pRobot = NULL;
	CResLibBaseComponentData * pBaseComponent = NULL;
	CCxStaticRes * pRobotImage = NULL;
	switch (m_pDataBase->m_nResType)
	{
	case CResLibData::RT_ROBOT:
		pRobot = dynamic_cast<CResLibRobotData *>(m_pDataBase);
		if ( pRobot == NULL )
		{
			return FALSE;
		}

		pRobotImage = GetRobotImageData( pRobot->m_RobotType.c_str(), 42 );
		if ( pRobotImage == NULL 
			|| !IsAccessibleFile( pRobotImage->m_strPath ) )
		{
			return FALSE;
		}

		m_sDragType = "component";

		break;
	case CResLibData::RT_PROCESS:
		pProcess = dynamic_cast<CResLibProcessData *>(m_pDataBase);

		if ( pProcess == NULL )
		{
			return FALSE;
		}

		pRobotImage = GetStaticResImageData( "StaticResId_ResLib_Flow_42" );
		if ( pRobotImage == NULL 
			|| !IsAccessibleFile( pRobotImage->m_strPath ) )
		{
			return FALSE;
		}
		
		m_sDragType = "process";
		break;
	case CResLibData::RT_BASECOMPONET:
		pBaseComponent = dynamic_cast<CResLibBaseComponentData *>(m_pDataBase);
		if ( pBaseComponent == NULL )
		{
			return FALSE;
		}

		pRobotImage = GetRobotImageData( pBaseComponent->m_RobotType.c_str(), 42 );
		if ( pRobotImage == NULL 
			|| !IsAccessibleFile( pRobotImage->m_strPath ) )
		{
			return FALSE;
		}
		
		if (pBaseComponent->m_RobotType == "E712BA3D-34AD-4286-AAC5-D800BAF60415")
		//if (pBaseComponent->m_RobotType == "E5FE966F-BD57-41D9-9C19-97F4337B5D51")
		{
			m_sDragType = "form";
		}
		else
		{
			m_sDragType = "baseComponent";
		}
		break;
	default:
		break;
	}

	if (m_sDragType.IsEmpty())
	{
		return FALSE;
	}

	if (pRobotImage == NULL || pRobotImage->m_pImgNormal == NULL)
	{
		return FALSE;
	}
	else
	{
		SetDragToolImage(pRobotImage->m_pImgNormal);
	}	
	
	return InitDstRect();
}

BOOL CSvgDragLayer::InitDstRect()
{
	ClearDstRects();

	if (m_pDstImage)
	{
		delete m_pDstImage;
		m_pDstImage = NULL;
	}
	if (m_pDstFormImage)
	{
		delete m_pDstFormImage;
		m_pDstFormImage = NULL;
	}
	if (m_pHighlightImage)
	{
		delete m_pHighlightImage;
		m_pHighlightImage = NULL;
	}
	if (m_pHighlightFormImage)
	{
		delete m_pHighlightFormImage;
		m_pHighlightFormImage = NULL;
	}

	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CDlgSvg * pDlgSvg = GetSvgDilogPtr();
	if ( pDlgSvg == NULL ) return FALSE;

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//方法名称
	wmd.m_sMethodName = "dragBegin";
	pParam = new CWebMethodParamData();
	pParam->m_sName = "type";
	pParam->m_sValue = m_sDragType;
	wmd.m_vParams.push_back(pParam);
	pDlgSvg->InvokeJsMethod( &wmd );
	CString sResult = wmd.m_sResult;

	Json::Value jRectArray;
	Json::Reader reader;
	if (reader.parse((LPCTSTR)sResult, jRectArray))
	{
		int nLength = jRectArray.size();
		for (int i = 0; i < nLength; i++)
		{
			Json::Value jRect = jRectArray[Json::Value::ArrayIndex(i)];
			AddDstRect(
				CDragRect(
				RectF(
				(Gdiplus::REAL)jRect["x"].asDouble(), 
				(Gdiplus::REAL)jRect["y"].asDouble(), 
				(Gdiplus::REAL)jRect["width"].asDouble(), 
				(Gdiplus::REAL)jRect["height"].asDouble()), 
				jRect["id"].asCString()
				)
				);
		}
	}	

	m_pDstImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_DST)->m_pImgNormal->Clone();
	ASSERT(m_pDstImage);

	m_pDstFormImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_DSTFORM)->m_pImgNormal->GetThumbnailImage(
		GetStaticResImageData(CCxStaticRes::RT_DRAG_DSTFORM)->m_pImgNormal->GetWidth()*4,
		GetStaticResImageData(CCxStaticRes::RT_DRAG_DSTFORM)->m_pImgNormal->GetHeight()*4
		);
	ASSERT(m_pDstFormImage);

	m_pHighlightImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_HIGHLIGHT)->m_pImgNormal->Clone();
	ASSERT(m_pHighlightImage);
	
	m_pHighlightFormImage = GetStaticResImageData(CCxStaticRes::RT_DRAG_HIGHLIGHT_FROM)->m_pImgNormal->GetThumbnailImage(
		GetStaticResImageData(CCxStaticRes::RT_DRAG_HIGHLIGHT_FROM)->m_pImgNormal->GetWidth()*4,
		GetStaticResImageData(CCxStaticRes::RT_DRAG_HIGHLIGHT_FROM)->m_pImgNormal->GetHeight()*4
		);
	ASSERT(m_pHighlightFormImage);

	return TRUE;
}

void CSvgDragLayer::PutJsonToSvg(CString sId, CString sType, CString sJson)
{
	CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	CDlgSvg * pDlgSvg = GetSvgDilogPtr();
	if ( pDlgSvg == NULL ) return;

	CWebMethodData wmd;
	CWebMethodParamData * pParam = NULL;
	//方法名称
	wmd.m_sMethodName = "dragEnd";
	pParam = new CWebMethodParamData();
	pParam->m_sName = "id";
	pParam->m_sValue = sId;
	wmd.m_vParams.push_back(pParam);
	pParam = new CWebMethodParamData();
	pParam->m_sName = "type";
	pParam->m_sValue = sType;
	wmd.m_vParams.push_back(pParam);
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonData";
	pParam->m_sValue = sJson;
	wmd.m_vParams.push_back(pParam);
	pDlgSvg->InvokeJsMethod( &wmd );

	/*
	CString ssj(strJson.c_str());
	ssj.Replace("\"", "\\\"");
	CString s;
	s.Format("javascript:dragEnd(\"%s\",\"%s\",\"%s\");", m_rectHighlightImage.m_sId, sDragType, ssj);
	MessageBox(s);
	pDlgSvg->Navigate(s);
	*/
}
