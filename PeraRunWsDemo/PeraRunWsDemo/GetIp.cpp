#include "stdafx.h"
#include "csharedmemory.h"
#define SM_LOCAL_IP_LENGTH 256
CString GetLocalIP()
{
	//return "192.168.114.156";
	CString sHostName,sIPAddress;
	WORD    wVersionRequested;
	WSADATA wsaData;
	int     err;
	char szHostName[256];   
	int  nRetCode;   

	wVersionRequested = MAKEWORD( 2, 0 );    
	err = WSAStartup( wVersionRequested, &wsaData );   
	if ( err != 0 ) 
	{    
		return "";   
	}    

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 ) 
	{   
		WSACleanup( );   
		return "";    
	}

	nRetCode = gethostname(szHostName, sizeof(szHostName));   

	if (nRetCode != 0) 
	{   
		sHostName = (""); 
		return "";   
	}   

	sHostName = szHostName;

	struct hostent FAR *lpHostEnt = gethostbyname (sHostName);   

	if (lpHostEnt == NULL) 
	{   
		sIPAddress = _T("");
		return sIPAddress;
	}   

	LPSTR lpAddr = lpHostEnt->h_addr_list[0];   
	if (lpAddr) 
	{   
		struct in_addr  inAddr;   
		memmove (&inAddr, lpAddr, 4);   
		sIPAddress = inet_ntoa (inAddr);   
		if (sIPAddress.IsEmpty())   
			sIPAddress = _T("");   
	}

	nRetCode = WSACleanup();  

	if (nRetCode != 0) 
	{ 
		return "";   
	}   

	return sIPAddress;   
}
CString ComGetLocalIp()
{
	CString sIp("");
	CSharedMemory* pSmIp = NULL;
	pSmIp = new CSharedMemory("SM_LOCAL_IP", SM_LOCAL_IP_LENGTH);
	pSmIp->Lock();
	sIp = (char*)pSmIp->GetData();
	pSmIp->Unlock();
	delete pSmIp;
	pSmIp = NULL;

	if (sIp.IsEmpty())
	{
		sIp = GetLocalIP();
	}

	return sIp;
}