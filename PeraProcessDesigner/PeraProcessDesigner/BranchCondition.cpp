#include "StdAfx.h"
#include "BranchCondition.h"
#include "PeraProcessDesigner.h"
#include "MethodForDataMapIfExpress.h"
#define  USE_BMPBTN //使用位图按钮

const ZRectF BranchCondition::m_conRectCondition(0.0f, 0.1f, 0.18f, 0.8f);
const ZRectF BranchCondition::m_conRectConditionList(0.0f, 0.0f, 0.18f, 0.8f);
//const ZRectF BranchCondition::m_conRectExp(0.2f, 0.0f, 0.65f, 0.8f);
const ZRectF BranchCondition::m_conRectExp(0.2f, 0.0f, 0.73f, 0.8f);
const ZRectF BranchCondition::m_conRectBtnEditor(0.67f, 0.0f, 0.73f, 0.8f);
const ZRectF BranchCondition::m_conRectBranch(0.75f, 0.0f, 0.9f, 0.8f);
const ZRectF BranchCondition::m_conRectArrow(0.9f, 0.0f, 1.0f, 0.8f);

const char* const BranchCondition::m_str[BranchCondition::MODE_INCLUSIVE + 1][BranchCondition::TYPE_END + 1] = {
	"if",
	"else if",
	"else if",

	"if",
	"if",
	"if"
};
const char* const BranchCondition::m_strCBSecond = "else";

// const char* const BranchCondition::m_str[BranchCondition::MODE_INCLUSIVE + 1][BranchCondition::TYPE_END + 1] = {
// 	"如果",
// 	"否则如果",
// 	"否则如果",
// 
// 	"如果",
// 	"如果",
// 	"如果"
// };
// 
// const char* const BranchCondition::m_strCBSecond = "否则";



BranchCondition::BranchCondition(void)
{
}

BranchCondition::~BranchCondition(void)
{
}

