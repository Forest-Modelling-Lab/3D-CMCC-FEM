/* treemodel.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "constants.h"
#include "common.h"
#include "print_output.h"
#include "settings.h"
#include "logger.h"
#include "matrix.h"
#include "g-function.h"
#include "structure.h"
#include "wue.h"
#include "check_balance.h"
#include "print.h"
#include "utility.h"
#include "dendometry.h"
#include "snow.h"
#include "phenology.h"
#include "peak_lai.h"
#include "canopy_radiation_sw_band.h"
#include "canopy_radiation_lw_band.h"
#include "canopy_net_radiation.h"
#include "canopy_temperature.h"
#include "modifiers.h"
#include "n-stock.h"
#include "canopy_evapotranspiration.h"
#include "photosynthesis.h"
#include "aut_respiration.h"
#include "fluxes.h"
#include "C-assimilation.h"
#include "C-deciduous-partitioning-allocation.h"
#include "C-evergreen-partitioning-allocation.h"
#include "mortality.h"
#include "biomass.h"
#include "management.h"
#include "soil_evaporation.h"
#include "soil_respiration.h"
#include "evapotranspiration.h"
#include "heat_fluxes.h"
#include "soil_water_balance.h"
#include "leaf_fall.h"
#include "settings.h"
#include "forest_tree_class.h"
#include "treemodel_daily.h"
#include "tree_class_remove.h"

//extern settings_t* g_settings;
extern logger_t* g_log;

//extern const char *szMonth[MONTHS_COUNT];

/* Days in Months */
extern int DaysInMonth[];

/* Last cumulative days in months */
//extern int MonthLength [];
//extern int MonthLength_Leap [];

