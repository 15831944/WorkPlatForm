// CxResLibImgList.cpp : 实现文件
//

#include "stdafx.h"
#include "CxResLibImgList.h"
#include <commctrl.h>
#include "FileFunc.h"
#include "PtrVectorHeader.h"
#include "MainFrm.h"
#include "ViewSet.h"
#include "DlgSvg.h"
#include "PeraProcessDesigner.h"
#include "UtilityFunc.h"
#include "Tools.h"
#include "ConvertData.h"
#include "PeraProcessDesignerUtility.h"

//rcIcon区域下方留有4像素空白区域
static const int CY_ICON_BOTTOM_SPACE = 4;
//rcIcon、rcLabel之间留有2像素空白区域
static const int CY_ICON_LABEL_SPACE = 2;
//文字区域最小高度
static const int CY_LABEL_TEXT = 19;

// CxResLibImgList

//ListCtrl默认背景色
const Color CxResLibImgList::COLOR_LISTCTRL_BG = Color::White;
//默认图片背景色
const Color CxResLibImgList::COLOR_IMAGE_BG = Color::White;
//Y轴方向，图片最小间距
const int CxResLibImgList::MIN_CY_SPACE_ITEM = CY_LABEL_TEXT + CY_ICON_LABEL_SPACE + CY_ICON_BOTTOM_SPACE;

BEGIN_MESSAGE_MAP(CxResLibImgList, CxListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CxResLibImgList::OnNMCustomdraw)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeaved)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_RESLIB_LIST_DELETE, &CxResLibImgList::OnCxResLibImgListDelete)
	ON_COMMAND(ID_RESLIB_LIST_UPDATE_PROP, &CxResLibImgList::OnUpdateProp)
	ON_COMMAND(ID_RESLIB_LIST_DOWNLOAD, &CxResLibImgList::OnDownload)
	
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CxResLibImgList, CxListCtrl)

CxResLibImgList::CxResLibImgList()
{
	m_bAutoDeleteData = TRUE;

	m_pGraphics = NULL;
	m_pBmpCanvas = NULL;
	m_pImgFocused = NULL;
	m_pImgSelected = NULL;

	m_crCanvasBk = COLOR_LISTCTRL_BG;

	m_pFont = NULL;
	m_FontColor = Color::Black;

	m_nImgCx = 0;
	m_nImgCy = 0;
	m_nItemCx = 0;
	m_nItemCy = 0;
	m_nItemCxSpace = 0;
	m_nItemCySpace = 0;
	m_crImgBk = COLOR_IMAGE_BG;

	m_nLastFocusItem = -1;

	m_bDragLayer = FALSE;

	m_bMouseTracked = FALSE;
	m_bSetCapture = FALSE;
}


CxResLibImgList::~CxResLibImgList()
{
}


void CxResLibImgList::Release( void )
{
	Clear();
	if (m_pImgFocused != NULL) { delete m_pImgFocused; m_pImgFocused = NULL; }
	if (m_pImgSelected != NULL) { delete m_pImgSelected; m_pImgSelected = NULL; }
	if (m_pGraphics != NULL) { delete m_pGraphics; m_pGraphics = NULL; }
	if (m_pBmpCanvas != NULL) { delete m_pBmpCanvas; m_pBmpCanvas = NULL; }
	if (m_pFont != NULL) { delete m_pFont; m_pFont = NULL; }
	if ( NULL != m_ImageList.GetSafeHandle() ) m_ImageList.DeleteImageList();
	m_bAutoDeleteData = TRUE;
	CxListCtrl::Release();
}

void CxResLibImgList::Clear( void )
{
	for (int i=GetItemCount() - 1; i>=0; i--)
	{
		DeleteItem(i);
	}

	if (m_bAutoDeleteData)
		PtrVectorClear(m_vImg);

	m_nLastFocusItem = -1;
}


