/*
 * print.c
 *
 *  Created on: 01/apr/2016
 *      Author: alessio
 */
#include <math.h>
#include "types.h"
#include "constants.h"

void Print_parameters (SPECIES *const s, int species_count, int month, int years)
{
	int species;

	if (!month && !years)
	{
		for (species = 0; species < species_count; species++)
		{
			Log("\n\n\n\n***********************************************************************************\n");
			Log("VALUE LIST FOR THE MAIN PARAMETERS FOR = %s\n", s->name);
			Log("ALPHA= %f \n"
					"EPSILONgCMJ= %f \n"
					//"Y= %f \n"
					"K= %f \n"
					"INT_COEFF = %f"
					"LAIGCX = %f \n"
					"LAIMAXINTCPTN = %f \n"
					"MAXINTCPTN = %f \n"
					"SLA = %f \n"
					"FRACBB0 = %f \n"
					"FRACBB1 = %f \n"
					"TBB = %f \n"
					"RHOMIN = %f \n"
					"RHOMAX = %f \n"
					"TRHO = %f \n"
					"COEFFCOND = %f \n"
					"BLCOND = %f \n"
					"MAXCOND = %f \n"
					"MAXAGE = %f \n"
					"RAGE = %f \n"
					"NAGE = %f \n"
					"GROWTHTMIN = %f \n"
					"GROWTHTMAX = %f \n"
					"GROWTHTOPT = %f \n"
					"GROWTHSTART = %f \n"
					"GROWTHEND = %f \n"
					"STEMCONST = %f \n"
					"STEMPOWER_A = %f \n"
					"STEMPOWER_B = %f \n"
					"STEMPOWER_C = %f \n",
					s->value[ALPHA],
					s->value[EPSILONgCMJ],
					//s->value[Y],
					s->value[K],
					s->value[INT_COEFF],
					s->value[LAIGCX],
					s->value[LAIMAXINTCPTN],
					s->value[MAXINTCPTN],
					s->value[FRACBB0],
					s->value[FRACBB1],
					s->value[TBB],
					s->value[RHOMIN],
					s->value[RHOMAX],
					s->value[TRHO],
					s->value[COEFFCOND],
					s->value[BLCOND],
					s->value[MAXCOND],
					s->value[MAXAGE],
					s->value[RAGE],
					s->value[NAGE],
					s->value[GROWTHTMIN],
					s->value[GROWTHTMAX],
					s->value[GROWTHTOPT],
					s->value[GROWTHSTART],
					s->value[GROWTHEND],
					s->value[STEMCONST],
					STEMPOWER_A,
					STEMPOWER_B,
					STEMPOWER_C
			);
		}
	}
}

void Print_met_data (const MET_DATA *const met, int month, int day)
{
	//here is valid only into function
	static int doy;

	if (day == 0 && month == 0)
	{
		doy = 0;
	}
	doy += 1;

	Log("***************\n");
	Log("**Daily MET DATA day %d month %d**\n", met[month].d[day].n_days, month+1);
	Log("-solar_rad = %.2f MJ/m^2/day\n"
			"-tavg = %.2f °C\n"
			"-tmax = %.2f °C\n"
			"-tmin = %.2f °C\n"
			"-tday (computed)= %.2f °C\n"
			"-tnight (computed)= %.2f °C\n"
			"-tsoil (computed)= %.2f °C\n"
			"-rh = %.2f %%\n"
			"-vpd = %.2f mbar\n"
			"-ts_f = %.2f °C\n"
			"-rain = %.2f mm\n"
			"-swc = %.2f %%vol\n"
			"-thermic_sum = %.2f °C\n"
			"-daylength = %.2f hrs\n"
			"-co2 concentration = %.2f ppmv\n"
			"-DOY = %d\n"

			//"-month avg temp = %.2f °C\n"
			//"-month cum rain = %.2f mm\n"
			,met[month].d[day].solar_rad,
			met[month].d[day].tavg,
			met[month].d[day].tmax,
			met[month].d[day].tmin,
			met[month].d[day].tday,
			met[month].d[day].tnight,
			met[month].d[day].tsoil,
			met[month].d[day].rh_f,
			met[month].d[day].vpd,
			met[month].d[day].ts_f,
			met[month].d[day].prcp,
			met[month].d[day].swc,
			met[month].d[day].thermic_sum,
			met[month].d[day].daylength,
			settings->co2Conc,
			doy
			//,met[month].avg_monthly_temp
			//,met[month].cum_monthly_rain
	);
	if (settings->spatial == 's')
	{
		Log("-lai from NDVI = %f \n", met[month].d[day].ndvi_lai);
	}


	Log("***************\n");

}

