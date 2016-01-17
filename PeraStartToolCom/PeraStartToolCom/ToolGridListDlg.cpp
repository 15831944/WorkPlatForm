// ToolGridListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolGridListDlg.h"
#include "afxdialogex.h"


// CToolGridListDlg 对话框

IMPLEMENT_DYNAMIC(CToolGridListDlg, CCxBCGPDialog)

CToolGridListDlg::CToolGridListDlg(map<CString,ToolObject> toolmap,CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CToolGridListDlg::IDD, pParent)
{
	m_ToolMap = toolmap;
}

CToolGridListDlg::~CToolGridListDlg()
{
}

void CToolGridListDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GRID, m_wndGridListPos);
}


BEGIN_MESSAGE_MAP(CToolGridListDlg, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_SEL, &CToolGridListDlg::OnBnClickedButtonEditSel)
	ON_BN_CLICKED(IDC_BUTTON_DEL_SEL, &CToolGridListDlg::OnBnClickedButtonDelSel)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ALL, &CToolGridListDlg::OnBnClickedButtonDelAll)
	ON_MESSAGE(WM_MSG_GRID_DBCLK,&CToolGridListDlg::OnMsgGridDoubleClick)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CToolGridListDlg 消息处理程序


BOOL CToolGridListDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitGridList();
	EnableVisualManagerStyle (TRUE, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void  CToolGridListDlg::LoadTooList()
{
	if ( !IsWindow( m_wndGridTools.GetSafeHwnd() ) ) return ;
	if ( m_wndGridTools.GetColumnCount() <= 0 ) return;

	const int nColumns = m_wndGridTools.GetColumnCount ();
	CBCGPGridRow * pRow = NULL;
	map<CString,ToolObject>::iterator iter = m_ToolMap.begin();
	for (; iter!=m_ToolMap.end(); iter++ )
	{
		pRow = m_wndGridTools.CreateRow (nColumns);
		pRow->AllowSubItems ();
		pRow->GetItem (0)->SetValue ((LPCTSTR)iter->second.m_strID);
		pRow->GetItem (1)->SetValue ((LPCTSTR)iter->second.m_strName);
		pRow->GetItem (2)->SetValue ((LPCTSTR)iter->second.m_strDescription);
		pRow->GetItem (3)->SetValue ((LPCTSTR)iter->second.m_strLocalPath);
		pRow->GetItem (4)->SetValue ((LPCTSTR)iter->second.m_strRunParam);
		m_wndGridTools.AddRow (pRow, FALSE);
	}
}
BOOL  CToolGridListDlg::InitGridList()
{
	if ( IsWindow( m_wndGridTools.GetSafeHwnd() ) ) return FALSE;

	CRect rc;
	m_wndGridListPos.GetClientRect (&rc);
	m_wndGridListPos.MapWindowPoints (this, &rc);
	m_wndGridListPos.ShowWindow(SW_HIDE);
	rc.InflateRect(-1, -1, -1, -1);

	m_wndGridTools.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, this, (UINT)-1);
	m_wndGridTools.EnableColumnAutoSize (TRUE);
	m_wndGridTools.SetSingleSel (TRUE);
	m_wndGridTools.EnableGroupByBox (FALSE);
	m_wndGridTools.SetReadOnly (TRUE);
	m_wndGridTools.SetWholeRowSel (TRUE);
	m_wndGridTools.EnableHeader (TRUE, 0);

	m_wndGridTools.SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	m_wndGridTools.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);

	//设置颜色
	CBCGPGridColors theme;
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme (&m_wndGridTools, theme);
	theme.m_clrBackground = RGB(249, 249, 249);
	theme.m_HeaderSelColors.InitColors();

	theme.m_clrHorzLine =  RGB( 0xd9, 0xd9, 0xd9 );
	theme.m_clrVertLine = RGB( 0xd9, 0xd9, 0xd9 );
	theme.m_SelColors.InitColors();
	theme.m_SelColors.m_clrBackground =  RGB( 0xfc, 0xeb, 0xbf );
	m_wndGridTools.SetColorTheme (theme);

	static const char * arrColName[] = {"软件ID","软件名称","软件描述","本地路径","运行参数"};
	static const int arrColWidth []  = {100,100,150,300,100};

	//初始化列
	int nColCount = sizeof( arrColName ) / sizeof( LPCTSTR );
	for (int i=0; i<nColCount; i++)
	{
		m_wndGridTools.InsertColumn ( i, arrColName[i], arrColWidth[i] );
		m_wndGridTools.SetHeaderAlign(i, HDF_CENTER);
	}
	//取得行数据
	LoadTooList();
	//更新
	m_wndGridTools.AdjustLayout ();

	m_wndGridTools.m_hwndRecvMsg = GetSafeHwnd();
	return TRUE;
}

