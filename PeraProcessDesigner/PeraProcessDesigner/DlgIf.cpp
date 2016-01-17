// DlgStructTreeShow.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgIf.h"
#include "afxdialogex.h"
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "WebDataDefine.h"
#include "PeraProcessDesignerUtility.h"
#include "CxBCGPVisualManager2007.h"
#include "Tools.h"

// CDlgIf 对话框
const int CDlgIf::m_nWidthMax = 1000;
const int CDlgIf::m_nHeightMax = 500;

IMPLEMENT_DYNAMIC(CDlgIf, CCxBCGPDialog)

CDlgIf::CDlgIf(std::string str, CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgIf::IDD, pParent), m_TreeRoot(theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData()), m_strJsonSrc(str)
	, m_radio(0)
{
	m_bIfNotSubFlow = TRUE;
	ZTools::WriteZToolsFormatLog("CDlgIf 传入JSon字符串为：%s",str);
}

CDlgIf::~CDlgIf()
{
}

void CDlgIf::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditShow);
	DDX_Control(pDX, IDC_STATIC_NAME, m_StaticName);
	DDX_Control(pDX, IDC_STATIC_NAME1, m_staticName1);
	DDX_Radio(pDX, IDC_RADIO_EXCLUSIVE, m_radio);
	DDX_Control(pDX, IDC_BUTTON_UP, m_btnUp);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_btnDown);
}


BEGIN_MESSAGE_MAP(CDlgIf, CCxBCGPDialog)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_RADIO_EXCLUSIVE, &CDlgIf::OnBnClickedRadioExclusive)
	ON_BN_CLICKED(IDC_RADIO_INCLUSIVE, &CDlgIf::OnBnClickedRadioInclusive)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CDlgIf::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDlgIf::OnBnClickedButtonDown)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgIf 消息处理程序


