#pragma once
class CResLibData;
class Cmpare
{
public:
	Cmpare(void);
	~Cmpare(void);
public:
	bool operator()(const CResLibData* st1,const CResLibData*  st2) const;

};

