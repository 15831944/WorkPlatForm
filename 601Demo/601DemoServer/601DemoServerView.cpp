
// 601DemoServerView.cpp : CMy601DemoServerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "601DemoServer.h"
#endif

#include "601DemoServerDoc.h"
#include "601DemoServerView.h"
#include "ServerSetDlg.h"
#include <Ice/Ice.h>
#include "PeraServerIceI.h"
#include "IceService.h"
#include <string>
#include "Tools.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy601DemoServerView

IMPLEMENT_DYNCREATE(CMy601DemoServerView, CListView)

BEGIN_MESSAGE_MAP(CMy601DemoServerView, CListView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_SERVER_PORT_SET, &CMy601DemoServerView::OnServerPortSet)
	ON_COMMAND(ID_QUIT_SERVER, &CMy601DemoServerView::OnQuitServer)
	ON_COMMAND(ID_CLEAR_LOG, &CMy601DemoServerView::OnClearLog)
	ON_COMMAND(ID_StartService, &CMy601DemoServerView::OnStartservice)
	ON_MESSAGE(WM_LOG_MSG,&CMy601DemoServerView::OnMsgLogMsg)
	ON_UPDATE_COMMAND_UI(ID_StartService, &CMy601DemoServerView::OnUpdateStartservice)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CMy601DemoServerView 构造/析构

CMy601DemoServerView::CMy601DemoServerView()
{
	// TODO: 在此处添加构造代码
	m_pIceServiceServer = NULL;
	m_bServerConfigChanged = FALSE;
}

CMy601DemoServerView::~CMy601DemoServerView()
{
	if (m_pIceServiceServer)
	{
		m_pIceServiceServer->OnStop();
		delete m_pIceServiceServer;
		m_pIceServiceServer = NULL;
	}
}

BOOL CMy601DemoServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CMy601DemoServerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
	//  从而可以用项填充 ListView。

	CListCtrl & listCtrl =  GetListCtrl();
	LONG lStyle;
	lStyle = GetWindowLong(listCtrl.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(listCtrl.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	listCtrl.SetExtendedStyle(dwStyle); //设置扩展风格

	CRect rect;
	GetListCtrl().GetClientRect(rect);
	GetListCtrl().InsertColumn(0,"输出时间",LVCFMT_LEFT,150);
	GetListCtrl().InsertColumn(1,"日志信息",LVCFMT_LEFT,rect.Width() - 300);
	OnStartservice();
}

void CMy601DemoServerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMy601DemoServerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	return;

#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMy601DemoServerView 诊断

#ifdef _DEBUG
void CMy601DemoServerView::AssertValid() const
{
	CListView::AssertValid();
}

void CMy601DemoServerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMy601DemoServerDoc* CMy601DemoServerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy601DemoServerDoc)));
	return (CMy601DemoServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy601DemoServerView 消息处理程序


void CMy601DemoServerView::OnServerPortSet()
{
	// TODO: 在此添加命令处理程序代码
	CServerSetDlg SetDlg;
	SetDlg.m_strServerPort = theApp.m_strServerPort;
	if (SetDlg.DoModal()!=IDOK)
		return;

	if (SetDlg.m_strServerPort != theApp.m_strServerPort)
	{
		theApp.m_strServerPort = SetDlg.m_strServerPort;
		m_bServerConfigChanged = TRUE;
		CString strLogMsg;
		strLogMsg.Format("修改端口号为：%s",SetDlg.m_strServerPort);
		AddLogMsg(strLogMsg);
		if (m_pIceServiceServer)
		{
			AddLogMsg("服务器端口发生变化，正在重启服务...");
			m_pIceServiceServer->OnStop();
			delete m_pIceServiceServer;
			m_pIceServiceServer = NULL;
			OnStartservice();
		}
		theApp.WriteSettingToIni();
	}
}
void CMy601DemoServerView::OnQuitServer()
{
	// TODO: 在此添加命令处理程序代码
	//PostQuitMessage(0);
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,NULL);
}

void  CMy601DemoServerView::AddLogMsg(CString strMsg)
{
	CTime t = CTime::GetCurrentTime(); 
	CString strTime = t.Format("%Y-%m-%d %H:%M:%S");
	CListCtrl & listCtrl = GetListCtrl();
	int iIndex = listCtrl.GetItemCount();
	listCtrl.InsertItem(iIndex,strTime,0);
	listCtrl.SetItemText(iIndex,1,strMsg);
	listCtrl.EnsureVisible(listCtrl.GetItemCount()-1,TRUE);
	ZTools::WriteZToolsFormatLog(strMsg);
}

void CMy601DemoServerView::OnClearLog()
{
	// TODO: 在此添加命令处理程序代码
	GetListCtrl().DeleteAllItems();
}

void CMy601DemoServerView::OnStartservice()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pIceServiceServer ==NULL)
	{
		m_pIceServiceServer = new CIceService;
		m_pIceServiceServer->SetServerPort((LPCTSTR)theApp.m_strServerPort);
		m_pIceServiceServer->StartService();
	}
	else
	{
		m_pIceServiceServer->OnStop();
		delete m_pIceServiceServer;
		m_pIceServiceServer = NULL;
		AddLogMsg("服务已停止！");
	}
}
LRESULT CMy601DemoServerView::OnMsgLogMsg(WPARAM wp,LPARAM lp)
{
	std::string *pstrMsg = (std::string*)(lp);
	if (lp == NULL)
		return S_FALSE;

	if (pstrMsg->empty())
		return S_OK;

	AddLogMsg(pstrMsg->c_str());
	delete pstrMsg;
	return S_OK;
}

void CMy601DemoServerView::OnUpdateStartservice(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (m_pIceServiceServer)
	{
		pCmdUI->SetText("停止服务");
	}
	else
	{
		pCmdUI->SetText("启动服务");
	}
}


void CMy601DemoServerView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(::IsWindow(GetListCtrl().m_hWnd))
	{
		CListCtrl &list = GetListCtrl();
		CHeaderCtrl *pHeader = list.GetHeaderCtrl();
		if(pHeader!=NULL && ::IsWindow(pHeader->m_hWnd))
		{
			if (pHeader->GetItemCount() == 2)
			{
				list.SetColumnWidth(1,cx - 150);
			}
		}

	}
}
