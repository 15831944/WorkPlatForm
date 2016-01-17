#pragma once
typedef struct
{
	void Clear()
	{
		m_strID.Empty();
		m_strDesrciption.Empty();
		m_strName.Empty();
		m_strAbsPath.Empty();
		m_strRunParam.Empty();
		m_strDefaultKey.Empty();
		m_strReverseKey.Empty();
		m_strKeyName.Empty(); 
		m_strKeyValueName.Empty();
		m_strRunPath.Empty();
		m_strRunName.Empty();

		m_strLocalPath.Empty();
		m_strLocalRunParam.Empty();
		m_strPathConfigType.Empty();
		m_strURL.Empty();
	}
	// 服务器端传给的配置信息（只读）
	CString m_strID;          // 软件ID
	CString m_strDesrciption; // 软件描述
	CString m_strName;        // 软件名称
	CString m_strAbsPath;     // 绝对路径
	CString m_strRunParam;     // 启动参数
	CString m_strDefaultKey;   // 默认主键
	CString m_strReverseKey;   // 保留主键
	CString m_strKeyName;      // 注册表项
	CString m_strKeyValueName;     // 注册表键值
	CString m_strRunPath;      // 启动路径
	CString m_strRunName;      // 启动名称

	CString m_strPathConfigType;      // 路径配置类型
	CString m_strURL;                //  URL地址(当且仅当m_strPathConfigType为2时该参数会被使用)

	// 以下是本地配置信息（可修改）
	CString m_strLocalPath; // 本地配置路径
	CString m_strLocalRunParam; // 本地配置启动参数
}ToolObject;