#include "StdAfx.h"
#include "IfEditTree.h"
#include "CxStructTreeNodeParam.h"

CIfEditTree::CIfEditTree(void)
{
	m_hWndRecvDbClichMsg = NULL;
}


CIfEditTree::~CIfEditTree(void)
{
}
BEGIN_MESSAGE_MAP(CIfEditTree, CZGridCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CIfEditTree, CZGridCtrl)
void CIfEditTree::SetMessReveiveWnd(HWND hRecvWnd)
{
	if ( hRecvWnd!=NULL && IsWindow(hRecvWnd))
		m_hWndRecvDbClichMsg = hRecvWnd;
	else
		m_hWndRecvDbClichMsg = NULL;
}
HWND CIfEditTree::GetMessReveiveWnd()
{
	return m_hWndRecvDbClichMsg;
}
void CIfEditTree::DrawTreeLine(CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset)
{
	if ( pCurRow == NULL ) return;
	if ( rcLeftOffset.IsRectEmpty() || rcLeftOffset.IsRectNull() ) return;
	CCxStructTreeNode * pCurNode = reinterpret_cast<CCxStructTreeNode*>(pCurRow->GetData());
	if ( pCurNode == NULL ) return;

	CRect rc = rcLeftOffset;
	rc.right = rc.left + rc.Height();
	int nCount = rcLeftOffset.Width() / rc.Width();
	CPoint ptCenter;

	//CPen penLine (PS_DASHDOT, 1, globalData.clrBarText);

	CBrush Brush(RGB(128, 128, 128));
	LOGBRUSH lBrush = { 0 };
	Brush.GetLogBrush( &lBrush );

	DWORD Style[] = { 0, 2 };   
	int StyleLength = 2;   

	CPen penLine( PS_GEOMETRIC | PS_USERSTYLE, 1, &lBrush, StyleLength, Style );   

	CPen* pOldPen = pDC->SelectObject (&penLine);

	CCxStructTreeNode * pTempNode = NULL;
	int nPos = -1;
	vector <CCxStructTreeNode *>::iterator itrNode;

	rc.OffsetRect(-rc.Height(), 0);
	for (int i=0; i<nCount; i++)
	{
		rc.OffsetRect(rc.Height(), 0);
		ptCenter = rc.CenterPoint ();
		ptCenter.Offset(1, -2);

		if ( i + 1 == nCount ) //最后一个区域
		{
			//绘制一条 从中心点 到 右边界 的横线
			pDC->MoveTo (ptCenter.x, ptCenter.y);
			pDC->LineTo (rc.right, ptCenter.y);

			if ( pCurNode->m_pParent != NULL )
			{
				if (pCurRow->GetParent()==0)
					return; // 显示子流程时,根节点不需要再往上划竖线

				//绘制一条 从中心点 到 上边界 的竖线
				pDC->MoveTo (ptCenter.x, ptCenter.y);
				pDC->LineTo (ptCenter.x, rc.top - 1);

				nPos = -1;
				itrNode = pCurNode->m_pParent->m_vChildren.begin();
				while ( itrNode != pCurNode->m_pParent->m_vChildren.end() )
				{
					nPos++;
					if ( pCurNode == *itrNode )
					{
						break;
					}
					++itrNode;
				}
				if ( nPos > -1 && nPos + 1 < (int)pCurNode->m_pParent->m_vChildren.size() )
				{
					//绘制一条 从中心点 到 下边界 的竖线
					pDC->MoveTo (ptCenter.x, ptCenter.y);
					pDC->LineTo (ptCenter.x, rc.bottom);
				}
			}
			else
			{
				//do nothing
			}
		}
		else //不是最后一个区域，之前的区域，绘制竖线。
		{
			if ( i == 0 ) continue;

			pTempNode = pCurNode;
			for (nPos = i+1; nPos < nCount; nPos++)
			{
				if ( pTempNode->m_pParent == NULL )
				{
					pTempNode = NULL;
					break;
				}
				pTempNode = pTempNode->m_pParent;
			}
			if ( pTempNode == NULL ) continue;
			if ( pTempNode->m_pParent != NULL )
			{
				nPos = -1;
				itrNode = pTempNode->m_pParent->m_vChildren.begin();
				while ( itrNode != pTempNode->m_pParent->m_vChildren.end() )
				{
					nPos++;
					if ( pTempNode == *itrNode )
					{
						break;
					}
					++itrNode;
				}
				if ( nPos > -1 && nPos + 1 < (int)pTempNode->m_pParent->m_vChildren.size() )
				{
					//绘制一条 从上边界 到 下边界 的竖线
					pDC->MoveTo (ptCenter.x, rc.top);
					pDC->LineTo (ptCenter.x, rc.bottom);
				}
			}
		}

	}

	pDC->SelectObject (pOldPen);
}
void CIfEditTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CBCGPGridRow * pRow = GetCurSel();
	if ( pRow != NULL )
	{
		CCxStructTreeNode * pData = reinterpret_cast< CCxStructTreeNode * > ( pRow->GetData() );
		if ( pData != NULL )
		{
			if (!pData->m_vChildren.empty())
			{
				pRow->Expand(!pRow->IsExpanded());
			}
			switch ( pData->m_NodeType )
			{
			case CCxStructTreeNode::NT_COMPONENT:
				return;
			case CCxStructTreeNode::NT_ROOT:
				return;
			case CCxStructTreeNode::NT_FOR:
				return;
			case CCxStructTreeNode::NT_IF:
				return;
			case CCxStructTreeNode::NT_PARALLEL:
				return;
			case CCxStructTreeNode::NT_SEQUENCE:
				return;
			case CCxStructTreeNode::NT_SUBFLOW:
				return;
			case CCxStructTreeNode::NT_PARAM:
			case CCxStructTreeNode::NT_PARAM_ARRAY:
				{
					{//是否静态数组
						CCxStructTreeNode * p;
						if(!pData->m_vChildren.empty()) return;//非最底层节点无效
						for(p = pData; p->m_NodeType != CCxStructTreeNode::NT_PARAM; p = p->m_pParent);
						if(((CCxStructTreeNodeParam*)p)->m_bIsDynamicArray || ((CCxStructTreeNodeParam*)p)->IsFile()) return;//属于动态数组或文件参数则无效
					}
					CString strName;
					CString strComponentName;
					{//参数名确定				
						if(pData->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY)
						{//数组参数
							//							if(!pData->m_vChildren.empty()) return; //非最底层节点则无效
							strName = ((CCxStructTreeNodeParam*)pData)->m_strText;
						}
						else
						{
							strName = pData->m_strOriName;
						}
					}
					{//组件名确定
						CCxStructTreeNode * p;
						for(p = pData; p->m_NodeType == CCxStructTreeNode::NT_PARAM || p->m_NodeType == CCxStructTreeNode::NT_PARAM_ARRAY || p->m_NodeType == CCxStructTreeNode::NT_GROUP; p = p->m_pParent);
						strComponentName = p->m_strOriName;
					}

					if (m_hWndRecvDbClichMsg && IsWindow(m_hWndRecvDbClichMsg))
					{
						::PostMessage(m_hWndRecvDbClichMsg,WM_STRUCTTREEE_LDLK,0,(LPARAM)pData);
					}
				}				
			default:
				break;
			}
		}
	}
}
void CIfEditTree::OnKillFocus(CWnd* pNewWnd)
{
	CBCGPGridCtrl::OnKillFocus(pNewWnd);
	// TODO: 在此处添加消息处理程序代码
	CWnd * pParent = GetParent();
	if (pParent && ::IsWindow(pParent->m_hWnd))
	{
		if (pParent->IsWindowVisible())
			::SendMessage(m_hWndRecvDbClichMsg,WM_STRUCTTREEE_LEAVEFOCUS,0,(LPARAM)pNewWnd); // 使用SendMessage保证在鼠标点击事件前获得失去焦点时光标位置
	}
}
BOOL CIfEditTree::UpdateRowByNode( CZGridRow * pRow, CCxStructTreeNode * pNode )
{
	if ( pRow == NULL || pNode == NULL )
	{
		return FALSE;
	}

	CCxStructTreeNodeParam * pNodePrarm = NULL;
	CCxStructTreeNodeArrayElement * pNodeArrayElement = NULL;

	try
	{
		pRow->SetData( reinterpret_cast<DWORD_PTR>( pNode ) );
		pRow->GetItem (COL_NAME)->SetValue ( (LPCTSTR)pNode->m_strText, FALSE );
		//pRow->GetItem (COL_NAME)->AllowEdit ( TRUE );
		//pNode->m_pRow = (CCxStructTreeRowT*)pRow;
		SetRowImage( pNode, pRow );
		pRow->AllowSubItems ();

		switch ( pNode->m_NodeType )
		{
		case CCxStructTreeNode::NT_PARAM:
			pNodePrarm = (CCxStructTreeNodeParam *)pNode;
			if ( pNodePrarm->IsArray() || pNodePrarm->IsFile() )
			{
				if (pNodePrarm->m_nDir == PARAM_DIR_IN || pNodePrarm->m_nDir == PARAM_DIR_INOUT)
					pRow->GetItem (COL_VALUE)->SetValue ( "<Edit...>", FALSE );
				else if (pNodePrarm->m_nDir == PARAM_DIR_OUT)
					pRow->GetItem (COL_VALUE)->SetValue ( "<View...>", FALSE );
				pRow->GetItem (COL_VALUE)->AllowEdit ( FALSE );
			}
			else
			{
				pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)pNodePrarm->m_strValue, FALSE );
				if ( pNodePrarm->m_nDir == PARAM_DIR_IN || pNodePrarm->m_nDir == PARAM_DIR_INOUT )
					pRow->GetItem (COL_VALUE)->AllowEdit ( TRUE );
			}

			pRow->GetItem (COL_TYPE)->SetValue ( (LPCTSTR) pNode->ConvertParamDataType( pNodePrarm->m_nDataType ), FALSE );
			pRow->GetItem (COL_DIR)->SetValue ( (LPCTSTR) pNode->ConvertParamDir( pNodePrarm->m_nDir ), FALSE );
			pRow->GetItem (COL_UNITS)->SetValue ( (LPCTSTR)pNodePrarm->m_strUnits, FALSE );
			break;

		case CCxStructTreeNode::NT_PARAM_ARRAY:
			pNodeArrayElement = (CCxStructTreeNodeArrayElement *)pNode;
			pRow->GetItem (COL_VALUE)->SetValue ( (LPCTSTR)pNodeArrayElement->m_strValue, FALSE );
			break;

		default:
			break;
		}
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

void CIfEditTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CZGridCtrl::OnLButtonDown(nFlags, point);
	UpdateSelToParamName();
}
void CIfEditTree::UpdateSelToParamName()
{
	CZGridRow * pCurRow = ( CZGridRow * )GetCurSel();
	if (pCurRow == NULL) return;

	m_pSelItem = pCurRow->GetItem( COL_NAME );

	CBCGPGridRange* pSelRange = 
		new CBCGPGridRange (COL_NAME, 0, COL_UNITS, GetRowCount());
	CList<CBCGPGridRange*, CBCGPGridRange*>	lstSel;
	lstSel.AddTail (pSelRange);
	while (!lstSel.IsEmpty ())
	{
		CBCGPGridRange* pRange = lstSel.RemoveTail ();
		//ASSERT (pRange != NULL);

		SelectRange (*pRange, FALSE, TRUE);
		delete pRange;
	}

	pSelRange = 
		new CBCGPGridRange (COL_NAME, pCurRow->GetRowId(), COL_NAME, pCurRow->GetRowId());
	lstSel.AddTail (pSelRange);
	while (!lstSel.IsEmpty ())
	{
		CBCGPGridRange* pRange = lstSel.RemoveTail ();
		//ASSERT (pRange != NULL);

		SelectRange (*pRange, TRUE, TRUE);
		delete pRange;
		//pCurRow->Redraw();
	}

}
