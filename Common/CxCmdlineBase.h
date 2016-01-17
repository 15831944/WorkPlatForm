#pragma once
#include "StlHeader.h"

class CCxCmdlineBase
{
public:
	CCxCmdlineBase();
	virtual ~CCxCmdlineBase();
	virtual void Clear( void );
	virtual BOOL Parse( LPCTSTR lpszData = NULL );
	virtual BOOL ParseJson( LPCTSTR lpszData );
	virtual CString GetProtocolName(void);
	CString	GetValue( LPCTSTR lpszFlage );
	int		GetParamSize();

protected:
	
	/*
	���ܣ�
		�ָ��һ�������͸ò�����Ӧ��ֵ��
	������
		_sFlag��_IN  ������־���Լ��ſ�ͷ���ַ��������磺-i��
		_sValue��_OUT ����ֵ
		_sSrcParam��_IN_OUT ����ԭʼ�����ַ����������ָ�󣬺���δ�ָ���ַ���
	*/
	BOOL SplitParamFlagValue( CString& _sFlag, CString& _sValue, CString& _sParamStr );
	/*
	���ܣ�
		�Ӳ������зָ��һ�������������Ż��߿ո������ָ�ı�־��
	������
		_sValue��_OUT ����ֵ
		_sParamStr��_IN_OUT ����ԭʼ�����ַ����������ָ�֮��Ĳ�����
	*/
	BOOL SplitParamValue( CString& _sValue, CString& _sParamStr );

	//��m_vCmdFlags��m_vCmdValues��ƥ��Ĳ������ݶ�
	void SetValue( LPCTSTR lpszFlag, LPCTSTR lpszValue );

public:
	CString m_sSrc; //ԭʼ������
	CString m_sExecDir;
	CString m_sExecFilename;

	/*
	��������·�����ʱ��m_vCmdFlags�д洢�ձ�ǣ�m_vCmdValues�洢·����
	��������Э�����ʱ��m_vCmdFlags�д洢����������m_vCmdValues�洢�����Ӧ��ֵ��
	��������json���ʱ��m_vCmdFlags�д洢������json�����m_vCmdValues�洢�����Ӧ��ֵ��
	��������-i��-u���ʱ��m_vCmdFlags�д洢��i����u����m_vCmdValuesΪ�գ�
	�����������key���ʱ��m_vCmdFlags�д洢����key��m_vCmdValues�洢����key��Ӧ��ֵ��*/	
	vector< CString > m_vCmdFlags;
	vector< CString > m_vCmdValues;
};
