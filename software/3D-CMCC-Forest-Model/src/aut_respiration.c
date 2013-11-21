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

	int i;
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
	float mrpern = 0.1584; //0.218; //linear N relationship with MR being kgC/kgN/day, 0.218 from Ryan 1991, 0.1584 Campili et al., 2013 and from Dufrene et al 2005
	float exponent_tday;
	float exponent_tnight;
	float exponent_tavg;
	float exponent_tsoil;
	//float n_area_sun, n_area_shade, dlmr_area_sun, dlmr_area_shade;

	//float n_area/*, dlmr_area*/;

	float leaf_nitrogen;
	float fine_root_nitrogen;
	float coarse_root_nitrogen;
	float stem_nitrogen;
	float branch_nitrogen;

	Log("\nGET_MAINTENANCE_RESPIRATION\n");
	/*
	//computing Nitrogen content from tons DM/ha to gC/m^2 and then as in BIOME to KgC
	leaf_nitrogen = (((s->value[BIOMASS_FOLIAGE_CTEM] / GC_GDM) * 1000.0) /settings->sizeCell) / s->value[CN_LEAVES];
	Log("Foliage nitrogen content = %g kgN/m^2\n", leaf_nitrogen);
	fine_root_nitrogen = (((s->value[BIOMASS_ROOTS_FINE_CTEM] / GC_GDM)*1000.0)/settings->sizeCell) / s->value[CN_FINE_ROOTS];
	Log("Fine root nitrogen content = %g kgN/m^2\n", fine_root_nitrogen);
	stem_nitrogen = ((((s->value[BIOMASS_STEM_LIVE_WOOD])/GC_GDM)*1000.0)/settings->sizeCell) / s->value[CN_LIVE_WOODS];
	Log("Live stem nitrogen content = %g kgN/m^2\n", stem_nitrogen);
	coarse_root_nitrogen = ((((s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD])/GC_GDM)*1000.0)/settings->sizeCell) / s->value[CN_LIVE_WOODS];
	Log("Live coarse root nitrogen content = %g kgN/cell\n", coarse_root_nitrogen);
	branch_nitrogen = ((((s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD])/GC_GDM)*1000.0)/settings->sizeCell) / s->value[CN_LIVE_WOODS];
	Log("Live branch nitrogen content = %g kgN/cell\n", branch_nitrogen);
	 */
	// leaf day and night maintenance respiration when leaves on
	if (s->counter[VEG_UNVEG] == 1)
	{

		/*foliage*/
		exponent_tday = (met[month].d[day].tday - 20.0) / 10.0;

		t1 = pow(q10, exponent_tday);

		s->value[DAILY_LEAF_MAINT_RESP] = (s->value[LEAF_NITROGEN] * mrpern * t1 * (met[month].d[day].daylength/24.0))*1000.0;
		//Log("daily leaf maintenance respiration = %g gC/day m^2\n", s->value[DAILY_LEAF_MAINT_RESP]);



		exponent_tnight =  (met[month].d[day].tnight - 20.0) / 10.0;

		t1 = pow(q10, exponent_tnight);

		s->value[NIGHTLY_LEAF_MAINT_RESP]= (s->value[LEAF_NITROGEN] * mrpern * t1 * (1.0 - (met[month].d[day].daylength/24.0)))*1000.0;
		//Log("nightly leaf maintenance respiration = %g gC/day m^2\n", s->value[NIGHTLY_LEAF_MAINT_RESP]);

		s->value[TOT_DAY_LEAF_MAINT_RESP]= s->value[DAILY_LEAF_MAINT_RESP] + s->value[NIGHTLY_LEAF_MAINT_RESP];
		Log("BIOME Total daily leaf maintenance respiration = %g gC/day m^2\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);


		/*fine roots*/

		exponent_tsoil = (met[month].d[day].tsoil - 20.0) / 10.0;
		t1 = pow(q10, exponent_tsoil);


		s->value[FINE_ROOT_MAINT_RESP] = (s->value[FINE_ROOT_NITROGEN] * mrpern * t1)*1000.0;
		Log("BIOME Fine root maintenance respiration = %g gC/day m^2\n", s->value[FINE_ROOT_MAINT_RESP]);
	}
	else //no leaves or fine roots on
	{
		s->value[DAILY_LEAF_MAINT_RESP] = 0.0;
		s->value[NIGHTLY_LEAF_MAINT_RESP] = 0.0;
		s->value[TOT_DAY_LEAF_MAINT_RESP]= 0.0;
		Log("BIOME Total daily leaf maintenance respiration = %g gC/day m^2\n", s->value[TOT_DAY_LEAF_MAINT_RESP]);
		s->value[FINE_ROOT_MAINT_RESP] = 0.0;
		Log("BIOME Fine root maintenance respiration = %g gC/day m^2\n", s->value[FINE_ROOT_MAINT_RESP]);
	}

	// live stem maintenance respiration
	exponent_tavg = (met[month].d[day].tavg - 20.0) / 10.0;
	t1 = pow(q10, exponent_tavg);
	s->value[STEM_MAINT_RESP] = (s->value[STEM_NITROGEN] * mrpern * t1)*1000.0;
	Log("BIOME Stem maintenance respiration = %g gC/day m^2\n", s->value[STEM_MAINT_RESP]);

	//live branch maintenance respiration
	s->value[BRANCH_MAINT_RESP] = (s->value[BRANCH_NITROGEN] * mrpern * t1)*1000.0;
	Log("BIOME Branch maintenance respiration = %g gC/day m^2\n", s->value[BRANCH_MAINT_RESP]);

	//live coarse root maintenance respiration
	exponent_tsoil = (met[month].d[day].tsoil - 20.0) / 10.0;
	t1 = pow(q10, exponent_tsoil);
	s->value[COARSE_ROOT_MAINT_RESP] = (s->value[COARSE_ROOT_NITROGEN] * mrpern * t1)*1000.0;
	Log("BIOME Coarse root maintenance respiration = %g gC/day m^2\n", s->value[COARSE_ROOT_MAINT_RESP]);

	//COMPUTE TOTAL MAINTENANCE RESPIRATION
	s->value[TOTAL_MAINT_RESP]= s->value[TOT_DAY_LEAF_MAINT_RESP]+
			s->value[FINE_ROOT_MAINT_RESP]+
			s->value[STEM_MAINT_RESP]+
			s->value[COARSE_ROOT_MAINT_RESP]+
			s->value[BRANCH_MAINT_RESP];
	Log("TOTAL maintenance respiration = %g gC/day m^2\n", s->value[TOTAL_MAINT_RESP]);

	i = c->heights[height].z;
	c->daily_maint_resp[i] += s->value[TOTAL_MAINT_RESP];
	c->monthly_maint_resp[i] += s->value[TOTAL_MAINT_RESP];
	c->annual_maint_resp[i] += s->value[TOTAL_MAINT_RESP];

	c->daily_tot_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->monthly_tot_maint_resp += s->value[TOTAL_MAINT_RESP];
	c->annual_tot_maint_resp += s->value[TOTAL_MAINT_RESP];
}

