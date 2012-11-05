/* treemodel.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"

extern  const char *szMonth[MONTHS];
extern int MonthLength[];
extern int DaysInMonth[];
extern int fill_cell_from_heights(CELL *const c, const ROW *const row);


/* */
int tree_model(MATRIX *const m, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	MET_DATA *met;

	static int cell;
	static int height;
	static int age;
	static int species;


	static int top_layer;
	//static int tree_soil;
	/*counter for numbers of layer*/
	static int number_of_layers;


	//static int DominantLightIndex;
	//dominanza anche se non si è l'albero piu alto:
	//caso: due strati, il piu alto è deciduo il piu basso è sempreverde.
	//il piu alto ha z = 1 il piu basso z = 0
	//durante i mesi in cui il deciduo è in dormienza (senza foglie)
	//il sempreverde diventa "dominante" per ciò che riguarda la luce
	//non sarà quindi influenzato dal deciduo

	int Veg_UnVeg;      //if class is in veg or unveg period


	static int n_counter;     // counter for soil evaporation in un-vegetated period


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

	static float MonthTransp;
	static float Interception;
	static float RainIntercepted;

	static float abscission_day_length;




	//static float Gap_Cover;
	//static float thermic_sum;
	//static float veg_index;     //counter of vegetative months
	static float MassDensity;
	static float lessrain;


	//	static float Cum_Av_Stem_Mass;

	/*fruit*/
	/*logistic equation*/
	//static int Seeds_Number_LE;
	/*treemig approach*/
	//static int Seeds_Number_T;
	static int Saplings_counter;
	static float dominant_prec_volume;
	static float dominated_prec_volume;

	/*establishment*/
	//static int Saplings_Number;

	//static int strata;

	//SOIL NITROGEN CONTENT see Peng et al., 2002
	//static float N_avl;  //Total nitrogen available for ttree growth see Peng et al., 2002
	//const float Ka = 0.6;  //see Peng et al., 2002
	//const float Kb = 0.0005; //see Peng et al., 2002

	// check paramenters
	assert(m && yos);

	// per evitare di riscrivere il tutto
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



	//*************FOREST STRUCTURE*********************

	//annual forest structure
	if (!month)
	{
		for ( cell = 0; cell < m->cells_count; cell++)
		{
			for ( height = m->cells[cell].heights_count - 1; height >= 0; height-- )
			{
				Get_annual_forest_structure (&m->cells[cell], &m->cells[cell].heights[height]);
			}
		}
	}
	//monthly forest structure
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		for ( height = m->cells[cell].heights_count - 1; height >= 0; height-- )
		{
			Get_monthly_forest_structure (&m->cells[cell], &m->cells[cell].heights[height], met, month);
		}
	}

	//*************************************************




	//monthly loop on each cell
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		//Print_parameters (&m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].ages[age].species_count, month, years);


		Log("***************************************************\n");

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


		if ( !month  && !years)
		{
			m->cells[cell].available_soil_water = site->initialAvailableSoilWater + met[month].rain;
			Log("Beginning month  %d ASW = %g mm\n", month  + 1 , m->cells[cell].available_soil_water);
		}
		else
		{
			m->cells[cell].available_soil_water +=  met[month].rain;
			Log("Beginning month  %d ASW = %g mm\n", month + 1, m->cells[cell].available_soil_water);
		}

		Log("********MAX available soil W = %g \n",site->maxAsw);
		Log("********MIN available soil W = %g \n",site->minAsw);

		//control
		if (m->cells[cell].available_soil_water > site->maxAsw)
		{
			Log("ASW > MAXASW !!!\n");
			//if the asw exceeds maxasw the plus is considered lost for turn off
			m->cells[cell].available_soil_water = site->maxAsw;
			Log("ASW month %d = %g mm\n", month + 1, m->cells[cell].available_soil_water);
		}



		//compute moist ratio
		//this function currently doesn't have effect
		m->cells[cell].soil_moist_ratio = m->cells[cell].available_soil_water / site->maxAsw;
		Log("Moist ratio outside modifier = %g\n", m->cells[cell].soil_moist_ratio);

		m->cells[cell].av_soil_moist_ratio += m->cells[cell].soil_moist_ratio;

		Log("YEAR SIMULATED = %d (%d)\n", years + 1, yos[years].year );
		Log("MONTH SIMULATED = %s\n", szMonth[month]);

		Print_met_data (met, vpd,  month,  m->cells[cell].daylength);

		//for each month of simulation the model recomputes the number of classes in veg period
		m->cells[cell].dominant_veg_counter = 0;
		m->cells[cell].dominated_veg_counter = 0;
		m->cells[cell].subdominated_veg_counter = 0;

		// sort by heights
		qsort (m->cells[cell].heights, m->cells[cell].heights_count, sizeof (HEIGHT), sort_by_heights_asc);

		//loop on each heights starting from higher to lower
		// get dominant index
		Log("*****************CELL x = %g, y = %g STRUCTURE*********************\n", m->cells[cell].x, m->cells[cell].y);
		//Get_monthly_forest_structure ();
		//Get_Layer_Cover


		Log("ASW month %d = %g mm\n", month + 1, m->cells[cell].available_soil_water);

		Get_Dominant_Light (m->cells[cell].heights, &m->cells[cell],  m->cells[cell].heights_count, met, month, DaysInMonth[month]);


		for ( height = m->cells[cell].heights_count - 1; height >= 0; height-- )
		{
			if (m->cells[cell].heights[height].dominance == 1)
			{
				top_layer = m->cells[cell].heights[height].z ;
				Log("-Top layer and in Dominant Light is layer with z = %d\n", top_layer);
				break;
			}
			if ( top_layer == -1)
			{
				Log("- %s -NO TREES IN VEGETATIVE PERIOD!!!\n", szMonth[month]);
				Log("**********************************************\n");
				break;
			}
		}


		for ( height = m->cells[cell].heights_count -1 ; height >= 0; height-- )
		{
			//Log("*RUN FOR TREE LAYERS\n");

			/*Set layer*/
			//Set_z_value (&m->cells[cell], m->cells[cell].heights[height].value , m->cells[cell].heights_count);

			//loop on each ages
			for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				/*increment age*/
				if( !month)
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
						n_counter += 1;
						//Log("n_counter =%d \n", n_counter);

						Log("******************************************************\n");

						m->cells[cell].heights[height].ages[age].species[species].value[FRACBB] = m->cells[cell].heights[height].ages[age].species[species].value[FRACBB1] +
								(m->cells[cell].heights[height].ages[age].species[species].value[FRACBB0] -
										m->cells[cell].heights[height].ages[age].species[species].value[FRACBB1]) *
										exp(-ln2 * (m->cells[cell].heights[height].ages[age].value /
												m->cells[cell].heights[height].ages[age].species[species].value[TBB]));
						//Log("fracBB = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]);
						MassDensity = m->cells[cell].heights[height].ages[age].species[species].value[RHOMAX] +
								(m->cells[cell].heights[height].ages[age].species[species].value[RHOMIN] -
										m->cells[cell].heights[height].ages[age].species[species].value[RHOMAX]) *
										exp(-ln2 * (m->cells[cell].heights[height].ages[age].value /
												m->cells[cell].heights[height].ages[age].species[species].value[TRHO]));

						//Log("Mass Density = %g\n", MassDensity);


						if (m->cells[cell].heights_count == 2)
						{
							if (!month && m->cells[cell].heights[height].z == 2)
							{
								dominant_prec_volume = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] *
										(1 - m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]) /	MassDensity;
								Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);
							}
							if (!month && m->cells[cell].heights[height].z < 2)
							{
								dominated_prec_volume = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] *
										(1 - m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]) /	MassDensity;
								Log("DominatedVolume = %g m^3/cell resolution\n", dominated_prec_volume);
							}
						}
						if (m->cells[cell].heights_count < 2)
						{
							dominant_prec_volume = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] *
									(1 - m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]) /	MassDensity;
							Log("DominantVolume = %g m^3/cell resolution\n", dominant_prec_volume);

						}
						if (m->cells[cell].heights_count > 2)
						{
							Log("FARE ROUTINE = \n");
						}
						Log("******************************************************\n\n");

						//compute number of layers
						Get_number_of_layers (&m->cells[cell]);


						Log("\n\n\n\ncell = \n");
						Log("- x = %g\n", m->cells[cell].x);
						Log("- y = %g\n", m->cells[cell].y);
						Log("- z = %d\n", m->cells[cell].heights[height].z);
						Log("- Class Age = %d years \n", m->cells[cell].heights[height].ages[age].value);
						Log("- Species = %s\n", m->cells[cell].heights[height].ages[age].species[species].name);
						Log("- Height = %g m\n", m->cells[cell].heights[height].value);
						Log("- Number of trees = %d trees \n", m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]);
						Log("- Vegetative Months %d months for year %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE], yos[years].year);
						//Log("- Monthly LAI from Model= %g \n",m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
						if (settings->version == 's')
						{
							Log("- Monthly LAI from NDVI = %g \n",m->cells[cell].heights[height].z, met[month].ndvi_lai);
						}
						else
						{
							Log("- Monthly LAI from Model= %g \n",m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
						}
						Log("- ASW layer %d month %d  = %g mm\n",  m->cells[cell].heights[height].z, month + 1, m->cells[cell].available_soil_water);




						//Log("- Dominance = %d\n", m->cells[cell].heights[height].dominance);

						if ( !month )
						{
							Reset_cumulative_variables (&m->cells[cell], m->cells[cell].heights_count);

							if (!years)
							{
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]
																														  + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM];
							}


							//reset only in unspatial version
							if (settings->version == 'u')
							{
								//reset foliage biomass for deciduous
								if ( m->cells[cell].heights[height].ages[age].species[species].phenology == 0)
								{
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM];

									//reset LAI
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] = 0;
								}
							}

							/*Phenology*/
							if ( m->cells[cell].heights[height].ages[age].species[species].phenology == 0)
							{
								Log("- Phenology = DECIDUOUS\n");
							}
							else
							{
								Log("- Phenology = EVERGREEN\n");
							}


							/*Management*/
							if ( m->cells[cell].heights[height].ages[age].species[species].management == 0)
							{
								Log("- Management type = TIMBER\n");
							}
							else
							{
								Log("- Management type = COPPICE\n");
							}


							m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT] = m->cells[cell].heights[height].value;
							//Log("Height = %g m\n", m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT]);
							m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE] = m->cells[cell].heights[height].ages[age].value;
							//Log("Age = %d years\n", m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE] );
							//Log("+ Lai = %g\n",       m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
							Log("+ AvDBH = %g cm\n",  m->cells[cell].heights[height].ages[age].species[species].value[AVDBH]);
							if (settings->version == 'u')
							{
								Log("+ Wf = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);
							}
							Log("+ Ws = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]);
							Log("+ Wrc = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]);
							Log("+ Wrf = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]);
							Log("+ Wr Tot = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM]);
							Log("+ Wres = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]);


						}

						/*modifiers*/
						Get_modifiers (&m->cells[cell].heights[height].ages[age].species[species], met, years, month, DaysInMonth[month], m->cells[cell].available_soil_water, vpd, m->cells[cell].soil_moist_ratio, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management);



						//deciduous
						if ( m->cells[cell].heights[height].ages[age].species[species].phenology == D )
						{
							//for unspatial version growth start, growth end and month drives start and end of growing season

							//PEAK LAI
							//Get_peak_lai (&m->cells[cell].heights[height].ages[age].species[species], years, month);
							if (settings->version == 'u')
							{
								//31 May 2012
								if (month == 0)
								{
									Get_peak_lai_from_pipe_model (&m->cells[cell].heights[height].ages[age].species[species], years, month);
								}


								if ((met[month].tav >= m->cells[cell].heights[height].ages[age].species[species].value[GROWTHSTART] && month < 6) || (met[month].tav >= m->cells[cell].heights[height].ages[age].species[species].value[GROWTHEND] && month >= 6))
								//to change: the vegetative period should be stopped when LAI values fall under 0
								{
									Veg_UnVeg = 1;
									Log("OK****************************************************************\n");
								}
								else
								{
									Veg_UnVeg = 0;
									Log("NO****************************************************************\n");
								}
							}
							//for spatial version start of growing season is driven by NDVI-LAI
							if ( settings->version == 's')
							{
								//compute peak lai only for biomass foliage partitioning
								if (month == 0)
								{
									Get_peak_lai_from_pipe_model (&m->cells[cell].heights[height].ages[age].species[species], years, month);
								}

								if ( met[month].ndvi_lai > 0.1)
								{
									Veg_UnVeg = 1;
								}
								else
								{
									Veg_UnVeg = 0;
								}
							}

							if (Veg_UnVeg == 1)    //vegetative period for deciduous
							{
								Log("*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", m->cells[cell].heights[height].ages[age].species[species].name );

								Log("--PHYSIOLOGICAL PROCESSES LAYER %d --\n", m->cells[cell].heights[height].z);
								//Log("Growth Start = %g °C\n", m->cells[cell].heights[height].ages[age].species[species].value[GROWTHSTART]);
								//Log("Growth End = %g °C\n", m->cells[cell].heights[height].ages[age].species[species].value[GROWTHEND]);


								/*counter for vegetative months*/
								//veg_index = 1;
								Log("Month = %s\n", szMonth[month]);
								m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS] += 1;
								Log("VEG_MONTHS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);


								Get_initial_month_lai (&m->cells[cell].heights[height].ages[age].species[species]);

								Get_light (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, m->cells[cell].heights[height].z, month,  top_layer, m->cells[cell].daylength, DaysInMonth[month]);


								MonthTransp = Get_canopy_transpiration ( &m->cells[cell].heights[height].ages[age].species[species], met, month, m->cells[cell].daylength, DaysInMonth[month], vpd, m->cells[cell].net_radiation, m->cells[cell].soil_moist_ratio);
								//compute traspiration for area
								//Log("Monthly Canopy Transpiration per area = %g mm-Kg H2o/ha^-1/month\n", MonthTransp * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] * SIZECELL);


								/////////////////////////////////////////////////////////////////////////////////////
/*
								//5 october 2012 "simplified evapotranspiration modifier" f(E), Angelo Nolè
								//alpha e beta andranno inserite come specie specifiche!!!!

								float alpha_evapo = 0.65;
								float beta_evapo = 0.95;
								float MonthTransp_Angelo;


								m->cells[cell].heights[height].ages[age].species[species].value[F_EVAPO] = 1 - exp (- alpha_evapo * pow (m->cells[cell].soil_moist_ratio, beta_evapo));
								Log("ANGELO F_EVAPO = %g \n", m->cells[cell].heights[height].ages[age].species[species].value[F_EVAPO]);


								MonthTransp_Angelo = MonthTransp * m->cells[cell].heights[height].ages[age].species[species].value[F_EVAPO];
								Log("ANGELO MonthTransp = %g \n", MonthTransp_Angelo);
*/


								//////////////////////////////////////////////////////////////////////////////////////



								//compute soil evaporation in the last loop of height
								if( height == 0)
								{
									Get_soil_evaporation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month,DaysInMonth[month], m->cells[cell].net_radiation, top_layer, m->cells[cell].heights[height].z, number_of_layers,
											m->cells[cell].net_radiation_for_dominated, m->cells[cell].net_radiation_for_subdominated, m->cells[cell].Veg_Counter, m->cells[cell].daylength);
									Log("Monthly Soil Evaporation at month %s = %g \n", szMonth[month],  m->cells[cell].soil_evaporation );
								}


								/*Canopy evaporation of intercepted rainfall*/

								//quantità di Rain intercettata e che quindi non arriva al suolo
								//interception is a rate not a quantity

								if ( met[month].rain > 0 )
								{
									Interception = Get_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

									//see also CLM model for rain interception
									/*
									Interception = 1 - exp (-0.5 * m->cells[cell].heights[height].ages[age].species[species].value[LAI])
									 */


									//taking into account cell coverage
									if ( m->cells[cell].heights[height].z == top_layer)
									{

										//Rainfall intercepted
										//heighest height class
										//interception for the highest of the the dominant class
										if (m->cells[cell].dominant_veg_counter == 1)
										{
											RainIntercepted = met[month].rain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
											Log("Percentage of Rain Intercepted from dominant canopy = %g%%\n", (RainIntercepted * 100) / met[month].rain );



											/*Evapotranspiration*/

											m->cells[cell].evapotranspiration = (met[month].rain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );
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
												RainIntercepted = lessrain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
												Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
											}

											/*Evapotranspiration*/

											m->cells[cell].evapotranspiration = (lessrain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );

											lessrain -= RainIntercepted;

										}
									}
									//dominated layers
									else
									{
										Interception = Get_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

										Log("Rain Interception rate for dominated layer = %g\n", Interception);

										if (lessrain > 0)
										{
											RainIntercepted = lessrain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
										}
										else
										{
											RainIntercepted = 0;
											Log("No Rainfall for this layer\n");
										}

										/*Evapotranspiration*/

										m->cells[cell].evapotranspiration = (lessrain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );

										lessrain -= RainIntercepted;


										//Assuming No Rain Intercpetion for Lower Layers
										//Interception = 0.01;
										//Log("No Rain Interception for Lower Layer = %g\n", Interception);
									}
								}
								else
								{
									Log("NO RAIN-NO INTERCEPTION\n");
									m->cells[cell].evapotranspiration =  MonthTransp;
									Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );
								}


								m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] += m->cells[cell].evapotranspiration;
								Log("Cumulated Evapotranspiration for this layer = %g mm\n", m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]);

								m->cells[cell].total_yearly_evapotransipration += m->cells[cell].evapotranspiration;
								Log("TOTAL Cumulated Evapotranspiration = %g mm\n",m->cells[cell].total_yearly_evapotransipration);


								/* Soil Water Balance*/
								Log("\n\n-----------------------------------------------\n");
								Log("*********MONTHLY SOIL WATER BALACE************\n");

								/*Take off Evapotraspiration*/
								m->cells[cell].available_soil_water -= (m->cells[cell].evapotranspiration + m->cells[cell].soil_evaporation);

								Log("ASW at the END of %s year %d less Evapotraspiration = %g mm\n",szMonth[month] ,years, m->cells[cell].available_soil_water);
								if ( m->cells[cell].available_soil_water < 0)
								{
									Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
									m->cells[cell].available_soil_water = site->minAsw;
									Log("ASW = %g\n", m->cells[cell].available_soil_water);
								}
								if ( m->cells[cell].available_soil_water > site->maxAsw)
								{
									Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
									m->cells[cell].available_soil_water = site->maxAsw;
									Log("Available soil water = %g\n", m->cells[cell].available_soil_water);
								}

								/*reset Evapotranspiration*/
								m->cells[cell].evapotranspiration = 0;

								Get_phosynthesis_monteith (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, DaysInMonth[month], m->cells[cell].heights[height].z, Veg_UnVeg);

								M_D_Get_Partitioning_Allocation_CTEM ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, m->cells[cell].daylength, DaysInMonth[month], years, Veg_UnVeg);

								Log("--------------------------------------------------------------------------\n\n\n");

								if (m->cells[cell].heights[height].z == 0)
								{
									Light_Absorb_for_establishment = (m->cells[cell].par_for_soil / m->cells[cell].par_over_dominant_canopy);
									Log("PAR OVER CANOPY = %g \n",  m->cells[cell].par_over_dominant_canopy);
									Log("PAR FOR SOIL = %g \n", m->cells[cell].par_for_soil);
									Log("Average Light Asorbed for establishment = %g \n", Light_Absorb_for_establishment);


								}
							}
							//unvegetative period for deciduous
							else
							{
								Veg_UnVeg = 0;

								Log("**UN-VEGETATIVE PERIOD FOR %s SPECIES in layer %d **\n", m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].z);

								//Productivity
								Get_phosynthesis_monteith (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, DaysInMonth[month], m->cells[cell].heights[height].z, Veg_UnVeg);

								M_D_Get_Partitioning_Allocation_CTEM ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, m->cells[cell].daylength, DaysInMonth[month], years, Veg_UnVeg);

								if (settings->version == 'u')
								{
									m->cells[cell].heights[height].ages[age].species[species].value[LAI] = 0;
									Log("++Lai layer %d = %g\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[LAI]);
								}
								else
								{
									Log("++Lai layer %d = %g\n", m->cells[cell].heights[height].z, met[month].ndvi_lai);
								}


								/* Soil Water Balance*/

								Log("-----------------------------------------------------------------\n");
								Log("********MONTHLY SOIL WATER BALACE in UNVEGETATIVE PERIOD*********\n");
								//None trees in veg period, soil evaporation
								//qui deve entrare una sola volta al mese
								if ( m->cells[cell].Veg_Counter == 0 && n_counter == 1)
								{
									Get_soil_evaporation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, DaysInMonth[month], m->cells[cell].net_radiation,  top_layer, m->cells[cell].heights[height].z, number_of_layers,
											m->cells[cell].net_radiation_for_dominated, m->cells[cell].net_radiation_for_subdominated, m->cells[cell].Veg_Counter, m->cells[cell].daylength);
									Log("Monthly Soil Evaporation at month %s = %g \n", szMonth[month],  m->cells[cell].soil_evaporation );

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
							//Log("Growth Start = %g °C\n", m->cells[cell].heights[height].ages[age].species[species].value[GROWTHSTART]);
							//Log("Growth End = %g °C\n", m->cells[cell].heights[height].ages[age].species[species].value[GROWTHEND]);


							/*counter for vegetative months*/
							//veg_index = 1;
							Log("Month = %s\n", szMonth[month]);
							m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS] += 1;
							Log("VEG_MONTHS = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);


							Get_light (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, m->cells[cell].heights[height].z, month,  top_layer, m->cells[cell].daylength, DaysInMonth[month]);


							MonthTransp = Get_canopy_transpiration ( &m->cells[cell].heights[height].ages[age].species[species], met, month, m->cells[cell].daylength, DaysInMonth[month], vpd, m->cells[cell].net_radiation, m->cells[cell].soil_moist_ratio);
							//compute traspiration for area
							//Log("Monthly Canopy Transpiration per area = %g mm-Kg H2o/ha^-1/month\n", MonthTransp * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] * SIZECELL);


							/////////////////////////////////////////////////////////////////////////////////////
/*
							//5 october 2012 "simplified evapotranspiration modifier" f(E), Angelo Nolè
							//alpha e beta andranno inserite come specie specifiche!!!!

							float alpha_evapo = 0.65;
							float beta_evapo = 0.95;
							float MonthTransp_Angelo;


							m->cells[cell].heights[height].ages[age].species[species].value[F_EVAPO] = 1 - exp (- alpha_evapo * pow (m->cells[cell].soil_moist_ratio, beta_evapo));
							Log("ANGELO F_EVAPO = %g \n", m->cells[cell].heights[height].ages[age].species[species].value[F_EVAPO]);


							MonthTransp_Angelo = MonthTransp * m->cells[cell].heights[height].ages[age].species[species].value[F_EVAPO];
							Log("ANGELO MonthTransp = %g \n", MonthTransp_Angelo);

*/

							//////////////////////////////////////////////////////////////////////////////////////



							//compute soil evaporation in the last loop of height
							if( height == 0)
							{
								Get_soil_evaporation ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month,DaysInMonth[month], m->cells[cell].net_radiation, top_layer, m->cells[cell].heights[height].z, number_of_layers,
										m->cells[cell].net_radiation_for_dominated, m->cells[cell].net_radiation_for_subdominated, m->cells[cell].Veg_Counter, m->cells[cell].daylength);
								Log("Monthly Soil Evaporation at month %s = %g \n", szMonth[month],  m->cells[cell].soil_evaporation );
							}


							/*Canopy evaporation of intercepted rainfall*/

							//quantità di Rain intercettata e che quindi non arriva al suolo
							//interception is a rate not a quantity

							if ( met[month].rain > 0 )
							{
								Interception = Get_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

								//see also CLM model for rain interception
								/*
									Interception = 1 - exp (-0.5 * m->cells[cell].heights[height].ages[age].species[species].value[LAI])
								 */


								//taking into account cell coverage
								if ( m->cells[cell].heights[height].z == top_layer)
								{

									//Rainfall intercepted
									//heighest height class
									//interception for the highest of the the dominant class
									if (m->cells[cell].dominant_veg_counter == 1)
									{
										RainIntercepted = met[month].rain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
										Log("Percentage of Rain Intercepted from dominant canopy = %g%%\n", (RainIntercepted * 100) / met[month].rain );



										/*Evapotranspiration*/

										m->cells[cell].evapotranspiration = ((met[month].rain * Interception) + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );
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
											RainIntercepted = lessrain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
											Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
										}

										/*Evapotranspiration*/

										m->cells[cell].evapotranspiration = (lessrain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );

										lessrain -= RainIntercepted;

									}
								}
								//dominated layers
								else
								{
									Interception = Get_canopy_interception ( &m->cells[cell].heights[height].ages[age].species[species], met, month);

									Log("Rain Interception rate for dominated layer = %g\n", Interception);

									if (lessrain > 0)
									{
										RainIntercepted = lessrain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
										Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
									}
									else
									{
										RainIntercepted = 0;
										Log("No Rainfall for this layer\n");
									}

									/*Evapotranspiration*/

									m->cells[cell].evapotranspiration = (lessrain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
									Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );

									lessrain -= RainIntercepted;


									//Assuming No Rain Intercpetion for Lower Layers
									//Interception = 0.01;
									//Log("No Rain Interception for Lower Layer = %g\n", Interception);
								}
							}
							else
							{
								Log("NO RAIN-NO INTERCEPTION\n");
								m->cells[cell].evapotranspiration =  MonthTransp;
								Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].heights[height].z, szMonth[month], m->cells[cell].evapotranspiration );
							}


							m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] += m->cells[cell].evapotranspiration;
							Log("Cumulated Evapotranspiration for this layer = %g mm\n", m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]);

							m->cells[cell].total_yearly_evapotransipration += m->cells[cell].evapotranspiration;
							Log("TOTAL Cumulated Evapotranspiration = %g mm\n",m->cells[cell].total_yearly_evapotransipration);


							/* Soil Water Balance*/
							Log("\n\n-----------------------------------------------\n");
							Log("*********MONTHLY SOIL WATER BALACE************\n");

							/*Take off Evapotraspiration*/
							m->cells[cell].available_soil_water -= (m->cells[cell].evapotranspiration + m->cells[cell].soil_evaporation);

							Log("ASW at the END of %s year %d less Evapotraspiration = %g mm\n",szMonth[month] ,years, m->cells[cell].available_soil_water);
							if ( m->cells[cell].available_soil_water < 0)
							{
								Log("ATTENTION Available Soil Water is low than MinASW!!! \n");
								m->cells[cell].available_soil_water = site->minAsw;
								Log("ASW = %g\n", m->cells[cell].available_soil_water);
							}
							if ( m->cells[cell].available_soil_water > site->maxAsw)
							{
								Log("ATTENTION Available Soil Water exceeds MAXASW!! \n");
								m->cells[cell].available_soil_water = site->maxAsw;
								Log("Available soil water = %g\n", m->cells[cell].available_soil_water);
							}

							/*reset Evapotranspiration*/
							m->cells[cell].evapotranspiration = 0;

							Get_phosynthesis_monteith (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], month, DaysInMonth[month], m->cells[cell].heights[height].z, Veg_UnVeg);

							M_E_Get_Partitioning_Allocation_CTEM ( &m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell], met, month, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].management, m->cells[cell].daylength, DaysInMonth[month], years, Veg_UnVeg);



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
							Log("Vegetative months for %s = %d \n",m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);
							Log("**CARBON BALANCE**\n");



							Log("**WATER BALANCE**\n");
							Log("TOTAL Cumulated Evapotranspiration = %g mm H2o/m^2/year\n",m->cells[cell].total_yearly_evapotransipration);
							Log("Available Soil Water month %d year %d = %g mm H2o/m^2\n",month, years, m->cells[cell].available_soil_water);


							/*
                            if(!years)
                            {
                                if ( m->cells[cell].heights[height].z != top_layer)
                                {
                                    //reset
                                    oldWf = 0;
                                }
                                oldWf = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM];
                            }
                            else
                            {
                                oldWf = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM];
                            }

							 */

							/*FRUIT ALLOCATION*/
							//Only for dominant layer

							//Log("Dominant Canopy Cover = %g\n", m->cells[cell].canopy_cover_dominant);

							/*
                           if (m->cells[cell].heights[height].ages[age].value >= m->cells[cell].heights[height].ages[age].species[species].value[SEXAGE] && (m->cells[cell].heights[height].z == 2 || m->cells[cell].heights[height].z == 1))
                           {
                           Get_Fruit_Allocation_LPJ ( &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z, years, Yearly_Rain, m->cells[cell].canopy_cover_dominant);
                           Seeds_Number_LE = Get_Fruit_Allocation_Logistic_Equation ( &m->cells[cell].heights[height].ages[age].species[species]);
                           Log("Seeds Number from Logistic Equation = %d\n", Seeds_Number_LE);

                            //Seeds_Number_T = Get_Fruit_Allocation_TREEMIG ( &m->cells[cell].heights[height].ages[age].species[species]);
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
							/*
                            if (!years)
                            {
                                m->cells[cell].litter += m->cells[cell].heights[height].ages[age].species[species].value[DEL_LITTER] + site->initialLitter;
                                Log("Total Litter = %g tDM/ha\n", m->cells[cell].litter);
                            }
                            else
                            {
                                m->cells[cell].litter += m->cells[cell].heights[height].ages[age].species[species].value[DEL_LITTER];
                                Log("Total Litter = %g tDM/ha\n", m->cells[cell].litter);
                            }

                            //reset
                            m->cells[cell].heights[height].ages[age].species[species].value[DEL_LITTER] = 0;
							 */




							// Total Biomass at the end
							m->cells[cell].heights[height].ages[age].species[species].value[TOTAL_W] =  m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] +
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]  +
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]  +
									m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] ;
							Log("Total Biomass less Litterfall and Root Turnover = %g tDM/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[TOTAL_W]);



							/*WATER USE EFFICIENCY*/

							/*
                            m->cells[cell].heights[height].ages[age].species[species].value[WUE] = 100 * ( m->cells[cell].heights[height].ages[age].species[species].value[YEARLY_NPP] /
                                                                                                    m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]) /
                                                                                                    (m->cells[cell].heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION] /
                                                                                                    m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS]);
                            //Log("Average Water use efficiency = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[WUE]);
							 */

							/*AVERAGE STEM MASS*/

							m->cells[cell].heights[height].ages[age].species[species].value[AV_STEM_MASS] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] *
									1000 / m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE];
							/*
                            m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOT_CTEM] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]
                                                                                                            + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM];
							 */

							m->cells[cell].heights[height].ages[age].species[species].value[AV_ROOT_MASS] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM] *
									1000 / m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE];

							Log("Average Stem Mass = %g kgDM stem /tree\n", m->cells[cell].heights[height].ages[age].species[species].value[AV_STEM_MASS]);

							Log("*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n");


							/*MORTALITY*/

							/*Mortality based on Self-Thinning Rule*/
							//TO DO CONTROLLARE E SOMMARE AD OGNI STRATO LA BIOMASSA DI QUELLA SOVRASTANTE

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

							//TO DO
							//WHEN MORTALITY OCCURED IN MULTILAYERED FOREST MODEL SHOULD CREATE A NEW CLASS FOR THE DOMINATED LAYER THAT
							//RECEIVES MORE LIGHT AND THEN GROWTH BETTER SO IT IS A NEW HEIGHT CLASS


							/*Mortality based on tree Age(LPJ)*/
							//Get_Age_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);

							/*Mortality based on Growth efficiency(LPJ)*/
							//Get_Greff_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);

							/*LIGHT MORTALITY & GROWTH EFFICIENCY*/
							if(m->cells[cell].heights[height].z < top_layer)
							{
								Log("MORTALITY FOR LOWER LAYER \n");
								/*Mortality based on Growth efficiency(LPJ)*/
								//Get_Greff_Mortality (&m->cells[cell].heights[height].ages[age].species[species]);
								Log("MORTALITY FOR LOWER LAYER BASED ON LIGHT AVAILABILITY\n");
							}

							if ( m->cells[cell].heights[height].ages[age].species[species].management == 1)
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





							/*Branch and Bark Ratio*/
							m->cells[cell].heights[height].ages[age].species[species].value[FRACBB] = m->cells[cell].heights[height].ages[age].species[species].value[FRACBB1] +
									(m->cells[cell].heights[height].ages[age].species[species].value[FRACBB0] -
											m->cells[cell].heights[height].ages[age].species[species].value[FRACBB1]) *
											exp(-ln2 * (m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE] /
													m->cells[cell].heights[height].ages[age].species[species].value[TBB]));
							Log("End fracBB = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]);

							/*MASS-DENSITY*/
							MassDensity = m->cells[cell].heights[height].ages[age].species[species].value[RHOMAX] +
									(m->cells[cell].heights[height].ages[age].species[species].value[RHOMIN] -
											m->cells[cell].heights[height].ages[age].species[species].value[RHOMAX]) *
											exp(-ln2 * (m->cells[cell].heights[height].ages[age].species[species].counter[TREE_AGE] /
													m->cells[cell].heights[height].ages[age].species[species].value[TRHO]));
							//Log("Mass Density (Rho) = %g\n",MassDensity);

							/*DBH + Tree Height*/

							/*CROWDING COMPETITION-BIOMASS RE-ALLOCATION*/

							// per determinare l'incremento di Height e DBH in funzione della densità di popolazione
							//alta densità maggior altezza
							//bassa densità maggior dbh
							// in the first year avdbh and height are from input data
							Get_crowding_competition (&m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z, years, top_layer);

							//ABG and BGB

							Log("**AGB & BGB**\n");
							Log("-for Class\n");
							m->cells[cell].heights[height].ages[age].species[species].value[CLASS_AGB] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM];
							Log("Yearly Class AGB = %g tDM/ha year\n", m->cells[cell].heights[height].ages[age].species[species].value[CLASS_AGB]);
							m->cells[cell].heights[height].ages[age].species[species].value[CLASS_AGB] = 0;
							m->cells[cell].heights[height].ages[age].species[species].value[CLASS_BGB] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM];
							Log("Yearly Class BGB = %g tDM/ha year\n", m->cells[cell].heights[height].ages[age].species[species].value[CLASS_BGB]);
							m->cells[cell].heights[height].ages[age].species[species].value[CLASS_BGB] = 0;
							Log("-for Stand\n");
							m->cells[cell].stand_agb += m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM];
							Log("Yearly Stand AGB = %g tDM/ha year\n", m->cells[cell].stand_agb);
							m->cells[cell].stand_bgb += m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM];
							Log("Yearly Stand BGB = %g tDM/ha year\n", m->cells[cell].stand_bgb);


							//DENDROMETRY
							Get_dendrometry (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height], m->cells[cell].heights_count);

							//TURNOVER
							Get_turnover ( &m->cells[cell].heights[height].ages[age].species[species]);

							/*height class summary*/
							Log("\n**height class summary **\n");


							Log("> x = %g\n", m->cells[cell].x);
							Log("> y = %g\n", m->cells[cell].y);
							Log("> z = %d\n", m->cells[cell].heights[height].z);
							Log("> height = %g\n", m->cells[cell].heights[height].value);
							Log("> age = %d\n", m->cells[cell].heights[height].ages[age].value);
							Log("> species = %s\n", m->cells[cell].heights[height].ages[age].species[species].name);
							Log("> phenology = %d\n", m->cells[cell].heights[height].ages[age].species[species].phenology);
							Log("> management = %d\n", m->cells[cell].heights[height].ages[age].species[species].management);
							//Log("[%d] PEAK Y LAI IN THIS YEAR LAYER %d = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[PEAK_Y_LAI]);
							Log("[%d] layer %d n tree = %d\n", yos[years].year,  m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]);
							Log("[%d] layer %d > avdbh = %g\n", yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[AVDBH]);
							Log("[%d] layer %d > height = %g\n", yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT]);
							Log("[%d] layer %d > wf = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);
							Log("[%d] layer %d > wr coarse = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]);
							Log("[%d] layer %d > wr fine = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]);
							Log("[%d] layer %d > wr Tot = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]
							                                                                                                                                                        + m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] );
							Log("[%d] layer %d > ws = %g\n", yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]);
							Log("[%d] layer %d > wres = %g tDM/ha\n", yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]);
							Log("[%d] layer %d > wres tree = %g gC/trees\n", yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM] * 1000000 / m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]);
							Log("[%d] layer %d > Dead Trees = %d\n",yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].counter[DEL_STEMS]);
							Log("> New Saplings = %d\n", m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP]);
							Log("*****************************\n");




							/*STAND VOLUME-(STEM VOLUME)*/
							m->cells[cell].heights[height].ages[age].species[species].value[STAND_VOLUME] = m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM] *
									(1 - m->cells[cell].heights[height].ages[age].species[species].value[FRACBB]) /
									MassDensity;
							Log("Volume for this layer = %g m^3/ha\n", m->cells[cell].heights[height].ages[age].species[species].value[STAND_VOLUME]);

							/*SINGLE TREE STEM VOLUME*/
							/*
                               m->cells[cell].heights[height].ages[age].species[species].value[TREE_VOLUME] = m->cells[cell].heights[height].ages[age].species[species].value[STAND_VOLUME] /
                               m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE];
                               Log("Indivual Stand Volume = %g m^3/tree\n", m->cells[cell].heights[height].ages[age].species[species].value[TREE_VOLUME]);
							 */



							Get_biomass_increment ( &m->cells[cell].heights[height].ages[age].species[species], m->cells[cell].heights[height].z,   m->cells[cell].heights_count, dominant_prec_volume,  dominated_prec_volume );




							//Log("***Modifiers  and met data control for layer = %d ****\n", m->cells[cell].heights[height].z );

							//VPD
							m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_VPD] /= m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS];
							//Log ("[%d]layer %d average  f_VPD = %g \n",yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_VPD] );
							m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_VPD] = 0;

							//TEMPERATURE
							m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_T] /= m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS];
							//Log ("[%d]layer %d average  f_T = %g \n",yos[years].year, m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_T] );
							m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_T] = 0;

							//AGE
							//Log ("[%d]layer %d average  f_AGE = %g \n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[F_AGE] );

							//SOIL WATER
							m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_SW] /= m->cells[cell].heights[height].ages[age].species[species].counter[VEG_MONTHS];
							//Log ("[%d]layer %d average  f_SW = %g \n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_SW] );
							m->cells[cell].heights[height].ages[age].species[species].value[AVERAGE_F_SW] = 0;



							Log("\n\n\n\n\n---------------------------------------------------\n");



							//CUMULATIVE BALANCE FOR ENTIRE LAYER
							Log("**CUMULATIVE BALANCE for layer %d ** \n", m->cells[cell].heights[height].z);
							Log("END of Year Yearly Cumulated GPP for layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[YEARLY_POINT_GPP_G_C]);
							Log("END of Year Yearly Cumulated NPP for layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[YEARLY_NPP]);
							Log("END of Year Yearly Cumulated DEL STEM layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WS]);
							//Log("END of Year Yearly Cumulated DEL FOLIAGE layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WF]);
							Log("END of Year Yearly Cumulated DEL FINE ROOT layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WFR]);
							Log("END of Year Yearly Cumulated DEL COARSE ROOT layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WCR]);
							Log("END of Year Yearly Cumulated DEL RESERVE layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WRES]);
							Log("END of Year Yearly Cumulated DEL RESERVE layer %d  = %g KgC tree year\n", m->cells[cell].heights[height].z, (m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WRES]*2000)/m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE]);
							Log("END of Year Yearly Cumulated DEL BB layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_BB]);
							Log("END of Year Yearly Cumulated DEL TOT ROOT layer %d  = %g tDM/ha year\n", m->cells[cell].heights[height].z, m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WR]);







							if (m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WS] +
									m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WR] +
									m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WF] +
									m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WRES] +
									m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_BB] !=
											m->cells[cell].heights[height].ages[age].species[species].value[YEARLY_NPP])
							{
								Log("ATTENTION SUM OF ALL INCREMENTS DIFFERENT FROM YEARLY NPP \n");
								Log("DEL SUM = %g \n", m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WS] +
										m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WCR] +
										m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WFR] +
										m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_BB] +
										m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WF] +
										m->cells[cell].heights[height].ages[age].species[species].value[DEL_Y_WRES]);
							}



							//CUMULATIVE BALANCE FOR ENTIRE STAND
							if ( height == 0)
							{


								Log("\n\n\n\n\n\n\n**********END OF YEARLY STAND RUN CARBON-SOIL WATER BALANCE (%d) ************\n", yos[years].year);

								//CUMULATIVE BALANCE FOR ENTIRE CELL
								Log("**CUMULATIVE BALANCE for cell (%g, %g) ** \n", m->cells[cell].x, m->cells[cell].y);

								//CARBON BALANCE
								Log("**CARBON BALANCE**\n");
								Log("END of Year Yearly Cumulated GPP for cell (%g, %g) = %g \n", m->cells[cell].x, m->cells[cell].y, m->cells[cell].gpp);
								Log("END of Year Yearly Cumulated NPP for cell (%g, %g) = %g \n", m->cells[cell].x, m->cells[cell].y, m->cells[cell].npp);

								if ((years + 1) == years_of_simulation)
								{
									m->cells[cell].av_gpp /= years_of_simulation;
									//Log("END OF RUN Average Ecosystem GPP = %g gC/m^2 year\n",m->cells[cell].av_gpp);
									m->cells[cell].av_npp /= years_of_simulation;
									//Log("END OF RUN Average Ecosystem NPP = %g tDM/ha year\n",m->cells[cell].av_npp);
								}

								//WATER BALANCE
								Log("**WATER BALANCE**\n");
								Log("[%d] EOY TOTAL Cumulated Evapotranspiration = %g mm H2o/m^2/year\n",yos[years].year, m->cells[cell].total_yearly_evapotransipration);
								Log("[%d] EOY Available Soil Water = %g mm H2o/m^2\n",yos[years].year,  m->cells[cell].available_soil_water);

								/*
                                Total_Run_Evapotranspiration +=  (m->cells[cell].total_yearly_evapotransipration + m->cells[cell].total_yearly_soil_evaporation );
                                Log("-TOTAL EVAPOTRASPIRATION  = %g mm H2o\n", m->cells[cell].total_yearly_evapotransipration);
                                Log("-TOTAL SOIL EVAPORATION = %g mm H2o\n", m->cells[cell].total_yearly_soil_evaporation);
                                Log("-TOTAL EVAPORATION = %g mm H2o\n", m->cells[cell].total_yearly_evapotransipration + m->cells[cell].total_yearly_soil_evaporation);
                                Log("-TOTAL RAIN = %g\n", Yearly_Rain);
                                Log("-AVAILABLE SOIL WATER = %g mm\n", Yearly_Rain - m->cells[cell].total_yearly_evapotransipration - m->cells[cell].total_yearly_soil_evaporation);
                                Log("-Available soil water = %g\n", m->cells[cell].available_soil_water);
								 */

								/*
                               if ( years == YEARSOFSIMULATION)
                               {
                                   Log("**********END OF RUN SOIL WATER BALANCE************\n");

                                   Log("-TOTAL EVAPORATION + EVAPOTRASPIRATION IN RUN = %g mm H2o\n", Total_Run_Evapotranspiration);
                                   Log("-TOTAL RAIN IN RUN = %g mm H2o\n", Total_Rain);
                                   Log("-Water Balance = %g mm ", Total_Rain - Total_Run_Evapotranspiration);
                               }
								 */

							}


							//Rev 9 May 2012

							/*
                            if (years == 0)
                            {
                                int Manag;
                                printf("END OF FIRST YEAR RUN \n");
                                //printf("INSERT VALUE FOR MANAGEMENT (T = timber; C = Coppice): ");
                                //scanf ("%c",&Manag);
                                //Log("Management routine choiced = %c \n", Manag);


                                //Management
                                if ( m->cells[cell].heights[height].ages[age].species[species].management == 0)
                                {
                                    Log("- Management type = TIMBER\n");
                                    printf("SELECT TYPE OF MANAGEMENT: \n"
                                           "-CLEARCUT = 1 \n"
                                           "-........ = 2 \n"
                                           "-........ = 3 \n"
                                           "-........ = 4 \n"
                                           "-........ = 5 \n");

                                    scanf ("%d",&Manag);

                                    switch (Manag)
                                    {
                                        case 1 :
                                        Log("Case CLEARCUT choiced \n");

                                        //call function
                                        Clearcut_Timber (&m->cells[cell].heights[height].ages[age].species[species],  years, m->cells[cell].heights[height].z, number_of_layers);

                                        break;

                                        case 2 :
                                        Log("Case ....... choiced \n");

                                        //call function

                                        break;

                                        case 3 :
                                        Log("Case .......  choiced \n");

                                        //call function

                                        break;

                                        case 4 :
                                        Log("Case .......  choiced \n");

                                        //call function

                                        break;

                                    }

                                }
                                else
                                {
                                    Log("- Management type = COPPICE\n");
                                    printf("SELECT TYPE OF MANAGEMENT: \n"
                                           "-CLEARCUT = 1 \n"
                                           "-........ = 2 \n"
                                           "-........ = 3 \n"
                                           "-........ = 4 \n"
                                           "-........ = 5 \n");

                                    scanf ("%d",&Manag);

                                    switch (Manag)
                                    {
                                        case 1 :
                                        Log("Case CLEARCUT choiced \n");

                                        //call function
                                        Clearcut_Coppice (&m->cells[cell].heights[height].ages[age].species[species],  years, m->cells[cell].heights[height].z, number_of_layers);

                                        break;

                                        case 2 :
                                        Log("Case ....... choiced \n");

                                        //call function

                                        break;

                                        case 3 :
                                        Log("Case .......  choiced \n");

                                        //call function

                                        break;

                                        case 4 :
                                        Log("Case .......  choiced \n");

                                        //call function

                                        break;
                                    }

                                    }

                                }
							 */


							/*
                           if ( m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE_SAP] != 0 && m->cells[cell].heights[height].z == top_layer )
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
                                m->cells[cell].heights[m->cells[cell].heights_count-1].ages[0].species[0].value[TREE_HEIGHT] = m->cells[cell].heights[height].ages[age].species[species].value[TREE_HEIGHT_SAP];
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
						/*END OF DECEMBER*/

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


		//Log("--AVERAGE YEARLY MET DATA--\n");

		//SOLAR RAD
		Yearly_Solar_Rad /= 12;
		//Log ("[%d]> average Solar Rad = %g MJ m^2 month\n",yos[years].year, Yearly_Solar_Rad );
		Yearly_Solar_Rad = 0;


		//VPD
		Yearly_Vpd /= 12;
		//Log ("[%d]> average Vpd = %g mbar\n",yos[years].year, Yearly_Vpd );
		Yearly_Vpd = 0;


		//TEMPERATURE
		Yearly_Temp /= 12;
		//Log ("[%d]> average Temperature = %g C° month\n",yos[years].year, Yearly_Temp );
		Yearly_Temp = 0;

		//RAIN
		//Log("[%d]> yearly Rain = %g mm year\n",yos[years].year, Yearly_Rain);
		Yearly_Rain = 0;

		//MOIST RATIO
		m->cells[cell].av_soil_moist_ratio /= 12;
		//Log("[%d]> average Moist Ratio = %g year\n",yos[years].year, m->cells[cell].av_soil_moist_ratio);



		//Total_Rain += met[month].rain;

		//Log ("average Yearly Rain = %g MJ m^2 month\n",  );


	}


	n_counter = 0;
	Log("****************END OF CELL***************\n");
	/* ok */
	return 1;
}
