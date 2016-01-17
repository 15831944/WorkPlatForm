// DlgNDArrayEditor.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgNDArrayEditor.h"
#include "afxdialogex.h"
#include "CxArrayND.h"
#include "ConvertData.h"
#include <BCGPGridCtrl.h>
#include "DlgNDArrayResize.h"
#include "PeraProcessDesignerUtility.h"
#include "CStringChecker.h"
#include "CxStructTreeNodeParam.h"

// CDlgNDArrayEditor 对话框

BEGIN_MESSAGE_MAP(CDlgNDArrayEditor, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgNDArrayEditor::OnBnClickedOk)
	ON_BN_CLICKED(IDC_NAE_CANCEL, &CDlgNDArrayEditor::OnBnClickedNaeCancel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, &CDlgNDArrayEditor::OnGridSelChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, &CDlgNDArrayEditor::OnGridItemChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_BEGININPLACEEDIT, &CDlgNDArrayEditor::OnGridBeginInplaceEdit)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_ENDINPLACEEDIT, &CDlgNDArrayEditor::OnGridEndInplaceEdit)
	ON_BN_CLICKED(IDC_NAE_OK, &CDlgNDArrayEditor::OnBnClickedNaeOk)
	ON_BN_CLICKED(IDC_NAE_RESIZE, &CDlgNDArrayEditor::OnBnClickedNaeResize)
END_MESSAGE_MAP()

void CDlgNDArrayEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAE_LIST, m_wndDummyList);
	DDX_Control(pDX, IDC_NAE_DIM, m_wndDummyDim);
	DDX_Control(pDX, IDC_NAE_OK, m_wndOk);
	DDX_Control(pDX, IDC_NAE_CANCEL, m_wndCancel);
	DDX_Control(pDX, IDC_NAE_RESIZE, m_wndResize);
}

IMPLEMENT_DYNAMIC(CDlgNDArrayEditor, CDialogEx)

CDlgNDArrayEditor::CDlgNDArrayEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNDArrayEditor::IDD, pParent)
{
	m_pArrayData = NULL;
	m_pLastDimTypeRow = NULL;
	m_pArrayDataCopied = NULL;
}

CDlgNDArrayEditor::~CDlgNDArrayEditor()
{
	if (m_pArrayDataCopied)
	{
		delete m_pArrayDataCopied;
	}
}

void CDlgNDArrayEditor::OnBnClickedOk()
{
	//按 Enter 不关闭对话框
}

