/* treemodel.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"

extern  const char *szMonth[MONTHS];
extern int MonthLength[];
extern int DaysInMonth[];
//extern int fill_cell_from_heights(CELL *const c, const ROW *const row);


/* */
int tree_model(MATRIX *const m, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	MET_DATA *met;

	static int cell;
	static int height;
	static int age;
	static int species;

	//static int tree_soil;
	static float Light_Absorb_for_establishment;
	static float Light_for_establishment;

	//Yearly average met data
	static float Yearly_Solar_Rad;
	static float Yearly_Temp;
	static float Yearly_Vpd;
	static float Yearly_Rain;
	//static float Total_Rain;


	//compute VPD
	static float vpd;
	/*WATER BALANCE*/


	/*EVAPOTRASPIRATION*/
	static float RainIntercepted;
	//static float thermic_sum;
	static float lessrain;

	/*fruit*/
	/*logistic equation*/
	//static int Seeds_Number_LE;
	/*treemig approach*/
	//static int Seeds_Number_T;
	static int Saplings_counter;


	/*establishment*/
	//static int Saplings_Number;

	//static int strata;

	//SOIL NITROGEN CONTENT see Peng et al., 2002
	//static float N_avl;  //Total nitrogen available for tree growth see Peng et al., 2002
	//const float Ka = 0.6;  //see Peng et al., 2002
	//const float Kb = 0.0005; //see Peng et al., 2002

	// check parameters
	assert(m && yos);
	met = (MET_DATA*) yos[years].m;


	if (!month && !years)
	{
		if (m->cells[cell].heights[height].ages[age].species[species].value[FRACBB0] == 0)
		{
			Log("I don't have FRACBB0 = FRACBB1 \n");
			m->cells[cell].heights[height].ages[age].species[species].value[FRACBB0] = m->cells[cell].heights[height].ages[age].species[species].value[FRACBB1];
			Log("FRACBB0 = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[FRACBB0]);
		}
	}

	/*somma termica per l'inizio della stagione vegetativa*/
	//thermic_sum = met[month].tav * DaysInMonth [month];

	//monthly loop on each cell
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		//*************FOREST STRUCTURE*********************
		if (month == JANUARY)
		{
			//annual forest structure
			Get_annual_numbers_of_layers (&m->cells[cell]);
			Get_annual_forest_structure (&m->cells[cell], &m->cells[cell].heights[height]);
		}

		//monthly forest structure
		Get_monthly_vegetative_period (&m->cells[cell], met, month);
		Get_monthly_numbers_of_layers (&m->cells[cell]);
		Get_monthly_layer_cover (&m->cells[cell],  met, month);
		//Print_parameters (&m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].ages[age].species_count, month, years);
		Get_Dominant_Light (m->cells[cell].heights, &m->cells[cell],  m->cells[cell].heights_count, met, month, DaysInMonth[month]);

		Log("***************************************************\n");
	}

	for ( cell = 0; cell < m->cells_count; cell++)
	{
		/*DayLength*/
		GetDayLength (&m->cells[cell], MonthLength[month]);
		/*Abscission DayLength*/
		Get_Abscission_DayLength (&m->cells[cell]);
		//compute vpd from RH
		vpd = Get_vpd (met, month);

		//average yearly met data
		Yearly_Solar_Rad += met[month].solar_rad;
		Yearly_Vpd += vpd;
		Yearly_Temp += met[month].tav;
		Yearly_Rain += met[month].rain;


		if ( month == JANUARY  && !years)
		{
			m->cells[cell].available_soil_water = site->initialAvailableSoilWater + met[month].rain;
			Log("Beginning month  %d ASW = %g mm\n", month  + 1 , m->cells[cell].available_soil_water);
		}
		else
		{
			m->cells[cell].available_soil_water +=  met[month].rain;
			Log("Beginning month  %d ASW = %g mm\n", month + 1, m->cells[cell].available_soil_water);
		}


		//control
		if (m->cells[cell].available_soil_water > site->maxAsw)
		{
			Log("ASW > MAXASW !!!\n");
			//if the asw exceeds maxasw the plus is considered lost for turn off
			m->cells[cell].available_soil_water = site->maxAsw;
			Log("ASW month %d = %g mm\n", month + 1, m->cells[cell].available_soil_water);
		}



		//compute moist ratio
		m->cells[cell].soil_moist_ratio = m->cells[cell].available_soil_water / site->maxAsw;
		Log("Moist ratio outside modifier = %g\n", m->cells[cell].soil_moist_ratio);

		m->cells[cell].av_soil_moist_ratio += m->cells[cell].soil_moist_ratio;

		Log("YEAR SIMULATED = %d (%d)\n", years + 1, yos[years].year );
		Log("MONTH SIMULATED = %s\n", szMonth[month]);

		Print_met_data (met, vpd,  month,  m->cells[cell].daylength);

		//for each month of simulation the model recomputes the number of classes in vegetative period
		m->cells[cell].dominant_veg_counter = 0;
		m->cells[cell].dominated_veg_counter = 0;
		m->cells[cell].subdominated_veg_counter = 0;

		// sort by heights
		qsort (m->cells[cell].heights, m->cells[cell].heights_count, sizeof (HEIGHT), sort_by_heights_asc);

		//loop on each heights starting from highest to lower

		Log("*****************CELL x = %g, y = %g STRUCTURE*********************\n", m->cells[cell].x, m->cells[cell].y);

		Log("ASW month %d = %g mm\n", month + 1, m->cells[cell].available_soil_water);

		for ( height = m->cells[cell].heights_count -1 ; height >= 0; height-- )
		{
			//Log("*RUN FOR TREE LAYERS\n");
			//loop on each ages
			for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				/*increment age*/
				if( month == JANUARY)
				{
					if (years != 0)
					{
						m->cells[cell].heights[height].ages[age].value += 1;
					}
				}
				/*Set Tree period*/
				if ( m->cells[cell].heights[height].ages[age].value >= settings->adult_age)
				{
					if ( m->cells[cell].heights[height].ages[age].period == 1)
					{
						m->cells[cell].heights[height].ages[age].period = 0;
						Saplings_counter -= 1;
					}
					else
					{
						m->cells[cell].heights[height].ages[age].period = 0;
						//Log("- Class Period = Adult Trees \n");
					}
				}
				else
				{
					m->cells[cell].heights[height].ages[age].period = 1;
					//Log("- Class Period = Saplings\n");
				}

				/*Loop for adult trees*/
				if (m->cells[cell].heights[height].ages[age].value >= settings->adult_age)
				{
					//loop on each species
					for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
					{
						Print_init_month_stand_data (&m->cells[cell], met, month, years, height, age, species);

						if (month == JANUARY)
						{
							Reset_annual_cumulative_variables (&m->cells[cell], m->cells[cell].heights_count);

							if (!years)
							{
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]
								                                                                                                                                                          + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM];
							}
						}

						/*modifiers*/
						Get_modifiers (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell], met, years, month, DaysInMonth[month], m->cells[cell].available_soil_water, vpd, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management);

						//deciduous
						if ( m->cells[cell].heights[height].ages[age].species[species].phenology == D )
						{
							//for unspatial version growth start, growthend/mindaylength and month drives start and end of growing season
							//PEAK LAI
							//Get_peak_lai (&m->cells[cell].heights[height].ages[age].species[species], years, month);

							//Peak LAI is also used in spatial version to drive carbon allocation
							if (month == JANUARY)
							{
								Get_peak_lai_from_pipe_model (&m->cells[cell].heights[height].ages[age].species[species], years, month);
							}

							//vegetative period for deciduous
							if (m->cells[cell].heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
							{
								Log("*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", m->cells[cell].heights[height].ages[age].species[species].name );

								Log("--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);

								Log("Month = %s\n", szMonth[month]);
								m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS] += 1;
								Log("VEG_MONTHS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);

								if (m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS] == 1 && settings->version == 'u')
								{
									Get_initial_month_lai (&m->cells[cell].heights[height].ages[age].species[species]);
								}

								Get_light (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], height);


								Get_canopy_transpiration ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], vpd);


								/*Canopy evaporation of intercepted rainfall*/
								if ( met[month].rain > 0 )
								{
									Get_frac_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

									//taking into account cell coverage
									if ( m->cells[cell].heights[height].z == m->cells[cell].top_layer)
									{

										//Rainfall intercepted
										//heighest height class
										//interception for the highest of the the dominant class
										if (m->cells[cell].dominant_veg_counter == 1)
										{
											RainIntercepted = met[month].rain
													* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
													                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
											Log("Percentage of Rain Intercepted from dominant canopy = %g%%\n", (RainIntercepted * 100) / met[month].rain );



											/*Evapotranspiration*/

											m->cells[cell].evapotranspiration = (met[month].rain
													* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
													                                                                  + m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP])
													                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											//control
											if (RainIntercepted >= met[month].rain)
											{
												RainIntercepted = met[month].rain;
												Log("Rain is completely intercepted by the highest height class in the dominant layer\n");
												lessrain = 0;
											}
											else
											{
												lessrain = met[month].rain - RainIntercepted;
											}
										}
										else
										{
											Log("Less Rain = %g mm\n", lessrain);
											if (lessrain <= 0)
											{
												Log("Rainfall is completely intercepted from the upper layer\n");
											}
											else
											{
												RainIntercepted = lessrain
														* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
														                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
												Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
											}

											m->cells[cell].evapotranspiration = (lessrain
													* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
													                                                                  + m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP])
													                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											lessrain -= RainIntercepted;

										}
									}
									//dominated layers
									else
									{
										Get_frac_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

										Log("Rain Interception rate for dominated layer = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC] );

										if (lessrain > 0)
										{
											RainIntercepted = lessrain
													* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
													                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
										}
										else
										{
											RainIntercepted = 0;
											Log("No Rainfall for this layer\n");
										}

										m->cells[cell].evapotranspiration = (lessrain
												* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
												                                                                  + m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP])
												                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										lessrain -= RainIntercepted;
									}
								}
								else
								{
									Log("NO RAIN-NO INTERCEPTION\n");
									m->cells[cell].evapotranspiration =  m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP];
								}
								//compute soil evaporation in the last loop of height
								if( height == 0)
								{
									Get_soil_evaporation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], m->cells[cell].net_radiation, m->cells[cell].top_layer, m->cells[cell].heights[height].z,
											m->cells[cell].net_radiation_for_dominated, m->cells[cell].net_radiation_for_subdominated, m->cells[cell].Veg_Counter, m->cells[cell].daylength);

									m->cells[cell].evapotranspiration += m->cells[cell].soil_evaporation;
									Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );
								}



								m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] += m->cells[cell].evapotranspiration;
								Log("Cumulated Evapotranspiration for this layer = %g mm\n", m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]);

								m->cells[cell].total_yearly_evapotransipration += m->cells[cell].evapotranspiration;
								Log("TOTAL Cumulated Evapotranspiration = %g mm\n",m->cells[cell].total_yearly_evapotransipration);


								/* Soil Water Balance*/
								Log("\n\n-----------------------------------------------\n");

								Get_soil_water_balance (&m->cells[cell]);

								/*reset Evapotranspiration*/
								m->cells[cell].evapotranspiration = 0;

								Get_phosynthesis_monteith (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, DaysInMonth[month], height);

								M_D_Get_Partitioning_Allocation_CTEM ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month,  DaysInMonth[month], years,  height, age, species);

								Log("--------------------------------------------------------------------------\n\n\n");

								if (m->cells[cell].heights_count == 0)
								{
									Light_Absorb_for_establishment = (m->cells[cell].par_for_soil / m->cells[cell].par_over_dominant_canopy);
									Log("PAR OVER CANOPY = %g \n",  m->cells[cell].par_over_dominant_canopy);
									Log("PAR FOR SOIL = %g \n", m->cells[cell].par_for_soil);
									Log("Average Light Absorbed for establishment = %g \n", Light_Absorb_for_establishment);


								}
								if (settings->version == 'u')
								{
									Log("PHENOLOGY LAI = %g \n", m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
								}
							}
							else
							{
								Log("**UN-VEGETATIVE PERIOD FOR %s SPECIES in layer %d **\n", m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].z);


								if (settings->version == 'u')
								{
									m->cells[cell].heights[height].ages[age].species[species].value[LAI] = 0;
									Log("++Lai layer %d = %g\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
									//Log("PHENOLOGY LAI = %g \n", m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
								}
								else
								{
									Log("++Lai layer %d = %g\n", m->cells[cell].heights[height].z, met[month].ndvi_lai);
								}

								//Productivity
								Get_phosynthesis_monteith (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, DaysInMonth[month], height);

								M_D_Get_Partitioning_Allocation_CTEM ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month,  DaysInMonth[month], years,  height, age, species);

								/* Soil Water Balance*/

								Log("-----------------------------------------------------------------\n");
								Log("********MONTHLY SOIL WATER BALACE in UNVEGETATIVE PERIOD*********\n");
								//compute soil evaporation in the last loop of height
								if( height == 0)
								{
									Get_soil_evaporation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], m->cells[cell].net_radiation,  m->cells[cell].heights[height].top_layer, m->cells[cell].heights[height].z,
											m->cells[cell].net_radiation_for_dominated, m->cells[cell].net_radiation_for_subdominated, m->cells[cell].Veg_Counter, m->cells[cell].daylength);

									if (m->cells[cell].available_soil_water <= m->cells[cell].soil_evaporation )
									{
										Log ("ATTENTION EVAPORATION EXCEEDS ASW!!!! \n");
									}

									m->cells[cell].available_soil_water -=  m->cells[cell].soil_evaporation;
									//Log("Available Soil Water at month %d years %d with LPJ soil evaporation = %g mm\n",month, years, m->cells[cell].available_soil_water);

									m->cells[cell].total_yearly_soil_evaporation += m->cells[cell].soil_evaporation;
									Log("Total Soil Evaporation = %g mm\n", m->cells[cell].total_yearly_soil_evaporation);

								}
								Log("Available Soil Water at month %d year %d = %g mm\n",month, years, m->cells[cell].available_soil_water);

								Log("*****************************************************************************\n");
								Log("*****************************************************************************\n");

							}
						}
						//evergreen
						else
						{
							//vegetative period is always equal to 1 for evergreen

							Log("*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", m->cells[cell].heights[height].ages[age].species[species].name );

							Log("--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);

							Log("Month = %s\n", szMonth[month]);
							m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS] += 1;
							Log("VEG_MONTHS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);


							Get_light (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], height);
							Get_canopy_transpiration ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], vpd);

							/*Canopy evaporation of intercepted rainfall*/
							if ( met[month].rain > 0 )
							{
								Get_frac_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);
								//taking into account cell coverage
								if ( m->cells[cell].heights[height].z == m->cells[cell].heights[height].top_layer)
								{

									//Rainfall intercepted
									//heighest height class
									//interception for the highest of the the dominant class
									if (m->cells[cell].dominant_veg_counter == 1)
									{
										RainIntercepted = met[month].rain
												* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
												                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
										Log("Percentage of Rain Intercepted from dominant canopy = %g%%\n", (RainIntercepted * 100) / met[month].rain );

										m->cells[cell].evapotranspiration = ((met[month].rain
												* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC] )
												+ m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP])
												* m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										//control
										if (RainIntercepted >= met[month].rain)
										{
											RainIntercepted = met[month].rain;
											Log("Rain is completely intercepted by the highest height class in the dominant layer\n");
											lessrain = 0;
										}
										else
										{
											lessrain = met[month].rain - RainIntercepted;
										}
									}
									else
									{
										Log("Less Rain = %g mm\n", lessrain);
										if (lessrain <= 0)
										{
											Log("Rainfall is completely intercepted from the upper layer\n");
										}
										else
										{
											RainIntercepted = lessrain
													* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
													                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
										}

										m->cells[cell].evapotranspiration = (lessrain
												* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
												                                                                  + m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP])
												                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];


										lessrain -= RainIntercepted;

									}
								}
								//dominated layers
								else
								{
									Get_frac_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

									Log("Rain Interception rate for dominated layer = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC] );

									if (lessrain > 0)
									{
										RainIntercepted = lessrain
												* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
												                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
									}
									else
									{
										RainIntercepted = 0;
										Log("No Rainfall for this layer\n");
									}

									m->cells[cell].evapotranspiration = (lessrain
											* m->cells[cell].heights[height].ages[age].species[species].value[FRAC_RAIN_INTERC]
											                                                                  + m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP])
											                                                                  * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];

									lessrain -= RainIntercepted;

								}
							}
							else
							{
								Log("NO RAIN-NO INTERCEPTION\n");
								m->cells[cell].evapotranspiration =  m->cells[cell].heights[height].ages[age].species[species].value[MONTH_TRANSP];
							}

							//compute soil evaporation in the last loop of height
							if( height == 0)
							{
								Get_soil_evaporation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], m->cells[cell].net_radiation, m->cells[cell].top_layer, m->cells[cell].heights[height].z,
										m->cells[cell].net_radiation_for_dominated, m->cells[cell].net_radiation_for_subdominated, m->cells[cell].Veg_Counter, m->cells[cell].daylength);

								m->cells[cell].evapotranspiration += m->cells[cell].soil_evaporation;
								Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );
							}


							m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] += m->cells[cell].evapotranspiration;
							Log("Cumulated Evapotranspiration for this layer = %g mm\n", m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]);

							m->cells[cell].total_yearly_evapotransipration += m->cells[cell].evapotranspiration;
							Log("TOTAL Cumulated Evapotranspiration = %g mm\n",m->cells[cell].total_yearly_evapotransipration);


							/* Soil Water Balance*/
							Log("\n\n-----------------------------------------------\n");

							Get_soil_water_balance (&m->cells[cell]);

							/*reset Evapotranspiration*/
							m->cells[cell].evapotranspiration = 0;

							Get_phosynthesis_monteith (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, DaysInMonth[month], height);

							M_E_Get_Partitioning_Allocation_CTEM ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age], &m->cells[cell], met, month,
									DaysInMonth[month], years, height, age);

							//Get_litterfall_evergreen ( m->cells[cell].heights,  oldWf, m->cells[cell].heights[height].ages_count -1, m->cells[cell].heights[height].ages[age].species_count -1, years);


							Log("--------------------------------------------------------------------------\n\n\n");

							if (m->cells[cell].heights[height].z == 0)
							{
								Light_Absorb_for_establishment = (m->cells[cell].par_for_soil / m->cells[cell].par_over_dominant_canopy);
								Log("PAR OVER CANOPY = %g \n",  m->cells[cell].par_over_dominant_canopy);
								Log("PAR FOR SOIL = %g \n", m->cells[cell].par_for_soil);
								Log("Average Light Absorbed for establishment = %g \n", Light_Absorb_for_establishment);
							}
						}




						/*SHARED FUNCTIONS FOR DECIDUOUS AND EVERGREEN*/

						/*END OF YEAR*/

						if (month == DECEMBER)
						{
							Log("*****END OF YEAR******\n");


							/*FRUIT ALLOCATION*/
							//Only for dominant layer

							//Log("Dominant Canopy Cover = %g\n", m->cells[cell].canopy_cover_dominant);

							/*
                           if (m->cells[cell].heights[height].ages[age].value >= m->cells[cell].heights[height].ages[age].species[species].value[SEXAGE] && (m->cells[cell].heights[height].z == 2 || m->cells[cell].heights[height].z == 1))
                           {
						   Get_Fruit_Allocation_LPJ ( &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z, years, Yearly_Rain, m->cells[cell].canopy_cover_dominant);
						   Seeds_Number_LE = Get_Fruit_Allocation_Logistic_Equation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
						   Log("Seeds Number from Logistic Equation = %d\n", Seeds_Number_LE);

                            //Seeds_Number_T = Get_Fruit_Allocation_TREEMIG ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);
                            //Log("Seeds Number from TREEMIG = %d\n", Seeds_Number_T);

                            //FRUIT ESTABLISHMENT
                            if (Yearly_Rain > m->cells[cell].heights[height].ages[age].species[species].value[MINRAIN])
                            {
                            //decidere se pssare numero di semi da LPJ o dall'Equazione Logistica
                            m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = Get_Establishment_LPJ ( &m->cells[cell].heights[height].ages[age].species[species], Light_Absorb_for_establishment);
                            Log("Saplings Number from LPJ = %d\n", m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP]);
                            }
                            else
                            {
                            Log("Yearly Rain = %g\n", Yearly_Rain);
                            Log("Minimum Rain for Establishment = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[MINRAIN]);
                            Log("NOT ENOUGH RAIN FOR ESTABLISHMENT\n");
                            }
                            }
							 */
							//Get_litterfall ( m->cells[cell].heights,  oldWf, m->cells[cell].heights[height].ages_count -1, m->cells[cell].heights[height].ages[age].species_count -1, years);

							Log("****LITTER BIOMASS****\n");


							//inserire anche la biomassa dei semi non germogliati

							Get_litterfall (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], years);


							// Total Biomass at the end
							m->cells[cell].heights[height].ages[age].species[species].value[TOTAL_W] =  m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] +
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]  +
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]  +
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] ;
							Log("Total Biomass less Litterfall and Root Turnover = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[TOTAL_W]);


							/*WATER USE EFFICIENCY*/


							m->cells[cell].heights[height].ages[age].species[species].value[WUE] = 100 * ( m->cells[cell].heights[height].ages[age].species[species].value[YEARLY_NPP] /
									m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]) /
									(m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] /
											m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);
							//Log("Average Water use efficiency = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[WUE]);


							Get_average_biomass (&m->cells[cell].heights[height].ages[age].species[species]);

							Log("*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n");


							/*MORTALITY*/

							/*Mortality based on Self-Thinning Rule*/
							//todo CONTROLLARE E SOMMARE AD OGNI STRATO LA BIOMASSA DI QUELLA SOVRASTANTE

							/*DENSITY MORTALITY*/

							//deselected algorithm for 1Km^2 spatial resolution
							/*m->cells[cell].heights[height].ages[age].species[species].value[WS_MAX] = m->cells[cell].heights[height].ages[age].species[species].value[WSX1000] *
                                pow((1000 / (float)m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]),
                                        m->cells[cell].heights[height].ages[age].species[species].value[THINPOWER]);
							 */

							//modifified version for 1Km^2 spatial resolution
							m->cells[cell].heights[height].ages[age].species[species].value[WS_MAX] = m->cells[cell].heights[height].ages[age].species[species].value[WSX1000];



							if ( m->cells[cell].heights[height].ages[age].species[species].value[AV_STEM_MASS] > m->cells[cell].heights[height].ages[age].species[species].value[WS_MAX])
							{
								Get_Mortality (&m->cells[cell].heights[height].ages[age].species[species], years);
							}
							else
							{
								Log("NO MORTALITY based SELF-THINNING RULE\n");
								Log("Average Stem Mass < WSMax\n");
							}

							//todo
							//WHEN MORTALITY OCCURED IN MULTILAYERED FOREST MODEL SHOULD CREATE A NEW CLASS FOR THE DOMINATED LAYER THAT
							//RECEIVES MORE LIGHT AND THEN GROWTH BETTER SO IT IS A NEW HEIGHT CLASS


							/*Mortality based on tree Age(LPJ)*/
							//Get_Age_Mortality (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height].ages[age]);

							/*Mortality based on Growth efficiency(LPJ)*/
							//Get_Greff_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);

							/*LIGHT MORTALITY & GROWTH EFFICIENCY*/
							if(m->cells[cell].heights[height].z < m->cells[cell].heights[height].top_layer)
							{
								Log("MORTALITY FOR LOWER LAYER \n");
								/*Mortality based on Growth efficiency(LPJ)*/
								//Get_Greff_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);
								Log("MORTALITY FOR LOWER LAYER BASED ON LIGHT AVAILABILITY\n");
							}

							if ( m->cells[cell].heights[height].ages[age].species[species].management == C)
							{
								Get_stool_mortality (&m->cells[cell].heights[height].ages[age].species[species], years);
							}


							/*RECRUITMENT*/
							/*
                            //compute light availabilty for seeds of dominant layer
                            if (m->cells[cell].heights[height].z == 0)
                            {
                                //Log("Average Yearly Par at Soil Level for recruitment = %g molPAR/m^2 month\n", m->cells[cell].av_yearly_par_soil);
                                //convert molPAR/m^2 month into W/m^2 hour
                                //3600 = seconds in a hour
                                Log("Average Yearly Par at Soil Level for recruitment = %g W/m^2 hour\n", m->cells[cell].av_yearly_par_soil);
                                m->cells[cell].av_yearly_par_soil = (m->cells[cell].av_yearly_par_soil / ( MOLPAR_MJ * 365 * m->cells[cell].av_yearly_daylength * 3600 )) * W_MJ;
                                Log("Average Yearly Par at Soil Level for recruitment from previous year = %g W/m^2 hour\n", m->cells[cell].av_yearly_par_soil);


                                //ERRORE PÈERCHÈ IN QUESTO CASO PRENDEREBBE IL VALORE "LIGHT_TOL" DELLA SPECIE A z == 0
                                if (m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] <= 2)
                                {
                                //Shade Tollerant
                                Log("%s is Shade Tollerant\n", m->cells[cell].heights[height].ages[age].species[species].name);
                                Log("Minimum Par at Soil level for Establishment = %g W/m^2 hour\n", m->cells[cell].heights[height].ages[age].species[species].value[MINPAREST]);
                                if (m->cells[cell].av_yearly_par_soil < m->cells[cell].heights[height].ages[age].species[species].value[MINPAREST])
                                {
                                Log("NO light available for establishment of SHADE TOLLERANT TREES Under Dominant Canopy !!\n");
                                }
                                else
                                {
                                Log("Light available for establishment of SHADE TOLLERANT TREES Under Dominant Canopy !!\n");
                                }
                                }
                                else
                                {
                                //Shade Intollerant
                                Log("%s is Shade Intollerant\n", m->cells[cell].heights[height].ages[age].species[species].name);
                                Log("Minimum Par at Soil level for Establishment = %g W/m^2 hour\n", m->cells[cell].heights[height].ages[age].species[species].value[MINPAREST]);
                                if (m->cells[cell].av_yearly_par_soil < m->cells[cell].heights[height].ages[age].species[species].value[MINPAREST])
                                {
                                Log("NO light available for establishment of SHADE INTOLLERANT TREES Under Dominant Canopy !!\n");
                                }
                                else
                                {
                                Log("Light available for establishment of SHADE INTOLLERANT TREES Under Dominant Canopy !!\n");
                                }
                                }
                            }
							 */

							/*CROWDING COMPETITION-BIOMASS RE-ALLOCATION*/
							Get_crowding_competition (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], m->cells[cell].heights[height].z, years, m->cells[cell].top_layer);

							//ABG and BGB
							Get_AGB_BGB_biomass (&m->cells[cell], height, age, species);

							//DENDROMETRY
							Get_dendrometry (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], m->cells[cell].heights_count);

							//TURNOVER
							Get_turnover ( &m->cells[cell].heights[height].ages[age].species[species]);

							//ANNUAL BIOMASS INCREMENT
							Get_biomass_increment ( &m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].top_layer,  m->cells[cell].heights[height].z, height, age);

							Print_end_month_stand_data (&m->cells[cell], yos, met, month, years, height, age, species);

							Get_annual_average_values_modifiers (&m->cells[cell].heights[height].ages[age].species[species]);

							Get_EOY_cumulative_balance_layer_level (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height]);

							if ( height == 0)
							{
								Get_EOY_cumulative_balance_cell_level (&m->cells[cell], yos, years);
							}


							//MANAGEMENT
							//Choose_management (&m->cells[cell], &m->cells[cell].heights[height].ages[age].species[species], years, height);


							/*
                           if ( m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] != 0 && m->cells[cell].heights[height].z == m->cells[cell].top_layer )
                           {

                                //create new class

                                ROW r;
                                int i;

                                //SET VALUES
                                m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE_SAP] = 0;
                                m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAP] = settings->lai_sapling;
                                //m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAP] = settings->avdbh_sapling;
                                m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT_SAP] = settings->height_sapling;
                                m->cells[cell].heights[height].ages[age].species[species].value[WF_SAP] = settings->wf_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[WR_SAP] = settings->wr_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[height].ages[age].species[species].value[WS_SAP] = settings->ws_sapling * m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];


                                Log("\n\n----A new Height class must be created---------\n\n");

                                //add values for a new a height class
                                //CREATE A NEW ROW
                                r.x = m->cells[cell].x;
                                r.y = m->cells[cell].y;
                                r.age = 0 ;
                                r.species = m->cells[cell].heights[height].ages[age].species[species].name;
                                r.phenology = m->cells[cell].heights[height].ages[age].species[species].phenology;
                                r.management = m->cells[cell].heights[height].ages[age].species[species].management;
                                r.lai = m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAP];
                                r.n = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                r.avdbh = m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAP];
                                r.height = m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT_SAP];
                                r.wf = m->cells[cell].heights[height].ages[age].species[species].value[WF_SAP];
                                r.wr = m->cells[cell].heights[height].ages[age].species[species].value[WR_SAP];
                                r.ws = m->cells[cell].heights[height].ages[age].species[species].value[WS_SAP];



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
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[LAI] = m->cells[cell].heights[height].ages[age].species[species].value[LAI_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].counter[N_TREE] = m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[AVDBH] = m->cells[cell].heights[height].ages[age].species[species].value[AVDBH_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[BIOMASS_FOLIAGE_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[WF_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[BIOMASS_ROOTS_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[WR_SAP];
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[BIOMASS_STEM_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[WS_SAP];

                                //height class summary
                                Log("**********************************\n");
                                Log("x = %d\n", r.x);
                                Log("y = %d\n", r.y);
                                Log("age = %d\n", r.age);
                                Log("species = %s\n", r.species);
                                Log("phenology = %d\n", r.phenology);
                                Log("management = %d\n", r.management);
                                Log("lai = %g\n", r.lai);
                                Log("n tree = %d\n", r.n);
                                Log("avdbh = %g\n", r.avdbh);
                                Log("height = %g\n", r.height);
                                Log("wf = %g\n", r.wf);
                                Log("wr = %g\n", r.wr);
                                Log("ws = %g\n", r.ws);

                                Saplings_counter += 1;
                                Log("Sapling Classes counter = %d\n", Saplings_counter);

                                Log("*****************************\n");
                                Log("*****************************\n");
                            }
                            if (m->cells[cell].heights[height].ages[age].value == settings->adult_age)
                            {
                                Log("....A NEW HEIGHT CLASS IS PASSING IN ADULT PERIOD\n");

                                m->cells[cell].heights[height].value = 1 ;
                                Log("Height class passing from Sapling to Adult = %g m\n", m->cells[cell].heights[height].value);

                                //Saplings_counter -= 1;
                            }
							 */

						}
					}
					Log("****************END OF SPECIES CLASS***************\n");
				}
				// for young trees

				else
				{
					if( month == 11)
					{
						Log("\n/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
						Log("SAPLINGS\n");
						/*
                           Saplings_counter += 1;
                           Log("-Number of Sapling class in layer 0 = %d\n", Saplings_counter);
						 */
						Log("Age %d\n", m->cells[cell].heights[height].ages[age].value);
						//Log("Species %s\n", m->cells[cell].heights[height].ages[age].species[species].name);

						/*Saplings mortality based on light availability*/
						Light_for_establishment = m->cells[cell].par_for_soil / MOLPAR_MJ;
						Log("Radiation for soil =  %g W/m^2\n", Light_for_establishment);


						if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 1)
						{
							if ( Light_for_establishment < settings->light_estab_very_tolerant)
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
								Log("NO Light for Establishment\n");
							}
						}
						else if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 2)
						{
							if ( Light_for_establishment < settings->light_estab_tolerant)
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
								Log("NO Light for Establishment\n");
							}
						}
						else if ( m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TOL] == 3)
						{
							if ( Light_for_establishment < settings->light_estab_intermediate)
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
								Log("NO Light for Establishment\n");
							}
						}
						else
						{
							if ( Light_for_establishment < settings->light_estab_intolerant)
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;
								Log("NO Light for Establishment\n");
							}
						}
						/*
                        if (m->cells[cell].heights[height].ages[age].value == settings->adult_age)
                        {
                        Log("....A NEW HEIGHT CLASS IS PASSING IN ADULT PERIOD\n");

                        Saplings_counter -= 1;
                        }
						 */

						Log("/*/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");
					}
				}
			}
			Log("****************END OF AGE CLASS***************\n");
		}
		Log("****************END OF HEIGHT CLASS***************\n");
		//}
		//else
		//{
		//    Log("\n\n");
		//    Log("RUN FOR SOIL LAYER\n");
		//    Log("* cell = \n");
		//    Log("* x = %g\n", m->cells[cell].x);
		//    Log("* y = %g\n", m->cells[cell].y);
		//    Log("* z = %d\n", m->cells[cell].heights[height].z);
		//    Log("ASW month %d = %g mm\n", month + 1, m->cells[cell].available_soil_water);



		//N_avl = (Ka * site->sN) + pN + (Kb * Yearly_Eco_NPP);
		//Log("Nitrogen available = %g g m^-2\n", N_avl);


		//    Log("************************************ \n");
		//    Log("\n\n");
		//}

	}

	//averaged monthly met data
	if (month == DECEMBER)
	{
		Get_annual_average_values_met_data (&m->cells[cell], Yearly_Solar_Rad, Yearly_Vpd, Yearly_Temp, Yearly_Rain );
	}

	Log("****************END OF CELL***************\n");
	/* ok */
	return 1;
}
