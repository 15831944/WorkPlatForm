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
	��webpage
	*/
	WM_ATTACH_WEBPAGE,

	/************************************************************************
	����ҵ�����У��鿴������
	*/
	WM_VIEW_RUNFLOW_WEBPAGE,
	/************************************************************************
	���ز�����Ϣ
	*/
	WM_RESLIB_NOTIFY_DOWNLOAD,
	/************************************************************************
	����dms����
	*/
	WM_STRUCTTREE_UPDATE_DMS_DATA,
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
	WM_STRUCTTREEE_LEAVEFOCUS
};

//CopyData ��Ϣ��ʶ
enum{
	/*
	���ܣ�
		WebPage
	*/
	CPMSG_WEBPAGE = 1
};

//ViewMg�У�view���
extern const LPCTSTR viewflagModel;
extern const LPCTSTR viewflagRunProcess;
extern const LPCTSTR viewflagHistoryMonitor;
extern const LPCTSTR viewflagDistribution;
extern const LPCTSTR viewflagWebPage;
extern const LPCTSTR viewflagWelcome;

//MDI������
extern const LPCTSTR g_lpszAppTitle;
//View-��ģ��������
extern const LPCTSTR g_lpszViewTitleModel;
//View-����������
extern const LPCTSTR g_lpszViewTitleHistoryMonitor;
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