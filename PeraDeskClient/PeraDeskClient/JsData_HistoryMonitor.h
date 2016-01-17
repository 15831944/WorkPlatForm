#pragma once

class CJsData_ShowViewRunFlow
{
public:
	CJsData_ShowViewRunFlow();
	~CJsData_ShowViewRunFlow();
	void Clear();
	BOOL Parse( LPCTSTR lpszData );
public:
	CString m_strUrl;
	int m_nWidth;
	int m_nHeight;
};
