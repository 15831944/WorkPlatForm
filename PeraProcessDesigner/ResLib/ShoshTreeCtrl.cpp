// ShoshTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ShoshTreeCtrl.h"
#include <afxcmn.h>


#include "ResLibProcessData.h"
#include "ReadRobotDB.h"
#include "Magicdb.h"
#include "EncodedTool.h"
#include "DlgResLibProcessEditor.h"
#include "DlgResLibEditor.h"
#include "ResLibRobotData.h"

#include <shlobj.h>
#include "UUID.h"
#include <CreateTreeDirDlg.h>
#include <ResLibImageSet.h>
#include <PeraProcessDesigner.h>
#include <CStringChecker.h>
#include <Tools.h>
#include <Cmpare.h>
#include <DlgRunFlow.h>
#include "GetGlobalDataFromReg.h"

using namespace ReslibMgr;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SHOSH_SETCHECK
/////////////////////////////////////////////////////////////////////////////
// CShoshTreeCtrl
using namespace  std;
CReadRobotDB g_ReadRobotDB;

CShoshTreeCtrl::CShoshTreeCtrl()
{

	//g_ReadRobotDB.LoadingDB();
	CreateShoshIL( g_crResLibTreeBg );	//树控件的初始颜色也在此设置
	for(int i=0;i<4;i++)
		bStyles[i]=true;
	bStyles[4]=false;
	m_xImage = new CxImage();
	m_bIsMenuShow = true;

}

CShoshTreeCtrl::~CShoshTreeCtrl()
{
	Release();
}

VOID CShoshTreeCtrl::CreateShoshIL(COLORREF clrColor)
{
	m_ImageList.Create(16, 16, ILC_COLOR32 ,10,CLR_NONE);
	m_ImageList.SetBkColor(clrColor);		//设置背景色
}

BEGIN_MESSAGE_MAP(CShoshTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CShoshTreeCtrl)	
	//}}AFX_MSG_MAP
	//	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//	ON_MESSAGE(WM_CLEARHOSTS, &CShoshTreeCtrl::OnClearhosts)
	ON_COMMAND(ID_TREE_MENU_ADD, &CShoshTreeCtrl::OnTreeMenuAdd)
	ON_COMMAND(ID_TREE_MENU_DEL, &CShoshTreeCtrl::OnTreeMenuDel)
	ON_COMMAND(ID_TREE_MENU_EDIT, &CShoshTreeCtrl::OnTreeMenuEdit)
	ON_COMMAND(ID_TREE_MENU_SEND, &CShoshTreeCtrl::OnTreeMenuSend)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CShoshTreeCtrl::OnTvnSelchanged)
	ON_MESSAGE(WM_RESLIB_NOTIFY_DELETE, &CShoshTreeCtrl::OnNotifyDelete)
	ON_MESSAGE(WM_RESLIB_NOTIFY_UPDATE_PROP, &CShoshTreeCtrl::OnNotifyEdit)
	ON_MESSAGE(WM_RESLIB_NOTIFY_DOWNLOAD, &CShoshTreeCtrl::OnDownload)
	//	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CShoshTreeCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShoshTreeCtrl message handlers


void CShoshTreeCtrl::SetItemIcons(HTREEITEM Treeitm,int imageID,int selImageID) 
{
	//	HTREEITEM selItem=GetSelectedItem();
	if(NULL==Treeitm)
		return;
	/*bool bRet= */SetItemImage(Treeitm,imageID,selImageID);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//函数功能:获取树项目全跟径
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
CString CShoshTreeCtrl::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn;
	strReturn = "";
	while(hCurrent != GetRootItem())
	{
		strTemp = GetItemText(hCurrent);
		if(strTemp.Right(1) != "\\")
			strTemp += "\\";
		strReturn = strTemp  + strReturn;
		hCurrent = GetParentItem(hCurrent);
	}
	return strReturn;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//添加子目录
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void CShoshTreeCtrl::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);
	if(strPath.Right(1) != "\\")
		strPath += "\\";
	strPath += "*.*";
	CFileFind file;
	BOOL bContinue = file.FindFile(strPath);
	while(bContinue)
	{
		bContinue = file.FindNextFile();
		if(file.IsDirectory() && !file.IsDots())
			InsertItem(file.GetFileName(),hParent);
	}
}

int CShoshTreeCtrl::AddImageList(const char* stfilePath)
{
	bool bRet=  m_xImage->Load(stfilePath,CXIMAGE_FORMAT_PNG);
	CDC *pDC = GetDC();

	Bitmap * pbmp = GetNewBitmap(stfilePath);
	if (pbmp == NULL) return -1;
	m_pBitMapList2.push_back(pbmp);

	Color cr = Color::White;


	HBITMAP hBitmap = NULL ;//m_xImage->MakeBitmap(pDC->GetSafeHdc());
	pbmp->GetHBITMAP( cr, &hBitmap );

	if ( NULL != hBitmap )  
	{  

		CBitmap* pBitmap;

		pBitmap = new CBitmap;
		m_pBitMapList.push_back(pBitmap);
		pBitmap->Attach(hBitmap);   

		int nRetNum = m_ImageList.Add(pBitmap,RGB(0,0,0));
		return nRetNum;
	}
	return -1;
}