BOOL CxResLibImgList::OnInit( void )
{
	//
	SetAutoDelete(TRUE);
	//图片模式
	if ( !SetImageMode() ) return FALSE;
	//背景色
//	m_crCanvasBk = Color::LightGreen;
	//图片尺寸、间距、透明色
	InitImageLayout(90, 100, 1, 1, 42, 42);
	SetFont(NULL);

	//
	COLORREF clr = RGB(m_crCanvasBk.GetR(), m_crCanvasBk.GetG(), m_crCanvasBk.GetB());
	SetBkColor(clr);

	OnLoadStateImage();
	OnLoadTextFont();

	ModifyStyle(DS_3DLOOK, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	m_ToolTip.Create (this);

	m_RMenu.LoadMenu(IDR_RESLIB_LIST_POP);
	CMenu * pPopup = m_RMenu.GetSubMenu(0);
	m_wndDummy.Create(WS_CHILD, CRect(0,0,0,0),this, IDC_RESLIBIMGLIST_DUMMY);
	return TRUE;
}


/*
仅图标模式有效。

初始化的时候，如果 SetFont(NULL); CY_LABEL_TEXT = 19。
如果 不写 SetFont(NULL); 这句话，CY_LABEL_TEXT = 16。
nSpaceY 小于 MIN_CY_IMAGESPACE 的话，会导致绘制后布局不正确。
另外，如果文字过长，覆盖到下一行图片的区域的话，也会导致刷新不正确。
*/
BOOL CxResLibImgList::InitImageLayout( int nItemCx, int nItemCy
	, int nSpaceX, int nSpaceY, int nImgCx, int nImgCy, Color crImgBk )
{
	//if ( nItemCx < 1 || nItemCy < MIN_CY_SPACE_IMAGE || nSpaceX < 0 ) return FALSE;
	//if (MIN_CY_SPACE_IMAGE > nSpaceY) return FALSE;

	Clear();
	DeleteAllItems();

	m_nItemCx = nItemCx;
	m_nItemCy = nItemCy;
	m_nItemCxSpace = nSpaceX;
	m_nItemCySpace = nSpaceY;
	m_nImgCx = nImgCx;
	m_nImgCy = nImgCy;

	/*
	The cx and cy parameters are relative to the upper-left corner of an icon bitmap.
	Therefore, to set spacing between icons that do not overlap,
	the cx or cy values must include the size of the icon,
	plus the amount of empty space desired between icons.
	Values that do not include the width of the icon will result in overlaps.
	*/
	CSize szSpacing = SetIconSpacing(m_nItemCx + m_nItemCxSpace, m_nItemCy + m_nItemCySpace);

	if ( NULL != m_ImageList.GetSafeHandle() )
		m_ImageList.DeleteImageList();
	if (!m_ImageList.Create(m_nItemCx, m_nItemCy - MIN_CY_SPACE_ITEM, ILC_COLOR32, 1, 1))
	{
		return FALSE;
	}

	//
	COLORREF clr = RGB(m_crImgBk.GetR(), m_crImgBk.GetG(), m_crImgBk.GetB());
	m_ImageList.SetBkColor(clr);
	SetImageList( &m_ImageList, LVSIL_NORMAL );

	return TRUE;
}


//加载状态、焦点图片
void CxResLibImgList::OnLoadStateImage( void )
{
	if (m_pImgSelected != NULL) delete m_pImgSelected; m_pImgSelected = NULL;
	if (m_pImgFocused != NULL) delete m_pImgFocused; m_pImgFocused = NULL;

	Bitmap * pBitmap = GetNewSudokuBitmap(GetExecDir() + "\\Res\\CoreImgs\\CxListCtrlImgSelected.png"
		, m_nItemCx, m_nItemCy );
	if (pBitmap != NULL)
	{
		m_pImgSelected = reinterpret_cast< Image * > ( pBitmap );
	}

	pBitmap = GetNewSudokuBitmap(GetExecDir() + "\\Res\\CoreImgs\\CxListCtrlImgFocused.png"
		, m_nItemCx, m_nItemCy );
	if (pBitmap != NULL)
	{
		m_pImgFocused = reinterpret_cast< Image * > ( pBitmap );
	}
}


//加载字体信息
void CxResLibImgList::OnLoadTextFont()
{
	if (m_pFont != NULL) { delete m_pFont; m_pFont = NULL; }

	CStringW strW;
	strW = m_strFontFamilyName;
	if (strW.IsEmpty()) strW = L"宋体";
	FontFamily family((WCHAR *)(LPCWSTR)strW);
	m_pFont = new Gdiplus::Font(&family, 12, FontStyleRegular, UnitPixel);

	//m_FontFormat.SetAlignment(StringAlignmentNear);
	//m_FontFormat.SetLineAlignment(StringAlignmentFar);
	m_FontFormat.SetAlignment(StringAlignmentCenter); //水平方向
	//m_FontFormat.SetLineAlignment(StringAlignmentCenter); //垂直方向

	m_FontColor = Color::Black;
}


/*
这个值，用于控制图片和文字之间的距离。
*/
int CxResLibImgList::GetSpaceOfIconLabel( void )
{
	return 5;
}


//
void CxResLibImgList::OnEraseBkgndCustom( void )
{
}


//焦点改变
void CxResLibImgList::OnFocusChanged( int nItem, BOOL bFocus )
{
	if ( nItem == -1 ) return;
	SetItemState(nItem, 0, LVIS_FOCUSED);
	if (bFocus) SetItemState(nItem, LVIS_FOCUSED, LVIS_FOCUSED);
	RedrawItems(nItem, nItem);

	if (bFocus)
	{
		CResLibData * pBase = reinterpret_cast< CResLibData * > ( GetItemData( nItem ) );
		if (pBase == NULL)
		{
			return;
		}

		//CRect rc;
		//this->GetItemRect(nItem, &rc, LVIR_BOUNDS);
		//CPoint pt = rc.TopLeft();
		//::ClientToScreen(this->GetSafeHwnd(), &pt);
		//pt.Offset(rc.Width()/2, rc.Height()/2);
		//rc.MoveToXY(pt);

		//CString strTip;
		//strTip.Format( "名称：%s\r\n作者：%s\r\n时间：%s\r\n版本：%s\r\n描述：%s"
		//	, pBase->m_sName.c_str()
		//	, pBase->m_sAuthor.c_str()
		//	, pBase->m_sCreatTime.c_str()
		//	, pBase->m_sVersions.c_str()
		//	, pBase->m_sDsec.c_str()
		//	);

		//m_ToolTip.SetTextMargin (TEXT_MARGIN);
		//m_ToolTip.SetFont ( &theApp.m_font );
		//m_ToolTip.Track (rc, strTip);
	}
	else
	{
		m_ToolTip.Deactivate();
		Invalidate();

		//CMenu * pPopup = m_RMenu.GetSubMenu(0);
		//MENUINFO mi;
		//memset(&mi, NULL, sizeof( MENUINFO ));
		//mi.cbSize = sizeof(mi);
		//m_RMenu.GetMenuInfo(&mi);
	}

	m_bMouseTracked = FALSE;
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd() );
}