BOOL CDlgNDArrayEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString str;
	int nColCnt = 0;
	int nRowCnt = 0;
	m_pArrayDataCopied = m_pArrayData->GetCopied();
	CxArrayND * pNDArray = m_pArrayDataCopied;
	CRect rectGrid;
	CBCGPGridColors colors;
	CBCGPGridItem* pItem = NULL;
	CBCGPGridRow * pRow = NULL;

	//////////////////////////////////////////////////////////////////////////

	m_wndDummyList.GetClientRect (&rectGrid);
	m_wndDummyList.MapWindowPoints (this, &rectGrid);
	rectGrid.DeflateRect(1,1);

	m_wndList.m_pArrayData = (CxArrayND*)m_pArrayDataCopied;
	m_wndList.Init( rectGrid, this );

	//////////////////////////////////////////////////////////////////////////

	m_wndDummyDim.GetClientRect (&rectGrid);
	m_wndDummyDim.MapWindowPoints (this, &rectGrid);
	rectGrid.DeflateRect(1,1);

	m_wndDim.m_pArrayData = (CxArrayND*)m_pArrayDataCopied;
	m_wndDim.Init( rectGrid, this );

	if ( pNDArray != NULL )
	{
		if ( 
			!pNDArray->m_bIsDynamicArray 
			|| pNDArray->m_nDir == PARAM_DIR_OUT 
			|| (m_pArrayDataCopied->m_pNode->m_dwStatus & CCxStructTreeNode::NS_VALUE_READONLY) 
			)
		{
			m_wndResize.EnableWindow(FALSE);
		}
		else
		{
			m_wndResize.EnableWindow(TRUE);
		}
	}
	else
	{
		m_wndResize.EnableWindow(FALSE);
	}
	
	UpdateDim();
	UpdateContent();
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgNDArrayEditor::UpdateContent( void )
{
	m_wndList.RemoveAll();
	m_wndList.DeleteAllColumns();
	m_wndList.AdjustLayout ();

	CxArrayND * pNDArray = reinterpret_cast< CxArrayND * > ( m_pArrayDataCopied );
	if ( pNDArray == NULL )
	{
		return;
	}

	CString str;
	CCxNDArrayItem* pItem = NULL;
	CCxNDArrayRow * pRow = NULL;
	vector < int > vPos;
	int nColCnt = 0;
	int nRowCnt = 0;
	if ( pNDArray->GetDimCount() == 1 )
	{
		vPos.push_back(0);
		nColCnt = 1;
		nRowCnt = pNDArray->GetDataCount(0);
		m_wndList.InsertColumn (0, "0", 48);
		for ( int i=0; i<nRowCnt; i++ )
		{
			vPos[0] = i;
			pRow = (CCxNDArrayRow *)m_wndList.CreateNewRow ();
			pRow->AllowSubItems (FALSE);
			LPARAM lp = pNDArray->GetDataPtr( vPos );
			pRow->GetItem (0)->SetData( lp );
			pRow->GetItem (0)->SetValue( (LPCTSTR)pNDArray->GetDataAsCString( vPos ) );
			m_wndList.AddRow (pRow, FALSE);
		}
	}
	else if ( pNDArray->GetDimCount() == 2 )
	{
		vPos.push_back(0);
		vPos.push_back(0);
		
		//pRow = m_wndDim.GetRow( 0 );
		//if ( pRow == NULL ) return;
		//ConvertBSTR2CString( str, pRow->GetItem( COL_DIM_TYPE )->GetValue().bstrVal );
		//if ( str.CompareNoCase( "Row" ) == 0 )
		//{
		//	nColCnt = pRow->GetItem( COL_DIM_VALUE )->GetValue().intVal;
		//}
		nRowCnt = pNDArray->GetDataCount( 0 );
		nColCnt = pNDArray->GetDataCount( 1 );
		for ( int i=0; i<nColCnt; i++ )
		{
			CString str;
			str.Format("%d", i);
			m_wndList.InsertColumn (i, (char*)(LPCTSTR)str, 48);
		}
		for ( int i=0; i<nRowCnt; i++ )
		{
			pRow = (CCxNDArrayRow *)m_wndList.CreateNewRow ();
			pRow->AllowSubItems (FALSE);
			m_wndList.AddRow (pRow, FALSE);
		}
		
		for ( int i=0; i<nRowCnt; i++ )
		{
			vPos[0] = i;
			for ( int j=0; j<nColCnt; j++ )
			{
				vPos[1] = j;
				pRow = (CCxNDArrayRow *)m_wndList.GetRow( i );
				LPARAM lp = pNDArray->GetDataPtr( vPos );
				pRow->GetItem (j)->SetData( lp );
				pRow->GetItem (j)->SetValue( (LPCTSTR)pNDArray->GetDataAsCString( vPos ) );
			}
		}
	}
	else if ( pNDArray->GetDimCount() > 2 )
	{
		int nDimCount = pNDArray->GetDimCount();
		int nRowPos = -1;
		int nColPos = -1;
		for (int i=0; i<nDimCount; i++)
		{
			vPos.push_back(0);
		}

		for (int i=0; i<nDimCount; i++)
		{
			pRow = (CCxNDArrayRow *)m_wndDim.GetRow( i );
			if ( pRow == NULL ) return;
			ConvertBSTR2CString( str, pRow->GetItem( COL_DIM_TYPE )->GetValue().bstrVal );

			vPos[i] = pRow->GetItem( COL_DIM_VALUE )->GetValue().intVal;
			if ( str.CompareNoCase( "Row" ) == 0 )
			{
				//nRowCnt = pRow->GetItem( COL_DIM_VALUE )->GetValue().intVal;
				nRowPos = i;
			}
			else if ( str.CompareNoCase( "Column" ) == 0 )
			{
				//nColCnt = pRow->GetItem( COL_DIM_VALUE )->GetValue().intVal;
				nColPos = i;
			}
		}

		if ( nColPos == -1 || nRowPos == -1 ) return;


		//pRow = m_wndDim.GetRow( 0 );
		//if ( pRow == NULL ) return;
		//ConvertBSTR2CString( str, pRow->GetItem( COL_DIM_TYPE )->GetValue().bstrVal );
		//if ( str.CompareNoCase( "Row" ) == 0 )
		//{
		//	nColCnt = pRow->GetItem( COL_DIM_VALUE )->GetValue().intVal;
		//}
		nRowCnt = pNDArray->GetDataCount( nRowPos );
		nColCnt = pNDArray->GetDataCount( nColPos );
		for ( int i=0; i<nColCnt; i++ )
		{
			CString str;
			str.Format("%d", i);
			m_wndList.InsertColumn (i, (char*)(LPCTSTR)str, 48);
			//m_wndList.InsertColumn (i, "", 48);
		}
		for ( int i=0; i<nRowCnt; i++ )
		{
			pRow = (CCxNDArrayRow *)m_wndList.CreateNewRow ();
			pRow->AllowSubItems (FALSE);
			m_wndList.AddRow (pRow, FALSE);
		}

		for ( int i=0; i<nRowCnt; i++ )
		{
			vPos[ nRowPos ] = i;
			for ( int j=0; j<nColCnt; j++ )
			{
				vPos[ nColPos ] = j;
				pRow = (CCxNDArrayRow *)m_wndList.GetRow ( i );
				LPARAM lp = pNDArray->GetDataPtr( vPos );
				pRow->GetItem (j)->SetData( lp );
				pRow->GetItem (j)->SetValue( (LPCTSTR)pNDArray->GetDataAsCString( vPos ) );
			}
		}
	}
	else
	{
		return;
	}

	m_wndList.AdjustLayout ();

	//{
	//	if (m_pArrayData->GetDimCount() == 1)
	//	{
	//		m_wndGridTree.InsertColumn (0, _T(""), 60);
	//		//
	//		CCxArray1D * pArr1D = reinterpret_cast<CCxArray1D *> (m_pArrayData);
	//		const int nColumns = m_wndGridTree.GetColumnCount ();
	//		//
	//		for (int i=0; i<pArr1D->GetDataCount(); i++)
	//		{
	//			CBCGPGridRow * pRow = m_wndGridTree.CreateRow (nColumns);
	//			pRow->AllowSubItems ();
	//			pRow->GetItem (0)->SetValue ( (LPCTSTR)pArr1D->GetData(i) );
	//			m_wndGridTree.AddRow (pRow, FALSE);
	//		}
	//	}
	//	else if (m_pArrayData->GetDimCount() == 2)
	//	{
	//		CCxArray2D * pArr2D = reinterpret_cast<CCxArray2D *> (m_pArrayData);

	//		for (int i=0; i<pArr2D->GetColCount(); i++)
	//		{
	//			m_wndGridTree.InsertColumn (i, _T(""), 60);
	//		}
	//		//

	//		const int nColumns = m_wndGridTree.GetColumnCount ();
	//		//
	//		for (int i=0; i<pArr2D->GetRowCount(); i++)
	//		{
	//			CBCGPGridRow * pRow = m_wndGridTree.CreateRow (nColumns);
	//			pRow->AllowSubItems ();
	//			for (int j=0; j<pArr2D->GetColCount(); j++)
	//			{
	//				pRow->GetItem (j)->SetValue ( (LPCTSTR)pArr2D->GetData(i, j) );
	//			}
	//			m_wndGridTree.AddRow (pRow, FALSE);
	//		}
	//	}
	//}

	//m_wndGridTree.AdjustLayout ();



}


