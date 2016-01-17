#pragma once
class CParameterJson
{
public:
	CParameterJson(void);
	~CParameterJson(void);


public:
	int JsonParamSort(Json::Value& jvParams);
	void jvSwap(Json::Value& jvParams_min, Json::Value&jvParams_i);
	int wordCompare( Json::Value& jvParamsOne, Json::Value& jvParamsTwo);

};

