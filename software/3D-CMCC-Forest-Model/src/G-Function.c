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


extern void Pool_fraction (SPECIES * s)
{
	/*using biome parameter for allocation recomputed values to have fraction*/
	s->value[FINE_ROOT_LEAF_FRAC] = s->value[FINE_ROOT_LEAF] / (s->value[FINE_ROOT_LEAF]+1.0);
	//Log("biome fine root leaf frac (fraction to fine root = %f; fraction to leaf = %f)\n", s->value[FINE_ROOT_LEAF_FRAC], 1.0 - s->value[FINE_ROOT_LEAF_FRAC]);
	s->value[STEM_LEAF_FRAC] = s->value[STEM_LEAF] / (s->value[STEM_LEAF]+1.0);
	//Log("biome stem leaf frac (fraction to stem = %f; fraction to leaf = %f\n", s->value[STEM_LEAF_FRAC], 1.0 - s->value[STEM_LEAF_FRAC]);
	s->value[COARSE_ROOT_STEM_FRAC] = s->value[COARSE_ROOT_STEM] / (s->value[COARSE_ROOT_STEM]+1.0);
	//Log("biome coarse root stem frac (fraction to coarse root = %f; fraction to stem = %f\n", s->value[COARSE_ROOT_STEM_FRAC], 1.0 - s->value[COARSE_ROOT_STEM_FRAC]);
	s->value[LIVE_TOTAL_WOOD_FRAC] = s->value[LIVE_TOTAL_WOOD] / (s->value[LIVE_TOTAL_WOOD]+1.0);
	//Log("biome live wood total wood frac (fraction to new live total wood = %f, fraction to total wood = %f\n", s->value[LIVE_TOTAL_WOOD_FRAC], 1.0 - s->value[LIVE_TOTAL_WOOD_FRAC]);


}

//----------------------------------------------------------------------------//
//                                                                            //
//                             GetDayLength                                   //
//                                                                            //
//----------------------------------------------------------------------------//

//BIOME-BGC version
//Running-Coughlan 1988, Ecological Modelling

void Day_Length ( CELL * c,  int day, int month, int years, YOS  *yos)
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


	met[month].d[day].daylength = ampl * (sin ((c->yearday - 79) * 0.01721)) + 12;

}



//3PG version
void DayLength_3PG (CELL * c, int day, int month, int years, int MonthLength ,  YOS  *yos)
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
void Abscission_DayLength ( CELL * c)
{
	c->abscission_daylength = (39132 + (pow (1.088, (site->lat + 60.753))))/(60*60);
	//Log("Abscission day length = %f hrs\n", c->abscission_daylength);
}

extern void Tree_period (SPECIES *s, AGE *a, CELL *c)
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


