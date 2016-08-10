/*
 * print.c
 *
 *  Created on: 01/apr/2016
 *      Author: alessio
 */
#include <math.h>
#include <assert.h>
#include "print.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void Print_parameters(species_t *const s, const int species_count, const int day, const int month, const int years)
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
					"K= %f \n"
					"INT_COEFF = %f"
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
					s->value[K],
					s->value[INT_COEFF],
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

void print_daily_met_data(const meteo_daily_t *const meteo_daily, const int day, const int month, const int year)
{

	static int doy;

	if (!day && !month)
	{
		doy = 0;
	}
	doy += 1;

	logger(g_log, "***************\n");
	logger(g_log, "**Daily MET DATA day %d month %d, year %d**\n", day + 1, month + 1, year + 1);
	logger(g_log, "-solar_rad = %.2f MJ/m^2/day\n"
			"-s-wave downward = %.2f W/m2\n"
			"-atmospheric l-wave downward (computed) = %.2f W/m2\n"
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
			,meteo_daily->solar_rad,
			meteo_daily->solar_rad* MJ_TO_W,
			meteo_daily->atm_lw_downward_W,
			meteo_daily->tavg,
			meteo_daily->tmax,
			meteo_daily->tmin,
			meteo_daily->tday,
			meteo_daily->tnight,
			meteo_daily->tsoil,
			meteo_daily->tdew,
			meteo_daily->rh_f,
			meteo_daily->vpd,
			meteo_daily->ts_f,
			meteo_daily->prcp,
			meteo_daily->swc,
			meteo_daily->thermic_sum,
			meteo_daily->rho_air,
			meteo_daily->daylength,
			meteo_daily->windspeed,
			meteo_daily->air_pressure,
			meteo_daily->es,
			meteo_daily->ea,
			meteo_daily->psych,
			g_settings->co2Conc,
			doy
	);
	if (g_settings->spatial == 's')
	{
		logger(g_log, "-lai from NDVI = %f \n", meteo_daily->ndvi_lai);
	}
	logger(g_log, "***************\n");
}
/*
void Print_met_daily_data(yos_t *const yos, const int day, const int month, const int years)
{
	meteo_t *met;

	assert(yos);

	met = yos[years].m;
	if (g_settings->time == 'd')
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
 */
void Print_stand_data(cell_t* const c, const int layer, const int height, const int age, const int species)
{
	tree_layer_t *l;
	height_t *h;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	l = &c->t_layers[layer];
	a = &h->ages[age];
	s = &a->species[species];

	/* print at the beginning of simulation class level data */
	logger(g_log, "******************************************************\n\n");
	logger(g_log, "cell = \n");
	logger(g_log, "* x = %d\n", c->x);
	logger(g_log, "* y = %d\n", c->y);
	logger(g_log, "* z = %d\n", layer);
	logger(g_log, "-class level data\n");
	logger(g_log, "- Height = %f m\n", h->value);
	logger(g_log, "- Class Age = %d years \n", a->value);
	logger(g_log, "- Species = %s\n", s->name);
	logger(g_log, "- DBH = %f\n", s->value[AVDBH]);
	logger(g_log, "- Number of trees = %d trees \n", s->counter[N_TREE]);
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		logger(g_log, "- Vegetative Days =  %d days\n", s->counter[DAY_VEG_FOR_LEAF_FALL]);
	}
	else
	{
		logger(g_log, "- Vegetative Days =  %d days\n", s->counter[DAY_VEG_FOR_LEAF_FALL]);
	}
	logger(g_log, "- Daily LAI from Model= %f \n", s->value[LAI]);
	logger(g_log, "- Crown Diameter = %f \n",  s->value[CROWN_DIAMETER_DBHDC_FUNC]);
	logger(g_log, "- Canopy Cover = %f \n", s->value[CANOPY_COVER_DBHDC]);
	logger(g_log, "- ASW = %f mm\n", c->asw);
	logger(g_log, "- Phenology type = %f\n", s->value[PHENOLOGY]);
	logger(g_log, "- Management type = %s\n", s->management ? "C" : "T");
	logger(g_log, "--Carbon pools in tC\n");
	logger(g_log, "+ Wf = %f tC/area\n", s->value[LEAF_C]);
	logger(g_log, "+ Wts = %f tC/area\n", s->value[TOT_STEM_C]);
	logger(g_log, "+ Ws = %f tC/area\n", s->value[STEM_C]);
	logger(g_log, "+ Wbb = %f tC/area\n", s->value[BRANCH_C]);
	logger(g_log, "+ Wrc = %f tC/area\n", s->value[COARSE_ROOT_C]);
	logger(g_log, "+ Wrf = %f tC/area\n", s->value[FINE_ROOT_C]);
	logger(g_log, "+ Wr Tot = %f tC/area\n", s->value[TOT_ROOT_C]);
	logger(g_log, "+ Wres = %f tC/area\n", s->value[RESERVE_C]);
	logger(g_log, "+ Ws live = %f tC/area\n", s->value[STEM_LIVE_WOOD_C]);
	logger(g_log, "+ Wrc live = %f tC/area\n", s->value[COARSE_ROOT_LIVE_WOOD_C]);
	logger(g_log, "+ wbb live = %f tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);

}


