// DlgExpEditor.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgExpEditor.h"
#include "afxdialogex.h"
#include "DlgIf.h"

int CALLBACK WordBreakProc(LPSTR, int, int, int) ; //�༭���ַ����лص�����
int FAR PASCAL WordBreakProc(LPSTR lpszEditText, int ichCurrent,
	int cchEditText, int wActionCode)
{
	switch (wActionCode)
	{

	case WB_ISDELIMITER:
		return FALSE; // �����ĳ���ַ����л���
	case WB_LEFT:
		return ichCurrent; // �ӵ�ǰλ�û��У����ҵ��ʵ����ַ�
	case WB_RIGHT:
		return cchEditText;
	default:
		return FALSE;
	}
}


// CDlgExpEditor �Ի���
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
	CString strAdd = str; //Ҫ��ӵ�
	CString strEdit;  //ԭʼ��
	edit.GetWindowText(strEdit);

	CStringW wstrEdit(strEdit);

	int nLen = wstrEdit.GetLength();
	int nPos = m_nEditPosStart; //���
	int nPosEnd = m_nEditPosEnd; //���

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

// CDlgExpEditor ��Ϣ�������

//  ==
void CDlgExpEditor::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(1);
}

// <=
void CDlgExpEditor::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(2);
}

// <
void CDlgExpEditor::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(3);
}

// >=
void CDlgExpEditor::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(4);
}

// >
void CDlgExpEditor::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(5);
}

// !=
void CDlgExpEditor::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(6);
}

// +
void CDlgExpEditor::OnBnClickedButton7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(7);
}

// -
void CDlgExpEditor::OnBnClickedButton8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(8);
}

// *
void CDlgExpEditor::OnBnClickedButton9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(9);
}

//  /
void CDlgExpEditor::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(10);
}

// (
void CDlgExpEditor::OnBnClickedButton11()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(11);
}

// )
void CDlgExpEditor::OnBnClickedButton12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(12);
}

// AND
void CDlgExpEditor::OnBnClickedButton13()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(13);
}

// OR
void CDlgExpEditor::OnBnClickedButton14()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(14);
}

// NOT
void CDlgExpEditor::OnBnClickedButton15()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(15);
}

// TRUE
void CDlgExpEditor::OnBnClickedButton16()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(16);
}

// FALSE
void CDlgExpEditor::OnBnClickedButton17()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InsertTextByID(17);
}


void CDlgExpEditor::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	//20140526,lc,�޸�Ϊ���������ʼ����Ӱ�����Ч�ʣ���Ϊ������������������
	if (m_IsInitTree)
	{
		if(m_IsShowRoot)
		{//���ڵ�
			m_wndGridTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot(),NULL );
		}
		else
		{//��ǰ�ڵ�
			m_wndGridTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData(),NULL );
		}
		m_wndGridTree.AdjustLayout();
	}
	//���༭�ؼ����� EM_SETWORDBREAKPROC ��Ϣע���Զ��廻�к������ú���Ӧ���ڸ��༭������ı�ǰ����
	SendDlgItemMessage(IDC_EDIT_EXP, EM_SETWORDBREAKPROC, 0,(LPARAM)(EDITWORDBREAKPROC)WordBreakProc) ;

	m_editExp.SetWindowText(m_strExp);
	m_nEditPosStart = 0;
	m_nEditPosEnd = 0;
	CStringW strW(m_strExp);
	m_nEditPosEnd = strW.GetLength(); 
	m_nEditPosStart = m_nEditPosEnd; //���ù��Ϊĩβ

	m_editExp.SetSel(m_nEditPosStart, m_nEditPosEnd);

	EnableVisualManagerStyle (TRUE, TRUE);

	m_editExp.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgExpEditor::OnEnSetfocusEditExp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CDlgExpEditor::OnEnKillfocusEditExp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_nEditPosStart = m_editExp.GetSel() & 0xFFFF;
	m_nEditPosEnd = m_editExp.GetSel() >> 16 & 0xFFFF;
}


void CDlgExpEditor::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	OnBnClickedOk();
}


void CDlgExpEditor::OnEnUpdateEditExp()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CCxBCGPDialog::OnInitDialog()
	// �������Խ� EM_SETEVENTMASK ��Ϣ���͵��ÿؼ���
	// ͬʱ�� ENM_UPDATE ��־�������㵽 lParam �����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_nEditPosStart = m_editExp.GetSel() & 0xFFFF;
	m_nEditPosEnd = m_editExp.GetSel() >> 16 & 0xFFFF;
}
