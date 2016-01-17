#pragma once
#include <vector>
#include <map>
using namespace std;

//�ؼ���־
enum {
	CF_BAR_JIANMO_STRUCTTREE = 1,
	CF_BAR_JIANMO_RESLIB,
	CF_BAR_MAIN_TOOLBAR,
	CF_BAR_MAIN_ROBOTBAR
};

//�˵��ؼ�ID
enum {
	//����ı�����Դ��˵�//////////
	ID_TREE_MENU_ADD = 40001,
	ID_TREE_MENU_DEL,     
	ID_TREE_MENU_EDIT,    
	ID_TREE_MENU_SEND
};

//�Ի���ؼ�ID
enum {
	//Nά����༭�Ի���////////////////
	ID_NDARRAY_VIEW = 41001,
	ID_NAE_ARRAY_DIM,

	//MainFrame toolbar
	IDC_MAIN_TOOLBAR,
	IDC_MAIN_ROBOTBAR,
	IDC_MAIN_LOGINBAR,
	IDC_MAIN_MENUBAR,

	//
	IDC_LOGINBAR_USER,
	IDC_LOGINBAR_LOGIN,

	//
	IDC_RESLIBIMGLIST_DUMMY
};

//////////////////////////////////////////////////////////////////////////
enum {
	IDM_TOOLBAR_BUTTON_FIRST = 45001,
	IDM_TOOLBAR_BUTTON_LAST = (IDM_TOOLBAR_BUTTON_FIRST + 256),

	ID_GRID_BUTTON_FIRST = (IDM_TOOLBAR_BUTTON_LAST + 1),
	ID_GRID_BUTTON_LAST = (ID_GRID_BUTTON_FIRST + 256),

	IDM_ROBOTBAR_BUTTON_FIRST = (ID_GRID_BUTTON_LAST + 1),
	IDM_ROBOTBAR_BUTTON_LAST = (IDM_ROBOTBAR_BUTTON_FIRST + 256),

	IDM_HISTORY_MODEL_START = (IDM_ROBOTBAR_BUTTON_LAST + 1),
	IDM_HISTORY_MODEL_LAST = (IDM_HISTORY_MODEL_START + 256),

	IDM_VIEW_START = (IDM_HISTORY_MODEL_LAST + 1),
	IDM_VIEW_LAST = (IDM_VIEW_START + 256)
};



//��Ϣ///////////////////////////////////////////////////////////

enum {

	/************************************************************************
	������Դ���ϣ����ڵ�ѡ��ı�ʱ�����ͱ���Ϣ��ˢ��ͼƬ�б�
	*/
	WM_RESLIB_NOTIFY_SELCHANGED = (WM_USER + 1),
	
	/************************************************************************
	���ܣ�
		������Դ��-ͼ���б��У�����ͼ���ϵ��ɾ��ʱ������Դ�����ʹ���Ϣ��ɾ��������Դ��
	������
		wp��LPARAM * ���顣
		lp������Ԫ�ظ�����
	˵����
		���շ�ɾ������ָ�롣
	*/
	WM_RESLIB_NOTIFY_DELETE,

	/************************************************************************
	���ܣ�
		������Դ��-ͼ���б��У�����ͼ���ϵ����������ʱ������Դ�����ʹ���Ϣ�����±�����Դ�⡣
	������
		wp��LPARAM Ԫ�ض���
		lp��0��
	*/
	WM_RESLIB_NOTIFY_UPDATE_PROP,

	/************************************************************************
	���ܣ�
		����Ϣ���ڸ��½ṹ���е�����ӳ����Ϣ��
	������
		wp��ѡ�еĽڵ�ָ��
		lp��0
	˵����
		��js����updatestructtreeʱ����������PostMessage����Ϣ������js��PeraProcessDesigner�������á�
	*/
	WM_UPDATE_DATAMAPPING,

