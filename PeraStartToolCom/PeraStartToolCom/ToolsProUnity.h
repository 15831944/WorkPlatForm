#pragma once
#include "ToolObjectDefine.h"
#include <map>
using namespace std;

#define USE_TINY_XML 0 
/************************************************************************/
/* �������ߴ�����                                                                 */
/************************************************************************/
class CToolsProUnity
{
public:
	CToolsProUnity(void);
	~CToolsProUnity(void);
public:
	static BOOL StartTool(ToolObject *ptool);
	static BOOL StartManagerUI();
	static BOOL ConfigTool(ToolObject * ptool,BOOL bCallInStartApp = FALSE);
	static BOOL CheckStartApp(const CString & strToolId);
	static CString GetToolVersion(CString strToolPath);
	static BOOL CompareLocalWithService(const ToolObject & toolService, const ToolObject & toolLocal);
	static CString GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot);
	static CString GetExePathFormKey(const ToolObject * pTool,CString strKeyRoot,BOOL bGetKeyV64);
	static CString GetFilePathFormServInfo(const ToolObject *pTool);
	static BOOL StartApp(LPCTSTR strApp,LPCTSTR strParam);
	static BOOL IsOpenByParam(const ToolObject *pTool);
    static BOOL OpenUrlWithIE(const CString &strUrl);

	/************************************************************************/
	/* ����ID��xml�л�ȡ���ߵļ�¼                                               */
	/************************************************************************/
	static BOOL GetToolFormXml(const CString & strId,ToolObject & toolLocal);
	/************************************************************************/
	/* ���ر����������ü�¼                                                  */
	/************************************************************************/
	static BOOL LoadAllLocalTool(map<CString,ToolObject> & mapTools);
	/************************************************************************/
	/* �����õĹ���д��xml��                                                   */
	/************************************************************************/
	static BOOL WriteToolConfigXml(const ToolObject & Tool); 
	/************************************************************************/
	/* �ӱ�������xml��ɾ��������                                             */
	/************************************************************************/
	static void  DeleteToolConfigFromXml(const CString & strToolId);
	/************************************************************************/
	/* �ӱ�������xml��ɾ�����й��ߵ�������                                     */
	/************************************************************************/
	static void  DeleteAllConfigFromXml();
public:
	static CString g_strConfigXml;
};

