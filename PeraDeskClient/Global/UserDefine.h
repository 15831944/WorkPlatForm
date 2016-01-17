#pragma once
#include <vector>
#include <map>
using namespace std;

//�ؼ�ID
enum {
	IDC_MAIN_TOOLBAR = 41001,
	IDC_MAIN_LOGINBAR,
	IDC_MAIN_MENUBAR,

	//
	IDC_LOGINBAR_USER,
	IDC_LOGINBAR_LOGIN
};

//////////////////////////////////////////////////////////////////////////
enum {
	IDM_TOOLBAR_BUTTON_FIRST = 45001,
	IDM_TOOLBAR_BUTTON_LAST = (IDM_TOOLBAR_BUTTON_FIRST + 256),

	IDM_VIEW_START = (IDM_TOOLBAR_BUTTON_LAST + 1),
	IDM_VIEW_LAST = (IDM_VIEW_START + 256)
};

//��Ϣ///////////////////////////////////////////////////////////

enum {
	WM_DUMMY = (WM_USER + 1),
	
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
	��webpage
	*/
	WM_ATTACH_WEBPAGE,

	/************************************************************************
	login bar clicked 
	*/
	WM_LOGINBAR_CLICKED
};

//CopyData ��Ϣ��ʶ
enum{
	/*
	���ܣ�
		WebPage
	*/
	CPMSG_WEBPAGE = 1,
	/*
	���ܣ�
		ͨ��PeraDCЭ�飬�����ͻ���
	*/
	CPMSG_RUN_PERADESKCLIENT_BY_PERADC = 2,
	/*
	���ܣ�
		ע����Ϣ�����͵�PeraDCWebContainer
	*/
	CPMSG_LOGIN_OUT = 3,
	/*
	���ܣ�
		ˢ����Ϣ�����͵�PeraDCWebContainer
	*/
	CPMSG_REFRESH = 4,
	/*
	���ܣ�
		��¼�Ѿ��򿪵Ŀͻ��ˣ�����ת��ָ��ҳ��
	*/
	CPMSG_LOGIN_DESKCLIENT = 5
};

#define TIMER_LOADING_WINDOW						2

//ViewMg�У�view���
extern const LPCTSTR viewflagTaskModel;


//MDI������
extern const LPCTSTR g_lpszAppTitle;
//View-��ģ��������
extern const LPCTSTR g_lpszViewTitleTaskModel;

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

//////////////////////////////////////////////////////////////////////////

//Tooltip��ʱ
extern const DWORD g_dwToolTipDelay;
//Tooltip���Ͻ� ���������ƫ��
extern const CPoint g_ToolTipOffset;