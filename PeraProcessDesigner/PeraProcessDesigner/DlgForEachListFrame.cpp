// DlgForEachListFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgForEachListFrame.h"
#include "afxdialogex.h"
#include "DlgForEach.h"


// CDlgForEachListFrame 对话框

IMPLEMENT_DYNAMIC(CDlgForEachListFrame, CDlgForBase)

CDlgForEachListFrame::CDlgForEachListFrame(CWnd* pParent /*=NULL*/)
	: CDlgForBase(pParent)
{

}

CDlgForEachListFrame::~CDlgForEachListFrame()
{
}

void CDlgForEachListFrame::DoDataExchange(CDataExchange* pDX)
{
	CDlgForBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgForEachListFrame, CDlgForBase)
	ON_WM_VSCROLL()
	//ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgForEachListFrame 消息处理程序


void CDlgForEachListFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// Get the minimum and maximum scroll-bar positions.
	int minpos;
	int maxpos;
	GetScrollRange(SB_VERT, &minpos, &maxpos); 
	if(maxpos == 0) maxpos = 1;
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
		float fScale = float(curpos) / maxpos;

		CRect rect;
		GetClientRect(rect); //加入滚动条后重新获取高度
		int nHeightWnd = rect.Height();
		CDlgForEach* pDlg = ((CDlgForEach*)GetParent());
		int nHeightData = pDlg->m_vpComboBox.size() * COMBO_HEIGHT;


		pDlg->m_dlgForEachList.GetWindowRect(rect);
		rect.MoveToXY(0, 0);
		int nOffset = (int)(fScale * (nHeightData));
		rect.OffsetRect(0, -nOffset);
		pDlg->m_dlgForEachList.MoveWindow(rect);

// 		if(nSBCode != SB_THUMBTRACK)
// 			RefreshBranchPos();
		//Invalidate();
	}

	CDlgForBase::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDlgForEachListFrame::RefreshScrollBar()
{
	CDlgForEach* pDlg = (CDlgForEach*)GetParent();
	int nHeightData = pDlg->m_vpComboBox.size() * COMBO_HEIGHT; //数据总高;
	CRect rect;
	GetClientRect(rect);
	int nHeightWnd = rect.Height();         //窗口高度

	if(nHeightData > nHeightWnd)
	{
		ShowScrollBar(SB_VERT);


		//m_bScrollBarIsVisible = true;
		GetClientRect(rect); //加入滚动条后重新获取高度
		nHeightWnd = rect.Height();
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
			OnVScroll(SB_THUMBPOSITION, GetScrollPos(SB_VERT), 0);
		}
	}
	else
	{
		{
			OnVScroll(SB_THUMBPOSITION, 0, 0);
			ShowScrollBar(SB_VERT, FALSE);
			//GetParent()->SetTimer(0,0,0);
		}
	}
}

void CDlgForEachListFrame::ReSetScrollBar()
{
	SetScrollPos(SB_VERT, 0);
}

void CDlgForEachListFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDlgForBase::OnPaint()

	CBrush backBrush(RGB(251, 251, 251));
	//保存旧的brush
	CBrush* pOldBrush = dc.SelectObject(&backBrush);
	CRect rect;
	GetClientRect(rect);

	//画需要的区域
	dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	dc.SelectObject(pOldBrush);

}
