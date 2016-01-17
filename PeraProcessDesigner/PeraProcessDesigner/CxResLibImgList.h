#pragma once
#include "CxListCtrl.h"
#include "CxResLibImageToolTipCtrl.h"

// CxResLibImgList

class CxResLibImgList : public CxListCtrl
{
	DECLARE_DYNAMIC(CxResLibImgList)

public:
	CxResLibImgList();
	virtual ~CxResLibImgList();
	BOOL m_bSetCapture;
	//ListCtrlĬ�ϱ���ɫ
	static const Color COLOR_LISTCTRL_BG;
	//Ĭ��ͼƬ����ɫ
	static const Color COLOR_IMAGE_BG;
	//Y�᷽��ͼƬ��С���
	static const int MIN_CY_SPACE_ITEM;

	//����������ݣ�����֮�󣬿������³�ʼ����
	virtual void Release(void);
	//���ͼƬ���ݣ���ɾ����ʼ��������
	virtual void Clear(void);
	
	//�ڳ�ʼ��ǰ����ʼ���ڼ���á���ʼ���󣬲������޸�
	BOOL SetAutoDelete(BOOL bEnabled = TRUE)
	{
		if (IsInitialed() && bEnabled != m_bAutoDeleteData) return FALSE;
		m_bAutoDeleteData = bEnabled; return TRUE;
	}
	//�Ƿ��Զ�ɾ��ͼƬ����
	//����Զ�ɾ������Ҫ����AddImage(lpszPath ...)��ָ��ͼƬ·�����Զ�����ͼƬ��
	//�������ⲿ���غ�ͼƬ�󣬵���AddImage(Bitmap * ...)��
	BOOL IsAutoDeleteData( void ) const { return m_bAutoDeleteData; }

	//��ʼ��ͼƬ�ߴ硢��ࡢ��ɫ����
	BOOL InitImageLayout(int nItemCx, int nItemCy, int nSpaceX, int nSpaceY
		, int nImgCx, int nImgCy, Color crImgBk = COLOR_IMAGE_BG);
	
	//���ͼƬ
	BOOL AddImage(LPCTSTR lpszImgPath, LPCTSTR lpszLabel = "", LPARAM lp = NULL, int nPos = -1);
	BOOL AddImage(Bitmap * pBitmap, LPCTSTR lpszLabel = "", LPARAM lp = NULL, int nPos = -1);
	//ɾ��ͼƬ
	BOOL DeleteIamge(int nItemIndex);

protected:
	virtual BOOL OnInit(void);
	//����״̬ͼƬ�����ڻ��ƽ���״̬��ѡ��״̬��
	virtual void OnLoadStateImage(void);
	//������������
	virtual void OnLoadTextFont();
	//ȡ��ͼƬ�����ֵļ��
	virtual int GetSpaceOfIconLabel(void);
	//��������ʱ
	virtual void OnEraseBkgndCustom(void);
	//����ı�ʱ
	virtual void OnFocusChanged(int nItem, BOOL bFocus);
	//����ͼƬ
	virtual BOOL OnItemPaint(LPNMLVCUSTOMDRAW lplvcd);
	//����ͼƬ����
	virtual void DrawItemText(int nItem, RECT& rcLabel);
	//
	int _AddImage( Bitmap * pBitmap, LPCTSTR lpszLabel = "", LPARAM lp = NULL, int nPos = -1 );

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wp,LPARAM lp);
	afx_msg LRESULT OnMouseLeaved(WPARAM wp,LPARAM lp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	void TestItem( vector<int>& vSels, int& nCurSel );

	CCxResLibImageToolTipCtrl m_ToolTip;
	CMenu m_RMenu;
	CEdit m_wndDummy;

	
protected:
	//�Ƿ��Զ�ɾ�����ݡ�
	//����ǣ��� Clear��Release ������ʱ��ɾ�� m_vImg �е����ݡ�
	//���򣬲�ɾ��
	BOOL m_bAutoDeleteData;
	vector< Image * > m_vImg; //ͼƬ����

	//�Ի����
	Graphics * m_pGraphics;
	Bitmap * m_pBmpCanvas; //�Ի��ڼ䣬�����Bitmap�����ϻ���˫���壩
	Image * m_pImgFocused; //��ý���ʱ��Ӧ��ͼƬ
	Image * m_pImgSelected; //ѡ��״̬��ͼƬ
	
	//ListCtrl����ɫ
	Color m_crCanvasBk;

	CString m_strFontFamilyName; //��������
	Gdiplus::Font * m_pFont; //�������
	StringFormat m_FontFormat; //����������
	Color m_FontColor; //������ɫ

	//����ΪͼƬ�ߴ硢���ȱ�������ϸ�����μ� InitImageLayout ����ʵ�֡�
	//ͼƬ�ߴ硣��ԭʼͼƬ�����óߴ粻��ʱ��������ʾ��
	int m_nImgCx;
	int m_nImgCy;
	//
	int m_nItemCx;
	int m_nItemCy;
	//ͼƬ��ࡣ
	int m_nItemCxSpace;
	int m_nItemCySpace;
	//��Ҫ���� ListCtrl ����ͼƬ����ֻ���˳�ʼֵ����û�����ʵ��ͼƬ
	CImageList m_ImageList;
	//ͼƬ����ɫ��͸��ɫ����
	Color m_crImgBk;

	BOOL m_bDragLayer;

	BOOL m_bMouseTracked;
private:
	//��ʾ�ϴλ�ý���� Item��
	//��ֵֻ������ OnMouseMove �б��޸ġ�
	int m_nLastFocusItem;

public:

	void DragLayer();
	
	afx_msg void OnCxResLibImgListDelete();
	afx_msg void OnUpdateProp();
	afx_msg void OnDownload();
};


