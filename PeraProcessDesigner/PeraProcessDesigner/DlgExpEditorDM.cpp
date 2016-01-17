#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "value.h"
#include "reader.h"
#include "writer.h"
#include "DlgExpEditorDM.h"
#include "PeraProcessDesignerUtility.h"


CDlgExpEditorDM::CDlgExpEditorDM( const char* strExp, CWnd* pParent /*= NULL*/ )
	: CDlgExpEditor(0, strExp, pParent)
{
}


CDlgExpEditorDM::~CDlgExpEditorDM(void)
{
}

void CDlgExpEditorDM::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_editExp.GetWindowText(m_strExp);
	m_strExp.Replace("\r\n", "");

	Json::Reader Jreader;
	Json::Value Jv;
	Json::FastWriter Jwriter;
	Json::Value JvRet;

	m_Jv["NewExp"] = m_strExp.GetBuffer();

	std::string str = Jwriter.write(m_Jv);

	ProcessUpdate(str.c_str());

	//MessageBox(str.c_str());
	return;

	CBCGPDialog::OnOK();
}


void CDlgExpEditorDM::ProcessUpdate( const char* str )
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL ) return;

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;

	wmd.m_sMethodName = "dataMappingExpUpdate";

	pParam = new CWebMethodParamData();
	pParam->m_sName = "json";
	pParam->m_sValue = str;
	wmd.m_vParams.push_back(pParam);
	BOOL b = pSvg->InvokeJsMethod( &wmd );

	if (!b || wmd.m_sResult.IsEmpty()) 
	{
		MessageBox("调用页面方法dataMappingExpUpdate失败！", g_lpszAppTitle, MB_ICONERROR);
		EndDialog(0);//无返回结果
		return;
	}

	else
	{
		//ParseJson(str);
		ParseJson(wmd.m_sResult);
	}

}

void CDlgExpEditorDM::ParseJson(const char* strJson)
{
	Json::Reader Jreader;
	Json::Value Jv;
	Json::FastWriter Jwriter;
	Json::Value JvRet;

	if(!Jreader.parse(strJson, m_Jv))
	{//解析失败
		return;
	}
	std::string strErr = m_Jv["Error"].asString();

	if(strErr.empty())
	{//成功则返回
		EndDialog(0);
	}
	else MessageBox(strErr.c_str(), g_lpszAppTitle, MB_ICONERROR);

}

//移动子窗口控件向上靠近目标控件，距离为d,返回值为移动距离
int CDlgExpEditorDM::MoveItemUp(CWnd* wndItem, CWnd* wndDst, int d)
{
	CRect rectItem, rectDst;
	wndDst->GetWindowRect(rectDst);
	wndItem->GetWindowRect(rectItem);
	int y = rectItem.top;
	rectItem.MoveToY(rectDst.bottom + d);
	int nR = y - rectItem.top;
	ScreenToClient(rectItem);
	wndItem->MoveWindow(rectItem);
	return nR;
}


//移动子窗口控件
void CDlgExpEditorDM::MoveItem( CWnd* wndItem, EDirection e, int d )
{
	CRect rect;
	wndItem->GetWindowRect(rect);

	int x = rect.left;
	int y = rect.top;

	switch(e)
	{
	case UP:
		{
			y -= d;
			break;
		}
	case DOWN:
		{
			y += d;
			break;
		}
	case LEFT:
		{
			x -= d;
			break;
		}
	case RIGHT:
		{
			y +=d;
			break;
		}
	}

	rect.MoveToXY(x, y);
	ScreenToClient(rect);
	wndItem->MoveWindow(rect);
}

BOOL CDlgExpEditorDM::OnInitDialog()
{
	// TODO:  在此添加额外的初始化

	Json::Reader jR;
	if(!jR.parse(std::string(m_strExp), m_Jv))
	{//解析失败
		EndDialog(0);
		return TRUE;
	}

	Json::Value& jStr = m_Jv["ShowRootFlag"];
	m_IsShowRoot = TRUE;
	std::string str = jStr.asString();
	if(str.empty() || str == "false")
	{
		m_IsShowRoot = FALSE;
	}

	//20140526,lc,修改为左侧树不初始化，影响加载效率，因为后期设计中已无左侧树
	m_IsInitTree = FALSE;

	m_strExp = m_Jv["OldExp"].asString().c_str();

	CDlgExpEditor::OnInitDialog();

	GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON6)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_BUTTON13)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON14)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON15)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON16)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON17)->ShowWindow(SW_HIDE);

	int d = 20;
	MoveItemUp(GetDlgItem(IDC_BUTTON7), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON8), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON9), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON10), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON11), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON12), GetDlgItem(IDC_EDIT_EXP), d);

	MoveItemUp(GetDlgItem(IDOK), GetDlgItem(IDC_BUTTON7), d);
	int nD= MoveItemUp(GetDlgItem(IDCANCEL), GetDlgItem(IDC_BUTTON7), d);

	CRect rect;
	GetDlgItem(IDC_STATIC_TREE)->GetWindowRect(rect);
	rect.DeflateRect(0, 0, 0, nD);
	ScreenToClient(rect);
	GetDlgItem(IDC_STATIC_TREE)->MoveWindow(rect);

	GetWindowRect(rect);
	rect.DeflateRect(0, 0, 0, nD);
	MoveWindow(rect);


	if(true)
	{//隐藏树
		CWnd& m_wndTreeLocation = *GetDlgItem(IDC_STATIC_TREE);
		m_wndGridTree.ShowWindow(SW_HIDE);
		m_wndTreeLocation.ShowWindow(SW_HIDE);
		CRect rect;
		m_wndTreeLocation.GetWindowRect(rect);
		int d = rect.Width() + 10;

		MoveItem(GetDlgItem(IDC_BUTTON7), LEFT, d);
		MoveItem(GetDlgItem(IDC_BUTTON8), LEFT, d);
		MoveItem(GetDlgItem(IDC_BUTTON9), LEFT, d);
		MoveItem(GetDlgItem(IDC_BUTTON10), LEFT, d);
		MoveItem(GetDlgItem(IDC_BUTTON11), LEFT, d);
		MoveItem(GetDlgItem(IDC_BUTTON12), LEFT, d);

		MoveItem(GetDlgItem(IDOK), LEFT, d);
		MoveItem(GetDlgItem(IDCANCEL), LEFT, d);

		MoveItem(GetDlgItem(IDC_EDIT_EXP), LEFT, d);

		GetWindowRect(rect);
		rect.DeflateRect(0, 0, d, 0);
		MoveWindow(rect);
	}

	return FALSE;
}
