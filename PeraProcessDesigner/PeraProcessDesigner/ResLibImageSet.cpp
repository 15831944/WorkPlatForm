#include "StdAfx.h"
#include "ResLibImageSet.h"
#include <PtrVectorHeader.h>
#include "ConvertData.h"

//Robot Type Guid
static const LPCTSTR arrRobotTypeId[] = {
	_T("58DBA6CB-0FB1-4495-8266-1FBE6D6AC518"), //script driver
	_T("69F08DE1-A7FC-470D-A2A2-A19626A22041"), //data parser
	_T("EA68A825-6EEA-4786-8BC2-85282F0B4B1F"), //excel
	_T("E5FE966F-BD57-41D9-9C19-97F4337B5D51"), //tool integration
	_T("WORD2B29-2FB6-4A5A-9EDD-8F42A1578EA2"), //word
	_T("30B22B29-2FB6-4A5A-9EDD-8F42A1578EA2"), //formula
	_T("E712BA3D-34AD-4286-AAC5-D800BAF60415"), //form
	_T("F342FEC9-B226-4529-A91C-AWORD33C1FE"), //document
	_T("32BBAFFD-FC24-41A6-B797-4570C7223461"), //Patran组件
	_T("43252AF9-3370-4349-950C-E1BEF7D7CDB2"), //Workbench组件
	_T("74FF1374-5E8E-486A-A023-2C4D28390218"), //catia组件
#ifdef PROJECT_203
	_T("71FE51D4-6580-4E0B-AF5A-58ED9A1F6588"), //脚本组件
	_T("6A47DE44-2A0E-402D-9F3A-0E4806C31498") //203UG组件
#else
	_T("71FE51D4-6580-4E0B-AF5A-58ED9A1F6588") //脚本组件
#endif
	
};

//Robot 42 * 42
static const LPCTSTR arrRobotFileName_42[] = {
	_T("ScriptDriver_42.png"),
	_T("DataParser_42.png"),
	_T("Excel_42.png"),
	_T("ToolIntegration_42.png"),
	_T("Word_42.png"),
	_T("Formula_42.png"),
	_T("Form_42.png"),
	_T("Document_42.png"),
	_T("Patran_42.png"),
	_T("Workbench_42.png"),
	_T("Catia_42.png"),
#ifdef PROJECT_203
	_T("ScriptCom_42.png"),
	_T("UGCom_42.png")
#else
	_T("ScriptCom_42.png")
#endif
};

//Robot 16 * 16
static const LPCTSTR arrRobotFileName_16[] = {
	_T("ScriptDriver_16.png"),
	_T("DataParser_16.png"),
	_T("Excel_16.png"),
	_T("ToolIntegration_16.png"),
	_T("Word_16.png"),
	_T("Formula_16.png"),
	_T("Form_16.png"),
	_T("Document_16.png"),
	_T("Patran_16.png"),
	_T("Workbench_16.png"),
	_T("Catia_16.png"),
#ifdef PROJECT_203
	_T("ScriptCom_16.png"),
	_T("UGCom_16.png")
#else
	_T("ScriptCom_16.png")
#endif
};

//static res id
static const LPCTSTR arrStaticResId[] = {
	_T("StaticResId_ResLib_CustomRobot_42"), //自定义robot，默认robot
	_T("StaticResId_ResLib_CustomRobot_16"), //自定义robot，默认robot
	_T("StaticResId_ResLib_Flow_42"), //设计过程
	_T("StaticResId_ResLib_Flow_16"), //设计过程
	_T("StaticResId_ResLib_PC_16"), //
	_T("StaticResId_ResLib_FOLDER_16"), //
	_T("StaticResId_ResLib_COMPONENT_16"), //
	//拖拽
	_T("StaticResId_Drag_DstForm"), //
	_T("StaticResId_Drag_Dst"), //
	_T("StaticResId_Drag_HighlightForm"), //
	_T("StaticResId_Drag_Highlight") //

};

static const LPCTSTR arrStaticRes[] = {
	_T("CustomRobot_42.png"),
	_T("CustomRobot_16.png"),
	_T("Flow_42.png"),
	_T("Flow_16.png"),
	_T("ResLibPC_16.png"),
	_T("ResLibFolder_16.png"),
	_T("ResLibComponent_16.png"),
	//拖拽
	_T("DragDstForm.emf"),
	_T("DragDst.png"),
	_T("DragHighlightForm.emf"),
	_T("DragHighlight.png")
};

