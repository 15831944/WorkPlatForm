#pragma once
#include "resource.h"
// CDlgProgress 对话框

class CDlgProgress : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent/* = NULL*/);   // 标准构造函数
	virtual ~CDlgProgress();
	void Close(); //关闭窗口
// 对话框数据
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	BOOL InitPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL UpdateBk(int i);
	void UndoInitPaint();
	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);

public:
	virtual BOOL OnInitDialog();
};
