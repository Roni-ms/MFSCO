

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include<map>
#include<cmath>
#include<list>
#include <ilcplex/ilocplex.h>
using namespace std;

typedef IloArray<IloIntVarArray> Arcs;
typedef IloArray<IloNumVarArray> ArcsL;

void createMainModel(IloModel model, ArcsL x, Arcs z, Arcs w, ArcsL x_c, IloNumVarArray carb_purchase, map<int, vector<Arc>> &Multi_period_allArcs, map<int, Arc> &mapcounty, 
	map<int, vector<Arc>> &Multi_period_countyArcs, map<int, vector<Arc>> &Multi_period_depotArcs,
	vector<Arc>&Depot_c_cost, vector<Arc>&CountyInfo, map<int, Biomass>&map_biomass, map<int, map<int, map<int, int>>>&map_deopt_arcs);




void CPLEXProgram_multi_period(map<int, vector<Arc>> &Multi_period_allArcs, map<int, Arc> & mapcounty, map<int, vector<Arc>>&Multi_period_countyArcs, map<int, vector<Arc>>&Multi_period_depotArcs,
	vector<Arc>&Depot_c_cost, map<int, Biomass>&map_biomass,int year, ofstream &output_2)
{
	
	IloEnv env;
	try {

		int i, j, k;

		
		output_2 << "\n";
		output_2 << "\n";
		output_2 << "\n";
		output_2 <<"******************************************"<< year << "******************************************" << endl;

		map<int, Arc>::iterator itw;
		vector<Arc>CountyInfo;

		//keep tracking county id for using in constraints in the model;
		for (itw = mapcounty.begin(); itw != mapcounty.end(); ++itw)
		{
			Arc *a;
			a = new Arc;
			a->loadArcID(itw->first);
			CountyInfo.push_back(*a);
			delete a;
		}
		map<int, map<int, map<int,int>>>map_deopt_arcs;//track x_c arc
		for (i = 0; i < Multi_period_depotArcs[1].size(); i++) //keep track the index of depot arcs 
		{
			map_deopt_arcs[Multi_period_depotArcs[1][i].getStart()][Multi_period_depotArcs[1][i].getEnd()][Multi_period_depotArcs[1][i].getArcType()] = i; //map_deopt_arcs[fips][fips][depot-refinery e.g. 2-1][farmgate price]
		}

		//declaring variables
		IloModel model(env);
		ArcsL x(env, period);
		for (int p = 0; p < period; p++)
		{
			x[p] = IloNumVarArray(env, Multi_period_allArcs[1].size(), 0, IloInfinity, ILOFLOAT);
		}

		ArcsL x_c(env, period);
		for (int p = 0; p < period; p++)
		{
			x_c[p] = IloNumVarArray(env, Multi_period_depotArcs[1].size(), 0, IloInfinity, ILOFLOAT);
		}
		//IloNumVarArray x(env, allArcs.size(), 0, IloInfinity, ILOFLOAT); //flow variable

		Arcs z(env, period);
		for (int p = 0; p < period; p++)
		{
			z[p] = IloIntVarArray(env, Multi_period_allArcs[1].size(), 0, 1);
		}

		//IloNumVarArray x_c(env, depotArcs.size(), 0, IloInfinity, ILOFLOAT); //flow variable carbohydate from depot
		//IloNumVarArray z(env, allArcs.size(), 0, 1, ILOINT); //binary variable to select price at a supply curve 
		Arcs w(env, CountyInfo.size()); //depot locaiton variable

		IloNumVarArray carb_purchase(env, period, 0, IloInfinity, ILOFLOAT);

		for (i = 0; i < CountyInfo.size(); i++)
		{
			w[i] = IloIntVarArray(env, Depot_c_cost.size(), 0, 1); //depot capacity range ;
		}

		cout << "End of declaring decision varaibles" << endl;


		//*************** Writing Economic objective *******************************************
		//need update cost in the arc:
		IloExpr obj(env);
		IloExpr rhs(env);

		IloExpr obj_ash(env);

		//float ash_disp_cost =37.16;

		//  minimization  problem
		
		for (int t=0; t<period; t++)
		{
			for (j = 0; j < Multi_period_allArcs[t].size(); j++)
			{
				if (Multi_period_allArcs[t][j].getArcType() == 1)
				{
					obj += x[t][j] * (Multi_period_allArcs[t][j].getGrowerPayment() + map_biomass[1].Harvest_collect*base_yield_3P / Multi_period_allArcs[t][j].getYield() + map_biomass[1].field_storage);
				}
				if (Multi_period_allArcs[t][j].getArcType() == 2)
				{
					obj += x[t][j] * (Multi_period_allArcs[t][j].getGrowerPayment() + 
						map_biomass[2].Harvest_collect*base_yield_2P / Multi_period_allArcs[t][j].getYield() / Two_pass_resource_factor + map_biomass[2].field_storage);
				}
				

				if (Multi_period_allArcs[t][j].getArcType() == 11)
				{
					obj += x[t][j] * (DFC_truck + Multi_period_allArcs[t][j].getArcCost()* DVC_truck) + x[t][j] * (map_biomass[1].prep_depot 
						+ map_biomass[1].depot_storage + map_biomass[1].Handling_queing_depot);
				}
				if (Multi_period_allArcs[t][j].getArcType() == 12)
				{
					obj += x[t][j] * (DFC_truck + Multi_period_allArcs[t][j].getArcCost()* DVC_truck) + x[t][j] * (map_biomass[2].prep_depot + 
						map_biomass[2].depot_storage + map_biomass[2].Handling_queing_depot);
				}
				

				if (Multi_period_allArcs[t][j].getArcType() == 21)
				{
					obj += x[t][j] * (map_biomass[1].transp_pellet_f + Multi_period_allArcs[t][j].getArcCost()* map_biomass[1].transp_pellet_v + 
						map_biomass[1].biorefinery_storage + map_biomass[1].handling_queuing_refinery + map_biomass[1].blending);
					obj_ash += x[t][j] * (Multi_period_allArcs[t][j].ash - a)*ash_disp_cost;
				}
				if (Multi_period_allArcs[t][j].getArcType() == 22)
				{
					obj += x[t][j] * (map_biomass[2].transp_pellet_f + Multi_period_allArcs[t][j].getArcCost()* map_biomass[2].transp_pellet_v + 
						map_biomass[2].biorefinery_storage + map_biomass[2].handling_queuing_refinery + map_biomass[2].blending);
					obj_ash += x[t][j] * (Multi_period_allArcs[t][j].ash - a)*ash_disp_cost;
				}
				
			}
			obj += carb_purchase[t] * carb_dockage;//adding carbohydrate dockage in the objective function
		}
		
		obj = obj + obj_ash;
		obj = obj / period;
		//determining construction cost of depot 

		for (i = 0; i < mapcounty.size(); i++)
		{
			for (j = 0; j < Depot_c_cost.size(); j++)
			{
				obj += w[i][j] * Depot_c_cost[j].getProduction()* Depot_c_cost[j].getArcCost(); //depot construction cost 
			}
		}

		
		cout << "End of writing objective function" << endl;

		//adding ash disposal cost in the objective function;
		

		createMainModel(model, x, z, w, x_c,carb_purchase, Multi_period_allArcs, mapcounty, Multi_period_countyArcs, Multi_period_depotArcs, Depot_c_cost, CountyInfo, map_biomass,  map_deopt_arcs);




		cout << "End of creating main model " << endl;

		//////*****************end of creating basic model****************************************************

		

		IloObjective objective(env);

		objective = IloAdd(model, IloMinimize(env, obj));

		IloCplex cplex (env);
		cplex.extract(model);

		cplex.exportModel("Multi_period_depot_blending_model.lp");

		/*cplex.setParam(IloCplex::Param::Benders::Strategy,
			IloCplex::BendersFull);*/

		
		cplex.solve();
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
		{
			output_2 << "modify model and generate result with penalty cost" << endl;
			exit;
		}

		
		//for (i = 0; i < depotArcs.size(); i++) //print_result(cplex, x, y, allArcs, CountyInfo, output_2);
		//	if (cplex.getValue(x_c[i]) > .01)
		//	{
		//		output_2 << i << "  " << depotArcs[i].getStart() << "   " << depotArcs[i].getEnd() << "  " << depotArcs[i].getArcType() << "   "<< depotArcs[i].getMid1() << "   "<< cplex.getValue(x_c[i]) << "  " << endl;
		//	}

		//output_2 << "supply chain network" << endl;
		//for (i = 0; i < allArcs.size(); i++)
		//if (cplex.getValue(x[i])>.01)
		//{
		//	output_2 << i << "  " << allArcs[i].getStart() << "   " << allArcs[i].getEnd() << "  " << allArcs[i].getArcType() << "   ";
		//	output_2 << allArcs[i].getSecArcType() << "  " << allArcs[i].getGrowerPayment() << "  " << allArcs[i].getProduction() << "   " << allArcs[i].getYield() << "  " << allArcs[i].getLatitude() << "   " << allArcs[i].getLongitude() << "    ";
		//	output_2 << allArcs[i].getArcCost() << "    " << allArcs[i].getSecArcCost() << "  " <<"   "<< allArcs[i].carb << "   " << allArcs[i].ash <<"  "<< cplex.getValue(x[i]) <<"  "<< endl;

		//}

		float depotcost = 0;;
		float depot_total_size = 0;
		output_2 << "Depot location " << endl;
		for (i = 0; i < CountyInfo.size(); i++)
		{
			for (j = 0; j<Depot_c_cost.size(); j++)
			if (cplex.getValue(w[i][j])>.01)
			{
				output_2 << CountyInfo[i].getArcID() << "  " << Depot_c_cost[j].getProduction() << "   " <<  endl;
				depotcost = depotcost + Depot_c_cost[j].getProduction()*Depot_c_cost[j].getArcCost();
				depot_total_size = depot_total_size + Depot_c_cost[j].getProduction();
			}
		}

		
		//double total_cost = 0, total_biomass = 0, delivered_feedstock_cost = 0;

		//
		output_2 << "Average depot cost " << depotcost / depot_total_size << endl;
		output_2 << "delivered cost using objective function objective value " << cplex.getObjValue() << endl;

		//
	


		////creating blending table 
		//double fed1 = 0, fed2 = 0, fed3 = 0, fed4 = 0, fed5 = 0;
		//double del_cost1 = 0, del_cost2 = 0, del_cost3 = 0, del_cost4 = 0, del_cost5 = 0;
		//double g1 = 0, g2 = 0, g3 = 0, g4 = 0, g5 = 0;


		//double fed1t = 0, fed2t = 0, fed3t = 0, fed4t = 0, fed5t = 0;
		//double t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0; //transportation cost
		//double d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0; //distance * ton



		//double fed1_ = 0, fed2_ = 0, fed3_ = 0, fed4_ = 0, fed5_ = 0;
		//double t1_ = 0, t2_ = 0, t3_ = 0, t4_ = 0, t5_ = 0; //transportation cost
		//double d1_ = 0, d2_ = 0, d3_ = 0, d4_ = 0, d5_ = 0; //distance * ton


		//double average_3P_yield_sum=0;
		//double average_2P_yield_sum=0;

		//double total_carb1 = 0, total_carb2 = 0, total_carb3 = 0, total_carb4 = 0;

		//for (int j = 0; j < allArcs.size(); j++)
		//if (cplex.getValue(x[j])>.01)
		//{
		//	if (allArcs[j].getArcType() == 1)
		//	{
		//		fed1 = fed1 + cplex.getValue(x[j]);
		//		g1 = g1 + cplex.getValue(x[j]) * (allArcs[j].getGrowerPayment());
		//		average_3P_yield_sum = average_3P_yield_sum + cplex.getValue(x[j])*allArcs[j].getYield();
		//	}
		//	if (allArcs[j].getArcType() == 2)
		//	{
		//		fed2 = fed2 + cplex.getValue(x[j]);
		//		g2 = g2 + cplex.getValue(x[j]) * (allArcs[j].getGrowerPayment());
		//		average_2P_yield_sum = average_2P_yield_sum + cplex.getValue(x[j])*allArcs[j].getYield();
		//	}

		//	if (allArcs[j].getArcType() == 3)
		//	{
		//		fed3 = fed3 + cplex.getValue(x[j]);
		//		g3 = g3 + cplex.getValue(x[j]) * (allArcs[j].getGrowerPayment());
		//	}
		//	if (allArcs[j].getArcType() == 11)
		//	{
		//		fed1t = fed1t + cplex.getValue(x[j]);
		//		t1 = t1 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
		//		d1 = d1 + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 

		//	}
		//	if (allArcs[j].getArcType() == 12)
		//	{
		//		fed2t = fed2t + cplex.getValue(x[j]);
		//		t2 = t2 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
		//		d2 = d2 + cplex.getValue(x[j]) * allArcs[j].getArcCost();
		//	}

		//	if (allArcs[j].getArcType() == 13)
		//	{
		//		fed3t = fed3t + cplex.getValue(x[j]);
		//		t3 = t3 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
		//		d3 = d3 + cplex.getValue(x[j]) * allArcs[j].getArcCost();
		//	}

		//	//summaring type2 arc
		//	if (allArcs[j].getArcType() == 21)
		//	{
		//		fed1_ = fed1_ + cplex.getValue(x[j]);
		//		t1_ = t1_ + cplex.getValue(x[j]) * ((map_biomass[1].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[1].transp_pellet_v));
		//		d1_ = d1_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
		//		total_carb1 = total_carb1 + cplex.getValue(x_c[map_deopt_arcs[allArcs[j].getStart()][allArcs[j].getEnd()][allArcs[j].getArcType()]]);
		//		cout << allArcs[j].getStart() <<"  "<<allArcs[j].getEnd() << "  " << allArcs[j].getArcType() <<"   " << map_deopt_arcs[allArcs[j].getStart()][allArcs[j].getEnd()][allArcs[j].getArcType()] << "   ";
		//		cout<< cplex.getValue(x_c[map_deopt_arcs[allArcs[j].getStart()][allArcs[j].getEnd()][allArcs[j].getArcType()]])<< endl;

		//	}
		//	if (allArcs[j].getArcType() == 22)
		//	{
		//		fed2_ = fed2_ + cplex.getValue(x[j]);
		//		t2_ = t2_ + cplex.getValue(x[j]) * ((map_biomass[2].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[2].transp_pellet_v));
		//		d2_ = d2_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
		//		total_carb2 = total_carb2 + cplex.getValue(x_c[map_deopt_arcs[allArcs[j].getStart()][allArcs[j].getEnd()][allArcs[j].getArcType()]]);
		//		cout << allArcs[j].getStart() <<"  "<<allArcs[j].getEnd() << "  " << allArcs[j].getArcType() <<"   " << map_deopt_arcs[allArcs[j].getStart()][allArcs[j].getEnd()][allArcs[j].getArcType()] << "   ";
		//		cout<< cplex.getValue(x_c[map_deopt_arcs[allArcs[j].getStart()][allArcs[j].getEnd()][allArcs[j].getArcType()]])<< endl;
		//	}

		//	if (allArcs[j].getArcType() == 23)
		//	{
		//		fed3_ = fed3_ + cplex.getValue(x[j]);
		//		t3_ = t3_ + cplex.getValue(x[j]) * ((map_biomass[3].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[3].transp_pellet_v));
		//		d3_ = d3_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
		//		total_carb3 = total_carb3 + cplex.getValue(x[j])*allArcs[j].carb;
		//	}

		//	if (allArcs[j].getArcType() == 25)
		//	{
		//		fed5_ = fed5_ + cplex.getValue(x[j]);
		//		t5_ = t5_ + cplex.getValue(x[j]) * ((map_biomass[5].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[5].transp_pellet_v));
		//		d5_ = d5_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
		//	}


		//}

		//float avg_yield_3P = 0;
		//float avg_yield_2P = 0;
		//avg_yield_3P = average_3P_yield_sum / fed1;
		//avg_yield_2P = average_2P_yield_sum / fed2;

		//float avg_carb_dockage = cplex.getValue(carb_purchase) * carb_dockage / total_fed_demand;
		//cout << " carbohydrate dockage is " << cplex.getValue(carb_purchase) << "   " << cplex.getValue(carb_purchase) * carb_dockage / total_fed_demand << endl;
		//cout<<"avg_yield_3P and avg_yield_2P is " <<avg_yield_3P<<"   "<<avg_yield_2P<<endl;


		//f1_other = /*map_biomass[1].Harvest_collect*base_yield_3P/avg_yield_3P +*/ map_biomass[1].field_storage + map_biomass[1].prep_depot + map_biomass[1].Handling_queing_depot + map_biomass[1].depot_storage + map_biomass[1].biorefinery_storage
		//	+ map_biomass[1].handling_queuing_refinery + map_biomass[1].blending;
		//f2_other =/* map_biomass[2].Harvest_collect*base_yield_2P/(avg_yield_2P*Two_pass_resource_factor) + */map_biomass[2].field_storage + map_biomass[2].prep_depot + map_biomass[2].Handling_queing_depot + map_biomass[2].depot_storage + map_biomass[2].biorefinery_storage
		//	+ map_biomass[2].handling_queuing_refinery + map_biomass[2].blending;
		//f3_other = /*map_biomass[3].Harvest_collect +*/ map_biomass[3].field_storage + map_biomass[3].prep_depot + map_biomass[3].Handling_queing_depot + map_biomass[3].depot_storage + map_biomass[3].biorefinery_storage
		//	+ map_biomass[3].handling_queuing_refinery + map_biomass[3].blending;
		//f4_other = /*map_biomass[4].Harvest_collect +*/ map_biomass[4].field_storage + map_biomass[4].prep_depot + map_biomass[4].Handling_queing_depot + map_biomass[4].depot_storage + map_biomass[4].biorefinery_storage
		//	+ map_biomass[4].handling_queuing_refinery + map_biomass[4].blending;
		//f5_other = /*map_biomass[5].Harvest_collect +*/ map_biomass[5].field_storage + map_biomass[5].prep_depot + map_biomass[5].Handling_queing_depot + map_biomass[5].depot_storage + map_biomass[5].biorefinery_storage
		//	+ map_biomass[5].handling_queuing_refinery + map_biomass[5].blending;
		//output_2 << "main resutls " << endl;


		//cout << map_biomass[2].Harvest_collect << "   " << map_biomass[2].Harvest_collect*base_yield_2P / avg_yield_2P / Two_pass_resource_factor << endl;

		//output_2 << "blended table " << endl;

		//output_2 << fed1 << "  " << g1 / fed1 << "   " << map_biomass[1].Harvest_collect*base_yield_3P / avg_yield_3P<<"  "<< t1 / fed1t << "  " << d1 / fed1t << "  " << fed1t << "   " << t1_ / fed1_ << "  " << d1_ / fed1_ << "   " << fed1_ << "   " << depotcost / depot_total_size << "  ";
		//output_2 <<  f1_other <<"  "<< avg_carb_dockage << "  " << total_carb1/ fed1_ << "  " << a1 << "   " << map_biomass[1].moisture_content << "    " << map_biomass[1].dml << endl;

		//output_2 << fed2 << "  " << g2 / fed2 <<  "   " << map_biomass[2].Harvest_collect*base_yield_2P /avg_yield_2P/Two_pass_resource_factor<<"   "<< t2 / fed2t <<"   "<< d2 / fed2t << "  " << fed2t << "   " << t2_ / fed2_ << "  " << d2_ / fed2_ << "   " << fed2_ << "   " << depotcost / depot_total_size << "  ";
		//output_2 << f2_other << "  "<< avg_carb_dockage <<"  "<<total_carb2/ fed2_ << "  " << a2 << "  " << map_biomass[2].moisture_content << "    " << map_biomass[2].dml << endl;
		//output_2 << 0 << "  " << 0 << "   " << 0 << "  " << 0<<"  "<< 0 << "  " << 0 << "   " << 0<< "  " << 0 << "   " << 0 << "   " << 0 << "  ";
		//output_2 <<0<<"  "<< 0 << "  " <<0<< "  " << 0 << "  " << 0 << "    " << 0 << endl;
		//// output_2 << fed4 << "  " << g4 / fed4 << "   " << t4 / fed4 << "  " << f4_other << "  " << del_cost4 / fed4 + f4_other << "  " << c4 << "  " << a4 << "  " << d4 / fed4 << endl;

		////output_2 << 0 << "  " << 0 << "  " <<0 << "  " <<0 << endl;
		//output_2 << 0 << "  " << 0 << "   " << 0 << "  " << 0 << "  " << 0 << "  " << 0 << "   " << 0 << "  " << 0 << "   " << 0 << "   " << 0 << "  ";
		//output_2 << 0 << "  " << 0 << "  " << 0 << "  " << 0 << "  " << 0 << "    " << 0 << endl;
		////output_2 << 5 << "  " << 0 << "  " << 0 << "  " << 0 << "  " << 0 << endl;

		////printing county supply and used supply:

		////create table 2: 
		//output_2 << "table 2 for field location " << endl;
		// for (i = 0; i < allArcs.size(); i++)
		// if (cplex.getValue(x[i])>.01)
		// {
		//	 if (allArcs[i].getArcType() == 1 )
		//	 {
		//		 output_2 << allArcs[i].getStart() << "   " << allArcs[i].getMid1() <<"    "<< "3P-CS" << "   " << allArcs[i].getProduction()<<"   "<< cplex.getValue(x[i]) << "   " << allArcs[i].getYield() << endl;
		//	 }
		//	 if (allArcs[i].getArcType() == 2)
		//	 {
		//		 output_2 << allArcs[i].getStart() << "   " << allArcs[i].getMid1() <<"   "<< "2P-CS" << "   " << allArcs[i].getProduction() << "   " << cplex.getValue(x[i]) << "   " << allArcs[i].getYield() << endl;
		//	 }
		//	 if (allArcs[i].getArcType() == 3 )
		//	 {
		//		 output_2 << allArcs[i].getStart() << "   " << allArcs[i].getMid1() <<"   "<< "SG" << "   " << allArcs[i].getProduction() << "   " << cplex.getValue(x[i]) << "   " << allArcs[i].getYield() << endl;
		//	 }
		//	 if (allArcs[i].getArcType() == 25 )
		//	 {
		//		 output_2 << allArcs[i].getStart() << "   " << 10 <<"  "<< "GC" << "   " << msw_prd << "   " << fed5_ / (1 - map_biomass[5].dml) << "   " << "n/a" << endl;
		//	 }
		// }


	

	



	}

	catch(IloException& e) {
	cerr  << " ERROR: " << e << endl;   
	}
	catch(...) {
	cerr  << " ERROR" << endl;   
	}

	env.end();
}








