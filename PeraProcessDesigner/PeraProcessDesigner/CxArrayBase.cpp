#include "StdAfx.h"
#include "CxArrayBase.h"
#include "CxDataType.h"
#include "ConvertData.h"
#include "PeraProcessDesignerUtility.h"
#include "PeraProcessDesigner.h"

/*
测试和使用说明：

//1维
CString str1d = "[1,2,3,4,5,6](6)";
//2维
CString str2d = "[1,2,3,4,5,6,7,8,9,10,11,12](3,4)";

CArray1D a1d(DT_ARRAY1D_INT);
a1d.Parse(str1d);

CArray2D a2d(DT_ARRAY2D_INT);
a2d.Parse(str2d);

for (int i=0; i<6; i++)
{
	CString str = a1d.GetDataStr(i);
	continue;
}

for (int nRow=0; nRow<3; nRow++)
{
    for (int nCol=0; nCol<4; nCol++)
    {
        CString str = a2d.GetDataStr(nRow, nCol);
        continue;
    }
}

str1d = a1d.GetArrayStr();
str2d = a2d.GetArrayStr();

*/


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CCxArrayBase::CCxArrayBase(int nDataType)
{
	m_DataType = nDataType;
}


CCxArrayBase::~CCxArrayBase(void)
{
	m_DataType = DT_NULL;
}


int CCxArrayBase::GetDimCount(void)
{
	switch (m_DataType)
	{
	case DT_ARRAY4D_BOOLEAN:
	case DT_ARRAY4D_INT:
	case DT_ARRAY4D_DOUBLE:
	case DT_ARRAY4D_STRING:
		return 4;
	case DT_ARRAY3D_BOOLEAN:
	case DT_ARRAY3D_INT:
	case DT_ARRAY3D_DOUBLE:
	case DT_ARRAY3D_STRING:
		return 3;
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY2D_INT:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY2D_STRING:
		return 2;
	case DT_ARRAY1D_BOOLEAN:
	case DT_ARRAY1D_INT:
	case DT_ARRAY1D_DOUBLE:
	case DT_ARRAY1D_STRING:
		return 1;
	default:
		return 0;
	}
}


BOOL CCxArrayBase::ExtractDimStr( CString& strDim, CString& strSrc )
{
	strDim = "";
	int nLeft, nRight;
	nRight = strSrc.ReverseFind(')');
	nLeft = strSrc.ReverseFind('(');
	if (nRight <= -1 || nLeft <= -1 || nRight <= nLeft + 1) return FALSE;
	if (nRight + 1 != strSrc.GetLength()) return FALSE; //最后一个字符
	//取得数据个数
	strDim = strSrc.Mid(nLeft + 1, nRight - nLeft - 1);
	if ( strDim.IsEmpty() ) return FALSE;
	return TRUE;
}


BOOL CCxArrayBase::ExtractDataStr( CString& strData, CString& strSrc )
{
	strData = "";
	int nLeft, nRight;
	nRight = strSrc.ReverseFind(']');
	nLeft = strSrc.Find('[');
	if (nRight <= -1 || nLeft <= -1 || nRight <= nLeft + 1) return FALSE;
	if ( nLeft != 0 ) return FALSE; //第一个字符
	//取得数据
	strData = strSrc.Mid(nLeft + 1, nRight - nLeft - 1);
	if ( strData.IsEmpty() ) return FALSE;
	return TRUE;
}