bool CShoshTreeCtrl::DelAllImageList()
{
	m_ImageList.DeleteImageList();
	m_ImageList.Create(16, 16, ILC_COLOR32 ,10,CLR_NONE);
	SetImageList(&m_ImageList,TVSIL_NORMAL);
	SetBkColor(m_ImageList.GetBkColor());
	return 1;
}
void CShoshTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bIsMenuShow)
	{
		return;
	}
	CPoint pointTree;

	GetCursorPos(&pointTree);

	CPoint pointInTree = point;

	ScreenToClient(&pointInTree);

	HTREEITEM item;

	UINT flag = TVHT_ONITEM;

	item = HitTest(point, &flag);

	if(item != NULL)

	{

		SelectItem(item);
		CResLibData* pResLibDataBaseEnt =(CResLibData*) GetItemData(item);
		if (pResLibDataBaseEnt == NULL)
		{
			return ;
		}
		//		const char *show; 

		CMenu menu;

		menu.CreatePopupMenu();

		switch (pResLibDataBaseEnt->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			{
				if (pResLibDataBaseEnt->m_sSrc.find("A") != string::npos)
				{
					menu.AppendMenu(MF_BYCOMMAND | MF_STRING,ID_TREE_MENU_ADD,"新建目录");
				}
				if (pResLibDataBaseEnt->m_sSrc.find("S") != string::npos)
				{
					menu.AppendMenu(MF_BYCOMMAND | MF_STRING,ID_TREE_MENU_SEND,"上传组件");
				}
				if (pResLibDataBaseEnt->m_sSrc.find("D") != string::npos)
				{
					menu.AppendMenu(MF_BYCOMMAND | MF_STRING,ID_TREE_MENU_DEL,"删除");
				}


				menu.TrackPopupMenu(TPM_LEFTALIGN |  

					TPM_RIGHTBUTTON, pointTree.x, pointTree.y, this, NULL);
			}
			break;
		case CResLibData::RT_PROCESS:
			{
				if (pResLibDataBaseEnt->m_sSrc.find("A") != string::npos)
				{
					menu.AppendMenu(MF_BYCOMMAND | MF_STRING,ID_TREE_MENU_ADD,"新建目录");
				}
				if (pResLibDataBaseEnt->m_sSrc.find("S") != string::npos)
				{
					menu.AppendMenu(MF_BYCOMMAND | MF_STRING,ID_TREE_MENU_SEND,"上传设计过程");
				}
				if (pResLibDataBaseEnt->m_sSrc.find("D") != string::npos)
				{
					menu.AppendMenu(MF_BYCOMMAND | MF_STRING,ID_TREE_MENU_DEL,"删除");
				}


				menu.TrackPopupMenu(TPM_LEFTALIGN |  

					TPM_RIGHTBUTTON, pointTree.x, pointTree.y, this, NULL);
			}
			break;

		default:
			break;
		}


	}
}
afx_msg void CShoshTreeCtrl::OnTreeMenuAdd()
{

	HTREEITEM hSelect = GetSelectedItem();
	if (hSelect == NULL)
	{
		return ;
	}
	CResLibData* pResLibDataBase =(CResLibData*) GetItemData(hSelect);
	if (pResLibDataBase == NULL)
	{
		return ;
	}
	switch (pResLibDataBase->m_nResType)
	{
	case CResLibData::RT_ROBOT:
		{
			CCreateTreeDirDlg ComponentDlg;
			ComponentDlg.m_PK_ID = GuidToString(CreateGuid()).c_str();
			ComponentDlg.m_FK_PID = pResLibDataBase->m_sPkID.c_str();
			ComponentDlg.SetCreateDir(CResLibData::RT_ROBOT);
			Invalidate ();
			if (ComponentDlg.DoModal() == IDOK)
			{
				CResLibRobotData * RobotEnt = new CResLibRobotData();
				RobotEnt->m_sPkID						=	ComponentDlg.m_PK_ID.GetBuffer();
				RobotEnt->m_sFkpID					=	ComponentDlg.m_FK_PID.GetBuffer();
				RobotEnt->m_sName			=	ComponentDlg.m_csDirName.GetBuffer();	
				RobotEnt->m_sEntType="1";
				RobotEnt->m_sSrc="ACDES";

				if(!g_ReadRobotDB.AddDB(RobotEnt))
				{
					delete RobotEnt;
					RobotEnt=NULL;
					return;
				}
				pResLibDataBase->m_pBaseList.push_back((CResLibData*)RobotEnt);

				HTREEITEM hTreeBotEnd = InsertItem(RobotEnt->m_sName.c_str(),hSelect);
				SetItemData(hTreeBotEnd,(DWORD)RobotEnt);
				SelectItem(hTreeBotEnd);
				SortChildren(hSelect);

			}
		}
		break;
	case CResLibData::RT_PROCESS:
		{
			CCreateTreeDirDlg ProceDlg;
			ProceDlg.m_PK_ID = GuidToString(CreateGuid()).c_str();


			ProceDlg.m_FK_PID = pResLibDataBase->m_sPkID.c_str();

			ProceDlg.SetCreateDir(CResLibData::RT_PROCESS);
			Invalidate ();
			if (ProceDlg.DoModal() == IDOK)
			{

				CResLibProcessData* ProcessEnt =new CResLibProcessData();

				ProcessEnt->m_sPkID = ProceDlg.m_PK_ID.GetBuffer();           
				ProcessEnt->m_sFkpID=ProceDlg.m_FK_PID.GetBuffer();                
				ProcessEnt->m_sName=ProceDlg.m_csDirName.GetBuffer();    
				ProcessEnt->m_sEntType="1";
				ProcessEnt->m_sSrc="ACDES";

				if(!g_ReadRobotDB.AddDB(ProcessEnt))
				{
					delete ProcessEnt;
					ProcessEnt= NULL;					
					return;
				}
				pResLibDataBase->m_pBaseList.push_back((CResLibData*)ProcessEnt);

				HTREEITEM hTreeBotEnd = InsertItem(ProcessEnt->m_sName.c_str(),hSelect);
				SetItemData(hTreeBotEnd,(DWORD)ProcessEnt);
				SelectItem(hTreeBotEnd);
				SortChildren(hSelect);
			}
		}
		break;

	default:
		break;
	}	
}


afx_msg void CShoshTreeCtrl::OnTreeMenuDel()
{
	HTREEITEM hSelect = GetSelectedItem();
	if (hSelect != NULL)
	{
		int nRet = ::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "确认删除文件夹及文件夹下所有内容吗？", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
		if(IDOK==nRet) 			
			DelTreeDate(hSelect);
		return ;
	}

}
void CShoshTreeCtrl::DelTreeDate(HTREEITEM hSelect)
{
	CResLibData* pResLibDataBase =(CResLibData*) GetItemData(hSelect);
	if (pResLibDataBase == NULL)
	{
		return ;
	}
	HTREEITEM hParent = GetParentItem(hSelect);
	CResLibData* pResLibDataParentEnt =(CResLibData*) GetItemData(hParent);
	CleanSelchanged();
	switch (pResLibDataBase->m_nResType)
	{
	case CResLibData::RT_ROBOT:
		{
			pResLibDataParentEnt->m_pBaseList;
			CResLibRobotData* pRobotEnt =(CResLibRobotData*) pResLibDataParentEnt;
			for (list<CResLibData*>::iterator itParent = pResLibDataParentEnt->m_pBaseList.begin();itParent != pResLibDataParentEnt->m_pBaseList.end();itParent++)
			{
				if (pResLibDataBase == (*itParent))
				{
					for (list<CResLibData*>::iterator it = (*itParent)->m_pBaseList.begin();it != (*itParent)->m_pBaseList.end();it++)
					{
						g_ReadRobotDB.DelDB((CResLibRobotData*)(*it));
						(*it)->DelPublishFile();
					}

				}

			}
		}
		break;
	case CResLibData::RT_PROCESS:
		{

			for (list<CResLibData*>::iterator itParent = pResLibDataParentEnt->m_pBaseList.begin();itParent != pResLibDataParentEnt->m_pBaseList.end();itParent++)
			{
				if (pResLibDataBase == (*itParent))
				{
					for (list<CResLibData*>::iterator it = (*itParent)->m_pBaseList.begin();it != (*itParent)->m_pBaseList.end();it++)
					{
						g_ReadRobotDB.DelDB((CResLibProcessData*)(*it));
						(*it)->DelPublishFile();
					}

				}

			}
		}		
		break;

	default:
		break;
	}
	if (pResLibDataParentEnt != NULL)
	{

		switch (pResLibDataBase->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			g_ReadRobotDB.DelDB((CResLibRobotData*)pResLibDataBase);
			break;
		case CResLibData::RT_PROCESS:
			g_ReadRobotDB.DelDB((CResLibProcessData*)pResLibDataBase);
			break;
		default:
			break;
		}
		pResLibDataParentEnt->RemoveBaseList(pResLibDataBase);

	}			
	DeleteItem(hSelect);
}

