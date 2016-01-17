#pragma once
#include <BCGPFrameWnd.h>
//#include "CxBCGPFrameWnd.h"
#include "CxBCGPSplitterWnd.h"
#include "CxSplitterFrameWnd.h"


// CDlgResLib �Ի���

class CDlgResLib : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgResLib)

public:
	CDlgResLib(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgResLib();

// �Ի�������
	enum { IDD = IDD_COMPONENT_LIB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	CCxSplitterFrameWnd * m_pFrameWnd;
	//CBCGPSplitterWnd m_wndSplitter;
	CCxBCGPSplitterWnd m_wndSplitter;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//virtual void PreSubclassWindow();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