	/************************************************************************
	���ܣ�
		����Ϣ�����ڽṹ���ϣ�����js�������������
	������
		wp��0
		lp��0
	˵����
		����Ҫ�ڽṹ�����������ʱ��ͨ�����ṹ��Post����Ϣ��ʵ�֣�Ŀ���Ƿ�ֹjs��PeraProcessDesigner�����������á�
	*/
	WM_STRUCTTREE_RUN_COMPNENT,

	/************************************************************************
	���ܣ�
		����Ϣ�����첽������ƹ��̡�
	������
		wp��0
		lp��0
	˵����
	*/
	WM_STRUCTTREE_RUN_PROCESS,
	
	/************************************************************************
	���ܣ�
		����Ϣ�����첽���������̡�
	������
		wp��0
		lp��0
	˵����
	*/
	WM_STRUCTTREE_RUN_SUB_PROCESS,
	
	/************************************************************************
	���ܣ�
		����Ϣ�����첽���Ϊ�����̡�
	������
		wp��0
		lp��0
	˵����
	*/
	WM_STRUCTTREE_SAVEAS_SUB_PROCESS,
	
	/************************************************************************
	���ܣ�
		����Ϣ���ڴ�SVG�ϸ����������ƹ��̵�������Ϣ��
	������
		wp���ڵ����ָ��
		lp��0
	˵����
		����Ҫ��SVG�ϸ�������ʱ���������ڷ��ͱ���Ϣ���������ڵ��ýṹ�������������Ա༭���档
		�༭�󣬵���js��propUpdate�������������ݡ�
		Ŀ��1����ֹjs��PeraProcessDesigner�����������á�
		Ŀ��2�����ָ������ԵĴ���һ���ԡ�
		���������Ϣ������ȫ�ֵ�ָ���������Ҫȷ���ڴ�����Ϣʱ���ڵ����ָ����Ч�����������쳣��
	*/
	WM_STRUCTTREE_UPDATE_PROP,
	
	/************************************************************************
	���ܣ�
		�༭����ʱ�����²���ֵ ��ɾ����ǰ��ӦԪ�ص��У������Ԫ�ض�Ӧ���У���
	������
		wp����ǰ�����ڵ����
		lp��0
	*/
	WM_STRUCTTREE_UPDATE_PARAM_ARRAY,
	
	/************************************************************************
	���ܣ�
		����Ϣ���ڴ�SVG����ʾ/����ʾ������Ϣ��
	������
		wp���ڵ����ָ��
		lp��BOOLֵ��TRUE����ʾ��FALSE������ʾ
	˵����
		����Ҫ��SVG�ϸ�������ʱ���������ڷ��ͱ���Ϣ���������ڵ��ýṹ��������ʾ��
		���������Ϣ������ȫ�ֵ�ָ���������Ҫȷ���ڴ�����Ϣʱ���ڵ����ָ����Ч�����������쳣��
	*/
	WM_STRUCTTREE_SHOW_TOOLTIP,

	/************************************************************************
	���ܣ�
		��ʾ����ӳ��Ի���
	������
		wp��0
		lp��0
	*/
	WM_STRUCTTREE_SHOW_DATAMAPPING,

	/************************************************************************
	���ܣ�
		��֧���öԻ���
	������
		wp��Json�ַ���
		lp��0
	˵����
	    ����Ҫ��SVG�ϱ༭������֧���������ڷ��ͱ���Ϣ���������ڵ��ýṹ��������ʾ��
	*/
	WM_STRUCTTREE_EDIT_IF,

	/************************************************************************
	���ܣ�
		�༭���ڵ������
	������
		wp��Json�ַ���
		lp��0
	˵����
	    ����Ҫ��SVG�ϱ༭���ڵ�������������ڷ��ͱ���Ϣ���������ڵ��ýṹ��������ʾ��
	*/
	WM_STRUCTTREE_EDIT_ROOTPARAMS,

