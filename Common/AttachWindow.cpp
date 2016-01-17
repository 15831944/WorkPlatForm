
/*
�����Ľ����߼���

Host�ˣ�

����aw���󣬸�ֵaw.m_dwHostProcessId��aw.m_hHostWnd��

ͨ���Զ��������У�����Client���̡�

Client�ˣ�

����aw���󣬽��������У��õ� aw.m_dwHostProcessId��aw.m_hHostWnd��
�� aw.m_dwClientProcessId��aw.m_hClientWnd ��ֵ��

���Լ������� Post ��Ϣ WM_AW_PROCESS_ATTACHING��
�� WM_AW_PROCESS_ATTACHING ����Ϣ�����У��� Host ���ͷ�֧Ϊ m_dwResponseCopyDataId �� Copydata ��Ϣ��

Host�ˣ�

�յ���֧Ϊ WM_AW_PROCESS_ATTACHING �� Copydata ��Ϣ�󣬸��Լ������� Post ��Ϣ WM_AW_PROCESS_ATTACHING��

�� WM_AW_PROCESS_ATTACHING ����Ϣ�����У������� aw.m_dwClientProcessId��aw.m_hClientWnd��
�� Client ������������������Ϊ�Ӵ��ڣ�
�� Client ������ Post WM_AW_PROCESS_ATTACHED ��Ϣ��

Client�ˣ�

�յ� WM_AW_PROCESS_ATTACHED �󣬺����Ĵ����߼���

///////////////////////////////////////////////////////////

Mainframe �� WebContainer Ӧ�ã�

�� UserDefine.h �ж������³�����

1-1��Copydata�У�Client�������ɹ��ķ�֧��CAttachWindow::m_dwHostProcessId����

#define CPMSG_AW_PROCESS_ATTACHING			7

1-2��Copydata�У�WebEvent�¼���֧��

#define CPMSG_WEBEVENT						1

1-3��web���ͱ�ʶ��CAttachWindow::m_nExeFlag��CWebEventData::m_dwWebType����

#define WEBTYPE_JOBMGR						1
#define WEBTYPE_SVG							2
#define WEBTYPE_RUNFLOW						3
#define WEBTYPE_RUNFLOW_VIEW				4
#define WEBTYPE_RUNPROCESS					5

1-4��WebContainer��Mainframe�����¼���ʶ��CWebEventData::m_dwWebEvent����

//RunFlowҳ�潻���¼�
#define WE_RUNFLOW_CANCEL					1
#define WE_RUNFLOW_COMPLETED				2
#define WE_RUNFLOW_SUBFLOWID				3

1-5����Ƕҳ��ص�exe�¼���ʶ��

#define WCB_SVGEVENT						1
#define WCB_WEB_LOADED						6

2����� WebEventData.h/.cpp������WebEvent�¼��������࣬��Щ������� CWebEventData ������
Mainframe�ˣ���� WebCommunicationHost.h/.cpp������� Mainframe ������ CPMSG_WEBEVENT ��֧������ͬ�����첽��
WebContainer�ˣ���� WebCommunicationClient.h/.cpp������� WebContainer ������ CPMSG_WEBEVENT ��֧������ͬ�����첽��

-----------------------

CMainframe		CAttachWindow aw;
				aw.CreateClient(); ��ֵaw.m_dwHostProcessId��aw.m_hHostWnd��������WebContainer���̡�
WebContainer	CWebContainerCmdLine theCmd;
				CAttachWindow aw;
				theCmd.Parse(); ������ aw.m_dwHostProcessId��aw.m_hHostWnd������ֵaw.m_dwClientProcessId��aw.m_hClientWnd��
				PostMessage( ClientMainHwnd, WM_AW_PROCESS_ATTACHING )���첽֪ͨCMainframe�����ɹ���׼��������

CMainframe		CMainframe::OnCopydata�У������ CPMSG_AW_PROCESS_ATTACHING ��֧��
					PostMessage( MainframeMainHwnd, WM_AW_PROCESS_ATTACHING )��
				��CMainframe::OnAwProcessAttaching�У�
					������
					����aw.m_dwClientProcessId��aw.m_hClientWnd��
					��ClientMainHwnd������������Ϊ�Ӵ��ڣ�
					PostMessage( ClientMainHwnd, WM_AW_PROCESS_ATTACHED );

WebContainer	WM_AW_PROCESS_ATTACHED ��Ϣ�����У���������߼���

-------------------

��CMainframe�е���WebContainerҳ�淽�����߼���

CMainframe		CWENavigate we;
				we.m_sUrl = "about:blank";
				SendCopyData( aw.m_hClientWnd, we.GetJsonStr() );

WebContainer	CMain::OnCopyData()�У�CPMSG_WEBEVENT ��֧��theWebCommClient.Receive();

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
������
	lpszData��json �����ַ���
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
	//��������
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
