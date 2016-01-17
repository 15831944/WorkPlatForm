// WndIf.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgWndIf.h"


// CDlgWndIf
CRect CDlgWndIf::m_conRectCurBranch(0, 0, 650, 25);
const int CDlgWndIf::m_conBlankH = 10;

IMPLEMENT_DYNAMIC(CDlgWndIf, CBCGPDialog)

CDlgWndIf::CDlgWndIf(CWnd* pParent /*=NULL*/)
  : CBCGPDialog(CDlgWndIf::IDD, pParent), 
  m_rectCurBranch(m_conRectCurBranch),
  m_bScrollBarIsVisible(false)
{
	m_nBranchSelect = -1;
}

CDlgWndIf::~CDlgWndIf()
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


BEGIN_MESSAGE_MAP(CDlgWndIf, CBCGPDialog)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	//ON_WM_PAINT()
	//ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CDlgWndIf ��Ϣ�������




int CDlgWndIf::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	m_nWidth = lpCreateStruct->cx;
	m_nHeight = lpCreateStruct->cy;

	ShowScrollBar(SB_HORZ, FALSE);
	ShowScrollBar(SB_VERT, FALSE);

	CZEdit::m_staCount = 0;
	CZButton::m_staCount = 0;
	// 	AddBranch("��֧1", BranchCondition::TYPE_START);
	// 	AddBranch("��֧2", BranchCondition::TYPE_MIDDLE);
	// 	AddBranch("��֧3", BranchCondition::TYPE_MIDDLE);
	// 	AddBranch("��֧4", BranchCondition::TYPE_MIDDLE);
	// 	AddBranch("��֧5", BranchCondition::TYPE_MIDDLE);
	// 	AddBranch("��֧6", BranchCondition::TYPE_MIDDLE);
	// 	AddBranch("��֧7", BranchCondition::TYPE_MIDDLE);
	// 	AddBranch("��֧8", BranchCondition::TYPE_END);

	EnableVisualManagerStyle (TRUE, TRUE);

	return 0;
}

bool CDlgWndIf::AddBranch( const char* strExp, const char* strbranch, BranchCondition::TYPE type, BranchCondition::MODE mode)
{
	BranchCondition* pBC = new BranchCondition();
	m_vBCs.push_back(pBC);
	BOOL bOK = pBC->Create(m_rectCurBranch, this, strExp, strbranch, type, mode);
	if (!bOK)
		return false;

	m_rectCurBranch.OffsetRect(0, m_rectCurBranch.Height() + m_conBlankH);

	RefreshScrollBar(); //ˢ�¹�����
	EnableVisualManagerStyle (TRUE, TRUE);

	return true;
}

void CDlgWndIf::Resize( int nOffsetX, int nOffsetY )
{
	for(int i = 0; i < (int)m_vBCs.size(); i++)
	{
		m_vBCs[i]->Resize(nOffsetX, nOffsetY);
	}
	RefreshScrollBar();
	RefreshBranchPos();
	//Invalidate();
}

void CDlgWndIf::SetBranchPos(float fScaleX, float fScaleY)
{
	CRect rect;
	GetClientRect(rect);
	SCROLLBARINFO sbi;
	sbi.cbSize=sizeof(SCROLLBARINFO);
	GetScrollBarInfo(OBJID_HSCROLL, &sbi);
	CRect rectBar(sbi.rcScrollBar);

	int nWidthData = rect.Width() + rectBar.Height();
	int nHeightData = (int)m_vBCs.size() * (m_rectCurBranch.Height() + m_conBlankH); //�����ܸ�

	m_rectCurBranch.MoveToXY(m_conRectCurBranch.left - (int)(nWidthData * fScaleX), m_conRectCurBranch.top - (int)(nHeightData * fScaleY));
	for(int i = 0; i < (int)m_vBCs.size(); i++)
	{
		m_vBCs[i]->SetPos(m_rectCurBranch.left, m_rectCurBranch.top);
		m_rectCurBranch.OffsetRect(0, m_rectCurBranch.Height() + m_conBlankH);
	}
	//Invalidate();
}

void CDlgWndIf::SetBranchMode(BranchCondition::MODE mode)
{
	for(int i = 0; i < (int)m_vBCs.size(); i++)
	{
		m_vBCs[i]->SetMode(mode);
	}
}

void CDlgWndIf::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
		if(nSBCode != SB_THUMBTRACK)
			RefreshBranchPos();
		//Invalidate();
	}


	CBCGPDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CDlgWndIf::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()

}



BOOL CDlgWndIf::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ASSERT_VALID(pDC);

	CRect rectClient;
	GetClientRect(rectClient);

	CBrush m_brBkgr;
	m_brBkgr.CreateSolidBrush(RGB(240, 240, 240));

	if (m_brBkgr.GetSafeHandle() != NULL)
	{
		pDC->FillRect(rectClient, &m_brBkgr);
	}

	return CBCGPDialog::OnEraseBkgnd(pDC);
}

