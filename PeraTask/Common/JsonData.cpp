#include "stdafx.h"
#include "JsonData.h"

void JosnDataRunRobot::Clear()
{
	botPath.clear();
	taskId.clear();
	flag.clear();
	title.clear();
	inXml.clear();
	outPath.clear();
	attachType.clear();
	attachData.clear();
	idList.clear();
	taskName.clear();
	processName.clear();
}

// void JosnDataRunRobot::FromJson( std::string strJson )
// {
// 
// }

std::string JosnDataRunRobot::ToJson()
{
	Json::FastWriter writer;
	Json::Value root;
	root["botPath"] = Json::Value(botPath);
	root["taskId"] = Json::Value(taskId);
	root["flag"] = Json::Value(flag);
	root["title"] = Json::Value(title);
	root["inXml"] = Json::Value(inXml);
	root["outPath"] = Json::Value(outPath);
	root["attachType"] = Json::Value(attachType);
	root["attachData"] = Json::Value(attachData);
	root["taskName"] = Json::Value(taskName);
	root["processName"] = Json::Value(processName);
	
	Json::Reader reader;
	Json::Value jIdList;
	if (reader.parse(idList, jIdList))
		root["idList"]=jIdList;
	
	return writer.write(root);
}
