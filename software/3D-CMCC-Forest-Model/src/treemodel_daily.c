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
#include "light.h"
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

int Tree_model_daily (matrix_t *const m, const int year, const int month, const int day, const int years_of_simulation, const int cell)
{
	meteo_t *met;

	static int height;
	static int age;
	static int species;

	//Yearly average met data
	static double Yearly_Solar_Rad;
	static double Yearly_Temp;
	static double Yearly_Vpd;
	static double Yearly_Rain;

	static int rotation_counter;

	/* check parameters */
	assert(m);
	met = m->cells[cell].years[year].m;


	//FIXME IT MUST BE USED FOR MULILAYERED SIMULATIONS!!!!!!!!!!!!!!!!!!

	//	/* daily loop on each cell before start with treemodel_daily */
	//	for ( cell = 0; cell < m->cells_count; cell++)
	//	{
	//		/* FOREST STRUCTURE */
	//		if (day == 0 && month == JANUARY)
	//		{
	//			/* compute annual number of different layers */
	//			Annual_numbers_of_layers (&m->cells[cell]);
	//		}
	//		/* daily forest structure */
	//		Daily_Forest_structure (&m->cells[cell], day, month, years);
	//		Daily_vegetative_period (&m->cells[cell], met, month, day);
	//		Daily_numbers_of_layers (&m->cells[cell]);
	//		Daily_layer_cover (&m->cells[cell], met, month, day);
	//		Daily_dominant_Light (m->cells[cell].heights, &m->cells[cell], m->cells[cell].heights_count, met, month, DaysInMonth[month]);
	//
	//		logger(g_log, "***************************************************\n");
	//	}
	//	for ( cell = 0; cell < m->cells_count; cell++)
	//	{


	/* compute daily-monthly-annual forest structure (overall cell) */
	if (day == 0 && month == JANUARY)Annual_numbers_of_layers (&m->cells[cell]);

	/* daily forest structure */
	//ALESSIOR
	//fixme it must be called in a previous "for" to compute the total number of layers, densities and other things as above
	//otherwise model cannot run for multi-layered purposes
	Daily_Forest_structure (&m->cells[cell], day, month, year);
	Daily_vegetative_period (&m->cells[cell], met, month, day);
	Daily_numbers_of_layers (&m->cells[cell]);
	Daily_layer_cover (&m->cells[cell], met, month, day);
	Daily_dominant_Light (m->cells[cell].heights, &m->cells[cell], m->cells[cell].heights_count, met, month, DaysInMonth[month]);

	logger(g_log, "%d-%d-%d\n", met[month].d[day].n_days, month+1, m->cells[cell].years[year].year);
	logger(g_log, "-YEAR SIMULATION = %d (%d)\n", year+1, m->cells[cell].years[year].year );
	logger(g_log, "--MONTH SIMULATED = %s\n", szMonth[month]);
	logger(g_log, "---DAY SIMULATED = %d\n", met[month].d[day].n_days);

	/* initialize days of year (each year) */
	if(day == 0 && month == 0) m->cells[cell].doy = 1;
	/* cumulate days of year (other days) */
	else m->cells[cell].doy += 1;

	/* compute average yearly met data */
	Yearly_Solar_Rad += met[month].d[day].solar_rad;
	Yearly_Vpd += met[month].d[day].vpd;
	Yearly_Temp += met[month].d[day].tavg;
	Yearly_Rain += met[month].d[day].prcp;

	/* print daily met data */
	Print_met_data (met, month, day);

	/* sort class in ascending way by heights */
	qsort (m->cells[cell].heights, m->cells[cell].heights_count, sizeof (height_t), sort_by_heights_asc);

	/* loop on each heights starting from highest to lower */
	logger(g_log, "******CELL x = %d, y = %d STRUCTURE******\n", m->cells[cell].x, m->cells[cell].y);
	for ( height = m->cells[cell].heights_count -1 ; height >= 0; height-- )
	{
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
					if(day == 0 && month == JANUARY && year == 0) First_day (&m->cells[cell], m->cells[cell].heights_count);

					/* beginning of simulation (every year included the first one) */
					if (day == 0 && month == JANUARY)
					{
						/* compute annual minimum reserve for incoming year */
						Annual_minimum_reserve(&m->cells[cell].heights[height].ages[age].species[species]);

						/* reset annual variables */
						Reset_annual_variables (&m->cells[cell], m->cells[cell].heights_count);

						/* compute annual prognostically Maximum LAI */
						Peak_lai(&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], year, month, day, height, age);
					}
					/* reset monthly variables */
					if (day == 0) Reset_monthly_variables (&m->cells[cell], m->cells[cell].heights_count);

					/* reset daily cell level variables */
					Reset_daily_variables(&m->cells[cell], m->cells[cell].heights_count);

					/* check precipitation and compute for snow if needs */
					Check_prcp (&m->cells[cell], met, month, day);

					/* compute species-specific phenological phase */
					Phenology (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], met, year, month, day);

					/* check for adult or sapling age */
					Tree_period (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell]);

					/* compute how many classes are in vegetative period */
					Daily_veg_counter (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], height);

					/* print at the beginning of simulation stand data */
					Print_stand_data (&m->cells[cell], month, year, height, age, species);

					/* Loop for adult trees */
					if (m->cells[cell].heights[height].ages[age].species[species].period == 0.0)
					{
						if (day == 0 && month == JANUARY)
						{
							if (!year)
							{
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_tDM] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM] +
										m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM];
							}
						}
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

								/* radiation block */
								Radiation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, m->cells[cell].years[year].year, month, day, DaysInMonth[month], height, age, species);

								/* compute daily modifier */
								Daily_modifiers (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell],
										met, month, day, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, height);

								/* nitrogen block */
								Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);

								/* canopy water fluxes block */
								canopy_evapotranspiration(&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);

								/* canopy carbon fluxes block */
								Phosynthesis (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, day, DaysInMonth[month], height, age, species);
								if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
								{
									Maintenance_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
									Growth_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
								}
								Autotrophic_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
								Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month);
								Carbon_assimilation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], year, month, day, height);
								Daily_C_Deciduous_Partitioning_Allocation(&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, year, height, age, species);
							}
							/*outside growing season*/
							else
							{
								/* assign zero value for LAI */
								if (g_settings->spatial == 'u') m->cells[cell].heights[height].ages[age].species[species].value[LAI] = 0;

								/* radiation block */
								Radiation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, m->cells[cell].years[year].year, month, day, DaysInMonth[month], height, age, species);

								/* nitrogen block */
								Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);

								/* canopy carbon fluxes block */
								Phosynthesis (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, day, DaysInMonth[month], height, age, species);
								if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
								{
									Maintenance_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
									Growth_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
								}
								Autotrophic_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
								Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month);
								Carbon_assimilation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], year, month, day, height);
								Daily_C_Deciduous_Partitioning_Allocation(&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, year, height, age, species);
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
							Radiation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, m->cells[cell].years[year].year, month, day, DaysInMonth[month], height, age, species);

							/* modifiers */
							Daily_modifiers (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell],
									met, month, day, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, height);

							/* canopy water fluxes block */
							canopy_evapotranspiration(&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);

							/* canopy carbon fluxes block */
							Phosynthesis (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, day, DaysInMonth[month], height, age, species);
							Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);
							if (!string_compare_i(g_settings->Prog_Aut_Resp, "on"))
							{
								Maintenance_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
								Growth_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
							}
							Autotrophic_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
							Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month);
							Carbon_assimilation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], year, month, day, height);
							Daily_C_Evergreen_Partitioning_Allocation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, year, DaysInMonth[month], height, age, species);
							logger(g_log, "--------------------------------------------------------------------------\n");
						}

						/* check for balance closure at the class level */
						logger(g_log, "\n**CLASS LEVEL BALANCE**\n");

						/* check for carbon balance closure */
						Check_class_carbon_balance(&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species]);

						/* check for water balance closure */
						Check_class_water_balance (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species]);
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
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = Establishment_LPJ ( &m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species]);
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
							if ( m->cells[cell].heights[height].ages[age].species[species].management == C) Stool_mortality (&m->cells[cell].heights[height].ages[age].species[species], year);

							//Renovation (&m->cells[cell], &m->cells[cell].heights[height], &m->cells[cell].heights[height].ages[age].species[species]);

							AGB_BGB_biomass(&m->cells[cell], height, age, species);

							Biomass_increment_EOY ( &m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].top_layer,  m->cells[cell].heights[height].z, height, age);

							/* print at the end of simulation class level data */
							Print_stand_data (&m->cells[cell], month, year, height, age, species);

							// ALESSIOR see utility.c... la funzione commentata non fa nulla!
							//Annual_average_values_modifiers (&m->cells[cell].heights[height].ages[age].species[species]);

							EOY_cumulative_balance_layer_level (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height]);

							Water_Use_Efficiency (&m->cells[cell].heights[height].ages[age].species[species]);

							/* management blocks */
							//Choose_management (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], years, height);
							/*
                           if ( m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] != 0 && m->cells[cell].heights[height].z == m->cells[cell].top_layer )
                           {

                                //create new class

                                ROW r;
                                int i;

                                //SET VALUES
                                m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE_SAPLING] = 0;
                                m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAPLING] = g_settings->lai_sapling;
                                //m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAPLING] = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAPLING] = g_settings->avdbh_sapling;
                                m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT_SAPLING] = g_settings->height_sapling;
                                m->cells[cell].heights[height].ages[age].species[species].value[WF_SAPLING] = g_settings->wf_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[WR_SAPLING] = g_settings->wr_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[WS_SAPLING] = g_settings->ws_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];

                                logger(g_log, "\n\n----A new Height class must be created---------\n\n");

                                //add values for a new a height class
                                //CREATE A NEW ROW
                                r.x = m->cells[cell].x;
                                r.y = m->cells[cell].y;
                                r.age = 0 ;
                                r.species = m->cells[cell].heights[height].ages[age].species[species].name;
                                r.phenology = m->cells[cell].heights[height].ages[age].species[species].phenology;
                                r.management = m->cells[cell].heights[height].ages[age].species[species].management;
                                r.lai = m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAPLING];
                                r.n = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAPLING];
                                r.avdbh = m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAPLING];
                                r.height = m->cells[cell].heights[height].ages[age].species[species].value[TREE_HELINGIGHT_SAPLING];
                                r.wf = m->cells[cell].heights[height].ages[age].species[species].value[WF_SAPLING];
                                r.wr = m->cells[cell].heights[height].ages[age].species[species].value[WR_SAPLING];
                                r.ws = m->cells[cell].heights[height].ages[age].species[species].value[WS_SAPLING];
							  	  logger(g_log, "....adding new row\n");
                                //create new height class
                                if ( !fill_cell_from_heights(m->cells, &r) )
                                {
                                logger(g_log, "UNABLE TO ADD NEW HEIGHT!!!\n");
                                return 0;
                                }
                                else
                                {
                                logger(g_log, "FILLED CELL!!\n");
                                }

                                //add new row  space for values of new height class

                                for ( i = 0; i < VALUES; i++ )
                                {
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[i] =
                                m->cells[cell].heights[height].ages[age].species[species].value[i];
                                }

                                for ( i = 0; i < COUNTERS; i++ )
                                {
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].counter[i] =
                                m->cells[cell].heights[height].ages[age].species[species].counter[i];
                                }


                                //pass new values to new height class

                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].period = 1;
                                m->cells[cell].heights[m->cells[cell].heights_count-1].value = m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].value = m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].name = m->cells[cell].heights[height].ages[age].species[species].name;
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].phenology = m->cells[cell].heights[height].ages[age].species[species].phenology;
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].management = m->cells[cell].heights[height].ages[age].species[species].management;
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[LAI] = m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAPLING];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].counter[N_TREE] = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAPLING];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[AVDBH] = m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAPLING];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[BIOMASS_FOLIAGE] = m->cells[cell].heights[height].ages[age].species[species].value[WF_SAPLING];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[BIOMASS_ROOTS_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[WR_SAPLING];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[BIOMASS_STEM] = m->cells[cell].heights[height].ages[age].species[species].value[WS_SAPLING];

                                //height class summary
                                logger(g_log, "**********************************\n");
                                logger(g_log, "x = %d\n", r.x);
                                logger(g_log, "y = %d\n", r.y);
                                logger(g_log, "age = %d\n", r.age);
                                logger(g_log, "species = %s\n", r.species);
                                logger(g_log, "phenology = %d\n", r.phenology);
                                logger(g_log, "management = %d\n", r.management);
                                logger(g_log, "lai = %f\n", r.lai);
                                logger(g_log, "n tree = %d\n", r.n);
                                logger(g_log, "avdbh = %f\n", r.avdbh);
                                logger(g_log, "height = %f\n", r.height);
                                logger(g_log, "wf = %f\n", r.wf);
                                logger(g_log, "wr = %f\n", r.wr);
                                logger(g_log, "ws = %f\n", r.ws);

                                Saplings_counter += 1;
                                logger(g_log, "Sapling Classes counter = %d\n", Saplings_counter);

                                logger(g_log, "*****************************\n");
                                logger(g_log, "*****************************\n");
                            }
                            if (m->cells[cell].heights[height].ages[age].species[species].period == 0)
                            {
                                logger(g_log, "....A NEW HEIGHT CLASS IS PASSING IN ADULT PERIOD\n");

                                m->cells[cell].heights[height].value = 1 ;
                                logger(g_log, "Height class passing from Sapling to Adult = %f m\n", m->cells[cell].heights[height].value);

                                //Saplings_counter -= 1;
                            }
							 */
							/* simulate management */
							if(! string_compare_i(g_settings->management, "on") && year == 0)
							{
								rotation_counter = (int)m->cells[cell].heights[height].ages[age].species[species].value[ROTATION];
							}
							else if( ! string_compare_i(g_settings->management, "on") && year > 0)
							{
								if(year == (int)m->cells[cell].heights[height].ages[age].species[species].value[ROTATION])
								{
									Clearcut_Timber_without_request (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], year);

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
							/*
								if (m->cells[cell].heights[height].ages[age].species[species].period == 0)
								{
									logger(g_log, "....A NEW HEIGHT CLASS IS PASSING IN ADULT PERIOD\n");

									Saplings_counter -= 1;
								}
							 */
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

	/* note: computations that involve all classes and are related to the overall cell */
	/* computations during the last height class processing */
	/* compute soil respiration */
	Soil_respiration (&m->cells[cell]);
	/* compute soil evaporation */
	Soil_evaporation (&m->cells[cell], met, month, day);
	/* compute evapotranspiration */
	Evapotranspiration (&m->cells[cell]);
	/* compute latent heat flux */
	Latent_heat_flux (&m->cells[cell], met, month, day);
	/* compute soil water balance */
	Soil_water_balance (&m->cells[cell], met, month, day);
	/* compute water fluxes */
	Water_fluxes (&m->cells[cell]);
	/* CHECK FOR CARBON BALANCE CLOSURE */
	Check_carbon_balance (&m->cells[cell]);
	/* CHECK FOR WATER BALANCE CLOSURE */
	Check_soil_water_balance (&m->cells[cell]);
	//todo
	/* CHECK FOR RADIATIVE BALANCE CLOSURE */
	//Check_radiative_balance (&m->cells[cell]);

	m->cells[cell].dos  += 1;
	//todo: soilmodel could stay here or in main.c
	//here is called at the end of all tree height age and species classes loops
	//todo: move all soil algorithms into soil_model function
	//soil_model (&m->cells[cell], yos, years, month, years_of_simulation);
	//N_avl = (Ka * g_soil->values[SOIL_sN) + pN + (Kb * Yearly_Eco_NPP);
	//logger(g_log, "Nitrogen available = %f g m^-2\n", N_avl);
	//}
	logger(g_log, "****************END OF CELL***************\n");

	/* ok */
	return 1;
}


