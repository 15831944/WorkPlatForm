#include "stdafx.h"
#include "CxCmdlineBase.h"
#include "FileFunc.h"
#include "GetGlobalDataFromReg.h"

/*
使用说明：
CCxCmdlineBase类，作为解析命令行的基类，提供了协议注册、卸载、参数分割等基本操作。

优先解析携带绝对路径的参数，将“Path”和“路径”存储在m_vCmdFlags和m_vCmdValues中。

其次解析带协议的命令行，需要在派生类中实现 GetProtocolName 虚函数，如果这个函数返回
非空的字符串，说明支持协议启动。协议名称后面的字符串必须是完整的json对象字符串。

再次解析带json的命令行，将“Json”和“json对象”存储在m_vCmdFlags和m_vCmdValues中，
可以在派生类中实现ParseJson函数解析。

然后，如果不是以上方式启动，则按照以下几种规则，解析出命令行中的每个标志和对应的值。
解析出来的标志和值，将成对放在 m_vCmdFlags、m_vCmdValues 数组中。解析规则如下：

1、标志必须以“减号”开头，以“空格”结束。
2、如果标志带参数，则必须以“冒号”作为分隔符，以空格结束。
3、如果参数本身含有空格，必须以“双引号”将参数阔起来。
4、标志与标志之间、值与值之间、值与标志之间，用“空格”分隔。

此规则参考 regsvr32 系统命令帮助。

现在可能出现的几种格式如下：

c:\workspace\peraworkspace.exe d:\doc 测试\1.txt
c:\workspace\peraworkspace.exe "d:\doc 测试\1.txt"
"c:\workspace\peraworkspace.exe" d:\doc 测试\1.txt
"c:\workspace\peraworkspace.exe" "d:\doc 测试\1.txt"

c:\workspace\peraworkspace.exe PeraWS:{ jsonStr }
"c:\workspace\peraworkspace.exe" PeraWS: { jsonStr }

c:\workspace\peraworkspace.exe -json:{ jsonStr }
"c:\workspace\peraworkspace.exe" -json:{ jsonStr }

c:\workspace\peraworkspace.exe -i
c:\workspace\peraworkspace.exe -u
"c:\workspace\peraworkspace.exe" -i
"c:\workspace\peraworkspace.exe" -u

c:\workspace\peraworkspace.exe -ws:"d:\doc 测试\1.txt"
"c:\workspace\peraworkspace.exe" -ws:"d:\doc 测试\1.txt"

c:\workspace\peraworkspace.exe -key:value (value中含有空格时，必须用双引号括起来)
"c:\workspace\peraworkspace.exe" -key:value


其他格式将不予支持。
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
	//取得完整命令行
	m_sSrc = lpszData == NULL ? ::GetCommandLine() : lpszData;
	m_sSrc.Trim();
	if ( m_sSrc.IsEmpty() ) return FALSE;

	int nPos = -1;
	CString sParam, sFlag, sValue, sExecPath;
	
	//分割 模块路径 和 参数
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
			//继续解析后续参数
		}
	}

	m_sExecDir = sExecPath.Left( sExecPath.ReverseFind( '\\' ) );
	m_sExecFilename = sExecPath.Right( sExecPath.GetLength()-m_sExecDir.GetLength()-1 );

	//解析参数是路径的情况
	int nLeft, nRight;
	nLeft = nRight = -1;
	CString sTemp;
	BOOL bMark = FALSE;
	//判断文档路径是否有双引号
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
			//双引号不匹配
			return FALSE;
		}
	}
	else if( nLeft == -1 )
	{
		//无双引号
		sTemp = sParam;
	}
	else
	{
		//有不在参数起始位置的引号
	}
	if ( IsAccessibleFile( sTemp ) )
	{
		SetValue( "", sTemp );
		return TRUE;
	}
	else
	{
		if ( bMark )//无效路径
		{
			return FALSE;
		}
		else
		{
			//继续以下情况解析
		}
	}

	//url协议方式启动
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
			return FALSE; //命令行中未找到匹配的协议名称
		}
	}
	else
	{
		//尝试解析-json
	}

	//解析参数是json的情况
	int nHeaderPos = sParam.Find( "-" );
	if ( nHeaderPos == 0 )
	{
		if ( sParam.GetLength() > 6 )//至少包含-json:
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
				//继续参数解析
			}
		}
		else
		{
			//继续参数解析
		}
	}
	else
	{
		return FALSE;//未以“-”开头
	}
	
	//解析参数
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
功能：
	分割出一个参数和该参数对应的值。
参数：
	_sFlag：_IN  参数标志，以减号开头的字符串。（如：-i）
	_sValue：_OUT 参数值
	_sSrcParam：_IN_OUT 传入原始参数字符串，传出分割后，后续未分割的字符串
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
	//去掉头尾空格
	_sParamStr.Trim();
	if ( _sParamStr.GetLength() < 1 ) return FALSE;
	//第一个字符是否为“-”
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
				//非法格式
				_sParamStr.Empty();
				return FALSE;
			}

			//找到一个参数
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
功能：
	从参数集中分割出一个参数。以引号或者空格做个分割的标志。
参数：
	_sValue：_OUT 参数值
	_sParamStr：_IN_OUT 传入原始参数集，传出分割之后的参数集
*/
BOOL CCxCmdlineBase::SplitParamValue( CString& _sValue, CString& _sParamStr )
{
	int nPos = -1;
	CString sTmp;

	_sValue.Empty();
	//
	_sParamStr.Trim();
	if ( _sParamStr.GetLength() < 1 ) return FALSE;
	//第一个字符是否为“双引号”
	if ( _sParamStr.GetAt( 0 ) == '\"' )
	{
		//第一个字符是双引号
		nPos = _sParamStr.Find( '\"', 1 );
		if ( nPos == -1 )
		{
			//非法格式，双引号不匹配
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
		//第一个字符不是双引号
		nPos = _sParamStr.Find( ' ', 0 );
		if ( nPos == -1 )
		{
			if ( _sParamStr.GetLength() < 1 )
			{
				//非法格式
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
获取参数个数，
返回参数个数，如果异常返回-1.
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
		return -1;//不配对，返回错误
	}
}
