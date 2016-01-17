#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "DlgFor.h"
#include "DlgForStep.h"
#include "DlgForArray.h"
#include "DlgForEach.h"
#include "DlgWhile.h"
#include "DlgDoWhile.h"

LPCTSTR CDlgFor::m_strForType[] = {
	"For Step",
	"For Array",
	"For Each",
	"While",
	"Do While"
};

IMPLEMENT_DYNAMIC(CDlgFor, CDlgParamSetBase)

CDlgFor::CDlgFor( LPCTSTR str, CWnd* pParent /*=NULL*/ )
	: CDlgParamSetBase(str, pParent)
{
	memset(m_pDlgFor, NULL, sizeof(CDlgForBase*) * Count);
}

CDlgFor::~CDlgFor(void)
{
	for(int i = 0; i < Count; i++)
	{
		if(m_pDlgFor[i])
		{
			delete m_pDlgFor[i];
			m_pDlgFor[i] = NULL;
		}
	}
}

void CDlgFor::HideUselessControl()
{
	GetDlgItem(IDC_STATIC_WEB)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RELATED_GRID)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PARAM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_RELATE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_DELETE3)->ShowWindow(SW_HIDE);
}

void CDlgFor::CreateForFrame()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(rect);
	ScreenToClient(rect);
	CRect rectTree;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rectTree);
	ScreenToClient(rectTree);
	CRect rectBtn;
	m_btnDelete.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);
	m_rectSel = rect;
	rect.top = rectBtn.bottom + 1;
	m_rectSel.bottom = rect.top + 1;
	rect.bottom = rectTree.bottom - 100;
	m_rectForFrame = rect;

	rect.top = m_rectForFrame.bottom - 1;
	rect.bottom = rectTree.bottom;
	m_rectLimit = rect;

	m_rectLimitText = m_rectLimit;

	m_rectLimit.top += 30;
	m_rectLimitText.bottom = m_rectLimit.top + 1;

	m_staSel.Create(NULL, WS_VISIBLE | WS_BORDER, m_rectSel, this);
	m_staForFrame.Create(NULL, WS_VISIBLE | WS_BORDER, m_rectForFrame, this);
	m_staLimitText.Create(NULL, WS_VISIBLE | WS_BORDER, m_rectLimitText, this);
	m_staLimit.Create(NULL, WS_VISIBLE | WS_BORDER, m_rectLimit, this);
	m_rectLimit.DeflateRect(1, 1, 1, 1);
	m_dlgForLimit.Create(IDD_FOR_LIMIT, this);
	m_dlgForLimit.MoveWindow(m_rectLimit);
	m_dlgForLimit.ShowWindow(SW_SHOW);

	m_rectForFrame.DeflateRect(1, 1, 1, 1);


	CClientDC dc(this);
	CString strText;
	CSize sz;

	strText = "循环限制";
	sz = dc.GetTextExtent(strText); 
	rect = m_rectLimitText;
	rect.top += 10;
	rect.left += 20;
	rect.right = rect.left + sz.cx;
	rect.bottom = rect.top + sz.cy;
	m_staLimitStr.Create(strText, WS_VISIBLE, rect, this);
	m_staLimitStr.SetFont(GetFont());

	strText = "请选择循环条件：";
	sz = dc.GetTextExtent(strText); 
	rect = m_rectSel;
	rect.top += 10;
	rect.left += 20;
	rect.right = rect.left + sz.cx;
	rect.bottom = rect.top + sz.cy;
	m_staSelText.Create(strText, WS_VISIBLE, rect, this);
	m_staSelText.SetFont(GetFont());

	rect.OffsetRect(rect.Width() - 10, -3);
	rect.right += 100;

	m_boxFor.Create(WS_VISIBLE | CBS_DROPDOWNLIST, rect, this, IDC_FOR_TYPE);
	m_boxFor.SetFont(GetFont());
	for(int i = 0; i < Count; i++)
	{
		m_boxFor.AddString(m_strForType[i]);
	}
	m_boxFor.SelectString(0, m_strForType[m_curForType]);
	EnableVisualManagerStyle (TRUE, TRUE);

	for(int i = 0; i < Count; i++)
	{
		switch (i)
		{
		case ForStep:
			m_pDlgFor[i] = new CDlgForStep(this);
			m_pDlgFor[i]->Create(IDD_FOR_STEP, this);
			break;
		case ForArray:
			m_pDlgFor[i] = new CDlgForArray(this);
			m_pDlgFor[i]->Create(IDD_FOR_ARRAY, this);
			break;
		case ForEach:
			m_pDlgFor[i] = new CDlgForEach(this);
			m_pDlgFor[i]->Create(IDD_FOR_EACH, this);
			break;
		case While:
			m_pDlgFor[i] = new CDlgWhile(this);
			m_pDlgFor[i]->Create(IDD_WHILE, this);
			break;
		case DoWhile:
			m_pDlgFor[i] = new CDlgDoWhile(this);
			m_pDlgFor[i]->Create(IDD_WHILE, this);
			break;
		}
		m_pDlgFor[i]->MoveWindow(m_rectForFrame);
		m_wndGridTree.AddListener(m_pDlgFor[i]); //参数事件监听
	}

	for(int i = 0; i < (int)m_wndGridTree.m_vParams.size(); i++)
	{
		for(int j = 0; j < (int)m_wndGridTree.m_vListeners.size(); j++)
		{
			m_wndGridTree.m_vListeners[j]->AddParam(m_wndGridTree.m_vParams[i]->m_strName.c_str());
		}
	}

	ActivateDlgForByIndex(GetCurForType());

}

