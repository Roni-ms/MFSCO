
#include <iostream>
#include <fstream>
#include<list>
#include<algorithm>
#include<vector>
#include<cmath>
#include <iterator>
#include <map>
#include<queue>
#include<string>
#include<numeric>
#include <stdlib.h>


using namespace std;



//writing a procedure that import data from sqlite database and process data in the form of arc:
//write a query that select state and required information for arc

map<int, Biomass> LoadBiomassInfo(string theFileName)
{
	ifstream theFile;
	theFile.open(theFileName.c_str());

	ofstream outputest("output_test_1.txt");



	int theID_ = 0;
	float Harvest_collect_=0;
	float field_storage_=0;
	float prep_depot_=0;
	float Handling_queing_depot_=0;
	float depot_storage_=0;
	float biorefinery_storage_=0;
	float handling_queing_refinery_ = 0;
	float blending_=0;
	float ch_content_=0;
	float ash_content_=0;
	float moisture_content_=0;
	float dml_=0;
	float transp_pellet_f_;
	float transp_pellet_v_;
	float dml_storage_;
	float dml_preprocessing_;


	map<int, Biomass>map_biomass;
	while (theFile >> theID_ >> Harvest_collect_ >> field_storage_ >> prep_depot_ >> Handling_queing_depot_ >> depot_storage_ >> biorefinery_storage_ >> handling_queing_refinery_ >> blending_ >> ch_content_ >> ash_content_ >>
		moisture_content_ >> dml_ >> transp_pellet_f_ >> transp_pellet_v_ >> dml_storage_ >> dml_preprocessing_)  //county id implies size of the biorefinery and the demand implies total cost
	{
		Biomass *a;
		a = new Biomass;
		a->Harvest_collect = Harvest_collect_;
		a->field_storage = field_storage_;
		a->prep_depot = prep_depot_;
		a->Handling_queing_depot = Handling_queing_depot_;
		a->depot_storage = depot_storage_;
		a->biorefinery_storage = biorefinery_storage_;
		a->handling_queuing_refinery = handling_queing_refinery_;
		a->blending = blending_;
		a->ch_content = ch_content_;
		a->ash_content = ash_content_;
		a->moisture_content = moisture_content_;
		a->dml = dml_;
		a->transp_pellet_f = transp_pellet_f_;
		a->transp_pellet_v = transp_pellet_v_;
		a->dml_storage = dml_storage_;
		a->dml_preprocessing = dml_preprocessing_;
		map_biomass[theID_] = *a;
		delete a;
	}
	theFile.close();
	map<int, Biomass>::iterator it;
	for (it = map_biomass.begin(); it != map_biomass.end(); it++)
	{
		outputest << it->first << "  " << it->second.Harvest_collect << "   " << it->second.ch_content<< endl;
	}
	return map_biomass;
}

vector<Arc> LoadDepotSize(string theFileName)
{
	ifstream theFile;
	theFile.open(theFileName.c_str());
	int thecountyID = 0;
	float capacity = 0;
	float  cost = 0;
	vector<Arc> DepotSize;
	while (theFile >> capacity >> cost)  //county id implies size of the biorefinery and the demand implies total cost
	{
		Arc *a;
		a = new Arc;
		a->loadProduction(capacity);
		a->loadArcCost(cost);
		DepotSize.push_back(*a);
		delete a;
	}
	theFile.close();
	return DepotSize;

}
void import_ILM_resource(string theFileName, vector<Arc> &allArcs, vector<Arc>&depotArcs, map<int, Arc> & mapcounty, float base_lat, float base_long, map<int, Biomass>&map_biomass, int base_loc, string theFileName2)
{
	ifstream theFile;
	theFile.open(theFileName2.c_str());
	//theFile.open("SOT_2019_resources.txt");

	ofstream output1("output1.txt");
	float dis_ = 0;
	int f_ = 0,fp = 0; //f=fips, type =feedstock type
	float y_ = 0, prod_ = 0, lat_ = 0, long_ = 0, area_ = 0;
	int state_id=0;

	int type__ = 0;

	float max_farmgate_price = 51;
	float mod_prod = 0;
	float increase_factor_sg = 10000;
	string count_name, state_name;

	float carb_, ash_;

	if (theFile.is_open())
	{
		while (theFile >> f_ >> type__ >> lat_ >> long_ >> area_ /*>> count_name >> state_name */>> fp >> y_ >> prod_ >> carb_ >> ash_)
		{
			
			if (fp == 40)
			{
				Arc *u;
				u = new Arc;
				u->loadStart(f_); //fips
				u->loadEnd(base_loc); //fips
				u->loadMid1(fp); //loading farmgate price
				u->loadArcType(20 + type__); //combination of feedstock and arc type , location from field to depot ( e.g 20+1 =depot to refinery biomass type =1 
				u->loadGroweryPayment(fp - base_data_gp); // farmgate-h&c cost
				u->loadSecArcType(2); // location 
				u->loadYield(y_);
				u->loadProduction(prod_);
				u->loadLatitude(lat_); //lat
				u->loadLongitude(long_); //long
				u->loadSecArcCost(sqrt(area_ / 3.14)); // calcuating draw radius of each county 
				u->loadArcCity(count_name);
				u->loadArcstate(state_name);
				u->ash = ash_;
				u->carb = carb_;

				dis_ = sqrt(69.1*69.1*(base_lat - lat_)*(base_lat - lat_) +
					53 * 53 * (base_long - long_)*(base_long - long_));
				if (f_ == base_loc)
				{
					u->loadArcCost(0); //distance from base location
				}
				else
				{
					u->loadArcCost(dis_); //distance from base location
				}

				depotArcs.push_back(*u); //this container is create t create arc from storage location to different counties 
				allArcs.push_back(*u);
				mapcounty[f_] = *u; //declared to track all county and initial depot 
				delete u;

			}
			

			Arc *u1; //creating arc type 0 and 1-4;
			u1 = new Arc;
			u1->loadStart(f_); //
			u1->loadEnd(f_);
			u1->loadMid1(fp); //loading farmgate price
			u1->loadArcType(type__); //feedstock type=1, location from field to depot : field to storage =1,2,3,4; storage to depot = 11,12,13,14,15; depot to biorefinery = 21,22,23,23,25;
			u1->loadGroweryPayment(fp - base_data_gp);
			u1->loadSecArcType(0); // field side
			u1->loadYield(y_);
			u1->loadProduction(prod_); //feedstock prd
			u1->loadLatitude(lat_); //lat
			u1->loadLongitude(long_); //long
			u1->loadSecArcCost(sqrt(area_ / 3.14)); // calcuating draw radius of each county 
			u1->loadArcCost(0); //distance from base location
			u1->loadArcCity(count_name);
			u1->loadArcstate(state_name);
			u1->ash = ash_;
			u1->carb = carb_;
			allArcs.push_back(*u1);
			//cout << "type__" << type__ << endl;
			delete u1;
		}
	}

	theFile.close();

	for (int i = 0; i < allArcs.size(); i++)
	{
		output1 << allArcs[i].getStart() << "   " << allArcs[i].getEnd() << "  " << allArcs[i].getArcType() << "   " << allArcs[i].getMid1() << "  " << allArcs[i].getProduction() << "   " << allArcs[i].getLatitude() << "   " << allArcs[i].getLongitude() << "    ";
		output1 << allArcs[i].getArcCost() << "    " << allArcs[i].getSecArcCost() << endl;
	}

}

