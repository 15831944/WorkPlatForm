#include "StdAfx.h"
#include "UserDefine.h"
#include "FileFunc.h"

const LPCTSTR viewflagModel = _T("��ģ����");
const LPCTSTR viewflagRunProcess = _T("������ƹ���");
const LPCTSTR viewflagHistoryMonitor = _T("��ʷ���");
const LPCTSTR viewflagDistribution = _T("�ֲ�ʽ��Դ����");
const LPCTSTR viewflagWebPage = _T("WebPage");
const LPCTSTR viewflagWelcome = _T("Welcome");

//////////////////////////////////////////////////////////////////////////

const LPCTSTR g_lpszAppTitle = _T("�ۺ���ƻ���");
//View-��ģ��������
const LPCTSTR g_lpszViewTitleModel = _T("��ģ����");
//View-����������
const LPCTSTR g_lpszViewTitleHistoryMonitor = _T("��ҵ����");
//View-�ֲ�ʽ��Դ��������
const LPCTSTR g_lpszViewTitleDistribution = _T("�ֲ�ʽ��Դ����");


//////////////////////////////////////////////////////////////////////////

//TabAera��ɫ
const COLORREF g_crTabBarBg = RGB( 0xd4, 0xd4, 0xd4 );
const COLORREF g_crTabBarBg2 = RGB( 0xc8, 0xc8, 0xc8 );
//TabAera�����������
const COLORREF g_crTabBarLeftLine = RGB( 151, 151, 151 );

//�������ϱ���
const COLORREF g_crToolbarTop = RGB( 0x97, 0x97, 0x97 ); //RGB( 0xff, 0x0, 0x0 ); //
const COLORREF g_crToolbarTop2 = RGB( 0xff, 0xff, 0xff ); //RGB( 0x0, 0xff, 0x0 ); //

//��������Robot����ɫ
const COLORREF g_crToolbarBg = RGB( 0xe9, 0xe9, 0xe9 ); //RGB(0,255,0); //
//splitter��ɫ
const COLORREF g_crSplitterBg = RGB( 0xc8, 0xc8, 0xc8 ); //RGB(0xff,0xff,0x0); //

//�ṹ��bar����߿��ڱ߿���ɫ
const COLORREF g_crBarBorderOut = RGB( 0x77, 0x77, 0x77 ); //RGB(0xff,0x0,0x0); //
const COLORREF g_crBarBorderIn = RGB( 0xff, 0xff, 0xff ); //RGB(0xff,0xff,0x0); //
const COLORREF g_crBarCaptionBottom = RGB( 0xff, 0xff, 0xfe ); //RGB(0xff,0x0,0x0); //
//�ṹ��bar����������ɫ
const COLORREF g_crBarCaptionStart = RGB( 0xee, 0xee, 0xee ); //RGB(0,210,0);//
const COLORREF g_crBarCaptionEnd = RGB( 0xdf, 0xdf, 0xdf ); //RGB(0,250,0); //

//�ṹ��////////////////////
//��ͷ��ɫ
const COLORREF g_crStructTreeHeaderBg = RGB( 0xec, 0xec, 0xec );
//��ͷ�ϱ���
const COLORREF g_crStructTreeHeaderTop = g_crStructTreeHeaderBg;
//��ͷ�±���
const COLORREF g_crStructTreeHeaderBottom = RGB( 0xcc, 0xcc, 0xcc );
//�ͻ������ɫ
const COLORREF g_crStructTreeClientBg = RGB( 0xfb, 0xfb, 0xfb ); //RGB(0xff, 0, 0); //
//ѡ�еĵ�Ԫ���ɫ
const COLORREF g_crStructTreeSelItemBg = RGB( 0xff, 0xe4, 0x91 ); //RGB(0xff, 0, 0); //
//������ɫ
const COLORREF g_crStructTreeText = RGB( 123, 123, 123 );
//������ɫ-�����޸�
const COLORREF g_crStructTreeTextInvalid = RGB( 0x99, 0x99, 0x99 );
//������ɫ-�ɱ༭
const COLORREF g_crStructTreeTextCanEdit = RGB( 10, 54, 143 );
//ÿ��Itemͼ���͸��ɫ
const COLORREF g_crStructTreeImgBg = RGB( 0xff, 0xff, 0xff );
//����
const COLORREF g_crStructTreeTreeLine = RGB( 128, 128, 128 );

//�����Դ��/////////////////////
//Tree��ɫ
const COLORREF g_crResLibTreeBg = g_crStructTreeClientBg;
//Image��ɫ
const COLORREF g_crResLibImageBg = RGB( 0xff, 0xff, 0xff );

//test
const COLORREF g_crTestBoder = RGB( 255, 0, 0 );
const COLORREF g_crTestBg = RGB( 0, 0, 255 );

//����༭//////////////////////
//��ͷ�ָ���
extern const COLORREF g_crNDArrayHeaderBorder = RGB( 125, 125, 125 );

extern const COLORREF g_crEditErrBkColor = RGB(255 , 255 ,0);
//////////////////////////////////////////////////////////////////////////

//Tooltip��ʱ
const DWORD g_dwToolTipDelay = 500;
//Tooltip���Ͻ� ���������ƫ��
const CPoint g_ToolTipOffset(1, 20);