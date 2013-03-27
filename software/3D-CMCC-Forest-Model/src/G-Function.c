//
#include <math.h>
#include "types.h"
#include "constants.h"

extern int sort_by_years(const void *a, const void *b)
{
	if ( ((YOS *)a)->year < ((YOS *)b)->year )
	{
		return -1;
	}
	else if ( ((YOS *)a)->year > ((YOS *)b)->year )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//----------------------------------------------------------------------------//
//                                                                            //
//                             GetDayLength                                   //
//                                                                            //
//----------------------------------------------------------------------------//

//BIOME-BGC version
//Running-Coughlan 1988, Ecological Modelling

void GetDayLength ( CELL * c,  int day, int month, int years, int MonthLength ,  YOS  *yos)
{
	MET_DATA *met;

	met = (MET_DATA*) yos[years].m;

	//compute yearday for GeDdayLength function
	if (day == 0 && month == JANUARY)
	{
		c->yearday = 0;
	}
	c->yearday +=1;

	float ampl;  //seasonal variation in Day Length from 12 h
	ampl = (exp (7.42 + (0.045 * site->lat))) / 3600;

	if (settings->time == 'm')
	{
		met[month].daylength = ampl * (sin ((c->yearday - 79) * 0.01721)) + 12;
	}
	else
	{
		met[month].d[day].daylength = ampl * (sin ((c->yearday - 79) * 0.01721)) + 12;
	}
}



//3PG version
//todo control
void GetDayLength_3PG (CELL * c, const MET_DATA *const met, int month, int day)
{
	// gets fraction of day when sun is "up"
	float sLat, cLat, sinDec, cosH0;
	//int dayOfYear;

	if (day == 0 && month == 0)
	{
		c->cum_dayOfyear = 0;
	}

	c->cum_dayOfyear += met[month].d[day].n_days;

	Log("dayOfYear = %d \n", met[month].d[day].n_days);
	Log("cumulative dayOfYear = %d \n", c->cum_dayOfyear);


	sLat = sin(Pi * site->lat / 180);
	cLat = cos(Pi * site->lat / 180);

	sinDec = 0.4 * sin(0.0172 * (c->cum_dayOfyear - 80));
	cosH0 = sinDec * sLat / (cLat * sqrt(1 - pow(sinDec,2)));
	if (cosH0 > 1)
	{
		Log("problem in 3PG daylength\n");
	}
	else if (cosH0 < -1)
	{
		Log("problem in 3PG daylength\n");
	}
	else
	{
		c->daylength_3PG = acos(cosH0) / Pi;
		Log("daylength 3PG = %g \n", c->daylength_3PG);
	}
}





/*to compute dayleght for stopping growth*/
//from Schwalm and Ek, 2004
//but it considers a value independently from species
void Get_Abscission_DayLength ( CELL * c)
{
	c->abscission_daylength = (39132 + (pow (1.088, (site->lat + 60.753))))/(60*60);
	Log("Abscission day length = %g hrs\n", c->abscission_daylength);

}

extern void Set_tree_period (SPECIES *s, AGE *a, CELL *c)
{
	/*Set Tree period*/
	// 0 = adult tree
	// 1 = young tree

	if ( a->value > s->value[ADULT_AGE])
	{
		s->period = 0;

		if (!c->saplings_counter)
		{
			c->saplings_counter -= 1;
		}
	}
	else
	{
		s->period = 1;
		//Log("- Class Period = Saplings\n");
	}
}




//compute annual number of vegetative months
extern void Get_Veg_Months (CELL *const c, const YOS *const yos, const int month, const int years)
{
	MET_DATA *met;

	static int height;
	static int age;
	static int species;



	met = (MET_DATA*) yos[years].m;

	Log("compute vegetative months for version '%c'\n", settings->spatial);

		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < c->heights[height].ages[age].species_count; species++)
				{
					if (settings->spatial == 'u')
					{
						if (!month)
						{
							c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 0;
						}
						if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0)
						{
							//todo decidere se utlizzare tday o tavg growthend o mindaylenght
							if ((met[month].tavg >= c->heights[height].ages[age].species[species].value[GROWTHSTART] && month < 6)
									|| (met[month].tavg >= c->heights[height].ages[age].species[species].value[GROWTHEND] && month >= 6))
							{
								c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] += 1;
								//Log("MONTHs = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
								//Log("----- Vegetative month = %d for species %s\n", month + 1, m->cells[cell].heights[height].ages[age].species[species].name );
							}
						}
						else
						{
							c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 12;
						}
						if (month == DECEMBER)
						{
							Log("Month = %d\n", month);
							Log("----- TOTAL VEGETATIVE MONTHS for species %s = %d \n\n", c->heights[height].ages[age].species[species].name, c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						}
					}
					else
					{
						if (!month)
						{
							c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 0;
						}
						if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0)
						{
							if (met[month].ndvi_lai >= 0.5)
							{
								c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] += 1;
								//Log("MONTHs = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
								//Log("----- Vegetative month = %d \n", month + 1);
							}
						}
						else
						{
							c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 12;
						}
						if (month == DECEMBER)
						{
							Log("----- TOTAL VEGETATIVE MONTHS = %d \n\n", c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						}
					}
				}
			}
		}
}