//背景擦除
BOOL CxResLibImgList::OnEraseBkgnd(CDC* pDC)
{
	//不执行默认的擦除操作，如果执行了，会导致闪烁。
	//原因是，每次图片更新前，控件会自动擦除图片对应的Item区域。
	//BOOL bOk = CxListCtrl::OnEraseBkgnd(pDC);
	OnEraseBkgndCustom();


	//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnEraseBkgnd %s", "\r\n" );

	return TRUE;
}


/*
通知自绘的消息入口。
流程：
1、CDDS_PREPAINT 开始自绘，如果 返回 CDRF_DODEFAULT，则 CListCtrl 自行绘制。
	如果不存在内存dc，bmp，则创建之，用于绘制。
	或者在 OnEraseBkgndCustom 中创建。

2、CDDS_ITEMPREPAINT，绘制每个 Item。

3、CDDS_POSTPAINT，绘制结束。
	将内存dc，bmp中的数据拷贝到实际dc中，并删除内存dc，bmp。
*/
void CxResLibImgList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
static const DWORD CDDS_SUBITEMPREPAINT = (DWORD)(CDDS_ITEMPREPAINT | CDDS_SUBITEM);
static const DWORD CDDS_SUBITEMPOSTPAINT = (CDDS_ITEMPOSTPAINT | CDDS_SUBITEM);

	static int nDrawCount = 0;
	LPNMLVCUSTOMDRAW lplvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//if (lplvcd == NULL) return;

	switch(lplvcd->nmcd.dwDrawStage)
	{
	//整个控件的绘制循环开始前。
	//如果此处返回 CDRF_DODEFAULT，控件将自己绘制自己，不会再收到任何通知。
	case CDDS_PREPAINT:
		{
			if (m_pBmpCanvas != NULL || m_pGraphics != NULL)
			{
				delete m_pGraphics; m_pGraphics = NULL;
				delete m_pBmpCanvas; m_pBmpCanvas = NULL;
			}

			if (m_pBmpCanvas == NULL && m_pGraphics == NULL)
			{
				CRect rc;
				GetClientRect(&rc);
				m_pBmpCanvas = new Bitmap(rc.Width(), rc.Height(), PixelFormat32bppARGB);
				m_pGraphics = new Graphics(m_pBmpCanvas);
				//设定透明模式，和点阵模式，如果不设定会出现毛边，造成图形边界不是很平滑
				m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
				m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);
				//m_pGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
				//擦除背景
				RectF rcf((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
				SolidBrush br(m_crCanvasBk);
				m_pGraphics->FillRectangle(&br, rcf);

				nDrawCount = 0;

				//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnNMCustomdraw %s", "创建Canvas\r\n" );
			}
		}
		
		*pResult = CDRF_NOTIFYITEMDRAW //Item 绘制前后，通知
			| CDRF_NOTIFYPOSTPAINT //整个控件绘制完成后，通知
			| CDRF_NOTIFYPOSTERASE //擦除之后，通知。完全不起作用，个人理解，可以通过实现 OnEraseBkgnd 替代。
			| CDRF_NOTIFYSUBITEMDRAW //子Item 绘制前后，通知
			; 
		break;

	case  CDDS_POSTPAINT: //整个控件的绘制循环完成。
		{
			if (m_pBmpCanvas != NULL && m_pGraphics != NULL )
			{
				BOOL bDraw = TRUE;
				if ( m_vImg.size() > 1 && nDrawCount <= 1 )
				{
					bDraw = FALSE;
				}
				else
				{
					//xp下，当有其他窗口档在最后一个组件上时，右侧区域不被绘制
					DWORD dwBakSpec = lplvcd->nmcd.dwItemSpec;
					for (int i=0; i<(int)m_vImg.size(); i++)
					{
						lplvcd->nmcd.dwItemSpec = i;
						OnItemPaint(lplvcd);
					}
					lplvcd->nmcd.dwItemSpec = dwBakSpec;
				}
				//if ( bDraw )
				{
					Graphics g(lplvcd->nmcd.hdc);
					CachedBitmap cachedBmp(m_pBmpCanvas, &g);
					g.DrawCachedBitmap(&cachedBmp, 0, 0);
					g.ReleaseHDC(lplvcd->nmcd.hdc);
				}

				delete m_pGraphics; m_pGraphics = NULL;
				delete m_pBmpCanvas; m_pBmpCanvas = NULL;

				nDrawCount = 0;

				//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnNMCustomdraw %s", "拷贝Canvas到DC\r\n" );
			}
		}

		*pResult = CDRF_DODEFAULT;
		break;

	case CDDS_PREERASE:      // Before the erase cycle begins
	case CDDS_POSTERASE:     // After the erase cycle is complete
	case CDDS_ITEMPREERASE:  // Before an item is erased
	case CDDS_ITEMPOSTERASE: // After an item has been erased
		// these are not handled now, but you might like to do so in the future
		*pResult = CDRF_DODEFAULT;
		break;

	//Before an item is drawn.
	case CDDS_ITEMPREPAINT: // Item 绘制前
		if ( !OnItemPaint(lplvcd) )
			*pResult = CDRF_DODEFAULT; //失败
		else
		{
			//成功
			nDrawCount++;
			*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT;
		}
		//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnNMCustomdraw %s", "OnItemPaint\r\n" );
		break;

	//After an item is drawn.
	case CDDS_ITEMPOSTPAINT: //Item 绘制完成后
		//*pResult = CDRF_DODEFAULT;
		//*pResult = CDRF_SKIPDEFAULT;
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	
	//Before an sub-item is drawn.
	case CDDS_SUBITEMPREPAINT: //SubItem 绘制前
		*pResult = CDRF_DODEFAULT;
		break;
	////after an sub-item is drawn.
	case CDDS_SUBITEMPOSTPAINT: //SubItem 绘制完成后
		*pResult = CDRF_DODEFAULT;
		break;
	}

	//*pResult = 0; 通过返回值控制绘制的各个阶段
}


