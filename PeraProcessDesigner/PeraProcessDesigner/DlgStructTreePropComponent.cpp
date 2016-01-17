// DlgStructTreePropComponent.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgStructTreePropComponent.h"
#include "afxdialogex.h"
#include "DlgSvg.h"
#include "PeraProcessDesignerUtility.h"
#include "ConvertData.h"
#include "CStringChecker.h"


// CDlgStructTreePropComponent 对话框

BEGIN_MESSAGE_MAP(CDlgStructTreePropComponent, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgStructTreePropComponent::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CDlgStructTreePropComponent::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_STPC_OK, &CDlgStructTreePropComponent::OnBnClickedStpcOk)
	ON_BN_CLICKED(IDC_STPC_CANCEL, &CDlgStructTreePropComponent::OnBnClickedStpcCancel)
END_MESSAGE_MAP()

void CDlgStructTreePropComponent::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STPC_NAME, m_wndName);
	DDX_Control(pDX, IDC_STPC_AUTHOR, m_wndAuthor);
	DDX_Control(pDX, IDC_STPC_CREATED, m_wndCreated);
	DDX_Control(pDX, IDC_STPC_VERSION, m_wndVersion);
	DDX_Control(pDX, IDC_STPC_DESCRP, m_wndDescrp);
	DDX_Control(pDX, IDC_STPC_OK, m_wndOk);
	DDX_Control(pDX, IDC_STPC_CANCEL, m_wndCancel);
}

IMPLEMENT_DYNAMIC(CDlgStructTreePropComponent, CDialogEx)

CDlgStructTreePropComponent::CDlgStructTreePropComponent(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgStructTreePropComponent::IDD, pParent)
{
	m_pNode = NULL;
}

CDlgStructTreePropComponent::~CDlgStructTreePropComponent()
{
}

void CDlgStructTreePropComponent::OnBnClickedOk()
{
}


void CDlgStructTreePropComponent::OnBnClickedCancel()
{
}


BOOL CDlgStructTreePropComponent::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (m_pNode != NULL)
	{
		m_wndAuthor.SetWindowText(m_pNode->m_strAuthor);
		m_wndCreated.SetWindowText(m_pNode->m_strCreated);
		m_wndDescrp.SetWindowText(m_pNode->m_strDescription);
		m_wndName.SetWindowText(m_pNode->m_strText);
		m_wndVersion.SetWindowText(m_pNode->m_strVersion);
	}
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



// CDlgStructTreePropComponent 消息处理程序


void CDlgStructTreePropComponent::OnBnClickedStpcOk()
{
	CDlgSvg * pSvg = GetSvgDilogPtr();
	if ( pSvg == NULL )
	{
		EndDialog(FALSE); return;
	}

	if(true)
	{//加入检测机制
		bool b; 
		b = CheckEdit(m_wndAuthor, STRATEGY_DEFAULT, "作者");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(m_wndCreated, STRATEGY_DEFAULT, "日期");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(m_wndDescrp, STRATEGY_DEFAULT, "描述", 200);
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(m_wndName, STRATEGY_NODE, "名称");
		if(b == false)
		{//检测失败直接返回
			return;
		}

		b = CheckEdit(m_wndVersion, STRATEGY_DEFAULT, "版本");
		if(b == false)
		{//检测失败直接返回
			return;
		}
	}

	BOOL bModified = FALSE;
	CString str;

	CCxStructTreeNodeComponent * pTempNode = new CCxStructTreeNodeComponent( NULL );
	pTempNode->m_strID = m_pNode->m_strID;

	m_wndAuthor.GetWindowText(pTempNode->m_strAuthor);
	if ( m_pNode->m_strAuthor != pTempNode->m_strAuthor )
	{
		bModified = TRUE;
	}
	m_wndCreated.GetWindowText(pTempNode->m_strCreated);
	if ( m_pNode->m_strCreated != pTempNode->m_strCreated )
	{
		bModified = TRUE;
	}
	m_wndDescrp.GetWindowText(pTempNode->m_strDescription);
	if ( m_pNode->m_strDescription != pTempNode->m_strDescription )
	{
		bModified = TRUE;
	}
	m_wndName.GetWindowText(pTempNode->m_strOriName);
	pTempNode->m_strText = pTempNode->m_strOriName;
	if ( m_pNode->m_strText != pTempNode->m_strText )
	{
		bModified = TRUE;
	}
	m_wndVersion.GetWindowText(pTempNode->m_strVersion);
	if ( m_pNode->m_strVersion != pTempNode->m_strVersion )
	{
		bModified = TRUE;
	}

	if ( !bModified )
	{
		delete pTempNode; pTempNode = NULL; EndDialog(FALSE); return;
	}

	CWebMethodParamData * pParam = NULL;
	CWebMethodData wmd;
	//
	wmd.m_sMethodName = "propUpdate";
	//第1个参数
	pParam = new CWebMethodParamData();
	pParam->m_sName = "jsonData";
	pParam->m_sValue = pTempNode->GetUpdatePropJsonStr();
	wmd.m_vParams.push_back(pParam);
	//
	if ( !pSvg->InvokeJsMethod( &wmd ) )
	{
		MessageBox("修改属性失败。请确保svg功能正常。", g_lpszAppTitle, MB_OK|MB_ICONERROR );
		delete pTempNode; pTempNode = NULL;
		EndDialog(FALSE); return;
	}
	if ( wmd.m_sResult.IsEmpty() )
	{
		m_pNode->m_strAuthor = pTempNode->m_strAuthor;
		m_pNode->m_strCreated = pTempNode->m_strCreated;
		m_pNode->m_strDescription = pTempNode->m_strDescription;
		m_pNode->m_strText = pTempNode->m_strText;
		m_pNode->m_strOriName = pTempNode->m_strOriName;
		m_pNode->m_strVersion = pTempNode->m_strVersion;
		delete pTempNode; pTempNode = NULL;
		EndDialog( TRUE ); return;
	}
	else
	{
		vector< CString > vErrFlag;
		CString2Vector( vErrFlag, wmd.m_sResult );
		for (int i=0; i<(int)vErrFlag.size(); i++)
		{
			if ( vErrFlag[i] == "same_name" )
			{
				MessageBox("名称必须确保唯一", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION );
				m_wndName.SetSel(0, -1);
				m_wndName.SetFocus();
				break;
			}
		}
		delete pTempNode; pTempNode = NULL;
	}
}


void CDlgStructTreePropComponent::OnBnClickedStpcCancel()
{
	EndDialog(FALSE); return;
}

// Edit控件字串过滤
bool CDlgStructTreePropComponent::CheckEdit( CEdit& edit, STRATEGY strategy /*= STRATEGY_DEFAULT */, CString cName /*= ""*/,int nLenLimit /*= 20*/ )
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