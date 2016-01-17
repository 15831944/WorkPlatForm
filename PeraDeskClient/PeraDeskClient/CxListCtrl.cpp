// CxListCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CxListCtrl.h"

BEGIN_MESSAGE_MAP(CxListCtrl, CListCtrl)
	//������д�������ϢӰ��󣬶Ի����Ͻ����ڴ����б�ؼ����Ҽ�����¼���
	//��ӵĲ˵���Ϣ������Ҳ���ᱻ��Ӧ��
	//ON_NOTIFY_REFLECT(NM_RCLICK, &CxListCtrl::OnNMRClick)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CxListCtrl, CListCtrl)

CxListCtrl::CxListCtrl()
{
	m_bInited = FALSE;
	m_hwndRecvMsg = NULL;
	m_bReadOnly = FALSE;
}

CxListCtrl::~CxListCtrl()
{
}

BOOL CxListCtrl::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_RBUTTONDOWN )
	//{
	//	if(pMsg->hwnd == m_hWnd)
	//	{
	//		return TRUE;
	//	}
	//}
	//�ж��Ƿ�Ϊ������Ϣ
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		UINT nRepCnt = LOWORD(pMsg->lParam);
		UINT nFlags = HIWORD(pMsg->lParam);
		//�Ƿ�����������ϴΰ�ס��һֱû��̧������
		BOOL bHolding = ((nFlags & 0x4000) == 0x4000);

		//����� Ctrl + �� �������ƶ� Item
		if(GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == VK_DOWN 
			&& !bHolding && !m_bReadOnly)
		{
			OnItemMoveBack();
			return TRUE;
		}
		//����� Ctrl + �� �������ƶ� Item
		if(GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == VK_UP 
			&& !bHolding && !m_bReadOnly)
		{
			OnItemMoveForward();
			return TRUE;
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


BOOL CxListCtrl::Init( void )
{
	if ( m_bInited ) return FALSE;
	if ( !OnInit() ) return FALSE;
	m_bInited = TRUE;
	return TRUE;
}


void CxListCtrl::Release( void )
{
	m_vItemDataPtr.clear();
	m_bInited = FALSE;
	m_hwndRecvMsg = NULL;
}


BOOL CxListCtrl::OnInit( void )
{
	return TRUE;
}


void CxListCtrl::Clear( void )
{
	DeleteAllItems();
}

//�Ƿ�Ϊ����ģʽ
BOOL CxListCtrl::IsReportMode( void )
{
	DWORD dwStyle = GetStyle() & LVS_TYPEMASK;
	if (dwStyle == LVS_REPORT) return TRUE;
	return FALSE;
}

//�Ƿ�ΪͼƬģʽ
BOOL CxListCtrl::IsImageMode( void )
{
	DWORD dwStyle = GetStyle() & LVS_TYPEMASK;
	if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON) return TRUE;
	return FALSE;
}

/*
	����Ϊ����ģʽ��
������
	lStyle��
	lExStyle��
*/
BOOL CxListCtrl::SetReportMode( LONG lStyle, LONG lExStyle )
{
	if (IsInitialed() && IsImageMode()) return FALSE;
	if ( !IsWindow(m_hWnd) ) return FALSE;

	//LVS_SHOWSELALWAYS //û�н��㣬Ҳ������ʾѡ�е������ʧȥ���㣬ѡ����ظ���ͨ״̬��
	//LONG lExStyle = LVS_EX_FULLROWSELECT //ȫ��ѡ��
	DWORD lNewStyle = 0;
	lNewStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lNewStyle &= ~LVS_TYPEMASK;
	lNewStyle |= LVS_REPORT; //����Ϊ ���� ģʽ
	lNewStyle |= lStyle;
	SetWindowLong(m_hWnd, GWL_STYLE, lNewStyle);

	//�⼸�仰�����ã���֪��ԭ��
	//LONG lExStyle = 0;
	//lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	//lExStyle |= LVS_EX_FULLROWSELECT; //ȫ��ѡ��
	//lExStyle |= LVS_EX_GRIDLINES; //����
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

	DWORD dwNewExStyle = 0;
	dwNewExStyle |= lExStyle;
	SetExtendedStyle(dwNewExStyle);

	return TRUE;
}

