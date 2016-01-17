#pragma once
#include <BCGPGridCtrl.h>
#include "CxStructTreeNode.h"
#include "CxStructTreeToolTipCtrl.h"
#include "CxStructTree.h"
#include "CxStructTreeNodeComponent.h"

class CCxStructTreeItemT;

class CCxStructTreeEditT : public CEdit
{
	DECLARE_DYNAMIC(CCxStructTreeEditT)

public:
	CCxStructTreeEditT();
	virtual ~CCxStructTreeEditT();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	DECLARE_MESSAGE_MAP()
public:
	CCxStructTreeItemT * m_pItem;
};

class CCxStructTreeItemT : public CBCGPGridItem
{
	DECLARE_SERIAL(CCxStructTreeItemT)
	friend class CCxStructTreeGridT;
	friend class CCxStructTreeRowT;
protected:
	CCxStructTreeItemT ();
public:
	virtual ~CCxStructTreeItemT();
public:
	CCxStructTreeItemT(const _variant_t& varValue, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);
	virtual CString GetValueTooltip ();
	virtual COLORREF OnFillBackground (CDC* pDC, CRect rect);
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual BOOL OnUpdateValue ();
protected:
	virtual CEdit* NewInPlaceEdit ();
	void FormatDoubleValue( CString& strValue );

};

class CCxStructTreeRowT : public CBCGPGridRow
{
	DECLARE_SERIAL(CCxStructTreeRowT)

	friend class CCxStructTreeItemT;
	friend class CCxStructTreeGridT;
public:
	// Group constructor
	CCxStructTreeRowT (const CString& strGroupName, DWORD_PTR dwData = 0, int nBlockSize = 10);

	// Simple item
	CCxStructTreeRowT (int nColumnsNum = 0, DWORD_PTR dwData = 0, int nBlockSize = 10);

	BOOL AddSubItem (CBCGPGridRow* pItem, int nPos = -1, BOOL bRedraw = TRUE);

	virtual void OnDrawExpandBox (CDC* pDC, CRect rectExpand);
	virtual void OnDrawItems (CDC* pDC, CRect rect);

	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);

	//BOOL m_bCustomExpandFlag; //自定义的展开标记，再添加节点时，决定是否展开。
	virtual void OnExpand (BOOL /*bExpand*/);
};

// CCxBCGPGridCtrl

class CCxStructTreeGridT : public CBCGPGridCtrl
{
	friend class CCxStructTreeItemT;
	friend class CCxStructTreeRowT;
	friend class CBarStructTreeT;
	friend class CCxStructTree;

	DECLARE_DYNAMIC(CCxStructTreeGridT)

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

	CCxStructTreeGridT();
	CCxStructTreeGridT(int nMemBlockSize);
	virtual ~CCxStructTreeGridT();

	BOOL InitTree( CRect& rc, CWnd * pParent );

	virtual CBCGPGridRow * CreateRow ();

	BOOL m_bModified;
	CMenu m_HeaderMenu;
	CCxStructTree m_StructTree;

	CCxStructTreeRowT * CreateNewRow ();

	//int AddRoot(CCxStructTreeNode * pNode);
	//CBCGPGridRow * GetRoot(LPCTSTR lpszId);

	//int AddComponent( CCxStructTreeNode * pNode, LPCTSTR lpszRootId );
	//CBCGPGridRow * GetComponent(LPCTSTR lpszId);

	//int AddParam( CCxStructTreeNode * pNode, LPCTSTR lpszComponentId );
	//CBCGPGridRow * GetParam(LPCTSTR lpszId);

	//BOOL AddStructTreeNode( CCxStructTreeNode * pNode );

	CCxStructTreeRowT * GetNodeRow( LPCTSTR lpszId );
	CCxStructTreeNode * GetCurSelNodeData(void);



	virtual void RebuildIndexes (int nStartFrom = -1);

	virtual CBCGPGridItem* CreateItem (int nRow, int nColumn);

