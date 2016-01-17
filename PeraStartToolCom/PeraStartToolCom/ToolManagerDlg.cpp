// ToolManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolManagerDlg.h"
#include "afxdialogex.h"


// CToolManagerDlg 对话框

IMPLEMENT_DYNAMIC(CToolManagerDlg, CCxBCGPDialog)

CToolManagerDlg::CToolManagerDlg(map<CString,ToolObject> toolmap,CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CToolManagerDlg::IDD, pParent)
{
	m_ToolMap = toolmap;
	m_iSortColumn = FALSE;
}

CToolManagerDlg::~CToolManagerDlg()
{
}

void CToolManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CToolManagerDlg, CCxBCGPDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CToolManagerDlg::OnNMClickList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, &CToolManagerDlg::OnLvnColumnclickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, &CToolManagerDlg::OnNMRClickList)
	ON_COMMAND(ID_DEL_ALL_SEL, &CToolManagerDlg::OnDelAllSel)
	ON_COMMAND(ID_DEL_ALL, &CToolManagerDlg::OnDelAll)
	ON_COMMAND(ID_RUN_CUR, &CToolManagerDlg::OnRunCur)
	ON_COMMAND(ID_CONFIG_CUR, &CToolManagerDlg::OnConfigCur)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_RUN_CUR, &CToolManagerDlg::OnUpdateRunCur)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_CUR, &CToolManagerDlg::OnUpdateConfigCur)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CToolManagerDlg::OnNMDblclkList)
END_MESSAGE_MAP()


// CToolManagerDlg 消息处理程序


BOOL CToolManagerDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT); // 添加网格属性
	m_ListCtrl.InsertColumn(0,"软件ID",LVCFMT_LEFT,50);
	m_ListCtrl.InsertColumn(1,"软件名称",LVCFMT_LEFT,100);
	m_ListCtrl.InsertColumn(2,"软件描述",LVCFMT_LEFT,120);
	m_ListCtrl.InsertColumn(3,"默认主键",LVCFMT_LEFT,120);
	m_ListCtrl.InsertColumn(4,"本地路径",LVCFMT_LEFT,350);
	m_ListCtrl.InsertColumn(5,"本地参数",LVCFMT_LEFT,150);
	m_bArrySortAscending[0] = FALSE;
	m_bArrySortAscending[1] = FALSE;
	m_bArrySortAscending[2] = FALSE;
	m_bArrySortAscending[3] = FALSE;
	m_bArrySortAscending[4] = FALSE;
	m_bArrySortAscending[5] = FALSE;

	int index = 0;
	map<CString,ToolObject>::iterator iter = m_ToolMap.begin();
	for (;iter!= m_ToolMap.end();iter++)
	{
		m_ListCtrl.InsertItem(index,iter->second.m_strID);
		m_ListCtrl.SetItemText(index,1,iter->second.m_strName);
		m_ListCtrl.SetItemText(index,2,iter->second.m_strDescription);
		m_ListCtrl.SetItemText(index,3,iter->second.m_strDefaultKey);
		m_ListCtrl.SetItemText(index,4,iter->second.m_strLocalPath);
		m_ListCtrl.SetItemText(index,5,iter->second.m_strLocalRunParam);
		m_ListCtrl.SetItemData(index,(DWORD_PTR)&(iter->second));
		index++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CToolManagerDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int iRow = pNMItemActivate->iItem;
	if (iRow<0)
		return;

	ToolObject *pTool = (ToolObject*)m_ListCtrl.GetItemData(iRow);
	*pResult = 0;
}

