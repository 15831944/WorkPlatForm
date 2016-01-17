// DlgForEach.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForEach.h"
#include "afxdialogex.h"
#include "ZTreeParamSet.h"
#include "DlgFor.h"


// CDlgForEach 对话框

IMPLEMENT_DYNAMIC(CDlgForEach, CDlgForBase)

CDlgForEach::CDlgForEach(CWnd* pParent /*=NULL*/)
	: CDlgForBase(pParent)
{

}

CDlgForEach::~CDlgForEach()
{
	for(int i = 0; i < (int)m_vpComboBox.size(); i++)
	{
		if(m_vpComboBox[i])
		{
			delete m_vpComboBox[i];
			m_vpComboBox[i] = NULL;
		}
	}
	m_vpComboBox.clear();
}

void CDlgForEach::DoDataExchange(CDataExchange* pDX)
{
	CDlgForBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgForEach, CDlgForBase)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgForEach::OnBnClickedButtonAdd)
	ON_CBN_SETFOCUS(IDC_COMBO_ID, &CDlgForEach::OnCbnFocusCombo)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgForEach::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CDlgForEach::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDlgForEach::OnBnClickedButtonDown)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgForEach 消息处理程序

void CDlgForEach::InitGrid()
{
	CWnd* wndGrid = GetDlgItem(IDC_STATIC_GRID);
	CRect rc;
	wndGrid->GetClientRect (&rc);
	wndGrid->MapWindowPoints (this, &rc);
	wndGrid->ShowWindow(SW_HIDE);
	rc.DeflateRect(1, 1, 1, 1);

	m_forEachGrid.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, this, (UINT)-1);
	m_forEachGrid.EnableColumnAutoSize (TRUE);
	m_forEachGrid.SetSingleSel (TRUE);
	m_forEachGrid.EnableGroupByBox (FALSE);
	m_forEachGrid.SetReadOnly (TRUE);
	m_forEachGrid.SetWholeRowSel (TRUE);
	m_forEachGrid.EnableHeader (FALSE);

	//m_forEachGrid.SetWindowPos (&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//m_forEachGrid.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);

	//设置颜色
	CBCGPGridColors theme;
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme (&m_forEachGrid, theme);
	theme.m_clrBackground = RGB(251, 251, 251);
	theme.m_HeaderSelColors.InitColors();

	m_forEachGrid.SetColorTheme (theme);

	//初始化列
	int nColCount = 1;
	for (int i=0; i<nColCount; i++)
	{
		m_forEachGrid.SetColumnAlign(i, HDF_CENTER);
	}
	m_forEachGrid.AdjustLayout ();

// 	const int nColumns = m_forEachGrid.GetColumnCount ();
// 
// 	m_relatedGrid.RemoveAll();
// 
// 	for(int i = 0; i < nCount; i++)
// 	{
// 		ZTreeParam* pParam = (ZTreeParam*)m_wndGridTree.GetRow(i)->GetData();
// 		if(pParam && pParam->m_pRelatedParam)
// 		{//存在关联
// 			CBCGPGridRow * pRow = NULL;
// 			pRow = m_relatedGrid.CreateRow (nColumns);
// 
// 			pRow->GetItem (COL_NAME1)->SetValue ( (LPCTSTR)pParam->m_strName.c_str() );
// 			CString str = pParam->m_strType.c_str();
// 			str.Replace("Int", "Integer");
// 			pRow->GetItem (COL_TYPE)->SetValue ( str.GetBuffer() );
// 			//pRow->GetItem (COL_TYPE)->SetValue ( (LPCTSTR)pParam->m_strType.c_str() );
// 			pRow->GetItem (COL_NAME2)->SetValue ( (LPCTSTR)pParam->m_pRelatedParam->m_strName.c_str() );
// 			pRow->GetItem (COL_PATH)->SetValue ( (LPCTSTR)pParam->m_pRelatedParam->m_strPath.c_str() );
// 			pRow->SetData((DWORD_PTR)pParam);
// 			//pRow->ReplaceItem(COL_NAME1, new CBCGPGridCheckItem(FALSE));
// 			//m_relatedGrid.getHeader
// 			m_relatedGrid.AddRow (pRow, FALSE);
// 		}
// 	}
// 
// 	for(int i = 0; i < 100; i++)
// 	{
// 		CBCGPGridRow * pRow = NULL;
// 		pRow = m_forEachGrid.CreateRow (nColumns);
// 		CBCGPGridItem* pItem = new CBCGPGrid;
// 		pRow->ReplaceItem(
// 		m_forEachGrid.AddRow()
// 	}
}

