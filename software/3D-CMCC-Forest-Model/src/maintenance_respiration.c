/*
 * maintenance_respiration.c
 *
 *  Created on: 25/set/2013
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


//FOLLOWING BIOME-BGC

void Get_maintenance_respiration (SPECIES *s, CELL *const c, const MET_DATA *const met, int month, int day, int height)
{

	//maintenance respiration routine
	//Uses reference values at 20 deg C and an empirical relationship between
	//tissue N content and respiration rate given in:

	//Ryan, M.G., 1991. Effects of climate change on plant respiration.
	//Ecological Applications, 1(2):157-167.

	//Uses the same value of Q_10 (2.0) for all compartments, leaf, stem,
	//coarse and fine roots.

	//From Ryan's figures and regressions equations, the maintenance respiration
	//in kgC/day per kg of tissue N is:
	//mrpern = 0.218 (kgC/kgN/d)

	//Leaf maintenance respiration is calculated separately for day and
	//night, since the PSN routine needs the daylight value.

	//Leaf and fine root respiration are dependent on phenology.

	float t1;
	float q10 = 2.0;
	float mrpern = 0.218; //linear N relationship with MR being kgC/kgN/day
	float exponent;
	//float n_area_sun, n_area_shade, dlmr_area_sun, dlmr_area_shade;

	float n_area/*, dlmr_area*/;

	float leaf_nitrogen;
	float fine_root_nitrogen;
	float coarse_root_nitrogen;
	float stem_nitrogen;

	float tsoil;


	Log("\nGET_MAINTENANCE_RESPIRATION\n");

	//TODO FIND A BETTER EQUATIONS FOR TSOIL (BIOME uses a 11 day weighted average of tavg)
	tsoil = met[month].d[day].tavg;


	// leaf day and night maintenance respiration when leaves on
	if (s->counter[VEG_UNVEG] == 1)
	{
		Log("--FOLIAGE\n");

		//convert biomass foliage from tons of DM to grams of Carbon then compute Nitrogen content using CN ratio
		leaf_nitrogen = ((s->value[BIOMASS_FOLIAGE_CTEM] / 2.0) * 1000000.0) / s->value[CN_LEAVES];
		Log("Foliage nitrogen content = %g gN/cell\n", leaf_nitrogen);
		t1 = leaf_nitrogen * mrpern;


		// leaf, day
		exponent = (met[month].d[day].tday - 20.0) / 10.0;
		//changing from biome
		//n_area = 1.0/(s->value[SLA]*s->value[CN_LEAVES]);
		//todo not clear why divide for 24?!?
		s->value[DAILY_LEAF_MAINT_RESP] = (t1 * pow(q10, exponent) * (met[month].d[day].daylength/24.0)/(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]));
		Log("daily leaf maintenance respiration = %g gC/day m^2\n", s->value[DAILY_LEAF_MAINT_RESP]);



		//for day respiration, also determine rates of maintenance respiration
		//per unit of projected leaf area in the sunlit and shaded portions of
		//the canopy, for use in the photosynthesis routine
		//first, calculate the mass of N per unit of projected leaf area
		//in each canopy fraction (kg N/m2 projected area)
		//n_area_sun   = 1.0/(epv->sun_proj_sla * epc->leaf_cn);
		//n_area_shade = 1.0/(epv->shade_proj_sla * epc->leaf_cn);



		//convert to respiration flux in kg C/m2 projected area/day, and
		//correct for temperature
		//dlmr_area_sun   = n_area_sun * mrpern * pow(q10, exponent);
		//dlmr_area_shade = n_area_shade * mrpern * pow(q10, exponent);

		//changing from biome
		//convert to effective area occupied by foliage and from kG of C to tons of DM
		//dlmr_area = ((n_area * mrpern * pow(q10, exponent) * (s->value[CANOPY_COVER]/settings->sizeCell)) * 2.0)/ 1000;
		//finally, convert from mass to molar units, and from a daily rate to
		//a rate per second

		//epv->dlmr_area_sun = dlmr_area_sun/(86400.0 * 12.011e-9);
		//epv->dlmr_area_shade = dlmr_area_shade/(86400.0 * 12.011e-9);

		// leaf, night
		exponent = (met[month].d[day].tnight - 20.0) / 10.0;
		s->value[NIGHTLY_LEAF_MAINT_RESP]= (t1 * pow(q10, exponent) * ((24.0 - met[month].d[day].daylength)/24.0)/(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]));
		Log("nightly leaf maintenance respiration = %g gC/day m^2\n", s->value[NIGHTLY_LEAF_MAINT_RESP]);
		//day+night
		s->value[TOT_DAY_LEAF_MAINT_RESP]= s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];
		Log("Total daily leaf maintenance respiration = %g gC/day m^2\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);
	}
	else //no leaves on
	{
		s->value[DAILY_LEAF_MAINT_RESP] = 0.0;
		s->value[NIGHTLY_LEAF_MAINT_RESP] = 0.0;
		s->value[TOT_DAY_LEAF_MAINT_RESP]= 0.0;
	}



	// fine root maintenance respiration when fine roots on
	// ammended to consider only the specified n concentration,
	//to avoid excessive MR with n-loading to fine roots
	if (s->counter[VEG_UNVEG] == 1)
	{
		Log("--FINE ROOT\n");
		exponent = (tsoil - 20.0) / 10.0;
		t1 = pow(q10, exponent);

		//convert biomass root from tons of DM to grams of Carbon then compute Nitrogen content using CN ratio
		fine_root_nitrogen = ((s->value[BIOMASS_ROOTS_FINE_CTEM] / 2.0)*1000000.0) / s->value[CN_FINE_ROOTS];
		Log("Fine root nitrogen content = %g gN/cell\n", fine_root_nitrogen);
		s->value[FINE_ROOT_MAINT_RESP] = ((leaf_nitrogen * mrpern * t1)/(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]));
		Log("Fine root maintenance respiration = %g gC/day m^2\n", s->value[FINE_ROOT_MAINT_RESP]);
	}
	else //no fine roots on
	{
		s->value[FINE_ROOT_MAINT_RESP] = 0.0;
	}



	// TREE-specific fluxes
	Log("--STEM\n");

	//convert biomass foliage from tones of DM to grams of Carbon then compute Nitrogen content using CN ratio
	stem_nitrogen = ((s->value[WS_sap]/2.0)*1000000.0) / s->value[CN_LIVE_WOODS];
	Log("Stem nitrogen content = %g gN/cell\n", stem_nitrogen);

	// live stem maintenance respiration
	exponent = (met[month].d[day].tavg - 20.0) / 10.0;
	t1 = pow(q10, exponent);
	//todo NOT CLEAR FOR WHAT DIVIDE
	s->value[STEM_MAINT_RESP] = ((stem_nitrogen * mrpern * t1)/settings->sizeCell);
	Log("Stem maintenance respiration = %g gC/day m^2\n", s->value[STEM_MAINT_RESP]);


	//IT MUST TAKES INTO ACCOUNT ONLY LIVE roots NOT ALL roots!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Log("--COARSE ROOT\n");

	//convert biomass foliage from tons of DM to grams of Carbon then compute Nitrogen content using CN ratio
	coarse_root_nitrogen = ((s->value[BIOMASS_ROOTS_COARSE_CTEM]/2.0)*1000000.0) / s->value[CN_LIVE_WOODS];
	Log("coarse root nitrogen content = %g gN/cell\n", coarse_root_nitrogen);

	//live coarse root maintenance respiration
	exponent = (tsoil - 20.0) / 10.0;
	t1 = pow(q10, exponent);
	s->value[COARSE_ROOT_MAINT_RESP] = ((coarse_root_nitrogen * mrpern * t1)/(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]));
	Log("Coarse root maintenance respiration = %g gC/day m^2\n", s->value[COARSE_ROOT_MAINT_RESP]);



	//COMPUTE TOTAL MAINTENANCE RESPIRATION
	s->value[TOTAL_MAINT_RESP]= s->value[TOT_DAY_LEAF_MAINT_RESP]+s->value[FINE_ROOT_MAINT_RESP]+ s->value[STEM_MAINT_RESP]+s->value[COARSE_ROOT_MAINT_RESP];
	Log("TOTAL maintenance respiration = %g gC/day m^2\n", s->value[TOTAL_MAINT_RESP]);


	//TODO change all if with a for

	if (c->annual_layer_number == 1)
	{
		c->daily_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
		c->monthly_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
		c->annual_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->daily_maint_resp[1] += s->value[TOTAL_MAINT_RESP];
			c->monthly_maint_resp[1] += s->value[TOTAL_MAINT_RESP];
			c->annual_maint_resp[1] += s->value[TOTAL_MAINT_RESP];
		}
		else
		{
			c->daily_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
			c->monthly_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
			c->annual_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->daily_maint_resp[2] += s->value[TOTAL_MAINT_RESP];
			c->monthly_maint_resp[2] += s->value[TOTAL_MAINT_RESP];
			c->annual_maint_resp[2] += s->value[TOTAL_MAINT_RESP];
		}
		if (c->heights[height].z == 1)
		{
			c->daily_maint_resp[1] += s->value[TOTAL_MAINT_RESP];
			c->monthly_maint_resp[1] += s->value[TOTAL_MAINT_RESP];
			c->annual_maint_resp[1] += s->value[TOTAL_MAINT_RESP];
		}
		if (c->heights[height].z == 0)
		{
			c->daily_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
			c->monthly_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
			c->annual_maint_resp[0] += s->value[TOTAL_MAINT_RESP];
		}
	}

	c->daily_tot_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->monthly_tot_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->annual_tot_maint_resp += s->value[TOTAL_MAINT_RESP];


	//todo see if could be better to multiply for ratio N/C
	//convert to tDM/ cell resolution
	//upscale to cell level and convert from gC to tons of DM
	s->value[DAILY_LEAF_MAINT_RESP] *= 2.0 / 1000000.0 *(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]);
	//Log("daily leaf maintenance respiration = %g tDM/day cell\n", s->value[DAILY_LEAF_MAINT_RESP]);

	s->value[NIGHTLY_LEAF_MAINT_RESP] *=  2.0/ 1000000.0 *(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]);
	//Log("nightly leaf maintenance respiration = %g tDM/day cell\n", s->value[DAILY_LEAF_MAINT_RESP]);

	//day+night
	s->value[TOT_DAY_LEAF_MAINT_RESP]= s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];
	Log("Total daily leaf maintenance respiration = %g tDM/ha cell\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);

	s->value[FINE_ROOT_MAINT_RESP] *=  2.0/ 1000000.0 *(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]);
	Log("Fine root maintenance respiration = %g tDM/day cell\n", s->value[FINE_ROOT_MAINT_RESP]);

	s->value[STEM_MAINT_RESP] *=  2.0/ 1000000.0 *(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]);
	Log("Stem maintenance respiration = %g tDM/day cell\n", s->value[STEM_MAINT_RESP]);

	s->value[COARSE_ROOT_MAINT_RESP] *=  2.0/ 1000000.0 *(settings->sizeCell * s->value[CANOPY_COVER_DBHDC]);
	Log("Coarse root maintenance respiration = %g tDM/day cell\n", s->value[COARSE_ROOT_MAINT_RESP]);

	//COMPUTE TOTAL MAINTENANCE RESPIRATION
	s->value[TOTAL_MAINT_RESP]= s->value[TOT_DAY_LEAF_MAINT_RESP]+s->value[FINE_ROOT_MAINT_RESP]+ s->value[STEM_MAINT_RESP]+s->value[COARSE_ROOT_MAINT_RESP];
	Log("TOTAL maintenance respiration = %g tDM/day cell\n", s->value[TOTAL_MAINT_RESP]);


}


