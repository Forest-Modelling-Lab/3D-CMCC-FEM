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

extern void Get_phenology_phase (CELL * c, const MET_DATA *const met, const int years, const int month, const int day, const int years_of_simulation)
{
	int height;
	int age;
	int species;

	for ( height = c->heights_count -1 ; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				Log("--GET_DAILY PHENOLOGY for SPECIES %s phenology = %f--\n", c->heights[height].ages[age].species[species].name, c->heights[height].ages[age].species[species].value[PHENOLOGY]);

				//defining phenology phase
				if (settings->spatial == 'u') //for unspatial version
				{
					if(settings->time == 'd') //for daily version
					{
						/*for deciduous*/
						if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
						{
							if (c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1 )
							{
								//Beginning of growing season

								//BUDBURST
								if (c->heights[height].ages[age].species[species].counter[VEG_DAYS] <= c->heights[height].ages[age].species[species].value[BUD_BURST])
								{
									if (c->heights[height].ages[age].species[species].value[LAI] < (c->heights[height].ages[age].species[species].value[PEAK_Y_LAI] * 0.5))
									{
										c->heights[height].ages[age].species[species].phenology_phase = 1;
									}
									//Maximum Growth
									else if (c->heights[height].ages[age].species[species].value[LAI] > (c->heights[height].ages[age].species[species].value[PEAK_Y_LAI] * 0.5)
											&& c->heights[height].ages[age].species[species].value[LAI] < c->heights[height].ages[age].species[species].value[PEAK_Y_LAI])
									{
										Log("LAI = %f\n", c->heights[height].ages[age].species[species].value[LAI]);
										Log("PEAK LAI = %f\n", c->heights[height].ages[age].species[species].value[PEAK_Y_LAI]);
										c->heights[height].ages[age].species[species].phenology_phase = 2;
									}
									//Full growing season
									else
									{
										c->heights[height].ages[age].species[species].phenology_phase = 4;
									}
								}
								else
								{
									//Maximum Growth
									if (c->heights[height].ages[age].species[species].value[LAI] <= (c->heights[height].ages[age].species[species].value[PEAK_Y_LAI] * 0.5))
									{
										c->heights[height].ages[age].species[species].phenology_phase = 2;
									}
									//Maximum Growth
									if (c->heights[height].ages[age].species[species].value[LAI] > (c->heights[height].ages[age].species[species].value[PEAK_Y_LAI] * 0.5)
											&& c->heights[height].ages[age].species[species].value[LAI] < c->heights[height].ages[age].species[species].value[PEAK_Y_LAI])
									{
										c->heights[height].ages[age].species[species].phenology_phase = 3;
									}
									//Full growing season
									if(fabs (c->heights[height].ages[age].species[species].value[LAI] - c->heights[height].ages[age].species[species].value[PEAK_Y_LAI]) < 0.1)
									{
										c->heights[height].ages[age].species[species].phenology_phase = 4;
									}
									if (met[month].d[day].daylength < c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month > 6/*c->abscission_daylength*/)
									{
										//Leaf fall
										c->heights[height].ages[age].species[species].phenology_phase = 5;
									}
								}
							}
							else
							{
								//Unvegetative period
								c->heights[height].ages[age].species[species].phenology_phase = 0;
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
							if (met[month].d[day].thermic_sum >= c->heights[height].ages[age].species[species].value[GROWTHSTART]
													 && c->heights[height].ages[age].species[species].value[LAI] < c->heights[height].ages[age].species[species].value[PEAK_Y_LAI]
														   && month < 6)
							{
								c->heights[height].ages[age].species[species].phenology_phase = 1;
								c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] = 0;
							}
							/*normal growth*/
							else
							{
								c->heights[height].ages[age].species[species].phenology_phase = 2;
								if (c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] == 0)
								{
									c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] = 1;
								} else if (c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] == 1)
								{
									c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] = 2;
								}
							}
							if (day == 0 && month == 0 && years == 0)
							{
								c->heights[height].ages[age].species[species].phenology_phase = 1;
																c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] = 0;
																c->heights[height].ages[age].species[species].value[DAILY_LEAVES_BIOMASS_TO_REMOVE] = 0.0;
							}

						}
					}
					//fixme
					else
					{

					}
				}
				//fixme
				else
				{

				}
				Log("phenology phase = %d\n", c->heights[height].ages[age].species[species].phenology_phase);
			}
		}
	}
}