//绘制图片
BOOL CxResLibImgList::OnItemPaint(LPNMLVCUSTOMDRAW lplvcd)
{
	LONG x = 0;

	int nItem = ( int ) ( lplvcd->nmcd.dwItemSpec );
	UINT nState = GetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED);

	RECT rcBound, rcIcon, rcLabel;
	GetItemRect(nItem, &rcBound, LVIR_BOUNDS);
	GetItemRect(nItem, &rcIcon, LVIR_ICON);
	GetItemRect(nItem, &rcLabel, LVIR_LABEL);

	UINT nSelCx = m_pImgSelected->GetHeight();
	UINT nSelCy = m_pImgSelected->GetWidth();

	rcBound.bottom = rcBound.top + nSelCy;
	rcLabel.bottom = rcBound.bottom;

	//调整 rcBound 尺寸
	int nCyMinBound = CY_ICON_BOTTOM_SPACE + CY_ICON_LABEL_SPACE + CY_LABEL_TEXT;
	//int nReal = m_nImgCySpace;
	//if (nReal < nCyMinBound)
	//{
	//	rcBound.bottom -= (nCyMinBound - nReal);
	//	if (rcLabel.bottom > rcBound.bottom)
	//		rcLabel.bottom = rcBound.bottom;
	//}
	//右边界
	//rcBound.right = rcBound.left + rcIcon.right - rcIcon.left 
	//	+ (rcIcon.left - rcBound.left) * 2;

	//rcBound.left += 20;
	//rcBound.right -= 20;
	//rcBound.top += 10;
	//rcBound.bottom -= 10;

	//Icon区域 在 Bound区域 居中
	x = ( ( rcBound.right - rcBound.left ) - m_nImgCx ) / 2;

	rcIcon.top = rcBound.top + 11;
	rcIcon.bottom = rcIcon.top + m_nImgCy;
	rcIcon.left = rcBound.left + x;
	rcIcon.right = rcIcon.left + m_nImgCx;

	//调整 rcLabel 位置和尺寸
	int nIconLabelSpaceOffset = GetSpaceOfIconLabel();
	int nLabelHeight = rcLabel.bottom - rcLabel.top;
	//调整 Icon 与 Label 之间的间距
	rcLabel.top = rcIcon.bottom + nIconLabelSpaceOffset;
	//rcLabel.bottom = rcLabel.top + nLabelHeight;
	rcLabel.bottom = rcBound.bottom - 6;
	//调整Lable宽度
	rcLabel.left = rcBound.left - 2;
	rcLabel.right = rcBound.right - 2;
	rcLabel.bottom+=2;
	//边界平行四边形点
	Point BoundPoints[] = {
		Point( rcBound.left, rcBound.top ) //左上
		, Point( rcBound.right, rcBound.top ) //右上
		, Point( rcBound.left, rcBound.bottom ) //左下
	};

	//图标平行四边形点
	Point IconPoints[] = {
		Point( rcIcon.left, rcIcon.top ) //左上
		, Point( rcIcon.right, rcIcon.top ) //右上
		, Point( rcIcon.left, rcIcon.bottom ) //左下
	};

	//绘制
	if (nState & LVIS_FOCUSED) //有焦点
	{
		if (nState & LVIS_SELECTED) //焦点 + 选中
		{
			RectF rcBoundF( (float)rcBound.left, (float)rcBound.top
				, (float)(rcBound.right - rcBound.left)
				, (float)(rcBound.bottom - rcBound.top) );

			if (m_pImgSelected != NULL)
			{
				m_pGraphics->DrawImage( m_pImgSelected, BoundPoints, 3 );
				//DrawHighlight(m_pImgSelected, m_pGraphics, rcBoundF, 1.5f);
			}
		}

		//焦点 + 未选中
		{
			RectF rcIconF( (float)rcIcon.left, (float)rcIcon.top
				, (float)(rcIcon.right - rcIcon.left), (float)(rcIcon.bottom - rcIcon.top) );

			m_pGraphics->DrawImage( m_vImg[nItem], IconPoints, 3 );

			//m_pGraphics->DrawImage( m_vImg[nItem], IconPoints, 3 );
			//m_pGraphics->DrawImage( m_pImgFocused, BoundPoints, 3 );
			////DrawHighlight(m_vImg[nItem], m_pGraphics, rcIconF);
		}
	}
	else //没有焦点
	{
		if (nState & LVIS_SELECTED) //无焦点 + 选中
		{
			if (m_pImgSelected != NULL)
			{
				m_pGraphics->DrawImage( m_pImgSelected, BoundPoints, 3 );
			}
		}

		//无焦点 + 未选中
		{
			m_pGraphics->DrawImage( m_vImg[nItem], IconPoints, 3 );
		}
	}

	//绘制文字
