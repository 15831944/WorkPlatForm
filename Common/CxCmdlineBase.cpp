#include "stdafx.h"
#include "CxCmdlineBase.h"
#include "FileFunc.h"
#include "GetGlobalDataFromReg.h"

/*
ʹ��˵����
CCxCmdlineBase�࣬��Ϊ���������еĻ��࣬�ṩ��Э��ע�ᡢж�ء������ָ�Ȼ���������

���Ƚ���Я������·���Ĳ���������Path���͡�·�����洢��m_vCmdFlags��m_vCmdValues�С�

��ν�����Э��������У���Ҫ����������ʵ�� GetProtocolName �麯������������������
�ǿյ��ַ�����˵��֧��Э��������Э�����ƺ�����ַ���������������json�����ַ�����

�ٴν�����json�������У�����Json���͡�json���󡱴洢��m_vCmdFlags��m_vCmdValues�У�
��������������ʵ��ParseJson����������

Ȼ������������Ϸ�ʽ�������������¼��ֹ��򣬽������������е�ÿ����־�Ͷ�Ӧ��ֵ��
���������ı�־��ֵ�����ɶԷ��� m_vCmdFlags��m_vCmdValues �����С������������£�

1����־�����ԡ����š���ͷ���ԡ��ո񡱽�����
2�������־��������������ԡ�ð�š���Ϊ�ָ������Կո������
3��������������пո񣬱����ԡ�˫���š���������������
4����־���־֮�䡢ֵ��ֵ֮�䡢ֵ���־֮�䣬�á��ո񡱷ָ���

�˹���ο� regsvr32 ϵͳ���������

���ڿ��ܳ��ֵļ��ָ�ʽ���£�

c:\workspace\peraworkspace.exe d:\doc ����\1.txt
c:\workspace\peraworkspace.exe "d:\doc ����\1.txt"
"c:\workspace\peraworkspace.exe" d:\doc ����\1.txt
"c:\workspace\peraworkspace.exe" "d:\doc ����\1.txt"

c:\workspace\peraworkspace.exe PeraWS:{ jsonStr }
"c:\workspace\peraworkspace.exe" PeraWS: { jsonStr }

c:\workspace\peraworkspace.exe -json:{ jsonStr }
"c:\workspace\peraworkspace.exe" -json:{ jsonStr }

c:\workspace\peraworkspace.exe -i
c:\workspace\peraworkspace.exe -u
"c:\workspace\peraworkspace.exe" -i
"c:\workspace\peraworkspace.exe" -u

c:\workspace\peraworkspace.exe -ws:"d:\doc ����\1.txt"
"c:\workspace\peraworkspace.exe" -ws:"d:\doc ����\1.txt"

c:\workspace\peraworkspace.exe -key:value (value�к��пո�ʱ��������˫����������)
"c:\workspace\peraworkspace.exe" -key:value


������ʽ������֧�֡�
*/

CCxCmdlineBase::CCxCmdlineBase()
{
}

CCxCmdlineBase::~CCxCmdlineBase()
{
	Clear();
}

void CCxCmdlineBase::Clear( void )
{
	m_sSrc.Empty();
	m_sExecDir.Empty();
	m_sExecFilename.Empty();
	m_vCmdFlags.clear();
	m_vCmdValues.clear();
}

BOOL CCxCmdlineBase::ParseJson( LPCTSTR lpszData )
{
	return TRUE;
}

