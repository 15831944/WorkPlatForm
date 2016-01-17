#pragma once
#include "StlHeader.h"
#include "Tools.h"

class CInterfaceData_CopyFiles;           // �ļ���������ӿ�������
class CInterfaceData_QueryFilesOperation; //��ѯ��������ӿ���
class CInterfaceData_DoFilesOperation; // ȡ����������ӿ���
/*
ҵ��㿽��������
*/
class CPeraCopyManager
{
public:
	static CPeraCopyManager * Instance(); //��ȡ����������
	CString AddCopyTask(CInterfaceData_CopyFiles * pIData,bool IsLoadFinished = false); // ��ҵ��㿽�������������ӿ������񣬷���Json�ַ���
	CString AddCopyTask(CInterfaceData_PasteFiles * pIData); // ��ҵ��㿽�������������ӿ������񣬷���Json�ַ���
	void QueryCopyProgress(CInterfaceData_QueryFilesOperation & IData);  // ҵ����ѯ����
	void DoFilesOperation(const CInterfaceData_DoFilesOperation &IData);   // ҵ���ȡ��
	void LoadHistoryCopyTask(); // ���������������ʷ��������
private:
	~CPeraCopyManager();
	 CPeraCopyManager(); // ���������캯������Ϊ˽���ͣ�ֻ�����û�ͨ��Instance()����ʵ��
private:
	class GabageCollector // ����˽����Ƕ��
	{
	public:
		GabageCollector()
		{
			ZTools::WriteZToolsLog("����������CPeraCopyManager::GabageCollector����...");
		}
		~GabageCollector()
		{
			if (m_PManagetr)
			{
				delete m_PManagetr;
				m_PManagetr = 0;
				ZTools::WriteZToolsLog("����������CPeraCopyManager::GabageCollectorִ�е�����������������...");
			}
		}

	};
	static CPeraCopyManager * m_PManagetr;
	static GabageCollector m_gc; // ���������������ľ�̬���󣬳��������ϵͳ����øö����������������ִ�жԵ������������
};