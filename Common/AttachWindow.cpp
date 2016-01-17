
/*
完整的交互逻辑：

Host端：

创建aw对象，赋值aw.m_dwHostProcessId、aw.m_hHostWnd。

通过自定义命令行，创建Client进程。

Client端：

创建aw对象，解析命令行，得到 aw.m_dwHostProcessId、aw.m_hHostWnd；
给 aw.m_dwClientProcessId、aw.m_hClientWnd 赋值。

给自己主窗口 Post 消息 WM_AW_PROCESS_ATTACHING；
在 WM_AW_PROCESS_ATTACHING 的消息处理中，给 Host 发送分支为 m_dwResponseCopyDataId 的 Copydata 消息；

Host端：

收到分支为 WM_AW_PROCESS_ATTACHING 的 Copydata 消息后，给自己主窗口 Post 消息 WM_AW_PROCESS_ATTACHING；

在 WM_AW_PROCESS_ATTACHING 的消息处理中，解析出 aw.m_dwClientProcessId、aw.m_hClientWnd；
把 Client 主窗口吸附进来，作为子窗口；
给 Client 主窗口 Post WM_AW_PROCESS_ATTACHED 消息。

Client端：

收到 WM_AW_PROCESS_ATTACHED 后，后续的处理逻辑。

///////////////////////////////////////////////////////////

Mainframe 与 WebContainer 应用：

在 UserDefine.h 中定义如下常量：

1-1、Copydata中，Client端启动成功的分支（CAttachWindow::m_dwHostProcessId）：

#define CPMSG_AW_PROCESS_ATTACHING			7

1-2、Copydata中，WebEvent事件分支：

#define CPMSG_WEBEVENT						1

1-3、web类型标识（CAttachWindow::m_nExeFlag，CWebEventData::m_dwWebType）：

#define WEBTYPE_JOBMGR						1
#define WEBTYPE_SVG							2
#define WEBTYPE_RUNFLOW						3
#define WEBTYPE_RUNFLOW_VIEW				4
#define WEBTYPE_RUNPROCESS					5

1-4、WebContainer与Mainframe交互事件标识（CWebEventData::m_dwWebEvent）：

//RunFlow页面交互事件
#define WE_RUNFLOW_CANCEL					1
#define WE_RUNFLOW_COMPLETED				2
#define WE_RUNFLOW_SUBFLOWID				3

1-5、内嵌页面回调exe事件标识：

#define WCB_SVGEVENT						1
#define WCB_WEB_LOADED						6

2、添加 WebEventData.h/.cpp，定义WebEvent事件的数据类，这些数据类从 CWebEventData 派生；
Mainframe端，添加 WebCommunicationHost.h/.cpp，定义从 Mainframe 过来的 CPMSG_WEBEVENT 分支处理，分同步和异步；
WebContainer端，添加 WebCommunicationClient.h/.cpp，定义从 WebContainer 过来的 CPMSG_WEBEVENT 分支处理，分同步和异步；

-----------------------

CMainframe		CAttachWindow aw;
				aw.CreateClient(); 赋值aw.m_dwHostProcessId、aw.m_hHostWnd，并创建WebContainer进程。
WebContainer	CWebContainerCmdLine theCmd;
				CAttachWindow aw;
				theCmd.Parse(); 解析出 aw.m_dwHostProcessId、aw.m_hHostWnd；并赋值aw.m_dwClientProcessId、aw.m_hClientWnd；
				PostMessage( ClientMainHwnd, WM_AW_PROCESS_ATTACHING )，异步通知CMainframe创建成功，准备吸附。

CMainframe		CMainframe::OnCopydata中，如果是 CPMSG_AW_PROCESS_ATTACHING 分支，
					PostMessage( MainframeMainHwnd, WM_AW_PROCESS_ATTACHING )。
				在CMainframe::OnAwProcessAttaching中：
					解析；
					更新aw.m_dwClientProcessId、aw.m_hClientWnd；
					将ClientMainHwnd吸附进来，作为子窗口；
					PostMessage( ClientMainHwnd, WM_AW_PROCESS_ATTACHED );

WebContainer	WM_AW_PROCESS_ATTACHED 消息处理中，处理后续逻辑。

-------------------

在CMainframe中调用WebContainer页面方法的逻辑：

CMainframe		CWENavigate we;
				we.m_sUrl = "about:blank";
				SendCopyData( aw.m_hClientWnd, we.GetJsonStr() );

WebContainer	CMain::OnCopyData()中，CPMSG_WEBEVENT 分支，theWebCommClient.Receive();

*/

