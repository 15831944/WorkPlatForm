#include "StdAfx.h"
#include "PeraProcessDesignerUtility.h"
#include "CxViewMgr.h"
#include "PeraProcessDesigner.h"
#include "DlgDistribution.h"
#include "CStringChecker.h"
#include "ConvertData.h"
#include "LoginFunc.h"

#include <tlhelp32.h>
#include "Tools.h"
#include "GetGlobalDataFromReg.h"
#include "AppFunc.h"
#include "PtrVectorHeader.h"

#include "MainFrm.h"

CDlgSvg * GetSvgDilogPtr (void)
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleModel );
	if ( pView == NULL ) return NULL;
	CDlgSvg * pSvg = reinterpret_cast< CDlgSvg * >( pView->m_dwExData );
	if ( pSvg == NULL ) return NULL;
	if ( !::IsWindow( pSvg->GetSafeHwnd() ) ) return NULL;

	return pSvg;
}

CDlgJobMgr * GetJobMgrDilogPtr (void)
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleJobMgr );
	if ( pView == NULL ) return NULL;
	CDlgJobMgr * pDlg = reinterpret_cast< CDlgJobMgr * >( pView->m_dwExData );
	if ( pDlg == NULL ) return NULL;
	if ( !::IsWindow( pDlg->GetSafeHwnd() ) ) return NULL;

	return pDlg;
}

CDlgDistribution * GetDistributionDilogPtr (void)
{
	VIEW_DATA * pView = theViewMgr.Find( g_lpszViewTitleDistribution );
	if ( pView == NULL ) return NULL;
	CDlgDistribution * pDlg = reinterpret_cast< CDlgDistribution * >( pView->m_dwExData );
	if ( pDlg == NULL ) return NULL;
	if ( !::IsWindow( pDlg->GetSafeHwnd() ) ) return NULL;

	return pDlg;
}

CString GetSvgDefaultPage( void )
{
	CString strPath = GetExecDir() + "\\Res\\SvgDefaultPage\\code.html";
	return strPath;
}

BOOL GetStructTreeCurSel( CCxStructTreeRowT ** ppSelRow, CCxStructTreeNode ** ppSelNode )
{
	if ( ppSelRow != NULL ) *ppSelRow = NULL;
	if ( ppSelNode != NULL ) *ppSelNode = NULL;

	if ( theApp.m_pBarJianmoTree == NULL ) return FALSE;
	if ( !::IsWindow( theApp.m_pBarJianmoTree->m_wndGridTree.GetSafeHwnd() ) ) return FALSE;
	CCxStructTreeGridT * pGrid = &theApp.m_pBarJianmoTree->m_wndGridTree;

	*ppSelRow = (CCxStructTreeRowT*)pGrid->GetCurSel();
	if ( *ppSelRow == NULL ) return TRUE;

	*ppSelNode = reinterpret_cast< CCxStructTreeNode* > ( (*ppSelRow)->GetData() );
	if ( (*ppSelNode) == NULL ) return TRUE;

	if ( !(*ppSelNode)->IsRowMatched() )
	{
		*ppSelNode = NULL;
		*ppSelRow = NULL;
		return FALSE;
	}
	return TRUE;
}