//FOLLOWING BIOME-BGC
void Get_growth_respiration (SPECIES *s, CELL *const c, int height, int day, int month, int years)
{

	int i;


	Log("\nGET_GROWTH_RESPIRATION\n");

	//to prevent negative values in the first years first month first day of the simulation
	if (day == 0 && month == 0)
	{
		Log("No growth resp\n");

		s->value[LEAF_GROWTH_RESP] = 0.0;
		Log("daily leaf growth respiration = %g gC/day m^2\n", s->value[LEAF_GROWTH_RESP]);

		s->value[FINE_ROOT_GROWTH_RESP] = 0.0;
		Log("daily fine root growth respiration = %g gC/day m^2\n", s->value[FINE_ROOT_GROWTH_RESP]);

		s->value[STEM_GROWTH_RESP] = 0.0;
		Log("daily stem growth respiration = %g gC/day m^2\n", s->value[STEM_GROWTH_RESP]);

		s->value[COARSE_ROOT_GROWTH_RESP] = 0.0;
		Log("daily coarse root growth respiration = %g gC/day m^2\n", s->value[COARSE_ROOT_GROWTH_RESP]);

		s->value[BRANCH_GROWTH_RESP] = 0.0;
		Log("daily branch respiration = %g gC/day m^2\n", s->value[BRANCH_GROWTH_RESP]);

		s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] +
				s->value[FINE_ROOT_GROWTH_RESP] +
				s->value[STEM_GROWTH_RESP] +
				s->value[COARSE_ROOT_GROWTH_RESP] +
				s->value[BRANCH_GROWTH_RESP];
		Log("TOTAL growth respiration = %g gC/day m^2\n", s->value[TOTAL_GROWTH_RESP]);

	}
	else
	{
		//COMPUTE GROWTH RESPIRATION using previous day biomass increment
		if (s->value[NPP] > 0.0)
		{
			s->value[LEAF_GROWTH_RESP] = (((s->value[DEL_FOLIAGE_CTEM]/GC_GDM)*1000000)/(s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)) * GRPERC;
			Log("daily leaf growth respiration = %g gC/day m^2\n", s->value[LEAF_GROWTH_RESP]);

			s->value[FINE_ROOT_GROWTH_RESP] = (((s->value[DEL_ROOTS_FINE_CTEM])*1000000)/(s->value[CANOPY_COVER_DBHDC]* settings->sizeCell))* GRPERC;
			Log("daily fine root growth respiration = %g gC/day m^2\n", s->value[FINE_ROOT_GROWTH_RESP]);

			s->value[STEM_GROWTH_RESP] = (((s->value[DEL_STEMS_CTEM])*1000000)/(s->value[CANOPY_COVER_DBHDC]* settings->sizeCell))* GRPERC;
			Log("daily stem growth respiration = %g gC/day m^2\n", s->value[STEM_GROWTH_RESP]);

			s->value[COARSE_ROOT_GROWTH_RESP] = (((s->value[DEL_ROOTS_COARSE_CTEM])*1000000)/(s->value[CANOPY_COVER_DBHDC]* settings->sizeCell))* GRPERC;
			Log("daily coarse root growth respiration = %g gC/day m^2\n", s->value[COARSE_ROOT_GROWTH_RESP]);

			s->value[BRANCH_GROWTH_RESP] = (((s->value[DEL_BB])*1000000)/(s->value[CANOPY_COVER_DBHDC]* settings->sizeCell))* GRPERC;
			Log("daily branch respiration = %g gC/day m^2\n", s->value[BRANCH_GROWTH_RESP]);

			s->value[TOTAL_GROWTH_RESP] = s->value[LEAF_GROWTH_RESP] +
					s->value[FINE_ROOT_GROWTH_RESP] +
					s->value[STEM_GROWTH_RESP] +
					s->value[COARSE_ROOT_GROWTH_RESP] +
					s->value[BRANCH_GROWTH_RESP];
		}
		else
		{
			Log("No growth resp\n");
			s->value[TOTAL_GROWTH_RESP] = 0.0;
		}
		Log("daily total growth respiration = %g gC/day m^2\n", s->value[TOTAL_GROWTH_RESP]);
	}

	i = c->heights[height].z;

	c->daily_growth_resp[i] += s->value[TOTAL_GROWTH_RESP];
	c->monthly_gowth_resp[i] += s->value[TOTAL_GROWTH_RESP];
	c->annual_growth_resp[i] += s->value[TOTAL_GROWTH_RESP];

	c->daily_tot_growth_resp += s->value[TOTAL_GROWTH_RESP];
	c->monthly_tot_growth_resp += s->value[TOTAL_GROWTH_RESP];
	c->annual_tot_growth_resp += s->value[TOTAL_GROWTH_RESP];
}

