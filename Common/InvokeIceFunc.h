#pragma once

//ʹ��COM�����������ICE�ӿ�

//�������������ӿڣ��������Ϊ���ֽڱ��룬�ڲ�����ʱתΪutf-8�ٵ��÷���������
extern CString InvokeWsMethod(CString sMethod, CString sJson);

//�������������ӿڣ����������������ת����ֱ�ӵ��÷���������
extern CString InvokeWsMethodOrignal(CString sMethod, CString sJson);

//���õ���������ӿڣ��������Ϊ���ֽڱ��룬�ڲ�����ʱתΪutf-8�ٵ��ñ�������
extern CString InvokeLocalWsMethod(CString sMethod, CString sJson);

//���õ���������ӿڣ����������������ת����ֱ�ӵ��ñ�������
extern CString InvokeLocalWsMethodOrignal(CString sMethod, CString sJson);