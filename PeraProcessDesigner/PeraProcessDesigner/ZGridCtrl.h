#pragma once
#include "bcgpgridctrl.h"
//#include "StructTreeNodeData.h"
#include "CxStructTreeNode.h"
#include "CxStructTreeNodeComponent.h"

class CZGridRow;

class CZGridItem : public CBCGPGridItem
{
	DECLARE_SERIAL(CZGridItem)
	friend class CZGridCtrl;
	friend class CZGridRow;
protected:
	CZGridItem ();
public:
	virtual ~CZGridItem();
public:
	CZGridItem(const _variant_t& varValue, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);
	virtual CString GetValueTooltip ();
	virtual COLORREF OnFillBackground (CDC* pDC, CRect rect);
	virtual BOOL OnSetCursor () const
	{
		return FALSE; /* Use default */	
	}

	//*****************************************************************************
	virtual BOOL OnEdit (LPPOINT);
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
};


class CZGridRow : public CBCGPGridRow
{
	DECLARE_SERIAL(CZGridRow)

	friend class CZGridItem;
	friend class CZGridCtrl;
public:
	// Group constructor
	CZGridRow (const CString& strGroupName, DWORD_PTR dwData = 0, int nBlockSize = 10);

	// Simple item
	CZGridRow (int nColumnsNum = 0, DWORD_PTR dwData = 0, int nBlockSize = 10);

	BOOL AddSubItem (CBCGPGridRow* pItem, int nPos = -1, BOOL bRedraw = TRUE);

	virtual void OnDrawExpandBox (CDC* pDC, CRect rectExpand);
	virtual void OnDrawItems (CDC* pDC, CRect rect);

	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);
};

class CZGridCtrl :
	public CBCGPGridCtrl
{
	friend class CZGridItem;
	friend class CZGridRow;

	DECLARE_DYNAMIC(CZGridCtrl)

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnDrawItem (CDC* pDC, CBCGPGridRow* pItem);
	virtual void DrawTreeLine(CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset);
	virtual void SetRowHeight ();
	virtual int GetExtraHierarchyOffset ()const {return 20;}
	virtual int GetHierarchyLevelOffset () const { return 20;}
public:

	enum {
		COL_NAME = 0
		, COL_VALUE
		, COL_TYPE
		, COL_DIR
		, COL_UNITS
	};

	enum {
		IMG_ROOT = 0,
		IMG_EMPTY,
		//subflow
		IMG_IF,
		IMG_FOR,
		IMG_PARALLEL,
		IMG_SEQUENCE,
		IMG_SUBFLOW,
		IMG_FRONTFORM,
		IMG_REARFORM,
		IMG_GROUP,
		//param
		IMG_PARAM_IN,
		IMG_PARAM_IN_INVALID,
		IMG_PARAM_OUT,
		IMG_PARAM_OUT_INVALID,
		IMG_PARAM_INOUT,
		IMG_PARAM_INOUT_INVALID,
		// robot
		IMG_SCRIPT_DRIVER,
		IMG_DATA_PARSER,
		IMG_EXCEL,
		IMG_TOOL_INTEGRATION,
		IMG_WORD,
		IMG_FORMULA,
		IMG_FORM,
		IMG_DOCUMENT,
		IMG_PATRAN,
		IMG_WORKBENCH,
		IMG_CATIA,
		IMG_SCRIPT,
#ifdef PROJECT_203
		IMG_UG,
#endif
		IMG_CUSTOM
	};
	CZGridCtrl(void);
	virtual ~CZGridCtrl(void);
	BOOL InitTree( CRect& rc, CWnd * pParent );
	BOOL LoadImage();
	void ClearImage( void );

	void SetParamImage( CCxStructTreeNodeParam * pNode , CZGridRow * pRow );
	int GetInsertRowPos( CZGridRow * pParentRow , CZGridRow * pCurRow, int nChildIndex );
	void SetComponentImage( CCxStructTreeNodeComponent * pNode , CZGridRow * pRow );
	CZGridRow * GetNodeRow( LPCTSTR lpszId );
	virtual CBCGPGridRow * CreateRow ();
	CZGridRow * CreateNewRow();
	CBCGPGridItem* CreateItem (int nRow, int nColumn);

	BOOL AddNode( CCxStructTreeNode * pSpecNode, CZGridRow * pSpecParentRow ,  int nIndex = -1, BOOL bRecursion = TRUE );
	//分支不做为子流程，意味着分支节点没有参数，表达式设置时可以看到其所属黑盒的参数20140321需求明确要求
	//使用此方法，在向下遍历时，到子流程节点即停止
	BOOL AddNodeSubFlow( CCxStructTreeNode * pSpecNode, CZGridRow * pSpecParentRow ,  int nIndex = -1, BOOL bRecursion = TRUE );
	virtual BOOL UpdateRowByNode( CZGridRow * pRow, CCxStructTreeNode * pNode );
	void SetRowImage( CCxStructTreeNode * pNode , CZGridRow * pRow );

	CImageList m_Images;
	vector< Bitmap * > m_vImg;
	HBITMAP m_hbmp;

	BOOL m_IsShowRoot; //显示整棵树

	BOOL IsShowRoot() const { return m_IsShowRoot; }
	void IsShowRoot(BOOL val) { m_IsShowRoot = val; }
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

