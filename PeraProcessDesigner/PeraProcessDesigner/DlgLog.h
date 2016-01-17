#pragma once
#include "dlginfoedit.h"


#define WM_LOG WM_USER + 9

class CDlgLog :
	public CDlgInfoEdit
{
	DECLARE_DYNAMIC(CDlgLog)

public:
	CDlgLog(std::string str = "", std::string strCaption = "", int nMax = -1, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLog(void);

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg LRESULT OnLog(WPARAM wParam, LPARAM lParam);
};

