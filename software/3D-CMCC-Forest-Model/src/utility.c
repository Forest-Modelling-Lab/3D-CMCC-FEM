/*utility.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Reset_daily_variables (CELL *const c, const int count)
{
	int height;
	int age;
	int species;

	Log("...resetting daily variables...\n");

	/*reset daily carbon variables*/
	c->daily_gpp = 0.0;
	c->daily_npp_tDM = 0.0;
	c->daily_npp_gC = 0.0;
	c->daily_leaf_maint_resp = 0.0;
	c->daily_stem_maint_resp = 0.0;
	c->daily_fine_root_maint_resp = 0.0;
	c->daily_branch_maint_resp = 0.0;
	c->daily_coarse_root_maint_resp = 0.0;
	c->daily_leaf_growth_resp = 0.0;
	c->daily_stem_growth_resp = 0.0;
	c->daily_fine_root_growth_resp = 0.0;
	c->daily_branch_growth_resp = 0.0;
	c->daily_coarse_root_growth_resp = 0.0;
	c->daily_leaf_carbon = 0.0;
	c->daily_stem_carbon = 0.0;
	c->daily_fine_root_carbon = 0.0;
	c->daily_coarse_root_carbon = 0.0;
	c->daily_branch_carbon = 0.0;
	c->daily_reserve_carbon = 0.0;
	c->daily_litter = 0.0;
	c->daily_litterfall = 0.0;

	/*reset daily water variables*/
	c->prcp_rain = 0.0;
	c->prcp_snow = 0.0;
	c->snow_melt = 0.0;
	c->snow_subl = 0.0;
	c->out_flow = 0.0;
	c->daily_c_transp = 0.0;
	c->daily_c_int = 0.0;
	c->daily_c_evapo = 0.0;
	c->daily_soil_evapo = 0.0;

	c->daily_litterfall = 0.0;
	c->dominant_veg_counter = 0.0;
	c->dominated_veg_counter = 0.0;
	c->subdominated_veg_counter = 0.0;

	c->layer_daily_dead_tree[0] = 0.0;
	c->layer_daily_dead_tree[1] = 0.0;
	c->layer_daily_dead_tree[2] = 0.0;
	c->daily_dead_tree = 0.0;

	for ( height = count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				c->heights[height].ages[age].species[species].value[DAILY_POINT_GPP_gC] = 0.0;
				c->heights[height].ages[age].species[species].value[NPP_tDM] = 0.0;
				c->heights[height].ages[age].species[species].value[RAIN_INTERCEPTED] = 0.0;
				c->heights[height].ages[age].species[species].value[CANOPY_WATER_STORED] = 0.0;
			}
		}
	}


}

void Reset_monthly_variables (CELL *const c, const int count)
{
	int height;
	int age;
	int species;

	Log("...resetting monthly variables...\n");

	c->monthly_gpp = 0.0;
	c->monthly_npp_gC = 0.0;
	c->monthly_npp_tDM = 0.0;
	c->monthly_aut_resp = 0.0;
	c->monthly_aut_resp_tDM = 0.0;
	c->monthly_maint_resp = 0.0;
	c->monthly_growth_resp = 0.0;
	c->monthly_r_eco = 0.0;
	c->monthly_het_resp = 0.0;
	c->monthly_gpp = 0.0;
	c->monthly_C_flux = 0.0;
	c->monthly_nee = 0.0;
	c->monthly_litterfall = 0.0;
	c->monthly_tot_w_flux = 0.0;
	c->monthly_c_int = 0.0;
	c->monthly_c_transp = 0.0;
	c->monthly_c_evapo = 0.0;
	c->monthly_c_water_stored = 0.0;
	c->monthly_c_evapotransp = 0.0;
	c->monthly_soil_evapo = 0.0;
	c->monthly_et = 0.0;

	c->layer_monthly_gpp[3] = 0.0;
	//tocontinue...

	for ( height = count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				c->heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] = 0.0;
			}
		}
	}



}


