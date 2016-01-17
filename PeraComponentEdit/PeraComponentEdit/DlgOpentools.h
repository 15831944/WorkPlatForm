#pragma once
#include "resource.h"
#include "SharedMemoryOnlineEdit.h"
#include "Tools.h"

// CDlgOpentools 对话框

class CDlgOpentools : public CDialog
{
	DECLARE_DYNAMIC(CDlgOpentools)

public:
	CDlgOpentools(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgOpentools();

	// 对话框数据
	enum { IDD = IDD_DLG_OPENTOOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString m_sOpenFlag;
	CString m_sBaseType;
	CString m_sNodeName;
	CString m_sComponentName;
	CString m_sLocalPath;
	CString m_sLocalRobotPath;
	CString m_sParamsFilePath;
	BOOL    m_bApplied;
	virtual BOOL OnInitDialog();

	SharedMemoryOnlineEdit* m_pSharedMemoryOnlineEdit;

	DWORD m_dProcessId;
	HWND m_hWndOpentools;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void MyClose(int nCode);
	void SendMession();
	void BindWindow();
	void UnbindWindow();
	int m_nCode;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	DWORD m_dwStyle;
	DWORD m_dwExStyle;

	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);
	BOOL UpdateBk(int i);
	BOOL InitPaint();
	void UndoInitPaint();
	static BOOL CALLBACK EnumWindowCallback_GetOpentoolsPopupWindow(HWND hWnd, LPARAM lParam);
	static HWND GetOpentoolsPopupWindow(DWORD dwPID);
	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Gdiplus::Image * &pImg);
	void SetWndPos();
	HDC m_hdcMemory;
	BLENDFUNCTION m_Blend;
	Gdiplus::Image* m_pImage;

	int m_nDstWidth;
	int m_nDstHeight;
	int m_nSrcWidth;
	int m_nSrcHeight;
	int m_nFrameCount;
	int m_nFrameDelay;
};