/*
ģ��������¼���
������
	ptDest��Ŀ�괰���ϣ�����������꣨0,0��ӷǿͻ������ϽǼ��㣩
	hwndDest��Ŀ�괰��
	ptSrc��������ڵ�ԭʼ�����ϣ����λ�ã�0,0��ӷǿͻ������ϽǼ��㣩
	hwndSrc��������ڵ�ԭʼ����
*/
void SetMouseClick( CPoint ptDest, HWND hwndDest, CPoint ptSrc, HWND hwndSrc )
{
	if ( hwndDest != NULL && !IsWindow(hwndDest) ) return;
	if ( hwndSrc != NULL && !IsWindow(hwndSrc) ) return;

	if ( hwndSrc == NULL && ptSrc == CPoint(-1, -1) )
	{
		::GetCursorPos( &ptSrc );
	}

	CPoint ptSrcScreen = ptSrc;
	CPoint ptDestScreen = ptDest;

	WINDOWINFO wiSrc, wiDest;
	memset(&wiSrc, NULL, sizeof(WINDOWINFO));
	wiSrc.cbSize = sizeof(WINDOWINFO);
	memset(&wiDest, NULL, sizeof(WINDOWINFO));
	wiDest.cbSize = sizeof(WINDOWINFO);

	BOOL bSrcClient, bSrcWnd, bDestClient, bDestWnd;
	bSrcClient = bSrcWnd = bDestClient = bDestWnd = FALSE;

	if ( hwndSrc != NULL )
	{
		CWnd::FromHandle( hwndSrc )->GetWindowInfo( &wiSrc );
		::ClientToScreen( hwndSrc, &ptSrcScreen );

		if ( ptSrcScreen.x > wiSrc.rcWindow.left && ptSrcScreen.x < wiSrc.rcWindow.right 
			&& ptSrcScreen.y > wiSrc.rcWindow.top && ptSrcScreen.y < wiSrc.rcWindow.bottom )
		{
			bSrcWnd = TRUE;
		}

		if ( ptSrcScreen.x > wiSrc.rcClient.left && ptSrcScreen.x < wiSrc.rcClient.right 
			&& ptSrcScreen.y > wiSrc.rcClient.top && ptSrcScreen.y < wiSrc.rcClient.bottom )
		{
			bSrcClient = TRUE;
		}
	}
		
	if ( hwndDest != NULL )
	{
		CWnd::FromHandle( hwndDest )->GetWindowInfo( &wiDest );
		::ClientToScreen( hwndDest, &ptDestScreen );

		if ( ptDestScreen.x > wiSrc.rcWindow.left && ptDestScreen.x < wiSrc.rcWindow.right 
			&& ptDestScreen.y > wiSrc.rcWindow.top && ptDestScreen.y < wiSrc.rcWindow.bottom )
		{
			bDestWnd = TRUE;
		}

		if ( ptDestScreen.x > wiSrc.rcClient.left && ptDestScreen.x < wiSrc.rcClient.right 
			&& ptDestScreen.y > wiSrc.rcClient.top && ptDestScreen.y < wiSrc.rcClient.bottom )
		{
			bDestClient = TRUE;
		}
	}

	::SetCursorPos( ptDestScreen.x, ptDestScreen.y );
	Sleep(0);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	Sleep(0);
	::SetCursorPos( ptSrcScreen.x, ptSrcScreen.y );
	Sleep(0);
}

CString ConvertRunFlowStatus( LPCTSTR lpszCode )
{
	CString strCode = lpszCode == NULL ? "" : lpszCode;
	if ( strCode.IsEmpty() ) return "";

	if ( strCode == "2" ) return "����ͣ";
	else if ( strCode == "4" ) return "��ֹͣ";
	else if ( strCode == "8" ) return "�����";
	else if ( strCode == "1" ) return "ִ����";
	else if ( strCode == "0" ) return "δ����";
	else return "δ֪״̬";
}

/*
��doubleֵͳһת��Ϊ����Ϊ2�ĸ��������ݡ�
*/
CString ConvertDoubleValue( LPCTSTR lpszSrc )
{
	CString strSrc = lpszSrc == NULL ? "" : lpszSrc;
	CString strResult;

	if ( strSrc.IsEmpty() ) return "0.0";
	if ( !CStringChecker::CheckDouble( strSrc ) ) return "";

	double dValue = strtod( (char*)(LPCTSTR)strSrc, NULL );

	//С����λ��
	int nDotPos = strSrc.Find('.', 0);
	if ( nDotPos == -1 ) //û��С����
	{
		strResult.Format("%.2f", dValue);
		return strResult;
	}

	//�ַ����ܳ���
	int nTotalLen = strSrc.GetLength();
	//С�������ַ�����
	int nDecimalCount = nTotalLen - ( nDotPos + 1 );

	//С����������ݲ���2λ������2λ
	if ( nDecimalCount < 2 )
	{
		strResult.Format("%.2f", dValue);
		return strResult;
	}
	//С����������ݳ���2λ������Ƿ�ȫΪ0������ǣ��ض϶����0
	else if ( nDecimalCount > 2 )
	{
		BOOL bZeroAll = TRUE; //�Ƿ�Ϊȫ0
		for ( int i=nDotPos+1; i<nTotalLen; i++ )
		{
			if ( strSrc.GetAt(i) != '0' )
			{
				bZeroAll = FALSE;
				break;
			}
		}
		if ( bZeroAll )
		{
			strResult.Format("%.2f", dValue);
			return strResult;
		}
	}

	return strSrc;
}

