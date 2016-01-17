// WndIf.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "WndIf.h"


// CWndIf
CRect CWndIf::m_conRectCurBranch(0, 0, 650, 28);
const int CWndIf::m_conBlankH = 20;

IMPLEMENT_DYNAMIC(CWndIf, CWnd)

CWndIf::CWndIf():
 m_rectCurBranch(m_conRectCurBranch)
{
	m_nBranchSelect = -1;
}

CWndIf::~CWndIf()
{
	if(!m_vBCs.empty())
	{
		for(int i = 0; i < (int)m_vBCs.size(); i++)
		{
			delete m_vBCs[i];
			m_vBCs[i] = NULL;
		}
		m_vBCs.clear();
	}
}


BEGIN_MESSAGE_MAP(CWndIf, CWnd)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	//ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CWndIf 消息处理程序




int CWndIf::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	m_nWidth = lpCreateStruct->cx;
	m_nHeight = lpCreateStruct->cy;

	ShowScrollBar(SB_HORZ, FALSE);
	ShowScrollBar(SB_VERT, FALSE);

	CZEdit::m_staCount = 0;
	CZButton::m_staCount = 0;
// 	AddBranch("分支1", BranchCondition::TYPE_START);
// 	AddBranch("分支2", BranchCondition::TYPE_MIDDLE);
// 	AddBranch("分支3", BranchCondition::TYPE_MIDDLE);
// 	AddBranch("分支4", BranchCondition::TYPE_MIDDLE);
// 	AddBranch("分支5", BranchCondition::TYPE_MIDDLE);
// 	AddBranch("分支6", BranchCondition::TYPE_MIDDLE);
// 	AddBranch("分支7", BranchCondition::TYPE_MIDDLE);
// 	AddBranch("分支8", BranchCondition::TYPE_END);


	return 0;
}

bool CWndIf::AddBranch( const char* strExp, const char* strbranch, BranchCondition::TYPE type, BranchCondition::MODE mode)
{
	BranchCondition* pBC = new BranchCondition();
	m_vBCs.push_back(pBC);
	pBC->Create(m_rectCurBranch, this, strExp, strbranch, type, mode);
	m_rectCurBranch.OffsetRect(0, m_rectCurBranch.Height() + m_conBlankH);

	RefreshScrollBar(); //刷新滚动条

	return true;
}

void CWndIf::Resize( int nOffsetX, int nOffsetY )
{
	for(int i = 0; i < (int)m_vBCs.size(); i++)
	{
		m_vBCs[i]->Resize(nOffsetX, nOffsetY);
	}
	RefreshScrollBar();
	RefreshBranchPos();
	Invalidate();
}

void CWndIf::SetBranchPos(float fScaleX, float fScaleY)
{
	CRect rect;
	GetClientRect(rect);
	SCROLLBARINFO sbi;
	sbi.cbSize=sizeof(SCROLLBARINFO);
	GetScrollBarInfo(OBJID_HSCROLL, &sbi);
	CRect rectBar(sbi.rcScrollBar);

	int nWidthData = rect.Width() + rectBar.Height();
	int nHeightData = (int)m_vBCs.size() * (m_rectCurBranch.Height() + m_conBlankH); //数据总高

	m_rectCurBranch.MoveToXY(m_conRectCurBranch.left - (int)(nWidthData * fScaleX), m_conRectCurBranch.top - (int)(nHeightData * fScaleY));
	for(int i = 0; i < (int)m_vBCs.size(); i++)
	{
		m_vBCs[i]->SetPos(m_rectCurBranch.left, m_rectCurBranch.top);
		m_rectCurBranch.OffsetRect(0, m_rectCurBranch.Height() + m_conBlankH);
	}
}

void CWndIf::SetBranchMode(BranchCondition::MODE mode)
{
	for(int i = 0; i < (int)m_vBCs.size(); i++)
	{
		m_vBCs[i]->SetMode(mode);
	}
}

void CWndIf::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// Get the minimum and maximum scroll-bar positions.
	int minpos;
	int maxpos;
	GetScrollRange(SB_VERT, &minpos, &maxpos); 
	//maxpos = GetScrollLimit(SB_VERT);

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_VERT);

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_TOP:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_BOTTOM:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINEUP:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINEDOWN:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;

	case SB_PAGEUP:    // Scroll one page left.
		{
			// Get the page size. 
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);

			if (curpos > minpos)
				curpos = max(minpos, curpos - (int) info.nPage);
		}
		break;

	case SB_PAGEDOWN:      // Scroll one page right.
		{
			// Get the page size. 
			SCROLLINFO   info;
			GetScrollInfo(SB_VERT, &info, SIF_ALL);

			if (curpos < maxpos)
				curpos = min(maxpos, curpos + (int) info.nPage);
		}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).	
	if(true/*nSBCode != SB_THUMBTRACK*/)
	{
		SetScrollPos(SB_VERT, curpos);
		RefreshBranchPos();
		Invalidate();
	}


	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CWndIf::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

}