//compute annual number of vegetative days
extern void Veg_Days (CELL *const c, const YOS *const yos, int day, int month, int years, int DaysInMonth)
{

	static int height;
	static int age;
	static int species;


	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

	//Log("compute vegetative days for version '%c'\n", settings->spatial);

	if (!day && !month)
		Log("VEG_DAYS_for year %d\n", years);


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
						c->heights[height].ages[age].species[species].counter[DAY_FRAC_FINE_ROOT_REMOVE] = (int) (c->heights[height].ages[age].species[species].value[LEAF_FALL_FRAC_GROWING]
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

void Print_init_month_stand_data (CELL *c, const MET_DATA *const met, const int month, const int years, int height, int age, int species)
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
	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
	{
		Log("- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	}
	else
	{
		Log("- Vegetative Days =  %d days\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
	}
	Log("- Daily LAI from Model= %f \n", c->heights[height].ages[age].species[species].value[LAI]);
	Log("- Crown Diameter = %f \n",  c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);
	Log("- Canopy Cover = %f \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);
	Log("- ASW layer %d month %d  = %f mm\n",  c->heights[height].z, month + 1, c->asw);

	if (!month)
	{
		/*Phenology*/
		Log("- Phenology type = %f\n", c->heights[height].ages[age].species[species].value[PHENOLOGY]);
		/*Management*/
		Log("- Management type = %s\n", c->heights[height].ages[age].species[species].management ? "C" : "T");
		//Log("+ Lai = %f\n", c->heights[height].ages[age].species[species].value[LAI]);
		Log("+ AvDBH = %f cm\n",  c->heights[height].ages[age].species[species].value[AVDBH]);
		Log("--Carbon pools in tC\n");
		Log("+ Wf = %f tC/area\n", c->heights[height].ages[age].species[species].value[LEAF_C]);
		Log("+ Wts = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_STEM_C]);
		Log("+ Ws = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_C]);
		Log("+ Wbb = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_C]);
		Log("+ Wrc = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]);
		Log("+ Wrf = %f tC/area\n", c->heights[height].ages[age].species[species].value[FINE_ROOT_C]);
		Log("+ Wr Tot = %f tC/area\n", c->heights[height].ages[age].species[species].value[ROOT_C]);
		Log("+ Wres = %f tC/area\n", c->heights[height].ages[age].species[species].value[RESERVE_C]);
		Log("+ Ws live = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]);
		Log("+ Wrc live = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]);
		Log("+ wbb live = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]);
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
	Log(">n tree = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE]);
	Log(">avdbh = %f\n", c->heights[height].ages[age].species[species].value[AVDBH]);
	Log("\n--Carbon pools in tC\n");
	Log("+ Wf = %f tC/area\n", c->heights[height].ages[age].species[species].value[LEAF_C]);
	Log("+ Wts = %f tC/area\n", c->heights[height].ages[age].species[species].value[TOT_STEM_C]);
	Log("+ Ws = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_C]);
	Log("+ Wbb = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_C]);
	Log("+ Wrc = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_C]);
	Log("+ Wrf = %f tC/area\n", c->heights[height].ages[age].species[species].value[FINE_ROOT_C]);
	Log("+ Wr Tot = %f tC/area\n", c->heights[height].ages[age].species[species].value[ROOT_C]);
	Log("+ Wres = %f tC/area\n", c->heights[height].ages[age].species[species].value[RESERVE_C]);
	Log("+ Ws live = %f tC/area\n", c->heights[height].ages[age].species[species].value[STEM_LIVE_WOOD_C]);
	Log("+ Wrc live = %f tC/area\n", c->heights[height].ages[age].species[species].value[COARSE_ROOT_LIVE_WOOD_C]);
	Log("+ wbb live = %f tC/area\n", c->heights[height].ages[age].species[species].value[BRANCH_LIVE_WOOD_C]);

//	Log("\n--Carbon pools in tDM\n");
//	Log("+ Wf = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_tDM]);
//	Log("+ Wts = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_TOT_STEM_tDM]);
//	Log("+ Ws = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_tDM]);
//	Log("+ Wbb = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_BRANCH_tDM]);
//	Log("+ Wrc = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_tDM]);
//	Log("+ Wrf = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_FINE_ROOT_tDM]);
//	Log("+ Wr Tot = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT_tDM]);
//	Log("+ Wres = %f tDM/area\n", c->heights[height].ages[age].species[species].value[RESERVE_tDM]);
//	Log("+ Ws live = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_LIVE_WOOD_tDM]);
//	Log("+ Wrc live = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
//	Log("+ wbb live = %f tDM/area\n", c->heights[height].ages[age].species[species].value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
	if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 1.2 )
	{
		Log("[%d] layer %d > wcones = %f\n", yos[years].year, c->heights[height].z, c->heights[height].ages[age].species[species].value[BIOMASS_FRUIT_tDM]);
	}
	Log("> Dead Trees = %d\n",c->heights[height].ages[age].species[species].counter[DEAD_STEMS]);
	Log("> New Saplings = %d\n", c->heights[height].ages[age].species[species].counter[N_TREE_SAP]);
	Log("*****************************\n");
}
