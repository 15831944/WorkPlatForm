
// 601DemoClientView.cpp : CMy601DemoClientView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "601DemoClient.h"
#endif

#include "601DemoClientDoc.h"
#include "601DemoClientView.h"
#include "ConnectionSetDlg.h"
#include "IceService.h"
#include "TestInvokeDlg.h"
#include "JsonData.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Tools.h"
// CMy601DemoClientView

UINT WM_START_LOCAL_SERVICE = ::RegisterWindowMessage (_T("WM_START_LOCAL_SERVICE"));

IMPLEMENT_DYNCREATE(CMy601DemoClientView, CListView)

BEGIN_MESSAGE_MAP(CMy601DemoClientView, CListView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_TestInvoke, &CMy601DemoClientView::OnTestinvoke)
	ON_COMMAND(ID_ServerConfig, &CMy601DemoClientView::OnConnectionSet)
	ON_COMMAND(ID_QUIT, &CMy601DemoClientView::OnQuit)
	ON_MESSAGE(WM_LOG_MSG,&CMy601DemoClientView::OnMsgLogMsg)
	ON_COMMAND(ID_CLEAR_LOG, &CMy601DemoClientView::OnClearLog)
	ON_COMMAND(ID_TestInvokeUserDefine, &CMy601DemoClientView::OnTestinvokeuserdefine)
	ON_REGISTERED_MESSAGE(WM_START_LOCAL_SERVICE,&CMy601DemoClientView::OnMsgStartLocalService)
	ON_WM_SIZE()
	ON_COMMAND(ID_TestInvokeByIp, &CMy601DemoClientView::OnTestinvokeByIp)
	ON_COMMAND(ID_Test_Invoke_By_Domain, &CMy601DemoClientView::OnTestInvokeByDomain)
END_MESSAGE_MAP()

// CMy601DemoClientView 构造/析构

DWORD WINAPI ThreadFunTestAdd(LPVOID pData)
{
	CDataAddFunParam DataAdd;
	DataAdd.m_da = 2.3;
	DataAdd.m_db = 1.2;
	theApp.GetMainWnd()->EnableWindow(FALSE);

	InvokeServerMethod * pInvokeObj =(InvokeServerMethod*)pData;
	std::string str = pInvokeObj->InvokeMethod("add",(LPCTSTR)DataAdd.GetJsonStr());
	theApp.TraceRunLog("-------------------完成测试--------------");

	theApp.GetMainWnd()->EnableWindow(TRUE);
	return 0;
}

CMy601DemoClientView::CMy601DemoClientView()
{
	// TODO: 在此处添加构造代码
	
	m_pIceServiceClient = NULL;
	if (theApp.m_bUseIp)
		m_ServerObj.SetServerIP(theApp.m_strServerIp);
	else
		m_ServerObj.SetServerIP(theApp.m_strServerDomain);

	m_ServerObj.SetServerPort(theApp.m_strServerPort);
	m_ServerObj.SetLocalPort(theApp.m_strLocalServicePort);
}

CMy601DemoClientView::~CMy601DemoClientView()
{
	if (m_pIceServiceClient)
	{
		m_pIceServiceClient->OnStop();
		delete m_pIceServiceClient;
		m_pIceServiceClient = NULL;
	}
}

BOOL CMy601DemoClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CListView::PreCreateWindow(cs);
}

void CMy601DemoClientView::OnInitialUpdate()
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
	GetListCtrl().InsertColumn(1,"日志信息",LVCFMT_LEFT,rect.Width() - 150);
	PostMessage(WM_START_LOCAL_SERVICE,0,NULL);
}

void CMy601DemoClientView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMy601DemoClientView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMy601DemoClientView 诊断

#ifdef _DEBUG
void CMy601DemoClientView::AssertValid() const
{
	CListView::AssertValid();
}