bool CShoshTreeCtrl::EditRobot(CResLibData* pResLibDataItem)
{
	switch (pResLibDataItem->m_nResType)
	{
	case CResLibData::RT_ROBOT:
		{
			CDlgResLibEditor RobotDlg;
			CResLibRobotData* RobotEnt =(CResLibRobotData*)pResLibDataItem;	
			RobotDlg.m_COMPONENT_PK_ID = GuidToString(CreateGuid()).c_str();
			RobotDlg.m_COMPONENT_FK_PID = pResLibDataItem->m_sPkID.c_str();
			RobotDlg.SetpResLibDataBase(pResLibDataItem);
			RobotDlg.SetSelectTree(FALSE);
			RobotDlg.m_COMPONENT_PK_ID = RobotEnt->m_sPkID.c_str();
			RobotDlg.m_COMPONENT_FK_PID = RobotEnt->m_sFkpID.c_str();
			RobotDlg.m_OldName = RobotEnt->m_sName.c_str();
			RobotDlg.m_COMPONENT_NAME = RobotEnt->m_sName.c_str();
			RobotDlg.m_COMPONENT_VERSION = RobotEnt->m_sVersions.c_str();
			RobotDlg.m_COMPONENT_AUTHOR = RobotEnt->m_sAuthor.c_str();
			char stNewRobotoPath[255];
			memset(stNewRobotoPath,0,255);
			CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
			if(strTmp.IsEmpty())
				strTmp = GetExecDir();

			memcpy(stNewRobotoPath,strTmp.GetBuffer(),strTmp.GetLength());

			RobotDlg.m_FILE_PATH = stNewRobotoPath;
			RobotDlg.m_FILE_PATH += RobotEnt->m_RobotFilePath.c_str(); 	
			RobotDlg.m_FILE_PATH.Replace("//","\\");
			RobotDlg.m_csDesc = RobotEnt->m_sDsec.c_str();
			RobotDlg.m_COMPONENT_CREATE_TIME = RobotEnt->m_sCreatTime.c_str();
			//RobotDlg.m_COMPONENT_CREATE_TIME = ZTools::GetNowTimeStr().c_str();	
			if (RobotDlg.DoModal() == IDOK)
			{							           
				RobotEnt->SetComponentName(RobotDlg.m_COMPONENT_NAME.GetBuffer());        
				RobotEnt->SetComponentVersion(RobotDlg.m_COMPONENT_VERSION.GetBuffer());   
				RobotEnt->SetComponentAuthor(RobotDlg.m_COMPONENT_AUTHOR.GetBuffer());     
				//RobotDlg.m_csDesc.Replace("—","-");
				RobotEnt->SetComponentDesc(RobotDlg.m_csDesc.GetBuffer());     
							
				RobotEnt->SetComponentCreatTime(RobotDlg.m_COMPONENT_CREATE_TIME.GetBuffer());
				RobotEnt->SetComponentPublishTime(ZTools::GetNowTimeStr());				
				if(!g_ReadRobotDB.EditDB(RobotEnt))
				{
					delete RobotEnt;
					RobotEnt= NULL;
					return false;
				}				
				RobotDlg.GetpResLibDataBase()->m_pBaseList.sort(Cmpare());				
			}
		}
		break;
	case CResLibData::RT_PROCESS:
		{
			CDlgResLibProcessEditor ProceDlg;
			CResLibProcessData* ProcessEnt =(CResLibProcessData*)pResLibDataItem;	
			ProceDlg.m_PK_ID = GuidToString(CreateGuid()).c_str();
			ProceDlg.m_FK_PID = pResLibDataItem->m_sPkID.c_str();
			ProceDlg.SetpResLibDataBase(pResLibDataItem);
			ProceDlg.SetSelectTree(FALSE);
			ProceDlg.m_OldName = ProcessEnt->m_sName.c_str();
			ProceDlg.m_PK_ID = ProcessEnt->m_sPkID.c_str();
			ProceDlg.m_FK_PID = ProcessEnt->m_sFkpID.c_str();
			ProceDlg.m_PROCESS_NAME = ProcessEnt->m_sName.c_str();
			ProceDlg.m_PROCESS_VERSION = ProcessEnt->m_sVersions.c_str();
			ProceDlg.m_PROCESS_AUTHOR = ProcessEnt->m_sAuthor.c_str();
			ProceDlg.m_PROCESS_DESC = ProcessEnt->m_sDsec.c_str();

			char stNewRobotoPath[255];
			memset(stNewRobotoPath,0,255);
			CString strTmp = GetPeraGlobalDataFromReg("LM","PeraLibraryPath");
			if(strTmp.IsEmpty())
				strTmp = GetExecDir();

			memcpy(stNewRobotoPath,strTmp.GetBuffer(),strTmp.GetLength());

			ProceDlg.m_PROCESS_FILE_PATH = stNewRobotoPath;
			ProceDlg.m_PROCESS_FILE_PATH += ProcessEnt->m_ProcessFilePath.c_str(); 
			ProceDlg.m_PROCESS_FILE_PATH.Replace("//","\\");
			ProceDlg.m_PROCESS_OPT = ProcessEnt->m_sOpt.c_str();
			ProceDlg.m_PROCESS_DESC = ProcessEnt->m_sDsec.c_str();
			ProceDlg.m_PROCESS_CREATE_TIME = ProcessEnt->m_sCreatTime.c_str();
			ProceDlg.m_PROCESS_CREAT_TIME = ZTools::GetNowTimeStr().c_str();	
			if (ProceDlg.DoModal() == IDOK)
			{							           
				ProcessEnt->SetProcessName(ProceDlg.m_PROCESS_NAME.GetBuffer());        
				ProcessEnt->SetProcessVersion(ProceDlg.m_PROCESS_VERSION.GetBuffer());   
				ProcessEnt->SetProcessAuthor(ProceDlg.m_PROCESS_AUTHOR.GetBuffer());  
				//ProceDlg.m_PROCESS_DESC.Replace("—","-");
				ProcessEnt->SetProcessDesc(ProceDlg.m_PROCESS_DESC.GetBuffer());     
				ProcessEnt->SetProcessOpt(ProceDlg.m_PROCESS_OPT.GetBuffer());				
				ProcessEnt->SetProcessCreatTime(ProceDlg.m_PROCESS_CREATE_TIME.GetBuffer());
				ProcessEnt->SetProcessPublishTime(ZTools::GetNowTimeStr());				
				if(!g_ReadRobotDB.EditDB(ProcessEnt))
				{
					delete ProcessEnt;
					ProcessEnt= NULL;
					return false;
				}				
				ProceDlg.GetpResLibDataBase()->m_pBaseList.sort(Cmpare());				
			}
		}
		break;
	}
	return true;
}