	BOOL m_bSetCapture;

	virtual BOOL DoDrawText (CDC* pDC, CString strText, CRect rect, UINT uiDrawTextFlags
		, LPRECT lpRectClip = NULL, BOOL bNoCalcExtent = FALSE);

	//virtual BOOL EndEditItem (BOOL bUpdateData = TRUE);

	//map <long, CCxStructTreeNode *> m_mapData;
	//void AddTreeNodeData(CCxStructTreeNode * pData);
	//void DeleteTreeNodeData(CCxStructTreeNode * pData);
	//CCxStructTreeNode * GetTreeNodeData(long lID);
	//void ClearTreeNodeData(void);
	//void UpdateTree(void);

	CImageList m_Images;
	CCxStructTreeToolTipCtrl m_ToolTip2;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMenuJianmoTreeComponentRun();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg void OnMenuStructTreePublishComponent();
	afx_msg void OnMenuStructTreePublishProcess();
	afx_msg void OnMenuJianmoTreeHeaderValue();
	afx_msg void OnMenuJianmoTreeHeaderType();
	afx_msg void OnMenuJianmoTreeHeaderDirection();
	afx_msg LRESULT OnUpdateDataMapping(WPARAM, LPARAM);
	BOOL LoadImage();
	void ClearImage(void);
	int GetInsertRowPos( CCxStructTreeRowT * pParentRow , CCxStructTreeRowT * pCurRow, int nChildIndex );

	
	BOOL UpdateParamValue( LPVOID lpNewValue, CCxStructTreeRowT * pCurRow );
	vector< Bitmap * > m_vImg;
	HBITMAP m_hbmp;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void PublishComponent( CCxStructTreeNodeComponent * pNode );

	virtual void OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn);

	//void UpdateParamValueBg( void );
protected:
	virtual void SetRowHeight ();
	virtual BOOL OnDrawItem (CDC* pDC, CBCGPGridRow* pItem);
	//virtual void OnFillLeftOffset (CDC* pDC, CRect rectFill, CBCGPGridRow* pRow,
	//	BOOL bDrawRightBorder = TRUE);
	void DrawTreeLine(CDC* pDC, CCxStructTreeRowT* pCurRow, CRect rcLeftOffset);
	afx_msg void OnMenuJianmoTreeComponentProp();
	afx_msg void OnMenuJianmoTreeProcessProp();
	afx_msg void OnMenuJianmoTreeFlowProp();
	afx_msg void OnMenuJianmoTreeComponentDataMapping();
	afx_msg void OnMenuJianmoTreeComponentDelete();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void TrackToolTip (CPoint point);
	void TrackToolTip2 (BOOL bShow = TRUE);

	virtual BOOL DoRemoveRow (POSITION posDel, BOOL bRedraw);
	
	virtual void OnDrawHeader (CDC* pDC);
	virtual void OnDraw(CDC* pDCPaint);
	virtual void OnDrawHeaderItemBorder (CDC* pDC, CRect rect, int nCol);
	virtual void DrawHeaderPart (CDC* pDC, const CBCGPGridHeaderParams& params);
	virtual void DrawHeaderItem (CDC* pDC, CRect rect, CBCGPHeaderItem* pHeaderItem);
	virtual CRect OnGetHeaderRect (CDC* pDC, const CRect& rectDraw);

	virtual void OnFillLeftOffset (CDC* pDC, CRect rectFill, CBCGPGridRow* pRow,
		BOOL bDrawRightBorder = TRUE);

	virtual BOOL DoClickValue (CBCGPGridItem* pItem, UINT uiMsg, CPoint point, 
		BOOL bFirstClick, BOOL bButtonClick);

	virtual void OnBeginInplaceEdit (CBCGPGridItem* pItem);
	virtual int GetExtraHierarchyOffset ()const {return 20;}
	virtual int GetHierarchyLevelOffset () const { return 20;}

