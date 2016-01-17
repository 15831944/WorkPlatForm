#pragma once
#include "afxwin.h"
#include "ZEdit.h"
#include "ZGridCtrl.h"
#include "Resource.h"
#include "WndIf.h"
#include "DlgWndIf.h"
#include "zbuttondraw.h"
#include "CxBCGPDialog.h"

// CDlgIf �Ի���

class CDlgIf : public CCxBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIf)

public:
	CDlgIf(std::string str, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgIf();

// �Ի�������
	enum { IDD = IDD_IF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��ʾEdit
	CZEdit m_EditShow;
	virtual BOOL OnInitDialog();
	// ����
	CStatic m_StaticName;

	std::string m_strJsonSrc; //����Json

	std::string m_strJsonRet; //����Json

	CCxStructTreeNode * m_TreeRoot; //�����ڵ�

	virtual void OnOK();

	CStatic m_staticName1;

	int m_radio;

	CZEdit m_editExp;  

	//CWndIf m_wndIf; //���ı༭����
	CDlgWndIf m_wndIf; //���ı༭����

	int m_nWidth; //���ڿ�
	int m_nHeight;// ���ڸ�
	static const int m_nWidthMax;
	static const int m_nHeightMax;

	int m_nVisualManager; //���

	//��֧����Ϊ�����̣���ζ�ŷ�֧�ڵ�û�в��������ʽ����ʱ���Կ����������ںеĲ���20140321������ȷҪ��
	BOOL m_bIfNotSubFlow;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	void MoveOffset( CWnd& wnd, CRect& rect );
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedRadioExclusive();
	afx_msg void OnBnClickedRadioInclusive();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	void ProcessUpdate( const char* str );
	void ParseJson(const char* strJson);
	CZButtonDraw m_btnUp; //�ϰ�ť
	CZButtonDraw m_btnDown; //�°�ť
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVisualManager();
};