//compute annual number of vegetative days
extern void Get_Veg_Days (CELL *const c, const YOS *const yos, int day, int month, int years, int MonthLength, int DaysInMonth)
{
	MET_DATA *met;
	static int height;
	static int age;
	static int species;



	met = (MET_DATA*) yos[years].m;

	//Log("compute vegetative days for version '%c'\n", settings->spatial);

	/*yearly reset thermic sum*/
	//todo decidere se calcolare la somma termica con tavg o tday
	if(day == 0 && month == 0)
	{
		c->thermic_sum = 0;
	}
	/*compute thermic sum*/
	c->thermic_sum += met[month].d[day].tavg;

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				if (settings->spatial == 'u')
				{
					if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0)
					{
						//reset 'day_veg_for_litterfall_rate'
						if (met[month].d[day].n_days == 1 && month == JANUARY)
						{
							c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 0;
							//Log("reset DAY_VEG_FOR_LITTERFALL_RATE\n");
						}
						//todo decidere se utlizzare growthend o mindaylenght
						//lo stesso approccio deve essere usato anche in Get_daily_vegetative_period func
						if ((c->thermic_sum >= c->heights[height].ages[age].species[species].value[GROWTHSTART] && month <= 6)
								|| (met[month].d[day].daylength >= c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6))
						{
							c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] += 1;
							//Log("day %d month %d add one day to DAY_VEG_FOR_LITTERFALL_RATE %d\n", met[month].d[day].n_days, month, m->cells[cell].heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
						}
					}
					else
					{
						c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 365;
					}
					if (day == 30 && month == DECEMBER)
					{
						Log("-TOTAL VEGETATIVE DAYS = %d \n\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
					}
				}
				else
				{
					if (!month)
					{
						c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 0;
					}
					if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0)
					{
						if (met[month].d[day].ndvi_lai >= 0.5)
						{
							c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] +=1;
						}
					}
					else
					{
						c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 365;
					}
					if (day == MonthLength)
					{
						Log("-MONTH = %d TOTAL VEGETATIVE DAYS for species %s = %d \n\n", month+1, c->heights[height].ages[age].species[species].name, c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
					}
					if (day == 30 && month == DECEMBER)
					{
						Log("----- TOTAL VEGETATIVE DAYS = %d \n\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
					}
				}
			}
		}
	}
}




extern void Print_init_month_stand_data (CELL *c, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("******************************************************\n\n");
	Log("cell = \n");
	Log("- x = %g\n", c->x);
	Log("- y = %g\n", c->y);
	Log("- z = %d\n", c->heights[height].z);
	Log("- Class Age = %d years \n", c->heights[height].ages[age].value);
	Log("- Species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("- Height = %g m\n", c->heights[height].value);
	Log("- Number of trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);

	if ( c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0)
	{
		if(settings->time == 'm')
		{
			Log("- Vegetative Months =  %d months\n", c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
		}
		else
		{
			Log("- Vegetative Days =  %d months\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
		}
	}
	else
	{
		if(settings->time == 'm')
		{
			Log("- Vegetative Months =  %d months\n", c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
		}
		else
		{
			Log("- Vegetative Days =  %d months\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
		}
	}

	if (settings->spatial == 's')
	{
		Log("- Monthly LAI from NDVI = %g \n",c->heights[height].z, met[month].ndvi_lai);
	}
	else
	{
		Log("- Monthly LAI from Model= %g \n",c->heights[height].z, c->heights[height].ages[age].species[species].value[LAI]);
	}
	Log("- ASW layer %d month %d  = %g mm\n",  c->heights[height].z, month + 1, c->available_soil_water);

	if (!month)
	{
		/*Phenology*/
		Log("- Phenology type = %g\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
		/*Management*/
		Log("- Management type = %s\n", c->heights[height].ages[age].species[species].management ? "C" : "T");
		//Log("+ Lai = %g\n", c->heights[height].ages[age].species[species].value[LAI]);
		Log("+ AvDBH = %g cm\n",  c->heights[height].ages[age].species[species].value[AVDBH]);
		Log("+ Wf = %g tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);
		Log("+ Ws = %g tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]);
		Log("+ Wrc = %g tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]);
		Log("+ Wrf = %g tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]);
		Log("+ Wr Tot = %g tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_CTEM]);
		Log("+ Wres = %g tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]);
	}

}

extern void Print_end_month_stand_data (CELL *c, const YOS *const yos, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("> x = %g\n", c->x);
	Log("> y = %g\n", c->y);
	Log("> z = %d\n", c->heights[height].z);
	Log("> height = %g\n", c->heights[height].value);
	Log("> age = %d\n", c->heights[height].ages[age].value);
	Log("> species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("> phenology = %d\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
	Log("> management = %d\n", c->heights[height].ages[age].species[species].management);
	//Log("[%d] PEAK Y LAI IN THIS YEAR LAYER %d = %g\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[PEAK_Y_LAI]);
	Log("[%d] layer %d n tree = %d\n", yos[years].year,  c->heights[height].z, c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("[%d] layer %d > avdbh = %g\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("[%d] layer %d > wf = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);
	Log("[%d] layer %d > wr coarse = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]);
	Log("[%d] layer %d > wr fine = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]);
	Log("[%d] layer %d > wr Tot = %g\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM] + c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM] );
	Log("[%d] layer %d > ws = %g\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]);
	Log("[%d] layer %d > wres = %g\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM]);
	Log("[%d] layer %d > wres tree = %g gC/trees\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM] * 1000000 / c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("[%d] layer %d > Dead Trees = %d\n",yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].counter[DEL_STEMS]);
	Log("> New Saplings = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE_SAP]);
	Log("*****************************\n");
}
