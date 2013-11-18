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
				//c->dead_tree = 0;
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
					if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
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
	Log("END of Year Yearly Cumulated DEL FOLIAGE layer %d  = %g tDM/area year\n", h->z, s->value[DEL_Y_WF]);
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


extern void Get_EOY_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int years_of_simulation)
{
	static float avg_gpp[3], avg_npp[3], avg_ce[3], avg_gpp_tot, avg_npp_tot, avg_ce_tot;
	static float avg_ar[3], avg_ar_tot;
	static int tot_dead_tree_tot;

	static int previous_layer_number;


	if (years == 0)
	{
		Annual_Log("Annual summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Annual_Log("years of simulation = %d\n", years_of_simulation);
		Annual_Log("\n\nCell %d, %d, Lat = %g, Long  = %g\n\n\n", c->x, c->y, site->lat, site->lon );
		Annual_Log("HC(n) = height class counter for n layer\n");
		Annual_Log("Annual GPP = annual total gross primary production (gC/m2/year)\n");
		Annual_Log("Annual RA = annual total autotrophic respiration (gC/m2/year)\n");
		Annual_Log("Annual Y = NPP/GPP ratio (%)\n");
		Annual_Log("Annual NPP = annual total net primary production (tDM/m2/year)\n");
		Annual_Log("Annual CE = annual canopy evapotranspiration(mm/year)\n");
		Annual_Log("Annual ASW = end of year annual available soil water(mm)\n");
		Annual_Log("Annual PEAK_LAI = annual Peak Lai (m^2/m^2)\n");
		Annual_Log("Annual Dead tree = annual dead tree (n tree/cell)\n\n\n");
	}

	//reset
	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;

		avg_gpp_tot = 0;
		avg_ar_tot = 0;
		avg_npp_tot = 0;
		avg_ce_tot = 0;
		tot_dead_tree_tot = 0;

		if (c->annual_layer_number == 1)
		{
			avg_gpp[0] = 0;
			avg_ar[0] = 0;
			avg_npp[0] = 0;
			avg_ce[0] = 0;
		}
		if (c->annual_layer_number == 2)
		{
			avg_gpp[1] = 0;
			avg_ar[1] = 0;
			avg_npp[1] = 0;
			avg_ce[1] = 0;
			avg_gpp[0] = 0;
			avg_ar[0] = 0;
			avg_npp[0] = 0;
			avg_ce[0] = 0;
		}
		if (c->annual_layer_number == 3)
		{
			avg_gpp[2] = 0;
			avg_ar[2] = 0;
			avg_npp[2] = 0;
			avg_ce[2] = 0;
			avg_gpp[1] = 0;
			avg_ar[1] = 0;
			avg_npp[1] = 0;
			avg_ce[1] = 0;
			avg_gpp[0] = 0;
			avg_ar[0] = 0;
			avg_npp[0] = 0;
			avg_ce[0] = 0;
		}
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}

	//Annual_Log ("-%d %10g %10g %10g %10g %10g %10g %10d\n", yos[years].year, c->annual_gpp, c->annual_npp, c->annual_et, c->stand_agb, c->stand_bgb, c->annual_peak_lai, c->dead_tree);


	if (c->annual_layer_number == 1)
	{
		if (years == 0 || previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n%s \t%2s \t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%8s \t%8s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n\n",
					"YEAR", "HC(0)", "GPP(0)", "GPP (tot)", "AR(0)", "AR (tot)","Y(%tot)", "NPP(0)", "NPP(tot)", "NPP(gC/m2yr)","CE(0)", "CE(tot)", "ASW", "PEAK_LAI(0)",
					"CC(0)", "DEAD TREE(0)", "DEAD TREE(tot)", "DELTA-Ws", "Ws");
		}
		Annual_Log ("%d \t%2d \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%12d \t%14d \t%11g \t%11g\n",
				yos[years].year,
				c->height_class_in_layer_dominant_counter,
				c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[0], c->annual_tot_aut_resp,
				((c->annual_tot_aut_resp * 100.0)/c->annual_tot_gpp),
				c->annual_npp[0], c->annual_tot_npp,
				((c->annual_tot_npp/settings->sizeCell)*1000000)/2,
				c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp ,
				c->available_soil_water,
				c->annual_peak_lai[0], c->annual_cc[0],
				c->annual_dead_tree[0], c->annual_tot_dead_tree,
				c->annual_delta_ws[0], c->annual_ws[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average or total
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_npp[0] += c->annual_npp[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[0];
		avg_npp_tot += c->annual_npp[0];
		avg_ce_tot += c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;

		//reset
		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_npp[0] = 0;
		c->annual_c_evapotransp[0] = 0;
		c->annual_cc[0] = 0;
		c->annual_dead_tree[0] = 0;

		c->annual_tot_gpp = 0;
		c->annual_tot_npp = 0;
		c->annual_tot_c_evapotransp = 0;
		c->annual_tot_aut_resp = 0;
		c->annual_tot_dead_tree = 0;

		c->annual_peak_lai[0] = 0;

		c->annual_delta_ws[0] = 0;
	}
	if (c->annual_layer_number == 2)
	{
		if (years == 0 || previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n%s \t%5s \t%5s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%7s \t%10s \t%10s \t%10s \t%10s \t%10s\n\n",
					"YEAR", "HC(1)", "HC(0)", "GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR(0)", "AR(tot)", "Y(1)", "Y(0)", "Y(tot)", "NPP(1)", "NPP(0)", "NPP (tot)", "CE(1)", "CE(0)", "CE (tot)", "ASW"
					"PEAK_LAI(1)", "PEAK_LAI(0)", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)",
					"Ws(1)", "Ws(0)");
		}
		Annual_Log ("%d \t%5d \t%5d \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g  \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%12d \t%12d \t%12d\n",
				yos[years].year,
				c->height_class_in_layer_dominant_counter, c->height_class_in_layer_dominated_counter,
				c->annual_gpp[1],c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[1],c->annual_aut_resp[0], c->annual_tot_aut_resp,
				(c->annual_aut_resp[1]*100)/ c->annual_gpp[1],((c->annual_aut_resp[0]*100)/ c->annual_gpp[0]),(c->annual_tot_aut_resp*100)/c->annual_tot_gpp,
				c->annual_npp[1], c->annual_npp[0],	c->annual_tot_npp,
				c->annual_c_evapotransp[1], c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp,
				c->available_soil_water,
				c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->annual_cc[1], c->annual_cc[0],
				c->annual_dead_tree[1], c->annual_dead_tree[0], c->annual_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[1] += c->annual_gpp[1];
		avg_ar[1] += c->annual_aut_resp[1];
		avg_npp[1] += c->annual_npp[1];
		avg_ce[1] += c->annual_c_evapotransp[1];
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_npp[0] += c->annual_npp[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[1] + c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[1] + c->annual_aut_resp[0];
		avg_npp_tot += c->annual_npp[1] + c->annual_npp[0];
		avg_ce_tot += c->annual_c_evapotransp[1] + c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;


		//reset
		c->annual_gpp[1] = 0;
		c->annual_aut_resp[1] = 0;
		c->annual_npp[1] = 0;
		c->annual_c_evapotransp[1] = 0;
		c->annual_cc[1] = 0;
		c->annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_ws[1] = 0;

		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_npp[0] = 0;
		c->annual_c_evapotransp[0] = 0;
		c->annual_cc[0] = 0;
		c->annual_dead_tree[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_ws[0] = 0;

		c->annual_tot_gpp = 0;
		c->annual_tot_aut_resp = 0;
		c->annual_tot_npp = 0;
		c->annual_tot_c_evapotransp = 0;
		c->annual_tot_dead_tree = 0;

		c->annual_tot_dead_tree = 0;

		c->annual_peak_lai[1] = 0;
		c->annual_peak_lai[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (years == 0 || previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n%s \t%5s \t%5s \t%5s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s %7s \t%10s \t%10s\n\n",
					"YEAR", "HC(2)", "HC(1)", "HC(0)", "GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)", "AR(1)", "AR(0)", "AR(tot)",
					"Y(2)", "Y(1)", "Y(0)", "Y(tot)","NPP(2)", "NPP(1)", "NPP(0)", "NPP (tot)", "CE(2)", "CE(1)", "CE(0)",
					"CE(tot)", "ASW", "PEAK_LAI(2)", "PEAK_LAI(1)", "PEAK_LAI(0)", "CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)",
					"DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Annual_Log ("%d \t%5d \t%5d \t%5d \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10d \t%10d \t%10d \t%10d\n",
				yos[years].year,
				c->height_class_in_layer_dominant_counter, c->height_class_in_layer_dominated_counter, c->height_class_in_layer_subdominated_counter,
				c->annual_gpp[2], c->annual_gpp[1],c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[2], c->annual_aut_resp[1],c->annual_aut_resp[0], c->annual_tot_aut_resp,
				(c->annual_aut_resp[2]*100.0)/c->annual_gpp[2],((c->annual_aut_resp[1]*100.0)/c->annual_gpp[1]),((c->annual_aut_resp[0]*100.0)/c->annual_gpp[0]),(c->annual_tot_aut_resp*100.0)/c->annual_tot_gpp,
				c->annual_npp[2], c->annual_npp[1],	c->annual_npp[0], c->annual_tot_npp,
				c->annual_c_evapotransp[2],c->annual_c_evapotransp[1], c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp,
				c->available_soil_water,
				c->annual_peak_lai[2], c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->annual_cc[2],c->annual_cc[1], c->annual_cc[0],
				c->annual_dead_tree[2], c->annual_dead_tree[1], c->annual_dead_tree[0], c->annual_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[2] += c->annual_gpp[2];
		avg_ar[2] += c->annual_aut_resp[2];
		avg_npp[2] += c->annual_npp[2];
		avg_ce[2] += c->annual_c_evapotransp[2];
		avg_gpp[1] += c->annual_gpp[1];
		avg_ar[1] += c->annual_aut_resp[1];
		avg_npp[1] += c->annual_npp[1];
		avg_ce[1] += c->annual_c_evapotransp[1];
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_npp[0] += c->annual_npp[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[2] +c->annual_gpp[1] + c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[2] +c->annual_aut_resp[1] + c->annual_aut_resp[0];
		avg_npp_tot += c->annual_npp[2] +c->annual_npp[1] + c->annual_npp[0];
		avg_ce_tot += c->annual_c_evapotransp[2] + c->annual_c_evapotransp[1] + c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;



		//reset
		c->annual_gpp[2] = 0;
		c->annual_aut_resp[2] = 0;
		c->annual_npp[2] = 0;
		c->annual_c_evapotransp[2] = 0;
		c->annual_cc[2] = 0;
		c->annual_dead_tree[2] = 0;
		c->annual_delta_ws[2] = 0;
		c->annual_ws[2] = 0;

		c->annual_gpp[1] = 0;
		c->annual_aut_resp[1] = 0;
		c->annual_npp[1] = 0;
		c->annual_c_evapotransp[1] = 0;
		c->annual_cc[1] = 0;
		c->annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_ws[1] = 0;

		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_npp[0] = 0;
		c->annual_c_evapotransp[0] = 0;
		c->annual_cc[0] = 0;
		c->annual_dead_tree[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_ws[0] = 0;

		c->annual_tot_gpp = 0;
		c->annual_tot_aut_resp = 0;
		c->annual_tot_npp = 0;
		c->annual_tot_c_evapotransp = 0;
		c->annual_tot_dead_tree = 0;

		c->annual_peak_lai[2] = 0;
		c->annual_peak_lai[1] = 0;
		c->annual_peak_lai[0] = 0;

	}

	//compute average values
	if (years == years_of_simulation -1 && years_of_simulation > 1)
	{
		if (c->annual_layer_number == 1)
		{
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;
			Annual_Log ("-----------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t%5g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%12g \t%9g \t%10g \t\t\t\t%49d\n",
					  avg_gpp[0], avg_gpp_tot, avg_ar[0], avg_ar_tot, (avg_ar_tot*100.0)/avg_gpp_tot, avg_npp[0], avg_npp_tot, ((avg_npp_tot/settings->sizeCell)*1000000)/2 ,avg_ce[0], avg_ce_tot, tot_dead_tree_tot);
		}
		if (c->annual_layer_number == 2)
		{
			avg_gpp[1] /= years_of_simulation;
			avg_ar[1] /= years_of_simulation;
			avg_npp[1] /= years_of_simulation;
			avg_ce[1] /= years_of_simulation;
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;
			Annual_Log ("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t\t%14g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t\t\t\t\t\t\t%76d \n",
					 avg_gpp[1], avg_gpp[0], avg_gpp_tot, avg_ar[1], avg_ar[0], avg_ar_tot, (avg_ar[1]*100.0)/avg_gpp[1],
					 ((avg_ar[0]*100.0)/avg_gpp[0]),(avg_ar_tot*100.0)/avg_gpp_tot,avg_npp[1], avg_npp[0], avg_npp_tot, avg_ce[1], avg_ce[0], avg_ce_tot, tot_dead_tree_tot);
		}
		if (c->annual_layer_number == 3)
		{
			avg_gpp[2] /= years_of_simulation;
			avg_ar[2] /= years_of_simulation;
			avg_npp[2] /= years_of_simulation;
			avg_ce[2] /= years_of_simulation;
			avg_gpp[1] /= years_of_simulation;
			avg_ar[1] /= years_of_simulation;
			avg_npp[1] /= years_of_simulation;
			avg_ce[1] /= years_of_simulation;
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;

			Annual_Log ("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t\t\t%17g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t\t\t\t\t\t\t\t\t%42d \n",
					avg_gpp[2], avg_gpp[1], avg_gpp[0], avg_gpp_tot, avg_ar[2], avg_ar[1], avg_ar[0], avg_ar_tot,
					(avg_ar[2]*100.0)/avg_gpp[2], (avg_ar[1]*100.0)/avg_gpp[1], (avg_ar[0]*100.0)/avg_gpp[0], (avg_ar_tot*100.0)/avg_gpp_tot,
					avg_npp[2], avg_npp[1], avg_npp[0], avg_npp_tot, avg_ce[2], avg_ce[1], avg_ce[0], avg_ce_tot, tot_dead_tree_tot);


			//
			//}
		}
	}
}

extern void Get_EOM_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month)
{
	static int previous_layer_number;

	if(month == 0 && years == 0)
	{
		Monthly_Log("Monthly summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Monthly_Log("\n\nCell %d, %d, Lat = %g, Long  = %g\n\n\n", c->x, c->y, site->lat, site->lon );
		Monthly_Log("Monthly GPP = monthly total gross primary production (gC/m2/month)\n");
		Monthly_Log("Monthly AR = monthly total autotrophic respiration (gC/m2/month)\n");
		Monthly_Log("Monthly NPP = monthly total net primary production (tDM/m2/month)\n");
		Monthly_Log("Monthly CE = monthly canopy evapotranspiration(mm/month)\n");
		Monthly_Log("Monthly DEAD TREE = monthly dead tree (n tree/cell)\n\n\n");
	}
	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			Monthly_Log ("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
		previous_layer_number = c->annual_layer_number;
	}

	if (c->annual_layer_number == 1)
	{
		if (month == 0)
		{
			Monthly_Log ("\n%s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n\n",
					"YEAR", "MONTH", "GPP(0)", "GPP (tot)", "AR(0)", "AR(tot)", "NPP(0)", "NPP (tot)", "CE(0)", "CE(tot)", "CC(0)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%10d \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10d \t%10d \n",
				yos[years].year, month+1, c->monthly_gpp[0], c->monthly_tot_gpp, c->monthly_aut_resp[0], c->monthly_tot_aut_resp, c->monthly_npp[0], c->monthly_tot_npp,
				c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp, c->monthly_cc[0], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);


		//reset
		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_npp[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;

		c->monthly_tot_gpp = 0;
		c->monthly_tot_aut_resp = 0;
		c->monthly_tot_npp = 0;
		c->monthly_tot_c_evapotransp = 0;
		c->monthly_tot_dead_tree = 0;

	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if (month == 0)
		{
			Monthly_Log ("\n%s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n\n",
					"YEAR", "MONTH", "GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR(0)", "AR(tot)","NPP(1)", "NPP(0)", "NPP(tot)", "CE(1)",
					"CE(0)", "CE(tot)", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%10d \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10d \t%10d \t%10d\n",
				yos[years].year, month+1,
				c->monthly_gpp[1],c->monthly_gpp[0], c->monthly_tot_gpp,
				c->monthly_maint_resp[1],c->monthly_aut_resp[0], c->monthly_tot_aut_resp,
				c->monthly_npp[1], c->monthly_npp[0],c->monthly_tot_npp,
				c->monthly_c_evapotransp[1], c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp,
				c->monthly_cc[1], c->monthly_cc[0],
				c->monthly_dead_tree[1], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[1] = 0;
		c->monthly_aut_resp[1] = 0;
		c->monthly_npp[1] = 0;
		c->monthly_c_evapotransp[1] = 0;
		c->monthly_cc[1] = 0;
		c->monthly_dead_tree[1] = 0;

		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_npp[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;

		c->monthly_tot_gpp = 0;
		c->monthly_tot_aut_resp = 0;
		c->monthly_tot_npp = 0;
		c->monthly_tot_c_evapotransp = 0;
		c->monthly_tot_dead_tree = 0;

	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (month == 0)
		{
			Monthly_Log ("\n%s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n\n",
					"YEAR", "MONTH", "GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)", "NPP(2)","NPP(1)", "NPP(0)","NPP (tot)",
					"CE(2)","CE(1)", "CE(0)", "CE(tot)", "CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)",
					"DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%10d \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10g \t%10d \t%10d \t%10d \t%10d\n",
				yos[years].year, month+1,
				c->monthly_gpp[2], c->monthly_gpp[1],c->monthly_gpp[0], c->monthly_tot_gpp,
				c->monthly_maint_resp[2], c->monthly_aut_resp[1],c->monthly_aut_resp[0], c->monthly_tot_aut_resp,
				c->monthly_npp[2], c->monthly_npp[1], c->monthly_npp[0], c->monthly_tot_npp,
				c->monthly_c_evapotransp[2],c->monthly_c_evapotransp[1], c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp,
				c->monthly_cc[2], c->monthly_cc[1], c->monthly_cc[0],
				c->monthly_dead_tree[2], c->monthly_dead_tree[1], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[2] = 0;
		c->monthly_aut_resp[2] = 0;
		c->monthly_npp[2] = 0;
		c->monthly_c_evapotransp[2] = 0;
		c->monthly_cc[2] = 0;
		c->monthly_dead_tree[2] = 0;

		c->monthly_gpp[1] = 0;
		c->monthly_aut_resp[1] = 0;
		c->monthly_npp[1] = 0;
		c->monthly_c_evapotransp[1] = 0;
		c->monthly_cc[1] = 0;
		c->monthly_dead_tree[1] = 0;

		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_npp[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;

		c->monthly_tot_gpp = 0;
		c->monthly_tot_aut_resp = 0;
		c->monthly_tot_npp = 0;
		c->monthly_tot_c_evapotransp = 0;
		c->monthly_tot_dead_tree = 0;
	}
}



extern void Get_EOD_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month, int day )
{

	static int previous_layer_number;

	if(day  == 0 && month == 0 && years == 0)
	{
		Daily_Log("Daily summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Daily_Log("\n\nCell %d, %d, Lat = %g, Long  = %g\n\n\n", c->x, c->y, site->lat, site->lon );
		Daily_Log("Daily GPP = daily total gross primary production (gC/m2/day)\n");
		Daily_Log("Daily AR = daily total autotrophic respiration (gC/m2/day)\n");
		Daily_Log("Daily Cf = daily c-fluxes (gC/m2/day)\n");
		Daily_Log("Daily NPP = daily total net primary production (tDM/m2/day)\n");
		Daily_Log("Daily CE = daily canopy evapotranspiration(mm/day)\n");
		Daily_Log("Daily LAI = daily Leaf Area Index (m^2/m^2)\n");
	}

	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}
	if (c->annual_layer_number == 1)
	{
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n%s \t%8s  \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s\n\n",
					"YEAR", "MONTH", "DAY", "GPP(0)", "GPP(tot)", "AR(0)", "AR(tot)", "Cf", "NPP(0)", "NPP(tot)","CE(0)", "CE(tot)", "ASW", "LAI(0)", "CC(0)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		Daily_Log ("%d \t%8d \t%8d \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8d \t%8d \n",
				yos[years].year, month+1, day+1, c->daily_gpp[0], c->daily_tot_gpp,
				c->daily_aut_resp[0], c->daily_tot_aut_resp,
				c->daily_c_flux,
				c->daily_npp[0], c->daily_tot_npp,
				c->daily_c_evapotransp[0],	c->daily_tot_c_evapotransp, c->available_soil_water,
				c->daily_lai[0], c->daily_cc[0]*100, c->daily_dead_tree[0], c->daily_tot_dead_tree);


		previous_layer_number = c->annual_layer_number;

		//reset
		c->daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_npp[0] = 0;
		c->daily_c_int[0] = 0;
		c->daily_c_transp[0] = 0;
		c->daily_c_evapotransp[0] = 0;
		c->daily_et[0] = 0;
		c->daily_cc[0] = 0;
		c->daily_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n%s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s  \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s\n\n",
					"YEAR", "MONTH", "DAY", "GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR", "AR(tot)", "NPP(1)", "NPP(0)", "NPP(tot)","CE(1)", "CE(0)", "CE(tot)","ASW",
					"LAI(1)", "LAI(0)", "CC(1)", "CC(0)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		Daily_Log ("%d \t%8d \t%8d \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8d \t%8d \t%8d\n",
				yos[years].year, month+1, day+1,
				c->daily_gpp[1], c->daily_gpp[0], c->daily_tot_gpp,
				c->daily_aut_resp[1], c->daily_aut_resp[0], c->daily_tot_aut_resp,
				c->daily_npp[1], c->daily_npp[0], c->daily_tot_npp,
				c->daily_c_evapotransp[1], c->daily_c_evapotransp[0], c->daily_tot_c_evapotransp,
				c->available_soil_water,
				c->daily_lai[1], c->daily_lai[0],
				c->daily_cc[1]*100, c->daily_cc[0]*100,
				c->daily_dead_tree[1], c->daily_dead_tree[0], c->daily_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//reset
		c->daily_gpp[1] = 0;
		c->daily_aut_resp[1] = 0;
		c->daily_npp[1] = 0;
		c->daily_c_int[1] = 0;
		c->daily_c_transp[1] = 0;
		c->daily_c_evapotransp[1] = 0;
		c->daily_et[1] = 0;
		c->daily_cc[1] = 0;
		c->daily_dead_tree[1] = 0;

		c->daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_npp[0] = 0;
		c->daily_c_int[0] = 0;
		c->daily_c_transp[0] = 0;
		c->daily_c_evapotransp[0] = 0;
		c->daily_et[0] = 0;
		c->daily_cc[0] = 0;
		c->daily_dead_tree[0] = 0;

	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n\t%s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%7s \t%7s \t%7s \t%7s\n\n",
					"YEAR", "MONTH", "DAY", "GPP(2)","GPP(1)", "GPP(0)", "GPP(tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)", "NPP(2)","NPP(1)", "NPP(0)", "NPP(0)", "NPP(tot)",
					"CE(2)","CE(1)", "CE(0)", "ASW",
					"LAI(2)","LAI(1)", "LAI(0)", "CC(2)", "CC(1)", "CC(0)", "DEADTREE(2)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		Daily_Log ("%d \t%8d \t%8d \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8.3g \t%8d \t%8d \t%8d \t%8d\n",
				yos[years].year, month+1, day+1,
				c->daily_gpp[2], c->daily_gpp[1],c->daily_gpp[0], c->daily_tot_gpp,
				c->daily_aut_resp[2], c->daily_aut_resp[1],c->daily_aut_resp[0], c->daily_tot_aut_resp,
				c->daily_npp[2], c->daily_npp[1],c->daily_npp[0], c->daily_tot_npp,
				c->daily_c_evapotransp[2],c->daily_c_evapotransp[1], c->daily_c_evapotransp[0], c->daily_tot_c_evapotransp,
				c->available_soil_water,
				c->daily_lai[2], c->daily_lai[1], c->daily_lai[0],
				c->daily_cc[2]*100, c->daily_cc[1]*100,c->daily_cc[0]*100,
				c->daily_dead_tree[2], c->daily_dead_tree[1], c->daily_dead_tree[0], c->daily_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//reset
		c->daily_gpp[2] = 0;
		c->daily_aut_resp[2] = 0;
		c->daily_npp[2] = 0;
		c->daily_c_int[2] = 0;
		c->daily_c_transp[2] = 0;
		c->daily_c_evapotransp[2] = 0;
		c->daily_et[2] = 0;
		c->daily_cc[2] = 0;
		c->daily_dead_tree[2] = 0;

		c->daily_gpp[1] = 0;
		c->daily_aut_resp[1] = 0;
		c->daily_npp[1] = 0;
		c->daily_c_int[1] = 0;
		c->daily_c_transp[1] = 0;
		c->daily_c_evapotransp[1] = 0;
		c->daily_et[1] = 0;
		c->daily_cc[1] = 0;
		c->daily_dead_tree[1] = 0;

		c->daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_npp[0] = 0;
		c->daily_c_int[0] = 0;
		c->daily_c_transp[0] = 0;
		c->daily_c_evapotransp[0] = 0;
		c->daily_et[0] = 0;
		c->daily_cc[0] = 0;
		c->daily_dead_tree[0] = 0;



	}

	c->daily_tot_gpp = 0;
	c->daily_tot_aut_resp = 0;
	c->daily_tot_npp = 0;
	c->daily_tot_c_int = 0;
	c->daily_tot_c_transp = 0;
	c->daily_tot_c_evapotransp = 0;
	c->daily_tot_et = 0;
	c->daily_tot_dead_tree = 0;


	//reset after printed at the end of the day
	/*
	c->daily_gpp = 0;
	c->daily_npp = 0;
	c->daily_et = 0;
	c->daily_lai = 0;
	 */
	c->daily_f_sw = 0;
	c->daily_f_psi = 0;
	c->daily_f_t = 0;
	c->daily_f_vpd = 0;

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
