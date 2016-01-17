#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <Guiddef.h>
//typedef struct _GUID
//{
//    unsigned long Data1;
//    unsigned short Data2;
//    unsigned short Data3;
//    unsigned char Data4[8];
//} GUID, UUID;

namespace ReslibMgr

{

GUID CreateGuid();

std::string GuidToString(const GUID &guid);

}