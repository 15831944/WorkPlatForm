#pragma once

//////////////////////////////////////////////////////////////////////////

class CCxStructTreeDataLinker
{
public:
	CCxStructTreeDataLinker();
	~CCxStructTreeDataLinker();
	void Clear(void);
	void GetElementPos( vector<int>& vFrom, vector<int>& vTo );
	//BOOL Parse(LPCTSTR lpszData);
public:
	CString m_strFrom;
	CString m_strTo;
	CString m_strOwnerId;
	CString m_strToIndex;
	CString m_strFromIndex;
	CString m_strFormula;
};


