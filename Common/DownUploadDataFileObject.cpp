#include "StdAfx.h"
#include "DownUploadDataFileObject.h"
#include "Tools.h"
#include "DownUploadCommonFunc.h"
#include "FileFunc.h"
CDownUploadDataFileObject::CDownUploadDataFileObject()
{
	m_strHproseRpc.clear();
	m_strFilePath.clear();
	m_strFileName.clear();
	m_strMd5.clear();
	m_strFileId.clear();
	m_strIndex.clear();
	m_strLastModifyTime.clear();
	m_strMsg.clear();
	m_nFileSize = 0;
	m_nFileSizeUploaded = 0;
}

BOOL CDownUploadDataFileObject::CheckInfo(BOOL bMd5/* = TRUE*/)
{
	if (m_strFilePath.empty() || !PathFileExists(m_strFilePath.c_str()))
	{
		m_strMsg = "�ļ�·���쳣";
		return FALSE;
	}
	if (IsAccessibleDir(m_strFilePath.c_str()))
	{
		m_strMsg = "ָ��·�������ļ�";
		return FALSE;
	}

	if (m_strHproseRpc.empty())
	{
		m_strHproseRpc = GetHproseRpc();
		if (m_strHproseRpc.empty())
		{
			m_strHproseRpc = GetHproseRpcEx();
			if (m_strHproseRpc.empty())
			{
				m_strMsg = "δȡ��HPROSERPC���õ�ַ";
				return FALSE;
			}
		}
	}

	std::replace(m_strFilePath.begin(), m_strFilePath.end(), '/', '\\');

	if (m_strFileName.empty())
	{
		m_strFileName = m_strFilePath.substr(m_strFilePath.find_last_of('\\') + 1, m_strFilePath.length() - m_strFilePath.find_last_of('\\') - 1);	
	}

	if (m_strIndex.empty())
	{
		m_strIndex = GuidToString(CreateGuid());
	}

	if (m_nFileSize == 0 || m_strLastModifyTime.empty())
	{
		LONGLONG iFileSize  = FileSize(m_strFilePath.c_str());
		std::string strLastTime = GetFileLastModifyTime(m_strFilePath);
		if (iFileSize < 0 || strLastTime.empty())
		{
			m_strMsg = "ȡ�ļ�������Ϣʧ��";
			return FALSE;
		}

		m_nFileSize = iFileSize;
		m_strLastModifyTime = strLastTime;
	}

	if (m_strMd5.empty() && bMd5 == TRUE)
	{
		m_strMd5 = GetFileMd5(m_strFilePath);
	}

	return TRUE;
}