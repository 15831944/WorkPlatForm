#include "StdAfx.h"
#include "CxDataType.h"

CString ConvertDataTypeName(int nDataType)
{
	switch (nDataType)
	{
	case DT_BOOLEAN:
		return "布尔型";
	case DT_INT:
		return "整型";
	case DT_DOUBLE:
		return "双精度浮点型";
	case DT_STRING:
		return "字符串型";
	case DT_FILE:
		return "文件型";
	case DT_ARRAY1D_BOOLEAN:
		return "布尔型1维数组";
	case DT_ARRAY2D_BOOLEAN:
		return "布尔型2维数组";
	case DT_ARRAY3D_BOOLEAN:
		return "布尔型3维数组";
	case DT_ARRAY1D_INT:
		return "整型1维数组";
	case DT_ARRAY2D_INT:
		return "整型2维数组";
	case DT_ARRAY3D_INT:
		return "整型3维数组";
	case DT_ARRAY1D_DOUBLE:
		return "双精度浮点型1维数组";
	case DT_ARRAY2D_DOUBLE:
		return "双精度浮点型2维数组";
	case DT_ARRAY3D_DOUBLE:
		return "双精度浮点型3维数组";
	case DT_ARRAY1D_STRING:
		return "字符串型1维数组";
	case DT_ARRAY2D_STRING:
		return "字符串型2维数组";
	case DT_ARRAY3D_STRING:
		return "字符串型3维数组";
	default:
		return "";
	}
}


CString ConvertParamDirName(int nDir)
{
	switch (nDir)
	{
	case PARAM_DIR_IN:
		return "输入";
	case PARAM_DIR_OUT:
		return "输出";
	case PARAM_DIR_INOUT:
		return "输入输出";
	default:
		return "";
	}
}