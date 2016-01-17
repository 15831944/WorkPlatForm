// DlgResLibTree.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgResLibTree.h"
//#include "afxdialogex.h"
//#include "Resource.h"
//#include <EncodedTool.h>
//#include <RobotPrivateEntity.h>
//#include <RobotResDB.h>
#include <ResLibData.h>
#include "MainFrm.h"
#include "VorxThread.h"
#include <ReadRobotDB.h>

//#include <ToolResData.h>
//
//
//#include "RobotPrivateDB.h"
//#include "RobotResDB.h"
//#include "RobotPrivateEntity.h"
//#include "EncodedTool.h"
//#include <atlstr.h>
//#include "ProcessDB.h"
//#include "ComponentDB.h"
//
//#include "BobotEntity.h"
//#include <string>
//#include "RobotDbBase.h"
//#include "BobotManage.h"
//#include "FileTool.h"
//#include <winuser.h>

// CDlgResLibTree 对话框
extern CReadRobotDB g_ReadRobotDB;

BEGIN_MESSAGE_MAP(CDlgResLibTree, CDialogEx)
	//ON_BN_CLICKED(IDC_BUTOPEN, &CDlgResLibTree::OnBnClickedButopen)
	//ON_NOTIFY(NM_CLICK, IDC_RESLIB_TREE, &CDlgResLibTree::OnNMClickTree)
	//ON_NOTIFY(TVN_SELCHANGED, IDC_RESLIB_TREE, &CDlgResLibTree::OnTvnSelchangedTree)
//	ON_NOTIFY(NM_RCLICK, IDC_RESLIB_TREE, &CDlgResLibTree::OnNMRClickTree)
//	ON_COMMAND(IDM_COMPONENT_TREE_ADD, &CDlgResLibTree::OnMenuAdd)
//	ON_COMMAND(IDM_COMPONENT_TREE_DELETE, &CDlgResLibTree::OnMenuDel)
	//ON_BN_CLICKED(IDC_BUT_REF, &CDlgResLibTree::OnBnClickedButRef)
	//ON_BN_CLICKED(IDC_BUT_PUBLISH, &CDlgResLibTree::OnBnClickedButPublish)
	//ON_COMMAND(IDM_COMPONENT_TREE_EDIT, &CDlgResLibTree::OnMenuEdit)
	ON_WM_SIZE()
	ON_MESSAGE(WM_RESLIB_NOTIFY_SELCHANGED, &CDlgResLibTree::OnNotifySelchanged)
END_MESSAGE_MAP()


IMPLEMENT_DYNAMIC(CDlgResLibTree, CDialogEx)

CDlgResLibTree::CDlgResLibTree(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgResLibTree::IDD, pParent)
{

}

CDlgResLibTree::~CDlgResLibTree()
{
}

void CDlgResLibTree::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESLIB_TREE, m_tree);
}


BOOL CDlgResLibTree::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。
	
	//memset(m_path,0,MAX_PATH);

	//GetCurrentDirectory(MAX_PATH,m_path);

//	CString str = GetExecDir();
//	strncpy(m_path, (char *)(LPCTSTR)str, str.GetLength());
//
////	m_ImageList.Create(42,42,ILC_COLOR32,10,30);
//	//m_list.SetImageList(&m_ImageList,LVSIL_NORMAL);
//
//	m_tree.SetImageList(&m_tree.m_ImageList,TVSIL_NORMAL);
//	m_tree.SetBkColor(m_tree.m_ImageList.GetBkColor());
//
//	DWORD dwStyle = GetWindowLong(m_tree.m_hWnd,GWL_STYLE);
//	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
//	SetWindowLong(m_tree.m_hWnd,GWL_STYLE,dwStyle);
//
	// TODO: 在此添加额外的初始化代码

	//OnBnClickedButopen();
	SetBackgroundColor( g_crResLibTreeBg );
	m_tree.Load();

	//m_tree.ModifyStyle(DS_3DLOOK, 0);
	m_tree.ModifyStyleEx(WS_EX_CLIENTEDGE | NM_CUSTOMDRAW, 0);

	ModifyStyle(DS_3DLOOK, 0);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


