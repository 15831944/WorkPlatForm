#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "DlgLog.h"


IMPLEMENT_DYNAMIC(CDlgLog, CDlgInfoEdit)

CDlgLog::CDlgLog(std::string str, std::string strCaption, int nMax, CWnd* pParent /*=NULL*/)
	: CDlgInfoEdit(str, strCaption, nMax, pParent)
{
}

CDlgLog::~CDlgLog(void)
{
}


BEGIN_MESSAGE_MAP(CDlgLog, CDlgInfoEdit)
	ON_MESSAGE(WM_LOG, &CDlgLog::OnLog)
END_MESSAGE_MAP()

afx_msg LRESULT CDlgLog::OnLog(WPARAM wParam, LPARAM lParam)
{
	std::string str = *(CString*)wParam;
	delete (CString*)wParam;

	((CEdit*)GetDlgItem(IDC_EDIT))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_EDIT))->ReplaceSel((str + '\n').c_str());
	((CEdit*)GetDlgItem(IDC_EDIT))->SetSel(-1, -1);

	return 0;
}