	/************************************************************************
	���ܣ�
		ѭ�����á�
	������
		wp��0
		lp��0
	˵����
	    ����Ҫ��SVG������ѭ�����������ڷ��ͱ���Ϣ���������ڵ��ýṹ��������ʾ��
	*/
	WM_STRUCTTREE_EDIT_FOR,

	/************************************************************************
	���ܣ�
		�༭����ӳ����ʽ�Ի���
	������
		wp��Json�ַ���
		lp��0
	˵����
	    ����Ҫ��SVG�ϱ༭����ӳ����ʽ���������ڷ��ͱ���Ϣ���������ڵ��ýṹ��������ʾ��
	*/
	WM_STRUCTTREE_EDIT_DATAMAPPING_EXP,

	/************************************************************************
	���ܣ�
		��ʾsvg�϶��㴰�ڡ�
	������
		wp��TRUE/FALSE
		lp�����
	˵����
	    ��ʾ/ȡ����
	*/
	WM_SHOW_SVG_DRAG_LAYER,
	/************************************************************************
	���ܣ�
		����������׼���ú�Post��MDI����Ϣ��
	������
		wp��0
		lp��0
	*/
	WM_UPDATE_RESLIB_TREE,

	/************************************************************************
	��¼�Ի���رհ�ť�����ͣ�¼�
	*/
	WM_LOGIN_CLOSEBUTTON_MOUSEOVER,
	
	/************************************************************************
	��¼�Ի���رհ�ť�����ͣ�¼�
	*/
	WM_LOGIN_CLOSEBUTTON_MOUSELEAVE,
	
	/************************************************************************
	��ʾ/���ؽ��ȶԻ���
	*/
	WM_PROGRESS_SHOW,

	/************************************************************************
	��������������ģ̬�Ի���
	*/
	WM_SHOW_RUNFLOW_DIALOG,

	/************************************************************************
	�������������
	*/
	WM_RUNFLOW_COMPLETED,

	/************************************************************************
	����ҵ�����У��鿴������
	*/
	WM_SHOW_RUNFLOW_VIEW,
	/************************************************************************
	���ز�����Ϣ
	*/
	WM_RESLIB_NOTIFY_DOWNLOAD,
	/************************************************************************
	SVG�����Ҽ��˵�,wParam��CCxStructTreePopMenuData����Ҫ�ֶ��ͷ�
	*/
	WM_POP_MENU,

	/************************************************************************
	���ܣ�
		�������ʱ����ʶ������ӳ�����
	������
		wp��0
		lp��To�ڵ�Id�ַ���ָ��
	*/
	WM_UPDATE_COMPONENT_BY_LINKER,
	WM_USER_SORTMSG,

	/************************************************************************
	login bar clicked 
	*/
	WM_LOGINBAR_CLICKED_LOGOUT,
	/**
	�༭�� CPeraAutoCheckEdit ʹ��SendMessage ������Ϣ���ַ���У�鴰��
	*/
	WM_EDIT_AUTOCHECK,
	/*
	if��ӱ��ʽ�༭���Ի����Ͻṹ��������˫����Ϣ
	*/
	WM_STRUCTTREEE_LDLK, 
	/*
	if��ӱ��ʽ�༭���Ի����Ͻṹ��ʧȥ������Ϣ
	*/
	WM_STRUCTTREEE_LEAVEFOCUS,
	/*
	���´��ڱ�����Ϣ
	*/
	WM_UPDATE_MAIN_TITLE,

	/************************************************************************
	���ܣ�
		����ṹ������ˢ�¡�
	������
		wp�������ѡ�нڵ�
		lp������
	˵����
	    �ڴ����ݶ��������֮�������������ڵ�
	*/
	WM_STRUCTTREE_REFRESH
};

#define TIMER_LOADING_WINDOW						2
#define TIMER_DLGLOGINBK_SETFOCUS					3
#define TIMER_MAINFRAME_RECENTLYMENU				4
#define TIMER_MAINFRAME_UPDATEWINDOW				5
#define TIMER_DLGIF_PAINTWINDOW						6
//#define TIMER_EDITBRANCH_UNKNOW						7
#define TIMER_XTIPCOMBOBOX_MOUSEMOVE				8
#define TIMER_XTIPLISTBOX_MOUSEMOVE					9
//#define TIMER_ZEDIT_UNKNOW							10