BOOL ConvertArrayDim( vector<int>& vPos, LPCTSTR lpszDimStr )
{
	vPos.clear();
	CString strDim = lpszDimStr == NULL ? "" : lpszDimStr;
	if ( strDim.IsEmpty() ) return FALSE;
	vector<CString> vStr;
	int nCount = CString2Vector( vStr, (LPCTSTR)strDim );
	if ( nCount <= 0 ) return FALSE;
	for (int i=0; i<nCount; i++)
	{
		if ( vStr[i].Find( '*' ) != -1 )
		{
			vPos.push_back(-1);
			continue;
		}

		int nDim = strtol( (char*)(LPCTSTR)vStr[i], NULL, 10 );
		vPos.push_back( nDim );
	}
	return TRUE;
}
BOOL LoginOut()
{
	vector<CView*> vViews;
	for (int i=0; i<(int)theViewMgr.m_vViews.size(); i++)
	{
		if ( theViewMgr.m_vViews[i]->m_strTitle.CompareNoCase((LPCTSTR)g_lpszViewTitleModel) != 0 )
		{
			vViews.push_back(theViewMgr.m_vViews[i]->m_pView);
		}
	}
	if (vViews.size() == 0)
	{
		int nRet = ::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "����ע����ǰ�û����Ƿ������", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
		if (IDOK == nRet)
		{
			Logout(theApp.m_LoginData.m_strTicket);
			theApp.m_LoginData.Clear();
		}
	}
	else
	{
		int nRet = ::MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), "����ע����ǰ�û�������رճ���ģ���������ҳ�棬�Ƿ������", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
		switch (nRet)
		{
		case IDOK:
			{
				for (int i = 0; i < (int)vViews.size(); i++)
				{
					vViews[i]->SendMessage(WM_COMMAND, ID_FILE_CLOSE);
				}
				Logout(theApp.m_LoginData.m_strTicket);
				theApp.m_LoginData.Clear();
			}
			break;
		case IDCANCEL:
		default:
			break;
		}
	}

	return TRUE;
}

static CString GetLocalIP()
{
	//return "192.168.114.156";
	CString sHostName,sIPAddress;
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err;
	char szHostName[256];   
	int  nRetCode;   

	wVersionRequested = MAKEWORD( 2, 0 );    
	err = WSAStartup( wVersionRequested, &wsaData );   
	if ( err != 0 ) 
	{    
		return "";   
	}    

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
	{   
		WSACleanup( );   
		return "";    
	}

	nRetCode = gethostname(szHostName, sizeof(szHostName));   

	if (nRetCode != 0) 
	{   
		sHostName = (""); 
		return "";   
	}   

	sHostName = szHostName;

	struct hostent FAR *lpHostEnt = gethostbyname (sHostName);   

	if (lpHostEnt == NULL) 
	{   
		sIPAddress = _T("");
		return sIPAddress;
	}   

	LPSTR lpAddr = lpHostEnt->h_addr_list[0];   
	if (lpAddr) 
	{   
		struct in_addr  inAddr;   
		memmove (&inAddr, lpAddr, 4);   
		sIPAddress = inet_ntoa (inAddr);   
		if (sIPAddress.IsEmpty())   
			sIPAddress = _T("");   
	}

	nRetCode = WSACleanup();  

	if (nRetCode != 0) 
	{ 
		return "";   
	}   

	return sIPAddress;   
}

