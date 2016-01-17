#pragma once
#include "CxArrayND.h"


class CCxNDArrayEditT : public CEdit
{
	DECLARE_DYNAMIC(CCxNDArrayEditT)

public:
	CCxNDArrayEditT();
	virtual ~CCxNDArrayEditT();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	DECLARE_MESSAGE_MAP()
public:
	CxArrayND * m_pArrayND;
};

//
class CCxNDArrayItem : public CBCGPGridItem
{
	friend class CCxNDArrayGrid;
	friend class CCxNDArrayRow;
	friend class CDlgNDArrayEditor;
	DECLARE_SERIAL(CCxNDArrayItem)
protected:
	CCxNDArrayItem ();
public:
	virtual ~CCxNDArrayItem();
public:
	CCxNDArrayItem(const _variant_t& varValue, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);
	virtual COLORREF OnFillBackground (CDC* pDC, CRect rect);
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual void OnDrawBorders (CDC* pDC, CRect rect);
	virtual BOOL OnUpdateValue ();
protected:
	virtual CEdit* NewInPlaceEdit ();
	void FormatDoubleValue( CString& strValue );
};

//
class CCxNDArrayRow : public CBCGPGridRow
{
	friend class CCxNDArrayGrid;
	friend class CCxNDArrayItem;
	DECLARE_SERIAL(CCxNDArrayRow)
public:
	// Group constructor
	CCxNDArrayRow (const CString& strGroupName, DWORD_PTR dwData = 0, int nBlockSize = 10);
	// Simple item
	CCxNDArrayRow (int nColumnsNum = 0, DWORD_PTR dwData = 0, int nBlockSize = 10);
	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);
	virtual void OnDrawItems (CDC* pDC, CRect rect);
};

// CCxNDArrayGrid

class CCxNDArrayGrid : public CBCGPGridCtrl
{
	friend class CCxNDArrayRow;
	friend class CCxNDArrayItem;
	DECLARE_DYNAMIC(CCxNDArrayGrid)

public:
	CCxNDArrayGrid();
	CCxNDArrayGrid(int nMemBlockSize);
	virtual ~CCxNDArrayGrid();

	BOOL Init( CRect& rc, CWnd * pParent );
	virtual CBCGPGridRow * CreateRow ();
	CCxNDArrayRow * CreateNewRow ();
	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);
	virtual BOOL DoDrawText (CDC* pDC, CString strText, CRect rect, UINT uiDrawTextFlags
		, LPRECT lpRectClip = NULL, BOOL bNoCalcExtent = FALSE);

public:
	CxArrayND * m_pArrayData;
	BOOL m_bModified;
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	virtual void SetRowHeight ();
	
	virtual void OnDraw(CDC* pDCPaint);
	virtual BOOL OnDrawItem (CDC* pDC, CBCGPGridRow* pItem);
	virtual void OnDrawLineNumber (CDC* pDC, CBCGPGridRow* pRow, CRect rect, BOOL bSelected, BOOL bPressed);

	virtual void FillHeaderPartBackground (CDC* pDC, const CBCGPGridHeaderParams& params);
	virtual void DrawHeaderPart (CDC* pDC, const CBCGPGridHeaderParams& params);

	virtual void OnRowHeaderDblClick (CBCGPGridRow* pRow, CPoint point, CRect rect)
	{
		return; //不处理
	}
	virtual void OnSelectAllClick ()
	{
		return; //不处理
	}
};


