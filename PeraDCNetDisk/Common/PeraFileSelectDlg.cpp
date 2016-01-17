// PeraFileSelectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraFileSelectDlg.h"
#include <dlgs.h>
#include <map>
#include <atlbase.h>			 // for CComQIPtr smart pointers
#include "Tools.h"
using namespace std;
using namespace ATL;

const int SELECT_MAX_FILE_NUMBER = 500; 
// CFileSelectDlg 对话框
WNDPROC CFileSelectDlg::m_wndProc = NULL;
CString CFileSelectDlg::m_strCurrendDirectory;
CStringArray CFileSelectDlg::m_SelectedItemList;

IMPLEMENT_DYNAMIC(CFileSelectDlg, CFileDialog)

	CFileSelectDlg::CFileSelectDlg(BOOL bOpenFileDialog,
	LPCTSTR lpszDefExt /*NULL*/,
	LPCTSTR lpszFileName /*NULL*/,
	DWORD dwFlags /* OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/,
	LPCTSTR lpszFilter /*NULL*/,
	CWnd* pParentWnd /*NULL*/,
	DWORD dwSize /*0*/,
	BOOL bVistaStyle /*TRUE*/)
	: CFileDialog(bOpenFileDialog,lpszDefExt,lpszFileName,dwFlags|OFN_ENABLEINCLUDENOTIFY|OFN_EXPLORER | OFN_HIDEREADONLY & (~OFN_SHOWHELP),lpszFilter,pParentWnd,dwSize,bVistaStyle)
{
   // m_ofn.lpTemplateName=MAKEINTRESOURCE(CFileSelectDlg::IDD);
	dwFlags |= (OFN_EXPLORER | OFN_HIDEREADONLY & (~OFN_SHOWHELP));
	m_strSelectListTemp.RemoveAll();
}

