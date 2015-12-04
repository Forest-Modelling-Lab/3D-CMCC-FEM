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


extern void Get_biome_fraction (SPECIES * s)
{
	/*using biome parameter recompute values to have fraction*/
	s->value[FINE_ROOT_LEAF_FRAC] = s->value[FINE_ROOT_LEAF] / (s->value[FINE_ROOT_LEAF]+1.0);
	Log("biome fine root leaf frac = %f\n", s->value[FINE_ROOT_LEAF_FRAC]);
	s->value[STEM_LEAF_FRAC] = s->value[STEM_LEAF] / (s->value[STEM_LEAF]+1.0);
	Log("biome stem leaf frac = %f\n", s->value[STEM_LEAF_FRAC]);
	s->value[COARSE_ROOT_STEM_FRAC] = s->value[COARSE_ROOT_STEM] / (s->value[COARSE_ROOT_STEM]+1.0);
	Log("biome coarse root stem frac = %f\n", s->value[COARSE_ROOT_STEM_FRAC]);
	s->value[LIVE_TOTAL_WOOD_FRAC] = s->value[LIVE_TOTAL_WOOD] / (s->value[LIVE_TOTAL_WOOD]+1.0);
	Log("biome live wood total wood frac = %f\n", s->value[LIVE_TOTAL_WOOD_FRAC]);

}

//----------------------------------------------------------------------------//
//                                                                            //
//                             GetDayLength                                   //
//                                                                            //
//----------------------------------------------------------------------------//

//BIOME-BGC version
//Running-Coughlan 1988, Ecological Modelling

void Get_Day_Length ( CELL * c,  int day, int month, int years, YOS  *yos)
{
	/*
	if (!day)
		Log("computing Get_Day_Length...\n");
		*/

	MET_DATA *met;
	double ampl;  //seasonal variation in Day Length from 12 h
	met = (MET_DATA*) yos[years].m;

	//compute yearday for GeDdayLength function
	if (day == 0 && month == JANUARY)
	{
		c->yearday = 0;
	}
	c->yearday +=1;

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
void GetDayLength_3PG (CELL * c, int day, int month, int years, int MonthLength ,  YOS  *yos)
{
	// gets fraction of day when sun is "up"
	double sLat, cLat, sinDec, cosH0;
	//int dayOfYear;

	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	Log("GET DAY LENGTH 3-PG\n");

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
		c->daylength_3PG = ((acos(cosH0) / Pi) * 86400);
		Log("daylength 3PG = %f hours\n", c->daylength_3PG);
	}
}





