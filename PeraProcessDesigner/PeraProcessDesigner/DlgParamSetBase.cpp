

	// DlgParamSetBase.cpp : ʵ���ļ�
	//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgParamSetBase.h"
#include "afxdialogex.h"
#include "DlgParamEdit.h"
#include <sstream>
#include "CxStructTreeNodeArrayElement.h"
#include "CxStructTreeNodeParam.h"
#include "CxStructTreeNodeFlow.h"
#include "PeraProcessDesignerUtility.h"

// CDlgParamSetBase �Ի���single


IMPLEMENT_DYNAMIC(CDlgParamSetBase, CCxBCGPDialog)

CDlgParamSetBase::CDlgParamSetBase(LPCTSTR str, CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgParamSetBase::IDD, pParent)
{
	m_strJson = str;
	m_bParamsChanged = FALSE;
}

CDlgParamSetBase::~CDlgParamSetBase()
{

}

void CDlgParamSetBase::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BTN_ADDGROUP, m_btnAddGroup);
	DDX_Control(pDX, IDC_BTN_MOVE, m_btnMoveTo);
	
}


BEGIN_MESSAGE_MAP(CDlgParamSetBase, CCxBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgParamSetBase::OnBnClickedButtonDelete)
//	ON_BN_CLICKED(IDC_BTN_ADDPARAM,  &CDlgParamSetBase::OnBnClickedBtnAddParam)
	ON_BN_CLICKED(IDC_BUTTON_ADD,  &CDlgParamSetBase::OnBnClickedBtnAddParam)
//	ON_BN_CLICKED(IDC_BUTTON_ADD,    &CDlgParamSetBase::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BTN_ADDGROUP, &CDlgParamSetBase::OnBnClickedBtnAddGroup)	
	ON_BN_CLICKED(IDC_BTN_MOVE, &CDlgParamSetBase::OnBnClickedBtnMove)
//	ON_BN_CLICKED(IDC_BUTTON1, &CDlgParamSet::OnBnClickedButton1)	
	//ON_BN_CLICKED(IDC_BTN_GROUP, &CDlgParamSet::OnBnClickedBtnGroup)
	//ON_MESSAGE(WM_USER_SORTMSG,  &CDlgParamSetBase::OnProcName)��
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
//	ON_MESSAGE(WM_HOTKEY,&CDlgParamSetBase::OnHotKey) 
	ON_MESSAGE(WM_USER_SORTMSG, &CDlgParamSetBase::OnUserSortmsg)
END_MESSAGE_MAP()


void CDlgParamSetBase::SetParamChanged(BOOL bChanged)
{
	m_bParamsChanged = bChanged;
}
void CDlgParamSetBase::DeleteRowItem(){}

//LRESULT CDlgParamSetBase::OnHotKey(WPARAM  wParam,LPARAM  lParam)    
//{
//	CWnd* phwndFocus = GetFocus();
//	
//	if (m_wndGridTree.m_pRowCur==NULL)
//		return 0;
//	 	
//	BOOL bIsEdit =m_wndGridTree.m_pRowCur->IsInPlaceEditing();
//	CRect rect;
//	//getite
//	//CPoint CurPoint;
//	//GetCursorPos(&CurPoint);
//	//CRect rect=m_wndGridTree.m_pRowCur->GetRect();
//	//GetDlgItem(IDD_PARAM_EDIT,(HWND*)this).GetWindowRect(&rect);  //Ҫ��ȡλ�õĿؼ�ID
//	//CWnd*pWnd = GetDlgItem(IDC_EDIT_CNNAME);
//	CWnd *pwnd = (CWnd*)m_dlgParamEdit.GetDlgItem(IDC_EDIT_CNNAME);
//	
//	if (pwnd==phwndFocus)
//		return 0;
//
//	pwnd = (CWnd*)m_dlgParamEdit.GetDlgItem(IDC_COMBO_TYPE);
//	if (pwnd==phwndFocus)
//		return 0;
//
//	pwnd = (CWnd*)m_dlgParamEdit.GetDlgItem(IDC_EDIT_VALUE);
//	if (pwnd==phwndFocus)
//		return 0;
//
//	pwnd = (CWnd*)m_dlgParamEdit.GetDlgItem(IDC_COMBO_TYPE);
//	if (pwnd==phwndFocus)
//		return 0;
//
//	pwnd = (CWnd*)m_dlgParamEdit.GetDlgItem(IDC_COMBO_UNIT);
//	if (pwnd==phwndFocus)
//		return 0;
//
//	pwnd = (CWnd*)m_dlgParamEdit.GetDlgItem(IDC_EDIT_DESCRIPTION);
//	if (pwnd==phwndFocus)
//		return 0;
//	
//	if (!bIsEdit)
//	{
//		if (::IsWindowEnabled(m_hWnd))
//		{
//			if(wParam==IDC_BUTTON_ADD)  
//			{
//				OnBnClickedBtnAddParam();
//			}
//			if (wParam==IDC_BUTTON_DELETE&&m_wndGridTree.m_pRowCur!=NULL)
//			{
//				DeleteRowItem();
//			}
//		}
//	}	
//	
//	return 0;
//}