void CDlgNDArrayEditor::OnBnClickedNaeCancel()
{
	EndDialog(FALSE); return;
}


LRESULT CDlgNDArrayEditor::OnGridSelChanged( WPARAM wp, LPARAM lp )
{
	CBCGPGridCtrl * pBaseGrid = (CBCGPGridCtrl *)lp;
	
	if ( pBaseGrid == NULL ) return 0;

	//维度Grids
	if ( pBaseGrid->IsKindOf (RUNTIME_CLASS (CCxNDArrayDimGrid)) )
	{
		CCxNDArrayDimGrid * pDimGrid = NULL;
		CCxNDArrayDimItem * pDimItem = NULL;

		pDimGrid = (CCxNDArrayDimGrid *)pBaseGrid;
		pDimItem = (CCxNDArrayDimItem*)pDimGrid->GetCurSelItem();
		if ( pDimItem == NULL ) return 0;

		_variant_t var( pDimItem->GetValue() );
		var.ChangeType(VT_BSTR);
		ConvertBSTR2CString( m_strLastDimType, var.bstrVal );
		m_pLastDimTypeRow = ( CCxNDArrayDimRow* )pDimItem->GetParentRow();
	}

	return 0;
}

LRESULT CDlgNDArrayEditor::OnGridItemChanged( WPARAM wp, LPARAM lp )
{
	UINT nID = (UINT)wp;
	BCGPGRID_ITEM_INFO * pii = reinterpret_cast< BCGPGRID_ITEM_INFO * > ( lp );
	if ( pii == NULL ) return 0;

	CString str, strLastValue;

	//数组数据变更
	if (nID == m_wndList.GetDlgCtrlID())
	{
		CString * pStr = reinterpret_cast< CString * > ( pii->pItem->GetData() );
		if ( pStr != NULL )
		{
			//ConvertBSTR2CString( *pStr, pii->pItem->GetValue().bstrVal );

			strLastValue = * pStr;

			CString strNewValue;
			ConvertBSTR2CString( strNewValue, pii->pItem->GetValue().bstrVal );
			if ( m_pArrayDataCopied->GetRealDataType() == DT_DOUBLE )
			{
				//CString strDoubleValue = ConvertDoubleValue( strNewValue );
				//strNewValue = strDoubleValue;
				if ( !strNewValue.IsEmpty() )
				{
					//这样处理是因为：
					//编辑Item内容期间，如果执行了SetValue函数，
					//会导致父窗口上面的确定按钮需要点击2次才能关闭对话框。
					BOOL bbak = ((CCxNDArrayItem*)pii->pItem)->m_bInPlaceEdit;
					((CCxNDArrayItem*)pii->pItem)->m_bInPlaceEdit = FALSE;
					pii->pItem->SetValue( (LPCTSTR)strNewValue );
					((CCxNDArrayItem*)pii->pItem)->m_bInPlaceEdit = bbak;
				}
				else
				{
					MessageBox("输入数据不是浮点型", g_lpszAppTitle, MB_OK|MB_ICONERROR);
					pii->pItem->SetValue( (LPCTSTR)strLastValue );
					return 0;
				}
			}
			
			//验证输入数据
			if ( m_pArrayDataCopied->GetRealDataType() == DT_INT )
			{
				if ( !CStringChecker::CheckInt( strNewValue ) )
				{
					MessageBox("输入数据不是整型", g_lpszAppTitle, MB_OK|MB_ICONERROR);
					pii->pItem->SetValue( (LPCTSTR)strLastValue );
					return 0;
				}

				strNewValue.Trim();
				int iVal = atoi(strNewValue);
				itoa(iVal,strNewValue.GetBuffer(50),10);
				strNewValue.ReleaseBuffer();
				pii->pItem->SetValue((LPCTSTR)strNewValue);
			}
			else if ( m_pArrayDataCopied->GetRealDataType() == DT_DOUBLE )
			{
				if ( !CStringChecker::CheckDouble( strNewValue ) )
				{
					MessageBox("输入数据不是浮点型", g_lpszAppTitle, MB_OK|MB_ICONERROR);
					pii->pItem->SetValue( (LPCTSTR)strLastValue );
					return 0;
				}
			}

			*pStr = strNewValue;

			m_wndList.m_bModified = TRUE;
			if ( !m_pArrayDataCopied->m_bResized )
			{
				//int nDimRowCount = m_wndDim.GetRowCount();
				//vector<int> vPos; vPos.resize( nDimRowCount );
				//for (int i=0; i<nDimRowCount; i++)
				//{
				//	CString strType;
				//	ConvertBSTR2CString( strType, m_wndDim.GetRow(i)->GetItem(COL_DIM_TYPE)->GetValue().bstrVal );
				//	if ( strType.CompareNoCase("Index") == 0 )
				//}
				//m_pArrayData->m_pNode
			}
		}
	}
	//维度数据变更
	else if (nID == m_wndDim.GetDlgCtrlID())
	{
		if ( pii->nCol == COL_DIM_VALUE )
		{
			UpdateContent();
		}

		CString strCurType;

		if ( pii->nCol == COL_DIM_TYPE )
		{
			ConvertBSTR2CString( strCurType, pii->pItem->GetValue().bstrVal );
			//如果不为以下3个值，则表示非法数据，恢复原来的值。
			if ( 
				strCurType.CollateNoCase("Row") != 0
				&& strCurType.CollateNoCase("Column") != 0
				&& strCurType.CollateNoCase("Index") != 0 
				)
			{
				pii->pItem->SetValue( (LPCTSTR)m_strLastDimType );

				//m_strLastDimType = "";
				//m_pLastDimTypeRow = NULL;

				m_pLastDimTypeRow = (CCxNDArrayDimRow*)pii->pItem->GetParentRow();

				return 0;
			}

			//if ( 
			//	str.CollateNoCase("Row") == 0
			//	|| str.CollateNoCase("Column") != 0
			//	)
			//	pii->pItem->GetParentRow()->GetItem( COL_DIM_VALUE )->Enable( FALSE );
			//else if ( 
			//	str.CollateNoCase("Index") == 0
			//	)
			//	pii->pItem->GetParentRow()->GetItem( COL_DIM_VALUE )->Enable( TRUE );

			EnsureOneRowOnColumn( pii->nRow, m_strLastDimType );
			UpdateDim();
			UpdateContent();

			//m_strLastDimType = "";
			//m_pLastDimTypeRow = NULL;

			m_strLastDimType = strCurType;
			m_pLastDimTypeRow = (CCxNDArrayDimRow*)pii->pItem->GetParentRow();
		}
	}

	return 0;
}
LRESULT CDlgNDArrayEditor::OnGridBeginInplaceEdit( WPARAM wp, LPARAM lp )
{
	return 0;
}
LRESULT CDlgNDArrayEditor::OnGridEndInplaceEdit( WPARAM wp, LPARAM lp )
{
	UINT nID = (UINT)wp;
	BCGPGRID_ITEM_INFO * pii = reinterpret_cast< BCGPGRID_ITEM_INFO * > ( lp );
	if ( pii == NULL ) return 0;
	CString str;

	return 0;
}