void Print_met_daily_data (const YOS *const yos, int day, int month, int years)
{
	MET_DATA *met;
	assert(yos);
	met = (MET_DATA*) yos[years].m;

	if (settings->time == 'd')
	{
		Log("n_days %10d "
				"Rg_f %10g "
				"Tavg %10g "
				"Tmax %10g "
				"Tmin %10g "
				"Precip %10g "
				"Tday %10g "
				"Tnight %10g \n",
				met[month].d[day].n_days,
				met[month].d[day].solar_rad,
				met[month].d[day].tavg,
				met[month].d[day].tmax,
				met[month].d[day].tmin,
				met[month].d[day].prcp,
				met[month].d[day].tday,
				met[month].d[day].tnight);
	}

}
void Print_init_month_stand_data (CELL *c, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("******************************************************\n\n");
	Log("cell = \n");
	Log("- x = %d\n", c->x);
	Log("- y = %d\n", c->y);
	Log("- z = %d\n", c->heights[height].z);
	Log("- Class Age = %d years \n", c->heights[height].ages[age].value);
	Log("- Species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("- DBH = %f\n", c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("- Height = %f m\n", c->heights[height].value);
	Log("- Number of trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);
	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
	{
		Log("- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	}
	else
	{
		Log("- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	}
	Log("- Daily LAI from Model= %f \n", c->heights[height].ages[age].species[species].value[LAI]);
	Log("- Crown Diameter = %f \n",  c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);
	Log("- Canopy Cover = %f \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);
	Log("- ASW layer %d month %d  = %f mm\n",  c->heights[height].z, month + 1, c->asw);

	if (!month)
	{
		/*Phenology*/
		Log("- Phenology type = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
		/*Management*/
		Log("- Management type = %s\n", c->heights[height].ages[age].species[species].management ? "C" : "T");
		//Log("+ Lai = %f\n", c->heights[height].ages[age].species[species].value[LAI]);
		Log("+ AvDBH = %f cm\n",  c->heights[height].ages[age].species[species].value[AVDBH]);
		Log("--Carbon pools in tC\n");
		Log("+ Wf = %f tC/area\n", c->heights[height].ages[age].species[species].value[LEAF_C]);
		Log("+ Wts = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_STEM_C]);
		Log("+ Ws = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_C]);
		Log("+ Wbb = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_C]);
		Log("+ Wrc = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]);
		Log("+ Wrf = %f tC/area\n", c->heights[height].ages[age].species[species].value[FINE_ROOT_C]);
		Log("+ Wr Tot = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_ROOT_C]);
		Log("+ Wres = %f tC/area\n", c->heights[height].ages[age].species[species].value[RESERVE_C]);
		Log("+ Ws live = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]);
		Log("+ Wrc live = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]);
		Log("+ wbb live = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]);
	}

}

void Print_end_month_stand_data (CELL *c, const YOS *const yos, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("> x = %d\n", c->x);
	Log("> y = %d\n", c->y);
	Log("> z = %d\n", c->heights[height].z);
	Log("> height = %f\n", c->heights[height].value);
	Log("> age = %d\n", c->heights[height].ages[age].value);
	Log("> species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("> phenology = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
	Log("> management = %d\n", c->heights[height].ages[age].species[species].management);
	Log(">n tree = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log(">avdbh = %f\n", c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("\n--Carbon pools in tC\n");
	Log("+ Wf = %f tC/area\n", c->heights[height].ages[age].species[species].value[LEAF_C]);
	Log("+ Wts = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_STEM_C]);
	Log("+ Ws = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_C]);
	Log("+ Wbb = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_C]);
	Log("+ Wrc = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]);
	Log("+ Wrf = %f tC/area\n", c->heights[height].ages[age].species[species].value[FINE_ROOT_C]);
	Log("+ Wr Tot = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_ROOT_C]);
	Log("+ Wres = %f tC/area\n", c->heights[height].ages[age].species[species].value[RESERVE_C]);
	Log("+ Ws live = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]);
	Log("+ Wrc live = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]);
	Log("+ wbb live = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]);

	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 1.2 )
	{
		Log("[%d] layer %d > wcones = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_FRUIT_tDM]);
	}
	Log("> Dead Trees = %d\n",c->heights[height].ages[age].species[species].counter[DEAD_STEMS]);
	Log("> New Saplings = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE_SAP]);
	Log("*****************************\n");
}