BOOL CDlgParamSetBase::GetParamChanged()
{
	return m_bParamsChanged;
}


Json::Value CDlgParamSetBase::ParamToJsonValue( ZTreeParam& param )
{
	Json::Value jv;

	jv["id"] = param.m_strID;
	jv["name"] = param.m_strName;
	jv["value"] = param.m_strValue;
	jv["direction"] = param.m_strDirection;
	jv["type"] = param.m_strType;
	jv["isdynamicarray"] = param.m_strIsDynamic;
	jv["issaveinmodel"] = param.m_strIsSaveInModel;
	jv["units"] = param.m_strUnit;
	jv["desc"] = param.m_strDesrciption;
	jv["format"] = param.m_strFormat;
	jv["cnname"] = param.m_strCnName;
	jv["group"] = param.m_strGroup;
	jv["listname"] = param.m_strListName;
	if (param.m_bIsGroup)
	{
		jv["isgroup"] = "true";
	}else
	{
		jv["isgroup"] = "false";
	}
	
	return jv;
}

void CDlgParamSetBase::UpdateJsonFromTree()
{
	Json::Value jv;
	Json::FastWriter jwrtier;
	Json::Value jvtmp;
	jv["Parameters"] = Json::Value(Json::arrayValue);
	jv["flowId"] = m_jv["flowId"];

	int nCount = m_wndGridTree.GetRowCount();

	for(int i = 0; i < nCount; i++)
	{
		ZTreeParam* pParam = (ZTreeParam*)m_wndGridTree.GetRow(i)->GetData();
		
		if(pParam)
		{
			int nSubItems = m_wndGridTree.GetRow(i)->GetSubItemsCount();

			if (pParam->m_bIsGroup)	continue;
			
			jvtmp = ParamToJsonValue(*pParam);
			jvtmp.removeMember("isgroup");
			jv["Parameters"].append(jvtmp);
			
		}
	}
	
	//add by zhaoyw�Բ�����������
	Json::Value& jvParams = jv["Parameters"];	//��������������
	ZTreeParamSet::CtlListSort(jvParams);

	m_strJson = jwrtier.write(jv);
}

void CDlgParamSetBase::UpdateTreeFromJson()
{
	m_wndGridTree.AddNodeFromJson(m_jv);
	m_wndGridTree.GetRow(0)->Expand(TRUE);
}

