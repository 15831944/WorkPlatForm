#pragma once
#include "zgridctrl.h"
#include "DlgParamEdit.h"
#include "value.h"
#include "ListenerManager.h"
#include "ParamEventListener.h"

//add by zhaoyw/////////////////////////////////////////////
typedef vector<Json::Value> VECTOR_JV;
typedef vector<Json::Value>::iterator ITER_JSVECTOR;

typedef struct JvNode 
{
	VECTOR_JV   vecJvNode;			 //������Ա
	CString     strGroupName;		//�������
	Json::Value jsGroup;			//���Ӧ������
}JS_NODE,*pJS_NODE;

struct CmpByChinese {  
	bool operator()(const CString& k1, const CString& k2) const
	{  
		std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
		setlocale(LC_ALL, "chs");
		int n = strcoll(k1, k2);
		setlocale(LC_ALL, curLocale.c_str());

		if (n < 0)
			return true;
		else
			return false;
	}  
};  

typedef map<CString, JS_NODE*, CmpByChinese> MAPSTR_JV;
typedef map<CString, JS_NODE*, CmpByChinese>::iterator MAPITER_JSNODE;
////////////////////////////////////////////////////////
typedef map<CString, Json::Value, CmpByChinese>MAP_STRJSON;
typedef map<CString, Json::Value, CmpByChinese>::iterator MAPITER_STRJV;


typedef map<CString, ZTreeParam*, CmpByChinese> MAP_PARAM;
typedef map<CString,ZTreeParam*, CmpByChinese>::iterator MAPITER_PARAM;

typedef	std::vector<ZTreeParam*>VECTOR_NODES;
typedef std::vector<ZTreeParam*>::iterator ITER_VECTOR_NODES;

class ZTreeParamSet :
	public CZGridCtrl,
	public ListenerManager<ParamEventListener>
{
public:
	ZTreeParamSet(void);
	virtual ~ZTreeParamSet(void);

	
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	virtual void OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn);
	virtual void OnSelChanged (const CBCGPGridRange &range, BOOL bSelect);
	//BOOL m_bCustomExpandFlag; //�Զ����չ����ǣ�����ӽڵ�ʱ�������Ƿ�չ����
	//virtual void OnExpand (BOOL /*bExpand*/);

public:
	MAPSTR_JV m_MapStrJv;
	MAP_STRJSON m_ParamMapList;

	//

public:
	BOOL DeleteFromvecParam(ZTreeParam* pParam);
	int FindIndexByParamName(CString strName, CString strGroupName);
	int GetGroupIndex();
	void AddNode2Vec(ZTreeParam Param);
	void AddNewGroup(ZTreeParam *ParamIn);
	void DestroyMvParams();
	void SplitGroup(Json::Value &jv);
	Json::Value AddParamToTree();
	void DestroyMapJvNode();//�ͷ�m_MapStrJv������ڴ�
	BOOL AddSubNodeFromParam(CZGridRow* pRow, Json::Value& jv );
	void SortParamList();
	//	
	void SortGroupByName();
	//ע���ͷ��ڴ�
	BOOL RemoveGroup(MAPSTR_JV& mapStrJvNode, CString strGroupName);
	int RemoveParamFromVec(VECTOR_JV& vectorParams, Json::Value jvDel);

	BOOL DeleteParamFromList(MAPSTR_JV& MapParams, Json::Value jvDel);

	CZGridRow * AddNodeFromParam(Json::Value& jv);
	void AddRoot();	void AddLast();	void SetParam();
	void AddNodeFromJson(Json::Value& jv);
	
		
	BOOL UpdateRowByParam(CZGridRow* pRow, ZTreeParam* pParam);
	void UpdateParamFromEditControl();
	void UpdateTreeParam( );
	void UpdateTreeGroup(MAPSTR_JV MapStrJv);

	void SetRowImage(CZGridRow* pRow, ZTreeParam* pParam, BOOL bRedraw = FALSE);
	void SetParamImage(CZGridRow* pRow, ZTreeParam* pParam, BOOL bRedraw = FALSE);
	void SetSelectedCurRow(ZTreeParam *pParam);
	

	BOOL ModifyParamGroupName(ZTreeParam* pParam, string strGroupName);
	BOOL CheckSameName(CString str);
	ZTreeParam ConvertToParam(CCxStructTreeNodeParam* p); //�ڵ�ת��Ϊ������
	BOOL RemoveParam(ZTreeParam* pParam);
	void DrawTreeLine( CDC* pDC, CZGridRow* pCurRow, CRect rcLeftOffset );
	

	VECTOR_NODES m_vParams; //��������
	ZTreeParam* m_pParamLast;			//���һ�εĲ���
	ZTreeParam* m_pParamCur;			//��ǰ�Ĳ���
	CZGridRow*  m_pRowCur;				//��ǰ��ѡ��

	CBCGPGridRange m_rangeLast;
	//CDlgParamEdit m_dlgParamEdit;


private:
	int m_nCurRow;
	CString m_strCurRowName;
	int m_bparamIsSameName;
public:
	void AddParamToGroup(ZTreeParam* pParam);
	int GetGroupIndex(CString str);
	int GetCurRowIndex();
	BOOL IsGroupExpanded(CString str);

	void SetCurRowIndex(int index);
	void SetCurRowName(CString strCurRowName);
	CString GetCurRowName();
public:
	//�����ؼ��б��еĲ�����������

	static int CtlListSort(Json::Value& jvParams );
	static void wordSwap(Json::Value& jvParams_min, Json::Value&jvParams_i);
	static bool localeLessThan (ZTreeParam* j1, ZTreeParam* j2) ;
	static int wordCompare( Json::Value& wordOne, Json::Value& wordTwo );

public:
	int VectorListSort(VECTOR_JV& jvParamVector);

	int ReNameGroup(ZTreeParam *pParamIn, CString strNewName);
	int ReNameParam(VECTOR_JV& vectorParams, string strNewGroup);

	int MouseClick();
	static int FindParam(VECTOR_JV& vectorParams, CString strName);
	static int CheckSameGroup(MAPSTR_JV& MapStrJv, CString strGroup);
	BOOL CheckSameName(CString str, ZTreeParam* pParamIn);
	int GetRowIDByParam(ZTreeParam*pParam);	

	CZGridRow* GetVisibleParentRow(CZGridRow* pRow);
};

