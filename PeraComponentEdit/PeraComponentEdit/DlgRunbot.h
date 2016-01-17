#pragma once
#include "ComponentEdit.h"

// DlgRunbot 对话框

class DlgRunbot : public CDialogEx
{
	DECLARE_DYNAMIC(DlgRunbot)

public:
	DlgRunbot(CWnd* pParent/* = NULL*/, CComponentEdit* pCComponentEdit, std::string& str);   // 标准构造函数
	virtual ~DlgRunbot();
	void Close(); //关闭窗口
// 对话框数据
	enum { IDD = IDD_DLG_RUNBOT };

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

	std::string m_strRet; //返回值
	CComponentEdit* m_pCComponentEdit; //组件对象指针
	std::string m_strJson; //传入串
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	BOOL InitPaint();
	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Gdiplus::Image * &pImg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL UpdateBk(int i);
	void UndoInitPaint();
	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);


public:
	virtual BOOL OnInitDialog();
};