// 	if (m_nImgCx <= 16 || m_nImgCy <= 16 
// 		|| rcLabel.bottom - rcLabel.top < 16)
// 	{
// 
// 	}
// 	else
	{
		DrawItemText(nItem, rcLabel);
	}
	return TRUE;
}


//绘制文字
void CxResLibImgList::DrawItemText( int nItem, RECT& rcLabel )
{
	CString strLabel = GetItemText(nItem, 0);

	SolidBrush brushText(m_FontColor);
	CStringW strLabelW;
	strLabelW = strLabel;

	if ( strLabelW.IsEmpty() ) return;

	RectF rcLabelF((float)rcLabel.left, (float)rcLabel.top, (float)rcLabel.right - (float)rcLabel.left
		, (float)rcLabel.bottom - (float)rcLabel.top);

	int nFirstLineCharCount = 1;
	CStringW strLineW;
	RectF rcfText;
	PointF ptfText;
	while ( nFirstLineCharCount < strLabelW.GetLength() )
	{
		strLineW = strLabelW.Mid(0, nFirstLineCharCount);
		m_pGraphics->MeasureString((WCHAR*)(LPCWSTR)strLabelW
			, nFirstLineCharCount
			, m_pFont
			, ptfText
			, &rcfText
			);

		if ( (int)rcLabelF.Width < 1 + rcfText.Width )
		{
			nFirstLineCharCount--;
			if ( nFirstLineCharCount < 0 ) nFirstLineCharCount = 0;
			break;
		}

		nFirstLineCharCount++;
	}

	if ( nFirstLineCharCount != strLabelW.GetLength() && nFirstLineCharCount > 0 )
	{
		//第一行
		RectF rcLine = rcLabelF;
		rcLine.Height = rcfText.Height;
		strLineW = strLabelW.Mid(0, nFirstLineCharCount);

		m_pGraphics->DrawString((LPCWSTR)strLineW
			, -1
			, m_pFont
			, rcLine
			, &m_FontFormat
			, &brushText);

		//第2行
		rcLine.Offset(0, rcLine.Height);
		strLineW = strLabelW.Mid(nFirstLineCharCount, strLabelW.GetLength() - nFirstLineCharCount);

		if ( strLineW.GetLength() > 6 )
		{
			CStringW str = strLineW.Mid( 0, 5 );
			strLineW = str + L"..";
		}

		m_pGraphics->DrawString((LPCWSTR)strLineW
			, -1
			, m_pFont
			, rcLine
			, &m_FontFormat
			, &brushText);
	}
	else
	{
		m_pGraphics->DrawString((LPCWSTR)strLabelW
			, -1
			, m_pFont
			, rcLabelF
			, &m_FontFormat
			, &brushText);
	}
}