BOOL CCxCmdlineBase::Parse( LPCTSTR lpszData )
{
	Clear();
	//ȡ������������
	m_sSrc = lpszData == NULL ? ::GetCommandLine() : lpszData;
	m_sSrc.Trim();
	if ( m_sSrc.IsEmpty() ) return FALSE;

	int nPos = -1;
	CString sParam, sFlag, sValue, sExecPath;
	
	//�ָ� ģ��·�� �� ����
	sParam = m_sSrc;
	if ( !SplitParamValue( sExecPath, sParam ) )
	{
		if ( !IsAccessibleFile( sExecPath ) )
		{
			return FALSE;
		}
		m_sExecDir = sExecPath.Left( sExecPath.ReverseFind( '\\' ) );
		m_sExecFilename = sExecPath.Right( sExecPath.GetLength()-m_sExecDir.GetLength()-1 );
		return TRUE;
	}
	else
	{
		if ( !IsAccessibleFile( sExecPath ) )
		{
			return FALSE;
		}
		else
		{
			//����������������
		}
	}

	m_sExecDir = sExecPath.Left( sExecPath.ReverseFind( '\\' ) );
	m_sExecFilename = sExecPath.Right( sExecPath.GetLength()-m_sExecDir.GetLength()-1 );

	//����������·�������
	int nLeft, nRight;
	nLeft = nRight = -1;
	CString sTemp;
	BOOL bMark = FALSE;
	//�ж��ĵ�·���Ƿ���˫����
	nLeft = sParam.Find( '\"', 0 );
	if ( nLeft == 0 )
	{
		bMark = TRUE;
		nRight = sParam.ReverseFind( '\"' );
		if ( nRight == sParam.GetLength() - 1 && nRight > nLeft + 1 )
		{
			sTemp = sParam;
			sTemp.Trim( '\"' );
		}
		else
		{
			//˫���Ų�ƥ��
			return FALSE;
		}
	}
	else if( nLeft == -1 )
	{
		//��˫����
		sTemp = sParam;
	}
	else
	{
		//�в��ڲ�����ʼλ�õ�����
	}
	if ( IsAccessibleFile( sTemp ) )
	{
		SetValue( "", sTemp );
		return TRUE;
	}
	else
	{
		if ( bMark )//��Ч·��
		{
			return FALSE;
		}
		else
		{
			//���������������
		}
	}

	//urlЭ�鷽ʽ����
	sFlag = GetProtocolName();
	if ( !sFlag.IsEmpty() )
	{
		nPos = sParam.Find( (LPCTSTR)sFlag, 0 );
		if ( nPos == 0 )
		{
			if ( sFlag.GetLength() + 3 > sParam.GetLength() )
			{
				return FALSE;
			}
			sValue = sParam.Mid( sFlag.GetLength() + 1, sParam.GetLength() - sFlag.GetLength() - 1 );
			sValue.Trim();
			return ParseJson( sValue );
		}
		else
		{
			return FALSE; //��������δ�ҵ�ƥ���Э������
		}
	}
	else
	{
		//���Խ���-json
	}

	//����������json�����
	int nHeaderPos = sParam.Find( "-" );
	if ( nHeaderPos == 0 )
	{
		if ( sParam.GetLength() > 6 )//���ٰ���-json:
		{
			CString strHeader = sParam.Mid(1, 4);
			if ( strHeader.CompareNoCase("json") == 0 )
			{
				CString sJson = sParam.Mid( 6, sParam.GetLength() - 6 );
				sJson.Trim();
				return ParseJson( sJson );
			}
			else
			{
				//������������
			}
		}
		else
		{
			//������������
		}
	}
	else
	{
		return FALSE;//δ�ԡ�-����ͷ
	}
	
	//��������
	BOOL bContinue = TRUE;
	while ( bContinue )
	{
		bContinue = SplitParamFlagValue( sFlag, sValue, sParam );
		if ( sFlag.IsEmpty() )
		{
			return FALSE;
		}
		if ( sFlag.CompareNoCase( "i" ) == 0 || sFlag.CompareNoCase( "u" ) == 0 )
		{
			m_vCmdFlags.clear();
			m_vCmdValues.clear();
			SetValue( sFlag, "" );
			break;
		}
		SetValue( sFlag, sValue );
	}

	for ( int i=0; i<(int)m_vCmdFlags.size(); i++ )
	{
		if ( m_vCmdFlags[i].CompareNoCase("i") == 0 )
		{
			return OnRegisterUrlProtocol( GetProtocolName() );
		}
		else if ( m_vCmdFlags[i].CompareNoCase("u") == 0 )
		{
			return OnUnregisterUrlProtocol( GetProtocolName() );
		}
	}

	return TRUE;
}

CString CCxCmdlineBase::GetProtocolName( void )
{
	return "";
}

/*
���ܣ�
	�ָ��һ�������͸ò�����Ӧ��ֵ��
������
	_sFlag��_IN  ������־���Լ��ſ�ͷ���ַ��������磺-i��
	_sValue��_OUT ����ֵ
	_sSrcParam��_IN_OUT ����ԭʼ�����ַ����������ָ�󣬺���δ�ָ���ַ���
*/
BOOL CCxCmdlineBase::SplitParamFlagValue( CString& _sFlag, CString& _sValue
	, CString& _sParamStr )
{
	int nPos = -1;
	CString sTmp;
	BOOL bValue = FALSE;
	BOOL bSpace = FALSE;

	_sFlag.Empty();
	_sValue.Empty();
	//ȥ��ͷβ�ո�
	_sParamStr.Trim();
	if ( _sParamStr.GetLength() < 1 ) return FALSE;
	//��һ���ַ��Ƿ�Ϊ��-��
	if ( _sParamStr.GetAt(0) == '-' )
	{
		nPos = 1;
		while ( nPos < _sParamStr.GetLength() )
		{
			if ( _sParamStr.GetAt( nPos ) == ' ' )
			{
				bSpace = TRUE;
				break;
			}
			else if ( _sParamStr.GetAt( nPos ) == ':' )
			{
				bValue = TRUE;
				break;
			}
			nPos++;
		}
		if ( bSpace )
		{
			if ( nPos < 2 )
			{
				//�Ƿ���ʽ
				_sParamStr.Empty();
				return FALSE;
			}

			//�ҵ�һ������
			_sFlag = _sParamStr.Mid( 1, nPos - 1 );
			sTmp = _sParamStr.Mid( nPos + 1, _sParamStr.GetLength() - nPos - 1 );
			_sParamStr = sTmp;
			_sParamStr.Trim();
			return ( !_sParamStr.IsEmpty() );
		}
		//
		if ( bValue )
		{
			if ( nPos <= 1 )
			{
				_sParamStr.Empty();
				return FALSE;
			}
			_sFlag = _sParamStr.Mid( 1, nPos - 1 );

			if ( nPos + 1 >= _sParamStr.GetLength() )
			{
				_sParamStr.Empty();
				return FALSE;
			}

			sTmp = _sParamStr.Mid( nPos + 1, _sParamStr.GetLength() - nPos - 1 );
			_sParamStr = sTmp;
			_sParamStr.Trim();

			return SplitParamValue( _sValue, _sParamStr );
		}

		_sFlag = _sParamStr.Mid( 1, _sParamStr.GetLength() - 1 );
		_sParamStr.Empty();
		return FALSE;
	}
	else
	{
		return SplitParamValue( _sValue, _sParamStr );
	}
}

