#include "StdAfx.h"
#include "Cmpare.h"
#include <ResLibData.h>


Cmpare::Cmpare(void)
{
}
Cmpare::~Cmpare(void)
{
}
bool Cmpare::operator()(const CResLibData* st1,const CResLibData*  st2) const 
{	
	return   st1->m_sName < st2->m_sName; 
}