void CDlgWndIf::ReSetScrollBar()
{
	SetScrollPos(SB_VERT, 0);

#ifdef USE_SCROLLBAR_H
	SetScrollPos(SB_HORZ, 0);
#endif // USE_SCROLLBAR_H

	SetBranchPos(0.0f, 0.0f);
	//Invalidate();
}

void CDlgWndIf::RefreshBranchPos()
{
	if(!m_bScrollBarIsVisible)return;
	float fX = 0.0f, fY = 0.0f;
	{//��ֱ
		int minpos;
		int maxpos;
		GetScrollRange(SB_VERT, &minpos, &maxpos); 
		int curpos = GetScrollPos(SB_VERT);
		//SetBranchPos(0.0f, float(curpos) / maxpos);
		fY = float(curpos) / maxpos;
	}

#ifdef USE_SCROLLBAR_H
	{//ˮƽ
		int minpos;
		int maxpos;
		GetScrollRange(SB_HORZ, &minpos, &maxpos); 
		int curpos = GetScrollPos(SB_HORZ);
		//SetBranchPos(0.0f, float(curpos) / maxpos);
		fX = float(curpos) / maxpos;
	}
#endif // USE_SCROLLBAR_H

	SetBranchPos(fX, fY);
}

void CDlgWndIf::RefreshScrollBar()
{
	int nHeightData = m_vBCs.size() * (m_rectCurBranch.Height() + m_conBlankH); //�����ܸ�;
	CRect rect;
	GetClientRect(rect);
	int nHeightWnd = rect.Height();         //���ڸ߶�

	if(nHeightData > nHeightWnd)
	{
		ShowScrollBar(SB_VERT);
		
#ifdef USE_SCROLLBAR_H
		ShowScrollBar(SB_HORZ);
#endif // USE_SCROLLBAR_H

		m_bScrollBarIsVisible = true;
		GetClientRect(rect); //��������������»�ȡ�߶�
		nHeightWnd = rect.Height();
		{//��ֱ����
			SCROLLBARINFO sbi;
			sbi.cbSize=sizeof(SCROLLBARINFO);
			GetScrollBarInfo(OBJID_VSCROLL, &sbi);
			CRect rectBar(sbi.rcScrollBar);
			int nHeightChannel = rectBar.Height() - 2 * sbi.dxyLineButton; //ͨ���߶�
			float fScale = float(nHeightWnd) / nHeightData; //����

			SCROLLINFO   info;
			info.nPage = (int)(nHeightChannel * fScale);
			info.nMin = 0;
			info.nMax = nHeightChannel;
			info.fMask = SIF_PAGE | SIF_RANGE;
			SetScrollInfo(SB_VERT, &info);
		}
#ifdef USE_SCROLLBAR_H
		{//ˮƽ����				
			SCROLLBARINFO sbi;
			sbi.cbSize=sizeof(SCROLLBARINFO);
			GetScrollBarInfo(OBJID_HSCROLL, &sbi);
			CRect rectBar(sbi.rcScrollBar);
			int nWidthChannel = rectBar.Width() - 2 * sbi.dxyLineButton; //ͨ���߶�
			int nWidthWnd = rect.Width();
			int nWidthData = nWidthWnd + rectBar.Height();
			float fScale = float(nWidthWnd) / nWidthData; //����

			SCROLLINFO   info;
			info.nPage = (int)(nWidthChannel * fScale);
			info.nMin = 0;
			info.nMax = nWidthChannel;
			info.fMask = SIF_PAGE | SIF_RANGE;
			SetScrollInfo(SB_HORZ, &info);
		}
#endif // USE_SCROLLBAR_H
	}
	else
	{
		ShowScrollBar(SB_VERT, FALSE);
#ifdef USE_SCROLLBAR_H
		ShowScrollBar(SB_HORZ, FALSE);
#endif // USE_SCROLLBAR_H
		m_bScrollBarIsVisible = false;
		ReSetScrollBar();
	}
}


void CDlgWndIf::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
		//Invalidate();
	}

	CBCGPDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CDlgWndIf::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CBCGPDialog::OnLButtonDown(nFlags, point);

}


BOOL CDlgWndIf::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if(m_bScrollBarIsVisible == false) return CBCGPDialog::OnMouseWheel(nFlags, zDelta, pt); //δ��ʾ��ֱ�������򷵻�

	BOOL bUp = TRUE;
	int nDelta = zDelta;
	if(zDelta < 0) {
		bUp = FALSE;
		nDelta = -nDelta;
	}
	UINT nWheelScrollLines;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0,
		&nWheelScrollLines, 0);
	int nLines = (nDelta * nWheelScrollLines) / WHEEL_DELTA;
	while(nLines--)
		SendMessage(WM_VSCROLL, MAKEWPARAM(bUp ? SB_LINEUP : SB_LINEDOWN, 0), 0);


	return CBCGPDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CDlgWndIf::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CBCGPDialog::OnOK();
}


BOOL CDlgWndIf::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	ASSERT(pMsg != NULL);
	/*if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP)
	{ 
		if (IsWindow(m_ToolTipCtrl.m_hWnd))
		m_ToolTipCtrl.RelayEvent(pMsg);
	}*/
	return CBCGPDialog::PreTranslateMessage(pMsg);
}