void Get_autotrophic_respiration (SPECIES *s, CELL *const c, int height)
{
	int i;

	Log("\nGET_AUTOTROPHIC_RESPIRATION\n");

	//compute autotrophic respiration for each classes
	s->value[TOTAL_AUT_RESP] = s->value[TOTAL_GROWTH_RESP] + s->value[TOTAL_MAINT_RESP];
	Log("TOTAL autotrophic respiration = %g gC/day m^2\n", s->value[TOTAL_AUT_RESP]);
	Log("TOTAL autotrophic respiration = %g tDM/cell \n", (s->value[TOTAL_AUT_RESP] * GC_GDM /1000000) * (s->value[CANOPY_COVER_DBHDC]* settings->sizeCell));


	//compute autotrophic respiration for each layer

	i = c->heights[height].z;

	c->daily_aut_resp[i] +=s->value[TOTAL_AUT_RESP];
	c->daily_aut_resp_tDM[i] += ((s->value[TOTAL_AUT_RESP] * GC_GDM) / 1000000) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
	c->monthly_aut_resp[i] += s->value[TOTAL_AUT_RESP];
	c->annual_aut_resp[i] += s->value[TOTAL_AUT_RESP];

	c->daily_tot_aut_resp += s->value[TOTAL_AUT_RESP];
	c->monthly_tot_aut_resp += s->value[TOTAL_AUT_RESP];
	c->annual_tot_aut_resp += s->value[TOTAL_AUT_RESP];

	//TODO
	//recompute biomass pools after respiration




}