//
//
//void CDlgResLibTree::OnBnClickedButopen()
//{
//	m_RobotPrivateDB = new CRobotPrivateDB();
//	m_RobotResDB = new CRobotResDB();
//	TiXmlDocument* doc =new TiXmlDocument;
//	string stPath =m_path;
//	stPath = stPath + "\\" + "reslib.xml";
//	if(!doc->LoadFile(stPath.c_str()))
//	{
//		return;
//	}
//	TiXmlElement* pRoot = doc->RootElement();
//	m_RobotPrivateDB->LoadingXml(pRoot);
//	m_RobotResDB->LoadingXml(pRoot);
//	m_tree.DeleteAllItems();
//
//	CEncodedTool encodeTool;
//	char lpData[255];
//	memset(lpData,0,255);
//	//encodeTool.StringToLPCTSTR("本地资源库",lpData,255);
//	memcpy(lpData,"本地资源库",strlen("本地资源库"));
//
//	m_hRoot = m_tree.InsertItem(lpData,NULL);
//	m_tree.SetItemData(m_hRoot,NULL);
//	//	m_tree.DelAllImageList();
//	for (list<CRobotPrivateEntity*>::iterator it = m_RobotPrivateDB->m_RobotPrivateEntlist.begin();
//		it != m_RobotPrivateDB->m_RobotPrivateEntlist.end();it++)
//	{
//		TreeRecursion(*it,m_hRoot);
//	}
//	for (list<CRobotPrivateEntity*>::iterator it = m_RobotResDB->m_ComponentDB->m_RobotPrivateEntlist.begin();
//		it != m_RobotResDB->m_ComponentDB->m_RobotPrivateEntlist.end();it++)
//	{
//		TreeRecursion(*it,m_hRoot);
//	}
//
//	for (list<CRobotPrivateEntity*>::iterator it = m_RobotResDB->m_ProcessDB->m_RobotPrivateEntlist.begin();
//		it != m_RobotResDB->m_ProcessDB->m_RobotPrivateEntlist.end();it++)
//	{
//		TreeRecursion(*it,m_hRoot);
//	}
//
//
//
//}
//HTREEITEM  CDlgResLibTree::TreeRecursion(CRobotPrivateEntity* pRobotPrivEnt,HTREEITEM hTree)
//{
//	CEncodedTool encodeTool;
//	WCHAR lpData[255];
//	memset(lpData,0,255);
//	encodeTool.StringToLPCTSTR(pRobotPrivEnt->GetName().c_str(),lpData,255);
//
//	HTREEITEM Treeitm = m_tree.InsertItem(pRobotPrivEnt->GetName().c_str(),hTree);
//	m_tree.SetItemData(Treeitm,(DWORD)pRobotPrivEnt);	
//
//
//	string stPathTmp = m_path;
//	stPathTmp.append(pRobotPrivEnt->m_pBotEnt->Pic);	
//
//	int nRetImage = m_tree.AddImageList(stPathTmp.c_str());
//	if (nRetImage == -1)
//		m_tree.SetItemIcons(Treeitm,nRetImage,0);
//	else
//		m_tree.SetItemIcons(Treeitm,nRetImage,nRetImage);
//	for (list<CRobotPrivateEntity*>::iterator it = pRobotPrivEnt->m_RobotPrivateEntList.begin();
//		it != pRobotPrivEnt->m_RobotPrivateEntList.end();it++)
//	{
//		TreeRecursion((*it),Treeitm);
//	}
//	return Treeitm;
//
//}
//
//
//void CDlgResLibTree::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}
//
//
//
//void CDlgResLibTree::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	m_vImgPath.clear();
//	//m_ImageList.DeleteImageList();
//	//m_ImageList.Create(42,42,ILC_COLOR32,10,30);
//	//m_list.SetImageList(&m_ImageList,LVSIL_NORMAL);
//
//	//m_list.DeleteAllItems();
//	int i=0;
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
//	TVITEM item = pNMTreeView->itemNew;
//	if(item.hItem == m_hRoot)
//		return;
//	//CxImage* m_xImage =new CxImage;
//	CRobotPrivateEntity* pBotPrivEnt =(CRobotPrivateEntity*) m_tree.GetItemData(item.hItem);
//	if (pBotPrivEnt == NULL)
//	{
//		return ;
//	}
//	for (list<CRobotPrivateEntity*>::iterator it = pBotPrivEnt->m_RobotPrivateEntList.begin();
//		it != pBotPrivEnt->m_RobotPrivateEntList.end();it++)
//	{
//		string stPathTmp = m_path;
//		stPathTmp.append((*it)->m_pBotEnt->Pic42);	
//
//		CString str = stPathTmp.c_str();
//		m_vImgPath.push_back(str);
//
//	/*	bool bRet=  m_xImage->Load(stPathTmp.c_str(),CXIMAGE_FORMAT_PNG);
//		CDC *pDC = GetDC();
//
//		HBITMAP hBitmap = m_xImage->MakeBitmap(pDC->GetSafeHdc());
//
//		if ( NULL == hBitmap )  
//		{  
//			delete m_xImage;
//			return;
//		}
//		CBitmap* pBitmap;
//
//		pBitmap = new CBitmap;
//
//		pBitmap->Attach(hBitmap);   
//
//		int nRetNum = m_ImageList.Add(pBitmap,RGB(0,0,0));
//*/
//
//		//CEncodedTool encodeTool;
//		//WCHAR lpData[255];
//		//memset(lpData,0,255);
//		//encodeTool.StringToLPCTSTR((*it)->GetName().c_str(),lpData,255);
//
//	//	int nRetListNum = m_list.InsertItem(i++,(*it)->GetName().c_str(),nRetNum);
//		//CBobotEntity* tmp = (*it)->m_pBotEnt;
//	//	m_list.SetItemData(nRetListNum,(DWORD_PTR )tmp);
//	}
//
//	if ( theApp.m_pDlgResLibImage != NULL )
//	{
//		theApp.m_pDlgResLibImage->m_vImgPath = m_vImgPath;
//		theApp.m_pDlgResLibImage->UpdateContent();
//	}
//
//	*pResult = 0;
//	//delete m_xImage;
//}
//
//
//
//void CDlgResLibTree::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	CPoint point;
//
//	GetCursorPos(&point);
//
//	CPoint pointInTree = point;
//
//	m_tree.ScreenToClient(&pointInTree);
//
//	HTREEITEM item;
//
//	UINT flag = TVHT_ONITEM;
//
//	item = m_tree.HitTest(pointInTree, &flag);
//
//	if(item != NULL)
//
//	{
//
//		m_tree.SelectItem(item);
//		CRobotPrivateEntity* pBotPrivEnt =(CRobotPrivateEntity*) m_tree.GetItemData(item);
//		if (pBotPrivEnt == NULL)
//		{
//			return ;
//		}
//		const char *show; 
//
//		CMenu menu;
//
//		menu.LoadMenu(IDR_COMPONENT_TREE_POP);
//
//
//		show=strstr(pBotPrivEnt->m_pBotEnt->Power.c_str(),"A");//返回指向第一次出现r位置的指针，如果没找到则返回NULL。
//		if (show == NULL)
//		{
//			menu.DeleteMenu(IDM_COMPONENT_TREE_ADD,MF_BYCOMMAND);
//		}
//		show=strstr(pBotPrivEnt->m_pBotEnt->Power.c_str(),"D");//返回指向第一次出现r位置的指针，如果没找到则返回NULL。
//		if (show == NULL)
//		{
//			menu.DeleteMenu(IDM_COMPONENT_TREE_DELETE,MF_BYCOMMAND);
//		}
//		show=strstr(pBotPrivEnt->m_pBotEnt->Power.c_str(),"E");//返回指向第一次出现r位置的指针，如果没找到则返回NULL。
//		if (show == NULL)
//		{
//			menu.DeleteMenu(IDM_COMPONENT_TREE_EDIT,MF_BYCOMMAND);
//		}
//
//		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |  
//
//			TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
//
//	}
//
//}
//
//
//void CDlgResLibTree::OnMenuAdd()
//{
//	HTREEITEM hSelect = m_tree.GetSelectedItem();
//	if (hSelect == NULL)
//	{
//		return ;
//	}
//	CRobotPrivateEntity* pBotPrivEnt =(CRobotPrivateEntity*) m_tree.GetItemData(hSelect);
//	if (pBotPrivEnt == NULL)
//	{
//		return;
//	}
//	
//}
//
//
//void CDlgResLibTree::OnMenuDel()
//{
//	HTREEITEM hSelect = m_tree.GetSelectedItem();
//	if (hSelect == NULL)
//	{
//		return ;
//	}
//	CRobotPrivateEntity* pBotPrivEnt =(CRobotPrivateEntity*) m_tree.GetItemData(hSelect);
//	if (pBotPrivEnt == NULL)
//	{
//		return;
//	}	
//
//
//	HTREEITEM hParent = m_tree.GetParentItem(hSelect);
//	CRobotPrivateEntity* pBotParentEnt =(CRobotPrivateEntity*) m_tree.GetItemData(hParent);
//
//	if (pBotParentEnt != NULL)
//	{
//		pBotParentEnt->m_RobotPrivateEntList.remove(pBotPrivEnt);
//	}
//
//	m_RobotResDB->m_BotoManage->DelBotEnt(pBotPrivEnt->ID);
//	m_RobotPrivateDB->m_BotoManage->DelBotEnt(pBotPrivEnt->ID);
//
//	m_tree.DeleteItem(hSelect);
//	delete pBotPrivEnt;
//
//	pBotPrivEnt =NULL;
//}
//
//void CDlgResLibTree::OnMenuEdit()
//{
//	HTREEITEM hSelect = m_tree.GetSelectedItem();
//	if (hSelect == NULL)
//	{
//		return ;
//	}
//	CRobotPrivateEntity* pBotPrivEnt =(CRobotPrivateEntity*) m_tree.GetItemData(hSelect);
//	if (pBotPrivEnt == NULL)
//	{
//		return;
//	}
//}
//
//void CDlgResLibTree::OnBnClickedButRef()
//{
//	m_tree.DeleteAllItems();
//
//	CEncodedTool encodeTool;
//	char lpData[255];
//	memset(lpData,0,255);
//	//encodeTool.StringToLPCTSTR("本地资源库",lpData,255);
//	memcpy(lpData,"本地资源库",strlen("本地资源库"));
//
//	m_hRoot = m_tree.InsertItem(lpData,NULL);
//	m_tree.SetItemData(m_hRoot,NULL);
//	for (list<CRobotPrivateEntity*>::iterator it = m_RobotPrivateDB->m_RobotPrivateEntlist.begin();
//		it != m_RobotPrivateDB->m_RobotPrivateEntlist.end();it++)
//	{
//		TreeRecursion(*it,m_hRoot);
//	}
//	for (list<CRobotPrivateEntity*>::iterator it = m_RobotResDB->m_ComponentDB->m_RobotPrivateEntlist.begin();
//		it != m_RobotResDB->m_ComponentDB->m_RobotPrivateEntlist.end();it++)
//	{
//		TreeRecursion(*it,m_hRoot);
//	}
//
//	for (list<CRobotPrivateEntity*>::iterator it = m_RobotResDB->m_ProcessDB->m_RobotPrivateEntlist.begin();
//		it != m_RobotResDB->m_ProcessDB->m_RobotPrivateEntlist.end();it++)
//	{
//		TreeRecursion(*it,m_hRoot);
//	}
//
//}
//void CDlgResLibTree::OnBnClickedButPublish()
//{
//	int nret =m_list.GetSelectionMark();
//	CBobotEntity* RobotoEnt = (CBobotEntity*)m_list.GetItemData(nret);
//	if (RobotoEnt == NULL)
//	{
//		return;
//	}
//	bool bRet = RobotoEnt->CopyRobotoFile("e:\\2\\","d:\\1\\");
//}
//



