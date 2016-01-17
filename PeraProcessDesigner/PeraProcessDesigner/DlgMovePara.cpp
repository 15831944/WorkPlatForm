// DlgMovePara.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PeraProcessDesigner.h"
#include "DlgMovePara.h"
#include "afxdialogex.h"


// CDlgMovePara �Ի���

IMPLEMENT_DYNAMIC(CDlgMovePara, CCxBCGPDialog)

CDlgMovePara::CDlgMovePara(CWnd* pParent /*=NULL*/)
	: CCxBCGPDialog(CDlgMovePara::IDD, pParent)
	, m_strCurCombox(_T(""))
{

}
CDlgMovePara::CDlgMovePara(MAPSTR_JV JvStrMap, CWnd* pParent )
	: CCxBCGPDialog(CDlgMovePara::IDD, pParent)
{
	m_JvStrMap = JvStrMap;
	m_nChanged = 0;
}


CDlgMovePara::~CDlgMovePara()
{
}

void CDlgMovePara::DoDataExchange(CDataExchange* pDX)
{
	CCxBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctrlComboBox);
	DDX_CBString(pDX, IDC_COMBO1, m_strCurCombox);
}


BEGIN_MESSAGE_MAP(CDlgMovePara, CCxBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgMovePara::OnBnClickedOk)
END_MESSAGE_MAP()

CString CDlgMovePara::GetMoveParaName()
{
	return m_strParamName;
}

CString CDlgMovePara::GetMoveGroupName()
{
	return m_strCurCombox;
}

void CDlgMovePara::InitComBox() 
{
	//Json::FastWriter fwrite;
	//string strText = fwrite.write(m_jvDel);
	MAPITER_JSNODE itertmp ;			//���ָʾ��
	CEdit* pEdit = (CEdit*)m_ctrlComboBox.GetWindow(GW_CHILD);

	if (pEdit)
		pEdit->SetReadOnly();

	m_ctrlComboBox.AddString("<��>");
	for (itertmp=m_JvStrMap.begin();itertmp!=m_JvStrMap.end();itertmp++)
	{
		CString strGroupName = itertmp->first;
		m_ctrlComboBox.AddString(strGroupName);
	}

	if (m_jvDel.type()!=Json::objectValue)
	{
		m_strCurCombox = "<��>";
		UpdateData(FALSE);
		return ;
	}

	m_strCurCombox = m_jvDel["group"].isString()?m_jvDel["group"].asCString():"";
	if (!m_strCurCombox.Compare(""))	m_strCurCombox = "<��>";
	
	//m_strCurCombox="<��>";
	UpdateData(FALSE);
}

// CDlgMovePara ��Ϣ�������
MAPSTR_JV CDlgMovePara::GetParaList()
{
	return m_JvStrMap;
}

void CDlgMovePara::SetDeletNode(Json::Value mvJson)
{
	m_jvDel = mvJson;
}

 void CDlgMovePara::SetParasList(MAP_STRJSON paramList)
 {
	 m_MapPrams = paramList;
 }

 MAP_STRJSON CDlgMovePara::GetParasList()
 {
	 return m_MapPrams;
 }

 int CDlgMovePara::IsMapListChanged()
 {
	 return m_nChanged;
 }

int CDlgMovePara::MoveParaNode(Json::Value mvJson, CString strNewGroup)
{
	//
	if (mvJson.type()!=Json::objectValue)
		return 0;

	int  nIndex =-1;
	MAPITER_JSNODE iterOld;
	MAPITER_JSNODE iterNew;
	VECTOR_JV iterFind ;
	MAPITER_STRJV iterMapPara;
	string strjvNewgroup(strNewGroup);
	//
	CString strOldGroup ;
	CString strParamName ;

	strParamName = mvJson["name"].asCString() ? mvJson["name"].asCString() : "";
	m_strParamName = strParamName;
	//json�������ڵ���
	strOldGroup =  mvJson["group"].asCString() ? mvJson["group"].asCString():"";

	//�ƶ����µ�������json����ԭ�����ڵ�����ʱ����
	if (!strNewGroup.Compare(strOldGroup))
			return 0;
	//�ƶ����µ����޸�json�����������
	if (!strjvNewgroup.compare("<��>"))
		strjvNewgroup = "";
	mvJson["group"]=strjvNewgroup;

	// ��ԭ�������е�json����ɾ��
	iterOld = m_JvStrMap.find(strOldGroup);
	if (iterOld!=m_JvStrMap.end())//����json�������ڵ��飬
	{
		iterFind = iterOld->second->vecJvNode;
		if (iterFind.size()>0)
		{
			//VECTOR_JV::iterator it = find(iterOld->second->vecJvNode.begin() , iterOld->second->vecJvNode.end(), mvJson) ;
			//iterOld->second->vecJvNode.erase(it);
			//�ڶ�Ӧ������ɾ��json����
			RemoveParam(iterOld->second->vecJvNode, mvJson);
		}
	}

	//��������<��>��ʱ
	if (strOldGroup.IsEmpty()|| (!strOldGroup.Compare("<��>")))
	{
		iterMapPara = m_MapPrams.find(strParamName);

		if (iterMapPara!=m_MapPrams.end())
		{
			m_MapPrams.erase(iterMapPara);
		}
	}	

	//��ӵ�������
	iterNew = m_JvStrMap.find(strNewGroup);
	if (iterNew!=m_JvStrMap.end())
	{
		iterNew->second->vecJvNode.push_back(mvJson);
	}else
	{
		m_MapPrams.insert(pair<CString, Json::Value>(strParamName, mvJson));
	}
	

	return 1;
}

BOOL CDlgMovePara::OnInitDialog()
{
	CCxBCGPDialog::OnInitDialog();

	InitComBox();

	return TRUE; 
}


void CDlgMovePara::OnBnClickedOk()
{
	UpdateData(TRUE);
	int  nIndex =-1;
	CString strNewGroup;
	Json::FastWriter fwriter;
	string strText = fwriter.write(m_jvDel);
	//��ȡ�ƶ������������
	nIndex = m_ctrlComboBox.GetCurSel();
	if( (nIndex!=-1))
	{
		m_ctrlComboBox.GetLBText(nIndex, strNewGroup);
		if (strNewGroup.IsEmpty())
		{
			AfxMessageBox("ѡ��������");
			return;
		}
		m_nChanged = MoveParaNode(m_jvDel, strNewGroup);
	}

	return CCxBCGPDialog::OnOK();
}


void CDlgMovePara::RemoveParam(VECTOR_JV& vectorParams, Json::Value jvDel)
{
	ITER_JSVECTOR iterVector;
	ITER_JSVECTOR iterDel;
	CString strParadel = jvDel["name"].isString()?jvDel["name"].asCString():"";
	CString strTemp;
	for (iterVector=vectorParams.begin();iterVector!=vectorParams.end();iterVector++)
	{
		strTemp = (*iterVector)["name"].isString()?(*iterVector)["name"].asCString():"";
		iterDel = iterVector;
		if (!strTemp.CompareNoCase(strParadel))
		{
			if (iterDel!=vectorParams.end())
			{
				vectorParams.erase(iterDel);
				//vectorParams.clear(*iterVector);
				return;
			}
		}
	}
}
