#pragma once
#include "StlHeader.h"

class CCxCmdlineBase
{
public:
	CCxCmdlineBase();
	virtual ~CCxCmdlineBase();
	virtual void Clear( void );
	virtual BOOL Parse( LPCTSTR lpszData = NULL );
	virtual BOOL ParseJson( LPCTSTR lpszData );
	virtual CString GetProtocolName(void);
	CString	GetValue( LPCTSTR lpszFlage );
	int		GetParamSize();

protected:
	
	/*
	功能：
		分割出一个参数和该参数对应的值。
	参数：
		_sFlag：_IN  参数标志，以减号开头的字符串。（如：-i）
		_sValue：_OUT 参数值
		_sSrcParam：_IN_OUT 传入原始参数字符串，传出分割后，后续未分割的字符串
	*/
	BOOL SplitParamFlagValue( CString& _sFlag, CString& _sValue, CString& _sParamStr );
	/*
	功能：
		从参数集中分割出一个参数。以引号或者空格做个分割的标志。
	参数：
		_sValue：_OUT 参数值
		_sParamStr：_IN_OUT 传入原始参数字符串，传出分割之后的参数集
	*/
	BOOL SplitParamValue( CString& _sValue, CString& _sParamStr );

	//向m_vCmdFlags、m_vCmdValues中匹配的插入数据对
	void SetValue( LPCTSTR lpszFlag, LPCTSTR lpszValue );

public:
	CString m_sSrc; //原始命令行
	CString m_sExecDir;
	CString m_sExecFilename;

	/*
	当参数是路径类别时：m_vCmdFlags中存储空标记，m_vCmdValues存储路径；
	当参数是协议类别时：m_vCmdFlags中存储解析后的列项，m_vCmdValues存储各项对应的值；
	当参数是json类别时：m_vCmdFlags中存储解析后json的列项，m_vCmdValues存储各项对应的值；
	当参数是-i或-u类别时：m_vCmdFlags中存储“i”或“u”，m_vCmdValues为空；
	当参数是组合key类别时：m_vCmdFlags中存储各项key，m_vCmdValues存储各项key对应的值；*/	
	vector< CString > m_vCmdFlags;
	vector< CString > m_vCmdValues;
};