#define TIMER_LOADING_WINDOW_TIMEOUT				90
#define TIMER_DLGLOGINBK_SETFOCUS_TIMEOUT			200
#define TIMER_MAINFRAME_RECENTLYMENU_TIMEOUT		0
#define TIMER_MAINFRAME_UPDATEWINDOW_TIMEOUT		0
#define TIMER_DLGIF_PAINTWINDOW_TIMEOUT				0
//#define TIMER_EDITBRANCH_UNKNOW_TIMEOUT				0
#define TIMER_XTIPCOMBOBOX_MOUSEMOVE_TIMEOUT		80
#define TIMER_XTIPLISTBOX_MOUSEMOVE_TIMEOUT			80
//#define TIMER_ZEDIT_UNKNOW_TIMEOUT					0


//CopyData ��Ϣ��ʶ
/*
���ܣ�
	WebPage
*/
#define CPMSG_WEBEVENT						1
/*
���ܣ�
	���Ѵ򿪵�ws�����ʹ�ws�ļ���Ϣ
*/
#define CPMSG_WORKSPACE_OPENWS				4
/*
��ģִ֪ͨ�л�����ִ��һ����ƹ���
*/
#define CPMSG_RUNNER_CACHE					3
/*
���ܣ�
	���Ѵ򿪵�ws�����ʹ�ws�ļ���Ϣ
*/
#define CPMSG_WORKSPACE_MAKESUREINFO		5
/*
���ܣ�
	WebPage - getClientSubFlowId
*/
#define CPMSG_WEBPAGE_getClientSubFlowId	6
/*
���ܣ�
WebContainer ������Ϣ
�� WM_AW_PROCESS_ATTACHING ��Ӧ
*/
#define CPMSG_AW_PROCESS_ATTACHING			7


/************************************************************************
���½ṹ��
************************************************************************/
#define WCB_SVGEVENT					1

/************************************************************************
��ȡ����
���룺����ΪJson�ַ�����json����Ϊ�ַ������飬ÿ���ַ���Ϊ��������ݱ�ʶ���б����£�
workpath                    ����·��
processname                 ��������
���أ�Json�ַ�����json����Ϊ����
************************************************************************/
#define WCB_GET_INFO					2

/************************************************************************
���������̽���
���룺����ΪJson�ַ�����
************************************************************************/
#define WCB_RUNFLOW_COMPLETED			4

/************************************************************************
����ҵ�����У��鿴����������
���룺ҳ��url��

js�������£�
	function(){
		var st = {};
		st.paramnamelist = "url";
		st.url = "about:blank";
		try{
			var ret = external.InvokeAppMethod(5, st);
			return ret;
		}catch(e){
		}
	}
************************************************************************/
#define WCB_SHOW_RUNFLOW_VIEW				5

/************************************************************************
webpage�У�svg������ɣ���ɺ��������
js�������£�
function(){
var st = {};
st.paramnamelist = "";
try{
var ret = external.InvokeAppMethod(6, st);
return ret;
}catch(e){
}
}
************************************************************************/
#define WCB_WEB_LOADED						6

/***********************************************************************/

//////////////////////////////////////////////////////////////////////////
//Web����
#define WEBTYPE_JOBMGR					1
#define WEBTYPE_SVG						2
#define WEBTYPE_RUNFLOW					3
#define WEBTYPE_RUNFLOW_VIEW			4
#define WEBTYPE_RUNPROCESS				5

//////////////////////////////////////////////////////////////////////////
//WebEvent
//RunFlowҳ�潻���¼�
#define WE_RUNFLOW_CANCEL				1
#define WE_RUNFLOW_COMPLETED			2
#define WE_RUNFLOW_SUBFLOWID			3



