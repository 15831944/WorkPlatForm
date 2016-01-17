// CxResLibImgList.cpp : ʵ���ļ�
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

//rcIcon�����·�����4���ؿհ�����
static const int CY_ICON_BOTTOM_SPACE = 4;
//rcIcon��rcLabel֮������2���ؿհ�����
static const int CY_ICON_LABEL_SPACE = 2;
//����������С�߶�
static const int CY_LABEL_TEXT = 19;

// CxResLibImgList

//ListCtrlĬ�ϱ���ɫ
const Color CxResLibImgList::COLOR_LISTCTRL_BG = Color::White;
//Ĭ��ͼƬ����ɫ
const Color CxResLibImgList::COLOR_IMAGE_BG = Color::White;
//Y�᷽��ͼƬ��С���
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
	//ͼƬģʽ
	if ( !SetImageMode() ) return FALSE;
	//����ɫ
//	m_crCanvasBk = Color::LightGreen;
	//ͼƬ�ߴ硢��ࡢ͸��ɫ
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
��ͼ��ģʽ��Ч��

��ʼ����ʱ����� SetFont(NULL); CY_LABEL_TEXT = 19��
��� ��д SetFont(NULL); ��仰��CY_LABEL_TEXT = 16��
nSpaceY С�� MIN_CY_IMAGESPACE �Ļ����ᵼ�»��ƺ󲼾ֲ���ȷ��
���⣬������ֹ��������ǵ���һ��ͼƬ������Ļ���Ҳ�ᵼ��ˢ�²���ȷ��
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


//����״̬������ͼƬ
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


//����������Ϣ
void CxResLibImgList::OnLoadTextFont()
{
	if (m_pFont != NULL) { delete m_pFont; m_pFont = NULL; }

	CStringW strW;
	strW = m_strFontFamilyName;
	if (strW.IsEmpty()) strW = L"����";
	FontFamily family((WCHAR *)(LPCWSTR)strW);
	m_pFont = new Gdiplus::Font(&family, 12, FontStyleRegular, UnitPixel);

	//m_FontFormat.SetAlignment(StringAlignmentNear);
	//m_FontFormat.SetLineAlignment(StringAlignmentFar);
	m_FontFormat.SetAlignment(StringAlignmentCenter); //ˮƽ����
	//m_FontFormat.SetLineAlignment(StringAlignmentCenter); //��ֱ����

	m_FontColor = Color::Black;
}


/*
���ֵ�����ڿ���ͼƬ������֮��ľ��롣
*/
int CxResLibImgList::GetSpaceOfIconLabel( void )
{
	return 5;
}


//
void CxResLibImgList::OnEraseBkgndCustom( void )
{
}


//����ı�
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
		//strTip.Format( "���ƣ�%s\r\n���ߣ�%s\r\nʱ�䣺%s\r\n�汾��%s\r\n������%s"
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



//��������
BOOL CxResLibImgList::OnEraseBkgnd(CDC* pDC)
{
	//��ִ��Ĭ�ϵĲ������������ִ���ˣ��ᵼ����˸��
	//ԭ���ǣ�ÿ��ͼƬ����ǰ���ؼ����Զ�����ͼƬ��Ӧ��Item����
	//BOOL bOk = CxListCtrl::OnEraseBkgnd(pDC);
	OnEraseBkgndCustom();


	//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnEraseBkgnd %s", "\r\n" );

	return TRUE;
}