BOOL CWndIf::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ASSERT_VALID(pDC);

	CRect rectClient;
	GetClientRect(rectClient);

	CBrush m_brBkgr;
	m_brBkgr.CreateSolidBrush(RGB(240, 240, 240));

	if (m_brBkgr.GetSafeHandle() != NULL)
	{
		pDC->FillRect(rectClient, &m_brBkgr);
	}

	return CWnd::OnEraseBkgnd(pDC);
}

void CWndIf::ReSetScrollBar()
{
	SetScrollPos(SB_VERT, 0);
	SetScrollPos(SB_HORZ, 0);
	SetBranchPos(0.0f, 0.0f);
	Invalidate();
}

void CWndIf::RefreshBranchPos()
{
	float fX,fY;
	{//垂直
		int minpos;
		int maxpos;
		GetScrollRange(SB_VERT, &minpos, &maxpos); 
		int curpos = GetScrollPos(SB_VERT);
		//SetBranchPos(0.0f, float(curpos) / maxpos);
		fY = float(curpos) / maxpos;
	}
	{//水平
		int minpos;
		int maxpos;
		GetScrollRange(SB_HORZ, &minpos, &maxpos); 
		int curpos = GetScrollPos(SB_HORZ);
		//SetBranchPos(0.0f, float(curpos) / maxpos);
		fX = float(curpos) / maxpos;
	}
	SetBranchPos(fX, fY);
}

void CWndIf::RefreshScrollBar()
{
	int nHeightData = m_vBCs.size() * (m_rectCurBranch.Height() + m_conBlankH); //数据总高;
	CRect rect;
	GetClientRect(rect);
	int nHeightWnd = rect.Height();         //窗口高度

	if(nHeightData > nHeightWnd)
	{
		ShowScrollBar(SB_VERT);
		ShowScrollBar(SB_HORZ);
		{//垂直滚动
			SCROLLBARINFO sbi;
			sbi.cbSize=sizeof(SCROLLBARINFO);
			GetScrollBarInfo(OBJID_VSCROLL, &sbi);
			CRect rectBar(sbi.rcScrollBar);
			int nHeightChannel = rectBar.Height() - 2 * sbi.dxyLineButton; //通道高度
			float fScale = float(nHeightWnd) / nHeightData; //比例

			SCROLLINFO   info;
			info.nPage = (int)(nHeightChannel * fScale);
			info.nMin = 0;
			info.nMax = nHeightChannel;
			info.fMask = SIF_PAGE | SIF_RANGE;
			SetScrollInfo(SB_VERT, &info);
		}
		{//水平滚动				
			SCROLLBARINFO sbi;
			sbi.cbSize=sizeof(SCROLLBARINFO);
			GetScrollBarInfo(OBJID_HSCROLL, &sbi);
			CRect rectBar(sbi.rcScrollBar);
			int nWidthChannel = rectBar.Width() - 2 * sbi.dxyLineButton; //通道高度
			int nWidthWnd = rect.Width();
			int nWidthData = nWidthWnd + rectBar.Height();
			float fScale = float(nWidthWnd) / nWidthData; //比例

			SCROLLINFO   info;
			info.nPage = (int)(nWidthChannel * fScale);
			info.nMin = 0;
			info.nMax = nWidthChannel;
			info.fMask = SIF_PAGE | SIF_RANGE;
			SetScrollInfo(SB_HORZ, &info);
		}
	}
	else
	{
		ShowScrollBar(SB_VERT, FALSE);
		ShowScrollBar(SB_HORZ, FALSE);
		ReSetScrollBar();
	}
}


void CWndIf::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	int minpos;
	int maxpos;
	GetScrollRange(SB_HORZ, &minpos, &maxpos); 
	//maxpos = GetScrollLimit(SB_VERT);

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_HORZ);

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
		{
			// Get the page size. 
			SCROLLINFO   info;
			GetScrollInfo(SB_HORZ, &info, SIF_ALL);

			if (curpos > minpos)
				curpos = max(minpos, curpos - (int) info.nPage);
		}
		break;

	case SB_PAGERIGHT:      // Scroll one page right.
		{
			// Get the page size. 
			SCROLLINFO   info;
			GetScrollInfo(SB_HORZ, &info, SIF_ALL);

			if (curpos < maxpos)
				curpos = min(maxpos, curpos + (int) info.nPage);
		}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).	
	if(true/*nSBCode != SB_THUMBTRACK*/)
	{
		SetScrollPos(SB_HORZ, curpos);
		RefreshBranchPos();
		Invalidate();
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CWndIf::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDown(nFlags, point);

}
