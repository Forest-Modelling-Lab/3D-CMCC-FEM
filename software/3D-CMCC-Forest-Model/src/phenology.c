/*
F * phenology.c
 *
 *  Created on: 21/mag/2013
 *      Author: alessio
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix.h"
#include "settings.h"
#include "soil_settings.h"
#include "phenology.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;
//extern soil_settings_t *g_soil_settings;

void prephenology (cell_t *const c, const meteo_daily_t *const meteo_daily, const int day, const int month, const int year)
{
	int height;
	int dbh;
	int age;
	int species;
        int mdl=0;   // ddalmo test
        
	species_t *s;

        
	/* it computes the vegetative state for each species class,
	 * the number of days of leaf fall and
	 * the rate for leaves reduction (for deciduous species) */

	/*VEG_UNVEG = 1 for veg period, = 0 for Un-Veg period*/


	logger(g_debug_log, "*prephenology*\n");
	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( dbh = 0; dbh < c->heights[height].dbhs_count; ++dbh)
		{
			for (age = c->heights[height].dbhs[dbh].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; species++)
				{
					s = &c->heights[height].dbhs[dbh].ages[age].species[species];
					
					mdl = s->value[MINDAYLENGTH] ;
					// for test purpose: to prescribe the beginning of the senescence phase
					 // if ( c->years[year].year == 2016 ) 
					// { 
					//  mdl = 10.4 ;
					//  }  

					/** FOR DECIDUOUS **/
					if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
					{
						/* compute days for leaf fall based on the annual number of veg days */
						s->counter[DAYS_LEAFFALL] = (int)(s->value[LEAF_FALL_FRAC_GROWING] * s->counter[DAY_VEG_FOR_LEAF_FALL]);
						logger(g_debug_log, "-days of leaf fall for %s = %d day\n", c->heights[height].dbhs[dbh].ages[age].species[species].name, s->counter[DAYS_LEAFFALL]);

						//note: currently model can simulate only forests in boreal hemisphere
						if ((meteo_daily->thermic_sum >= s->value[GROWTHSTART] && month <= 6) ||
								(meteo_daily->daylength >= mdl  && month >= 6 && c->north == 0))
						{
							s->counter[VEG_UNVEG] = 1;

							/* set first veg days */
							if ( ! s->counter[FIRST_VEG_DAYS] )
							{
								s->counter[FIRST_VEG_DAYS] = c->doy;
							}

							logger(g_debug_log, "-%s is in veg period\n", s->name);
						}
						else
						{
				
					
							if (meteo_daily->daylength <= mdl  && month >= 6 && c->north == 0 )
							{
								s->counter[LEAF_FALL_COUNTER] += 1;

								if(s->counter[LEAF_FALL_COUNTER] <= (int)s->counter[DAYS_LEAFFALL])
								{
									/*days of leaf fall*/
									s->counter[VEG_UNVEG] = 1;
								}
								else
								{
									/*outside days of leaf fall*/
									s->counter[VEG_UNVEG] = 0;
								}

							}
							else
							{
								s->counter[VEG_UNVEG] = 0;
								logger(g_debug_log, "-%s is in un-veg period\n", s->name);
							}
						}

					}
					/* FOR EVERGREEN */
					else
					{
						s->counter[VEG_UNVEG] = 1;
						logger(g_debug_log, "-%s is in veg period\n", s->name);
					}
				}
			}
		}
	}
	logger(g_debug_log, "**************************************\n");
}

void phenology(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const int day, const int month, const int year)
{

 
        int mdl=0;   // ddalmo test
        
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n--DAILY PHENOLOGY for SPECIES %s phenology = %.1f--\n", s->name, s->value[PHENOLOGY]);
	logger(g_debug_log, "-LAI_PROJ = %g\n-PEAK_LAI_PROJ = %g\n", s->value[LAI_PROJ], s->value[PEAK_LAI_PROJ]);

	/*for deciduous*/
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		if (s->counter[VEG_UNVEG] == 1 )
		{
			/* Beginning of growing season */
			/* BUDBURST PHASE */
			if (s->counter[VEG_DAYS] <= ((int)s->value[BUD_BURST]))
			{
				/* Maximum growth */
				if (s->value[LAI_PROJ] < s->value[PEAK_LAI_PROJ])
				{
					s->phenology_phase = 1;
				}
				/* Normal Growth */
				else
				{
					s->phenology_phase = 2;
				}
			}
			else
			{
				/* Normal growth */
				if ( month + 1 <= 6)
				{
					s->phenology_phase = 2;
				}
				else
				{
				mdl = s->value[MINDAYLENGTH] ;
				// for test purpose to prescribe senescence phase
				//if ( c->years[year].year == 2016 ) 
				//	 { 
				//	  mdl = 10.4 ;
				//	  }  
					  
					/* Normal growth */
					if ( meteo_daily->daylength > mdl )
					{
						s->phenology_phase = 2;
					}
					/* Leaf fall */
					else
					{
						s->phenology_phase = 3;
					}
				}
			}
		}
		else
		{
			/* Un-vegetative period */
			s->phenology_phase = 0;
		}
	}
	/*for evergreen*/
	else
	{

		/* a very simplistic way to define a phenological phase for evergreen*/
		/*just two phase are considered
		 * shoot elongation
		 * secondary growth*/
		/*see Ludeke et al., 1994*/

		/* Beginning of a "growing season" */
		/* Maximum growth */
		// old if ( meteo_daily->thermic_sum >= s->value[GROWTHSTART] && s->value[LAI] < s->value[PEAK_LAI] )
		if ( s->value[LAI_PROJ] < s->value[PEAK_LAI_PROJ] && ( ( c->lat > 0 && month < 5 ) ||
				( c->lat < 0 && month > 11 ) ) )
		{
			s->phenology_phase = 1;
		}
		/* Normal growth*/
		else
		{
			s->phenology_phase = 2;
		}
	}
	logger(g_debug_log, "phenology phase = %d\n", s->phenology_phase);


	if ( s->counter[VEG_UNVEG] == 1 )
	{
		logger(g_debug_log, "\n\n*****VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

		/* increment vegetative days counter */
		++s->counter[VEG_DAYS];
		logger(g_debug_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);
	}
	else
	{
		logger(g_debug_log, "\n\n*****UN-VEGETATIVE PERIOD FOR %s SPECIES*****\n", s->name );

		/* increment vegetative days counter */
		s->counter[VEG_DAYS] = 0;
		logger(g_debug_log, "VEG_DAYS = %d \n", s->counter[VEG_DAYS]);
	}

}