void CMy601DemoClientView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMy601DemoClientDoc* CMy601DemoClientView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy601DemoClientDoc)));
	return (CMy601DemoClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy601DemoClientView 消息处理程序


void CMy601DemoClientView::OnTestinvoke()
{
	// TODO: 在此添加命令处理程序代码
	CreateThread(NULL,0,ThreadFunTestAdd,&m_ServerObj,0,NULL);
}


void CMy601DemoClientView::OnConnectionSet()
{
	// TODO: 在此添加命令处理程序代码
	CConnectionSetDlg dlg;
	dlg.m_strLocalPort  = theApp.m_strLocalServicePort;
	dlg.m_strServerPort = theApp.m_strServerPort;
	dlg.m_strServerIp   = theApp.m_strServerIp;
	dlg.m_strServerDomain = theApp.m_strServerDomain;
	dlg.m_iUseIp        = theApp.m_bUseIp? 0 : -1;

	if (dlg.DoModal()!=IDOK)
		return;

	if (theApp.m_strLocalServicePort!= dlg.m_strLocalPort)
	{
		theApp.m_strLocalServicePort = dlg.m_strLocalPort;
		if (m_pIceServiceClient)
		{
			AddLogMsg("本地服务端口已改变,正在重启本地服务...");
			m_pIceServiceClient->OnStop();
			delete m_pIceServiceClient;
			m_pIceServiceClient = NULL;
			PostMessage(WM_START_LOCAL_SERVICE,0,NULL);
		}
	}
	theApp.m_bUseIp = dlg.m_iUseIp==0 ? TRUE:FALSE;
	theApp.m_strServerPort = dlg.m_strServerPort;

	if (theApp.m_bUseIp)
		theApp.m_strServerIp   = dlg.m_strServerIp;
	else
		theApp.m_strServerDomain = dlg.m_strServerDomain;

	CString strServerLocation;
	if (theApp.m_bUseIp)
		strServerLocation = theApp.m_strServerIp;
	else
		strServerLocation = theApp.m_strServerDomain;

	m_ServerObj.SetServerIP(strServerLocation);
	m_ServerObj.SetServerPort(theApp.m_strServerPort);
	m_ServerObj.SetLocalPort(theApp.m_strLocalServicePort);

	theApp.WriteSettingToIni(); // 写配置信息
	CString strMsg;
	strMsg.Format("通讯设置：服务器地址：%s,服务器端口号：%s,本机服务端口号%s",strServerLocation,theApp.m_strServerPort,theApp.m_strLocalServicePort);
	theApp.TraceRunLog(strMsg);
	MessageBox("设置成功!","提示",MB_OK|MB_ICONINFORMATION);
}


void CMy601DemoClientView::OnQuit()
{
	// TODO: 在此添加命令处理程序代码
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,NULL);
}
LRESULT CMy601DemoClientView::OnMsgLogMsg(WPARAM wp,LPARAM lp)
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
void  CMy601DemoClientView::AddLogMsg(CString strMsg)
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

void CMy601DemoClientView::OnClearLog()
{
	// TODO: 在此添加命令处理程序代码
	CListCtrl & listCtrl = GetListCtrl();
	listCtrl.DeleteAllItems();
}


void CMy601DemoClientView::OnTestinvokeuserdefine()
{
	// TODO: 在此添加命令处理程序代码
	CTestInvokeDlg *pDlg = new CTestInvokeDlg(&m_ServerObj);
	pDlg->Create(pDlg->IDD,this);
	pDlg->ShowWindow(SW_SHOW);
}
CString CMy601DemoClientView::GetConfigIniFile()
{
	CString strFileName;
	AfxGetModuleFileName(NULL,strFileName);
	strFileName = strFileName.Left(strFileName.GetLength() -3);
	strFileName  += "ini";
	return strFileName;

}
LRESULT CMy601DemoClientView::OnMsgStartLocalService(WPARAM wp,LPARAM lp)
{
	m_pIceServiceClient = new CIceService;
	m_pIceServiceClient->SetServerPort((LPCTSTR)theApp.m_strLocalServicePort);
	m_pIceServiceClient->StartService();

	return S_OK;
}

void CMy601DemoClientView::OnSize(UINT nType, int cx, int cy)
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


void CMy601DemoClientView::OnTestinvokeByIp()
{
	// TODO: 在此添加命令处理程序代码
	theApp.TraceRunLog("------------开始测试通过Ip调用服务器--------------");
	m_ServerObj.SetServerIP(theApp.m_strServerIp);
	CreateThread(NULL,0,ThreadFunTestAdd,&m_ServerObj,0,NULL);
}


void CMy601DemoClientView::OnTestInvokeByDomain()
{
	// TODO: 在此添加命令处理程序代码
	theApp.TraceRunLog("------------开始测试通过域名调用服务器--------------");
	m_ServerObj.SetServerIP(theApp.m_strServerDomain);
	CreateThread(NULL,0,ThreadFunTestAdd,&m_ServerObj,0,NULL);
}