void CDlgNDArrayEditor::EnsureOneRowOnColumn( int nClickRow, LPCTSTR lpszOldValue )
{
	CxArrayND * pArray = reinterpret_cast< CxArrayND * > ( m_pArrayDataCopied ); 
	if ( pArray == NULL )
		return;
	int nDimCount = pArray->GetDimCount();
	CCxNDArrayDimRow * pCurRow = (CCxNDArrayDimRow *)m_wndDim.GetRow( nClickRow );
	if ( pCurRow == NULL )
		return;
	CString strNewType;
	ConvertBSTR2CString( strNewType, pCurRow->GetItem( COL_DIM_TYPE )->GetValue().bstrVal );
	CString strOldType = lpszOldValue == NULL ? "" : lpszOldValue;

	if (strOldType.IsEmpty())
	{
		Sleep(0);
	}

	CCxNDArrayDimRow * pRow = NULL;
	CString str;
	vector<CString> vCurList;
	for (int i=0; i<nDimCount; i++)
	{
		pRow = (CCxNDArrayDimRow *)m_wndDim.GetRow(i);
		if ( pRow == NULL )
			return;
		ConvertBSTR2CString( str, pRow->GetItem( COL_DIM_TYPE )->GetValue().bstrVal );

		if (str.IsEmpty())
		{
			Sleep(0);
		}

		vCurList.push_back(str);
	}
	Sleep(0);

	for (int i=0; i<nDimCount; i++)
	{
		if ( vCurList[i].CompareNoCase((LPCTSTR)strNewType) == 0 && nClickRow != i )
		{
			pRow = (CCxNDArrayDimRow *)m_wndDim.GetRow(i);
			if ( pRow == NULL )
				return;
			pRow->GetItem( COL_DIM_TYPE )->SetValue( (LPCTSTR)strOldType );
			vCurList[i] = strOldType;
			//break;
		}
	}
	Sleep(0);

	for (int i=0; i<nDimCount; i++)
	{
		pRow = (CCxNDArrayDimRow *)m_wndDim.GetRow(i);
		if ( pRow == NULL )
			return;
		pRow->GetItem( COL_DIM_TYPE )->SetValue( (LPCTSTR)vCurList[i] );
	}
}


