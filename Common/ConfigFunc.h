#pragma once
//ͳ��ȡ���õĵ��ýӿ�
//�ڲ�ά��һ��MAP���󣬻�����ȡ�������ã���һ��ȡ���û�ǿ�ƶ�����ʱ�����ȡ���õ�ͨ��COM�ӿ�
//

//��������ʱ������config.ini�д���һ���µ��½�section��������ģ�黮��
//�����½ڻ������½��´�����ֵ�ԣ����¸�ʽ
/*
[��ģ����section]
��=ֵ
key=value
*/
//�޸ĺ��config.ini�ύ���������Ŀ¼WorkPlatForm\PeraClientConfig\bin�£������汾ʱ�Դ�Ŀ¼�������ļ�Ϊ׼

extern CString GetConfig(CString sSection, CString sKey, BOOL bReRead = FALSE);
extern BOOL SetConfig(CString sSection, CString sKey, CString sValue);