CRect CalculateRect(const ZRectF& zrectf, CRect& rect)
{
	return CRect(rect.left + int(zrectf.left * rect.Width()), rect.top + int(zrectf.top * rect.Height()), rect.left + int(zrectf.right * rect.Width()), rect.top + int(zrectf.bottom * rect.Height()));
}
BOOL BranchCondition::Create( CRect rect, CWnd* wndParent, const char* strIdExp, const char* strbranch, TYPE type, MODE mode)
{
	m_rect = rect;
	m_type = type;
	m_nSizeIncX = 0;
	m_nSIzeIncY = 0;
	//条件
	switch (type)
	{
	case TYPE_START:     //if
	case TYPE_MIDDLE:  //else if
		{
			m_staCondition.Create(m_str[mode][type], WS_VISIBLE | SS_RIGHT/* | WS_BORDER*/, CalculateRect(m_conRectCondition, rect), wndParent);
			break;
		}
	case TYPE_END:        //else
		{
			m_listCondition.Create(WS_VISIBLE | CBS_DROPDOWNLIST, CalculateRect(m_conRectConditionList, rect), wndParent, 65535U);
			m_listCondition.AddString(m_str[mode][type]);
			m_listCondition.AddString(m_strCBSecond);
			m_listCondition.SelectString(0, m_strCBSecond);
			break;
		}
	}
	//表达式
	m_editExp.Create(WS_VISIBLE /*| WS_BORDER*/ | ES_AUTOHSCROLL | ES_MULTILINE, CalculateRect(m_conRectExp, rect), wndParent, 65535U); 
	m_editExp.ModifyStyleEx(0, WS_EX_STATICEDGE );
	m_editExp.SetWindowPos(0,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE | SWP_DRAWFRAME);
	m_BranchExpress.Clear();
	if (strIdExp !=NULL && strcmp("",strIdExp)!=0)
	{
		bool bOK = ParseIfParam(strIdExp,m_BranchExpress.m_LeftParam,m_BranchExpress.m_RightParam,
			m_BranchExpress.m_strExpressSymbol);
		if (!bOK)
		{
			CString strErr;
			strErr.Format("解析条件表达式【%s】参数失败!",strIdExp);
			::MessageBox(GetMainWnd(),strErr,g_lpszAppTitle,MB_OK|MB_ICONWARNING);
			return FALSE;
		}
	}
	// 在此需解析成全路径的表达式
	CString strPathLeft,strPathRight;
	if (m_BranchExpress.m_LeftParam.strID.IsEmpty())
		strPathLeft = m_BranchExpress.m_LeftParam.strConst;
	else
	{
		strPathLeft = GetDataFullPathByID(m_BranchExpress.m_LeftParam.strID,NULL) + m_BranchExpress.m_LeftParam.strArry;;
		m_BranchExpress.m_LeftParam.strParamFullPath = strPathLeft;
	}
	
	if (m_BranchExpress.m_RightParam.strID.IsEmpty())
		strPathRight = m_BranchExpress.m_RightParam.strConst;
	else
	{
		strPathRight = GetDataFullPathByID(m_BranchExpress.m_RightParam.strID,NULL) + m_BranchExpress.m_RightParam.strArry;
		m_BranchExpress.m_RightParam.strParamFullPath = strPathRight;
	}

	m_BranchExpress.m_strIdExpress = strIdExp;
	m_BranchExpress.m_strFullPathExpress = strPathLeft + m_BranchExpress.m_strExpressSymbol + strPathRight;
	m_editExp.SetWindowText(m_BranchExpress.m_strFullPathExpress);
	m_editExp.SetReadOnly(TRUE);
	if (TYPE_END == type) // 初始化控件时，记住原始的表达式
	{
		CZComboBox::g_BranchExpress = m_BranchExpress;
		CZComboBox::g_strEditText = m_BranchExpress.m_strFullPathExpress;
	}

	//编辑器
	m_btnEditor.Create("...", WS_VISIBLE/* | WS_BORDER*/, CalculateRect(m_conRectBtnEditor, rect), wndParent, 65535U);
	m_btnEditor.ModifyStyleEx(0, WS_EX_STATICEDGE );
	m_btnEditor.SetWindowPos(0,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE | SWP_DRAWFRAME);
	m_btnEditor.ShowWindow(SW_SHOW);
	//分支
	m_editBranch.Create(WS_VISIBLE /*| WS_BORDER */| ES_AUTOHSCROLL | ES_MULTILINE, CalculateRect(m_conRectBranch, rect), wndParent, 65535U);
	m_editBranch.SetWindowText(strbranch);
	m_editBranch.SetReadOnly(TRUE);
	m_editBranch.ModifyStyleEx(0, WS_EX_STATICEDGE );
	m_editBranch.SetWindowPos(0,0,0,0,0,SWP_NOMOVE| SWP_NOSIZE | SWP_DRAWFRAME);
	//箭头
	m_staArrow.Create("<<<<<<<<<<<<<<<<<--------------------------", WS_VISIBLE | SS_CENTERIMAGE/* | SS_BITMAP*/ | SS_OWNERDRAW/* | WS_BORDER*/, CalculateRect(m_conRectArrow, rect), wndParent);
	//HBITMAP bitmap;
	//bitmap=(HBITMAP)::LoadBitmap(AfxGetInstanceHandle(), (LPSTR)IDB_ARROW);
	//m_staArrow.SetBitmap(bitmap);
	m_staArrow.ShowWindow(SW_HIDE);
	
	{//设置字体
		if(m_type == TYPE_END)
		{//列表
			m_listCondition.SetFont(m_editExp.GetParent()->GetFont());
		}
		else
		{
			m_staCondition.SetFont(m_editExp.GetParent()->GetFont());
		}
		m_editExp.SetFont(m_editExp.GetParent()->GetFont());
		m_btnEditor.SetFont(m_editExp.GetParent()->GetFont());
		m_editBranch.SetFont(m_editExp.GetParent()->GetFont());
		m_staArrow.SetFont(m_editExp.GetParent()->GetFont());
	}
	return TRUE;
}

void BranchCondition::Resize( int nOffsetX, int nOffsetY )
{
	CRect rect;
	m_editExp.GetWindowRect(rect);
	m_editExp.GetParent()->ScreenToClient(rect);
	rect.right += nOffsetX;
	m_editExp.MoveWindow(rect);

	Move(m_btnEditor, nOffsetX, 0);
	Move(m_editBranch, nOffsetX, 0);
	Move(m_staArrow, nOffsetX, 0);

	//m_rect.InflateRect(0, 0, nOffsetX, nOffsetY);
	m_nSizeIncX += nOffsetX;
	m_nSIzeIncY += nOffsetY;
	
}

