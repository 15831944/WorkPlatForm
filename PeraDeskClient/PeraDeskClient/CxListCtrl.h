#pragma once
#include <afxwin.h>
#include "StlHeader.h"
#include "GDIPlusInit.h"


/*
说明：
CxListCtrl 类主要提供一些方法，简化了 CListCtrl 的操作，可以直接用于原始的 CListCtrl 对象。
CxListCtrl 派生2个类，用于显示图片的 CxListCtrlImage，和以网格形式显示的 CxListCtrlReport。
CxListCtrlImage 和 CxListCtrlReport 都自定义绘制实现。
*/

// CxListCtrl

class CxListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CxListCtrl)

public:
	CxListCtrl();
	virtual ~CxListCtrl();
	
	//初始化
	BOOL Init(void);

	//清除所有数据，调用之后，可以重新初始化。
	virtual void Release(void);
	//清除表格中的数据，不删除初始化的数据
	virtual void Clear(void);

	//设置接收消息的窗口句柄（表格给父窗口发送的消息）
	//目的是防止父窗口不为消息处理窗口时的情况
	void SetRecvMsgWindow(HWND hwnd)
	{
		if ( !IsWindow(hwnd) ) return;
		m_hwndRecvMsg = hwnd;
	}

	//是否已经初始化
	BOOL IsInitialed(void) const { return m_bInited; }
	//是表格模式
	BOOL IsReportMode(void);
	//是图片模式
	BOOL IsImageMode(void);

	//设置为报表模式
	BOOL SetReportMode( LONG lStyle = 0, LONG lExStyle = 0 );
	//设置为图片模式
	BOOL SetImageMode( LONG lStyle = 0, LONG lExStyle = 0 );

	//取得选择的记录索引号
	int GetCurSel( vector<int>& vItemIndex );
	//取得选择的记录参数
	int GetCurSelParam( vector<LPARAM>& vParams );
	//设置指定索引的记录为选中状态
	void SetCurSel( vector<int>& vItems );
	//清除选中状态
	virtual int ClearCurSel( vector<int> * pvItemIndex = NULL );
	//设置是否可以多选
	BOOL SetMultiSelMode(BOOL bMultiSel = TRUE);

	//删除选中的条目，返回每个条目对应的参数
	virtual int DeleteCurSel( vector<LPARAM> * pvParams = NULL );
	//取得鼠标点击处的 Item 或者 SubItem
	BOOL GetClickItemPos(int& nRow, int& nCol);

	//添加一行新数据（一个Item）
	virtual int AddRow(LPCTSTR lpszText, LPARAM lpParam = NULL, int nRow = -1);
	//添加一列数据（一个SubItem，前提是要先添加行数据）
	virtual BOOL AddColumn(int nRow, int nCol, LPCTSTR lpszText);



protected:
	//初始化时
	virtual BOOL OnInit(void);
	//向前移动一个Item
	virtual void OnItemMoveForward(void);
	//向后移动一个Item
	virtual void OnItemMoveBack(void);
	//移动 Item 前 更新列表数据，保持跟列表一致
	virtual int OnUpdateItemDataPtr(void);
	//移动 Item 完成
	virtual void OnItemMoved( vector<int>& vNewSel );

public:
	//是否能通过 【Ctrl + 上】或者 【Ctrl + 下】 组合键移动 Item
	BOOL m_bReadOnly;

protected:
	BOOL m_bInited;
	//
	HWND m_hwndRecvMsg;
	//列表中的数据对象，仅在 移动 Item 事件中使用
	vector<LPVOID> m_vItemDataPtr;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
};


