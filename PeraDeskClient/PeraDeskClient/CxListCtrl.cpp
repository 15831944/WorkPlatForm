// CxListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "CxListCtrl.h"

BEGIN_MESSAGE_MAP(CxListCtrl, CListCtrl)
	//在这里写了这个消息影射后，对话框上将不在触发列表控件的右键点击事件。
	//添加的菜单消息处理函数也不会被响应。
	//ON_NOTIFY_REFLECT(NM_RCLICK, &CxListCtrl::OnNMRClick)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CxListCtrl, CListCtrl)

CxListCtrl::CxListCtrl()
{
	m_bInited = FALSE;
	m_hwndRecvMsg = NULL;
	m_bReadOnly = FALSE;
}

CxListCtrl::~CxListCtrl()
{
}

BOOL CxListCtrl::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_RBUTTONDOWN )
	//{
	//	if(pMsg->hwnd == m_hWnd)
	//	{
	//		return TRUE;
	//	}
	//}
	//判断是否为键盘消息
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		UINT nRepCnt = LOWORD(pMsg->lParam);
		UINT nFlags = HIWORD(pMsg->lParam);
		//是否持续按键（上次按住后，一直没有抬起来）
		BOOL bHolding = ((nFlags & 0x4000) == 0x4000);

		//如果是 Ctrl + 下 则向下移动 Item
		if(GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == VK_DOWN 
			&& !bHolding && !m_bReadOnly)
		{
			OnItemMoveBack();
			return TRUE;
		}
		//如果是 Ctrl + 上 则向上移动 Item
		if(GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == VK_UP 
			&& !bHolding && !m_bReadOnly)
		{
			OnItemMoveForward();
			return TRUE;
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


BOOL CxListCtrl::Init( void )
{
	if ( m_bInited ) return FALSE;
	if ( !OnInit() ) return FALSE;
	m_bInited = TRUE;
	return TRUE;
}


void CxListCtrl::Release( void )
{
	m_vItemDataPtr.clear();
	m_bInited = FALSE;
	m_hwndRecvMsg = NULL;
}


BOOL CxListCtrl::OnInit( void )
{
	return TRUE;
}


void CxListCtrl::Clear( void )
{
	DeleteAllItems();
}

//是否为报表模式
BOOL CxListCtrl::IsReportMode( void )
{
	DWORD dwStyle = GetStyle() & LVS_TYPEMASK;
	if (dwStyle == LVS_REPORT) return TRUE;
	return FALSE;
}

//是否为图片模式
BOOL CxListCtrl::IsImageMode( void )
{
	DWORD dwStyle = GetStyle() & LVS_TYPEMASK;
	if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON) return TRUE;
	return FALSE;
}

/*
	设置为报表模式。
参数：
	lStyle：
	lExStyle：
*/
BOOL CxListCtrl::SetReportMode( LONG lStyle, LONG lExStyle )
{
	if (IsInitialed() && IsImageMode()) return FALSE;
	if ( !IsWindow(m_hWnd) ) return FALSE;

	//LVS_SHOWSELALWAYS //没有焦点，也高亮显示选中的项（否则失去焦点，选中项回复普通状态）
	//LONG lExStyle = LVS_EX_FULLROWSELECT //全行选择
	DWORD lNewStyle = 0;
	lNewStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lNewStyle &= ~LVS_TYPEMASK;
	lNewStyle |= LVS_REPORT; //设置为 报表 模式
	lNewStyle |= lStyle;
	SetWindowLong(m_hWnd, GWL_STYLE, lNewStyle);

	//这几句话不管用，不知道原因
	//LONG lExStyle = 0;
	//lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	//lExStyle |= LVS_EX_FULLROWSELECT; //全行选择
	//lExStyle |= LVS_EX_GRIDLINES; //网格
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

	DWORD dwNewExStyle = 0;
	dwNewExStyle |= lExStyle;
	SetExtendedStyle(dwNewExStyle);

	return TRUE;
}

