#include "StdAfx.h"
#include "PeraProcessDesigner.h"
#include "ZTreeParamSet.h"
#include "DlgParamEdit.h"
#include "DlgParamSet.h"
#include <ConvertData.h>
#include <sstream>
#include <CStringChecker.h>
#include "CxStructTreeNodeParam.h"

#define PARAM_EVENT(event) \
	for(int nRowIndex = 0; nRowIndex < (int)m_vListeners.size(); nRowIndex++) \
	{ \
		m_vListeners[nRowIndex]->event(pParam->m_strName.c_str()); \
	} \

#define ADD_PARAM PARAM_EVENT(AddParam)
#define DELETE_PARAM PARAM_EVENT(DeleteParam)

#define RENAME_PARAM \
	for(int nRowIndex = 0; nRowIndex < (int)m_vListeners.size(); nRowIndex++) \
	{ \
		m_vListeners[nRowIndex]->RenameParam(pParam->m_strName.c_str(), str); \
	} \




ZTreeParamSet::ZTreeParamSet(void)
{
	m_pParamCur = NULL;
	m_pParamLast = NULL;
	m_pRowCur = NULL;
}

int ZTreeParamSet::GetCurRowIndex()
{
	return m_nCurRow ;
}

void ZTreeParamSet::SetCurRowIndex(int index)
{
	m_nCurRow = index;
}

void ZTreeParamSet::SetCurRowName(CString strCurRowName)
{
	m_strCurRowName = strCurRowName;
}

CString ZTreeParamSet::GetCurRowName()
{
	return m_strCurRowName;
}

ZTreeParamSet::~ZTreeParamSet(void)
{
	for(int i = 0; i < (int)m_vParams.size(); i++)
	{
		delete m_vParams[i];
		m_vParams[i] = NULL;
	}
	m_vParams.clear();
	DestroyMapJvNode();
}

void ZTreeParamSet::DestroyMvParams()
{
	for(int i = 0; i < (int)m_vParams.size(); i++)
	{
		delete m_vParams[i];
		m_vParams[i] = NULL;
	}
	m_vParams.clear();
}

BEGIN_MESSAGE_MAP(ZTreeParamSet, CZGridCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

//******************************************************************************************
BOOL ZTreeParamSet::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
#ifndef _BCGPGRID_STANDALONE
	if (CBCGPPopupMenu::GetActiveMenu () != NULL)
	{
		return TRUE;
	}
#endif

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return FALSE;
	}

	if (m_nVertScrollTotal <= m_nVertScrollPage)
	{
		return FALSE;
	}

	if ((m_fScrollRemainder > 0) != (zDelta > 0))
	{
		m_fScrollRemainder = 0;
	}

	UINT nLinesToScrollUserSetting;
	if (!::SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &nLinesToScrollUserSetting, 0))
	{
		nLinesToScrollUserSetting = 1;
	}

	if(nLinesToScrollUserSetting == WHEEL_PAGESCROLL) // scroll one page at a time
	{
		OnVScroll (zDelta < 0 ? SB_PAGEDOWN : SB_PAGEUP, 0, NULL);
		return TRUE;
	}

	float fTotalLinesToScroll = ((float)zDelta / WHEEL_DELTA) * nLinesToScrollUserSetting + m_fScrollRemainder;

	int nSteps = abs((int)fTotalLinesToScroll);
	m_fScrollRemainder = fTotalLinesToScroll - (int)fTotalLinesToScroll;

	for (int i = 0; i < nSteps; i++)
	{
		OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
	}

	return TRUE;
}

//******************************************************************************************
void ZTreeParamSet::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	if (m_nDraggedColumn >= 0 || m_bTracking || m_bSelecting)
	{
		return;
	}

	CBCGPGridRow* pSel = GetCurSel ();

// 	if (pSel && pSel->IsInPlaceEditing())
// 	{
// 		return;
// 	}

	if (pSel != NULL && 
		pScrollBar->GetSafeHwnd () != NULL)
	{
		CSpinButtonCtrl* pWndSpin = pSel->GetSpinWnd ();
		if (pWndSpin != NULL && 
			pWndSpin->GetSafeHwnd () == pScrollBar->GetSafeHwnd ())
		{
			return;
		}
	}

	SetEndEditReason (EndEdit_Cancel | EndEdit_Layout);
	EndEditItem (TRUE);

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_TRACKPOS;

	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
	{
		GetScrollInfo(SB_VERT, &info);
		nPos = info.nTrackPos;
	}

	int nPrevOffset = m_nVertScrollOffset;

	int nVertScrollPage = m_nVertScrollPage;
	if (m_bVirtualMode)
	{
		int nItemsCount = max (1, m_nVertScrollPage / m_nRowHeight);
		nVertScrollPage = nItemsCount * m_nRowHeight;
	}

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nVertScrollOffset -= m_nRowHeight;
		break;

	case SB_LINEDOWN:
		m_nVertScrollOffset += m_nRowHeight;
		break;

	case SB_TOP:
		m_nVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nVertScrollOffset = m_nVertScrollTotal;
		break;

	case SB_PAGEUP:
		m_nVertScrollOffset -= nVertScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nVertScrollOffset += nVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	m_nVertScrollOffset = max (0, min (m_nVertScrollOffset, 
		m_nVertScrollTotal - nVertScrollPage + 1));
	if (m_bVirtualMode)
	{
		// row alignment
		int nResidue = m_nVertScrollOffset % m_nRowHeight;
		if (nResidue > 0)
		{
			m_nVertScrollOffset -= nResidue;
			if (m_nVertScrollOffset + m_nVertScrollPage < m_nVertScrollTotal)
			{
				m_nVertScrollOffset += m_nRowHeight;
			}
		}
	}

	if (m_nVertScrollOffset == nPrevOffset)
	{
		return;
	}

	OnUpdateVScrollPos (m_nVertScrollOffset, nPrevOffset);

	SetScrollPos (SB_VERT, m_nVertScrollOffset);

	int dy = nPrevOffset - m_nVertScrollOffset;

	ShiftItems (0, dy);
	OnPosSizeChanged ();
	m_rectTrackSel = OnGetSelectionRect ();
	ScrollWindow (0, dy, m_rectList, m_rectList);
	ScrollWindow (0, dy, m_rectRowHeader, m_rectRowHeader);

	pSel = GetCurSel ();
	if (pSel != NULL)
	{
		pSel->AdjustButtonRect ();
	}

	OnAfterVScroll (m_nVertScrollOffset, nPrevOffset);
	
	UpdateWindow ();
}


void ZTreeParamSet::DrawTreeLine( CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset )
{
	//if(!m_IsShowRoot) return;//显示选中节点时不画线
	if ( pCurRow == NULL ) return;
	if ( rcLeftOffset.IsRectEmpty() || rcLeftOffset.IsRectNull() ) return;

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

	int nPos = -1;

	rc.OffsetRect(-rc.Height(), 0);
	for (int i=0; i<nCount; i++)
	{
		rc.OffsetRect(rc.Height(), 0);
		ptCenter = rc.CenterPoint ();
		ptCenter.Offset(1, -2);

		if ( i + 1 == nCount && i != 0) //最后一个区域
		{
			//绘制一条 从中心点 到 右边界 的横线
			pDC->MoveTo (ptCenter.x, ptCenter.y);
			pDC->LineTo (rc.right, ptCenter.y);

			//绘制一条 从中心点 到 上边界 的竖线
			pDC->MoveTo (ptCenter.x, ptCenter.y);
			pDC->LineTo (ptCenter.x, rc.top - 1);

			//if(CString(pCurRow->GetItem(COL_NAME)->GetValue()) != CString("双击添加参数"))
			//if(pCurRow != GetRow(GetRowCount() - 1))
			CBCGPGridRow* pParent = pCurRow->GetParent();
			if (pParent)
			{
				int nCountSubItems = pParent->GetSubItemsCount(FALSE);
				if (pCurRow != pParent->GetSubItem(nCountSubItems - 1))
				{
					//绘制一条 从中心点 到 下边界 的竖线
					pDC->MoveTo (ptCenter.x, ptCenter.y);
					pDC->LineTo (ptCenter.x, rc.bottom);
				}
			}
		}
		else //不是最后一个区域，之前的区域，绘制竖线。
		{
			if ( i == 0 ) continue;
			//绘制一条 从上边界 到 下边界 的竖线
			pDC->MoveTo (ptCenter.x, rc.top);
			pDC->LineTo (ptCenter.x, rc.bottom);
		}
	}

	pDC->SelectObject (pOldPen);
}

