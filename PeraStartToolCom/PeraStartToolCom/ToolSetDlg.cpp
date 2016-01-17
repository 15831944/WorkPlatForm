// ToolSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolSetDlg.h"
#include "afxdialogex.h"
#include "CommonMethod.h"
#include "Tools.h"

// CToolSetDlg 对话框

IMPLEMENT_DYNAMIC(CToolSetDlg, CCxBCGPDialog)

CToolSetDlg::CToolSetDlg(ToolObject *pTool,CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CToolSetDlg::IDD, pParent)
	, m_pTool(pTool)
	, m_strToolName(_T(""))
	, m_strAbsPath(_T(""))
	, m_strDefaultKey(_T(""))
	, m_strReverseKey(_T(""))
	, m_strKeyName(_T(""))
	, m_strKeyValueName(_T(""))
	, m_strRunPath(_T(""))
	, m_strRunParam(_T(""))
	, m_strRunName(_T(""))
{
}

CToolSetDlg::~CToolSetDlg()
{
}

void CToolSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SOFTNAME, m_strToolName);
	DDX_Text(pDX, IDC_EDIT_DEFAULT_KEY, m_strDefaultKey);
	DDX_Text(pDX, IDC_EDIT_RESERVE_KEY, m_strReverseKey);
	DDX_Text(pDX, IDC_EDIT_KEY_NAME, m_strKeyName);
	DDX_Text(pDX, IDC_EDIT_KEY_VALUE, m_strKeyValueName);
	DDX_Text(pDX, IDC_EDIT_RUN_PATH, m_strRunPath);
	DDX_Text(pDX, IDC_EDIT_ABS_PATH, m_strAbsPath); // 绝对路径
	DDX_Text(pDX, IDC_EDIT_RUN_PARAM, m_strRunParam);
	DDX_Text(pDX, IDC_EDIT_RUN_NAME, m_strRunName);
	

	// 本地配置信息
	DDX_Text(pDX, IDC_EDIT_LOCAL_PATH, m_strLocalPath); // 本地配置路径
	DDX_Text(pDX, IDC_EDIT_LOCAL_RUN_PARAM, m_strLocalRunParam);
}


BEGIN_MESSAGE_MAP(CToolSetDlg, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CToolSetDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CToolSetDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDOK, &CToolSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CToolSetDlg 消息处理程序


BOOL CToolSetDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	if (m_pTool)
	{
		m_strToolName  = m_pTool->m_strName;
		m_strAbsPath   = m_pTool->m_strAbsPath;
		m_strDefaultKey = m_pTool->m_strDefaultKey;
		m_strReverseKey  = m_pTool->m_strReverseKey;
		m_strKeyName   = m_pTool->m_strKeyName;
		m_strKeyValueName  = m_pTool->m_strKeyValueName;
		m_strRunPath   = m_pTool->m_strRunPath;
		m_strRunName   = m_pTool->m_strRunName;
		m_strRunParam  = m_pTool->m_strRunParam;

		m_strLocalPath = m_pTool->m_strLocalPath;
		m_strLocalRunParam = m_pTool->m_strLocalRunParam;
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CToolSetDlg::OnBnClickedButtonSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,6UL,"可执行文件(*.exe;*.bat)|*.exe;*.bat;||",NULL,0,FALSE);
	if (dlg.DoModal()!=IDOK)
		return;

	m_strLocalPath = dlg.GetPathName();
	UpdateData(FALSE);
}


void CToolSetDlg::OnBnClickedButtonTest()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString strAppPath;
	CString strParam;
	BOOL bIsTestServ = TRUE;
	if (!m_strLocalPath.IsEmpty()) // 本地路径非空时测试本地路径
	{
		m_strLocalPath = ExpandEnvironmentStrings(m_strLocalPath,TRUE);
		strAppPath = m_strLocalPath;
		strParam = m_strLocalRunParam;
		bIsTestServ = FALSE;
	}
	else // 从软件清册中查找App
	{
		ASSERT(m_pTool);
		CString strFullPath = ExpandEnvironmentStrings(m_pTool->m_strAbsPath,TRUE);
		if (PathFileExists(strFullPath))
		{
			// 软件清册中绝对路径有效
			strAppPath = strFullPath;
			strParam = m_pTool->m_strRunParam;
		}
		else
		{
			strAppPath = CToolsProUnity::GetExePathFormKey(m_pTool,m_pTool->m_strDefaultKey);
			if (strAppPath.IsEmpty())
				strAppPath = CToolsProUnity::GetExePathFormKey(m_pTool,m_pTool->m_strReverseKey);

			if (!strAppPath.IsEmpty()) // 软件清册中注册表指定的路径有效
			{
				strParam = m_pTool->m_strRunParam;
			}
		}

	}
	if (strAppPath.IsEmpty() && strParam.IsEmpty() && !m_strLocalRunParam.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("从启动参数打开程序,启动参数=【%s】",m_strLocalRunParam);
		if (!CToolsProUnity::StartApp(m_strLocalRunParam,""))
			MessageBox("启动失败!","提示",MB_OK|MB_ICONERROR);

		return;
	}

	ZTools::WriteZToolsFormatLog("测试启动，路径=%s,参数=%s",strAppPath,strParam);
	if (!CToolsProUnity::StartApp(strAppPath,strParam))
	{
		MessageBox("启动失败!","提示",MB_OK|MB_ICONERROR);
		return;
	}
	if (1/*bIsTestServ*/)
	{
		m_strLocalPath = strAppPath;
		m_strLocalRunParam = strParam;
		UpdateData(FALSE); // 将测试成功后的路径更新到界面
	}
}


void CToolSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_strLocalPath.IsEmpty() && !m_strLocalRunParam.IsEmpty())
	{
		return CCxBCGPDialog::EndDialog(IDOK); // 本地路径为空，只有参数路径允许保存
	}

	if (m_strLocalPath.IsEmpty())
	{
		MessageBox("可执行文件路径不能为空!","提示",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (!PathFileExists(m_strLocalPath))
	{
		CString strMsg;
		strMsg.Format("文件【%s】不存在!",m_strLocalPath);
		MessageBox(strMsg,"提示",MB_OK|MB_ICONINFORMATION);
		return;
	}
	CCxBCGPDialog::EndDialog(IDOK);
}