BOOL CDlgForEach::OnInitDialog()
{
	CDlgForBase::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//InitGrid();
	m_nIndexSel = -1;

	m_dlgForEachListFrame.Create(IDD_FOR_EACH_LIST_FRAME, this);
	CWnd* wndGrid = GetDlgItem(IDC_STATIC_GRID);
	CRect rc;
	wndGrid->GetClientRect (&rc);
	wndGrid->MapWindowPoints (this, &rc);
	//wndGrid->ShowWindow(SW_HIDE);
	m_dlgForEachListFrame.MoveWindow(rc);
	m_dlgForEachListFrame.ShowWindow(SW_SHOW);

	m_dlgForEachList.Create(IDD_FOR_EACH_LIST, &m_dlgForEachListFrame);
	m_dlgForEachList.ShowWindow(SW_SHOW);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgForEach::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = m_vpComboBox.size();
	CBCGPComboBox* pBox = new CBCGPComboBox();
	CRect rect(0, i * COMBO_HEIGHT, COMBO_WIDTH, i * COMBO_HEIGHT + COMBO_HEIGHT);
	pBox->Create(WS_VISIBLE | CBS_DROPDOWNLIST, rect, &m_dlgForEachList, IDC_COMBO_ID);
// 	pBox->AddString("fsd");
// 	pBox->AddString("fsssd");

	ZTreeParamSet& wndGrid = ((CDlgFor*)GetParent())->m_wndGridTree;
	for(int i = 0; i < (int)wndGrid.m_vParams.size(); i++)
	{
		ZTreeParam* pParam = wndGrid.m_vParams[i];
		if((pParam->m_strType == "Int" || pParam->m_strType == "Double") && (pParam->m_strDirection == "In" || pParam->m_strDirection == "InOut"))
		{
			AddStringToComboBox(pBox, pParam->m_strName.c_str());
		}
	}

	pBox->SetFont(GetFont());
	m_vpComboBox.push_back(pBox);
	m_dlgForEachList.EnableVisualManagerStyle(TRUE);
	CheckButtonCanEnable();

	CRect rectWnd(0, 0, COMBO_WIDTH, (i + 1) * COMBO_HEIGHT);
	m_dlgForEachList.MoveWindow(rectWnd);

	m_dlgForEachListFrame.RefreshScrollBar();
	
}

void CDlgForEach::OnCbnFocusCombo()
{
	CWnd* pWnd = GetFocus();
	CBCGPComboBox* pBox = dynamic_cast<CBCGPComboBox*>(pWnd);
	if(pBox == NULL) return;

	int i;
	for(i = 0; i < (int)m_vpComboBox.size(); i++)
	{
		if(m_vpComboBox[i] == pBox)
		{
			break;
		}
	}

	m_nIndexSel = i;
	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(TRUE);

	CheckButtonCanEnable();
}


void CDlgForEach::OnBnClickedButtonDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	m_vpComboBox[m_nIndexSel]->DestroyWindow();
	delete m_vpComboBox[m_nIndexSel];

	std::vector<CBCGPComboBox*>::iterator iter = m_vpComboBox.begin() + m_nIndexSel; 

	m_vpComboBox.erase(iter);

	//下面的移上来
	for(int i = m_nIndexSel; i < (int)m_vpComboBox.size(); i++)
	{
		CRect rect(0, i * COMBO_HEIGHT, COMBO_WIDTH, i * COMBO_HEIGHT + COMBO_HEIGHT);
		m_vpComboBox[i]->MoveWindow(rect);
	}

	GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);

	m_dlgForEachListFrame.RefreshScrollBar();

	CheckButtonCanEnable();

}

void CDlgForEach::CheckButtonCanEnable()
{
	if(m_nIndexSel < 0) return;

	if(m_nIndexSel > 0)
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);
	}

	if(m_nIndexSel < (int)m_vpComboBox.size() - 1)
	{
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);
	}
}