#include "StdAfx.h"
#include "AttachWindow.h"

#include "value.h"
#include "reader.h"
#include "writer.h"

#include "FileFunc.h"
#include "UtilityFunc.h"
#include "ProcessFunc.h"

UINT WM_AW_PROCESS_ATTACHING = ::RegisterWindowMessage (_T("WM_AW_PROCESS_ATTACHING"));
UINT WM_AW_PROCESS_ATTACHED = ::RegisterWindowMessage (_T("WM_AW_PROCESS_ATTACHED"));
UINT WM_AW_PROCESS_DETACHING = ::RegisterWindowMessage (_T("WM_AW_PROCESS_DETACHING"));
UINT WM_AW_PROCESS_DETACHED = ::RegisterWindowMessage (_T("WM_AW_PROCESS_DETACHED"));

CAttachWindow::CAttachWindow()
{
	//m_sId
	m_nExeFlag				= 0;
	m_dwResponseCopyDataId	= 0;
	m_hHostWnd				= NULL;
	m_hClientWnd			= NULL;
	m_dwHostProcessId		= 0;
	m_dwClientProcessId		= 0;
	//m_sExData
	
}

CAttachWindow::~CAttachWindow(void)
{
	Clear();
}

void CAttachWindow::Clear()
{
	m_sId					.Empty();
	m_nExeFlag				= 0;
	m_dwResponseCopyDataId	= 0;
	m_hHostWnd				= NULL;
	m_hClientWnd			= NULL;
	m_dwHostProcessId		= 0;
	m_dwClientProcessId		= 0;
	m_sExData				.Empty();
}

BOOL CAttachWindow::CopyFrom( CAttachWindow* pSrc )
{
	if ( pSrc == NULL ) return FALSE;
	m_sId					= pSrc->m_sId					;			
	m_nExeFlag				= pSrc->m_nExeFlag				;
	m_dwResponseCopyDataId	= pSrc->m_dwResponseCopyDataId	;
	m_hHostWnd				= pSrc->m_hHostWnd				;
	m_hClientWnd			= pSrc->m_hClientWnd			;
	m_dwHostProcessId		= pSrc->m_dwHostProcessId		;
	m_dwClientProcessId		= pSrc->m_dwClientProcessId		;
	m_sExData				= pSrc->m_sExData				;
	return TRUE;
}

