#pragma once
#include <AfxDialogEx.h>
#include "GDIPlusInit.h"

/*
ʹ��˵����

ʹ��ǰ������ȷ����ǰʵ�����̳ɹ���ʼ�� Gdiplus��
���Ը� CXXXApp ��� CGDIPlusInit ���࣬Ȼ���� CXXXApp::InitInstance �У�������µ��ó�ʼ����
InitGDIPlus();

ע�����
�뾡��ȷ�����̲߳��������������̱߳�����ʱ���޷�������ʾ��

*/

class CLoadingWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CLoadingWindow)

public:
	CLoadingWindow( UINT nTimerId, UINT nWindowId, CWnd* pParent );
	virtual ~CLoadingWindow();

	/*
	��ʼ����
	lpszFrontImagePath��	ͼƬ·������һ��Loading״̬��
	dwFrameCount��			Loading״̬������
	byteBkGray��			����ɫ�Ҷ�ֵ��0 - 255��0����ɫ��255����ɫ����
	byteTransparent��		͸��ֵ��0 - 255��
	dwDelay��				�л�Loading״̬�ļ����
	dwZoom��				���ű�����0 - 100��
	*/
	BOOL Init( LPCTSTR lpszFrontImagePath, DWORD dwFrameCount = 12
		, BYTE byteBkGray = 250, BYTE byteTransparent = 128, DWORD dwDelay = 90, DWORD dwZoom = 100 );
	BOOL Release();
	BOOL Show();
	BOOL Hide();

protected:
	BOOL DrawFrontImage( void );
	void NextFrame();
	BOOL ResetWindowPos( CRect& rcWnd );
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	
public:
	BOOL m_bCanceled;
protected:
	UINT m_nTimerId;
	Gdiplus::Image* m_pFrontImage;
	DWORD m_dwFrontImageFrameCount;
	DWORD m_dwCurFrame;
	DWORD m_dwDelay;
	DWORD m_dwZoom;
	BLENDFUNCTION m_Blend;
	Gdiplus::Brush * m_pBkBr;
	CRect m_rcWnd;
	BOOL  m_bVisable;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

extern CLoadingWindow* g_pLoadingWindow;