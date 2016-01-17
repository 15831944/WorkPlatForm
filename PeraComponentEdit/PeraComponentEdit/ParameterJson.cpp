#include "StdAfx.h"
#include "ParameterJson.h"


CParameterJson::CParameterJson(void)
{
}


CParameterJson::~CParameterJson(void)
{
}

// CParameterJson.cpp�ڴ��ļ��е�AddNodeFromJson��������ӣ�
// ʵ�ַ��������ȶԲ����б�����Ȼ����뵽�б���
//�Ƚ��������ʣ�wordOne�󣬷���������С���ظ�������ȷ���0
int CParameterJson::wordCompare( Json::Value& jvParamsOne, Json::Value& jvParamsTwo)
{  	
	CString strParamsOne; 
	CString strParamsTwo ; 

	std::string strParamsOnetmp  = jvParamsOne["name"].asString();
	std::string strParamsTwotmp  = jvParamsTwo["name"].asString();

	strParamsOne.Format("%s", strParamsOnetmp.c_str());
	strParamsTwo.Format("%s", strParamsTwotmp.c_str());

	strParamsOne.MakeUpper();
	strParamsTwo.MakeUpper();

	return strParamsOne.Compare(strParamsTwo);
}

void CParameterJson::jvSwap(Json::Value& jvParams_min, Json::Value&jvParams_i)
{   // �����������ʵ����ݡ�
	Json::Value jvTmp;
	jvTmp = jvParams_min;
	jvParams_min = jvParams_i;
	jvParams_i = jvTmp;
}

int CParameterJson::JsonParamSort(Json::Value& jvParams)
{
	int nSize =(int)jvParams.size();
	int j,min;

	for(int i=0;i<nSize-1;i++)
	{         //ѡ������
		min=i;
		for(j=i+1;j<nSize;j++)
		{
			if(wordCompare(jvParams[j],jvParams[min])<0) min=j;
		}
		jvSwap(jvParams[min],jvParams[i]);
	}

	return 0;
}