BOOL CDlgIf::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	//globalData.SetDPIAware();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon

	// TODO:  在此添加额外的初始化
	{//设置位图按钮
// 		CButton& btnUp = *(CButton*)GetDlgItem(IDC_BUTTON_UP);
// 		CButton& btnDown = *(CButton*)GetDlgItem(IDC_BUTTON_DOWN);
// 		btnUp.SetBitmap(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ARROW_UP)));
// 		btnDown.SetBitmap(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ARROW_DOWN)));
		m_btnUp.SetImage(GetFlowIfResImageData( "FlowIfResId_ResLib_ArrowUp" ));
		m_btnUp.SetImageDisabled(GetFlowIfResImageData( "FlowIfResId_ResLib_ArrowUpGray" ));
		m_btnDown.SetImage(GetFlowIfResImageData( "FlowIfResId_ResLib_ArrowDown" ));
		m_btnDown.SetImageDisabled(GetFlowIfResImageData( "FlowIfResId_ResLib_ArrowDownGray" ));
		
		m_btnUp.SetWindowText(NULL);
		m_btnDown.SetWindowText(NULL);
	}

	SetWindowText(CString(g_lpszAppTitle) + "--分支条件设置");

	Json::Reader Jreader;
	Json::Value Jv;
	if(!Jreader.parse(m_strJsonSrc, Jv))
	{
		EndDialog(0);
		return TRUE;
	}

	Json::Value& jStr = Jv["ShowRootFlag"];
	BOOL b = TRUE;
	std::string str = jStr.asString();
	if(str.empty() || str == "false")
	{
		b = FALSE;
	}

	m_StaticName.SetWindowText("if");

	//BOOL b = m_wndIf.Create(NULL, "If Editor", WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, CRect(450, 100, 1100, 350), this, 65535U);
	b = m_wndIf.Create(IDD_WND_IF, this);
	m_wndIf.ModifyStyleEx(0, WS_CHILD | WS_CLIPCHILDREN /* | WS_CLIPSIBLINGS*/);

	{//位置计算
		CRect rect;
		GetDlgItem(IDC_STATIC_BRANCH)->GetClientRect(rect);
		GetDlgItem(IDC_STATIC_BRANCH)->ClientToScreen(rect);
		ScreenToClient(rect);
		rect.DeflateRect(40, 60, 50, 20);
		//rect.right = rect.left + 650;
		m_wndIf.m_rectCurBranch.right = m_wndIf.m_rectCurBranch.left + rect.Width();

		//m_wndIf.MoveWindow(CRect(450, 100, 1100, 350));
		m_wndIf.MoveWindow(rect);
		m_wndIf.ShowWindow(SW_SHOW);
	}

	Json::Value jExpArray = Jv["Exps"];

	BranchCondition::MODE mode;
	jStr = Jv["IfFlag"];
	if(jStr.isString() && jStr.asString() == "false")
	{//包容模式
		mode = BranchCondition::MODE_INCLUSIVE;
	}
	else
	{
		mode = BranchCondition::MODE_EXCLUSIVE;
	}
	m_radio = mode;
	UpdateData(FALSE);

	if(jExpArray.isArray())
	{
		if(jExpArray.size() > 0)
		{
			for(int i = 0; i < (int)jExpArray.size(); i++)
			{
				jStr = jExpArray[Json::Value::ArrayIndex(i)]["Exp"];
				if(jStr.isString())				
				{
					std::string strExp = jStr.asString().c_str();
					//strExp = "#{n9sx64aa82ba296-9848-4b36-9b29-cbb41ba2e0c1} != #{nw8272b64b08669-ddd1-46cf-aa61-c2ca0945a6b0[3,4]}";
					jStr = jExpArray[Json::Value::ArrayIndex(i)]["NodeName"];
					if(jStr.isString())				
					{		
						std::string strBranch = jStr.asString().c_str();
						BranchCondition::TYPE type = BranchCondition::TYPE_MIDDLE;
						if(i == 0) type =  BranchCondition::TYPE_START;
						else if(i == jExpArray.size() - 1) type = BranchCondition::TYPE_END;

						BOOL bOK = m_wndIf.AddBranch(strExp.c_str(), strBranch.c_str(), type, mode);
						if (!bOK)
						{
							EndDialog(0);
							return FALSE;
						}

						jStr = Jv["ElseFlag"];
						if(type == BranchCondition::TYPE_END)
						{//下拉框模式
							int nCurSelect; //当前选择
							if(jStr.isString() && jStr.asString() == "true")
							{//ELSE
								nCurSelect = 1;
								m_wndIf.m_vBCs[i]->m_editExp.SetReadOnly(TRUE);
								m_wndIf.m_vBCs[i]->m_editExp.SetWindowText(NULL);
								m_wndIf.m_vBCs[i]->m_btnEditor.EnableWindow(FALSE);
							}
							else
							{//IF OR IF ELSE
								nCurSelect = 0;
							}
							CComboBox& listCondition = m_wndIf.m_vBCs[i]->m_listCondition;
							listCondition.SetCurSel(nCurSelect);
							m_wndIf.m_nCurSelect = nCurSelect;
						}
					}
				}			
			}
		}
	}

	else
	{
		EndDialog(0);
		return TRUE;
	}

	CRect rect;
	GetClientRect(rect);
	m_nWidth = rect.Width();
	m_nHeight = rect.Height();

	SetTimer( TIMER_DLGIF_PAINTWINDOW, TIMER_DLGIF_PAINTWINDOW_TIMEOUT, 0);

	SetWindowPos(NULL,0,0,m_nWidthMax,m_nHeightMax,SWP_NOMOVE|SWP_NOZORDER);
	//OnVisualManager();
	EnableVisualManagerStyle (TRUE, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgIf::ParseJson(const char* strJson)
{
	Json::Reader Jreader;
	Json::Value Jv;
	Json::FastWriter Jwriter;
	Json::Value JvRet;

	if(!Jreader.parse(strJson, Jv))
	{
		return;
	}
	Json::Value& jExpArray = Jv["Exps"];


	if(jExpArray.isArray())
	{
		if(jExpArray.size() > 0)
		{
			std::string strErr; //错误信息收集
			for(int i = 0; i < (int)jExpArray.size(); i++)
			{
				Json::Value& jStrErr = jExpArray[Json::Value::ArrayIndex(i)]["Error"];	
				Json::Value& jStrBranch = jExpArray[Json::Value::ArrayIndex(i)]["NodeName"];
				if(jStrErr.isString())
				{
					std::string str = jStrErr.asString();
					if(str.empty())continue;
					strErr = strErr + jStrBranch.asString() + "  :  " + str + '\n';
				}
			}
			if(strErr.empty())
			{//成功则返回
				theApp.m_processMgr.SetModified(TRUE);
				MessageBox("保存成功！", g_lpszAppTitle, MB_ICONINFORMATION);
				EndDialog(0);
			}
			else MessageBox(strErr.c_str(), g_lpszAppTitle, MB_ICONERROR);
		}
	}
}

void CDlgIf::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	Json::Reader Jreader;
	Json::Value Jv;
	Json::FastWriter Jwriter;
	Json::Value JvRet;

	if(!Jreader.parse(m_strJsonSrc, Jv))
	{
		return;
	}
	Json::Value& jExpArray = Jv["Exps"];

	//IF FLAG
	Jv["IfFlag"] = !m_radio ? "true" : "false";

	if(jExpArray.isArray())
	{
		if(jExpArray.size() > 0)
		{
			for(int i = 0; i < (int)jExpArray.size(); i++)
			{
				Json::Value& jStr = jExpArray[Json::Value::ArrayIndex(i)]["Exp"];	
				Json::Value& jStrBranch = jExpArray[Json::Value::ArrayIndex(i)]["NodeName"];
				//表达式
				//m_wndIf.m_vBCs[i]->m_editExp.GetWindowText(str);
				CString str = m_wndIf.m_vBCs[i]->m_BranchExpress.m_strIdExpress;
				jStr = Json::Value(str);
				//分支
				m_wndIf.m_vBCs[i]->m_editBranch.GetWindowText(str);
				jStrBranch = Json::Value(str);
				if(m_wndIf.m_vBCs[i]->m_type == BranchCondition::TYPE_END)
				{//有下拉框
					int nCurSelect = m_wndIf.m_vBCs[i]->m_listCondition.GetCurSel();
					Jv["ElseFlag"] = nCurSelect ? "true" : "false";
				}
			}
		}
	}

	std::string str = Jwriter.write(Jv);

	ProcessUpdate(str.c_str());

	//MessageBox(str.c_str());
	return;

	CCxBCGPDialog::OnOK();
}