BOOL CDlgParamSetBase::OnInitDialog()
{
	CString strDir = GetExecDir() + "\\Res\\DlgParamSet\\";

	m_imageBarParam.Load(strDir + "BarParam.jpg");

	Json::Reader jR;
	if(!jR.parse(std::string(m_strJson), m_jv))
	{//����ʧ��
		EndDialog(0);
		return TRUE;
	}

	CCxBCGPDialog::OnInitDialog();
//	RegisterHotKey((HWND)this,IDC_BUTTON_ADD, 0, VK_INSERT); 
//	RegisterHotKey((HWND)this,IDC_BUTTON_DELETE, 0, VK_DELETE); 

	COLORREF clrBg = RGB(255,255,255);

	m_btnDelete.SetImage(GetDlgParamSetResImageData( "DlgParamSet_Btn_Delete" ), GetDlgParamSetResImageData( "DlgParamSet_Btn_DeleteGray" ));
	m_btnDelete.SetTooltip("ɾ��");

	m_btnAdd.SetImage(GetDlgParamSetResImageData( "DlgParamSet_Btn_Add" ), GetDlgParamSetResImageData( "DlgParamSet_Btn_AddGray" ));
	m_btnAdd.SetTooltip("��Ӳ���");

	m_btnAddGroup.SetImage(GetDlgParamSetResImageData( "DlgParamSet_Btn_AddGroup" ), GetDlgParamSetResImageData( "DlgParamSet_Btn_AddGroup" ));
	m_btnAddGroup.SetTooltip("�����");

	m_btnMoveTo.SetImage(GetDlgParamSetResImageData( "DlgParamSet_Btn_MoveTo" ), GetDlgParamSetResImageData( "DlgParamSet_Btn_MoveTo" ));
	m_btnMoveTo.SetTooltip("�ƶ���");

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE); // Set big icon

	EnableVisualManagerStyle (TRUE, TRUE);

	CRect rectStatic, rectWeb;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rectStatic);
	ScreenToClient(rectStatic);
	GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(rectWeb);
	ScreenToClient(rectWeb);

	CRect rectGrid;
	CWnd& m_wndTreeLocation = *GetDlgItem(IDC_STATIC_GRID);
	m_wndTreeLocation.GetClientRect (&rectGrid);
	m_wndTreeLocation.MapWindowPoints (this, &rectGrid);
	rectGrid.DeflateRect(1, 50, 1, 1);

	CRect rectRelate;
	GetDlgItem(IDC_STATIC_RELATED_GRID)->GetWindowRect(rectRelate);
	ScreenToClient(rectRelate);
	rectGrid.bottom = rectRelate.top;
	//rectGrid.DeflateRect(1, 1, 1, 1);
	CRect rectBtn;
	m_btnDelete.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);
	rectGrid.top = rectBtn.bottom + 3;
	m_wndGridTree.InitTree(rectGrid, this);

	m_wndGridTree.IsShowRoot(FALSE);
	m_wndGridTree.EnableHeader(FALSE);
	if(FALSE)
	{//���ڵ�
		m_wndGridTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.m_StructTree.GetRoot(),NULL );
	}
	else
	{//��ǰ�ڵ�
		//m_wndGridTree.AddNode( theApp.m_pBarJianmoTree->m_wndGridTree.GetCurSelNodeData(),NULL );
		UpdateTreeFromJson();
	}
	m_wndGridTree.AdjustLayout();


	m_dlgParamEdit.Create(IDD_PARAM_EDIT, this);
	CRect rectEdit;
	m_dlgParamEdit.GetWindowRect(rectEdit);
	rectEdit.MoveToXY(rectStatic.left - 5, rectWeb.bottom + 10);
	m_dlgParamEdit.MoveWindow(rectEdit);
	//m_dlgParamEdit.MoveWindow(rectStatic.left - 15, rectStatic.bottom + 20, rectStatic.Width() + 30, rectStatic.Height());
	m_dlgParamEdit.ShowWindow(SW_SHOW);

	m_dlgParamEdit.Enable(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgParamSetBase::OnOK()
{
	//CCxBCGPDialog::OnOK();
}

BOOL CDlgParamSetBase::RemoveParam(ZTreeParam* pParam)
{
	return m_wndGridTree.RemoveParam(pParam);
}

void CDlgParamSetBase::OnBnClickedButtonDelete()
{
	int nRet = MessageBox( "�Ƿ�ɾ���ò�����", g_lpszAppTitle, MB_OKCANCEL|MB_ICONQUESTION);
	switch (nRet)
	{
	case IDOK:
		{
			if(m_wndGridTree.m_pRowCur)
			{
				
				CZGridRow* pRow = m_wndGridTree.m_pRowCur;
				ZTreeParam* pParam = (ZTreeParam*)pRow->GetData();
				m_wndGridTree.m_pRowCur = NULL;
				m_wndGridTree.m_pParamCur = NULL;

				RemoveParam(pParam);
				m_wndGridTree.RemoveRow(pRow->GetRowId());
				m_wndGridTree.SetCurSel(NULL);
				m_wndGridTree.SetParam();
				m_wndGridTree.RebuildIndexes();
				m_wndGridTree.m_rangeLast.Set(-1, -1, -1, -1);
				//UpdateRelatedGrid();
				m_bParamsChanged = TRUE;
			}
		}
		break;
	case IDCANCEL:
	default:
		break;
	}

	//	m_btnDelete.Reset();
}

void CDlgParamSetBase::AddParamFromNode( Json::Value& jv, CCxStructTreeNode* pNode )
{
	for(int i = 0; i < (int)pNode->m_vChildren.size(); i++)
	{
		CCxStructTreeNode* p = pNode->m_vChildren[i];
		//if (p->m_NodeType != CCxStructTreeNode::NT_PARAM && p->m_NodeType != CCxStructTreeNode::NT_PARAM_ARRAY) continue;

		CCxStructTreeNodeParam * pPrarm = NULL;
		CCxStructTreeNodeArrayElement * pArrayElement = NULL;

		switch ( p->m_NodeType )
		{
		case CCxStructTreeNode::NT_GROUP:
			{//group�ڵ���ݹ�
				AddParamFromNode(jv, p);
				break;
			}
		case CCxStructTreeNode::NT_PARAM:
			{
				pPrarm = (CCxStructTreeNodeParam *)p;
				ZTreeParam param;
				param.Init();			
				param.m_strID = pPrarm->m_strID;
				param.m_strName = pPrarm->m_strOriName;
				param.m_strValue = pPrarm->m_strValue;
				CString str;
				switch(pPrarm->m_nDir)
				{
				case PARAM_DIR_IN:
					str = "In";
					break;
				case PARAM_DIR_INOUT:
					str = "InOut";
					break;
				case PARAM_DIR_OUT:
					str = "Out";
					break;
				}

				param.m_strDirection = str;
				param.m_strType = pPrarm->ConvertParamDataType(pPrarm->m_nDataType);
				param.m_strIsDynamic = pPrarm->m_bIsDynamicArray ? "true" : "false";
				param.m_strIsSaveInModel = pPrarm->m_bIsSaveInModel ? "true" : "false";
				param.m_strUnit = pPrarm->m_strUnits;
				param.m_strDesrciption = pPrarm->m_strDescrip;
				param.m_strFormat = pPrarm->m_strFormat;
				param.m_strCnName= pPrarm->m_strCnName;
				param.m_strGroup = pPrarm->m_strGroup;
				param.m_strListName = "";
				
				jv.append(ParamToJsonValue(param));
				break;
			}
		}
	}
}

std::string CDlgParamSetBase::GetJsonStrFromSelNode( CCxStructTreeNode* pNode )
{
	if(pNode == NULL)
	{
		return "";
	}

	Json::Value jv;

	jv["Parameters"] = Json::Value(Json::arrayValue);

	jv["flowId"] = pNode->m_strID.GetBuffer();

	Json::FastWriter jwrtier;
	
	AddParamFromNode(jv["Parameters"], pNode);

	return jwrtier.write(jv);
}



void CDlgParamSetBase::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CCxBCGPDialog::OnPaint()
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

	CPen pen(0, 1, RGB(151,151,151));
	dc.SelectObject(pen);
	CRect rectParamName;
	GetDlgItem(IDC_STATIC_PARAM_NAME)->GetWindowRect(rectParamName);
	ScreenToClient(rectParamName);
	dc.MoveTo(rectDraw.left, rectParamName.top - 7);
	dc.LineTo(rectDraw.right, rectParamName.top - 7);
}

