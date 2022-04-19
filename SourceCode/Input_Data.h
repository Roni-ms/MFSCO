//#include <cmath>


	



///*********************General input **********************************
   


 


	int period = 10;
  

   float winding_factor = 1.2; //reference http://www.ncbi.nlm.nih.gov/pmc/articles/PMC3835347/
  



/********************************************** Model input parameter **************************************///
	//Cost information 
	
	
	float DVC_truck = .112*1.012573964; //regression equation from BLM
	float DFC_truck = 3.38*1.012573964; //regression equation from BLM
	
	
	 

	

	
	////* feedstock characteristics : 

	float c = .59;

	//ash fractions in delivered feedstock 
	
	float a = .05;

	

	double total_fed_demand = 725000;

	float capacity_utilization = .9;

	float max_capacity_utilization = .95;

	float Two_pass_resource_factor = .487;


	//float base_lat = 39.349445, base_long = -99.880952; //grahama county locaiton
	//int  base_loc = 20165;


	float base_lat = 39.35054, base_long = -100.441; //sheridan county locaiton
	int  base_loc= 20179;

	float ash_disp_cost = 37.16*1.012573964;
	float base_data_gp = 19.44;

	float min_yield_2P = 1.99;

	float base_yield_3P = 1.2; 
	float base_yield_2P = .7;

	float carb_dockage = 293.85; //$/per % ch contenct 



