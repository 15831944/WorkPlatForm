// DlgExpEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgExpEditor.h"
#include "afxdialogex.h"
#include "DlgIf.h"

int CALLBACK WordBreakProc(LPSTR, int, int, int) ; //编辑框字符换行回调函数
int FAR PASCAL WordBreakProc(LPSTR lpszEditText, int ichCurrent,
	int cchEditText, int wActionCode)
{
	switch (wActionCode)
	{

	case WB_ISDELIMITER:
		return FALSE; // 不针对某种字符进行换行
	case WB_LEFT:
		return ichCurrent; // 从当前位置换行，不找单词的首字符
	case WB_RIGHT:
		return cchEditText;
	default:
		return FALSE;
	}
}


// CDlgExpEditor 对话框
const char* const CDlgExpEditor::m_str[] = {
	"==",
	"<=",
	"<",
	">=",
	">",
	"!=",
	"+",
	"-",
	"*",
	"/",
	"(",
	")",
	"and",
	"or",
	"not",
	"true",
	"false"
};

IMPLEMENT_DYNAMIC(CDlgExpEditor, CCxBCGPDialog)

CDlgExpEditor::CDlgExpEditor(int nIndex, const char* strExp, CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgExpEditor::IDD, pParent)
	, m_nIndex(nIndex)
	, m_strExp(strExp)
{
	m_IsInitTree = TRUE;
}

CDlgExpEditor::~CDlgExpEditor()
{
}

void CDlgExpEditor::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_EXP, m_editExp);
}


BEGIN_MESSAGE_MAP(CDlgExpEditor, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgExpEditor::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgExpEditor::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgExpEditor::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgExpEditor::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgExpEditor::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDlgExpEditor::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CDlgExpEditor::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CDlgExpEditor::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CDlgExpEditor::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CDlgExpEditor::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CDlgExpEditor::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CDlgExpEditor::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CDlgExpEditor::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CDlgExpEditor::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CDlgExpEditor::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CDlgExpEditor::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CDlgExpEditor::OnBnClickedButton17)
	//ON_BN_CLICKED(IDOK, &CDlgExpEditor::OnBnClickedOk)
	ON_EN_SETFOCUS(IDC_EDIT_EXP, &CDlgExpEditor::OnEnSetfocusEditExp)
	ON_EN_KILLFOCUS(IDC_EDIT_EXP, &CDlgExpEditor::OnEnKillfocusEditExp)
	ON_EN_UPDATE(IDC_EDIT_EXP, &CDlgExpEditor::OnEnUpdateEditExp)
END_MESSAGE_MAP()


void  CDlgExpEditor::InsertText(const char* str)
{
	CEdit& edit = m_editExp;
	CString strAdd = str; //要添加的
	CString strEdit;  //原始的
	edit.GetWindowText(strEdit);

	CStringW wstrEdit(strEdit);

	int nLen = wstrEdit.GetLength();
	int nPos = m_nEditPosStart; //光标
	int nPosEnd = m_nEditPosEnd; //光标

	CString strLeft(wstrEdit.Left(nPos));
	CString strRight(wstrEdit.Right(wstrEdit.GetLength() - nPosEnd));

	//if(nPos > 0) strAdd = " " + strAdd;
	//if(nPos < nLen) strAdd += " ";
	CString strT = strLeft + strAdd + strRight;			

	edit.SetSel(m_nEditPosStart, m_nEditPosEnd);
	edit.ReplaceSel(strAdd);
	//edit.SetWindowText(strT);

// 	CStringW wstrAdd(strAdd);
//  	m_nEditPosStart +=wstrAdd.GetLength();
//  	m_nEditPosEnd = m_nEditPosStart;
}

void  CDlgExpEditor::InsertTextByID(int nID)
{
	InsertText(CString(" ") + m_str[nID - 1] + " ");
	m_editExp.SetSel(m_nEditPosStart, m_nEditPosEnd);
	m_editExp.SetFocus();
}

// CDlgExpEditor 消息处理程序

//  ==
void CDlgExpEditor::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(1);
}

// <=
void CDlgExpEditor::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(2);
}

// <
void CDlgExpEditor::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(3);
}

// >=
void CDlgExpEditor::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(4);
}

// >
void CDlgExpEditor::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(5);
}

// !=
void CDlgExpEditor::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(6);
}