void CDlgParamSetBase::UpdateTreeGrid(MAPSTR_JV MapStrJv)
{
	//ÿ�������
	int nIndex = -1;
	CZGridRow * pRow=NULL;
	m_wndGridTree.RemoveAll();
		
	m_wndGridTree.AddRoot();
	m_wndGridTree.DestroyMvParams();
	m_wndGridTree.AddParamToTree();

	nIndex = m_wndGridTree.GetCurRowIndex();
	pRow = (CZGridRow*)m_wndGridTree.GetRow(nIndex);

	if (pRow!=NULL)
	{
		m_wndGridTree.AddLast();
		m_wndGridTree.AdjustLayout();
		m_wndGridTree.SetCurSel(pRow);
		m_wndGridTree.SetParam();
		pRow->HitTest(NULL);
	}	
}

//�����
void CDlgParamSetBase::OnBnClickedBtnAddGroup()
{
	ZTreeParam param;
	memset(&param, 0,sizeof(ZTreeParam));
	MAPSTR_JV tmpMap = m_wndGridTree.m_MapStrJv;
	CDlgAddGroup dlgGroup(tmpMap);
	if(dlgGroup.DoModal()==IDOK)
	{
		if (dlgGroup.IsMapListChanged())
		{
			SetParamChanged(TRUE);
			m_wndGridTree.m_MapStrJv = dlgGroup.GetGroupList();
			//m_wndGridTree.m_ParamMapList= dlgGroup.GetParasList();
			m_wndGridTree.SetCurRowName(dlgGroup.GetAddGroupName());
			//UpdateTreeGrid(m_wndGridTree.m_MapStrJv);

			param.m_strName =dlgGroup.GetAddGroupName();
			param.m_bIsGroup = TRUE;
			m_wndGridTree.AddNode2Vec(param);
			m_dlgParamEdit.Enable(FALSE);
			//m_wndGridTree.SortGroupByName();
		}		
	}
}

