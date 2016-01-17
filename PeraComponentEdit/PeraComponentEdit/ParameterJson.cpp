#include "StdAfx.h"
#include "ParameterJson.h"


CParameterJson::CParameterJson(void)
{
}


CParameterJson::~CParameterJson(void)
{
}

// CParameterJson.cpp在此文件中的AddNodeFromJson函数中添加，
// 实现方法，是先对参数列表排序，然后插入到列表中
//比较两个单词，wordOne大，返回正数，小返回负数，相等返回0
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
{   // 交换两个单词的内容。
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
	{         //选择法排序
		min=i;
		for(j=i+1;j<nSize;j++)
		{
			if(wordCompare(jvParams[j],jvParams[min])<0) min=j;
		}
		jvSwap(jvParams[min],jvParams[i]);
	}

	return 0;
}