void Reset_annual_variables (CELL *const c, const int count)
{
	int height;
	int age;
	int species;
	Log("...resetting annual variables...\n");

	/*reset cell related variables*/
	c->canopy_cover_dominant = 0.0;
	c->canopy_cover_dominated = 0.0;
	c->canopy_cover_subdominated = 0.0;

	c->annual_gpp = 0.0;
	c->annual_npp_gC = 0.0;
	c->annual_npp_tDM = 0.0;
	c->annual_aut_resp = 0.0;
	c->annual_aut_resp_tDM = 0.0;
	c->annual_maint_resp = 0.0;
	c->annual_growth_resp = 0.0;
	c->annual_r_eco = 0.0;
	c->annual_het_resp = 0.0;
	c->annual_c_int = 0.0;
	c->annual_c_transp = 0.0;
	c->annual_c_evapo = 0.0;
	c->annual_c_water_stored = 0.0;
	c->annual_c_evapotransp = 0.0;
	c->annual_soil_evapo = 0.0;
	c->annual_et = 0.0;


	c->stand_agb = 0.0;
	c->stand_bgb = 0.0;
	//c->dead_tree = 0;
	c->annual_soil_evapo = 0.0;

	for ( height = count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				/*reset cumulative values*/

				c->heights[height].ages[age].species[species].counter[VEG_DAYS] = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_PHYS_MOD] = 0;

				c->heights[height].ages[age].species[species].value[YEARLY_GPP_gC]  = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_POINT_GPP_gC]  = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_NPP_tDM]  = 0;

				// ALESSIOR DEL_STEMS used instead of DEAD_STEMS
				c->heights[height].ages[age].species[species].counter[DEAD_STEMS] = 0;
				c->heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;

				//INITIALIZE AVERAGE YEARLY MODIFIERS
				c->heights[height].ages[age].species[species].value[AVERAGE_F_VPD]  = 0;
				c->heights[height].ages[age].species[species].value[AVERAGE_F_T]  = 0;
				c->heights[height].ages[age].species[species].value[AVERAGE_F_SW]  = 0;
				c->heights[height].ages[age].species[species].value[F_AGE]  = 0;


				c->heights[height].ages[age].species[species].value[DEL_Y_WS] = 0;
				c->heights[height].ages[age].species[species].value[DEL_Y_WF] = 0;
				c->heights[height].ages[age].species[species].value[DEL_Y_WFR] = 0;
				c->heights[height].ages[age].species[species].value[DEL_Y_WCR] = 0;
				c->heights[height].ages[age].species[species].value[DEL_Y_WRES] = 0;
				c->heights[height].ages[age].species[species].value[DEL_Y_WR] = 0;
				c->heights[height].ages[age].species[species].value[DEL_Y_BB] = 0;
			}
		}
	}
}

void First_day (CELL *const c, const int count)
{
	int height;
	int age;
	int species;

	Log("..first day..\n");


	for ( height = count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				c->heights[height].ages[age].species[species].turnover->FINERTOVER = 365 /
						c->heights[height].ages[age].species[species].value[LEAVES_FINERTTOVER];
				c->heights[height].ages[age].species[species].turnover->COARSERTOVER = 365 /
						c->heights[height].ages[age].species[species].value[COARSERTTOVER];
				c->heights[height].ages[age].species[species].turnover->STEMTOVER = 365 /
						c->heights[height].ages[age].species[species].value[LIVE_WOOD_TURNOVER];
				c->heights[height].ages[age].species[species].turnover->BRANCHTOVER = 365 /
						c->heights[height].ages[age].species[species].value[BRANCHTTOVER];
			}
		}
	}
}


void Annual_average_values_modifiers (SPECIES *s)
{

	//compute to control annual average values for modifiers
	//VPD
	s->value[AVERAGE_F_VPD] /= s->counter[VEG_MONTHS];
	//Log ("average  f_VPD = %f \n", s->value[AVERAGE_F_VPD] );
	s->value[AVERAGE_F_VPD] = 0;

	//TEMPERATURE
	s->value[AVERAGE_F_T] /= s->counter[VEG_MONTHS];
	//Log ("average  f_T = %f \n",s->value[AVERAGE_F_T] );
	s->value[AVERAGE_F_T] = 0;

	//AGE
	//Log ("average  f_AGE = %f \n",s->value[F_AGE] );

	//SOIL WATER
	s->value[AVERAGE_F_SW] /= s->counter[VEG_MONTHS];
	//Log ("average  f_SW = %f \n",s->value[AVERAGE_F_SW] );
	s->value[AVERAGE_F_SW] = 0;

}

void Annual_average_values_met_data (CELL *c, double Yearly_Solar_Rad, double Yearly_Vpd, double Yearly_Temp, double Yearly_Rain )
{
	//Log("--AVERAGE YEARLY MET DATA--\n");
	//SOLAR RAD
	Yearly_Solar_Rad /= 12;
	//Log ("average Solar Rad = %f MJ m^2 month\n", Yearly_Solar_Rad );
	Yearly_Solar_Rad = 0;
	//VPD
	Yearly_Vpd /= 12;
	//Log ("average Vpd = %f mbar\n", Yearly_Vpd );
	Yearly_Vpd = 0;
	//TEMPERATURE
	Yearly_Temp /= 12;
	//Log ("average Temperature = %f C° month\n", Yearly_Temp );
	Yearly_Temp = 0;
	//RAIN
	//Log("yearly Rain = %f mm year\n", Yearly_Rain);
	Yearly_Rain = 0;
	//MOIST RATIO
	c->av_soil_moist_ratio /= 12;
	//Log("average Moist Ratio = %f year\n",c->av_soil_moist_ratio);
	//Log ("average Yearly Rain = %f MJ m^2 month\n",  );
}