void CDlgForEach::SwapBox(int nID1, int nID2)
{
	CRect rect1;
	CRect rect2;

	CBCGPComboBox* pBox1 = m_vpComboBox[nID1];
	CBCGPComboBox* pBox2 = m_vpComboBox[nID2];

	pBox1->GetWindowRect(rect1);
	pBox2->GetWindowRect(rect2);

	m_dlgForEachList.ScreenToClient(rect1);
	m_dlgForEachList.ScreenToClient(rect2);

	pBox1->MoveWindow(rect2);
	pBox2->MoveWindow(rect1);

	m_vpComboBox[nID1] = pBox2;
	m_vpComboBox[nID2] = pBox1;
}

void CDlgForEach::OnBnClickedButtonUp()
{
	// TODO: 在此添加控件通知处理程序代码
	SwapBox(m_nIndexSel, m_nIndexSel - 1);
	m_nIndexSel--;

	CheckButtonCanEnable();
}


void CDlgForEach::OnBnClickedButtonDown()
{
	// TODO: 在此添加控件通知处理程序代码
	SwapBox(m_nIndexSel, m_nIndexSel + 1);
	m_nIndexSel++;

	CheckButtonCanEnable();
}



void CDlgForEach::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_dlgForEachListFrame.ShowScrollBar(SB_VERT, TRUE);
	m_dlgForEachListFrame.ShowScrollBar(SB_VERT, FALSE);
	KillTimer(nIDEvent);
	CDlgForBase::OnTimer(nIDEvent);
}


void CDlgForEach::AddParam( LPCTSTR str )
{
	ZTreeParam* pParam = GetParamByName(str);
	if(pParam)
	{
		if((pParam->m_strType == "Int" || pParam->m_strType == "Double") && (pParam->m_strDirection == "In" || pParam->m_strDirection == "InOut"))
		{
			AddStringToComboBox(IDC_COMBO_PARAM, str);
			for(int i = 0; i < (int)m_vpComboBox.size(); i++)
			{
				AddStringToComboBox(m_vpComboBox[i], str);
			}
		}
	}
}

void CDlgForEach::ParamChanged( ZTreeParam* pParam )
{
	CString str = pParam->m_strName.c_str();
	if((pParam->m_strType == "Int" || pParam->m_strType == "Double") && (pParam->m_strDirection == "In" || pParam->m_strDirection == "InOut"))
	{
		AddStringToComboBox(IDC_COMBO_PARAM, str);
		for(int i = 0; i < (int)m_vpComboBox.size(); i++)
		{
			AddStringToComboBox(m_vpComboBox[i], str);
		}
	}
	else
	{
		DeleteStringFromComboBox(IDC_COMBO_PARAM, str);
		for(int i = 0; i < (int)m_vpComboBox.size(); i++)
		{
			DeleteStringFromComboBox(m_vpComboBox[i], str);
		}
	}
}


void CDlgForEach::DeleteParam( LPCTSTR str )
{
	DeleteStringFromComboBox(IDC_COMBO_PARAM, str);
	for(int i = 0; i < (int)m_vpComboBox.size(); i++)
	{
		DeleteStringFromComboBox(m_vpComboBox[i], str);
	}
}

void CDlgForEach::RenameParam( LPCTSTR strOld, LPCTSTR strNew )
{
	BOOL bSelectParam = FALSE;

	CComboBox* pBox;
	CString str;
	int n;

	pBox = (CComboBox*)GetDlgItem(IDC_COMBO_PARAM);

	n = pBox->GetCurSel();
	if(n != -1)
	{
		pBox->GetLBText(n, str);
		if(str == strOld) bSelectParam = TRUE;
	}

	DeleteStringFromComboBox(pBox, strOld);

	AddStringToComboBox(pBox, strNew);

	if(bSelectParam) SelectStringFromComboBox(pBox, strNew);

	for(int i = 0; i < (int)m_vpComboBox.size(); i++)
	{	
		BOOL bSelect = FALSE;
		pBox = m_vpComboBox[i];

		n = pBox->GetCurSel();
		if(n != -1)
		{
			pBox->GetLBText(n, str);
			if(str == strOld) bSelect = TRUE;
		}

		DeleteStringFromComboBox(pBox, strOld);

		AddStringToComboBox(pBox, strNew);

		if(bSelect) SelectStringFromComboBox(pBox, strNew);
	}

}