void CDlgIf::ProcessUpdate( const char* str )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;

	wmd.m_sMethodName = "ifProcessUpdate";

	pParam = new CWebMethodParamData();
	pParam->m_sName = "json";
	pParam->m_sValue = str;
	wmd.m_vParams.push_back(pParam);
	BOOL b = pSvg->InvokeJsMethod( &wmd );

	if (!b || wmd.m_sResult.IsEmpty()) 
	{
		EndDialog(0);//无返回结果
		return;
	}

	else
	{
		//ParseJson(str);
		ParseJson(wmd.m_sResult);
	}
	
}

void CDlgIf::OnSize(UINT nType, int cx, int cy)
{
	CCxBCGPDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(rect);
	int nOffsetX = rect.Width() - m_nWidth;
	int nOffsetY = rect.Height() - m_nHeight;
	m_nWidth = rect.Width();
	m_nHeight = rect.Height();


	if(::IsWindow(m_wndIf.GetSafeHwnd()))
	{
		{//核心窗口
			MoveOffset(m_wndIf, CRect(0, 0, nOffsetX, nOffsetY));
			m_wndIf.Resize(nOffsetX, nOffsetY);
		}

		{//其他
			MoveOffset(*GetDlgItem(IDC_STATIC_BRANCH), CRect(0, 0, nOffsetX, nOffsetY));
			MoveOffset(*GetDlgItem(IDC_STATIC_MODE), CRect(0, - nOffsetY, nOffsetX, nOffsetY));
			MoveOffset(*GetDlgItem(IDC_STATIC_TEXT1), CRect(0, - nOffsetY, 0, nOffsetY));
			MoveOffset(*GetDlgItem(IDC_STATIC_TEXT2), CRect(0, - nOffsetY, 0, nOffsetY));
			MoveOffset(*GetDlgItem(IDC_BUTTON_UP), CRect(- nOffsetX, - nOffsetY / 2, nOffsetX, nOffsetY / 2));
			MoveOffset(*GetDlgItem(IDC_BUTTON_DOWN), CRect(- nOffsetX, - nOffsetY / 2, nOffsetX, nOffsetY / 2));
			MoveOffset(*GetDlgItem(IDOK), CRect(- nOffsetX, - nOffsetY, nOffsetX, nOffsetY));
			MoveOffset(*GetDlgItem(IDCANCEL), CRect(- nOffsetX, - nOffsetY, nOffsetX, nOffsetY));
			MoveOffset(*GetDlgItem(IDC_RADIO_EXCLUSIVE), CRect(0, - nOffsetY, 0, nOffsetY));
			MoveOffset(*GetDlgItem(IDC_RADIO_INCLUSIVE), CRect(0, - nOffsetY, 0, nOffsetY));		
			MoveOffset(*GetDlgItem(IDC_STATIC_TEXT3), CRect(- nOffsetX, 0, nOffsetX, 0));						
		}
		Invalidate();
	}
}

