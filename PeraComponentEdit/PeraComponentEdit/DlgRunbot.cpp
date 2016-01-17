// DlgRunbot.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgRunbot.h"
#include "afxdialogex.h"

#define TIMER_ANIMATE							14
// DlgRunbot 对话框

IMPLEMENT_DYNAMIC(DlgRunbot, CDialogEx)

DlgRunbot::DlgRunbot(CWnd* pParent /*=NULL*/, CComponentEdit* p, std::string& strJson)
	: CDialogEx(DlgRunbot::IDD, pParent), m_pCComponentEdit(p), m_strJson(strJson)
{

}

DlgRunbot::~DlgRunbot()
{
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void DlgRunbot::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgRunbot, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL DlgRunbot::ImageFromIDResource(UINT nID, LPCTSTR sTR, Gdiplus::Image * &pImg)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type
	if (!hRsrc)
		return FALSE;

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;

	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

	// load from stream
	pImg=Gdiplus::Image::FromStream(pstm);

	// free/release stuff
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);

	return TRUE;
}

// DlgRunbot 消息处理程序
BOOL DlgRunbot::InitPaint()
{
	//////////////////////////////////////////////////////////////////////////

	m_dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	m_dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);

	ModifyStyle(WS_CAPTION|WS_BORDER|WS_DLGFRAME|WS_THICKFRAME, NULL);
	ModifyStyleEx(WS_EX_WINDOWEDGE|WS_EX_OVERLAPPEDWINDOW|WS_EX_PALETTEWINDOW, NULL);

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);

	ImageFromIDResource(IDB_PNG_ANIMATE, "PNG", m_pImage);

	m_nSrcWidth = m_pImage->GetWidth();
	m_nSrcHeight = m_pImage->GetHeight();

	m_nDstWidth = 70;
	m_nDstHeight = 70;
	m_nFrameCount = 12;
	m_nFrameDelay = 90;

	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

	SetTimer(TIMER_ANIMATE,m_nFrameDelay,NULL);
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
}

void DlgRunbot::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == TIMER_ANIMATE)
	{
		static int i = 0;
		i = i % m_nFrameCount;
		UpdateBk(i);
		i++;
	}

	CDialog::OnTimer(nIDEvent);
}

HBITMAP DlgRunbot::KCreateCompatibleBitmap(int nWidth, int nHeight)  
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

BOOL DlgRunbot::UpdateBk(int i)
{
	HDC hdcTemp=::GetDC (m_hWnd);
	m_hdcMemory=CreateCompatibleDC(hdcTemp);
	//HBITMAP hBitMap=CreateCompatibleBitmap(hdcTemp, m_nBkWidth, m_nBkHeight);
	HBITMAP hBitMap=KCreateCompatibleBitmap(m_nDstWidth, m_nDstHeight);
	SelectObject(m_hdcMemory,hBitMap);

	m_Blend.SourceConstantAlpha=255;
	HDC hdcScreen=::GetDC (m_hWnd);

	RECT rct;
	CWnd* pWnd = GetParent();
	if (pWnd)
	{
		pWnd->GetWindowRect(&rct);
	}
	else
	{
		GetWindowRect(&rct);
	}
	rct.left = (rct.left + rct.right - m_nDstWidth)/2;
	rct.top = (rct.top + rct.bottom - m_nDstHeight)/2;
	POINT ptWinPos={rct.left, rct.top};

	Gdiplus::Graphics graph(m_hdcMemory);

	Gdiplus::Rect rc(0,0,m_nDstWidth,m_nDstHeight);

	int nTempWidth = m_nSrcWidth/m_nFrameCount;
	graph.DrawImage(m_pImage, rc, i*nTempWidth,0,nTempWidth,m_nSrcHeight,Gdiplus::UnitPixel);

	SIZE sizeWindow={m_nDstWidth, m_nDstHeight};
	POINT ptSrc={0,0};
	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);

	BOOL bRet=FALSE;
	bRet= ::UpdateLayeredWindow( m_hWnd, hdcScreen, &ptWinPos,
		&sizeWindow, m_hdcMemory, &ptSrc, 0, &m_Blend, 2);
	graph.ReleaseHDC(m_hdcMemory);
	::ReleaseDC(m_hWnd,hdcScreen);
	hdcScreen=NULL;
	::ReleaseDC(m_hWnd,hdcTemp);
	hdcTemp=NULL;
	DeleteObject(hBitMap);
	DeleteDC(m_hdcMemory);
	m_hdcMemory=NULL;
	return bRet;
}

void DlgRunbot::UndoInitPaint()
{
	KillTimer(TIMER_ANIMATE);

	ShowWindow(SW_HIDE);
	//////////////////////////////////////////////////////////////////////////
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);
	int nWorkHeight = rcArce.Height();
	int nWorkWidth = rcArce.Width();

	int x = 1120;
	int y = 630;
	int xStart = (nWorkWidth - x) / 2;
	int yStart = (nWorkHeight - y) / 2;

	::SetWindowPos(m_hWnd, NULL, xStart , yStart, x-1, y-1, SWP_NOZORDER|SWP_FRAMECHANGED);
	//////////////////////////////////////////////////////////////////////////

	SetWindowLong(m_hWnd, GWL_STYLE, m_dwStyle);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, m_dwExStyle);

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle^WS_EX_LAYERED);
	ShowWindow(SW_SHOW);
}

static UINT RunThread( LPVOID pParam )
{
	DlgRunbot* pDlg = (DlgRunbot*)pParam;
	pDlg->m_strRet = pDlg->m_pCComponentEdit->ProcessJsonInOut(pDlg->m_strJson);
	PostMessage(pDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
	//pDlg->Close();
	return 0;
}


BOOL DlgRunbot::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitPaint();
	//开启组件线程
	AfxBeginThread(RunThread, this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgRunbot::Close()
{
	OnOK();
}