void createMainModel(IloModel model, ArcsL x, Arcs z, Arcs w, ArcsL x_c, IloNumVarArray carb_purchase, map<int, vector<Arc>> &Multi_period_allArcs, map<int, Arc> &mapcounty,
	map<int, vector<Arc>> &Multi_period_countyArcs, map<int, vector<Arc>> &Multi_period_depotArcs,
	vector<Arc>&Depot_c_cost, vector<Arc>&CountyInfo, map<int, Biomass>&map_biomass, map<int, map<int, map<int, int>>>&map_deopt_arcs)
{
	int i,j,k=0;
	IloEnv env =model.getEnv();
	ofstream output_3("debug_result.txt");
	
	char *VarName;
	VarName=new char[200];
	for (auto &t : Multi_period_allArcs)
	{
		for (i = 0; i < Multi_period_allArcs[t.first].size(); i++)
		{
			sprintf(VarName, "x[%d][%d]", t.first, i);
			x[t.first][i].setName(VarName);
		}

		for (i = 0; i < Multi_period_depotArcs[t.first].size(); i++)
		{
			sprintf(VarName, "x_c[%d][%d]",t.first, i);
			x_c[t.first][i].setName(VarName);
		}

		for (i = 0; i < Multi_period_allArcs[t.first].size(); i++)
		{
			sprintf(VarName, "z[%d][%d]", t.first, i);
			z[t.first][i].setName(VarName);
		}
	}
	
	
	for (i = 0; i < CountyInfo.size(); i++)
	{
		for (j = 0; j<Depot_c_cost.size(); j++)
		{
			sprintf(VarName, "w[%d][%d]", i, j);
			w[i][j].setName(VarName);
		}
	}

	delete [] VarName;

	
	cout<<" End of declaring printing variable  "<<endl;
	
	//constraint 2, 10,11 (supply constraints, carbo demand, and biomass demand constraint )


	
	for (int t = 0; t < period; t++)
	{
		IloExpr totalcarbo(env); //feedstock type 1
		IloExpr totalfeedstock(env);
		for (int i = 0; i < Multi_period_allArcs[t].size(); i++)
		{
			if (Multi_period_allArcs[t][i].getSecArcType() == 0)
			{
				model.add(x[t][i] <= z[t][i] * Multi_period_allArcs[t][i].getProduction()); //constraint 1 
			}

			if (Multi_period_allArcs[t][i].getArcType() == 21) //three pass
			{
				totalcarbo += x_c[t][map_deopt_arcs[Multi_period_allArcs[t][i].getStart()][Multi_period_allArcs[t][i].getEnd()][Multi_period_allArcs[t][i].getArcType()]];
				totalfeedstock += x[t][i];
			}
			if (Multi_period_allArcs[t][i].getArcType() == 22) //two pass stover
			{
				totalcarbo += x_c[t][map_deopt_arcs[Multi_period_allArcs[t][i].getStart()][Multi_period_allArcs[t][i].getEnd()][Multi_period_allArcs[t][i].getArcType()]];
				totalfeedstock += x[t][i];
			}

		}

		model.add(totalcarbo + carb_purchase[t] >= c * total_fed_demand); //constraint total ch demand
		model.add(totalfeedstock == total_fed_demand); //total biomass demand
		totalcarbo.end();
		totalfeedstock.end();
	} 


	cout << " end of loading  constraitns 2, 10 and 11 " << endl;
	//constraint 2 
	//this loop make has multiple set of constraints: for each period 1)  select exactly one farmgate price 2)biomass coming in field storage from harvesting site = sum of going out to different depot from filed storage 
	for (int t = 0; t < period; t++)
	{
		for (i = 0; i < CountyInfo.size(); i++)
		{
			IloExpr helper01(env); //feedstock type 1
			IloExpr helper02(env);//feedstock type 2
			IloExpr helper01_(env); // track feedstock type 1 from harvestign site to field storage 
			IloExpr helper02_(env);// track feedstock type 2 from harvestign site to field storage 
			
			IloExpr helper11(env); //track arc from storage to depot for feedstock type 1
			IloExpr helper12(env); //track arc from storage to depot for feedstock type 2

			for (j = 0; j < Multi_period_allArcs[t].size(); j++)
			{
				if (Multi_period_allArcs[t][j].getStart() == CountyInfo[i].getArcID() && Multi_period_allArcs[t][j].getArcType() == 1)
				{
					helper01 += z[t][j];

				}
				if (Multi_period_allArcs[t][j].getStart() == CountyInfo[i].getArcID() && Multi_period_allArcs[t][j].getArcType() == 2)
				{
					helper02 += z[t][j];
				}
				//outgoing arc from harvest location to storage 
				if (Multi_period_allArcs[t][j].getEnd() == CountyInfo[i].getArcID() && Multi_period_allArcs[t][j].getArcType() == 1)
				{

					helper01_ += (1 - map_biomass[1].dml_storage)*x[t][j];
				}
				if (Multi_period_allArcs[t][j].getEnd() == CountyInfo[i].getArcID() && Multi_period_allArcs[t][j].getArcType() == 2)
				{

					helper02_ += (1 - map_biomass[2].dml_storage)*x[t][j];
				}
				

				//incoming arc to depot 
				if (Multi_period_allArcs[t][j].getStart() == CountyInfo[i].getArcID() && Multi_period_allArcs[t][j].getArcType() == 11)
				{
					helper11 += x[t][j];

				}
				if (Multi_period_allArcs[t][j].getStart() == CountyInfo[i].getArcID() && Multi_period_allArcs[t][j].getArcType() == 12)
				{
					helper12 += x[t][j];

				}
				
			}

			model.add(helper01 <= 1); //select exactly one farmgate price for 3P corn stover 
			model.add(helper02 <= 1); //select exactly one farmgate price for 2P corn stover 

			model.add(helper01_ - helper11 == 0); // biomass coming in field storage from harvesting site = sum of going out to different depot from filed storage for 3P CS
			model.add(helper02_ - helper12 == 0); //biomass coming in field storage = sum of going out to different depot for 2P CS
		
			helper01.end();
			helper02.end();
		
			helper01_.end();
			helper02_.end();
			helper11.end();
			helper12.end();
			
		}
	}
	
	


	cout << " end of loading costraint 3 and 4" << endl;
	


	///adding dml at conservation: 
	bool expfound1, expfound2;
	
	//flow based on each feedstock type 
	//this loop has multiple set of constrtains 1) total biomass flow constraitns at depot 2) individual biomass flow constraitns at depot ,3) depot capacity constaitns ,
	//4) depot utilization cosntraints  5) maximum number of depot can be located at a specific location. 
	for (int t = 0; t < period; t++)
	{
		for (i = 0; i < CountyInfo.size(); i++)
		{
			IloExpr helper2(env); // expression to incoming
			IloExpr helper21(env); // expression to incoming
			IloExpr helper22(env); // expression to incoming
			

			IloExpr Carb_in11(env);
			IloExpr Carb_in12(env);
			IloExpr Carb_out21(env);
			IloExpr Carb_out22(env);

			IloExpr Carb_in(env);
			IloExpr Carb_out(env);

			IloExpr helper3(env); // expression to outgoing
			IloExpr helper31(env); // expression to outgoing
			IloExpr helper32(env); // expression to outgoing
			


			IloExpr helper4(env); // expression to sum size of depot 
			IloExpr helper5(env); // expression to sum number  of depot at a location


			expfound1 = false;
			expfound2 = false;
			for (j = 0; j < Multi_period_allArcs[t].size(); j++)
			{
				if (Multi_period_allArcs[t][j].getEnd() == CountyInfo[i].getArcID() && (Multi_period_allArcs[t][j].getArcType() == 11)) //arc incoming to depot 

				{
					helper2 += x[t][j] * (1 - map_biomass[1].dml_preprocessing);
					helper21 += x[t][j] * (1 - map_biomass[1].dml_preprocessing);
					Carb_in11 += x[t][j] * (1 - map_biomass[1].dml_preprocessing)*Multi_period_allArcs[t][j].carb;
					Carb_in += x[t][j] * (1 - map_biomass[1].dml_preprocessing)*Multi_period_allArcs[t][j].carb;

					expfound1 = true;
				}
				if (Multi_period_allArcs[t][j].getEnd() == CountyInfo[i].getArcID() && (Multi_period_allArcs[t][j].getArcType() == 12)) //arc incoming to depot 

				{
					helper2 += x[t][j] * (1 - map_biomass[2].dml_preprocessing);
					helper22 += x[t][j] * (1 - map_biomass[2].dml_preprocessing);
					Carb_in12 += x[t][j] * (1 - map_biomass[2].dml_preprocessing)*Multi_period_allArcs[t][j].carb;
					Carb_in += x[t][j] * (1 - map_biomass[2].dml_preprocessing)*Multi_period_allArcs[t][j].carb;
					expfound1 = true;
				}
				
				if (Multi_period_allArcs[t][j].getStart() == CountyInfo[i].getArcID() && (Multi_period_allArcs[t][j].getArcType() == 21)) //arc 11= truck arc from biorefinery to blending
				{
					helper3 += x[t][j];
					helper31 += x[t][j];
					Carb_out21 += x_c[t][map_deopt_arcs[Multi_period_allArcs[t][j].getStart()][Multi_period_allArcs[t][j].getEnd()][Multi_period_allArcs[t][j].getArcType()]];
					Carb_out += x_c[t][map_deopt_arcs[Multi_period_allArcs[t][j].getStart()][Multi_period_allArcs[t][j].getEnd()][Multi_period_allArcs[t][j].getArcType()]];
					expfound2 = true;
				}
				if (Multi_period_allArcs[t][j].getStart() == CountyInfo[i].getArcID() && (Multi_period_allArcs[t][j].getArcType() == 22)) //arc 11= truck arc from biorefinery to blending
				{
					helper3 += x[t][j];
					helper32 += x[t][j];
					Carb_out22 += x_c[t][map_deopt_arcs[Multi_period_allArcs[t][j].getStart()][Multi_period_allArcs[t][j].getEnd()][Multi_period_allArcs[t][j].getArcType()]];
					Carb_out += x_c[t][map_deopt_arcs[Multi_period_allArcs[t][j].getStart()][Multi_period_allArcs[t][j].getEnd()][Multi_period_allArcs[t][j].getArcType()]];
					expfound2 = true;
				}

			}
			if (expfound1 == true && expfound2 == true)
			{
				model.add(helper2 - helper3 == 0); //flow conservation constraints
				model.add(helper21 - helper31 == 0); //flow conservation constraints
				model.add(helper22 - helper32 == 0); //flow conservation constraints
				

				model.add(Carb_in11 - Carb_out21 == 0);
				model.add(Carb_in12 - Carb_out22 == 0);
				model.add(Carb_in - Carb_out == 0);
			}
			for (k = 0; k < Depot_c_cost.size(); k++)
			{
				helper4 += Depot_c_cost[k].getProduction()*w[i][k];
				helper5 += w[i][k];
			}

			model.add(helper2 - helper4 <= 0); //constraints 7
			model.add(helper2 - capacity_utilization * helper4 >= 0); //constraints 8
			model.add(helper2 - max_capacity_utilization * helper4 <= 0); //constraints 8
			model.add(helper5 <= 1);  ////constraints 9

			helper2.end();
			helper21.end();
			helper22.end();

			helper3.end();
			helper31.end();
			helper32.end();


			helper4.end();
			helper5.end();

			Carb_in.end();
			Carb_out.end();
			Carb_in11.end();
			Carb_in12.end();
			Carb_out21.end();
			Carb_out22.end();

		}

	}
	
	cout << " end of loading constratins 5,6,7,8,9 " << endl;


	
	
	

	
	

	map<int, map<int, map<int, map<int, int>>>>map_arcs;

	map<int, int>::iterator it1; // Please use it1 this way : it1 = map_arcs[t][l][it3->first][it2->first].begin()
	map<int, int>::iterator inner_it1; // Please use it1 this way : it1 = map_arcs[t][l][it3->first][it2->first].begin()
	map<int, map<int, int>>::iterator it2;
	map<int, map<int, map<int, int>>>::iterator it3;

	map<int, map<int, map<int, map<int, int>>>>::iterator it4;

	map<int, map<int, map<int, map<int, map<int, int>>>>>::iterator it5; //please aware that it5=map_arcs.begin()



	for (i = 0; i < Multi_period_allArcs[1].size(); i++) //map_arcs[fips][fips][arc type][farmgate price]
	{
		map_arcs[Multi_period_allArcs[1][i].getStart()][Multi_period_allArcs[1][i].getEnd()][Multi_period_allArcs[1][i].getArcType()][Multi_period_allArcs[1][i].getMid1()] = i; //maparcs[county][county][arc type][farm gate price ]allArcs[i].getMid1()=farmgate price 
		//output_3 << allArcs[i].getStart() << "  " << allArcs[i].getEnd() << "  " << allArcs[i].getMid1()<<"   "<< allArcs[i].getArcType() << "  " << map_arcs[allArcs[i].getStart()][allArcs[i].getEnd()][allArcs[i].getArcType()][allArcs[i].getMid1()] << endl;
	}
	//

	//
	//
	//


	map<int, int>::iterator it1_; // Please use it1 this way : it1 = map_arcs[t][l][it3->first][it2->first].begin()
	map<int, map<int, int>>::iterator it2_;
	map<int, map<int, map<int, int>>>::iterator it3_;

	map<int, map<int, map<int, map<int, int>>>>::iterator it4_;



	


	////contraints sum 2P and 3P limitations 
	for (int t = 0; t < period; t++) //x[1][1][1][40]+x[1][1][2][40]/.487<=Production from x[1][1][1][40]
	{
		for (i = 0; i < CountyInfo.size(); i++)
		{
			for (it1_ = map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].begin(); it1_ != map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].end(); it1_++)
			{
				IloExpr helperx(env);
				if (map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].find(it1_->first) != map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].end())
				{
					helperx += x[t][map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1][it1_->first]];
				}
				if (map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][2].find(it1_->first) != map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][2].end())
				{
					helperx += x[t][map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][2][it1_->first]] / Two_pass_resource_factor;
				}
				model.add(helperx <= Multi_period_allArcs[t][map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1][it1_->first]].getProduction());
				helperx.end();
			}
		}
	}
	
}

	