//void Phenology_phase (species_t *const s, const meteo_t* const met, const int year, const int month, const int day)
//{
//	logger(g_debug_log, "--GET_DAILY PHENOLOGY for SPECIES %s phenology = %.1f--\n", s->name, s->value[PHENOLOGY]);
//
//	/*for deciduous*/
//	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
//	{
//		if (s->counter[VEG_UNVEG] == 1 )
//		{
//			//Beginning of growing season
//
//			//BUDBURST
//			if (s->counter[VEG_DAYS] <= s->value[BUD_BURST])
//			{
//				if (s->value[LAI] < (s->value[PEAK_LAI] * 0.5))
//				{
//					s->phenology_phase = 1;
//				}
//				//Maximum Growth
//				else if (s->value[LAI] > (s->value[PEAK_LAI] * 0.5)	&& s->value[LAI] < s->value[PEAK_LAI])
//				{
//					logger(g_debug_log, "LAI = %f\n", s->value[LAI]);
//					logger(g_debug_log, "PEAK LAI = %f\n", s->value[PEAK_LAI]);
//					s->phenology_phase = 2;
//				}
//				//Full growing season
//				else
//				{
//					s->phenology_phase = 4;
//				}
//			}
//			else
//			{
//				//Maximum Growth
//				if (s->value[LAI] <= (s->value[PEAK_LAI] * 0.5))
//				{
//					s->phenology_phase = 2;
//				}
//				//Maximum Growth
//				if (s->value[LAI] > (s->value[PEAK_LAI] * 0.5)&& s->value[LAI] < s->value[PEAK_LAI])
//				{
//					s->phenology_phase = 3;
//				}
//				//Full growing season
//				if(fabs (s->value[LAI] - s->value[PEAK_LAI]) < 0.1)
//				{
//					s->phenology_phase = 4;
//				}
//				if (met[month].d[day].daylength < s->value[MINDAYLENGTH] && month+1 > 6 /*c->abscission_daylength*/)
//				{
//					//Leaf fall
//					s->phenology_phase = 5;
//				}
//			}
//		}
//		else
//		{
//			//Unvegetative period
//			s->phenology_phase = 0;
//		}
//	}
//	/*for evergreen*/
//	else
//	{
//		//fixme
//		/* a very simplistic way to define a phenological phase for evergreen*/
//		/*just two phase are considered
//		 * shoot elongation
//		 * secondary growth*/
//		/*see Ludeke et al., 1994*/
//		/*Beginning of a "growing season"*/
//		if (met[month].d[day].thermic_sum >= s->value[GROWTHSTART] && s->value[LAI] < s->value[PEAK_LAI]																										   && month < 6)
//		{
//			s->phenology_phase = 1;
//			s->counter[LEAF_FALL_COUNTER] = 0;
//		}
//		/*normal growth*/
//		else
//		{
//			s->phenology_phase = 2;
//			if (s->counter[LEAF_FALL_COUNTER] == 0)
//			{
//				s->counter[LEAF_FALL_COUNTER] = 1;
//			}
//			else if (s->counter[LEAF_FALL_COUNTER] == 1)
//			{
//				s->counter[LEAF_FALL_COUNTER] = 2;
//			}
//		}
//		if (day == 0 && month == 0 && year == 0)
//		{
//			s->phenology_phase = 1;
//			s->counter[LEAF_FALL_COUNTER] = 0;
//			s->value[DAILY_LEAVES_BIOMASS_TO_REMOVE] = 0.0;
//		}
//	}
//
//	logger(g_debug_log, "phenology phase = %d\n", s->phenology_phase);
//}


