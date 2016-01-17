#include "StdAfx.h"
#include "CxDataType.h"

CString ConvertDataTypeName(int nDataType)
{
	switch (nDataType)
	{
	case DT_BOOLEAN:
		return "������";
	case DT_INT:
		return "����";
	case DT_DOUBLE:
		return "˫���ȸ�����";
	case DT_STRING:
		return "�ַ�����";
	case DT_FILE:
		return "�ļ���";
	case DT_ARRAY1D_BOOLEAN:
		return "������1ά����";
	case DT_ARRAY2D_BOOLEAN:
		return "������2ά����";
	case DT_ARRAY3D_BOOLEAN:
		return "������3ά����";
	case DT_ARRAY1D_INT:
		return "����1ά����";
	case DT_ARRAY2D_INT:
		return "����2ά����";
	case DT_ARRAY3D_INT:
		return "����3ά����";
	case DT_ARRAY1D_DOUBLE:
		return "˫���ȸ�����1ά����";
	case DT_ARRAY2D_DOUBLE:
		return "˫���ȸ�����2ά����";
	case DT_ARRAY3D_DOUBLE:
		return "˫���ȸ�����3ά����";
	case DT_ARRAY1D_STRING:
		return "�ַ�����1ά����";
	case DT_ARRAY2D_STRING:
		return "�ַ�����2ά����";
	case DT_ARRAY3D_STRING:
		return "�ַ�����3ά����";
	default:
		return "";
	}
}


CString ConvertParamDirName(int nDir)
{
	switch (nDir)
	{
	case PARAM_DIR_IN:
		return "����";
	case PARAM_DIR_OUT:
		return "���";
	case PARAM_DIR_INOUT:
		return "�������";
	default:
		return "";
	}
}