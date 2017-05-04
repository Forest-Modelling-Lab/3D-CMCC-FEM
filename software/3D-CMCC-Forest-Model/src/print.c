/*
 * print.c
 *
 *  Created on: 01/apr/2016
 *      Author: alessio
 */
#include <math.h>
#include <assert.h>
#include "common.h"
#include "print.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern soil_settings_t* g_soil_settings;
extern logger_t* g_debug_log;

void print_daily_met_data(cell_t *c, const int day, const int month, const int year)
{
	static int doy;

	meteo_t *met;
	meteo_annual_t *meteo_annual;
	met = c->years[year].m;
	meteo_annual = &c->years[year];

	if (!day && !month)
	{
		doy = 0;
	}
	doy += 1;

	logger(g_debug_log, "***************\n");
	logger(g_debug_log, "**Daily MET DATA day %d month %d, year %d (yos %d)**\n", day + 1, month + 1, c->years[year].year, year + 1);
	logger(g_debug_log, "-%d-%d-%d\n", met[month].d[day].n_days, month+1, c->years[year].year);
	logger(g_debug_log,
			"-solar_rad = %f MJ/m^2/day\n"
			"-PAR = %f molPAR/m^2/day\n"
			"-s-wave downward = %f W/m2\n"
			"-PPFD = %f umolPAR/m^2/sec\n"
			"-atm l-wave downward (computed) = %f W/m2\n"
			"-tavg = %f °C\n"
			"-tmax = %f °C\n"
			"-tmin = %f °C\n"
			"-tday (computed)= %f °C\n"
			"-tnight (computed)= %f °C\n"
			"-tsoil (computed)= %f °C\n"
			"-tdew (computed)= %f °C\n"
			"-relative humidity = %f %%\n"
			"-vpd = %f mbar/hPa\n"
			"-ts_f (measured)= %f °C\n"
			"-precip = %f mm\n"
			"-rain = %f mm\n"
			"-snow = %f cm\n"
			"-swc (measured)= %f %%vol\n"
			"-thermic_sum = %f °C\n"
			"-rho air = %f kg/m3\n"
			"-daylength = %f hrs\n"
			"-wind speed = %f m/sec\n"
			"-air pressure = %f Pa\n"
			"-es = %f KPa\n"
			"-ea = %f KPa\n"
			"-air psych = %f KPa\n"
			"-DOY = %d\n",
			met[month].d[day].solar_rad,
			met[month].d[day].par,
			met[month].d[day].solar_rad* MJ_TO_W,
			met[month].d[day].ppfd,
			met[month].d[day].atm_lw_downward_W,
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
			met[month].d[day].rain,
			met[month].d[day].snow,
			met[month].d[day].swc,
			met[month].d[day].thermic_sum,
			met[month].d[day].rho_air,
			met[month].d[day].daylength,
			met[month].d[day].windspeed,
			met[month].d[day].air_pressure,
			met[month].d[day].es,
			met[month].d[day].ea,
			met[month].d[day].psych,
			doy
	);

	logger(g_debug_log,"-co2 concentration (variable) = %f ppmv\n", meteo_annual->co2Conc);


	if (g_settings->spatial == 's')
	{
		logger(g_debug_log, "-lai from NDVI = %f \n", met[month].d[day].ndvi_lai);
	}
	logger(g_debug_log, "***************\n");
}

void print_daily_cell_data (cell_t *const c)
{
	/* print at the beginning stand data */
	logger(g_debug_log, "******************DAILY CELL DATA********************\n\n");
	logger(g_debug_log, "cell = \n");
	logger(g_debug_log, "* x = %d\n", c->x);
	logger(g_debug_log, "* y = %d\n", c->y);
	logger(g_debug_log, "* landuse = %c\n", (LANDUSE_F == g_soil_settings->landuse) ? 'F' : 'Z');
	logger(g_debug_log, "* asw = %f mm\n", c->asw);
	logger(g_debug_log, "* swc = %f %%\n", c->swc);
	logger(g_debug_log, "* snow pack = %f cm\n", c->snow_pack);
}