void CDlgIf::MoveOffset( CWnd& wnd, CRect& rect )
{
	CRect rectT;
	wnd.GetWindowRect(rectT);
	wnd.GetParent()->ScreenToClient(rectT);
	rectT.InflateRect(rect);
	wnd.MoveWindow(rectT);
}


void CDlgIf::OnSizing(UINT fwSide, LPRECT pRect)
{
	CCxBCGPDialog::OnSizing(fwSide, pRect);
	
	// TODO: 在此处添加消息处理程序代码
	CRect   rect(pRect);   
	int   width   =   rect.Width();   
	int   height   =   rect.Height(); 

	switch(fwSide)   
	{   
	case   WMSZ_BOTTOM:   
		{   
			if   (height   <   m_nHeightMax)   
				pRect->bottom   =   pRect->top   +   m_nHeightMax;   
			break;   
		}   
	case   WMSZ_LEFT:   
		{   
			if   (width   <   m_nWidthMax)   
				pRect->left   =   pRect->right   -   m_nWidthMax;   
			break;   
		}   
	case   WMSZ_RIGHT:   
		{   
			if   (width   <   m_nWidthMax)   
				pRect->right   =   pRect->left   +   m_nWidthMax;   
			break;   
		}   
	case   WMSZ_TOP:   
		{   
			if   (height   <   m_nHeightMax)   
				pRect->top   =   pRect->bottom   -   m_nHeightMax;   
			break;   
		}   
	case   WMSZ_TOPLEFT:   
		{   
			if   (width   <   m_nWidthMax)   
				pRect->left   =   pRect->right   -   m_nWidthMax;   
			if   (height   <   m_nHeightMax)   
				pRect->top   =   pRect->bottom   -   m_nHeightMax;   
			break;   
		}   
	case   WMSZ_TOPRIGHT:   
		{   
			if   (width   <   m_nWidthMax)   
				pRect->right   =   pRect->left   +   m_nWidthMax;   
			if   (height   <   m_nHeightMax)   
				pRect->top   =   pRect->bottom   -   m_nHeightMax;   
			break;   
		}   
	case   WMSZ_BOTTOMLEFT:   
		{   
			if   (width   <   m_nWidthMax)   
				pRect->left   =   pRect->right   -   m_nWidthMax;   
			if   (height   <   m_nHeightMax)   
				pRect->bottom   =   pRect->top   +   m_nHeightMax;   
			break;   
		}   
	case   WMSZ_BOTTOMRIGHT:   
		{   
			if   (width   <   m_nWidthMax)   
				pRect->right   =   pRect->left   +   m_nWidthMax;   
			if   (height   <   m_nHeightMax)   
				pRect->bottom   =   pRect->top   +   m_nHeightMax;   
			break;   
		}   
	}   
}


