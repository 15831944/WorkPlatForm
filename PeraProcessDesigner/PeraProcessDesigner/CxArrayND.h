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
	CxArrayDim * m_pParent; //父维（当前数据隶属于哪维数据）
	int m_nDimIndex; //当前数据的维度编号，第一维编号为：0
	vector < LPARAM > m_vData; //子维数据数组
	int m_nRealDataType; //元素类型（DT_BOOLEAN、DT_INT、DT_DOUBLE、DT_STRING、DT_DIM）
};

class CxArrayND : public CCxArrayBase
{
public:
	CxArrayND( int nDataType );
	CxArrayND(CxArrayND & other);
	virtual ~CxArrayND(void);
	void Clear(void);
	BOOL Parse( LPCTSTR lpszSrc );
	//返回维度个数（多少维）
	int GetDimCount(void);
	//返回某个数据指针
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
	int m_nDimCount; //维度，几维数组。
	BOOL m_bIsDynamicArray;
	int m_nDir;
	//CStructTreeNodeData * m_pNode;
	CCxStructTreeNodeParam * m_pNode;
	BOOL m_bResized; //是否调整过维度
};

