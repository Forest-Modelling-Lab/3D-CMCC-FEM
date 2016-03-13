/*utility.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Reset_daily_variables (CELL *const c)
{
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
  c->daily_litter = 0.0;

  /*reset daily water variables*/
  c->prcp_rain = 0.0;
  c->prcp_snow = 0.0;
  c->snow_melt = 0.0;
  c->snow_subl = 0.0;
  c->out_flow = 0.0;
  c->daily_tot_c_transp = 0.0;
  c->daily_tot_c_int = 0.0;
  c->daily_tot_c_evapo = 0.0;
  c->soil_evaporation = 0.0;

  c->daily_tot_litterfall = 0;
  c->dominant_veg_counter = 0;
  c->dominated_veg_counter = 0;
  c->subdominated_veg_counter = 0;

  c->daily_dead_tree[0] = 0;
  c->daily_dead_tree[1] = 0;
  c->daily_dead_tree[2] = 0;
  c->daily_tot_dead_tree = 0;
  c->monthly_dead_tree[0] = 0;
  c->monthly_dead_tree[1] = 0;
  c->monthly_dead_tree[2] = 0;
  c->monthly_tot_dead_tree = 0;
  c->annual_dead_tree[0] = 0;
  c->annual_dead_tree[1] = 0;
  c->annual_dead_tree[2] = 0;
  c->annual_tot_dead_tree = 0;

}


void Reset_annual_cumulative_variables (CELL *const c, const int count)
{
  int height;
  int age;
  int species;

  /*reset cell related variables*/
  c->canopy_cover_dominant = 0;
  c->canopy_cover_dominated = 0;
  c->canopy_cover_subdominated = 0;

  c->stand_agb = 0;
  c->stand_bgb = 0;
  //c->dead_tree = 0;
  c->total_yearly_soil_evaporation = 0;

  for ( height = count - 1; height >= 0; height-- )
    {
      for (age = c->heights[height].ages_count - 1; age >= 0; age --)
	{
	  for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
	    {
	      /*reset cumulative values*/
	      c->heights[height].ages[age].species[species].counter[VEG_MONTHS] = 0;
	      c->heights[height].ages[age].species[species].counter[VEG_DAYS] = 0;
	      c->heights[height].ages[age].species[species].value[YEARLY_PHYS_MOD] = 0;
	      c->heights[height].ages[age].species[species].value[POINT_GPP_g_C] = 0;
	      c->heights[height].ages[age].species[species].value[NPP] = 0;
	      c->heights[height].ages[age].species[species].value[RAIN_INTERCEPTED] = 0;
	      c->heights[height].ages[age].species[species].value[CANOPY_WATER_STORED] = 0;
	      c->heights[height].ages[age].species[species].value[YEARLY_GPP_G_C]  = 0;
	      c->heights[height].ages[age].species[species].value[YEARLY_POINT_GPP_G_C]  = 0;
	      c->heights[height].ages[age].species[species].value[YEARLY_NPP]  = 0;
	      c->heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]  = 0;
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


extern void Annual_average_values_modifiers (SPECIES *s)
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

extern void Annual_average_values_met_data (CELL *c, double Yearly_Solar_Rad, double Yearly_Vpd, double Yearly_Temp, double Yearly_Rain )
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