/*
	����ΪͼƬģʽ��
������
	lStyle��
	lExStyle��
*/
BOOL CxListCtrl::SetImageMode( LONG lStyle, LONG lExStyle )
{
	if (IsInitialed() && IsReportMode()) return FALSE;
	if ( !IsWindow(m_hWnd) ) return FALSE;

	LONG lNewStyle;
	lNewStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lNewStyle &= ~LVS_TYPEMASK;
	lNewStyle |= LVS_ICON; //���� ��ͼ�� ģʽ
	lNewStyle &= ~LVS_ALIGNMASK;
	lNewStyle |= LVS_ALIGNTOP; //���� �϶���
	lNewStyle |= LVS_AUTOARRANGE; // ��������һ�к��Զ�����
	//lNewStyle |= LVS_SHOWSELALWAYS; //û�н��㣬Ҳ������ʾѡ�е������ʧȥ���㣬ѡ����ظ���ͨ״̬��
	//lNewStyle |= LVS_OWNERDRAWFIXED; //
	lNewStyle |= lStyle;
	SetWindowLong(m_hWnd, GWL_STYLE, lNewStyle);

	//�����б���չ����
	DWORD dwNewExStyle = 0;
	//dwNewExStyle |= LVS_EX_FULLROWSELECT;
	//dwNewExStyle |= LVS_EX_GRIDLINES;
	dwNewExStyle |= lExStyle;
	SetExtendedStyle(dwNewExStyle);
	return TRUE;
}

/*
	ȡ�õ�ǰѡ����Ŀ�������ţ�˳��
*/
int CxListCtrl::GetCurSel( vector<int>& vItems )
{
	POSITION pos = NULL;
	int nItem = -1;

	vItems.clear();

	pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		nItem = GetNextSelectedItem(pos);
		vItems.push_back(nItem);
	}
	sort(vItems.begin(), vItems.end());
	return (int)vItems.size();
}

//ȡ�õ�ǰѡ����Ŀ�Ĳ�����˳��
int CxListCtrl::GetCurSelParam( vector<LPARAM>& vParams )
{
	vector<int> vItems;
	LPARAM lParam = 0;

	vParams.clear();
	if (0 == GetCurSel(vItems)) return 0;
	for (int i = (int)vItems.size() - 1; i >= 0; i--)
	{
		lParam = GetItemData(vItems[i]);
		vParams.push_back(lParam);
	}
	return (int)vParams.size();
}