//添加图片
BOOL CxResLibImgList::AddImage( LPCTSTR lpszImgPath, LPCTSTR lpszLabel, LPARAM lp, int nPos )
{
	if (!m_bAutoDeleteData) return FALSE;
	CString strImgPath = lpszImgPath == NULL ? "" : lpszImgPath;
	Bitmap * pBitmap = GetNewBitmap(strImgPath);
	if (pBitmap == NULL) return FALSE;
	if ( NULL == ScaleBitmap(pBitmap, m_nImgCx, m_nImgCy, FALSE) ) return FALSE;
	_AddImage(pBitmap, lpszLabel, lp, nPos);
	return TRUE;
}


//添加图片
BOOL CxResLibImgList::AddImage( Bitmap * pBitmap, LPCTSTR lpszLabel, LPARAM lp, int nPos )
{
	if (m_bAutoDeleteData) return FALSE;
	if (pBitmap == NULL) return FALSE;
	_AddImage(pBitmap, lpszLabel, lp, nPos);
	return TRUE;
}


//添加图片
int CxResLibImgList::_AddImage( Bitmap * pBitmap, LPCTSTR lpszLabel, LPARAM lp, int nPos )
{
	CString strLabel = lpszLabel == NULL ? "" : lpszLabel;

	Image * pImg = reinterpret_cast< Image * > ( pBitmap );
	m_vImg.push_back(pImg);

	if (nPos < 0 || nPos > GetItemCount())
		nPos = GetItemCount();

	LVITEM lvi;
	memset(&lvi, NULL, sizeof(LVITEM));
	lvi.iItem = nPos;
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.pszText = (LPSTR)(LPCTSTR)strLabel;
	lvi.lParam = lp;
	int nItem = InsertItem(&lvi);

	//if ( nItem >= 0 )
	//{
	//	RedrawItems(nItem, nItem);
	//}

	return nItem;
}


//删除图片
BOOL CxResLibImgList::DeleteIamge( int nItemIndex )
{
	if (nItemIndex < 0 
		|| nItemIndex > (int)m_vImg.size()
		|| nItemIndex > GetItemCount() ) return FALSE;

	if (m_bAutoDeleteData)
	{
		vector<Image *>::iterator itr = find(m_vImg.begin(), m_vImg.end(), m_vImg[nItemIndex]);
		if (itr != m_vImg.end())
		{
			delete *itr;
			m_vImg.erase(itr);
		}
	}
	
	DeleteItem(nItemIndex);
	return TRUE;
}



