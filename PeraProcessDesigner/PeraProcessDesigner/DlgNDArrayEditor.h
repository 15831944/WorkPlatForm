#pragma once
#include "afxwin.h"
#include <BCGPGridCtrl.h>
#include "CxArrayBase.h"
#include "CxNDArrayGrid.h"
#include "CxNDArrayDimGrid.h"


// CDlgNDArrayEditor 对话框

class CDlgNDArrayEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNDArrayEditor)

public:
	enum {
		COL_DIM_INDEX = 0,
		COL_DIM_TYPE,
		COL_DIM_VALUE
	};
	CDlgNDArrayEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgNDArrayEditor();

	void UpdateContent(void);
	void UpdateDim(void);

// 对话框数据
	enum { IDD = IDD_NDARRAY_EDITOR };

protected:
	void EnsureOneRowOnColumn( int nClickRow, LPCTSTR lpszOldValue );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedNaeCancel();
	afx_msg LRESULT OnGridSelChanged( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnGridItemChanged( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnGridBeginInplaceEdit( WPARAM wp, LPARAM lp );
	afx_msg LRESULT OnGridEndInplaceEdit( WPARAM wp, LPARAM lp );
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_wndDummyList;
	CStatic m_wndDummyDim;
	CButton m_wndOk;
	CButton m_wndCancel;

	CCxNDArrayGrid m_wndList;
	CCxNDArrayDimGrid m_wndDim;
	CxArrayND * m_pArrayData;
	CxArrayND * m_pArrayDataCopied;
	CCxNDArrayDimRow * m_pLastDimTypeRow;
	CString m_strLastDimType;
	afx_msg void OnBnClickedNaeOk();
	CButton m_wndResize;
	afx_msg void OnBnClickedNaeResize();
};
