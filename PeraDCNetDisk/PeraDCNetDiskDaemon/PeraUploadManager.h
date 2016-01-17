#pragma once
#include "StlHeader.h"
#include "Tools.h"

class CInterfaceData_UploadFile; // �ϴ�����ӿ�������
class CInterfaceData_QueryFilesOperation; //��ѯ��������ӿ���
class CInterfaceData_DoFilesOperation; // ȡ����������ӿ���
/*
 ҵ����ϴ�������
*/
class CPeraUploadManager
{
public:
	static CPeraUploadManager * Instance(); //��ȡ����������
	CString AddUploadTask(CInterfaceData_UploadFile * pIData,bool bLoadFinished = false); // ��ҵ����ϴ��������������ϴ����񣬷���Json�ַ���
	void QueryUploadProgress(CInterfaceData_QueryFilesOperation & IData); // ҵ����ѯ����
	void DoFilesOperation(const CInterfaceData_DoFilesOperation &IData); // ִ��delete��stop��pause��resume�Ľӿ�
	void LoadHistoryUploadTask(); // ���������������ʷ�ϴ�����
	CString AddOutPutTask(CInterfaceData_UploadFile * pIData); // �ύ��������øú������ϴ���ַ��ͬ
private:
	CPeraUploadManager(); // ���������캯������Ϊ˽���ͣ�ֻ�����û�ͨ��Instance()����ʵ��
	~CPeraUploadManager();
private:
	class GabageCollector // ����˽����Ƕ��
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("����������CPeraUploadManager::GabageCollector����...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("����������CPeraUploadManager::GabageCollectorִ�е�����������������...");
			}
		}

	};
	static CPeraUploadManager * m_PManagetr;
	static GabageCollector m_gc; // ���������������ľ�̬���󣬳��������ϵͳ����øö����������������ִ�жԵ������������
};