bool CShoshTreeCtrl::RemoveItem(string sItemID)
{
	HTREEITEM hSelect = GetSelectedItem();
	if (hSelect == NULL)
	{
		return false;
	}
	CResLibData* pResLibDataBase =(CResLibData*)GetItemData(hSelect); 
	if (pResLibDataBase == NULL)
	{
		return false;
	}
	for (list<CResLibData*>::iterator it = pResLibDataBase->m_pBaseList.begin();it != pResLibDataBase->m_pBaseList.end();it++)
	{
		if ((*it)->m_sPkID == sItemID)
		{
			CResLibData* pDelItem =(*it);

			switch (pDelItem->m_nResType)
			{
			case CResLibData::RT_ROBOT:
				{
					g_ReadRobotDB.DelDB((CResLibRobotData*)pDelItem);
					((CResLibRobotData*)pDelItem)->DelPublishFile();
				}
				break;
			case CResLibData::RT_PROCESS:
				{
					g_ReadRobotDB.DelDB((CResLibProcessData*)pDelItem);
					((CResLibProcessData*)pDelItem)->DelPublishFile();
				}
				break;
			default:
				break;
			}			
			pResLibDataBase->m_pBaseList.remove(pDelItem);
			delete pDelItem;
			pDelItem =NULL;
			return true;
		}
	}
	return false;
}
bool CShoshTreeCtrl::RemoveDir(string sDirID)
{
	map<string,HTREEITEM> IDTreeMap;
	HTREEITEM hRoot = GetRootItem();
	TreeVisit(hRoot,IDTreeMap);

	for (map<string,HTREEITEM>::iterator it = IDTreeMap.begin();it != IDTreeMap.end();it++)
	{
		if ((it->first)== sDirID)
		{
			DelTreeDate(it->second);
			return true;
		}
	}
	return false;
}
void CShoshTreeCtrl::SetMenuShow(bool bMenuShow)
{
	m_bIsMenuShow = bMenuShow;
}
bool CShoshTreeCtrl::GetBaseComponentRoot(list<CResLibData*>& pBaseList)
{
	if (g_ReadRobotDB.m_PRoot == NULL)
	{
		return false;
	}
	for (list<CResLibData*>::iterator it = g_ReadRobotDB.m_PRoot->m_pBaseList.begin();it != g_ReadRobotDB.m_PRoot->m_pBaseList.end();it++)
	{		
		if ((*it)->m_nResType == CResLibData::RT_BASECOMPONET)
		{
			pBaseList =  (*it)->m_pBaseList;
			return true;
		}
	}
	return false;
}

