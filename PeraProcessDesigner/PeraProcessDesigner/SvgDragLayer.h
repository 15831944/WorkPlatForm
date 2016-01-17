/*
	�����C++��SVG Viewer�Ͽؼ��Ľ���Ч��ʵ��
	ʹ��ʱ��ӶԻ�����Դ���࣬��������CDialog��ΪCSvgDragLayer��������Դ�Ի����Ϊ�ޱ߿�
	��Ҫgdi+��ʼ��
*/

#pragma once

#include <ResLibData.h>

#include <set>

class CDragRect
{
public:
	CDragRect():m_sId(""), m_rect(){}
	CDragRect(Gdiplus::RectF rect, CString sId):m_rect(rect), m_sId(sId){}
	void Clear()
	{
		m_rect = Gdiplus::RectF();
		m_sId.Empty();
	}

	CDragRect& operator=(const CDragRect &p1)
	{
		m_rect = p1.m_rect;
		m_sId = p1.m_sId;
		return *this;
	}

	Gdiplus::RectF m_rect;
	CString m_sId;
};

//�Ƚ�CPoint��С�ķº���
class CDragRectSort //�䵱����׼��ķº���  
{  
public:  
	bool operator()(const CDragRect &rc1,const CDragRect &rc2)  
	{  
		return rc1.m_sId > rc2.m_sId;
	}  
};  

// CSvgDragLayer �Ի���

class CSvgDragLayer : public CDialog
{
	DECLARE_DYNAMIC(CSvgDragLayer)

public:
	CSvgDragLayer(UINT nID, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSvgDragLayer();

// �Ի�������
//	enum { IDD = IDD_SVGDRAGLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	//Ŀ�ĵ�ͼ�꼰��������
	Gdiplus::Image* m_pDstImage;
	Gdiplus::Image* m_pDstFormImage;
	std::set<CDragRect, CDragRectSort> m_setPointDstImage;//�������ں��������׶������Ҫ�淽�α�����������ͼƬʱ����ú�ҳ�濪������������ÿ��������Ӧ�ڵ�rect
	//��ʶ��ǰ����Ƿ���SVG�����У��粻�ڣ������Ŀ�ĵ�
	BOOL m_bPointInSvg;
	//������ͼ�꼰����
	Gdiplus::Image* m_pHighlightImage;
	Gdiplus::Image* m_pHighlightFormImage;
	CDragRect m_rectHighlightImage;
	//�϶��Ĺ���ͼ�꼰����
	Gdiplus::Image* m_pDragToolImage;
	CPoint m_pointDragToolImage;
	//���㴰��Ӧ�ڵ�λ�ã���Ļ���꣩
	CRect m_rcCanvasWnd;
	//��ģ��̬ҳ�����������ڻ��������꣩
	CRect m_rcModelingWnd;

	BLENDFUNCTION m_Blend;
	//��ʼ��ͼƬ�����ڵ���Ϣ
	BOOL Init();
	//����
	BOOL UpdateBk();
	//����32λλͼ
	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);
	CResLibData* m_pDataBase;
	CString m_sDragType;

public:
	//����Ŀ�ĵ���������
	void ClearDstRects();
	//����Ŀ�ĵ�(����Ϊ��ģҳ���е�����)
	void AddDstRect(CDragRect point);
	//��ʼ����������ģ���ڴ�С��������Ϊ��Ļ����
	void InitSize(CRect rcCanvasWnd, CRect rcModelingWnd);
	//�����϶��Ĺ��ߵ�ͼ��
	void SetDragToolImage(Gdiplus::Image* pToolImage);
	//��ʼ��ʾ��ק
	void StartDrag(CResLibData* pResLibDataBase);
	//������ʾ��ק
	void EndDrag(BOOL bClearSvgDraw = TRUE);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//��ʼ���϶�����
	BOOL InitDragData(CResLibData* pDataBase);
	BOOL InitDstRect();
	void PutJsonToSvg(CString sId, CString sType, CString sJson);
	void DrawMyImage(Gdiplus::Graphics &graph, Gdiplus::Image* m_pImage, const Gdiplus::RectF &oDstRect );
};
