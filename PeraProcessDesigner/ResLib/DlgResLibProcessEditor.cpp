// ProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgResLibProcessEditor.h"


#include "afxdialogex.h"
#include <Resource.h>
#include <Resource.h>
#include <Magicdb.h>
#include <ReadRobotDB.h>
#include <FlexRobotData.h>
#include <ResLibProcessData.h>
#include "ResLibData.h"
#include "UUID.h"
#include <CStringChecker.h>
#include <PeraProcessDesignerUtility.h>
#include <PeraProcessDesigner.h>

extern CReadRobotDB g_ReadRobotDB;
// CProcessDlg 对话框

IMPLEMENT_DYNAMIC(CDlgResLibProcessEditor, CDialog)

	CDlgResLibProcessEditor::CDlgResLibProcessEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResLibProcessEditor::IDD, pParent)
	, m_PK_ID(_T(""))
	, m_PROCESS_DESC(_T(""))
	, m_PROCESS_VERSION(_T(""))
	, m_PROCESS_AUTHOR(_T(""))
	, m_PROCESS_CREATE_TIME(_T(""))
{

}

CDlgResLibProcessEditor::~CDlgResLibProcessEditor()
{
}

void CDlgResLibProcessEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROCESS_NAME        ,m_PROCESS_NAME);
	DDX_Text(pDX, IDC_EDIT_PROCESS_FILE_PATH   ,m_PROCESS_FILE_PATH);
	DDX_Text(pDX, IDC_RESLIB_PROCESS_EDIT_DESC, m_PROCESS_DESC);
	DDX_Text(pDX, IDC_RLE_COMPONENT_EDIT_VERSION, m_PROCESS_VERSION);
	DDX_Text(pDX, IDC_RLE_COMPONENT_EDIT_AUTHOR, m_PROCESS_AUTHOR);
	DDX_Text(pDX, IDC_RLE_COMPONENT_EDIT_TIME, m_PROCESS_CREATE_TIME);
	DDX_Control(pDX, IDC_RESLIB_PEOCESS_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CDlgResLibProcessEditor, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgResLibProcessEditor::OnBnClickedOk)
END_MESSAGE_MAP()


// CProcessDlg 消息处理程序

BOOL CDlgResLibProcessEditor::SelectProcessFile(PublishInfo* ResLibDataInfo)
{
	
		CFlexRobotData* RobotData = new CFlexRobotData();
		RobotData->m_strLocalPath = ResLibDataInfo->sFilePath.c_str();
		if (!RobotData->GetProcessCaption())
		{
			AfxMessageBox("请检查当前上传设计过程版本！");
			return FALSE;
		}		
		

		m_PROCESS_FILE_PATH = ResLibDataInfo->sFilePath.c_str();
		m_PROCESS_ID	=	((CResLibProcessData*)RobotData->m_pBase)->m_InstanceID.c_str();
		m_PROCESS_NAME			=		ResLibDataInfo->sName.c_str();
		m_PROCESS_DESC					=		ResLibDataInfo->sDesc.c_str();
		m_PROCESS_VERSION		=		ResLibDataInfo->sVersion.c_str();
		m_PROCESS_AUTHOR			=		ResLibDataInfo->sAuthor.c_str();
		m_PROCESS_CREATE_TIME		=		ResLibDataInfo->sTime.c_str();
		return TRUE;
}


BOOL CDlgResLibProcessEditor::SelectProcessFile()
{
	CString strFile = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.ws)|*.ws|"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		CFlexRobotData* RobotData = new CFlexRobotData();
		RobotData->m_strLocalPath = dlgFile.GetPathName();;
		if (!RobotData->GetProcessCaption())
		{
			AfxMessageBox( "请检查当前上传设计过程版本！");
			return FALSE;
		}		
		strFile = dlgFile.GetPathName();

		m_PROCESS_FILE_PATH = strFile;
		m_PROCESS_ID	=	((CResLibProcessData*)RobotData->m_pBase)->m_InstanceID.c_str();
		m_PROCESS_DESC	=	((CResLibProcessData*)RobotData->m_pBase)->m_sDsec.c_str();
		m_PROCESS_AUTHOR	=	((CResLibProcessData*)RobotData->m_pBase)->m_sAuthor.c_str();
		m_PROCESS_VERSION	=	((CResLibProcessData*)RobotData->m_pBase)->m_sVersions.c_str();
		m_PROCESS_CREATE_TIME	=	((CResLibProcessData*)RobotData->m_pBase)->m_sCreatTime.c_str();
		if (g_ReadRobotDB.FindProcessName(m_FK_PID.GetBuffer(),CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle(), "_")))
		{
			for (int i=1;i<100000;i++)
			{
				char sNum[10];
				memset(sNum,0,10);
				sprintf(sNum,"%d",i);
				if (!(g_ReadRobotDB.FindProcessName(m_FK_PID.GetBuffer(),CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle() + sNum, "_"))))
				{
					m_PROCESS_NAME	=	CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle() + sNum, "_").c_str();
					break;
				}
			}
		}
		else
		{
			m_PROCESS_NAME		=			CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle() , "_").c_str();
		}
		return TRUE;
	}
	return FALSE;
}

