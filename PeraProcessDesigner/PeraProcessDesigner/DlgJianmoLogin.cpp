// DlgJianmoLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraWorkspace.h"
#include "DlgJianmoLogin.h"
#include "CxBCGPGridItem.h"
#include "ViewSet.h"
#include "DlgSvg.h"
#include <dwmapi.h>

static const LPCTSTR arrColName[] = {
	"名称", "位置", "操作"
};
static const int arrColWidth[] = {
	100, 170, 48
};

// CDlgJianmoLogin 对话框

BEGIN_MESSAGE_MAP(CDlgJianmoLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgJianmoLogin::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CDlgJianmoLogin::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_JML_NEW, &CDlgJianmoLogin::OnBnClickedJmlNew)
	ON_BN_CLICKED(IDC_JML_OPEN, &CDlgJianmoLogin::OnBnClickedJmlOpen)
	ON_COMMAND_RANGE(ID_GRID_BUTTON_FIRST, ID_GRID_BUTTON_LAST, OnGridButtonClicked)
END_MESSAGE_MAP()

void CDlgJianmoLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JML_NEW, m_wndNew);
	DDX_Control(pDX, IDC_JML_OPEN, m_wndOpen);
	DDX_Control(pDX, IDC_JML_HISTORY, m_wndHistoryList);
}

IMPLEMENT_DYNAMIC(CDlgJianmoLogin, CDialogEx)

CDlgJianmoLogin::CDlgJianmoLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgJianmoLogin::IDD, pParent)
{

}

CDlgJianmoLogin::~CDlgJianmoLogin()
{
}


void CDlgJianmoLogin::OnBnClickedOk()
{
}


void CDlgJianmoLogin::OnBnClickedCancel()
{
}


BOOL CDlgJianmoLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwAttr = DWMNCRP_DISABLED;
	globalData.DwmSetWindowAttribute (this->GetSafeHwnd(), DWMWA_NCRENDERING_POLICY, &dwAttr, sizeof(DWORD) );
	InitHistoryList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgJianmoLogin::InitHistoryList( void )
{
	if ( IsWindow( m_wndGridHistory.GetSafeHwnd() ) ) return FALSE;

	CRect rc;
	m_wndHistoryList.GetClientRect (&rc);
	m_wndHistoryList.MapWindowPoints (this, &rc);

	m_wndGridHistory.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rc, this, (UINT)-1);
	m_wndGridHistory.m_hwndRecvMsg = this->GetSafeHwnd();
	m_wndGridHistory.EnableColumnAutoSize (FALSE);
	m_wndGridHistory.SetSingleSel (TRUE);
	m_wndGridHistory.EnableGroupByBox (FALSE);
	m_wndGridHistory.SetReadOnly (TRUE);
	m_wndGridHistory.SetWholeRowSel (TRUE);
	m_wndGridHistory.EnableHeader (TRUE, 0);

	m_wndGridHistory.SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	//设置颜色
	CBCGPGridColors colors;
	colors.m_LeftOffsetColors.m_clrBackground = globalData.clrWindow;
	m_wndGridHistory.SetColorTheme (colors);
	//初始化列
	int nColCount = sizeof( arrColName ) / sizeof( LPCTSTR );
	for (int i=0; i<nColCount; i++)
	{
		m_wndGridHistory.InsertColumn ( i, arrColName[i], arrColWidth[i] );
	}
	//取得行数据
	LoadHistoryList();
	//更新
	m_wndGridHistory.AdjustLayout ();
	return TRUE;
}

BOOL CDlgJianmoLogin::LoadHistoryList( void )
{
	if ( !IsWindow( m_wndGridHistory.GetSafeHwnd() ) ) return FALSE;
	if ( m_wndGridHistory.GetColumnCount() <= 0 ) return FALSE;

	m_xhm;
	m_xhm.Load();
	const int nColumns = m_wndGridHistory.GetColumnCount ();
	CBCGPGridRow * pRow = NULL;
	CCxGridButtonItem * pBtn = NULL;
	for ( int i=0; i<(int)m_xhm.m_vDatas.size(); i++ )
	{
		pRow = m_wndGridHistory.CreateRow (nColumns);
		pRow->AllowSubItems ();
		pRow->GetItem (COL_NAME)->SetValue ( (LPCTSTR)m_xhm.m_vDatas[i]->m_strName );
		pRow->GetItem (COL_PATH)->SetValue ( (LPCTSTR)m_xhm.m_vDatas[i]->m_strPath );

		pBtn = new CCxGridButtonItem (_T("打开"), ID_GRID_BUTTON_FIRST);
		pRow->ReplaceItem (COL_OP, pBtn);

		m_wndGridHistory.AddRow (pRow, FALSE);
	}
	return TRUE;
}


void CDlgJianmoLogin::OnBnClickedJmlNew()
{
	theApp.m_pMainWnd->PostMessage(WM_COMMAND, IDM_TASK_NEW);
	EndDialog(TRUE); return;
}


void CDlgJianmoLogin::OnBnClickedJmlOpen()
{
	theApp.m_pMainWnd->PostMessage(WM_COMMAND, IDM_TASK_OPEN);
	EndDialog(TRUE);  return;
}


void CDlgJianmoLogin::OnGridButtonClicked(UINT uID)
{
	static const int BTN_OP = 0;
	uID -= ID_GRID_BUTTON_FIRST;

	CBCGPGridRow * pRow = m_wndGridHistory.GetCurSel();
	if ( pRow == NULL ) return;
	CHistoryModelData * pData = NULL;

	switch (uID)
	{
	case BTN_OP: //
		 pData = m_xhm.m_vDatas[ pRow->GetRowId() ];
		break;

	default:
		return;
	}

	if (pData != NULL)
	{
		theApp.m_processMgr.Open(pData->m_strPath);
// 		theApp.m_strXpdlName = pData->m_strName;
// 		theApp.m_strXpdlWorkPath = pData->m_strPath;
// 
// 		VIEW_DATA * pView = theViewMgr.Find( "建模环境" );
// 		if ( pView == NULL )
// 		{
// 			pView = theViewMgr.NewView("建模环境");
// 			if ( pView == NULL ) return;
// 		}
// 		CDlgSvg * pSvg = reinterpret_cast< CDlgSvg * >( pView->m_dwExData );
// 		if ( pSvg == NULL ) return;
// 		pSvg->Navigate( theGlobalConfig.m_strSvgUrl );
	}

	EndDialog(TRUE); return;
}