void LoadArcs(vector<Arc> &allArcs, map<int, Arc> & mapcounty, vector<Arc>&countyArcs, vector<Arc>&depotArcs, vector<Arc>&Depotsize,map<int, Biomass>&map_biomass, string theFileName)
{
	
	int i, j, k;
	ofstream output("output.txt");


	float dis_ = 0, yld = 0, ocost = 0, en2 = 0;
	int st = 0, en = 0, prd = 0;


	import_ILM_resource("SOT_2019_resources.txt", allArcs, depotArcs, mapcounty, base_lat, base_long, map_biomass, base_loc, theFileName);

	for (int i = 0; i < allArcs.size(); i++)
	{
		output << allArcs[i].getStart() << "   " << allArcs[i].getEnd()<<"  "<< allArcs[i].getArcType() << "   " << allArcs[i].getMid1() << "  " << allArcs[i].getProduction() << "   " << allArcs[i].getLatitude() << "   " << allArcs[i].getLongitude()<<"    ";
		output << allArcs[i].getArcCost() << "    " << allArcs[i].getSecArcCost() << endl;
	}
		
	//creating arc from location to depot 

	
	map<int, Arc>::iterator it;
	for (it = mapcounty.begin(); it != mapcounty.end(); it++)
	{
		for (i = 0; i <depotArcs.size(); i++)
		{
			Arc *u;
			u = new Arc;
			u->loadStart(it->first);
			u->loadEnd(depotArcs[i].getStart());
			u->loadMid1(depotArcs[i].getMid1()); //loading farmgate price
			u->loadArcType(depotArcs[i].getArcType() - 10); //feedstock type=1, location from field to depot =1 ( e.g if 23 then 23-10 becomes 13 which is location 1 feedstock type 3) 
			u->loadGroweryPayment(it->second.getGrowerPayment());
			u->loadSecArcType(1); // 1 for field
			u->loadProduction(it->second.getProduction()); //feedstock prd
			u->loadYield(it->second.getYield());
			u->loadLatitude(it->second.getLatitude()); //lat
			u->loadLongitude(it->second.getLongitude()); //long
			u->loadSecArcCost(depotArcs[i].getSecArcCost()); // calcuating draw radius of each county
			u->loadArcCity(depotArcs[i].getCity());
			u->loadArcstate(depotArcs[i].getState());
			u->carb = depotArcs[i].carb;
			u->ash = depotArcs[i].ash;
			dis_ = sqrt(69.1*69.1*(it->second.getLatitude() - depotArcs[i].getLatitude())*(it->second.getLatitude() - depotArcs[i].getLatitude()) +
				53 * 53 * (it->second.getLongitude() - depotArcs[i].getLongitude())*(it->second.getLongitude() - depotArcs[i].getLongitude()));
			if (it->first == depotArcs[i].getStart())
			{
				u->loadArcCost(depotArcs[i].getSecArcCost());
			}
			else{
				u->loadArcCost(dis_); //distance from county centriod to depot centroid location:
			}
			allArcs.push_back(*u);
			countyArcs.push_back(*u); //loading arcs from county to depot only
			delete u;
		}
		//cout << it->first << " fips id" << endl;
	}
	
	


	cout << " end of preparing input  total   " <<allArcs.size()<<"   arcs" << endl;
	

} 