void CDlgResLibProcessEditor::SetpResLibDataBase(CResLibData* pResLibDataBase)
{
	m_pResLibDataBase	=	pResLibDataBase;
}
void CDlgResLibProcessEditor::SetSelectTree(BOOL bSelectTree)
{
	m_bSelectTree = bSelectTree;
}
void CDlgResLibProcessEditor::OnBnClickedOk()
{
	UpdateData();
	if (m_bSelectTree)
	{
		HTREEITEM hSelect = m_Tree.GetSelectedItem();
		if (hSelect == NULL)
		{
			AfxMessageBox("请选择正确的发布目录！");
			return ;
		}
		if (m_Tree.GetTreeRoot() == hSelect)
		{
			AfxMessageBox("请选择正确的发布目录！");
			return ;
		}
		m_pResLibDataBase =(CResLibData*) m_Tree.GetItemData(hSelect);
		if (m_pResLibDataBase == NULL)
		{
			AfxMessageBox( "请选择正确的发布目录！");
			return ;
		}
		m_FK_PID =  m_pResLibDataBase->m_sPkID.c_str();
		m_PK_ID   =  ReslibMgr::GuidToString(ReslibMgr::CreateGuid()).c_str();
	}
	if(true)
	{//加入检测机制
		bool b; 
		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RLE_COMPONENT_EDIT_AUTHOR), STRATEGY_DEFAULT, "作者");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RESLIB_PROCESS_EDIT_DESC), STRATEGY_DEFAULT, "描述", 200);
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RLE_COMPONENT_EDIT_TIME), STRATEGY_DEFAULT, "时间");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_EDIT_PROCESS_NAME), STRATEGY_NODE, "名称");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RLE_COMPONENT_EDIT_VERSION), STRATEGY_DEFAULT, "版本");
		if(b == false)
		{//检测失败直接返回
			return;
		}
	}
	if (m_OldName == "")
	{
		if (g_ReadRobotDB.FindProcessName(m_FK_PID.GetBuffer(),m_PROCESS_NAME.GetBuffer()))
		{
			AfxMessageBox( "名称重复，请修改！");
			return;
		}
	}
	else
	{
		if (m_OldName != m_PROCESS_NAME)
		{
			if (g_ReadRobotDB.FindProcessName(m_FK_PID.GetBuffer(),m_PROCESS_NAME.GetBuffer()))
			{
				AfxMessageBox( "名称重复，请修改！");
				return;
			}
		}
	}
	

	UpdateData(FALSE);
	CDialog::OnOK(); 
}


BOOL CDlgResLibProcessEditor::OnInitDialog()
{
	CDialog::OnInitDialog();
	//m_OldName = "";
	// TODO:  在此添加额外的初始化
	if (m_bSelectTree)
	{
		m_Tree.Load(CResLibData::RT_PROCESS);
		HTREEITEM hChild = m_Tree.GetChildItem(m_Tree.GetTreeRoot());
		if (hChild != NULL)
		{
			m_Tree.SelectItem(hChild);
		}
		
		m_Tree.SetMenuShow(false);
	}
	else
	{
		GetDlgItem(IDC_RESLIB_PEOCESS_TREE)->EnableWindow(FALSE);
		CRect rect,StatRect;  
		GetDlgItem(IDC_RLE_PROCESS_STAT_TREE)->GetWindowRect(&StatRect);
		this->GetWindowRect(&rect);
		CRect rectNew(rect.left, rect.top, StatRect.left, rect.bottom); 
		BOOL bret = SetWindowPos(GetDlgItem(IDD_RESLIB_PROCESS_EDIT), rectNew.left, rectNew.top, rectNew.Width(), rectNew.Height(), SWP_NOMOVE);

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// Edit控件字串过滤
bool CDlgResLibProcessEditor::CheckEdit( CEdit& edit, STRATEGY strategy /*= STRATEGY_DEFAULT */, CString cName /*= ""*/,int nLenLimit /*= 20*/ )
{
	CString str;
	BOOL b;
	edit.GetWindowText(str);

	switch(strategy)
	{
	case STRATEGY_NODE:
		b = CStringChecker::CheckNodeName(str);
		break;
	case STRATEGY_DIR:
		b = CStringChecker::CheckDirName(str);
		break;
	case STRATEGY_NAME:
		b = CStringChecker::CheckName(str);
		break;
	case STRATEGY_VERSION:
		b = CStringChecker::CheckVersion(str);
		break;
	case STRATEGY_DEFAULT:
		b = CStringChecker::CheckLength(str, nLenLimit);
		break;
	}

	if(!b)
	{//检测失败
		str = cName + CStringChecker::GetLastErr();
		edit.GetParent()->MessageBox(str, g_lpszAppTitle, MB_ICONERROR);
		edit.SetSel(0, -1);
		edit.SetFocus();
		return false;
	}

	return true;
}