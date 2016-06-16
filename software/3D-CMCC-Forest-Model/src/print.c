/*
 * print.c
 *
 *  Created on: 01/apr/2016
 *      Author: alessio
 */
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void Print_parameters (SPECIES *const s, int species_count, int month, int years)
{
	int species;

	if (!month && !years)
	{
		for (species = 0; species < species_count; species++)
		{
			logger(g_log, "\n\n\n\n***********************************************************************************\n");
			logger(g_log, "VALUE LIST FOR THE MAIN PARAMETERS FOR = %s\n", s->name);
			logger(g_log, "ALPHA= %f \n"
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

	logger(g_log, "***************\n");
	logger(g_log, "**Daily MET DATA day %d month %d**\n", met[month].d[day].n_days, month+1);
	logger(g_log, "-solar_rad = %.2f MJ/m^2/day\n"
			"-tavg = %.2f °C\n"
			"-tmax = %.2f °C\n"
			"-tmin = %.2f °C\n"
			"-tday (computed)= %.2f °C\n"
			"-tnight (computed)= %.2f °C\n"
			"-tsoil (computed)= %.2f °C\n"
			"-tdew (computed)= %.2f °C\n"
			"-relative humidity = %.2f %%\n"
			"-vpd = %.2f mbar/hPa\n"
			"-ts_f (measured)= %.2f °C\n"
			"-precip = %.2f mm\n"
			"-swc (computed/measured)= %.2f %%vol\n"
			"-thermic_sum = %.2f °C\n"
			"-rho air = %.2f kg/m3\n"
			"-daylength = %.2f hrs\n"
			"-wind speed = %.2f m/sec\n"
			"-air pressure = %.2f Pa\n"
			"-es = %.2f KPa\n"
			"-ea = %.2f KPa\n"
			"-air psych = %.2f KPa\n"
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
			met[month].d[day].tdew,
			met[month].d[day].rh_f,
			met[month].d[day].vpd,
			met[month].d[day].ts_f,
			met[month].d[day].prcp,
			met[month].d[day].swc,
			met[month].d[day].thermic_sum,
			met[month].d[day].rho_air,
			met[month].d[day].daylength,
			met[month].d[day].windspeed,
			met[month].d[day].air_pressure,
			met[month].d[day].es,
			met[month].d[day].ea,
			met[month].d[day].psych,
			settings->co2Conc,
			doy
	);
	if (settings->spatial == 's')
	{
		logger(g_log, "-lai from NDVI = %f \n", met[month].d[day].ndvi_lai);
	}


	logger(g_log, "***************\n");

}

void Print_met_daily_data (const YOS *const yos, int day, int month, int years)
{
	MET_DATA *met;
	assert(yos);
	met = (MET_DATA*) yos[years].m;

	if (settings->time == 'd')
	{
		logger(g_log, "n_days %10d "
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
	logger(g_log, "******************************************************\n\n");
	logger(g_log, "cell = \n");
	logger(g_log, "- x = %d\n", c->x);
	logger(g_log, "- y = %d\n", c->y);
	logger(g_log, "- z = %d\n", c->heights[height].z);
	logger(g_log, "- Class Age = %d years \n", c->heights[height].ages[age].value);
	logger(g_log, "- Species = %s\n", c->heights[height].ages[age].species[species].name);
	logger(g_log, "- DBH = %f\n", c->heights[height].ages[age].species[species].value[AVDBH]);
	logger(g_log, "- Height = %f m\n", c->heights[height].value);
	logger(g_log, "- Number of trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);
	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
	{
		logger(g_log, "- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	}
	else
	{
		logger(g_log, "- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	}
	logger(g_log, "- Daily LAI from Model= %f \n", c->heights[height].ages[age].species[species].value[LAI]);
	logger(g_log, "- Crown Diameter = %f \n",  c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);
	logger(g_log, "- Canopy Cover = %f \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);
	logger(g_log, "- ASW layer %d month %d  = %f mm\n",  c->heights[height].z, month + 1, c->asw);

	if (!month)
	{
		/*Phenology*/
		logger(g_log, "- Phenology type = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
		/*Management*/
		logger(g_log, "- Management type = %s\n", c->heights[height].ages[age].species[species].management ? "C" : "T");
		//logger(g_log, "+ Lai = %f\n", c->heights[height].ages[age].species[species].value[LAI]);
		logger(g_log, "+ AvDBH = %f cm\n",  c->heights[height].ages[age].species[species].value[AVDBH]);
		logger(g_log, "--Carbon pools in tC\n");
		logger(g_log, "+ Wf = %f tC/area\n", c->heights[height].ages[age].species[species].value[LEAF_C]);
		logger(g_log, "+ Wts = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_STEM_C]);
		logger(g_log, "+ Ws = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_C]);
		logger(g_log, "+ Wbb = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_C]);
		logger(g_log, "+ Wrc = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]);
		logger(g_log, "+ Wrf = %f tC/area\n", c->heights[height].ages[age].species[species].value[FINE_ROOT_C]);
		logger(g_log, "+ Wr Tot = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_ROOT_C]);
		logger(g_log, "+ Wres = %f tC/area\n", c->heights[height].ages[age].species[species].value[RESERVE_C]);
		logger(g_log, "+ Ws live = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]);
		logger(g_log, "+ Wrc live = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]);
		logger(g_log, "+ wbb live = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]);
	}

}

void Print_end_month_stand_data (CELL *c, const YOS *const yos, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	logger(g_log, "> x = %d\n", c->x);
	logger(g_log, "> y = %d\n", c->y);
	logger(g_log, "> z = %d\n", c->heights[height].z);
	logger(g_log, "> height = %f\n", c->heights[height].value);
	logger(g_log, "> age = %d\n", c->heights[height].ages[age].value);
	logger(g_log, "> species = %s\n", c->heights[height].ages[age].species[species].name);
	logger(g_log, "> phenology = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
	logger(g_log, "> management = %d\n", c->heights[height].ages[age].species[species].management);
	logger(g_log, ">n tree = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE]);
	logger(g_log, ">avdbh = %f\n", c->heights[height].ages[age].species[species].value[AVDBH]);
	logger(g_log, "\n--Carbon pools in tC\n");
	logger(g_log, "+ Wf = %f tC/area\n", c->heights[height].ages[age].species[species].value[LEAF_C]);
	logger(g_log, "+ Wts = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_STEM_C]);
	logger(g_log, "+ Ws = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_C]);
	logger(g_log, "+ Wbb = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_C]);
	logger(g_log, "+ Wrc = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]);
	logger(g_log, "+ Wrf = %f tC/area\n", c->heights[height].ages[age].species[species].value[FINE_ROOT_C]);
	logger(g_log, "+ Wr Tot = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_ROOT_C]);
	logger(g_log, "+ Wres = %f tC/area\n", c->heights[height].ages[age].species[species].value[RESERVE_C]);
	logger(g_log, "+ Ws live = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]);
	logger(g_log, "+ Wrc live = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]);
	logger(g_log, "+ wbb live = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]);

	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 1.2 )
	{
		logger(g_log, "[%d] layer %d > wcones = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_FRUIT_tDM]);
	}
	logger(g_log, "> Dead Trees = %d\n",c->heights[height].ages[age].species[species].counter[DEAD_STEMS]);
	logger(g_log, "> New Saplings = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE_SAP]);
	logger(g_log, "*****************************\n");
}

