#pragma once
#include "ComponentEdit.h"

// DlgRunbot �Ի���

class DlgRunbot : public CDialogEx
{
	DECLARE_DYNAMIC(DlgRunbot)

public:
	DlgRunbot(CWnd* pParent/* = NULL*/, CComponentEdit* pCComponentEdit, std::string& str);   // ��׼���캯��
	virtual ~DlgRunbot();
	void Close(); //�رմ���
// �Ի�������
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

	std::string m_strRet; //����ֵ
	CComponentEdit* m_pCComponentEdit; //�������ָ��
	std::string m_strJson; //���봮
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