/*to compute dayleght for stopping growth*/
//from Schwalm and Ek, 2004
//but it considers a value independently from species
void Get_Abscission_DayLength ( CELL * c)
{
	c->abscission_daylength = (39132 + (pow (1.088, (site->lat + 60.753))))/(60*60);
	//Log("Abscission day length = %f hrs\n", c->abscission_daylength);
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
						if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
						{
							//todo decidere se utlizzare tday o tavg growthend o mindaylenght
							if (((met[month].tavg >= c->heights[height].ages[age].species[species].value[GROWTHSTART] && month < 6)
									|| (met[month].tavg >= c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6)) && c->north == 0)
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
						if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 ||c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
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
extern void Get_Veg_Days (CELL *const c, const YOS *const yos, int day, int month, int years, int DaysInMonth)
{

	static int height;
	static int age;
	static int species;


	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	//Log("compute vegetative days for version '%c'\n", settings->spatial);

	if (!day && !month)
		Log("GET_VEG_DAYS_for year %d\n", years);


	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				if (settings->spatial == 'u')
				{
					if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
					{

						//reset 'annual day_veg_for_litterfall_rate'
						if (day == 0 && month == JANUARY)
						{
							c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 0;
							//Log("reset DAY_VEG_FOR_LITTERFALL_RATE\n");
						}
						//the same approach must be used in "Get_daily_vegetative_period" func

						if ((met[month].d[day].thermic_sum >= c->heights[height].ages[age].species[species].value[GROWTHSTART] && month <= 6)
								|| (met[month].d[day].daylength >= c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6))
						{
							c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] += 1;
							if (c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] == 1)
							{
								Log("GDD basis = %.0f\n", settings->gdd_basis);
								Log("species %s First day of growing season day = %d month = %d\n", c->heights[height].ages[age].species[species].name, day+1, month+1);
							}
						}
					}
					else
					{
						c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 365;
					}
					//compute last year day the number of days for leaf fall
					if (day == 30 && month == DECEMBER)
					{
						c->heights[height].ages[age].species[species].counter[DAY_FRAC_FOLIAGE_REMOVE] =  (int) (c->heights[height].ages[age].species[species].value[LEAF_FALL_FRAC_GROWING]
												                                                        * c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
						Log("Day frac foliage remove = %d\n", c->heights[height].ages[age].species[species].counter[DAY_FRAC_FOLIAGE_REMOVE] );
						//add leaf fall days
						if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
						{
							c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] += (int)(c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]
																												 * c->heights[height].ages[age].species[species].value[LEAF_FALL_FRAC_GROWING]);

						}
						Log("-SPECIES %s TOTAL VEGETATIVE DAYS = %d \n", c->heights[height].ages[age].species[species].name, c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
					}
				}
				else
				{
					if (!month)
					{
						c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE] = 0;
					}
					if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
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
					if (day == DaysInMonth)
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
	Log("- x = %d\n", c->x);
	Log("- y = %d\n", c->y);
	Log("- z = %d\n", c->heights[height].z);
	Log("- Class Age = %d years \n", c->heights[height].ages[age].value);
	Log("- Species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("- DBH = %f\n", c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("- Height = %f m\n", c->heights[height].value);
	Log("- Number of trees = %d trees \n", c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("- Ws = %f \n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM]);



	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
	{
		if(settings->time == 'm')
		{
			Log("- Vegetative Months =  %d months\n", c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
		}
		else
		{
			Log("- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
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
			Log("- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
		}
	}

	if (settings->spatial == 's')
	{
		Log("- Monthly LAI from NDVI = %f \n", met[month].ndvi_lai);
	}
	else
	{
		if(settings->time == 'm')
		{
			Log("- Monthly LAI from Model= %f \n",c->heights[height].ages[age].species[species].value[LAI]);
		}
		else
		{
			Log("- Daily LAI from Model= %f \n", c->heights[height].ages[age].species[species].value[LAI]);
			Log("- Crown Diameter = %f \n",  c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);
			Log("- Canopy Cover = %f \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);
		}
	}
	Log("- ASW layer %d month %d  = %f mm\n",  c->heights[height].z, month + 1, c->available_soil_water);

	if (!month)
	{
		/*Phenology*/
		Log("- Phenology type = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
		/*Management*/
		Log("- Management type = %s\n", c->heights[height].ages[age].species[species].management ? "C" : "T");
		//Log("+ Lai = %f\n", c->heights[height].ages[age].species[species].value[LAI]);
		Log("+ AvDBH = %f cm\n",  c->heights[height].ages[age].species[species].value[AVDBH]);
		Log("+ Wf = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE]);
		Log("+ Wts = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_TOT_STEM]);
		Log("+ Ws = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM]);
		Log("+ Wbb = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_BRANCH]);
		Log("+ Wrc = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE]);
		Log("+ Wrf = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE]);
		Log("+ Wr Tot = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT]);
		Log("+ Wres = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE]);
		Log("+ Ws live = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_LIVE_WOOD]);
		Log("+ Wrc live = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
		Log("+ wbb live = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);

	}

}

extern void Print_end_month_stand_data (CELL *c, const YOS *const yos, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
{
	Log("> x = %d\n", c->x);
	Log("> y = %d\n", c->y);
	Log("> z = %d\n", c->heights[height].z);
	Log("> height = %f\n", c->heights[height].value);
	Log("> age = %d\n", c->heights[height].ages[age].value);
	Log("> species = %s\n", c->heights[height].ages[age].species[species].name);
	Log("> phenology = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
	Log("> management = %d\n", c->heights[height].ages[age].species[species].management);
	//Log("[%d] PEAK Y LAI IN THIS YEAR LAYER %d = %f\n",yos[years].year, m->cells[cell].heights[height].z,  m->cells[cell].heights[height].ages[age].species[species].value[PEAK_Y_LAI]);
	Log("[%d] layer %d n tree = %d\n", yos[years].year,  c->heights[height].z, c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("[%d] layer %d > avdbh = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("[%d] layer %d > wf = %f\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE]);
	Log("[%d] layer %d > wr coarse = %f\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE]);
	Log("[%d] layer %d > wr fine = %f\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE]);
	Log("[%d] layer %d > wr Tot = %f\n",yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE] + c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE] );
	Log("[%d] layer %d > wts = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_TOT_STEM]);
	Log("[%d] layer %d > ws = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_STEM]);
	Log("[%d] layer %d > ws_bb = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_BRANCH]);
	Log("[%d] layer %d > wres = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE]);
	Log("[%d] layer %d > wres tree = %f gC/trees\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE] * 1000000 / c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log("[%d] layer %d > ws live = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_STEM_LIVE_WOOD]);
	Log("[%d] layer %d > wrc live = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
	Log("[%d] layer %d > wbb live = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 1.2 )
	{
		Log("[%d] layer %d > wcones = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_FRUIT]);
	}

	Log("[%d] layer %d > w total = %f \n", yos[years].year, c->heights[height].z,  c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE]+
			c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE]+
			c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE]+
			c->heights[height].ages[age].species[species].value[BIOMASS_STEM]+
			c->heights[height].ages[age].species[species].value[BIOMASS_BRANCH]+
			c->heights[height].ages[age].species[species].value[BIOMASS_RESERVE]);
	Log("[%d] layer %d > Dead Trees = %d\n",yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].counter[DEL_STEMS]);
	Log("> New Saplings = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE_SAP]);
	Log("*****************************\n");
}
