#include "stdafx.h"
#include "LoadingWindow.h"
#include "FileFunc.h"
#include "Tools.h"
static HBITMAP KCreateCompatibleBitmap( int nWidth, int nHeight )
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
	bmih.biClrImportant         = 0 ;   

	HBITMAP hBitMap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, 0, (VOID**)&pBits, NULL, 0);
	return hBitMap;
}

BEGIN_MESSAGE_MAP(CLoadingWindow, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoadingWindow::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoadingWindow::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void CLoadingWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

IMPLEMENT_DYNAMIC(CLoadingWindow, CDialogEx)

CLoadingWindow::CLoadingWindow( UINT nTimerId, UINT nWindowId, CWnd* pParent )
	: CDialogEx( nWindowId, pParent )
{
	m_nTimerId						= nTimerId	;
	m_pFrontImage					= NULL		;
	m_dwFrontImageFrameCount		= 0			;
	m_dwCurFrame					= 0			;
	m_dwDelay						= 0			;
	m_dwZoom						= 100		;
	memset( &m_Blend, NULL, sizeof( BLENDFUNCTION ) );
	m_pBkBr							= NULL		;
	m_bCanceled						= FALSE		;
	m_bVisable						= FALSE;
}

CLoadingWindow::~CLoadingWindow()
{
	Release();
}

void CLoadingWindow::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}

void CLoadingWindow::OnBnClickedCancel()
{
	//::SendMessage(this->GetSafeHwnd(),BM_CLICK,IDCANCEL,IDCANCEL);
	//按 ESC 不关闭对话框
	//注意：此消息将自动屏蔽 WM_CLOSE 消息
}

BOOL CLoadingWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CLoadingWindow::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
// 		if ( pMsg->wParam == VK_ESCAPE )
// 		{
// 			KillTimer( m_nTimerId );
// 			int nRet = MessageBox( "是否取消？", "", MB_YESNO );
// 			if ( nRet == IDYES )
// 			{
// 				Hide();
// 				m_bCanceled = TRUE;
// 				return TRUE;
// 			}
// 			else
// 			{
// 				SetTimer( m_nTimerId, m_dwDelay, NULL );
// 			}
// 			return TRUE;
//		}
	}

	return CDialog::PreTranslateMessage( pMsg );
}

void CLoadingWindow::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == m_nTimerId )
	{
		DrawFrontImage();
		NextFrame();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CLoadingWindow::Init( LPCTSTR lpszFrontImagePath, DWORD dwFrameCount, BYTE byteBkGray, BYTE byteTransparent, DWORD dwDelay, DWORD dwZoom )
{
	if ( !IsAccessibleFile( lpszFrontImagePath ) ) return FALSE;
	CString sPath = lpszFrontImagePath;
	CStringW sPathW;
	sPathW = sPath;

	m_pFrontImage = Gdiplus::Image::FromFile( (WCHAR*)(LPCWSTR)sPathW );
	if ( NULL == m_pFrontImage )
	{
		return FALSE;
	}

	m_dwFrontImageFrameCount = dwFrameCount;
	m_dwDelay = dwDelay;
	m_dwZoom = dwZoom;

	DWORD dwStyle	= ::GetWindowLong( m_hWnd, GWL_STYLE );
	DWORD dwExStyle	= ::GetWindowLong( m_hWnd, GWL_EXSTYLE );

	ModifyStyle( WS_CAPTION | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME, 0 );
	ModifyStyleEx( WS_EX_WINDOWEDGE | WS_EX_OVERLAPPEDWINDOW | WS_EX_PALETTEWINDOW, 0 );

	dwExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE );
	if ( ( dwExStyle & WS_EX_LAYERED ) != WS_EX_LAYERED )
	{
		SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED );
	}

	m_Blend.BlendOp					= 0					; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags				= 0					; //nothingelseisspecial...
	m_Blend.AlphaFormat				= 1					; //...
	m_Blend.SourceConstantAlpha		= byteTransparent	;//AC_SRC_ALPHA

	m_pBkBr = new Gdiplus::SolidBrush( Color( byteBkGray, byteBkGray, byteBkGray ) );
	return TRUE;
}

BOOL CLoadingWindow::Release()
{
	if ( m_nTimerId != 0 && ::IsWindow(m_hWnd))
	{
		KillTimer( m_nTimerId );
	}
	delete m_pFrontImage; m_pFrontImage = NULL;
	delete m_pBkBr; m_pBkBr = NULL;
	return TRUE;
}