//ViewMg�У�view���
extern const LPCTSTR viewflagModel;
extern const LPCTSTR viewflagRunProcess;
extern const LPCTSTR viewflagJobMgr;
extern const LPCTSTR viewflagDistribution;
//extern const LPCTSTR viewflagWebPage;
//extern const LPCTSTR viewflagWelcome;

//MDI������
extern const LPCTSTR g_lpszAppTitle;
//View-��ģ��������
extern const LPCTSTR g_lpszViewTitleModel;
//View-����������
extern const LPCTSTR g_lpszViewTitleJobMgr;
//View-�ֲ�ʽ��Դ��������
extern const LPCTSTR g_lpszViewTitleDistribution;

//TabAera��ɫ
extern const COLORREF g_crTabBarBg;
extern const COLORREF g_crTabBarBg2;
//TabAera�����������
extern const COLORREF g_crTabBarLeftLine;

//�������ϱ���
extern const COLORREF g_crToolbarTop;
extern const COLORREF g_crToolbarTop2;

//��������Robot����ɫ
extern const COLORREF g_crToolbarBg;
//splitter��ɫ
extern const COLORREF g_crSplitterBg;

//�ṹ��bar����߿��ڱ߿���ɫ
extern const COLORREF g_crBarBorderOut;
extern const COLORREF g_crBarBorderIn;
extern const COLORREF g_crBarCaptionBottom;
//�ṹ��bar����������ɫ
extern const COLORREF g_crBarCaptionStart;
extern const COLORREF g_crBarCaptionEnd;

//�ṹ��////////////////////
//��ͷ��ɫ
extern const COLORREF g_crStructTreeHeaderBg;
//��ͷ�ϱ���
extern const COLORREF g_crStructTreeHeaderTop;
//��ͷ�±���
extern const COLORREF g_crStructTreeHeaderBottom;
//�ͻ������ɫ
extern const COLORREF g_crStructTreeClientBg;
//ѡ�еĵ�Ԫ���ɫ
extern const COLORREF g_crStructTreeSelItemBg;
//������ɫ-����
extern const COLORREF g_crStructTreeText;
//������ɫ-�����޸�
extern const COLORREF g_crStructTreeTextInvalid;
//������ɫ-�ɱ༭
extern const COLORREF g_crStructTreeTextCanEdit;
//ÿ��Itemͼ���͸��ɫ
extern const COLORREF g_crStructTreeImgBg;
//����
extern const COLORREF g_crStructTreeTreeLine;

//�����Դ��/////////////////////
//Tree��ɫ
extern const COLORREF g_crResLibTreeBg;
//Image��ɫ
extern const COLORREF g_crResLibImageBg;

//test
extern const COLORREF g_crTestBoder;
extern const COLORREF g_crTestBg;

//����༭//////////////////////
//��ͷ�ָ���
extern const COLORREF g_crNDArrayHeaderBorder;

// �༭���ַ���У�鲻ͨ��ʱ������ɫ
extern const COLORREF g_crEditErrBkColor;

//////////////////////////////////////////////////////////////////////////

//Tooltip��ʱ
extern const DWORD g_dwToolTipDelay;
//Tooltip���Ͻ� ���������ƫ��
extern const CPoint g_ToolTipOffset;
//�Ƿ���Է��ʹ�ws�ļ������б�ǹ����ڴ���
extern const LPCTSTR g_lpszDoubleOpenWsMemName;
//�Ƿ���Է��ʹ�ws�ļ������б�ǹ����ڴ�����
extern const LPCTSTR g_lpszDoubleOpenWsMemStr;
//�����潨ģ���У�����ִ�л���������
extern const LPCTSTR g_lpszWSRunnerProcessName;
//�����潨ģ���У�����ִ�л���,Captionģ������
extern const LPCTSTR g_lpszWSRunnerFindCaption;

//webcontainer�����б���ws���
extern HWND g_hwndMainWindow;