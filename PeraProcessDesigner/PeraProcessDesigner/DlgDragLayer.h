#pragma once
#include "SvgDragLayer.h"


// CDlgDragLayer �Ի���

class CDlgDragLayer : public CSvgDragLayer
{
	DECLARE_DYNAMIC(CDlgDragLayer)

public:
	CDlgDragLayer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDragLayer();

// �Ի�������
	enum { IDD = IDD_DRAG_LAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