void CDlgResLibTree::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CRect rcClient(3,5,cx,cy);
	if ( IsWindow(m_tree.GetSafeHwnd()))
	{
		m_tree.MoveWindow(&rcClient);
	}
}



afx_msg LRESULT CDlgResLibTree::OnNotifySelchanged(WPARAM wParam, LPARAM lParam)
{
	if (theApp.m_pDlgResLibImage != NULL 
		&& IsWindow( theApp.m_pDlgResLibImage->GetSafeHwnd() ))
	{
		return theApp.m_pDlgResLibImage->SendMessage(WM_RESLIB_NOTIFY_SELCHANGED
			, wParam, lParam);
	}
	return 0;
	//DWORD_PTR * pArray = reinterpret_cast< DWORD_PTR * > (wParam);
	//int nCount = (int)(lParam);
	//CToolResData * pData = NULL;
	//for (int i=0; i<nCount; i++)
	//{
	//	pData = reinterpret_cast< CToolResData * > ( pArray[i] );
	//	Sleep(0);

	//	//
	//	pData->GetTYPE();

	//	//
	//	pData->GetNAME();
	//}

	//list<CToolResData*> tmpList;
	//DWORD_PTR** tmp=(DWORD_PTR**)wParam;
	//for (int i=0;i <lParam;i++)
	//{
	//	CToolResData* NewToolResData = (CToolResData*)tmp[i];
	//	tmpList.push_back(NewToolResData);
	//}
	//return 1;

}


BOOL CDlgResLibTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
// 	switch (pMsg->message)
// 	{
// 	case WM_LBUTTONDOWN:
// 	case WM_RBUTTONDOWN:
// 	case WM_RBUTTONUP:
// 	case WM_MBUTTONDOWN:
// 	case WM_MBUTTONUP:
// 		CBCGPPopupMenu* pActivePopupMenu = CBCGPPopupMenu::GetSafeActivePopupMenu();
// 		if(pActivePopupMenu)
// 		{
// 			pActivePopupMenu->SendMessage (WM_CLOSE);	
// 			pActivePopupMenu = NULL;
// 		}
// 		break;
// 	}
// 	return CDialogEx::PreTranslateMessage(pMsg);

	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	CWnd* pOwner = GetOwner();

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	while (pOwner != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pOwner->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		if (IsWindow (pOwner->GetSafeHwnd ()))
		{
			pOwner = pOwner->GetParentFrame();
		}
		else
		{
			break;
		}
	}

	// filter both messages to dialog and from children
	//return PreTranslateInput(pMsg);


	return CDialogEx::PreTranslateMessage(pMsg);
}
