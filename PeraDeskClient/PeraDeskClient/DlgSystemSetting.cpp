// DlgSystemSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraDeskClient.h"
#include "DlgSystemSetting.h"
#include "afxdialogex.h"
#include "PeraDeskClientUtility.h"
#include "InvokeMethod.h"
#include "CxNetDiskSettingData.h"
#include "CStringChecker.h"
#include "Tools.h"
#include <imagehlp.h>
// CDlgSystemSetting 对话框

BEGIN_MESSAGE_MAP(CDlgSystemSetting, CCxBCGPDialog)
	ON_BN_CLICKED(ID_SETTING_OK, &CDlgSystemSetting::OnBnClickedOk)
	ON_BN_CLICKED(ID_SETTING_CACNEL, &CDlgSystemSetting::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SEL_DIR, &CDlgSystemSetting::OnBnClickedBtnSelDir)
END_MESSAGE_MAP()

void CDlgSystemSetting::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, ID_SETTING_OK, m_wndOk);
	//DDX_Control(pDX, ID_SETTING_CACNEL, m_wndCancel);
	DDX_Text(pDX, IDC_EDIT_CACHE_DIR, m_strEditCacheDir);
	//DDX_Control(pDX, IDC_BTN_SEL_DIR, m_wndCacheDirSel);
	//DDX_Control(pDX, IDC_EDIT_CACHE_DIR, m_EditCtrlCacheDir);
}

IMPLEMENT_DYNAMIC(CDlgSystemSetting, CCxBCGPDialog)

CDlgSystemSetting::CDlgSystemSetting(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgSystemSetting::IDD, pParent)
	, m_strEditCacheDir(_T(""))
{

}

CDlgSystemSetting::~CDlgSystemSetting()
{
}

BOOL CDlgSystemSetting::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),TRUE);

	if ( !CanInvokeMethod() )
	{
		MessageBox( "网盘进程没有运行，无法获取或设置参数", g_lpszAppTitle );
		PostMessage(WM_CLOSE);
	}
	else
	{
		string sResult;
		if ( InvokeMethod( sResult, "GetSettings", "" ) )
		{
			CCxNetDiskSettingData data;
			if ( data.Parse( sResult.c_str() ) )
			{
				m_strEditCacheDir = data.m_sCacheDir;
				m_strOldCacheDir = data.m_sCacheDir;
				UpdateData(FALSE);
			}
		}
	}

	return TRUE;
}

void CDlgSystemSetting::OnBnClickedOk()
{
	UpdateData(TRUE);
	if ( !CanInvokeMethod() )
	{
		MessageBox( "网盘进程没有运行，无法获取或设置参数", g_lpszAppTitle );
		return;
	}
	else
	{
		if (m_strOldCacheDir.CompareNoCase(m_strEditCacheDir)!=0)
		{
			if (!IsCacheDirEmpty(m_strOldCacheDir))
			{
				if(IDNO==MessageBox( "修改缓存路径可能导致正在运行的任务无法找到文件,是否继续？", g_lpszAppTitle,MB_YESNO|MB_ICONQUESTION))
					return;
			}
		}

		m_strEditCacheDir.Replace("/","\\");
		m_strEditCacheDir.Replace("\\\\","\\");
		if (m_strEditCacheDir.Right(1)!='\\')
			m_strEditCacheDir.AppendChar('\\');

		if (!MakeSureDirectoryPathExists(m_strEditCacheDir))
		{
			CString str;
			str.Format("参数设置失败,指定的目录不存在,且不能创建,请检测路径是否合法！");
			MessageBox(str, g_lpszAppTitle,MB_OK|MB_ICONERROR);
			return;
		}
		
		CCxNetDiskSettingData data;
		data.m_sCacheDir = m_strEditCacheDir;

		string sResult;
		CString strPsRes;
		BOOL bok = InvokeMethod( sResult, "SetSettings", data.GetJsonStr_SetSetting() ) ;
		if ( bok && ParseResult(sResult.c_str(),strPsRes))
		{
			MessageBox( "参数设置成功!", g_lpszAppTitle,MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			CString str = strPsRes;
			if (str.IsEmpty()) str = "参数设置失败！";
			MessageBox(str, g_lpszAppTitle,MB_OK|MB_ICONERROR);
			return;
		}
	}

	CCxBCGPDialog::OnOK();
}


void CDlgSystemSetting::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CCxBCGPDialog::OnCancel();
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData); //初始化打开上次目录
		HWND hFind = ::GetWindow(hwnd,GW_CHILD);
		while(hFind)
		{
			char szName[256] ={0};
			GetClassName(hFind,szName,256);
			if (_stricmp(szName,"Edit")==0)
			{
				CEdit *pEdit = (CEdit*)CWnd::FromHandle(hFind);
				pEdit->SetReadOnly(TRUE); //将控件设置为只读，方便复制，但不允许修改
				break;
			}
			hFind = ::GetWindow(hFind,GW_HWNDNEXT);
		}

		break;
	}
	return 0;
}

void CDlgSystemSetting::OnBnClickedBtnSelDir()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;  
	ZeroMemory(&bi,sizeof(BROWSEINFO)); 
	bi.hwndOwner = this->GetSafeHwnd();   
	bi.lpszTitle = "请选择缓存目录：";  

	bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)(LPCTSTR)m_strEditCacheDir;

	LPMALLOC pMalloc; 
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);  
	if(!pidl ) return ;

	CString strFolderOut = "";
	SHGetPathFromIDList(pidl,strFolderOut.GetBuffer(512));  
	if(SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{  
		pMalloc->Free(pidl);  
		pMalloc->Release();  
	}

	strFolderOut.ReleaseBuffer();
	if (strFolderOut.IsEmpty())
		return;

	m_strEditCacheDir = strFolderOut;
	UpdateData(FALSE);
}
BOOL CDlgSystemSetting::IsCacheDirEmpty(const CString & strDir)
{
	if (strDir.IsEmpty())
		return TRUE;

	CString strFinder = strDir;
	if(strFinder.Right(1) != "\\")
		strFinder += "\\";
	strFinder += "*.*";

	CFileFind finder;
	BOOL bContinue = finder.FindFile(strFinder);
	while(bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots()) continue;

		if(finder.IsDirectory())
		{
			CString strChild = finder.GetFilePath();
			BOOL IsEmpty = IsCacheDirEmpty(strChild);
			if (!IsEmpty)
				return FALSE;
		}
		else
			return FALSE; // 找到文件，则目录非空
			
	}

	return TRUE;
}