/*
֪ͨ�Ի����Ϣ��ڡ�
���̣�
1��CDDS_PREPAINT ��ʼ�Ի棬��� ���� CDRF_DODEFAULT���� CListCtrl ���л��ơ�
	����������ڴ�dc��bmp���򴴽�֮�����ڻ��ơ�
	������ OnEraseBkgndCustom �д�����

2��CDDS_ITEMPREPAINT������ÿ�� Item��

3��CDDS_POSTPAINT�����ƽ�����
	���ڴ�dc��bmp�е����ݿ�����ʵ��dc�У���ɾ���ڴ�dc��bmp��
*/
void CxResLibImgList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
static const DWORD CDDS_SUBITEMPREPAINT = (DWORD)(CDDS_ITEMPREPAINT | CDDS_SUBITEM);
static const DWORD CDDS_SUBITEMPOSTPAINT = (CDDS_ITEMPOSTPAINT | CDDS_SUBITEM);

	static int nDrawCount = 0;
	LPNMLVCUSTOMDRAW lplvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//if (lplvcd == NULL) return;

	switch(lplvcd->nmcd.dwDrawStage)
	{
	//�����ؼ��Ļ���ѭ����ʼǰ��
	//����˴����� CDRF_DODEFAULT���ؼ����Լ������Լ����������յ��κ�֪ͨ��
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
				//�趨͸��ģʽ���͵���ģʽ��������趨�����ë�ߣ����ͼ�α߽粻�Ǻ�ƽ��
				m_pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
				m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);
				//m_pGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
				//��������
				RectF rcf((float)rc.left, (float)rc.top, (float)rc.Width(), (float)rc.Height());
				SolidBrush br(m_crCanvasBk);
				m_pGraphics->FillRectangle(&br, rcf);

				nDrawCount = 0;

				//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnNMCustomdraw %s", "����Canvas\r\n" );
			}
		}
		
		*pResult = CDRF_NOTIFYITEMDRAW //Item ����ǰ��֪ͨ
			| CDRF_NOTIFYPOSTPAINT //�����ؼ�������ɺ�֪ͨ
			| CDRF_NOTIFYPOSTERASE //����֮��֪ͨ����ȫ�������ã�������⣬����ͨ��ʵ�� OnEraseBkgnd �����
			| CDRF_NOTIFYSUBITEMDRAW //��Item ����ǰ��֪ͨ
			; 
		break;

	case  CDDS_POSTPAINT: //�����ؼ��Ļ���ѭ����ɡ�
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
					//xp�£������������ڵ������һ�������ʱ���Ҳ����򲻱�����
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

				//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnNMCustomdraw %s", "����Canvas��DC\r\n" );
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
	case CDDS_ITEMPREPAINT: // Item ����ǰ
		if ( !OnItemPaint(lplvcd) )
			*pResult = CDRF_DODEFAULT; //ʧ��
		else
		{
			//�ɹ�
			nDrawCount++;
			*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT | CDRF_SKIPDEFAULT;
		}
		//ZTools::WriteZToolsFormatLog( "CxResLibImgList::OnNMCustomdraw %s", "OnItemPaint\r\n" );
		break;

	//After an item is drawn.
	case CDDS_ITEMPOSTPAINT: //Item ������ɺ�
		//*pResult = CDRF_DODEFAULT;
		//*pResult = CDRF_SKIPDEFAULT;
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	
	//Before an sub-item is drawn.
	case CDDS_SUBITEMPREPAINT: //SubItem ����ǰ
		*pResult = CDRF_DODEFAULT;
		break;
	////after an sub-item is drawn.
	case CDDS_SUBITEMPOSTPAINT: //SubItem ������ɺ�
		*pResult = CDRF_DODEFAULT;
		break;
	}

	//*pResult = 0; ͨ������ֵ���ƻ��Ƶĸ����׶�
}