//��Ӳ���
void CDlgParamSetBase::OnBnClickedBtnAddParam()
{
	MAPSTR_JV tmpMap = m_wndGridTree.m_MapStrJv;
	CDlgAddParam dlg(tmpMap, m_wndGridTree.m_vParams);
	dlg.SetParasList(m_wndGridTree.m_ParamMapList);

	ZTreeParam param;
	CZGridRow* pRow = NULL;
	ZTreeParam* pParam = NULL;

	memset(&param, 0, sizeof(ZTreeParam));
	pRow=(CZGridRow*)m_wndGridTree.m_pRowCur;

	if (pRow!=NULL)
	{
		pParam = reinterpret_cast<ZTreeParam*>(pRow->GetData());  
		if (pParam!=NULL)
		{
			if (pParam->m_bIsGroup)
				dlg.SetGroupName(pParam->m_strName.c_str());
			else
			{
				m_wndGridTree.AdjustLayout();
				dlg.SetGroupName(pParam->m_strGroup.c_str());
			}
		}
	}

	if(dlg.DoModal()==IDOK)
	{
		if (dlg.IsMapListChanged())
		{
			m_wndGridTree.SelectRow(-1);
			SetParamChanged(TRUE);
			m_wndGridTree.m_MapStrJv = dlg.GetGroupList();
			m_wndGridTree.m_ParamMapList= dlg.GetParasList();
			m_wndGridTree.SetCurRowName(dlg.GetAddParaName()) ;
			param.m_strGroup = (LPCSTR)dlg.GetGroupName();

			param.m_strName =dlg.GetAddParaName();
			m_wndGridTree.AddNode2Vec(param);

 			m_wndGridTree.SetParam();
		}
	}
}

UINT CDlgParamSetBase::MoveSingleNode(ZTreeParam *pParam, string strGroupName)
{
	int nCurIndex  = 0;
	int nSelcount  = 0;
	int nCount     = 0;
	CZGridRow* pRow = NULL;
	Json::Value mvJson;
	string strOldGroup;
	int  nMoveType = GROUP_TO_GROUP;

	//if (pParam==NULL)	return nMoveType;

	//���ɾ���ڵ������ֵ
	/*nCurIndex = m_wndGridTree.FindIndexByParamId()
		m_wndGridTree.m_pRowCur->GetRowId();*/
	
	SetParamChanged(TRUE);

	strOldGroup = pParam->m_strGroup;
	pRow=(CZGridRow*)m_wndGridTree.FindRowByData(reinterpret_cast<DWORD_PTR>(pParam));
	nCurIndex = pRow->GetRowId();
	m_wndGridTree.ModifyParamGroupName(pParam, strGroupName);

	////�޸ı༭���е�����
	//m_dlgParamEdit.SetParamInfo(*pParam);
	//m_wndGridTree.SetCurRowName(pParam->m_strName.c_str());

	//�ƶ����
	if (strGroupName=="<��>"||strGroupName=="")
	{
		nCount = m_wndGridTree.GetRowCount();

		pRow = (CZGridRow*)m_wndGridTree.GetRow(--nCount);

		pRow->SetData(reinterpret_cast<DWORD_PTR>(pParam));
		m_wndGridTree.UpdateRowByParam(pRow, pParam);



		m_wndGridTree.RemoveRow(nCurIndex);
		m_wndGridTree.AddLast();
		nMoveType = GROUP_TO_OUT;
	}
	else
	{
		//������ߵĲ����ƶ���ĳ������ʱ
		if (strOldGroup==""||strOldGroup=="<��>")
		{

			m_wndGridTree.RebuildIndexes();
			m_wndGridTree.AdjustLayout();
			m_wndGridTree.SortGroupByName();
			m_wndGridTree.RemoveRow(++nCurIndex);	
			nMoveType = OUT_TO_GROUP;
		}
	}
	m_wndGridTree.RebuildIndexes();
	m_wndGridTree.AdjustLayout();

	return nMoveType;
}

