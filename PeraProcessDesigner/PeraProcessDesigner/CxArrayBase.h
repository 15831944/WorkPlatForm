#pragma once
#include "StlHeader.h"
#include "CxDataType.h"

class CCxArrayBase
{
public:
	static const int _MaxDimSize = 0xFFFF;
	static const int _MaxDim = 3;

	CCxArrayBase(int nDataType);
	virtual ~CCxArrayBase(void);
	//取得数据类型
	int GetDataType(void) const { return m_DataType; }
	//取得维数
	int GetDimCount(void);
	//生成单个数据字符串
	BOOL ConvertDataToString( CString & strData, _variant_t & varSrc );
	//根据数据类型，将字符串转化为数据
	BOOL ConvertDataToVariant( _variant_t& varData, CString & strSrc );
	int GetRealDataType(void) const ;
	//
	virtual BOOL CheckData( LPCTSTR lpszData );

protected:
	//提取维度字符串
	BOOL ExtractDimStr(CString& strDim, CString& strSrc);
	//提取数据字符串
	BOOL ExtractDataStr(CString& strData, CString& strSrc);
protected:
	int m_DataType;
};
