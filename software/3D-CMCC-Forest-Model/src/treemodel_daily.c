/* treemodel.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "constants.h"
#include "cumulative_balance.h"
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
#include "common.h"
#include "biomass.h"
#include "management.h"
#include "soil_evaporation.h"
#include "soil_respiration.h"
#include "evapotranspiration.h"
#include "heat_fluxes.h"
#include "soil_water_balance.h"
#include "settings.h"

extern settings_t* g_settings;
extern logger_t* g_log;

//extern const char *szMonth[MONTHS_COUNT];
extern int DaysInMonth[];

int Tree_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	static int layer;
	static int height;
	static int age;
	static int species;

	static int rotation_counter;

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
	if(day == 0 && month == 0)c->doy = 1;
	else c->doy += 1;

	/* annual forest structure (except the first year) */
	if( !day && !month && year != 0 ) annual_forest_structure ( c );

	/* daily check for vegetative period */
	daily_check_for_veg_period ( c, meteo_daily, day, month );

	/* daily forest structure */
	daily_forest_structure ( c );

	/***********************************************************************************************/

	/* sort class in ascending way by heights */
	qsort ( c->heights, c->heights_count, sizeof (height_t), sort_by_heights_asc );

	/* loop on each cell layers starting from highest to lower */
	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
	{
		/* assign shortcut */
		l = &m->cells[cell].t_layers[layer];

		logger(g_log,"*****************************************************************************\n"
				"                                                                             \n"
				"                                layer = %d                                 \n"
				"                                                                             \n"
				"*****************************************************************************\n",
				layer);

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
						"                               height = %g                              \n"
						"*****************************************************************************\n", h->value);

				/* loop on each age class */
				for ( age = h->ages_count - 1 ; age >= 0 ; --age )
				{
					/* assign shortcut */
					a = &m->cells[cell].heights[height].ages[age];

					logger(g_log,"*****************************************************************************\n"
							"                                   age = %d                                 \n"
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
							if ( !day && !month )
							{
								/* beginning of simulation (only for first year) */
								if ( !year )First_day ( c, layer, height, age, species );

								/* compute annual minimum reserve for incoming year */
								annual_minimum_reserve( s );

								/* compute annual Maximum LAI */
								peak_lai( s, day, month, year );
							}
							/***************************************************************/

							/* print at the beginning of simulation stand data */
							print_stand_data ( c, layer, height, age, species );

							/* compute species-specific phenological phase */
							phenology ( c, layer, height, age, species, meteo_daily, month);

							/* check for adult or sapling age */
							tree_period ( c, layer, height, age, species);

							/* Loop for adult trees */
							if ( !s->period )
							{
								/* loop for deciduous */
								if ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 )
								{
									/* within vegetative period for deciduous */
									if ( s->counter[VEG_UNVEG] == 1 )
									{
										logger(g_log, "\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );
										logger(g_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", h->z);

										/* increment vegetative days counter */
										s->counter[VEG_DAYS] += 1;

										/* radiation */
										canopy_radiation_sw_band( c, layer, height, age, species, meteo_daily );
										canopy_radiation_lw_band( c, layer, height, age, species, meteo_daily );
										canopy_net_radiation( c, layer, height, age, species );

										/* canopy temperature */
										canopy_temperature( c, layer, height, age, species, meteo_daily );

										/* daily modifier */
										modifiers( c, layer, height, age, species, meteo_daily);

										/* nitrogen */
										nitrogen_stock( s );

										/* canopy water fluxes */
										canopy_evapotranspiration( c, layer, height, age, species, meteo_daily );

										/* canopy carbon fluxes */
										phosynthesis( c, layer, height, age, species, DaysInMonth[month]);

										/* respiration */
										if (!string_compare_i ( g_settings->Prog_Aut_Resp, "on" ) )
										{
											maintenance_respiration( c, layer, height, age, species, meteo_daily );
											growth_respiration( c, layer, height, age, species );
										}
										autotrophic_respiration ( c, layer, height, age, species );

										/* carbon fluxes */
										carbon_fluxes ( s );

										/* C assimilation */
										carbon_assimilation( c, layer, height, age, species );

										/* C-N-partitioning-allocation */
										daily_C_deciduous_partitioning_allocation( c, layer, height, age, species, meteo_daily, day, year );
									}
									/*outside growing season*/
									else
									{
										/* assign zero value for LAI */
										if ( g_settings->spatial == 'u' ) s->value[LAI] = 0;

										/* radiation */
										canopy_radiation_sw_band( c, layer, height, age, species, meteo_daily );
										canopy_radiation_lw_band( c, layer, height, age, species, meteo_daily );
										canopy_net_radiation( c, layer, height, age, species );

										/* canopy temperature */
										canopy_temperature ( c, layer, height, age, species, meteo_daily );

										/* nitrogen */
										nitrogen_stock ( s );

										/* canopy carbon fluxes */
										phosynthesis (&m->cells[cell], layer, height, age, species, DaysInMonth[month]);

										/* respiration */
										if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
										{
											maintenance_respiration ( c, layer, height, age, species, meteo_daily );
											growth_respiration ( c, layer, height, age, species );
										}
										autotrophic_respiration ( c, layer, height, age, species );

										/* carbon fluxes */
										carbon_fluxes ( s );

										/* C assimilation */
										carbon_assimilation( c, layer, height, age, species );

										/* C-N-partitioning-allocation */
										daily_C_deciduous_partitioning_allocation( c, layer, height, age, species, meteo_daily, day, year);
									}
								}
								/* loop for evergreen */
								else
								{
									logger(g_log, "*****VEGETATIVE PERIOD FOR %s SPECIES *****\n", s->name);
									logger(g_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", h->z);

									/* increment vegetative days counter */
									s->counter[VEG_DAYS] += 1;
									logger(g_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);

									/* radiation block */
									canopy_radiation_sw_band( c, layer, height, age, species, meteo_daily );
									canopy_radiation_lw_band( c, layer, height, age, species, meteo_daily );
									canopy_net_radiation( c, layer, height, age, species );

									/* canopy temperature */
									canopy_temperature ( c, layer, height, age, species, meteo_daily );

									/* daily modifier */
									modifiers ( c, layer, height, age, species, meteo_daily );

									/* canopy water fluxes block */
									canopy_evapotranspiration ( c, layer, height, age, species, meteo_daily );

									/* canopy carbon fluxes block */
									phosynthesis ( c, layer, height, age, species, DaysInMonth[month] );

									/* nitrogen */
									nitrogen_stock ( s );

									/* respiration */
									if ( !string_compare_i ( g_settings->Prog_Aut_Resp, "on" ) )
									{
										maintenance_respiration ( c, layer, height, age, species, meteo_daily );
										growth_respiration ( c, layer, height, age, species );
									}
									autotrophic_respiration ( c, layer, height, age, species );

									/* carbon fluxes */
									carbon_fluxes ( s );

									/* C assimilation */
									carbon_assimilation( c, layer, height, age, species );

									/* C-N-partitioning-allocation */
									daily_C_evergreen_partitioning_allocation ( c, layer, height, age, species, meteo_daily, day, year );
									logger(g_log, "---------------------------------------------------------------------------------\n");
								}

								/* check for balance closure at the class level */
								logger(g_log, "\n**CLASS LEVEL BALANCE**\n");

								/* check for radiative balance closure */
								Check_class_radiation_balance ( c, layer, height, age, species );

								/* check for carbon balance closure */
								Check_class_carbon_balance( c, layer, height, age, species );

								/* check for water balance closure */
								Check_class_water_balance ( c, layer, height, age, species );
								/****************************************************************************************************************************************/

								/* SHARED FUNCTIONS FOR DECIDUOUS AND EVERGREEN */
								/* END OF YEAR */
								if (day == 30 && month == DECEMBER)
								{
									logger(g_log, "*****END OF YEAR******\n");
									/*FRUIT ALLOCATION*/
									/*
								if (m->cells[cell].heights[height].ages[age].value >= s->value[SEXAGE] && (m->cells[cell].heights[height].z == 2 || m->cells[cell].heights[height].z == 1))
								{
								Fruit_Allocation_LPJ ( &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z, years, Yearly_Rain, m->cells[cell].canopy_cover_dominant);
								Seeds_Number_LE = Fruit_Allocation_Logistic_Equation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
								logger(g_log, "Seeds Number from Logistic Equation = %d\n", Seeds_Number_LE);

								//Seeds_Number_T = Fruit_Allocation_TREEMIG ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
								//logger(g_log, "Seeds Number from TREEMIG = %d\n", Seeds_Number_T);

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

									logger(g_log, "*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n");

									/*MORTALITY*/
									//todo CONTROLLARE E SOMMARE AD OGNI STRATO LA BIOMASSA DI QUELLA SOVRASTANTE
									logger(g_log, "Get_Mortality COMMENTATA per bug, REINSERIRE!!!!!!!!!!!!!!!!!\n");
									//Mortality (&m->cells[cell].heights[height].ages[age].species[species], years);
									//todo
									//WHEN MORTALITY OCCURs IN MULTILAYERED FOREST, MODEL SHOULD CREATE A NEW CLASS FOR THE DOMINATED LAYER THAT
									//RECEIVES MORE LIGHT AND THEN GROWTH BETTER SO IT IS A NEW HEIGHT CLASS
									/*Mortality based on tree Age(LPJ)*/
									//Age_Mortality (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
									/*Mortality based on Growth efficiency(LPJ)*/
									//Mortality (&m->cells[cell].heights[height].ages[age].species[species]);

									if ( s->management == C) Stool_mortality ( c, layer, height, age, species );

									//Renovation (&m->cells[cell], &m->cells[cell].heights[height], &m->cells[cell].heights[height].ages[age].species[species]);

									AGB_BGB_biomass ( c, height, age, species );

									Biomass_increment_EOY ( c, layer, height, age, species );

									/* print at the end of simulation class level data */
									print_stand_data ( c, layer, height, age, species );

									Water_Use_Efficiency ( s );

									/* management blocks */
									//Choose_management (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], years, height);

									/* simulate management */
									if ( !string_compare_i (g_settings->management, "on") && !year )
									{
										rotation_counter = (int)s->value[ROTATION];
									}
									else if ( ! string_compare_i (g_settings->management, "on") && year > 0 )
									{
										if ( year == (int)s->value[ROTATION] )
										{
											Clearcut_Timber_without_request ( c, layer, height, age, species, year );

											/* add multiples */
											s->value[ROTATION] += rotation_counter;
											/*
										if(g_settings->replanted_tree != 0.0)
										{
											if ( ! Create_new_class(&m->cells[cell], height, age, species) )
											{
												logger(g_log, "unable to add new height class!");
												// DIE !
												exit(1);
											}
										}
											 */
										}
									}
								}
							}
							/* functions for saplings */
							else
							{
								if( day == 30 && month == DECEMBER )
								{
									logger(g_log, "\n/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
									logger(g_log, "SAPLINGS\n");
									/*
								 Saplings_counter += 1;
								 logger(g_log, "-Number of Sapling class in layer 0 = %d\n", Saplings_counter);
									 */
									logger(g_log, "Age %d\n", a->value);

									if ( s->value[LIGHT_TOL] == 1)
									{
										if (c->par_for_establishment < g_settings->light_estab_very_tolerant)
										{
											s->counter[N_TREE_SAP] = 0;
											logger(g_log, "NO Light for Establishment\n");
										}
									}
									else if ( s->value[LIGHT_TOL] == 2)
									{
										if (c->par_for_establishment < g_settings->light_estab_tolerant)
										{
											s->counter[N_TREE_SAP] = 0;
											logger(g_log, "NO Light for Establishment\n");
										}
									}
									else if ( s->value[LIGHT_TOL] == 3)
									{
										if (c->par_for_establishment < g_settings->light_estab_intermediate)
										{
											s->counter[N_TREE_SAP] = 0;
											logger(g_log, "NO Light for Establishment\n");
										}
									}
									else
									{
										if (c->par_for_establishment < g_settings->light_estab_intolerant)
										{
											s->counter[N_TREE_SAP] = 0;
											logger(g_log, "NO Light for Establishment\n");
										}
									}
									logger(g_log, "/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
								}
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
	/* note: computations that involve all classes and are related to the overall cell */
	/* computations during the last height class processing */

	/* compute snow melt, snow sublimation */
	snow_melt_subl ( c, meteo_daily );

	/* compute soil respiration */
	Soil_respiration ( c );

	/* compute soil evaporation */
	Soil_evaporation ( c, meteo_daily );

	/* compute evapotranspiration */
	Evapotranspiration ( c );

	/* compute latent heat fluxes */
	Latent_heat_flux ( c, meteo_daily );

	/* compute sensible heat fluxes */
	Sensible_heat_flux ( c, meteo_daily );

	/* compute soil water balance */
	Soil_water_balance ( c, meteo_daily );

	/* compute water fluxes */
	Water_fluxes ( c, meteo_daily );

	/*******************************************************************************************************/
	/* CHECK FOR BALANCE CLOSURE */

	/* CHECK FOR RADIATIVE BALANCE CLOSURE */
	Check_radiation_balance ( c, meteo_daily );

	/* CHECK FOR CARBON BALANCE CLOSURE */
	Check_carbon_balance ( c );

	/* CHECK FOR WATER BALANCE CLOSURE */
	Check_soil_water_balance ( c, meteo_daily );

	/****************************************************************************************************/

	/****************************************************************************************************/
	/* RESET VARIABLES */

	/* reset daily variables */
	reset_daily_class_variables ( c );
	reset_daily_layer_variables ( c );
	reset_daily_cell_variables ( c );

	/* reset monthly variables */
	if ( !day )
	{
		reset_monthly_class_variables( c );
		reset_monthly_layer_variables( c );
		reset_monthly_cell_variables( c );

		if ( !month )
		{
			/* reset annual variables */
			reset_annual_class_variables( c );
			reset_annual_layer_variables( c );
			reset_annual_cell_variables( c );
		}
	}
	/****************************************************************************************************/

	c->dos += 1;
	//todo: soilmodel could stay here or in main.c
	//here is called at the end of all tree height age and species classes loops
	//todo: move all soil algorithms into soil_model function
	//soil_model (&m->cells[cell], yos, years, month, years_of_simulation);
	//N_avl = (Ka * g_soil->values[SOIL_sN) + pN + (Kb * Yearly_Eco_NPP);
	//logger(g_log, "Nitrogen available = %f g m^-2\n", N_avl);
	logger(g_log, "****************END OF CELL***************\n");

	/* ok */
	return 1;
}


