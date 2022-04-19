
//using namespace std;
#include<string>
#include<list>
#include<map>
#include<vector>
#include <iostream>
//#include"county.h"
using namespace std;
class Arc
{
public:
	int ArcID;
	int start;
	int mid1;
	int end;
	float ArcCost;
	float SecArcCost;
	int ArcType;
	int sec_type;
	string arcOwner;
	list<int>path;
	float lat;
	float lng;
	int price_tag;
	map<int, map<int, float>>Farmgate_price;
	
	string textId;
	string state;
	string city;

	float growerpayment;
	float production;

	float production_2;

	float production_3;

	float production_4;


	float production_5;

	float production_6;

	float yield;
	float carb;
	float ash;
	Arc();
	~Arc();
    
	void loadStart(int a);

	void loadPrice_tag(int a);


	void loadMid1(int a);
	void loadEnd(int a);
	void loadArcCost(float a);

	void loadSecArcCost(float a);

	void loadArcID(int ID);
	void loadArcType(int Type);
	void loadSecArcType(int Type);
	void loadarcOwner(string arcO);
	void loadLatitude(float a);
	void loadLongitude(float a);
	void loadTextId(string txt);

	void loadGroweryPayment(float gp );

	void loadProduction(float pr);

	void loadProduction_2(float pr2);

	void loadProduction_3(float pr3);

	void loadProduction_4(float pr4);

	void loadProduction_5(float pr5);

	void loadProduction_6(float pr6);

	void loadYield(float yl);

	void loadArcCity(string city_);
	void loadArcstate(string state_);
		
	int getStart();

	int  getPrice_tag();
	int getMid1();
	int getEnd();
	float getArcCost();

	float getSecArcCost();


	int getArcID();
	int getArcType();
	int getSecArcType();

	string getarcOwner();
	float getLatitude();
	float getLongitude();
	string getTextId();

	float getGrowerPayment();

	float getProduction();

	float getProduction_2();
	float getProduction_3();

	float getProduction_4();
	float getProduction_5();

	float getProduction_6();

	float getYield();

	string getCity();
	string getState();

	
};

Arc::Arc() { }

Arc::~Arc( ) { }

void Arc::loadStart(int st)
{
	start = st;
}

void Arc::loadPrice_tag(int st)
{
	price_tag = st;
}

void Arc::loadMid1(int md)
{
	mid1 = md;
}

void Arc::loadEnd(int en)
{
	end = en;
}

void Arc::loadArcCost(float cap)
{
	ArcCost = cap;
}

void Arc::loadSecArcCost(float cap)
{
	SecArcCost = cap;
}

void Arc:: loadArcID( int aID)
{ 
	ArcID = aID;
}
void Arc::loadSecArcType(int Type)
{ 
	sec_type = Type;
}

void Arc::loadArcType(int Type)
{
	ArcType = Type;
}

void Arc:: loadarcOwner(string arcOw)
{ 
	arcOwner = arcOw;
}
void Arc::loadLatitude(float en)
{
	lat = en;
}

void Arc::loadLongitude(float cap)
{
	lng = cap;
}

void Arc::loadTextId(string txt1)
{
	textId = txt1;
}

void Arc::loadGroweryPayment(float gp)
{
	growerpayment = gp;
}

void Arc:: loadProduction(float pr)
{
	production = pr;
}

void Arc::loadProduction_2(float pr_2)
{
	production_2 = pr_2;
}

void Arc::loadProduction_3(float pr3)
{
	production_3 = pr3;
}

void Arc::loadProduction_4(float pr_4)
{
	production_4 = pr_4;
}


void Arc::loadProduction_5(float pr5)
{
	production_5 = pr5;
}

void Arc::loadProduction_6(float pr_6)
{
	production_6 = pr_6;
}

void Arc::loadYield(float yl)
{
	yield = yl;
}

void Arc::loadArcCity(string city_)
{
	city = city_;
}
void Arc::loadArcstate(string state_)
{
	state = state_;
}


int Arc::getStart()
{
	return start;
}

int Arc::getPrice_tag()
{
	return price_tag;
}
int Arc::getMid1()
{
	return mid1;
}


int Arc::getEnd()
{
	return end;
}

float Arc::getArcCost()
{
	return ArcCost;
}


float Arc::getSecArcCost()
{
	return SecArcCost;
}


int Arc::getArcID()
{
	return ArcID;
}
int Arc::getArcType()
{
	return ArcType;
}

int Arc::getSecArcType()
{
	return sec_type;
}

string Arc::getarcOwner()
{
	return arcOwner;
}
float Arc::getLatitude()
{
	return lat;
}

float Arc::getLongitude()
{
	return lng;
}

string Arc::getTextId()
{
	return textId;
}

float Arc::getGrowerPayment()
{
	return growerpayment;
}
float Arc::getProduction()
{
	return production;
}
float Arc::getProduction_2()
{
	return production_2;
}

float Arc::getProduction_3()
{
	return production_3;
}
float Arc::getProduction_4()
{
	return production_4;
}

float Arc::getProduction_5()
{
	return production_5;
}
float Arc::getProduction_6()
{
	return production_6;
}

float Arc::getYield()
{
	return yield;
}

string Arc::getState()
{
	return state;
}
string Arc::getCity()
{
	return city ;
}