int CDlgParamSetBase::GetSelMoveParamCount()
{
	int  ncount  =0;
	ZTreeParam* pParam  = NULL;
	CBCGPGridItem*pItem = NULL;
	CZGridRow  *pRow =  NULL;
	CList <CBCGPGridItem*, CBCGPGridItem*> lstSelected;
	m_wndGridTree.GetSelectedItems (lstSelected);

	for (POSITION pos = lstSelected.GetHeadPosition(); pos != NULL; )
	{
		pItem = lstSelected.GetAt(pos);
		lstSelected.GetNext(pos);
		pRow = (CZGridRow*)pItem->GetParentRow();
		pParam = (ZTreeParam*)pRow->GetData();

		if (pParam==NULL)	
			continue;
		if(pParam->m_bIsGroup)
			continue;
		ncount++;
		//vecParams.push_back(pParam);
	}

	return ncount;

}
int CDlgParamSetBase::GetMoveParamlst(vector<ZTreeParam*> &vecParams, CString strGroup)
{
	CString strCompareGroup;
	int  ncount  =0;
	ZTreeParam* pParam  = NULL;
	CBCGPGridItem*pItem = NULL;
	CZGridRow  *pRow =  NULL;
	CList <CBCGPGridItem*, CBCGPGridItem*> lstSelected;
	m_wndGridTree.GetSelectedItems (lstSelected);
	strCompareGroup =strGroup;
	for (POSITION pos = lstSelected.GetHeadPosition(); pos != NULL; )
	{
		pItem = lstSelected.GetAt(pos);
		lstSelected.GetNext(pos);
		pRow = (CZGridRow*)pItem->GetParentRow();
		pParam = (ZTreeParam*)pRow->GetData();

		if (!strGroup.Compare("<��>"))
			strCompareGroup.Empty();
		if (pParam==NULL)	
			continue;
		if (pParam->m_bIsGroup)
			continue;
		if (pParam->m_strGroup.c_str()==strCompareGroup&&!pParam->m_bIsGroup)
			continue;
		
		ncount++;
		vecParams.push_back(pParam);
	}
	return ncount;
}

void CDlgParamSetBase::MoveSelNodes(vector<ZTreeParam*>  &vecParams,string strGroupName)
{
	int  ncount  =0;
	UINT nMoveType=0;
	ZTreeParam* pParam  = NULL;
	CBCGPGridItem*pItem = NULL;
	CZGridRow  *pRow =  NULL;

	vector<ZTreeParam*>::iterator iterTemp;

	nMoveType = GROUP_TO_GROUP;

	for (iterTemp=vecParams.begin();iterTemp!=vecParams.end();++iterTemp)
	{
		pParam=*iterTemp;
		if (pParam!=NULL)
		{
			if (pParam->m_bIsGroup)		
				continue;
			else
				nMoveType = MoveSingleNode(pParam, strGroupName);
		}
	}

	if(nMoveType==GROUP_TO_GROUP)
		m_wndGridTree.SortGroupByName();

	if (nMoveType==GROUP_TO_OUT)
		m_wndGridTree.UpdateTreeParam();
}