/*
参数：
	lpszData：json 对象字符串
*/
BOOL CAttachWindow::Deserialize( LPCTSTR lpszData )
{
	Clear();
	CString strSrc = lpszData == NULL ? "" : lpszData;
	if ( strSrc.IsEmpty() ) return FALSE;

	CString str;
	Json::Reader reader;
	Json::Value obj( Json::objectValue );
	try
	{
		if ( !reader.parse((char*)(LPCTSTR)strSrc, obj, false) ) return FALSE;
		if ( !obj.isObject() ) return FALSE;

		m_sId = ( obj["id"].isString()) ? obj["id"].asCString() : _T("");

		str = ( obj["exeFlag"].isString()) ? obj["exeFlag"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_nExeFlag = strtol( (char*)(LPCTSTR)str, NULL, 10 );
		
		str = ( obj["responseCopydataId"].isString()) ? obj["responseCopydataId"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_dwResponseCopyDataId = (ULONG_PTR)strtol( (char*)(LPCTSTR)str, NULL, 10 );
		
		str = ( obj["hostWnd"].isString()) ? obj["hostWnd"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_hHostWnd = (HWND)strtol( (char*)(LPCTSTR)str, NULL, 10 );
		str = ( obj["clientWnd"].isString()) ? obj["clientWnd"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_hClientWnd = (HWND)strtol( (char*)(LPCTSTR)str, NULL, 10 );
		str = ( obj["hostProcessId"].isString()) ? obj["hostProcessId"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_dwHostProcessId = (DWORD)strtol( (char*)(LPCTSTR)str, NULL, 10 );
		str = ( obj["clientProcessId"].isString()) ? obj["clientProcessId"].asCString() : _T("");
		if ( !str.IsEmpty() )
			m_dwClientProcessId = (DWORD)strtol( (char*)(LPCTSTR)str, NULL, 10 );
		m_sExData = ( obj["exData"].isString()) ? obj["exData"].asCString() : _T("");
	}
	catch (...)
	{
		Clear(); return FALSE;
	}
	return TRUE;
}

CString CAttachWindow::Serialize()
{
	CString str;
	try
	{
		Json::Value obj( Json::objectValue );
		obj[ "id" ] = Json::Value( (const char *)(LPCTSTR)m_sId );
		str.Format( "%d", m_nExeFlag );
		obj[ "exeFlag" ] = Json::Value( (const char *)(LPCTSTR)str );
		str.Format( "%d", m_dwResponseCopyDataId );
		obj[ "responseCopydataId" ] = Json::Value( (const char *)(LPCTSTR)str );
		str.Format( "%d", m_hHostWnd );
		obj[ "hostWnd" ] = Json::Value( (const char *)(LPCTSTR)str );
		str.Format( "%d", m_hClientWnd );
		obj[ "clientWnd" ] = Json::Value( (const char *)(LPCTSTR)str );
		str.Format( "%d", m_dwHostProcessId );
		obj[ "hostProcessId" ] = Json::Value( (const char *)(LPCTSTR)str );
		str.Format( "%d", m_dwClientProcessId );
		obj[ "clientProcessId" ] = Json::Value( (const char *)(LPCTSTR)str );
		obj[ "exData" ] = Json::Value( (const char *)(LPCTSTR)m_sExData );

		Json::FastWriter fast_writer;
		CString sResult = fast_writer.write( obj ).c_str();
		return sResult;
	}
	catch(...)
	{
		return "";
	}
}

BOOL CAttachWindow::CreateClient( LPCTSTR lpszId, int nExeFlag, ULONG_PTR dwResponseCopyDataId
	, DWORD dwHostProcessId, HWND hHostWnd
	, LPCTSTR lpszClientPath, LPCTSTR lpszExData )
{
	CString sId = lpszId == NULL ? "" : lpszId;
	if ( sId.IsEmpty() ) return FALSE;
	if ( nExeFlag == 0 ) return FALSE;
	if ( dwResponseCopyDataId == 0 ) return FALSE;
	if ( dwHostProcessId == 0 ) return FALSE;
	if ( !IsWindow( hHostWnd ) ) return FALSE;
	if ( !IsAccessibleFile( lpszClientPath ) ) return FALSE;

	//
	m_sId = sId;
	m_nExeFlag = nExeFlag;
	m_dwHostProcessId = dwHostProcessId;
	m_hHostWnd = hHostWnd;
	m_dwResponseCopyDataId = dwResponseCopyDataId;
	m_sExData = lpszExData == NULL ? "" : lpszExData;
	//
	CString sJsonData = Serialize();
	if ( sJsonData.IsEmpty() ) return FALSE;
	//启动参数
	CString sExeParam = " -json:";
	sExeParam += sJsonData;
	sExeParam = lpszClientPath + sExeParam;

	if ( !CreateProcessInJob( lpszClientPath, sExeParam, SW_HIDE ) )
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CAttachWindow::ResponseHost( DWORD dwClientProcessId, HWND hClientWnd )
{
	if ( m_sId.IsEmpty() ) return FALSE;
	if ( m_nExeFlag == 0 ) return FALSE;
	if ( m_dwResponseCopyDataId == 0 ) return FALSE;
	if ( m_dwHostProcessId == 0 ) return FALSE;
	if ( !IsWindow( m_hHostWnd ) ) return FALSE;
	if ( dwClientProcessId == 0 ) return FALSE;
	if ( !IsWindow( hClientWnd ) ) return FALSE;

	m_dwClientProcessId = dwClientProcessId;
	m_hClientWnd = hClientWnd;

	CString sJson = Serialize();
	SendCopyData( m_hHostWnd, m_dwResponseCopyDataId, (LPVOID)(LPCTSTR)sJson, sJson.GetLength() );
	return TRUE;
}

BOOL CAttachWindow::Attach( HWND hParentWnd, int nProcessWndAttachedMsgCode, BOOL bTheFilled )
{
	if ( nProcessWndAttachedMsgCode == 0 ) return FALSE;
	if ( !IsWindow( m_hClientWnd ) ) return FALSE;
	if ( !IsWindow( hParentWnd ) ) return FALSE;

	::AttachWindow( hParentWnd, m_hClientWnd, bTheFilled );
	::PostMessage( m_hClientWnd, nProcessWndAttachedMsgCode, 0, 0 );
	return TRUE;
}
