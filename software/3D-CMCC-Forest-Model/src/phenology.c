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
#include "types.h"
#include "constants.h"

void Phenology_phase (SPECIES * s, const MET_DATA *const met, const int years, const int month, const int day)
{
	int height;
	int age;
	int species;


	Log("--GET_DAILY PHENOLOGY for SPECIES %s phenology = %.1f--\n", s->name, s->value[PHENOLOGY]);

	//defining phenology phase
	if (settings->spatial == 'u') //for unspatial version
	{
		/*for deciduous*/
		if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
		{
			if (s->counter[VEG_UNVEG] == 1 )
			{
				//Beginning of growing season

				//BUDBURST
				if (s->counter[VEG_DAYS] <= s->value[BUD_BURST])
				{
					if (s->value[LAI] < (s->value[PEAK_LAI] * 0.5))
					{
						s->phenology_phase = 1;
					}
					//Maximum Growth
					else if (s->value[LAI] > (s->value[PEAK_LAI] * 0.5)	&& s->value[LAI] < s->value[PEAK_LAI])
					{
						Log("LAI = %f\n", s->value[LAI]);
						Log("PEAK LAI = %f\n", s->value[PEAK_LAI]);
						s->phenology_phase = 2;
					}
					//Full growing season
					else
					{
						s->phenology_phase = 4;
					}
				}
				else
				{
					//Maximum Growth
					if (s->value[LAI] <= (s->value[PEAK_LAI] * 0.5))
					{
						s->phenology_phase = 2;
					}
					//Maximum Growth
					if (s->value[LAI] > (s->value[PEAK_LAI] * 0.5)&& s->value[LAI] < s->value[PEAK_LAI])
					{
						s->phenology_phase = 3;
					}
					//Full growing season
					if(fabs (s->value[LAI] - s->value[PEAK_LAI]) < 0.1)
					{
						s->phenology_phase = 4;
					}
					if (met[month].d[day].daylength < s->value[MINDAYLENGTH] && month+1 > 6 /*c->abscission_daylength*/)
					{
						//Leaf fall
						s->phenology_phase = 5;
					}
				}
			}
			else
			{
				//Unvegetative period
				s->phenology_phase = 0;
			}
		}
		/*for evergreen*/
		else
		{
			//fixme
			/* a very simplistic way to define a phenological phase for evergreen*/
			/*just two phase are considered
			 * shoot elongation
			 * secondary growth*/
			/*see Ludeke et al., 1994*/
			/*Beginning of a "growing season"*/
			if (met[month].d[day].thermic_sum >= s->value[GROWTHSTART] && s->value[LAI] < s->value[PEAK_LAI]																										   && month < 6)
			{
				s->phenology_phase = 1;
				s->counter[LEAF_FALL_COUNTER] = 0;
			}
			/*normal growth*/
			else
			{
				s->phenology_phase = 2;
				if (s->counter[LEAF_FALL_COUNTER] == 0)
				{
					s->counter[LEAF_FALL_COUNTER] = 1;
				}
				else if (s->counter[LEAF_FALL_COUNTER] == 1)
				{
					s->counter[LEAF_FALL_COUNTER] = 2;
				}
			}
			if (day == 0 && month == 0 && years == 0)
			{
				s->phenology_phase = 1;
				s->counter[LEAF_FALL_COUNTER] = 0;
				s->value[DAILY_LEAVES_BIOMASS_TO_REMOVE] = 0.0;
			}
		}
	}
	//fixme
	else
	{

	}
	Log("phenology phase = %d\n", s->phenology_phase);
}

void Test_phenology_phase (SPECIES * s, const MET_DATA *const met, const int years, const int month, const int day)
{

	Log("--GET_DAILY PHENOLOGY for SPECIES %s phenology = %.1f--\n", s->name, s->value[PHENOLOGY]);

	/*for deciduous*/
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		if (s->counter[VEG_UNVEG] == 1 )
		{
			//Beginning of growing season
			//BUDBURST
			if (s->counter[VEG_DAYS] <= s->value[BUD_BURST])
			{
				if (s->value[LAI] < (s->value[PEAK_LAI]))
				{
					s->phenology_phase = 1;
				}
				//Normal Growth
				else
				{
					s->phenology_phase = 4;
				}
			}
			else
			{
				if (month+1 <= 6)
				{
					s->phenology_phase = 4;
				}
				else
				{
					if (met[month].d[day].daylength > s->value[MINDAYLENGTH])
					{
						s->phenology_phase = 4;
					}
					else
					{
						//leaf fall
						s->phenology_phase = 5;
					}
				}
			}
		}
		else
		{
			//Unvegetative period
			s->phenology_phase = 0;
		}
	}
	/*for evergreen*/
	else
	{
		//fixme
	}
	Log("phenology phase = %d\n LAI = %f\n month = %d\n", s->phenology_phase, s->value[LAI], month);

}
