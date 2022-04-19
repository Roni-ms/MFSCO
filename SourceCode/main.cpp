

#include <iostream>
#include "Arc.h" 
#include "Biomass_info.h"
#include "Input_Data.h"

//#include "Optimization Model_SOT_2020_distributed.h"
#include "Optimization Model_multi_period.h"

//#include "ImportData_SOT_2020_distributed.h"
#include "Import_multi_period_data.h"


#include<cmath>
#include<fstream>
#include <ctime>
using namespace std;


int main(int argc, const char * argv[])
{
	
	float start_s = clock();
	
	int i;
	int j;
	ofstream outputarc("generatedArct.txt");
	ofstream outputnode("generatedNodet.txt");
	ofstream output_2("print_result.txt");
	map<int, Biomass>map_biomass;
	map_biomass = LoadBiomassInfo("C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/Biomass_info.txt");

	//vector<Arc>ILMCS =loadILMBiomass_CS("ILM_CS.txt"); //only for 2022 case

	vector<Arc> Depotsize = LoadDepotSize("C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/Depot_size.txt");
	
	map<int, Biomass>::iterator it;
	/*for (it = map_biomass.begin(); it != map_biomass.end(); it++)
	{
		outputarc << it->first << "  " << it->second.Harvest_collect << "  " << it->second.field_storage << "  " << it->second.prep_depot << "  " << it->second.Handling_queing_depot << "  " << it->second.depot_storage <<"  ";
		outputarc << it->second.biorefinery_storage << "  " << it->second.handling_queuing_refinery<<"  "<< it->second.blending << "  " << it->second.ch_content << "  " << it->second.ash_content << "  " << it->second.moisture_content << "  " << it->second.dml << endl;
	}*/
	

	/*for (i = 0; i < Depotsize.size(); i++)
	{
		outputarc << Depotsize[i].getProduction() << "  " << Depotsize[i].getArcCost() << endl;
	}*/
	map<int, Arc> mapcounty;
	vector<Arc> allArcs; //Here I declare a vector of Arcs
	vector<Arc>countyArcs;
	vector<Arc>depotArcs;
	map<int, map<int, map<int, map<int, int>>>>county_production;

	map<int, vector<Arc>>Multi_period_allArcs;
	map<int, vector<Arc>>Multi_period_countyArcs;
	map<int, vector<Arc>>Multi_period_depotArcs;

	
	int year = 2019;
	for (int i = 0; i < period; i++)
	{
		year--;
		allArcs.clear();
		mapcounty.clear();
		countyArcs.clear();
		depotArcs.clear();
		county_production.clear();

		if (i == 0)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize,  map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2019_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;


			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production,year, output_2); 
		}
		if (i == 1)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize,  map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2018_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2); 
		}
		if (i == 2)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2017_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2);
		}
		if (i == 3)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize,  map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2016_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2);
		}
		if (i == 4)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2015_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2);
		}
		if (i == 5)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2014_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2); 
		}
		if (i == 6)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize,  map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2013_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2); 
		}
		if (i == 7)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2012_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2); 
		}
		if (i == 8)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2011_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2); 
		}
		if (i == 9)
		{
			LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, "C:/Users/RONIMS/Documents/C++ project/Multi_period_depot_opt/Multi_period_depot_opt/SOT_2010_resources.txt");
			Multi_period_allArcs[i] = allArcs;
			Multi_period_countyArcs[i] = countyArcs;
			Multi_period_depotArcs[i] = depotArcs;
			//CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production, year, output_2); 
		}
	}

	//outputarc << " allArcs.size() is " << allArcs.size() << endl;
	CPLEXProgram_multi_period(Multi_period_allArcs, mapcounty, Multi_period_countyArcs, Multi_period_depotArcs, Depotsize, map_biomass, year, output_2);
	
	int stop_s = clock();

   cout << "time: in seconds " << (stop_s-start_s)/double(CLOCKS_PER_SEC) << endl;
	//cout<<'\a'<<endl;
	//cout<<'\a'<<endl;
	//cout<<'\a'<<endl;
	//cout<<'\a'<<endl;
 
   return 0;
}