CFileSelectDlg::~CFileSelectDlg()
{
}
map<CString,CString> GetSystemDrives()
{
	char  szDrives[512];
	char* pDrive;

	map<CString,CString> vDriverMap;
	if ( !GetLogicalDriveStrings( sizeof(szDrives), szDrives ) )
		return vDriverMap;

	pDrive = szDrives;
	while( *pDrive )
	{
		CString strDrive = pDrive;
		SHFILEINFO    sfi;
		SHGetFileInfo((LPCTSTR)strDrive,0,&sfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
		CString strDisplayName = sfi.szDisplayName;
		vDriverMap.insert(make_pair(strDisplayName,strDrive));
		pDrive += strlen( pDrive ) + 1;
	}
	return vDriverMap;
}

//void CFileSelectDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CFileDialog::DoDataExchange(pDX);
//}


BEGIN_MESSAGE_MAP(CFileSelectDlg, CFileDialog)
END_MESSAGE_MAP()


// CFileSelectDlg 消息处理程序
void CFileSelectDlg::OnInitDone()
{
	// TODO: 在此添加专用代码和/或调用基类
	CFileDialog::OnInitDone();
	CWnd* pWnd=FromHandle(::GetParent(m_hWnd))->GetDlgItem(IDOK);
	ASSERT(pWnd);
	pWnd->SetWindowText(_T("上传"));
	CWnd *Parent = this->GetParent(); 
	Parent->SendMessage(CDM_SETCONTROLTEXT, stc3, (LPARAM)_T("名称："));
	Parent->SendMessage(CDM_SETCONTROLTEXT, stc2, (LPARAM)_T("类型："));

	m_wndProc = (WNDPROC)::SetWindowLong(::GetParent(m_hWnd), GWL_WNDPROC, (long)WindowProcNew);
}
void CFileSelectDlg::OnFolderChange()
{
	m_strCurrendDirectory = GetFolderPath();
	m_strSelectListTemp.RemoveAll();
	CFileSelectDlg::m_SelectedItemList.RemoveAll();
	CListCtrl *pLsit = GetListCtrl();
	if (pLsit) // 切换目录后清空选择
	{
		CString strEmpty;
		GetParent()->SendMessage(CDM_SETCONTROLTEXT, edt1, (LPARAM)strEmpty.GetBuffer());
		strEmpty.ReleaseBuffer();

		POSITION pos = pLsit->GetFirstSelectedItemPosition();
		while(pos)
		{
			int iSelID  = pLsit->GetNextSelectedItem(pos);
			pLsit->SetItemState(iSelID,0,LVIS_SELECTED);
		}
		pLsit->SetSelectionMark(-1);
		//pLsit->SetFocus();
	}
	CFileDialog::OnFolderChange();
}
int CFileSelectDlg::GetSelectCount()
{
	CListCtrl *pList = GetListCtrl();
	if (pList==NULL)
		return 0;

	ASSERT(pList!=NULL);
	return pList->GetSelectedCount();
}
CStringArray & CFileSelectDlg::GetSelectList()
{
	return m_strSelectListNow;
}
CListCtrl* CFileSelectDlg::GetListCtrl()
{
	CWnd* pWnd = GetParent()->GetDlgItem(lst2);	//getting list
	if (pWnd == NULL)
		return NULL;

	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
	return wndLst1;
}
BOOL CFileSelectDlg::OnNotify(WPARAM wp, LPARAM lp, LRESULT* pRes)
{
	OFNOTIFYEX& of = *(OFNOTIFYEX*)lp;
	switch(of.hdr.code) {
	case CDN_INCLUDEITEM:
		{
			CString strItemPath;
			IShellFolder *ish = (IShellFolder*)of.psf;
			LPCITEMIDLIST pidl = (LPCITEMIDLIST)of.pidl;
			STRRET str;
			if (NO_ERROR==ish->GetDisplayNameOf(pidl, SHGDN_NORMAL|SHGDN_FORPARSING, &str))
			{
				strItemPath = str.pOleStr;
			   m_strSelectListTemp.Add(strItemPath);
			}
			return TRUE;
		}
	}
	return CFileDialog::OnNotify(wp, lp, pRes);
}
void CFileSelectDlg::OnFileNameChange()
{
	int iSelCount = GetSelectCount();
	ZTools::WriteZToolsFormatLog("OnFileNameChange 开始进入,当前路径【%s】,GetSelectCount返回%d",m_strCurrendDirectory,iSelCount);
	if(iSelCount<=0)
	{
		m_strSelectListTemp.RemoveAll();
		m_strSelectListNow.RemoveAll();
		CFileSelectDlg::m_SelectedItemList.RemoveAll();
		return;
	}

	int iNotifyCount = m_strSelectListTemp.GetCount();
	if (iNotifyCount != iSelCount) 
	{
		// 1. xp下选择磁盘时,notify选择的个数和树上选择的个数不一致
		// 2. win7下会出现iNotifyCount>iSelCount
		// 3. xp下notify过滤了文件夹，所以以list控件上选择的为标准
		BOOL bSingleFileSuccess = FALSE;
		m_strSelectListTemp.RemoveAll();
		if (iSelCount==1)
		{
			CString fileslist = _T("");
			GetParent()->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH * SELECT_MAX_FILE_NUMBER,
				(LPARAM)fileslist.GetBuffer(MAX_PATH * SELECT_MAX_FILE_NUMBER));
			fileslist.ReleaseBuffer();
			if (!fileslist.IsEmpty())
			{
				CString strItemText = m_strCurrendDirectory + CString("\\")  + fileslist;
				if (PathFileExists(strItemText))
				{
					bSingleFileSuccess = TRUE;
					m_strSelectListTemp.Add(strItemText);
					ZTools::WriteZToolsFormatLog("单个文件选择成功【%s】",strItemText);
				}
			}
		}

		if (bSingleFileSuccess == FALSE)
		{
			CListCtrl * wndLst1 = GetListCtrl();
			int nItem = wndLst1->GetNextItem(-1,LVNI_SELECTED);
			map<CString,CString> vDriverMap =  GetSystemDrives();
			for (int i=0;i<iSelCount;i++)
			{
				CString  strItemText;
				if (m_strCurrendDirectory.IsEmpty())
					strItemText = wndLst1->GetItemText(nItem,0);
				else
					strItemText = m_strCurrendDirectory + CString("\\") +  wndLst1->GetItemText(nItem,0);

				ZTools::WriteZToolsFormatLog("GetItemText =【%s】",wndLst1->GetItemText(nItem,0));
				ZTools::WriteZToolsFormatLog("拼接路径【%s】",strItemText);

				LPCITEMIDLIST li = (LPCITEMIDLIST)wndLst1->GetItemData(nItem);
				if (li!=NULL)
				{
					CString strTmp = GetDisplayNameOf(li,SHGDN_NORMAL|SHGDN_FORPARSING); // xp下GetItemData返回非空，win7下不会走本行代码
					if (!strTmp.IsEmpty())
						strItemText = strTmp;

					ZTools::WriteZToolsFormatLog("GetDisplayNameOf路径【%s】",strItemText);
				}

				if (PathFileExists(strItemText))
					m_strSelectListTemp.Add(strItemText);
				else 
				{
					ZTools::WriteZToolsFormatLog("PathFileExists判断路径【%s】不存在",strItemText);
					map<CString,CString>::iterator iter = vDriverMap.find(strItemText);
					if (iter!=vDriverMap.end())
					{
						CString strValue = iter->second;
						m_strSelectListTemp.Add(strValue);
					}
					else
					{
						ZTools::WriteZToolsFormatLog("磁盘列表判断路径【%s】也不存在",strItemText);
					}
				}
				nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED);
			}
		}
		

		ZTools::WriteZToolsFormatLog("Notify选择的个数和树上选择的个数不一致！");
	}

	m_strSelectListNow.RemoveAll();
	m_strSelectListNow.Append(m_strSelectListTemp);
	m_strSelectListTemp.RemoveAll();
	CFileSelectDlg::m_SelectedItemList.RemoveAll();
	CString strEditText;
	for (int i=0;i<m_strSelectListNow.GetCount();i++)
	{
		if (PathFileExists(m_strSelectListNow[i]))
		{
			CFileSelectDlg::m_SelectedItemList.Add(m_strSelectListNow[i]);
			CString strName = PathFindFileName(m_strSelectListNow[i]);
			strEditText += CString("\"") + strName + CString("\"");
			strEditText += " ";
		}
		else
		{
			ZTools::WriteZToolsFormatLog("【%s】不存在",m_strSelectListNow[i]);
		}
	}
	strEditText.TrimRight(" ");
	if (m_strSelectListNow.GetCount()==1) // 选择一个时不加引号
	{
		strEditText.TrimLeft("\"");
		strEditText.TrimRight("\"");
	}

	ZTools::WriteZToolsFormatLog("最终选择%d个文件",m_strSelectListNow.GetCount());

	GetParent()->SendMessage(CDM_SETCONTROLTEXT, edt1, (LPARAM)strEditText.GetBuffer());
	strEditText.ReleaseBuffer();

	CString strFile = GetPathName();
	if (!strFile.IsEmpty() && m_strCurrendDirectory.IsEmpty())
	{
		TCHAR* pName = PathFindFileName(strFile); 
		CString strPath = strFile.Left(strFile.Find(pName));
		m_strCurrendDirectory = strPath;
	}
	CFileDialog::OnFileNameChange();
}
LRESULT CALLBACK CFileSelectDlg::WindowProcNew(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message ==  WM_COMMAND)
	{
		if (HIWORD(wParam) == BN_CLICKED)
		{
			if (LOWORD(wParam) == IDOK)
			{
				if (CFileDialog* pDlg = (CFileDialog*)CWnd::FromHandle(hwnd))
				{
				
					CWnd* pWnd = pDlg->GetDlgItem(lst2);	//getting list
					if (pWnd == NULL)
					{
						pDlg->EndDialog(IDOK);
						return S_OK;
					}

					CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
					if (wndLst1 == NULL)
					{
						pDlg->EndDialog(IDOK);
						return S_OK;
					}

					int nSelected = wndLst1->GetSelectedCount();
					if (nSelected >0)
					{
			
						pDlg->EndDialog(IDOK);
						return S_OK;
					}

					CString fileslist = _T("");
					pDlg->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH * SELECT_MAX_FILE_NUMBER,
						(LPARAM)fileslist.GetBuffer(MAX_PATH * SELECT_MAX_FILE_NUMBER));
					fileslist.ReleaseBuffer();
					if (fileslist.IsEmpty())
					{
						pDlg->EndDialog(IDOK);
						return S_OK;
					}

					CString strItemText, strDirectory = m_strCurrendDirectory;
					if (strDirectory.Right(1) != _T("\\"))
						strDirectory += _T("\\");

					int MoreThanOnFile = fileslist.Find(_T("\""));
					if(MoreThanOnFile != -1)
					{
						for(int i=0; i<fileslist.GetLength(); i++)
							if(fileslist[i] != '\"')
							{
								strItemText.AppendFormat(_T("%c"),fileslist[i]);
								if(fileslist[i-1] == '\"' && fileslist[i] == ' ')
									strItemText.Delete(strItemText.GetLength()-1);
							}
							else if(!strItemText.IsEmpty())
							{
								if (PathFileExists(strDirectory+strItemText))
									m_SelectedItemList.Add(strDirectory+strItemText);

								strItemText.Empty();
							}
					}
					else
					{
						if (PathFileExists(strDirectory+fileslist))
							m_SelectedItemList.Add((strDirectory+fileslist));
						else if (PathFileExists(fileslist)) // 单选文件名可能是其他盘文件的完整路径
							m_SelectedItemList.Add(fileslist);
					}


					pDlg->EndDialog(IDOK);
					return S_OK;

					//m_SelectedItemList.RemoveAll();			// emptying list
					//CWnd* pWnd = pDlg->GetDlgItem(lst2);	//getting list
					//if (pWnd == NULL)
					//	return FALSE;

					//CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));

					//int nSelected = wndLst1->GetSelectedCount();
					//if (!nSelected)		// nothing selected -- don't retrieve list
					//	return FALSE;
					//CString strItemText, strDirectory = m_strCurrendDirectory;
					//if (strDirectory.Right(1) != _T("\\"))
					//	strDirectory += _T("\\");
					//
					//int nItem = wndLst1->GetNextItem(-1,LVNI_SELECTED);
					//map<CString,CString> vDriverMap =  GetSystemDrives();
					////////////////////   Add directory names to list
					//while((nSelected--) > 0)
					//{
					//	strItemText = wndLst1->GetItemText(nItem,0);
					//	CString strListText = strItemText;
					//	strItemText = strDirectory + strItemText;
					//	DWORD attr = GetFileAttributes(strItemText);
					//	if((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY))
					//		m_SelectedItemList.Add(strItemText);
					//	else
					//	{
					//		map<CString,CString>::iterator iter = vDriverMap.find(strListText);
					//		if (iter!=vDriverMap.end())
					//		{
					//			CString strValue = iter->second;
					//			m_SelectedItemList.Add(strValue);
					//		}
					//	}
					//	nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED);
					//}
					////////////////////   Add FILE names to list
					//strItemText = _T("");
					//nSelected = wndLst1->GetSelectedCount();
					//CString fileslist = _T("");
					//pDlg->SendMessage(CDM_GETSPEC, (WPARAM)MAX_PATH * SELECT_MAX_FILE_NUMBER,
					//	(LPARAM)fileslist.GetBuffer(MAX_PATH * SELECT_MAX_FILE_NUMBER));
					//fileslist.ReleaseBuffer();

					//if(nSelected > m_SelectedItemList.GetCount())
					//{
					//	int MoreThanOnFile = fileslist.Find(_T("\""));
					//	if(MoreThanOnFile != -1)
					//	{
					//		for(int i=0; i<fileslist.GetLength(); i++)
					//			if(fileslist[i] != '\"')
					//			{
					//				strItemText.AppendFormat(_T("%c"),fileslist[i]);
					//				if(fileslist[i-1] == '\"' && fileslist[i] == ' ')
					//					strItemText.Delete(strItemText.GetLength()-1);
					//			}
					//			else if(!strItemText.IsEmpty())
					//			{
					//				m_SelectedItemList.Add((strDirectory+strItemText));
					//				strItemText.Empty();
					//			}
					//	}
					//	else
					//		m_SelectedItemList.Add(strDirectory+fileslist);
					//}
					//pDlg->EndDialog(IDOK);
					//return NULL;
				} // if IDOK
			}
		} // if BN_CLICKED
	}// if WM_COMMAND
	return CallWindowProc(m_wndProc, hwnd, message, wParam, lParam);
}
CString CFileSelectDlg::GetDisplayNameOf(LPCITEMIDLIST pidl, DWORD flags)
{
	int len = GetParent()->SendMessage(CDM_GETFOLDERIDLIST, 0,NULL);
	if (len<=0)
		return "";

	CComQIPtr<IMalloc> imalloc;
	VERIFY(SUCCEEDED(SHGetMalloc(&imalloc)));
	LPCITEMIDLIST pidlFolder = (LPCITEMIDLIST)imalloc->Alloc(len);
	ASSERT(pidlFolder);
	GetParent()->SendMessage(CDM_GETFOLDERIDLIST, len,(LPARAM)(void*)pidlFolder);

	// Now get IShellFolder for pidlFolder
	CComQIPtr<IShellFolder> ishDesk;
	SHGetDesktopFolder(&ishDesk);
	CComQIPtr<IShellFolder> ishFolder;
	HRESULT hr = ishDesk->BindToObject(pidlFolder, NULL,IID_IShellFolder, (void**)&ishFolder);
	if (!SUCCEEDED(hr))
	{
		ishFolder = ishDesk;
	}

	CString strPath;
	STRRET str;
	if (NO_ERROR==ishFolder->GetDisplayNameOf(pidl, SHGDN_NORMAL|SHGDN_FORPARSING, &str))
		strPath = str.pOleStr;

	SHFree(imalloc);
	return strPath;
}