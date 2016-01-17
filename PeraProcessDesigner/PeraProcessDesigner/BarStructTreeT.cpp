// CxFloatDlgBar.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraWorkspace.h"
#include "BarStructTreeT.h"
//#include "CxArray1D.h"
//#include "CxArray2D.h"
//#include "CxArray3D.h"
//#include "DlgArrayEditor.h"
#include "CxArrayND.h"
#include "DlgNDArrayEditor.h"
#include "DlgSvg.h"
#include "ConvertData.h"
#include "DlgStructTreeFileParamEditor.h"
#include "CStringChecker.h"

static int g_nCloseButtonMargin = 1;
static int g_nCaptionVertMargin = 2;
//static int g_nCaptionHorzMargin = 5;

static CString g_strLastValue;

// CCxFloatDlgBar

BEGIN_MESSAGE_MAP(CBarStructTreeT, CCxFloatBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, &CBarStructTreeT::OnGridTreeSelChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, &CBarStructTreeT::OnGridTreeItemChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_BEGININPLACEEDIT, &CBarStructTreeT::OnGridTreeBeginInplaceEdit)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_ENDINPLACEEDIT, &CBarStructTreeT::OnGridTreeEndInplaceEdit)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CBarStructTreeT, CCxFloatBar)

CBarStructTreeT::CBarStructTreeT()
{
//	m_pRoot = NULL;
}

CBarStructTreeT::~CBarStructTreeT()
{
	//if (m_pRoot)
	//{
	//	CCxStructTreeNode::m_pRoot = NULL;
	//	delete m_pRoot;
	//	m_pRoot = NULL;
	//}
}


int CBarStructTreeT::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCxFloatBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect(&rcClient);
	m_wndGridTree.InitTree(rcClient, this);

	CCxStructTreeNode * pNode = NULL;

	UpdateTree();

	return 0;
}


