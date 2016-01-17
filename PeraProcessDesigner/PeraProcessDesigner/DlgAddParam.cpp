// DlgAddGroup.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgAddParam.h"
#include "DlgParamEdit.h"
#include "DlgParamSetBase.h"


// CDlgAddParam �Ի���

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


// CDlgAddParam ��Ϣ�������


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
	if (!m_strGroupName.Compare("<��>"))
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
			if (pParamIn!=NULL)		//�½��ڵ�ʱΪNULL
			{
				nGroup = pParamIn->m_bIsGroup;	//Ҫ���ҵĽڵ�Node����	
				if (!pParam->m_strID.compare(pParamIn->m_strID))
				{//bRet = FALSE;
					continue;	//˵�����Լ��Ƚ�
				}
			}
			//���е�
			nfindGroup = pParam->m_bIsGroup;
			if(!m_strParamName.Compare(pParam->m_strName.c_str())&&(nfindGroup==nGroup))//����ʱ
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
	MAPITER_JSNODE itertmp ;			//���ָʾ��
	CEdit* pEdit = (CEdit*)m_ctlComBox.GetWindow(GW_CHILD);
	if (pEdit)
		pEdit->SetReadOnly();
	m_ctlComBox.AddString("<��>");
	if (m_strGroupName.IsEmpty())
		m_strGroupName="<��>";
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
		AfxMessageBox("����������Ϊ��");
		return 0;
	}
	
	int nindex = m_ctlComBox.GetCurSel();
	if ((nindex==-1)&&(m_strGroupName.IsEmpty()))
	{
		AfxMessageBox("ѡ�в���������!");
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
	if (!strGroupName.Compare("<��>"))
		param.m_strGroup="";
	else
		param.m_strGroup = strGroupName;
	jsonTmp = CDlgParamSetBase::ParamToJsonValue(param);
	
	if(CStringChecker::CheckParamName(m_strParamName) == FALSE)
	{
		MessageBox( "��������" + CStringChecker::GetLastErr(), g_lpszAppTitle, MB_ICONWARNING);
		return 0;
	}
	if(CheckSameName(&param))
	{
			MessageBox( "�ò��������Ѿ����ڣ�", g_lpszAppTitle, MB_ICONWARNING);
			return 0;
	}
	if (itertmp!=m_JvStrMap.end())
	{	
		param.m_strGroup = itertmp->first;
		itertmp->second->vecJvNode.push_back(jsonTmp);
		return 1;
	}
	//int ncount = m_wndGridTree.GetRowCount();
	
	////���������ڴ����µ�����
	//JS_NODE*pJsNode = new JS_NODE;
	//jsonTmp["isgroup"]="true";
	//pJsNode->jsGroup = jsonTmp;
	//pJsNode->strGroupName = m_strParamName;
	//
	m_MapPrams.insert(pair<CString, Json::Value>(m_strParamName, jsonTmp));
//	AfxMessageBox("����������");
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
			return bin_search(key,mid+1,high,k);        /*�����еĺ�벿�ֲ���*/
		else
			return bin_search(key,low,mid-1,k);            /*�����е�ǰ�벿�ֲ���*/
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
