#pragma once

//�ϴ�
std::string CallUploadHprose(std::string strHproseRpc, std::string strFilePath, std::string strMd5, BOOL bMove);
//����
std::string CallDownloadHprose(std::string strHproseBaseUrl, std::string strFileId, std::string strFileName);
//�ϴ����
std::string CallUploadFilesHprose( std::string strHproseRpc, std::string strJson, BOOL bMove );
//���ض��
std::string CallDownloadFilesHprose(std::string strHproseBaseUrl, std::string strJson);