/* g-function.c */
#include <stdio.h>
#include <math.h>
#include "common.h"
#include "soil_settings.h"
#include "constants.h"
#include "settings.h"
#include "meteo.h"
#include "logger.h"
#include "matrix.h"

extern logger_t* g_debug_log;
extern soil_settings_t *g_soil_settings;

int sort_by_years(const void *a, const void *b)
{
	if ( ((meteo_annual_t*)a)->year < ((meteo_annual_t*)b)->year )
	{
		return -1;
	} else if ( ((meteo_annual_t*)a)->year > ((meteo_annual_t*)b)->year )
	{
		return 1;
	} else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_heights_asc(const void * a, const void * b)
{
	if ( ((height_t *)a)->value < ((height_t *)b)->value )
	{
		return -1;
	} else if ( ((height_t *)a)->value > ((height_t *)b)->value )
	{
		return 1;
	} else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_heights_desc(const void * a, const void * b)
{
	if ( ((height_t *)a)->value < ((height_t *)b)->value )
	{
		return 1;
	} else if ( ((height_t *)a)->value > ((height_t *)b)->value )
	{
		return -1;
	} else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_dbhs_asc(const void * a, const void * b)
{
	if ( ((dbh_t *)a)->value < ((dbh_t *)b)->value )
	{
		return -1;
	} else if ( ((dbh_t *)a)->value > ((dbh_t *)b)->value )
	{
		return 1;
	} else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_dbhs_desc(const void * a, const void * b)
{
	if ( ((dbh_t *)a)->value < ((dbh_t *)b)->value )
	{
		return 1;
	} else if ( ((dbh_t *)a)->value > ((dbh_t *)b)->value )
	{
		return -1;
	} else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_ages_asc(const void * a, const void * b)
{
	if ( ((age_t *)a)->value < ((age_t *)b)->value )
	{
		return -1;
	} else if ( ((age_t *)a)->value > ((age_t *)b)->value )
	{
		return 1;
	} else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_ages_desc(const void * a, const void * b)
{
	if ( ((age_t *)a)->value < ((age_t *)b)->value )
	{
		return 1;
	} else if ( ((age_t *)a)->value > ((age_t *)b)->value )
	{
		return -1;
	} else
	{
		return 0;
	}
}


void carbon_pool_fraction(cell_t *c)
{
	int height;
	int dbh;
	int age;
	int species;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	for ( height = 0; height < c->heights_count; ++height )
	{
		h = &c->heights[height];

		for ( dbh = 0; dbh < h->dbhs_count; ++dbh )
		{
			d = &c->heights[height].dbhs[dbh];

			for ( age = 0; age < d->ages_count; ++age )
			{
				a = &c->heights[height].dbhs[dbh].ages[age];

				for ( species = 0; species < a->species_count; ++species )
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					s->value[FROOT_LEAF_FRAC]      = s->value[FINE_ROOT_LEAF]   / (s->value[FINE_ROOT_LEAF]   +1.);
					s->value[STEM_LEAF_FRAC]       = s->value[STEM_LEAF]        / (s->value[STEM_LEAF]        +1.);
					s->value[CROOT_STEM_FRAC]      = s->value[COARSE_ROOT_STEM] / (s->value[COARSE_ROOT_STEM] +1.);
					s->value[LIVE_TOTAL_WOOD_FRAC] = s->value[LIVE_TOTAL_WOOD]  / (s->value[LIVE_TOTAL_WOOD]  +1.);
				}
			}
		}
	}
}

void Veg_Days(cell_t *const c, const int day, const int month, const int year)
{
	int height;
	int dbh;
	int age;
	int species;

	meteo_t *met;
	species_t *s;

	met = c->years[year].m;

	/* compute annual number of vegetative days */

	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
		{
			for (age = c->heights[height].dbhs[dbh].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; species++)
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];

					if ( s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2 )
					{
						/* reset 'annual day_veg_for_litterfall_rate'*/
						if (day == 0 && month == JANUARY)
						{
							s->counter[DAY_VEG_FOR_LEAF_FALL] = 0;
						}

						if ((met[month].d[day].thermic_sum >= s->value[GROWTHSTART] && month <= 6)
								|| (met[month].d[day].daylength >= s->value[MINDAYLENGTH] && month >= 6))
						{
							s->counter[DAY_VEG_FOR_LEAF_FALL] += 1;
						}
					}
					else
					{
						if ( IS_LEAP_YEAR( c->years[year].year ) )
						{
							s->counter[DAY_VEG_FOR_LEAF_FALL] = 366;
						}
						else
						{
							s->counter[DAY_VEG_FOR_LEAF_FALL] = 365;
						}
					}
					/* compute last year day the number of days for leaf fall */
					if (day == 30 && month == DECEMBER)
					{
						s->counter[DAYS_LEAFFALL] =  (int) (s->value[LEAF_FALL_FRAC_GROWING] *
								s->counter[DAY_VEG_FOR_LEAF_FALL]);
						logger(g_debug_log, "Days of leaf fall = %d\n", s->counter[DAYS_LEAFFALL] );

						if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
						{
							s->counter[DAY_VEG_FOR_LEAF_FALL] += (int)(s->counter[DAY_VEG_FOR_LEAF_FALL] *
									s->value[LEAF_FALL_FRAC_GROWING]);

						}
						logger(g_debug_log, "-species %s annual vegetative days = %d \n", s->name, s->counter[DAY_VEG_FOR_LEAF_FALL]);
					}
				}
			}
		}
	}
}


/* not used */
void Abscission_DayLength (cell_t *c)
{
	/*to compute day length for stopping growth*/
	//from Schwalm and Ek, 2004
	//but it considers a value independently from species
	c->abscission_daylength = (39132 + (pow (1.088, (g_soil_settings->values[SOIL_LAT] + 60.753))))/(60*60);
	//logger(g_debug_log, "Abscission day length = %f hrs\n", c->abscission_daylength);
}

