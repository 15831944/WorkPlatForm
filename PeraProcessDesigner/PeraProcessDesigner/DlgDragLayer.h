#pragma once
#include "SvgDragLayer.h"


// CDlgDragLayer 对话框

class CDlgDragLayer : public CSvgDragLayer
{
	DECLARE_DYNAMIC(CDlgDragLayer)

public:
	CDlgDragLayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDragLayer();

// 对话框数据
	enum { IDD = IDD_DRAG_LAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
