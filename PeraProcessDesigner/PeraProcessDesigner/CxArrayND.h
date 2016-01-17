#pragma once
#include <CxArrayBase.h>

class CCxStructTreeNodeParam;

class CxArrayDim
{
public:

	CxArrayDim( vector<int>& vDimSize, CxArrayDim * pParent, int& nDataType
		, const BOOL bCreateDefaultData = FALSE );
	virtual ~CxArrayDim();
	void Clear(void);
	void ClearElement(void);
	LPARAM GetDataPtr( vector< int >& vPos );
	int GetDataCount( int nDim = -1 );
	void Parse( vector<CString>& vSrc, int& nPos );
	CString GetArrayStr(void);
	BOOL SetDefaultData(void);
	
	BOOL Resize( vector<int>& vDimSize, int nDataType );

public:
	CxArrayDim * m_pParent; //��ά����ǰ������������ά���ݣ�
	int m_nDimIndex; //��ǰ���ݵ�ά�ȱ�ţ���һά���Ϊ��0
	vector < LPARAM > m_vData; //��ά��������
	int m_nRealDataType; //Ԫ�����ͣ�DT_BOOLEAN��DT_INT��DT_DOUBLE��DT_STRING��DT_DIM��
};

class CxArrayND : public CCxArrayBase
{
public:
	CxArrayND( int nDataType );
	CxArrayND(CxArrayND & other);
	virtual ~CxArrayND(void);
	void Clear(void);
	BOOL Parse( LPCTSTR lpszSrc );
	//����ά�ȸ���������ά��
	int GetDimCount(void);
	//����ĳ������ָ��
	LPARAM GetDataPtr( vector<int>& nPos );
	CString GetDataAsCString( vector<int>& nPos );
	BOOL ParseDataVector( vector< CString >& vData, LPCTSTR lpszSrc );
	int GetDataCount( int nDim = -1 );
	CString GetArrayStr(void);
	CString GetDimStr(void);

	BOOL Resize( vector<int>& vDimSize );
	CxArrayND *GetCopied();
public:
	CxArrayDim * m_pData;
	int m_nDimCount; //ά�ȣ���ά���顣
	BOOL m_bIsDynamicArray;
	int m_nDir;
	//CStructTreeNodeData * m_pNode;
	CCxStructTreeNodeParam * m_pNode;
	BOOL m_bResized; //�Ƿ������ά��
};