void CDlgNDArrayEditor::OnBnClickedNaeOk()
{
	if (m_wndList.m_bModified)
	{
		if (m_pArrayDataCopied->m_bResized)
		{
			int iDimCount = m_pArrayDataCopied->GetDimCount();
			vector<int> vDim(iDimCount);
			for (int i=0;i<iDimCount;i++)
				vDim[i] = m_pArrayDataCopied->GetDataCount(i);
			m_pArrayData->Resize(vDim);
		}
	
		vector<int> vPos;
		if (m_pArrayDataCopied->GetDimCount() == 1)
		{
			vPos.push_back(0);
			int iDataCount = m_pArrayDataCopied->GetDataCount();
			for (int i =0; i<iDataCount; i++)
			{
				vPos[0] = i;
				CString * pDataNew = reinterpret_cast<CString*>(m_pArrayDataCopied->GetDataPtr(vPos));
				CString * pDataOld = reinterpret_cast<CString*>(m_pArrayData->GetDataPtr(vPos));
				if (pDataNew!=NULL)
				{
					*pDataOld = *pDataNew;
				}
			}
		}
		else if (m_pArrayDataCopied->GetDimCount() == 2)
		{
			vPos.push_back(0);
			vPos.push_back(0);
			int nRowCnt = m_pArrayDataCopied->GetDataCount( 0 );
			int nColCnt = m_pArrayDataCopied->GetDataCount( 1 );
			for (int i =0; i<nRowCnt; i++)
			{
				vPos[0] = i;
				for (int j=0;j< nColCnt;j++)
				{
					vPos[1] = j;
					CString * pDataNew = reinterpret_cast<CString*>(m_pArrayDataCopied->GetDataPtr(vPos));
					CString * pDataOld = reinterpret_cast<CString*>(m_pArrayData->GetDataPtr(vPos));
					if (pDataNew!=NULL)
					{
						*pDataOld = *pDataNew;
					}
				}
				
			}
		}
		else if (m_pArrayDataCopied->GetDimCount() == 3)
		{
			vPos.push_back(0);
			vPos.push_back(0);
			vPos.push_back(0);

			int nDim = m_pArrayDataCopied->GetDataCount( 0 );
			int nRowCnt = m_pArrayDataCopied->GetDataCount( 1 );
			int nColCnt = m_pArrayDataCopied->GetDataCount( 2 );
		
			for (int k =0; k<nDim; k++)
			{
				vPos[0] = k;
				for (int i =0; i<nRowCnt; i++)
				{
					vPos[1] = i;
					for (int j=0;j< nColCnt;j++)
					{
						vPos[2] = j;
						CString * pDataNew = reinterpret_cast<CString*>(m_pArrayDataCopied->GetDataPtr(vPos));
						CString * pDataOld = reinterpret_cast<CString*>(m_pArrayData->GetDataPtr(vPos));
						if (pDataNew!=NULL)
						{
							*pDataOld = *pDataNew;
						}
					}

				}
			}
			
		}
		

	}

	EndDialog(m_wndList.m_bModified); return;
}