public:
	afx_msg void OnMenuJianmoTreeProcessEdit();
	afx_msg void OnMenuJianmoTreeProcessEditFor();
	afx_msg void OnMenuJianmoTreeProcessRun();
	afx_msg void OnMenuStructTreeProcessDataMapping();
	afx_msg void OnMenuJianmoTreeFlowEdit();
	afx_msg void OnMenuJianmoTreeFlowEditIf();
	afx_msg void OnMenuJianmoTreeFlowRun();
	afx_msg void OnMenuJianmoTreeFlowDelete();
	afx_msg void OnMenuStructTreePublishFlow();
	afx_msg void OnMenuStructTreeFlowSaveAs();
	afx_msg void OnMenuStructTreeFlowDataMapping();
	afx_msg void OnMenuJianmoTreeComponentEdit();
	afx_msg void OnMenuStructTreeComponentSaveAs();
	afx_msg void OnMenuJianmoTreeFormEdit();
	afx_msg void OnMenuJianmoTreeFormDelete();
	afx_msg void OnUpdateHeaderValueStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHeaderTypeStatus(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHeaderDirectionStatus(CCmdUI* pCmdUI);
	afx_msg void OnMenuJianmoTreeHeaderUnits();
	afx_msg void OnUpdateHeaderUnitsStatus(CCmdUI* pCmdUI);
	void UpdateSelStatus(CCxStructTreeRowT *& pCurRow );
	BOOL m_bMouseHover;
	BOOL m_bTrackMouseHover;

	BOOL UpdateRowByNode( CCxStructTreeRowT * pRow, CCxStructTreeNode * pNode );

	void SetComponentImage( CCxStructTreeNodeComponent * pNode /*, CCxStructTreeRowT * pRow*/ );
	void SetParamImage( CCxStructTreeNodeParam * pNode/* , CCxStructTreeRowT * pRow */);
	void SetRowImage( CCxStructTreeNode * pNode/* , CCxStructTreeRowT * pRow*/ );

	BOOL Refresh( void );
	BOOL AddNode( CCxStructTreeNode * pSpecNode, CCxStructTreeRowT * pSpecParentRow
		, int nIndex = -1, BOOL bRecursion = TRUE );
	BOOL DeleteNode( CCxStructTreeNode * pSpecNode );
	//BOOL EditNode( CCxStructTreeNode * pSpecNode );
	//BOOL MoveNode( CCxStructTreeMoveEventData & MoveData );
	//void AddReplaceNode( CStructTreeReplaceData & ReplaceData );
	//void MoveReplaceNode( CStructTreeReplaceData & ReplaceData );
	//BOOL SelectNode( CCxStructTreeNode * pSpecNode );
	void UpdateDataMapping(void);
	CCxStructTreeRowT * FindRow( LPCTSTR lpszNodeId );
	BOOL SwitchRow( CCxStructTreeRowT * pRow1, CCxStructTreeRowT * pRow2 );

	BOOL CanAddNewRow( CCxStructTreeNode* pNode );
	BOOL CanRemoveRow( CCxStructTreeNode* pNode );

	virtual BOOL SetCurSel (CBCGPGridItemID idItem,
		DWORD dwSelMode = SM_SINGE_SEL_GROUP | SM_SINGLE_ITEM, 
		BOOL bRedraw = TRUE);

	void ShowPopMenuForSvg(CCxStructTreePopMenuData* popd);
	void TellSvgFormShowStateChange(CString strId,BOOL bShowForm);
protected:
	CBCGPGridItemID m_idRealSelItem;
	//BOOL m_bTreeClicked;
public:
	afx_msg void OnJianmoTreeFormPopform();
	afx_msg void OnJianmoTreeFormNotpopform();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	afx_msg void OnUpdateJianmoTreeFormPopform(CCmdUI *pCmdUI);
	afx_msg void OnUpdateJianmoTreeFormNotpopform(CCmdUI *pCmdUI);
};


