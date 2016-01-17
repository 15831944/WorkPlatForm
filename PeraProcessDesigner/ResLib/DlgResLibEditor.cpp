// ComponentDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgResLibEditor.h"
#include "afxdialogex.h"
#include "FlexRobotData.h"
#include "ResLibRobotData.h"
//#include "ComponentInfo.h"
#include "ReadRobotDB.h"
#include <shlwapi.h>
#include "ResLibData.h"
#include "UUID.h"
#include <CStringChecker.h>
#include <PeraProcessDesignerUtility.h>
#include <PeraProcessDesigner.h>
#include "Tools.h"
#include "FlexRobotWithForm.h"

#pragma comment(lib,"Shlwapi.lib") //如果没有这行，会出现link错误

// CComponentDlg 对话框
extern CReadRobotDB g_ReadRobotDB;

IMPLEMENT_DYNAMIC(CDlgResLibEditor, CDialog)

CDlgResLibEditor::CDlgResLibEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResLibEditor::IDD, pParent)
	, m_COMPONENT_PK_ID(_T(""))
	, m_COMPONENT_FK_PID(_T(""))
	, m_COMPONENT_NAME(_T(""))
	, m_COMPONENT_VERSION(_T(""))
	, m_COMPONENT_PIC_16(_T(""))
	, m_COMPONENT_SRC(_T(""))
	, m_FILE_PATH(_T(""))
	, m_INSTANCE_PK_ID(_T(""))
	, m_TAG(_T(""))
	, m_csDesc(_T(""))
	, m_COMPONENT_CREATE_TIME(_T(""))
	, m_COMPONENT_AUTHOR(_T(""))
	
{

}

CDlgResLibEditor::~CDlgResLibEditor()
{
}

void CDlgResLibEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_COMPONENT_NAME, m_COMPONENT_NAME);
	DDX_Text(pDX, IDC_EDIT_COMPONENT_FILE_PATH, m_FILE_PATH);
	DDX_Text(pDX, IDC_RESLIB_EDIT_DESC, m_csDesc);
	DDX_Text(pDX, IDC_RLE_COMPONENT_EDIT_VERSION, m_COMPONENT_VERSION);
	DDX_Text(pDX, IDC_RLE_COMPONENT_EDIT_AUTHOR, m_COMPONENT_AUTHOR);
	DDX_Text(pDX, IDC_RLE_COMPONENT_EDIT_TIME, m_COMPONENT_CREATE_TIME);
	DDX_Control(pDX, IDC_RESLIB_ROBOT_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CDlgResLibEditor, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgResLibEditor::OnIDOK)
END_MESSAGE_MAP()


// CComponentDlg 消息处理程序


