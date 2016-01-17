#pragma once
#include <CxArrayND.h>

class CCxNDArrayDimItem;

class CCxNDArrayDimEditT : public CEdit
{
	DECLARE_DYNAMIC(CCxNDArrayDimEditT)

public:
	CCxNDArrayDimEditT();
	virtual ~CCxNDArrayDimEditT();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	DECLARE_MESSAGE_MAP()
public:
	CCxNDArrayDimItem * m_pItem;
	BOOL IsSelectedAll( void );
};

//
class CCxNDArrayDimItem : public CBCGPGridItem
{
	friend class CCxNDArrayDimEditT;
	friend class CCxNDArrayDimGrid;
	friend class CCxNDArrayDimRow;
	friend class CDlgNDArrayEditor;
	DECLARE_SERIAL(CCxNDArrayDimItem)
protected:
	CCxNDArrayDimItem ();
	virtual CEdit* NewInPlaceEdit ();
public:
	virtual ~CCxNDArrayDimItem();
public:
	CCxNDArrayDimItem(const _variant_t& varValue, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);
	virtual COLORREF OnFillBackground (CDC* pDC, CRect rect);
	//
	virtual BOOL OnEdit (LPPOINT lptClick);
	virtual CSpinButtonCtrl* CreateSpinControl (CRect rectSpin);
};

//
class CCxNDArrayDimRow : public CBCGPGridRow
{
	friend class CCxNDArrayDimGrid;
	friend class CCxNDArrayDimItem;
	friend class CDlgNDArrayEditor;
	DECLARE_SERIAL(CCxNDArrayDimRow)
public:
	// Group constructor
	CCxNDArrayDimRow (const CString& strGroupName, DWORD_PTR dwData = 0, int nBlockSize = 10);
	// Simple item
	CCxNDArrayDimRow (int nColumnsNum = 0, DWORD_PTR dwData = 0, int nBlockSize = 10);
	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);
	virtual void OnDrawItems (CDC* pDC, CRect rect);
};

// CCxNDArrayGrid

class CCxNDArrayDimGrid : public CBCGPGridCtrl
{
	friend class CCxNDArrayDimRow;
	friend class CCxNDArrayDimItem;
	friend class CDlgNDArrayEditor;
	DECLARE_DYNAMIC(CCxNDArrayDimGrid)

public:
	enum {
		COL_DIM_INDEX = 0,
		COL_DIM_TYPE,
		COL_DIM_VALUE
	};

	CCxNDArrayDimGrid();
	CCxNDArrayDimGrid(int nMemBlockSize);
	virtual ~CCxNDArrayDimGrid();

	BOOL Init( CRect& rc, CWnd * pParent );
	virtual CBCGPGridRow * CreateRow ();
	CCxNDArrayDimRow * CreateNewRow ();
	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);
	virtual BOOL EditItem (CBCGPGridRow* pItem, LPPOINT lptClick = NULL);

public:
	CxArrayND * m_pArrayData;
	BOOL m_bModified;
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

protected:
	virtual void SetRowHeight ();

	virtual void OnDraw(CDC* pDCPaint);
	virtual BOOL OnDrawItem (CDC* pDC, CBCGPGridRow* pItem);

	virtual void FillHeaderPartBackground (CDC* pDC, const CBCGPGridHeaderParams& params);
	virtual void DrawHeaderPart (CDC* pDC, const CBCGPGridHeaderParams& params);
};

