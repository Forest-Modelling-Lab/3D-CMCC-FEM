/* treemodel.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <new_forest_tree_class.h>
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
#include "treemodel_daily.h"
#include "remove_tree_class.h"
#include "regeneration.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;

//extern const char sz_err_out_of_memory[];

//extern const char *szMonth[MONTHS_COUNT];

/* Days in Months */
extern int DaysInMonth[];

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];

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
	yos_t *meteo_annual;

	/* assign shortcuts */
	c = &m->cells[cell];
	meteo_daily = &m->cells[cell].years[year].m[month].d[day];
	meteo_annual = &m->cells[cell].years[year];

	/* check parameters */
	assert( m );

	logger (g_debug_log, "\n*********TREE_MODEL_DAILY*********\n");

	//FIXME move to meteo_t structure
	/* counter day of the year */
	if( !day && !month )c->doy = 1;
	else ++c->doy;
	/* counter day of simulation */
	if( !day && !month && !year )c->dos = 1;
	else ++c->dos;

	/* annual forest structure */
	if ( ! day && ! month && year )
	{
		annual_forest_structure ( c );
	}

	/* print  forest cell data */
	print_daily_forest_data ( c );

	/* prephenology */
	prephenology ( c, meteo_daily, day, month );

	/***********************************************************************************************/

	/* sort class in ascending way by heights */
	qsort ( c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc );

	/* loop on each cell layers starting from highest to lower */
	for ( layer = c->tree_layers_count -1 ; layer >= 0; --layer )
	{
		/* assign shortcut */
		l = &m->cells[cell].tree_layers[layer];

		logger(g_debug_log,"*****************************************************************************\n"
				"                                layer = %d                                 \n"
				"*****************************************************************************\n",layer);

		l->layer_z = layer;

		/* loop on each heights starting from highest to lower */
		for ( height = c->heights_count -1 ; height >= 0; --height )
		{
			/* assign shortcut */
			h = &m->cells[cell].heights[height];

			//ALESSIOC FIXME PORCATA
			c->cell_heights_count ++;

			/* check if tree height class matches with corresponding cell layer */
			if( h->height_z == l->layer_z )
			{
				logger(g_debug_log,"*****************************************************************************\n"
						"                              height = %g                              \n"
						"*****************************************************************************\n", h->value);

				/* loop on each dbh starting from highest to lower */
				for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* assign shortcut */
					d = &h->dbhs[dbh];

					logger(g_debug_log,"*****************************************************************************\n"
							"                              dbh = %g                              \n"
							"*****************************************************************************\n", d->value);

					/* loop on each age class */
					for ( age = d->ages_count - 1 ; age >= 0 ; --age )
					{
						/* assign shortcut */
						a = &m->cells[cell].heights[height].dbhs[dbh].ages[age];

						logger(g_debug_log,"*****************************************************************************\n"
								"                                  age = %d                                 \n"
								"*****************************************************************************\n", a->value);

						/* increment age after first year */
						if( !day && !month && year != 0) a->value += 1;

						/* loop on each species class */
						for ( species = 0; species < a->species_count; ++species )
						{
							/* assign shortcut */
							s = &m->cells[cell].heights[height].dbhs[dbh].ages[age].species[species];

							logger(g_debug_log,"*****************************************************************************\n"
									"*                              species = %s                         *\n"
									"*****************************************************************************\n", s->name);

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
							print_daily_forest_class_data ( c, layer, height, dbh, age, species );

							/* compute species-specific phenological phase */
							phenology ( c, layer, height, dbh, age, species, meteo_daily, month);

							logger(g_debug_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", l->layer_z);

							if ( s->counter[VEG_UNVEG] == 1 )
							{
								logger(g_debug_log, "\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

								/* increment vegetative days counter */
								++s->counter[VEG_DAYS];
								logger(g_debug_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);

								++s->counter[YEARLY_VEG_DAYS];
							}
							else
							{
								logger(g_debug_log, "\n\n*****UN-VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

								/* increment vegetative days counter */
								s->counter[VEG_DAYS] = 0;
								logger(g_debug_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);

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
							modifiers( c, layer, height, dbh, age, species, meteo_daily, meteo_annual);

							/* canopy water fluxes */
							canopy_evapotranspiration( c, layer, height, dbh, age, species, meteo_daily );

							/* canopy carbon fluxes */
							photosynthesis( c, layer, height, dbh, age, species, DaysInMonth[month], meteo_annual);

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
							logger(g_debug_log, "\n**CLASS LEVEL BALANCE**\n");

							/* check for radiative balance closure */
							check_class_radiation_balance ( c, layer, height, dbh, age, species );

							/* check for carbon balance closure */
							check_class_carbon_balance( c, layer, height, dbh, age, species );

							/* check for water balance closure */
							check_class_water_balance ( c, layer, height, dbh, age, species );

							/****************************************************************************************************************************************/
							/* END OF MONTH */
							/* last day of the month */
							if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
							{
								//dendrometry ( c, layer, height, dbh, age, species, meteo_daily );
							}

							/****************************************************************************************************************************************/
							/* END OF YEAR */
							/* last day of the year */
							if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
							{
								logger(g_debug_log, "*****END OF YEAR %d ******\n", c->years[year].year);

								/************************************************************************************************************************************/
								/* MORTALITY and RENOVATION*/

								/* Mortality based on growth efficiency */
								/* note: it currently works on a annual scale (no more daily) */
								annual_growth_efficiency_mortality ( c, height, dbh, age, species );

								/* Mortality */
								//if ( s->management == C ) stool_mortality ( c, layer, height, age, species );

								/*Mortality based on tree Age (LPJ)*/
								age_mortality ( c, height, dbh, age, species);

								/************************************************************************************************************************************/
								/* above ground-below ground biomass */
								abg_bgb_biomass ( c, height, dbh, age, species );

								/* annual branch and bark fraction */
								tree_branch_and_bark ( c, height, dbh, age, species );

								/* annual volume, MAI and CAI */
								annual_tree_increment ( c, height, dbh, age, species, year );

								/* print at the end of simulation class level data */
								print_daily_forest_class_data ( c, layer, height, dbh, age, species );

								/************************************************************************************************************************************/
								if ( g_settings->regeneration && ( a->value > s->value[SEXAGE] ) )
								{
									/* regeneration */
									regeneration ( c, height, dbh, age, species);
								}
								if ( g_settings->management && year )
								{
									/* management blocks */
									forest_management (c, layer, height, dbh, age, species, year);
								}

								/* update pointers */
								l = &c->tree_layers[layer];
								h = &c->heights[height];
								d = &h->dbhs[dbh];
								a = &d->ages[age];
								s = &a->species[species];

								/************************************************************************************************************************************/
							}
						}
						logger(g_debug_log, "****************END OF SPECIES CLASS***************\n");
					}
					logger(g_debug_log, "****************END OF AGES CLASS***************\n");
				}
				logger(g_debug_log, "****************END OF DBH CLASS***************\n");
			}
		}
		logger(g_debug_log, "****************END OF HEIGHT CLASS***************\n");
	}
	logger(g_debug_log, "****************END OF LAYER CLASS***************\n");

	/* ok */
	return 1;
}