//����ͼƬ
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

	//���� rcBound �ߴ�
	int nCyMinBound = CY_ICON_BOTTOM_SPACE + CY_ICON_LABEL_SPACE + CY_LABEL_TEXT;
	//int nReal = m_nImgCySpace;
	//if (nReal < nCyMinBound)
	//{
	//	rcBound.bottom -= (nCyMinBound - nReal);
	//	if (rcLabel.bottom > rcBound.bottom)
	//		rcLabel.bottom = rcBound.bottom;
	//}
	//�ұ߽�
	//rcBound.right = rcBound.left + rcIcon.right - rcIcon.left 
	//	+ (rcIcon.left - rcBound.left) * 2;

	//rcBound.left += 20;
	//rcBound.right -= 20;
	//rcBound.top += 10;
	//rcBound.bottom -= 10;

	//Icon���� �� Bound���� ����
	x = ( ( rcBound.right - rcBound.left ) - m_nImgCx ) / 2;

	rcIcon.top = rcBound.top + 11;
	rcIcon.bottom = rcIcon.top + m_nImgCy;
	rcIcon.left = rcBound.left + x;
	rcIcon.right = rcIcon.left + m_nImgCx;

	//���� rcLabel λ�úͳߴ�
	int nIconLabelSpaceOffset = GetSpaceOfIconLabel();
	int nLabelHeight = rcLabel.bottom - rcLabel.top;
	//���� Icon �� Label ֮��ļ��
	rcLabel.top = rcIcon.bottom + nIconLabelSpaceOffset;
	//rcLabel.bottom = rcLabel.top + nLabelHeight;
	rcLabel.bottom = rcBound.bottom - 6;
	//����Lable���
	rcLabel.left = rcBound.left - 2;
	rcLabel.right = rcBound.right - 2;
	rcLabel.bottom+=2;
	//�߽�ƽ���ı��ε�
	Point BoundPoints[] = {
		Point( rcBound.left, rcBound.top ) //����
		, Point( rcBound.right, rcBound.top ) //����
		, Point( rcBound.left, rcBound.bottom ) //����
	};

	//ͼ��ƽ���ı��ε�
	Point IconPoints[] = {
		Point( rcIcon.left, rcIcon.top ) //����
		, Point( rcIcon.right, rcIcon.top ) //����
		, Point( rcIcon.left, rcIcon.bottom ) //����
	};

	//����
	if (nState & LVIS_FOCUSED) //�н���
	{
		if (nState & LVIS_SELECTED) //���� + ѡ��
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

		//���� + δѡ��
		{
			RectF rcIconF( (float)rcIcon.left, (float)rcIcon.top
				, (float)(rcIcon.right - rcIcon.left), (float)(rcIcon.bottom - rcIcon.top) );

			m_pGraphics->DrawImage( m_vImg[nItem], IconPoints, 3 );

			//m_pGraphics->DrawImage( m_vImg[nItem], IconPoints, 3 );
			//m_pGraphics->DrawImage( m_pImgFocused, BoundPoints, 3 );
			////DrawHighlight(m_vImg[nItem], m_pGraphics, rcIconF);
		}
	}
	else //û�н���
	{
		if (nState & LVIS_SELECTED) //�޽��� + ѡ��
		{
			if (m_pImgSelected != NULL)
			{
				m_pGraphics->DrawImage( m_pImgSelected, BoundPoints, 3 );
			}
		}

		//�޽��� + δѡ��
		{
			m_pGraphics->DrawImage( m_vImg[nItem], IconPoints, 3 );
		}
	}

	//��������
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


//��������
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
		//��һ��
		RectF rcLine = rcLabelF;
		rcLine.Height = rcfText.Height;
		strLineW = strLabelW.Mid(0, nFirstLineCharCount);

		m_pGraphics->DrawString((LPCWSTR)strLineW
			, -1
			, m_pFont
			, rcLine
			, &m_FontFormat
			, &brushText);

		//��2��
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


//���ͼƬ
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


//���ͼƬ
BOOL CxResLibImgList::AddImage( Bitmap * pBitmap, LPCTSTR lpszLabel, LPARAM lp, int nPos )
{
	if (m_bAutoDeleteData) return FALSE;
	if (pBitmap == NULL) return FALSE;
	_AddImage(pBitmap, lpszLabel, lp, nPos);
	return TRUE;
}


//���ͼƬ
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


//ɾ��ͼƬ
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
	//GetModuleFileName(NULL,exeFullPath,MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��
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
		strTip.Format( "���ƣ�%s\r\n���ߣ�%s\r\nʱ�䣺%s\r\n�汾��%s\r\n������%s"
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

	//��� Item �Ƿ�ѡ��
	//��ͼƬ�����������ʱ��ListCtrl���ص���������Դ�����ʵ����
	//�˴����Կ�����ѭ��������ʵ��ͼ�����жϡ�
	nItem = HitTest(&hti);

	//�����б仯
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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

		//������仰ʱ����ͼƬ�ϵ���Ҽ��˵���Ȼ�����������ڵ����꣬ʹ�Ҽ��˵���ʧ��
		//��ʱ��ԭ�����˵����ǵ�ͼƬ����û�б��ػ档
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
	int nRet = MessageBox( "ɾ�����޷��ָ����Ƿ����ɾ����", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
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
		::MessageBox(GetMainWnd(), "ֻ��ѡ��һ���������ƹ��̱༭��", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
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
		::MessageBox(GetMainWnd(), "ֻ��ѡ��һ���������ƹ������ء�", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}
	theApp.m_pDlgResLibTree->m_tree.PostMessage(WM_RESLIB_NOTIFY_DOWNLOAD
		, (WPARAM)vParam[0], (LPARAM)0 );

	//::ReleaseCapture();
}
