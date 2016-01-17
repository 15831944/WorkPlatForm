// DlgStart.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgStart.h"
#include "CxBCGPGridItem.h"
#include "ViewSet.h"
#include "DlgSvg.h"

static const LPCTSTR arrColName[] = {
	"����", "λ��", "����"
};
static const int arrColWidth[] = {
/*	130, 370, 70*/
	130, 370 + 70, 70
};

// CDlgStart �Ի���

BEGIN_MESSAGE_MAP(CDlgStart, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgStart::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CDlgStart::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_JML_NEW, &CDlgStart::OnBnClickedJmlNew)
	ON_BN_CLICKED(IDC_JML_OPEN, &CDlgStart::OnBnClickedJmlOpen)
	ON_COMMAND_RANGE(ID_GRID_BUTTON_FIRST, ID_GRID_BUTTON_LAST, OnGridButtonClicked)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CDlgStart::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JML_NEW, m_wndNew);
	DDX_Control(pDX, IDC_JML_OPEN, m_wndOpen);
	DDX_Control(pDX, IDC_JML_HISTORY, m_wndHistoryList);
}

IMPLEMENT_DYNAMIC(CDlgStart, CCxBCGPDialog)

CDlgStart::CDlgStart(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgStart::IDD, pParent)
{

}

CDlgStart::~CDlgStart()
{
}


void CDlgStart::OnBnClickedOk()
{
}


void CDlgStart::OnBnClickedCancel()
{
}


BOOL CDlgStart::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	//ˢ����ʷ��¼
	theApp.m_processMgr.RefreshHistory();
	InitHistoryList();

	EnableVisualManagerStyle (TRUE, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CDlgStart::InitHistoryList( void )
{
	if ( IsWindow( m_wndGridHistory.GetSafeHwnd() ) ) return FALSE;

	CRect rc;
	m_wndHistoryList.GetClientRect (&rc);
	m_wndHistoryList.MapWindowPoints (this, &rc);
	m_wndHistoryList.ShowWindow(SW_HIDE);
	rc.InflateRect(-1, -1, -1, -1);

	m_wndGridHistory.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, this, (UINT)-1);
	m_wndGridHistory.m_hwndRecvMsg = this->GetSafeHwnd();
	m_wndGridHistory.EnableColumnAutoSize (TRUE);
	m_wndGridHistory.SetSingleSel (TRUE);
	m_wndGridHistory.EnableGroupByBox (FALSE);
	m_wndGridHistory.SetReadOnly (TRUE);
	m_wndGridHistory.SetWholeRowSel (TRUE);
	m_wndGridHistory.EnableHeader (TRUE, 0);
		
	m_wndGridHistory.SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	m_wndGridHistory.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);

	//������ɫ
	CBCGPGridColors theme;
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme (&m_wndGridHistory, theme);
	theme.m_clrBackground = RGB(249, 249, 249);
	theme.m_HeaderSelColors.InitColors();

	m_wndGridHistory.SetColorTheme (theme);

	//��ʼ����
	int nColCount = sizeof( arrColName ) / sizeof( LPCTSTR );
	for (int i=0; i<nColCount; i++)
	{
		m_wndGridHistory.InsertColumn ( i, arrColName[i], arrColWidth[i] );
		m_wndGridHistory.SetHeaderAlign(i, HDF_CENTER);
	}
	//ȡ��������
	LoadHistoryList();
	m_wndGridHistory.SetColumnVisible(COL_OP, FALSE); //���ز�����
	//����
	m_wndGridHistory.AdjustLayout ();
	return TRUE;
}

BOOL CDlgStart::LoadHistoryList( void )
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

		pBtn = new CCxGridButtonItem (_T("��"), ID_GRID_BUTTON_FIRST);
		pRow->ReplaceItem (COL_OP, pBtn);

		m_wndGridHistory.AddRow (pRow, FALSE);
	}
	return TRUE;
}


void CDlgStart::OnBnClickedJmlNew()
{
	theApp.m_pMainWnd->PostMessage(WM_COMMAND, IDM_TASK_NEW);
	EndDialog(TRUE); return;
}


void CDlgStart::OnBnClickedJmlOpen()
{
	theApp.m_pMainWnd->PostMessage(WM_COMMAND, IDM_TASK_OPEN);
	EndDialog(TRUE);  return;
}


void CDlgStart::OnGridButtonClicked(UINT uID)
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
// 		VIEW_DATA * pView = theViewMgr.Find( "��ģ����" );
// 		if ( pView == NULL )
// 		{
// 			pView = theViewMgr.NewView("��ģ����");
// 			if ( pView == NULL ) return;
// 		}
// 		CDlgSvg * pSvg = reinterpret_cast< CDlgSvg * >( pView->m_dwExData );
// 		if ( pSvg == NULL ) return;
// 		pSvg->Navigate( theGlobalConfig.m_strSvgUrl );
	}

	EndDialog(TRUE); return;
}

BOOL CDlgStart::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CCxBCGPDialog::OnEraseBkgnd(pDC);

	CRect rc;
	m_wndHistoryList.GetClientRect (&rc);
	m_wndHistoryList.MapWindowPoints (this, &rc);
	
	CBrush br(RGB(151, 151, 151));
	pDC->FillRect(&rc, &br);

	return TRUE;
}