//排序函数实现
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CToolManagerDlg *pWnd = (CToolManagerDlg*)(lParamSort);
	CListCtrl & List = pWnd->m_ListCtrl;
	ToolObject t1 = *(ToolObject*)(lParam1);
	ToolObject t2 = *(ToolObject*)(lParam2); 
	int iCmp = 0;
	if (pWnd->m_iSortColumn==0)
		iCmp = t1.m_strID.Compare(t2.m_strID);
	else if (pWnd->m_iSortColumn==1)
		iCmp = t1.m_strName.Compare(t2.m_strName);
	else if (pWnd->m_iSortColumn==2)
		iCmp = t1.m_strDescription.Compare(t2.m_strDescription);
	else if (pWnd->m_iSortColumn==3)
		iCmp = t1.m_strDefaultKey.Compare(t2.m_strDefaultKey);
	else if (pWnd->m_iSortColumn==4)
		iCmp = t1.m_strLocalPath.Compare(t2.m_strLocalPath);
	else if (pWnd->m_iSortColumn==5)
		iCmp = t1.m_strLocalRunParam.Compare(t2.m_strLocalRunParam);
	else
		return 0;

	if (pWnd->m_bArrySortAscending[pWnd->m_iSortColumn])
	{
		if ( iCmp >0 )
			return 1;
		else if (iCmp < 0)
			return -1;
	}
	else
	{
		if ( iCmp >0 )
			return -1;
		else if (iCmp < 0)
			return 1;
	}

	return 0;
}
void CToolManagerDlg::OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_iSortColumn = pNMLV->iSubItem; // 设置排序的列
	m_ListCtrl.SortItems(ListCompare,(DWORD_PTR)this);
	m_bArrySortAscending[m_iSortColumn] = !m_bArrySortAscending[m_iSortColumn];
	*pResult = 0;
}


void CToolManagerDlg::OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu  *pSub = menu.GetSubMenu(0);
	CPoint pt;
	GetCursorPos(&pt);
	pSub->TrackPopupMenu(TPM_TOPALIGN|TPM_LEFTALIGN,pt.x,pt.y,this,NULL);
}


void CToolManagerDlg::OnDelAllSel()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (pos==NULL)
		return;

	if (MessageBox("确定要删除选中的配置？","提示",MB_YESNO|MB_ICONQUESTION)==IDNO)
		return;

	while (pos)
	{
		int iIndex  = m_ListCtrl.GetNextSelectedItem(pos);

		ToolObject *pTool = (ToolObject *)m_ListCtrl.GetItemData(iIndex);
		CToolsProUnity::DeleteToolConfigFromXml(pTool->m_strID); // 从xml里删除配置

		m_ListCtrl.DeleteItem(iIndex); // 删除列表控件上的该行
		pos = m_ListCtrl.GetFirstSelectedItemPosition();
	}
}


void CToolManagerDlg::OnDelAll()
{
	// TODO: 在此添加命令处理程序代码
	if (MessageBox("确定要删除所有的配置？","提示",MB_YESNO|MB_ICONQUESTION)==IDNO)
		return;

	m_ListCtrl.DeleteAllItems();
	CToolsProUnity::DeleteAllConfigFromXml();
}


void CToolManagerDlg::OnRunCur()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (pos==NULL)
		return;

	int iRow = int(pos) - 1;
	ToolObject * pTool = (ToolObject*)m_ListCtrl.GetItemData(iRow);
	if (CToolsProUnity::StartTool(pTool))
	{
		UpdateItem(iRow,pTool);
	}
}

void CToolManagerDlg::UpdateItem(int iRow,ToolObject *pTool)
{
	m_ListCtrl.SetItemText(iRow,4,pTool->m_strLocalPath);
	m_ListCtrl.SetItemText(iRow,5,pTool->m_strLocalRunParam);
	m_ListCtrl.SetItemData(iRow,(DWORD_PTR)pTool);
}
void CToolManagerDlg::OnConfigCur()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (pos==NULL)
		return;

	int iRow = int(pos) - 1;
	ToolObject * pTool = (ToolObject*)m_ListCtrl.GetItemData(iRow);
	if (CToolsProUnity::ConfigTool(pTool))
	{
		UpdateItem(iRow,pTool);
		MessageBox("配置成功!","提示",MB_OK|MB_ICONINFORMATION);
	}
}


void CToolManagerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CCxBCGPDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: 在此处添加消息处理程序代码
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child Windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}


void CToolManagerDlg::OnUpdateRunCur(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	int iSelCount = m_ListCtrl.GetSelectedCount();
	if (iSelCount >1 || iSelCount==0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}


void CToolManagerDlg::OnUpdateConfigCur(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	int iSelCount = m_ListCtrl.GetSelectedCount();
	if (iSelCount >1 || iSelCount==0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}


void CToolManagerDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	OnConfigCur(); //双击执行配置
	*pResult = 0;
}