ZTreeParam ZTreeParamSet::ConvertToParam(CCxStructTreeNodeParam* p)
{
	ZTreeParam zTreeParam;
	int nType = p->m_nDataType;

	std::string str;
	switch (nType)
	{
		case DT_STRING:
			str = "String";
			break;
		case DT_DOUBLE:
			str = "Double";
			break;
		case DT_BOOLEAN:
			str = "bool";
			break;
		case DT_INT:
			str = "Int";
			break;
		case DT_FILE:
			str = "File";
			break;
		case DT_ARRAY1D_BOOLEAN:
		case DT_ARRAY2D_BOOLEAN:
		case DT_ARRAY3D_BOOLEAN:
			str = "bool[]";
			break;
		case DT_ARRAY1D_INT:
		case DT_ARRAY2D_INT:
		case DT_ARRAY3D_INT:
			str = "Int[]";
			break;
		case DT_ARRAY1D_DOUBLE:
		case DT_ARRAY2D_DOUBLE:
		case DT_ARRAY3D_DOUBLE:
			str = "Double[]";
			break;
		case DT_ARRAY1D_STRING:
		case DT_ARRAY2D_STRING:
		case DT_ARRAY3D_STRING:
			str = "String[]";
			break;
		default:
			break;
	}
	zTreeParam.m_strType = str;

	if(p->IsIn())
	{
		zTreeParam.m_strDirection = "In";
	}
	else if(p->IsInOut())
	{
		zTreeParam.m_strDirection = "InOut";
	}
	else if(p->IsOut())
	{
		zTreeParam.m_strDirection = "Out";
	}

	if(p->m_bIsDynamicArray)
	{
		zTreeParam.m_strIsDynamic = "true";
	}
	else
	{
		zTreeParam.m_strIsDynamic = "false";
	}

	if(p->m_bIsSaveInModel)
	{
		zTreeParam.m_strIsSaveInModel = "true";
	}
	else
	{
		zTreeParam.m_strIsSaveInModel = "false";
	}

	zTreeParam.m_strUnit = p->m_strUnits;
	zTreeParam.m_strValue = p->m_strValue;
	zTreeParam.m_strDesrciption = p->m_strDescrip;
	
	return zTreeParam;
}

//检查组名是否重名
int ZTreeParamSet::CheckSameGroup(MAPSTR_JV& MapStrJv, CString strGroup)
{
	MAPITER_JSNODE iterOld;
	// 将原来的组中的json对象删除
	iterOld = MapStrJv.find(strGroup);
	if (iterOld!=MapStrJv.end())//查找json对象所在的组，
		return 1;
	return 0;
}

//在Vector中查找参数名
int ZTreeParamSet::FindParam(VECTOR_JV& vectorParams, CString strName)
{
	ITER_JSVECTOR iterVector;
	//ITER_JSVECTOR iterFind;
	Json::FastWriter fWriter;
	string strText;
	CString strTemp;
	Json::Value jvParam;
	int nIndex = -1;
	int nVectorSize = vectorParams.size();
//	for (iterVector=vectorParams.begin();iterVector!=vectorParams.end();++iterVector)
	for (int i=0;i<nVectorSize;i++)
	{
		jvParam = vectorParams.at(i);
		strText = fWriter.write(jvParam);
		strTemp =jvParam["name"].isString()?jvParam["name"].asCString():"";
//		 = iterVector;
		
		if (!strTemp.Compare(strName))
		{
			nIndex = i;
			return nIndex; 
		}
	}
	return -1;
}


int ZTreeParamSet::ReNameGroup(ZTreeParam *pParamIn, CString strNewName)
{
	Json::FastWriter fwrite;
	ZTreeParam *pParam;
	MAPITER_JSNODE iterOld;
	MAPITER_STRJV iterParam;
	VECTOR_JV iterFind ;

	// 
	if (pParamIn==NULL)	return 0;
	int nIndex =0;
	CString strOldName; 

	//
	if (pParamIn->m_bIsGroup)//重命名的节点为组时
	{
		//需要修改参数所在的组名
		//修改组的节点名称
		strOldName = pParamIn->m_strName.c_str();
		iterOld = m_MapStrJv.find(strOldName);
		if (iterOld!=m_MapStrJv.end())//查找json对象所在的组，
		{
			iterOld->second->jsGroup["name"]=(LPCSTR)strNewName;
			iterOld->second->strGroupName = strNewName;
			iterFind=iterOld->second->vecJvNode;
			if (iterFind.size()>0)
			{
				ReNameParam(iterOld->second->vecJvNode, (LPCSTR)strNewName);
			}

			//重新插入新的节点
			JS_NODE*pJsNode = new JS_NODE;
			pJsNode->jsGroup = iterOld->second->jsGroup;
			pJsNode->strGroupName = strNewName;
			string stext=fwrite.write(pJsNode->jsGroup);
			memcpy(pJsNode, iterOld->second, sizeof(JS_NODE));
			m_MapStrJv.insert(pair<CString, JS_NODE*>(strNewName, pJsNode));
			/*if (iterOld->second!=NULL)
			{
				delete	iterOld->second;
				iterOld->second=NULL;
			}*/
		
			m_MapStrJv.erase(iterOld);
		}
	}
	else//重命名的节点为参数//找到对应的节点将此节点修改
	{
		strOldName = pParamIn->m_strName.c_str();
		CString strGroupName = pParamIn->m_strGroup.c_str();
		iterOld = m_MapStrJv.find(strGroupName);
		if (iterOld!=m_MapStrJv.end())//查找json对象所在的组，
		{
			iterFind=iterOld->second->vecJvNode;
			if (iterFind.size()>0)
			{
				nIndex = FindParam(iterOld->second->vecJvNode, strOldName);
				if(nIndex>=0)
				{
					iterOld->second->vecJvNode.at(nIndex)["name"]=(LPCSTR)strNewName;
					return 1;
				}
			}
		}else
		{
			Json::Value jvParam;
			iterParam = m_ParamMapList.find(strOldName);
			if (iterParam!=m_ParamMapList.end())
			{
				jvParam = iterParam->second;
				jvParam["name"]=(LPCSTR)strNewName;
				m_ParamMapList.insert(pair<CString, Json::Value>(strNewName, jvParam));
				m_ParamMapList.erase(iterParam);
			}
		}
	}

	//此处修改原因：组名和参数名重名时，修改参数名，组名也会重命名了
	std::vector<ZTreeParam*>::iterator iter;
	for(iter = m_vParams.begin(); iter != m_vParams.end(); iter++)
	{
		pParam = (*iter);
	
		//修改的节点为参数时
		if (pParam==pParamIn&&!(pParamIn->m_bIsGroup))
		{
			(*iter)->m_strName = strNewName;
			break;
		}
		//修改节点为组时
		if (pParamIn->m_bIsGroup)
		{	
			string	stroldGroupName = pParam->m_strGroup;
			if (!stroldGroupName.compare(strOldName))
				(*iter)->m_strGroup=strNewName;

			if (!(pParam->m_strName.compare(strOldName))&&(pParam->m_bIsGroup))
			{
				(*iter)->m_strName = strNewName;
			}
		}
	}

	return 1;
}

int ZTreeParamSet::ReNameParam(VECTOR_JV& vectorParams, string strNewGroup)
{
//	string strText;
//	Json::FastWriter jwrtier;
	ITER_JSVECTOR iterVector;
	int nVectorSize = vectorParams.size();
//	Json::Value jvParam;
	//for (iterVector=vectorParams.begin();iterVector!=vectorParams.end();iterVector++)
	//{
	//	
	//	//	for (iterVector=vectorParams.begin();iterVector!=vectorParams.end();++iterVector)
	//	for (int i=0;i<nVectorSize;i++)
	//		(*iterVector)["group"]=strNewGroup;
	//	strText = jwrtier.write(*iterVector);
	//}
	for (int i=0;i<nVectorSize;i++)
	{
		vectorParams.at(i)["group"]=strNewGroup;
//		strText = jwrtier.write(vectorParams.at(i));
	}


	return 1;
}
//
void ZTreeParamSet::UpdateTreeGroup(MAPSTR_JV MapStrJv)
{
	int nCount = GetRowCount();
	int nRow =0;
	vector<ZTreeParam*> vecParams;
	for(nRow = 1; nRow < nCount; nRow++)
	{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(nRow)->GetData();
		if (pParam!=NULL)
		{
			if ((!pParam->m_strGroup.compare(""))&&pParam->m_bIsGroup)
				vecParams.push_back(pParam);
		}
	}

	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs"); 	
	sort(vecParams.begin(),vecParams.end(),localeLessThan);
	setlocale(LC_ALL, curLocale.c_str());

	nRow=1;
	vector<ZTreeParam*>::iterator iterTemp;
	for (iterTemp=vecParams.begin();iterTemp!=vecParams.end();++iterTemp)
	{

		CZGridRow*pRow=(CZGridRow*)GetRow(nRow);

		if(pRow!=NULL)
		{
			pRow->SetData( reinterpret_cast<DWORD_PTR>(*iterTemp) );
			pRow->GetItem(COL_NAME)->SetValue((*iterTemp)->m_strName.c_str());
		}
		nRow++;
	}
}

void ZTreeParamSet::UpdateTreeParam()
{
	int nCount = 0;
	CZGridRow*pRow=NULL;
	CString strGroup;
	BOOL bRowIsGroup;
	int nSubRowCount=0;
	int i =0;
	vector<ZTreeParam*> vecParams;

	nCount=GetRowCount();
	if (nCount<4)return;
	for(i = nCount-1; i > 0; i--)
	{
		ZTreeParam* pParam = NULL;
		pParam = (ZTreeParam*)GetRow(i)->GetData();
		if (pParam!=NULL)
		{
			pRow=(CZGridRow *)GetRow(i);
			bRowIsGroup=pRow->IsGroup();
			if (bRowIsGroup)
			{
				nSubRowCount = pRow->GetSubItemsCount();
			}
			strGroup.Format("%s", pParam->m_strGroup.c_str());
			//if (!pParam->m_strGroup.compare(""))	
			if (strGroup.IsEmpty()&&pParam->m_bIsGroup)
			{
					i=i+nSubRowCount;
					for (int j=0;j<nSubRowCount;j++)
					{
						vecParams.pop_back();
					}
					
					break;
			}
			//if (pParam->m_strGroup.compare("")|| pParam->m_strGroup.compare("<无>"))
			//if (pParam->m_bIsGroup)
						
			vecParams.push_back(pParam);
		}
	}
	if (vecParams.size()<2)return;

	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs"); 	
	sort(vecParams.begin(),vecParams.end(),localeLessThan);
	setlocale(LC_ALL, curLocale.c_str());

	vector<ZTreeParam*>::iterator iterTemp;
	for (iterTemp=vecParams.begin();iterTemp!=vecParams.end();++iterTemp)
	{
		CZGridRow*pRow=(CZGridRow*)GetRow(++i);

		if(pRow!=NULL)
		{
			pRow->SetData( reinterpret_cast<DWORD_PTR>(*iterTemp) );
			UpdateRowByParam(pRow, *iterTemp);
			//pRow->GetItem(COL_NAME)->SetValue((*iterTemp)->m_strName.c_str());
		}//Sleep(100);
	}
	AdjustLayout();
}

