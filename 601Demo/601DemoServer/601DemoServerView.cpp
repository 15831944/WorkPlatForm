
// 601DemoServerView.cpp : CMy601DemoServerView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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

// CMy601DemoServerView ����/����

CMy601DemoServerView::CMy601DemoServerView()
{
	// TODO: �ڴ˴���ӹ������
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

void CMy601DemoServerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: ���� GetListCtrl() ֱ�ӷ��� ListView ���б�ؼ���
	//  �Ӷ������������ ListView��

	CListCtrl & listCtrl =  GetListCtrl();
	LONG lStyle;
	lStyle = GetWindowLong(listCtrl.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
	lStyle |= LVS_REPORT; //����style
	SetWindowLong(listCtrl.m_hWnd, GWL_STYLE, lStyle);//����style

	DWORD dwStyle = listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	//dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
	listCtrl.SetExtendedStyle(dwStyle); //������չ���

	CRect rect;
	GetListCtrl().GetClientRect(rect);
	GetListCtrl().InsertColumn(0,"���ʱ��",LVCFMT_LEFT,150);
	GetListCtrl().InsertColumn(1,"��־��Ϣ",LVCFMT_LEFT,rect.Width() - 300);
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


// CMy601DemoServerView ���

#ifdef _DEBUG
void CMy601DemoServerView::AssertValid() const
{
	CListView::AssertValid();
}

void CMy601DemoServerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMy601DemoServerDoc* CMy601DemoServerView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy601DemoServerDoc)));
	return (CMy601DemoServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy601DemoServerView ��Ϣ�������


void CMy601DemoServerView::OnServerPortSet()
{
	// TODO: �ڴ���������������
	CServerSetDlg SetDlg;
	SetDlg.m_strServerPort = theApp.m_strServerPort;
	if (SetDlg.DoModal()!=IDOK)
		return;

	if (SetDlg.m_strServerPort != theApp.m_strServerPort)
	{
		theApp.m_strServerPort = SetDlg.m_strServerPort;
		m_bServerConfigChanged = TRUE;
		CString strLogMsg;
		strLogMsg.Format("�޸Ķ˿ں�Ϊ��%s",SetDlg.m_strServerPort);
		AddLogMsg(strLogMsg);
		if (m_pIceServiceServer)
		{
			AddLogMsg("�������˿ڷ����仯��������������...");
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
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
	GetListCtrl().DeleteAllItems();
}

void CMy601DemoServerView::OnStartservice()
{
	// TODO: �ڴ���������������
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
		AddLogMsg("������ֹͣ��");
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
	// TODO: �ڴ������������û����洦��������
	if (m_pIceServiceServer)
	{
		pCmdUI->SetText("ֹͣ����");
	}
	else
	{
		pCmdUI->SetText("��������");
	}
}


void CMy601DemoServerView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
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