void CBarStructTreeT::OnSize(UINT nType, int cx, int cy)
{
	CCxFloatBar::OnSize(nType, cx, cy);
	//m_wndGridTree.SetWindowPos (NULL, -1, -1, cx, cy,
	//	SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndGridTree.SetWindowPos (NULL, 1, -1, cx - 2, cy 
		, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CBarStructTreeT::UpdateTree()
{
	m_wndGridTree.AdjustLayout ();
}

int CBarStructTreeT::GetCaptionHeight () const
{
	if (IsFloating () || IsMDITabbed () || m_cyGripper == 0)
	{
		return 0;
	}

	return m_cyGripper + CBCGPVisualManager::GetInstance ()->GetDockingBarCaptionExtraHeight ();
}

void CBarStructTreeT::OnEraseNCBackground (CDC* pDC, CRect rcBar)
{
	ASSERT_VALID (this);
	//theApp.m_nFlagFillBarBackground = CF_BAR_JIANMO_STRUCTTREE;
	CBCGPVisualManager::GetInstance ()->OnFillBarBackground (pDC, this, rcBar, rcBar, 
		TRUE /* NC area */);
	//theApp.m_nFlagFillBarBackground = 0;
}

//*****************************************************************************************
//void CBarStructTreeT::DrawCaption (CDC* pDC, CRect rectCaption)
//{
//	ASSERT_VALID (pDC);
//
//	CRect rcbtnRight = CRect (rectCaption.BottomRight (), CSize (0, 0));
//	int i = 0;
//
//	for (i = (int) m_arrButtons.GetUpperBound (); i >= 0; i --)
//	{
//		if (!m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
//		{
//			rcbtnRight = m_arrButtons [i]->GetRect();
//			break;
//		}
//	}
//
//	CRect rcbtnLeft = CRect (rectCaption.TopLeft (), CSize (0, 0));
//	for (i = (int) m_arrButtons.GetUpperBound (); i >= 0; i --)
//	{
//		if (m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
//		{
//			rcbtnLeft = m_arrButtons [i]->GetRect();
//			break;
//		}
//	}
//
//	COLORREF clrCptnText = CBCGPVisualManager::GetInstance ()->OnDrawControlBarCaption (
//		pDC, this, m_bActive, rectCaption, rcbtnRight);
//
//	for (i = 0; i < m_arrButtons.GetSize (); i ++)
//	{
//		CBCGPCaptionButton* pbtn = m_arrButtons [i];
//		ASSERT_VALID (pbtn);
//
//		pbtn->m_clrForeground = clrCptnText;
//	}
//
//    int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
//    COLORREF clrOldText = pDC->SetTextColor (clrCptnText);
//
//    CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);
//	ASSERT (pOldFont != NULL);
//
//    CString strTitle;
//    GetWindowText (strTitle);
//
//	rectCaption.right = rcbtnRight.left;
//	rectCaption.left = rcbtnLeft.right;
//	rectCaption.top++;
//	rectCaption.DeflateRect (g_nCaptionHorzMargin * 2, 0);
//
//	pDC->DrawText (strTitle, rectCaption, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
//
//    pDC->SelectObject(pOldFont);
//    pDC->SetBkMode(nOldBkMode);
//    pDC->SetTextColor(clrOldText);
//}


LRESULT CBarStructTreeT::OnGridTreeSelChanged( WPARAM wp, LPARAM lp )
{
	int nFlag = (int)wp;
	CCxStructTreeGridT * pTree = reinterpret_cast< CCxStructTreeGridT * > (lp);
	if (pTree == NULL) return 0;
	CCxStructTreeRow * pCurRow = ( CCxStructTreeRow * )pTree->GetCurSel();
	if (pCurRow == NULL) return 0;
	CBCGPGridItem * pCurItem = pTree->GetCurSelItem();
	if (pCurItem == NULL) return 0;
	CBCGPGridItemID id = pTree->GetCurSelItemID();
	CBCGPGridItem * pTempItem = NULL;

	//CCxStructTreeRow * pCurRow2 = NULL;
	//m_wndGridTree.UpdateSelStatus(pCurRow2);

//	pTree->m_pSelItem = pCurItem;

	//for (int i=0; i<pTree->GetRowCount(); i++)
	//{

	//}

	//if (pCurItem != NULL && id.m_nColumn != 0)
	//{
	//	pCurItem->Select(FALSE);
	//}

	//for (int i=0; i<pTree->GetRowCount(); i++)
	//{
	//	if ( i != id.m_nRow )
	//	{
	//		pTree->GetRow(i)->GetItem(0)->Select(FALSE);
	//	}
	//}
	//pCurRow->GetItem(0)->Select(TRUE);

	//pTree->SetRedraw();

	CCxStructTreeNode * pData = reinterpret_cast< CCxStructTreeNode * > ( pCurRow->GetData() );
	CCxStructTreeNode * pCanSelectNode = NULL;
	VIEW_DATA * pViewData = theViewMgr.Find( g_lpszViewTitleModel );
	CDlgSvg * pDlgSvg = NULL;
	
	if ( pData != NULL && pViewData != NULL )
	{
		pCanSelectNode = pData->GetCanSelectedNode();
		pDlgSvg = reinterpret_cast< CDlgSvg * > ( pViewData->m_dwExData );
	}
	if ( pCanSelectNode != NULL && pDlgSvg != NULL )
	{
		CWebMethodData wmd;
		CWebMethodParamData * pParam = NULL;
		//方法名称
		wmd.m_strMethodName = "xpdlNodeSelect";
		//第1个参数
		pParam = new CWebMethodParamData();
		pParam->m_strName = "id";
		pParam->m_strValue = pCanSelectNode->m_strID;
		wmd.m_vParams.push_back(pParam);
		pDlgSvg->InvokeJsMethod( &wmd );
	}

	if ( pData != NULL && id.m_nColumn == CCxStructTreeGridT::COL_VALUE && nFlag != 1 )
	{
		switch ( pData->m_NodeType )
		{
		case CCxStructTreeNode::NT_COMPONENT:
			//编辑组件
			break;
		case CCxStructTreeNode::NT_PARAM:
			switch ( ((CCxStructTreeNodeParam*)pData)->m_nDataType )
			{
			case DT_ARRAY1D_BOOLEAN:
			case DT_ARRAY1D_INT:
			case DT_ARRAY1D_DOUBLE:
			case DT_ARRAY1D_STRING:
				{
					//CCxArray1D * pArray1D = reinterpret_cast < CCxArray1D * > ( pData->m_lpRealData );
					//CDlgArrayEditor dlg;
					//dlg.m_pArrayData = pArray1D;
					//dlg.DoModal();
					
					//CxArrayND * pArray1D = reinterpret_cast < CxArrayND * > ( pData->m_lpRealData );
					//CDlgNDArrayEditor dlg;
					//dlg.m_pArrayData = ( CxArrayND * )pArray1D;
					//pArray1D->m_pNode = pData;
					//if ( dlg.DoModal() )
					//{
					//	((CCxStructTreeNodeParam*)pData)->m_strValue = pArray1D->GetArrayStr();
					//	pData->ParseValue();
					//	m_wndGridTree.UpdateParamValue( (LPVOID)(LPCTSTR)pData->m_strValue, pCurRow );
					//	m_wndGridTree.UpdateNode(pData);

					//	pData->m_bModified = TRUE;

					//	m_wndGridTree.UpdateDataMapping();

					//	CCxStructTreeNode * pNodeRef = NULL;
					//	CCxStructTreeItem * pItem = NULL;
					//	if ( pData->m_vRefId.size() == 0 ) return 0;
					//	if ( CCxStructTreeNode::m_pRoot == NULL ) return 0;
					//	for (int i=0; i<(int)pData->m_vRefId.size(); i++)
					//	{
					//		pNodeRef = CCxStructTreeNode::m_pRoot->GetChild( pData->m_vRefId[i] );
					//		if ( pNodeRef == NULL ) continue;
					//		if ( pNodeRef->m_pRow == NULL ) continue;

					//		pItem = (CCxStructTreeItem *)pNodeRef->m_pRow->GetItem ( CCxStructTreeGrid::COL_NAME );
					//		if (pItem == NULL) continue;
					//		//pItem->SetImage(14);
					//		m_wndGridTree.SetParamImage(pNodeRef, (CCxStructTreeRow*)pNodeRef->m_pRow);
					//		pItem->Redraw();

					//		pItem = (CCxStructTreeItem*)pNodeRef->m_pRow->GetItem ( CCxStructTreeGrid::COL_VALUE );
					//		pItem->SetTextColor( g_crStructTreeText );
					//		pItem->Redraw();	
					//	}
					//}

				}
				break;

			case DT_ARRAY2D_BOOLEAN:
			case DT_ARRAY2D_INT:
			case DT_ARRAY2D_DOUBLE:
			case DT_ARRAY2D_STRING:
				{
					//CCxArray2D * pArray2D = reinterpret_cast < CCxArray2D * > ( pData->m_lpRealData );
					//CDlgArrayEditor dlg;
					//dlg.m_pArrayData = pArray2D;
					//dlg.DoModal();

					//CxArrayND * pArray2D = reinterpret_cast < CxArrayND * > ( pData->m_lpRealData );
					//CDlgNDArrayEditor dlg;
					//dlg.m_pArrayData = ( CxArrayND * )pArray2D;
					//pArray2D->m_pNode = pData;
					//if ( dlg.DoModal() )
					//{
						//pData->m_strValue = pArray2D->GetArrayStr();
						//pData->ParseValue();
						//m_wndGridTree.UpdateParamValue( (LPVOID)(LPCTSTR)pData->m_strValue, pCurRow );
						//m_wndGridTree.UpdateNode(pData);

						//pData->m_bModified = TRUE;

						//m_wndGridTree.UpdateDataMapping();

						//CCxStructTreeNode * pNodeRef = NULL;
						//CCxStructTreeItem * pItem = NULL;
						//if ( pData->m_vRefId.size() == 0 ) return 0;
						//if ( CCxStructTreeNode::m_pRoot == NULL ) return 0;
						//for (int i=0; i<(int)pData->m_vRefId.size(); i++)
						//{
						//	pNodeRef = CCxStructTreeNode::m_pRoot->GetChild( pData->m_vRefId[i] );
						//	if ( pNodeRef == NULL ) continue;
						//	if ( pNodeRef->m_pRow == NULL ) continue;

						//	pItem = (CCxStructTreeItem *)pNodeRef->m_pRow->GetItem ( CCxStructTreeGrid::COL_NAME );
						//	if (pItem == NULL) continue;
						//	//pItem->SetImage(14);
						//	m_wndGridTree.SetParamImage(pNodeRef, (CCxStructTreeRow*)pNodeRef->m_pRow);
						//	pItem->Redraw();

						//	pItem = (CCxStructTreeItem*)pNodeRef->m_pRow->GetItem ( CCxStructTreeGrid::COL_VALUE );
						//	pItem->SetTextColor( g_crStructTreeText );
						//	pItem->Redraw();	
						//}
					//}

				}
				break;

			case DT_ARRAY3D_BOOLEAN:
			case DT_ARRAY3D_INT:
			case DT_ARRAY3D_DOUBLE:
			case DT_ARRAY3D_STRING:
				{
					//CCxArray3D * pArray3D = reinterpret_cast < CCxArray3D * > ( pData->m_lpRealData );
					//CDlgArrayEditor dlg;
					//dlg.m_pArrayData = pArray3D;
					//dlg.DoModal();

					//CxArrayND * pArray3D = reinterpret_cast < CxArrayND * > ( pData->m_lpRealData );
					//CDlgNDArrayEditor dlg;
					//dlg.m_pArrayData = ( CxArrayND * )pArray3D;
					//pArray3D->m_pNode = pData;
					//if ( dlg.DoModal() )
					//{
						//pData->m_strValue = pArray3D->GetArrayStr();
						//pData->ParseValue();
						//m_wndGridTree.UpdateParamValue( (LPVOID)(LPCTSTR)pData->m_strValue, pCurRow );
						//m_wndGridTree.UpdateNode(pData);

						//pData->m_bModified = TRUE;

						//m_wndGridTree.UpdateDataMapping();

						//CCxStructTreeNode * pNodeRef = NULL;
						//CCxStructTreeItem * pItem = NULL;
						//if ( pData->m_vRefId.size() == 0 ) return 0;
						//if ( CCxStructTreeNode::m_pRoot == NULL ) return 0;
						//for (int i=0; i<(int)pData->m_vRefId.size(); i++)
						//{
						//	pNodeRef = CCxStructTreeNode::m_pRoot->GetChild( pData->m_vRefId[i] );
						//	if ( pNodeRef == NULL ) continue;
						//	if ( pNodeRef->m_pRow == NULL ) continue;

						//	pItem = (CCxStructTreeItem *)pNodeRef->m_pRow->GetItem ( CCxStructTreeGrid::COL_NAME );
						//	if (pItem == NULL) continue;
						//	//pItem->SetImage(14);
						//	m_wndGridTree.SetParamImage(pNodeRef, (CCxStructTreeRow*)pNodeRef->m_pRow);
						//	pItem->Redraw();

						//	pItem = (CCxStructTreeItem*)pNodeRef->m_pRow->GetItem ( CCxStructTreeGrid::COL_VALUE );
						//	pItem->SetTextColor( g_crStructTreeText );
						//	pItem->Redraw();	
						//}
					//}
				}
				break;

			case DT_FILE:
				{
					//CDlgStructTreeFileParamEditor dlg;
				//	dlg.m_pNode = pData;
					//if ( dlg.DoModal() )
					//{
					//	//更新
					//	m_wndGridTree.UpdateParamValueBg();
					//	m_wndGridTree.UpdateParamValue( (LPVOID)(LPCTSTR)pData->m_strValue, pCurRow );
					//}
				}
				break;

			default:
				break;
			}
			break;
		}
	}

//	pTree->AdjustLayout();

	//if (id.m_nColumn == 1)
	//{
	//	switch (id.m_nRow)
	//	{
	//	//case 2:
	//	//case 3:
	//	//case 9:
	//	//	pTree->SetReadOnly(FALSE);
	//	//	return 0;

	//	case 6: //编辑文件
	//		AfxMessageBox("编辑文件");
	//		break;




	//	case 10: //查看结果
	//		AfxMessageBox("查看结果");
	//		break;
	//	}
	//}
	//if (pItem != NULL) pItem->Select(FALSE);
	//pTree->SetReadOnly();

	return 0;
}



LRESULT CBarStructTreeT::OnGridTreeBeginInplaceEdit( WPARAM wp, LPARAM lp )
{
	BCGPGRID_ITEM_INFO * pii = reinterpret_cast< BCGPGRID_ITEM_INFO * > ( lp );
	int nCtrlID = (int)( wp );
	if ( pii == NULL ) return 0;
	if ( pii->pItem == NULL ) return 0;

	ConvertBSTR2CString( g_strLastValue, pii->pItem->GetValue().bstrVal );
	return 0;
}

//Item内容变更
LRESULT CBarStructTreeT::OnGridTreeItemChanged( WPARAM wp, LPARAM lp )
{
	BCGPGRID_ITEM_INFO * pii = reinterpret_cast< BCGPGRID_ITEM_INFO * > ( lp );
	int nCtrlID = (int)( wp );
	if ( pii == NULL ) return 0;
	if ( pii->pItem == NULL ) return 0;

	CCxStructTreeRow * pCurRow = NULL;
	CCxStructTreeNode * pCurNode = NULL;
	CCxStructTreeNode * pNodeRef = NULL;
	CBCGPGridItem * pItem = NULL;

	CString strCurValue;
	ConvertBSTR2CString( strCurValue, pii->pItem->GetValue().bstrVal );
	if ( g_strLastValue == strCurValue ) return 0;

	pCurRow = (CCxStructTreeRow *)m_wndGridTree.GetRow (pii->nRow);
	if (pCurRow == NULL) return 0;

	pCurNode = reinterpret_cast < CCxStructTreeNode * > ( pCurRow->GetData() );
	if ( pCurNode == NULL ) return 0;

	//验证输入数据
	switch ( ((CCxStructTreeNodeParam*)pCurNode)->m_nDataType )
	{
	case DT_INT:
		if ( !CStringChecker::CheckInt( strCurValue ) )
		{
			AfxMessageBox("输入数据不是整型");
			pii->pItem->SetValue( (LPCTSTR)g_strLastValue );
			return 0;
		}
		break;
	case DT_DOUBLE:
		if ( !CStringChecker::CheckDouble( strCurValue ) )
		{
			AfxMessageBox("输入数据不是浮点型");
			pii->pItem->SetValue( (LPCTSTR)g_strLastValue );
			return 0;
		}
		break;
	default:
		break;
	}


	m_wndGridTree.UpdateParamValue( (LPVOID)(LPCTSTR)strCurValue, (CCxStructTreeRowT*)pCurRow );

	pCurNode->m_bModified = TRUE;

	m_wndGridTree.UpdateDataMapping();

	if ( ((CCxStructTreeNodeParam*)pCurNode)->m_vRefId.size() == 0 ) return 0;
	//if ( CCxStructTreeNode::m_pRoot == NULL ) return 0;
	for (int i=0; i<(int)((CCxStructTreeNodeParam*)pCurNode)->m_vRefId.size(); i++)
	{
		pNodeRef = m_wndGridTree.m_StructTree.Find( ((CCxStructTreeNodeParam*)pCurNode)->m_vRefId[i] );
		if ( pNodeRef == NULL ) continue;
		if ( pNodeRef->m_pRow == NULL ) continue;

		pItem = pNodeRef->m_pRow->GetItem ( CCxStructTreeGridT::COL_NAME );
		if (pItem == NULL) continue;
		//pItem->SetImage(14);
		m_wndGridTree.SetParamImage((CCxStructTreeNodeParam*)pNodeRef, (CCxStructTreeRowT*)pNodeRef->m_pRow);
		pItem->Redraw();

		pItem = pNodeRef->m_pRow->GetItem ( CCxStructTreeGridT::COL_VALUE );
		pItem->SetTextColor( g_crStructTreeText );
		pItem->Redraw();	
	}

	return 0;
}

LRESULT CBarStructTreeT::OnGridTreeEndInplaceEdit( WPARAM wp, LPARAM lp )
{
	BCGPGRID_ITEM_INFO * pii = reinterpret_cast< BCGPGRID_ITEM_INFO * > ( lp );
	int nCtrlID = (int)( wp );
	if ( pii == NULL ) return 0;

	return 0;
}

void CBarStructTreeT::OnPaint() 
{
	ASSERT_VALID (this);
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(&rcClient);

	//CBCGPDrawManager dm( dc );
	//dm.DrawRect(rcClient, g_crStructTreeClientBg, g_crStructTreeClientBg);
}