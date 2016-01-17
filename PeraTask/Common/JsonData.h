#pragma once

class JosnDataRunRobot
{
public:
	std::string botPath;
	std::string taskId;
	std::string flag;
	std::string title;
	std::string inXml;
	std::string outPath;
	// 新增以下三个属性
	std::string attachType;
	std::string attachData;
	std::string idList;
	//增加两个属性
	std::string taskName;
	std::string processName;
	void Clear();
	//void FromJson(std::string strJson);
	std::string ToJson();
};