//FlowIf res id
static const LPCTSTR arrFlowIfResId[] = {
	_T("FlowIfResId_ResLib_Arrow"),
	_T("FlowIfResId_ResLib_ArrowUp"),
	_T("FlowIfResId_ResLib_ArrowUpGray"),
	_T("FlowIfResId_ResLib_ArrowDown"),
	_T("FlowIfResId_ResLib_ArrowDownGray")
};

static const LPCTSTR arrFlowIfRes[] = {
	_T("Arrow.png"),
	_T("ArrowUp.png"),
	_T("ArrowUpGray.png"),
	_T("ArrowDown.png"),
	_T("ArrowDownGray.png")
};

//登陆对话框资源
static const LPCTSTR arrDlgLoginResId[] = {
	_T("DlgLoginBk_Bk"),
	_T("DlgLoginBk_Close_Nor"),
	_T("DlgLoginBk_Close_Over"),
	_T("DlgLoginMain_BK")
};

static const LPCTSTR arrDlgLoginRes[] = {
	_T("DlgLoginBk.png"),
	_T("LoginCloseNor.png"),
	_T("LoginCloseOver.png"),
	_T("DlgLoginMain.png")
};

//进度对话框资源
static const LPCTSTR arrDlgProgressResId[] = {
	_T("DlgProgress")
};

static const LPCTSTR arrDlgProgressRes[] = {
	_T("DlgProgress.png")
};

//参数设置对话框资源
static const LPCTSTR arrDlgParamSetResId[] = {
	_T("DlgParamSet_Btn_Delete"),
	_T("DlgParamSet_Btn_DeleteGray"),
	_T("DlgParamSet_Btn_Add"),
	_T("DlgParamSet_Btn_AddGray"),
	_T("DlgParamSet_Btn_Hover"),
	_T("DlgParamSet_Btn_Down"),
	_T("DlgParamSet_Btn_AddGroup"),
	_T("DlgParamSet_Btn_MoveTo")
};

static const LPCTSTR arrDlgParamSetRes[] = {
	_T("BtnDelete.png"),
	_T("BtnDeleteGray.png"),
	_T("BtnAdd.png"),
	_T("BtnAddGray.png"),
	_T("BtnHover.png"),
	_T("BtnDown.png"),
	_T("AddGroup.png"),
	_T("MoveTo.png")
};

CCxStaticRes::CCxStaticRes()
{
	m_nCode = RT_NULL;
	m_strId = "";
	m_strPath = "";
	m_pImgNormal = NULL;
}

CCxStaticRes::~CCxStaticRes()
{
	Clear();
}

