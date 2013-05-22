/*utility.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Reset_annual_cumulative_variables (CELL *const c, const int count)
{
	int height;
	int age;
	int species;

	for ( height = count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				/*reset cumulative values*/

				c->canopy_cover_dominant = 0;
				c->canopy_cover_dominated = 0;
				c->canopy_cover_subdominated = 0;


				c->gpp = 0;
				c->npp = 0;
				c->stand_agb = 0;
				c->stand_bgb = 0;
				c->evapotranspiration = 0;
				c->heights[height].ages[age].species[species].counter[VEG_MONTHS] = 0;
				c->heights[height].ages[age].species[species].counter[VEG_DAYS] = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_PHYS_MOD] = 0;
				c->heights[height].ages[age].species[species].value[POINT_GPP_g_C]  = 0;
				c->heights[height].ages[age].species[species].value[NPP]  = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_GPP_G_C]  = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_POINT_GPP_G_C]  = 0;
				c->heights[height].ages[age].species[species].value[YEARLY_NPP]  = 0;
				c->heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]  = 0;
				c->heights[height].ages[age].species[species].counter[DEL_STEMS] = 0;
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



				//reset only in unspatial version
				if (settings->spatial == 'u')
				{
					//reset foliage biomass for deciduous
					if ( c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0)
					{
						c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] = c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM];

						//reset LAI
						c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] = 0;
					}
				}

			}
		}
	}
}


extern void Get_annual_average_values_modifiers (SPECIES *s)
{

	//compute to control annual average values for modifiers
	//VPD
	s->value[AVERAGE_F_VPD] /= s->counter[VEG_MONTHS];
	//Log ("average  f_VPD = %g \n", s->value[AVERAGE_F_VPD] );
	s->value[AVERAGE_F_VPD] = 0;

	//TEMPERATURE
	s->value[AVERAGE_F_T] /= s->counter[VEG_MONTHS];
	//Log ("average  f_T = %g \n",s->value[AVERAGE_F_T] );
	s->value[AVERAGE_F_T] = 0;

	//AGE
	//Log ("average  f_AGE = %g \n",s->value[F_AGE] );

	//SOIL WATER
	s->value[AVERAGE_F_SW] /= s->counter[VEG_MONTHS];
	//Log ("average  f_SW = %g \n",s->value[AVERAGE_F_SW] );
	s->value[AVERAGE_F_SW] = 0;

}

extern void Get_annual_average_values_met_data (CELL *c, float Yearly_Solar_Rad, float Yearly_Vpd, float Yearly_Temp, float Yearly_Rain )
{
	//Log("--AVERAGE YEARLY MET DATA--\n");
	//SOLAR RAD
	Yearly_Solar_Rad /= 12;
	//Log ("average Solar Rad = %g MJ m^2 month\n", Yearly_Solar_Rad );
	Yearly_Solar_Rad = 0;
	//VPD
	Yearly_Vpd /= 12;
	//Log ("average Vpd = %g mbar\n", Yearly_Vpd );
	Yearly_Vpd = 0;
	//TEMPERATURE
	Yearly_Temp /= 12;
	//Log ("average Temperature = %g C° month\n", Yearly_Temp );
	Yearly_Temp = 0;
	//RAIN
	//Log("yearly Rain = %g mm year\n", Yearly_Rain);
	Yearly_Rain = 0;
	//MOIST RATIO
	c->av_soil_moist_ratio /= 12;
	//Log("average Moist Ratio = %g year\n",c->av_soil_moist_ratio);
	//Log ("average Yearly Rain = %g MJ m^2 month\n",  );
}