void CxResLibImgList::DragLayer()
{
	if ( m_nLastFocusItem == -1 ) return;
	CResLibData * pBase = reinterpret_cast< CResLibData * > ( GetItemData( m_nLastFocusItem ) );
	if (pBase == NULL)
	{
		return;
	}

	//char drive[_MAX_DRIVE];
	//char dir[_MAX_DIR];
	//char fname[_MAX_FNAME];
	//char ext[_MAX_EXT];
	//char exeFullPath[MAX_PATH]; // MAX_PATH
	//GetModuleFileName(NULL,exeFullPath,MAX_PATH);//得到程序模块名称，全路径
	//_splitpath_s( exeFullPath, drive, dir, fname, ext );
	//CString sPicPath;
	//CStringW strW;

	//sPicPath.Format("%s%simg\\formula.png", drive, dir);
	//strW = sPicPath;
	//Gdiplus::Image* pToolImage = Gdiplus::Image::FromFile(strW);
//	ASSERT(pToolImage);

	if (theApp.m_pDlgDragLayer == NULL)
	{
		CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
		theApp.m_pDlgDragLayer = new CDlgDragLayer();
		theApp.m_pDlgDragLayer->Create(CDlgDragLayer::IDD, pMainWnd);
	}

	theApp.m_pDlgDragLayer->StartDrag(pBase);
/*	delete pToolImage;
	pToolImage = NULL;	*/	
}



LRESULT CxResLibImgList::OnMouseHover(WPARAM wp,LPARAM lp)
{
	if ( m_nLastFocusItem == -1 ) return 0;
	CResLibData * pBase = reinterpret_cast< CResLibData * > ( GetItemData( m_nLastFocusItem ) );
	if (pBase == NULL) return 0;

	if ( m_nLastFocusItem >= 0 )
	{
		CRect rc;
		this->GetItemRect(m_nLastFocusItem, &rc, LVIR_BOUNDS);

		UINT nSelCx = m_pImgSelected->GetHeight();
		UINT nSelCy = m_pImgSelected->GetWidth();

		rc.bottom = rc.top + nSelCy;

		CPoint pt = rc.TopLeft();
		::ClientToScreen(this->GetSafeHwnd(), &pt);
		//pt.Offset(rc.Width(), rc.Height());
		//rc.MoveToXY(pt);

		::GetCursorPos(&pt);
		pt.x += g_ToolTipOffset.x;
		pt.y += g_ToolTipOffset.y;
		rc.MoveToXY(pt);

		CString strTip;
		strTip.Format( "名称：%s\r\n作者：%s\r\n时间：%s\r\n版本：%s\r\n描述：%s"
			, pBase->m_sName.c_str()
			, pBase->m_sAuthor.c_str()
			, pBase->m_sCreatTime.c_str()
			, pBase->m_sVersions.c_str()
			, pBase->m_sDsec.c_str()
			);

		m_ToolTip.SetTextMargin (TEXT_MARGIN + 2);
		m_ToolTip.SetFont ( &theApp.m_font );
		m_ToolTip.Track (rc, strTip);
	}
	return 0;
}

LRESULT CxResLibImgList::OnMouseLeaved(WPARAM wp,LPARAM lp)
{
	m_ToolTip.Deactivate();
	Invalidate();
	m_bMouseTracked = FALSE;
	return 0;
}

//
void CxResLibImgList::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMouseTrack( m_bMouseTracked, GetSafeHwnd(), g_dwToolTipDelay );

	LVHITTESTINFO hti;
	memset(&hti, NULL, sizeof(LVHITTESTINFO));
	hti.flags = LVHT_ABOVE;

	GetCursorPos(&(hti.pt));
	ScreenToClient(&hti.pt);

	int nItem = -1;

	//检测 Item 是否选中
	//当图片文字区域过长时，ListCtrl返回的区域会明显大于真实区域。
	//此处可以考虑用循环遍历真实绘图区来判断。
	nItem = HitTest(&hti);

	//焦点有变化
	if (m_nLastFocusItem != nItem)
	{
		if (m_nLastFocusItem >= 0)
			OnFocusChanged(m_nLastFocusItem, FALSE);
		if (nItem >= 0)
			OnFocusChanged(nItem, TRUE);
	}

	m_nLastFocusItem = nItem;

	if (m_nLastFocusItem == -1)
	{
		m_bMouseTracked = FALSE;
	}

	if (m_bDragLayer == TRUE && nFlags == MK_LBUTTON)
	{
		m_bDragLayer = FALSE;
		m_ToolTip.Deactivate();
		Invalidate();
		DragLayer();
	}

	Invalidate();

	//CxListCtrl::OnMouseMove(nFlags, point);
}



void CxResLibImgList::OnLButtonDown(UINT nFlags, CPoint point)
{
	CxListCtrl::OnLButtonDown(nFlags, point);

	m_bDragLayer = TRUE;
	m_ToolTip.Deactivate();
	Invalidate();
	UpdateWindow();
	this->RedrawItems( 0, (int)m_vImg.size() );

	//vector<int> vSel;
	//int nCurSel = -1;
	//TestItem(vSel, nCurSel);

	//m_RMenu.DestroyMenu();
}