void BranchCondition::SetMode( MODE mode )
{
	if(m_type == TYPE_END)
	{//列表
		int i =m_listCondition.GetCurSel();
		m_listCondition.DeleteString(0);
		m_listCondition.CComboBox::InsertString(0, m_str[mode][m_type]);
		CString str;
		m_listCondition.SetCurSel(i);
	}
	else
	{
		m_staCondition.SetWindowText(m_str[mode][m_type]);
	}
}

void BranchCondition::Move( CWnd& wnd, int nOffsetX, int nOffsetY )
{
	CRect rect;
	wnd.GetWindowRect(rect);
	wnd.GetParent()->ScreenToClient(rect);
	rect.OffsetRect(nOffsetX, nOffsetY);
	wnd.MoveWindow(rect);
}

void BranchCondition::SetPos( CWnd& wnd, int nX, int nY )
{
	CRect rect;
	wnd.GetWindowRect(rect);
	wnd.GetParent()->ScreenToClient(rect);
	rect.MoveToXY(nX, nY);
	wnd.MoveWindow(rect);
}

void BranchCondition::SetPos(int nX, int nY )
{
	m_rect.MoveToXY(nX, nY);
	CRect rect;

	if(m_type == TYPE_END)
	{//列表
		rect = CalculateRect(m_conRectConditionList, m_rect);
		SetPos(m_listCondition, rect.left, rect.top);
	}
	else
	{
		rect = CalculateRect(m_conRectCondition, m_rect);
		SetPos(m_staCondition, rect.left, rect.top);
	}
	rect = CalculateRect(m_conRectExp, m_rect);
	SetPos(m_editExp, rect.left, rect.top);
	rect = CalculateRect(m_conRectBtnEditor, m_rect);
	SetPos(m_btnEditor, rect.left + m_nSizeIncX, rect.top);
	rect = CalculateRect(m_conRectBranch, m_rect);
	SetPos(m_editBranch, rect.left + m_nSizeIncX, rect.top);
	rect = CalculateRect(m_conRectArrow, m_rect);
	SetPos(m_staArrow, rect.left + m_nSizeIncX, rect.top);
}

void BranchCondition::SetPosY( CWnd& wnd, int nY )
{
	CRect rect;
	wnd.GetWindowRect(rect);
	wnd.GetParent()->ScreenToClient(rect);
	rect.MoveToY(nY);
	wnd.MoveWindow(rect);
}

void BranchCondition::SetPosY(int nY )
{
	if(m_type == TYPE_END)
	{//列表
		SetPosY(m_listCondition, nY);
	}
	else
	{
		SetPosY(m_staCondition, nY);
	}
	SetPosY(m_editExp, nY);
	SetPosY(m_btnEditor, nY);
	SetPosY(m_editBranch, nY);
	SetPosY(m_staArrow, nY);

	m_rect.MoveToY(nY);
}

void BranchCondition::Swap(BranchCondition* pBC)
{
	CString strSrc, strDst;

	m_editExp.GetWindowText(strSrc);
	pBC->m_editExp.GetWindowText(strDst);
	m_editExp.SetWindowText(strDst);
	pBC->m_editExp.SetWindowText(strSrc);

	m_editBranch.GetWindowText(strSrc);
	pBC->m_editBranch.GetWindowText(strDst);
	m_editBranch.SetWindowText(strDst);
	pBC->m_editBranch.SetWindowText(strSrc);

	std::swap(m_BranchExpress,pBC->m_BranchExpress);

	// 检测最后一个分支是否受到影响
	if (m_type == TYPE_END)
	{
		//上下交互表达式的时候,最下面一行的条件表达式已经失效了,需重新赋值
		CZComboBox::g_strEditText = m_BranchExpress.m_strFullPathExpress;
		CZComboBox::g_BranchExpress = m_BranchExpress;
	}
	else if (pBC->m_type == TYPE_END)
	{
		//上下交互表达式的时候,最下面一行的条件表达式已经失效了,需重新赋值
		CZComboBox::g_strEditText = pBC->m_BranchExpress.m_strFullPathExpress;
		CZComboBox::g_BranchExpress = pBC->m_BranchExpress;
	}
}
