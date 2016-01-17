// ToolSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ToolSetDlg.h"
#include "afxdialogex.h"
#include "CommonMethod.h"
#include "Tools.h"

// CToolSetDlg �Ի���

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
	DDX_Text(pDX, IDC_EDIT_ABS_PATH, m_strAbsPath); // ����·��
	DDX_Text(pDX, IDC_EDIT_RUN_PARAM, m_strRunParam);
	DDX_Text(pDX, IDC_EDIT_RUN_NAME, m_strRunName);
	

	// ����������Ϣ
	DDX_Text(pDX, IDC_EDIT_LOCAL_PATH, m_strLocalPath); // ��������·��
	DDX_Text(pDX, IDC_EDIT_LOCAL_RUN_PARAM, m_strLocalRunParam);
}


BEGIN_MESSAGE_MAP(CToolSetDlg, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CToolSetDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CToolSetDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDOK, &CToolSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CToolSetDlg ��Ϣ�������


BOOL CToolSetDlg::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CToolSetDlg::OnBnClickedButtonSelect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlg(TRUE,NULL,NULL,6UL,"��ִ���ļ�(*.exe;*.bat)|*.exe;*.bat;||",NULL,0,FALSE);
	if (dlg.DoModal()!=IDOK)
		return;

	m_strLocalPath = dlg.GetPathName();
	UpdateData(FALSE);
}


void CToolSetDlg::OnBnClickedButtonTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CString strAppPath;
	CString strParam;
	BOOL bIsTestServ = TRUE;
	if (!m_strLocalPath.IsEmpty()) // ����·���ǿ�ʱ���Ա���·��
	{
		m_strLocalPath = ExpandEnvironmentStrings(m_strLocalPath,TRUE);
		strAppPath = m_strLocalPath;
		strParam = m_strLocalRunParam;
		bIsTestServ = FALSE;
	}
	else // ���������в���App
	{
		ASSERT(m_pTool);
		CString strFullPath = ExpandEnvironmentStrings(m_pTool->m_strAbsPath,TRUE);
		if (PathFileExists(strFullPath))
		{
			// �������о���·����Ч
			strAppPath = strFullPath;
			strParam = m_pTool->m_strRunParam;
		}
		else
		{
			strAppPath = CToolsProUnity::GetExePathFormKey(m_pTool,m_pTool->m_strDefaultKey);
			if (strAppPath.IsEmpty())
				strAppPath = CToolsProUnity::GetExePathFormKey(m_pTool,m_pTool->m_strReverseKey);

			if (!strAppPath.IsEmpty()) // ��������ע���ָ����·����Ч
			{
				strParam = m_pTool->m_strRunParam;
			}
		}

	}
	if (strAppPath.IsEmpty() && strParam.IsEmpty() && !m_strLocalRunParam.IsEmpty())
	{
		ZTools::WriteZToolsFormatLog("�����������򿪳���,��������=��%s��",m_strLocalRunParam);
		if (!CToolsProUnity::StartApp(m_strLocalRunParam,""))
			MessageBox("����ʧ��!","��ʾ",MB_OK|MB_ICONERROR);

		return;
	}

	ZTools::WriteZToolsFormatLog("����������·��=%s,����=%s",strAppPath,strParam);
	if (!CToolsProUnity::StartApp(strAppPath,strParam))
	{
		MessageBox("����ʧ��!","��ʾ",MB_OK|MB_ICONERROR);
		return;
	}
	if (1/*bIsTestServ*/)
	{
		m_strLocalPath = strAppPath;
		m_strLocalRunParam = strParam;
		UpdateData(FALSE); // �����Գɹ����·�����µ�����
	}
}


void CToolSetDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	if (m_strLocalPath.IsEmpty() && !m_strLocalRunParam.IsEmpty())
	{
		return CCxBCGPDialog::EndDialog(IDOK); // ����·��Ϊ�գ�ֻ�в���·��������
	}

	if (m_strLocalPath.IsEmpty())
	{
		MessageBox("��ִ���ļ�·������Ϊ��!","��ʾ",MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (!PathFileExists(m_strLocalPath))
	{
		CString strMsg;
		strMsg.Format("�ļ���%s��������!",m_strLocalPath);
		MessageBox(strMsg,"��ʾ",MB_OK|MB_ICONINFORMATION);
		return;
	}
	CCxBCGPDialog::EndDialog(IDOK);
}
