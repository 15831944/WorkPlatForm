#pragma once

class CJsData_Svg_UpdateRunFlagData
{
public:
	CJsData_Svg_UpdateRunFlagData(void);
	~CJsData_Svg_UpdateRunFlagData(void);
	void Clear(void);
public:
	BOOL m_bRan;
	CString m_strId;
};

class CJsData_Svg_UpdateRunFlag
{
public:
	CJsData_Svg_UpdateRunFlag(void);
	~CJsData_Svg_UpdateRunFlag(void);
	void Clear(void);
	CString GetJsonStr(void);
public:
	vector< CJsData_Svg_UpdateRunFlagData* > m_vData;
};

//////////////////////////////////////////////////////////////////////////

class CJsData_Svg_UpdateParamStatusData
{
public:
	CJsData_Svg_UpdateParamStatusData(void);
	~CJsData_Svg_UpdateParamStatusData(void);
	void Clear(void);
public:
	DWORD m_dwStatus;
	CString m_strId;
};

class CJsData_Svg_UpdateParamStatus
{
public:
	CJsData_Svg_UpdateParamStatus(void);
	~CJsData_Svg_UpdateParamStatus(void);
	void Clear(void);
	CString GetJsonStr(void);
public:
	vector< CJsData_Svg_UpdateParamStatusData* > m_vData;
};