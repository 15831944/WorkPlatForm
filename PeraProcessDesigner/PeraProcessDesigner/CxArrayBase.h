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
	//ȡ����������
	int GetDataType(void) const { return m_DataType; }
	//ȡ��ά��
	int GetDimCount(void);
	//���ɵ��������ַ���
	BOOL ConvertDataToString( CString & strData, _variant_t & varSrc );
	//�����������ͣ����ַ���ת��Ϊ����
	BOOL ConvertDataToVariant( _variant_t& varData, CString & strSrc );
	int GetRealDataType(void) const ;
	//
	virtual BOOL CheckData( LPCTSTR lpszData );

protected:
	//��ȡά���ַ���
	BOOL ExtractDimStr(CString& strDim, CString& strSrc);
	//��ȡ�����ַ���
	BOOL ExtractDataStr(CString& strData, CString& strSrc);
protected:
	int m_DataType;
};