void CxResLibImgList::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_bDragLayer = FALSE;
	CxListCtrl::OnLButtonUp(nFlags, point);
}

void CxResLibImgList::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_ToolTip.Deactivate();
	Invalidate();

	vector<int> vSel;
	int nCurSel = -1;
	TestItem(vSel, nCurSel);

	m_RMenu.DestroyMenu();

	if ( nCurSel != -1 )
	{
		SetItemState(m_nLastFocusItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		RedrawItems(m_nLastFocusItem, m_nLastFocusItem);
	}

	if ( nFlags == MK_RBUTTON && m_nLastFocusItem != -1 )
	{
		if ( vSel.size() == 1 )
		{
			ClearCurSel();
			SetItemState(m_nLastFocusItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			RedrawItems(m_nLastFocusItem, m_nLastFocusItem);
		}

		if ( m_RMenu.GetSafeHmenu() == NULL )
		{
			m_RMenu.LoadMenu(IDR_RESLIB_LIST_POP);
		}
		
		CPoint pt = point;
		ClientToScreen( &pt);
		CMenu * pPopup = m_RMenu.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

		//不加这句话时，在图片上点击右键菜单，然后在其他窗口点击鼠标，使右键菜单消失。
		//此时，原来被菜单覆盖的图片区域没有被重绘。
		Invalidate();
	}
	
	CxListCtrl::OnRButtonDown(nFlags, point);
}

void CxResLibImgList::OnRButtonUp(UINT nFlags, CPoint point)
{
	CxListCtrl::OnRButtonUp(nFlags, point);
}

void CxResLibImgList::OnCxResLibImgListDelete()
{
	int nRet = MessageBox( "删除后将无法恢复，是否继续删除？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
	if ( nRet != IDOK ) return;

	vector<LPARAM> vParam;
	if ( GetCurSelParam( vParam ) == 0 ) return;

	m_nLastFocusItem = -1;
	ClearCurSel();

	LPARAM * pData = new LPARAM[ vParam.size() ];
	for (int i=0; i<(int)vParam.size(); i++)
	{
		pData[i] = vParam[i];
	}

	//vector< CResLibData * > vResPtr;
	//for (int i=0; i<(int)vParam.size(); i++)
	//{
	//	vResPtr.push_back( reinterpret_cast< CResLibData * > ( vParam[i] ) );
	//}

	m_bSetCapture = TRUE;
	theApp.m_pDlgResLibTree->SetCapture();
	//theApp.m_pDlgResLibTree->SetFocus();
	//theApp.m_pDlgResLibTree->SetActiveWindow();

	theApp.m_pDlgResLibTree->m_tree.PostMessage(WM_RESLIB_NOTIFY_DELETE
		, (WPARAM)pData, (LPARAM)vParam.size() );

}

void CxResLibImgList::TestItem( vector<int>& vSels, int& nCurSel )
{
	vSels.clear();
	nCurSel = -1;

	int nSelCnt = GetCurSel( vSels );

	LVHITTESTINFO hti;
	memset(&hti, NULL, sizeof(LVHITTESTINFO));
	hti.flags = LVHT_ABOVE;

	GetCursorPos(&(hti.pt));
	ScreenToClient(&hti.pt);

	int nItem = -1;
	nCurSel = HitTest(&hti);
}

void CxResLibImgList::OnUpdateProp()
{
	vector<LPARAM> vParam;
	if ( GetCurSelParam( vParam ) == 0 ) return;
	if ( vParam.size() != 1 )
	{
		::MessageBox(GetMainWnd(), "只能选择一个组件或设计过程编辑。", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	theApp.m_pDlgResLibTree->m_tree.PostMessage(WM_RESLIB_NOTIFY_UPDATE_PROP
		, (WPARAM)vParam[0], (LPARAM)0 );

	::ReleaseCapture();
}

void CxResLibImgList::OnDownload()
{
	vector<LPARAM> vParam;
	if ( GetCurSelParam( vParam ) == 0 ) return;
	if ( vParam.size() != 1 )
	{
		::MessageBox(GetMainWnd(), "只能选择一个组件或设计过程下载。", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}
	theApp.m_pDlgResLibTree->m_tree.PostMessage(WM_RESLIB_NOTIFY_DOWNLOAD
		, (WPARAM)vParam[0], (LPARAM)0 );

	//::ReleaseCapture();
}