/*
���ܣ�
	�Ӳ������зָ��һ�������������Ż��߿ո������ָ�ı�־��
������
	_sValue��_OUT ����ֵ
	_sParamStr��_IN_OUT ����ԭʼ�������������ָ�֮��Ĳ�����
*/
BOOL CCxCmdlineBase::SplitParamValue( CString& _sValue, CString& _sParamStr )
{
	int nPos = -1;
	CString sTmp;

	_sValue.Empty();
	//
	_sParamStr.Trim();
	if ( _sParamStr.GetLength() < 1 ) return FALSE;
	//��һ���ַ��Ƿ�Ϊ��˫���š�
	if ( _sParamStr.GetAt( 0 ) == '\"' )
	{
		//��һ���ַ���˫����
		nPos = _sParamStr.Find( '\"', 1 );
		if ( nPos == -1 )
		{
			//�Ƿ���ʽ��˫���Ų�ƥ��
			_sParamStr.Empty();
			return FALSE;
		}
		else
		{
			_sValue = _sParamStr.Mid( 1, nPos - 1 );
			if ( nPos + 1 >= _sParamStr.GetLength() )
			{
				_sParamStr.Empty();
				return FALSE;
			}
			else
			{
				sTmp = _sParamStr.Mid( nPos + 1, _sParamStr.GetLength() - nPos - 1 );
				_sParamStr = sTmp;
				_sParamStr.Trim();
				return ( !_sParamStr.IsEmpty() );
			}
		}
	}
	else
	{
		//��һ���ַ�����˫����
		nPos = _sParamStr.Find( ' ', 0 );
		if ( nPos == -1 )
		{
			if ( _sParamStr.GetLength() < 1 )
			{
				//�Ƿ���ʽ
				_sParamStr.Empty();
				return FALSE;
			}
			else
			{
				_sValue = _sParamStr;
				_sParamStr.Empty();
				return FALSE;
			}
		}
		else
		{
			_sValue = _sParamStr.Mid( 0, nPos );
			sTmp = _sParamStr.Mid( nPos + 1, _sParamStr.GetLength() - nPos - 1 );
			_sParamStr = sTmp;
			_sParamStr.Trim();
			return ( !_sParamStr.IsEmpty() );
		}
	}
}

CString CCxCmdlineBase::GetValue( LPCTSTR lpszFlage )
{
	CString sFlag = lpszFlage == NULL ? "" : lpszFlage;
	vector< CString >::iterator itFlag = m_vCmdFlags.begin();
	vector< CString >::iterator itValue = m_vCmdValues.begin();
	for ( ; itFlag != m_vCmdFlags.end() && itValue != m_vCmdValues.end(); itFlag++, itValue++ )
	{
		if ( ( sFlag.IsEmpty() && itFlag->CompareNoCase( "" ) == 0 && m_vCmdFlags.size() == 1 ) ||
			( sFlag.IsEmpty() && itFlag->CompareNoCase( "ws" ) == 0 ) )
		{
			if ( IsAccessibleFile( *itValue ) )
			{
				return *itValue;//Path
			}
			else
			{
				return "";
			}
		}
		if ( sFlag.CompareNoCase( *itFlag ) == 0 )
		{
			return *itValue;//Path
		}
	}
	return "";
}

void CCxCmdlineBase::SetValue( LPCTSTR lpszFlag, LPCTSTR lpszValue )
{
	CString sFlag = lpszFlag == NULL ? "" : lpszFlag;
	CString sValue = lpszValue == NULL ? "" : lpszValue;
	m_vCmdFlags.push_back( sFlag );
	m_vCmdValues.push_back( sValue );
}

/*
��ȡ����������
���ز�������������쳣����-1.
*/
int CCxCmdlineBase::GetParamSize()
{
	int nFlagNum = m_vCmdFlags.size();
	int nValueNum = m_vCmdValues.size();
	if ( nFlagNum == nValueNum )
	{
		return nFlagNum;
	}
	else
	{
		return -1;//����ԣ����ش���
	}
}
