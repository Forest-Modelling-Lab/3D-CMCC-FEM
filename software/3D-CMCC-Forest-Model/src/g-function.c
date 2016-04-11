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
/* todo : implement a better comparison for equality */
int sort_by_heights_asc(const void * a, const void * b)
{
	if ( ((HEIGHT *)a)->value < ((HEIGHT *)b)->value )
	{
		return -1;
	}
	else if ( ((HEIGHT *)a)->value > ((HEIGHT *)b)->value )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_heights_desc(const void * a, const void * b)
{
	if ( ((HEIGHT *)a)->value < ((HEIGHT *)b)->value )
	{
		return 1;
	}
	else if ( ((HEIGHT *)a)->value > ((HEIGHT *)b)->value )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void Pool_fraction (SPECIES * s)
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
void Veg_Days (CELL *const c, const YOS *const yos, int day, int month, int years)
{

	static int height;
	static int age;
	static int species;


	MET_DATA *met;
	met = (MET_DATA*) yos[years].m;

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
								Log("GDD basis = %d\n", GDD_BASIS);
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
					if (day == 30 && month == DECEMBER)
					{
						Log("----- TOTAL VEGETATIVE DAYS = %d \n\n", c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
					}
				}
			}
		}
	}
}