extern void Get_EOY_cumulative_balance_layer_level (SPECIES *s, HEIGHT *h)
{

	//CUMULATIVE BALANCE FOR ENTIRE LAYER
	Log("**CUMULATIVE BALANCE for layer %d ** \n", h->z);
	Log("END of Year Yearly Cumulated GPP for layer %d  = %g gCm^2 year\n", h->z, s->value[YEARLY_POINT_GPP_G_C]);
	Log("END of Year Yearly Cumulated NPP for layer %d  = %g tDM/area year\n", h->z, s->value[YEARLY_NPP]);
	Log("END of Year Yearly Cumulated NPP/ha_area_covered for layer %d  = %g tDM/ha_area_covered year\n", h->z, (s->value[YEARLY_NPP]/settings->sizeCell) * (10000 * s->value[CANOPY_COVER_DBHDC]));
	Log("END of Year Yearly Cumulated DEL STEM layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WS]);
	//Log("END of Year Yearly Cumulated DEL FOLIAGE layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WF]);
	Log("END of Year Yearly Cumulated DEL FINE ROOT layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WFR]);
	Log("END of Year Yearly Cumulated DEL COARSE ROOT layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WCR]);
	Log("END of Year Yearly Cumulated DEL RESERVE layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WRES]);
	Log("END of Year Yearly Cumulated DEL RESERVE layer %d  = %g KgC tree year\n", h->z, (s->value[DEL_Y_WRES]*2000)/s->counter[N_TREE]);
	Log("END of Year Yearly Cumulated DEL BB layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_BB]);
	Log("END of Year Yearly Cumulated DEL TOT ROOT layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WR]);

	if (s->value[DEL_Y_WS] + s->value[DEL_Y_WR] + s->value[DEL_Y_WF] + s->value[DEL_Y_WRES] + s->value[DEL_Y_BB] != s->value[YEARLY_NPP])
	{
		Log("ATTENTION SUM OF ALL INCREMENTS DIFFERENT FROM YEARLY NPP \n");
		Log("DEL SUM = %g \n", s->value[DEL_Y_WS] + s->value[DEL_Y_WCR] + s->value[DEL_Y_WFR] + s->value[DEL_Y_BB] + s->value[DEL_Y_WF] + s->value[DEL_Y_WRES]);
	}
}




extern void Get_EOD_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month, int day )
{
	if(day  == 0 && month == 0 && years == 0)
	{
		Daily_Log("Daily summary output from 3D-CMCC FEM\n");
		Daily_Log("Daily GPP = daily total gross primary production (gC/m2/day)\n");
		Daily_Log("Daily NPP = daily total net primary production (tDM/m2/day)\n");
		Daily_Log("Daily ET = daily canopy transpiration(mm/day)\n");
		Daily_Log("Daily LAI = daily Leaf Area Index (m^2/m^2)\n\n\n");
	}
	if (day == 0 && month == 0)
	{
		Daily_Log ("\n-%s %10s %10s %10s %10s %10s %10s\n\n", "YEAR", "MONTH", "DAY","GPP", "NPP", "ET", "LAI");
	}
	Daily_Log ("-%d %10d %10d %10g %10g %10g %10g\n", yos[years].year,  month+1, day+1, c->daily_gpp, c->daily_npp, c->daily_et, c->daily_lai);

}


extern void Get_EOM_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month)
{
	if(month == 0 && years == 0)
	{
		Monthly_Log("Monthly summary output from 3D-CMCC FEM\n");
		Monthly_Log("Monthly GPP = monthly total gross primary production (gC/m2/month)\n");
		Monthly_Log("Monthly NPP = monthly total net primary production (tDM/m2/month)\n");
		Monthly_Log("Monthly ET = monthly canopy transpiration(mm/month)\n\n\n");
	}
	if(month == 0)
	{
		Monthly_Log ("\n-%s %10s %10s %10s %10s\n\n", "YEAR", "MONTH", "GPP", "NPP", "ET");
	}
	Monthly_Log ("-%d %10d %10g %10g %10g\n", yos[years].year, month+1, c->monthly_gpp, c->monthly_npp, c->monthly_et);
}

extern void Get_EOY_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years)
{
	if (years == 0)
	{
		Annual_Log("Annual summary output from 3D-CMCC FEM\n");
		Annual_Log("Annual GPP = annual total gross primary production (gC/m2/year)\n");
		Annual_Log("Annual NPP = annual total net primary production (tDM/m2/year)\n");
		Annual_Log("Annual ET = annual canopy transpiration(mm/year)\n");
		Annual_Log("Annual AGB = annual above-ground biomass(tDM/area/year)\n");
		Annual_Log("Annual BGB = annual below-ground biomass(tDM/area/year)\n\n\n");
		Annual_Log ("-%s %10s %10s %10s %10s %10s\n", "YEAR", "GPP", "NPP", "ET", "AGB", "BGB");
	}
	Annual_Log ("-%d %10g %10g %10g %10g %10g\n", yos[years].year, c->annual_gpp, c->annual_npp, c->annual_et, c->stand_agb, c->stand_bgb);
}

void Get_a_Power_Function (AGE *a, SPECIES *s)
{
	//todo ask to Laura references
	//this function computes the STEMCONST values using the values reported from ...... ask to Laura

	float MassDensity;

	MassDensity = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (a->value / s->value[TRHO]));
	Log("-Mass Density = %g\n", MassDensity);

	if (s->value[AVDBH] < 9)
	{
		s->value[STEMCONST] = pow (e, -1.6381);
	}
	else if (s->value[AVDBH]>9 && s->value[AVDBH]<15)
	{
		s->value[STEMCONST] = pow (e, -3.51+1.27*MassDensity);
	}
	else
	{
		s->value[STEMCONST] = pow (e, -3.51+1.27*MassDensity);
	}
	Log("-Stem const = %g\n", s->value[STEMCONST]);
}
