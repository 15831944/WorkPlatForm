#pragma once

//上传
std::string CallUploadHprose(std::string strHproseRpc, std::string strFilePath, std::string strMd5, BOOL bMove);
//下载
std::string CallDownloadHprose(std::string strHproseBaseUrl, std::string strFileId, std::string strFileName);
//上传多个
std::string CallUploadFilesHprose( std::string strHproseRpc, std::string strJson, BOOL bMove );
//下载多个
std::string CallDownloadFilesHprose(std::string strHproseBaseUrl, std::string strJson);