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
#include "new_forest_tree_class.h"

//extern settings_t* g_settings;
extern logger_t* g_log;

//extern const char *szMonth[MONTHS_COUNT];

/* Days in Months */
extern int DaysInMonth[];

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];

int Tree_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	static int layer;
	static int height;
	static int age;
	static int species;

	/* shortcuts */
	cell_t *c;
	tree_layer_t *l;
	height_t *h;
	age_t *a;
	species_t *s;
	meteo_daily_t *meteo_daily;

	/* assign shortcuts */
	c = &m->cells[cell];
	meteo_daily = &m->cells[cell].years[year].m[month].d[day];

	/* check parameters */
	assert(m);

	//FIXME move to meteo_t structure
	/* counter day of the year */
	if( !day && !month )c->doy = 1;
	else ++c->doy;
	/* counter day of simulation */
	if(!day && !month && !year)c->dos = 1;
	else ++c->dos;

	/* print at the beginning of simulation forest class data */
	print_cell_data ( c );

	/* annual forest structure (except the first year) */
	if( !day && !month && year != 0 )
	{
		annual_forest_structure ( c );
	}

	/* daily forest structure (except the first day, month and year)*/
	if( c->dos > 1 )
	{
		daily_forest_structure ( c );
	}

	/* daily check for vegetative period */
	daily_check_for_veg_period ( c, meteo_daily, day, month );

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

		l->z = layer;

		/* loop on each heights starting from highest to lower */
		for ( height = c->heights_count -1 ; height >= 0; --height )
		{
			/* assign shortcut */
			h = &m->cells[cell].heights[height];

			/* check if tree height class matches with corresponding cell layer */
			if( h->z == l->z )
			{
				logger(g_log,"*****************************************************************************\n"
						"                              height = %g                              \n"
						"*****************************************************************************\n", h->value);

				/* loop on each age class */
				for ( age = h->ages_count - 1 ; age >= 0 ; --age )
				{
					/* assign shortcut */
					a = &m->cells[cell].heights[height].ages[age];

					logger(g_log,"*****************************************************************************\n"
							"                                  age = %d                                 \n"
							"*****************************************************************************\n", a->value);

					/* increment age after first year */
					if( !day && !month && year != 0) a->value += 1;

					/* loop on each species class */
					for ( species = 0; species < a->species_count; ++species )
					{
						/* assign shortcut */
						s = &m->cells[cell].heights[height].ages[age].species[species];

						logger(g_log,"*****************************************************************************\n"
								"*                              species = %s                         *\n"
								"*****************************************************************************\n", s->name);

						if(s->counter[N_TREE] > 0)
						{
							/* note: this is valid only for north hemisphere */
							/* beginning of simulation (every year included the first one) */
							if ( c->doy == 1)
							{
								/* beginning of simulation (only for first year) */
								if ( !year )First_day ( c, layer, height, age, species );

								/* compute annual minimum reserve for incoming year */
								annual_minimum_reserve( s );

								/* compute annual Maximum LAI */
								peak_lai( s, day, month, year );
							}
							/***************************************************************/
							/* print at the beginning of simulation forest class data */
							print_forest_class_data ( c, layer, height, age, species );

							/* compute species-specific phenological phase */
							phenology ( c, layer, height, age, species, meteo_daily, month);

							/* check for adult or sapling age */
							tree_period ( c, layer, height, age, species);

							logger(g_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", h->z);

							if ( s->counter[VEG_UNVEG] == 1 )
							{
								logger(g_log, "\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

								/* increment vegetative days counter */
								s->counter[VEG_DAYS] += 1;
								logger(g_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);
							}
							else
							{
								logger(g_log, "\n\n*****UN-VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

								/* increment vegetative days counter */
								s->counter[VEG_DAYS] = 0;
								logger(g_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);
							}

							/* radiation */
							/* short wave band */
							canopy_radiation_sw_band( c, layer, height, age, species, meteo_daily );
							/* long wave band */
							canopy_radiation_lw_band( c, layer, height, age, species, meteo_daily );
							/* net radiation */
							canopy_net_radiation( c, layer, height, age, species );

							/* canopy temperature */
							canopy_temperature( c, layer, height, age, species, meteo_daily );

							/* daily modifier */
							modifiers( c, layer, height, age, species, meteo_daily);

							/* canopy water fluxes */
							canopy_evapotranspiration( c, layer, height, age, species, meteo_daily );

							/* canopy carbon fluxes */
							phosynthesis( c, layer, height, age, species, DaysInMonth[month]);

							/* autotrophic respiration */
							autotrophic_respiration ( c, layer, height, age, species, meteo_daily );

							/* carbon fluxes */
							carbon_fluxes ( s );

							/* C assimilation */
							carbon_assimilation( c, layer, height, age, species );

							/* C-N-partitioning-allocation */
							if ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 )
							{
								/* deciduous */
								daily_C_deciduous_partitioning_allocation( c, layer, height, age, species, meteo_daily, day, month, year );
							}
							else
							{
								/*evergreen */
								daily_C_evergreen_partitioning_allocation( c, layer, height, age, species, meteo_daily, day, month, year );
							}

							/* nitrogen */
							nitrogen_stock( s );

							/****************************************************************************************************************************************/
							/* check for balance closure at the class level */
							logger(g_log, "\n**CLASS LEVEL BALANCE**\n");

							/* check for radiative balance closure */
							check_class_radiation_balance ( c, layer, height, age, species );

							/* check for carbon balance closure */
							check_class_carbon_balance( c, layer, height, age, species );

							/* check for water balance closure */
							check_class_water_balance ( c, layer, height, age, species );

							/****************************************************************************************************************************************/
							/* END OF YEAR */
							/* last day of the year */
							if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[DECEMBER]) : (MonthLength[DECEMBER] )) == c->doy )
							{
								logger(g_log, "*****END OF YEAR %d ******\n", c->years[year].year);

								/*FRUIT ALLOCATION*/
								/*

								//FRUIT ESTABLISHMENT
								if (Yearly_Rain > s->value[MINRAIN])
								{
								//decidere se passare numero di semi da LPJ o dall'Equazione Logistica
								Establishment_LPJ ( &m->cells[cell],layer, height, age, species);
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

								/* Mortality based on self thinning (3-PG) */
								//logger(g_log, "Get_Mortality COMMENTATA per bug, REINSERIRE!!!!!!!!!!!!!!!!!\n");
								//if ( s->management == T ) Mortality (c, height, age, species, years);
								//if ( s->management == C ) stool_mortality ( c, layer, height, age, species );

								/*Mortality based on tree Age (LPJ)*/
								//Age_Mortality ( a, s);

								/* renovation */
								//renovation ( c, layer, height, age, species);

								/************************************************************************************************************************************/
								/* above ground-below ground biomass */
								abg_bgb_biomass ( c, height, age, species );

								/* annual volume, MAI and CAI */
								annual_tree_increment ( c, layer, height, age, species );

								/* print at the end of simulation class level data */
								print_forest_class_data ( c, layer, height, age, species );

								/************************************************************************************************************************************/
								/* management blocks */
								forest_management (c, layer, height, age, species, year);

								/************************************************************************************************************************************/
							}
						}
						else
						{
							logger(g_log, "\n\n**************************************************************\n"
									"No trees for height %g age %d species %s dbh %g are died!!!!\n"
									"**************************************************************\n\n",
									h->value, a->value, s->name, s->value[AVDBH]);
						}
					}
				}
				logger(g_log, "****************END OF SPECIES CLASS***************\n");
			}
		}
		logger(g_log, "****************END OF AGE CLASS***************\n");
	}
	logger(g_log, "****************END OF HEIGHT CLASS***************\n");

	/*******************************************************************************************************/
	/* SOIL POOL */

	/* compute snow melt, snow sublimation */
	snow_melt_subl ( c, meteo_daily );

	/* compute soil respiration */
	soil_respiration ( c );

	/* compute soil evaporation */
	soil_evaporation ( c, meteo_daily );

	/* compute soil water balance */
	soil_water_balance ( c, meteo_daily );

	/*******************************************************************************************************/
	/* OVERALL CELL */

	/* compute evapotranspiration */
	evapotranspiration ( c );

	/* compute latent heat fluxes */
	latent_heat_flux ( c, meteo_daily );

	/* compute sensible heat fluxes */
	sensible_heat_flux ( c, meteo_daily );

	/* compute water fluxes */
	water_fluxes ( c, meteo_daily );

	/*******************************************************************************************************/
	/* CHECK FOR BALANCE CLOSURE */

	/* CHECK FOR RADIATIVE BALANCE CLOSURE */
	check_radiation_balance ( c, meteo_daily );

	/* CHECK FOR CARBON BALANCE CLOSURE */
	check_carbon_balance ( c );

	/* CHECK FOR WATER BALANCE CLOSURE */
	check_soil_water_balance ( c, meteo_daily );

	/****************************************************************************************************/

	/****************************************************************************************************/