/*
	设置为图片模式。
参数：
	lStyle：
	lExStyle：
*/
BOOL CxListCtrl::SetImageMode( LONG lStyle, LONG lExStyle )
{
	if (IsInitialed() && IsReportMode()) return FALSE;
	if ( !IsWindow(m_hWnd) ) return FALSE;

	LONG lNewStyle;
	lNewStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lNewStyle &= ~LVS_TYPEMASK;
	lNewStyle |= LVS_ICON; //设置 大图标 模式
	lNewStyle &= ~LVS_ALIGNMASK;
	lNewStyle |= LVS_ALIGNTOP; //设置 上对齐
	lNewStyle |= LVS_AUTOARRANGE; // 横着排满一行后，自动换行
	//lNewStyle |= LVS_SHOWSELALWAYS; //没有焦点，也高亮显示选中的项（否则失去焦点，选中项回复普通状态）
	//lNewStyle |= LVS_OWNERDRAWFIXED; //
	lNewStyle |= lStyle;
	SetWindowLong(m_hWnd, GWL_STYLE, lNewStyle);

	//设置列表扩展类型
	DWORD dwNewExStyle = 0;
	//dwNewExStyle |= LVS_EX_FULLROWSELECT;
	//dwNewExStyle |= LVS_EX_GRIDLINES;
	dwNewExStyle |= lExStyle;
	SetExtendedStyle(dwNewExStyle);
	return TRUE;
}

/*
	取得当前选中项目的索引号（顺序）
*/
int CxListCtrl::GetCurSel( vector<int>& vItems )
{
	POSITION pos = NULL;
	int nItem = -1;

	vItems.clear();

	pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		nItem = GetNextSelectedItem(pos);
		vItems.push_back(nItem);
	}
	sort(vItems.begin(), vItems.end());
	return (int)vItems.size();
}

//取得当前选中项目的参数（顺序）
int CxListCtrl::GetCurSelParam( vector<LPARAM>& vParams )
{
	vector<int> vItems;
	LPARAM lParam = 0;

	vParams.clear();
	if (0 == GetCurSel(vItems)) return 0;
	for (int i = (int)vItems.size() - 1; i >= 0; i--)
	{
		lParam = GetItemData(vItems[i]);
		vParams.push_back(lParam);
	}
	return (int)vParams.size();
}