void CDlgNDArrayEditor::OnBnClickedNaeResize()
{
	CDlgNDArrayResize dlg;
	dlg.m_pArrayData = m_pArrayDataCopied;
	if ( !dlg.DoModal() )
	{
		return;
	}
	UpdateDim();
	UpdateContent();
	m_wndList.m_bModified = TRUE;
}

void CDlgNDArrayEditor::UpdateDim( void )
{
	if ( !IsWindow ( m_wndDim.GetSafeHwnd() ) ) return;
	if ( m_wndDim.GetColumnCount() < 3 ) return;
//	if ( m_wndDim.GetRowCount() < 3 ) return;

	CxArrayND * pNDArray = (CxArrayND*)m_pArrayDataCopied;
	CCxNDArrayDimItem * pValueItem = NULL;
	CCxNDArrayDimItem * pIndexValueItem = NULL;
	CCxNDArrayDimRow * pCxRow = NULL;
	CString strType;
	int nRowCnt = 0;
	int nColCnt = 0;
	int nIdxCnt = -1;

	//此处如果不这样写，SetValue 方法中，会判断类型
	//m_varValue 和 传入的 值的类型 一个为 VT_INT ，一个为 VT_I4
	//导致断言异常。
	_variant_t varZero((int)0);

	for ( int i=0; i<m_wndDim.GetRowCount(); i++ )
	{
		pCxRow = (CCxNDArrayDimRow*)m_wndDim.GetRow(i);
		if ( pCxRow == NULL ) { ::MessageBox(GetMainWnd(), "获取指定行数据失败，程序异常", g_lpszAppTitle, MB_OK|MB_ICONERROR); return; }
		pValueItem = (CCxNDArrayDimItem*)pCxRow->GetItem(COL_DIM_VALUE);
		if ( pValueItem == NULL ) { ::MessageBox(GetMainWnd(), "获取指定维尺寸数据失败，程序异常", g_lpszAppTitle, MB_OK|MB_ICONERROR); return; }

		ConvertBSTR2CString( strType, pCxRow->GetItem(COL_DIM_TYPE)->GetValue().bstrVal );
		
		pValueItem->m_varValue.ChangeType(VT_INT);
		if ( strType.CompareNoCase("Row") == 0 )
		{
			pValueItem->EnableSpinControl( FALSE );
			pValueItem->SetValue(varZero, FALSE);
			nRowCnt = pNDArray->GetDataCount( i );
			pValueItem->SetTextColor( g_crStructTreeTextInvalid );
			pValueItem->Enable(FALSE);
		}
		else if ( strType.CompareNoCase("Column") == 0 )
		{
			pValueItem->EnableSpinControl( FALSE );
			pValueItem->SetValue(varZero, FALSE);
			nColCnt = pNDArray->GetDataCount( i );
			pValueItem->SetTextColor( g_crStructTreeTextInvalid );
			pValueItem->Enable(FALSE);
		}
		else if ( strType.CompareNoCase("Index") == 0 )
		{
			pIndexValueItem = pValueItem;
			pValueItem->SetTextColor( RGB(0,0,0) );
			//int nDivCnt = nRowCnt * nColCnt;
			//nIdxCnt = nDivCnt > 0 ? pNDArray->GetDataCount() / nDivCnt : -1;
			//pValueItem->EnableSpinControl(TRUE, 0, 0 );
			//pValueItem->SetValue(varZero, FALSE);
			pValueItem->Enable(TRUE);
		}
	}

	if ( pIndexValueItem != NULL )
	{
		int nDivCnt = nRowCnt * nColCnt;
		nIdxCnt = nDivCnt > 0 ? (pNDArray->GetDataCount() / nDivCnt) - 1 : 0;
		pIndexValueItem->EnableSpinControl(TRUE, 0, nIdxCnt );
		pIndexValueItem->SetValue(varZero, TRUE);
		pIndexValueItem->AllowEdit();
		pIndexValueItem->SetTextColor( RGB(0,0,0) );
		pIndexValueItem->Enable(TRUE);
	}

	m_wndDim.AdjustLayout ();
}