void print_daily_forest_data (cell_t *const c)
{
	/* print at the beginning stand data */
	logger(g_debug_log, "**************DAILY FOREST CELL DATA**************\n\n");
	logger(g_debug_log, "* number of trees = %d\n", c->n_trees);
	logger(g_debug_log, "* number of layers = %d\n", c->tree_layers_count);
	logger(g_debug_log, "* number of height class = %d\n", c->heights_count);
	//logger(g_debug_log, "* number of height class = %d\n", c->cell_heights_count);
	//logger(g_debug_log, "* number of dbh = %d\n", c->cell_dbhs_count);
	//logger(g_debug_log, "* number of age class = %d\n", c->cell_ages_count);
	//logger(g_debug_log, "* number of species class = %d\n", c->cell_species_count);
	logger(g_debug_log, "**************************************************\n\n");


}
void print_daily_forest_class_data(cell_t* const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	tree_layer_t *l;
	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	l = &c->tree_layers[layer];
	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &d->ages[age];
	// ALESSIOR
	//if ( ! a ) return;
	s = &a->species[species];

	/* print at the beginning of simulation class level data */
	logger(g_debug_log, "**************DAILY FOREST CLASS DATASET***********\n\n");
	logger(g_debug_log, "cell = \n");
	logger(g_debug_log, "* x  = %d\n", c->x);
	logger(g_debug_log, "* y  = %d\n", c->y);
	logger(g_debug_log, "* z (layer) = %d\n", l->layer_z);
	logger(g_debug_log, "* z (height) = %d\n", h->height_z);
	logger(g_debug_log, "--class level data--\n");
	logger(g_debug_log, "- Height            = %f m\n", h->value);
	logger(g_debug_log, "- DBH               = %f cm\n", d->value);
	logger(g_debug_log, "- Class Age         = %d years \n", a->value);
	logger(g_debug_log, "- Species           = %s\n", s->name);
	logger(g_debug_log, "- Number of trees   = %d trees \n", s->counter[N_TREE]);
	logger(g_debug_log, "- Vegetative Days   =  %d days\n", s->counter[DAY_VEG_FOR_LEAF_FALL]);
	logger(g_debug_log, "- LAI_PROJ          = %f \n", s->value[LAI_PROJ]);
	logger(g_debug_log, "- Crown Diameter    = %f \n",  s->value[CROWN_DIAMETER]);
	logger(g_debug_log, "- Canopy Cover Proj = %f \n", s->value[CANOPY_COVER_PROJ]);
	logger(g_debug_log, "- Phenology type    = %f\n", s->value[PHENOLOGY]);
	logger(g_debug_log, "- Management type   = %c\n", (0 == s->management) ? 'T' : ((1 == s->management) ? 'C' : 'N'));
	logger(g_debug_log, "+ leaf              = %f tC/area\n", s->value[LEAF_C]);
	logger(g_debug_log, "+ stem              = %f tC/area\n", s->value[STEM_C]);
	logger(g_debug_log, "+ branch and bark   = %f tC/area\n", s->value[BRANCH_C]);
	logger(g_debug_log, "+ coarse root       = %f tC/area\n", s->value[CROOT_C]);
	logger(g_debug_log, "+ fine root         = %f tC/area\n", s->value[FROOT_C]);
	logger(g_debug_log, "+ reserve           = %f tC/area\n", s->value[RESERVE_C]);
	logger(g_debug_log, "+ fruit             = %f tC/area\n", s->value[FRUIT_C]);
	logger(g_debug_log, "+ stem live         = %f tC/area\n", s->value[STEM_LIVE_WOOD_C]);
	logger(g_debug_log, "+ coarse live       = %f tC/area\n", s->value[CROOT_LIVE_WOOD_C]);
	logger(g_debug_log, "+ branch live       = %f tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);
	logger(g_debug_log, "+ stem dead         = %f tC/area\n", s->value[STEM_DEAD_WOOD_C]);
	logger(g_debug_log, "+ coarse dead       = %f tC/area\n", s->value[CROOT_DEAD_WOOD_C]);
	logger(g_debug_log, "+ branch dead       = %f tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);
	logger(g_debug_log, "+ leaf              = %f tN/area\n", s->value[LEAF_N]);
	logger(g_debug_log, "+ stem              = %f tN/area\n", s->value[STEM_N]);
	logger(g_debug_log, "+ branch and bark   = %f tN/area\n", s->value[BRANCH_N]);
	logger(g_debug_log, "+ coarse root       = %f tN/area\n", s->value[CROOT_N]);
	logger(g_debug_log, "+ fine root         = %f tN/area\n", s->value[FROOT_N]);
	logger(g_debug_log, "+ reserve           = %f tN/area\n", s->value[RESERVE_N]);
	logger(g_debug_log, "+ fruit             = %f tN/area\n", s->value[FRUIT_N]);

}

void print_new_daily_forest_class_data (cell_t* const c, const int height, const int dbh, const int age, const int species)
{
	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &d->ages[age];
	s = &a->species[species];

	/* print at the beginning of simulation class level data */
	logger(g_debug_log, "******************************************************\n\n");
	logger(g_debug_log, "cell = \n");
	logger(g_debug_log, "* x  = %d\n", c->x);
	logger(g_debug_log, "* y  = %d\n", c->y);
	logger(g_debug_log, "--class level data--\n");
	logger(g_debug_log, "- Height            = %f m\n", h->value);
	logger(g_debug_log, "- DBH               = %f cm\n", d->value);
	logger(g_debug_log, "- Class Age         = %d years \n", a->value);
	logger(g_debug_log, "- Species           = %s\n", s->name);
	logger(g_debug_log, "- Number of trees   = %d trees \n", s->counter[N_TREE]);
	logger(g_debug_log, "- LAI_PROJ          = %f \n", s->value[LAI_PROJ]);
	logger(g_debug_log, "- Crown Diameter    = %f \n",  s->value[CROWN_DIAMETER]);
	logger(g_debug_log, "- Canopy Cover Proj = %f \n", s->value[CANOPY_COVER_PROJ]);
	logger(g_debug_log, "- Phenology type    = %f\n", s->value[PHENOLOGY]);
	logger(g_debug_log, "- Management type   = %c\n", (0 == s->management) ? 'T' : ((1 == s->management) ? 'C' : 'N'));
	logger(g_debug_log, "++Carbon pools in tC++\n");
	logger(g_debug_log, "+ leaf              = %f tC/area\n", s->value[LEAF_C]);
	logger(g_debug_log, "+ stem              = %f tC/area\n", s->value[STEM_C]);
	logger(g_debug_log, "+ branch and bark   = %f tC/area\n", s->value[BRANCH_C]);
	logger(g_debug_log, "+ coarse root       = %f tC/area\n", s->value[CROOT_C]);
	logger(g_debug_log, "+ fine root         = %f tC/area\n", s->value[FROOT_C]);
	logger(g_debug_log, "+ reserve           = %f tC/area\n", s->value[RESERVE_C]);
	logger(g_debug_log, "+ fruit             = %f tC/area\n", s->value[FRUIT_C]);
	logger(g_debug_log, "+ stem live         = %f tC/area\n", s->value[STEM_LIVE_WOOD_C]);
	logger(g_debug_log, "+ coarse live       = %f tC/area\n", s->value[CROOT_LIVE_WOOD_C]);
	logger(g_debug_log, "+ branch live       = %f tC/area\n", s->value[BRANCH_LIVE_WOOD_C]);
	logger(g_debug_log, "+ stem dead         = %f tC/area\n", s->value[STEM_DEAD_WOOD_C]);
	logger(g_debug_log, "+ coarse dead       = %f tC/area\n", s->value[CROOT_DEAD_WOOD_C]);
	logger(g_debug_log, "+ branch dead       = %f tC/area\n", s->value[BRANCH_DEAD_WOOD_C]);
	logger(g_debug_log, "+ leaf              = %f tN/area\n", s->value[LEAF_N]);
	logger(g_debug_log, "+ stem              = %f tN/area\n", s->value[STEM_N]);
	logger(g_debug_log, "+ branch and bark   = %f tN/area\n", s->value[BRANCH_N]);
	logger(g_debug_log, "+ coarse root       = %f tN/area\n", s->value[CROOT_N]);
	logger(g_debug_log, "+ fine root         = %f tN/area\n", s->value[FROOT_N]);
	logger(g_debug_log, "+ reserve           = %f tN/area\n", s->value[RESERVE_N]);
	logger(g_debug_log, "+ fruit             = %f tN/area\n", s->value[FRUIT_N]);
}