//将指定索引号的项目设置为选中状态
void CxListCtrl::SetCurSel( vector<int>& vItems )
{
	POSITION pos = NULL;
	int nItem = -1;

	ClearCurSel();

	SetFocus(); //不加这句，无法实时更新状态

	sort(vItems.begin(), vItems.end());

	nItem = -1;
	for (int i=0; i<(int)vItems.size(); i++)
	{
		SetItemState(vItems[i], LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		RedrawItems(nItem, nItem);
		nItem = vItems[i];
	}
	if (nItem != -1)
	{
		EnsureVisible(nItem, FALSE);
	}
}

//清除选中状态。
int CxListCtrl::ClearCurSel( vector<int> * pvItemIndex )
{
	POSITION pos = NULL;
	vector<int> vSel;
	int nItem = -1;

	GetCurSel(vSel);

	if (pvItemIndex != NULL)
	{
		pvItemIndex->clear();
		*pvItemIndex = vSel;
	}

	pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		nItem = GetNextSelectedItem(pos);
		SetItemState(nItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
		RedrawItems(nItem, nItem); //只有图片需要这句？
	}

	return (int)vSel.size();
}

//设置 单选 或者 多选 模式
BOOL CxListCtrl::SetMultiSelMode( BOOL bEnabled /*= TRUE*/ )
{
	LONG lStyle;
	lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if (bEnabled)
		lStyle &= ~LVS_SINGLESEL;
	else
		lStyle |= LVS_SINGLESEL;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	return TRUE;
}

int CxListCtrl::DeleteCurSel( vector<LPARAM> * pvParams )
{
	vector<int> vSel;
	DWORD dwStyle = GetStyle();

	if (pvParams != NULL)
	{
		pvParams->clear();
	}

	if (0 == GetCurSel(vSel)) return 0;

	if (pvParams != NULL)
	{
		for (int i=0; i<(int)vSel.size(); i++)
		{
			pvParams->push_back( GetItemData( vSel[i] ) );
		}
	}

	for (int i = (int)vSel.size() - 1; i >= 0; i--)
	{
		DeleteItem(vSel[i]);
	}

	if (IsImageMode())
	{
		Arrange(LVS_ALIGNLEFT); //不加这句，界面不会刷新
	}
	return (int)vSel.size();
}

//
BOOL CxListCtrl::GetClickItemPos(int& nRow, int& nCol)
{
	nRow = -1;
	nCol = -1;

	LVHITTESTINFO hti;
	memset(&hti, NULL, sizeof(LVHITTESTINFO));
	hti.flags = LVHT_ABOVE;

	GetCursorPos(&(hti.pt));
	ScreenToClient(&hti.pt);
	
	//检测 Item 是否选中
	nRow = HitTest(&hti);
	if (nRow >= 0)
	{
		nCol = hti.iSubItem;
		return TRUE;
	}

	//检测 SubItem 是否选中 （如果有 LVS_EX_FULLROWSELECT 标志，则子项无效）
	nRow = SubItemHitTest(&hti);
	if (nRow >= 0)
	{
		nCol = hti.iSubItem;
		return TRUE;
	}

	return FALSE;
}

int CxListCtrl::AddRow( LPCTSTR lpszText, LPARAM lpParam /*= NULL*/, int nRow /*= -1*/ )
{
	CString strText = lpszText == NULL ? "" : lpszText;
	if (!m_bInited) return -1;

	LVITEM lvi;
	memset(&lvi, NULL, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iItem = nRow >= 0 ? nRow : GetItemCount();
	lvi.pszText = (LPTSTR)(LPCTSTR)strText;
	lvi.lParam = lpParam;
	return InsertItem(&lvi);
	//return GetItemCount();
}

BOOL CxListCtrl::AddColumn( int nRow, int nCol, LPCTSTR lpszText )
{
	return SetItemText(nRow, nCol, lpszText);
	//LVITEM lvi;
	//memset(&lvi, NULL, sizeof(LVITEM));
	//lvi.mask = LVIF_PARAM | LVIF_TEXT | LVIF_COLUMNS;
	//lvi.lParam = lpParam;
	//lvi.pszText = (LPSTR)(LPCTSTR)lpszText;
	//lvi.cColumns = nCol;
	//lvi.iItem = nRow;
	//nRow = InsertItem(&lvi);
	//if (nRow != -1) return TRUE;
	//else return FALSE;
}

void CxListCtrl::OnItemMoveForward( void )
{
	vector<int> vCurSel, vNewSel;
	LPVOID lpData = NULL;

	//当前选中的列表项
	if (0 == GetCurSel(vCurSel)) return;
	//更新调整顺序的数据列表，确保 m_vItemDataPtr 中的数据 与列表中的数据一一对应
	if (0 == OnUpdateItemDataPtr()) return;
	vNewSel = vCurSel;

	//顺序遍历
	for (int i=0; i<(int)vCurSel.size(); i++)
	{
		if (0 == vCurSel[i]) continue; //已经是第一条记录，无需移动
		//前后交换
		lpData = m_vItemDataPtr[ vCurSel[i] ];
		m_vItemDataPtr[ vCurSel[i] ] = m_vItemDataPtr[ vCurSel[i] - 1 ];
		m_vItemDataPtr[ vCurSel[i] - 1 ] = lpData;
		vNewSel[i] = vNewSel[i] - 1;
	}
	OnItemMoved(vNewSel);
}

void CxListCtrl::OnItemMoveBack( void )
{
	vector<int> vCurSel, vNewSel;
	LPVOID lpData = NULL;

	//当前选中的列表项
	if (0 == GetCurSel(vCurSel)) return;
	//更新调整顺序的数据列表，确保 m_vItemDataPtr 中的数据 与列表中的数据一一对应
	if (0 == OnUpdateItemDataPtr()) return;
	vNewSel = vCurSel;

	//倒序遍历
	for (int i = vCurSel.size() - 1; i >= 0; i--)
	{
		if (GetItemCount() - 1 <= vCurSel[i]) continue; //已经是最后一条记录，无需移动
		lpData = m_vItemDataPtr[ vCurSel[i] ];
		m_vItemDataPtr[ vCurSel[i] ] = m_vItemDataPtr[ vCurSel[i] + 1 ];
		m_vItemDataPtr[ vCurSel[i] + 1 ] = lpData;
		vNewSel[i] = vNewSel[i] + 1;
	}
	OnItemMoved(vNewSel);
}

int CxListCtrl::OnUpdateItemDataPtr( void )
{
	return 0;
}

void CxListCtrl::OnItemMoved( vector<int>& vNewSel )
{

}