void CToolGridListDlg::OnBnClickedButtonEditSel()
{
	// TODO: 在此添加控件通知处理程序代码
	CBCGPGridRow *pSelRow = m_wndGridTools.GetCurSel();
	if (pSelRow==NULL) return;

	CBCGPGridItem * pItemId = pSelRow->GetItem(0);
	if (pItemId==NULL) return;

	CString strId = pItemId->GetValue(); 
	if (m_ToolMap.find(strId)== m_ToolMap.end()) return;
		
	ToolObject & tool = m_ToolMap[strId];
	if (CToolsProUnity::ConfigTool(&tool))
	{
		CToolsProUnity::WriteToolConfigXml(tool);
	}
}


void CToolGridListDlg::OnBnClickedButtonDelSel()
{
	// TODO: 在此添加控件通知处理程序代码
	CBCGPGridRow *pSelRow = m_wndGridTools.GetCurSel();
	if (pSelRow==NULL) return;

	CBCGPGridItem * pItemId = pSelRow->GetItem(0);
	if (pItemId==NULL) return;

	CString strId = pItemId->GetValue();
	map<CString,ToolObject>::iterator iterFind = m_ToolMap.find(strId);
	if (iterFind== m_ToolMap.end()) return;

	if (MessageBox("确定要删除选中的配置？","提示",MB_YESNO|MB_ICONQUESTION)==IDNO)
		return;

	m_ToolMap.erase(iterFind);
	CToolsProUnity::DeleteToolConfigFromXml(strId);
	m_wndGridTools.RemoveRow(pSelRow->GetRowId());
	m_wndGridTools.AdjustLayout ();
}


void CToolGridListDlg::OnBnClickedButtonDelAll()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_ToolMap.empty()) return;

	if (MessageBox("确定要删除所有的配置？","提示",MB_YESNO|MB_ICONQUESTION)==IDNO)
		return;

	m_ToolMap.clear();
	CToolsProUnity::DeleteAllConfigFromXml();
	m_wndGridTools.RemoveAll();
	m_wndGridTools.AdjustLayout ();
}
 HRESULT CToolGridListDlg::OnMsgGridDoubleClick(WPARAM wp,LPARAM lp)
 {
	 int iRow = wp;
	 CBCGPGridRow *pDbClicRow = m_wndGridTools.GetRow(iRow);
	 if (pDbClicRow==NULL) return S_OK;

	 CBCGPGridItem * pItemId = pDbClicRow->GetItem(0);
	 if (pItemId==NULL) return S_OK;

	 CString strId = pItemId->GetValue(); 
	 if (m_ToolMap.find(strId)== m_ToolMap.end()) return S_OK;

	 ToolObject & tool = m_ToolMap[strId];
	 if (CToolsProUnity::ConfigTool(&tool))
	 {
		 CToolsProUnity::WriteToolConfigXml(tool);
	 }

	 return S_OK;
 }

 BOOL CToolGridListDlg::OnEraseBkgnd(CDC* pDC)
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值

	 CCxBCGPDialog::OnEraseBkgnd(pDC);

	 CRect rc;
	 m_wndGridListPos.GetClientRect (&rc);
	 m_wndGridListPos.MapWindowPoints (this, &rc);

	 CBrush br(RGB(151, 151, 151));
	 pDC->FillRect(&rc, &br);

	 return TRUE;
 }
