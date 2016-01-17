#pragma once

//��������
enum {
	DT_NULL = 0
	, DT_BOOLEAN
	, DT_INT
	, DT_DOUBLE
	, DT_STRING
	, DT_FILE
	, DT_ARRAY1D_BOOLEAN
	, DT_ARRAY2D_BOOLEAN
	, DT_ARRAY3D_BOOLEAN
	, DT_ARRAY4D_BOOLEAN
	, DT_ARRAY1D_INT
	, DT_ARRAY2D_INT
	, DT_ARRAY3D_INT
	, DT_ARRAY4D_INT
	, DT_ARRAY1D_DOUBLE
	, DT_ARRAY2D_DOUBLE
	, DT_ARRAY3D_DOUBLE
	, DT_ARRAY4D_DOUBLE
	, DT_ARRAY1D_STRING
	, DT_ARRAY2D_STRING
	, DT_ARRAY3D_STRING
	, DT_ARRAY4D_STRING
	, DT_DIM // �Զ�������ͣ���Ӧ CxArrayDim ����ָ��
};

//��������
enum {
	PARAM_DIR_NULL = 0
	, PARAM_DIR_IN //����
	, PARAM_DIR_OUT //���
	, PARAM_DIR_INOUT //�������
};

extern CString ConvertDataTypeName(int nDataType);
extern CString ConvertParamDirName(int nDir);