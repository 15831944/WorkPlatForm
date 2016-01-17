// DlgAddGroup.cpp : 实现文件
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgAddParam.h"
#include "DlgParamEdit.h"
#include "DlgParamSetBase.h"


// CDlgAddParam 对话框

IMPLEMENT_DYNAMIC(CDlgAddParam, CCxBCGPDialog)

CDlgAddParam::CDlgAddParam(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgAddParam::IDD, pParent)
	, m_strParamName(_T(""))
	,m_strGroupName(_T(""))
{

}
CDlgAddParam::CDlgAddParam(MAPSTR_JV mapStrJv, VECTOR_NODES vParams,CWnd* pParent)
	: CCxBCGPDialog(CDlgAddParam::IDD, pParent)
	, m_strParamName(_T(""))
	,m_strGroupName(_T(""))
{
	m_JvStrMap = mapStrJv; 
	//((CDlgParamSetBase*)GetParent())->GetGridTree(m_wndGridTree);
	m_nChanged = 0;
	m_vParams= vParams;
}

void CDlgAddParam::SetParasList(MAP_STRJSON paramList)
{
	m_MapPrams = paramList;
}

VECTOR_NODES CDlgAddParam::GetParasVector()
{
	return m_vParams;
}

MAP_STRJSON CDlgAddParam::GetParasList()
{
	return m_MapPrams;
}

CDlgAddParam::~CDlgAddParam()
{
}

void CDlgAddParam::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_strParamName);
	DDX_Control(pDX, IDC_COMBO2, m_ctlComBox);
	DDX_CBString(pDX, IDC_COMBO2, m_strGroupName);
}


BEGIN_MESSAGE_MAP(CDlgAddParam, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgAddParam::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAddParam 消息处理程序


BOOL CDlgAddParam::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();
	InitComBox() ;
	UpdateData(FALSE);
	return TRUE; 
}
void CDlgAddParam::SetGroupName(CString strName)
{
	m_strGroupName = strName;
}
CString CDlgAddParam::GetGroupName()
{
	if (!m_strGroupName.Compare("<无>"))
		m_strGroupName="";
	return m_strGroupName;
}
CString CDlgAddParam::GetAddParaName()
{
	return m_strParamName.Trim();
}

void CDlgAddParam::SetGroupName(BOOL bIsSame)
{
	//m_bIsSameParam = bIsSame;
}

BOOL CDlgAddParam::CheckSameName(ZTreeParam* pParamIn) 
{
	//VECTOR_NODES
	ITER_VECTOR_NODES iterTemp;
	ZTreeParam* pParam = NULL;
	BOOL bRet = FALSE;
	//CString str =m_strParamName;

	BOOL nGroup =FALSE;
	BOOL nfindGroup = 0;
	if (pParamIn==NULL) return 1;

	for (iterTemp=m_vParams.begin();iterTemp!=m_vParams.end();++iterTemp)
	{
		pParam = (*iterTemp);
		if(pParam!=NULL)
		{
			if (pParamIn!=NULL)		//新建节点时为NULL
			{
				nGroup = pParamIn->m_bIsGroup;	//要查找的节点Node属性	
				if (!pParam->m_strID.compare(pParamIn->m_strID))
				{//bRet = FALSE;
					continue;	//说明与自己比较
				}
			}
			//树中的
			nfindGroup = pParam->m_bIsGroup;
			if(!m_strParamName.Compare(pParam->m_strName.c_str())&&(nfindGroup==nGroup))//重名时
			{
				bRet = TRUE;
				break;
			}
		}
	}
	
	return bRet;
}

void CDlgAddParam::InitComBox() 
{
	MAPITER_JSNODE itertmp ;			//组的指示器
	CEdit* pEdit = (CEdit*)m_ctlComBox.GetWindow(GW_CHILD);
	if (pEdit)
		pEdit->SetReadOnly();
	m_ctlComBox.AddString("<无>");
	if (m_strGroupName.IsEmpty())
		m_strGroupName="<无>";
	for (itertmp=m_JvStrMap.begin();itertmp!=m_JvStrMap.end();itertmp++)
	{
		CString strGroupName = itertmp->first;
		m_ctlComBox.AddString(strGroupName);
	}
	
}

int CDlgAddParam::AddParam2Group()
{
	UpdateData(TRUE);
	CString strGroupName;
	ZTreeParam param;	
	Json::Value jsonTmp ;
	//
	m_strParamName=m_strParamName.Trim();
	memset(&param, 0,sizeof(ZTreeParam));
	if (m_strParamName.IsEmpty())
	{
		AfxMessageBox("参数名不能为空");
		return 0;
	}
	
	int nindex = m_ctlComBox.GetCurSel();
	if ((nindex==-1)&&(m_strGroupName.IsEmpty()))
	{
		AfxMessageBox("选中参数所属组!");
		return 0;
	}

	//m_ctlComBox.GetLBText(nindex, strGroupName);
	strGroupName = m_strGroupName;
	MAPITER_JSNODE itertmp = m_JvStrMap.find(strGroupName);
	//
	//param.Init();
	string strid;
	param.m_strName = m_strParamName;
	param.m_strDirection = "In";
	param.m_strType = "String";
	
	strid= ReslibMgr::GuidToString(ReslibMgr::CreateGuid());
	//std::transform(strid.begin(), strid.end(), strid.begin(), std::tolower);
	param.m_strID = strid;
	param.m_bIsGroup = FALSE;
	if (!strGroupName.Compare("<无>"))
		param.m_strGroup="";
	else
		param.m_strGroup = strGroupName;
	jsonTmp = CDlgParamSetBase::ParamToJsonValue(param);
	
	if(CStringChecker::CheckParamName(m_strParamName) == FALSE)
	{
		MessageBox( "参数名称" + CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
		return 0;
	}
	if(CheckSameName(&param))
	{
			MessageBox( "该参数名称已经存在！", g_lpszAppTitle, MB_ICONWARNING);
			return 0;
	}
	if (itertmp!=m_JvStrMap.end())
	{	
		param.m_strGroup = itertmp->first;
		itertmp->second->vecJvNode.push_back(jsonTmp);
		return 1;
	}
	//int ncount = m_wndGridTree.GetRowCount();
	
	////组名不存在创建新的组名
	//JS_NODE*pJsNode = new JS_NODE;
	//jsonTmp["isgroup"]="true";
	//pJsNode->jsGroup = jsonTmp;
	//pJsNode->strGroupName = m_strParamName;
	//
	m_MapPrams.insert(pair<CString, Json::Value>(m_strParamName, jsonTmp));
//	AfxMessageBox("组名不存在");
	return 1;
}
MAPSTR_JV CDlgAddParam::GetGroupList()
{
	return m_JvStrMap;
}

int CDlgAddParam::bin_search(VECTOR_JV key,int low, int high,int k)
{
	return 0;
#if 0


	int mid;
	if(low>high)
		return -1;
	else{
		mid = (low+high) / 2;
		if(key[mid].==k)
			return mid;
		if(k>key[mid])
			return bin_search(key,mid+1,high,k);        /*在序列的后半部分查找*/
		else
			return bin_search(key,low,mid-1,k);            /*在序列的前半部分查找*/
	}
	#endif
}



int CDlgAddParam::IsMapListChanged()
{
	return m_nChanged;
}

void CDlgAddParam::OnBnClickedOk()
{
	
	m_nChanged = AddParam2Group();
	if (m_nChanged)
		return CCxBCGPDialog::OnOK();
	return;
}