//��ָ�������ŵ���Ŀ����Ϊѡ��״̬
void CxListCtrl::SetCurSel( vector<int>& vItems )
{
	POSITION pos = NULL;
	int nItem = -1;

	ClearCurSel();

	SetFocus(); //������䣬�޷�ʵʱ����״̬

	sort(vItems.begin(), vItems.end());

	nItem = -1;
	for (int i=0; i<(int)vItems.size(); i++)
	{
		SetItemState(vItems[i], LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		RedrawItems(nItem, nItem);
		nItem = vItems[i];
	}
	if (nItem != -1)
	{
		EnsureVisible(nItem, FALSE);
	}
}

//���ѡ��״̬��
int CxListCtrl::ClearCurSel( vector<int> * pvItemIndex )
{
	POSITION pos = NULL;
	vector<int> vSel;
	int nItem = -1;

	GetCurSel(vSel);

	if (pvItemIndex != NULL)
	{
		pvItemIndex->clear();
		*pvItemIndex = vSel;
	}

	pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		nItem = GetNextSelectedItem(pos);
		SetItemState(nItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
		RedrawItems(nItem, nItem); //ֻ��ͼƬ��Ҫ��䣿
	}

	return (int)vSel.size();
}

//���� ��ѡ ���� ��ѡ ģʽ
BOOL CxListCtrl::SetMultiSelMode( BOOL bEnabled /*= TRUE*/ )
{
	LONG lStyle;
	lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if (bEnabled)
		lStyle &= ~LVS_SINGLESEL;
	else
		lStyle |= LVS_SINGLESEL;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	return TRUE;
}

int CxListCtrl::DeleteCurSel( vector<LPARAM> * pvParams )
{
	vector<int> vSel;
	DWORD dwStyle = GetStyle();

	if (pvParams != NULL)
	{
		pvParams->clear();
	}

	if (0 == GetCurSel(vSel)) return 0;

	if (pvParams != NULL)
	{
		for (int i=0; i<(int)vSel.size(); i++)
		{
			pvParams->push_back( GetItemData( vSel[i] ) );
		}
	}

	for (int i = (int)vSel.size() - 1; i >= 0; i--)
	{
		DeleteItem(vSel[i]);
	}

	if (IsImageMode())
	{
		Arrange(LVS_ALIGNLEFT); //������䣬���治��ˢ��
	}
	return (int)vSel.size();
}

//
BOOL CxListCtrl::GetClickItemPos(int& nRow, int& nCol)
{
	nRow = -1;
	nCol = -1;

	LVHITTESTINFO hti;
	memset(&hti, NULL, sizeof(LVHITTESTINFO));
	hti.flags = LVHT_ABOVE;

	GetCursorPos(&(hti.pt));
	ScreenToClient(&hti.pt);
	
	//��� Item �Ƿ�ѡ��
	nRow = HitTest(&hti);
	if (nRow >= 0)
	{
		nCol = hti.iSubItem;
		return TRUE;
	}

	//��� SubItem �Ƿ�ѡ�� ������� LVS_EX_FULLROWSELECT ��־����������Ч��
	nRow = SubItemHitTest(&hti);
	if (nRow >= 0)
	{
		nCol = hti.iSubItem;
		return TRUE;
	}

	return FALSE;
}

int CxListCtrl::AddRow( LPCTSTR lpszText, LPARAM lpParam /*= NULL*/, int nRow /*= -1*/ )
{
	CString strText = lpszText == NULL ? "" : lpszText;
	if (!m_bInited) return -1;

	LVITEM lvi;
	memset(&lvi, NULL, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iItem = nRow >= 0 ? nRow : GetItemCount();
	lvi.pszText = (LPTSTR)(LPCTSTR)strText;
	lvi.lParam = lpParam;
	return InsertItem(&lvi);
	//return GetItemCount();
}

BOOL CxListCtrl::AddColumn( int nRow, int nCol, LPCTSTR lpszText )
{
	return SetItemText(nRow, nCol, lpszText);
	//LVITEM lvi;
	//memset(&lvi, NULL, sizeof(LVITEM));
	//lvi.mask = LVIF_PARAM | LVIF_TEXT | LVIF_COLUMNS;
	//lvi.lParam = lpParam;
	//lvi.pszText = (LPSTR)(LPCTSTR)lpszText;
	//lvi.cColumns = nCol;
	//lvi.iItem = nRow;
	//nRow = InsertItem(&lvi);
	//if (nRow != -1) return TRUE;
	//else return FALSE;
}

void CxListCtrl::OnItemMoveForward( void )
{
	vector<int> vCurSel, vNewSel;
	LPVOID lpData = NULL;

	//��ǰѡ�е��б���
	if (0 == GetCurSel(vCurSel)) return;
	//���µ���˳��������б�ȷ�� m_vItemDataPtr �е����� ���б��е�����һһ��Ӧ
	if (0 == OnUpdateItemDataPtr()) return;
	vNewSel = vCurSel;

	//˳�����
	for (int i=0; i<(int)vCurSel.size(); i++)
	{
		if (0 == vCurSel[i]) continue; //�Ѿ��ǵ�һ����¼�������ƶ�
		//ǰ�󽻻�
		lpData = m_vItemDataPtr[ vCurSel[i] ];
		m_vItemDataPtr[ vCurSel[i] ] = m_vItemDataPtr[ vCurSel[i] - 1 ];
		m_vItemDataPtr[ vCurSel[i] - 1 ] = lpData;
		vNewSel[i] = vNewSel[i] - 1;
	}
	OnItemMoved(vNewSel);
}

void CxListCtrl::OnItemMoveBack( void )
{
	vector<int> vCurSel, vNewSel;
	LPVOID lpData = NULL;

	//��ǰѡ�е��б���
	if (0 == GetCurSel(vCurSel)) return;
	//���µ���˳��������б�ȷ�� m_vItemDataPtr �е����� ���б��е�����һһ��Ӧ
	if (0 == OnUpdateItemDataPtr()) return;
	vNewSel = vCurSel;

	//�������
	for (int i = vCurSel.size() - 1; i >= 0; i--)
	{
		if (GetItemCount() - 1 <= vCurSel[i]) continue; //�Ѿ������һ����¼�������ƶ�
		lpData = m_vItemDataPtr[ vCurSel[i] ];
		m_vItemDataPtr[ vCurSel[i] ] = m_vItemDataPtr[ vCurSel[i] + 1 ];
		m_vItemDataPtr[ vCurSel[i] + 1 ] = lpData;
		vNewSel[i] = vNewSel[i] + 1;
	}
	OnItemMoved(vNewSel);
}

int CxListCtrl::OnUpdateItemDataPtr( void )
{
	return 0;
}

void CxListCtrl::OnItemMoved( vector<int>& vNewSel )
{

}
