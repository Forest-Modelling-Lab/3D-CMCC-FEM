/* treemodel.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include "constants.h"

extern const char *szMonth[MONTHS];
//extern int MonthLength[];
extern int DaysInMonth[];
//extern int fill_cell_from_heights(CELL *const c, const ROW *const row);


/* */
int Tree_model_daily (MATRIX *const m, const YOS *const yos, const int years, const int month, const int day, const int years_of_simulation)
{
	MET_DATA *met;

	static int cell;
	static int height;
	static int age;
	static int species;

	//Yearly average met data
	static double Yearly_Solar_Rad;
	static double Yearly_Temp;
	static double Yearly_Vpd;
	static double Yearly_Rain;

	//SOIL NITROGEN CONTENT see Peng et al., 2002
	//static double N_avl;  //Total nitrogen available for tree growth see Peng et al., 2002
	//const double Ka = 0.6;  //see Peng et al., 2002
	//const double Kb = 0.0005; //see Peng et al., 2002

	// check parameters
	assert(m && yos);
	met = (MET_DATA*) yos[years].m;

	//annual daily loop on each cell before start with treemodel_daily
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		//*************FOREST CHARACTERISTIC*********************
		if (day == 0 && month == JANUARY)
		{
			//annual forest structure
			Annual_numbers_of_layers (&m->cells[cell]);
		}
		/* daily forest structure */
		Forest_structure (&m->cells[cell], day, month, years);
		Daily_vegetative_period (&m->cells[cell], met, month, day);
		Daily_numbers_of_layers (&m->cells[cell]);
		Daily_layer_cover (&m->cells[cell], met, month, day);
		//Print_parameters (&m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].ages[age].species_count, month, years);
		Dominant_Light (m->cells[cell].heights, &m->cells[cell],  m->cells[cell].heights_count, met, month, DaysInMonth[month]);
		Log("***************************************************\n");
	}

	for ( cell = 0; cell < m->cells_count; cell++)
	{
		Log("%d-%d-%d\n", met[month].d[day].n_days, month+1, yos[years].year);
		Log("-YEAR SIMULATION = %d (%d)\n", years+1, yos[years].year );
		Log("--MONTH SIMULATED = %s\n", szMonth[month]);
		Log("---DAY SIMULATED = %d\n", met[month].d[day].n_days);

		m->cells[cell].doy += 1;
		if(day == 0 && month == 0) m->cells[cell].doy = 1;

		//average yearly met data
		Yearly_Solar_Rad += met[month].d[day].solar_rad;
		Yearly_Vpd += met[month].d[day].vpd;
		Yearly_Temp += met[month].d[day].tavg;
		Yearly_Rain += met[month].d[day].prcp;

		Print_met_data (met, month, day);
		/* compute latent heat values */
		Latent_heat (&m->cells[cell], met, month, day);


		/* sort by heights */
		qsort (m->cells[cell].heights, m->cells[cell].heights_count, sizeof (HEIGHT), sort_by_heights_asc);

		/* loop on each heights starting from highest to lower */
		Log("******CELL x = %d, y = %d STRUCTURE******\n", m->cells[cell].x, m->cells[cell].y);
		for ( height = m->cells[cell].heights_count -1 ; height >= 0; height-- )
		{
			/*loop on each ages*/
			for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				/*increment age after first year*/
				if( day == 0 && month == JANUARY && years != 0)
				{
					m->cells[cell].heights[height].ages[age].value += 1;
				}
				/*loop on each species*/
				for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
				{
					if(m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE] > 0)
					{
						/* beginning of simulation */
						if(day == 0 && month == JANUARY && years == 0)
						{
							First_day (&m->cells[cell], m->cells[cell].heights_count);
						}
						/* reset annual variables */
						if (day == 0 && month == JANUARY)
						{
							Biomass_increment_BOY ( &m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], height, age, years);
							Reset_annual_variables (&m->cells[cell], m->cells[cell].heights_count);
						}
						/* reset monthly variables */
						if (day == 0)
						{
							Reset_monthly_variables (&m->cells[cell], m->cells[cell].heights_count);
						}
						/* reset daily variables */
						Reset_daily_variables(&m->cells[cell], m->cells[cell].heights_count);

						/* check and compute for snow */
						Check_prcp (&m->cells[cell], met, month, day);

						//test new function
						simple_phenology_phase (&m->cells[cell].heights[height].ages[age].species[species], met, years, month, day);
						/* compute species-specific phenological phase */
						//Phenology_phase (&m->cells[cell].heights[height].ages[age].species[species], met, years, month, day);
						Tree_period (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell]);
						Daily_veg_counter (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], height);
						Print_init_month_stand_data (&m->cells[cell], met, month, years, height, age, species);

						/* Loop for adult trees */
						if (m->cells[cell].heights[height].ages[age].species[species].period == 0.0)
						{
							if (day == 0 && month == JANUARY)
							{
								if (!years)
								{
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_tDM] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM]								                                                                                                                                                          + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM];
								}
							}
							/* loop for deciduous */
							if ( m->cells[cell].heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || m->cells[cell].heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
							{
								if (day == 0 && month == JANUARY)
								{
									Peak_lai_from_pipe_model (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], years, month, day, height, age);
								}
								/* vegetative period for deciduous */
								if (m->cells[cell].heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
								{
									Log("\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", m->cells[cell].heights[height].ages[age].species[species].name );
									Log("--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);
									m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS] += 1;
									Log("VEG_DAYS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS]);
									Radiation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, DaysInMonth[month], height);
									Daily_modifiers (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell],
											met, month, day, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, height);

									/* canopy water fluxes block */
									//Canopy_interception (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
									//Canopy_transpiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);
									//Canopy_transpiration_biome (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);
									//Canopy_evapotranspiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);


									//test
									canopy_evapotranspiration_biome (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);


									/* canopy carbon fluxes block */
									Phosynthesis (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, day, DaysInMonth[month], height, age, species);
									Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);
									Maintenance_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
									Growth_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month, years);
									Autotrophic_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
									Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month);
									Carbon_assimilation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], years, month, day, height);
									//Deciduous_Partitioning_Allocation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, years, DaysInMonth[month],  height, age, species);
									simple_Deciduous_Partitioning_Allocation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, years, DaysInMonth[month],  height, age, species);
									Turnover  (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], DaysInMonth[month], height);
								}
								/*outside growing season*/
								else
								{
									if (settings->spatial == 'u')
									{
										m->cells[cell].heights[height].ages[age].species[species].value[LAI] = 0;
										Log("day %d month %d MODEL_LAI = %f \n", day+1, month+1, m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
									}
									else
									{
										Log("++Lai layer %d = %f\n", m->cells[cell].heights[height].z, met[month].d[day].ndvi_lai);
									}
									Radiation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, DaysInMonth[month], height);
									Phosynthesis (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, day, DaysInMonth[month], height, age, species);
									Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);
									Maintenance_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
									Growth_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month, years);
									Autotrophic_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
									Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month);
									Carbon_assimilation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], years, month, day, height);
									//Deciduous_Partitioning_Allocation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, years, DaysInMonth[month], height, age, species);
									simple_Deciduous_Partitioning_Allocation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, years, DaysInMonth[month],  height, age, species);
									Turnover (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], DaysInMonth[month], height);
								}
							}
							/* loop for evergreen */
							else
							{
								//Peak LAI is also used in spatial version to drive carbon allocation
								if (day == 0 && month == JANUARY)
								{
									Peak_lai_from_pipe_model (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], years, month, day, height, age);
								}
								Log("*****VEGETATIVE PERIOD FOR %s SPECIES *****\n", m->cells[cell].heights[height].ages[age].species[species].name);
								Log("--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);

								m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS] += 1;
								Log("VEG_DAYS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_DAYS]);

								Radiation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, DaysInMonth[month], height);
								/* modifiers */
								Daily_modifiers (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell],
										met, month, day, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, height);

								/* canopy water fluxes block */
								Canopy_interception (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
								//Canopy_transpiration ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);
								Canopy_transpiration_biome (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height, age, species);
								Canopy_evapotranspiration ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);

								/* canopy carbon fluxes block */
								Phosynthesis (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, day, DaysInMonth[month], height, age, species);
								Nitrogen_stock (&m->cells[cell].heights[height].ages[age].species[species]);
								Maintenance_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, day, height);
								Growth_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month, years);
								Autotrophic_respiration (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height);
								Carbon_fluxes (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], height, day, month);
								Carbon_assimilation (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], years, month, day, height);
								Evergreen_Partitioning_Allocation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, day, month, years, DaysInMonth[month], height, age, species);
								Turnover (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], DaysInMonth[month], height);
								Log("--------------------------------------------------------------------------\n\n\n");
							}
							/* SHARED FUNCTIONS FOR DECIDUOUS AND EVERGREEN */
							//to prevent jumps in dendrometric values it must be computed at the beginning of each month
							if (day == 0)
							{
								Average_tree_biomass (&m->cells[cell].heights[height].ages[age].species[species]);
								/* DENDROMETRY */
								Dendrometry (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], m->cells[cell].heights_count);
							}
							/* END OF YEAR */
							if (day == 30 && month == DECEMBER)
							{
								Log("*****END OF YEAR******\n");
								/*FRUIT ALLOCATION*/
								//Only for dominant layer

								//Log("Dominant Canopy Cover = %f\n", m->cells[cell].canopy_cover_dominant);

								/*
							   if (m->cells[cell].heights[height].ages[age].value >= m->cells[cell].heights[height].ages[age].species[species].value[SEXAGE] && (m->cells[cell].heights[height].z == 2 || m->cells[cell].heights[height].z == 1))
							   {
							   Fruit_Allocation_LPJ ( &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z, years, Yearly_Rain, m->cells[cell].canopy_cover_dominant);
							   Seeds_Number_LE = Fruit_Allocation_Logistic_Equation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
							   Log("Seeds Number from Logistic Equation = %d\n", Seeds_Number_LE);

								//Seeds_Number_T = Fruit_Allocation_TREEMIG ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
								//Log("Seeds Number from TREEMIG = %d\n", Seeds_Number_T);

								//FRUIT ESTABLISHMENT
								if (Yearly_Rain > m->cells[cell].heights[height].ages[age].species[species].value[MINRAIN])
								{
								//decidere se passare numero di semi da LPJ o dall'Equazione Logistica
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = Establishment_LPJ ( &m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species]);
								Log("Saplings Number from LPJ = %d\n", m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP]);
								}
								else
								{
								Log("Yearly Rain = %f\n", Yearly_Rain);
								Log("Minimum Rain for Establishment = %f\n", m->cells[cell].heights[height].ages[age].species[species].value[MINRAIN]);
								Log("NOT ENOUGH RAIN FOR ESTABLISHMENT\n");
								}
								}
								 */

								Total_class_level_biomass (&m->cells[cell].heights[height].ages[age].species[species]);
								Water_Use_Efficiency (&m->cells[cell].heights[height].ages[age].species[species]);
								//Average_tree_biomass (&m->cells[cell].heights[height].ages[age].species[species]);
								Log("*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n");
								/*MORTALITY*/
								//todo CONTROLLARE E SOMMARE AD OGNI STRATO LA BIOMASSA DI QUELLA SOVRASTANTE
								Log("Get_Mortality COMMENTATA per bug, REINSERIRE!!!!!!!!!!!!!!!!!\n");
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
									//Log("Greff_Mortality COMMENTATA per bug, REINSERIRE!!!!!!!!!!!!!!!!!\n");
									/*Mortality based on Growth efficiency(LPJ)*/
									//Greff_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);
								}
								if ( m->cells[cell].heights[height].ages[age].species[species].management == C)
								{
									Stool_mortality (&m->cells[cell].heights[height].ages[age].species[species], years);
								}
								//Renovation (&m->cells[cell], &m->cells[cell].heights[height], &m->cells[cell].heights[height].ages[age].species[species]);

								/*CROWDING COMPETITION-BIOMASS RE-ALLOCATION*/
								//DON'T DELETE IT!!!!
								//currently not used
								//Crowding_competition (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], m->cells[cell].heights[height].z, years, m->cells[cell].top_layer);

								//ABG and BGB
								AGB_BGB_biomass(&m->cells[cell], height, age, species);

								//DENDROMETRY
								//Get_dendrometry (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], m->cells[cell].heights_count);

								//TURNOVER
								//FIXME MOVE IT TO MONTHLY TIME STEP AT THE END OF EACH MONTH
								//Turnover ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], DaysInMonth[month], height);

								//ANNUAL BIOMASS INCREMENT
								Biomass_increment_EOY ( &m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].top_layer,  m->cells[cell].heights[height].z, height, age);

								Print_end_month_stand_data (&m->cells[cell], yos, met, month, years, height, age, species);

								Annual_average_values_modifiers (&m->cells[cell].heights[height].ages[age].species[species]);

								EOY_cumulative_balance_layer_level (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height]);

								//MANAGEMENT
								//Choose_management (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], years, height);
								/*
                           if ( m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] != 0 && m->cells[cell].heights[height].z == m->cells[cell].top_layer )
                           {

                                //create new class

                                ROW r;
                                int i;

                                //SET VALUES
                                m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE_SAPLING] = 0;
                                m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAPLING] = settings->lai_sapling;
                                //m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAPLING] = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAPLING] = settings->avdbh_sapling;
                                m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT_SAPLING] = settings->height_sapling;
                                m->cells[cell].heights[height].ages[age].species[species].value[WF_SAPLING] = settings->wf_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[WR_SAPLING] = settings->wr_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[WS_SAPLING] = settings->ws_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];


                                Log("\n\n----A new Height class must be created---------\n\n");

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
							  	  Log("....adding new row\n");
                                //create new height class
                                if ( !fill_cell_from_heights(m->cells, &r) )
                                {
                                Log("UNABLE TO ADD NEW HEIGHT!!!\n");
                                return 0;
                                }
                                else
                                {
                                Log("FILLED CELL!!\n");
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
                                Log("**********************************\n");
                                Log("x = %d\n", r.x);
                                Log("y = %d\n", r.y);
                                Log("age = %d\n", r.age);
                                Log("species = %s\n", r.species);
                                Log("phenology = %d\n", r.phenology);
                                Log("management = %d\n", r.management);
                                Log("lai = %f\n", r.lai);
                                Log("n tree = %d\n", r.n);
                                Log("avdbh = %f\n", r.avdbh);
                                Log("height = %f\n", r.height);
                                Log("wf = %f\n", r.wf);
                                Log("wr = %f\n", r.wr);
                                Log("ws = %f\n", r.ws);

                                Saplings_counter += 1;
                                Log("Sapling Classes counter = %d\n", Saplings_counter);

                                Log("*****************************\n");
                                Log("*****************************\n");
                            }
                            if (m->cells[cell].heights[height].ages[age].species[species].period == 0)
                            {
                                Log("....A NEW HEIGHT CLASS IS PASSING IN ADULT PERIOD\n");

                                m->cells[cell].heights[height].value = 1 ;
                                Log("Height class passing from Sapling to Adult = %f m\n", m->cells[cell].heights[height].value);

                                //Saplings_counter -= 1;
                            }
								 */
							}
						}
						/*FUNCTIONS FOR SAPLINGS*/
						else
						{
							if(day == 30 && month == DECEMBER)
							{
								Log("\n/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
								Log("SAPLINGS\n");
								/*
								 Saplings_counter += 1;
								 Log("-Number of Sapling class in layer 0 = %d\n", Saplings_counter);
								 */
								Log("Age %d\n", m->cells[cell].heights[height].ages[age].value);
								//Log("Species %s\n", m->cells[cell].heights[height].ages[age].species[species].name);

								if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 1)
								{
									if (m->cells[cell].par_for_establishment < settings->light_estab_very_tolerant)
									{
										m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
										Log("NO Light for Establishment\n");
									}
								}
								else if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 2)
								{
									if (m->cells[cell].par_for_establishment < settings->light_estab_tolerant)
									{
										m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
										Log("NO Light for Establishment\n");
									}
								}
								else if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 3)
								{
									if (m->cells[cell].par_for_establishment < settings->light_estab_intermediate)
									{
										m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
										Log("NO Light for Establishment\n");
									}
								}
								else
								{
									if (m->cells[cell].par_for_establishment < settings->light_estab_intolerant)
									{
										m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
										Log("NO Light for Establishment\n");
									}
								}
								/*
								if (m->cells[cell].heights[height].ages[age].species[species].period == 0)
								{
								Log("....A NEW HEIGHT CLASS IS PASSING IN ADULT PERIOD\n");

								Saplings_counter -= 1;
								}
								 */
								Log("/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
							}
						}

						/* simulate management */
						 //toconclude set function for mulple of rotation
						 //fixme include management choice
						if( ! mystricmp(settings->management, "on")
							&& ( (years == (int)m->cells[cell].heights[height].ages[age].species[species].value[ROTATION])
								|| ! ((int)m->cells[cell].heights[height].ages[age].species[species].value[ROTATION] % years)) )
						{
							Clearcut_Timber_without_request (&m->cells[cell].heights[height].ages[age].species[species], years);
						}
					}
					else
					{
						Log("\n\n**************************************************************\n"
						"No trees for species %s s dbh %g height %g age %d are died!!!!\n"
						"**************************************************************\n\n",
						m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].ages[age].species[species].value[AVDBH],
						m->cells[cell].heights[height].value, m->cells[cell].heights[height].ages[age].value);
					}
				}
				Log("****************END OF SPECIES CLASS***************\n");
			}
			Log("****************END OF AGE CLASS***************\n");
		}
		Log("****************END OF HEIGHT CLASS***************\n");

		/*compute soil respiration*/
		Soil_respiration (&m->cells[cell]);
		/*compute soil evaporation-cell evapotranspiration-cell water balance in the last loop of height*/
		//Soil_evaporation (&m->cells[cell], met, month, day);
		//test
		soil_evaporation_biome (&m->cells[cell], met, month, day);
		/*compute evapotranspiration*/
		Evapotranspiration (&m->cells[cell]);
		/*compute latent heat flux*/
		Latent_heat_flux (&m->cells[cell]);
		/*compute soil water balance*/
		Soil_water_balance (&m->cells[cell], met, month, day);
		/*compute water fluxes*/
		Water_fluxes (&m->cells[cell]);
		/*CHECK FOR CARBON BALANCE CLOSURE*/
		//fixme
		Check_carbon_balance (&m->cells[cell]);
		/*CHECK FOR WATER BALANCE CLOSURE*/
		Check_water_balance (&m->cells[cell]);
		/*CHECK FOR CARBON FLUX BALANCE CLOSURE*/
		//fixme
		Check_C_flux_balance (&m->cells[cell]);

		m->cells[cell].dos  += 1;
		//todo: soilmodel could stay here or in main.c
		//here is called at the end of all tree height age and species classes loops
		//todo: move all soil algorithms into soil_model function
		//soil_model (&m->cells[cell], yos, years, month, years_of_simulation);
		//N_avl = (Ka * site->sN) + pN + (Kb * Yearly_Eco_NPP);
		//Log("Nitrogen available = %f g m^-2\n", N_avl);
	}
	Log("****************END OF CELL***************\n");
	/* ok */
	return 1;
}