void CDlgFor::ActivateDlgForByIndex(ForType type)
{
	for(int i = 0; i < Count; i++)
	{
		if(i == type)
		{
			m_pDlgFor[i]->ShowWindow(SW_SHOW);
		}
		else
		{
			m_pDlgFor[i]->ShowWindow(SW_HIDE);
		}
	}
}

BOOL CDlgFor::OnInitDialog()
{
	CDlgParamSetBase::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetWindowText("循环设置");

	HideUselessControl(); //隐藏无用的控件

	SetCurForType(ForStep);

	CreateForFrame();



	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
BEGIN_MESSAGE_MAP(CDlgFor, CDlgParamSetBase)
	ON_WM_MOVE()
	//ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_FOR_TYPE, &CDlgFor::OnCbnSelchangeComboFor)
	ON_BN_CLICKED(IDOK1, &CDlgFor::OnBnClickedOk1)
END_MESSAGE_MAP()


void CDlgFor::OnMove(int x, int y)
{
	CDlgParamSetBase::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
}


void CDlgFor::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CCxBCGPDialog::OnPaint()
	CRect rect;

	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rect);
	ScreenToClient(rect);
	CRect rectBtn;
	m_btnDelete.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);

	CRect rectWeb;
	GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(rectWeb);
	ScreenToClient(rectWeb);
	CRect rectDraw;
	rectDraw.left = rect.left + 1;
	rectDraw.right = rect.right - 1;
	rectDraw.top = rectWeb.bottom + 38;
	rectDraw.bottom = rect.bottom - 1;
	CBrush brush(RGB(251, 251, 251));
	dc.FillRect (rectDraw, &brush);

	CRect rectFor = m_rectForFrame;
	rectFor.DeflateRect(1, 1, 1, 1);
	dc.FillRect(rectFor, &brush); 	//填充循环区域

	CPen pen(0, 1, RGB(151,151,151));
	dc.SelectObject(pen);
	CRect rectParamName;
	GetDlgItem(IDC_STATIC_PARAM_NAME)->GetWindowRect(rectParamName);
	ScreenToClient(rectParamName);
	dc.MoveTo(rectDraw.left, rectParamName.top - 7);
	dc.LineTo(rectDraw.right, rectParamName.top - 7);
}

void CDlgFor::OnCbnSelchangeComboFor()
{
	CString str;
	int n = ((CComboBox*)GetDlgItem(IDC_FOR_TYPE))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_FOR_TYPE))->GetLBText(n, str);

	SetCurForType((ForType)n);
	ActivateDlgForByIndex((ForType)n);
}


void CDlgFor::OnBnClickedOk1()
{
	// TODO: 在此添加控件通知处理程序代码
	SharedData sharedData;
	sharedData = GetData(); //共享数据
	if(m_curForType == ForStep)
	{
		ForStepData data; //ForStep数据
		data = ((CDlgForStep*)m_pDlgFor[m_curForType])->GetData();
		return;
	}
}


SharedData CDlgFor::GetData()
{
	SharedData data;

	data.strType = m_strForType[m_curForType];
	data.bContinue = m_dlgForLimit.m_bContinue;
	data.nConut = m_dlgForLimit.m_nConut;

	return data;
}