void CCxStaticRes::Clear( void )
{
	m_nCode = RT_NULL;
	m_strId = "";
	m_strPath = "";
	if ( m_pImgNormal != NULL )
	{
		delete m_pImgNormal; m_pImgNormal = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef map< CString, CCxStaticRes * > STATICRES_IMAGESET_BYID;
typedef map< int, CCxStaticRes * > STATICRES_IMAGESET_BYCODE;

static STATICRES_IMAGESET_BYID g_ImageSetById_RobotLarge;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_RobotLarge;

static STATICRES_IMAGESET_BYID g_ImageSetById_RobotSmall;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_RobotSmall;

static STATICRES_IMAGESET_BYID g_ImageSetById_StaticRes;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_StaticRes;

static STATICRES_IMAGESET_BYID g_ImageSetById_FlowIfRes;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_FlowIfRes;

//登陆对话框资源
static STATICRES_IMAGESET_BYID g_ImageSetById_DlgLoginRes;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_DlgLoginRes;

//进度对话框资源
static STATICRES_IMAGESET_BYID g_ImageSetById_DlgProgressRes;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_DlgProgressRes;

//参数设置对话框资源
static STATICRES_IMAGESET_BYID g_ImageSetById_DlgParamSetRes;
static STATICRES_IMAGESET_BYCODE g_ImageSetByCode_DlgParamSetRes;

//////////////////////////////////////////////////////////////////////////

void ClearResLibImageSet(void)
{
	PtrMapClear( g_ImageSetById_RobotLarge );
	g_ImageSetByCode_RobotLarge.clear();
	//
	PtrMapClear( g_ImageSetById_RobotSmall );
	g_ImageSetByCode_RobotSmall.clear();
	//
	PtrMapClear( g_ImageSetById_StaticRes );
	g_ImageSetByCode_StaticRes.clear();

	//
	PtrMapClear( g_ImageSetById_FlowIfRes );
	g_ImageSetByCode_FlowIfRes.clear();
	//
	PtrMapClear( g_ImageSetById_DlgLoginRes );
	g_ImageSetByCode_DlgLoginRes.clear();
	//
	PtrMapClear( g_ImageSetById_DlgProgressRes );
	g_ImageSetByCode_DlgProgressRes.clear();
	//
	PtrMapClear( g_ImageSetById_DlgParamSetRes );
	g_ImageSetByCode_DlgParamSetRes.clear();
}

void LoadResLibImageSet(void)
{
	ClearResLibImageSet();

	CStringW strW;
	CString strDir;
	CCxStaticRes * pImageData = NULL;
	int nCount = 0;
	//robot
	strDir = GetExecDir() + "\\Res\\RobotImage\\";
	nCount = sizeof( arrRobotTypeId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_NULL + 1 + i;
		pImageData->m_strId = arrRobotTypeId[i];
		pImageData->m_strPath = strDir + arrRobotFileName_42[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_RobotLarge.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_RobotLarge.insert( 
			make_pair(pImageData->m_nCode, pImageData) );

		//
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_NULL + 1 + i;
		pImageData->m_strId = arrRobotTypeId[i];
		pImageData->m_strPath = strDir + arrRobotFileName_16[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_RobotSmall.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_RobotSmall.insert( 
			make_pair(pImageData->m_nCode, pImageData) );
	}
	//
	strDir = GetExecDir() + "\\Res\\StaticRes\\";
	nCount = sizeof( arrStaticResId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_CUSTOM_ROBOT_42 + i;
		pImageData->m_strId = arrStaticResId[i];
		pImageData->m_strPath = strDir + arrStaticRes[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_StaticRes.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_StaticRes.insert( 
			make_pair(pImageData->m_nCode, pImageData) );
	}
	// If对话框图标
	strDir = GetExecDir() + "\\Res\\FlowIfImg\\";
	nCount = sizeof( arrFlowIfResId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_IF_ARROW + i;
		pImageData->m_strId = arrFlowIfResId[i];
		pImageData->m_strPath = strDir + arrFlowIfRes[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_FlowIfRes.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_FlowIfRes.insert( 
			make_pair(pImageData->m_nCode, pImageData) );
	}
	//登陆对话框资源
	strDir = GetExecDir() + "\\Res\\DlgLogin\\";
	nCount = sizeof( arrDlgLoginResId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_DLG_LOGIN_BK_BK + i;
		pImageData->m_strId = arrDlgLoginResId[i];
		pImageData->m_strPath = strDir + arrDlgLoginRes[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_DlgLoginRes.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_DlgLoginRes.insert( 
			make_pair(pImageData->m_nCode, pImageData) );
	}
	//进度对话框资源
	strDir = GetExecDir() + "\\Res\\DlgProgress\\";
	nCount = sizeof( arrDlgProgressResId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_DLG_PROGRESS + i;
		pImageData->m_strId = arrDlgProgressResId[i];
		pImageData->m_strPath = strDir + arrDlgProgressRes[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_DlgProgressRes.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_DlgProgressRes.insert( 
			make_pair(pImageData->m_nCode, pImageData) );
	}

	//参数设置对话框资源
	strDir = GetExecDir() + "\\Res\\DlgParamSet\\";
	nCount = sizeof( arrDlgParamSetResId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		pImageData = new CCxStaticRes();
		pImageData->m_nCode = CCxStaticRes::RT_DLG_PARAMSET + i;
		pImageData->m_strId = arrDlgParamSetResId[i];
		pImageData->m_strPath = strDir + arrDlgParamSetRes[i];

		strW = pImageData->m_strPath;
		pImageData->m_pImgNormal = Image::FromFile( (WCHAR*)(LPCWSTR)strW );

		g_ImageSetById_DlgParamSetRes.insert( 
			make_pair(pImageData->m_strId, pImageData) );
		g_ImageSetByCode_DlgParamSetRes.insert( 
			make_pair(pImageData->m_nCode, pImageData) );
	}
}

int ConvertRobotId2Code(LPCTSTR lpszId)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	int nCount = sizeof( arrRobotTypeId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		if ( strId.CompareNoCase( arrRobotTypeId[i] ) == 0 )
		{
			return CCxStaticRes::RT_NULL + 1 + i;
		}
	}
	return CCxStaticRes::RT_NULL;
}

CString ConvertRobotCode2Id(int nCode)
{
	int nCount = sizeof( arrRobotTypeId ) / sizeof( LPCTSTR );
	for (int i=0; i<nCount; i++)
	{
		if ( nCode == CCxStaticRes::RT_NULL + 1 + i )
		{
			return arrRobotTypeId[i];
		}
	}
	return "";
}

CCxStaticRes * GetRobotImageData(LPCTSTR lpszId, int nSize)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if ( strId.IsEmpty() ) return NULL;

	STATICRES_IMAGESET_BYID::iterator itr;
	
	switch ( nSize )
	{
	case 16:
		itr = g_ImageSetById_RobotSmall.find( strId );
		if ( itr != g_ImageSetById_RobotSmall.end() )
			return itr->second;
		else return NULL;
	case 42:
		itr = g_ImageSetById_RobotLarge.find( strId );
		if ( itr != g_ImageSetById_RobotLarge.end() )
			return itr->second;
		else return NULL;
	default:
		return NULL;
	}
}


CCxStaticRes * GetRobotImageData(int nCode, int nSize)
{
	CString strId = ConvertRobotCode2Id(nCode);
	return GetRobotImageData(strId, nSize);
}


CCxStaticRes * GetStaticResImageData(LPCTSTR lpszId)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if ( strId.IsEmpty() ) return NULL;

	STATICRES_IMAGESET_BYID::iterator itr = g_ImageSetById_StaticRes.find(strId);
	if ( itr != g_ImageSetById_StaticRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetStaticResImageData(int nCode)
{
	STATICRES_IMAGESET_BYCODE::iterator itr = g_ImageSetByCode_StaticRes.find(nCode);
	if ( itr != g_ImageSetByCode_StaticRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetFlowIfResImageData(LPCTSTR lpszId)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if ( strId.IsEmpty() ) return NULL;

	STATICRES_IMAGESET_BYID::iterator itr = g_ImageSetById_FlowIfRes.find(strId);
	if ( itr != g_ImageSetById_FlowIfRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetFlowIfResImageData(int nCode)
{
	STATICRES_IMAGESET_BYCODE::iterator itr = g_ImageSetByCode_FlowIfRes.find(nCode);
	if ( itr != g_ImageSetByCode_FlowIfRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetDlgLoginResImageData(LPCTSTR lpszId)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if ( strId.IsEmpty() ) return NULL;

	STATICRES_IMAGESET_BYID::iterator itr = g_ImageSetById_DlgLoginRes.find(strId);
	if ( itr != g_ImageSetById_DlgLoginRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetDlgLoginResImageData(int nCode)
{
	STATICRES_IMAGESET_BYCODE::iterator itr = g_ImageSetByCode_DlgLoginRes.find(nCode);
	if ( itr != g_ImageSetByCode_DlgLoginRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetDlgProgressResImageData(LPCTSTR lpszId)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if ( strId.IsEmpty() ) return NULL;

	STATICRES_IMAGESET_BYID::iterator itr = g_ImageSetById_DlgProgressRes.find(strId);
	if ( itr != g_ImageSetById_DlgProgressRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetDlgProgressResImageData(int nCode)
{
	STATICRES_IMAGESET_BYCODE::iterator itr = g_ImageSetByCode_DlgProgressRes.find(nCode);
	if ( itr != g_ImageSetByCode_DlgProgressRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetDlgParamSetResImageData(LPCTSTR lpszId)
{
	CString strId = lpszId == NULL ? "" : lpszId;
	if ( strId.IsEmpty() ) return NULL;

	STATICRES_IMAGESET_BYID::iterator itr = g_ImageSetById_DlgParamSetRes.find(strId);
	if ( itr != g_ImageSetById_DlgParamSetRes.end() )
		return itr->second;
	else
		return NULL;
}

CCxStaticRes * GetDlgParamSetResImageData(int nCode)
{
	STATICRES_IMAGESET_BYCODE::iterator itr = g_ImageSetByCode_DlgParamSetRes.find(nCode);
	if ( itr != g_ImageSetByCode_DlgParamSetRes.end() )
		return itr->second;
	else
		return NULL;
}