#define SM_LOCAL_IP_LENGTH 256
CString GetLocalIpFromIce()
{
	CString sIp("");
	CSharedMemory* pSmIp = NULL;
	pSmIp = new CSharedMemory("SM_LOCAL_IP", SM_LOCAL_IP_LENGTH);
	pSmIp->Lock();
	sIp = (char*)pSmIp->GetData();
	pSmIp->Unlock();
	delete pSmIp;
	pSmIp = NULL;
	
	if (sIp.IsEmpty())
	{
		sIp = GetLocalIP();
	}

	return sIp;
}

BOOL FindProcessByName(LPCTSTR szFileName)
{
	// ���ý��̿���ö�ٽ��̵ķ�������ָ�����ƽ���
	HANDLE hProcesses;
	PROCESSENTRY32 lpe =
	{
		sizeof(PROCESSENTRY32)
	};
	CString strFileName(szFileName);
	strFileName.MakeLower();

	// �������̿���
	hProcesses = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcesses == INVALID_HANDLE_VALUE)
		return FALSE;
	// ��ȡ��һ������ʵ��
	BOOL isExist = ::Process32First(hProcesses, &lpe);
	BOOL isRunning = FALSE;
	CString strName;
	while (isExist)
	{
		strName = lpe.szExeFile;
		strName.MakeLower();
		if (-1 != strFileName.Find(strName))
		{
			isRunning = TRUE;
			break;
		}
		// ������һ������ʵ��
		isExist = ::Process32Next(hProcesses, &lpe);
	}

	CloseHandle(hProcesses);
	return isRunning;
}

void StartPeraTaskService()
{
	if (!FindProcessByName("PeraTaskService.exe"))
	{
		ZTools::WriteZToolsFormatLog("����PeraTaskService���̣�δ�ҵ�����������");

		CString sDependsPath = GetPeraGlobalDataFromReg( "lm", "Depends" );
		if (sDependsPath.IsEmpty())
		{
			ZTools::WriteZToolsFormatLog("��ע����в���·��ʧ��");
			return;
		}

		CString sExePath = sDependsPath + "\\PeraTaskService.exe";

		if (!PathFileExists(sExePath))
		{
			ZTools::WriteZToolsFormatLog("PeraTaskService.exe·�������ڣ�%s", sExePath);
			return;
		}

		SHELLEXECUTEINFO si;
		memset(&si, NULL, sizeof(si));
		si.cbSize = sizeof(si);
		si.nShow = SW_HIDE;
		si.fMask = SEE_MASK_NOCLOSEPROCESS;
		si.lpVerb = _T("open");
		si.lpDirectory = sDependsPath;
		si.lpFile = sExePath;
		si.lpParameters = NULL;
		if (!ShellExecuteEx(&si))
		{
			ZTools::WriteZToolsFormatLog("����PeraTaskService���̳����쳣");
			return;
		}

		DWORD dwPid = GetProcessId(si.hProcess);
		CloseHandle(si.hProcess);
		ZTools::WriteZToolsFormatLog("����PeraTaskService���̳ɹ�,pid:%d", dwPid);
	}
}

HWND FindPeraProcessDesignerMainWindow( void )
{
	CString strProcessName = "PeraProcessDesigner.exe";

	vector<CString> vCaption;
	vCaption.push_back( g_lpszAppTitle );

	DWORD dwCurProcessId = GetCurrentProcessId();
	DWORD dwProcessIdOther = 0;

	vector<PROCESSENTRY32 *> vProcess;
	int nCnt = GetProcessEntry32( vProcess, (LPCTSTR)strProcessName );
	if ( nCnt > 1 )
	{
		for (int i=0; i<nCnt; i++)
		{
			if ( vProcess[i]->th32ProcessID == dwCurProcessId )
			{
				continue;
			}
			dwProcessIdOther = vProcess[i]->th32ProcessID;
			break;
		}
		PtrVectorClear( vProcess );
	}
	else
	{
		PtrVectorClear( vProcess );
		return NULL;
	}

	if ( dwProcessIdOther == 0 ) return NULL;


	HWND hwndOther = GetMainWindow( dwProcessIdOther, vCaption );
	return hwndOther;
}