//ALESSIOC TO ALESSIOR PLEASE MOVE INTO tree_class_remove.c
static int species_remove(age_t *a, const int index) {
	int i;
	int y;

	assert(a);

	if ( index > a->species_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < a->species_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		a->species[y++] = a->species[i];
	}
	--a->species_count;
	++a->species_avail;

	return 1;
}
//ALESSIOC TO ALESSIOR PLEASE MOVE INTO tree_class_remove.c
static int age_remove(dbh_t *d, const int index) {
	int i;
	int y;

	assert(d);

	if ( index > d->ages_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < d->ages_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		d->ages[y++] = d->ages[i];
	}
	--d->ages_count;
	++d->ages_avail;

	return 1;
}
//ALESSIOC TO ALESSIOR PLEASE MOVE INTO tree_class_remove.c
static int dbh_remove(height_t *h, const int index) {
	int i;
	int y;

	assert(h);

	if ( index > h->dbhs_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < h->dbhs_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		h->dbhs[y++] = h->dbhs[i];
	}
	--h->dbhs_count;
	++h->dbhs_avail;

	return 1;
}
//ALESSIOC TO ALESSIOR PLEASE MOVE INTO tree_class_remove.c
static int height_remove(cell_t *c, const int index) {
	int i;
	int y;

	assert(c);

	if ( index > c->heights_count ) {
		return 0;
	}

	y = 0;
	for ( i = 0; i < c->heights_count; ++i ) {
		if ( index == i ) continue;
		// ALESSIOR: shallow copy
		c->heights[y++] = c->heights[i];
	}
	--c->heights_count;
	++c->heights_avail;

	return 1;
}
//ALESSIOC TO ALESSIOR PLEASE MOVE INTO tree_class_remove.c
int tree_class_remove(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	height_t* h;
	dbh_t* d;
	age_t* a;
	species_t* s;

	assert(c);

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &d->ages[age];
	s = &a->species[species];

	if ( ! s->counter[N_TREE] )
	{
		if ( ! species_remove(a, species) )	return 0;
		if ( ! age_remove(d, age) )			return 0;
		if ( ! dbh_remove(h, dbh) )			return 0;
		if ( ! height_remove(c, height) )	return 0;
	}

	return 1;
}
/*****************************************************************************************************************/
int Tree_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	static int layer;
	static int height;
	int dbh;
	static int age;
	static int species;

	/* shortcuts */
	cell_t *c;
	tree_layer_t *l;
	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;
	meteo_daily_t *meteo_daily;

	/* assign shortcuts */
	c = &m->cells[cell];
	meteo_daily = &m->cells[cell].years[year].m[month].d[day];

	/* check parameters */
	assert( m );

	logger (g_log, "*********TREE_MODEL_DAILY*********\n");

	//FIXME move to meteo_t structure
	/* counter day of the year */
	if( !day && !month )c->doy = 1;
	else ++c->doy;
	/* counter day of simulation */
	if(!day && !month && !year)c->dos = 1;
	else ++c->dos;

	/* print cell data */
	print_cell_data ( c );

	/* forest structure */
	if ( ! day && ! month && year )
	{
		forest_structure ( c, meteo_daily, day, month, year );
	}

	/* prephenology */
	prephenology ( c, meteo_daily, day, month );

	/***********************************************************************************************/

	/* sort class in ascending way by heights */
	qsort ( c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc );

	/* loop on each cell layers starting from highest to lower */
	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
	{
		/* assign shortcut */
		l = &m->cells[cell].t_layers[layer];

		logger(g_log,"*****************************************************************************\n"
				"                                layer = %d                                 \n"
				"*****************************************************************************\n",layer);

		l->layer_z = layer;

		/* loop on each heights starting from highest to lower */
		for ( height = c->heights_count -1 ; height >= 0; --height )
		{
			/* assign shortcut */
			h = &m->cells[cell].heights[height];

			/* check if tree height class matches with corresponding cell layer */
			if( h->height_z == l->layer_z )
			{
				logger(g_log,"*****************************************************************************\n"
						"                              height = %g                              \n"
						"*****************************************************************************\n", h->value);

				/* loop on each dbh starting from highest to lower */
				for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* assign shortcut */
					d = &h->dbhs[dbh];

					logger(g_log,"*****************************************************************************\n"
							"                              dbh = %g                              \n"
							"*****************************************************************************\n", d->value);

					/* loop on each age class */
					for ( age = d->ages_count - 1 ; age >= 0 ; --age )
					{
						/* assign shortcut */
						a = &m->cells[cell].heights[height].dbhs[dbh].ages[age];

						logger(g_log,"*****************************************************************************\n"
								"                                  age = %d                                 \n"
								"*****************************************************************************\n", a->value);

						/* increment age after first year */
						if( !day && !month && year != 0) a->value += 1;

						/* loop on each species class */
						for ( species = 0; species < a->species_count; ++species )
						{
							/* assign shortcut */
							s = &m->cells[cell].heights[height].dbhs[dbh].ages[age].species[species];

							logger(g_log,"*****************************************************************************\n"
									"*                              species = %s                         *\n"
									"*****************************************************************************\n", s->name);

							if(s->counter[N_TREE] > 0)
							{
								//todo move into forest structure
								/* note: this is valid only for north hemisphere */
								/* beginning of simulation (every year included the first one) */
								if ( c->doy == 1)
								{
									/* compute annual minimum reserve for incoming year */
									annual_minimum_reserve( s );

									/* compute annual Maximum LAI */
									peak_lai( s, day, month, year );
								}
								/***************************************************************/
								/* print at the beginning of simulation forest class data */
								print_forest_class_data ( c, layer, height, dbh, age, species );

								/* compute species-specific phenological phase */
								phenology ( c, layer, height, dbh, age, species, meteo_daily, month);

								/* check for adult or sapling age */
								tree_period ( c, layer, height, dbh, age, species);

								logger(g_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", l->layer_z);

								if ( s->counter[VEG_UNVEG] == 1 )
								{
									logger(g_log, "\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

									/* increment vegetative days counter */
									++s->counter[VEG_DAYS];
									logger(g_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);

									++s->counter[YEARLY_VEG_DAYS];
								}
								else
								{
									logger(g_log, "\n\n*****UN-VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

									/* increment vegetative days counter */
									s->counter[VEG_DAYS] = 0;
									logger(g_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);

									s->counter[YEARLY_VEG_DAYS] += 0;
								}

								/* radiation */
								/*********************************************************************/
								/* short wave band */
								canopy_radiation_sw_band( c, layer, height, dbh, age, species, meteo_daily );
								/* long wave band */
								canopy_radiation_lw_band( c, layer, height, dbh, age, species, meteo_daily );
								/* net radiation */
								canopy_net_radiation( c, layer, height, dbh, age, species );
								/**********************************************************************/

								/* canopy temperature */
								canopy_temperature( c, layer, height, dbh, age, species, meteo_daily );

								/* daily modifier */
								modifiers( c, layer, height, dbh, age, species, meteo_daily);

								/* canopy water fluxes */
								canopy_evapotranspiration( c, layer, height, dbh, age, species, meteo_daily );

								/* canopy carbon fluxes */
								photosynthesis( c, layer, height, dbh, age, species, DaysInMonth[month]);

								/* autotrophic respiration */
								autotrophic_respiration ( c, layer, height, dbh, age, species, meteo_daily );

								/* carbon fluxes */
								carbon_fluxes ( s );

								/* C assimilation */
								carbon_assimilation( c, layer, height, dbh, age, species );

								/* C-N-partitioning-allocation */
								if ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 )
								{
									/* deciduous */
									daily_C_deciduous_partitioning_allocation( c, layer, height, dbh, age, species, meteo_daily, day, month, year );
								}
								else
								{
									/* evergreen */
									daily_C_evergreen_partitioning_allocation( c, layer, height, dbh, age, species, meteo_daily, day, month, year );
								}

								/* nitrogen */
								nitrogen_stock( s );

								/****************************************************************************************************************************************/
								/* check for balance closure at the class level */
								logger(g_log, "\n**CLASS LEVEL BALANCE**\n");

								/* check for radiative balance closure */
								check_class_radiation_balance ( c, layer, height, dbh, age, species );

								/* check for carbon balance closure */
								check_class_carbon_balance( c, layer, height, dbh, age, species );

								/* check for water balance closure */
								check_class_water_balance ( c, layer, height, dbh, age, species );

								/****************************************************************************************************************************************/
								/* END OF YEAR */
								/* last day of the year */
								if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
								{
									logger(g_log, "*****END OF YEAR %d ******\n", c->years[year].year);

									/*FRUIT ALLOCATION*/
									/*

								//FRUIT ESTABLISHMENT
								if (Yearly_Rain > s->value[MINRAIN])
								{
								//decidere se passare numero di semi da LPJ o dall'Equazione Logistica
								Establishment_LPJ ( &m->cells[cell],layer, height, dbh, age, species);
								logger(g_log, "Saplings Number from LPJ = %d\n", s->counter[N_TREE_SAP]);
								}
								else
								{
								logger(g_log, "Yearly Rain = %f\n", Yearly_Rain);
								logger(g_log, "Minimum Rain for Establishment = %f\n", s->value[MINRAIN]);
								logger(g_log, "NOT ENOUGH RAIN FOR ESTABLISHMENT\n");
								}
								}
									 */

									/************************************************************************************************************************************/
									/* MORTALITY and RENOVATION*/

									//ALESSIOC to do
									/* Mortality based on growth efficiency */
									//annual_growth_efficiency_mortality ( s );

									/* Mortality based on self thinning (3-PG) */
									//logger(g_log, "Get_Mortality COMMENTATA per bug, REINSERIRE!!!!!!!!!!!!!!!!!\n");
									//if ( s->management == T ) self_thinning_mortality_3PG (c, height, age, species, years);
									//if ( s->management == C ) stool_mortality ( c, layer, height, age, species );

									/*Mortality based on tree Age (LPJ)*/
									age_mortality ( a, s);

									/* renovation */
									//renovation ( c, layer, height, dbh, age, species);

									//todo todo todo maybe move within each mortality function (age_mortality and self-thinning)
									if ( ! tree_class_remove(c, height, dbh, age, species) ) {
										logger(g_log, "unable to remove tree class");
										exit(1);
									}

									/************************************************************************************************************************************/
									/* above ground-below ground biomass */
									abg_bgb_biomass ( c, height, dbh, age, species );

									/* annual volume, MAI and CAI */
									annual_tree_increment ( c, layer, height, dbh, age, species );

									/* print at the end of simulation class level data */
									print_forest_class_data ( c, layer, height, dbh, age, species );

									/************************************************************************************************************************************/
									/* management blocks */
									forest_management (c, layer, height, dbh, age, species, year);

									/************************************************************************************************************************************/
								}
							}
							else
							{
								logger(g_log, "\n\n**************************************************************\n"
										"No trees for height %g dbh %g age %d species %s are died!!!!\n"
										"**************************************************************\n\n",
										h->value, d->value, a->value, s->name);
							}
						}
					}
					logger(g_log, "****************END OF SPECIES CLASS***************\n");
				}
			}
		}
		logger(g_log, "****************END OF AGE CLASS***************\n");
	}
	logger(g_log, "****************END OF HEIGHT CLASS***************\n");

	/* ok */
	return 1;
}