//	/* RESET VARIABLES */
//
//	/* reset daily variables */
//	reset_daily_class_variables ( c );
//	reset_daily_layer_variables ( c );
//	reset_daily_cell_variables  ( c );
//
//	/* reset monthly variables */
//	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month]) : (MonthLength[month] )) == c->doy )
//	{
//		reset_monthly_class_variables ( c );
//		reset_monthly_layer_variables ( c );
//		reset_monthly_cell_variables  ( c );
//
//		/* reset annual variables */
//		if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[DECEMBER]) : (MonthLength[DECEMBER] )) == c->doy )
//		{
//			reset_annual_class_variables ( c );
//			reset_annual_layer_variables ( c );
//			reset_annual_cell_variables  ( c );
//		}
//	}

	/****************************************************************************************************/
	//todo: soilmodel could stay here or in main.c
	//here is called at the end of all tree height age and species classes loops
	//todo: move all soil algorithms into soil_model function
	//soil_model (&m->cells[cell], yos, years, month, years_of_simulation);
	//N_avl = (Ka * g_soil->values[SOIL_sN) + pN + (Kb * Yearly_Eco_NPP);
	//logger(g_log, "Nitrogen available = %f g m^-2\n", N_avl);
	logger(g_log, "******************END OF CELL*********************\n");

	/* ok */
	return 1;
}


