#pragma once
typedef struct
{
	void Clear()
	{
		m_strID.Empty();
		m_strDesrciption.Empty();
		m_strName.Empty();
		m_strAbsPath.Empty();
		m_strRunParam.Empty();
		m_strDefaultKey.Empty();
		m_strReverseKey.Empty();
		m_strKeyName.Empty(); 
		m_strKeyValueName.Empty();
		m_strRunPath.Empty();
		m_strRunName.Empty();

		m_strLocalPath.Empty();
		m_strLocalRunParam.Empty();
		m_strPathConfigType.Empty();
		m_strURL.Empty();
	}
	// �������˴�����������Ϣ��ֻ����
	CString m_strID;          // ���ID
	CString m_strDesrciption; // �������
	CString m_strName;        // �������
	CString m_strAbsPath;     // ����·��
	CString m_strRunParam;     // ��������
	CString m_strDefaultKey;   // Ĭ������
	CString m_strReverseKey;   // ��������
	CString m_strKeyName;      // ע�����
	CString m_strKeyValueName;     // ע����ֵ
	CString m_strRunPath;      // ����·��
	CString m_strRunName;      // ��������

	CString m_strPathConfigType;      // ·����������
	CString m_strURL;                //  URL��ַ(���ҽ���m_strPathConfigTypeΪ2ʱ�ò����ᱻʹ��)

	// �����Ǳ���������Ϣ�����޸ģ�
	CString m_strLocalPath; // ��������·��
	CString m_strLocalRunParam; // ����������������
}ToolObject;