void CDlgParamSetBase::OnBnClickedBtnMove()
{
	int nCurIndex  = 0;
	int nSelcount  = 0;
	ZTreeParam *pParam = NULL;	
	Json::Value mvJson;
	vector<ZTreeParam*>  vecParamslst;
	CList <CBCGPGridItem*, CBCGPGridItem*> lstSelected;

	//ѡ�еĽڵ���
	m_wndGridTree.GetSelectedItems (lstSelected);
	int nSelcountRow = lstSelected.GetCount();

	//Ҫ�ƶ��Ĳ�������
	nSelcount = GetSelMoveParamCount();	

	if (nSelcount<=0)
	{		
		AfxMessageBox("��ѡ�в����ڵ�!");
		return;
	}
	if (nSelcount ==1 && m_wndGridTree.m_pRowCur==NULL)
	{
		AfxMessageBox("�߼�����!");
		return;
	}

	
	//�ƶ������ڵ�ʱ�����õ���Ϣ
	if (nSelcount==SINGLE_MOVE_NODE && nSelcountRow == SINGLE_MOVE_NODE)
	{	
		//��ȡ��ǰҪ�ƶ��Ĳ�����Ϣ	
		pParam = m_wndGridTree.m_pParamCur;
		*pParam =m_dlgParamEdit.GetParamInfo();
		 mvJson = ParamToJsonValue(*pParam);
		 nCurIndex = m_wndGridTree.m_pRowCur->GetRowId();
	}

	CDlgMovePara dlg(m_wndGridTree.m_MapStrJv);

	dlg.SetDeletNode(mvJson);
	dlg.SetParasList(m_wndGridTree.m_ParamMapList);

	if(dlg.DoModal()==IDOK)
	{
		{
			CString strGroupName = dlg.GetMoveGroupName();
			string strName(strGroupName);
			
			//�ƶ������ڵ�
			if (nSelcount==SINGLE_MOVE_NODE)
			{
				if (dlg.IsMapListChanged())
				{
					SetParamChanged(TRUE);
					string strOldGroup = pParam->m_strGroup;

					m_wndGridTree.ModifyParamGroupName(pParam,strName);

					//�޸ı༭���е�����
					m_dlgParamEdit.SetParamInfo(*pParam);
					m_wndGridTree.SetCurRowName(dlg.GetMoveParaName());
					if (strName=="<��>"||strName=="")
					{
						int nCount = m_wndGridTree.GetRowCount();

						CZGridRow* pRow = (CZGridRow*)m_wndGridTree.GetRow(--nCount);

						pRow->SetData(reinterpret_cast<DWORD_PTR>(pParam));
						m_wndGridTree.UpdateRowByParam(pRow, pParam);

						m_wndGridTree.UpdateTreeParam();
						m_wndGridTree.RemoveRow(nCurIndex);
						m_wndGridTree.AddLast();
						//return;
					}
					else
					{
						m_wndGridTree.SelectRow(-1);
						m_wndGridTree.SortGroupByName();
						if (strOldGroup==""||strOldGroup=="<��>")
						{
							m_wndGridTree.RebuildIndexes();
							m_wndGridTree.AdjustLayout();
							m_wndGridTree.RemoveRow(++nCurIndex);	
						}
					}

					m_wndGridTree.RebuildIndexes();
					m_wndGridTree.AdjustLayout();

					if (strName!="" && strName!="<��>" && !m_wndGridTree.IsGroupExpanded(strName.c_str()))
					{
						SelectCurRow(NULL);
						m_dlgParamEdit.Enable(FALSE);
					}
					else
						SelectCurRow(pParam);
					return;
				}
				else
					return; // �ƶ�����ǰ�飬���ϵû�б仯
			}
			//
			nSelcount = GetMoveParamlst(vecParamslst,strGroupName);
			SelectCurRow(NULL);
			//�ƶ�����ڵ�
			if (nSelcount>=SINGLE_MOVE_NODE)
			{
				m_dlgParamEdit.SetParamEditIsEnable(FALSE);
				MoveSelNodes(vecParamslst,strName);	
				SetParamChanged(TRUE);
				m_dlgParamEdit.Enable(FALSE);
				return;
			}
		}
	}
}