BOOL CLoadingWindow::DrawFrontImage( void )
{
	if ( m_pFrontImage == NULL ) return FALSE;
	if ( m_dwFrontImageFrameCount == 0 ) return FALSE;
	if ( m_rcWnd.IsRectEmpty() ) return FALSE;

	LONG cxFrame = m_pFrontImage->GetWidth() / m_dwFrontImageFrameCount;
	LONG cyFrame = m_pFrontImage->GetHeight();
	if ( cxFrame <= 0 || cxFrame <= 0 )
	{
		return FALSE;
	}

	//计算Frame（m_pFrontImage 坐标）
	CRect rcFrame;
	rcFrame.left = cxFrame * m_dwCurFrame;
	rcFrame.right = rcFrame.left + cxFrame;
	rcFrame.top = 0;
	rcFrame.bottom = rcFrame.top + cyFrame;

	//计算绘制的图片区域（rcWnd 坐标）
	CRect rcDraw;
	if ( (DWORD)m_rcWnd.Width() > cxFrame * 100 / m_dwZoom )
	{
		rcDraw.left = m_rcWnd.CenterPoint().x - cxFrame * 100 / m_dwZoom / 2;
		rcDraw.right = rcDraw.left + cxFrame * 100 / m_dwZoom;
	}
	else
	{
		rcDraw.left = m_rcWnd.left;
		rcDraw.right = m_rcWnd.right;
	}
	if ( (DWORD)m_rcWnd.Height() > cyFrame * 100 / m_dwZoom )
	{
		rcDraw.top = m_rcWnd.CenterPoint().y - cyFrame * 100 / m_dwZoom / 2;
		rcDraw.bottom = rcDraw.top + cyFrame * 100 / m_dwZoom;
	}
	else
	{
		rcDraw.top = m_rcWnd.top;
		rcDraw.bottom = m_rcWnd.bottom;
	}
	rcDraw.OffsetRect( - m_rcWnd.left, - m_rcWnd.top );

	HDC hDC = ::GetDC ( m_hWnd );
	HDC hMemDc = ::CreateCompatibleDC( hDC );
	HBITMAP hMemBmp = KCreateCompatibleBitmap( m_rcWnd.Width(), m_rcWnd.Height() );
	HBITMAP hOldBmp = (HBITMAP)SelectObject( hMemDc, hMemBmp );

	Gdiplus::Graphics graph( hMemDc );
	Gdiplus::Rect rcDrawF( rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height() );
	graph.FillRectangle( m_pBkBr, 0, 0, m_rcWnd.Width(), m_rcWnd.Height() );
	graph.DrawImage( m_pFrontImage, rcDrawF
		, rcFrame.left, rcFrame.top, rcFrame.Width(), rcFrame.Height()
		, Gdiplus::UnitPixel );

	POINT ptOri = { 0, 0 };
	::UpdateLayeredWindow( m_hWnd, hDC, &m_rcWnd.TopLeft(), &m_rcWnd.Size()
		, hMemDc, &ptOri, 0, &m_Blend, ULW_ALPHA );

	graph.ReleaseHDC( hMemDc );
	SelectObject( hMemDc, hOldBmp );
	DeleteDC( hMemDc ); hMemDc = NULL;
	DeleteObject( hMemBmp ); hMemBmp = NULL;
	::ReleaseDC( m_hWnd, hDC );

	ShowWindow( SW_SHOW );
	return TRUE;
}

void CLoadingWindow::NextFrame()
{
	if ( m_dwFrontImageFrameCount == 0 ) return;
	if ( m_dwCurFrame + 1 >= m_dwFrontImageFrameCount )
	{
		m_dwCurFrame = 0;
	}
	else
	{
		m_dwCurFrame++;
	}
}

BOOL CLoadingWindow::ResetWindowPos( CRect& rcWnd )
{
	CWnd* pParent = NULL;
	//计算父窗口矩形区域（屏幕坐标）
	pParent = GetParent();
	if ( NULL != (pParent = GetParent() ) )
	{
		pParent->GetWindowRect( &rcWnd );
	}
	else if ( NULL != (pParent = GetOwner() ) )
	{
		pParent->GetWindowRect( &rcWnd );
	}
	else if ( NULL != (pParent = GetDesktopWindow() ) )
	{
		pParent->GetWindowRect( &rcWnd );
	}
	else
	{
		return FALSE;
	}
	SetWindowPos( NULL, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height()
		, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOREDRAW | SWP_NOZORDER );
	return TRUE;
}

BOOL CLoadingWindow::Show()
{
	m_bCanceled = FALSE;
	ResetWindowPos( m_rcWnd );
	//ShowWindow( SW_SHOW );
	m_bVisable = TRUE;
	SetTimer( m_nTimerId, m_dwDelay, NULL );
	SetFocus();
	return TRUE;
}

BOOL CLoadingWindow::Hide()
{
	KillTimer( m_nTimerId );
	BOOL bRev = ShowWindow( SW_HIDE );
	m_bVisable = FALSE;
	return TRUE;
}

CLoadingWindow* g_pLoadingWindow = NULL;

void CLoadingWindow::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (m_bVisable == FALSE)
	{
		if (bShow == TRUE)
		{
			ZTools::WriteZToolsFormatLog("阻止窗口再次显示");
			return;
		}
	}
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
}
