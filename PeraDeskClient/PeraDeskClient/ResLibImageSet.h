#pragma once
#include "StlHeader.h"

/*
ʹ��˵����
��App��ʼ���ڼ䣬��� LoadResLibImageSet();
��App����ǰ����� ClearResLibImageSet();
*/

class CCxStaticRes
{
public:
	static const enum {
		RT_NULL = 0,
		RT_SCRIPT_DRIVER,
		RT_DATA_PARSER,
		RT_EXCEL,
		RT_TOOL_INTEGRATION,
		RT_WORD,
		RT_FORMULA,
		RT_FORM,
		RT_DOCUMENT,
		RT_PATRAN,
		RT_WORKBENCH,

		//static res
		RT_CUSTOM_ROBOT_42, //�Զ���robot
		RT_CUSTOM_ROBOT, //�Զ���robot
		RT_FLOW_42, //����
		RT_FLOW_16, //����
		RT_PC_16, //������Դ�� ���ڵ�
		RT_FOLDER_16, //������Դ�� �ļ���
		RT_COMPONENT_16, //������Դ�� ���
		//��קͼ��
		RT_DRAG_DSTFORM,
		RT_DRAG_DST,
		RT_DRAG_HIGHLIGHT_FROM,
		RT_DRAG_HIGHLIGHT,

		//��֧�༭�Ի������ͼ��
		RT_IF_ARROW,	//�����ͷ
		RT_IF_ARROW_UP,	//�����ͷ
		RT_IF_ARROW_UP_GRAY,	//�����ͷ
		RT_IF_ARROW_DOWN,	//�����ͷ
		RT_IF_ARROW_DOWN_GRAY,	//�����ͷ

		//��½�Ի�����Դ
		RT_DLG_LOGIN_BK_BK,	//��½�ı������ڵı���
		RT_DLG_LOGIN_CLOSE_NOR,	//�رյ�����
		RT_DLG_LOGIN_CLOSE_OVER,	//�رյĻ���
		RT_DLG_LOGIN_MAIN_BK,	//��½�����ı���

		//���ȶԻ�����Դ
		RT_DLG_PROGRESS,			//���ȶԻ����õ���pngģ�¶���Ч����Դ

		//�������öԻ�����Դ
		RT_DLG_PARAMSET			//
	};

	CCxStaticRes();
	~CCxStaticRes();
	void Clear(void);
public:
	int m_nCode;
	CString m_strId;
	CString m_strPath;
	Image * m_pImgNormal;
};

extern void ClearResLibImageSet(void);
extern void LoadResLibImageSet(void);
extern CString ConvertRobotCode2Id(int nCode);
extern int ConvertRobotId2Code(LPCTSTR lpszId);
extern CCxStaticRes * GetRobotImageData(LPCTSTR lpszId, int nSize);
extern CCxStaticRes * GetRobotImageData(int nCode, int nSize);
extern CCxStaticRes * GetStaticResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetStaticResImageData(int nCode);
extern CCxStaticRes * GetFlowIfResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetFlowIfResImageData(int nCode);
extern CCxStaticRes * GetDlgLoginResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetDlgLoginResImageData(int nCode);
extern CCxStaticRes * GetDlgProgressResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetDlgProgressResImageData(int nCode);
extern CCxStaticRes * GetDlgParamSetResImageData(LPCTSTR lpszId);
extern CCxStaticRes * GetDlgParamSetResImageData(int nCode);