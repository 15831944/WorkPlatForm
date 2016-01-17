#pragma once
#include <afxwin.h>
#include "StlHeader.h"
#include "GDIPlusInit.h"


/*
˵����
CxListCtrl ����Ҫ�ṩһЩ���������� CListCtrl �Ĳ���������ֱ������ԭʼ�� CListCtrl ����
CxListCtrl ����2���࣬������ʾͼƬ�� CxListCtrlImage������������ʽ��ʾ�� CxListCtrlReport��
CxListCtrlImage �� CxListCtrlReport ���Զ������ʵ�֡�
*/

// CxListCtrl

class CxListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CxListCtrl)

public:
	CxListCtrl();
	virtual ~CxListCtrl();
	
	//��ʼ��
	BOOL Init(void);

	//����������ݣ�����֮�󣬿������³�ʼ����
	virtual void Release(void);
	//�������е����ݣ���ɾ����ʼ��������
	virtual void Clear(void);

	//���ý�����Ϣ�Ĵ��ھ�������������ڷ��͵���Ϣ��
	//Ŀ���Ƿ�ֹ�����ڲ�Ϊ��Ϣ������ʱ�����
	void SetRecvMsgWindow(HWND hwnd)
	{
		if ( !IsWindow(hwnd) ) return;
		m_hwndRecvMsg = hwnd;
	}

	//�Ƿ��Ѿ���ʼ��
	BOOL IsInitialed(void) const { return m_bInited; }
	//�Ǳ��ģʽ
	BOOL IsReportMode(void);
	//��ͼƬģʽ
	BOOL IsImageMode(void);

	//����Ϊ����ģʽ
	BOOL SetReportMode( LONG lStyle = 0, LONG lExStyle = 0 );
	//����ΪͼƬģʽ
	BOOL SetImageMode( LONG lStyle = 0, LONG lExStyle = 0 );

	//ȡ��ѡ��ļ�¼������
	int GetCurSel( vector<int>& vItemIndex );
	//ȡ��ѡ��ļ�¼����
	int GetCurSelParam( vector<LPARAM>& vParams );
	//����ָ�������ļ�¼Ϊѡ��״̬
	void SetCurSel( vector<int>& vItems );
	//���ѡ��״̬
	virtual int ClearCurSel( vector<int> * pvItemIndex = NULL );
	//�����Ƿ���Զ�ѡ
	BOOL SetMultiSelMode(BOOL bMultiSel = TRUE);

	//ɾ��ѡ�е���Ŀ������ÿ����Ŀ��Ӧ�Ĳ���
	virtual int DeleteCurSel( vector<LPARAM> * pvParams = NULL );
	//ȡ����������� Item ���� SubItem
	BOOL GetClickItemPos(int& nRow, int& nCol);

	//���һ�������ݣ�һ��Item��
	virtual int AddRow(LPCTSTR lpszText, LPARAM lpParam = NULL, int nRow = -1);
	//���һ�����ݣ�һ��SubItem��ǰ����Ҫ����������ݣ�
	virtual BOOL AddColumn(int nRow, int nCol, LPCTSTR lpszText);



protected:
	//��ʼ��ʱ
	virtual BOOL OnInit(void);
	//��ǰ�ƶ�һ��Item
	virtual void OnItemMoveForward(void);
	//����ƶ�һ��Item
	virtual void OnItemMoveBack(void);
	//�ƶ� Item ǰ �����б����ݣ����ָ��б�һ��
	virtual int OnUpdateItemDataPtr(void);
	//�ƶ� Item ���
	virtual void OnItemMoved( vector<int>& vNewSel );

public:
	//�Ƿ���ͨ�� ��Ctrl + �ϡ����� ��Ctrl + �¡� ��ϼ��ƶ� Item
	BOOL m_bReadOnly;

protected:
	BOOL m_bInited;
	//
	HWND m_hwndRecvMsg;
	//�б��е����ݶ��󣬽��� �ƶ� Item �¼���ʹ��
	vector<LPVOID> m_vItemDataPtr;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()
};