BOOL CCxArrayBase::ConvertDataToString( CString& strData, _variant_t & varSrc )
{
	strData = "";
	switch (m_DataType)
	{
	case DT_ARRAY4D_BOOLEAN:
	case DT_ARRAY3D_BOOLEAN:
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY1D_BOOLEAN:
	case DT_BOOLEAN:
		if ( varSrc.boolVal == VARIANT_TRUE ) strData = "true";
		else strData = "false";
		return TRUE;
	
	case DT_ARRAY4D_INT:
	case DT_ARRAY3D_INT:
	case DT_ARRAY2D_INT:
	case DT_ARRAY1D_INT:
	case DT_INT:
		strData.Format("%d", varSrc.intVal);
		return TRUE;

	case DT_ARRAY4D_DOUBLE:
	case DT_ARRAY3D_DOUBLE:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY1D_DOUBLE:
	case DT_DOUBLE:
		strData.Format("%0.15f", varSrc.dblVal);
		return TRUE;

	case DT_ARRAY4D_STRING:
	case DT_ARRAY3D_STRING:
	case DT_ARRAY2D_STRING:
	case DT_ARRAY1D_STRING:
	case DT_STRING:
		return ConvertBSTR2CString(strData, varSrc.bstrVal);

	default:
		return FALSE;
	}
}


BOOL CCxArrayBase::ConvertDataToVariant( _variant_t& varData, CString & strSrc )
{
	varData.Clear();
	switch (m_DataType)
	{
	case DT_ARRAY4D_BOOLEAN:
	case DT_ARRAY3D_BOOLEAN:
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY1D_BOOLEAN:
	case DT_BOOLEAN:
		if ( strSrc.CompareNoCase("true") == 0 )
		{
			varData.boolVal = VARIANT_TRUE;
		}
		else if ( strSrc.CompareNoCase("false") == 0 )
		{
			varData.boolVal = VARIANT_FALSE;
		}
		else
		{
			::MessageBox(GetMainWnd(), "解析boolean型数组失败，数据异常", g_lpszAppTitle, MB_OK|MB_ICONERROR);
			return FALSE;
		}
		return TRUE;

	case DT_ARRAY4D_INT:
	case DT_ARRAY3D_INT:
	case DT_ARRAY2D_INT:
	case DT_ARRAY1D_INT:
	case DT_INT:
		varData.intVal = (int)strtol( (char *)(LPCTSTR)strSrc, NULL, 10 );
		return TRUE;

	case DT_ARRAY4D_DOUBLE:
	case DT_ARRAY3D_DOUBLE:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY1D_DOUBLE:
	case DT_DOUBLE:
		varData.dblVal = strtod( (char *)(LPCTSTR)strSrc, NULL );
		return TRUE;

	case DT_ARRAY4D_STRING:
	case DT_ARRAY3D_STRING:
	case DT_ARRAY2D_STRING:
	case DT_ARRAY1D_STRING:
	case DT_STRING:
		{
			CComBSTR bstr;
			bstr = _bstr_t(strSrc).GetBSTR();
			varData.bstrVal = bstr.m_str;
		}
		return TRUE;

	default:
		return FALSE;
	}
}


BOOL CCxArrayBase::CheckData( LPCTSTR lpszData )
{
	return TRUE;
}

int CCxArrayBase::GetRealDataType( void ) const
{
	switch (m_DataType)
	{
	case DT_ARRAY4D_BOOLEAN:
	case DT_ARRAY3D_BOOLEAN:
	case DT_ARRAY2D_BOOLEAN:
	case DT_ARRAY1D_BOOLEAN:
	case DT_BOOLEAN:
		return DT_BOOLEAN;

	case DT_ARRAY4D_INT:
	case DT_ARRAY3D_INT:
	case DT_ARRAY2D_INT:
	case DT_ARRAY1D_INT:
	case DT_INT:
		return DT_INT;

	case DT_ARRAY4D_DOUBLE:
	case DT_ARRAY3D_DOUBLE:
	case DT_ARRAY2D_DOUBLE:
	case DT_ARRAY1D_DOUBLE:
	case DT_DOUBLE:
		return DT_DOUBLE;

	case DT_ARRAY4D_STRING:
	case DT_ARRAY3D_STRING:
	case DT_ARRAY2D_STRING:
	case DT_ARRAY1D_STRING:
	case DT_STRING:
		return DT_STRING;

	default:
		return 0;
	}
}