void CDlgParamSetBase::SelectCurRow(ZTreeParam *pParam)
{	
	m_wndGridTree.SelectRow(-1);
	m_wndGridTree.AdjustLayout();
	m_wndGridTree.SetCurSel(NULL);
	m_wndGridTree.AdjustLayout();
	CZGridRow* pCurRow=NULL;

	if (pParam == NULL)
	{
		//pCurRow = (CZGridRow*)m_wndGridTree.GetRow(0);
	}
	else
	{
		pCurRow=(CZGridRow*)m_wndGridTree.FindRowByData(reinterpret_cast<DWORD_PTR>(pParam));
	}
	//if (pCurRow!=NULL)
	m_wndGridTree.m_pRowCur = pCurRow;
	m_wndGridTree.SetCurSel(m_wndGridTree.m_pRowCur);
	//m_wndGridTree.m_pRowCur->HitTest(NULL);
}



BOOL CDlgParamSetBase::OnEraseBkgnd(CDC* pDC)
{
	BOOL b = CCxBCGPDialog::OnEraseBkgnd(pDC);

	CRect rect;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rect);
	ScreenToClient(rect);
	CRect rectBtn;
	m_btnDelete.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);
	m_imageBarParam.Draw2(pDC->GetSafeHdc(), rect.left + 1, rectBtn.top - 4, rect.Width() - 2, rectBtn.Height() + 7);
	return b;
}

BOOL CDlgParamSetBase::PreTranslateMessage(MSG* pMsg)
{
	HACCEL hAcclAdd;
	HACCEL hAcclDel;
	HACCEL hAcclAddGroup;
	HACCEL hAcclMoveGroup;

	int iResult;

	//���WM_KEYDOWN��Ϣ��WM_SYSKEYDOWN��Ϣ�������ݼ�
	switch(pMsg->message)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:

			hAcclAdd = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_INSERT));
			hAcclDel = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_DELETE));

			hAcclAddGroup = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_ADDGROUP));
			hAcclMoveGroup = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCELE_MOVEGROUP));

			iResult = TranslateAccelerator(m_hWnd,hAcclAdd,pMsg);
			//�����ݼ��ɹ�������TRUE
			if(iResult)
			{
				OnBnClickedBtnAddParam();
				return TRUE;
			}
			iResult = TranslateAccelerator(m_hWnd,hAcclDel,pMsg);
			if (iResult)
			{
				DeleteRowItem();
				return TRUE;
			}
			iResult = TranslateAccelerator(m_hWnd,hAcclAddGroup,pMsg);
			if (iResult)
			{
				OnBnClickedBtnAddGroup();
				return TRUE;
			}
			iResult = TranslateAccelerator(m_hWnd,hAcclMoveGroup,pMsg);
			if (iResult)
			{
				OnBnClickedBtnMove();
				return TRUE;
			}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}
afx_msg LRESULT CDlgParamSetBase::OnUserSortmsg(WPARAM wParam, LPARAM lParam)
{
	ZTreeParam * pParam=NULL;
	CZGridRow *pCurRow=NULL;

	pParam = (ZTreeParam *)wParam;
	pCurRow = (CZGridRow*)lParam;

	if (pParam==NULL || pCurRow==NULL) return 0;

	if (pParam->m_strGroup.compare("")==0)
		m_wndGridTree.UpdateTreeParam();
	else
		m_wndGridTree.SortGroupByName();
	int Index =-1;

	//���õ�ǰ�޸ĵ���
	Index=m_wndGridTree.FindIndexByParamName(pParam->m_strName.c_str(),pParam->m_strGroup.c_str());
	pCurRow = (CZGridRow*)m_wndGridTree.GetRow(Index);
	if (pCurRow!=NULL)
		m_wndGridTree.m_pRowCur = pCurRow;
	
	m_wndGridTree.SetCurSel(m_wndGridTree.m_pRowCur);
	m_wndGridTree.m_pRowCur->HitTest(NULL);
	
	return 0;
}


int CDlgParamSetBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCxBCGPDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//RegisterHotKey(this->GetSafeHwnd(),IDC_BUTTON_ADD,0,VK_INSERT);    
	//RegisterHotKey(m_hWnd,1002,MOD_CONTROL|MOD_SHIFT,'a');   
	return 0;
}


void CDlgParamSetBase::OnDestroy()
{
	CCxBCGPDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	//::UnregisterHotKey(this->GetSafeHwnd(),IDC_BUTTON_ADD);
	//::UnregisterHotKey(this->GetSafeHwnd(),IDC_BUTTON_DELETE);
}
