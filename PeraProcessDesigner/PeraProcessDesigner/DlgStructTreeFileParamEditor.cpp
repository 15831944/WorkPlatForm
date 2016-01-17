// DlgStructTreeFileParamEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgStructTreeFileParamEditor.h"
#include "afxdialogex.h"


// DlgStructTreeFileParamEditor 对话框


BEGIN_MESSAGE_MAP(CDlgStructTreeFileParamEditor, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgStructTreeFileParamEditor::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CDlgStructTreeFileParamEditor::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_STFP_OK, &CDlgStructTreeFileParamEditor::OnBnClickedStfpOk)
	ON_BN_CLICKED(IDC_STFP_CANCEL, &CDlgStructTreeFileParamEditor::OnBnClickedStfpCancel)
	ON_BN_CLICKED(IDC_STFP_VIEW, &CDlgStructTreeFileParamEditor::OnBnClickedStfpView)
	ON_BN_CLICKED(IDC_STFP_PATHSEL, &CDlgStructTreeFileParamEditor::OnBnClickedStfpPathsel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CDlgStructTreeFileParamEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STFP_PATH, m_wndPath);
	DDX_Control(pDX, IDC_STFP_PATHSEL, m_wndPathSel);
	DDX_Control(pDX, IDC_STFP_VIEW, m_wndViewFile);
	DDX_Control(pDX, IDC_STFP_OK, m_wndOk);
	DDX_Control(pDX, IDC_STFP_CANCEL, m_wndCancel);
}

IMPLEMENT_DYNAMIC(CDlgStructTreeFileParamEditor, CDialogEx)

CDlgStructTreeFileParamEditor::CDlgStructTreeFileParamEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgStructTreeFileParamEditor::IDD, pParent)
{
	m_pNode = NULL;
}

CDlgStructTreeFileParamEditor::~CDlgStructTreeFileParamEditor()
{
}

void CDlgStructTreeFileParamEditor::OnBnClickedOk()
{
}


void CDlgStructTreeFileParamEditor::OnBnClickedCancel()
{
}


BOOL CDlgStructTreeFileParamEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_wndViewFile.m_bDefaultClick = TRUE;
	m_menu.LoadMenu (IDR_JIANMO_PARAM_FILE_VIEW_POP);
	m_wndViewFile.m_hMenu = m_menu.GetSubMenu (0)->GetSafeHmenu ();
	m_wndViewFile.m_bOSMenu = FALSE;
	m_wndViewFile.m_bStayPressed = TRUE;

	if ( m_pNode != NULL )
	{
		m_wndPath.SetWindowText( m_pNode->m_strValue );
		
		if (
			m_pNode->m_nDir == PARAM_DIR_OUT
			|| (m_pNode->m_dwStatus & CCxStructTreeNode::NS_VALUE_READONLY)
			)
		{
			m_wndPath.SetReadOnly(TRUE);
			m_wndPathSel.EnableWindow(FALSE);
		}
	}
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	return TRUE;
}

void CDlgStructTreeFileParamEditor::OnBnClickedStfpOk()
{
	if ( m_pNode == NULL )
	{
		EndDialog(FALSE); return;
	}

	if ( m_pNode->m_nDir == PARAM_DIR_OUT )
	{
		EndDialog(FALSE); return;
	}

	CString strNewValue;
	m_wndPath.GetWindowText( strNewValue );

	if (!strNewValue.IsEmpty() && !PathFileExists(strNewValue))
	{
		CString sMsg;
		sMsg.Format("文件 %s 不存在", strNewValue);
		MessageBox(sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	if ( strNewValue == m_pNode->m_strValue )
	{
		EndDialog(FALSE); return;
	}
	
	m_pNode->m_strValue = strNewValue;
//	m_pNode->SetModified();
	EndDialog(TRUE); return;
}


void CDlgStructTreeFileParamEditor::OnBnClickedStfpCancel()
{
	EndDialog(FALSE); return;
}


void CDlgStructTreeFileParamEditor::OnBnClickedStfpView()
{
	CString strNewValue;
	m_wndPath.GetWindowText( strNewValue );

	if (strNewValue.IsEmpty())
	{
		MessageBox("文件路径为空，无法查看！", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	if (PathFileExists(strNewValue) == FALSE || PathIsDirectory(strNewValue) == FILE_ATTRIBUTE_DIRECTORY)
	{
		MessageBox("文件路径无效，无法查看！", g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		return;
	}

	switch (m_wndViewFile.m_nMenuResult)
	{
	case IDR_JIANMO_PARAM_FILE_VIEW_POP_NOTEPAD:
		{
			ShellExecute(NULL, "open", "notepad.exe", strNewValue, NULL, SW_SHOW);
		}
		break;

	case IDR_JIANMO_PARAM_FILE_VIEW_POP_FOLDER:
		{
			CString strParam;
			strParam.Format("/e,/select,\"%s\"", strNewValue);
			ShellExecute(NULL, "open", "explorer.exe", strParam, NULL, SW_SHOW);
		}
		break;
		
	default:
		{
			SHELLEXECUTEINFO si;
			memset(&si, NULL, sizeof(si));
			si.cbSize = sizeof(si);
			si.nShow = SW_SHOW;
			si.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
			si.lpVerb = _T("open");
			//si.lpDirectory = pThis->m_strAppWorkPath;
			si.lpFile = strNewValue;
			//si.lpParameters = pThis->m_strAppParam;
			if (!ShellExecuteEx(&si))
			{
				CString sErrorMessage;

				DWORD errCode = GetLastError();
				LPVOID lpMsgBuf;
				FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					errCode,
					0, // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
					);
				sErrorMessage.Format("%s", lpMsgBuf);
				LocalFree( lpMsgBuf );
				MessageBox(sErrorMessage, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);

				//AfxMessageBox( "没有找到对应的应用程序，请手动设置文件关联后，再查看" );
				return;
			}
		}
		break;
	}
	
}


void CDlgStructTreeFileParamEditor::OnBnClickedStfpPathsel()
{
	CFileDialog dlgFile(TRUE
		, NULL
		, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, "All Files (*.*)|*.*||"
		//, AfxGetMainWnd()
		);

	dlgFile.m_ofn.lpstrFile = new TCHAR[MAX_PATH*4]; //重新定义缓冲区大小          
	memset(dlgFile.m_ofn.lpstrFile, 0, MAX_PATH * 4);  //初始化定义的缓冲区
	dlgFile.m_ofn.nMaxFile = MAX_PATH*4; // 指定缓冲区大小

	CString strFilePath/* = m_pNode->m_strValue*/;
// 	strFilePath.Replace('/', '\\');
// 	if (PathFileExists(strFilePath))
// 	{
// 		strFilePath = strFilePath.Left(strFilePath.ReverseFind('\\') + 1);
// 		dlgFile.m_ofn.lpstrInitialDir = strFilePath;
// 	}
	if (dlgFile.DoModal() == IDOK)
	{
		strFilePath = dlgFile.GetPathName();
		if (!strFilePath.IsEmpty() && !PathFileExists(strFilePath))
		{
			CString sMsg;
			sMsg.Format("文件 %s 不存在", strFilePath);
			MessageBox(sMsg, g_lpszAppTitle, MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			m_wndPath.SetWindowText(strFilePath);
		}
	}
	delete[] dlgFile.m_ofn.lpstrFile;     //最后别忘了释放内存
}


void CDlgStructTreeFileParamEditor::OnClose()
{
	EndDialog(FALSE); return;
	//
	CDialogEx::OnClose();
}