//****************************************************************************************
void ZTreeParamSet::OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn)
{
   
   ZTreeParam* pParam = NULL;
    CString strExistTip="该参数名称已经存在!";
	CString strEmptyTip="参数名称不能为空!";
	CString strNameTip="参数名称";

   m_pRowCur = dynamic_cast<CZGridRow*>(GetRow(nRow));
   if (m_pRowCur==NULL)
	   return;

   pParam = reinterpret_cast<ZTreeParam*>(pItem->GetParentRow()->GetData());
   if (pParam!=NULL)
   {
	   if (pParam->m_bIsGroup)
	   {
		   strEmptyTip = "组名不能为空!";
		   strExistTip = "组名已经存在!";
		   strNameTip = "组名称";
	   }
   }
   
   if(true)
	{
// 		if(((CDlgParamSet*)GetParent())->m_dlgParamEdit.CheckRelatedParam() == FALSE)
// 		{//已关联，不允许修改
// 			pItem->SetValue(pParam->m_strName.c_str());
// 			return;
// 		}

		CString str;
		
		ConvertBSTR2CString( str, pItem->GetValue().bstrVal );

		str.Trim();
		if (pParam!=NULL)
		{
			if(!str.Compare(pParam->m_strName.c_str()))
			{	
				UpdateRowByParam(m_pRowCur,pParam);
				return;
			}
		}
		if(str.IsEmpty())//字符串空的验证
		{
			MessageBox( strEmptyTip, g_lpszAppTitle, MB_ICONWARNING);
			if(pParam)
			{
				pItem->SetValue(pParam->m_strName.c_str());
			}
			else
			{
				pItem->SetValue("双击添加参数");
			}
		}//命名是否符合规则
		else if(CStringChecker::CheckParamName(str) == FALSE)
		{
			MessageBox( strNameTip + CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
			if(pParam)
			{
				pItem->SetValue(pParam->m_strName.c_str());
			}
			else
			{
				pItem->SetValue("双击添加参数");
			}
		}//是否重名
		else if(CheckSameName(str, pParam) == FALSE)
		{
			if(pParam)
			{//不是结尾
				if(str == "双击添加参数")
				{
					MessageBox( "参数名称不能包含[双击添加参数]！", g_lpszAppTitle, MB_ICONWARNING);
					pItem->SetValue(pParam->m_strName.c_str());
					return;
				}

				CString strOldName=pParam->m_strName.c_str();
				RENAME_PARAM			 //重命名参数事件响应
				
				ReNameGroup(pParam, str);
				pParam->m_strName = str;	
				SetParam();
				if (!(pParam->m_bIsGroup))
					UpdateTreeParam();	//对不属于任何分组的参数进行排序

				//为组时,或者为组内参数时，进行排序
				if (pParam->m_bIsGroup||pParam->m_strGroup.compare(""))
						SortGroupByName();
				
				SetSelectedCurRow(pParam);
			}
			else
			{
				ZTreeParam* pParam = new ZTreeParam();
				pParam->Init();
				pParam->m_bIsGroup=FALSE;
				pParam->m_strName = str;
				m_vParams.push_back(pParam);
				ADD_PARAM //添加参数事件响应

				SetRowImage( m_pRowCur, pParam );
				m_pRowCur->SetData( reinterpret_cast<DWORD_PTR>( pParam ) );
				
				UpdateRowByParam(m_pRowCur, pParam);
				
				Json::Value jvAdd;
				CString strName;
				strName.Format("%s", pParam->m_strName.c_str());
				pParam->m_bIsGroup = FALSE;
				jvAdd = CDlgParamSetBase::ParamToJsonValue(*pParam);
				m_ParamMapList.insert(pair<CString, Json::Value>(strName,jvAdd));
				
				AddLast();
				AdjustLayout();
				CBCGPGridRow * pRow = GetCurSel();
				if (pRow!=NULL)SetCurSel(pRow);		
 
				if (!(pParam->m_bIsGroup))
					UpdateTreeParam();
				else
				{
					((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(FALSE);
					SortGroupByName();
				}
				pRow=(CZGridRow*)FindRowByData(reinterpret_cast<DWORD_PTR>(pParam));
				if (pRow!=NULL)
					m_pRowCur = (CZGridRow*)pRow;
				SetCurSel(m_pRowCur);
				m_pRowCur->HitTest(NULL);
				SetParam();
				//((CDlgParamSet*)GetParent())->m_btnAdd.Reset();
			}
			((CDlgParamSet*)GetParent())->m_bParamsChanged = TRUE;
		}
		else
		{
			MessageBox( strExistTip, g_lpszAppTitle, MB_ICONWARNING);
			if(pParam)
			{
				pItem->SetValue(pParam->m_strName.c_str());
			}
			else
			{
				pItem->SetValue("双击添加参数");
			}
		}
	}
	
	//解决修改参数后，点击两次对话框消失
	MouseClick();
	CZGridCtrl::OnItemChanged(pItem, nRow, nColumn);
}
void ZTreeParamSet::SetSelectedCurRow(ZTreeParam *pParam)
{

	if (pParam==NULL)	return;
	CZGridRow* pRow =NULL;
	pRow=(CZGridRow*)FindRowByData(reinterpret_cast<DWORD_PTR>(pParam));
	if (pRow!=NULL)
		m_pRowCur = (CZGridRow*)pRow;
	SetCurSel(m_pRowCur);
	m_pRowCur->HitTest(NULL);
}
int ZTreeParamSet::MouseClick()
{
	BOOL bIsIn;

	{
		POINT CurPoint;
		GetCursorPos(&CurPoint);
		CRect rect;
		((CDlgParamSetBase*)GetParent())->GetDlgItem(IDCANCEL)->GetWindowRect(&rect);  //要获取位置的控件ID
		ScreenToClient(rect); //rect.left 就是X坐标,rect.top 就是Y坐标
		bIsIn = rect.PtInRect(CurPoint);
		((CDlgParamSetBase*)GetParent())->GetDlgItem(IDOK1)->GetWindowRect(&rect);  //要获取位置的控件ID
		if(rect.PtInRect(CurPoint)||bIsIn) 
		{
			for (int i=0;i<10;i++)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			}
		}			
	}
	return 1;
}

void ZTreeParamSet::AddParamToGroup(ZTreeParam*pParam)
{
	int nRowIndex=0;
	CZGridRow* pRowRoot=NULL;
	CZGridRow* pRow=NULL;
	BOOL bIsInGroup = FALSE;
	CZGridRow* pCurRow = NULL;
	nRowIndex = GetGroupIndex(pParam->m_strGroup.c_str());

	pRow = CreateNewRow ();
	pCurRow = pRow;
	if (pParam->m_bIsGroup)
	{
		pRowRoot = (CZGridRow*)GetRow(0);
		pRowRoot->CBCGPGridRow::AddSubItem(pRow,FALSE);
	}else
	{
		//在外边添加
		if (!pParam->m_strGroup.compare("<无>")||!pParam->m_strGroup.compare(""))
		{
			nRowIndex;
			pRow = (CZGridRow*)GetRow(--nRowIndex);
			//pRowRoot->CBCGPGridRow::AddSubItem(pRow,FALSE);
		}
		else
		{
			//在组内添加
			pRowRoot = (CZGridRow*)GetRow(nRowIndex);
			pRowRoot->AllowSubItems(TRUE);
			pRowRoot->CBCGPGridRow::AddSubItem(pRow,FALSE);
			bIsInGroup = TRUE;
		}
	}
	
	UpdateRowByParam(pRow, pParam);	
	pRow->SetData( reinterpret_cast<DWORD_PTR>( pParam ) );

	if (!bIsInGroup)
	{
		//UpdateTreeParam();
		AddLast();
	}
	AdjustLayout();

	//pRow->HitTest(NULL);
	if (!(pParam->m_bIsGroup))
	{	
		UpdateTreeParam();
		if (pParam->m_strGroup.compare(""))
		{
			SortGroupByName();
		}
	}
	//设置当前选中的行并且高亮显示
	pCurRow=(CZGridRow*)FindRowByData(reinterpret_cast<DWORD_PTR>(pParam));
	pCurRow = GetVisibleParentRow(pCurRow);
	if (pCurRow!=NULL)
		m_pRowCur = pCurRow;
	SetCurSel(m_pRowCur);
	m_pRowCur->HitTest(NULL);
	return;
	//FindRowByData()
}

void ZTreeParamSet::AddNewGroup(ZTreeParam *ParamIn)
{
	if (ParamIn==NULL) return;

	int nIndex=0;
	CZGridRow* pRowRoot=NULL;
	CZGridRow* pRow=NULL;
	BOOL bIsInGroup = FALSE;
	CZGridRow* pCurRow = NULL;
	vector<ZTreeParam*> vecParams;

	int nCount = GetRowCount();
	nIndex = GetGroupIndex();
	nIndex++;

	int i = nIndex;//要插入的索引值
	for(; i < nCount-1; i++)
	{
		ZTreeParam* pParam = NULL;
		pParam = (ZTreeParam*)GetRow(i)->GetData();
		if (pParam==NULL)continue;
		if (pParam->m_strGroup=="")
			vecParams.push_back(pParam);
	}
	pRowRoot =(CZGridRow*) GetRow(0);
	pRow =(CZGridRow*) GetRow(nIndex);
	if (pRow==NULL)
	{
		pRow = CreateNewRow();
		pRowRoot->AddSubItem(pRow);
	}

	pRow->SetData( reinterpret_cast<DWORD_PTR>(ParamIn));
	UpdateRowByParam(pRow, ParamIn);
	pRow->AllowSubItems();	
	
	RebuildIndexes();
	AdjustLayout();
	i= nIndex;
	
	vector<ZTreeParam*>::iterator iterTemp;
	for (iterTemp=vecParams.begin();iterTemp!=vecParams.end();++iterTemp)
	{
		pRow = (CZGridRow*)GetRow(++i);
		
		if(pRow!=NULL)
		{
			pRow->SetData( reinterpret_cast<DWORD_PTR>(*iterTemp) );
			UpdateRowByParam(pRow, *iterTemp);
		}
	}
	
	AddLast();
	AdjustLayout();

	SortGroupByName();
	
	//设置当前选中的行并且高亮显示
	pCurRow=(CZGridRow*)FindRowByData(reinterpret_cast<DWORD_PTR>(ParamIn));
	pCurRow = GetVisibleParentRow(pCurRow);	
	if (pCurRow!=NULL)
		m_pRowCur = pCurRow;
	SetCurSel(m_pRowCur);
	m_pRowCur->HitTest(NULL);
	//SetParam();
	return;
}

void ZTreeParamSet::AddNode2Vec(ZTreeParam ParamIn)
{
	BOOL bFlag = TRUE; // 是否存在同名参数
	ZTreeParam* pParam = new ZTreeParam();
	pParam->m_bIsGroup = TRUE;

	pParam->Init();
	//
	bFlag = ParamIn.m_bIsGroup;
	pParam->m_strName = ParamIn.m_strName;
	pParam->m_strGroup = ParamIn.m_strGroup;
	pParam->m_bIsGroup = ParamIn.m_bIsGroup;
	string strParam = ParamIn.m_strName;

	m_vParams.push_back(pParam);

	if (bFlag)//添加组
	{
		AddNewGroup(pParam);
	}
	else	//添加参数
	{
		AddParamToGroup(pParam);
	}

	return ;
}

int  ZTreeParamSet::FindIndexByParamName(CString strName, CString strGroupName)
{
	int nIndex =-1;
	int nCount = GetRowCount();
	for(int i = 0; i < nCount; i++)
	{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();
		if(pParam)
		{
			if((strName == pParam->m_strName.c_str())
				&&!pParam->m_bIsGroup&&pParam->m_strGroup.c_str()==strGroupName)
			{
				nIndex = i;
				break;
			}
		}
	}

	return nIndex;

}
int ZTreeParamSet::GetGroupIndex()
{
	int nCount = GetRowCount();

	int i=0;
	//if (nCount==2)
	//	return 1;

	//最后一个节点的索引值
	i=nCount-2;

	for (; i >= 0;--i)
	{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();
		if (pParam == NULL)
		{
			break;
		}
		if (pParam->m_bIsGroup == TRUE)
		{
			break;
		}
		if (pParam->m_strGroup.length() != 0)
		{
			break;
		}
	}
	return i;
// 	ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();
// 	if (pParam==NULL)return i;
// 
// 	for(i=nCount-2;((pParam->m_strGroup=="")&&i>0); --i)
// 	{
// 		pParam = (ZTreeParam*)GetRow(i)->GetData();
// 		//break;
// 	}
// 
// 	return i;
}

int ZTreeParamSet::GetGroupIndex(CString str)
{
	int nIndex =-1;
	int nCount = GetRowCount();
	nIndex = nCount;
	for(int i = 0; i < nCount; i++)
	{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();

		if(pParam)
		{
			if((str == pParam->m_strName.c_str())&&(pParam->m_bIsGroup))
			{
				nIndex = i;
				break;
			}
		}
	}

	return nIndex;

}
int ZTreeParamSet::GetRowIDByParam(ZTreeParam*pParam)
{
	if (pParam==NULL)return -1;

	int nIndex =-1;
	int nCount = GetRowCount();
	nIndex = nCount;
	for(int i = 0; i < nCount; i++)
	{
		ZTreeParam* tmpParam = (ZTreeParam*)GetRow(i)->GetData();
		{
			if(tmpParam==pParam)
			{
				nIndex = i;
				break;
			}
		}
	}

	return nIndex;
}
BOOL ZTreeParamSet::CheckSameName(CString str)
{
	BOOL bRet = 0;

	int nCount = GetRowCount();

	for(int i = 0; i < nCount; i++)
	{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();
		
		if(pParam)
		{
			if(str == pParam->m_strName.c_str())
			{
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;

}

//
BOOL ZTreeParamSet::CheckSameName(CString str, ZTreeParam* pParamIn)
{
	BOOL bRet = FALSE;

	int nCount = GetRowCount();
	BOOL nGroup =FALSE;
	BOOL nfindGroup = 0;

	for(int i = 0; i < nCount; i++)
	{
		ZTreeParam* pParam = NULL;
		pParam = (ZTreeParam*)GetRow(i)->GetData();
		if(pParam)
		{
			if (pParamIn!=NULL)		//新建节点时为NULL
			{
				nGroup = pParamIn->m_bIsGroup;	//要查找的节点Node属性	
			}
			//树中的
			nfindGroup = pParam->m_bIsGroup;
			if(str == pParam->m_strName.c_str()&&(nfindGroup==nGroup))//重名时
			{
				bRet = TRUE;
				break;
			}
		}
	}
	return bRet;
}

void ZTreeParamSet::SetParam()
{
	CBCGPGridRow * pRow = GetCurSel();
	m_pRowCur = (CZGridRow*)pRow;

	//BOOL bIsGroup = pRow->IsGroup();
	//int nSubitem = pRow->GetSubItemsCount();
	if (pRow==NULL)
	{
		((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(FALSE);
		return;
	}
	ZTreeParam* pParam = (ZTreeParam*)pRow->GetData();

	//不判空，可能存在崩溃
	if (!pParam)
	{
		{
			m_pParamCur = NULL;
			((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(FALSE);
			//((CDlgParamSet*)GetParent())->m_btnDelete.ShowWindow(SW_HIDE);
			//((CDlgParamSet*)GetParent())->m_btnDelete.EnableWindow(FALSE);
			return;
		}
		return;
	}

//Modify by zhaoyw
//	if ( pRow != NULL )
//	if (!bIsGroup||nSubitem<=0)//不是组,对话框处于可编辑状态
	if (!pParam->m_bIsGroup)	//不是组,对话框处于可编辑状态
	{
		ZTreeParam * pData = reinterpret_cast< ZTreeParam * > ( pRow->GetData() );
		if ( pData != NULL )
		{	
			m_pParamCur = pData;
			ZTreeParam& zTreeParam = *pData;
			((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(TRUE);
			((CDlgParamSet*)GetParent())->m_dlgParamEdit.SetParamInfo(zTreeParam);
			//((CDlgParamSet*)GetParent())->m_btnDelete.ShowWindow(SW_SHOW);
			((CDlgParamSet*)GetParent())->m_btnDelete.EnableWindow(TRUE);
			return;					
		}
	}

	{
		m_pParamCur = NULL;
		((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(FALSE);
		//((CDlgParamSet*)GetParent())->m_btnDelete.ShowWindow(SW_HIDE);
		//((CDlgParamSet*)GetParent())->m_btnDelete.EnableWindow(FALSE);
		return;
	}
	
}

void ZTreeParamSet::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CBCGPGridRow::ClickArea clickArea;
	CBCGPGridItemID id;
	CBCGPGridItem* pHitItem = NULL;
	CBCGPGridRow* pHitRow = HitTest (point, id, pHitItem, &clickArea);
	CString strGroupName;
	
	if(pHitRow == NULL) return;
	CZGridCtrl::OnLButtonDown(nFlags, point);

	return;
}

//*****************************************************************************
void ZTreeParamSet::OnSelChanged (const CBCGPGridRange &range, BOOL bSelect)
{
	CZGridCtrl::OnSelChanged(range, bSelect);
	//((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(FALSE);
	//CZGridRow* pRow = m_pRowCur;
	//ZTreeParam* pParam=NULL;
	//if (pRow!=NULL)
	//	 pParam = (ZTreeParam*)pRow->GetData();
	//if (pParam==NULL) return;
	//int nSelcount = GetSelectionCount();
	int nSelcount = -1;
	CList <CBCGPGridItem*, CBCGPGridItem*> lstSelected;
	GetSelectedItems (lstSelected);
	nSelcount = lstSelected.GetCount();
	BOOL bEnable =((CDlgParamSet*)GetParent())->m_dlgParamEdit.ParamEditIsEnable();
	if (nSelcount>1)
	{
		((CDlgParamSet*)GetParent())->m_dlgParamEdit.Enable(FALSE);
		return;
	}
	
//	if(bSelect||range.m_nTop>0)
	if(bSelect)
	{
		if(m_rangeLast != range)
		{
			m_rangeLast = range;
			SetParam();
		}
		if (!bEnable&&m_rangeLast == range)
		{
			m_rangeLast = range;
			SetParam();
		}
	}
	else
	{
		if(bEnable)
			UpdateParamFromEditControl();
// 		m_pRowCur = NULL;
// 		m_pParamCur = NULL;
// 		SetCurSel(NULL);
// 		SetParam();
	}
}

void ZTreeParamSet::UpdateParamFromEditControl()
{
	if(m_pParamCur)
	{//先保存
		ZTreeParam paramOld = *m_pParamCur; //保存旧的

		std::string strName = m_pParamCur->m_strName;
		CDlgParamSet* pParent = (CDlgParamSet*)GetParent();
		*m_pParamCur = pParent->m_dlgParamEdit.GetParamInfo();
		m_pParamCur->m_strName = strName;

		if(!(paramOld == *m_pParamCur))
		{//与之前的不相等
			((CDlgParamSet*)GetParent())->m_bParamsChanged = TRUE;
		}
	}

}

void ZTreeParamSet::OnLButtonDown(UINT nFlags, CPoint point)
{
	//point.x = 0;
	CBCGPGridRow::ClickArea clickArea;
	CBCGPGridItemID id;
	CBCGPGridItem* pHitItem = NULL;
	CBCGPGridRow* pHitRow = HitTest (point, id, pHitItem, &clickArea);
	if(pHitRow == NULL) return;
	if (pHitRow->GetRowId()==0)
	{
	}else if (clickArea!=CBCGPGridRow::ClickExpandBox)
	{
		point.x = 0;
	}
	CZGridCtrl::OnLButtonDown(nFlags, point);
}

void ZTreeParamSet::AddNodeFromJson(Json::Value& jv)
{
	Json::Value& jvParams = jv["Parameters"];
	if(!jvParams.isArray())
	{
		return;
	}

	AddRoot();
	//CtlListSort(jvParams);

	//将参数进行分组
	SplitGroup(jv);

	//将参数进行排序
	//SortParamList();

	//将分组后的参数插入的树控件中
	AddParamToTree();
	//将外层的参数排序
	UpdateTreeParam();

	AddLast();
}

void ZTreeParamSet::SplitGroup(Json::Value &jv)
{
	Json::Value jvDSt;
	jvDSt["Parameters"] = Json::Value(Json::arrayValue);
	Json::Value jvParams = jv["Parameters"];

	CString strName;
	CString strGroupName;
	CString strIsGroupName;
	
	MAPITER_JSNODE iterTemp;
	//int ncount = 0;
	if(!jvParams.isArray())
	{
		return ;
	}

	for(int i = 0; i < (int)jvParams.size(); i++)
	{
		strName.Empty();
		strGroupName.Empty();

//		ncount = -1;

		Json::Value jvParam = jvParams[i];

		//测试字符串用的
#ifdef _DEBUG
		string strText;
		Json::FastWriter jWriter;
		strText = jWriter.write(jvParam);
#endif	

		strName = jvParam["name"].isString() ? jvParam["name"].asCString() : "";
		strGroupName = jvParam["group"].isString() ? jvParam["group"].asCString() : "";
		strIsGroupName = jvParam[""].isString()? jvParam["isgroup"].asCString():"";
//		ncount  = m_MapStrJv.count(strGroupName);	
		iterTemp = m_MapStrJv.find(strGroupName);

		if (strGroupName.Compare(""))	//不为空//不是组//为参数
		{
			jvParam["isgroup"]="false";
			//if (ncount>0)				//组名在map表中已经存在了
			if (iterTemp!=m_MapStrJv.end())
			{
				
				iterTemp->second->vecJvNode.push_back(jvParam);
			}
			else						//为参数且组名在map表中不存在时
			{
				Json::Value jtmp=jvParam;

				JS_NODE *pjsNode = new JS_NODE();

				pjsNode->strGroupName = strGroupName;
				jtmp["name"] = jvParam["group"];		//参数所在的组名
				jtmp["group"]= "";
				jtmp["isgroup"]="true";
				pjsNode->jsGroup = jtmp;
				//strText = jWriter.write(jvParam);
				pjsNode->vecJvNode.push_back(jvParam);//组名插入map中
				pair<MAPITER_JSNODE, bool> ret   =	\
					m_MapStrJv.insert(pair<CString, JS_NODE*>(strGroupName,pjsNode));
			
				if (!ret.second)        
				 {// word already in word_count
					 ret.first->second->vecJvNode.push_back(jvParam);  // increment counter
					 ret.first++;
				}

				//iterTemp = m_MapStrJv.find(strGroupName);//将参数插入对应的vector中
				//iterTemp->second->vecJvNode.push_back(jvParam);
			}
		}
		else//为空的情况					//是组的情况
		{
			//if (ncount<=0)				//是组但不在map列表中
			//{
			//	JS_NODE *pjsNode = new JS_NODE();
			//	pjsNode->strGroupName = strGroupName;
			//	pjsNode->jsGroup = jvParam;
			//	m_MapStrJv.insert(pair<CString, JS_NODE*>(strName,pjsNode));

			//}
//			jvParam[""]FALSE
			jvParam["isgroup"] = "false";
#ifdef _DEBUG
			strText = jWriter.write(jvParam);
#endif // _DEBUG
			//m_ParamMapList.push_back(jvParam);
			m_ParamMapList.insert(pair<CString, Json::Value>(strName,jvParam));
			//sort(m_ParamMapList.begin(), m_ParamMapList.end(), wordCompare1);
		}
	}
}

//数组的第一个是[组节点]
//
//{
//	"Parameters":
//	[
//		{
//			{"a":[{},{}]},
//			[{"b":[{},{}]}]
//		}
//	]
//}
//将数据插入树控件中去
Json::Value ZTreeParamSet::AddParamToTree()
{
	//最终存放新的分组数据
	Json::Value jvDSt;
	jvDSt["Parameters"] = Json::Value(Json::arrayValue);

	//存放参数列表
	Json::Value jvParams;

	MAPITER_JSNODE itertmp ;		//组的指示器
	VECTOR_JV	vectorParams;		//结构中的
	ITER_JSVECTOR iterVector;		//每个组中的参数指示器
	string strText;					//为调试方便使用字符串
	MAPITER_STRJV itermapJv;
	Json::FastWriter jWriter;
	CString strGroupName;
	CZGridRow* pRowRoot =NULL;
	int nIndRow = 0;
	CString strRowName;

	//对map中的vector进行排序
	SortParamList();
	//m_vParams.clear();
	for (itertmp=m_MapStrJv.begin();itertmp!=m_MapStrJv.end();itertmp++)
	{
		nIndRow++;
		strGroupName = itertmp->first;
		Json::Value jParam = itertmp->second->jsGroup;
		Json::Value jvParams;
		jvParams[strGroupName]=Json::Value(Json::arrayValue);
		jvParams[strGroupName].append(jParam);


		strText = jWriter.write(jvParams);
		strText = jWriter.write(jParam);
		//插入组
		pRowRoot = AddNodeFromParam(itertmp->second->jsGroup);
		
		strRowName =itertmp->first;
		if(!strRowName.Compare(m_strCurRowName))
			m_nCurRow = nIndRow;
		vectorParams = itertmp->second->vecJvNode;
		for (iterVector=vectorParams.begin();iterVector!=vectorParams.end();iterVector++)
		{
			nIndRow++;
			jvParams[strGroupName].append(*iterVector);
			strRowName = (*iterVector)["name"].asCString();
			if(!strRowName.Compare(m_strCurRowName))
				m_nCurRow = nIndRow;

			//插入组中的参数列表
			AddSubNodeFromParam(pRowRoot,*iterVector);
			strText = jWriter.write(*iterVector);
		}
		jvDSt["Parameters"].append(jvParams);
	}
	//ITER_JSVECTOR iterVectorParam;
	for (itermapJv=m_ParamMapList.begin();itermapJv!=m_ParamMapList.end();itermapJv++)
	{
		nIndRow++;
		AddNodeFromParam(itermapJv->second);
		strText = jWriter.write(itermapJv->second);
		strRowName = ((itermapJv->second))["name"].asCString();
		if(!strRowName.Compare(m_strCurRowName))
			m_nCurRow = nIndRow;
		jvDSt["Parameters"].append(itermapJv->second);
	}

	//m_ParamList
//	m_jv = jvDSt;
	strText = jWriter.write(jvDSt);
	return jvDSt;
}

void ZTreeParamSet::SortGroupByName()
{
	vector<ZTreeParam*>::iterator iter;
	vector<ZTreeParam*>::iterator iterInset;
	vector<ZTreeParam*> subRowlist;
	ZTreeParam* pParam=NULL;
	MAPITER_PARAM iterfind;
	int nSubRowCount = 0;
	int nvectorSize=0;
	MAP_PARAM m_MapZTreeParams;
	
	//m_MapZTreeParams.clear();
	int nCount = GetRowCount();
	for (int i = 0; i < nCount; i++)
	{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();
		if (!pParam)
		{
			continue;
		}
		if (pParam->m_bIsGroup)
		{
			iterfind = m_MapZTreeParams.find(pParam->m_strName.c_str());
			if (iterfind==m_MapZTreeParams.end())
				m_MapZTreeParams.insert(pair<CString,ZTreeParam*>\
				(pParam->m_strName.c_str(),pParam));
		}
		else
		{
			iterfind = m_MapZTreeParams.find(pParam->m_strGroup.c_str());
			if (iterfind==m_MapZTreeParams.end()&&pParam->m_strGroup!="")
			{
				int nGroupIndx = GetGroupIndex(pParam->m_strGroup.c_str());
				ZTreeParam* tmppParam = (ZTreeParam*)GetRow(nGroupIndx)->GetData();
				m_MapZTreeParams.insert(pair<CString,ZTreeParam*>\
				(tmppParam->m_strName.c_str(),tmppParam));
				iterfind = m_MapZTreeParams.find(pParam->m_strGroup.c_str());
			}
			if (pParam->m_strGroup!="")
			{
				vector<ZTreeParam*> tmpVector =iterfind->second->m_parmList;
				//if (tmpVector.size()>0)
				{
					vector<ZTreeParam*>::iterator tmpiter ;
					tmpiter =find(tmpVector.begin(),tmpVector.end(), pParam);
					//ZTreeParam* ptmparam=NULL;
					//ptmparam=(*tmpiter) ;
					if (tmpiter==tmpVector.end())
						iterfind->second->m_parmList.push_back(pParam);	
				}
			}
		}
	}
	int nIndex=0;
	CZGridRow*pRow =NULL;
	for (iterfind=m_MapZTreeParams.begin();iterfind!=m_MapZTreeParams.end();iterfind++)
	{
		//将list里面进行排序
		std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
		setlocale(LC_ALL, "chs"); 	
		sort(iterfind->second->m_parmList.begin(),\
			iterfind->second->m_parmList.end(),localeLessThan);
		setlocale(LC_ALL, curLocale.c_str());
		subRowlist = iterfind->second->m_parmList;

		pRow=(CZGridRow*)GetRow(++nIndex);

		if(pRow!=NULL)
		{
			pRow->SetData( reinterpret_cast<DWORD_PTR>(iterfind->second));
			//pRow->GetItem(COL_NAME)->SetValue((LPCTSTR)iterfind->first);
			UpdateRowByParam(pRow, (iterfind->second));
		}

		//实际有的参数个数
		nvectorSize = subRowlist.size();

		//在树控件中的item的个数
		nSubRowCount = pRow->GetSubItemsCount();

		int nInsert = nSubRowCount - nvectorSize;
		if (nvectorSize>=0)
		{
			iterInset = subRowlist.begin(); 
			if (nInsert<0)//需要添加Item的值
			{
				CBCGPGridRow* pRootRow=NULL;
				//将已经存在的row利用了
				for (int nsubIndex=1; nsubIndex<=nSubRowCount; nsubIndex++)
				{					
					pRow = (CZGridRow*)GetRow(nsubIndex+nIndex);
					pRow->SetData( reinterpret_cast<DWORD_PTR>( (*iterInset) ) );
					//pRow->GetItem(COL_NAME)->SetValue((*iterInset)->m_strName.c_str());
					UpdateRowByParam(pRow, *iterInset);
					++iterInset;
				}

				while(iterInset != subRowlist.end())//添加新的
				{
					if (pRow->IsGroup())
					{
						pRootRow = pRow;
					}
					else
						pRootRow = pRow->GetParent();
					if (pRootRow!=NULL)
					{
						pRow = CreateNewRow ();
						pRootRow->CBCGPGridRow::AddSubItem(pRow, TRUE);
						pRow->SetData( reinterpret_cast<DWORD_PTR>( (*iterInset) ) );
						UpdateRowByParam(pRow, (*iterInset));
						//pRow->GetItem(COL_NAME)->SetValue((*iterInset)->m_strName.c_str());
					}
					++iterInset;
				}
				RebuildIndexes();
				AdjustLayout();
			}
			else if (nInsert==0)
			{

				for (int nsubIndex=1; nsubIndex<=nSubRowCount; nsubIndex++)
				{					
					pRow = (CZGridRow*)GetRow(nsubIndex+nIndex);
					pRow->SetData( reinterpret_cast<DWORD_PTR>( (*iterInset) ) );
					//pRow->GetItem(COL_NAME)->SetValue((*iterInset)->m_strName.c_str());
					UpdateRowByParam(pRow, (*iterInset));
					++iterInset;
				}
				CBCGPGridRow* pRootRow=NULL;
				if (!nvectorSize)
				{
					while(iterInset != subRowlist.end())//添加新的
					{
						if (pRow->IsGroup())
						{
							pRootRow = pRow;
						}
						else
							pRootRow = pRow->GetParent();
						if (pRootRow!=NULL)
						{
							pRow = CreateNewRow ();
							pRootRow->CBCGPGridRow::AddSubItem(pRow, TRUE);
							pRow->SetData( reinterpret_cast<DWORD_PTR>( (*iterInset) ) );
							UpdateRowByParam(pRow, (*iterInset));
							//pRow->GetItem(COL_NAME)->SetValue((*iterInset)->m_strName.c_str());
						}
						++iterInset;
					}
					RebuildIndexes();
					AdjustLayout();
				}
			}
			else
			{
				int nsubIndex = 0;
				for (nsubIndex=1; nsubIndex<=nvectorSize; nsubIndex++)
				{					
					pRow = (CZGridRow*)GetRow(nsubIndex+nIndex);
					pRow->SetData( reinterpret_cast<DWORD_PTR>( (*iterInset) ) );
					UpdateRowByParam(pRow,(*iterInset));
					++iterInset;
				}

				//nIndex+=nsubIndex;
				int nDelete = nIndex+nsubIndex;
				int i=0;
				for (i=0;i<nInsert;i++)
				{
					RemoveRow(nDelete);
					RebuildIndexes();
					AdjustLayout();
				}
			}
		}
		nIndex+=nvectorSize;
	}

	for (iterfind=m_MapZTreeParams.begin();iterfind!=m_MapZTreeParams.end();iterfind++)
	{
		if (iterfind->second!=NULL)
		{
			//if (iterfind->second->m_parmList!=NULL)
			{

				if (iterfind->second->m_parmList.size()>0)
					iterfind->second->m_parmList.clear();
			}
		}

	}
	if (m_MapZTreeParams.size()>0)
		m_MapZTreeParams.clear();
	AdjustLayout();
}

void ZTreeParamSet::SortParamList()
{
	MAPITER_JSNODE iterTemp;
	for(iterTemp=m_MapStrJv.begin();iterTemp!=m_MapStrJv.end();++iterTemp)
	{
		VectorListSort(iterTemp->second->vecJvNode);
		//sort(iterTemp->second->vecJvNode.begin(),iterTemp->second->vecJvNode.end(),wordCompare1);
		//ZTools::WriteZToolsLog()
	}
}

void ZTreeParamSet::DestroyMapJvNode()
{
	//释放申请的内存
	MAPITER_JSNODE iterTemp;
	for(iterTemp=m_MapStrJv.begin();iterTemp!=m_MapStrJv.end();iterTemp++)
	{
		if (iterTemp->second!=NULL)
		{
			delete iterTemp->second;
			iterTemp->second = NULL;
		}
	}
}


#if 0
void ZTreeParamSet::AddNodeFromJson(Json::Value& jv)
{
	Json::Value& jvParams = jv["Parameters"];
	if(!jvParams.isArray())
	{
		return;
	}

	AddRoot();
	CtlListSort(jvParams);

	Json::FastWriter writer;
	std::string strText1 = writer.write(jvParams);
	for(int i = 0; i < (int)jvParams.size(); i++)
	{
		Json::Value& jvParam = jvParams[i];
		strText1 = writer.write(jvParam);
		Json::Value::Members JMembers =  jvParam.getMemberNames();

		for(size_t j = 0; j < JMembers.size(); j++)
		{		
			string& strName = JMembers[j];
			Json::Value jvtmpParams = jvParam[strName];
			strText1 = writer.write(jvtmpParams);
			size_t k=0;
			Json::Value jvtmpParam=jvtmpParams[k];

		//	if (jvtmpParam.type()==Json::nullValue) continue;

			CZGridRow* pRowRoot = AddNodeFromParam(jvtmpParam);
			for (k=1;k<jvtmpParams.size();k++)
			{
				jvtmpParam =jvtmpParams[k];
				strText1 = writer.write(jvtmpParam);
				AddSubNodeFromParam(pRowRoot,jvtmpParam);
			}
		}
		
	}
	AddLast();
}

BOOL ZTreeParamSet::AddParam(CZGridRow* pRowRoot, Json::Value jvtmpParams)
{
	int k = 0;
	if (jvtmpParams.type()==Json::arrayValue)
	{
		for (k=1;k<jvtmpParams.size();k++)
		{
			jvtmpParam =jvtmpParams[k];
			strText1 = writer.write(jvtmpParam);
			AddSubNodeFromParam(pRowRoot,jvtmpParam);
		}
		return TRUE;
	}
	return FALSE;
}
#endif
bool ZTreeParamSet::localeLessThan (ZTreeParam* j1, ZTreeParam* j2) 
{ 
	std::string s1 = j1->m_strName;
	std::string s2 = j2->m_strName;

	if( (j1->m_strDirection=="In"&&j2->m_strDirection=="In")||\
		(j1->m_strDirection=="Out"&&j2->m_strDirection=="Out")||\
		(j1->m_strDirection=="InOut"&&j2->m_strDirection=="InOut"))
	{
		if (strcoll(s1.c_str(), s2.c_str()) <0)
			return true;
		else
			return false;
	}
	
	if( (j1->m_strDirection=="In") &&(j2->m_strDirection=="Out"))	return true;
	if (j1->m_strDirection=="In"  &&j2->m_strDirection=="InOut")		return true;
	if (j1->m_strDirection=="Out"  &&j2->m_strDirection=="InOut")	return true;
				
	return false;
		
		
	/*if (j1->m_strDirection=="Out" && j2->m_strDirection=="In")	return false;
	if (j1->m_strDirection=="InOut"  &&j2->m_strDirection=="In")	return false;
	if (j1->m_strDirection=="InOut"  &&j2->m_strDirection=="Out")	return false;*/
	//if (j1->m_strDirection=="InOut"  &&j2->m_strDirection=="InOut")	return true;
	//if (j1->m_strDirection=="Out"  &&j2->m_strDirection=="Out")	return true;
	//if (j1->m_strDirection=="Out"  &&j2->m_strDirection=="Out")	return true;

	
} 


// ZTreeParamSet.cpp在此文件中的AddNodeFromJson函数中添加，
// 实现方法，是先对参数列表排序，然后插入到列表中
//比较两个单词，wordOne大，返回正数，小返回负数，相等返回0
int ZTreeParamSet::wordCompare( Json::Value& jvParamsOne, Json::Value& jvParamsTwo)
{	
	int nResult=1;
	CString strParamsOne; 
	CString strParamsTwo ; 
	std::string strParamsOnetmp  = jvParamsOne["name"].isString()? jvParamsOne["name"].asString():"";
	std::string strParamsTwotmp  = jvParamsTwo["name"].isString()? jvParamsTwo["name"].asString():"";

	std::string j1  = jvParamsOne["dir"].isString()? jvParamsOne["dir"].asString():"";
	std::string j2  = jvParamsTwo["dir"].isString()? jvParamsTwo["dir"].asString():"";

	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs"); 

	if( (j1=="In"&&j2=="In")||\
		(j1=="Out"&&j2=="Out")||\
		(j1=="InOut"&&j2=="InOut"))
	{
		if (strcoll(strParamsOnetmp.c_str(), strParamsTwotmp.c_str()) <0)
			return true;
		else
			return false;
	}

	if( (j1=="In") &&(j2=="Out"))	return true;
	if (j1=="In"  &&j2=="InOut")		return true;
	if (j1=="Out"  &&j2=="InOut")	return true;

	return false;



	nResult = strcoll(strParamsOnetmp.c_str(), strParamsTwotmp.c_str());

	setlocale(LC_ALL, curLocale.c_str());

	return nResult;
}

void ZTreeParamSet::wordSwap(Json::Value& jvParams_min, Json::Value&jvParams_i)
{   // 交换两个单词的内容。
	Json::Value jvTmp;
	jvTmp = jvParams_min;
	jvParams_min = jvParams_i;
	jvParams_i = jvTmp;
}

int ZTreeParamSet::VectorListSort(VECTOR_JV& jvParamVector)
{
	int nSize =(int)jvParamVector.size();
	int j,min;
	
	for(int i=0;i<nSize-1;i++)
	{         //选择法排序
		min=i;
		for(j=i+1;j<nSize;j++)
		{
			if(wordCompare(jvParamVector.at(j),jvParamVector.at(min))<0) min=j;
		}
		Json::Value jvTmp;
		jvTmp = jvParamVector.at(i);
		jvParamVector.at(i)=jvParamVector.at(min);
		jvParamVector.at(min)=jvTmp;

		//wordSwap(jvParamVector.at(min),jvParamVector.at(i));
	}
	return 0;
}

int ZTreeParamSet::CtlListSort(Json::Value& jvParams)
{
	int nSize =(int)jvParams.size();
	int j,min;

	for(int i=0;i<nSize-1;i++)
	{         //选择法排序
		min=i;
		for(j=i+1;j<nSize;j++)
		{
			if(wordCompare(jvParams[j],jvParams[min])<0) min=j;
		}
		wordSwap(jvParams[min],jvParams[i]);
	}

	return 0;
}



void ZTreeParamSet::AddLast()
{
	CZGridRow * pRow = NULL;
	try 
	{
		CZGridRow* pRowRoot = (CZGridRow*)GetRow(0);

		ASSERT_VALID(pRowRoot);

		pRow = CreateNewRow ();
		if ( pRow == NULL ) return;

		pRow->GetItem(COL_NAME)->SetValue("双击添加参数");

		pRow->GetItem(COL_NAME)->AllowEdit();

		pRowRoot->CBCGPGridRow::AddSubItem(pRow, FALSE);

		//pRow->GetItem(COL_NAME)->TextToVar("双击添加参数");
		
		pRow->Redraw();
		//pRow->Select(FALSE);
	}
	catch (...)
	{
		return;
	}

}

void ZTreeParamSet::AddRoot()
{
	CZGridRow * pRow = NULL;
	try 
	{
		if ( GetRowCount() != 0 ) return;
		//添加根节点
		pRow = CreateNewRow ();
		if ( pRow == NULL ) return;

		pRow->AllowSubItems ();

		AddRow( pRow, FALSE );

		UpdateRowByParam( pRow, NULL);
	}
	catch (...)
	{
		return;
	}

}


void ZTreeParamSet::SetRowImage( CZGridRow* pRow, ZTreeParam* pParam, BOOL bRedraw /*= FALSE*/ )
{
	if(pParam == NULL)
	{//ROOT
		CCxStructTreeNode* pNode = theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData();
		switch ( pNode->m_NodeType )
		{
		case CCxStructTreeNode::NT_ROOT:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_ROOT, bRedraw );
			break;
		case CCxStructTreeNode::NT_PARALLEL:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARALLEL, bRedraw );
			break;
		case CCxStructTreeNode::NT_SEQUENCE:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_SEQUENCE, bRedraw );
			break;
		case CCxStructTreeNode::NT_IF:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_IF, bRedraw );
			break;
		case CCxStructTreeNode::NT_FOR:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_FOR, bRedraw );
			break;
		case CCxStructTreeNode::NT_FRONT_FORM:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_FRONTFORM, bRedraw );
			break;
		case CCxStructTreeNode::NT_REAR_FORM:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_REARFORM, bRedraw );
			break;
		case CCxStructTreeNode::NT_GROUP:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_GROUP, bRedraw );
			break;
		default:
			return;
		}
		return;
	}
	SetParamImage( pRow, pParam, bRedraw);
}

void ZTreeParamSet::SetParamImage( CZGridRow* pRow, ZTreeParam* pParam, BOOL bRedraw /*= FALSE*/ )
{
	int nDir = -1;//add by zhaoyw

	if (pParam->m_bIsGroup == TRUE)
	{//组节点,nDir仍为-1
	}
	else if(pParam->m_strDirection == "In")
	{
		nDir = PARAM_DIR_IN;
	}
	else if(pParam->m_strDirection == "Out")
	{
		nDir = PARAM_DIR_OUT;
	}
	else if(pParam->m_strDirection == "InOut")
	{
		nDir = PARAM_DIR_INOUT;
	} 

	switch ( nDir)
	{
	case PARAM_DIR_IN:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_IN, bRedraw );
		break;
	case PARAM_DIR_OUT:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_OUT, bRedraw );
		break;
	case PARAM_DIR_INOUT:
			pRow->GetItem (COL_NAME)->SetImage ( IMG_PARAM_INOUT, bRedraw );
		break;
	default:
		if (pParam->m_bIsGroup == TRUE)
		{
			pRow->GetItem (COL_NAME)->SetImage ( IMG_GROUP, bRedraw );
		}
		break;
	}
}

BOOL ZTreeParamSet::UpdateRowByParam( CZGridRow* pRow, ZTreeParam* pParam )
{
	if ( pRow == NULL )
	{
		return FALSE;
	}

	SetRowImage( pRow, pParam );

	if(pParam == NULL)
	{//ROOT
		pRow->GetItem (COL_NAME)->SetValue ( "参数列表", FALSE );
		return TRUE;
	}

	pRow->GetItem (COL_NAME)->SetValue ( pParam->m_strName.c_str(), FALSE );
	pRow->GetItem (COL_NAME)->AllowEdit(TRUE);
	//this->SetReadOnly(TRUE);
	pRow->GetItem (COL_VALUE)->SetValue ( pParam->m_strValue.c_str(), FALSE );


	return TRUE;
}

//注意释放内存
BOOL ZTreeParamSet::RemoveGroup(MAPSTR_JV& mapStrJvNode, CString strGroupName)
{
	//
	MAPITER_JSNODE mapIterDel;
	ITER_JSVECTOR iterVector;
	mapIterDel = mapStrJvNode.find(strGroupName);
	
	if (mapIterDel!=mapStrJvNode.end())
	{
		mapIterDel->second->vecJvNode.empty();
		if ( mapIterDel->second!=NULL)
		{
			delete mapIterDel->second;
			mapIterDel->second =NULL;
		}
		mapStrJvNode.erase(mapIterDel);
		return TRUE;
	}
	return FALSE;
}

BOOL ZTreeParamSet::DeleteParamFromList(MAPSTR_JV& MapParams, Json::Value jvDel)
{
	ITER_JSVECTOR iterVector;
	ITER_JSVECTOR iterDel;
	CString strTemp;
	MAPITER_JSNODE mapIterDel;
	MAPITER_STRJV mapIterStrjv;
	BOOL bIsDelete=FALSE;
	CString strParadelName ;
	CString strGroupName ;

	strGroupName   = jvDel["group"].isString()?jvDel["group"].asCString():"";
	strParadelName = jvDel["name"].isString()?jvDel["name"].asCString():"";
	//参数存在两部分，一是不在任何组里面，二是在组中

	//删除组里面的参数
	mapIterDel = MapParams.find(strGroupName);
	if (mapIterDel!=MapParams.end())
	{
		bIsDelete = RemoveParamFromVec(mapIterDel->second->vecJvNode,jvDel);
	}

	if (!bIsDelete)
	{
		//删除不在任何组里面的参数
		mapIterStrjv = m_ParamMapList.find(strParadelName);
		if (mapIterStrjv!=m_ParamMapList.end())
		{
			m_ParamMapList.erase(mapIterStrjv);
			bIsDelete = TRUE;
		}
	}

	return bIsDelete;
}

int ZTreeParamSet::RemoveParamFromVec(VECTOR_JV& vectorParams, Json::Value jvDel)
{
	ITER_JSVECTOR iterVector;
	ITER_JSVECTOR iterDel;
	CString strTemp;	
	CString strParadelName;

	strParadelName = jvDel["name"].isString()?jvDel["name"].asCString():"";;

	for (iterVector=vectorParams.begin();iterVector!=vectorParams.end();iterVector++)
	{
		strTemp = (*iterVector)["name"].isString()?(*iterVector)["name"].asCString():"";
	//	iterDel = iterVector;
		if (!strTemp.Compare(strParadelName))
		{
			if (iterVector!=vectorParams.end())
			{
				vectorParams.erase(iterVector);
				//vectorParams.clear(*iterVector);
				return 1;
			}
		}
	}
	return 0;
}

BOOL ZTreeParamSet::DeleteFromvecParam(ZTreeParam* pParam)
{
	BOOL bRet = FALSE;

	std::vector<ZTreeParam*>::iterator iter;
	if (pParam==NULL)return FALSE;

	for(iter = m_vParams.begin(); iter != m_vParams.end(); iter++)
	{
		if(*iter == pParam)
		{
			DELETE_PARAM //删除参数事件响应
			delete pParam;
			m_vParams.erase(iter);
			bRet = TRUE;
			break;
		}	
	}
	return bRet;
}


BOOL ZTreeParamSet::ModifyParamGroupName(ZTreeParam* pParam, string strGroupName)
{
	BOOL bRet = FALSE;
	BOOL isSameGroup = TRUE;
	std::vector<ZTreeParam*>::iterator iter;

	for(iter = m_vParams.begin(); iter != m_vParams.end(); iter++)
	{
		if(pParam==(*iter))
		{
			if (strGroupName=="<无>")
				strGroupName="";
			
			(*iter)->m_strGroup = strGroupName;
			bRet = TRUE;
			break;
		}	
	}

//if (pParam->m_strGroup=="")
	//	RemoveRow();

	return bRet;
}
BOOL ZTreeParamSet::RemoveParam(ZTreeParam* pParam)
{
	BOOL bRet = FALSE;
	CString strGrouName;
	Json::Value jvDel ;
	

	strGrouName.Empty();
	if(pParam == NULL)	return bRet;
	
	strGrouName.Format("%s", pParam->m_strName.c_str());
	if (pParam->m_bIsGroup)//是组时删除组
	{
		RemoveGroup(m_MapStrJv, strGrouName);
	}
	else
	{
		////删除参数
		jvDel = CDlgParamSetBase::ParamToJsonValue(*pParam);
		DeleteParamFromList(m_MapStrJv, jvDel);
	}

	if (pParam->m_bIsGroup)
	{
		/*int nIndex = GetGroupIndex(pParam->m_strName.c_str());
		CZGridRow*pRow;
		pRow = (CZGridRow*)GetRow(nIndex);
		int ncount = pRow->GetSubItemsCount();
		int nAllCount = nIndex+ncount;
		for (int i=nIndex;i<=nAllCount;i++)
		{
		ZTreeParam* pParam = (ZTreeParam*)GetRow(i)->GetData();
		bRet = DeleteFromvecParam(pParam);
		}
		return bRet;*/
	}
	bRet = DeleteFromvecParam(pParam);
	
	return bRet;
}

BOOL ZTreeParamSet::AddSubNodeFromParam(CZGridRow* pRowRoot, Json::Value& jv )
{
		/*m_vParams.clear();*/
	if(!jv.isObject()||pRowRoot==NULL) return FALSE;
	ZTreeParam* pParam = new ZTreeParam();
	pParam->Init();
	m_vParams.push_back(pParam);
	ADD_PARAM //添加参数事件响应

	pParam->m_strID = jv["id"].isString() ? jv["id"].asString() : "";
	pParam->m_strName = jv["name"].isString() ? jv["name"].asString() : "";
	pParam->m_strValue = jv["value"].isString() ? jv["value"].asString() : "";
	pParam->m_strDirection = jv["direction"].isString() ? jv["direction"].asString() : "";
	pParam->m_strType = jv["type"].isString() ? jv["type"].asString() : "";
	pParam->m_strIsDynamic = jv["isdynamicarray"].isString() ? jv["isdynamicarray"].asString() : "";
	pParam->m_strIsSaveInModel = jv["issaveinmodel"].isString() ? jv["issaveinmodel"].asString() : "";
	pParam->m_strUnit = jv["units"].isString() ? jv["units"].asString() : "";
	pParam->m_strDesrciption = jv["desc"].isString() ? jv["desc"].asString() : "";
	pParam->m_strFormat = jv["format"].isString() ? jv["format"].asString() : "";
	pParam->m_strCnName= jv["cnname"].isString() ? jv["cnname"].asString() : "";
	pParam->m_strGroup = jv["group"].isString() ? jv["group"].asString() : "";
	pParam->m_strListName = jv["listname"].isString() ? jv["listname"].asString() : "";
	string strIsGroup = jv["isgroup"].isString() ? jv["isgroup"].asString() : "";
	
	if (!strIsGroup.compare("false"))
		pParam->m_bIsGroup = FALSE;
	else 
		pParam->m_bIsGroup = TRUE;

	//
	//CZGridRow * pRowRoot = NULL;
	CZGridRow * pRow = NULL;

	//pRowRoot = (CZGridRow*)GetRow(0);

	ASSERT_VALID(pRowRoot);

	pRow = CreateNewRow ();
	if ( pRow == NULL ) return FALSE;

	if ( !UpdateRowByParam( pRow, pParam ) )
	{
		return FALSE;
	}
//	pRow->AllowSubItems(TRUE);
//	pRow->SetFlags();
	pRowRoot->CBCGPGridRow::AddSubItem(pRow, FALSE);
	pRow->SetData( reinterpret_cast<DWORD_PTR>( pParam ) );

	return TRUE;
}

CZGridRow * ZTreeParamSet::AddNodeFromParam( Json::Value& jv )
{	
	if(!jv.isObject()) return NULL;
	ZTreeParam* pParam = new ZTreeParam();
	pParam->Init();
	m_vParams.push_back(pParam);

	ADD_PARAM //添加参数事件响应

	pParam->m_strID = jv["id"].isString() ? jv["id"].asString() : "";
	pParam->m_strName = jv["name"].isString() ? jv["name"].asString() : "";
	pParam->m_strValue = jv["value"].isString() ? jv["value"].asString() : "";
	pParam->m_strDirection = jv["direction"].isString() ? jv["direction"].asString() : "";
	pParam->m_strType = jv["type"].isString() ? jv["type"].asString() : "";
	pParam->m_strIsDynamic = jv["isdynamicarray"].isString() ? jv["isdynamicarray"].asString() : "";
	pParam->m_strIsSaveInModel = jv["issaveinmodel"].isString() ? jv["issaveinmodel"].asString() : "";
	pParam->m_strUnit = jv["units"].isString() ? jv["units"].asString() : "";
	pParam->m_strDesrciption = jv["desc"].isString() ? jv["desc"].asString() : "";
	pParam->m_strFormat = jv["format"].isString() ? jv["format"].asString() : "";
	pParam->m_strCnName= jv["cnname"].isString() ? jv["cnname"].asString() : "";
	pParam->m_strGroup = jv["group"].isString() ? jv["group"].asString() : "";
	pParam->m_strListName = jv["listname"].isString() ? jv["listname"].asString() : "";
	
	string strIsGroup = jv["isgroup"].isString() ? jv["isgroup"].asString() : "";
	if (!strIsGroup.compare("false"))
		pParam->m_bIsGroup = FALSE;
	else 
		pParam->m_bIsGroup = TRUE;
	
	//
	CZGridRow * pRowRoot = NULL;
	CZGridRow * pRow = NULL;

	pRowRoot = (CZGridRow*)GetRow(0);

	ASSERT_VALID(pRowRoot);

	pRow = CreateNewRow ();
	if ( pRow == NULL ) return FALSE;

	if ( !UpdateRowByParam( pRow, pParam ) )
	{
		return FALSE;
	}
	pRow->AllowSubItems(TRUE);
	
	pRowRoot->CBCGPGridRow::AddSubItem(pRow, FALSE);
	pRow->SetData( reinterpret_cast<DWORD_PTR>( pParam ) );

	return pRow;	
}

//查找能看到的、最近的父行
CZGridRow* ZTreeParamSet::GetVisibleParentRow(CZGridRow* pRow)
{
	if (pRow == NULL)
	{
		return NULL;
	}

	std::vector<CZGridRow*> vRows;
	vRows.push_back(pRow);

	CZGridRow* pParentRow = (CZGridRow*)pRow->GetParent();
	while (pParentRow)
	{
		vRows.push_back(pParentRow);
		pParentRow = (CZGridRow*)pParentRow->GetParent();
	}

	CZGridRow* pTopRow = NULL;
	for (int i = (int)vRows.size() - 1; i >= 0; i--)
	{
		pTopRow = vRows[i];
		if (pTopRow->IsExpanded())
		{
			continue;
		}
		else
		{
			break;
		}
	}
	return pTopRow;
}
BOOL ZTreeParamSet::IsGroupExpanded(CString str)
{
	int iIndex = GetGroupIndex(str);
	if (iIndex < 0) return FALSE;
	CBCGPGridRow * pRow = GetRow(iIndex);
	if (pRow==NULL) return FALSE;
	if (pRow->IsExpanded())
		return TRUE;

	return FALSE;
}