#define SM_WORKSPACE_LOGIN_INFO_LENGTH 300
static CSharedMemory *g_pSmLoginInfo = NULL;
void WriteShareMemoryLoginInfo()
{
	CString strLoginInfo;
	strLoginInfo.Format("{ \"userName\":\"%s\",\"realName\":\"%s\"}",theApp.m_LoginData.m_strUser, theApp.m_LoginData.m_strRealName);

	if (strLoginInfo.GetLength() >= SM_WORKSPACE_LOGIN_INFO_LENGTH)
		strLoginInfo.Left(SM_WORKSPACE_LOGIN_INFO_LENGTH - 1);

	if (g_pSmLoginInfo==NULL)
		g_pSmLoginInfo = new CSharedMemory("SM_WORKSPACE_LOGIN_INFO", SM_WORKSPACE_LOGIN_INFO_LENGTH);
		
	g_pSmLoginInfo->Lock();
	memset(g_pSmLoginInfo->GetData(), 0, SM_WORKSPACE_LOGIN_INFO_LENGTH);
	memcpy(g_pSmLoginInfo->GetData(), (LPCTSTR)strLoginInfo, strLoginInfo.GetLength() + 1);
	g_pSmLoginInfo->Unlock();
}
void DeleteShareMemoryLoginInfo()
{
	if (g_pSmLoginInfo)
	{
		delete g_pSmLoginInfo;
		g_pSmLoginInfo = NULL;
	}
}

BOOL ShowLoading()
{
	CMainFrame* pFrame = (CMainFrame*)theApp.m_pMainWnd;
	if ( pFrame != NULL && pFrame->GetSafeHwnd() != NULL )
	{
		pFrame->SendMessage( WM_PROGRESS_SHOW, (WPARAM)TRUE );
// 		if ( IsWindow( g_hwndWebPage ) )
// 		{
// 			//CWnd::FromHandle( g_hwndWebPage )->/*GetParent()->*/ModifyStyle( 0,WS_DISABLED );
// 			CWnd::FromHandle( g_hwndWebPage )->SetRedraw( FALSE );
// 		}
// 		CRunTree* RunTreePtr = GetRunTreePtr();
// 		if ( IsWindow( RunTreePtr->GetSafeHwnd() ) )
// 		{
// 			RunTreePtr->SetRedraw( FALSE );
// 		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL HideLoading()
{
	CMainFrame* pFrame = (CMainFrame*)theApp.m_pMainWnd;
	if ( pFrame != NULL && pFrame->GetSafeHwnd() != NULL )
	{
		pFrame->SendMessage( WM_PROGRESS_SHOW, (WPARAM)FALSE );
// 		if ( IsWindow( g_hwndWebPage ) )
// 		{
// 			CWnd::FromHandle( g_hwndWebPage )->SetRedraw( TRUE );
// 			CWnd::FromHandle( g_hwndWebPage )->RedrawWindow();
// 			CRect rcClient;
// 			CWnd::FromHandle( g_hwndWebPage )->GetClientRect( rcClient );
// 			CWnd::FromHandle( g_hwndWebPage )->MoveWindow( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height()-1 );
// 			CWnd::FromHandle( g_hwndWebPage )->MoveWindow( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height() );
// 		}
// 		CRunTree* RunTreePtr = GetRunTreePtr();
// 		if ( IsWindow( RunTreePtr->GetSafeHwnd() ) )
// 		{
// 			RunTreePtr->SetRedraw( TRUE );
// 			RunTreePtr->RedrawWindow();
// 		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


HWND GetRunnerHandle()
{
	vector< HWND > vWSRunner;
	vector< CString > vCaption;
	vCaption.push_back( g_lpszWSRunnerFindCaption );
	int nCount = GetMainWindow( vWSRunner, g_lpszWSRunnerProcessName, vCaption );
	if ( 0 <  nCount )
	{	
		if ( nCount > 1 )
		{
			ZTools::WriteZToolsFormatLog( "%s ���̴��ڶ��[%d]", g_lpszWSRunnerProcessName, nCount );
		}
		vector< HWND >::iterator itRunner = vWSRunner.begin();
		if ( itRunner == vWSRunner.end() )
		{
			return NULL;
		}
		return *itRunner;
	}
	return NULL;
}