// +
void CDlgExpEditor::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(7);
}

// -
void CDlgExpEditor::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(8);
}

// *
void CDlgExpEditor::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(9);
}

//  /
void CDlgExpEditor::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(10);
}

// (
void CDlgExpEditor::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(11);
}

// )
void CDlgExpEditor::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(12);
}

// AND
void CDlgExpEditor::OnBnClickedButton13()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(13);
}

// OR
void CDlgExpEditor::OnBnClickedButton14()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(14);
}

// NOT
void CDlgExpEditor::OnBnClickedButton15()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(15);
}

// TRUE
void CDlgExpEditor::OnBnClickedButton16()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(16);
}

// FALSE
void CDlgExpEditor::OnBnClickedButton17()
{
	// TODO: 在此添加控件通知处理程序代码
	InsertTextByID(17);
}


void CDlgExpEditor::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_editExp.GetWindowText(m_strExp);
	m_strExp.Replace("\r\n", "");
	CCxBCGPDialog::OnOK();
}

int CDlgExpEditor::MoveItemUp(CWnd* wndItem, CWnd* wndDst, int d)
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


BOOL CDlgExpEditor::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon
	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_BUTTON13)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON14)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON15)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON16)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON17)->ShowWindow(SW_HIDE);

	int d = 50;
	MoveItemUp(GetDlgItem(IDC_BUTTON1), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON2), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON3), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON4), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON5), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON6), GetDlgItem(IDC_EDIT_EXP), d);

	d = 100;
	MoveItemUp(GetDlgItem(IDC_BUTTON7), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON8), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON9), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON10), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON11), GetDlgItem(IDC_EDIT_EXP), d);
	MoveItemUp(GetDlgItem(IDC_BUTTON12), GetDlgItem(IDC_EDIT_EXP), d);

	CString str;
	GetWindowText(str);
	SetWindowText(CString(g_lpszAppTitle) + "--" + str);

	CRect rectGrid;
	CWnd& m_wndTreeLocation = *GetDlgItem(IDC_STATIC_TREE);
	m_wndTreeLocation.GetClientRect (&rectGrid);
	m_wndTreeLocation.MapWindowPoints (this, &rectGrid);
	rectGrid.DeflateRect(1, 1, 1, 1);
	m_wndGridTree.InitTree(rectGrid, this);

	m_wndGridTree.IsShowRoot(m_IsShowRoot);
	//20140526,lc,修改为左侧树不初始化，影响加载效率，因为后期设计中已无左侧树
	if (m_IsInitTree)
	{
		if(m_IsShowRoot)
		{//根节点
			m_wndGridTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot(),NULL );
		}
		else
		{//当前节点
			m_wndGridTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData(),NULL );
		}
		m_wndGridTree.AdjustLayout();
	}
	//给编辑控件发送 EM_SETWORDBREAKPROC 消息注册自定义换行函数，该函数应该在给编辑框填充文本前调用
	SendDlgItemMessage(IDC_EDIT_EXP, EM_SETWORDBREAKPROC, 0,(LPARAM)(EDITWORDBREAKPROC)WordBreakProc) ;

	m_editExp.SetWindowText(m_strExp);
	m_nEditPosStart = 0;
	m_nEditPosEnd = 0;
	CStringW strW(m_strExp);
	m_nEditPosEnd = strW.GetLength(); 
	m_nEditPosStart = m_nEditPosEnd; //设置光标为末尾

	m_editExp.SetSel(m_nEditPosStart, m_nEditPosEnd);

	EnableVisualManagerStyle (TRUE, TRUE);

	m_editExp.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgExpEditor::OnEnSetfocusEditExp()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDlgExpEditor::OnEnKillfocusEditExp()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nEditPosStart = m_editExp.GetSel() & 0xFFFF;
	m_nEditPosEnd = m_editExp.GetSel() >> 16 & 0xFFFF;
}


void CDlgExpEditor::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	OnBnClickedOk();
}


void CDlgExpEditor::OnEnUpdateEditExp()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CCxBCGPDialog::OnInitDialog()
	// 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
	// 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_nEditPosStart = m_editExp.GetSel() & 0xFFFF;
	m_nEditPosEnd = m_editExp.GetSel() >> 16 & 0xFFFF;
}
