// DlgLoginBk.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgLoginBk.h"
#include "ResLibImageSet.h"
#include "PeraProcessDesigner.h"

// CDlgLoginBk �Ի���

IMPLEMENT_DYNAMIC(CDlgLoginBk, CDialog)

	CDlgLoginBk::CDlgLoginBk(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLoginBk::IDD, pParent)
{

}

CDlgLoginBk::~CDlgLoginBk()
{
}

void CDlgLoginBk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CDlgLoginBk, CDialog)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVING()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgLoginBk::OnBnClickedButtonClose)
	ON_MESSAGE(WM_LOGIN_CLOSEBUTTON_MOUSEOVER, OnMouseOverClose)
	ON_MESSAGE(WM_LOGIN_CLOSEBUTTON_MOUSELEAVE, OnMouseLeaveClose)
END_MESSAGE_MAP()


// CDlgLoginBk ��Ϣ�������

BOOL CDlgLoginBk::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CWnd* pParent = GetParent();
	if (pParent && IsWindow(pParent->GetSafeHwnd()))
	{
		pParent->RedrawWindow();
	}

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	UpdateBk();
	CenterWindow();
	CRect rc;
	GetWindowRect(&rc);
	SetWindowPos(NULL, rc.left+1, rc.top+1, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_FRAMECHANGED);
	rc.DeflateRect(12, 12 + 16, 14 + 16, 14);
	m_DlgLoginMain.Create(CDlgLoginMain::IDD, this);
	m_DlgLoginMain.SetWindowPos(NULL, rc.left+1, rc.top+1, rc.Width(), rc.Height(), SWP_NOZORDER|SWP_SHOWWINDOW|SWP_FRAMECHANGED);

	GetClientRect(&rc);
	rc.left = rc.right - m_nCloseWidth;
	rc.bottom = rc.top + m_nCloseHeight;
	m_btnClose.MoveWindow(&rc);

	//ʹ����������뽹��
	SetTimer( TIMER_DLGLOGINBK_SETFOCUS, TIMER_DLGLOGINBK_SETFOCUS_TIMEOUT, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

HBITMAP CDlgLoginBk::KCreateCompatibleBitmap(int nWidth, int nHeight)  
{  
	BYTE                      * pBits ;   
	BITMAPINFOHEADER          bmih;   
	ZeroMemory( &bmih, sizeof(BITMAPINFOHEADER));   

	bmih.biSize                 = sizeof (BITMAPINFOHEADER) ;   
	bmih.biWidth                = nWidth;   
	bmih.biHeight               = nHeight;   
	bmih.biPlanes               = 1 ;   
	bmih.biBitCount             = 32;        //����һ��Ҫ��32   
	bmih.biCompression          = BI_RGB ;   
	bmih.biSizeImage            = 0 ;   
	bmih.biXPelsPerMeter        = 0 ;   
	bmih.biYPelsPerMeter        = 0 ;   
	bmih.biClrUsed              = 0 ;   
	bmih.biClrImportant          = 0 ;   

	HBITMAP hBitMap = CreateDIBSection (NULL, (BITMAPINFO *)  &bmih, 0, (VOID**)&pBits, NULL, 0) ;  
	return hBitMap;  
}  

BOOL CDlgLoginBk::UpdateBk( BOOL bOverClose )
{
	HDC hdcTemp=::GetDC (m_hWnd);
	m_hdcMemory=CreateCompatibleDC(hdcTemp);
	//HBITMAP hBitMap=CreateCompatibleBitmap(hdcTemp, m_nBkWidth, m_nBkHeight);
	HBITMAP hBitMap=KCreateCompatibleBitmap(m_nWndWidth, m_nWndHeight);
	SelectObject(m_hdcMemory,hBitMap);

	m_Blend.SourceConstantAlpha=255;
	HDC hdcScreen=::GetDC (m_hWnd);
	RECT rct;
	GetWindowRect(&rct);
	POINT ptWinPos={rct.left, rct.top};
	Gdiplus::Graphics graph(m_hdcMemory);

	Gdiplus::Point points[] = { Gdiplus::Point(0, m_nWndHeight - m_nBkHeight), 
		Gdiplus::Point(m_nBkWidth, m_nWndHeight - m_nBkHeight), 
		Gdiplus::Point(0, m_nWndHeight)
	};
	graph.DrawImage(m_pImageBk, points, 3); 

	Gdiplus::Point points1[] = { Gdiplus::Point(m_nWndWidth - m_nCloseWidth, 0), 
		Gdiplus::Point(m_nWndWidth, 0), 
		Gdiplus::Point(m_nWndWidth - m_nCloseWidth, m_nCloseHeight)
	};
	if (bOverClose == FALSE)
	{
		graph.DrawImage(m_pImageCloseNor, points1, 3); 
	}
	else
	{
		graph.DrawImage(m_pImageCloseOver, points1, 3);
	}

	SIZE sizeWindow={m_nWndWidth, m_nWndHeight};
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
int CDlgLoginBk::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	//��ʼ��gdiplus�Ļ���
	// Initialize GDI+.
	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA

//	ImageFromIDResource(IDR_PNG_LOGIN_BK, "PNG", m_pImageBk);
	m_pImageBk = GetDlgLoginResImageData(CCxStaticRes::RT_DLG_LOGIN_BK_BK)->m_pImgNormal;
	m_nBkWidth  =m_pImageBk->GetWidth();
	m_nBkHeight =m_pImageBk->GetHeight();
	m_nWndWidth = m_nBkWidth + 16;
	m_nWndHeight = m_nBkHeight + 16;
//	ImageFromIDResource(IDR_PNG_LOGIN_NOR, "PNG", m_pImageCloseNor);
//	ImageFromIDResource(IDR_PNG_LOGIN_OVER, "PNG", m_pImageCloseOver);
	m_pImageCloseNor = GetDlgLoginResImageData(CCxStaticRes::RT_DLG_LOGIN_CLOSE_NOR)->m_pImgNormal;
	m_pImageCloseOver = GetDlgLoginResImageData(CCxStaticRes::RT_DLG_LOGIN_CLOSE_OVER)->m_pImgNormal;
	m_nCloseWidth  =m_pImageCloseNor->GetWidth();
	m_nCloseHeight =m_pImageCloseNor->GetHeight();
	//::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);   
	return 0;
}

BOOL CDlgLoginBk::ImageFromIDResource(UINT nID, LPCTSTR sTR, Gdiplus::Image * &pImg)
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
void CDlgLoginBk::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgLoginBk::OnMoving(UINT fwSide, LPRECT pRect)
{
	if (pRect->top<0)
	{
		pRect->bottom = pRect->bottom - pRect->top;
		pRect->top = 0;
	}
	CDialog::OnMoving(fwSide, pRect);

	CRect rc(*pRect);
	rc.DeflateRect(12, 12 + 16, 14 + 16, 14);
	m_DlgLoginMain.MoveWindow(&rc);

	// TODO: �ڴ˴������Ϣ����������
}

void CDlgLoginBk::OnMyOk()
{
	CDialog::OnOK();
}
void CDlgLoginBk::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}

void CDlgLoginBk::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialog::OnOK();
}

void CDlgLoginBk::OnMyCancle()
{
	CDialog::OnCancel();
}
void CDlgLoginBk::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == TIMER_DLGLOGINBK_SETFOCUS)
	{
		KillTimer(TIMER_DLGLOGINBK_SETFOCUS);
		m_DlgLoginMain.SetFocus();
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgLoginBk::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

LRESULT CDlgLoginBk::OnMouseLeaveClose( WPARAM /*wParam*/,LPARAM /*lParam*/ )
{
	UpdateBk(FALSE);
	return 0;
}

LRESULT CDlgLoginBk::OnMouseOverClose( WPARAM /*wParam*/,LPARAM /*lParam*/ )
{
	UpdateBk(TRUE);
	return 0;
}


BOOL CDlgLoginBk::PreTranslateMessage(MSG* pMsg)
{
	//��س�ʱ���رմ��ڣ����õ�½
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			m_DlgLoginMain.OnBnClickedButtonLogin();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
