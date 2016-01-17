#pragma once
#include "resource.h"
// CDlgProgress �Ի���

class CDlgProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent/* = NULL*/);   // ��׼���캯��
	virtual ~CDlgProgress();
	void Close(); //�رմ���
// �Ի�������
	enum { IDD = IDD_DIALOG_WAIT };

	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	Gdiplus::Image* m_pImage;

	int m_nDstWidth;
	int m_nDstHeight;
	int m_nSrcWidth;
	int m_nSrcHeight;
	int m_nFrameCount;
	int m_nFrameDelay;
	HDC m_hdcMemory;
	BLENDFUNCTION m_Blend;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	BOOL InitPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL UpdateBk(int i);
	void UndoInitPaint();
	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);

public:
	virtual BOOL OnInitDialog();
};
