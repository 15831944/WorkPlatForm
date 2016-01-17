#pragma once
class CGlobalConfig
{
public:
	CGlobalConfig(void);
	~CGlobalConfig(void);

	void Load(void);

	CString m_strSvgUrl;
};

extern CGlobalConfig theGlobalConfig;