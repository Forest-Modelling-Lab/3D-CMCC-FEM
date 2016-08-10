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
#include "check_prcp.h"
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

extern const char *szMonth[MONTHS_COUNT];
extern int DaysInMonth[];

int Tree_model_daily (matrix_t *const m, const int cell, const int day, const int month, const int year)
{
	static int layer;
	static int height;
	static int age;
	static int species;

	static int rotation_counter;

	meteo_daily_t *meteo_daily;
	meteo_daily = &m->cells[cell].years[year].m[month].d[day];

	/* check parameters */
	assert(m);

	if(day == 0 && month == 0)m->cells[cell].doy = 1;
	else m->cells[cell].doy += 1;

	logger(g_log, "\n\n%d-%d-%d\n", meteo_daily->n_days, month+1, m->cells[cell].years[year].year);

	/* print daily met data */
	print_daily_met_data (meteo_daily, day, month, year);

	/* reset daily variables at cell level */
	reset_daily_cell_variables (&m->cells[cell]);

	/* annual forest structure (except the first year) */
	if (day == 0 && month == JANUARY && year != 0) annual_forest_structure (&m->cells[cell]);

	/* daily check for vegetative period */
	daily_check_for_veg_period (&m->cells[cell], meteo_daily, day, month);

	/* daily forest structure */
	daily_forest_structure (&m->cells[cell]);

//	/* daily check for vegetative layer */
//	daily_forest_structure_in_veg (&m->cells[cell]);

	daily_dominant_light (&m->cells[cell], layer, height, age, species);



	/* sort class in ascending way by heights */
	qsort (m->cells[cell].heights, m->cells[cell].heights_count, sizeof (height_t), sort_by_heights_asc);

	/* loop on each heights starting from highest to lower */
	logger(g_log, "******CELL x = %d, y = %d ******\n", m->cells[cell].x, m->cells[cell].y);
	for ( height = m->cells[cell].heights_count -1 ; height >= 0; height-- )
	{
	//test remove "for" above
		/* loop on each layers starting from highest to lower */
//	for ( layer = m->cells[cell].t_layers_count -1 ; layer >= 0; layer-- )
//	{
		/* loop on each age class */
		for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			/* increment age after first year */
			if( day == 0 && month == JANUARY && year != 0) m->cells[cell].heights[height].ages[age].value += 1;

			/* loop on each species class */
			for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
			{
				if(m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE] > 0)
				{
					/* beginning of simulation (only for first year) */
					if(day == 0 && month == JANUARY && year == 0) First_day (&m->cells[cell], layer, height, age, species);

					/* beginning of simulation (every year included the first one) */
					if (day == 0 && month == JANUARY)
					{
						/* compute annual minimum reserve for incoming year */
						Annual_minimum_reserve(&m->cells[cell].heights[height].ages[age].species[species]);

						/* reset annual variables */
						Reset_annual_variables (&m->cells[cell], layer, height, age, species);

						/* compute prognostically annual Maximum LAI */
						Peak_lai(&m->cells[cell].heights[height].ages[age].species[species], day, month, year);
					}
					/* reset monthly variables */
					if (day == 0) Reset_monthly_variables (&m->cells[cell], layer, height, age, species);

					/* reset daily class level variables */
					reset_daily_class_variables(&m->cells[cell], layer, height, age, species);

					/* check precipitation and compute for snow if needs */
					Check_prcp (&m->cells[cell], meteo_daily);

					/* compute species-specific phenological phase */
					Phenology (&m->cells[cell], layer, height, age, species, meteo_daily, month);

					/* check for adult or sapling age */
					Tree_period (&m->cells[cell], layer, height, age, species);

					/* compute how many classes are in vegetative period */
					daily_veg_counter (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], height);

					/* print at the beginning of simulation stand data */
					Print_stand_data (&m->cells[cell], layer, height, age, species);

					/* Loop for adult trees */
					if (m->cells[cell].heights[height].ages[age].species[species].period == 0.0)
					{

						/* loop for deciduous */
						if (m->cells[cell].heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || m->cells[cell].heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
						{
							/* within vegetative period for deciduous */
							if (m->cells[cell].heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
							{
								logger(g_log, "\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", m->cells[cell].heights[height].ages[age].species[species].name );
								logger(g_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);

								/* increment vegetative days counter */
								m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS] += 1;

								/* radiation */
								canopy_radiation_sw_band(&m->cells[cell], layer, height, age, species, meteo_daily);
								canopy_radiation_lw_band(&m->cells[cell], layer, height, age, species, meteo_daily);
								canopy_net_radiation(&m->cells[cell], layer, height, age, species);

								/* canopy temperature */
								canopy_temperature (&m->cells[cell], layer, height, age, species, meteo_daily);

								/* daily modifier */
								Daily_modifiers (&m->cells[cell], layer, height, age, species, m->cells[cell].heights[height].ages[age].species[species].management, meteo_daily);

								/* nitrogen */
								Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);

								/* canopy water fluxes */
								canopy_evapotranspiration(&m->cells[cell], layer, height, age, species, meteo_daily);

								/* canopy carbon fluxes */
								Phosynthesis (&m->cells[cell], layer, height, age, species, DaysInMonth[month]);

								/* respiration */
								if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
								{
									Maintenance_respiration (&m->cells[cell], layer, height, age, species, meteo_daily);
									Growth_respiration (&m->cells[cell], layer, height, age, species);
								}
								Autotrophic_respiration (&m->cells[cell], layer, height, age, species);

								/* carbon fluxes */
								Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species]);

								/* C assimilation */
								Carbon_assimilation(&m->cells[cell], layer, height, age, species);

								/* C-N-partitioning-allocation */
								Daily_C_Deciduous_Partitioning_Allocation(&m->cells[cell], layer, height, age, species, meteo_daily, day, year);
							}
							/*outside growing season*/
							else
							{
								/* assign zero value for LAI */
								if (g_settings->spatial == 'u') m->cells[cell].heights[height].ages[age].species[species].value[LAI] = 0;

								/* radiation */
								canopy_radiation_sw_band(&m->cells[cell], layer, height, age, species, meteo_daily);
								canopy_radiation_lw_band(&m->cells[cell], layer, height, age, species, meteo_daily);
								canopy_net_radiation(&m->cells[cell], layer, height, age, species);

								/* canopy temperature */
								canopy_temperature (&m->cells[cell], layer, height, age, species, meteo_daily);

								/* nitrogen */
								Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);

								/* canopy carbon fluxes */
								Phosynthesis (&m->cells[cell], layer, height, age, species, DaysInMonth[month]);

								/* respiration */
								if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
								{
									Maintenance_respiration (&m->cells[cell], layer, height, age, species, meteo_daily);
									Growth_respiration (&m->cells[cell], layer, height, age, species);
								}
								Autotrophic_respiration (&m->cells[cell], layer, height, age, species);

								/* carbon fluxes */
								Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species]);

								/* C assimilation */
								Carbon_assimilation(&m->cells[cell], layer, height, age, species);

								/* C-N-partitioning-allocation */
								Daily_C_Deciduous_Partitioning_Allocation(&m->cells[cell], layer, height, age, species, meteo_daily, day, year);
							}
						}
						/* loop for evergreen */
						else
						{
							logger(g_log, "*****VEGETATIVE PERIOD FOR %s SPECIES *****\n", m->cells[cell].heights[height].ages[age].species[species].name);
							logger(g_log, "--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);

							/* increment vegetative days counter */
							m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS] += 1;
							logger(g_log, "VEG_DAYS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS]);

							/* radiation block */
							canopy_radiation_sw_band(&m->cells[cell], layer, height, age, species, meteo_daily);
							canopy_radiation_lw_band(&m->cells[cell], layer, height, age, species, meteo_daily);
							canopy_net_radiation(&m->cells[cell], layer, height, age, species);

							/* canopy temperature */
							canopy_temperature (&m->cells[cell], layer, height, age, species, meteo_daily);

							/* daily modifier */
							Daily_modifiers (&m->cells[cell], layer, height, age, species, m->cells[cell].heights[height].ages[age].species[species].management, meteo_daily);

							/* canopy water fluxes block */
							canopy_evapotranspiration(&m->cells[cell], layer, height, age, species, meteo_daily);

							/* canopy carbon fluxes block */
							Phosynthesis (&m->cells[cell], layer, height, age, species, DaysInMonth[month]);

							/* nitrogen */
							Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);

							/* respiration */
							if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
							{
								Maintenance_respiration (&m->cells[cell], layer, height, age, species, meteo_daily);
								Growth_respiration (&m->cells[cell], layer, height, age, species);
							}
							Autotrophic_respiration (&m->cells[cell], layer, height, age, species);

							/* carbon fluxes */
							Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species]);

							/* C assimilation */
							Carbon_assimilation(&m->cells[cell], layer, height, age, species);

							/* C-N-partitioning-allocation */
							Daily_C_Evergreen_Partitioning_Allocation (&m->cells[cell], layer, height, age, species, meteo_daily, day, year);
							logger(g_log, "--------------------------------------------------------------------------\n");
						}

						/* check for balance closure at the class level */
						logger(g_log, "\n**CLASS LEVEL BALANCE**\n");

						/* check for radiative balance closure */
						Check_class_radiation_balance (&m->cells[cell], layer, height, age, species);

						/* check for carbon balance closure */
						Check_class_carbon_balance(&m->cells[cell], layer, height, age, species);

						/* check for water balance closure */
						Check_class_water_balance (&m->cells[cell], layer, height, age, species);
						/****************************************************************************************************************************************/

						/* SHARED FUNCTIONS FOR DECIDUOUS AND EVERGREEN */
						/* END OF YEAR */
						if (day == 30 && month == DECEMBER)
						{
							logger(g_log, "*****END OF YEAR******\n");
							/*FRUIT ALLOCATION*/
							/*
								if (m->cells[cell].heights[height].ages[age].value >= m->cells[cell].heights[height].ages[age].species[species].value[SEXAGE] && (m->cells[cell].heights[height].z == 2 || m->cells[cell].heights[height].z == 1))
								{
								Fruit_Allocation_LPJ ( &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z, years, Yearly_Rain, m->cells[cell].canopy_cover_dominant);
								Seeds_Number_LE = Fruit_Allocation_Logistic_Equation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
								logger(g_log, "Seeds Number from Logistic Equation = %d\n", Seeds_Number_LE);

								//Seeds_Number_T = Fruit_Allocation_TREEMIG ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
								//logger(g_log, "Seeds Number from TREEMIG = %d\n", Seeds_Number_T);

								//FRUIT ESTABLISHMENT
								if (Yearly_Rain > m->cells[cell].heights[height].ages[age].species[species].value[MINRAIN])
								{
								//decidere se passare numero di semi da LPJ o dall'Equazione Logistica
								Establishment_LPJ ( &m->cells[cell],layer, height, age, species);
								logger(g_log, "Saplings Number from LPJ = %d\n", m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP]);
								}
								else
								{
								logger(g_log, "Yearly Rain = %f\n", Yearly_Rain);
								logger(g_log, "Minimum Rain for Establishment = %f\n", m->cells[cell].heights[height].ages[age].species[species].value[MINRAIN]);
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
							/*LIGHT MORTALITY & GROWTH EFFICIENCY*/
							if(m->cells[cell].heights[height].z < m->cells[cell].top_layer)
							{
								/*Mortality based on Growth efficiency(LPJ)*/
								//Greff_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);
							}
							if ( m->cells[cell].heights[height].ages[age].species[species].management == C) Stool_mortality (&m->cells[cell], layer, height, age, species);

							//Renovation (&m->cells[cell], &m->cells[cell].heights[height], &m->cells[cell].heights[height].ages[age].species[species]);

							AGB_BGB_biomass(&m->cells[cell], height, age, species);

							Biomass_increment_EOY(&m->cells[cell], layer, height, age, species);

							/* print at the end of simulation class level data */
							Print_stand_data (&m->cells[cell], layer, height, age, species);

							Water_Use_Efficiency (&m->cells[cell].heights[height].ages[age].species[species]);

							/* management blocks */
							//Choose_management (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], years, height);

							/* simulate management */
							if(! string_compare_i(g_settings->management, "on") && year == 0)
							{
								rotation_counter = (int)m->cells[cell].heights[height].ages[age].species[species].value[ROTATION];
							}
							else if( ! string_compare_i(g_settings->management, "on") && year > 0)
							{
								if(year == (int)m->cells[cell].heights[height].ages[age].species[species].value[ROTATION])
								{
									Clearcut_Timber_without_request (&m->cells[cell], layer, height, age, species, year);

									/* add multiples */
									m->cells[cell].heights[height].ages[age].species[species].value[ROTATION] += rotation_counter;
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
						if(day == 30 && month == DECEMBER)
						{
							logger(g_log, "\n/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
							logger(g_log, "SAPLINGS\n");
							/*
								 Saplings_counter += 1;
								 logger(g_log, "-Number of Sapling class in layer 0 = %d\n", Saplings_counter);
							 */
							logger(g_log, "Age %d\n", m->cells[cell].heights[height].ages[age].value);
							//logger(g_log, "Species %s\n", m->cells[cell].heights[height].ages[age].species[species].name);

							if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 1)
							{
								if (m->cells[cell].par_for_establishment < g_settings->light_estab_very_tolerant)
								{
									m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
									logger(g_log, "NO Light for Establishment\n");
								}
							}
							else if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 2)
							{
								if (m->cells[cell].par_for_establishment < g_settings->light_estab_tolerant)
								{
									m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
									logger(g_log, "NO Light for Establishment\n");
								}
							}
							else if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 3)
							{
								if (m->cells[cell].par_for_establishment < g_settings->light_estab_intermediate)
								{
									m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
									logger(g_log, "NO Light for Establishment\n");
								}
							}
							else
							{
								if (m->cells[cell].par_for_establishment < g_settings->light_estab_intolerant)
								{
									m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
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
							"No trees for species %s s dbh %g height %g age %d are died!!!!\n"
							"**************************************************************\n\n",
							m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].ages[age].species[species].value[AVDBH],
							m->cells[cell].heights[height].value, m->cells[cell].heights[height].ages[age].value);
				}
			}
			logger(g_log, "****************END OF SPECIES CLASS***************\n");
		}
		logger(g_log, "****************END OF AGE CLASS***************\n");
	}
	logger(g_log, "****************END OF HEIGHT CLASS***************\n");

	/*******************************************************************************************************/
	/* note: computations that involve all classes and are related to the overall cell */
	/* computations during the last height class processing */
	/* compute soil respiration */
	Soil_respiration (&m->cells[cell]);
	/* compute soil evaporation */
	Soil_evaporation (&m->cells[cell], meteo_daily);
	/* compute evapotranspiration */
	Evapotranspiration (&m->cells[cell]);
	/* compute latent heat flux */
	Latent_heat_flux (&m->cells[cell], meteo_daily);
	/* compute sensible heat flux */
	Sensible_heat_flux (&m->cells[cell], meteo_daily);
	/* compute soil water balance */
	Soil_water_balance (&m->cells[cell], meteo_daily);
	/* compute water fluxes */
	Water_fluxes (&m->cells[cell]);

	/*******************************************************************************************************/
	/* CHECK FOR BALANCE CLOSURE */

	/* CHECK FOR RADIATIVE BALANCE CLOSURE */
	Check_radiation_balance (&m->cells[cell], meteo_daily);
	/* CHECK FOR CARBON BALANCE CLOSURE */
	Check_carbon_balance (&m->cells[cell]);
	/* CHECK FOR WATER BALANCE CLOSURE */
	Check_soil_water_balance (&m->cells[cell]);
	/*******************************************************************************************************/

	m->cells[cell].dos  += 1;
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


