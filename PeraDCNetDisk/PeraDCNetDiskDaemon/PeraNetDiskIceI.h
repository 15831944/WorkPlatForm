#pragma once
#include "PeraNetDiskIce.h"
#include <vector>
using namespace std;

class CInterfaceData_QueryFilesOperation ;
class CInterfaceData_CreateFile;
class CInterfaceData_QueryFilesInfo;
class CInterfaceData_OpenFile;

class Json::Value;
class PeraNetDiskIceI : public Pera::NetDisk::Daemon::PeraNetDiskDaemonService
{
public:
	PeraNetDiskIceI();
	~PeraNetDiskIceI();

	virtual ::std::string InvokeMethod( const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current( ) );
private:
	std::string GetDefaultErrMsgJson(const char * pszErrMsg = "����Json��������",int iErrCode = 0);
	void QueryLocalFileType(std::string & strJsonOut);		 // ���ⲿ����ѯ���ؿ��½����ļ�����
	void QueryFilesInfo(const ::std::string&, std::string&); //���ⲿ����ѯ�ļ�������Ϣ
	void CreateFile(const ::std::string&,std::string&);		 // ���ⲿ�����������ļ�
	void OpenFile(const ::std::string&,std::string&);
	void CopyFiles(const ::std::string&,std::string&);		 //���ⲿ�����������ļ����ļ���
	void GetCacheDir(const ::std::string&,std::string&);	 //���ⲿ����ȡ����·��
	void SetCacheDir(const ::std::string&,std::string&);	 //���ⲿ�����û���·��
	void QueryFilesOperation(const ::std::string&,std::string&); //���ⲿ����ѯ�ļ�����״̬
	void DoFilesOperation(const ::std::string&,std::string&); //���ⲿ��ȡ���ļ���������
	void OnDownloadFile(const ::std::string&, std::string&); //���ⲿ�������ļ��ӿ�
	void OnUploadFile(const ::std::string&, std::string&); //���ⲿ���ϴ��ļ��ӿ�
	void OnGetSettings(const ::std::string&, std::string&); //���ⲿ����ȡ���ýӿ�
	void OnSetSettings(const ::std::string&, std::string&);  //���ⲿ�����ýӿ�
	void OnQueryDir(const ::std::string&, std::string&);  //���ⲿ����ѯ�ļ������ݽӿ�
	void QueryDirSubCount(const ::std::string&, std::string&);  //���ⲿ����ѯ�ļ���������Ŀ�ӿ�
	void QueryLocalOfficeFileType(std::string & strJsonOut);  //���ⲿ����ѯ�������½���Office�ļ����ͽӿ�
	void ReNameItem(const ::std::string&, std::string&); //���ⲿ�������������ļ������ļ��еĽӿ�
	void DeleteItem(const ::std::string&, std::string&); //���ⲿ��ɾ������ļ����ļ��еĽӿ�
	void PasteFiles(const ::std::string&, std::string&); //���ⲿ������ճ��ʱ���ļ����ƽӿ�
	void MoveFiles(const ::std::string&, std::string&); //���ⲿ������ճ��ʱ���ļ��ƶ��ӿ�
	void SubmitToOutPut(const ::std::string&, std::string&); //���ⲿ���ύ�����ļ�������ӿ�

	void QueryDownloadProgress(CInterfaceData_QueryFilesOperation & IData);
	void CreateFile(CInterfaceData_CreateFile &IData);
	void QueryFilesInfo(CInterfaceData_QueryFilesInfo &IData);
};
