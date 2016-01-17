/*
	完成由C++向SVG Viewer拖控件的界面效果实现
	使用时添加对话框资源及类，将基类由CDialog改为CSvgDragLayer，并将资源对话框改为无边框
	需要gdi+初始化
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

//比较CPoint大小的仿函数
class CDragRectSort //充当排序准则的仿函数  
{  
public:  
	bool operator()(const CDragRect &rc1,const CDragRect &rc2)  
	{  
		return rc1.m_sId > rc2.m_sId;
	}  
};  

// CSvgDragLayer 对话框

class CSvgDragLayer : public CDialog
{
	DECLARE_DYNAMIC(CSvgDragLayer)

public:
	CSvgDragLayer(UINT nID, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSvgDragLayer();

// 对话框数据
//	enum { IDD = IDD_SVGDRAGLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	//目的点图标及坐标数组
	Gdiplus::Image* m_pDstImage;
	Gdiplus::Image* m_pDstFormImage;
	std::set<CDragRect, CDragRectSort> m_setPointDstImage;//该数组在后续开发阶段如果需要绘方形表单高亮及缩放图片时，最好和页面开发商量，返回每个高亮点应在的rect
	//标识当前鼠标是否在SVG区域中，如不在，不需绘目的点
	BOOL m_bPointInSvg;
	//高亮点图标及坐标
	Gdiplus::Image* m_pHighlightImage;
	Gdiplus::Image* m_pHighlightFormImage;
	CDragRect m_rectHighlightImage;
	//拖动的工具图标及坐标
	Gdiplus::Image* m_pDragToolImage;
	CPoint m_pointDragToolImage;
	//本层窗口应在的位置（屏幕坐标）
	CRect m_rcCanvasWnd;
	//建模静态页面的区域（相对于画布的坐标）
	CRect m_rcModelingWnd;

	BLENDFUNCTION m_Blend;
	//初始化图片、窗口等信息
	BOOL Init();
	//绘制
	BOOL UpdateBk();
	//创建32位位图
	HBITMAP KCreateCompatibleBitmap(int nWidth, int nHeight);
	CResLibData* m_pDataBase;
	CString m_sDragType;

public:
	//重置目的点坐标数组
	void ClearDstRects();
	//增加目的点(坐标为建模页面中的坐标)
	void AddDstRect(CDragRect point);
	//初始化画布及建模窗口大小，参数均为屏幕坐标
	void InitSize(CRect rcCanvasWnd, CRect rcModelingWnd);
	//设置拖动的工具的图标
	void SetDragToolImage(Gdiplus::Image* pToolImage);
	//开始显示拖拽
	void StartDrag(CResLibData* pResLibDataBase);
	//结束显示拖拽
	void EndDrag(BOOL bClearSvgDraw = TRUE);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//初始化拖动数据
	BOOL InitDragData(CResLibData* pDataBase);
	BOOL InitDstRect();
	void PutJsonToSvg(CString sId, CString sType, CString sJson);
	void DrawMyImage(Gdiplus::Graphics &graph, Gdiplus::Image* m_pImage, const Gdiplus::RectF &oDstRect );
};