void CDlgIf::OnBnClickedRadioExclusive()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_radio)
	{
	case 0:
		m_wndIf.SetBranchMode(BranchCondition::MODE_EXCLUSIVE);
		break;
	case 1:
		m_wndIf.SetBranchMode(BranchCondition::MODE_INCLUSIVE);
		break;
	}
}


void CDlgIf::OnBnClickedRadioInclusive()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedRadioExclusive();
}


void CDlgIf::OnBnClickedButtonUp()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCur = m_wndIf.m_nBranchSelect;
	if(nCur > 0)
	{
		m_wndIf.m_vBCs[nCur]->Swap(m_wndIf.m_vBCs[nCur - 1]);
		m_wndIf.m_vBCs[nCur - 1]->m_editExp.SetFocus();
	}
}


void CDlgIf::OnBnClickedButtonDown()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCur = m_wndIf.m_nBranchSelect;
	if(nCur < (int)m_wndIf.m_vBCs.size() - 1)
	{
		m_wndIf.m_vBCs[nCur]->Swap(m_wndIf.m_vBCs[nCur + 1]);
		m_wndIf.m_vBCs[nCur + 1]->m_editExp.SetFocus();
	}
}


void CDlgIf::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	KillTimer(nIDEvent);
	Invalidate();
	CCxBCGPDialog::OnTimer(nIDEvent);
}


void CDlgIf::OnVisualManager() 
{
	CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
	return;

	CWaitCursor wait;

#ifdef _BCGSUITE_INC_
	int nVisualManagerOld = m_nVisualManager;
#endif

// 	UpdateData ();
// 
// 	BOOL bIsDarkBackground = m_bIsDarkBackground;
// 	m_bIsDarkBackground = FALSE;

#ifndef _BCGSUITE_INC_
	switch (m_nVisualManager)
	{
	case 0:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		break;

	case 1:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case 2:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;

	case 3:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;

	case 4:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;

	case 5:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;

	case 6:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerCarbon));
		//m_bIsDarkBackground = TRUE;
		break;

	case 7:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2010));
		break;

	case 8:
		CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Blue);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2010));
		break;

	case 9:
		CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Silver);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2010));
		break;

	case 10:
		CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Black);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2010));
		break;
	}
#else
	switch (m_nVisualManager)
	{
	case 0:
		BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_OFFICE_2003);
		break;

	case 1:
		BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_WINXP);
		break;

	case 2:
		BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_OFFICE_2007_BLUE);
		break;

	case 3:
		BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_OFFICE_2007_BLACK);
		break;

	case 4:
		BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_OFFICE_2007_SILVER);
		break;

	case 5:
		BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_OFFICE_2007_AQUA);
		break;

	default:
		if (m_nVisualManager != nVisualManagerOld)
		{
			MessageBox(_T("This feature is available in BCGContgrolBar Pro only"));
			m_nVisualManager = nVisualManagerOld;
		}
		else
		{
			BCGPSetVisualManagerSuite (CBCGPVisualManager::BCGPVM_OFFICE_2007_BLUE);
			m_nVisualManager = 2;
		}
		UpdateData(FALSE);
		break;
	}
#endif

// 	theApp.WriteInt (_T("VisualManager"), m_nVisualManager);
// 	PostMessage (BCGM_CHANGEVISUALMANAGER);
// 
// 	if (m_bIsDarkBackground != bIsDarkBackground && m_bImage)
// 	{
// 		OnBackImage();
// 	}
// 
// 	if (m_bIsVistaGlass)
// 	{
// 		OnVistaGlass();
// 	}
// 
// 	RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}