void CDlgResLibEditor::OnIDOK()
{
	UpdateData();
	
// 	if (!PathFileExists(m_FILE_PATH))
// 	{
// 		AfxMessageBox("请正常选择路径！");
// 		return;
// 	}
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
			AfxMessageBox("请选择正确的发布目录！");
			return ;
		}
		m_COMPONENT_FK_PID =  m_pResLibDataBase->m_sPkID.c_str();
		m_COMPONENT_PK_ID   =  ReslibMgr::GuidToString(ReslibMgr::CreateGuid()).c_str();
	}

	if(true)
	{//加入检测机制
		bool b; 
		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RLE_COMPONENT_EDIT_AUTHOR), STRATEGY_DEFAULT, "作者");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RESLIB_EDIT_DESC), STRATEGY_DEFAULT, "描述", 200);
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_RLE_COMPONENT_EDIT_TIME), STRATEGY_DEFAULT, "时间");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(*(CEdit*)GetDlgItem(IDC_EDIT_COMPONENT_NAME), STRATEGY_NODE, "名称");
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
		if (g_ReadRobotDB.FindRobotName(m_COMPONENT_FK_PID.GetBuffer(),m_COMPONENT_NAME.GetBuffer()))
		{
			AfxMessageBox("名称重复，请修改！");
			return;
		}
	}
	else
	{
		if (m_OldName != m_COMPONENT_NAME)
		{
			if (g_ReadRobotDB.FindRobotName(m_COMPONENT_FK_PID.GetBuffer(),m_COMPONENT_NAME.GetBuffer()))
			{
				AfxMessageBox("名称重复，请修改！");
				return;
			}
		}
	}

	
	
	UpdateData(FALSE);
	CDialog::OnOK(); 
}
void CDlgResLibEditor::SetpResLibDataBase(CResLibData* pResLibDataBase)
{
	m_pResLibDataBase	=	pResLibDataBase;
}
void CDlgResLibEditor::SetSelectTree(BOOL bSelectTree)
{
	m_bSelectTree = bSelectTree;
}
BOOL CDlgResLibEditor::SelectRobotFile()
{
	CString strFile = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.bot;*.botw)|*.bot;*.botw||"), NULL);

	if (dlgFile.DoModal() == IDOK)
	{
		strFile = dlgFile.GetPathName();
		m_FILE_PATH = strFile;

		if (g_ReadRobotDB.FindRobotName(m_COMPONENT_FK_PID.GetBuffer(),CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle(), "_")))
		{
			for (int i=1;i<100000;i++)
			{
				char sNum[10];
				memset(sNum,0,10);
				sprintf(sNum,"%d",i);
				if (!(g_ReadRobotDB.FindRobotName(m_COMPONENT_FK_PID.GetBuffer(),CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle() + sNum, "_"))))
				{
					m_COMPONENT_NAME	=	CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle() + sNum, "_").c_str();
					break;
				}
			}
		}
		else
		{
			m_COMPONENT_NAME		=			CStringChecker::ReplaceNodeName(dlgFile.GetFileTitle() , "_").c_str();
		}

		CFlexRobotData* RobotData = NULL;

		if (ZTools::CompareExtName((LPCTSTR)strFile, ".bot"))
		{
			RobotData = new CFlexRobotData();
			RobotData->m_strLocalPath = m_FILE_PATH;
			if(!RobotData->GetRobotCaption())
			{
				AfxMessageBox("所选文件不是有效组件文件！");
				return FALSE;
			}
			if (((CResLibRobotData*)RobotData->m_pBase)->m_RobotType == "E712BA3D-34AD-4286-AAC5-D800BAF60415")
			{
				AfxMessageBox("不允许添加自定义界面组件！");
				m_FILE_PATH = "";
				return FALSE;
			}
			
			m_COMPONENT_VERSION		=			((CResLibRobotData*)RobotData->m_pBase)->m_sVersions.c_str();
			m_COMPONENT_AUTHOR		=			((CResLibRobotData*)RobotData->m_pBase)->m_sAuthor.c_str();
			m_csDesc				=			((CResLibRobotData*)RobotData->m_pBase)->m_sDsec.c_str();
			m_COMPONENT_CREATE_TIME	=			((CResLibRobotData*)RobotData->m_pBase)->m_sPublishTime.c_str();

			m_COMPONENT_SRC			=			((CResLibRobotData*)RobotData->m_pBase)->m_sSrc.c_str();		

			m_INSTANCE_PK_ID		=			((CResLibRobotData*)RobotData->m_pBase)->m_InstanceID.c_str();
			m_TAG					=			((CResLibRobotData*)RobotData->m_pBase)->m_RobotType.c_str();
		}
		else if (ZTools::CompareExtName((LPCTSTR)strFile, ".botw"))
		{
			RobotData = new CFlexRobotWithForm();
			RobotData->m_strLocalPath = m_FILE_PATH;
			if(!RobotData->GetRobotCaption())
			{
				AfxMessageBox("所选文件不是有效组件文件！");
				return FALSE;
			}
			if (((CFlexRobotWithForm*)RobotData)->m_strCompType == "E712BA3D-34AD-4286-AAC5-D800BAF60415")
			{
				AfxMessageBox("不允许添加自定义界面组件！");
				m_FILE_PATH = "";
				return FALSE;
			}

			m_COMPONENT_VERSION		=			((CFlexRobotWithForm*)RobotData)->m_strVersion.c_str();
			m_COMPONENT_AUTHOR		=			((CFlexRobotWithForm*)RobotData)->m_strAuth.c_str();
			m_csDesc				=			((CFlexRobotWithForm*)RobotData)->m_strDesc.c_str();
			m_COMPONENT_CREATE_TIME	=			((CFlexRobotWithForm*)RobotData)->m_strTime.c_str();

			m_COMPONENT_SRC			=			"";		
			
			m_TAG					=			((CFlexRobotWithForm*)RobotData)->m_strCompType.c_str();
			m_INSTANCE_PK_ID		=			((CFlexRobotWithForm*)RobotData)->m_strCompInstanceId.c_str();
		}
		else
		{
			return FALSE;
		}
		
		m_ParametsJson			=			RobotData->GetParametJson().c_str();
		
		if (RobotData != NULL)
		{
			delete RobotData;
			RobotData = NULL;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CDlgResLibEditor::SelectRobotFile(PublishInfo* ResLibDataInfo)
{
		m_FILE_PATH					=		ResLibDataInfo->sFilePath.c_str();
		m_COMPONENT_NAME			=		ResLibDataInfo->sName.c_str();
		m_csDesc					=		ResLibDataInfo->sDesc.c_str();
		m_COMPONENT_VERSION			=		ResLibDataInfo->sVersion.c_str();
		m_COMPONENT_AUTHOR			=		ResLibDataInfo->sAuthor.c_str();
		m_COMPONENT_CREATE_TIME		=		ResLibDataInfo->sTime.c_str();

		CFlexRobotData* RobotData = NULL;

		if (ZTools::CompareExtName(ResLibDataInfo->sFilePath, ".bot"))
		{
			RobotData = new CFlexRobotData();
			RobotData->m_strLocalPath = m_FILE_PATH;
			if(!RobotData->GetRobotCaption())
			{
				AfxMessageBox("所选文件不是有效组件文件！");
				return FALSE;
			}
			if (((CResLibRobotData*)RobotData->m_pBase)->m_RobotType == "E712BA3D-34AD-4286-AAC5-D800BAF60415")
			{
				AfxMessageBox("不允许添加自定义界面组件！");
				m_FILE_PATH = "";
				return FALSE;
			}

			m_TAG					=			((CResLibRobotData*)RobotData->m_pBase)->m_RobotType.c_str();
			m_INSTANCE_PK_ID		=			((CResLibRobotData*)RobotData->m_pBase)->m_InstanceID.c_str();
		}
		else if (ZTools::CompareExtName(ResLibDataInfo->sFilePath, ".botw"))
		{
			RobotData = new CFlexRobotWithForm();
			RobotData->m_strLocalPath = m_FILE_PATH;
			if(!RobotData->GetRobotCaption())
			{
				AfxMessageBox("所选文件不是有效组件文件！");
				return FALSE;
			}
			if (((CFlexRobotWithForm*)RobotData)->m_strCompType == "E712BA3D-34AD-4286-AAC5-D800BAF60415")
			{
				AfxMessageBox("不允许添加自定义界面组件！");
				m_FILE_PATH = "";
				return FALSE;
			}

			m_TAG					=			((CFlexRobotWithForm*)RobotData)->m_strCompType.c_str();
			m_INSTANCE_PK_ID		=			((CFlexRobotWithForm*)RobotData)->m_strCompInstanceId.c_str();
		}
		else
		{
			return FALSE;
		}

		m_ParametsJson			=			RobotData->GetParametJson().c_str();

		if (RobotData != NULL)
		{
			delete RobotData;
			RobotData = NULL;
		}
		return TRUE;
	
}
BOOL CDlgResLibEditor::OnInitDialog()
{
	CDialog::OnInitDialog();
	//m_OldName = "";
	// TODO:  在此添加额外的初始化
	if (m_bSelectTree)
	{
		m_Tree.Load(CResLibData::RT_ROBOT);
		m_Tree.SetMenuShow(false);
		HTREEITEM hChild = m_Tree.GetChildItem(m_Tree.GetTreeRoot());
		if (hChild != NULL)
		{
			m_Tree.SelectItem(hChild);
		}
	}
	else
	{
		GetDlgItem(IDC_RESLIB_ROBOT_TREE)->EnableWindow(FALSE);
		CRect rect,StatRect;  
		GetDlgItem(IDC_RLE_COMPONENT_STAT_TREE)->GetWindowRect(&StatRect);
		this->GetWindowRect(&rect);
		CRect rectNew(rect.left, rect.top, StatRect.left, rect.bottom); 
		BOOL bret = SetWindowPos(GetDlgItem(IDD_RESLIB_EDIT), rectNew.left, rectNew.top, rectNew.Width(), rectNew.Height(), SWP_NOMOVE);

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// Edit控件字串过滤
bool CDlgResLibEditor::CheckEdit( CEdit& edit, STRATEGY strategy /*= STRATEGY_DEFAULT */, CString cName /*= ""*/,int nLenLimit /*= 20*/ )
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