afx_msg void CShoshTreeCtrl::OnTreeMenuEdit()
{
}
void CShoshTreeCtrl::InitTree()
{
	ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|WS_CLIPCHILDREN);
	SetImageList(&m_ImageList,TVSIL_NORMAL);
	SetBkColor(m_ImageList.GetBkColor());
	DeleteAllItems();
}
void CShoshTreeCtrl::InsertRootTree()
{
	CEncodedTool encodeTool;
	char lpData[255];
	memset(lpData,0,255);
	CCxStaticRes * pImgData = NULL;
	pImgData = GetStaticResImageData(CCxStaticRes::RT_FOLDER_16);
	if ( pImgData == NULL )
	{
		return;
	}
	m_nFileImageNo = AddImageList(pImgData->m_strPath.GetBuffer());

	pImgData = GetStaticResImageData(CCxStaticRes::RT_FLOW_16);
	if ( pImgData == NULL )
	{
		return;
	}
	m_nWSImageNo = AddImageList(pImgData->m_strPath.GetBuffer());

	pImgData = GetStaticResImageData(CCxStaticRes::RT_COMPONENT_16);
	if ( pImgData == NULL )
	{
		return;
	}
	m_nRobotImageNo = AddImageList(pImgData->m_strPath.GetBuffer());

	pImgData = GetStaticResImageData(CCxStaticRes::RT_PC_16);
	if ( pImgData == NULL )
	{
		return;
	}
	m_nPCImageNo = AddImageList(pImgData->m_strPath.GetBuffer());

	m_hRoot = InsertItem(g_ReadRobotDB.m_stName.c_str(),NULL);
	SetItemData(m_hRoot,(DWORD)g_ReadRobotDB.m_PRoot);
	SetItemIcons(m_hRoot,m_nPCImageNo,m_nPCImageNo);
}
void CShoshTreeCtrl::InsertRobotTree()
{
	if (g_ReadRobotDB.m_PRoot == NULL)
	{
		return;
	}
	for (list<CResLibData*>::iterator it = g_ReadRobotDB.m_PRoot->m_pBaseList.begin();it != g_ReadRobotDB.m_PRoot->m_pBaseList.end();it++)
	{
		switch ((*it)->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			{
				TreeRecursion((CResLibRobotData*)(*it),m_hRoot);
			}
			break;
		default:
			break;
		}
	}
}
void CShoshTreeCtrl::InsertProcessTree()
{
	if (g_ReadRobotDB.m_PRoot == NULL)
	{
		return;
	}
	for (list<CResLibData*>::iterator it = g_ReadRobotDB.m_PRoot->m_pBaseList.begin();it != g_ReadRobotDB.m_PRoot->m_pBaseList.end();it++)
	{
		switch ((*it)->m_nResType)
		{		
		case CResLibData::RT_PROCESS:
			{
				TreeRecursion((CResLibProcessData*)(*it),m_hRoot);
			}
			break;
		default:
			break;
		}
	}
}
void CShoshTreeCtrl::Load()
{
	InitTree();
	InsertRootTree();
	InsertRobotTree();
	InsertProcessTree();	
	this->Expand( m_hRoot, TVE_EXPAND );
}
void CShoshTreeCtrl::Load(int nResType)
{
	InitTree();
	InsertRootTree();
	switch (nResType)
	{		
	case CResLibData::RT_ROBOT:
		{
			InsertRobotTree();
		}
		break;
	case CResLibData::RT_PROCESS:
		{
			InsertProcessTree();
		}
		break;
	default:
		break;
	}	

	this->Expand( m_hRoot, TVE_EXPAND );
}
void CShoshTreeCtrl::Release()
{
	if (m_xImage != NULL)
	{
		m_xImage->Clear();
		delete m_xImage;
		m_xImage = NULL;
	}	
	for (list<CBitmap*>::iterator it = m_pBitMapList.begin();it != m_pBitMapList.end();
		it++)
	{
		if ((*it) != NULL)
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	m_pBitMapList.clear();

	for (list<Bitmap*>::iterator it = m_pBitMapList2.begin();it != m_pBitMapList2.end();
		it++)
	{
		if ((*it) != NULL)
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	m_pBitMapList2.clear();
}
HTREEITEM  CShoshTreeCtrl::TreeRecursion(CResLibProcessData* pProcessBase,HTREEITEM hTree)
{
	CEncodedTool encodeTool;
	WCHAR lpData[255];
	memset(lpData,0,255);
	encodeTool.StringToLPCTSTR(pProcessBase->m_sName.c_str(),lpData,255);
	HTREEITEM Treeitm=NULL;
	if (pProcessBase->m_sEntType.find("1") != string::npos)
	{
		Treeitm = InsertItem(pProcessBase->m_sName.c_str(),hTree);
		SetItemData(Treeitm,(DWORD)pProcessBase);	
		SetItemIcons(Treeitm,m_nWSImageNo,m_nWSImageNo);
		m_nWSImageNo = m_nFileImageNo;
	}



	for (list<CResLibData*>::iterator it = pProcessBase->m_pBaseList.begin();
		it != pProcessBase->m_pBaseList.end();it++)
	{
		switch (pProcessBase->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			TreeRecursion((CResLibRobotData*)(*it),Treeitm);
			break;
		case CResLibData::RT_PROCESS:
			TreeRecursion((CResLibProcessData*)(*it),Treeitm);
			break;

		default:
			break;
		}
	}
	SortChildren(hTree);
	return Treeitm;

}

HTREEITEM  CShoshTreeCtrl::TreeRecursion(CResLibRobotData* pRobotBase,HTREEITEM hTree)
{

	HTREEITEM Treeitm=NULL;
	if (pRobotBase->m_sEntType.find("1") != string::npos)
	{
		Treeitm = InsertItem(pRobotBase->m_sName.c_str(),hTree);
		SetItemData(Treeitm,(DWORD)pRobotBase);
		SetItemIcons(Treeitm,m_nRobotImageNo,m_nRobotImageNo);
		m_nRobotImageNo = m_nFileImageNo;
	}

	for (list<CResLibData*>::iterator it = pRobotBase->m_pBaseList.begin();
		it != pRobotBase->m_pBaseList.end();it++)
	{
		switch (pRobotBase->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			TreeRecursion((CResLibRobotData*)(*it),Treeitm);
			break;
		case CResLibData::RT_PROCESS:
			TreeRecursion((CResLibProcessData*)(*it),Treeitm);
			break;
		default:
			break;
		}
	}
	SortChildren(hTree);
	return Treeitm;

}



void  CShoshTreeCtrl::TreeVisit(HTREEITEM hItem,map<string,HTREEITEM>& IDTreeMap)
{  
	//DoWithItem(hItem); //访问当前节点  
	if (ItemHasChildren(hItem))  
	{
		CResLibData* pResLibDataBase = (CResLibData*)GetItemData(hItem);		
		IDTreeMap.insert(make_pair(pResLibDataBase->m_sPkID,hItem));

	}

	HTREEITEM hChildItem = GetChildItem(hItem);  
	while(hChildItem!=NULL)  
	{
		TreeVisit(hChildItem,IDTreeMap);     //递归遍历孩子节点  
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);  
	}  

}
bool CShoshTreeCtrl::SendProcessTreeDB(CResLibData* pResLibDataBase)
{
	CDlgResLibProcessEditor ProceDlg;
	ProceDlg.m_PK_ID = GuidToString(CreateGuid()).c_str();
	ProceDlg.m_FK_PID = pResLibDataBase->m_sPkID.c_str();
	ProceDlg.SetpResLibDataBase(pResLibDataBase);
	ProceDlg.SetSelectTree(FALSE);
	if (ProceDlg.SelectProcessFile())
	{
		if (ProceDlg.DoModal() == IDOK)
		{
			ZTools::WriteZToolsFormatLog("开始发布");
			CResLibProcessData* ProcessEnt =new CResLibProcessData();

			ProcessEnt->m_sPkID = ProceDlg.m_PK_ID.GetBuffer();           
			ProcessEnt->m_sFkpID=ProceDlg.m_FK_PID.GetBuffer();                
			ProcessEnt->m_sName=ProceDlg.m_PROCESS_NAME.GetBuffer();         
			ProcessEnt->m_sVersions=ProceDlg.m_PROCESS_VERSION.GetBuffer();   
			ProcessEnt->m_sAuthor=ProceDlg.m_PROCESS_AUTHOR.GetBuffer();      
			ProcessEnt->m_sDsec=ProceDlg.m_PROCESS_DESC.GetBuffer();      
			ProcessEnt->m_ProcessFilePath=ProceDlg.m_PROCESS_FILE_PATH.GetBuffer(); 
			ProcessEnt->m_sOpt=ProceDlg.m_PROCESS_OPT.GetBuffer(); 
			//ProceDlg.m_PROCESS_DESC.Replace("—","-");
			ProcessEnt->m_sDsec	=	ProceDlg.m_PROCESS_DESC.GetBuffer();
			ProcessEnt->m_sCreatTime=ProceDlg.m_PROCESS_CREATE_TIME.GetBuffer();    
			ProcessEnt->m_sPublishTime=ZTools::GetNowTimeStr();  
			ProcessEnt->m_InstanceID	=	ProceDlg.m_PROCESS_ID.GetBuffer();
			ProcessEnt->m_sEntType="0";  
			ProcessEnt->m_nResType = CResLibData::RT_PROCESS;
			ProcessEnt->m_sSrc="ACDES";

			if(!ProcessEnt->PublishFile())//发布文件，本地目录存储
			{
				return false;
			}
			if(!g_ReadRobotDB.AddDB(ProcessEnt))
			{
				delete ProcessEnt;
				ProcessEnt= NULL;
				return false;
			}
			ProceDlg.GetpResLibDataBase()->m_pBaseList.push_back((CResLibData*)ProcessEnt);
			ProceDlg.GetpResLibDataBase()->m_pBaseList.sort(Cmpare());
			ZTools::WriteZToolsFormatLog("完成发布");
			return true;
		}
	}
	return false;
}
bool CShoshTreeCtrl::SendRobotTreeDB(PublishInfo ResLibDataInfo)
{
	CDlgResLibEditor ComponentDlg;	
	ComponentDlg.SetSelectTree(TRUE);

	if (ComponentDlg.SelectRobotFile(&ResLibDataInfo))
	{
		if (ComponentDlg.DoModal() == IDOK)
		{
			CResLibRobotData * RobotEnt = new CResLibRobotData();
			RobotEnt->m_sPkID						=	ComponentDlg.m_COMPONENT_PK_ID.GetBuffer();
			RobotEnt->m_sFkpID					=	ComponentDlg.m_COMPONENT_FK_PID.GetBuffer();
			RobotEnt->m_sName			=	ComponentDlg.m_COMPONENT_NAME.GetBuffer();
			RobotEnt->m_sVersions			=	ComponentDlg.m_COMPONENT_VERSION.GetBuffer();
			RobotEnt->m_sSrc		=	ComponentDlg.m_COMPONENT_SRC.GetBuffer();
			RobotEnt->m_RobotType	=ComponentDlg.m_TAG.GetBuffer();
			//ComponentDlg.m_csDesc.Replace("—","-");
			RobotEnt->m_sDsec		=ComponentDlg.m_csDesc.GetBuffer();
			RobotEnt->m_InstanceID	=ComponentDlg.m_INSTANCE_PK_ID.GetBuffer();
			RobotEnt->m_sAuthor		=ComponentDlg.m_COMPONENT_AUTHOR.GetBuffer();
			RobotEnt->m_sCreatTime	=	ComponentDlg.m_COMPONENT_CREATE_TIME.GetBuffer();
			RobotEnt->m_sPublishTime = ZTools::GetNowTimeStr();
			RobotEnt->m_RobotFilePath = ComponentDlg.m_FILE_PATH.GetBuffer();		
			RobotEnt->m_RobotParametersJson		=	ComponentDlg.m_ParametsJson.GetBuffer();
			RobotEnt->m_sEntType="0";
			RobotEnt->m_nResType = CResLibData::RT_ROBOT;
			RobotEnt->m_sSrc="ACDES";
			if(!RobotEnt->PublishFile())//发布文件，本地目录存储
			{
				return false;
			}
			if(!g_ReadRobotDB.AddDB(RobotEnt))
			{
				delete RobotEnt;
				RobotEnt=NULL;					
				return false;
			}
			ComponentDlg.GetpResLibDataBase()->m_pBaseList.push_back(RobotEnt);
			ComponentDlg.GetpResLibDataBase()->m_pBaseList.sort(Cmpare());
			UpdateDataSelChanged();
			return true;
		}
	}
	return false;
}
bool CShoshTreeCtrl::SendProcessTreeDB(PublishInfo ResLibDataInfo)
{
	CDlgResLibProcessEditor ProceDlg;
	ProceDlg.SetSelectTree(TRUE);


	if (ProceDlg.SelectProcessFile(&ResLibDataInfo))
	{
		if (ProceDlg.DoModal() == IDOK)
		{
			ZTools::WriteZToolsFormatLog("开始发布");
			CResLibProcessData* ProcessEnt =new CResLibProcessData();

			ProcessEnt->m_sPkID = ProceDlg.m_PK_ID.GetBuffer();           
			ProcessEnt->m_sFkpID=ProceDlg.m_FK_PID.GetBuffer();                
			ProcessEnt->m_sName=ProceDlg.m_PROCESS_NAME.GetBuffer();         
			ProcessEnt->m_sVersions=ProceDlg.m_PROCESS_VERSION.GetBuffer();   
			ProcessEnt->m_sAuthor=ProceDlg.m_PROCESS_AUTHOR.GetBuffer();   
			//ProceDlg.m_PROCESS_DESC.Replace("—","-");
			ProcessEnt->m_sDsec=ProceDlg.m_PROCESS_DESC.GetBuffer();      
			ProcessEnt->m_ProcessFilePath=ProceDlg.m_PROCESS_FILE_PATH.GetBuffer(); 
			ProcessEnt->m_sOpt=ProceDlg.m_PROCESS_OPT.GetBuffer();    
			ProcessEnt->m_sDsec	=	ProceDlg.m_PROCESS_DESC.GetBuffer();
			ProcessEnt->m_sCreatTime=ProceDlg.m_PROCESS_CREATE_TIME.GetBuffer();    
			ProcessEnt->m_sPublishTime = ZTools::GetNowTimeStr();
			ProcessEnt->m_InstanceID	=	ProceDlg.m_PROCESS_ID.GetBuffer();
			ProcessEnt->m_sEntType="0";  
			ProcessEnt->m_nResType = CResLibData::RT_PROCESS;
			ProcessEnt->m_sSrc="ACDES";

			if(!ProcessEnt->PublishFile())//发布文件，本地目录存储
			{
				return false;
			}
			if(!g_ReadRobotDB.AddDB(ProcessEnt))
			{
				delete ProcessEnt;
				ProcessEnt= NULL;

				return false;
			}
			ProceDlg.GetpResLibDataBase()->m_pBaseList.push_back((CResLibData*)ProcessEnt);
			ProceDlg.GetpResLibDataBase()->m_pBaseList.sort(Cmpare());
			UpdateDataSelChanged();
			ZTools::WriteZToolsFormatLog("完成发布");
			return true;
		}
	}
	return false;
}
bool CShoshTreeCtrl::SendRobotTreeDB(CResLibData* pResLibDataBase)
{
	if (pResLibDataBase == NULL)
	{
		return false;
	}
	CDlgResLibEditor ComponentDlg;
	ComponentDlg.m_COMPONENT_PK_ID = GuidToString(CreateGuid()).c_str();
	ComponentDlg.m_COMPONENT_FK_PID = pResLibDataBase->m_sPkID.c_str();
	ComponentDlg.SetpResLibDataBase(pResLibDataBase);
	ComponentDlg.SetSelectTree(FALSE);
	if (ComponentDlg.SelectRobotFile())
	{
		if (ComponentDlg.DoModal() == IDOK)
		{
			CResLibRobotData * RobotEnt = new CResLibRobotData();
			RobotEnt->m_sPkID						=	ComponentDlg.m_COMPONENT_PK_ID.GetBuffer();
			RobotEnt->m_sFkpID					=	ComponentDlg.m_COMPONENT_FK_PID.GetBuffer();
			RobotEnt->m_sName			=	ComponentDlg.m_COMPONENT_NAME.GetBuffer();
			RobotEnt->m_sVersions			=	ComponentDlg.m_COMPONENT_VERSION.GetBuffer();
			RobotEnt->m_sSrc		=	ComponentDlg.m_COMPONENT_SRC.GetBuffer();
			RobotEnt->m_RobotType	=ComponentDlg.m_TAG.GetBuffer();
			//ComponentDlg.m_csDesc.Replace("—","-");
			RobotEnt->m_sDsec		=ComponentDlg.m_csDesc.GetBuffer();
			RobotEnt->m_InstanceID	=ComponentDlg.m_INSTANCE_PK_ID.GetBuffer();
			RobotEnt->m_sAuthor		=ComponentDlg.m_COMPONENT_AUTHOR.GetBuffer();
			RobotEnt->m_sCreatTime	=	ComponentDlg.m_COMPONENT_CREATE_TIME.GetBuffer();
			RobotEnt->m_sPublishTime = ZTools::GetNowTimeStr();
			RobotEnt->m_RobotFilePath = ComponentDlg.m_FILE_PATH.GetBuffer();		
			RobotEnt->m_RobotParametersJson		=	ComponentDlg.m_ParametsJson.GetBuffer();
			RobotEnt->m_sEntType="0";
			RobotEnt->m_nResType = CResLibData::RT_ROBOT;
			RobotEnt->m_sSrc="ACDES";
			if(!RobotEnt->PublishFile())//发布文件，本地目录存储
			{
				return false;
			}
			if(!g_ReadRobotDB.AddDB(RobotEnt))
			{
				delete RobotEnt;
				RobotEnt=NULL;					
				return false;
			}
			ComponentDlg.GetpResLibDataBase()->m_pBaseList.push_back(RobotEnt);
			ComponentDlg.GetpResLibDataBase()->m_pBaseList.sort(Cmpare());

			return true;
		}
	}
	return false;
}

afx_msg void CShoshTreeCtrl::OnTreeMenuSend()
{
	HTREEITEM hSelect = GetSelectedItem();
	if (hSelect == NULL)
	{
		return ;
	}
	CResLibData* pResLibDataBase =(CResLibData*) GetItemData(hSelect);
	if (pResLibDataBase == NULL)
	{
		return ;
	}
	switch (pResLibDataBase->m_nResType)
	{
	case CResLibData::RT_ROBOT:
		{
			if(!SendRobotTreeDB(pResLibDataBase))
				return ;
		}
		break;
	case CResLibData::RT_PROCESS:
		{
			if (!SendProcessTreeDB(pResLibDataBase))
				return ;
		}
		break;
	}	
	UpdateDataSelChanged();
}
LRESULT CShoshTreeCtrl::OnNotifyDelete(WPARAM wParam, LPARAM lParam)
{
	CCxStaticRes * pRobotImage = NULL;
	//vector<LPARAM>& vParam = *((vector<LPARAM>*) wParam);
	LPARAM * pData = (LPARAM *)wParam;
	int nCount = (int)lParam;

	for (int i=0; i<nCount; i++)
	{
		CResLibData * pRes = reinterpret_cast< CResLibData * > ( pData[i] );
		if ( pRes == NULL ) continue;
		RemoveItem( pRes->m_sPkID );
	}

	delete[] pData;
	pData = NULL;

	UpdateDataSelChanged();
	return NULL;
}
LRESULT CShoshTreeCtrl::OnNotifyEdit(WPARAM wParam, LPARAM lParam)
{
	CCxStaticRes * pRobotImage = NULL;
	//vector<LPARAM>& vParam = *((vector<LPARAM>*) wParam);
	LPARAM * pData = (LPARAM *)wParam;
	int nCount = (int)lParam;
	if (pData != NULL)
	{
		EditRobot((CResLibData*)pData);
	}
	UpdateDataSelChanged();
	return NULL;
}

LRESULT CShoshTreeCtrl::OnDownload(WPARAM wParam, LPARAM lParam)
{
	CCxStaticRes * pRobotImage = NULL;
	//vector<LPARAM>& vParam = *((vector<LPARAM>*) wParam);
	LPARAM * pData = (LPARAM *)wParam;
	int nCount = (int)lParam;
	if (pData != NULL)
	{
		switch (((CResLibData*)pData)->m_nResType)
		{
		case CResLibData::RT_ROBOT:
			{
				CString strFile = _T("");

				CString sDefExt;
				CString sFilter;

				if (ZTools::CompareExtName(((CResLibRobotData*)pData)->m_RobotFilePath, ".bot"))
				{
					sDefExt = "bot";
					sFilter = "Describe Files (*.bot)|*.bot|All Files (*.*)|*.*||";
				}
				else if (ZTools::CompareExtName(((CResLibRobotData*)pData)->m_RobotFilePath, ".botw"))
				{
					sDefExt = "botw";
					sFilter = "Describe Files (*.botw)|*.botw|All Files (*.*)|*.*||";
				}
				else
				{
					break;
				}

				CFileDialog    dlgFile(FALSE, sDefExt, ((CResLibRobotData*)pData)->m_sName.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, NULL);

				if (dlgFile.DoModal() == IDOK)
				{
					CString sFileName = dlgFile.GetPathName();
					if (!ZTools::CompareExtName((LPCTSTR)sFileName, (LPCTSTR)sDefExt))
					{
						sFileName += ".";
						sFileName += sDefExt;
					}

					((CResLibRobotData*)pData)->DownloadFile((LPCTSTR)sFileName);
				}
			}
			break;
		case CResLibData::RT_PROCESS:
			{
				CString strFile = _T("");

				CFileDialog    dlgFile(FALSE, "ws", ((CResLibProcessData*)pData)->m_sName.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Describe Files (*.ws)|*.ws|All Files (*.*)|*.*||"), NULL);

				if (dlgFile.DoModal() == IDOK)
				{
					CString sFileName = dlgFile.GetPathName();
					if (!ZTools::CompareExtName((LPCTSTR)sFileName, "ws"))
					{
						sFileName += ".ws";
					}

					((CResLibProcessData*)pData)->DownloadFile((LPCTSTR)sFileName);
				}
			}
			break;
		}
		
	}
	UpdateDataSelChanged();
	return NULL;
}
void CShoshTreeCtrl::UpdateDataSelChanged()
{
	CWnd * pParent = NULL;
	pParent = GetParent();
	if (pParent == NULL) pParent = GetOwner();
	HTREEITEM hSelect = GetSelectedItem();
	if (hSelect == NULL)
	{
		return;
	}
	CResLibData* pResLibDataBase =(CResLibData*) GetItemData(hSelect);
	if (pResLibDataBase == NULL)
	{
		return ;
	}

	DWORD_PTR* pArray = new DWORD_PTR[pResLibDataBase->m_pBaseList.size()];
	int nRetRobotLen =0;

	for (list<CResLibData*>::iterator it = pResLibDataBase->m_pBaseList.begin(); 
		it != pResLibDataBase->m_pBaseList.end()
		;it++)
	{
		if (((*it))->m_sEntType.find("0") != string::npos)
		{
			pArray[nRetRobotLen++] = (DWORD_PTR)(*it);
		}
	}
	if (pParent != NULL && IsWindow(pParent->GetSafeHwnd()))
		::SendMessage(pParent->GetSafeHwnd()
		, WM_RESLIB_NOTIFY_SELCHANGED ,(WPARAM )pArray, (LPARAM)nRetRobotLen);

	delete[] pArray;
	pArray = NULL;
}
void CShoshTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hSelect;

	CWnd * pParent = NULL;
	pParent = GetParent();
	if (pParent == NULL) pParent = GetOwner();

	UINT flag = TVHT_ONITEM;

	hSelect =pNMTreeView->itemNew.hItem;

	if(hSelect == NULL)
	{
		return;
	}	
	CResLibData* pResLibDataBase =(CResLibData*) GetItemData(hSelect);
	if (pResLibDataBase == NULL)
	{
		return ;
	}

	DWORD_PTR* pArray = new DWORD_PTR[pResLibDataBase->m_pBaseList.size()];
	int nRetRobotLen =0;
	pResLibDataBase->m_pBaseList.sort(Cmpare());
	for (list<CResLibData*>::iterator it = pResLibDataBase->m_pBaseList.begin(); 
		it != pResLibDataBase->m_pBaseList.end()
		;it++)
	{
		if (((*it))->m_sEntType.find("0") != string::npos)
		{
			pArray[nRetRobotLen++] = (DWORD_PTR)(*it);
		}
	}
	if (pParent != NULL && IsWindow(pParent->GetSafeHwnd()))
		::SendMessage(pParent->GetSafeHwnd()
		, WM_RESLIB_NOTIFY_SELCHANGED ,(WPARAM )pArray, (LPARAM)nRetRobotLen);

	delete[] pArray;
	pArray = NULL;
}
void CShoshTreeCtrl::CleanSelchanged()
{

	CWnd * pParent = NULL;
	pParent = GetParent();
	if (pParent == NULL) pParent = GetOwner();

	if (pParent != NULL && IsWindow(pParent->GetSafeHwnd()))
		::SendMessage(pParent->GetSafeHwnd()
		, WM_RESLIB_NOTIFY_SELCHANGED ,(WPARAM )NULL, (LPARAM)0);
}

void CShoshTreeCtrl::Myreplace(string &str,string oldstr,string newstr) 
{  
	int pos=string::npos; 
	int start_index=0;  
	int old_str_len=oldstr.size(),new_str_len=new_str_len=newstr.size(); 
	while((pos=str.find(oldstr,start_index))!=string::npos) 
	{  
		str.replace(pos,old_str_len,newstr); 
		start_index=pos+new_str_len; 
	} 
}  


//添加设置字体、颜色、粗体的函数
void CShoshTreeCtrl::SetItemFont(HTREEITEM hItem, LOGFONT& logfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.color = (COLORREF)-1;
	cf.logfont = logfont;
	m_mapColorFont[hItem] = cf;
}
void CShoshTreeCtrl::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
	SetItemState( hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD );
}
void CShoshTreeCtrl::SetItemColor(HTREEITEM hItem, COLORREF color)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.logfont.lfFaceName[0] = '\0';
	cf.color = color;
	m_mapColorFont[hItem] = cf;
}
BOOL CShoshTreeCtrl::GetItemFont(HTREEITEM hItem, LOGFONT * plogfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return FALSE;
	if( cf.logfont.lfFaceName[0] == '\0' )
		return FALSE;*plogfont = cf.logfont;
	return TRUE;
}
BOOL CShoshTreeCtrl::GetItemBold(HTREEITEM hItem)
{
	return GetItemState( hItem, TVIS_BOLD ) & TVIS_BOLD;
}
COLORREF CShoshTreeCtrl::GetItemColor(HTREEITEM hItem)
{
	// Returns (COLORREF)-1 if color was not set
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return (COLORREF)-1;
	return cf.color;
}
//添加WM_PAINT 信息监控，重载OnPaint()函数来实现绘制
//void CShoshTreeCtrl::OnPaint()
//{
//	//获取当前绘制对象的DC
//	CPaintDC dc(this);
//	// 使用将要绘制的对象的DC创建一个memory DC
//	//memory device context的概念：是在内存中创建一个结构来反映一个显示（屏幕区域、窗口、
//	//打印机等）的表面。可以用来先在内存中准备好要显示的图像，从而实现双缓存，提高刷新
//	//速度减少刷新时产生的闪烁。
//	CDC memDC;
//	//从当前DC创建内存对象
//	memDC.CreateCompatibleDC( &dc );
//	//定义CRect对象，用来确定区域
//	CRect rcClip, rcClient;
//	//获取当前对象的边界区域
//	dc.GetClipBox( &rcClip );
//	//获取当前对象的用户区域
//	GetClientRect(&rcClient);
//	// Select a compatible bitmap into the memory DC
//	//创建一个bmp文件，作为memDC的内容
//	//该文件的大小与用于区域相同
//	CBitmap bitmap;
//	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
//	memDC.SelectObject( &bitmap );
//	// Set clip region to be same as that in paint DC
//	//通过对象的边界区域创建CRgn对象
//	CRgn rgn;
//	rgn.CreateRectRgnIndirect( &rcClip );
//	memDC.SelectClipRgn(&rgn);
//	rgn.DeleteObject();
//	// First let the control do its default drawing.
//	//首先让控件自己进行默认的绘制，绘制到内存中
//	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
//	//获取树状控件的第一个节点
//	HTREEITEM hItem = GetFirstVisibleItem();
//	//遍历这棵树
//	int n = GetVisibleCount()+1;while( hItem && n--)
//	{
//		CRect rect;
//		// Do not meddle with selected items or drop highlighted items
//		//不对选中的节点和实行拖放功能的节点进行操作
//		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
//		//定义字体、颜色
//		Color_Font cf;
//		//设置字体
//		if ( !(GetItemState( hItem, selflag ) & selflag )&& m_mapColorFont.Lookup( hItem, cf ))
//		{
//			CFont *pFontDC;
//			CFont fontDC;
//			LOGFONT logfont;
//			if( cf.logfont.lfFaceName[0] != '\0' )
//			{
//				//用户定义了字体
//				logfont = cf.logfont;
//			}
//			else
//			{
//				// 用户没有定义，使用系统字体
//				CFont *pFont = GetFont();
//				pFont->GetLogFont( &logfont );
//			}
//			//用户是否设定节点为加粗
//			if( GetItemBold( hItem ) )
//				logfont.lfWeight = 700;
//			//创建字体
//			fontDC.CreateFontIndirect( &logfont );
//			pFontDC = memDC.SelectObject( &fontDC );
//			//设置字体颜色
//			if( cf.color != (COLORREF)-1 )
//				memDC.SetTextColor( cf.color );
//			//获取节点文字
//			CString sItem = GetItemText( hItem );
//			//获取节点区域
//			GetItemRect( hItem, &rect, TRUE );
//			//设置背景色为系统色
//			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
//			//向内存中的图片写入内容,为该节点的内容
//			memDC.TextOut( rect.left+2, rect.top+1, sItem );
//			memDC.SelectObject( pFontDC );
//		}
//		hItem = GetNextVisibleItem( hItem );
//	}
//	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC,rcClip.left, rcClip.top, SRCCOPY );
//}


void CShoshTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	switch (pNMCD->nmcd.dwDrawStage)
	{   
	case CDDS_PREPAINT:  
		*pResult = CDRF_NOTIFYITEMDRAW;   
		return; 
	case CDDS_ITEMPREPAINT: 
		if(GetSelectedItem() == (HTREEITEM)pNMCD->nmcd.dwItemSpec)
		{
			//if (GetItemState((HTREEITEM)pNMCD->nmcd.dwItemSpec, TVIS_SELECTED) == TVIS_SELECTED)  
			{   
				pNMCD->clrText =RGB(0, 0, 0);
				pNMCD->clrTextBk = g_crStructTreeSelItemBg;
			}
		}		
		break;  
	}  
	*pResult = 0;
	return;
	// 	NMTVCUSTOMDRAW *pListCtrl=(NMTVCUSTOMDRAW *)pNMHDR;
	// 	if ( CDDS_PREPAINT == pListCtrl->nmcd.dwDrawStage )
	// 	{
	// 
	// 		*pResult = CDRF_NOTIFYITEMDRAW;
	// 	}
	// 
	// 	else if ( CDDS_ITEMPREPAINT == pListCtrl->nmcd.dwDrawStage )
	// 	{
	// 
	// 		pListCtrl->clrText=RGB(0,0,0);//字体颜色
	// 		CDC dc;
	// 		dc.Attach(pListCtrl->nmcd.hdc);
	// 		dc.SelectObject(&theApp.m_font);
	// 		dc.Detach();
	// 		*pResult =CDRF_DODEFAULT;
	// 	} 


}
